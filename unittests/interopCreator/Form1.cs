using System.Diagnostics;

namespace interopCreator
{
    using System;
    using System.IO;
    using System.Windows.Forms;
    using MapWinGIS;

    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void CheckNewMethods()
        {
            axMap1.DeserializeLayer(1, "Test");
            var projection = new GeoProjection();
            projection.ImportFromProj4("g");
            var t = tkSavingMode.modeXMLOverwrite;
            axMap1.SaveMapState("r", true, true);

            axMap1.set_LayerSkipOnSaving(1, true);

            var sf = new Shapefile();
            var reprojectedCount = 0;
            sf.Reproject(projection, ref reprojectedCount);
            var geoProjection = sf.GeoProjection;
            var sfSimple = sf.SimplifyLines(10, false);

            var gridHeader = new GridHeader();
            var gridHeaderGeoProjection = gridHeader.GeoProjection;

            var ext = axMap1.MaxExtents;

            var utils = new Utils();
            utils.ClipGridWithPolygon("test", sfSimple.Shape[0], "new", false);

            utils.Polygonize(string.Empty, string.Empty, 1, false, string.Empty, "ESRI Shapefile", string.Empty, "DN", null);

            utils.GDALInfo(string.Empty, string.Empty, null);
            utils.TranslateRaster(string.Empty, string.Empty, string.Empty, null);

            utils.OGRInfo(string.Empty, string.Empty, null, null);
            utils.OGR2OGR(string.Empty, string.Empty, string.Empty, null);
            utils.GDALAddOverviews(string.Empty, string.Empty, string.Empty, null);
            utils.GDALBuildVrt(string.Empty, string.Empty, null);
            utils.GDALRasterize(string.Empty, string.Empty, string.Empty, null);
            utils.GDALWarp(string.Empty, string.Empty, string.Empty, null);
        }

        private void ClipGridWithPolygon()
        {
            var workingFolder = @"C:\dev\TestingScripts\ScriptData\ClipGridWithPolygon";
            var tempFolder = @"C:\dev\TestingScripts\Temp\ClipGridWithPolygon";

            var gridFilename = Path.Combine(workingFolder, "AHN2.asc");
            var sf = new Shapefile();
            sf.Open(Path.Combine(workingFolder, "Areas.shp"), null);
            var numShapes = sf.NumShapes;
            var clippedWrong = 0;
            var clippedGood = 0;

            // Needed for the new method:
            var utils = new Utils();
            var grd = new Grid();
            grd.Open(gridFilename, GridDataType.UnknownDataType, true, GridFileType.UseExtension, null);

            for (var i = 0; i < numShapes; i++)
            {
                var polygon = sf.get_Shape(i);
                var resultGrid = Path.Combine(tempFolder, "AHN2_clipped" + i + ".asc");

                // Using thr mwGeoProc version takes almost 4 hours with this data:
                // if (MapWinGeoProc.SpatialOperations.ClipGridWithPolygon(ref gridFilename, ref polygon, ref resultGrid))
                if (utils.ClipGridWithPolygon2(grd, polygon, resultGrid, false))
                {
                    clippedGood++;
                }
                else
                {
                    clippedWrong++;
                    /*
                    testsMethods.ReportMessage(
                      string.Format(
                        "Clipping grid with polygon number {0} failed. Error: {1}", i, MapWinGeoProc.Error.GetLastErrorMsg()));
                    */
                }
            }

            var msg = string.Empty;
            msg = clippedWrong == 0 ? "All tests successful!" : string.Format("{0} clippings went OK, {1} went wrong", clippedGood, clippedWrong);
            MessageBox.Show(msg);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            ClipGridWithPolygon();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            GridStatistics();
        }

