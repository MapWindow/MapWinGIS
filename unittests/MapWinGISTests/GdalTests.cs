using System;
using System.Diagnostics;
using System.Runtime.Remoting.Metadata.W3cXsd2001;
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
        public void OpenSQLiteTest()
        {
            var ogrDatasource = new OgrDatasourceClass();
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

                TestSQLiteLayers(ogrDatasource);
            }
            finally
            {
                ogrDatasource.Close();
            }
        }

        [TestMethod]
        public void CreateLayerSQLiteTest()
        {
            var ogrDatasource = new OgrDatasourceClass();
            try
            {
                var result = ogrDatasource.Open2(@"sqlite\onepoint.sqlite", true);
                Assert.IsTrue(result, "Cannot open SQLite file: " + ogrDatasource.GdalLastErrorMsg);
                var settings = new GlobalSettings { OgrLayerForceUpdateMode = true };

                var capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateLayer);
                Debug.WriteLine("odcCreateLayer: " + capability);
                Assert.IsTrue(capability, "Cannot create layer");

                var originalLayerCount = ogrDatasource.LayerCount;

                var projection = new GeoProjectionClass();
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

                TestSQLiteLayers(ogrDatasource);
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
            var utils = new UtilsClass();
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
            var grd = new GridClass();
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
            var utils = new UtilsClass();
            var result = utils.GDALInfo("", "--formats");
            Assert.IsNotNull(result, "GdalInfo returned null");
            Debug.WriteLine(result);
            Assert.IsTrue(result.Contains("Supported Formats:"), "result is invalid");
            Assert.IsTrue(result.Contains("GTiff (rw+): GeoTIFF"), "result is invalid");
        }

        private static void TestSQLiteLayers(IOgrDatasource ogrDatasource)
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
                Assert.AreEqual("SQLite", layer.DriverName, "Wrong driver name");
                var capability = layer.TestCapability(tkOgrLayerCapability.olcRandomRead);
                Debug.WriteLine("olcRandomRead: " + capability);
                Assert.IsTrue(capability, "Cannot random read");
                capability = layer.TestCapability(tkOgrLayerCapability.olcRandomWrite);
                Debug.WriteLine("olcRandomWrite: " + capability);
                Assert.IsTrue(capability, "Cannot random write");
                capability = layer.TestCapability(tkOgrLayerCapability.olcSequentialWrite);
                Debug.WriteLine("olcSequentialWrite: " + capability);
                Assert.IsTrue(capability, "Cannot sequential write");
            }

            Debug.WriteLine("Last layer name: " + lastLayername);
            if (!string.IsNullOrEmpty(lastLayername))
            {
                var layer = ogrDatasource.GetLayerByName(lastLayername, true);
                var layerCapability = layer.TestCapability(tkOgrLayerCapability.olcRandomRead);
                Debug.WriteLine("olcRandomRead: " + layerCapability);
                Assert.IsTrue(layerCapability, "Cannot random read");
                layerCapability = layer.TestCapability(tkOgrLayerCapability.olcRandomWrite);
                Debug.WriteLine("olcRandomWrite: " + layerCapability);
                Assert.IsTrue(layerCapability, "Cannot random write");
                layerCapability = layer.TestCapability(tkOgrLayerCapability.olcSequentialWrite);
                Debug.WriteLine("olcSequentialWrite: " + layerCapability);
                Assert.IsTrue(layerCapability, "Cannot sequential write");
            }
        }
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
