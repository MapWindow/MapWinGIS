using System;
using System.Diagnostics;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class PostGisTest
    {

        [TestInitialize]
        public void Start()
        {
            Debug.WriteLine("Start PostGIS tests");
            Debug.WriteLine(DateTime.Now);
        }

        /// <summary>
        /// Opens the post gis different port test.
        /// </summary>
        /// <remarks>Because it needs an existing host, database and table it is not a 'proper' test by default</remarks>
        // [TestMethod]
        public void OpenPostGISDifferentPortTest()
        {
            var ogrDatasource = new OgrDatasource();
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

        /// <summary>
        /// Reads the attributes from a postGIS layer.
        /// </summary>
        /// <remarks>See https://mapwindow.atlassian.net/browse/CORE-177 for the create table script.</remarks>
        [TestMethod]
        public void ReadAttributesFromPostGISLayer()
        {
            var ogrDatasource = new OgrDatasource();
            try
            {
                var result = ogrDatasource.Open("PG:host=127.0.0.1 port=5432 dbname=mw_test user=mapwindow password=test123");
                Assert.IsTrue(result, "Cannot open PostGIS Connectie: " + ogrDatasource.GdalLastErrorMsg);
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
                Assert.IsTrue(numFields > 0, "No fields found");

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

        /// <summary>
        /// Reads the attributes from a postGIS layer.
        /// </summary>
        /// <remarks>See https://mapwindow.atlassian.net/browse/CORE-177 for the create table script.</remarks>
        [TestMethod]
        public void ReadSchemaNamesFromPostGIS()
        {
            var ogrDatasource = new OgrDatasource();
            try
            {
                var result = ogrDatasource.Open("PG:host=127.0.0.1 port=5432 dbname=mw_test user=mapwindow password=test123");
                Assert.IsTrue(result, "Cannot open PostGIS Connectie: " + ogrDatasource.GdalLastErrorMsg);
                Assert.IsTrue(ogrDatasource.LayerCount > 1, "No layers found");

                var stopwatch = new Stopwatch();

                stopwatch.Start();
                Debug.WriteLine("Using GetLayer:");
                for (var i = 0; i < ogrDatasource.LayerCount; i++)
                {
                    var layer = ogrDatasource.GetLayer(i);
                    Debug.WriteLine(layer.Name);
                }
                stopwatch.Stop();
                Debug.WriteLine("GetLayer took " + stopwatch.Elapsed);

                // TODO Get layername with schema name
            }
            finally
            {
                ogrDatasource.Close();
            }
        }
    }
}