        private static void GridStatistics()
        {
            const string workingFolder = @"D:\dev\GIS-Data\Siebe\NetCDF\";
            var currentProcess = Process.GetCurrentProcess();

            using (var writer = new StreamWriter(Path.Combine(workingFolder, "GridStatisticsForPolygon.log"), false))
            {
                currentProcess.Refresh();
                writer.WriteLine(DateTime.Now + " Starting. Total memory: " + FormatBytes(currentProcess.WorkingSet64));
                // Load shapefile with polygons:
                var sf = new ShapefileClass();
                if (!sf.Open(Path.Combine(workingFolder, "ARK.shp")))
                {
                    writer.WriteLine(DateTime.Now + " Could not open shapefile. Reason " + sf.ErrorMsg[sf.LastErrorCode]);
                    return;
                }

                var numShapes = sf.NumShapes;

                var netCdfFiles = Directory.GetFiles(workingFolder, "*.nc");
                var utils = new UtilsClass();
                var settings = new GlobalSettings();
                var amersfoort = new GeoProjectionClass();
                amersfoort.ImportFromEPSG(28992);

                // Check projection:
                if (!sf.GeoProjection.IsSame[amersfoort])
                {
                    sf.GeoProjection = amersfoort.Clone();
                }

                // Loop through all netCDF files in folder:
                foreach (var netCdfFile in netCdfFiles)
                {
                    var grd = new GridClass();
                    if (!grd.Open(netCdfFile))
                    {
                        writer.WriteLine(DateTime.Now + " Could not open {0}. Reason: {1}, GDAL Error: {2}", netCdfFile,
                            grd.ErrorMsg[grd.LastErrorCode], settings.GdalLastErrorMsg);
                        continue;
                    }

                    currentProcess.Refresh();
                    writer.WriteLine(DateTime.Now + "Loaded grid " + netCdfFile + " Current memory " + FormatBytes(currentProcess.WorkingSet64));

                    var header = grd.Header;
                    var noDataValue = (double)grd.Header.NodataValue;
                    var extents = grd.Extents;

                    // Set projection:
                    header.GeoProjection = amersfoort.Clone();

                    // Get each band:
                    var numBands = grd.NumBands;
                    for (var i = 0; i < numBands; i++)
                    {
                        if (!grd.OpenBand(i))
                        {
                            writer.WriteLine(DateTime.Now + " Could not open band {0}. Reason: {1}, GDAL Error: {2}", i,
                                grd.ErrorMsg[grd.LastErrorCode], settings.GdalLastErrorMsg);
                            continue;
                        }

                        currentProcess.Refresh();
                        writer.WriteLine(DateTime.Now + " Working with band {0}  Current memory: {1} ", i, FormatBytes(currentProcess.WorkingSet64));
                        var mean = 0d;
                        var min = 0d;
                        var max = 0d;

                        for (var j = 0; j < numShapes; j++)
                        {
                            var shp = sf.Shape[j];
                            if (
                                !utils.GridStatisticsForPolygon(grd, header, extents, shp, noDataValue, ref mean,
                                    ref min,
                                    ref max))
                            {
                                writer.WriteLine(DateTime.Now + " Error getting statistics: " + utils.ErrorMsg[utils.LastErrorCode]);
                            }

                            currentProcess.Refresh();
                            writer.WriteLine(DateTime.Now + " Mean: {0}, Min: {1}, Max: {2}, Current memory: {3}", mean, min, max, FormatBytes(currentProcess.WorkingSet64));
                        }
                    }

                    // Close grid:
                    grd.Close();
                    currentProcess.Refresh();
                    writer.WriteLine(DateTime.Now + " Closed grid Current memory: " + FormatBytes(currentProcess.WorkingSet64));
                }
                currentProcess.Refresh();
                writer.WriteLine(DateTime.Now + " Done. Current memory: " + FormatBytes(currentProcess.WorkingSet64));
            }

            Debug.WriteLine("Ready");
        }

        private static string FormatBytes(long bytes)
        {
            string[] Suffix = { "B", "KB", "MB", "GB", "TB" };
            int i;
            double dblSByte = bytes;
            for (i = 0; i < Suffix.Length && bytes >= 1024; i++, bytes /= 1024)
            {
                dblSByte = bytes / 1024.0;
            }

            return string.Format("{0:0.##} {1}", dblSByte, Suffix[i]);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            TestLabelSerialize();
        }

        private void TestLabelSerialize()
        {
            // As mentioned here https://github.com/MapWindow/MapWinGIS/issues/1
            var sf = new ShapefileClass();
            if (!sf.Open(@"D:\dev\GIS-Data\Issues\0001\LOT_N.shp"))
            {
                Debug.WriteLine(DateTime.Now + " Could not open shapefile. Reason " + sf.ErrorMsg[sf.LastErrorCode]);
                return;
            }

            if (sf.Labels.Count == 0)
            {
                Debug.WriteLine("Create labels");
                sf.GenerateLabels(0, tkLabelPositioning.lpCenter, true);
            }

            Debug.WriteLine(sf.Labels.Serialize());

            // Add shapefile to map:
            var hndl = axMap1.AddLayer(sf, true);

            var sf2 = axMap1.get_Shapefile(hndl);
            txtResults.Text = sf2.Labels.Serialize();
        }

