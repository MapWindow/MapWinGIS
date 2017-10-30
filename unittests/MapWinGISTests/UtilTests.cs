using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Remoting;
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

            var newShapefileFile = Path.Combine(Path.GetTempPath(), "ZonalStatistics.shp");
            if (File.Exists(newShapefileFile))
            {
                foreach (var f in Directory.EnumerateFiles(Path.GetDirectoryName(newShapefileFile), "ZonalStatistics.*"))
                {
                    File.Delete(f);
                }
            }

            Assert.IsTrue(sf.SaveAs(newShapefileFile), "Could not save resulting shapefile");

            Console.WriteLine("Saved as " + newShapefileFile);
        }

        [TestMethod]
        public void GdalInfoEcw()
        {
            var utils = new Utils();
            var retVal = utils.GDALInfo(@"D:\dev\GIS-Data\Issues\ECW-crash\TK25.ecw", string.Empty);
            Debug.WriteLine(retVal);
            Assert.IsTrue(retVal.Contains("Driver: ECW/ERDAS Compressed Wavelets (SDK 5."), "Wrong ECW driver");
        }

        [TestMethod]
        public void GdalFormats()
        {
            var utils = new Utils();
            var retVal = utils.GDALInfo(@"D:\dev\GIS-Data\Issues\ECW-crash\TK25.ecw", "--formats");
            Debug.WriteLine(retVal);
        }

        [TestMethod]
        public void ReclassifyRaster()
        {
            const string input = @"D:\dev\TopX\TopX-Agri\TestData\Chlorofyl-index.clipped.optimized.tif";
            const string output = @"D:\dev\TopX\TopX-Agri\TestData\Chlorofyl-index.clipped.optimized.reclassified.tif";
            var gridSource = new GridClass();
            gridSource.Open(input);

            var nodataValue = (double)gridSource.Header.NodataValue;
            double min, max, mean, stdDev;
            var retVal = gridSource.ActiveBand.GetStatistics(false, true, out min, out max, out mean, out stdDev);
            Assert.IsTrue(retVal, "Could not get statistics: " + gridSource.ErrorMsg[gridSource.LastErrorCode]);
            gridSource.Close();
            Console.WriteLine($"Input statistics: {min} - {max}");
            var newMax = 0.9 * max;
            var newMin = 1.2 * min;

            var arr = new[]
            {
                new {Low = nodataValue + 1, High = newMin, NewValue = newMin},
                new {Low = newMax, High = max + 1, NewValue = newMax}
            };
            var utils = new Utils();
            retVal = utils.ReclassifyRaster(input, 1, output, arr.Select(i => i.Low).ToArray(),
                arr.Select(i => i.High).ToArray(), arr.Select(i => i.NewValue).ToArray(), "GTiff", null);
            Assert.IsTrue(retVal, "ReclassifyRaster failed: " + utils.ErrorMsg[utils.LastErrorCode]);

            gridSource = new GridClass();
            gridSource.Open(output);

            var nodataValueOutput = (double)gridSource.Header.NodataValue;
            double minOutput, maxOutput, meanOutput, stdDevOutput;
            retVal = gridSource.ActiveBand.GetStatistics(false, true, out minOutput, out maxOutput, out meanOutput, out stdDevOutput);
            Assert.IsTrue(retVal, "Could not get statistics: " + gridSource.ErrorMsg[gridSource.LastErrorCode]);
            gridSource.Close();
            Console.WriteLine($"Output statistics: {minOutput} - {maxOutput}");

            // Checks:
            Assert.AreEqual(nodataValue, nodataValueOutput, "Nodata values are different");
            Assert.IsTrue(Math.Round(minOutput, 4) >= Math.Round(newMin, 4), $"New minimum is incorrect. got {minOutput} expected {newMin}");
            Assert.IsTrue(Math.Round(maxOutput, 4) <= Math.Round(newMax, 4), $"New maximum is incorrect. got {maxOutput} expected {newMax}");
        }

        [TestMethod]
        public void ClipPolygon()
        {
            const string folder = @"D:\dev\GIS-Data\Issues\ClipGridWithPolygon\";

            // Create new in-memory shapefile:
            var sf = new Shapefile();
            var retVal = sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);
            Assert.IsTrue(retVal, "Could not CreateNewWithShapeID: " + sf.ErrorMsg[sf.LastErrorCode]);

            // Assign projection:
            var projection = new GeoProjection();
            retVal = projection.ImportFromEPSG(4326);
            Assert.IsTrue(retVal, "Could not ImportFromEPSG(4326): " + projection.ErrorMsg[projection.LastErrorCode]);
            sf.GeoProjection = projection;

            // Create shape:
            var shp = new Shape();
            retVal = shp.Create(ShpfileType.SHP_POLYGON);
            Assert.IsTrue(retVal, "Could not shp.Create: " + shp.ErrorMsg[shp.LastErrorCode]);

            // Add point of polygon:
            var numPoints = 0;
            shp.InsertPoint(new Point { y = 38.25853, x = 15.7033983 }, ref numPoints);
            shp.InsertPoint(new Point { y = 38.248108, x = 15.7033983 }, ref numPoints);
            shp.InsertPoint(new Point { y = 38.248108, x = 15.7245293 }, ref numPoints);
            shp.InsertPoint(new Point { y = 38.25853, x = 15.7245293 }, ref numPoints);
            // Make sure the polygon is closed by adding the first point as last:
            shp.InsertPoint(new Point { y = 38.25853, x = 15.7033983 }, ref numPoints);
            Assert.IsTrue(shp.IsValid, "Shape is invalid: " + shp.IsValidReason);

            // Add shape to shapefile:
            sf.EditAddShape(shp);

            // Save to file:
            retVal = sf.SaveAs(Path.Combine(folder, "ClippingArea-4326.shp"));
            Assert.IsTrue(retVal, "Could not SaveAs: " + sf.ErrorMsg[sf.LastErrorCode]);

            // Clip grid, using Utils.ClipGridWithPolygon fails on the LandSat data, probably because it is in UInt16.
            var gdalUtils = new GdalUtils();
            var input = Path.Combine(folder, "LC08_L1TP_188033_20170919_20170920_01_RT_B4.TIF");
            Assert.IsTrue(File.Exists(input), "Input file does not exists");
            var output = Path.Combine(folder, "clipped.tif");
            if (File.Exists(output)) File.Delete(output);
            var cutline = Path.Combine(folder, "ClippingArea-4326.shp");
            Assert.IsTrue(File.Exists(cutline), "Cutline file does not exists");

            var options = new[]
            {
                "-overwrite",
                "-crop_to_cutline",
                "-cutline", cutline
            };
            retVal = gdalUtils.GDALWarp(input, output, options);

            Assert.IsTrue(retVal, "Could not ClipGridWithPolygon: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode]);
            Assert.IsTrue(File.Exists(output), "Output file does not exists");
            Debug.WriteLine(output);
        }
    }
}
