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
        private AxMap axMap1;
        private Form myForm;

        [TestInitialize]
        public void Init()
        {
            // Create MapWinGIS:
            axMap1 = new AxMap();
            axMap1.CreateControl();

            // Create form and add MapWinGIS:
            myForm = new Form();
            myForm.Controls.Add(axMap1);

            axMap1.Projection = tkMapProjection.PROJECTION_WGS84;
            axMap1.KnownExtents = tkKnownExtents.keLatvia;
            axMap1.ScalebarVisible = true;
            axMap1.ShowCoordinates = tkCoordinatesDisplay.cdmAuto;
            axMap1.ShowRedrawTime = true;
            axMap1.ShowVersionNumber = true;
            axMap1.ShowZoomBar = true;

            CreateLayer();
        }

        [TestMethod]
        public void DrawCircle()
        {
            axMap1.ClearDrawings();
            var dhandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            axMap1.DrawCircle(24.0, 57.0, 0.1, 0, false);
            SaveSnapshot("DrawCircle.jpg");
        }

        [TestMethod]
        public void DrawCircleEx()
        {
            axMap1.ClearDrawings();
            var dhandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var utils = new Utils();
            axMap1.DrawCircleEx(dhandle, 24.0, 57.0, 0.1, utils.ColorByName(tkMapColor.IndianRed), true);
            SaveSnapshot("DrawCircleEx.jpg");
        }

        [TestMethod]
        public void DrawLabel()
        {
            axMap1.ClearDrawings();
            var dhandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var retVal = axMap1.DrawLabel("label", 24.0, 57.0, 0.0);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            retVal = axMap1.DrawLabel("Воздух", 24.05, 57.05, 0.0);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            SaveSnapshot("DrawLabel.jpg");
        }

        [TestMethod]
        public void DrawLabelEx()
        {
            axMap1.ClearDrawings();
            var dhandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var retVal = axMap1.DrawLabelEx(dhandle, "Воздух", 24.0, 57.0, 45.0);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            SaveSnapshot("DrawLabelEx.jpg");
        }

        [TestMethod]
        public void CheckUnicodeLabelText()
        {
            // https://mapwindow.atlassian.net/browse/MWGIS-81
            axMap1.ClearDrawings();
            var dhandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            const string labelText = "Воздух";
            const double rotation = 45.0;
            var retVal = axMap1.DrawLabelEx(dhandle, labelText, 24.0, 57.0, rotation);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            var labels = axMap1.get_DrawingLabels(dhandle);
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
            SaveSnapshot("SaveSnapshot2.jpg");
            SaveSnapshot("SaveSnapshot2", true);
        }

        private void SaveSnapshot(string baseName, bool shouldFail = false)
        {
            Application.DoEvents();
            var filename = Path.Combine(Path.GetTempPath(), baseName);
            if (File.Exists(filename)) File.Delete(filename);

            var img = axMap1.SnapShot2(0, axMap1.CurrentZoom, 1000);
            Assert.IsNotNull(img, "Snapshot is null");
            var retVal = img.Save(filename);
            if (!shouldFail)
            {
                Assert.IsTrue(retVal, "Snapshot could not be saved: " + img.ErrorMsg[img.LastErrorCode]);
                Assert.IsTrue(File.Exists(filename), "The file doesn't exists.");
                DebugMsg(filename);
            }
            else
            {
                DebugMsg("Expected error: " + img.ErrorMsg[img.LastErrorCode]);
                Assert.IsFalse(retVal, "Image could be saved. This is unexpected.");
            }
        }
        [TestMethod]
        public void CaptureSnapshot()
        {
            // clear map
            axMap1.ClearDrawings();
            axMap1.RemoveAllLayers();
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
                var img = axMap1.SnapShot(axMap1.Extents);
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
            axMap1.AddLayer(sf, true);
            axMap1.ZoomToShape(0, 0);
            axMap1.ZoomOut(0.5);
            axMap1.ZoomToTileLevel(axMap1.Tiles.CurrentZoom);
        }

        private void DebugMsg(string msg)
        {
            // Debug.WriteLine(msg);
            Console.WriteLine(msg);
        }

    }
}

