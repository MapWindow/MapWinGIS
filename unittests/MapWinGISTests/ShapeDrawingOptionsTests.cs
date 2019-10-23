using System;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class ShapeDrawingOptionsTests
    {
        [TestMethod]
        public void Draw()
        {
            const int width = 300;
            const int height = 300;
            var utils = new Utils();

            var sf = new Shapefile();
            var shapeDrawingOptions = sf.DefaultDrawingOptions;
            var filename = Path.Combine(Path.GetTempPath(), "ShapeDrawingOptionsDraw.png");
            if (File.Exists(filename)) File.Delete(filename);

            using (var bmp = new Bitmap(width, height, PixelFormat.Format32bppArgb))
            using (var g = Graphics.FromImage(bmp))
            {
                var hdc = g.GetHdc();
                var hdcInt = hdc.ToInt32();
                var retVal = shapeDrawingOptions.DrawRectangle(hdcInt, 40.0f, 40.0f, width - 80, height - 80, true, width,
                    height, utils.ColorByName(tkMapColor.Brown), 255);
                Assert.IsTrue(retVal, "ShapeDrawingOptions.DrawRectangle failed");
                retVal = shapeDrawingOptions.DrawLine(hdcInt, 40.0f, 40.0f, width - 90, height - 90, true, width, height,
                    utils.ColorByName(tkMapColor.Beige), 255);
                Assert.IsTrue(retVal, "ShapeDrawingOptions.DrawLine failed");
                retVal = shapeDrawingOptions.DrawPoint(hdcInt, 40.0f, 40.0f, width - 100, height - 100,
                    utils.ColorByName(tkMapColor.BlueViolet), 255);
                Assert.IsTrue(retVal, "ShapeDrawingOptions.DrawPoint failed");
                g.ReleaseHdc(hdc);
                
                bmp.Save(filename, ImageFormat.Png);
                Debug.WriteLine(filename);
            }
        }
    }
}
