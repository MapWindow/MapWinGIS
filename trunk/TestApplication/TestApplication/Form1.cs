// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Form1.cs" company="MapWindow developers community - 2014">
//   MapWindow Open Source GIS
// </copyright>
// <summary>
//   Form to test some of the functionality of MapWinGIS
// </summary>
// --------------------------------------------------------------------------------------------------------------------
namespace TestApplication
{
    #region

    using System;
    using System.Diagnostics;
    using System.Drawing;
    using System.IO;
    using System.Linq;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.Windows.Forms;

    using AxMapWinGIS;

    using MapWinGIS;

    using TestApplication.Properties;

    using _DMapEvents_MouseMoveEventHandler = AxMapWinGIS._DMapEvents_MouseMoveEventHandler;

    #endregion

    /// <summary>Defines the form</summary>
    [ComVisible(true)]
    public partial class Form1 : Form, ICallback
    {
        #region Constructors and Destructors

        /// <summary>
        ///     Initializes a new instance of the <see cref="Form1" /> class.
        /// </summary>
        public Form1()
        {
            this.InitializeComponent();
            Instance = this;
        }

        #endregion

        #region Delegates

        /// <summary>The error callback</summary>
        /// <param name="keyOfSender">
        ///     The key of sender.
        /// </param>
        /// <param name="errorMsg">
        ///     The error msg.
        /// </param>
        public delegate void ErrorCallback(string keyOfSender, string errorMsg);

        /// <summary>The progress callback</summary>
        /// <param name="keyOfSender">
        ///     The key of sender.
        /// </param>
        /// <param name="percent">
        ///     The percentage.
        /// </param>
        /// <param name="message">
        ///     The message.
        /// </param>
        public delegate void ProgressCallback(string keyOfSender, int percent, string message);

        #endregion

        #region Public Properties

        /// <summary>
        ///     Gets Instance of the form
        /// </summary>
        public static Form1 Instance { get; private set; }

        #endregion

        #region Public Methods and Operators

        /// <summary>
        /// The error.
        /// </summary>
        /// <param name="errorMsg">
        /// The error msg.
        /// </param>
        public void Error(string errorMsg)
        {
            this.Error(string.Empty, errorMsg);
        }

