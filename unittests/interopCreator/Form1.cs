using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace interopCreator
{
    public partial class Form1 : Form, ICallback
    {
        private static readonly GlobalSettings _settings = new GlobalSettings();

        public Form1()
        {
            InitializeComponent();
            _settings.ApplicationCallback = this;
            _settings.SetHttpUserAgent("MapWinGIS Testapplication");
            const string tilesLogging = @"D:\tmp\axmap.tiles\TileRequests.log";
            _settings.StartLogTileRequests(tilesLogging);
            txtResults.Text += @"Tiles logging at " + tilesLogging;
            axMap1.MapUnits = tkUnitsOfMeasure.umKilometers;
            axMap1.ShowCoordinates = tkCoordinatesDisplay.cdmMapUnits;
        }

        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            txtResults.Text += $@"{Percent} {Message}";
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            txtResults.Text += $@"Error: {ErrorMsg}";
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

            utils.Polygonize(string.Empty, string.Empty, 1, false, string.Empty, "ESRI Shapefile", string.Empty);

            utils.GDALInfo(string.Empty, string.Empty);
            utils.TranslateRaster(string.Empty, string.Empty, string.Empty);

            utils.OGRInfo(string.Empty, string.Empty);
            utils.OGR2OGR(string.Empty, string.Empty, string.Empty);
            utils.GDALAddOverviews(string.Empty, string.Empty, string.Empty);
            utils.GDALBuildVrt(string.Empty, string.Empty);
            utils.GDALRasterize(string.Empty, string.Empty, string.Empty);
            // Removed since v5.1.0: utils.GDALWarp(string.Empty, string.Empty, string.Empty);

            var ds = new GdalDataset();
            var subDatasetCount = ds.SubDatasetCount;
        }

        private void ClipGridWithPolygon()
        {
            var workingFolder = @"C:\dev\TestingScripts\ScriptData\ClipGridWithPolygon";
            var tempFolder = @"C:\dev\TestingScripts\Temp\ClipGridWithPolygon";

            var gridFilename = Path.Combine(workingFolder, "AHN2.asc");
            var sf = new Shapefile();
            sf.Open(Path.Combine(workingFolder, "Areas.shp"));
            var numShapes = sf.NumShapes;
            var clippedWrong = 0;
            var clippedGood = 0;

            // Needed for the new method:
            var utils = new Utils();
            var grd = new Grid();
            grd.Open(gridFilename);

            for (var i = 0; i < numShapes; i++)
            {
                var polygon = sf.Shape[i];
                var resultGrid = Path.Combine(tempFolder, "AHN2_clipped" + i + ".asc");

                // Using thr mwGeoProc version takes almost 4 hours with this data:
                // if (MapWinGeoProc.SpatialOperations.ClipGridWithPolygon(ref gridFilename, ref polygon, ref resultGrid))
                if (utils.ClipGridWithPolygon2(grd, polygon, resultGrid, false))
                    clippedGood++;
                else
                    clippedWrong++;
            }

            var msg = clippedWrong == 0
                ? "All tests successful!"
                : $"{clippedGood} clippings went OK, {clippedWrong} went wrong";
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
                    writer.WriteLine(
                        DateTime.Now + " Could not open shapefile. Reason " + sf.ErrorMsg[sf.LastErrorCode]);
                    return;
                }

                var numShapes = sf.NumShapes;

                var netCdfFiles = Directory.GetFiles(workingFolder, "*.nc");
                var utils = new UtilsClass();
                _settings.ResetGdalError();
                var amersfoort = new GeoProjectionClass();
                amersfoort.ImportFromEPSG(28992);

                // Check projection:
                if (!sf.GeoProjection.IsSame[amersfoort]) sf.GeoProjection = amersfoort.Clone();

                // Loop through all netCDF files in folder:
                foreach (var netCdfFile in netCdfFiles)
                {
                    var grd = new GridClass();
                    if (!grd.Open(netCdfFile))
                    {
                        writer.WriteLine(DateTime.Now + " Could not open {0}. Reason: {1}, GDAL Error: {2}", netCdfFile,
                            grd.ErrorMsg[grd.LastErrorCode], _settings.GdalLastErrorMsg);
                        continue;
                    }

                    currentProcess.Refresh();
                    writer.WriteLine(DateTime.Now + "Loaded grid " + netCdfFile + " Current memory " +
                                     FormatBytes(currentProcess.WorkingSet64));

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
                                grd.ErrorMsg[grd.LastErrorCode], _settings.GdalLastErrorMsg);
                            continue;
                        }

                        currentProcess.Refresh();
                        writer.WriteLine(DateTime.Now + " Working with band {0}  Current memory: {1} ", i,
                            FormatBytes(currentProcess.WorkingSet64));
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
                                writer.WriteLine(DateTime.Now + " Error getting statistics: " +
                                                 utils.ErrorMsg[utils.LastErrorCode]);

                            currentProcess.Refresh();
                            writer.WriteLine(DateTime.Now + " Mean: {0}, Min: {1}, Max: {2}, Current memory: {3}", mean,
                                min, max, FormatBytes(currentProcess.WorkingSet64));
                        }
                    }

                    // Close grid:
                    grd.Close();
                    currentProcess.Refresh();
                    writer.WriteLine(DateTime.Now + " Closed grid Current memory: " +
                                     FormatBytes(currentProcess.WorkingSet64));
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
            for (i = 0; i < Suffix.Length && bytes >= 1024; i++, bytes /= 1024) dblSByte = bytes / 1024.0;

            return $"{dblSByte:0.##} {Suffix[i]}";
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
            _settings.ResetGdalError();
            var utils = new UtilsClass();
            var options =
                "--config CONFIG_FILE \"D:\\dev\\GIS-Data\\Djordje\\osmconf.ini\" --config OSM_USE_CUSTOM_INDEXING NO -skipfailures -f \"ESRI Shapefile\"";
            var result = utils.OGR2OGR(@"D:\dev\GIS-Data\Djordje\map.osm", @"D:\dev\GIS-Data\Djordje\osm-result",
                options);
            if (!result)
            {
                // Both line should be the same:
                Debug.WriteLine(utils.ErrorMsgFromObject(utils));
                Debug.WriteLine(utils.ErrorMsg[utils.LastErrorCode]);
                Debug.WriteLine(_settings.GdalLastErrorMsg);
            }

            txtResults.Text = @"OGR2OGR result: " + result;
            if (!result) return;

            // Add files:
            var shapefileFilenames = Directory.GetFiles(@"D:\dev\GIS-Data\Djordje\osm-result\", "*.shp");
            foreach (var filename in shapefileFilenames)
                axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosVectorLayer, true);
        }

        private void btnGdalInfo_Click(object sender, EventArgs e)
        {
            _settings.ResetGdalError();
            var utils = new UtilsClass();

            var filename = @"D:\dev\GIS-Data\HDF5\RAD_NL25_RAC_03H_201612040000.h5";
            var result = utils.GDALInfo(filename, "");
            if (!string.IsNullOrEmpty(result))
            {
                txtResults.Text = result;
                var hndle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
                if (hndle >= 1) return;

                txtResults.Text += @" GDAL Error: " + _settings.GdalLastErrorMsg
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

            txtResults.Text = @" GDAL Error: " + _settings.GdalLastErrorMsg
                                               + @" GdalPluginPath: " + _settings.GdalPluginPath;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            _settings.OgrStringEncoding = tkOgrEncoding.oseUtf8;

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
            txtResults.Text += @"Added layer with handle " + hndl;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            const string filename = @"D:\dev\MapwinGIS\GitHub\unittests\MapWinGISTests\Testdata\HDF5\test.h5";
            var subset = $"HDF5:\"{filename}\"://image1/image_data";
            // Open subdataset as grid:
            var grd = new GridClass();
            if (grd.Open(subset)) axMap1.AddLayer(grd, true);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            // Create a polygon shapefile:
            var sfPolygon = new Shapefile();
            sfPolygon.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);
            var shp = new Shape();
            shp.ImportFromWKT(
                "POLYGON((330695.973322992 5914896.16305817, 330711.986129861 5914867.19586245, 330713.350435287 5914867.56644015, 330716.510827627 5914862.28973662, 330715.632568651 5914860.60107999, 330652.234582712 5914803.80510632, 330553.749382483 5914715.80328169, 330551.979355848 5914714.83347535, 330549.911988583 5914715.86502807, 330545.027807355 5914724.05916443, 330544.592985976 5914725.93531509, 330544.30963704 5914726.72754692, 330543.612620707 5914726.14904553, 330543.271515787 5914727.06633931, 330542.234090059 5914729.85597723, 330542.959654761 5914730.50411962, 330530.319252794 5914765.86064153, 330505.294840402 5914836.7930124, 330471.411812074 5914931.61558331, 330486.074748666 5914941.33795239, 330585.983154737 5915010.32749106, 330618.427962455 5915031.20447119, 330653.234601917 5914970.37328093, 330695.973322992 5914896.16305817))");
            sfPolygon.EditAddShape(shp);
            // Create some random points:
            var random = new Random();
            const int numPoints = 50;
            var extents = sfPolygon.Extents;
            var width = extents.xMax - extents.xMin;
            var height = extents.yMax - extents.yMin;

            var sfPoint = new Shapefile();
            sfPoint.CreateNewWithShapeID("", ShpfileType.SHP_POINT);
            for (var i = 0; i < numPoints; i++)
            {
                var x = extents.xMin + width * random.NextDouble();
                var y = extents.yMin + height * random.NextDouble();
                var shpPoint = new Shape();
                shpPoint.Create(ShpfileType.SHP_POINT);
                shpPoint.AddPoint(x, y);
                sfPoint.EditAddShape(shpPoint);
            }

            axMap1.AddLayer(sfPolygon, true);
            axMap1.AddLayer(sfPoint, true);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            const string filename = @"J:\Akkerweb.Shared\tmp\ndre-copied.tif";
            // axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
            var fm = new FileManager();
            var obj = fm.Open(filename);
            if (obj != null && fm.LastOpenIsSuccess)
            {
                var handle = axMap1.AddLayer(obj, true);
                if (handle == -1)
                    MessageBox.Show(@"Failed to add layer to the map: " + axMap1.get_ErrorMsg(axMap1.LastErrorCode));
            }
            else
            {
                MessageBox.Show(@"Failed to open datasource: " + fm.ErrorMsg[fm.LastErrorCode]);
            }
        }

        private void button8_Click(object sender, EventArgs e)
        {
            var utils = new Utils();
            var grd = new Grid();
            if (!grd.Open(@"J:\Akkerweb.Shared\tmp\ndre-copied.tif", GridDataType.FloatDataType))
                throw new Exception("Can't open grid file: " + grd.ErrorMsg[grd.LastErrorCode]);
            var sf = new Shapefile();
            if (!sf.Open(@"J:\Akkerweb.Shared\tmp\Fishnet.shp"))
                throw new Exception("Can't open shapefile file: " + sf.ErrorMsg[sf.LastErrorCode]);
            if (!utils.GridStatisticsToShapefile(grd, sf, false, true))
                throw new Exception("GridStatisticsToShapefile failed: " + utils.ErrorMsg[utils.LastErrorCode]);

            axMap1.AddLayer(grd, true);
            axMap1.AddLayer(sf, true);
        }

        private void axMap1_ShapeIdentified(object sender, _DMapEvents_ShapeIdentifiedEvent e)
        {
            axMap1.ZoomToShape(e.layerHandle, e.shapeIndex);
        }

        private void axMap1_FileDropped(object sender, _DMapEvents_FileDroppedEvent e)
        {
            txtResults.Text += $@"Opening {e.filename} after dropping.{Environment.NewLine}";
            axMap1.AddLayerFromFilename(e.filename, tkFileOpenStrategy.fosAutoDetect, true);
            if (axMap1.GeoProjection.TryAutoDetectEpsg(out var epsgCode))
            {
                txtResults.Text += $@"Map projection: EPSG:{epsgCode}{Environment.NewLine}";
            }
            else
            {
                txtResults.Text += $@"Map projection: {axMap1.GeoProjection.ExportToWktEx()}{Environment.NewLine}";
            }
        }

        private void button9_Click(object sender, EventArgs e)
        {
            //    axMap1.Projection = tkMapProjection.PROJECTION_WGS84;
            //    axMap1.KnownExtents = tkKnownExtents.keLatvia;

            //    var dhandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            //    axMap1.DrawCircle(24.0, 57.0, 0.1, 0, false);
            //    axMap1.DrawLabel("Label", 24.0, 57.0, 0.0);
            //    axMap1.DrawLabel("Воздух", 24.1, 57.1, 0.0);

            axMap1.Projection = tkMapProjection.PROJECTION_WGS84;
            axMap1.Latitude = 0;
            axMap1.Longitude = 0;
            axMap1.CurrentZoom = 10;

            // Add drawing layer and circle with its own label
            axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var utils = new Utils();
            axMap1.DrawCircle(0, 0, 0.1, utils.ColorByName(tkMapColor.Yellow), true);
            axMap1.DrawLabel("ABC", 0, 0, 0);
        }

        private void button10_Click(object sender, EventArgs e)
        {
            axMap1.AddLayerFromFilename(
                @"D:\dev\MapwinGIS\GitHub\unittests\MapWinGISTests\Testdata\Issues\MWGIS-98\3dPoint.shp",
                tkFileOpenStrategy.fosAutoDetect, true);
        }

        private void btnPrefetchTiles_Click(object sender, EventArgs e)
        {
            if (axMap1.Projection == tkMapProjection.PROJECTION_NONE)
            {
                axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
                axMap1.Tiles.Provider = tkTileProvider.OpenStreetMap;
                axMap1.KnownExtents = tkKnownExtents.keNetherlands;
            }

            axMap1.Redraw();
            axMap1.ZoomBehavior = tkZoomBehavior.zbUseTileLevels;
            var outputFolder = $@"D:\tmp\axmap.tiles\{axMap1.Tiles.Provider.ToString()}";
            if (!Directory.Exists(outputFolder)) Directory.CreateDirectory(outputFolder);

            var numTilesToCache = axMap1.Tiles.PrefetchToFolder(axMap1.Extents, axMap1.Tiles.CurrentZoom,
                Convert.ToInt32(axMap1.Tiles.Provider), outputFolder, ".png", null);
            txtResults.Text += $@"{Environment.NewLine}numTilesToCache: " + numTilesToCache;
        }

        private void txtResults_TextChanged(object sender, EventArgs e)
        {
            txtResults.SelectionStart = txtResults.Text.Length;
            txtResults.ScrollToCaret();
        }

        private void button12_Click(object sender, EventArgs e)
        {
            axMap1.ZoomIn(0.3);
        }

        private void BtnOpenGeoPackage_Click(object sender, EventArgs e)
        {
            const string fileName = @"D:\dev\GIS-data\Formats\GeoPackage\codepo_gb.gpkg";
            var retVal = axMap1.FileManager.CanOpenAs[fileName, tkFileOpenStrategy.fosAutoDetect];
            retVal = axMap1.FileManager.CanOpenAs[fileName, tkFileOpenStrategy.fosVectorLayer];
            retVal = axMap1.FileManager.IsSupported[fileName];
            retVal = axMap1.FileManager.IsVectorLayer[fileName];
            var ogrLayer = axMap1.FileManager.OpenVectorLayer(fileName);
            var featureCounts = ogrLayer.FeatureCount[false];
            var buffer = ogrLayer.GetBuffer();
            axMap1.AddLayer(buffer, true);
            //axMap1.AddLayer(ogrLayer, true);
            //var ogrDatasource = axMap1.FileManager.OpenVectorDatasource(fileName);
            //axMap1.AddLayer(ogrDatasource, true);
            //axMap1.AddLayerFromFilename(fileName, tkFileOpenStrategy.fosVectorLayer, true);
            //txtResults.Text += $@"{Environment.NewLine}Add GeoPackage: " + axMap1.FileManager.ErrorMsg[axMap1.FileManager.LastErrorCode];
        }

        private void BtnClearMap_Click(object sender, EventArgs e)
        {
            axMap1.Clear();
            txtResults.Text = "";
            txtResults.Text += $@"Map is cleared. Projection is empty: {axMap1.GeoProjection.IsEmpty}";
        }
    }
}