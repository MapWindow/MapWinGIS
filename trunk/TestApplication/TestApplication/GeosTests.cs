// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GeosTests.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   Static class to hold the tests methods for the GEOS functions
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
  using System;
  using System.IO;

  using MapWinGIS;

  /// <summary>Static class to hold the tests methods</summary>
  internal static class GeosTests
  {
    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap MyAxMap { get; set; }

    /// <summary>Run the Buffer shapefile test</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunBufferShapefileTest(string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Buffer shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        // Open the sf:
        // First check if the MWShapeID field is present:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start buffering " + Path.GetFileName(shapefilename));

        // Make the distance depending on the projection.
        var distance = 1000;
        if (sf.GeoProjection.IsGeographic)
        {
          distance = 1;
        }

        var bufferedSf = sf.BufferByDistance(distance, 16, false, false);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, bufferedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        Helper.ColorShapes(ref bufferedSf, 0, tkMapColor.LightBlue, tkMapColor.LightYellow, false);

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-buffered.shp");
        Helper.DeleteShapefile(newFilename);
        bufferedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        Fileformats.OpenShapefileAsLayer(shapefilename, theForm, true);
        bufferedSf.DefaultDrawingOptions.FillVisible = false;
        MyAxMap.AddLayer(bufferedSf, true);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Buffer shapefile test has finished.");
    }

    /// <summary>Run the Simplify shapefile test</summary>
    /// <param name="shapefilename">The shapefile name</param>
    /// <param name="theForm">The form</param>
    internal static void RunSimplifyShapefileTest(string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Simplify shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }
        
        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start simplifying " + Path.GetFileName(shapefilename));

        // Make the tolerance depending on the projection.
        var tolerance = 10d;
        if (sf.GeoProjection.IsGeographic)
        {
          tolerance = 1;
        }

        var simplifiedSf = sf.SimplifyLines(tolerance, false);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, simplifiedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        // give the resulting lines a good width and color:
        var utils = new Utils { GlobalCallback = theForm };
        simplifiedSf.DefaultDrawingOptions.LineWidth = 2;
        simplifiedSf.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.OrangeRed);
        simplifiedSf.DefaultDrawingOptions.LineStipple = tkDashStyle.dsSolid;

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-intersected.shp");
        Helper.DeleteShapefile(newFilename);
        simplifiedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(simplifiedSf, true);
        Fileformats.OpenShapefileAsLayer(shapefilename, theForm, false);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Simplify shapefile test has finished.");
    }

    /// <summary>Run Aggregate shapefile test</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="fieldIndex">
    /// The field index.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunDissolveShapefileTest(string shapefilename, int fieldIndex, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Dissolve shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start dissolving " + Path.GetFileName(shapefilename));

        var dissolvedSf = sf.Dissolve(fieldIndex, false);

        // Do some checks:))
        if (!Helper.CheckShapefile(sf, dissolvedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        // The resulting shapefile has only 1 field:
        Helper.ColorShapes(ref dissolvedSf, 0, tkMapColor.BlueViolet, tkMapColor.Wheat, true);

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-dissolved.shp");
        Helper.DeleteShapefile(newFilename);
        dissolvedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(dissolvedSf, true);
        sf.DefaultDrawingOptions.FillVisible = false;
        MyAxMap.AddLayer(sf, true);

        theForm.Progress(
          string.Empty,
          100,
          string.Format(
            "The dissolved shapefile now has {0} shapes instead of {1} and has {2} rows",
            dissolvedSf.NumShapes,
            sf.NumShapes,
            dissolvedSf.Table.NumRows));
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Dissolve shapefile test has finished");
    }

    /// <summary>Run the clip shapefile test</summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunClipShapefileTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Clip shapefile test has started.", Environment.NewLine));

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      // Get every first line and second line:
      for (var i = 0; i < lines.Count; i = i + 2)
      {
        if (i + 1 > lines.Count)
        {
          theForm.Error(string.Empty, "Input file is incorrect.");
          break;
        }

        ClipShapefile(lines[i], lines[i + 1], theForm);
      }

      theForm.Progress(string.Empty, 100, "The clipping shapefile test has finished.");
    }

    /// <summary>Run the WKT - shapefile conversion test</summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunWktShapefileTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The WKT - shapefile conversion test has started.", Environment.NewLine));

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      foreach (var line in lines)
      {
        var shapeIndex = 0; // if needed change this method to read this value from the input text file
        var wkt = ConvertToWkt(line, shapeIndex, theForm);
        if (wkt != null)
        {
          theForm.Progress(string.Empty, 100, wkt);
          ConvertFromWkt(line, shapeIndex, wkt, theForm);
        }
      }

      theForm.Progress(string.Empty, 100, "The WKT - shapefile conversion test has finished.");
    }

    /// <summary>Run the intersect shapefile test</summary>
    /// <param name="shapefilenameFirst">
    /// The shapefilename first.
    /// </param>
    /// <param name="shapefilenameSecond">
    /// The shapefilename second.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    internal static void RunIntersectionShapefileTest(string shapefilenameFirst, string shapefilenameSecond, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The get Intersection shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilenameFirst, theForm))
        {
          return;
        }

        if (!Helper.CheckShapefileLocation(shapefilenameSecond, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilenameFirst, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var overlaySf = Fileformats.OpenShapefile(shapefilenameSecond, theForm);
        if (overlaySf == null)
        {
          theForm.Error(string.Empty, "Opening overlay shapefile was unsuccessful");
          return;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start intersecting " + Path.GetFileName(shapefilenameFirst));

        var intersectedSf = sf.GetIntersection(false, overlaySf, false, ShpfileType.SHP_NULLSHAPE, theForm);

        // Do some checks:))
        if (!Helper.CheckShapefile(sf, intersectedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        Helper.ColorShapes(ref intersectedSf, 0, tkMapColor.BlueViolet, tkMapColor.DarkRed, true);

        // Save result:
        var newFilename = shapefilenameSecond.Replace(".shp", "-intersected.shp");
        Helper.DeleteShapefile(newFilename);
        intersectedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(intersectedSf, true);
        MyAxMap.AddLayer(overlaySf, true);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The get intersection shapefile test has finished.");
    }

    /// <summary>Test the closest points method</summary>
    /// <param name="pointShapefile">The point shapefile</param>
    /// <param name="searchShapefile">The search shapefile</param>
    /// <param name="theForm">The form</param>
    internal static void RunClosestPointTest(string pointShapefile, string searchShapefile, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The closest point test has started.", Environment.NewLine));

        const string path = @"c:\dev\TestingScripts\ScriptData\General\MapWindow-Projects\UnitedStates\Shapefiles\";
            pointShapefile = path + "cities.shp";
            searchShapefile = path + "states.shp";

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(pointShapefile, theForm))
        {
          return;
        }

        if (!Helper.CheckShapefileLocation(searchShapefile, theForm))
        {
          return;
        }

        // Open the sf:
        var pointSf = Fileformats.OpenShapefile(pointShapefile, theForm);
        if (pointSf == null)
        {
          theForm.Error(string.Empty, "Opening point shapefile was unsuccessful");
          return;
        }

        var searchSf = Fileformats.OpenShapefile(searchShapefile, theForm);
        if (searchSf == null)
        {
          theForm.Error(string.Empty, "Opening search shapefile was unsuccessful");
          return;
        }

        // Create resulting shapefile:
        var linksSf = new Shapefile { GlobalCallback = theForm };
        linksSf.CreateNew(string.Empty, ShpfileType.SHP_POLYLINE);
        var fieldIndex = linksSf.EditAddField("FoundId", FieldType.INTEGER_FIELD, 0, 0);

        // Get a random point:
        var index = new Random().Next(pointSf.NumShapes - 1);
        var pointShp = pointSf.get_Shape(index);

        // Select the shape:
        var utils = new Utils { GlobalCallback = theForm };
        pointSf.SelectionColor = utils.ColorByName(tkMapColor.Yellow);
        pointSf.set_ShapeSelected(index, true);

        // To store the lenght and index:
        var minLength = double.MaxValue;
        var closestIndex = -1;

        // Search around the location until at least on shape is found.
        // To optimize searching make sure a spatial index is used:
        if (!searchSf.HasSpatialIndex)
        {
          searchSf.CreateSpatialIndex(searchSf.Filename);
        }

        searchSf.UseSpatialIndex = true;

        // create start search box:
        var searchExtent = pointShp.Extents;

        // Make the start tolerance depending on the projection:
        var tolerance = 2D;
        if (searchSf.GeoProjection.IsGeographic)
        {
          tolerance = 0.01;
        }
        
        var foundShapeID = -1;
        object results = null;
        while (true)
        {
          if (searchSf.SelectShapes(searchExtent, tolerance, SelectMode.INTERSECTION, ref results))
          {
            var shapes = results as int[];

            // Use the first one:
            if (shapes != null)
            {
              foundShapeID = shapes[0];
            }

            // stop searching:
            break;
          }
          
          // increase tolerance:
          tolerance = tolerance + tolerance;
        }
        
        if (foundShapeID == -1)
        {
          theForm.Error(string.Empty, "Error! Could not find any shapes");
          return;
        }

        // Select the found shape:
        searchSf.SelectionColor = utils.ColorByName(tkMapColor.Red);
        searchSf.set_ShapeSelected(foundShapeID, true);

        var link = pointShp.ClosestPoints(searchSf.get_Shape(foundShapeID));
        if (link != null && link.NumPoints > 1)
        {
          var shapeId = linksSf.EditAddShape(link);
          linksSf.EditCellValue(fieldIndex, shapeId, foundShapeID);
          if (minLength > link.Length)
          {
            minLength = link.Length;
            closestIndex = foundShapeID;
          }
        }

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(searchSf, true);
        MyAxMap.AddLayer(pointSf, true);

        linksSf.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.Black);
        linksSf.DefaultDrawingOptions.LineWidth = 2;
        MyAxMap.AddLayer(linksSf, true);
        linksSf.GenerateLabels(fieldIndex, tkLabelPositioning.lpMiddleSegment, false);
        linksSf.Labels.OffsetX = 10;
        linksSf.Labels.OffsetY = 10;

        MyAxMap.ZoomToMaxExtents();
        MyAxMap.Redraw();

        theForm.Progress(string.Empty, 0, string.Format("The closest shape is: {0} and has a length of {1}", searchSf.get_CellValue(0, closestIndex), minLength));

        // Save result:
        var newFilename = pointSf.Filename.Replace(".shp", "-Closest.shp");
        Helper.DeleteShapefile(newFilename);
        linksSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The closest point test has finished.");
    }

    /// <summary>Clip the shapefile</summary>
    /// <param name="shapefilename">The shapefile name</param>
    /// <param name="overlayFilename">The name of the overlay shapefile</param>
    /// <param name="theForm">The form</param>
    private static void ClipShapefile(string shapefilename, string overlayFilename, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        if (!Helper.CheckShapefileLocation(overlayFilename, theForm))
        {
          return;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return;
        }

        var overlaySf = Fileformats.OpenShapefile(overlayFilename, theForm);
        if (overlaySf == null)
        {
          theForm.Error(string.Empty, "Opening overlay shapefile was unsuccessful");
          return;
        }

        theForm.Progress(
          string.Empty,
          0,
          string.Format(
            "Clipping {0} ({1}) with {2} ({3})",
            Path.GetFileName(shapefilename),
            sf.ShapefileType,
            Path.GetFileName(overlayFilename),
            overlaySf.ShapefileType));

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start clipping " + Path.GetFileName(shapefilename));

        var clippedSf = sf.Clip(false, overlaySf, false);

        // Do some checks:)
        if (!Helper.CheckShapefile(sf, clippedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        Helper.ColorShapes(ref clippedSf, 0, tkMapColor.DarkRed, tkMapColor.LightSeaGreen, true);

        // Count the resulting shapes it should be higher than the input shapefile.
        var numClippedSf = clippedSf.NumShapes;
        var numInputSf = sf.NumShapes;

        if (numClippedSf == numInputSf)
        {
          // Nothing was clipped
          theForm.Error(string.Empty, "The resulting shapefile has the same number of shapes as the input shapefile. Either the input files are incorrect or the clipping function doesn't behaves as expected.");
        }
        else
        {
          // Save result:
          var newFilename = shapefilename.Replace(
            ".shp", string.Format("-{0}", Path.GetFileName(overlayFilename).Replace(".shp", "-clipped.shp")));
          Helper.DeleteShapefile(newFilename);
          clippedSf.SaveAs(newFilename, theForm);
          theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

          // Load the files:
          MyAxMap.RemoveAllLayers();
          Fileformats.OpenShapefileAsLayer(overlayFilename, theForm, false);
          MyAxMap.AddLayer(clippedSf, true);
        }
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "This clipping has finished.");
    }

    /// <summary>Convert the shape to WKT</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="shapeIndex">
    /// The shape index.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>The WKT of the shape</returns>
    private static string ConvertToWkt(string shapefilename, int shapeIndex, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return null;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return null; 
        }

        // convert to WKT:
        string wkt = sf.Shape[shapeIndex].ExportToWKT();
        if (wkt == null || wkt.Trim() == string.Empty)
        {
          theForm.Error(string.Empty, "ExportToWKT was unsuccessful");
          return null; 
        }

        return wkt;
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      return null; 
    }

    /// <summary>Convert WKT to shape and test result</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="shapeIndex">
    /// The shape index.
    /// </param>
    /// <param name="wkt">
    /// The wkt.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    private static void ConvertFromWkt(string shapefilename, int shapeIndex, string wkt, Form1 theForm)
    {
      // Convert the WKT
      var shapeWkt = new Shape();
      if (!shapeWkt.ImportFromWKT(wkt))
      {
        theForm.Error(string.Empty, "ImportFromWKT was unsuccessful: " + shapeWkt.get_ErrorMsg(shapeWkt.LastErrorCode));
        return;
      }

      // Check with the original shape if it is still the same:
      var sf = Fileformats.OpenShapefile(shapefilename, theForm);
      if (Helper.AreShapesDifferent(sf.get_Shape(shapeIndex), shapeWkt, theForm))
      {
        theForm.Progress(string.Empty, 100, "The two shapes are identical.");
      }
    }
  }

  /*
   * Methods to test:
   * sf.SelectByShapefile()
  */
}
