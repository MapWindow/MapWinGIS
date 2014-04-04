// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Helper.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   Static class to hold the helper methods
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
  using System;
  using System.Collections.Generic;
  using System.IO;
  using System.Linq;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Static class to hold the tests methods</summary>
  internal static class Helper
  {
    /// <summary>Check the given shapefile location</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    /// <returns>True when no errors else false</returns>
    internal static bool CheckShapefileLocation(string shapefilename, ICallback theForm)
    {
      if (shapefilename == string.Empty)
      {
        theForm.Error(string.Empty, "Input parameters are wrong");
        return false;
      }

      var folder = Path.GetDirectoryName(shapefilename);
      if (folder == null)
      {
        theForm.Error(string.Empty, "Input parameters are wrong");
        return false;
      }

      if (!Directory.Exists(folder))
      {
        theForm.Error(string.Empty, "Output folder doesn't exists");
        return false;
      }

      if (!File.Exists(shapefilename))
      {
        theForm.Error(string.Empty, "Input shapefile doesn't exists");
        return false;
      }

      return true;
    }

    /// <summary>Check if the resulting shapefile is correct</summary>
    /// <param name="inputSf">
    /// The input sf.
    /// </param>
    /// <param name="resultingSf">
    /// The resulting sf.
    /// </param>
    /// <param name="gdalLastErrorMsg">
    /// The gdal last error msg.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    /// <returns>True when no errors else false</returns>
    internal static bool CheckShapefile(IShapefile inputSf, IShapefile resultingSf, string gdalLastErrorMsg, ICallback theForm)
    {
      if (resultingSf == null)
      {
        var msg = "The resulting shapefile was not created: " + inputSf.get_ErrorMsg(inputSf.LastErrorCode);
        if (gdalLastErrorMsg != string.Empty)
        {
          msg += Environment.NewLine + "GdalLastErrorMsg: " + gdalLastErrorMsg;
        }

        theForm.Error(string.Empty, msg);
        return false;
      }

      if (resultingSf.NumShapes < -1)
      {
        theForm.Error(string.Empty, "Resulting shapefile has no shapes");
        return false;
      }

      if (resultingSf.HasInvalidShapes())
      {
        theForm.Error(string.Empty, "Resulting shapefile has invalid shapes");
        return false;
      }

      if (resultingSf.NumFields < -1)
      {
        theForm.Error(string.Empty, "Resulting shapefile has no fields");
        return false;
      }

      if (resultingSf.NumShapes != resultingSf.Table.NumRows)
      {
        theForm.Error(string.Empty, string.Format("The resulting shapefile has {0} shapes and {1} rows. This should be equal!", resultingSf.NumShapes, resultingSf.Table.NumRows));
        return false;
      }

      return true;
    }

    /// <summary>Color the shapes</summary>
    /// <param name="sf">
    /// The shapefile
    /// </param>
    /// <param name="fieldIndex">The index to color from</param>
    /// <param name="mapColorFrom">
    /// The color from.
    /// </param>
    /// <param name="mapColorTo">
    /// The color to.
    /// </param>
    /// <param name="forceUnique">
    /// Force unique.
    /// </param>
    internal static void ColorShapes(ref Shapefile sf, int fieldIndex, tkMapColor mapColorFrom, tkMapColor mapColorTo, bool forceUnique)
    {
      // Create visualization categories 
      var utils = new Utils();

      if (sf.ShapefileType == ShpfileType.SHP_POLYGON || sf.ShapefileType == ShpfileType.SHP_POLYGONZ
          || sf.ShapefileType == ShpfileType.SHP_POLYGONM)
      {
        sf.DefaultDrawingOptions.FillType = tkFillType.ftStandard;
        sf.DefaultDrawingOptions.FillColor = utils.ColorByName(tkMapColor.Tomato);
      }

      if (sf.ShapefileType == ShpfileType.SHP_POLYLINE
        || sf.ShapefileType == ShpfileType.SHP_POLYLINEZ
        || sf.ShapefileType == ShpfileType.SHP_POLYLINEM)
      {
        sf.DefaultDrawingOptions.LineWidth = 3;
      }

      if (!forceUnique && sf.NumShapes > 10)
      {
        sf.Categories.Generate(fieldIndex, tkClassificationType.ctNaturalBreaks, 9);
      }
      else
      {
        sf.Categories.Generate(fieldIndex, tkClassificationType.ctUniqueValues, 0);
      }

      sf.Categories.ApplyExpressions();

      // apply color scheme
      var scheme = new ColorScheme();
      scheme.SetColors2(mapColorFrom, mapColorTo);
      sf.Categories.ApplyColorScheme(tkColorSchemeType.ctSchemeGraduated, scheme);
    }

    /// <summary>Delete the shapefile</summary>
    /// <param name="filename">The filename</param>
    /// <remarks>A shapefile has at least 3 files</remarks>
    internal static void DeleteShapefile(string filename)
    {
      if (!File.Exists(filename))
      {
        // Nothing to do:
        return;
      }

      var basename = Path.GetFileNameWithoutExtension(filename);
      var folder = Path.GetDirectoryName(filename);
      if (folder == null)
      {
        // Folder does not exists
        return;
      }

      foreach (var f in new DirectoryInfo(folder).GetFiles(basename + ".*"))
      {
        f.Delete();
      }
    }

    /// <summary>Delete the grid file</summary>
    /// <param name="filename">The filename</param>
    internal static void DeleteGridfile(string filename)
    {
      if (!File.Exists(filename))
      {
        // Nothing to do:
        return;
      }

      var basename = Path.GetFileNameWithoutExtension(filename);
      var folder = Path.GetDirectoryName(filename);
      if (folder == null)
      {
        // Folder does not exists
        return;
      }

      foreach (var f in new DirectoryInfo(folder).GetFiles(basename + ".*"))
      {
        f.Delete();
      }
    }

    /// <summary>
    /// Read the text file with the file loctions
    /// </summary>
    /// <param name="textfileLocation">The location of the text file</param>
    /// <returns>The relevant lines in a collection</returns>
    internal static List<string> ReadTextfile(string textfileLocation)
    {
      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      // Open file, read line by line, skip lines starting with #
      var lines = File.ReadAllLines(textfileLocation);
      
      // lines = lines.Select(l => l.Replace("%DATA%", Constants.SCRIPT_DATA_PATH)).ToArray();
      // Replace env. var in line from text file:
      var scriptPath = Environment.GetEnvironmentVariable("MW_SAMPLEDATA") ?? Constants.SCRIPT_DATA_PATH;

      lines = lines.Select(l => l.Replace("%MW_SAMPLEDATA%", scriptPath)).ToArray();

      return (from t in lines where !t.StartsWith("#") && t.Length != 0 select t.Trim()).ToList();
    }

    /// <summary>Checking the difference between the two shapes</summary>
    /// <param name="shp">The first shape</param>
    /// <param name="shp2">The second shape</param>
    /// <param name="theForm">The form</param>
    /// <returns>True when different</returns>
    internal static bool AreShapesDifferent(Shape shp, Shape shp2, ICallback theForm)
    {
      theForm.Progress(string.Empty, 100, "Checking the difference between the two shapes");

      if (!shp.IsValid)
      {
        theForm.Error(string.Empty, "The first shape is invalid: " + shp.IsValidReason);
        return false;
      }

      if (!shp2.IsValid)
      {
        theForm.Error(string.Empty, "The second shape is invalid: " + shp2.IsValidReason);
        return false;
      }

      if (shp.ExportToWKT() != shp2.ExportToWKT())
      {
        theForm.Error(string.Empty, "Export to WKT for new shape is different from original one");
        return false;
      }

      if (shp.ShapeType != shp2.ShapeType)
      {
        theForm.Error(
          string.Empty,
          string.Format("The first shape is a {0} and the second shape is a {1}", shp.ShapeType, shp2.ShapeType));
        return false;
      }

      if (shp.NumPoints != shp2.NumPoints)
      {
        theForm.Error(
          string.Empty,
          string.Format("The first shape has {0} points and the second shape has {1} points", shp.NumPoints, shp2.NumParts));
        return false;
      }

      if (shp.NumParts != shp2.NumParts)
      {
        theForm.Error(
          string.Empty,
          string.Format("The first shape has {0} points and the second shape has {1} points", shp.NumParts, shp2.NumParts));
        return false;
      }

      if (shp.ShapeType == ShpfileType.SHP_POLYGON 
       || shp.ShapeType == ShpfileType.SHP_POLYGONM 
       || shp.ShapeType == ShpfileType.SHP_POLYGONZ)
      {
        // Check area:
        if (shp.Area != shp2.Area)
        {
          theForm.Error(
            string.Empty,
            string.Format("The first shape has an area of {0} and the second shape has an area of {1}", shp.Area, shp2.Area));
          return false;
        }

        // Check perimeter:
        if (shp.Perimeter != shp2.Perimeter)
        {
          theForm.Error(
            string.Empty,
            string.Format("The first shape has a perimeter of {0} and the second shape has a perimeter of {1}", shp.Perimeter, shp2.Perimeter));
          return false;
        }
      }

      if (shp.ShapeType == ShpfileType.SHP_POLYLINE
       || shp.ShapeType == ShpfileType.SHP_POLYLINEM
       || shp.ShapeType == ShpfileType.SHP_POLYLINEZ)
      {
        // Check length:
        if (shp.Length != shp2.Length)
        {
          theForm.Error(
            string.Empty,
            string.Format("The first shape has a length of {0} and the second shape has a length of {1}", shp.Length, shp2.Length));
          return false;
        }
      }

      if (shp.SerializeToString() != shp2.SerializeToString())
      {
        theForm.Error(string.Empty, "The SerializeToString methods return different values:");
        theForm.Error(string.Empty, shp.SerializeToString());
        theForm.Error(string.Empty, shp2.SerializeToString());
        return false;
      }

      if (shp.Extents.xMax != shp2.Extents.xMax
       || shp.Extents.yMax != shp2.Extents.yMax
       || shp.Extents.zMax != shp2.Extents.zMax
       || shp.Extents.mMax != shp2.Extents.mMax)
      {
        theForm.Error(string.Empty, "The max values of X, Y, Z or M return different values");
      }

      for (var i = 0; i < shp.NumPoints; i++)
      {
        if (shp.get_Point(i).M != shp2.get_Point(i).M)
        {
          theForm.Error(string.Empty, "M values are different");
        }
      }

      if (shp.Extents.xMin != shp2.Extents.xMin
       || shp.Extents.yMin != shp2.Extents.yMin
       || shp.Extents.zMin != shp2.Extents.zMin
       || shp.Extents.mMin != shp2.Extents.mMin)
      {
        theForm.Error(string.Empty, "The min values of X, Y, Z or M return different values");
      }

      if (!shp.Equals(shp2))
      {
        theForm.Error(string.Empty, "Equals returns false");
        return false;
      }
      
      return true;
    }

    /// <summary>Run all tests in group box</summary>
    /// <param name="parentBox">The parent group box</param>
    internal static void RunAllTestsInGroupbox(GroupBox parentBox)
    {
      var groups = parentBox.Controls.OfType<GroupBox>();
      var buttons = groups.SelectMany(groupBox => groupBox.Controls.OfType<Button>());
      var buttonsWithRunTag = buttons.Where(button => button.Tag != null && button.Tag.ToString() == "run");
      var orderByTabIndex = buttonsWithRunTag.OrderBy(button => button.TabIndex);

      foreach (var button in orderByTabIndex)
      {
        button.PerformClick();
      }
    }
  }
}
