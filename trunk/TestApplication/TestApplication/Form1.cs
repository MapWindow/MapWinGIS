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
  using System.Diagnostics;
  using System.IO;
  using System.Linq;
  using System.Reflection;
  using System.Runtime.InteropServices;
  using System.Windows.Forms;
  using AxMapWinGIS;
  using MapWinGIS;
  using _DMapEvents_MouseMoveEventHandler = AxMapWinGIS._DMapEvents_MouseMoveEventHandler;

  /// <summary>Defines the form</summary>
  [ComVisible(true)]
  public partial class Form1 : Form, ICallback
  {
    /// <summary>
    /// Initializes a new instance of the <see cref="Form1"/> class.
    /// </summary>
    public Form1()
    {
      InitializeComponent();
      Instance = this;
    }

    /// <summary>
    /// Gets Instance of the form
    /// </summary>
    public static Form1 Instance { get; private set; }

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

    /// <summary>
    /// Writes a message to output
    /// </summary>
    /// <param name="msg">Message to be written</param>
    public void WriteMsg(string msg)
    {
      Debug.WriteLine(msg);
      if (this.InvokeRequired)
      {
        this.Invoke(new ProgressCallback(this.Progress), string.Empty, string.Empty, msg);
      }
      else
      {
        Progressbox.AppendText(msg + Environment.NewLine);
      }
    }

    /// <summary>
    /// Writes an error message to output
    /// </summary>
    /// <param name="msg">Message to be written</param>
    public void WriteError(string msg)
    {
      var errorMsg = "Error: " + msg;
      Debug.WriteLine(errorMsg);
      if (this.InvokeRequired)
      {
        this.Invoke(new ErrorCallback(this.Error), string.Empty, errorMsg);
      }
      else
      {
        Progressbox.AppendText(errorMsg + Environment.NewLine);
      }
    }

    /// <summary>The error callback</summary>
    /// <param name="keyOfSender">
    /// The key of sender.
    /// </param>
    /// <param name="errorMsg">
    /// The error msg.
    /// </param>
    public void Error(string keyOfSender, string errorMsg)
    {
      Debug.WriteLine(errorMsg);
      if (this.InvokeRequired)
      {
        this.Invoke(new ErrorCallback(this.Error), keyOfSender, errorMsg);
      }
      else
      {
        Progressbox.AppendText(string.Format("{0}Error: {1}{0}", Environment.NewLine, errorMsg));
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
              this.Progressbox.AppendText(
                string.Format("{2}{0} - {1}{2}", DateTime.Now, message, Environment.NewLine));
            }

            break;
          case 100:
            this.Progressbox.AppendText(string.Format("{2}{0} - {1}  ", DateTime.Now, message, Environment.NewLine));
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
      // Save all settings:
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
      this.ResetMapSettings(false);

      this.axMap1.GlobalCallback = this;

      // Copy the map reference to the test methods:
      Fileformats.Map = this.axMap1;
      Tests.MyAxMap = this.axMap1;
      GeosTests.MyAxMap = this.axMap1;
      FileManagerTests.MyAxMap = this.axMap1;

      // Write version number:
      this.Progress(string.Empty, 100, string.Format("MapWinGIS version: {0} Test application version: {1}", this.axMap1.VersionNumber, Assembly.GetEntryAssembly().GetName().Version));

      // Also write GDAL version
      var utils = new Utils();
      var version = utils.GDALInfo(string.Empty, "--version", this);
      this.Progress(string.Empty, 100, string.Format("GDAL version: {0}", version));
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
      // Clear the map:
      this.ResetMapSettings(true);

      // Use the new AddLayerFromFilename method:
      this.axMap1.AddLayerFromFilename(e.filename, tkFileOpenStrategy.fosAutoDetect, true);

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
    private void SelectGridOpenInputClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(GridOpenInput, "Select text file with on every line the location of an grid file");
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
      Tests.SelectTextfile(this.ShapefileToGridInputfile, "Select text file with polygons");
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
      Tests.SelectTextfile(this.RasterizeInputfile, "Select text file with shapefiles to rasterize");
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
      Tests.SelectTextfile(this.BufferShapefileInput, "Select text file with shapefiles to buffer");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectAxMapClearInputClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.AxMapClearInput, "Select text file with shapefiles");
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
      Tests.SelectTextfile(this.SimplifyShapefileInput, "Select text file with shapefiles to simplify");
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
      Tests.SelectTextfile(this.DissolveShapefileInput, "Select text file with on every line the location of a shapefile and every second line the dissolve index");
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
      Tests.SelectTextfile(this.IntersectionShapefileInput, "Select text file with on two lines the locations of the shapefiles");
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
    private void SelectClosestPointInputClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.ClosestPointInput, "Select text file with on two lines the locations of the shapefiles");
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
      Tests.SelectTextfile(this.OGRInfoInputfile, "Select text file with on each line the location of a shapefile");
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
      Tests.SelectTextfile(this.TilesInputfile, "Select text file with on each line the location of a shapefile");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>    
    private void SelectAnalyzedFilesClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.AnalyzeFilesInput, "Select text file with on each line the location of a raster file");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectRasterAInputClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.RasterCalculatorInput, "Select text file with rasters and formulas");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectGridProxyInputClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.GridProxyInput, "Select text file with grids (tiff) and color schemas");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void SelectReclassifyInputClick(object sender, EventArgs e)
    {
      Tests.SelectTextfile(this.ReclassifyInput, "Select text file with grids (tiff)");
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
      this.ResetMapSettings(false);

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
      this.ResetMapSettings(false);

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
      this.ResetMapSettings(false);
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
      this.ResetMapSettings(false);
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunShapefileToGridTest(this.ShapefileToGridInputfile.Text, this);
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunRasterizeTest(this.RasterizeInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = GeosTests.RunBufferShapefileTest(this.BufferShapefileInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = GeosTests.RunSimplifyShapefileTest(this.SimplifyShapefileInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = GeosTests.RunDissolveShapefileTest(this.DissolveShapefileInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = GeosTests.RunClosestPointTest(this.ClosestPointInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = GeosTests.RunClipShapefileTest(this.ClipShapefileInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = GeosTests.RunIntersectionShapefileTest(this.IntersectionShapefileInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = GeosTests.RunWktShapefileTest(this.WktShapefileInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunSpatialIndexTest(this.SpatialIndexInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.ResetMapSettings(true);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunOGRInfoTest(this.OGRInfoInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
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
      this.Progress(string.Empty, 100, "Done running all Fileformats tests.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunAllGdalMethodsTestsClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.GdalMethodsGroupBox);
      this.Progress(string.Empty, 100, "Done running all GDAL Methods tests.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>    
    private void RunAllMiscTestsClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.MiscGroupBox);
      this.Progress(string.Empty, 100, "Done running all Miscellaneous tests.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunAllFilemanagerTestsClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.FileManagerGroupBox);
      this.Progress(string.Empty, 100, "Done running all File manager tests.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunAllTestClick(object sender, EventArgs e)
    {
      var groups = this.splitContainer2.Panel1.Controls.OfType<GroupBox>();
      var buttons = groups.SelectMany(groupBox => groupBox.Controls.OfType<Button>());
      var buttonsWithRunTag = buttons.Where(button => button.Tag != null && button.Tag.ToString() == "RunAll");
      var orderByTabIndex = buttonsWithRunTag.OrderBy(button => button.TabIndex);

      foreach (var button in orderByTabIndex)
      {
        button.PerformClick();
      }

      this.Progress(string.Empty, 100, "Done running all possible tests.");
    }

    /// <summary>Click event</summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunAllRasterToolTestsClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.RasterToolsGroupBox);
      this.Progress(string.Empty, 100, "Done running all Raster tools tests.");
    }

    /// <summary>Run all Spatial Operation tests at once</summary>
    /// <param name="sender">The sender</param>
    /// <param name="e">The event arguments</param>
    private void RunAllSpatialOperationTestsClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.SpatialOperationGroupBox);
      this.Progress(string.Empty, 100, "Done running all Spatial Operation tests.");
    }

    /// <summary>Run all GEOS tests at once</summary>
    /// <param name="sender">The sender</param>
    /// <param name="e">The event arguments</param>
    private void RunAllGeosMethodsTestsClick(object sender, EventArgs e)
    {
      Helper.RunAllTestsInGroupbox(this.GeosMethodsGroupBox);
      this.Progress(string.Empty, 100, "Done running all GEOS tests.");
    }

    /// <summary>Run the Tiles load test</summary>
    /// <param name="sender">The sender</param>
    /// <param name="e">The event arguments</param>
    private void RunTilesLoadTestClick(object sender, EventArgs e)
    {
      this.ResetMapSettings(true);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunTilesLoadTest(this.TilesInputfile.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>Make sure the user sees the color change</summary>
    /// <param name="sender">The sender</param>
    /// <param name="e">The event arguments</param>
    private void ButtonBackColorChanged(object sender, EventArgs e)
    {
      Application.DoEvents();
    }

    /// <summary>Make sure the user sees the color change</summary>
    /// <param name="sender">The sender</param>
    /// <param name="e">The event arguments</param>
    private void BtnPopulateClick(object sender, EventArgs e)
    {
      // Use folder open dialog if constant doesn't exists:
      var path = Constants.DATA_FILES_PATH;
      if (!Directory.Exists(Constants.DATA_FILES_PATH))
      {
        using (var folderBrowserDialog = new FolderBrowserDialog())
        {
          folderBrowserDialog.Description =
            @"Select the location of the data folder that contains the text files for the TestApplication";

          // Set default starting path:
          folderBrowserDialog.SelectedPath = this.GetFolderOfAssembly();

          // Do not allow the user to create new files via the FolderBrowserDialog. 
          folderBrowserDialog.ShowNewFolderButton = false;
          var result = folderBrowserDialog.ShowDialog();

          // Cancel button was not pressed. 
          if (result == DialogResult.OK)
          {
            path = folderBrowserDialog.SelectedPath;
          }
        }
      }

      // File formats:
      SetTextfileLocation(ShapefileInputfile, path, "shapefiles.txt");
      SetTextfileLocation(GridInputfile, path, "gridfiles.txt");
      SetTextfileLocation(ImageInputfile, path, "imagefiles.txt");

      // Spatial operations:
      SetTextfileLocation(ClipGridByPolygonInputfile, path, "ClipGridWithPolygon.txt");
      SetTextfileLocation(ShapefileToGridInputfile, path, "shapefiles.txt");
      SetTextfileLocation(AggregateShapefileInput, path, "aggregate.txt");

      // GDAL Methods:
      SetTextfileLocation(RasterizeInputfile, path, "shapefiles.txt");
      SetTextfileLocation(OGRInfoInputfile, path, "shapefiles.txt");
      
      // GEOS Methods
      SetTextfileLocation(BufferShapefileInput, path, "bufferShapefiles.txt");
      SetTextfileLocation(DissolveShapefileInput, path, "aggregate.txt");
      SetTextfileLocation(SimplifyShapefileInput, path, "simplifyShapefiles.txt");
      SetTextfileLocation(ClipShapefileInput, path, "clipping.txt");
      SetTextfileLocation(IntersectionShapefileInput, path, "intersection.txt");
      SetTextfileLocation(WktShapefileInput, path, "wkt.txt");
      SetTextfileLocation(ClosestPointInput, path, "closestPoint.txt");

      // File manager
      SetTextfileLocation(AnalyzeFilesInput, path, "filemanager.txt");
      SetTextfileLocation(GridOpenInput, path, "gridfiles.txt");

      // Misc
      SetTextfileLocation(AxMapClearInput, path, "shapefiles.txt");
      SetTextfileLocation(GridProxyInput, path, "gridProxy.txt");
      SetTextfileLocation(SpatialIndexInputfile, path, "shapefiles.txt");
      SetTextfileLocation(TilesInputfile, path, "tiles.txt");

      // Raster tools
      SetTextfileLocation(RasterCalculatorInput, path, "rasterCalulator.txt");
      SetTextfileLocation(ReclassifyInput, path, "reclassify.txt");

      // Save all settings:
      Properties.Settings.Default.Save();
    }

    /// <summary>Set the text file location of the input box</summary>
    /// <param name="textInputfile">
    /// The text inputfile.
    /// </param>
    /// <param name="path">
    /// The path.
    /// </param>
    /// <param name="filename">
    /// The filename.
    /// </param>
    private static void SetTextfileLocation(Control textInputfile, string path, string filename)
    {
      // First check if the current one is invalid:
      if (File.Exists(textInputfile.Text))
      {
        // nothing to do:
        return;
      }

      var newPath = Path.Combine(path, filename);
      textInputfile.Text = File.Exists(newPath) ? newPath : string.Empty;
    }

    /// <summary>
    /// Analyzes raster files, displaying possible open strategies
    /// </summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The e.
    /// </param>
    private void RunAnalyzeFilesClick(object sender, EventArgs e)
    {
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var result = FileManagerTests.RunAnalyzeFilesTest(AnalyzeFilesInput.Text, this);
      this.Progress(string.Empty, 100, "TEST RESULTS: " + (result ? "sucess" : "failed"));
      ((Button)sender).BackColor = result ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>
    /// Opens grid with different options and checks how the open strategy is chosen
    /// </summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The event arguments
    /// </param>
    private void RunGridOpenTestClick(object sender, EventArgs e)
    {
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = FileManagerTests.RunGridOpenTest(GridOpenInput.Text, this);
      this.Progress(string.Empty, 100, "TEST RESULTS: " + (retVal ? "sucess" : "failed"));
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    private void RunAxMapClearTest_Click(object sender, EventArgs e)
    {
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunAxMapClearTest(this.AxMapClearInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>
    /// Perform several raster calculations
    /// </summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The event arguments
    /// </param>
    private void RunRasterCalculatorTestClick(object sender, EventArgs e)
    {
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunRasterCalculatorTest(this.RasterCalculatorInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>
    /// Run grid proxy creation tests
    /// </summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The event arguments
    /// </param>
    private void RunGridProxyTest_Click(object sender, EventArgs e)
    {
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunGridProxyTest(this.GridProxyInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }

    /// <summary>
    /// Run reclassify tests
    /// </summary>
    /// <param name="sender">
    /// The sender.
    /// </param>
    /// <param name="e">
    /// The event arguments
    /// </param>
    private void RunReclassifyTestClick(object sender, EventArgs e)
    {
      this.ResetMapSettings(false);
      ((Button)sender).BackColor = System.Drawing.Color.Blue;
      var retVal = Tests.RunReclassifyTest(this.ReclassifyInput.Text, this);
      ((Button)sender).BackColor = retVal ? System.Drawing.Color.Green : System.Drawing.Color.Red;
    }


    private void axMap1_MouseMoveEvent(object sender, _DMapEvents_MouseMoveEvent e)
    {
      if (this.InvokeRequired)
        this.Invoke(new _DMapEvents_MouseMoveEventHandler(axMap1_MouseMoveEvent), sender, e);
      else
      {
        double Lat = 0.0, Lon = 0.0;

        if (axMap1.PixelToDegrees(e.x, e.y, ref Lon, ref Lat))
          this.toolStripStatusCoordLabel.Text = string.Format("{0:0.000}, {1:0.000}", Lat, Lon);
        else
        {
          double clientX = 0.0, clientY = 0.0;
          this.axMap1.PixelToProj(e.x, e.y, ref clientX, ref clientY);
          this.toolStripStatusCoordLabel.Text = string.Format("{0:0.00}, {1:0.00}", clientX, clientY);
        }

        this.statusStrip1.Refresh();
      }
    }

    /// <summary>
    /// Clear the map and reset settings
    /// </summary>
    /// <param name="showTiles">
    /// Show the Tiles.
    /// </param>
    private void ResetMapSettings(bool showTiles)
    {
      this.axMap1.Clear();

      this.axMap1.Tiles.Visible = showTiles;

      this.axMap1.Redraw();
      Application.DoEvents();

      this.axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;
      this.axMap1.ShowCoordinates = tkCoordinatesDisplay.cdmAuto;
      this.axMap1.ShowZoomBar = true;

      var gs = new GlobalSettings
      {
        ShapeOutputValidationMode = tkShapeValidationMode.TryFixProceedOnFailure,
        ShapeInputValidationMode = tkShapeValidationMode.TryFixProceedOnFailure,
        GeometryEngine = tkGeometryEngine.engineGeos,
        GridProxyFormat = tkGridProxyFormat.gpfTiffProxy,
        GridProxyMode = tkGridProxyMode.gpmAuto,
        ImageDownsamplingMode = tkInterpolationMode.imBilinear,
        ImageUpsamplingMode = tkInterpolationMode.imHighQualityBilinear,
        LoadSymbologyOnAddLayer = true,
        ZoomToFirstLayer = true,
        LabelsCollisionMode = tkCollisionMode.GlobalList,
        RasterOverviewCreation = tkRasterOverviewCreation.rocAuto,
        TiffCompression = tkTiffCompression.tkmJPEG,
        RasterOverviewResampling = tkGDALResamplingMethod.grmGauss
      };
    }

    private string GetFolderOfAssembly()
    {
      // Get the full location of the assembly with DaoTests in it
      var fullPath = Assembly.GetAssembly(typeof(Form1)).Location;

      // Get the folder that's in
      return Path.GetDirectoryName(fullPath);
    }
  }
}
