using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class GdalUtilsTests
    {
        [TestMethod]
        public void GdalWarp()
        {
            var gdalUtils = new MapWinGIS.GdalUtils();
            const string inputFilename = @"J:\NBS3\src\NBS3.Services.Tests\TestData\GeoTiff\bioscope.tif";
            const string outputFilename = inputFilename + "-clipped.vrt";
            var retVal = gdalUtils.GDALWarp(inputFilename, outputFilename);
            Assert.IsTrue(retVal, "gdalUtils.GDALWarp() returned false: " + gdalUtils.ErrorMsg[gdalUtils.LastErrorCode]);
        }
    }
}
