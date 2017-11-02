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
        private GdalUtils _gdalUtils;

        public GdalUtilsTests()
        {
            _gdalUtils = new GdalUtils { GlobalCallback = this };
        }

    [TestMethod]
        public void GdalWarp()
        {
            // gdalwarp -dstnodata 0 -multi -overwrite -crop_to_cutline -cutline border.shp "bc1b395d-5011-4b25-947c-b06db932493b_index_ci_red-uptake.Amersfoort - RD New.tif" clipped.tif
            const string inputFilename = @"D:\dev\TopX\TopX-Agri\TestData\Chlorofyl-index.tif";
            const string borderFilename = @"D:\dev\TopX\TopX-Agri\TestData\Valtermond.shp";
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
            var retVal = _gdalUtils.GDALWarp(inputFilename, outputFilename, options);
            WriteLine("retVal: " + retVal);
            Assert.IsTrue(retVal, "gdalUtils.GDALWarp() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode]);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
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
            Assert.IsTrue(retVal, "gdalUtils.GdalVectorTranslate() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode]);
            Assert.IsTrue(File.Exists(outputFilename), "Can't find the output file");
            WriteLine(outputFilename + " is created");
        }

        [TestMethod]
        public void VerySmallClip()
        {
            var tempFolder = Helper.WorkingFolder("VerySmallClip");
            var sfBorder = Helper.CreateSfFromWkt(
                "POLYGON ((693416.416338362 5841003.20610673,693424.331109333 5840997.77042745,693415.26280084 5840989.96669721,693403.190049054 5841000.68434421,693416.416338362 5841003.20610673))",
                32631);
            var borderFilename = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} border.shp");
            Helper.SaveShapefile(sfBorder, borderFilename);
            sfBorder.Close();

            var sfSubject = Helper.CreateSfFromWkt(
                "POLYGON ((693395.4 5840980.6,693395.4 5840995.6,693410.4 5840995.6,693410.4 5840980.6,693395.4 5840980.6))",
                32631);
            var subjectFilename = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} subject.shp");
            Helper.SaveShapefile(sfSubject, subjectFilename);
            sfSubject.Close();

            var outputFilename = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} GdalVectorTranslate.shp");
            if (File.Exists(outputFilename)) File.Delete(outputFilename);

            // ogr2ogr -clipsrc "Droog-Sloefweg.WGS 84 - UTM zone 31N.shp" clipped.shp Fishnet.shp
            var options = new[]
            {
                "-f", "ESRI Shapefile",
                "-overwrite",
                "-clipsrc", borderFilename
            };
            var retVal = _gdalUtils.GdalVectorTranslate(subjectFilename, outputFilename, options, true);
            Assert.IsTrue(retVal, "gdalUtils.GdalVectorTranslate() returned false: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode]);
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
            WriteLine(Message + ": " + Percent);
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            WriteLine("Error: " + ErrorMsg);
        }
    }
}