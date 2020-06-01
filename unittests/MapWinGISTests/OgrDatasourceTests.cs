using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class OgrDatasourceTests
    {
        private const string CONNECTION_STRING = "PG:host=localhost dbname=mw_test user=mapwindow password=test123";

        [TestMethod]
        public void ImportShapefileTest()
        {
            var ds = new OgrDatasource();
            try
            {
                if (!ds.Open(CONNECTION_STRING))
                {
                    Assert.Fail("Failed to establish connection: " + ds.GdalLastErrorMsg);
                }

                // Point shapefile:
                ImportShapefile(ds, @"D:\dev\GIS-Data\MapWindow-Projects\UnitedStates\Shapefiles\cities.shp", "cities_points");

                // Polygon shapefile:
                ImportShapefile(ds, @"D:\dev\GIS-Data\MapWindow-Projects\UnitedStates\Shapefiles\states.shp", "states_polygon");

                // Linestring shapefile:
                //ImportShapefile(ds, @"D:\dev\GIS-Data\MapWindow-Projects\UnitedStates\Shapefiles\roads.shp", "roads_linestring");
            }
            finally
            {
                ds.Close();
            }
        }


        [TestMethod]
        public void MWGIS61Test()
        {
            var ds = new OgrDatasource();
            try
            {
                if (!ds.Open(CONNECTION_STRING))
                {
                    Assert.Fail("Failed to establish connection: " + ds.GdalLastErrorMsg);
                }

                // Provided by Olivier:
                ImportShapefile(ds, @"D:\dev\GIS-Data\Issues\MWGIS-61\t_cheminement.shp", "Olivier_lines");
            }
            finally
            {
                ds.Close();
            }
        }

        private static void ImportShapefile(IOgrDatasource ds, string filename, string name)
        {
            // Check file:
            if (!File.Exists(filename)) Assert.Fail(filename + " does not exists.");
            // Open shapefile:
            var sf = new Shapefile();
            if (!sf.Open(filename))
                Assert.Fail("Failed to open shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);
            // Import:
            if (!ds.ImportShapefile(sf, name, "OVERWRITE=YES", tkShapeValidationMode.NoValidation))
                Assert.Fail("Failed to import shapefile");
            Debug.Print($"ds.Error: {ds.ErrorMsg[ds.LastErrorCode]} Gdal error: {ds.GdalLastErrorMsg}");

            // Check:
            Debug.Print("Layer was imported: " + name);
            var layer = ds.GetLayerByName(name);
            Assert.IsNotNull(layer, "layer is null");
            Debug.Print("Imported features count: " + layer.FeatureCount);
            Assert.AreEqual(sf.NumShapes, layer.FeatureCount,
                $"The amount of imported shapes is incorrect. GDAL Error: {layer.GdalLastErrorMsg} Error: {layer.ErrorMsg[layer.LastErrorCode]}");

            // Export again, using GetBuffer:
            var sfFromBuffer = layer.GetBuffer();
            Debug.Print("Number shapes from GetBuffer: " + sfFromBuffer.NumShapes);
            Assert.AreEqual(sf.NumShapes, sfFromBuffer.NumShapes, "The amount of exported shapes is incorrect.");

            // Save shapefile:
            var tmpFilename = Path.ChangeExtension(Path.Combine(Path.GetTempPath(), Path.GetTempFileName()), ".shp");
            DeleteShapefile(tmpFilename);
            if (!sfFromBuffer.SaveAs(tmpFilename))
                Assert.Fail("Failed to save shapefile: " + sfFromBuffer.ErrorMsg[sfFromBuffer.LastErrorCode]);

            if (!ds.ImportShapefile(sfFromBuffer, "sfFromBuffer", "OVERWRITE=YES",
                    tkShapeValidationMode.NoValidation))
                Assert.Fail("Failed to import buffered shapefile");

            layer.Close();
        }

        [TestMethod]
        public void GetBuffer()
        {
            // MWGIS-67
            var ds = new OgrDatasource();
            try
            {
                if (!ds.Open(CONNECTION_STRING))
                {
                    Assert.Fail("Failed to establish connection: " + ds.GdalLastErrorMsg);
                }

                // Get layer using buffer:
                var layer = ds.GetLayerByName("states_polygon");
                Assert.IsNotNull(layer, "layer is null");
                Console.WriteLine("Layer type is " + layer.ShapeType);

                var sfFromBuffer = layer.GetBuffer();
                Assert.IsNotNull(sfFromBuffer, "sfFromBuffer is null");
                Debug.WriteLine("NumShapes: " + sfFromBuffer.NumShapes);

                var tmpFilename = Path.ChangeExtension(Path.Combine(Path.GetTempPath(), Path.GetTempFileName()), ".shp");
                if (!sfFromBuffer.SaveAs(tmpFilename))
                    Assert.Fail("Failed to save shapefile: " + sfFromBuffer.ErrorMsg[sfFromBuffer.LastErrorCode]);
            }
            finally
            {
                ds.Close();
            }
        }

        [TestMethod]
        public void ListAllLayers()
        {
            var ds = new OgrDatasource();
            try
            {
                if (!ds.Open(CONNECTION_STRING))
                {
                    Assert.Fail("Failed to establish connection: " + ds.GdalLastErrorMsg);
                }
                Debug.WriteLine("ds.LayerCount: " + ds.LayerCount);
                for (var i = 0; i < ds.LayerCount; i++)
                {
                    var layer = ds.GetLayer2(i, false, false);
                    Debug.WriteLine(layer.Name + " has " + layer.FeatureCount + " features. Projection is " + layer.GeoProjection.Name);
                }

            }
            finally
            {
                ds.Close();
            }
        }

        private static void DeleteShapefile(string fileLocation)
        {
            if (!File.Exists(fileLocation))
            {
                Debug.WriteLine("In DeleteShapefile. Cannot find " + fileLocation);
                return;
            }

            var myDirectory = Path.GetDirectoryName(fileLocation);
            if (myDirectory == null) return;

            var baseName = Path.GetFileNameWithoutExtension(fileLocation);
            foreach (var f in Directory.EnumerateFiles(myDirectory, baseName + ".*"))
            {
                File.Delete(f);
                Debug.WriteLine("Deleting " + f);
            }
        }

        [TestMethod]
        public void GenerateEmptyWKT()
        {
            // MW uses GDAL to generate WKT for shapes.  As of MWGIS-219,
            // we should be able to generate proper WKT for empty shapes.
            Shape shp = new Shape();
            string wkt = string.Empty;
            // create each of the shape types without adding any points.
            // verify that the WKT does not come back as an empty string, 
            // but instead as the proper "EMPTY" WKT text (e.g "POINT EMPTY")
            shp.Create(ShpfileType.SHP_POINT);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POINTM);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POINTZ);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_MULTIPOINT);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_MULTIPOINTM);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_MULTIPOINTZ);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POLYGON);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POLYGONM);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POLYGONZ);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POLYLINE);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POLYLINEM);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_POLYLINEZ);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(!string.IsNullOrEmpty(wkt) && wkt.Contains("EMPTY"));
            shp = null;

            shp = new Shape();
            shp.Create(ShpfileType.SHP_NULLSHAPE);
            wkt = shp.ExportToWKT();
            Assert.IsTrue(string.IsNullOrEmpty(wkt));
            shp = null;

        }
    }
}
