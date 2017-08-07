using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [DeploymentItem("Testdata")]
    [TestClass]
    public class ImageTests
    {
        [TestMethod]
        public void ProjectionToImage()
        {
            var img = LoadImage(@"GeoTiff\5band.tif");
            Assert.IsNotNull(img, "Loaded object is not an image");

            Debug.WriteLine($"img.Width: {img.OriginalWidth}, img.Height: {img.OriginalHeight}");
            int row, column;

            img.ProjectionToImage(img.Extents.xMin, img.Extents.yMax - img.OriginalDY / 2, out column, out row);
            Debug.WriteLine($"Row: {row}, Column: {column}");
            Assert.AreEqual(0, row, "Row has unexpected value");
            Assert.AreEqual(0, column, "Column has unexpected value");

            img.ProjectionToImage(img.Extents.xMax, img.Extents.yMax - img.OriginalDY / 2, out column, out row);
            Debug.WriteLine($"Row: {row}, Column: {column}");
            Assert.AreEqual(0, row, "Row has unexpected value");
            Assert.AreEqual(img.OriginalWidth, column, "Column has unexpected value");

            img.ProjectionToImage(img.Extents.xMax, img.Extents.yMin, out column, out row);
            Debug.WriteLine($"Row: {row}, Column: {column}");
            Assert.AreEqual(img.OriginalHeight, row, "Row has unexpected value");
            Assert.AreEqual(img.OriginalWidth, column, "Column has unexpected value");

        }

        [TestMethod]
        public void ImageToProjection()
        {
            var img = LoadImage(@"GeoTiff\5band.tif");
            Assert.IsNotNull(img, "Loaded object is not an image");

            Debug.WriteLine($"xMin: {img.Extents.xMin}, yMax: {img.Extents.yMax}");
            Debug.WriteLine($"OriginalDX: {img.OriginalDX}, OriginalDY: {img.OriginalDY}");
            double projX, projY;

            img.ImageToProjection(0, 0, out projX, out projY);
            Debug.WriteLine($"projX: {projX}, projY: {projY}");
            Assert.AreEqual(img.Extents.xMin, projX, "projX has unexpected value");
            Assert.AreEqual(img.Extents.yMax - img.OriginalDY / 2, projY, "projY has unexpected value");

            img.ImageToProjection(img.OriginalWidth, 0, out projX, out projY);
            Debug.WriteLine($"projX: {projX}, projY: {projY}");
            Assert.AreEqual(img.Extents.xMax - img.OriginalDX / 2, projX, "projX has unexpected value");
            Assert.AreEqual(img.Extents.yMax - img.OriginalDY / 2, projY, "projY has unexpected value");

            img.ImageToProjection(img.OriginalWidth, img.OriginalHeight, out projX, out projY);
            Debug.WriteLine($"projX: {projX}, projY: {projY}");
            Assert.AreEqual(img.Extents.xMax - img.OriginalDX / 2, projX, "projX has unexpected value");
            Assert.AreEqual(img.Extents.yMin, projY, "projY has unexpected value");
        }

        [TestMethod, Timeout(30000)]
        //[TestMethod]
        public void LargeEcwFile()
        {
            // Timout after 30 seconds:
            var img = LoadImage(@"D:\dev\GIS-Data\Issues\MWGIS-70 ECW-crash\TK25.ecw");
            Assert.IsNotNull(img, "Could not open ECW file.");
        }

        [TestMethod]
        public void SmallEcwFile()
        {
            // Timout after 30 seconds:
            var img = LoadImage(@"D:\dev\GIS-Data\Raster\ecw\ECW-ERMapper-Compressed-Wavelets.ecw");
            Assert.IsNotNull(img, "Loaded object is not an image");
        }

        private static Image LoadImage(string filename)
        {
            if (!File.Exists(filename)) Assert.Fail("Input file does not exist: " + filename);

            var fm = new FileManager();
            var obj = fm.Open(filename, tkFileOpenStrategy.fosAutoDetect, null);
            if (obj != null && fm.LastOpenIsSuccess)
            {
                var img = obj as Image;
                Assert.IsNotNull(img, "Loaded object is not an image");
                return img;
            }

            return null;
        }
    }
}
