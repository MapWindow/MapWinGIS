using System;
using System.Diagnostics;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class GdalUtilsTests: ICallback
    {
        [TestMethod]
        public void GdalWarp()
        {
            // gdalwarp -dstnodata 0 -multi -overwrite -crop_to_cutline -cutline border.shp "bc1b395d-5011-4b25-947c-b06db932493b_index_ci_red-uptake.Amersfoort - RD New.tif" clipped.tif
            var gdalUtils = new MapWinGIS.GdalUtils {GlobalCallback = this};
            const string inputFilename = @"D:\dev\TopX\TopX-Agri\TestData\Chlorofyl-index.tif3";
            const string outputFilename = inputFilename + "-clipped.vrt";
            const string borderFilename = @"D:\dev\TopX\TopX-Agri\TestData\Valtermond.shp";
            var options = new[]
            {
                "-of", "vrt",
                "-overwrite",
                "-dstnodata", "0",
                "-crop_to_cutline",
                "-cutline", borderFilename
            };
            var retVal = gdalUtils.GDALWarp(inputFilename, outputFilename, options);
            WriteLine("retVal: " + retVal);
            WriteLine("Output: " + outputFilename);
            Assert.IsTrue(retVal,
                "gdalUtils.GDALWarp() returned false: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode]);
        }


        private static void WriteLine(string msg)
        {
            Debug.WriteLine(msg);
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