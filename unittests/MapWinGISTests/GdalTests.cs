using System;
using System.Diagnostics;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class GdalTests
    {
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

                TestLayers(ogrDatasource);
            }
            catch (AssertFailedException)
            {
                throw;
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
                Assert.Fail();
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
                var result = ogrDatasource.Open(@"sqlite\onepoint.sqlite");
                Assert.IsTrue(result, "Cannot open SQLite file: " + ogrDatasource.GdalLastErrorMsg);

                var capability = ogrDatasource.TestCapability(tkOgrDSCapability.odcCreateLayer);
                Debug.WriteLine("odcCreateLayer: " + capability);
                Assert.IsTrue(capability, "Cannot create layer");

                var originalLayerCount = ogrDatasource.LayerCount;

                var projection = new GeoProjectionClass();
                Assert.IsTrue(projection.SetWgs84(), "Cannot set projection");

                var layerCreated = ogrDatasource.CreateLayer("Test", ShpfileType.SHP_POINT, projection, "");
                Assert.IsTrue(layerCreated, "Cannot create layer");

                Assert.AreEqual(originalLayerCount + 1, ogrDatasource.LayerCount, "New layer isn't created");

                TestLayers(ogrDatasource);
            }
            catch (AssertFailedException)
            {
                throw;
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
                Assert.Fail();
            }
            finally
            {
                if (ogrDatasource.LayerCount > 1)
                    ogrDatasource.DeleteLayer(ogrDatasource.LayerCount);
                ogrDatasource.Close();
            }
        }

        private void TestLayers(OgrDatasourceClass ogrDatasource)
        {
            // Get layers:
            var lastLayername = string.Empty;
            Debug.WriteLine("Number of layers: " + ogrDatasource.LayerCount);
            for (var i = 0; i < ogrDatasource.LayerCount; i++)
            {
                var layer = ogrDatasource.GetLayer(i, true);
                lastLayername = layer.Name;
                Debug.WriteLine("Layer name: " + layer.Name);
                Debug.WriteLine("Driver Name: " + layer.DriverName);
                Assert.AreEqual("SQLite", layer.DriverName, "Wrong driver name");
                var capability = layer.TestCapability(tkOgrLayerCapability.olcRandomRead);
                Assert.IsTrue(capability, "Cannot random read");
                Debug.WriteLine("olcRandomRead: " + capability);
                capability = layer.TestCapability(tkOgrLayerCapability.olcRandomWrite);
                Debug.WriteLine("olcRandomWrite: " + capability);
                Assert.IsTrue(capability, "Cannot random write");
                capability = layer.TestCapability(tkOgrLayerCapability.olcSequentialWrite);
                Debug.WriteLine("olcSequentialWrite: " + capability);
                Assert.IsTrue(capability, "Cannot sequential write");
            }

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
