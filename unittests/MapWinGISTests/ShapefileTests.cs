using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class ShapefileTests
    {

        [TestMethod]
        public void SaveShapefileTest()
        {
            var tempFolder = Path.GetTempPath();
            var tempFilename = Path.Combine(tempFolder, "CreateShapefileTest.shp");
            DeleteShapefile(tempFilename);

            bool result;
            // Create shapefile
            var sf = new ShapefileClass();
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
            DeleteShapefile(tempFilename);

            bool result;
            // Create shapefile
            var sf = new ShapefileClass();
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
                var shp = new ShapeClass();
                result = shp.Create(sf.ShapefileType);
                Assert.IsTrue(result, "Could not create shape");
                // Create point:
                var pnt = new Point
                {
                    x = 200,
                    y = 200
                };
                // Add point:
                var pointIndex = shp.numPoints;
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

        /// <summary>
        /// Checks the null value table data.
        /// </summary>
        /// <remarks>https://mapwindow.atlassian.net/projects/CORE/issues/CORE-177</remarks>
        [TestMethod]
        public void CheckNullValueTableData()
        {
            bool result;
            var sf = new ShapefileClass();

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

                fieldIndex = sf.Table.EditAddField("double", FieldType.DOUBLE_FIELD, 0, 50);
                Assert.AreEqual(3, fieldIndex, "Could not add field");

                Assert.AreEqual(4, sf.NumFields, "Wrong number of fields");

                // Create shape:
                var shp = new ShapeClass();
                result = shp.Create(ShpfileType.SHP_POINT);
                Assert.IsTrue(result, "Could not create point shape");
                // Create point:
                var pnt = new Point
                {
                    x = 200,
                    y = 200
                };
                // Add point:
                var pointIndex = shp.numPoints;
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
                    Debug.WriteLine($"Is the value of fieldId {i} NULL: {value == null} Type of field is {field.Type}");
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
            var sf = new ShapefileClass();

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
                Debug.WriteLine(sf.Table.ErrorMsg[sf.Table.LastErrorCode]);

                // This should fail, because precsion cannot be 0 when type is double:
                fieldIndex = sf.Table.EditAddField("date", FieldType.DOUBLE_FIELD, 0, 20);
                Assert.AreEqual(-1, fieldIndex, "Field is not added but -1 is not returned. ");
                Debug.WriteLine(sf.Table.ErrorMsg[sf.Table.LastErrorCode]);
            }
            finally
            {
                // Close the shapefile:
                result = sf.Close();
                Assert.IsTrue(result, "Could not close shapefile");
            }
        }

        private static void DeleteShapefile(string filename)
        {
            var folder = Path.GetDirectoryName(filename);
            if (folder == null) return;
            var filenameBody = Path.GetFileNameWithoutExtension(filename);
            foreach (var f in Directory.EnumerateFiles(folder, filenameBody + ".*"))
            {
                Debug.WriteLine("deleting " + f);
                File.Delete(f);
            }
        }
    }
}
