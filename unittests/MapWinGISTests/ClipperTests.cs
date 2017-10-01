using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class ClipperTests
    {
        #region Dissolve
        [TestCategory("Dissolve"), TestMethod]
        public void DissolveLargefileClipper()
        {
            DissolveLargefile(tkGeometryEngine.engineClipper);
        }

        [TestCategory("Dissolve"), TestMethod]
        public void DissolveLargefileGeos()
        {
            DissolveLargefile(tkGeometryEngine.engineGeos);
        }

        public void DissolveLargefile(tkGeometryEngine geometryEngine)
        {
            var tempFolder = Path.GetTempPath();
            const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FishnetClipped.shp";
            var sfOutput = Path.Combine(tempFolder, $"FishnetDissolved_{geometryEngine.ToString()}.shp");
            const int fieldIndex = 5;

            var sf = new Shapefile();
            if (!sf.Open(sfLocation))
                Assert.Fail("Can't open " + sfLocation + " Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            if (File.Exists(sfOutput)) Helper.DeleteShapefile(sfOutput);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sf.FastMode = true;
            sf.GeometryEngine = geometryEngine;
            var sfDissolved = sf.Dissolve(fieldIndex, false);
            stopWatch.Stop();
            DebugMsg("Time it took: " + stopWatch.Elapsed);
            Assert.IsNotNull(sfDissolved, "sfDissolved is null: " + sf.ErrorMsg[sf.LastErrorCode]);

            var retVal = sfDissolved.SaveAs(sfOutput);
            Assert.IsTrue(retVal, "Failed to save file");
            Assert.IsTrue(File.Exists(sfOutput), "Output does not exists");
            DebugMsg(sfOutput);

            DebugMsg("NumShapes: " + sfDissolved.NumShapes);
            Assert.AreEqual(28, sfDissolved.NumShapes, 1, "Too few shapes");
        }

        [TestCategory("Dissolve"), TestMethod]
        public void DissolveSmallfileClipper()
        {
            DissolveSmallfile(tkGeometryEngine.engineClipper);
        }

        [TestCategory("Dissolve"), TestMethod]
        public void DissolveSmallfileGeos()
        {
            DissolveSmallfile(tkGeometryEngine.engineGeos);
        }
        public void DissolveSmallfile(tkGeometryEngine geometryEngine)
        {
            var tempFolder = Path.GetTempPath();
            // const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FewPolygons.shp";
            const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FishnetClipped_export2.shp";
            var sfOutput = Path.Combine(tempFolder, $"FewPolygons-Dissolved_{geometryEngine.ToString()}.shp");
            const int fieldIndex = 5;

            var sf = new Shapefile();
            if (!sf.Open(sfLocation))
                Assert.Fail("Can't open " + sfLocation + " Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            if (File.Exists(sfOutput)) Helper.DeleteShapefile(sfOutput);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sf.FastMode = true;
            sf.GeometryEngine = geometryEngine;
            var sfDissolved = sf.Dissolve(fieldIndex, false);
            stopWatch.Stop();
            DebugMsg("Time it took: " + stopWatch.Elapsed);
            Assert.IsNotNull(sfDissolved, "sfDissolved is null: " + sf.ErrorMsg[sf.LastErrorCode]);

            var retVal = sfDissolved.SaveAs(sfOutput);
            Assert.IsTrue(retVal, "Failed to save file");
            Assert.IsTrue(File.Exists(sfOutput), "Output does not exists");
            DebugMsg(sfOutput);

            DebugMsg("NumShapes: " + sfDissolved.NumShapes);
            Assert.AreEqual(25, sfDissolved.NumShapes, 1, "Too few shapes");
        }

        #endregion

        #region Clip
        [TestCategory("Clip"), TestMethod]
        public void ClipLargefileClipper()
        {
            ClipLargefile(tkGeometryEngine.engineClipper);
        }

        [TestCategory("Clip"), TestMethod]
        public void ClipLargefileGeos()
        {
            ClipLargefile(tkGeometryEngine.engineGeos);
        }

        private static void ClipLargefile(tkGeometryEngine geometryEngine)
        {
            var tempFolder = Path.GetTempPath();
            const string sfFishnetFile = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\Fishnet.shp";
            const string sfBorderFile = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\border.shp";
            var sfOutputFile = Path.Combine(tempFolder, $"FishnetClipped_{geometryEngine.ToString()}.shp");

            var sfFishnet = new Shapefile();
            if (!sfFishnet.Open(sfFishnetFile))
                Assert.Fail("Can't open " + sfFishnet + " Error: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);

            var sfBorder = new Shapefile();
            if (!sfBorder.Open(sfBorderFile))
                Assert.Fail("Can't open " + sfBorderFile + " Error: " + sfBorder.ErrorMsg[sfBorder.LastErrorCode]);

            if (File.Exists(sfOutputFile)) Helper.DeleteShapefile(sfOutputFile);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sfFishnet.FastMode = true;
            sfFishnet.GeometryEngine = geometryEngine;
            var sfClipped = sfFishnet.Clip(false, sfBorder, false);
            stopWatch.Stop();
            DebugMsg("Time it took: " + stopWatch.Elapsed);
            Assert.IsNotNull(sfClipped, "sfClipped is null: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);

            var retVal = sfClipped.SaveAs(sfOutputFile);
            Assert.IsTrue(retVal, "Failed to save file");
            Assert.IsTrue(File.Exists(sfOutputFile), "Output does not exists");
            DebugMsg(sfOutputFile);

            DebugMsg("NumShapes: " + sfClipped.NumShapes);
            Assert.IsTrue(sfClipped.NumShapes > 1000, "Too few shapes");
        }
        #endregion

        #region Merge
        [TestCategory("Merge"), TestMethod]
        public void MergeShapefileGeos()
        {
            MergeShapefile(tkGeometryEngine.engineGeos);
        }

        [TestCategory("Merge"), TestMethod]
        public void MergeShapefileClipper()
        {
            MergeShapefile(tkGeometryEngine.engineClipper);
        }

        private static void MergeShapefile(tkGeometryEngine geometryEngine)
        {
            var tempFolder = Path.GetTempPath();
            const string sfInputfile = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FewPolygons.shp";
            var sfOutputFile = Path.Combine(tempFolder, $"FewPolygons_Merged_{geometryEngine.ToString()}.shp");

            var sfInput = new Shapefile();
            if (!sfInput.Open(sfInputfile))
                Assert.Fail("Can't open " + sfInput + " Error: " + sfInput.ErrorMsg[sfInput.LastErrorCode]);

            if (File.Exists(sfOutputFile)) Helper.DeleteShapefile(sfOutputFile);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sfInput.FastMode = true;
            sfInput.GeometryEngine = geometryEngine;
            var newShape = sfInput.Shape[0];
            for (var i = 1; i < sfInput.NumShapes; i++)
            {
                // Union all shapes together:
                newShape = newShape.Clip(sfInput.Shape[i], tkClipOperation.clUnion);
            }
            stopWatch.Stop();
            DebugMsg("Time it took: " + stopWatch.Elapsed);
            Assert.IsNotNull(newShape, "newShape is null: " + newShape.ErrorMsg[newShape.LastErrorCode]);

            var sfOutput = new Shapefile();
            sfOutput.CreateNewWithShapeID(sfOutputFile, ShpfileType.SHP_POLYGON);
            var numShape = 0;
            var retVal = sfOutput.EditInsertShape(newShape, ref numShape);
            Assert.IsTrue(retVal, "Failed to add shape");
            sfOutput.StopEditingShapes();

            Assert.IsTrue(File.Exists(sfOutputFile), "Output does not exists");
            DebugMsg(sfOutputFile);

            DebugMsg("NumShapes: " + sfOutput.NumShapes);
            Assert.AreEqual(sfOutput.NumShapes, 1);
        }
        #endregion

        private static void DebugMsg(string msg)
        {
            Debug.WriteLine(msg);
            Console.WriteLine(msg);
        }

    }
}
