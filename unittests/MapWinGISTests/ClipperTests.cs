using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class ClipperTests
    {
        private static GlobalSettings _settings;

        public ClipperTests()
        {
            _settings = new GlobalSettings
            {
                ClipperGcsMultiplicationFactor = 10000000.0,
                MinAreaToPerimeterRatio = 0.0001, // Default value
                MinPolygonArea = 0.01, // Important, default is 1.0
                ShapeOutputValidationMode = tkShapeValidationMode.TryFixProceedOnFailure
            };
            Helper.DebugMsg("Test was run at " + DateTime.Now);
        }

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
            var tempFolder = Helper.WorkingFolder("DissolveLargefile");
            const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FishnetClipped_fix.shp";
            var sfOutput = Path.Combine(tempFolder, $"FishnetDissolved-{geometryEngine.ToString()}.shp");
            if (File.Exists(sfOutput)) Helper.DeleteShapefile(sfOutput);
            const int fieldIndex = 5;

            var sf = Helper.OpenShapefile(sfLocation);

            sf.GeometryEngine = geometryEngine;
            Helper.DebugMsg("numShapes Input: " + sf.NumShapes);
            var stopWatch = new Stopwatch();
            stopWatch.Start();
            var sfDissolved = sf.Dissolve(fieldIndex, false);
            Helper.DebugMsg("Time it took to dissolve: " + stopWatch.Elapsed);
            stopWatch.Restart();

            Assert.IsNotNull(sfDissolved, "sfDissolved is null: " + sf.ErrorMsg[sf.LastErrorCode]);

            Helper.SaveAsShapefile(sfDissolved, sfOutput);
            Helper.DebugMsg("NumShapes: " + sfDissolved.NumShapes);
            Helper.CheckValidity(sfDissolved);
            Assert.IsFalse(sfDissolved.HasInvalidShapes(), "Result has invalid shapes");
            if (!sfDissolved.Close())
                Assert.Fail("Can't close sfDissolved Error: " + sfDissolved.ErrorMsg[sfDissolved.LastErrorCode]); ;
            if (!sf.Close())
                Assert.Fail("Can't close sf Error: " + sf.ErrorMsg[sf.LastErrorCode]); ;
        }

        [TestCategory("Dissolve"), TestMethod]
        public void OpenShapefile()
        {
            const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FishnetClipped_fix.shp";
            var sf = Helper.OpenShapefile(sfLocation);
            sf.GeometryEngine = tkGeometryEngine.engineGeos;
            Helper.DebugMsg("numShapes Input: " + sf.NumShapes);

            // Before closing this shapefile open the file again:
            var sf2 = Helper.OpenShapefile(sfLocation);
            sf2.GeometryEngine = tkGeometryEngine.engineGeos;
            Helper.DebugMsg("numShapes Input: " + sf2.NumShapes);

            if (!sf2.Close())
                Assert.Fail("Can't close sf2 Error: " + sf2.ErrorMsg[sf2.LastErrorCode]); ;
            if (!sf.Close())
                Assert.Fail("Can't close sf Error: " + sf.ErrorMsg[sf.LastErrorCode]); ;
        }

        [TestCategory("Dissolve"), TestMethod]
        public void ClipperInvalidResults()
        {
            const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FishnetClipped_invalid3.shp";
            const string sfOutput = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FishnetDissolved_invalid-clipper.shp";
            const int fieldIndex = 5;

            var sf = Helper.OpenShapefile(sfLocation);
            if (File.Exists(sfOutput)) Helper.DeleteShapefile(sfOutput);

            sf.GeometryEngine = tkGeometryEngine.engineClipper;
            Helper.DebugMsg("numShapes Input: " + sf.NumShapes);
            var stopWatch = new Stopwatch();
            stopWatch.Start();
            var sfDissolved = sf.Dissolve(fieldIndex, false);
            Helper.DebugMsg("Time it took to dissolve: " + stopWatch.Elapsed);
            stopWatch.Reset();

            Assert.IsNotNull(sfDissolved, "sfDissolved is null: " + sf.ErrorMsg[sf.LastErrorCode]);

            Helper.SaveAsShapefile(sfDissolved, sfOutput);
            Helper.DebugMsg("NumShapes: " + sfDissolved.NumShapes);

            Helper.CheckValidity(sfDissolved);
            Assert.IsFalse(sfDissolved.HasInvalidShapes(), "Result has invalid shapes");

            if (!sfDissolved.Close())
                Assert.Fail("Can't close dissolved shapefile Error: " + sfDissolved.ErrorMsg[sfDissolved.LastErrorCode]); ;
            if (!sf.Close())
                Assert.Fail("Can't close input shapefile Error: " + sf.ErrorMsg[sf.LastErrorCode]);
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

        private static void DissolveSmallfile(tkGeometryEngine geometryEngine)
        {
            var tempFolder = Helper.WorkingFolder("DissolveSmallfile");
            // const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FewPolygons.shp";
            const string sfLocation = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FishnetClipped_export2.shp";
            var sfOutput = Path.Combine(tempFolder, $"FewPolygons-Dissolved_{geometryEngine.ToString()}.shp");
            const int fieldIndex = 5;

            var sf = Helper.OpenShapefile(sfLocation);
            if (File.Exists(sfOutput)) Helper.DeleteShapefile(sfOutput);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sf.GeometryEngine = geometryEngine;
            var sfDissolved = sf.Dissolve(fieldIndex, false);
            Helper.DebugMsg("Time it took: " + stopWatch.Elapsed);
            stopWatch.Restart();
            Assert.IsNotNull(sfDissolved, "sfDissolved is null: " + sf.ErrorMsg[sf.LastErrorCode]);

            Helper.SaveAsShapefile(sfDissolved, sfOutput);
            Helper.DebugMsg("NumShapes: " + sfDissolved.NumShapes);
            Assert.AreEqual(12, sfDissolved.NumShapes, 1, "Too few shapes");

            Helper.CheckValidity(sfDissolved);
            Assert.IsFalse(sfDissolved.HasInvalidShapes(), "Result has invalid shapes");
        }

        #endregion

        #region Clip

        [TestCategory("Clip"), TestMethod]
        public void VerySmallClipClipper()
        {
            VerySmallClip(tkGeometryEngine.engineClipper);
        }

        [TestCategory("Clip"), TestMethod]
        public void VerySmallClipGeos()
        {
            VerySmallClip(tkGeometryEngine.engineGeos);
        }

        private static void VerySmallClip(tkGeometryEngine geometryEngine)
        {
            var tempFolder = Helper.WorkingFolder("VerySmallClip");

            // POLYGON ((693416.416338362 5841003.20610673,693424.331109333 5840997.77042745,693415.26280084 5840989.96669721,693403.190049054 5841000.68434421,693416.416338362 5841003.20610673))
            var sfBorder = Helper.CreateSfFromWkt(
                    "POLYGON ((693416.416338362 5841003.20610673,693424.331109333 5840997.77042745,693415.26280084 5840989.96669721,693403.190049054 5841000.68434421,693416.416338362 5841003.20610673))",
                    32631);
            Helper.SaveAsShapefile(sfBorder, Path.Combine(tempFolder, $"{DateTime.Now.Ticks} border.shp"));

            //var shpSubject = new Shape();
            //if (!shpSubject.ImportFromWKT("POLYGON ((693395.4 5840980.6,693395.4 5840995.6,693410.4 5840995.6,693410.4 5840980.6,693395.4 5840980.6))"))
            //    Assert.Fail("Could not import wkt" + shpSubject.ErrorMsg[shpSubject.LastErrorCode]);

            var sfSubject = Helper.CreateSfFromWkt(
                "POLYGON ((693395.4 5840980.6,693395.4 5840995.6,693410.4 5840995.6,693410.4 5840980.6,693395.4 5840980.6))",
                32631);

            sfSubject.GeometryEngine = geometryEngine;
            Helper.SaveAsShapefile(sfSubject, Path.Combine(tempFolder, $"{DateTime.Now.Ticks} subject.shp"));

            var sfClipped = sfSubject.Clip(false, sfBorder, false);
            Assert.IsNotNull(sfClipped, "sfClipped is null: " + sfSubject.ErrorMsg[sfSubject.LastErrorCode]);
            Assert.IsFalse(sfClipped.HasInvalidShapes(), "Output file has invalid shapes");

            Helper.SaveAsShapefile(sfClipped, Path.Combine(tempFolder, $"{DateTime.Now.Ticks} clip_FailingClipClipper.shp"));

            if (!sfSubject.Close())
                Assert.Fail("Can't close sfSubject Error: " + sfSubject.ErrorMsg[sfSubject.LastErrorCode]);
            if (!sfClipped.Close())
                Assert.Fail("Can't close sfClipped Error: " + sfClipped.ErrorMsg[sfClipped.LastErrorCode]);
        }

        [TestCategory("Clip"), TestMethod]
        public void ClipSmallfileClipper()
        {
            Clipfile(tkGeometryEngine.engineClipper, 1, 15);
        }

        [TestCategory("Clip"), TestMethod]
        public void ClipSmallfileGeos()
        {
            Clipfile(tkGeometryEngine.engineGeos, 1, 15);
        }

        [TestCategory("Clip"), TestMethod]
        public void ClipLargefileClipper()
        {
            Clipfile(tkGeometryEngine.engineClipper, 5, 0.5);
        }

        [TestCategory("Clip"), TestMethod]
        public void ClipLargefileGeos()
        {
            Clipfile(tkGeometryEngine.engineGeos, 5, 0.5);
        }

        [TestCategory("Clip"), TestMethod]
        public void CreateFishnet()
        {
            var tempFolder = Helper.WorkingFolder("CreateFishnet");
            const double multiplier = 1d;
            var sfBorder = CreateBorder(multiplier);
            Helper.DebugMsg("Size of border " + sfBorder.Shape[0].Area);
            Helper.SaveAsShapefile(sfBorder, Path.Combine(tempFolder, $"{DateTime.Now.Ticks} clip_border_{multiplier.ToString(CultureInfo.InvariantCulture)}.shp"));

            const double size = 0.5d;
            var sfFishnet = Helper.CreateFishnet(sfBorder.Extents, size, size);
            sfFishnet.GeoProjection = sfBorder.GeoProjection;
            Helper.SaveAsShapefile(sfFishnet, Path.Combine(tempFolder, $"{DateTime.Now.Ticks} clip_smallFishnet_{size.ToString(CultureInfo.InvariantCulture)}.shp"));

            if (sfBorder.HasInvalidShapes()) Helper.DebugMsg("Border has invalid shapes");
            if (sfFishnet.HasInvalidShapes()) Helper.DebugMsg("Fishnet has invalid shapes");

            if (!sfBorder.Close())
                Assert.Fail("Can't close sfBorder Error: " + sfBorder.ErrorMsg[sfBorder.LastErrorCode]);
            if (!sfFishnet.Close())
                Assert.Fail("Can't close sfClipped Error: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);
        }

        private static void Clipfile(tkGeometryEngine geometryEngine, double multiplier, double size)
        {
            if (_settings == null) Assert.Fail("Settings are not set");
            var tempFolder = Helper.WorkingFolder("Clipfile");
            var stopWatch = new Stopwatch();

            _settings.MinPolygonArea = Math.Min(_settings.MinPolygonArea, size * size / 4);

            stopWatch.Start();
            var sfBorder = CreateBorder(multiplier);
            Helper.DebugMsg("Time it took to create the border: " + stopWatch.Elapsed);
            stopWatch.Restart();

            Helper.SaveAsShapefile(sfBorder, Path.Combine(tempFolder, $"{DateTime.Now.Ticks} clip_border_{geometryEngine.ToString()}_{multiplier}.shp"));
            var sfFishnet = Helper.CreateFishnet(sfBorder.Extents, size, size);
            Helper.DebugMsg("Time it took to create the fishnet: " + stopWatch.Elapsed);
            stopWatch.Restart();

            sfFishnet.GeoProjection = sfBorder.GeoProjection;
            Helper.SaveAsShapefile(sfFishnet, Path.Combine(tempFolder, $"{DateTime.Now.Ticks} clip_smallFishnet_{geometryEngine.ToString()}_{size}.shp"));
            
            if (sfBorder.HasInvalidShapes()) Helper.DebugMsg("Border has invalid shapes");
            if (sfFishnet.HasInvalidShapes()) Helper.DebugMsg("Fishnet has invalid shapes");
            
            sfFishnet.GeometryEngine = geometryEngine;
            var sfClipped = sfFishnet.Clip(false, sfBorder, false);
            Helper.DebugMsg("Time it took to clip: " + stopWatch.Elapsed);
            stopWatch.Restart();

            Assert.IsNotNull(sfClipped, "sfClipped is null: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);

            var sfOutputFile = Path.Combine(tempFolder, $"{DateTime.Now.Ticks}  FishnetClipped_{geometryEngine.ToString()}_{multiplier}_{size}.shp");
            Helper.SaveAsShapefile(sfClipped, sfOutputFile);

            if (!sfBorder.Close())
                Assert.Fail("Can't close sfBorder Error: " + sfBorder.ErrorMsg[sfBorder.LastErrorCode]);
            if (!sfFishnet.Close())
                Assert.Fail("Can't close sfClipped Error: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);

            Helper.DebugMsg("Checking if the output file has any invalid shapes .... ");
            Assert.IsFalse(sfClipped.HasInvalidShapes(), "Output file has invalid shapes");
            if (!sfClipped.Close())
                Assert.Fail("Can't close sfClipped Error: " + sfClipped.ErrorMsg[sfClipped.LastErrorCode]);
        }

        [TestCategory("Clip"), TestMethod]
        public void ClipLargefileClipper2()
        {
            ClipLargefile(tkGeometryEngine.engineClipper);
        }

        [TestCategory("Clip"), TestMethod]
        public void ClipLargefileGeos2()
        {
            ClipLargefile(tkGeometryEngine.engineGeos);
        }

        private static void ClipLargefile(tkGeometryEngine geometryEngine)
        {
            // When not calles
            if (_settings == null) Assert.Fail("Settings are not set");

            // ogr2ogr -clipsrc "Droog-Sloefweg.WGS 84 - UTM zone 31N.shp" clipped.shp Fishnet.shp
            var tempFolder = Helper.WorkingFolder("ClipLargefile");

            const string sfFishnetFile = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\Fishnet.shp";
            const string sfBorderFile = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\border.shp";
            var sfOutputFile = Path.Combine(tempFolder, $"{DateTime.Now.Ticks} FishnetClipped_{geometryEngine.ToString()}.shp");

            var sfFishnet = Helper.OpenShapefile(sfFishnetFile, false);
            var sfBorder = Helper.OpenShapefile(sfBorderFile, false);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sfFishnet.GeometryEngine = geometryEngine;
            var sfClipped = sfFishnet.Clip(false, sfBorder, false);
            Helper.DebugMsg("Time it took to clip: " + stopWatch.Elapsed);
            stopWatch.Restart();
            Helper.DebugMsg("sfBorder.ErrorMsg: " + sfBorder.ErrorMsg[sfBorder.LastErrorCode]);
            Helper.DebugMsg("sfFishnet.ErrorMsg: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);
            Helper.DebugMsg("sfClipped.ErrorMsg: " + sfClipped.ErrorMsg[sfClipped.LastErrorCode]);
            Assert.IsNotNull(sfClipped, "sfClipped is null: " + sfClipped.ErrorMsg[sfClipped.LastErrorCode]);

            Helper.SaveAsShapefile(sfClipped, sfOutputFile);
            Helper.DebugMsg("NumShapes: " + sfClipped.NumShapes);

            if (!sfBorder.Close())
                Assert.Fail("Can't close sfBorder Error: " + sfBorder.ErrorMsg[sfBorder.LastErrorCode]);
            if (!sfFishnet.Close())
                Assert.Fail("Can't close sfClipped Error: " + sfFishnet.ErrorMsg[sfFishnet.LastErrorCode]);

            Helper.DebugMsg("Checking if the output file has any invalid shapes .... ");
            Assert.IsFalse(sfClipped.HasInvalidShapes(), "Output file has invalid shapes");
            if (!sfClipped.Close())
                Assert.Fail("Can't close sfClipped Error: " + sfClipped.ErrorMsg[sfClipped.LastErrorCode]);
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
            var tempFolder = Helper.WorkingFolder("MergeShapefile");

            const string sfInputfile = @"D:\dev\GIS-Data\Issues\MWGIS-78 Clipper\FewPolygons.shp";
            var sfOutputFile = Path.Combine(tempFolder, $"FewPolygons_Merged_{geometryEngine.ToString()}.shp");

            var sfInput = Helper.OpenShapefile(sfInputfile);
            if (File.Exists(sfOutputFile)) Helper.DeleteShapefile(sfOutputFile);

            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sfInput.GeometryEngine = geometryEngine;
            var newShape = sfInput.Shape[0];
            for (var i = 1; i < sfInput.NumShapes; i++)
            {
                // Union all shapes together:
                newShape = newShape.Clip(sfInput.Shape[i], tkClipOperation.clUnion);
            }
            Helper.DebugMsg("Time it took: " + stopWatch.Elapsed);
            stopWatch.Restart();
            Assert.IsNotNull(newShape, "newShape is null: " + newShape.ErrorMsg[newShape.LastErrorCode]);

            var sfOutput = new Shapefile();
            sfOutput.CreateNewWithShapeID(sfOutputFile, ShpfileType.SHP_POLYGON);
            var numShape = 0;
            var retVal = sfOutput.EditInsertShape(newShape, ref numShape);
            Assert.IsTrue(retVal, "Failed to add shape");
            sfOutput.StopEditingShapes();

            Assert.IsTrue(File.Exists(sfOutputFile), "Output does not exists");
            Helper.DebugMsg(sfOutputFile);

            Helper.DebugMsg("NumShapes: " + sfOutput.NumShapes);
            Assert.AreEqual(sfOutput.NumShapes, 1);

            if (!sfInput.Close())
                Assert.Fail("Can't close sfBorder Error: " + sfInput.ErrorMsg[sfInput.LastErrorCode]);
            if (!sfOutput.Close())
                Assert.Fail("Can't close sfClipped Error: " + sfOutput.ErrorMsg[sfOutput.LastErrorCode]);
        }
        #endregion

        #region SetupFiles

        [TestMethod]
        public void CreateSmallBorder()
        {
            var sf = CreateBorder();
            var tempFolder = Path.GetTempPath();
            var filename = Path.Combine(tempFolder, "smallBorder.shp");
            Helper.SaveAsShapefile(sf, filename);
        }

        [TestMethod]
        public void CreateLargeBorder()
        {
            var sf = CreateBorder(5);
            var tempFolder = Path.GetTempPath();
            var filename = Path.Combine(tempFolder, "largeBorder.shp");
            if (File.Exists(filename)) Helper.DeleteShapefile(filename);
            Helper.SaveAsShapefile(sf, filename);
        }

        private static Shapefile CreateBorder(double multiplier = 1d)
        {
            var sf = new Shapefile();
            if (!sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON))
                Assert.Fail("Can't create shapefile Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            var geoProjection = new GeoProjection();
            // WGS 84 / UTM zone 31N
            if (!geoProjection.ImportFromEPSG(32631))
                Assert.Fail("Can't ImportFromEPSG Error: " + geoProjection.ErrorMsg[geoProjection.LastErrorCode]);
            sf.GeoProjection = geoProjection;

            const double startX = 693502.4;
            const double startY = 5841019.6;

            var shp = new Shape();
            if (!shp.Create(ShpfileType.SHP_POLYGON))
                Assert.Fail("Can't create shape Error: " + shp.ErrorMsg[shp.LastErrorCode]);

            var numPoints = 0;
            if (!shp.InsertPoint(new Point { x = startX, y = startY }, ref numPoints))
                Assert.Fail($"Can't insert point with id: {numPoints} Error: {shp.ErrorMsg[shp.LastErrorCode]}");
            if (!shp.InsertPoint(new Point { x = startX - 107 * multiplier, y = startY - 12 * multiplier }, ref numPoints))
                Assert.Fail($"Can't insert point with id: {numPoints} Error: {shp.ErrorMsg[shp.LastErrorCode]}");
            if (!shp.InsertPoint(new Point { x = startX - 9 * multiplier, y = startY - 99 * multiplier }, ref numPoints))
                Assert.Fail($"Can't insert point with id: {numPoints} Error: {shp.ErrorMsg[shp.LastErrorCode]}");
            if (!shp.InsertPoint(new Point { x = startX + 11 * multiplier, y = startY - 83 * multiplier }, ref numPoints))
                Assert.Fail($"Can't insert point with id: {numPoints} Error: {shp.ErrorMsg[shp.LastErrorCode]}");
            if (!shp.InsertPoint(new Point { x = startX, y = startY }, ref numPoints))
                Assert.Fail($"Can't insert point with id: {numPoints} Error: {shp.ErrorMsg[shp.LastErrorCode]}");

            if (!shp.IsValid)
                Assert.Fail("Shape is invalid: " + shp.IsValidReason);

            var numShapes = 0;
            if (!sf.EditInsertShape(shp, ref numShapes))
                Assert.Fail("Can't insert shape Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            Assert.IsFalse(sf.HasInvalidShapes(), "Created border file has invalid shapes");
            return sf;
        }

        #endregion
    }
}
