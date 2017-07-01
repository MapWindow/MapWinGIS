using System;
using System.Diagnostics;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class GdalUtilsTests
    {
        [TestMethod]
        public void GdalWarp()
        {
            // gdalwarp -dstnodata 0 -multi -overwrite -crop_to_cutline -cutline border.shp "bc1b395d-5011-4b25-947c-b06db932493b_index_ci_red-uptake.Amersfoort - RD New.tif" clipped.tif
            var gdalUtils = new MapWinGIS.GdalUtils();
            const string inputFilename = @"D:\dev\TopX\TopX-Agri\TestData\Chlorofyl-index.tif";
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
            Debug.WriteLine("retVal: " + retVal);
            Assert.IsTrue(retVal, "gdalUtils.GDALWarp() returned false: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode]);
        }
    }
}
