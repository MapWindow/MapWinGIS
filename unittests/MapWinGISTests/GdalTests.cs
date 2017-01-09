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
            Debug.WriteLine("Start unit tests");
            Debug.WriteLine(DateTime.Now);
        }
        
        [TestMethod]
        public void OpenSQLiteTest()
        {
            var ogrDatasource = new OgrDatasourceClass();
            try
            {
                var result = ogrDatasource.Open(@"sqlite\onepoint.sqlite");
                Assert.IsTrue(result, "Cannot open SQLite file: " + ogrDatasource.GdalLastErrorMsg);
                var settings = new GlobalSettings { OgrLayerForceUpdateMode = true };

                var capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateLayer);
                Debug.WriteLine("odcCreateLayer: " + capability);
                // Assert.IsTrue(capability, "Cannot create layer");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcDeleteLayer);
                Debug.WriteLine("odcDeleteLayer: " + capability);
                // Assert.IsTrue(capability, "Cannot delete layer");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateDataSource);
                Debug.WriteLine("odcCreateDataSource: " + capability);
                //Assert.IsTrue(capability), "Cannot create datasource");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcDeleteDataSource);
                Debug.WriteLine("odcDeleteDataSource: " + capability);
                //Assert.IsTrue(capability, "Cannot delete datasource");
                capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateGeomFieldAfterCreateLayer);
                Debug.WriteLine("odcCreateGeomFieldAfterCreateLayer: " + capability);
                // Assert.IsTrue(capability, "Cannot create GeomField After CreateLayer");

                TestSQLiteLayers(ogrDatasource);
            }
            finally
            {
                ogrDatasource.Close();
            }
        }

        /// <summary>
        /// Opens the post gis different port test.
        /// </summary>
        /// <remarks>Because it needs an existing host, database and table it is not a 'proper' test by default</remarks>
        // [TestMethod]
        public void OpenPostGISDifferentPortTest()
        {
            var ogrDatasource = new OgrDatasourceClass();
            try
            {
                var result = ogrDatasource.Open("PG:host=127.0.0.1 port=55432 dbname=aw_croppingscheme user=aw_croppingschem password=test123");
                Assert.IsTrue(result, "Cannot open PostGIS Connectie: " + ogrDatasource.GdalLastErrorMsg);
                var settings = new GlobalSettings();

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

                Assert.IsTrue(ogrDatasource.LayerCount > 1, "No layers found");

                for (var i = 0; i < ogrDatasource.LayerCount; i++)
                {
                    var layer = ogrDatasource.GetLayer(i);
                    Debug.WriteLine(layer.Name);
                }
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

            // Open HDF file using subset:
            var subset = $"HDF5:\"{filename}\"://image1/image_data";
            info = utils.GDALInfo(subset, string.Empty);
            Assert.IsNotNull(info, "Could not read gdalinfo: " + settings.GdalLastErrorMsg);
            Debug.WriteLine(info);
            Assert.IsTrue(info.Contains("Driver: HDF5Image/HDF5 Dataset"), "File is not recognized");

            // TODO: Open subdataset as grid:
        }

        /// <summary>
        /// Reads the attributes from a postGIS layer.
        /// </summary>
        /// <remarks>See https://mapwindow.atlassian.net/browse/CORE-177 for the create table script.</remarks>
        [TestMethod]
        public void ReadAttributesFromPostGISLayer()
        {
            var ogrDatasource = new OgrDatasourceClass();
            try
            {
                var result = ogrDatasource.Open("PG:host=127.0.0.1 port=5432 dbname=mw_test user=mapwindow password=test123");
                Assert.IsTrue(result, "Cannot open PostGIS Connectie: " + ogrDatasource.GdalLastErrorMsg);
                // var settings = new GlobalSettings();

                Assert.IsTrue(ogrDatasource.LayerCount > 1, "No layers found");

                OgrLayer attributeLayer = null;
                for (var i = 0; i < ogrDatasource.LayerCount; i++)
                {
                    var layer = ogrDatasource.GetLayer(i);
                    Debug.WriteLine(layer.Name);
                    if (layer.Name == "attributes") attributeLayer = layer;
                }

                Assert.IsNotNull(attributeLayer, "Couldn't find the attribute layer");
                Debug.WriteLine("Working with attributes layer");

                var sf = attributeLayer.GetBuffer();
                Assert.IsNotNull(sf, "Could not get buffer");

                var numShapes = sf.NumShapes;
                Assert.AreEqual(3, numShapes);

                var numFields = sf.NumFields;
                Assert.IsTrue(numFields>0, "No fields found");

                var fidColumn = attributeLayer.FIDColumnName;
                Debug.WriteLine("fidColumn: " + fidColumn);

                // First shape has all attributes filled:
                Debug.WriteLine("Testing first shape");
                for (var fieldIndex = 0; fieldIndex < numFields; fieldIndex++)
                {
                    var value = sf.CellValue[fieldIndex, 0];
                    Debug.WriteLine($"{sf.Field[fieldIndex].Name}: {value}");
                    Assert.IsNotNull(value, $"{sf.Field[fieldIndex].Name} should not be null");
                }

                // Second shape has only the geometry filled and the rest default values (which should be NULL):
                Debug.WriteLine("Testing second shape");
                var numNonNulls = 0;
                for (var fieldIndex = 0; fieldIndex < numFields; fieldIndex++)
                {
                    var value = sf.CellValue[fieldIndex, 1];
                    Debug.WriteLine($"{sf.Field[fieldIndex].Name}: {value}");
                    // Skip FID because it always has a value:
                    if (sf.Field[fieldIndex].Name == fidColumn) continue;
                    if (value != null) numNonNulls++;
                }
                if (numNonNulls > 0) Debug.WriteLine($"Error! Second shape has {numNonNulls} non NULL fields!");

                // Third shape has all fields explicitly NULL:
                Debug.WriteLine("Testing third shape");
                numNonNulls = 0;
                for (var fieldIndex = 0; fieldIndex < numFields; fieldIndex++)
                {
                    var value = sf.CellValue[fieldIndex, 2];
                    Debug.WriteLine($"{sf.Field[fieldIndex].Name}: {value}");
                    // Skip FID because it always has a value:
                    if (sf.Field[fieldIndex].Name == fidColumn) continue;
                    if (value != null) numNonNulls++;
                }
                Assert.AreEqual(0, numNonNulls, $"Third shape has {numNonNulls} non NULL fields!");
            }
            finally
            {
                ogrDatasource.Close();
            }
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
                layerCapability = layer.TestCapability(tkOgrLayerCapability.olcRandomWrite);
                Debug.WriteLine("olcRandomWrite: " + layerCapability);
                layerCapability = layer.TestCapability(tkOgrLayerCapability.olcSequentialWrite);
                Debug.WriteLine("olcSequentialWrite: " + layerCapability);
            }
        }
    }
}
