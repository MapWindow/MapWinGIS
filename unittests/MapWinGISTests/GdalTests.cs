using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class GdalTests
    {

        [TestInitialize]
        public void Start()
        {
            Debug.WriteLine("Start Gdal tests");
            Debug.WriteLine(DateTime.Now);
        }

        [TestMethod]
        public void OpenGdb2Test()
        {
            try
            {
                var ogr = new OgrDatasource();
                var source = @"D:\dev\MapWindow\MapWinGIS\_issues\Forum\1193 add-layer-from-geodatabase-not-working\data.gdb";
                if (!ogr.Open(source)) // no error here
                {
                    Debug.WriteLine("Failed to open data source. Err: " + ogr.GdalLastErrorMsg);
                }
                else
                {
                    int count = ogr.LayerCount; // This displays count properly
                    Debug.WriteLine(count);

                    var layer = new OgrLayer();
                    layer = ogr.GetLayerByName("Road");
                    
                    //int a = axMap1.AddLayer(layer, true) ; // here a gets number 1
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.ToString());
            }
        }

        [TestMethod]
        public void OpenGdbTest()
        {
            var ogrDatasource = new OgrDatasource();
            try
            {
                var result = ogrDatasource.Open(@"D:\dev\MapWindow\MapWinGIS\_issues\Forum\1193 add-layer-from-geodatabase-not-working\data.gdb");
                Assert.IsTrue(result, "Cannot open gdb file: " + ogrDatasource.GdalLastErrorMsg);
                var settings = new GlobalSettings { OgrLayerForceUpdateMode = true };

                var capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateLayer);
                Debug.WriteLine("odcCreateLayer: " + capability);
                //Assert.IsTrue(capability, "Cannot create layer");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcDeleteLayer);
                Debug.WriteLine("odcDeleteLayer: " + capability);
                //Assert.IsTrue(capability, "Cannot delete layer");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateDataSource);
                Debug.WriteLine("odcCreateDataSource: " + capability);
                //Assert.IsTrue(capability), "Cannot create datasource");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcDeleteDataSource);
                Debug.WriteLine("odcDeleteDataSource: " + capability);
                //Assert.IsTrue(capability, "Cannot delete datasource");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateGeomFieldAfterCreateLayer);
                Debug.WriteLine("odcCreateGeomFieldAfterCreateLayer: " + capability);
                //Assert.IsTrue(capability, "Cannot create GeomField After CreateLayer");

                TestOgrLayers(ogrDatasource, "OpenFileGDB", true);
            }
            finally
            {
                ogrDatasource.Close();
            }
        }


        [TestMethod]
        public void OpenSQLiteTest()
        {
            var ogrDatasource = new OgrDatasource();
            try
            {
                var result = ogrDatasource.Open2(@"sqlite\onepoint.sqlite", true);
                Assert.IsTrue(result, "Cannot open SQLite file: " + ogrDatasource.GdalLastErrorMsg);
                var settings = new GlobalSettings { OgrLayerForceUpdateMode = true };

                var capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateLayer);
                Debug.WriteLine("odcCreateLayer: " + capability);
                Assert.IsTrue(capability, "Cannot create layer");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcDeleteLayer);
                Debug.WriteLine("odcDeleteLayer: " + capability);
                Assert.IsTrue(capability, "Cannot delete layer");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateDataSource);
                Debug.WriteLine("odcCreateDataSource: " + capability);
                //Assert.IsTrue(capability), "Cannot create datasource");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcDeleteDataSource);
                Debug.WriteLine("odcDeleteDataSource: " + capability);
                //Assert.IsTrue(capability, "Cannot delete datasource");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateGeomFieldAfterCreateLayer);
                Debug.WriteLine("odcCreateGeomFieldAfterCreateLayer: " + capability);
                Assert.IsTrue(capability, "Cannot create GeomField After CreateLayer");

                TestOgrLayers(ogrDatasource, "SQLite");
            }
            finally
            {
                ogrDatasource.Close();
            }
        }

        [TestMethod]
        public void CreateLayerSQLiteTest()
        {
            var ogrDatasource = new OgrDatasource();
            try
            {
                var result = ogrDatasource.Open2(@"sqlite\onepoint.sqlite", true);
                Assert.IsTrue(result, "Cannot open SQLite file: " + ogrDatasource.GdalLastErrorMsg);
                var settings = new GlobalSettings { OgrLayerForceUpdateMode = true };

                var capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateLayer);
                Debug.WriteLine("odcCreateLayer: " + capability);
                Assert.IsTrue(capability, "Cannot create layer");

                var originalLayerCount = ogrDatasource.LayerCount;

                var projection = new GeoProjection();
                Assert.IsTrue(projection.SetWgs84(), "Cannot set projection");

                var layerCreated = ogrDatasource.CreateLayer("Test", ShpfileType.SHP_POINT, projection, "OVERWRITE=YES");
                Assert.IsTrue(layerCreated, "Cannot create layer");
                Debug.WriteLine(ogrDatasource.GdalLastErrorMsg);

                Assert.AreEqual(originalLayerCount + 1, ogrDatasource.LayerCount, "New layer isn't created");
                Debug.WriteLine("GetLayerName: " + ogrDatasource.GetLayerName(ogrDatasource.LayerCount - 1));

                var firstLayer = ogrDatasource.GetLayer(0);
                Assert.IsNotNull(firstLayer, $"Could not get first layer: {ogrDatasource.GdalLastErrorMsg}");

                // Get layer:
                var newLayer = ogrDatasource.GetLayer(ogrDatasource.LayerCount - 1, true);
                // var newLayer = ogrDatasource.GetLayerByName("test", true);
                Assert.IsNotNull(newLayer, $"Could not get new layer: {ogrDatasource.GdalLastErrorMsg}");
                // Add field:
                var numFeatures = newLayer.FeatureCount[true];
                Debug.WriteLine("numFeatures: " + numFeatures);

                TestOgrLayers(ogrDatasource, "SQLite");
            }
            finally
            {
                if (ogrDatasource.LayerCount > 1)
                    ogrDatasource.DeleteLayer(ogrDatasource.LayerCount);
                ogrDatasource.Close();
            }
        }

        /// <summary>
        /// Opens the HDF5 file
        /// </summary>
        /// <remarks>https://mapwindow.atlassian.net/browse/MWGIS-56</remarks>
        [TestMethod]
        public void GdalInfoHdf5()
        {
            const string filename = @"HDF5\test.h5";
            // Check if GDAL can open it:
            var utils = new Utils();
            var settings = new GlobalSettings();
            var info = utils.GDALInfo(filename, string.Empty);
            Assert.IsNotNull(info, "Could not read gdalinfo: " + settings.GdalLastErrorMsg);
            Debug.WriteLine(info);
            Assert.IsTrue(info.Contains("Driver: HDF5/Hierarchical Data Format Release 5"), "File is not recognized");

            var ds = new GdalDataset();
            if (ds.Open(filename, true))
            {
                Debug.Print("Sub datasets");
                for (int i = 0; i < ds.SubDatasetCount; i++)
                {
                    string name = ds.GetSubDatasetName(i);
                    Debug.Print(name);
                }
            }

            // Open HDF file using subset:
            var subset = $"HDF5:\"{filename}\"://image1/image_data";
            var infoSubset = utils.GDALInfo(subset, string.Empty);
            Assert.IsNotNull(infoSubset, "Could not read gdalinfo: " + settings.GdalLastErrorMsg);
            Debug.WriteLine(infoSubset);
            Assert.IsTrue(infoSubset.Contains("Driver: HDF5Image/HDF5 Dataset"), "File is not recognized");

            // Open subdataset as grid:
            var grd = new Grid();
            var result = grd.Open(subset);
            Assert.IsTrue(result, "Could not open HDF5 subset as grid");

            Debug.WriteLine(grd.NumBands);
            Debug.WriteLine(grd.Extents.ToDebugString());
            Debug.WriteLine(grd.Minimum.ToString());
            Debug.WriteLine(grd.Maximum.ToString());

        }

        /// <summary>
        /// Get the supported formats
        /// </summary>
        /// <remarks>https://mapwindow.atlassian.net/browse/MWGIS-50</remarks>
        [TestMethod]
        public void GdalInfoFormats()
        {
            var utils = new Utils();
            var result = utils.GDALInfo("", "--formats");
            Assert.IsNotNull(result, "GdalInfo returned null");
            Debug.WriteLine(result);
            Assert.IsTrue(result.Contains("Supported Formats:"), "result is invalid");
            Assert.IsTrue(result.Contains("GTiff (rw+): GeoTIFF"), "result is invalid");
        }

        private static void TestOgrLayers(IOgrDatasource ogrDatasource, string driverName, bool inReadOnlyMode = false)
        {
            // Get layers:
            var lastLayername = string.Empty;
            Debug.WriteLine("Number of layers: " + ogrDatasource.LayerCount);
            for (var i = 0; i < ogrDatasource.LayerCount; i++)
            {
                var layer = ogrDatasource.GetLayer(i, true);
                Assert.IsNotNull(layer, "Layer is null");
                lastLayername = layer.Name;
                Debug.WriteLine("Layer name: " + layer.Name);
                Debug.WriteLine("Driver Name: " + layer.DriverName);
                Assert.AreEqual(driverName, layer.DriverName, "Wrong driver name");
                Debug.WriteLine("ActiveShapeType: " + layer.ActiveShapeType);
                var capability = layer.TestCapability(tkOgrLayerCapability.olcRandomRead);
                Debug.WriteLine("olcRandomRead: " + capability);
                if (!inReadOnlyMode) Assert.IsTrue(capability, "Cannot random read");
                capability = layer.TestCapability(tkOgrLayerCapability.olcRandomWrite);
                Debug.WriteLine("olcRandomWrite: " + capability);
                if (!inReadOnlyMode) Assert.IsTrue(capability, "Cannot random write");
                capability = layer.TestCapability(tkOgrLayerCapability.olcSequentialWrite);
                Debug.WriteLine("olcSequentialWrite: " + capability);
                if (!inReadOnlyMode) Assert.IsTrue(capability, "Cannot sequential write");
            }

            Debug.WriteLine("Last layer name: " + lastLayername);
            if (!string.IsNullOrEmpty(lastLayername))
            {
                var layer = ogrDatasource.GetLayerByName(lastLayername, true);
                var layerCapability = layer.TestCapability(tkOgrLayerCapability.olcRandomRead);
                Debug.WriteLine("olcRandomRead: " + layerCapability);
                if (!inReadOnlyMode) Assert.IsTrue(layerCapability, "Cannot random read");
                layerCapability = layer.TestCapability(tkOgrLayerCapability.olcRandomWrite);
                Debug.WriteLine("olcRandomWrite: " + layerCapability);
                if (!inReadOnlyMode) Assert.IsTrue(layerCapability, "Cannot random write");
                layerCapability = layer.TestCapability(tkOgrLayerCapability.olcSequentialWrite);
                Debug.WriteLine("olcSequentialWrite: " + layerCapability);
                if (!inReadOnlyMode) Assert.IsTrue(layerCapability, "Cannot sequential write");
            }
        }

        //[Obsolete("utils.GDALWarp is deprecated, please use gdalUtils.GDALWarp instead.")]
        //public void GdalWarp()
        //{
        //    var utils = new Utils();
        //    var output = Path.GetTempPath() + "GdalWarp.vrt";
        //    if (File.Exists(output)) File.Delete(output);

        //    var retVal = utils.GDALWarp(@"GeoTiff/5band.tif", output, "-of vrt");
        //    Assert.IsTrue(retVal, "GdalWarp failed: " + utils.ErrorMsg[utils.LastErrorCode]);
        //    Assert.IsTrue(File.Exists(output), "Output file doesn't exists");
        //    Debug.WriteLine(output);
        //}

        //[Obsolete("utils.GDALWarp is deprecated, please use gdalUtils.GDALWarp instead.")]
        //public void GdalWarpCutline()
        //{
        //    var utils = new Utils();
        //    var output = Path.GetTempPath() + "GdalWarpCutline.vrt";
        //    if (File.Exists(output)) File.Delete(output);
        //    var border = @"J:\NBS3\src\NBS3.Rest\data\1a4be5f3deae4445893cb289c8124a02\border.shp";

        //    var options = "-of vrt -overwrite -crop_to_cutline -cutline " + border;
        //    var retVal = utils.GDALWarp(@"J:\NBS3\src\NBS3.Rest\data\1a4be5f3deae4445893cb289c8124a02\Smegen.tif", output, options);
        //    Assert.IsTrue(retVal, "GdalWarp failed: " + utils.ErrorMsg[utils.LastErrorCode]);
        //    Assert.IsTrue(File.Exists(output), "Output file doesn't exists");
        //    Debug.WriteLine(output);
        //}

        public void OpenPostGISDifferentPortTest()
        {
            // Moved to PostGisTests.cs
        }

        public void ReadAttributesFromPostGISLayer()
        {
            // Moved to PostGisTests.cs
        }
    }
}
