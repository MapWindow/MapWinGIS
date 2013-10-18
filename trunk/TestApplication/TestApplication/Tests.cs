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
  }
}
