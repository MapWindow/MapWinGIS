using System.Diagnostics;
using System.IO;
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
            var img = LoadImageUsingFileManager(@"GeoTiff\5band.tif");
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
            var img = LoadImageUsingFileManager(@"GeoTiff\5band.tif");
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

        [TestMethod, Timeout(20000)]
        public void LargeEcwFile()
        {
            // Timout after 20 seconds:
            var img = LoadImageUsingFileManager(@"D:\dev\GIS-Data\Issues\MWGIS-70 ECW-crash\TK25.ecw");
            Assert.IsNotNull(img, "Could not open ECW file.");
        }

        [TestMethod]
        public void LargeEcwFileAsImage()
        {
            var img = new Image();
            const string filename = @"D:\dev\GIS-Data\Issues\MWGIS-70 ECW-crash\TK25.ecw";
            if (!img.Open(filename))
            {
                Assert.Fail("Failed to open datasource: " + img.ErrorMsg[img.LastErrorCode]);
            }
            Assert.IsNotNull(img, "Could not open ECW file as image");
        }

        [TestMethod]
        public void SmallEcwFile()
        {
            // Timout after 30 seconds:
            var img = LoadImageUsingFileManager(@"D:\dev\GIS-Data\Raster\ecw\ECW-ERMapper-Compressed-Wavelets.ecw");
            Assert.IsNotNull(img, "Loaded object is not an image");
        }

        [TestMethod]
        public void SaveImageShouldWork()
        {
            // MWGIS-80
            var img = new Image();
            img.Open(@"GeoTiff/5band.tif");
            Assert.IsNotNull(img, "Loaded object is not an image");
            var filename = Path.Combine(Path.GetTempPath(), "SaveImage.jpg");
            if (File.Exists(filename)) File.Delete(filename);
            var retVal = img.Save(filename);
            Assert.IsTrue(retVal, "Image could not be saved: " + img.ErrorMsg[img.LastErrorCode]);
            Assert.IsTrue(File.Exists(filename), "The file doesn't exists.");
            Debug.WriteLine(filename);
        }

        [TestMethod]
        public void SaveImageShouldFail()
        {
            // MWGIS-80
            var img = new Image();
            img.Open(@"GeoTiff/5band.tif");
            Assert.IsNotNull(img, "Loaded object is not an image");
            var filename = Path.Combine(Path.GetTempPath(), "SaveImage");
            if (File.Exists(filename)) File.Delete(filename);
            var retVal = img.Save(filename);
            Debug.WriteLine("Error: " + img.ErrorMsg[img.LastErrorCode]);
            Assert.IsFalse(retVal, "Image could be saved. This is unexpected.");
        }

        private static Image LoadImageUsingFileManager(string filename)
        {
            if (!File.Exists(filename))
                Assert.Fail("Input file does not exist: " + filename);

            var fm = new FileManager();
            var obj = fm.Open(filename);
            if (obj == null || !fm.LastOpenIsSuccess) return null;

            var img = obj as Image;
            Assert.IsNotNull(img, "Loaded object is not an image");
            return img;
        }

        [TestMethod]
        public void SerializeDeserializeImage()
        {
            // MWGIS-97

            int size = 2;
            Image original = new Image();
            // create 2x2 red square
            if (original.CreateNew(size, size))
            {
                for (int i = 0; i < size; i++)
                    for (int j = 0; j < size; j++)
                        original.Value[i, j] = 0xFF;
            }
            // serialize it
            string s = original.Serialize(true);

            // deserialize it
            Image modified = new Image();
            modified.Deserialize(s);

            // verify pixels
            Assert.AreEqual(original.Value[0, 0], modified.Value[0, 0]);
            Assert.AreEqual(original.Value[0, 1], modified.Value[0, 1]);
            Assert.AreEqual(original.Value[1, 0], modified.Value[1, 0]);
            Assert.AreEqual(original.Value[1, 1], modified.Value[1, 1]);
        }
    }
}
