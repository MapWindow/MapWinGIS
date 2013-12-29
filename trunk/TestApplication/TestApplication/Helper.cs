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
  using System.Text;

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
  }
}
