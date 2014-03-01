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
  using System.IO;
  using System.Threading;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Static class to hold the tests methods</summary>
  internal static class Tests
  {
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
    internal static void RunShapefileTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The shapefile open tests have started.", Environment.NewLine));

      // Read text file:
      var lines = Helper.ReadTextfile(textfileLocation);

      foreach (var line in lines)
      {
        // Open shapefile:
        Fileformats.OpenShapefileAsLayer(line, theForm, true);
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The shapefile open tests have finished.");
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

        // Log:

        // Add to map:
        MyAxMap.AddLayer(sf, true);

        // Wait to show the map:
        Application.DoEvents();

        // Check:
        theForm.Progress(string.Empty, 0, "Shapefile has index: " + sf.HasSpatialIndex);

        // Now do some selecting to time without spatial index.
        var timeWithoutIndex = TimeSelectShapes(ref sf, theForm);

        // for debugging
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
        if (File.Exists(baseFilename + ".mwd") || File.Exists(baseFilename + ".dat"))
        {
          //theForm.Progress(string.Empty, 0, "The mwd file exists");
        }
        else
        {
          theForm.Error(string.Empty, "The mwd file does not exists");
        }

        if (File.Exists(baseFilename + ".mwx") || File.Exists(baseFilename + ".idx"))
        {
          //theForm.Progress(string.Empty, 0, "The mwx file exists");
        }
        else
        {
          theForm.Error(string.Empty, "The mwx file does not exists");
          continue;
        }

        // Now do some selecting to time wit spatial index.
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

    /// <summary>Time the select shapes method</summary>
    /// <param name="sf">The shapefile</param>
    /// <param name="theForm">The form</param>
    /// <returns>The needed time</returns>
    private static long TimeSelectShapes(ref Shapefile sf, ICallback theForm)
    {
      var boundBox = new Extents();
      var width = sf.Extents.xMax - sf.Extents.xMin;
      var height = sf.Extents.yMax - sf.Extents.yMin;
      var stopWatch = Stopwatch.StartNew();
      
      sf.SelectNone();
      boundBox.SetBounds(sf.Extents.xMin + (width / 3), sf.Extents.yMin + (height / 3), 0.0, sf.Extents.xMax - (width / 3), sf.Extents.yMax - (height / 3), 0.0);
      object result = null;
      if (!sf.SelectShapes(boundBox, 0, SelectMode.INTERSECTION, ref result))
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

          MyAxMap.Redraw();
        }
        else
        {
          theForm.Error(string.Empty, "No shapes found");
        }
      }

      return EndStopWatch(ref stopWatch, theForm);
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
    internal static void RunImagefileTest(string textfileLocation, Form1 theForm)
    {
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
      var lines = Helper.ReadTextfile(textfileLocation);
      foreach (var line in lines)
      {
        // Open image:
        Fileformats.OpenImageAsLayer(line, theForm, true);

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The image open tests have finished.");
      MyAxMap.Redraw();
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
    internal static void RunGridfileTest(string textfileLocation, Form1 theForm)
    {
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
        Fileformats.OpenGridAsLayer(line, theForm, true);

        // Wait a second to show something:
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The grid open tests have finished.");
      MyAxMap.Redraw();
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

    /// <summary>Run the Clip grid by polygon test</summary>
    /// <param name="gridFilename">
    /// The grid filename.
    /// </param>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunClipGridByPolygonTest(string gridFilename, string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Clip grid by polygon test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (gridFilename == string.Empty || shapefilename == string.Empty)
        {
          theForm.Error(string.Empty, "Input parameters are wrong");
          return;
        }

        var folder = Path.GetDirectoryName(gridFilename);
        if (folder == null)
        {
          theForm.Error(string.Empty, "Input parameters are wrong");
          return;
        }

        if (!Directory.Exists(folder))
        {
          theForm.Error(string.Empty, "Output folder doesn't exists");
          return;
        }

        if (!File.Exists(gridFilename))
        {
          theForm.Error(string.Empty, "Input grid file doesn't exists");
          return;
        }

        if (!File.Exists(shapefilename))
        {
          theForm.Error(string.Empty, "Input shapefile doesn't exists");
          return;
        }
        
        var resultGrid = Path.Combine(folder, "ClipGridByPolygonTest" + Path.GetExtension(gridFilename));
        if (File.Exists(resultGrid))
        {
          File.Delete(resultGrid);
        }

        var globalSettings = new GlobalSettings();
        globalSettings.ResetGdalError();
        var utils = new Utils { GlobalCallback = theForm };
        var sf = new Shapefile();
        sf.Open(shapefilename, theForm);
        var polygon = sf.get_Shape(0);

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
          Fileformats.OpenGridAsLayer(resultGrid, theForm, true);
        }
        else
        {
          theForm.Error(string.Empty, "No grid was created");
        }
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Clip grid by polygon test has finished.");
    }

    /// <summary>Run the Shapefile to grid test</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="theForm">
    /// The the form.
    /// </param>
    internal static void RunShapefileToGridTest(string shapefilename, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Shapefile to grid test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (!Helper.CheckShapefileLocation(shapefilename, theForm))
        {
          return;
        }

        var folder = Path.GetDirectoryName(shapefilename);
        if (folder != null)
        {
          var resultGridFilename = Path.Combine(folder, "ShapefileToGridTest.asc");
          if (File.Exists(resultGridFilename))
          {
            File.Delete(resultGridFilename);
          }

          // Setup grid header:
          const int NumCols = 100;
          const int NumRows = 100;
          var sf = Fileformats.OpenShapefile(shapefilename, theForm);
          if (sf == null)
          {
            return;
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
          }
          else
          {
            if (!resultGrid.Save(resultGridFilename, GridFileType.UseExtension, null))
            {
              theForm.Error(string.Empty, "Error in Grid.Save(): " + resultGrid.get_ErrorMsg(resultGrid.LastErrorCode));
            }
            else
            {
              Fileformats.OpenGridAsLayer(resultGridFilename, theForm, true);
              MyAxMap.AddLayer(sf, true);
            }
          }
        }
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }

      theForm.Progress(string.Empty, 100, "The Shapefile to grid test has finished.");
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
        string Output = utils.OGRInfo(fileName, String.Empty, Path.GetFileNameWithoutExtension(fileName), theForm);
        MessageBox.Show(Output, "OGRInfo Test", MessageBoxButtons.OK);
        theForm.Progress(string.Empty, 100, "The OGRInfo test has finished.");
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
          System.Diagnostics.Debug.WriteLine(options);
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

    /// <summary>Run Aggregate shapefile test</summary>
    /// <param name="textfileLocation">
    /// The location of the shapefile.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    internal static void RunAggregateShapefileTest(string textfileLocation, Form1 theForm)
    {
      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Aggregate shapefile test has started.", Environment.NewLine));

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

        AggregateShapefile(lines[i], fieldIndex, theForm);
        
        Application.DoEvents();
        Thread.Sleep(1000);
      }

      theForm.Progress(string.Empty, 100, "The Aggregate shapefile test has finished.");
    }

    /// <summary>Aggregate shapefile</summary>
    /// <param name="shapefilename">
    /// The shapefilename.
    /// </param>
    /// <param name="fieldIndex">
    /// The field index.
    /// </param>
    /// <param name="theForm">
    /// The form.
    /// </param>
    private static void AggregateShapefile(string shapefilename, int fieldIndex, Form1 theForm)
    {
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

        theForm.Progress(string.Empty, 0, "Start aggregating " + Path.GetFileName(shapefilename));
        var aggregatedSf = sf.AggregateShapes(false, fieldIndex);

        // The aggregate method is returning invalid shapes, fix them first:
        Shapefile fixedSf;

        theForm.Progress(string.Empty, 0, "Start fixing " + Path.GetFileName(shapefilename));
        aggregatedSf.GlobalCallback = theForm;
        if (!aggregatedSf.FixUpShapes(out fixedSf))
        {
          theForm.Error(string.Empty, "The fixup returned false");
          return;
        }

        // Close file, because we continue with the fixed version:
        aggregatedSf.Close();

        // Do some checks:)
        if (!Helper.CheckShapefile(sf, fixedSf, globalSettings.GdalLastErrorMsg, theForm))
        {
          return;
        }

        // Save result:
        var newFilename = shapefilename.Replace(".shp", "-aggregate.shp");
        Helper.DeleteShapefile(newFilename);
        fixedSf.SaveAs(newFilename, theForm);
        theForm.Progress(string.Empty, 100, "The resulting shapefile has been saved as " + newFilename);

        Helper.ColorShapes(ref fixedSf, 0, tkMapColor.YellowGreen, tkMapColor.RoyalBlue, true);

        // Load the files:
        MyAxMap.RemoveAllLayers();
        MyAxMap.AddLayer(fixedSf, true);

        theForm.Progress(
          string.Empty,
          100,
          string.Format(
            "The Aggregate shapefile now has {0} shapes instead of {1} and has {2} rows",
            fixedSf.NumShapes,
            sf.NumShapes,
            fixedSf.Table.NumRows));
      }
      catch (Exception exception)
      {
        theForm.Error(string.Empty, "Exception: " + exception.Message);
      }
    }

    /// <summary>End the stop watch and log the time needed</summary>
    /// <param name="stopWatch">The stop watch</param>
    /// <param name="theForm">The form</param>
    /// <returns>The elapsed time</returns>
    private static long EndStopWatch(ref Stopwatch stopWatch, ICallback theForm)
    {
      Application.DoEvents();
      stopWatch.Stop();
      var ts = stopWatch.Elapsed;
      var elapsedTime = string.Format("{0:00}:{1:00}:{2:00}.{3:00}", ts.Hours, ts.Minutes, ts.Seconds, ts.Milliseconds);
      //theForm.Progress(string.Empty, 0, "Time needed: " + elapsedTime);
      return stopWatch.ElapsedMilliseconds;
    }
  }
}
