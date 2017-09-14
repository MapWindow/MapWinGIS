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
        public void DrawLabel()
        {
            Debug.WriteLine("num layers: " + axMap1.NumLayers);
            axMap1.ClearDrawings();
            var dhandle = axMap1.NewDrawing(tkDrawReferenceList.dlSpatiallyReferencedList);
            var retVal = axMap1.DrawLabel("label", 24.0, 57.0, 0.0);
            Assert.AreEqual(dhandle, retVal, "Unexpected handle");
            SaveSnapshot("DrawLabel.jpg");
        }

        [TestMethod]
        public void SaveSnapshot2()
        {
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

        private void CreateLayer()
        {
            var sf = new Shapefile();
            sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);

            var shp = new Shape();
            shp.Create(sf.ShapefileType);
            shp.AddPoint(23, 56);
            shp.AddPoint(23, 58);
            shp.AddPoint(25, 58);
            shp.AddPoint(25, 56);
            shp.AddPoint(23, 56);
            Debug.WriteLine("IsValid: " + shp.IsValid);

            sf.EditAddShape(shp);
            axMap1.AddLayer(sf, true);
            axMap1.ZoomToShape(0, 0);
            axMap1.ZoomOut(0.5);
        }

        private void DebugMsg(string msg)
        {
            Debug.WriteLine(msg);
            Console.WriteLine(msg);
        }
    }
}

