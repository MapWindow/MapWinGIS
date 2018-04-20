using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class UtilTests : ICallback
    {
        [TestInitialize]
        public void Start()
        {
            Console.WriteLine("Start Util tests");
            Console.WriteLine(DateTime.Now);
        }

        [TestMethod]
        public void CalculateRaster()
        {
            var utils = new Utils { GlobalCallback = this };
            var tmpFile = Path.GetTempFileName();
            const string tiffInput = "GeoTiff/5band.tif";
            string errorMsg;
            var formula = $"([{tiffInput}@5]-[{tiffInput}@3])/([{tiffInput}@5]+[{tiffInput}@3])";
            // TODO: Can't use only one file:
            var result = utils.CalculateRaster(new[] { tiffInput, "GeoTiff/5bandCopy.tif" }, formula, tmpFile, "GTiff", 0f, null, out errorMsg);
            Assert.IsTrue(result, "utils.CalculateRaster was unsuccessful: " + errorMsg);
            Console.WriteLine(tmpFile);

            var tiffIn = new Image();
            result = tiffIn.Open(tiffInput);
            Assert.IsTrue(result, "Cannot open input tiff: " + tiffIn.ErrorMsg[tiffIn.LastErrorCode]);

            var tiffOut = new Image();
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
            var utils = new Utils { GlobalCallback = this };
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
                    Console.WriteLine(reason);
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

            var grd = new Grid { GlobalCallback = this };
            if (!grd.Open(rasterFile, GridDataType.FloatDataType))
            {
                Assert.Fail("Can't open grid file: " + grd.ErrorMsg[grd.LastErrorCode]);
            }
            var sf = new Shapefile { GlobalCallback = this };
            if (!sf.Open(shapefileFile))
            {
                Assert.Fail("Can't open shapefile file: " + sf.ErrorMsg[sf.LastErrorCode]);
            }

            var utils = new Utils { GlobalCallback = this };
            Console.WriteLine("Before utils.GridStatisticsToShapefile");
            if (!utils.GridStatisticsToShapefile(grd, sf, false, true))
            {
                Assert.Fail("GridStatisticsToShapefile failed: " + utils.ErrorMsg[utils.LastErrorCode]);
            }

            var newShapefileFile = Path.Combine(Path.GetTempPath(), "ZonalStatistics.shp");
            if (File.Exists(newShapefileFile))
            {
                // ReSharper disable once AssignNullToNotNullAttribute
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
            var utils = new Utils { GlobalCallback = this };
            var retVal = utils.GDALInfo(@"D:\dev\GIS-Data\Issues\MWGIS-70 ECW-crash\TK25.ecw", string.Empty);
            Assert.IsNotNull(retVal, "GDALInfo failed: " + utils.ErrorMsg[utils.LastErrorCode]);
            Console.WriteLine(retVal);
            Assert.IsTrue(retVal.Contains("Driver: ECW/ERDAS Compressed Wavelets (SDK 5."), "Wrong ECW driver");
        }

        [TestMethod]
        public void GdalFormats()
        {
            var utils = new Utils { GlobalCallback = this };
            var retVal = utils.GDALInfo(@"D:\dev\GIS-Data\Issues\ECW-crash\TK25.ecw", "--formats");
            Console.WriteLine(retVal);
        }

        [TestMethod]
        public void ReclassifyRaster()
        {
            const string input = @"D:\dev\TopX\TopX-Agri\TestData\Chlorofyl-index.clipped.optimized.tif";
            const string output = @"D:\dev\TopX\TopX-Agri\TestData\Chlorofyl-index.clipped.optimized.reclassified.tif";
            var gridSource = new Grid { GlobalCallback = this };
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
            var utils = new Utils { GlobalCallback = this };
            retVal = utils.ReclassifyRaster(input, 1, output, arr.Select(i => i.Low).ToArray(),
                arr.Select(i => i.High).ToArray(), arr.Select(i => i.NewValue).ToArray(), "GTiff", this);
            Assert.IsTrue(retVal, "ReclassifyRaster failed: " + utils.ErrorMsg[utils.LastErrorCode]);

            gridSource = new Grid { GlobalCallback = this };
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
        public void PointInPolygon()
        {
            // It goes too fast for DotMemory:
            Thread.Sleep(2000);

            const string folder = @"D:\dev\GIS-Data\Issues\Point in Polygon";
            Assert.IsTrue(Directory.Exists(folder), "Input folder doesn't exists");
            var sfPolygons = new Shapefile { GlobalCallback = this };
            var sfPoints = new Shapefile { GlobalCallback = this };
            var found = 0;
            var stopWatch = new Stopwatch();
            stopWatch.Start();

            try
            {
                var retVal = sfPolygons.Open(Path.Combine(folder, "CatchmentBuilderShapefile.shp"));
                Assert.IsTrue(retVal, "Can't open polygon shapefile");

                retVal = sfPoints.Open(Path.Combine(folder, "Sbk_FGrPt_n.shp"));
                Assert.IsTrue(retVal, "Can't open point shapefile");

                var utils = new Utils { GlobalCallback = this };
                var numPolygons = sfPolygons.NumShapes;
                Assert.IsTrue(numPolygons > 0, "No polygon shapes in shapefile");

                // Loop polygons:
                for (var i = 0; i < numPolygons; i++)
                {
                    if (found > 100) break;

                    // Get polygon
                    var polygonShape = sfPolygons.Shape[i];
                    Assert.IsNotNull(polygonShape, "polygonShape == null");

                    var numPoints = sfPoints.NumShapes;
                    Assert.IsTrue(numPoints > 0, "No point shapes in shapefile");

                    for (var j = 0; j < numPoints; j++)
                    {
                        var pointShape = sfPoints.Shape[j];
                        Assert.IsNotNull(pointShape, "pointShape == null");

                        if (utils.PointInPolygon(polygonShape, pointShape.Point[0]))
                        {
                            Console.WriteLine($"Point {j} lies in polygon {i}");
                            found++;
                        }
                    }
                }
            }
            finally
            {
                // Close shapefiles:
                sfPolygons.Close();
                sfPoints.Close();
            }

            stopWatch.Stop();
            Console.WriteLine("The process took " + stopWatch.Elapsed);
            Console.WriteLine(found + " matching polygons where found");
        }

        [TestMethod]
        public void ProjectionStrings()
        {
            var utils = new Utils {GlobalCallback = this};
            var gp = new GeoProjection {GlobalCallback = this};

            // get NAD83 name
            var utilProjection = utils.GetNAD83ProjectionName(tkNad83Projection.Nad83_Alabama_East);
            gp.ImportFromEPSG((int)tkNad83Projection.Nad83_Alabama_East);
            var importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get WGS84 name
            utilProjection = utils.GetWGS84ProjectionName(tkWgs84Projection.Wgs84_BLM_14N_ftUS);
            gp.ImportFromEPSG((int)tkWgs84Projection.Wgs84_BLM_14N_ftUS);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get ESRI-specific NAD83 name
            utilProjection = utils.GetNAD83ProjectionName(tkNad83Projection.NAD_1983_StatePlane_Alabama_East_FIPS_0101_Feet);
            gp.ImportFromEPSG((int)tkNad83Projection.NAD_1983_StatePlane_Alabama_East_FIPS_0101_Feet);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get ESRI-specific WGS84 name
            utilProjection = utils.GetWGS84ProjectionName(tkWgs84Projection.WGS_1984_Web_Mercator);
            gp.ImportFromEPSG((int)tkWgs84Projection.WGS_1984_Web_Mercator);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get NAD83 name by ID
            utilProjection = utils.GetProjectionNameByID((int)tkNad83Projection.Nad83_Alabama_East);
            gp.ImportFromEPSG((int)tkNad83Projection.Nad83_Alabama_East);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get WGS84 name by ID
            utilProjection = utils.GetProjectionNameByID((int)tkWgs84Projection.Wgs84_BLM_14N_ftUS);
            gp.ImportFromEPSG((int)tkWgs84Projection.Wgs84_BLM_14N_ftUS);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get ESRI-specific NAD83 name by ID
            utilProjection = utils.GetProjectionNameByID((int)tkNad83Projection.NAD_1983_StatePlane_Alabama_East_FIPS_0101_Feet);
            gp.ImportFromEPSG((int)tkNad83Projection.NAD_1983_StatePlane_Alabama_East_FIPS_0101_Feet);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get ESRI-specific WGS84 name by ID
            utilProjection = utils.GetProjectionNameByID((int)tkWgs84Projection.WGS_1984_Web_Mercator);
            gp.ImportFromEPSG((int)tkWgs84Projection.WGS_1984_Web_Mercator);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get obscure names by ID
            utilProjection = utils.GetProjectionNameByID(2402);
            gp.ImportFromEPSG(2402);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // get obscure names by ID
            utilProjection = utils.GetProjectionNameByID(20005);
            gp.ImportFromEPSG(20005);
            importProjection = gp.Name;
            Assert.AreEqual(utilProjection, importProjection);

            // verify error
            utilProjection = utils.GetProjectionNameByID(100);
            Assert.IsTrue(utilProjection.Length == 0);
            // should return Index Out-of-bounds error
            var errorMsg = utils.ErrorMsg[utils.LastErrorCode];
            Console.WriteLine(errorMsg);
            Assert.AreEqual("Index Out of Bounds", errorMsg);
        }


        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            Console.Write(".");
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            Assert.Fail("Error found: " + ErrorMsg);
        }
    }
}
