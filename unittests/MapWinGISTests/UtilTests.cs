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
            var utils = new UtilsClass();
            var tmpFile = Path.GetTempFileName();
            string errorMsg;
            const string formula = "([5band.tif@5]-[5band.tif@3])/([5band.tif@5]+[5band.tif@3])";
            // TODO: Can't use only one file:
            var result = utils.CalculateRaster(new[] {@"GeoTiff/5band.tif", "HDF5/test.h5" }, formula, tmpFile, "GTiff", 0f, null, out errorMsg);
            // TODO: throws a memory error
            Assert.IsTrue(result, "utils.CalculateRaster was unsuccessful: " + errorMsg);
        }
    }
}
