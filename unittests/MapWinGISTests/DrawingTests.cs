using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Resources;

namespace MapWinGISTests
{
    [TestClass]
    public class DrawingTests
    {
        private AxMap _axMap1;

        [TestInitialize]
        public void Init()
        {
            // Create MapWinGIS:
            _axMap1 = Helper.GetAxMap();
            _axMap1.Projection = tkMapProjection.PROJECTION_WGS84;
            _axMap1.KnownExtents = tkKnownExtents.keLatvia;

            CreateLayer();
        }

        [TestMethod]
        public void DrawCircle()
        {
            _axMap1.ClearDrawings();
            var dhandle = _axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            _axMap1.DrawCircle(24.0, 57.0, 0.1, 0, false);
            Helper.SaveSnapshot2(_axMap1, "DrawCircle.jpg");
        }

        [TestMethod]
        public void DrawCircleEx()
        {
            _axMap1.ClearDrawings();
            var dhandle = _axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var utils = new Utils();
            _axMap1.DrawCircleEx(dhandle, 24.0, 57.0, 0.1, utils.ColorByName(tkMapColor.IndianRed), true);
            Helper.SaveSnapshot2(_axMap1, "DrawCircleEx.jpg");
        }

        [TestMethod]
        public void DrawLabel()
        {
            _axMap1.ClearDrawings();
            var dhandle = _axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var retVal = _axMap1.DrawLabel("label", 24.0, 57.0, 0.0);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            retVal = _axMap1.DrawLabel("Воздух", 24.05, 57.05, 0.0);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            Helper.SaveSnapshot2(_axMap1, "DrawLabel.jpg");
        }

        [TestMethod]
        public void DrawLabelEx()
        {
            _axMap1.ClearDrawings();
            var dhandle = _axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var retVal = _axMap1.DrawLabelEx(dhandle, "Воздух", 24.0, 57.0, 45.0);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            Helper.SaveSnapshot2(_axMap1, "DrawLabelEx.jpg");
        }

        [TestMethod]
        public void CheckUnicodeLabelText()
        {
            // https://mapwindow.atlassian.net/browse/MWGIS-81
            _axMap1.ClearDrawings();
            var dhandle = _axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            const string labelText = "Воздух";
            const double rotation = 45.0;
            var retVal = _axMap1.DrawLabelEx(dhandle, labelText, 24.0, 57.0, rotation);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            var labels = _axMap1.get_DrawingLabels(dhandle);
            for (var i = 0; i < labels.Count; i++)
            {
                var label = labels.Label[i, 0];
                Assert.AreEqual(rotation, label.Rotation);
                DebugMsg(label.Text);
                Assert.AreEqual(labelText, label.Text);
            }
        }

        [TestMethod]
        public void SaveSnapshot2()
        {
            // MWGIS-80
            Helper.SaveSnapshot2(_axMap1, "SaveSnapshot2.jpg");
            Helper.SaveSnapshot2(_axMap1, "SaveSnapshot2", true);
        }

 
        [TestMethod]
        public void CaptureSnapshot()
        {
            // clear map
            _axMap1.ClearDrawings();
            _axMap1.RemoveAllLayers();
            // create layer
            CreateLayer();
            
            //
            var bpp = Screen.PrimaryScreen.BitsPerPixel;
            DebugMsg($"Current color depth is {bpp} bits per pixel");
            // test is only valid when not in 32 bit color depth
            if (bpp == 32)
                DebugMsg("  Test is only valid for color depth less than 32 bpp");
            try
            {
                DebugMsg("Calling AxMap.Snapshot() method.  Watch for Access Violation Exception if color depth is less than 32 bpp.");
                var img = _axMap1.SnapShot(_axMap1.Extents);
                Assert.IsNotNull(img, "axMap1.SnapShot returned null");
                DebugMsg($"Successfully called Snapshot() with color depth = {bpp}.");
                if (bpp != 32)
                {
                    DebugMsg("  Test verified.");
                }
            }
            catch (AccessViolationException avex)
            {
                Assert.Fail(avex.ToString());
            }
            catch (Exception ex)
            {
                Assert.Fail(ex.ToString());
            }
        }


        private void CreateLayer()
        {
            var sf = new Shapefile();
            sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);

            var shp = new Shape();
            shp.Create(sf.ShapefileType);
            // 24.0, 57.0
            shp.AddPoint(23.8, 56.8);
            shp.AddPoint(23.8, 57.2);
            shp.AddPoint(24.2, 57.2);
            shp.AddPoint(24.2, 56.8);
            shp.AddPoint(23.8, 56.8);
            Assert.IsTrue(shp.IsValid, "Shape is invalid");

            sf.EditAddShape(shp);
            _axMap1.AddLayer(sf, true);
            _axMap1.ZoomToShape(0, 0);
            _axMap1.ZoomOut(0.5);
            _axMap1.ZoomToTileLevel(_axMap1.Tiles.CurrentZoom);
        }

        private void DebugMsg(string msg)
        {
            // Debug.WriteLine(msg);
            Console.WriteLine(msg);
        }

    }
}