        private void btn_ogrinfo_Click(object sender, EventArgs e)
        {
            var utils = new UtilsClass();
            var formats = utils.OGRInfo(@"D:\dev\GIS-Data\Djordje\map.osm", "--formats");
            txtResults.Text = formats;
        }

        private void btnOgr2Ogr_Click(object sender, EventArgs e)
        {
            GlobalSettings settings = new GlobalSettings();
            settings.ResetGdalError();
            UtilsClass utils = new MapWinGIS.UtilsClass();
            string options = "--config CONFIG_FILE \"D:\\dev\\GIS-Data\\Djordje\\osmconf.ini\" --config OSM_USE_CUSTOM_INDEXING NO -skipfailures -f \"ESRI Shapefile\"";
            bool result = utils.OGR2OGR(@"D:\dev\GIS-Data\Djordje\map.osm", @"D:\dev\GIS-Data\Djordje\osm-result", options);
            if (!result)
            {
                // Both line should be the same:
                Debug.WriteLine(utils.ErrorMsgFromObject(utils));
                Debug.WriteLine(utils.ErrorMsg[utils.LastErrorCode]);
                Debug.WriteLine(settings.GdalLastErrorMsg);
            }

            txtResults.Text = @"OGR2OGR result: " + result;
            if (!result) return;

            // Add files:
            string[] shapefileFilenames = Directory.GetFiles(@"D:\dev\GIS-Data\Djordje\osm-result\", "*.shp");
            foreach (string filename in shapefileFilenames)
            {
                axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosVectorLayer, true);
            }
        }

        private void btnGdalInfo_Click(object sender, EventArgs e)
        {
            GlobalSettings settings = new GlobalSettings();
            settings.ResetGdalError();
            UtilsClass utils = new MapWinGIS.UtilsClass();

            var filename = @"D:\dev\GIS-Data\HDF5\RAD_NL25_RAC_03H_201612040000.h5";
            var result = utils.GDALInfo(filename, "");
            if (!string.IsNullOrEmpty(result))
            {
                txtResults.Text = result;
                var hndle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
                if (hndle >= 1) return;

                txtResults.Text += @" GDAL Error: " + settings.GdalLastErrorMsg
                                   + axMap1.get_ErrorMsg(axMap1.LastErrorCode);
                var grd = new GridClass();
                var opened = grd.Open(filename);
                if (!opened)
                {
                    txtResults.Text += @" Could not open grid: " + grd.ErrorMsg[grd.LastErrorCode];
                    return;
                }

                if (!grd.OpenBand(0))
                {
                    txtResults.Text += @" Could not open grid band: " + grd.ErrorMsg[grd.LastErrorCode];
                    return;
                }

                hndle = axMap1.AddLayer(grd, true);
                if (hndle >= 1) return;

                txtResults.Text += @" Could not add grid: " + grd.ErrorMsg[grd.LastErrorCode];

                return;
            }

            txtResults.Text = @" GDAL Error: " + settings.GdalLastErrorMsg
                              + @" GdalPluginPath: " + settings.GdalPluginPath;

        }

        private void button4_Click(object sender, EventArgs e)
        {
            var settings = new GlobalSettings { OgrStringEncoding = tkOgrEncoding.oseUtf8 };

            var sf = new ShapefileClass();
            if (!sf.Open(@"D:\dev\GIS-Data\Issues\Persian\roads.shp"))
            {
                Debug.WriteLine(DateTime.Now + " Could not open shapefile. Reason " + sf.ErrorMsg[sf.LastErrorCode]);
                return;
            }

            if (sf.Labels.Count == 0)
            {
                Debug.WriteLine("Create labels");
                sf.GenerateLabels(1, tkLabelPositioning.lpLongestSegement, true);
            }

            // Add shapefile to map:
            var hndl = axMap1.AddLayer(sf, true);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            const string filename = @"D:\dev\MapwinGIS\GitHub\unittests\MapWinGISTests\Testdata\HDF5\test.h5";
            var subset = $"HDF5:\"{filename}\"://image1/image_data";
            // Open subdataset as grid:
            var grd = new GridClass();
            if (grd.Open(subset)) axMap1.AddLayer(grd, true);
        }
    }
}