        /// <summary>
        /// The error callback
        /// </summary>
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
                this.Progressbox.AppendText(string.Format("{0}Error: {1}{0}", Environment.NewLine, errorMsg));
            }
        }

        /// <summary>
        /// The progress.
        /// </summary>
        /// <param name="message">
        /// The message.
        /// </param>
        public void Progress(string message)
        {
            this.Progress(string.Empty, 100, message);
        }

        /// <summary>
        /// The progress callback
        /// </summary>
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
                        this.Progressbox.AppendText(
                            string.Format("{2}{0} - {1}  ", DateTime.Now, message, Environment.NewLine));
                        break;
                    default:
                        var msg = percent + @"% ... ";
                        this.Progressbox.AppendText(msg);
                        break;
                }
            }
        }

        /// <summary>
        /// Writes an error message to output
        /// </summary>
        /// <param name="msg">
        /// Message to be written
        /// </param>
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
                this.Progressbox.AppendText(errorMsg + Environment.NewLine);
            }
        }

        /// <summary>
        /// Writes a message to output
        /// </summary>
        /// <param name="msg">
        /// Message to be written
        /// </param>
        public void WriteMsg(string msg)
        {
            Debug.WriteLine(msg);
            if (this.InvokeRequired)
            {
                this.Invoke(new ProgressCallback(this.Progress), string.Empty, string.Empty, msg);
            }
            else
            {
                this.Progressbox.AppendText(msg + Environment.NewLine);
            }
        }

        #endregion

        #region Methods

        /// <summary>
        /// Set the text file location of the input box
        /// </summary>
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
        /// Handle the dropped file
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void AxMap1FileDropped(object sender, _DMapEvents_FileDroppedEvent e)
        {
            // Clear the map:
            this.ResetMapSettings(true);

            // Use the new AddLayerFromFilename method:
            this.axMap1.AddLayerFromFilename(e.filename, tkFileOpenStrategy.fosAutoDetect, true);
        }

        /// <summary>
        /// Make sure the user sees the color change
        /// </summary>
        /// <param name="sender">
        /// The sender
        /// </param>
        /// <param name="e">
        /// The event arguments
        /// </param>
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
            SetTextfileLocation(this.ShapefileInputfile, path, "shapefiles.txt");
            SetTextfileLocation(this.GridInputfile, path, "gridfiles.txt");
            SetTextfileLocation(this.ImageInputfile, path, "imagefiles.txt");

            // Spatial operations:
            SetTextfileLocation(this.ClipGridByPolygonInputfile, path, "ClipGridWithPolygon.txt");
            SetTextfileLocation(this.ShapefileToGridInputfile, path, "shapefiles.txt");
            SetTextfileLocation(this.AggregateShapefileInput, path, "aggregate.txt");

            // GDAL Methods:
            SetTextfileLocation(this.RasterizeInputfile, path, "shapefiles.txt");
            SetTextfileLocation(this.OGRInfoInputfile, path, "shapefiles.txt");

            // GEOS Methods
            SetTextfileLocation(this.BufferShapefileInput, path, "bufferShapefiles.txt");
            SetTextfileLocation(this.SingleSidedBufferInput, path, "singleSidedBuffer.txt");
            SetTextfileLocation(this.DissolveShapefileInput, path, "aggregate.txt");
            SetTextfileLocation(this.SimplifyShapefileInput, path, "simplifyShapefiles.txt");
            SetTextfileLocation(this.ClipShapefileInput, path, "clipping.txt");
            SetTextfileLocation(this.IntersectionShapefileInput, path, "intersection.txt");
            SetTextfileLocation(this.WktShapefileInput, path, "wkt.txt");
            SetTextfileLocation(this.ClosestPointInput, path, "closestPoint.txt");

            // File manager
            SetTextfileLocation(this.AnalyzeFilesInput, path, "filemanager.txt");
            SetTextfileLocation(this.GridOpenInput, path, "gridfiles.txt");

            // Misc
            SetTextfileLocation(this.AxMapClearInput, path, "shapefiles.txt");
            SetTextfileLocation(this.GridProxyInput, path, "gridProxy.txt");
            SetTextfileLocation(this.SpatialIndexInputfile, path, "shapefiles.txt");
            SetTextfileLocation(this.TilesInputfile, path, "tiles.txt");

            // Raster tools
            SetTextfileLocation(this.RasterCalculatorInput, path, "rasterCalulator.txt");
            SetTextfileLocation(this.ReclassifyInput, path, "reclassify.txt");

            // PostGIS
            SetTextfileLocation(this.PostGisCreateDbInput, path, "PostGISCreateDatabase.txt");
            SetTextfileLocation(this.PostGisPrivilegesInput, path, "PostGISDatabaseSettings.txt");
            SetTextfileLocation(this.PostGisImportSfInput, path, "PostGisImportSf.txt");
            SetTextfileLocation(this.PostGisInput, path, "PostGISLayers.txt");

            // Save all settings:
            Settings.Default.Save();
        }

        /// <summary>
        /// Make sure the user sees the color change
        /// </summary>
        /// <param name="sender">
        /// The sender
        /// </param>
        /// <param name="e">
        /// The event arguments
        /// </param>
        private void ButtonBackColorChanged(object sender, EventArgs e)
        {
            Application.DoEvents();
        }

        /// <summary>
        /// Form closing event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void Form1FormClosing(object sender, FormClosingEventArgs e)
        {
            // Save all settings:
            Settings.Default.Save();
        }

        /// <summary>
        /// Form load event
        /// </summary>
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
            this.Progress(
                string.Empty,
                100,
                string.Format(
                    "MapWinGIS version: {0} Test application version: {1}",
                    this.axMap1.VersionNumber,
                    Assembly.GetEntryAssembly().GetName().Version));

            // Also write GDAL version
            var utils = new Utils();
            var version = utils.GDALInfo(string.Empty, "--version", this);
            this.Progress(string.Empty, 100, string.Format("GDAL version: {0}", version));
        }

        /// <summary>
        /// The get folder of assembly.
        /// </summary>
        /// <returns>
        /// The <see cref="string"/>.
        /// </returns>
        private string GetFolderOfAssembly()
        {
            // Get the full location of the assembly with DaoTests in it
            var fullPath = Assembly.GetAssembly(typeof(Form1)).Location;

            // Get the folder that's in
            return Path.GetDirectoryName(fullPath);
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

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunAggregateShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunAggregateShapefileTest(this.AggregateShapefileInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Run all GEOS tests at once
        /// </summary>
        /// <param name="sender">
        /// The sender
        /// </param>
        /// <param name="e">
        /// The event arguments
        /// </param>
        private void RunAllGeosMethodsTestsClick(object sender, EventArgs e)
        {
            Helper.RunAllTestsInGroupbox(this.GeosMethodsGroupBox);
            this.Progress(string.Empty, 100, "Done running all GEOS tests.");
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Run all Spatial Operation tests at once
        /// </summary>
        /// <param name="sender">
        /// The sender
        /// </param>
        /// <param name="e">
        /// The event arguments
        /// </param>
        private void RunAllSpatialOperationTestsClick(object sender, EventArgs e)
        {
            Helper.RunAllTestsInGroupbox(this.SpatialOperationGroupBox);
            this.Progress(string.Empty, 100, "Done running all Spatial Operation tests.");
        }

        /// <summary>
        /// Click event
        /// </summary>
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
            ((Button)sender).BackColor = Color.Blue;
            var result = FileManagerTests.RunAnalyzeFilesTest(this.AnalyzeFilesInput.Text, this);
            this.Progress(string.Empty, 100, "TEST RESULTS: " + (result ? "sucess" : "failed"));
            ((Button)sender).BackColor = result ? Color.Green : Color.Red;
        }

        /// <summary>
        /// The run ax map clear test_ click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunAxMapClearTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunAxMapClearTest(this.AxMapClearInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunBufferShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunBufferShapefileTest(this.BufferShapefileInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunClipGridByPolygonTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunClipGridByPolygonTest(this.ClipGridByPolygonInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunClipShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunClipShapefileTest(this.ClipShapefileInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunClosestPointTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunClosestPointTest(this.ClosestPointInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunDissolveShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunDissolveShapefileTest(this.DissolveShapefileInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
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
            ((Button)sender).BackColor = Color.Blue;
            var retVal = FileManagerTests.RunGridOpenTest(this.GridOpenInput.Text, this);
            this.Progress(string.Empty, 100, "TEST RESULTS: " + (retVal ? "sucess" : "failed"));
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
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
        private void RunGridProxyTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunGridProxyTest(this.GridProxyInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Start Grid open tests
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunGridTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunGridfileTest(this.GridInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Start Image open tests
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunImageTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);

            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunImagefileTest(this.ImageInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunIntersectionShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunIntersectionShapefileTest(this.IntersectionShapefileInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunOgrInfoTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(true);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunOGRInfoTest(this.OGRInfoInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// The run open post gis test_ click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunOpenPostGisTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = PostGisTests.RunOpenPostGisLayers(this.PostGisInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;            
        }

        /// <summary>
        /// The run post gis create db_ click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunPostGisCreateDbClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = PostGisTests.RunPostGisCreateDatabase(this.PostGisCreateDbInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// The run post gis drop db click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunPostGisDropDbClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = PostGisTests.RunPostGisDropDatabase(this.PostGisDropDbInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// The run post gis import sf click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunPostGisImportSfClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(true);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = PostGisTests.RunPostGisImportSf(this.PostGisImportSfInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// The run post gis privileges click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunPostGisPrivilegesClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = PostGisTests.RunPostGisPostGisPrivileges(this.PostGisPrivilegesInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
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
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunRasterCalculatorTest(this.RasterCalculatorInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunRasterizeTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunRasterizeTest(this.RasterizeInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
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
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunReclassifyTest(this.ReclassifyInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Start Shapefile open tests
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);

            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunShapefileTest(this.ShapefileInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunShapefileToGridTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunShapefileToGridTest(this.ShapefileToGridInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunSimplifyShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunSimplifyShapefileTest(this.SimplifyShapefileInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunSingleSidedBufferTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunSingleSidedBuffer(this.SingleSidedBufferInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunSpatialIndexTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunSpatialIndexTest(this.SpatialIndexInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Run the Tiles load test
        /// </summary>
        /// <param name="sender">
        /// The sender
        /// </param>
        /// <param name="e">
        /// The event arguments
        /// </param>
        private void RunTilesLoadTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(true);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = Tests.RunTilesLoadTest(this.TilesInputfile.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void RunWktShapefileTestClick(object sender, EventArgs e)
        {
            this.ResetMapSettings(false);
            ((Button)sender).BackColor = Color.Blue;
            var retVal = GeosTests.RunWktShapefileTest(this.WktShapefileInput.Text, this);
            ((Button)sender).BackColor = retVal ? Color.Green : Color.Red;
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectAggregateShapefileClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.AggregateShapefileInput, 
                "Select text file with on every line the location of a shapefile and every second line the field index");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectAnalyzedFilesClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.AnalyzeFilesInput, 
                "Select text file with on each line the location of a raster file");
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectClipShapefileInputClick(object sender, EventArgs e)
        {
            // Select text file with on every line the location of a shapefile
            Tests.SelectTextfile(
                this.ClipShapefileInput, 
                "Select text file with on two lines the locations of the shapefiles");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectClosestPointInputClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.ClosestPointInput, 
                "Select text file with on two lines the locations of the shapefiles");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectDissolveShapefileClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.DissolveShapefileInput, 
                "Select text file with on every line the location of a shapefile and every second line the dissolve index");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectGridInputfileClick(object sender, EventArgs e)
        {
            // Select text file with on every line the location of an image file
            Tests.SelectTextfile(this.GridInputfile, "Select text file with on every line the location of an grid file");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectGridOpenInputClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.GridOpenInput, "Select text file with on every line the location of an grid file");
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectImageInputfileClick(object sender, EventArgs e)
        {
            // Select text file with on every line the location of an image file
            Tests.SelectTextfile(
                this.ImageInputfile, 
                "Select text file with on every line the location of an image file");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectIntersectionShapefileFirstInputClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.IntersectionShapefileInput, 
                "Select text file with on two lines the locations of the shapefiles");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectOgrInfoFileClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.OGRInfoInputfile, 
                "Select text file with on each line the location of a shapefile");
        }

        /// <summary>
        /// The select post gis create db click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectPostGisCreateDbClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.PostGisCreateDbInput, "Select text file with create statements");
        }

        /// <summary>
        /// The select post gis drop db click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectPostGisDropDbClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.PostGisDropDbInput, "Select text file with the Drop database query");
        }

        /// <summary>
        /// The select post gis import sf click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectPostGisImportSfClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.PostGisImportSfInput, "Select text file with locations of shapefiles to import");
        }

        /// <summary>
        /// The select postgis input click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectPostgisInputClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.PostGisInput, "Select text file with locations of PostGIS layers to open");
        }

        /// <summary>
        /// The select post gis privileges_ click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectPostGisPrivileges_Click(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.PostGisPrivilegesInput, 
                "Select text file with queries to set grants and privileges");
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// The select raster a input_ click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectRasterAInput_Click(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.RasterCalculatorInput, "Select text file with rasters and formulas");
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// The select reclassify input_ click.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectReclassifyInput_Click(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.RasterCalculatorInput, "Select text file with rasters");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectShapefileInputfileClick(object sender, EventArgs e)
        {
            // Select text file with on every line the location of a shapefile
            Tests.SelectTextfile(
                this.ShapefileInputfile, 
                "Select text file with on every line the location of a shapefile");
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectSingleSidedBufferClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(this.SingleSidedBufferInput, "Select text file with WKT to buffer");
        }

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectSpatialIndexInputfileClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.SpatialIndexInputfile, 
                "Select text file with on each line the location of the shapefiles");
        }

        /// <summary>
        /// Click event
        /// </summary>
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

        /// <summary>
        /// Click event
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void SelectWktShapefileInputClick(object sender, EventArgs e)
        {
            Tests.SelectTextfile(
                this.WktShapefileInput, 
                "Select text file with on each line the location of the shapefiles");
        }

        /// <summary>
        /// The ax map 1_ mouse move event.
        /// </summary>
        /// <param name="sender">
        /// The sender.
        /// </param>
        /// <param name="e">
        /// The e.
        /// </param>
        private void axMap1_MouseMoveEvent(object sender, _DMapEvents_MouseMoveEvent e)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new _DMapEvents_MouseMoveEventHandler(this.axMap1_MouseMoveEvent), sender, e);
            }
            else
            {
                double Lat = 0.0, Lon = 0.0;

                if (this.axMap1.PixelToDegrees(e.x, e.y, ref Lon, ref Lat))
                {
                    this.toolStripStatusCoordLabel.Text = string.Format("{0:0.000}, {1:0.000}", Lat, Lon);
                }
                else
                {
                    double clientX = 0.0, clientY = 0.0;
                    this.axMap1.PixelToProj(e.x, e.y, ref clientX, ref clientY);
                    this.toolStripStatusCoordLabel.Text = string.Format("{0:0.00}, {1:0.00}", clientX, clientY);
                }

                this.statusStrip1.Refresh();
            }
        }
        #endregion
    }
}