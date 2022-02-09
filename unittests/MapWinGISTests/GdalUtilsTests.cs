using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class GdalUtilsTests : ICallback
    {
        private readonly GdalUtils _gdalUtils;

        public GdalUtilsTests()
        {
            // https://github.com/dwtkns/gdal-cheat-sheet
            _gdalUtils = new GdalUtils { GlobalCallback = this };
            Debug.WriteLine("Start of tests " + DateTime.Now);
        }

        [TestMethod, Timeout(5 * 60 * 1000)]
        public void GdalRasterTranslate()
        {
            const string inputFilename = @"D:\dev\MapWindow\MapWinGIS\git\unittests\MapWinGISTests\Testdata\GeoTiff\5band.tif";
            const string outputFilename = inputFilename + "-translated.tif";
            if (File.Exists(outputFilename)) File.Delete(outputFilename);

            // -tr 0.2 0.2 -r average -projwin -180 90 180 -90 -ot Float32
            var options = new[]
            {
                "-ot", "Float32",
                "-tr", "0.2", "0.2",
                "-r", "average",
                "-projwin", "-180", "90", "180", "-90"
            };
            var retVal = _gdalUtils.GdalRasterTranslate(inputFilename, outputFilename, options);
            WriteLine("retVal: " + retVal);
            Assert.IsTrue(retVal, "gdalUtils.GdalRasterTranslate() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
        }

        [TestMethod, Timeout(5 * 60 * 1000)]
        public void GdalWarp()
        {
            // gdalwarp -dstnodata 0 -multi -overwrite -crop_to_cutline -cutline border.shp "bc1b395d-5011-4b25-947c-b06db932493b_index_ci_red-uptake.Amersfoort - RD New.tif" clipped.tif
            const string inputFilename = @"D:\dev\MapWindow\MapWinGIS\git\unittests\MapWinGISTests\Testdata\GeoTiff\Chlorofyl.tif";
            const string borderFilename = @"D:\dev\MapWindow\MapWinGIS\git\unittests\MapWinGISTests\Testdata\sf\ClipForChlorofyl.shp";
            const string outputFilename = inputFilename + "-clipped.vrt";
            if (File.Exists(outputFilename)) File.Delete(outputFilename);

            var options = new[]
            {
                "-of", "vrt",
                "-overwrite",
                "-crop_to_cutline",
                "-cutline", borderFilename,
                "-dstnodata", "0"
            };
            var retVal = _gdalUtils.GdalRasterWarp(inputFilename, outputFilename, options);
            WriteLine("options.Length: " + options.Length);
            WriteLine("retVal: " + retVal);
            Assert.IsTrue(retVal, "gdalUtils.GDALWarp() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
        }

        [TestMethod, Timeout(5 * 60 * 1000)]
        public void GdalWarp2()
        {
            // Comes from GdalTests.GdalWarp, which uses the old
            // utils.gdalwarp (now deprecated)

            var output = Path.GetTempPath() + "GdalWarp.vrt";
            if (File.Exists(output)) File.Delete(output);

            var options = new[]
            {
                "-of", "vrt",
                "-overwrite"
            };
            var retVal = _gdalUtils.GdalRasterWarp(@"GeoTiff/5band.tif", output, options);
            Assert.IsTrue(retVal, "GdalWarp failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(output), "Output file doesn't exists");
            Debug.WriteLine(output);
        }

        [TestMethod, Timeout(5 * 60 * 1000)]
        public void GdalBuildOverviews()
        {
            // the list of overview decimation factors to build, or NULL if nOverviews == 0:
            var overviewList = new[] { 2, 4, 8, 16, 32, 64, 128 };
            var configOptions = new[] { "COMPRESS_OVERVIEW DEFLATE" };
            // list of band numbers:
            var bandList = new[] { 1, 2, 3, 4, 5 };
            GdalBuildOverviewsCore(@"D:\dev\MapWindow\MapWinGIS\git\unittests\MapWinGISTests\Testdata\GeoTiff\5band.tif", overviewList, bandList, configOptions);
        }

        [TestMethod, Timeout(5 * 60 * 1000)]
        public void GdalBuildOverviewsAuto()
        {
            var configOptions = new[] { "COMPRESS_OVERVIEW DEFLATE" };
            GdalBuildOverviewsCore(@"D:\dev\MapWindow\MapWinGIS\git\unittests\MapWinGISTests\Testdata\GeoTiff\5band.tif", null, null, configOptions);
        }        
        
        [TestMethod, Timeout(5 * 60 * 1000)]
        public void GdalBuildOverviewsAuto2()
        {
            var configOptions = new[] { "COMPRESS_OVERVIEW DEFLATE", "INTERLEAVE_OVERVIEW PIXEL" };
            GdalBuildOverviewsCore(@"D:\dev\MapWindow\MapWinGIS\git\unittests\MapWinGISTests\Testdata\GeoTiff\Chlorofyl.tif", null, null, configOptions);
        }

        private void GdalBuildOverviewsCore(string inputFilename, int[] overviewList, int[] bandList, string[] configOptions
            )
        {
            if (!File.Exists(inputFilename)) Assert.Fail("Input file doesn't exists: " + inputFilename);
            var ovrFilename = inputFilename + ".ovr";
            if (File.Exists(ovrFilename)) File.Delete(ovrFilename);

            // https://gdal.org/programs/gdaladdo.html
            // https://gdal.org/api/gdaldataset_cpp.html#classGDALDataset_1a2aa6f88b3bbc840a5696236af11dde15
            // To produce the smallest possible JPEG-In-TIFF overviews, you should use:
            //--config COMPRESS_OVERVIEW JPEG --config PHOTOMETRIC_OVERVIEW YCBCR --config INTERLEAVE_OVERVIEW PIXEL

            var retVal = _gdalUtils.GdalBuildOverviews(inputFilename, tkGDALResamplingMethod.grmCubic, overviewList, bandList, configOptions);
            WriteLine("retVal: " + retVal);
            Assert.IsTrue(retVal, "gdalUtils.GdalBuildOverviews() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(ovrFilename), ".ovr file is not found");
        }

        // Missing data [TestMethod, Timeout(5 * 60 * 1000)]
        public void GdalWarpCutline()
        {
            // Comes from GdalTests.GdalWarpCutline, which uses the old
            // utils.gdalwarp (now deprecated)

            var output = Path.GetTempPath() + "GdalWarpCutline.vrt";
            if (File.Exists(output)) File.Delete(output);
            const string border = @"J:\_testdata\Haulmkilling2\20171019-Agrifac-586-Prinzen-wdvi\Prinzen - Dinxperlo.shp";

            var options = new[]
            {
                "-of", "vrt",
                "-overwrite",
                "-crop_to_cutline",
                "-cutline", border
            };
            var retVal = _gdalUtils.GdalRasterWarp(@"J:\_testdata\Haulmkilling2\20171019-Agrifac-586-Prinzen-wdvi\7da7d241-aa4f-4955-861e-62efbe85adf8_index_wdvi.tif", output, options);
            Assert.IsTrue(retVal, "GdalWarp failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(output), "Output file doesn't exists");
            Debug.WriteLine(output);
        }

        // Missing data [TestMethod, Timeout(5 * 60 * 1000)]
        public void ClipPolygon()
        {
            var settings = new GlobalSettings { OgrShareConnection = true };
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
            Helper.SaveAsShapefile(sf, Path.Combine(folder, "ClippingArea-4326.shp"));

            // Clip grid, using Utils.ClipGridWithPolygon fails on the LandSat data, probably because it is in UInt16.
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
            retVal = _gdalUtils.GdalRasterWarp(input, output, options);
            Assert.IsTrue(retVal, "Could not ClipGridWithPolygon: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(output), "Output file does not exists");
            Debug.WriteLine(output);
        }

        [TestMethod]
        public void Shapefile2Gml()
        {
            // ogr2ogr -f "GML" Bioscope.gml  Bioscope.shp
            const string inputFilename = @"sf\MWGIS-65.shp";
            var outputFilename = Path.Combine(Path.GetTempPath(), "translated.gml");
            if (File.Exists(outputFilename)) File.Delete(outputFilename);

            var options = new[]
            {
                "-f", "GML"
            };

            var retVal = _gdalUtils.GdalVectorTranslate(inputFilename, outputFilename, options, true);
            Assert.IsTrue(retVal, "gdalUtils.GdalVectorTranslate() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
        }

        [TestMethod]
        public void VerySmallClip()
        {
            var settings = new GlobalSettings { OgrShareConnection = true };
            var tempFolder = Helper.WorkingFolder("VerySmallClip");
            var sfBorder = Helper.CreateSfFromWkt(
                "POLYGON ((693416.416338362 5841003.20610673,693424.331109333 5840997.77042745,693415.26280084 5840989.96669721,693403.190049054 5841000.68434421,693416.416338362 5841003.20610673))",
                32631);
            var borderFilename = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} border.shp");
            Helper.SaveAsShapefile(sfBorder, borderFilename);
            sfBorder.Close();

            var sfSubject = Helper.CreateSfFromWkt(
                "POLYGON ((693395.4 5840980.6,693395.4 5840995.6,693410.4 5840995.6,693410.4 5840980.6,693395.4 5840980.6))",
                32631);
            var subjectFilename = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} subject.shp");
            Helper.SaveAsShapefile(sfSubject, subjectFilename);
            sfSubject.Close();

            var outputFilename = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} GdalVectorTranslate.shp");
            if (File.Exists(outputFilename)) File.Delete(outputFilename);

            // ogr2ogr -clipsrc "Droog-Sloefweg.WGS 84 - UTM zone 31N.shp" clipped.shp Fishnet.shp
            var stopWatch = new Stopwatch();
            stopWatch.Start();
            var retVal = _gdalUtils.ClipVectorWithVector(subjectFilename, borderFilename, outputFilename);
            stopWatch.Stop();
            WriteLine("VerySmallClip took " + stopWatch.Elapsed);
            WriteLine("Detailed error: " + _gdalUtils.DetailedErrorMsg);

            Assert.IsTrue(retVal, "gdalUtils.GdalVectorTranslate() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode]);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
        }

        [TestMethod]
        public void GdalVectorReproject()
        {
            var outputFilename = Path.ChangeExtension(Path.Combine(Path.GetTempPath(), Path.GetRandomFileName()), ".shp");
            var retVal = _gdalUtils.GdalVectorReproject(
                @"D:\dev\GIS-data\Siebe\20200116 Clip\afwateringseenheden_2019_copy_join_pgb_20190704.shp",
                outputFilename, 28992, 4326);
            Assert.IsTrue(retVal, "gdalUtils.GdalVectorReproject() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode]);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
        }

        // No testdata: [TestMethod, Timeout(5 * 60 * 1000)]
        public void VeryLargeClip()
        {
            var settings = new GlobalSettings { OgrShareConnection = true };
            var tempFolder = Helper.WorkingFolder("VeryLargeClip");
            const string subjectFilename = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\Fishnet.shp";
            const string borderFilename = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\border.shp";

            var outputFilename = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} GdalVectorTranslate.shp");
            if (File.Exists(outputFilename)) File.Delete(outputFilename);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            var retVal = _gdalUtils.ClipVectorWithVector(subjectFilename, borderFilename, outputFilename);
            stopWatch.Stop();
            WriteLine("VeryLargeClip took " + stopWatch.Elapsed);

            Assert.IsTrue(retVal, "gdalUtils.GdalVectorTranslate() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
        }

        private static void WriteLine(string msg)
        {
            // Debug.WriteLine(msg);
            Console.WriteLine(msg);
        }

        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            WriteLine("Callback Progress: " + Message + ": " + Percent);
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            WriteLine("Callback Error: " + ErrorMsg + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
        }
    }
}