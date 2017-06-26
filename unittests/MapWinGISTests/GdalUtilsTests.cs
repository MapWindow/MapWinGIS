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
            var retVal = gdalUtils.GDALWarp();
            Assert.IsTrue(retVal, "gdalUtils.GDALWarp() returned false");
        }
    }
}
