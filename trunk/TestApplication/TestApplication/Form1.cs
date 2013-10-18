// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Form1.cs" company="MapWindow Open Source GIS Community">
//   MapWindow Open Source GIS Community
// </copyright>
// <summary>
//   Form to test some of the functionality of MapWinGIS
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
  using System;
  using System.Windows.Forms;

  using MapWinGIS;

  /// <summary>Defines the form</summary>
  public partial class Form1 : Form, ICallback
  {
    /// <summary>
    /// Initializes a new instance of the <see cref="Form1"/> class.
    /// </summary>
    public Form1()
    {
      InitializeComponent();
    }

    #region ICallback Members

    /// <summary>The error callback</summary>
    /// <param name="keyOfSender">
    /// The key of sender.
    /// </param>
    /// <param name="errorMsg">
    /// The error msg.
    /// </param>
    public void Error(string keyOfSender, string errorMsg)
    {
      var msg = @"Error:" + errorMsg;
      Progressbox.AppendText(msg + Environment.NewLine);
    }

    /// <summary>The progress callback</summary>
    /// <param name="keyOfSender">
    /// The key of sender.
    /// </param>
    /// <param name="percent">
    /// The percent.
    /// </param>
    /// <param name="message">
    /// The message.
    /// </param>
    public void Progress(string keyOfSender, int percent, string message)
    {
      switch (percent)
      {
        case 0:
          if (message != string.Empty)
          {
            this.Progressbox.AppendText(message + Environment.NewLine);
          }

          break;
        case 100:
          this.Progressbox.AppendText(message + Environment.NewLine);
          break;
        default:
          var msg = percent + @"% ... ";
          this.Progressbox.AppendText(msg);
          break;
      }
    }

    #endregion

    /// <summary>Form load event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void Form1Load(object sender, EventArgs e)
    {
      Fileformats.Map = axMap1;
      Tests.Map = axMap1;
    }

    /// <summary>Form closing event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
// ReSharper disable MemberCanBeMadeStatic.Local
    private void Form1FormClosing(object sender, FormClosingEventArgs e)
// ReSharper restore MemberCanBeMadeStatic.Local
    {
      Properties.Settings.Default.Save();
    }

    /// <summary>Handle the dropped file</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void AxMap1FileDropped(object sender, AxMapWinGIS._DMapEvents_FileDroppedEvent e)
    {
      // Check extension:
      var ext = System.IO.Path.GetExtension(e.filename);
      if (ext == null)
      {
        return;
      }

      // Load shapefile
      if (ext == ".shp")
      {
        Fileformats.OpenShapefileAsLayer(e.filename, this);
        return;
      }
      
      // Track GDAL Errors:
      var settings = new GlobalSettings();

      // Load image file
      var img = new Image();
      if (img.CdlgFilter.Contains(ext))
      {
        Fileformats.OpenImageAsLayer(e.filename, this);
        return;
      }

      // Load grid file
      var grd = new Grid();
      if (grd.CdlgFilter.Contains(ext))
      {
        Fileformats.OpenGridAsLayer(e.filename, this);
        return;
      }

      // Don't know how to handle the dropped file:
      this.Error(string.Empty, "Don't know how to handle the dropped file");
      return;
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectShapefileInputfileClick(object sender, EventArgs e)
    {
      // Select text file with on every line the location of a shapefile
      Tests.SelectTextfile(ShapefileInputfile, "Select text file with on every line the location of a shapefile");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectGridInputfileClick(object sender, EventArgs e)
    {
      // Select text file with on every line the location of an image file
      Tests.SelectTextfile(GridInputfile, "Select text file with on every line the location of an grid file");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectImageInputfileClick(object sender, EventArgs e)
    {
      // Select text file with on every line the location of an image file
      Tests.SelectTextfile(ImageInputfile, "Select text file with on every line the location of an image file");
    }

    /// <summary>Start Shapefile open tests</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunShapefileTestClick(object sender, EventArgs e)
    {
      Tests.RunShapefileTest(ShapefileInputfile.Text, this);
    }

    /// <summary>Start Image open tests</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunImageTestClick(object sender, EventArgs e)
    {
      Tests.RunImagefileTest(ImageInputfile.Text, this);
    }

    /// <summary>Start Grid open tests</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunGridTestClick(object sender, EventArgs e)
    {
      Tests.RunGridfileTest(GridInputfile.Text, this);
    }
  }
}
