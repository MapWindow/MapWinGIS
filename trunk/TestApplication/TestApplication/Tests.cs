// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Tests.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   Static class to hold the tests methods
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
  using System;
  using System.Diagnostics;
  using System.Globalization;
  using System.IO;
  using System.Threading;
  using System.Windows.Forms;

  using MapWinGIS;
  using NCalc;

  /// <summary>Static class to hold the tests methods</summary>
  internal static class Tests
  {
    /// <summary>Are the tiles loaded or not</summary>
    private static bool tilesAreLoaded;

    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap MyAxMap { get; set; }

    /// <summary>Select a text file using an OpenFileDialog</summary>
    /// <param name="textBox">
    /// The text box.
    /// </param>
    /// <param name="title">
    /// The title.
    /// </param>
    internal static void SelectTextfile(Control textBox, string title)
    {
      using (var ofd = new OpenFileDialog
      {
        CheckFileExists = true,
        Filter = @"Text file (*.txt)|*.txt|All files|*.*",
        Multiselect = false,
        SupportMultiDottedExtensions = true,
        Title = title
      })
      {
        if (textBox.Text != string.Empty)
        {
          var folder = Path.GetDirectoryName(textBox.Text);
          if (folder != null)
          {
            if (Directory.Exists(folder))
            {
              ofd.InitialDirectory = folder;
            }
          }
        }

        if (ofd.ShowDialog() == DialogResult.OK)
        {
          textBox.Text = ofd.FileName;
        }
      }
    }

    /// <summary>
    /// Run shapefile open tests
    /// </summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <exception cref="FileNotFoundException">
    /// When the file is not found
    /// </exception>
    /// <returns>
    /// True when no errors
    /// </returns>
    internal static bool RunShapefileTest(string textfileLocation, Form1 theForm)
    {
      var retVal = true;

      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The shapefile open tests have started.", Environment.NewLine));

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      foreach (var line in lines)
      {
        // Open shapefile:
        if (Fileformats.OpenShapefileAsLayer(line, theForm, true) == -1)
        {
          // Something went wrong:
          retVal = false;
        }

        // Suddenly causes a SEH exception: 
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The shapefile open tests have finished.");
      return retVal;
    }

    /// <summary>
    /// Run tiles load tests
    /// </summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <returns>
    /// True when no errors.
    /// </returns>
    internal static bool RunTilesLoadTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The tiles load tests have started.", Environment.NewLine));

      var retVal = true;

      // Set up event handler:
      MyAxMap.TilesLoaded += MyAxMapTilesLoaded;

      // Enable logging:
      var logPath = Path.GetDirectoryName(Application.ExecutablePath) + "\\tiles.log";
      MyAxMap.Tiles.StartLogRequests(logPath, false);
      Debug.Print("Log is opened: " + MyAxMap.Tiles.LogIsOpened);

      // Enable tiling:
      MyAxMap.Tiles.GlobalCallback = theForm;
      MyAxMap.Tiles.Visible = true;
      MyAxMap.Tiles.AutodetectProxy();
      MyAxMap.Tiles.set_DoCaching(tkCacheType.Disk, true);

      // probably better to turn if off otherwise on second run nothing will be downloaded (everything in cache)  
      MyAxMap.Tiles.set_UseCache(tkCacheType.Disk, false);
      MyAxMap.Tiles.UseServer = true;

      // Do some logging
      theForm.Progress(string.Empty, 0, "DiskCacheFilename: " + MyAxMap.Tiles.DiskCacheFilename);
      theForm.Progress(string.Empty, 0, "CheckConnection: " + MyAxMap.Tiles.CheckConnection("http://www.google.com"));

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      // Get every first line and second line:
      for (var i = 0; i < lines.Count; i = i + 2)
      {
        if (i + 1 > lines.Count)
        {
          theForm.Error(string.Empty, "Input file is incorrect.");
          retVal = false;
          break;
        }

        int provId;

        if (!int.TryParse(lines[i + 1], out provId))
        {
          theForm.Progress(string.Empty, 0, "Use custom provider");

          // Add custom provider:
          MyAxMap.Tiles.Providers.Clear(true);
          provId = 1024 + MyAxMap.Tiles.Providers.Count;

          // TODO: Make tkTileProjection flexible, grab it from the map projection:
          if (!MyAxMap.Tiles.Providers.Add(provId, "Custom", lines[i + 1], tkTileProjection.Amersfoort, 0, 16))
          {
            theForm.Error(string.Empty, "Provider add error: " + MyAxMap.Tiles.get_ErrorMsg(MyAxMap.Tiles.LastErrorCode));
            retVal = false;
          }
        }

        // Load the tiles:
        if (!LoadTiles(lines[i], provId, theForm))
        {
          retVal = false;
        }
      }

      theForm.Error(string.Empty, !retVal ? "Tiles weren't loaded for some zooms" : "Tiles were loaded for all zoom levels");
      theForm.Progress(string.Empty, 100, "The tiles load tests have finished.");

      // Stop logging:
      MyAxMap.Tiles.StopLogRequests();

      return retVal;
    }

    /// <summary>Run the Spatial Index tests</summary>
    /// <param name="textfileLocation">The textfile location</param>
    /// <param name="theForm">The form.</param>
    internal static void RunSpatialIndexTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Spatial Index tests have started.", Environment.NewLine));

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      foreach (var line in lines)
      {
        // Clear map:
        MyAxMap.RemoveAllLayers();

        // Remove index files:
        var baseFilename = Path.Combine(Path.GetDirectoryName(line), Path.GetFileNameWithoutExtension(line));
        File.Delete(baseFilename + ".mwd");
        File.Delete(baseFilename + ".dat");
        File.Delete(baseFilename + ".mwx");
        File.Delete(baseFilename + ".idx");

        // Open shapefile:
        var sf = Fileformats.OpenShapefile(line, theForm);

        // Add to map:
        MyAxMap.AddLayer(sf, true);

        // Wait to show the map:
        Application.DoEvents();

        // Check:
        theForm.Progress(string.Empty, 0, "Shapefile has index: " + sf.HasSpatialIndex);

        // Now do some selecting to time without spatial index.
        var utils = new Utils { GlobalCallback = theForm };
        sf.SelectionColor = utils.ColorByName(tkMapColor.Yellow);
        var timeWithoutIndex = TimeSelectShapes(ref sf, theForm);

        // for debugging:
        Application.DoEvents();
        Thread.Sleep(1000);

        // Create index:
        if (!sf.CreateSpatialIndex(sf.Filename))
        {
          theForm.Error(string.Empty, "Error creating spatial index: " + sf.get_ErrorMsg(sf.LastErrorCode));
          continue;
        }

        // Check:
        theForm.Progress(string.Empty, 0, "SpatialIndexMaxAreaPercent: " + sf.SpatialIndexMaxAreaPercent);

        // Set index:
        sf.UseSpatialIndex = true;

        // Check:
        theForm.Progress(string.Empty, 0, "Shapefile has index: " + sf.HasSpatialIndex);

        // Check if the files are created:
        if (!(File.Exists(baseFilename + ".mwd") || File.Exists(baseFilename + ".dat")))
        {
          theForm.Error(string.Empty, "The mwd file does not exists");
        }

        if (!(File.Exists(baseFilename + ".mwx") || File.Exists(baseFilename + ".idx")))
        {
          theForm.Error(string.Empty, "The mwx file does not exists");
          continue;
        }

        // Now do some selecting to time with spatial index.
        sf.SelectionColor = utils.ColorByName(tkMapColor.Red);
        var timeWithIndex = TimeSelectShapes(ref sf, theForm);

        theForm.Progress(
          string.Empty,
          0,
          string.Format("Select shapes without spatial index took {0} seconds, with spatial index it took {1}", timeWithoutIndex / 1000.0, timeWithIndex / 1000.0));

        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The spatial index tests have finished.");
    }

    /// <summary>
    /// Run image open tests
    /// </summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <exception cref="FileNotFoundException">
    /// When the file is not found
    /// </exception>
    /// <returns>
    /// True when no errors.
    /// </returns>
    internal static bool RunImagefileTest(string textfileLocation, Form1 theForm)
    {
      var retVal = true;

      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The image open tests have started.", Environment.NewLine));

      // Read text file:
      int count = 1;
      var lines = Helper.ReadTextfile(textfileLocation);
      foreach (var line in lines)
      {
        theForm.Progress(string.Empty, 0, " ");
        theForm.Progress(string.Empty, 0, "Image: " + count);
        theForm.Progress(string.Empty, 0, "-------------------------------");
        count++;

        // Open image:
        var hndle = Fileformats.OpenImageAsLayer(line, theForm, true);

        if (hndle == -1)
        {
          retVal = false;
          continue;
        }

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);

        // now do some zooming:
        var img = MyAxMap.get_Image(hndle);
        DoSomeZooming(theForm, img.NumOverviews);
      }

      theForm.Progress(string.Empty, 100, "The image open tests have finished.");
      MyAxMap.Redraw();

      return retVal;
    }

    /// <summary>
    /// Run grid open tests
    /// </summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form with the callback implementation.
    /// </param>
    /// <exception cref="FileNotFoundException">
    /// When the file is not found
    /// </exception>
    /// <returns>
    /// True when no errors
    /// </returns>
    internal static bool RunGridfileTest(string textfileLocation, Form1 theForm)
    {
      var retVal = true;

      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The grid open tests have started.", Environment.NewLine));

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);
      foreach (var line in lines)
      {
        // Open image:
        if (Fileformats.OpenGridAsLayer(line, theForm, true) == -1)
        {
          retVal = false;
        }

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The grid open tests have finished.");
      MyAxMap.Redraw();

      return retVal;
    }

    /// <summary>Select a shapefile</summary>
    /// <param name="textBox">
    /// The text box.
    /// </param>
    /// <param name="title">
    /// The title.
    /// </param>
    internal static void SelectAnyfile(TextBox textBox, string title)
    {
      using (var ofd = new OpenFileDialog
      {
        CheckFileExists = true,
        Filter = @"All Files|*.*",
        Multiselect = false,
        SupportMultiDottedExtensions = true,
        Title = title
      })
      {
        if (textBox.Text != string.Empty)
        {
          var folder = Path.GetDirectoryName(textBox.Text);
          if (folder != null)
          {
            if (Directory.Exists(folder))
            {
              ofd.InitialDirectory = folder;
            }
          }
        }

        if (ofd.ShowDialog() == DialogResult.OK)
        {
          textBox.Text = ofd.FileName;
        }
      }
    }

    /// <summary>Select a grid file</summary>
    /// <param name="textBox">
    /// The text box.
    /// </param>
    /// <param name="title">
    /// The title.
    /// </param>
    internal static void SelectGridfile(TextBox textBox, string title)
    {
      var grd = new Grid();

      using (var ofd = new OpenFileDialog
      {
        CheckFileExists = true,
        Filter = grd.CdlgFilter,
        Multiselect = false,
        SupportMultiDottedExtensions = true,
        Title = title
      })
      {
        if (textBox.Text != string.Empty)
        {
          var folder = Path.GetDirectoryName(textBox.Text);
          if (folder != null)
          {
            if (Directory.Exists(folder))
            {
              ofd.InitialDirectory = folder;
            }
          }
        }

        if (ofd.ShowDialog() == DialogResult.OK)
        {
          textBox.Text = ofd.FileName;
        }
      }
    }

    /// <summary>Select a shapefile</summary>
    /// <param name="textBox">
    /// The text box.
    /// </param>
    /// <param name="title">
    /// The title.
    /// </param>
    internal static void SelectShapefile(TextBox textBox, string title)
    {
      using (var ofd = new OpenFileDialog
      {
        CheckFileExists = true,
        Filter = @"Shapefiles|*.shp",
        Multiselect = false,
        SupportMultiDottedExtensions = true,
        Title = title
      })
      {
        if (textBox.Text != string.Empty)
        {
          var folder = Path.GetDirectoryName(textBox.Text);
          if (folder != null)
          {
            if (Directory.Exists(folder))
            {
              ofd.InitialDirectory = folder;
            }
          }
        }

        if (ofd.ShowDialog() == DialogResult.OK)
        {
          textBox.Text = ofd.FileName;
        }
      }
    }

    /// <summary>
    /// Run the Clip grid by polygon test
    /// </summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>
    /// True when no errors
    /// </returns>
    internal static bool RunClipGridByPolygonTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Clip grid by polygon test has started.", Environment.NewLine));

      var retVal = true;

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      // Get every first line and second line:
      for (var i = 0; i < lines.Count; i = i + 2)
      {
        if (i + 1 > lines.Count)
        {
          theForm.Error(string.Empty, "Input file is incorrect.");
          retVal = false;
          break;
        }

        if (!ClipGridByPolygon(lines[i], lines[i + 1], theForm))
        {
          retVal = false;
        }
      }

      theForm.Progress(string.Empty, 100, "The Clip grid by polygon test has finished.");

      return retVal;
    }

    /// <summary>
    /// Run the Shapefile to grid test
    /// </summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    /// <returns>
    /// True when no errors
    /// </returns>
    internal static bool RunShapefileToGridTest(string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Shapefile to grid test has started.", Environment.NewLine));

      var retVal = true;

      MyAxMap.RemoveAllLayers();

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return false;
        }

        var folder = Path.GetDirectoryName(shapefilename);
        if (folder != null)
        {
          var resultGridFilename = Path.Combine(folder, "ShapefileToGridTest.asc");
          Helper.DeleteGridfile(resultGridFilename);

          // Setup grid header:
          const int NumCols = 100;
          const int NumRows = 100;
          var sf = Fileformats.OpenShapefile(shapefilename, theForm);
          if (sf == null)
          {
            return false;
          }

          double minX, minY, minZ, maxX, maxY, maxZ;
          sf.Extents.GetBounds(out minX, out minY, out minZ, out maxX, out maxY, out maxZ);
          var gridHeader = new GridHeader
          {
            NodataValue = -1,
            NumberCols = NumCols,
            NumberRows = NumRows,
            Projection = sf.Projection,
            Notes = "Created using ShapefileToGrid",
            XllCenter = minX,
            YllCenter = minY,
            dX = (maxX - minX) / NumCols,
            dY = (maxY - minY) / NumRows
          };

          var utils = new Utils { GlobalCallback = theForm };
          var resultGrid = utils.ShapefileToGrid(sf, false, gridHeader, 30, true, 1);
          if (resultGrid == null)
          {
            theForm.Error(string.Empty, "Error in ShapefileToGrid: " + utils.get_ErrorMsg(utils.LastErrorCode));
            retVal = false;
          }
          else
          {
            if (!resultGrid.Save(resultGridFilename, GridFileType.UseExtension, null))
            {
              theForm.Error(string.Empty, "Error in Grid.Save(): " + resultGrid.get_ErrorMsg(resultGrid.LastErrorCode));
              retVal = false;
            }
            else
            {
              Fileformats.OpenGridAsLayer(resultGridFilename, theForm, true);

              // Open shapefile:
              sf.DefaultDrawingOptions.LineColor = utils.ColorByName(tkMapColor.Black);
              sf.DefaultDrawingOptions.LineWidth = 1;
              sf.DefaultDrawingOptions.FillVisible = false;
              MyAxMap.AddLayer(sf, true);
            }
          }
        }
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
        retVal = false;
      }

      theForm.Progress(string.Empty, 100, "The Shapefile to grid test has finished.");

      return retVal;
    }

    /// <summary>Run the OGRInfo test</summary>
    /// <param name="fileName">
    /// The file.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunOGRInfoTest(string fileName, Form1 theForm)
    {
      var utils = new Utils { GlobalCallback = theForm };
      var output = utils.OGRInfo(fileName, "-so", Path.GetFileNameWithoutExtension(fileName), theForm);
      theForm.Progress(string.Empty, 100, output.Replace("\n", Environment.NewLine));
    }

    /// <summary>Run the Rasterize shapefile test</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunRasterizeTest(string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Rasterize shapefile test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        // First check if the MWShapeID field is present:
        var sf = Fileformats.OpenShapefile(shapefilename, theForm);
        if (sf == null)
        {
          return;
        }

        // Get target resolution. The values must be expressed in georeferenced units (-tr):
        double minX, minY, minZ, maxX, maxY, maxZ;
        sf.Extents.GetBounds(out minX, out minY, out minZ, out maxX, out maxY, out maxZ);

        const string FieldName = "MWShapeID";
        if (sf.Table.get_FieldIndexByName(FieldName) == -1)
        {
          theForm.Progress(string.Empty, 0, "Adding " + FieldName + " as field");

          if (!sf.StartEditingShapes(true, theForm))
          {
            theForm.Error(string.Empty, "Could not put shapefile in edit mode: " + sf.get_ErrorMsg(sf.LastErrorCode));
            return;
          }

          if (sf.EditAddField(FieldName, FieldType.INTEGER_FIELD, 0, 10) == -1)
          {
            theForm.Error(string.Empty, "Could not add the fieldname: " + sf.get_ErrorMsg(sf.LastErrorCode));
            return;
          }

          if (!sf.StopEditingShapes(true, true, theForm))
          {
            theForm.Error(string.Empty, "Could not end shapefile in edit mode: " + sf.get_ErrorMsg(sf.LastErrorCode));
            return;
          }
        }

        if (!sf.Close())
        {
          theForm.Error(string.Empty, "Could not close the shapefile: " + sf.get_ErrorMsg(sf.LastErrorCode));
          return;
        }

        var folder = Path.GetDirectoryName(shapefilename);
        var utils = new Utils { GlobalCallback = theForm };
        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        if (folder != null)
        {
          var outputFile = Path.Combine(folder, "GDALRasterizeTest.tif");
          if (File.Exists(outputFile))
          {
            File.Delete(outputFile);
          }

          var options = string.Format(
            "-a {0} -l {1} -of GTiff -a_nodata -999 -init -999 -ts 800 800",
            FieldName,
            Path.GetFileNameWithoutExtension(shapefilename));
          Debug.WriteLine(options);
          if (!utils.GDALRasterize(shapefilename, outputFile, options, theForm))
          {
            var msg = " in GDALRasterize: " + utils.get_ErrorMsg(utils.LastErrorCode);
            if (globalSettings.GdalLastErrorMsg != string.Empty)
            {
              msg += Environment.NewLine + "GdalLastErrorMsg: " + globalSettings.GdalLastErrorMsg;
            }

            theForm.Error(string.Empty, msg);
            return;
          }

          // Open the files:
          Fileformats.OpenImageAsLayer(outputFile, theForm, true);
        }

        Fileformats.OpenShapefileAsLayer(shapefilename, theForm, false);
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Rasterize shapefile test has finished.");
    }

    /// <summary>
    /// Tests the Clear method of the AxMap class
    /// </summary>
    /// <param name="shapefileLocation">Location of a shapefile</param>
    /// <param name="theForm">The main form of the application</param>
    /// <returns>True on success</returns>
    internal static bool RunAxMapClearTest(string shapefileLocation, Form1 theForm)
    {
      theForm.Progress(
            string.Empty,
            0,
            string.Format("{0}-----------------------{0}The AxMap.Clear() test has started.", Environment.NewLine));

      try
      {
        // Check the inputs
        if (!Helper.CheckShapefileLocation(shapefileLocation, theForm))
        {
          return false;
        }

        // Add the shapefile as a layer
        Fileformats.OpenShapefileAsLayer(shapefileLocation, theForm, true);

        // Draw something on the map
        int DrawHandle = MyAxMap.NewDrawing(tkDrawReferenceList.dlScreenReferencedList);
        MyAxMap.DrawCircle(50, 50, 50, 0x0, true);

        // Change some of the settings
        MyAxMap.ShowVersionNumber = true;
        MyAxMap.ShowCoordinates = tkCoordinatesDisplay.cdmDegrees;

        // Get the layer count
        int layerCount = MyAxMap.NumLayers;

        // Get the new projection after adding the shapefile
        tkMapProjection shapeProj = MyAxMap.Projection;

        // Clear everything from the map
        MyAxMap.Clear();

        // Test the projection
        if (shapeProj == tkMapProjection.PROJECTION_NONE)
          theForm.Progress(string.Empty, 10, "Could not test Projection.");
        else if (MyAxMap.Projection != tkMapProjection.PROJECTION_NONE)
        {
          theForm.Error(string.Empty, "Failed to reset Projection to default.");
          return false;
        }
        else
          theForm.Progress(string.Empty, 10, "Successfully reset Projection to default.");

        // Test the settings
        if (MyAxMap.ShowVersionNumber || MyAxMap.ShowCoordinates != tkCoordinatesDisplay.cdmAuto)
        {
          theForm.Error(string.Empty, "Failed to reset settings to defaults.");
          return false;
        }
        else
          theForm.Progress(string.Empty, 20, "Successfully reset settings to defaults.");

        // Make sure all the layers are removed
        if (layerCount != 2)
          theForm.Progress(string.Empty, 30, "Could not test removing layers.");
        else if (MyAxMap.NumLayers != 0)
        {
          theForm.Error(string.Empty, "Failed to remove all layers.");
          return false;
        }
        else
          theForm.Progress(string.Empty, 30, "Successfully removed all layers.");
      }
      catch (Exception ex)
      {
        theForm.Error(string.Empty, "Exception: " + ex.Message);
      }

      theForm.Progress(string.Empty, 100, "The AxMap.Clear() test has finished.");

      return true;
    }

    /// <summary>Run the raster calculator test</summary>
    /// <param name="textfileLocation">
    /// The textfile location.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns> True on success </returns>
    internal static bool RunRasterCalculatorTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Raster calculator test has started.", Environment.NewLine));

      var retVal = true;

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      // Get every first, second and third line:
      for (var i = 0; i < lines.Count; i = i + 3)
      {
        if (i + 1 > lines.Count)
        {
          theForm.Error(string.Empty, "Input file is incorrect. Not enough lines");
          break;
        }

        var stopWatch = Stopwatch.StartNew();
        retVal = RasterCalculatorTest(lines[i], lines[i + 1], lines[i + 2], theForm);
        var selectTime = EndStopWatch(ref stopWatch);
        theForm.Progress(string.Empty, 100, string.Format("The calculation of {0} took {1} seconds", lines[i + 2], selectTime / 1000.0));

        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The Raster calculator test has finished.");

      return retVal;
    }

    /// <summary>
    /// Run Aggregate shapefile test
    /// </summary>
    /// <param name="textfileLocation">
    /// The location of the shapefile.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>
    /// True when no errors
    /// </returns>
    internal static bool RunAggregateShapefileTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Aggregate shapefile test has started.", Environment.NewLine));

      var retVal = true;

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

        if (!AggregateShapefile(lines[i], fieldIndex, theForm))
        {
          retVal = false;
        }

        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The Aggregate shapefile test has finished.");

      return retVal;
    }

    /// <summary>Load the tiles for the shapefile</summary>
    /// <param name="shapefileLocation">
    /// The shapefile location.
    /// </param>
    /// <param name="provId">
    /// The prov id.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    /// <returns>True when no errors </returns>
    private static bool LoadTiles(string shapefileLocation, int provId, Form1 theForm)
    {
      var waitCount = 0;
      const int MaxWaitCount = 15;
      const int MaxSleepTime = 500;
      var hasErrors = false;

      // Open shapefile:
      var layerHandle = Fileformats.OpenShapefileAsLayer(shapefileLocation, theForm, true);
      MyAxMap.ZoomToLayer(layerHandle);

      theForm.Progress(string.Empty, 0, string.Format("Map projection: {0} ({1})", MyAxMap.GeoProjection.Name, MyAxMap.GeoProjection.GeogCSName));

      var sf = MyAxMap.get_Shapefile(layerHandle);

      if (sf == null)
      {
        theForm.Error(string.Empty, "Shapefile could not be loaded");
        return false;
      }

      // turn off the symbology to see the tiles  
      theForm.Progress(string.Empty, 0, string.Format("Layer projection: {0} ({1})", sf.GeoProjection.Name, sf.GeoProjection.GeogCSName));
      sf.DefaultDrawingOptions.FillVisible = false;
      sf.Categories.Clear();
      sf.Labels.Clear();
      sf.Charts.Clear();

      Thread.Sleep(MaxSleepTime);
      Application.DoEvents();

      // Set Tiles provider:
      MyAxMap.Tiles.ProviderId = provId;

      // Save some settings:
      var providerId = MyAxMap.Tiles.Providers.get_IndexByProviderId(MyAxMap.Tiles.ProviderId);
      var maxZoom = MyAxMap.Tiles.Providers.get_MaxZoom(providerId);
      var minZoom = MyAxMap.Tiles.Providers.get_MinZoom(providerId);
      const int StartZoom = 10;
      var numFailed = 0;

      theForm.Progress(
      string.Empty,
      0,
      string.Format("{0} has a min zoom of {1} and  a max zoom of {2}", MyAxMap.Tiles.ProviderName, minZoom, maxZoom));

      // start from current zoom after zooming to layer; when starting from minimum you won't 
      // return back to the same shapefile because of projection distortions
      // infact probably would better to take cities shapefile, take several random cities and zoom on the in particular
      MyAxMap.ZoomToTileLevel(StartZoom);

      Thread.Sleep(MaxSleepTime);
      Application.DoEvents();

      // Continue when tiles are loaded:
      while (!tilesAreLoaded && waitCount < MaxWaitCount)
      {
        Thread.Sleep(MaxSleepTime);
        Application.DoEvents();
        waitCount++;
      }

      if (!tilesAreLoaded)
      {
        theForm.Error(string.Empty, "No tiles have been loaded");
        hasErrors = true;
      }

      // Do some zooming:
      for (var zoom = StartZoom; zoom <= maxZoom - 2; zoom++)
      {
        theForm.Progress(string.Empty, 0, "Zooming to: " + zoom);
        MyAxMap.ZoomToTileLevel(zoom);

        // Continue when tiles are loaded:
        waitCount = 0;

        while (!tilesAreLoaded && waitCount < MaxWaitCount)
        {
          Thread.Sleep(MaxSleepTime);
          Application.DoEvents();
          waitCount++;
        }

        if (!tilesAreLoaded)
        {
          theForm.Error(string.Empty, "Failed to load tiles for zoom: " + zoom);
          hasErrors = true;
          numFailed++;
        }

        tilesAreLoaded = false;

        if (numFailed > 2)
        {
          // Tried several times, without luck. Stop trying:
          return false;
        }
      }

      return !hasErrors;
    }

    /// <summary>
    /// Aggregate shapefile
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
    /// True when no errors
    /// </returns>
    private static bool AggregateShapefile(string shapefilename, int fieldIndex, Form1 theForm)
    {
      var retVal = true;

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

        //bool hasInvalid = sf.HasInvalidShapes();

        theForm.Progress(string.Empty, 0, "Start aggregating " + Path.GetFileName(shapefilename));
        var aggregatedSf = sf.AggregateShapes(false, fieldIndex);
        var info = sf.LastOutputValidation;
        if (info != null)
        {
          Debug.Print("Shapefile returned: " + (aggregatedSf != null));
          if (aggregatedSf != null)
          {
            bool hasInvalidShapes = aggregatedSf.HasInvalidShapes();
            Debug.Print("Has invalid shapes: " + hasInvalidShapes);
          }
          Debug.Print("Operation is valid: " + info.IsValid);
        }

        // Check if the result still contained invalid shapes:
        Shapefile correctSf;
        //if (!aggregatedSf.HasInvalidShapes())
        //{
        correctSf = aggregatedSf;
        //  theForm.Progress(string.Empty, 0, "The aggregated shapefile has no invalid shapes");
        //  for (int i = 0; i < aggregatedSf.NumShapes; i++)
        //  {
        //    var shp = aggregatedSf.get_Shape(i);
        //    if (!shp.IsValid)
        //    {
        //      Debug.Print("Is valid index: " + shp.IsValidReason);
        //      Debug.Print(shp.ExportToWKT());
        //      aggregatedSf.set_ShapeSelected(i, true);
        //    }
        //    else
        //    {
        //      Debug.Print("This shape is valid");
        //    }
        //  }
        //}
        //else
        //{
        //  theForm.Progress(string.Empty, 0, string.Empty);
        //  theForm.Progress(string.Empty, 0, "Warning! The aggregated shapefile has invalid shapes");
        //  retVal = false;

        //  // The aggregate method is returning invalid shapes, fix them first:
        //  Shapefile fixedSf;

        //  theForm.Progress(string.Empty, 0, "Start fixing " + Path.GetFileName(shapefilename));
        //  aggregatedSf.GlobalCallback = theForm;
        //  if (!aggregatedSf.FixUpShapes(out fixedSf))
        //  {
        //    theForm.Error(string.Empty, "The fixup returned false");
        //    return false;
        //  }

        //  // Close file, because we continue with the fixed version:
        //  aggregatedSf.Close();

        //  // Do some checks:)
        //  if (!Helper.CheckShapefile(sf, fixedSf, globalSettings.GdalLastErrorMsg, theForm))
        //  {
        //    return false;
        //  }

        //  // Continue with this sf:
        //  correctSf = fixedSf;
        //}

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-aggregate.shp");
        Helper.DeleteShapefile(newFilename);
        correctSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        Helper.ColorShapes(ref correctSf, 0, tkMapColor.YellowGreen, tkMapColor.RoyalBlue, true);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(correctSf, true);
        //MyAxMap.AddLayer(sf, true);

        theForm.Progress(
          string.Empty,
          100,
          string.Format(
            "The Aggregate shapefile now has {0} shapes instead of {1} and has {2} rows",
            correctSf.NumShapes,
            sf.NumShapes,
            correctSf.Table.NumRows));
        theForm.Progress(string.Empty, 100, " ");

      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
        retVal = false;
      }

      return retVal;
    }

    /// <summary>End the stop watch and log the time needed</summary>
    /// <param name="stopWatch">The stop watch</param>
    /// <returns>The elapsed time</returns>
    private static long EndStopWatch(ref Stopwatch stopWatch)
    {
      Application.DoEvents();
      stopWatch.Stop();
      return stopWatch.ElapsedMilliseconds;
    }

    /// <summary>Time the select shapes method</summary>
    /// <param name="sf">The shapefile</param>
    /// <param name="theForm">The form</param>
    /// <returns>The needed time</returns>
    private static long TimeSelectShapes(ref Shapefile sf, ICallback theForm)
    {
      var boundBox = new Extents();
      var width = sf.Extents.xMax - sf.Extents.xMin;
      var height = sf.Extents.yMax - sf.Extents.yMin;

      sf.SelectNone();
      Application.DoEvents();
      Thread.Sleep(100);

      boundBox.SetBounds(sf.Extents.xMin + (width / 3), sf.Extents.yMin + (height / 3), 0.0, sf.Extents.xMax - (width / 3), sf.Extents.yMax - (height / 3), 0.0);
      object result = null;

      // Start stopwatch:
      var stopWatch = Stopwatch.StartNew();
      var returnCode = sf.SelectShapes(boundBox, 0, SelectMode.INTERSECTION, ref result);
      var selectTime = EndStopWatch(ref stopWatch);

      // Start selecting:
      if (!returnCode)
      {
        theForm.Error(string.Empty, "Error in SelectShapes: " + sf.get_ErrorMsg(sf.LastErrorCode));
      }
      else
      {
        var shapes = result as int[];
        if (shapes != null)
        {
          theForm.Progress(string.Empty, 0, "Number of shapes found: " + shapes.Length);
          foreach (var index in shapes)
          {
            sf.set_ShapeSelected(index, true);
          }

          Application.DoEvents();
          MyAxMap.Redraw();
        }
        else
        {
          theForm.Error(string.Empty, "No shapes found");
        }
      }

      return selectTime;
    }

    /// <summary>Do some zooming</summary>
    /// <param name="theForm">The form</param>
    private static void DoSomeZooming(ICallback theForm)
    {
      DoSomeZooming(theForm, -1);
    }

    /// <summary>Do some zooming</summary>
    /// <param name="theForm">The form</param>
    /// <param name="numOverviews">The number of overviews, -1 if not an image layer</param>
    private static void DoSomeZooming(ICallback theForm, int numOverviews)
    {
      // Zoom in several times:
      theForm.Progress(string.Empty, 0, "Zoom in several times");

      // Initial zooms for shapefiles and grids:
      var numZooms = 4;

      if (numOverviews != -1)
      {
        // image layer
        numZooms = numOverviews == 0 ? 2 : 4;
      }

      for (var i = 0; i < numZooms; i++)
      {
        MyAxMap.ZoomIn(0.2);
        Application.DoEvents();
      }

      // Zoom out again:
      theForm.Progress(string.Empty, 0, "Zoom out again");
      for (var i = 0; i < numZooms - 1; i++)
      {
        MyAxMap.ZoomOut(0.2);
        Application.DoEvents();
      }

      // Zoom max extent:
      theForm.Progress(string.Empty, 0, "Zoom max extent");
      MyAxMap.ZoomToMaxExtents();
    }

    /// <summary>
    /// Clip Grid by Polygon
    /// </summary>
    /// <param name="gridFilename">
    /// The grid filename.
    /// </param>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>
    /// True when no errors
    /// </returns>
    private static bool ClipGridByPolygon(string gridFilename, string shapefilename, Form1 theForm)
    {
      var retVal = true;

      try
      {
        // Clear the map:
        MyAxMap.RemoveAllLayers();
        Application.DoEvents();

        // Check inputs:
        if (gridFilename == string.Empty || shapefilename == string.Empty)
        {
          theForm.Error(string.Empty, "Input parameters are wrong");
          return false;
        }

        var folder = Path.GetDirectoryName(gridFilename);
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

        if (!File.Exists(gridFilename))
        {
          theForm.Error(string.Empty, "Input grid file doesn't exists");
          return false;
        }

        if (!File.Exists(shapefilename))
        {
          theForm.Error(string.Empty, "Input shapefile doesn't exists");
          return false;
        }

        var resultGrid = Path.Combine(folder, "ClipGridByPolygonTest" + Path.GetExtension(gridFilename));
        Helper.DeleteGridfile(resultGrid);

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        var utils = new Utils { GlobalCallback = theForm };
        var sf = new Shapefile();
        sf.Open(shapefilename, theForm);

        // Get one polygon the clip with:
        var index = new Random().Next(sf.NumShapes - 1);
        var polygon = sf.get_Shape(index);

        // It returns false even if it is created
        if (!utils.ClipGridWithPolygon(gridFilename, polygon, resultGrid, false))
        {
          var msg = "Failed to process: " + utils.get_ErrorMsg(utils.LastErrorCode);
          if (globalSettings.GdalLastErrorMsg != string.Empty)
          {
            msg += Environment.NewLine + "GdalLastErrorMsg: " + globalSettings.GdalLastErrorMsg;
          }

          theForm.Error(string.Empty, msg);
        }

        if (File.Exists(resultGrid))
        {
          theForm.Progress(string.Empty, 100, resultGrid + " was successfully created");

          // Add the layers:
          //Fileformats.OpenGridAsLayer(gridFilename, theForm, true);
          MyAxMap.AddLayer(sf, true);
          Fileformats.OpenGridAsLayer(resultGrid, theForm, false);

          Application.DoEvents();
        }
        else
        {
          theForm.Error(string.Empty, "No grid was created");
          retVal = false;
        }
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
        retVal = false;
      }

      return retVal;
    }

    /// <summary>TilesLoaded event handle</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private static void MyAxMapTilesLoaded(object sender, AxMapWinGIS._DMapEvents_TilesLoadedEvent e)
    {
      tilesAreLoaded = true;
    }

    /// <summary>The raster calculator test</summary>
    /// <param name="rasterA">The first raster</param>
    /// <param name="rasterB">The second raster</param>
    /// <param name="formula">The formula</param>
    /// <param name="theForm">The form</param>
    /// <returns>True on success</returns>
    private static bool RasterCalculatorTest(string rasterA, string rasterB, string formula, Form1 theForm)
    {
      bool retVal;

      try
      {
        // Clear the map:
        MyAxMap.Clear();
        Application.DoEvents();

        // Check inputs:
        if (rasterA == string.Empty || rasterB == string.Empty)
        {
          theForm.Error(string.Empty, "Input parameters are wrong");
          return false;
        }

        var folder = Path.GetDirectoryName(rasterA);
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

        if (!File.Exists(rasterA))
        {
          theForm.Error(string.Empty, "First raster file doesn't exists");
          return false;
        }

        if (!File.Exists(rasterB))
        {
          theForm.Error(string.Empty, "Second raster file doesn't exists");
          return false;
        }

        var resultRaster = Path.Combine(folder, "RasterCalculatorTest.tif");
        Helper.DeleteGridfile(resultRaster);

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        var utils = new Utils { GlobalCallback = theForm };

        string[] names = { rasterA, rasterB };
        var expression = formula.Replace("[A]", string.Format("[{0}@1]", Path.GetFileName(rasterA))).Replace(
          "[B]", string.Format("[{0}@1]", Path.GetFileName(rasterB)));

        string errorMsg;
        retVal = utils.CalculateRaster(names, expression, resultRaster, "GTiff", 0f /* no data value */, theForm /*callback */, out errorMsg);

        if (!retVal)
        {
          var msg = "Failed to process: " + utils.get_ErrorMsg(utils.LastErrorCode);
          if (globalSettings.GdalLastErrorMsg != string.Empty)
          {
            msg += Environment.NewLine + "GdalLastErrorMsg: " + globalSettings.GdalLastErrorMsg;
          }

          theForm.Error(string.Empty, msg);
        }

        if (retVal && File.Exists(resultRaster))
        {
          theForm.Progress(string.Empty, 100, resultRaster + " was successfully created");

          // Do some checking:
          retVal = CheckRasterCalculator(rasterA, rasterB, resultRaster, formula, theForm);
          if (retVal)
          {
            theForm.Progress(string.Empty, 100, "The random check was successful");
          }

          // Add the layer:
          theForm.Progress(string.Empty, 100, "Adding the calculated raster.");
          Fileformats.AddLayer(resultRaster, theForm);

          Application.DoEvents();
        }
        else
        {
          theForm.Error(string.Empty, "No grid was created");
        }
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
        retVal = false;
      }

      return retVal;
    }

    /// <summary>Get some random cells and recalculate the values</summary>
    /// <param name="rasterA">
    /// The first raster
    /// </param>
    /// <param name="rasterB">
    /// The second raster
    /// </param>
    /// <param name="resultRaster">
    /// The result raster.
    /// </param>
    /// <param name="formula">
    /// The formula.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    /// <returns>True when OK</returns>
    private static bool CheckRasterCalculator(string rasterA, string rasterB, string resultRaster, string formula, Form1 theForm)
    {
      var retVal = false;
      var grdA = new Grid { GlobalCallback = theForm };
      var grdB = new Grid { GlobalCallback = theForm };
      var grdC = new Grid { GlobalCallback = theForm };

      try
      {
        // Open the rasters:
        if (!grdA.Open(rasterA, GridDataType.UnknownDataType, false, GridFileType.UseExtension, theForm))
        {
          theForm.Error(string.Empty, "Something went wrong opening the first raster: " + grdA.get_ErrorMsg(grdA.LastErrorCode));
          return false;
        }

        if (!grdB.Open(rasterB, GridDataType.UnknownDataType, false, GridFileType.UseExtension, theForm))
        {
          theForm.Error(string.Empty, "Something went wrong opening the first raster: " + grdB.get_ErrorMsg(grdB.LastErrorCode));
          return false;
        }

        if (!grdC.Open(resultRaster, GridDataType.UnknownDataType, false, GridFileType.UseExtension, theForm))
        {
          theForm.Error(string.Empty, "Something went wrong opening the first raster: " + grdC.get_ErrorMsg(grdC.LastErrorCode));
          return false;
        }

        // Rasters have the same size and the same number of columns and rows.
        var numColumns = grdA.Header.NumberCols;
        var numRows = grdA.Header.NumberCols;
        var nodata = grdA.Header.NodataValue; // Returns an object
        var rnd = new Random();
        var randomColumn = 0; // = rnd.Next(numColumns);
        var randomRow = 0; // = rnd.Next(numRows);

        // read cell:
        var valA = nodata;

        // Check for nodata value:
        while (valA == nodata)
        {
          randomColumn = rnd.Next(numColumns);
          randomRow = rnd.Next(numRows);
          valA = grdA.get_Value(randomColumn, randomRow); // Returns an object
        }

        Debug.WriteLine("valA: " + valA);

        // Get values from other rasters, make expression and evaluate:
        var valB = grdB.get_Value(randomColumn, randomRow); // Returns an object
        Debug.WriteLine("valB: " + valB);
        var valC = grdC.get_Value(randomColumn, randomRow); // Returns an object
        Debug.WriteLine("valC: " + valC);
        var expression = formula.Replace("[A]", string.Format("({0})", ((double)valA).ToString(CultureInfo.InvariantCulture))).Replace("[B]", string.Format("({0})", ((double)valB).ToString(CultureInfo.InvariantCulture)));
        theForm.Progress(string.Empty, 100, "Checking expression: " + expression);
        var e = new Expression(expression);
        var result = e.Evaluate(); // Returns an object
        if (result == null)
        {
          theForm.Error(string.Empty, "Could not check this expression: " + e.Error);
          return false;
        }

        // TODO: Most likely this can be done in a better way, but I don't know how:
        // Without it I keep getting invalid casts with result of valC:
        var goodCalculation = false;
        if (result.GetType() == typeof(int))
        {
          if (valC.GetType() == typeof(int))
          {
            if (Math.Abs((int)valC - (int)result) < 1)
            {
              goodCalculation = true;
            }
          }
          else
          {
            if (Math.Abs((double)valC - (int)result) < 1)
            {
              goodCalculation = true;
            }
          }
        }
        else
        {
          if (valC.GetType() == typeof(int))
          {
            if (Math.Abs((int)valC - (double)result) < 1)
            {
              goodCalculation = true;
            }
          }
          else
          {
            if (Math.Abs((double)valC - (double)result) < 1)
            {
              goodCalculation = true;
            }
          }
        }

        if (goodCalculation)
        {
          theForm.Progress(string.Empty, 100, "Checking some random values was correct: " + expression);
          retVal = true;
        }
        else
        {
          theForm.Error(
            string.Empty,
            string.Format("The expression {0} returned {1}. But {2} was expected!", expression, result, valC));
        }
      }
      catch (InvalidCastException ex)
      {
        // TODO: Don't know how to fix this, continue for now.
        theForm.Error(string.Empty, "InvalidCastException in CheckRasterCalculator: " + ex.Message);
      }
      catch (Exception ex)
      {
        theForm.Error(string.Empty, "Error in CheckRasterCalculator: " + ex.Message);
      }
      finally
      {
        // Close the rasters again:
        grdA.Close();
        grdB.Close();
        grdC.Close();
      }

      return retVal;
    }
  }
}