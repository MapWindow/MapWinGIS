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
                // ImportShapefile(ds, @"D:\dev\GIS-Data\MapWindow-Projects\UnitedStates\Shapefiles\cities.shp", "cities_points");

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
            if (!sfFromBuffer.SaveAs(tmpFilename))
                Assert.Fail("Failed to save shapefile: " + sfFromBuffer.ErrorMsg[sfFromBuffer.LastErrorCode]);

            if (!ds.ImportShapefile(sfFromBuffer, "sfFromBuffer", "OVERWRITE=YES",
                    tkShapeValidationMode.NoValidation))
                Assert.Fail("Failed to import buffered shapefile");

            layer.Close();
        }
    }
}
