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
  using System.Threading;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Static class to hold the tests methods</summary>
  internal static class GeosTests
  {
    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap MyAxMap { get; set; }

    /// <summary>Run the Buffer shapefile test</summary>
    /// <param name="textfileLocation">
    /// The location of the text file.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static bool RunBufferShapefileTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;

      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(string.Empty, 0, "-----------------------The GEOS Buffer tests have started.");

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);
      foreach (var line in lines)
      {
        if (!BufferShapefile(line, theForm))
        {
          numErrors++;
        }

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, string.Format("The GEOS Buffer tests have finished, with {0} errors", numErrors));

      return numErrors == 0;
    }

    /// <summary>Buffer the shapefile</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>True on success</returns>
    private static bool BufferShapefile(string shapefilename, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return false;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return false;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 100, "Start buffering " + Path.GetFileName(shapefilename));

        // Make the distance depending on the projection.
        var distance = 1000.0;
        if (sf.GeoProjection.IsGeographic)
        {
          distance = 0.1;
        }

        var bufferedSf = sf.BufferByDistance(distance, 16, false, false);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, bufferedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return false;
        }

        Helper.ColorShapes(ref bufferedSf, 0, tkMapColor.LightBlue, tkMapColor.LightYellow, false);

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-buffered.shp");
        Helper.DeleteShapefile(newFilename);
        bufferedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        MyAxMap.Clear();
        Fileformats.OpenShapefileAsLayer(shapefilename, theForm, true);
        bufferedSf.DefaultDrawingOptions.FillVisible = false;
        if (MyAxMap.AddLayer(bufferedSf, true) == -1)
        {
          theForm.Error(string.Empty, "Could not add the buffered shapefile to the map");
          return false;
        }

        // Wait to show the map:
        Application.DoEvents();
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      return true;
    }

    /// <summary>
    /// Run the Simplify shapefile test
    /// </summary>
    /// <param name="textfileLocation">
    /// The location of the text file
    /// </param>
    /// <param name="theForm">
    /// The form
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    internal static bool RunSimplifyShapefileTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;

      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(string.Empty, 0, "-----------------------The GEOS Simplify tests have started.");

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);
      foreach (var line in lines)
      {
        if (!SimplifyShapefile(line, theForm))
        {
          numErrors++;
        }

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, string.Format("The GEOS Simplify tests have finished, with {0} errors", numErrors));

      return numErrors == 0;
    }

    private static bool SimplifyShapefile(string shapefilename, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return false;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return false;
        }
        
        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start simplifying " + Path.GetFileName(shapefilename));

        // Make the tolerance depending on the projection.
        var tolerance = 10d;
        if (sf.GeoProjection.IsGeographic)
        {
          tolerance = 0.1;
        }

        var simplifiedSf = sf.SimplifyLines(tolerance, false);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, simplifiedSf, globalSettings.GdalLastErrorMsg, theForm))
		{
        {
          return false;
        }

        // give the resulting lines a good width and color:
        var utils = new Utils { GlobalCallback = theForm };
        simplifiedSf.DefaultDrawingOptions.LineWidth = 2;
        simplifiedSf.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.OrangeRed);
        simplifiedSf.DefaultDrawingOptions.LineStipple = tkDashStyle.dsSolid;

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-simplified.shp");
        Helper.DeleteShapefile(newFilename);
        simplifiedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        MyAxMap.Clear();
        if (MyAxMap.AddLayer(simplifiedSf, true) == -1)
        {
            theForm.Error(string.Empty, "Could not add the simplified shapefile to the map");
            return false;
        }

        Fileformats.OpenShapefileAsLayer(shapefilename, theForm, false);
      
        // Wait to show the map:
        Application.DoEvents();
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      return true;
    }

    /// <summary>
    /// Run the Dissolve shapefile test
    /// </summary>
    /// <param name="textfileLocation">
    /// The location of the text file.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    internal static bool RunDissolveShapefileTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;

      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(string.Empty, 0, "-----------------------The GEOS Dissolve tests have started.");


      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);
      
      // Get every first line and second line:
      for (var i = 0; i < lines.Count; i = i + 2)
      {
        if (i + 1 > lines.Count)
        {
          theForm.Error(string.Empty, "Input file is incorrect. Not enough lines");
          break;
        }

        int fieldIndex;
        if (!int.TryParse(lines[i + 1], out fieldIndex))
        {
          theForm.Error(string.Empty, "Input file is incorrect. Can't find field index value");
          break;
        }

        if (!DissolveShapefile(lines[i], fieldIndex, theForm))
        {
          numErrors++;
        }

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, string.Format("The GEOS Dissolve tests have finished, with {0} errors", numErrors));

      return numErrors == 0;
    }

    /// <summary>
    /// Dissolve the shapefile
    /// </summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="fieldIndex">
    /// The field index.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    internal static bool DissolveShapefile(string shapefilename, int fieldIndex, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return false;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return false;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start dissolving " + Path.GetFileName(shapefilename));

        var dissolvedSf = sf.Dissolve(fieldIndex, false);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, dissolvedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return false;
        }

        // The resulting shapefile has only 1 field:
        Helper.ColorShapes(ref dissolvedSf, 0, tkMapColor.BlueViolet, tkMapColor.Wheat, true);

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-dissolved.shp");
        Helper.DeleteShapefile(newFilename);
        dissolvedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        MyAxMap.Clear();
        if (MyAxMap.AddLayer(dissolvedSf, true) == -1)
        {
          theForm.Error(string.Empty, "Cannot open dissolved shapefiles");
          return false;
        }

        sf.DefaultDrawingOptions.FillVisible = false;
        //MyAxMap.AddLayer(sf, true);

        // Wait to show the map:
        Application.DoEvents();

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

      return true;
    }

    /// <summary>Run the clip shapefile test</summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static bool RunClipShapefileTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;
      theForm.Progress(string.Empty, 0, "-----------------------The Clip shapefile test has started.");

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

        if (!ClipShapefile(lines[i], lines[i + 1], theForm))
        {
          numErrors++;
        }

        Thread.Sleep(1000);
      }

      theForm.Progress(
        string.Empty, 100, string.Format("The clipping shapefile test has finished. {0} tests went wrong", numErrors));

      return numErrors == 0;
    }

    /// <summary>
    /// Run the WKT - shapefile conversion test
    /// </summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    internal static bool RunWktShapefileTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;

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
        else
        {
          numErrors++;
        }

        theForm.Progress(string.Empty, 100, " ");
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The WKT - shapefile conversion test has finished.");

      return numErrors == 0;
    }

    /// <summary>Run the intersection shapefile test</summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static bool RunIntersectionShapefileTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;
      theForm.Progress(string.Empty, 0, "-----------------------The intersection shapefile test has started.");

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

        if (!IntersectShapefile(lines[i], lines[i + 1], theForm))
        {
          numErrors++;
        }

        Thread.Sleep(1000);
      }

      theForm.Progress(
        string.Empty, 100, string.Format("The intersection shapefile test has finished. {0} tests went wrong", numErrors));

      return numErrors == 0;
    }

    /// <summary>
    /// Run the intersect shapefile test
    /// </summary>
    /// <param name="shapefilename">The shapefile name</param>
    /// <param name="overlayFilename">The name of the overlay shapefile</param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    internal static bool IntersectShapefile(string shapefilename, string overlayFilename, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return false;
        }

        if (!Helper.CheckShapefileLocation(overlayFilename, theForm))
        {
          return false;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return false;
        }

        var overlaySf = Fileformats.OpenShapefile(overlayFilename, theForm);
        if (overlaySf == null)
        {
          theForm.Error(string.Empty, "Opening overlay shapefile was unsuccessful");
          return false;
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        theForm.Progress(string.Empty, 0, "Start intersecting " + Path.GetFileName(shapefilename));

        // var intersectedSf = sf.Union(false, overlaySf, false);

        // With SHP_NULLSHAPE the most obvious type will be used:
        var intersectedSf = sf.GetIntersection(false, overlaySf, false, ShpfileType.SHP_NULLSHAPE, theForm);

        // Do some checks:)
        if (!Helper.CheckShapefile(sf, intersectedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return false;
        }

        Helper.ColorShapes(ref intersectedSf, 0, tkMapColor.BlueViolet, tkMapColor.DarkRed, true);

        // Save result:
        var newFilename = overlayFilename.Replace(".shp", "-intersected.shp");
        Helper.DeleteShapefile(newFilename);
        intersectedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        if (MyAxMap.AddLayer(intersectedSf, true) == -1)
        {
          theForm.Error(string.Empty, "Could not add the intersected shapefile to the map");
          return false;
        }

        // Wait to show the map:
        Application.DoEvents();

        //MyAxMap.AddLayer(overlaySf, true);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      return true;
    }

    /// <summary>
    /// Test the closest points method
    /// </summary>
    /// <param name="textfileLocation">
    /// The textfile Location.
    /// </param>
    /// <param name="theForm">
    /// The form
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    internal static bool RunClosestPointTest(string textfileLocation, Form1 theForm)
    {
      var numErrors = 0;
      theForm.Progress(string.Empty, 0, "-----------------------The closest point test has started.");

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

        if (!ClosestPoint(lines[i], lines[i + 1], theForm))
        {
          numErrors++;
        }

        Thread.Sleep(1000);
      }

      theForm.Progress(
        string.Empty, 100, string.Format("The closest point test has finished. {0} tests went wrong", numErrors));

      return numErrors == 0;
    }

    /// <summary>Get the closest point
    /// </summary>
    /// <param name="pointShapefile">
    /// The point shapefile.
    /// </param>
    /// <param name="searchShapefile">
    /// The search shapefile.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>True on success</returns>
    private static bool ClosestPoint(string pointShapefile, string searchShapefile, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(pointShapefile, theForm))
        {
          return false;
        }

        if (!Helper.CheckShapefileLocation(searchShapefile, theForm))
        {
          return false;
        }

        // Open the sf:
        var pointSf = Fileformats.OpenShapefile(pointShapefile, theForm);
        if (pointSf == null)
        {
          theForm.Error(string.Empty, "Opening point shapefile was unsuccessful");
          return false;
        }

        var searchSf = Fileformats.OpenShapefile(searchShapefile, theForm);
        if (searchSf == null)
        {
          theForm.Error(string.Empty, "Opening search shapefile was unsuccessful");
          return false;
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
          return false;
        }

        // Select the found shape:
        searchSf.SelectionColor = utils.ColorByName(tkMapColor.Red);
        searchSf.set_ShapeSelected(foundShapeID, true);

        var link = pointShp.ClosestPoints(searchSf.get_Shape(foundShapeID));
        if (link != null && link.numPoints > 1)
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

        // MyAxMap.Redraw();
        // Wait to show the map:
        Application.DoEvents();

        theForm.Progress(
          string.Empty,
          100,
          string.Format(
            "The closest shape is{0}, has a value of {1} and a length of {2}",
            closestIndex,
            searchSf.get_CellValue(0, closestIndex),
            minLength));

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

      return true;
    }

    /// <summary>
    /// Clip the shapefile
    /// </summary>
    /// <param name="shapefilename">
    /// The shapefile name
    /// </param>
    /// <param name="overlayFilename">
    /// The name of the overlay shapefile
    /// </param>
    /// <param name="theForm">
    /// The form
    /// </param>
    /// <returns>
    /// True on success
    /// </returns>
    private static bool ClipShapefile(string shapefilename, string overlayFilename, Form1 theForm)
    {
      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return false;
        }

        if (!Helper.CheckShapefileLocation(overlayFilename, theForm))
        {
          return false;
        }

        // Open the sf:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          theForm.Error(string.Empty, "Opening input shapefile was unsuccessful");
          return false;
        }

        var overlaySf = Fileformats.OpenShapefile(overlayFilename, theForm);
        if (overlaySf == null)
        {
          theForm.Error(string.Empty, "Opening overlay shapefile was unsuccessful");
          return false;
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

        // selecting only part of overlay, then the check about the varying 
	    // number of shapes in input/output will work
        int max = Math.Max(overlaySf.NumShapes / 2, 10);
        for (int i = 0; i < max; i++)
		{
            overlaySf.ShapeSelected[i] = true;
	    }

        var clippedSf = sf.Clip(false, overlaySf, true);

        // Do some checks:
        if (!Helper.CheckShapefile(sf, clippedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return false;
        }

        Helper.ColorShapes(ref clippedSf, 0, tkMapColor.DarkRed, tkMapColor.LightSeaGreen, true);

        // Count the resulting shapes it should be higher than the input shapefile.
        var numClippedSf = clippedSf.NumShapes;
        var numInputSf = sf.NumShapes;

        // Reset map:
        MyAxMap.Clear();

        if (numClippedSf == numInputSf)
        {
          // Nothing was clipped
          theForm.Error(string.Empty, "The resulting shapefile has the same number of shapes as the input shapefile. Either the input files are incorrect or the clipping function doesn't behaves as expected.");
          MyAxMap.AddLayer(sf, true);
          MyAxMap.AddLayer(overlaySf, true);

          // Wait to show the map:
          Application.DoEvents();

          return false;
        }
        
        // Save result:
        if (overlayFilename != null)
        {
          var newFilename = shapefilename.Replace(
            ".shp", string.Format("-{0}", Path.GetFileName(overlayFilename).Replace(".shp", "-clipped.shp")));
          Helper.DeleteShapefile(newFilename);
          clippedSf.SaveAs(newFilename, theForm);
          theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

          // Load the files:
          Fileformats.OpenShapefileAsLayer(overlayFilename, theForm, false);
          if (MyAxMap.AddLayer(clippedSf, true) == -1)
          {
            theForm.Error(string.Empty, "Could not add the clipped shapefile to the map");
            return false;
          }

          // Wait to show the map:
          Application.DoEvents();
        }
        else
        {
          return false;
        }
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "This clipping has finished.");
      return true;
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

        var wkt = sf.get_Shape(shapeIndex).ExportToWKT();
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
