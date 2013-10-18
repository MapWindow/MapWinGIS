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
  using System.IO;
  using System.Linq;
  using System.Threading;
  using System.Windows.Forms;

  /// <summary>Static class to hold the tests methods</summary>
  internal static class Tests
  {

    /// <summary>
    /// Gets or sets Map.
    /// </summary>
    internal static AxMapWinGIS.AxMap Map { get; set; }

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
      // Open text file:
      if (!File.Exists(textfileLocation))
      {
        throw new FileNotFoundException("Cannot find text file.", textfileLocation);
      }

      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The shapefile open tests have started.", Environment.NewLine));

      // Open file, read line by line, skip lines starting with #
      var lines = File.ReadAllLines(textfileLocation);
      foreach (var line in lines.Where(line => !line.StartsWith("#") && line.Length != 0))
      {
        try
        {
          // Open shapefile:
          Fileformats.OpenShapefileAsLayer(line, theForm);

          // Wait a second to show something:
          Application.DoEvents();
          Thread.Sleep(1000);
        }
        catch (System.Runtime.InteropServices.SEHException sehException)
        {
          theForm.Error(string.Empty, "SEHException in RunShapefileTest: " + sehException.Message);
        }
      }

      theForm.Progress(string.Empty, 100, "The shapefile open tests have finished.");
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

      // Open file, read line by line, skip lines starting with #
      var lines = File.ReadAllLines(textfileLocation);
      foreach (var line in lines.Where(line => !line.StartsWith("#") && line.Length != 0))
      {
        try
        {
          // Open image:
          Fileformats.OpenImageAsLayer(line, theForm);

          // Wait a second to show something:
          Application.DoEvents();
          Thread.Sleep(1000);
        }
        catch (System.Runtime.InteropServices.SEHException sehException)
        {
          theForm.Error(string.Empty, "SEHException in RunImagefileTest: " + sehException.Message);
        }
      }

      theForm.Progress(string.Empty, 100, "The image open tests have finished.");
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

      // Open file, read line by line, skip lines starting with #
      var lines = File.ReadAllLines(textfileLocation);
      foreach (var line in lines.Where(line => !line.StartsWith("#") && line.Length != 0))
      {
        try
        {
          // Open image:
          Fileformats.OpenGridAsLayer(line, theForm);

          // Wait a second to show something:
          Application.DoEvents();
          Thread.Sleep(1000);
        }
        catch (System.Runtime.InteropServices.SEHException sehException)
        {
          theForm.Error(string.Empty, "SEHException in RunGridfileTest: " + sehException.Message);
        }
      }

      theForm.Progress(string.Empty, 100, "The grid open tests have finished.");
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
      var grd = new MapWinGIS.Grid();

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

    internal static void RunClipGridByPolygonTest(string GridFilename, string Shapefilename, Form1 theForm)
    {

      theForm.Progress(
        string.Empty,
        0,
        string.Format("{0}-----------------------{0}The Clip grid by polygon test has started.", Environment.NewLine));

      try
      {
        // Check inputs:
        if (GridFilename == string.Empty || Shapefilename == string.Empty)
        {
          theForm.Error(string.Empty, "Input parameters are wrong");
          return;
        }

        var folder = Path.GetDirectoryName(GridFilename);
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

        if (!File.Exists(GridFilename))
        {
          theForm.Error(string.Empty, "Input grid file doesn't exists");
          return;
        }

        if (!File.Exists(Shapefilename))
        {
          theForm.Error(string.Empty, "Input shapefile doesn't exists");
          return;
        }
        
        var resultGrid = Path.Combine(folder, "ClipGridByPolygonTest" + Path.GetExtension(GridFilename));
        if (File.Exists(resultGrid))
        {
          File.Delete(resultGrid);
        }

        var globalSettings = new MapWinGIS.GlobalSettings();
        globalSettings.ResetGdalError();
        var utils = new MapWinGIS.Utils { GlobalCallback = theForm };
        var sf = new MapWinGIS.Shapefile();
        sf.Open(Shapefilename, theForm);
        var polygon = sf.get_Shape(0);

        // It returns false even if it is created
        if (!utils.ClipGridWithPolygon(GridFilename, polygon, resultGrid, false))
        {
          var msg = "Failed to process: " + utils.get_ErrorMsg(utils.LastErrorCode);
          if (globalSettings.GdalLastErrorMsg != string.Empty)
          {
            msg += "GdalLastErrorMsg: " + globalSettings.GdalLastErrorMsg;
          }

          theForm.Error(string.Empty, msg);
        }

        if (File.Exists(resultGrid))
        {
          theForm.Progress(string.Empty, 100, resultGrid + " was successfully created");
          Fileformats.OpenGridAsLayer(resultGrid, theForm);
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
  }
}
