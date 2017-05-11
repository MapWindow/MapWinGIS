using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class UtilTests
    {
        [TestInitialize]
        public void Start()
        {
            Debug.WriteLine("Start Util tests");
            Debug.WriteLine(DateTime.Now);
        }

        [TestMethod]
        public void CalculateRaster()
        {
            var utils = new Utils();
            var tmpFile = Path.GetTempFileName();
            const string tiffInput = "GeoTiff/5band.tif";
            string errorMsg;
            var formula = $"([{tiffInput}@5]-[{tiffInput}@3])/([{tiffInput}@5]+[{tiffInput}@3])";
            // TODO: Can't use only one file:
            var result = utils.CalculateRaster(new[] { tiffInput, "GeoTiff/5bandCopy.tif" }, formula, tmpFile, "GTiff", 0f, null, out errorMsg);
            Assert.IsTrue(result, "utils.CalculateRaster was unsuccessful: " + errorMsg);
            Debug.WriteLine(tmpFile);

            var tiffIn = new ImageClass();
            result = tiffIn.Open(tiffInput);
            Assert.IsTrue(result, "Cannot open input tiff: " + tiffIn.ErrorMsg[tiffIn.LastErrorCode]);

            var tiffOut = new ImageClass();
            result = tiffOut.Open(tmpFile, ImageType.TIFF_FILE);
            Assert.IsTrue(result, "Cannot open output tiff: " + tiffOut.ErrorMsg[tiffOut.LastErrorCode]);

            // Check the output file with the input file:
            Assert.AreEqual(tiffIn.Extents.xMin, tiffOut.Extents.xMin, "Extents.xMin are not equal");
            Assert.AreEqual(tiffIn.Extents.xMax, tiffOut.Extents.xMax, "Extents.xMax are not equal");
            Assert.AreEqual(tiffIn.Extents.yMin, tiffOut.Extents.yMin, "Extents.yMin are not equal");
            Assert.AreEqual(tiffIn.Extents.yMax, tiffOut.Extents.yMax, "Extents.yMax are not equal");
            Assert.IsTrue(tiffIn.GeoProjection.IsSame[tiffOut.GeoProjection], "Projections are not the same");
        }

        [TestMethod]
        public void FixUpShapes()
        {
            var utils = new Utils();
            // Open shapefile:
            var sfInvalid = new Shapefile();
            var sfFixed = new Shapefile();
            try
            {
                var result = sfInvalid.Open(@"sf\invalid.shp");
                Assert.IsTrue(result, "Could not open shapefile");

                result = sfInvalid.HasInvalidShapes();
                Assert.IsTrue(result, "Shapefile has no invalid shapes");
                Helper.PrintExtents(sfInvalid.Extents);

                for (var i = 0; i < sfInvalid.NumShapes; i++)
                {
                    var shp = sfInvalid.Shape[i];
                    Assert.IsFalse(shp.IsValid, "Shape should be invalid");
                    var reason = shp.IsValidReason;
                    Debug.WriteLine(reason);
                    Assert.IsFalse(string.IsNullOrEmpty(reason), "Cannot get validation reason");
                }

                var newFilename = Path.Combine(Path.GetTempPath(), "FixUpShapes.shp");
                result = utils.FixUpShapes(sfInvalid, false, newFilename, true);
                Assert.IsTrue(result, "Could not fix shapefile");
                Assert.IsTrue(File.Exists(newFilename), newFilename + " doesn't exists");

                result = sfFixed.Open(newFilename);
                Assert.IsTrue(result, "Could not open fixed shapefile");

                Assert.AreEqual(sfInvalid.NumShapes, sfFixed.NumShapes, "Number of shapes are not equal");
                Helper.PrintExtents(sfFixed.Extents);
            }
            finally
            {
                sfInvalid.Close();
                sfFixed.Close();
            }
        }

        /// <summary>
        /// Test for https://mapwindow.atlassian.net/browse/MWGIS-65
        /// </summary>
        [TestMethod]
        public void ZonalStatistics()
        {
            const string rasterFile = @"GeoTiff\MWGIS-65.tif";
            if (!File.Exists(rasterFile)) throw new FileNotFoundException("Can't open " + rasterFile);

            const string shapefileFile = @"sf\MWGIS-65.shp";
            if (!File.Exists(shapefileFile)) throw new FileNotFoundException("Can't open " + shapefileFile);

            var grd = new Grid();
            if (!grd.Open(rasterFile, GridDataType.FloatDataType))
            {
                Assert.Fail("Can't open grid file: " + grd.ErrorMsg[grd.LastErrorCode]);
            }
            var sf = new Shapefile();
            if (!sf.Open(shapefileFile))
            {
                Assert.Fail("Can't open shapefile file: " + sf.ErrorMsg[sf.LastErrorCode]);
            }

            var utils = new Utils();
            Console.WriteLine("Before utils.GridStatisticsToShapefile");
            if (!utils.GridStatisticsToShapefile(grd, sf, false, true))
            {
                Assert.Fail("GridStatisticsToShapefile failed: " + utils.ErrorMsg[utils.LastErrorCode]);
            }
        }
    }
}
