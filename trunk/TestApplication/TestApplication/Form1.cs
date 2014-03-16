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
  using System.Linq;
  using System.Reflection;
  using System.Threading;
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

    #region Delegates

    /// <summary>The error callback</summary>
    /// <param name="keyOfSender">
    /// The key of sender.
    /// </param>
    /// <param name="errorMsg">
    /// The error msg.
    /// </param>
    public delegate void ErrorCallback(string keyOfSender, string errorMsg);

    /// <summary>The progress callback</summary>
    /// <param name="keyOfSender">
    /// The key of sender.
    /// </param>
    /// <param name="percent">
    /// The percentage.
    /// </param>
    /// <param name="message">
    /// The message.
    /// </param>
    public delegate void ProgressCallback(string keyOfSender, int percent, string message);

    #endregion

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
      System.Diagnostics.Debug.WriteLine(errorMsg);
      if (this.InvokeRequired)
      {
        this.Invoke(new ErrorCallback(this.Error), keyOfSender, errorMsg);
      }
      else
      {
            var msg = @"Error:" + errorMsg;
            Progressbox.AppendText(msg + Environment.NewLine);
      }
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
      if (percent == 0)
      {
        System.Diagnostics.Debug.WriteLine(message);
      }

      if (this.InvokeRequired)
      {
        this.Invoke(new ProgressCallback(this.Progress), keyOfSender, percent, message);
      }
      else
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
    }

    #endregion

    /// <summary>Form closing event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void Form1FormClosing(object sender, FormClosingEventArgs e)
    {
      Properties.Settings.Default.Save();
    }

    /// <summary>Form load event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void Form1Load(object sender, EventArgs e)
    {
      // Reset tiles;
      axMap1.Tiles.Visible = false;

      // Copy the map reference to the test methods:
      Fileformats.Map = axMap1;
      Tests.MyAxMap = axMap1;
      GeosTests.MyAxMap = axMap1;

      // Write version number:
      this.Progress(string.Empty, 100, string.Format("MapWinGIS version: {0} Test application version: {1}", this.axMap1.VersionNumber, Assembly.GetEntryAssembly().GetName().Version));
      
      // TODO: Also write GDAL version
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
        Fileformats.OpenShapefileAsLayer(e.filename, this, true);
        return;
      }
      
      // Load image file
      var img = new Image();
      if (img.CdlgFilter.Contains(ext))
      {
        Fileformats.OpenImageAsLayer(e.filename, this, true);
        return;
      }

      // Load grid file
      var grd = new Grid();
      if (grd.CdlgFilter.Contains(ext))
      {
        Fileformats.OpenGridAsLayer(e.filename, this, true);
        return;
      }

      // Don't know how to handle the dropped file:
      this.Error(string.Empty, "Don't know how to handle the dropped file");
      return;
    }

    #region Select file click event
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
      Tests.SelectTextfile(this.ImageInputfile, "Select text file with on every line the location of an image file");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectClipGridByPolygonInputfile(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.ClipGridByPolygonInputfile, "Select text file with files to use.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectShapefileToGridClick(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.ShapefileToGrid, "Select polygon shapefile to grid");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectRasterizeInputfileClick(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.RasterizeInputfile, "Select polygon shapefile to rasterize");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectBufferShapefileClick(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.BufferShapefileInput, "Select shapefile to buffer");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectSimplifyShapefileClick(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.SimplifyShapefileInput, "Select shapefile to simplify");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectAggregateShapefileClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.AggregateShapefileInput, "Select text file with on every line the location of a shapefile and every second line the field index");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectDissolveShapefileClick(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.DissolveShapefileInput, "Select shapefile to dissolve");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectClipShapefileInputClick(object sender, EventArgs e)
    {
      // Select text file with on every line the location of a shapefile
      Tests.SelectTextfile(this.ClipShapefileInput, "Select text file with on two lines the locations of the shapefiles");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectIntersectionShapefileFirstInputClick(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.IntersectionShapefileFirstInput, "Select first shapefile");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectIntersectionShapefileSecondInputClick(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.IntersectionShapefileSecondInput, "Select second shapefile");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectWktShapefileInputClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.WktShapefileInput, "Select text file with on each line the location of the shapefiles");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectSpatialIndexInputfileClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.SpatialIndexInputfile, "Select text file with on each line the location of the shapefiles");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectClosestPointShapefileInput1Click(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.ClosestPointShapefileInput1, "Select point shapefile");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectClosestPointShapefileInput2Click(object sender, EventArgs e)
    {
      Tests.SelectShapefile(this.ClosestPointShapefileInput2, "Select shapefile to get the closest point from.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectOGRInfoFileClick(object sender, EventArgs e)
    {
      Tests.SelectAnyfile(this.OGRInfoInputfile, "Select file");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectTilesInputfileClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.TilesInputfile, "Select text file with on each line the location of the shapefiles");
    }

    #endregion

    #region Run test click event
    /// <summary>Start Shapefile open tests</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunShapefileTestClick(object sender, EventArgs e)
    {
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunShapefileTest(ShapefileInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunImagefileTest(ImageInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunGridfileTest(this.GridInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunClipGridByPolygonTestClick(object sender, EventArgs e)
    {
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunClipGridByPolygonTest(this.ClipGridByPolygonInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunShapefileToGridTestClick(object sender, EventArgs e)
    {
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunShapefileToGridTest(this.ShapefileToGrid.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunRasterizeTestClick(object sender, EventArgs e)
    {
      Tests.RunRasterizeTest(this.RasterizeInputfile.Text, this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunBufferShapefileTestClick(object sender, EventArgs e)
    {
      GeosTests.RunBufferShapefileTest(this.BufferShapefileInput.Text, this);
    }

    #endregion

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunSimplifyShapefileTestClick(object sender, EventArgs e)
    {
      GeosTests.RunSimplifyShapefileTest(this.SimplifyShapefileInput.Text, this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunAggregateShapefileTestClick(object sender, EventArgs e)
    {
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunAggregateShapefileTest(this.AggregateShapefileInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunDissolveShapefileTestClick(object sender, EventArgs e)
    {
      GeosTests.RunDissolveShapefileTest(this.DissolveShapefileInput.Text, (int)Math.Floor(this.DissolveFieldindex.Value), this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunClipShapefileTestClick(object sender, EventArgs e)
    {
      GeosTests.RunClipShapefileTest(this.ClipShapefileInput.Text, this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunIntersectionShapefileTestClick(object sender, EventArgs e)
    {
      GeosTests.RunIntersectionShapefileTest(this.IntersectionShapefileFirstInput.Text, this.IntersectionShapefileSecondInput.Text, this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunWktShapefileTestClick(object sender, EventArgs e)
    {
      GeosTests.RunWktShapefileTest(this.WktShapefileInput.Text, this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunSpatialIndexTestClick(object sender, EventArgs e)
    {
      Tests.RunSpatialIndexTest(this.SpatialIndexInputfile.Text, this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunOGRInfoTestClick(object sender, EventArgs e)
    {
        Tests.RunOGRInfoTest(this.OGRInfoInputfile.Text, this);
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunAllFileFormatsTestsButtonClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.FileFormatsGroupBox);

      this.Progress(string.Empty, 0, "Done running all Fileformats tests.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunClosestPointTestClick(object sender, EventArgs e)
    {
      GeosTests.RunClosestPointTest(this.ClosestPointShapefileInput1.Text, this.ClosestPointShapefileInput2.Text, this);
    }

    /// <summary>Run all Spatial Operation tests at once</summary>
    /// <param name="sender">The sender</param>
    /// <param name="e">The event arguments</param>
    private void RunAllSpatialOperationTestsClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.SpatialOperationGroupBox);
      this.Progress(string.Empty, 0, "Done running all Spatial Operation tests.");
    }

    /// <summary>Run the Tiles load test</summary>
    /// <param name="sender">The sender</param>
    /// <param name="e">The event arguments</param>
    private void RunTilesLoadTestClick(object sender, EventArgs e)
    {
      Tests.RunTilesLoadTest(this.TilesInputfile.Text, this);
    }
  }
}
