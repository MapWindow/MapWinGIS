using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Threading;
using AxMapWinGIS;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class ShapefileTests : ICallback
    {
        private AxMap _axMap1;

        [TestInitialize]
        public void Init()
        {
            Helper.DebugMsg("Test was run at " + DateTime.Now);
        }

        [TestMethod]
        public void OpenShapefile()
        {
            Helper.OpenShapefile(@"D:\dev\MapWindow\MapWinGIS\git\MapWinGisTests-net6\TestData\Issue-216.shp", this);
        }

        [TestMethod]
        public void SpatialIndexTest()
        {
            // Create shapefile:
            var sfPolygon = new Shapefile();
            Assert.IsNotNull(sfPolygon, "Could not initialize Shapefile object");
            var retVal = sfPolygon.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);
            Assert.IsTrue(retVal, "sf.CreateNewWithShapeID() failed");

            // Add shape to shapefile:
            var shp = new Shape();
            retVal = shp.ImportFromWKT("POLYGON((330695.973322992 5914896.16305817, 330711.986129861 5914867.19586245, 330713.350435287 5914867.56644015, 330716.510827627 5914862.28973662, 330715.632568651 5914860.60107999, 330652.234582712 5914803.80510632, 330553.749382483 5914715.80328169, 330551.979355848 5914714.83347535, 330549.911988583 5914715.86502807, 330545.027807355 5914724.05916443, 330544.592985976 5914725.93531509, 330544.30963704 5914726.72754692, 330543.612620707 5914726.14904553, 330543.271515787 5914727.06633931, 330542.234090059 5914729.85597723, 330542.959654761 5914730.50411962, 330530.319252794 5914765.86064153, 330505.294840402 5914836.7930124, 330471.411812074 5914931.61558331, 330486.074748666 5914941.33795239, 330585.983154737 5915010.32749106, 330618.427962455 5915031.20447119, 330653.234601917 5914970.37328093, 330695.973322992 5914896.16305817))");
            Assert.IsTrue(retVal, "shp.ImportFromWKT() failed");
            // Add shape to shapefile:
            sfPolygon.EditAddShape(shp);

            // Test, shoud return false and error:
            retVal = sfPolygon.RemoveSpatialIndex();
            Assert.IsFalse(retVal);
            Assert.AreEqual("The method isn't applicable to the in-memory object", sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);

            // Save shapefile:
            var sfFileLocation = Path.ChangeExtension(Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(Path.GetRandomFileName())), ".shp");
            sfPolygon.SaveAsEx(sfFileLocation, true, false);
            Console.WriteLine(sfFileLocation);

            // Test, shoud return false and error:
            retVal = sfPolygon.RemoveSpatialIndex();
            Assert.IsFalse(retVal);
            Assert.AreEqual("The method isn't applicable to the in-memory object", sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);

            // Create index::
            retVal = sfPolygon.CreateSpatialIndex();
            Assert.IsTrue(retVal, "CreateSpatialIndex failed");

            // Test, shoud return false and error:
            retVal = sfPolygon.RemoveSpatialIndex();
            Assert.IsTrue(retVal, "RemoveSpatialIndex failed: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);


            // Test again with larger extent, should return true:
            //sfPolygon.Extents.GetBounds(out var xMin, out var yMin, out var zMin, out var xMax, out var yMax, out var zMax);
            //var enlargedExtent = new Extents();
            //const int enlargeValue = -100;
            //enlargedExtent.SetBounds(xMin - enlargeValue, yMin - enlargeValue, zMin, xMax + enlargeValue, yMax + enlargeValue, zMax);
            //retVal = sfPolygon.CanUseSpatialIndex[enlargedExtent];
            //Assert.IsTrue(retVal,"CanUseSpatialIndex failed: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);
        }

        [TestMethod]
        public void SaveShapefileTest()
        {
            var tempFolder = Path.GetTempPath();
            var tempFilename = Path.Combine(tempFolder, "CreateShapefileTest.shp");
            Helper.DeleteShapefile(tempFilename);

            bool result;
            // Create shapefile
            var sf = new Shapefile { GlobalCallback = this };
            try
            {
                result = sf.CreateNewWithShapeID(tempFilename, ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create shapefile");

                Assert.IsTrue(sf.EditingShapes, "Shapefile is not in edit shapes mode");
                Assert.IsTrue(sf.EditingTable, "Shapefile is not in edit table mode");

                // Add fields:
                var fieldIndex = sf.EditAddField("date", FieldType.STRING_FIELD, 0, 50);
                Assert.AreEqual(1, fieldIndex, "Could not add field");
                fieldIndex = sf.EditAddField("remarks", FieldType.STRING_FIELD, 0, 100);
                Assert.AreEqual(2, fieldIndex, "Could not add field");
                fieldIndex = sf.EditAddField("amount", FieldType.INTEGER_FIELD, 0, 3);
                Assert.AreEqual(3, fieldIndex, "Could not add field");
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");

                result = sf.Save();
                Assert.IsTrue(result, "Could not save shapefile");
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");
            }
            finally
            {
                // Close the shapefile:
                result = sf.Close();
                Assert.IsTrue(result, "Could not close shapefile");
            }
        }

        [TestMethod]
        public void CreateShapefileTest()
        {
            var tempFolder = Path.GetTempPath();
            var tempFilename = Path.Combine(tempFolder, "CreateShapefileTest.shp");
            Helper.DeleteShapefile(tempFilename);

            bool result;
            // Create shapefile
            var sf = new Shapefile { GlobalCallback = this };
            try
            {
                result = sf.CreateNewWithShapeID(tempFilename, ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create shapefile");

                Assert.IsTrue(sf.EditingShapes, "Shapefile is not in edit shapes mode");
                Assert.IsTrue(sf.EditingTable, "Shapefile is not in edit table mode");

                // Add fields:
                Assert.IsTrue(sf.Table.EditingTable, "Table is not in edit table mode");
                var fieldIndex = sf.Table.EditAddField("date", FieldType.STRING_FIELD, 0, 50);
                Assert.AreEqual(1, fieldIndex, "Could not add field");
                fieldIndex = sf.Table.EditAddField("remarks", FieldType.STRING_FIELD, 0, 100);
                Assert.AreEqual(2, fieldIndex, "Could not add field");
                fieldIndex = sf.Table.EditAddField("amount", FieldType.INTEGER_FIELD, 0, 3);
                Assert.AreEqual(3, fieldIndex, "Could not add field");
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");

                result = sf.Table.Save();
                Assert.IsTrue(result, "Could not save table");
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");

                // Create shape:
                var shp = new Shape();
                result = shp.Create(sf.ShapefileType);
                Assert.IsTrue(result, "Could not create shape");
                // Create point:
                var pnt = new Point
                {
                    x = 200,
                    y = 200
                };
                // Add point:
                var pointIndex = shp.NumPoints;
                result = shp.InsertPoint(pnt, ref pointIndex);
                Assert.IsTrue(result, "Could not insert point");
                var shapeIndex = sf.NumShapes;
                result = sf.EditInsertShape(shp, ref shapeIndex);
                Assert.IsTrue(result, "Could not insert shape");
                // Update attributes:
                sf.EditCellValue(fieldIndex, shapeIndex, 3);

                result = sf.Save();
                Assert.IsTrue(result, "Could not save shapefile");

                // Check shapefile:
                Assert.AreEqual(shapeIndex + 1, sf.NumShapes, "Number of shapes are incorrect");
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");

                // Close shapefile and re-open:
                result = sf.Close();
                Assert.IsTrue(result, "Could not close shapefile");

                result = sf.Open(tempFilename);
                Assert.IsTrue(result, "Could not open shapefile");
                // Check shapefile:
                Assert.AreEqual(shapeIndex + 1, sf.NumShapes, "Number of shapes are incorrect");
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");

            }
            finally
            {
                // Close the shapefile:
                result = sf.Close();
                Assert.IsTrue(result, "Could not close shapefile");
            }
        }

        [TestMethod]
        public void ShapefileDataTest()
        {
            var tempFilename = Path.Combine(Helper.WorkingFolder("ShapefileDataTest"), "ShapefileDataTest.shp");
            Helper.DeleteShapefile(tempFilename);

            bool result;
            // Create shapefile
            var sf = new Shapefile { GlobalCallback = this };
            try
            {
                result = sf.CreateNewWithShapeID(tempFilename, ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create shapefile");

                Assert.IsTrue(sf.EditingShapes, "Shapefile is not in edit shapes mode");
                Assert.IsTrue(sf.EditingTable, "Shapefile is not in edit table mode");

                // Add fields of each data type:
                var fieldIndex = sf.EditAddField("intField", FieldType.INTEGER_FIELD, 0, 10);
                Assert.AreEqual(1, fieldIndex, "Could not add Integer field");
                var width = sf.Field[fieldIndex].Width;
                Assert.AreEqual(9, width, "Integer field did not shrink to 9 digits");
                fieldIndex = sf.EditAddField("dateField", FieldType.DATE_FIELD, 0, 6);
                Assert.AreEqual(2, fieldIndex, "Could not add Date field");
                width = sf.Field[fieldIndex].Width;
                Assert.AreEqual(8, width, "Date field did not expand to 8 digits");
                fieldIndex = sf.EditAddField("boolField", FieldType.BOOLEAN_FIELD, 0, 3);
                Assert.AreEqual(3, fieldIndex, "Could not add Boolean field");
                width = sf.Field[fieldIndex].Width;
                Assert.AreEqual(1, width, "Boolean field did not shrink to 1 character");
                //
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");

                result = sf.Save();
                Assert.IsTrue(result, "Could not save shapefile");
                Assert.AreEqual(fieldIndex + 1, sf.NumFields, "Number of fields are incorrect");

                // Create shape:
                var shp = new Shape();
                result = shp.Create(ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create point shape");
                var idx = sf.EditAddShape(shp);
                // Add data:
                result = sf.EditCellValue(sf.FieldIndexByName["intField"], idx, 99);
                Assert.IsTrue(result, "Could not edit intField");
                var dt = DateTime.Now;
                result = sf.EditCellValue(sf.FieldIndexByName["dateField"], idx, dt);
                Assert.IsTrue(result, "Could not edit dateField");
                result = sf.EditCellValue(sf.FieldIndexByName["boolField"], idx, true);
                Assert.IsTrue(result, "Could not edit boolField");

                result = sf.StopEditingShapes();
                Assert.IsTrue(result, "Could not stop editing shapefile");

                // Read back data:
                for (idx = 0; idx < sf.NumShapes; idx++)
                {
                    var iField = (int)sf.CellValue[sf.FieldIndexByName["intField"], idx];
                    Assert.AreEqual(iField, 99, "intField value of 99 was not returned");
                    var dField = (DateTime)sf.CellValue[sf.FieldIndexByName["dateField"], idx];
                    Assert.IsTrue(DateTime.Now.DayOfYear.Equals(dField.DayOfYear), "dateField value of Now was not returned");
                    var bField = (bool)sf.CellValue[sf.FieldIndexByName["boolField"], idx];
                    Assert.AreEqual(bField, true, "boolField value of True was not returned");
                }
            }
            finally
            {
                // Close the shapefile:
                result = sf.Close();
                Assert.IsTrue(result, "Could not close shapefile");
            }

            // although the default setting, indicate intent to interpret Y/N OGR String fields as Boolean
            var gs = new GlobalSettings();
            gs.OgrInterpretYNStringAsBoolean = true;  // setting to false results in exception reading boolField below

            // open as OGRLayer
            var _datasource = new OgrDatasource { GlobalCallback = this };

            if (_datasource.Open(tempFilename)) // "ESRI Shapefile:" + 
            {
                // read layer through OGR library
                var ogrLayer = _datasource.GetLayer(0);
                sf = ogrLayer.GetBuffer();
                for (var idx = 0; idx < sf.NumShapes; idx++)
                {
                    var iField = (int)sf.CellValue[sf.FieldIndexByName["intField"], idx];
                    Assert.AreEqual(iField, 99, "intField value of 99 was not returned");
                    var dField = (DateTime)sf.CellValue[sf.FieldIndexByName["dateField"], idx];
                    Assert.IsTrue(DateTime.Now.DayOfYear.Equals(((DateTime)dField).DayOfYear), "dateField value of Now was not returned");
                    var bField = (bool)sf.CellValue[sf.FieldIndexByName["boolField"], idx];
                    Assert.AreEqual(bField, true, "boolField value of True was not returned");
                }
                sf.Close();
            }

            // open and read a Shapefile created by ESRI MapObjects, including a Boolean and Date field
            // table has a Boolean 'Inspected' field, and a Date 'InspDate' field
            const string esriShapefilePath = @"Issues\MWGIS-48-68\EsriShapefile.shp";
            Assert.IsTrue(sf.Open(esriShapefilePath, this));
            for (var fld = 0; fld < sf.NumFields; fld++)
            {
                Console.WriteLine($"Field({fld}): Name = '{sf.Field[fld].Name}', Fieldtype = {sf.Field[fld].Type}");
            }

            for (var idx = 0; idx < sf.NumShapes; idx++)
            {
                // read 'Inspected' value as object
                var inspected = sf.CellValue[sf.FieldIndexByName["Inspected"], idx];
                // verify that it's a bool
                Assert.IsTrue(inspected is bool);
                // watch for Inspected rows (there aren't many)
                if ((bool)inspected != true) continue;

                // read 'InspDate' value as object
                var dt = sf.CellValue[sf.FieldIndexByName["InspDate"], idx];
                // verify that it's a Date
                Assert.IsTrue(dt is DateTime);
                Console.WriteLine($"idx = {idx}, Inspected = true, Inspection Date = {(DateTime)dt}");
            }
            sf.Close();
        }

        /// <summary>
        /// Checks the null value table data.
        /// </summary>
        /// <remarks>https://mapwindow.atlassian.net/projects/CORE/issues/CORE-177</remarks>
        [TestMethod]
        public void CheckNullValueTableData()
        {
            bool result;
            var sf = new Shapefile { GlobalCallback = this };

            try
            {
                // Create in-memory shapefile
                result = sf.CreateNewWithShapeID(string.Empty, ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create shapefile");

                // Add fields:
                var fieldIndex = sf.Table.EditAddField("string", FieldType.STRING_FIELD, 0, 50);
                Assert.AreEqual(1, fieldIndex, "Could not add field");

                fieldIndex = sf.Table.EditAddField("integer", FieldType.INTEGER_FIELD, 0, 50);
                Assert.AreEqual(2, fieldIndex, "Could not add field");

                fieldIndex = sf.Table.EditAddField("double", FieldType.DOUBLE_FIELD, 2, 50);
                Assert.AreEqual(3, fieldIndex, "Could not add field");

                fieldIndex = sf.Table.EditAddField("double", FieldType.DOUBLE_FIELD, 0, 50);
                Assert.AreEqual(-1, fieldIndex, "Field was added. This is not correct.");

                Assert.AreEqual(4, sf.NumFields, "Wrong number of fields");

                // Create shape:
                var shp = new Shape();
                result = shp.Create(ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create point shape");
                // Create point:
                var pnt = new Point
                {
                    x = 200,
                    y = 200
                };
                // Add point:
                var pointIndex = shp.NumPoints;
                result = shp.InsertPoint(pnt, ref pointIndex);
                Assert.IsTrue(result, "Could not insert point");
                var shapeIndex = sf.NumShapes;
                result = sf.EditInsertShape(shp, ref shapeIndex);
                Assert.IsTrue(result, "Could not insert shape");

                // Read attribute data, skip the first because that is the ID which always has a value
                for (var i = 1; i < sf.NumFields; i++)
                {
                    var value = sf.CellValue[i, 0];
                    var field = sf.Field[i];
                    Console.WriteLine($"Is the value of fieldId {i} NULL: {value == null} Type of field is {field.Type}");
                    // Assert.IsNull(value, $"Value with fieldId {i} is not null, but is '{value}'");
                }
            }
            finally
            {
                // Close the shapefile:
                result = sf.Close();
                Assert.IsTrue(result, "Could not close shapefile");
            }

        }

        /// <summary>
        /// Adds the field.
        /// </summary>
        /// <remarks>https://mapwindow.atlassian.net/browse/MWGIS-55</remarks>
        [TestMethod]
        public void AddField()
        {
            bool result;
            var sf = new Shapefile { GlobalCallback = this };

            try
            {
                // Create in-memory shapefile
                result = sf.CreateNewWithShapeID(string.Empty, ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create shapefile");

                Assert.IsTrue(sf.EditingShapes, "Shapefile is not in edit shapes mode");
                Assert.IsTrue(sf.EditingTable, "Shapefile is not in edit table mode");

                // Add fields:
                Assert.IsTrue(sf.Table.EditingTable, "Table is not in edit table mode");
                // This should work:
                var fieldIndex = sf.Table.EditAddField("date", FieldType.STRING_FIELD, 0, 50);
                Assert.AreEqual(sf.NumFields - 1, fieldIndex, "Could not add string field");
                // This should work:
                fieldIndex = sf.Table.EditAddField("double", FieldType.DOUBLE_FIELD, 10, 20);
                Assert.AreEqual(sf.NumFields - 1, fieldIndex, "Could not add double field");

                // This should fail, because width cannot be 0:
                fieldIndex = sf.Table.EditAddField("date", FieldType.STRING_FIELD, 50, 0);
                Assert.AreEqual(-1, fieldIndex, "Field is not added but -1 is not returned. ");
                Console.WriteLine("Expected error: " + sf.Table.ErrorMsg[sf.Table.LastErrorCode]);

                // This should fail, because precsion cannot be 0 when type is double:
                fieldIndex = sf.Table.EditAddField("date", FieldType.DOUBLE_FIELD, 0, 20);
                Assert.AreEqual(-1, fieldIndex, "Field is not added but -1 is not returned. ");
                Console.WriteLine("Expected error: " + sf.Table.ErrorMsg[sf.Table.LastErrorCode]);
            }
            finally
            {
                // Close the shapefile:
                result = sf.Close();
                Assert.IsTrue(result, "Could not close shapefile");
            }
        }

        [TestMethod]
        public void FixUpShapes()
        {
            // MWGIS-90
            // Open shapefile:
            var sfInvalid = new Shapefile { GlobalCallback = this };
            Shapefile sfFixed = null;
            try
            {
                var result = sfInvalid.Open(@"sf\invalid.shp");
                Assert.IsTrue(result, "Could not open shapefile");

                Assert.IsTrue(sfInvalid.HasInvalidShapes(), "Shapefile has no invalid shapes");
                Helper.PrintExtents(sfInvalid.Extents);

                result = sfInvalid.FixUpShapes(out sfFixed);
                Assert.IsTrue(result, "Could not fix shapefile");
                Assert.IsFalse(sfFixed.HasInvalidShapes(), "Returning shapefile has invalid shapes");

                Assert.AreEqual(sfInvalid.NumShapes, sfFixed.NumShapes, "Number of shapes are not equal");
                Helper.PrintExtents(sfFixed.Extents);
            }
            finally
            {
                sfInvalid.Close();
                sfFixed?.Close();
            }
        }

        // Missing data: [TestMethod]
        /*
        private void Reproject2281Test()
        {
            var sf = new Shapefile {GlobalCallback = this};
            const string filename = @"Issues/MWGIS-91/utah_central_arcs.shp"; // In NAD83 / Utah Central (ft), EPSG:2281
            if (!sf.Open(filename)) Assert.Fail("Could not open shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);
            Assert.IsTrue(sf.NumShapes == 1, "Unexpected number of shapes in " + filename);
            Console.WriteLine(sf.GeoProjection.ProjectionName);
            Helper.PrintExtents(sf.Extents);

            var proj = new GeoProjection();
            proj.ImportFromEPSG(32612); // WGS 84 / UTM zone 12N
            var numShps = 0;
            var reprojectedSf = sf.Reproject(proj, numShps);
            Assert.IsTrue(numShps > 0, "Nothing is reprojected. Error: " + sf.ErrorMsg[sf.LastErrorCode]);
            Assert.IsNotNull(reprojectedSf, "reprojectedSf == null");
            Assert.AreEqual(sf.NumShapes, reprojectedSf.NumShapes);
            Helper.PrintExtents(reprojectedSf.Extents);

            Helper.SaveAsShapefile(reprojectedSf, Path.ChangeExtension(filename, ".WGS84-UTM12N.shp"));
        }
        */

        [TestMethod]
        public void Reproject2280Test()
        {
            var sf = new Shapefile { GlobalCallback = this };
            const string filename = @"Issues/MWGIS-91/utah_north_arcs.shp"; // In NAD83 / Utah North (ft), EPSG:2280
            if (!sf.Open(filename)) Assert.Fail("Could not open shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);
            Assert.IsTrue(sf.NumShapes == 1, "Unexpected number of shapes in " + filename);
            Console.WriteLine(sf.GeoProjection.ProjectionName);
            Helper.PrintExtents(sf.Extents);

            var proj = new GeoProjection();
            proj.ImportFromEPSG(32612); // WGS 84 / UTM zone 12N
            var numShps = 0;
            var reprojectedSf = sf.Reproject(proj, ref numShps);
            Assert.IsTrue(numShps > 0, "Nothing is reprojected. Error: " + sf.ErrorMsg[sf.LastErrorCode]);
            Assert.IsNotNull(reprojectedSf, "reprojectedSf == null. Error: " + sf.ErrorMsg[sf.LastErrorCode]);
            Assert.AreEqual(sf.NumShapes, reprojectedSf.NumShapes);
            Helper.PrintExtents(reprojectedSf.Extents);

            Helper.SaveAsShapefile(reprojectedSf, Path.Combine(Path.GetTempPath(), "Reproject2280Test.shp"));

            Assert.AreNotEqual(Math.Round(sf.Extents.xMin, MidpointRounding.AwayFromZero),
                Math.Round(reprojectedSf.Extents.xMin, MidpointRounding.AwayFromZero), "xMin are the same, no projection has happened.");
        }

        [TestMethod]
        public void ClipByMapExtents()
        {
            _axMap1 = Helper.GetAxMap(true);

            // Start with fresh map:
            _axMap1.Clear();
            _axMap1.TileProvider = tkTileProvider.ProviderNone;
            _axMap1.ZoomBehavior = tkZoomBehavior.zbDefault;
            _axMap1.KnownExtents = tkKnownExtents.keWorld;

            // Load shapefile:
            const string filename = @"sf/clipByMapExtents.shp";
            var handle = _axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosVectorLayer, true);
            Assert.IsTrue(handle != -1, "Could not load layer: " + _axMap1.get_ErrorMsg(_axMap1.LastErrorCode));
            Thread.Sleep(10000);
            var sf = _axMap1.get_Shapefile(handle);
            Debug.WriteLine($"Shapefile has {sf.NumShapes} shapes");

            // Save the current view as an image:
            Helper.SaveSnapshot(_axMap1, "AfterLoading.png", _axMap1.Extents);

            // Zoom in:
            Debug.WriteLine("Zoom in");
            _axMap1.ZoomIn(0.3);
            _axMap1.ZoomIn(0.3);
            // New snapshot
            Helper.SaveSnapshot(_axMap1, "AfterZooming.png", _axMap1.Extents);
            Thread.Sleep(10000);

            // Make shape from map extents:
            var clipShape = _axMap1.Extents.ToShape();
            Assert.IsNotNull(clipShape, "Could not make shape from map extents");
            Debug.WriteLine(clipShape.NumPoints);
            // Make in-memory shapefile from shape:
            var sfClip = new Shapefile();
            if (!sfClip.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON))
                Assert.Fail("Can't create shapefile. Error: " + sfClip.ErrorMsg[sfClip.LastErrorCode]);
            // Set projection:
            sfClip.GeoProjection = _axMap1.GeoProjection.Clone();
            var shpIndex = sfClip.EditAddShape(clipShape);
            Assert.IsTrue(shpIndex != -1, "Could not add shape: " + sfClip.ErrorMsg[sfClip.LastErrorCode]);

            // Clip:
            var sfResult = sf.Clip(false, sfClip, false);
            Debug.WriteLine(sfResult.NumShapes);

            Assert.AreNotEqual(sf.NumShapes, sfResult.NumShapes, "No shapes were clipped.");

            _axMap1.AddLayer(sfResult, true);
            Thread.Sleep(20000);
        }

        [TestMethod]
        public void SnapShotTest()
        {
            _axMap1 = Helper.GetAxMap();

            // Process the list of files found in the directory.
            var fileEntries = Directory.GetFiles("sf", "*.shp");
            foreach (var sfName in fileEntries)
            {
                // Start with fresh map:
                _axMap1.Clear();
                _axMap1.TileProvider = tkTileProvider.ProviderNone;
                _axMap1.ZoomBehavior = tkZoomBehavior.zbDefault;
                _axMap1.KnownExtents = tkKnownExtents.keWorld;

                var baseName = Path.GetFileNameWithoutExtension(sfName);
                // if (baseName == "onepoint") continue;

                Helper.DebugMsg("Working on " + baseName);

                // Load shapefile:
                var handle = _axMap1.AddLayerFromFilename(sfName, tkFileOpenStrategy.fosVectorLayer, true);
                Assert.IsTrue(handle != -1, "Could not load layer: " + _axMap1.get_ErrorMsg(_axMap1.LastErrorCode));

                Helper.DebugMsg(_axMap1.Extents.ToDebugString());

                try
                {
                    Helper.SaveSnapshot(_axMap1, baseName + "-loaded.png", _axMap1.Extents);
                }
                catch (Exception e)
                {
                    Helper.DebugMsg(e.Message);
                    // throw; just continue
                }

                // Zoom in:
                _axMap1.ZoomIn(0.3);
                Helper.DebugMsg(_axMap1.Extents.ToDebugString());

                try
                {
                    Helper.SaveSnapshot(_axMap1, baseName + "-zoomin.png", _axMap1.Extents);
                }
                catch (Exception e)
                {
                    Helper.DebugMsg(e.Message);
                    // throw; just continue
                }
            }
        }

        [TestMethod]
        public void CreateFishnet()
        {
            // Create shape from WKT:
            var shp = new Shape();
            if (!shp.ImportFromWKT(
                "POLYGON ((330918.422383554 5914432.9952417,330791.425601288 5914677.56286955,330799.804294765 5914682.67199867,330809.295198231 5914690.83057468,330851.753425698 5914726.8399904,330890.161005985 5914760.37492299,330891.883975456 5914761.87973075,330894.499450693 5914766.14773636,330895.001406323 5914766.9673645,330894.821345632 5914768.00066471,330895.626814712 5914772.6474656,330898.544123647 5914779.26299206,331042.140051675 5914906.23861184,331066.955908721 5914928.22692301,331071.290848669 5914932.66233604,331075.531881961 5914935.23930972,331086.549669788 5914904.76350951,331104.67722032 5914852.28308518,331120.597430814 5914804.83997655,331131.133792741 5914775.21848511,331118.884180716 5914770.93369604,331091.649916887 5914758.80097565,331072.712088731 5914748.36652613,331052.802159239 5914734.4060014,331043.093305417 5914725.75786093,331036.001117512 5914716.44788158,331028.749419581 5914706.61634015,331024.121040336 5914698.88986961,331020.742433359 5914692.31867847,331016.329278393 5914678.37004658,331011.623099594 5914661.25749118,331005.798813818 5914627.24754255,331002.264592162 5914601.86413354,330997.932682632 5914565.92459662,330994.902438802 5914545.28431325,330991.611136112 5914516.84204983,330989.261968268 5914496.56381567,330984.441627117 5914474.55626726,330974.218375295 5914440.74529109,330918.422383554 5914432.9952417))"))
                Assert.Fail("Could not create shape from wkt: " + shp.ErrorMsg[shp.LastErrorCode]);

            // Create fishnet for bounds of shape:
            var sf = Helper.CreateFishnet(shp.Extents, 20, 20);
            Helper.SaveAsShapefile(sf, Path.Combine(Path.GetTempPath(), "CreateFishnet.shp"));
        }

        [TestMethod]
        public void ImportExportWKT()
        {
            // Create shape from WKT:
            var shp = new Shape();
            if (!shp.ImportFromWKT(
                "POLYGON ((330918.422383554 5914432.9952417,330791.425601288 5914677.56286955,330799.804294765 5914682.67199867,330809.295198231 5914690.83057468,330851.753425698 5914726.8399904,330890.161005985 5914760.37492299,330891.883975456 5914761.87973075,330894.499450693 5914766.14773636,330895.001406323 5914766.9673645,330894.821345632 5914768.00066471,330895.626814712 5914772.6474656,330898.544123647 5914779.26299206,331042.140051675 5914906.23861184,331066.955908721 5914928.22692301,331071.290848669 5914932.66233604,331075.531881961 5914935.23930972,331086.549669788 5914904.76350951,331104.67722032 5914852.28308518,331120.597430814 5914804.83997655,331131.133792741 5914775.21848511,331118.884180716 5914770.93369604,331091.649916887 5914758.80097565,331072.712088731 5914748.36652613,331052.802159239 5914734.4060014,331043.093305417 5914725.75786093,331036.001117512 5914716.44788158,331028.749419581 5914706.61634015,331024.121040336 5914698.88986961,331020.742433359 5914692.31867847,331016.329278393 5914678.37004658,331011.623099594 5914661.25749118,331005.798813818 5914627.24754255,331002.264592162 5914601.86413354,330997.932682632 5914565.92459662,330994.902438802 5914545.28431325,330991.611136112 5914516.84204983,330989.261968268 5914496.56381567,330984.441627117 5914474.55626726,330974.218375295 5914440.74529109,330918.422383554 5914432.9952417))"))
                Assert.Fail("Could not create shape from wkt: " + shp.ErrorMsg[shp.LastErrorCode]);

            // Convert back to WKT string
            try
            {
                var wkt = shp.ExportToWKT();
                Console.WriteLine(wkt);
            }
            catch (Exception ex)
            {
                Assert.Fail("Exception exporting to wkt string: " + ex.Message);
            }
        }


        [TestMethod]
        public void SaveAs()
        {
            var filename = @"D:\dev\GIS-Data\MapWindow-Projects\UnitedStates\Shapefiles\states.shp";

            // Check file:
            if (!File.Exists(filename)) Assert.Fail(filename + " does not exists.");
            // Open shapefile:
            var sf = new Shapefile { GlobalCallback = this };
            if (!sf.Open(filename))
                Assert.Fail("Failed to open shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);

            // Save shapefile:
            var tmpFilename = Path.ChangeExtension(Path.Combine(Path.GetTempPath(), Path.GetTempFileName()), ".shp");
            if (!sf.SaveAs(tmpFilename))
                Assert.Fail("Failed to save shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);
        }

        /// <summary>
        /// Merges the sf.
        /// </summary>
        /// <remarks>MWGIS-69</remarks>
        [TestMethod]
        public void MergeSf()
        {
            const string sf3Location = @"Issues\MWGIS-69\SHP3_POINT.shp";
            const string sf4Location = @"Issues\MWGIS-69\SHP4_POINT.shp";

            var sf3 = new Shapefile { GlobalCallback = this };
            if (!sf3.Open(sf3Location)) Assert.Fail("Can't open " + sf3Location + " Error: " + sf3.ErrorMsg[sf3.LastErrorCode]);

            var sf4 = new Shapefile { GlobalCallback = this };
            if (!sf4.Open(sf4Location)) Assert.Fail("Can't open " + sf4Location + " Error: " + sf4.ErrorMsg[sf4.LastErrorCode]);

            var sfMerged = sf3.Merge(false, sf4, false);
            Assert.IsNotNull(sfMerged, "Merge failed. Error: " + sf3.ErrorMsg[sf3.LastErrorCode]);
            Assert.AreEqual(2, sfMerged.NumShapes, "Incorrect number of shapes");
            Helper.SaveAsShapefile(sfMerged, Path.Combine(Path.GetTempPath(), "MergeSf.shp"));
        }

        /// <summary>
        /// Merges the M shapefiles
        /// </summary>
        /// <remarks>MWGIS-69</remarks>
        [TestMethod]
        public void MergeM()
        {
            const string sf1Location = @"Issues\MWGIS-69\SHP1_POINT_M.shp";
            const string sf2Location = @"Issues\MWGIS-69\SHP2_POINT_M.shp";

            var sf1 = new Shapefile { GlobalCallback = this };
            if (!sf1.Open(sf1Location)) Assert.Fail("Can't open " + sf1Location + " Error: " + sf1.ErrorMsg[sf1.LastErrorCode]);
            Console.WriteLine("num shapes in sf1: " + sf1.NumShapes);

            var sf2 = new Shapefile { GlobalCallback = this };
            if (!sf2.Open(sf2Location)) Assert.Fail("Can't open " + sf2Location + " Error: " + sf2.ErrorMsg[sf2.LastErrorCode]);
            Console.WriteLine("num shapes in sf2: " + sf2.NumShapes);

            Console.WriteLine("Before merge");
            var sfMerged = sf1.Merge(false, sf2, false);
            Assert.IsNotNull(sfMerged, "Merge failed. Error: " + sf1.ErrorMsg[sf1.LastErrorCode]);
            Assert.AreEqual(2, sfMerged.NumShapes, "Incorrect number of shapes");
            Helper.SaveAsShapefile(sfMerged, Path.Combine(Path.GetTempPath(), "MergeM.shp"));
        }

        [TestMethod]
        public void LoadAmericanData()
        {
            const string sfLocation = @"D:\dev\GIS-Data\MapWindow-Projects\UnitedStates\Shapefiles\states.shp";

            var sf = new Shapefile { GlobalCallback = this };
            if (!sf.Open(sfLocation)) Assert.Fail("Can't open " + sfLocation + " Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            var value = sf.CellValue[1, 0] as string;
            sf.Close();
            Console.WriteLine(value);
            Assert.IsNotNull(value, "CellValue failed");
            // Value should be Washington
            Assert.AreEqual("washington", value.ToLower());
        }

        [TestMethod]
        public void ReadRussianDataFromTable()
        {
            const string sfLocation = @"Issues\MWGIS-72\point.shp";
            const int fieldIndex = 2;

            var sf = new Shapefile { GlobalCallback = this };
            if (!sf.Open(sfLocation))
                Assert.Fail("Can't open " + sfLocation + " Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            var value = sf.CellValue[fieldIndex, 0] as string;
            Assert.IsNotNull(value, "No value returned");
            sf.Close();
            Console.WriteLine(value);
            // Value should be Воздух
            Assert.AreEqual('д', value[3]);
        }

        [TestMethod]
        public void ReadRussianDataFromOgrDatasource()
        {
            // https://github.com/MapWindow/MapWinGIS/issues/121
            _axMap1 = Helper.GetAxMap();
            _axMap1.Clear();
            _axMap1.TileProvider = tkTileProvider.OpenStreetMap;
            _axMap1.ZoomBehavior = tkZoomBehavior.zbDefault;
            _axMap1.KnownExtents = tkKnownExtents.keWorld;

            var sfName = @"Issues\MWGIS-72\monitoring_object.shp";
            var baseName = Path.GetFileNameWithoutExtension(sfName);
            var query = "Select * from monitoring_object";
            var layerHandle = _axMap1.AddLayerFromDatabase(sfName, query, true);
            var shpfile = _axMap1.get_Shapefile(layerHandle);
            shpfile.Labels.Generate("[Type]", tkLabelPositioning.lpCenter, false);
            shpfile.Labels.FrameVisible = true;
            shpfile.Labels.FrameType = tkLabelFrameType.lfRectangle;
            Helper.SaveSnapshot(_axMap1, $"{baseName}.jpg", _axMap1.get_layerExtents(layerHandle));
        }

        [TestMethod]
        public void CreateRussianCategories()
        {
            const string sfLocation = @"Issues\MWGIS-72\point.shp";
            const int fieldIndex = 2;

            var sf = new Shapefile { GlobalCallback = this };
            if (!sf.Open(sfLocation))
                Assert.Fail("Can't open " + sfLocation + " Error: " + sf.ErrorMsg[sf.LastErrorCode]);

            // create visualization categories
            sf.DefaultDrawingOptions.FillType = tkFillType.ftStandard;
            sf.Categories.Generate(fieldIndex, tkClassificationType.ctUniqueValues, 0);
            sf.Categories.ApplyExpressions();

            // apply color scheme
            var scheme = new ColorScheme();
            scheme.SetColors2(tkMapColor.LightBlue, tkMapColor.LightYellow);
            sf.Categories.ApplyColorScheme(tkColorSchemeType.ctSchemeGraduated, scheme);
            Assert.IsTrue(sf.Categories.Count > 0, "No categories were made");

            var cat = sf.Categories.Item[0];
            Console.WriteLine(cat.Name);
            Assert.AreNotEqual(cat.Name[0], '?', "The category name is invalid");
        }

        [TestMethod]
        public void PointInShapefile()
        {
            // It goes too fast for DotMemory:
            Thread.Sleep(2000);

            const string folder = @"D:\dev\GIS-Data\Issues\Point in Polygon";
            Assert.IsTrue(Directory.Exists(folder), "Input folder doesn't exists");
            var sfPolygons = new Shapefile { GlobalCallback = this };
            var sfPoints = new Shapefile { GlobalCallback = this };
            var found = 0;
            var stopWatch = new Stopwatch();
            stopWatch.Start();

            try
            {
                var retVal = sfPolygons.Open(Path.Combine(folder, "CatchmentBuilderShapefile.shp"));
                Assert.IsTrue(retVal, "Can't open polygon shapefile");

                retVal = sfPoints.Open(Path.Combine(folder, "Sbk_FGrPt_n.shp"));
                Assert.IsTrue(retVal, "Can't open point shapefile");

                // Caches the coordinates of shapefile points for faster point in shape test:
                retVal = sfPolygons.BeginPointInShapefile();
                Assert.IsTrue(retVal, "Can't cache points");

                var numPoints = sfPoints.NumShapes;
                Assert.IsTrue(numPoints > 0, "No point shapes in shapefile");

                for (var i = 0; i < numPoints; i++)
                {
                    var pointShape = sfPoints.Shape[i];
                    Assert.IsNotNull(pointShape, "pointShape == null");

                    double x = 0d, y = 0d;
                    retVal = pointShape.XY[0, ref x, ref y];
                    Assert.IsTrue(retVal, "Can't get XY from first point");

                    // Returns a number which indicates the index of shapes within which a test point is situated:
                    var shapeIndex = sfPolygons.PointInShapefile(x, y);
                    Console.WriteLine($"Point {i} lies within polygon {shapeIndex}");
                    found++;
                }
            }
            finally
            {
                // Clear cache:
                sfPolygons.EndPointInShapefile();

                // Close shapefiles:
                sfPolygons.Close();
                sfPoints.Close();
            }

            stopWatch.Stop();
            Console.WriteLine("The process took " + stopWatch.Elapsed);
            Console.WriteLine(found + " matching polygons where found");
        }

        [TestMethod]
        public void SpatialIndexMWGIS98()
        {
            const string sfName = @"Issues\MWGIS-98\3dPoint.shp";
            var result = GetInfoShapefile(sfName);
            Assert.IsTrue(result);
            TestSpatialIndex(sfName);
        }


        [TestMethod]
        public void SpatialIndexAllTypes()
        {
            var numErrors = 0;
            foreach (var filename in Directory.EnumerateFiles(@"sf", "*.shp", SearchOption.AllDirectories))
            {
                try
                {
                    Console.WriteLine("***************************");
                    GetInfoShapefile(filename);
                    TestSpatialIndex(filename);
                    Console.WriteLine("Test was successful");
                    Console.WriteLine();
                }
                catch (Exception e)
                {
                    numErrors++;
                    Console.WriteLine(e);
                    // Don't stop
                }
            }

            Assert.AreEqual(0, numErrors);
        }

        private void TestSpatialIndex(string sfName)
        {
            // Delete previous spatial index files:
            Helper.DeleteFile(Path.ChangeExtension(sfName, ".mwd"));
            Helper.DeleteFile(Path.ChangeExtension(sfName, ".mwx"));

            var sf = Helper.OpenShapefile(sfName, true, this);
            bool retVal;
            var baseName = Path.GetFileNameWithoutExtension(sfName);
            try
            {
                Console.WriteLine("Numshapes: " + sf.NumShapes);

                // Get map to create snapshot:
                _axMap1 = Helper.GetAxMap();
                _axMap1.TileProvider = tkTileProvider.ProviderNone;
                _axMap1.ZoomBehavior = tkZoomBehavior.zbDefault;
                _axMap1.KnownExtents = tkKnownExtents.keWorld;

                var layerHandle = _axMap1.AddLayer(sf, true);
                Assert.IsTrue(layerHandle > -1, "Can't add layer");
                _axMap1.ZoomToLayer(layerHandle);
                var snapshot = Helper.SaveSnapshot(_axMap1, $"{baseName}-without.jpg", _axMap1.get_layerExtents(layerHandle), 1);
                var colorsWithout = Helper.GetColorsFromBitmap(snapshot);
                Assert.IsTrue(colorsWithout.Count > 1, "No colors found. Most likely the points are not loaded on the map.");
                Console.WriteLine("Number of unique colors without index: " + colorsWithout.Count);
                // Zoom in:
                _axMap1.ZoomIn(0.3);
                snapshot = Helper.SaveSnapshot(_axMap1, $"{baseName}-without-zoomin.jpg", _axMap1.get_layerExtents(layerHandle), 2);
                var colorsWithoutZoomIn = Helper.GetColorsFromBitmap(snapshot);
                Assert.IsTrue(colorsWithoutZoomIn.Count > 1, "No colors found. Most likely the points are not loaded on the map.");
                Console.WriteLine("Number of unique colors without index zoom in: " + colorsWithoutZoomIn.Count);

                // Create spatial index:
                retVal = sf.CreateSpatialIndex(sfName);
                Assert.IsTrue(retVal, "Can't create spatial index: " + sf.ErrorMsg[sf.LastErrorCode]);
                Assert.IsTrue(sf.IsSpatialIndexValid(), "Spatial index is invalid");
                sf.UseSpatialIndex = true;
                _axMap1.ZoomToLayer(layerHandle);
                snapshot = Helper.SaveSnapshot(_axMap1, $"{baseName}-with.jpg", _axMap1.get_layerExtents(layerHandle), 1);
                var colorsWith = Helper.GetColorsFromBitmap(snapshot);
                Assert.IsTrue(colorsWith.Count > 1, "No colors found. Most likely the points are not loaded on the map.");
                Console.WriteLine("Number of unique colors with index: " + colorsWith.Count);

                // Zoom in:
                _axMap1.ZoomIn(0.3);
                snapshot = Helper.SaveSnapshot(_axMap1, $"{baseName}-with-zoomin.jpg", _axMap1.get_layerExtents(layerHandle), 2);
                var colorsWithZoomIn = Helper.GetColorsFromBitmap(snapshot);
                Console.WriteLine("Number of unique colors with index zoom in: " + colorsWithZoomIn.Count);

                Assert.AreEqual(colorsWith.Count, colorsWithout.Count, "The snapshots no zoom are not identical");
                Assert.AreEqual(colorsWithZoomIn.Count, colorsWithoutZoomIn.Count, "The snapshots zoom in are not identical");
            }
            finally
            {
                retVal = sf.Close();
                Assert.IsTrue(retVal, "Can't close shapefile");
            }
        }

        [TestMethod]
        public void GetInfoAllShapefiles()
        {
            var failed = 0;
            var success = 0;
            // Call EnumerateFiles in a foreach-loop.
            foreach (var filename in Directory.EnumerateFiles(@"sf", "*.shp", SearchOption.AllDirectories))
            {
                try
                {
                    Console.WriteLine("***************************");
                    if (GetInfoShapefile(filename))
                    {
                        success++;
                    }
                    else
                    {
                        failed++;
                    }
                    Console.WriteLine();

                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                    // Don't stop
                    failed++;
                }
            }

            Console.WriteLine($"{success} files were read successfully, {failed} files failed.");

            Assert.IsTrue(failed == 0, "Not all files could be read.");
        }

        [TestMethod]
        public void PointM()
        {
            const string filename = @"d:\dev\GIS-Data\Issues\MWGIS-69 Merge M\shp1_point_m\SHP1_POINT_M.shp";
            var retVal = GetInfoShapefile(filename);
            Assert.IsTrue(retVal);
        }

        [TestMethod]
        public void UpdatePolygonZ()
        {
            const string filename = @"D:\dev\GIS-Data\Issues\CORE-198-crash on view properties\ND_-_Export.shp";
            var retVal = GetInfoShapefile(filename);
            Assert.IsTrue(retVal);

            // Update Z Value:
            var sf = new Shapefile { GlobalCallback = this };
            if (!sf.Open(filename))
                throw new Exception("Cannot open this file: " + sf.ErrorMsg[sf.LastErrorCode]);

            // Go into edit mode:
            if (!sf.StartEditingShapes())
                throw new Exception("Cannot StartEditingShapes: " + sf.ErrorMsg[sf.LastErrorCode]);

            var shp = sf.Shape[0];
            var numPoints = shp.NumPoints;
            for (var i = 0; i < numPoints; i++)
            {
                // Add the index as Z value
                shp.put_Z(i, i + 10);
            }

            // Save and close shapefile:
            if (!sf.StopEditingShapes())
                throw new Exception("Cannot StopEditingShapes: " + sf.ErrorMsg[sf.LastErrorCode]);
            if (!sf.Close())
                throw new Exception("Cannot close shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);

            retVal = GetInfoShapefile(filename);
            Assert.IsTrue(retVal);

        }

        [TestMethod]
        public void UnionShapefiles()
        {
            const string sfSoilLocation = @"Issues\MWGIS-125-Union\SoilTest.shp";
            const string sfUsageLocation = @"Issues\MWGIS-125-Union\UseTest.shp";

            Shapefile sfSoil = null;
            Shapefile sfUsage = null;
            Shapefile sfUnion = null;

            var globalSettings = new GlobalSettings
            {
                CallbackVerbosity = tkCallbackVerbosity.cvAll,
                ShapeInputValidationMode = tkShapeValidationMode.AbortOnErrors,
                MinAreaToPerimeterRatio = 0.021
            };

            try
            {
                sfSoil = Helper.OpenShapefile(sfSoilLocation, true, this);
                var result = Helper.GetInfoShapefile(ref sfSoil);
                Assert.IsTrue(result);

                sfUsage = Helper.OpenShapefile(sfUsageLocation, true, this);
                result = Helper.GetInfoShapefile(ref sfUsage);
                Assert.IsTrue(result);

                var stopWatch = new Stopwatch();
                stopWatch.Start();
                sfUnion = sfSoil.Union(false, sfUsage, false);
                stopWatch.Stop();
                Helper.DebugMsg("Time it took to union 2 shapefile: " + stopWatch.Elapsed);
                if (sfUnion == null) throw new Exception("Error in union: " + sfSoil.ErrorMsg[sfSoil.LastErrorCode]);

                // Save resulting shapefile:
                var workingFolder = Helper.WorkingFolder("MWGIS-125-Union");
                var resultFilename = Path.Combine(workingFolder, "Unioned.shp");
                Helper.SaveAsShapefile(sfUnion, resultFilename);

                result = Helper.GetInfoShapefile(ref sfUnion);
                Assert.IsTrue(result);

                Console.WriteLine("*************** Validate union *************");
                Console.WriteLine("globalSettings.MinPolygonArea: " + globalSettings.MinPolygonArea.ToString(CultureInfo.InvariantCulture));
                Console.WriteLine("globalSettings.MinAreaToPerimeterRatio: " + globalSettings.MinAreaToPerimeterRatio.ToString(CultureInfo.InvariantCulture));

                // Check all fields:
                var numFields = sfUnion.NumFields;
                var numShapes = sfUnion.NumShapes;
                for (var shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                {
                    // Get shape:
                    var shp = sfUnion.Shape[shapeIndex];
                    Console.WriteLine("Area: " + shp.Area.ToString(CultureInfo.InvariantCulture));
                    Console.WriteLine("Perimeter: " + shp.Perimeter.ToString(CultureInfo.InvariantCulture));
                    for (var fieldIndex = 0; fieldIndex < numFields; fieldIndex++)
                    {
                        var fieldName = sfUnion.Field[fieldIndex].Name;
                        var value = sfUnion.CellValue[fieldIndex, shapeIndex].ToString();
                        Console.WriteLine($"Field name: {fieldName} Value: {value}");
                        Assert.AreNotEqual(value, string.Empty, "Value is empty");
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                Assert.Fail(e.Message);
            }
            finally
            {
                sfSoil?.Close();
                sfUsage?.Close();
                sfUnion?.Close();
            }
        }

        [TestMethod]
        public void ClipShapefiles()
        {
            const string sfSoilLocation = @"Issues\MWGIS-125-Union\SoilTest.shp";
            const string sfUsageLocation = @"Issues\MWGIS-125-Union\UseTest.shp";

            Shapefile sfSoil = null;
            Shapefile sfUsage = null;
            Shapefile sfClip = null;

            var globalSettings = new GlobalSettings
            {
                CallbackVerbosity = tkCallbackVerbosity.cvAll,
                ShapeInputValidationMode = tkShapeValidationMode.AbortOnErrors
            };

            try
            {
                sfSoil = Helper.OpenShapefile(sfSoilLocation, true, this);
                var result = Helper.GetInfoShapefile(ref sfSoil);
                Assert.IsTrue(result);

                sfUsage = Helper.OpenShapefile(sfUsageLocation, true, this);
                result = Helper.GetInfoShapefile(ref sfUsage);
                Assert.IsTrue(result);

                var stopWatch = new Stopwatch();
                stopWatch.Start();
                sfClip = sfSoil.Clip(false, sfUsage, false);
                stopWatch.Stop();
                Helper.DebugMsg("Time it took to clip 2 shapefile: " + stopWatch.Elapsed);
                if (sfClip == null) throw new Exception("Error in Clip: " + sfSoil.ErrorMsg[sfSoil.LastErrorCode]);

                result = Helper.GetInfoShapefile(ref sfClip);
                Assert.IsTrue(result);

                Console.WriteLine("*************** Validate clip *************");
                Console.WriteLine("globalSettings.MinPolygonArea: " + globalSettings.MinPolygonArea.ToString(CultureInfo.InvariantCulture));
                Console.WriteLine("globalSettings.MinAreaToPerimeterRatio: " + globalSettings.MinAreaToPerimeterRatio.ToString(CultureInfo.InvariantCulture));

                // Check all fields:
                var numFields = sfClip.NumFields;
                var numShapes = sfClip.NumShapes;
                for (var shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                {
                    // Get shape:
                    var shp = sfClip.Shape[shapeIndex];
                    Console.WriteLine("Area: " + shp.Area.ToString(CultureInfo.InvariantCulture));
                    for (var fieldIndex = 0; fieldIndex < numFields; fieldIndex++)
                    {
                        var fieldName = sfClip.Field[fieldIndex].Name;
                        var value = sfClip.CellValue[fieldIndex, shapeIndex].ToString();
                        Console.WriteLine($"Field name: {fieldName} Value: {value}");
                        Assert.AreNotEqual(value, string.Empty, "Value is empty");
                    }
                }

                // Save resulting shapefile:
                var workingFolder = Helper.WorkingFolder("MWGIS-125-Union");
                var resultFilename = Path.Combine(workingFolder, "Clipped.shp");
                Helper.SaveAsShapefile(sfClip, resultFilename);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                Assert.Fail(e.Message);
            }
            finally
            {
                sfSoil?.Close();
                sfUsage?.Close();
                sfClip?.Close();
            }
        }

        [TestMethod]
        public void DifferenceShapefiles()
        {
            const string sfSoilLocation = @"Issues\MWGIS-125-Union\SoilTest.shp";
            const string sfUsageLocation = @"Issues\MWGIS-125-Union\UseTest.shp";

            Shapefile sfSoil = null;
            Shapefile sfUsage = null;
            Shapefile sfDifference = null;

            var globalSettings = new GlobalSettings
            {
                CallbackVerbosity = tkCallbackVerbosity.cvAll,
                ShapeInputValidationMode = tkShapeValidationMode.AbortOnErrors
            };

            try
            {
                sfSoil = Helper.OpenShapefile(sfSoilLocation, true, this);
                var result = Helper.GetInfoShapefile(ref sfSoil);
                Assert.IsTrue(result);

                sfUsage = Helper.OpenShapefile(sfUsageLocation, true, this);
                result = Helper.GetInfoShapefile(ref sfUsage);
                Assert.IsTrue(result);

                var stopWatch = new Stopwatch();
                stopWatch.Start();
                sfDifference = sfSoil.Difference(false, sfUsage, false);
                stopWatch.Stop();
                Helper.DebugMsg("Time it took to difference 2 shapefile: " + stopWatch.Elapsed);
                if (sfDifference == null) throw new Exception("Error in Clip: " + sfSoil.ErrorMsg[sfSoil.LastErrorCode]);

                result = Helper.GetInfoShapefile(ref sfDifference);
                Assert.IsTrue(result);

                Console.WriteLine("*************** Validate difference *************");
                Console.WriteLine("globalSettings.MinPolygonArea: " + globalSettings.MinPolygonArea.ToString(CultureInfo.InvariantCulture));
                Console.WriteLine("globalSettings.MinAreaToPerimeterRatio: " + globalSettings.MinAreaToPerimeterRatio.ToString(CultureInfo.InvariantCulture));

                // Check all fields:
                var numFields = sfDifference.NumFields;
                var numShapes = sfDifference.NumShapes;
                for (var shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                {
                    // Get shape:
                    var shp = sfDifference.Shape[shapeIndex];
                    Console.WriteLine("Area: " + shp.Area.ToString(CultureInfo.InvariantCulture));
                    for (var fieldIndex = 0; fieldIndex < numFields; fieldIndex++)
                    {
                        var fieldName = sfDifference.Field[fieldIndex].Name;
                        var value = sfDifference.CellValue[fieldIndex, shapeIndex].ToString();
                        Console.WriteLine($"Field name: {fieldName} Value: {value}");
                        Assert.AreNotEqual(value, string.Empty, "Value is empty");
                    }
                }
                // Save resulting shapefile:
                var workingFolder = Helper.WorkingFolder("MWGIS-125-Union");
                var resultFilename = Path.Combine(workingFolder, "Difference1.shp");
                Helper.SaveAsShapefile(sfDifference, resultFilename);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                Assert.Fail(e.Message);
            }
            finally
            {
                sfSoil?.Close();
                sfUsage?.Close();
                sfDifference?.Close();
            }
        }

        [TestMethod]
        public void DifferenceReverseShapefiles()
        {
            const string sfSoilLocation = @"Issues\MWGIS-125-Union\SoilTest.shp";
            const string sfUsageLocation = @"Issues\MWGIS-125-Union\UseTest.shp";

            Shapefile sfSoil = null;
            Shapefile sfUsage = null;
            Shapefile sfDifference = null;

            var globalSettings = new GlobalSettings
            {
                CallbackVerbosity = tkCallbackVerbosity.cvAll,
                ShapeInputValidationMode = tkShapeValidationMode.AbortOnErrors
            };

            try
            {
                sfSoil = Helper.OpenShapefile(sfSoilLocation, true, this);
                var result = Helper.GetInfoShapefile(ref sfSoil);
                Assert.IsTrue(result);

                sfUsage = Helper.OpenShapefile(sfUsageLocation, true, this);
                result = Helper.GetInfoShapefile(ref sfUsage);
                Assert.IsTrue(result);
                var stopWatch = new Stopwatch();
                stopWatch.Start();
                sfDifference = sfUsage.Difference(false, sfSoil, false);
                stopWatch.Stop();
                Helper.DebugMsg("Time it took to difference 2 shapefile: " + stopWatch.Elapsed);

                if (sfDifference == null) throw new Exception("Error in Difference: " + sfSoil.ErrorMsg[sfSoil.LastErrorCode]);

                result = Helper.GetInfoShapefile(ref sfDifference);
                Assert.IsTrue(result);

                Console.WriteLine("*************** Validate difference reverse *************");
                Console.WriteLine("globalSettings.MinPolygonArea: " + globalSettings.MinPolygonArea.ToString(CultureInfo.InvariantCulture));
                Console.WriteLine("globalSettings.MinAreaToPerimeterRatio: " + globalSettings.MinAreaToPerimeterRatio.ToString(CultureInfo.InvariantCulture));

                // Check all fields:
                var numFields = sfDifference.NumFields;
                var numShapes = sfDifference.NumShapes;
                for (var shapeIndex = 0; shapeIndex < numShapes; shapeIndex++)
                {
                    // Get shape:
                    var shp = sfDifference.Shape[shapeIndex];
                    Console.WriteLine("Area: " + shp.Area.ToString(CultureInfo.InvariantCulture));
                    for (var fieldIndex = 0; fieldIndex < numFields; fieldIndex++)
                    {
                        var fieldName = sfDifference.Field[fieldIndex].Name;
                        var value = sfDifference.CellValue[fieldIndex, shapeIndex].ToString();
                        Console.WriteLine($"Field name: {fieldName} Value: {value}");
                        Assert.AreNotEqual(value, string.Empty, "Value is empty");
                    }
                }
                var workingFolder = Helper.WorkingFolder("MWGIS-125-Union");
                var resultFilename = Path.Combine(workingFolder, "Difference2.shp");
                Helper.SaveAsShapefile(sfDifference, resultFilename);

            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                Assert.Fail(e.Message);
            }
            finally
            {
                sfSoil?.Close();
                sfUsage?.Close();
                sfDifference?.Close();
            }
        }

        [TestMethod]
        public void CreatePolylineShapefile()
        {
            // https://mapwindow.discourse.group/t/create-polyline-shapefile-adding-attributes-issue/147/1

            // First read some coordinates, in this case I read them from another polyline shapefile:
            var sfInput = Helper.OpenShapefile(@"sf\SHP_POLYLINE.shp");
            var lines = new List<List<Point>>(); // lines is a list of line and each line is a list of point
            for (var i = 0; i < sfInput.NumShapes; i++)
            {
                var shp = sfInput.Shape[i];
                var line = new List<Point>();
                for (var j = 0; j < shp.NumPoints; j++)
                {
                    line.Add(shp.Point[j]);
                }
                lines.Add(line);
            }

            // Create a new shapefile
            var sf = new Shapefile { GlobalCallback = this };
            Assert.IsNotNull(sf, "sf is null");
            Assert.IsTrue(sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYLINE), "Cannot create shapefile");

            // Create fields:
            var keyFieldIndex = sf.EditAddField("key", FieldType.INTEGER_FIELD, 0, 5);
            var evenFieldIndex = sf.EditAddField("even", FieldType.BOOLEAN_FIELD, 0, 1);
            var dateFieldIndex = sf.EditAddField("date", FieldType.DATE_FIELD, 0, 12);

            // Create new lines:
            foreach (var line in lines)
            {
                var shp = new Shape();
                Assert.IsTrue(shp.Create(sf.ShapefileType));
                foreach (var point in line)
                {
                    // Add point to shape:
                    shp.AddPoint(point.x, point.y);
                }

                // Add the new shape to the shapefile:
                var shpIndex = sf.EditAddShape(shp);
                // Add some attributes:
                Assert.IsTrue(sf.EditCellValue(keyFieldIndex, shpIndex, shpIndex), "Cannot add key attribute");
                Assert.IsTrue(sf.EditCellValue(evenFieldIndex, shpIndex, shpIndex % 2 == 0), "Cannot add even attribute");
                Assert.IsTrue(sf.EditCellValue(dateFieldIndex, shpIndex, DateTime.Now), "Cannot add date attribute");
            }

            // Optional: Add projection:
            sf.GeoProjection = sfInput.GeoProjection.Clone();

            // Now the new shapefile is created, check it and save it:
            Assert.IsFalse(sf.HasInvalidShapes(), "sf has invalid shapes");
            var fileLocation = Path.Combine(Path.GetTempPath(), "NewShapefile.shp");
            Helper.SaveAsShapefile(sf, fileLocation);
            Debug.WriteLine(fileLocation);
        }

        [TestMethod]
        public void TableQueryTest()
        {
            const string sfName = @"D:\dev\GIS-data\Forum\Search-speed-issue\RD12115.shp";
            // Delete previous spatial index files:
            Helper.DeleteFile(Path.ChangeExtension(sfName, ".mwd"));
            Helper.DeleteFile(Path.ChangeExtension(sfName, ".mwx"));

            var sf = Helper.OpenShapefile(sfName, true, this);

            const string query = @"[NAME] =""la France Ave"" and (([FROMLEFT]<=4201 and [TOLEFT]>=4499) or ([FROMRIGHT]<=4200 and [TORIGHT]>=4498)) and ([ZIP_LEFT] = ""34286"" or [ZIP_RIGHT] = ""34286"")";
            Helper.DebugMsg(query);

            TableQuery(sf, query, "filebased shapefile");
            TableQuery(sf, query, "filebased shapefile second time");

            // Make memory copy:
            var stopWatch = new Stopwatch();
            stopWatch.Start();
            sf.SelectAll();
            var sfTemp = sf.ExportSelection();
            sf.SelectNone();
            stopWatch.Stop();
            Helper.DebugMsg($"Time it took to ExportSelection: { stopWatch.Elapsed}");

            TableQuery(sfTemp, query, "memory shapefile");
            TableQuery(sfTemp, query, "memory shapefile second time");
            var fileLocation = Path.Combine(Path.GetTempPath(), "speed.shp");
            Helper.SaveAsShapefile(sfTemp, fileLocation);

            // Again after saving
            TableQuery(sfTemp, query, "memory shapefile after saving");
            TableQuery(sfTemp, query, "memory shapefile after saving second time");
            sfTemp.Close();

            // Use QTree
            sf.UseQTree = true;
            TableQuery(sf, query, "shapefile with QTree index");
            TableQuery(sf, query, "shapefile with QTree index second time");

            // Use spatial index:
            Assert.IsTrue(sf.CreateSpatialIndex(sf.Filename), "Cannot create spatial index");
            sf.UseQTree = false;
            TableQuery(sf, query, "shapefile with spatial index");
            TableQuery(sf, query, "shapefile with spatial index second time");

            // Open temp shapefile:
            var sfTemp2 = Helper.OpenShapefile(fileLocation);
            sfTemp2.UseQTree = true;
            TableQuery(sfTemp2, query, " temp shapefile with QTree index after loading from disk");
            TableQuery(sfTemp2, query, " temp shapefile with QTree index after loading from disk second time");
        }

        private static void TableQuery(IShapefile sf, string query, string logMsg)
        {
            object result = null;
            string errorString = null;

            var stopWatch = new Stopwatch();
            Helper.DebugMsg("Start sf.Table.Query");
            stopWatch.Start();
            Assert.IsTrue(sf.Table.Query(query, ref result, ref errorString), "Table.Query: " + errorString);
            stopWatch.Stop();
            Helper.DebugMsg($"Time it took to query {logMsg}: { stopWatch.Elapsed}");

            Assert.IsNotNull(result);
            var indexes = result as int[];
            Assert.IsNotNull(indexes);
            Assert.IsTrue(indexes.Length > 0, "No results found");
        }

        private bool GetInfoShapefile(string filename)
        {
            if (!File.Exists(filename))
                throw new FileNotFoundException("Cannot find file", filename);

            Console.WriteLine("Working with " + filename);
            var sf = new Shapefile { GlobalCallback = this };
            var result = sf.Open(filename, this);
            Assert.IsTrue(result, "Could not open shapefile");

            var retVal = Helper.GetInfoShapefile(ref sf);
            sf.Close();
            return retVal;
        }

        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            Console.Write(".");
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            Assert.Fail("Found error: " + ErrorMsg);
        }

    }
}
