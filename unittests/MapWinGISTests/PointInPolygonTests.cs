using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class PointInPolygonTests
    {
        [TestMethod]
        public void TimePointLoop()
        {
            bool retVal;
            // Open shapefile:
            var fileName = Path.Combine(@"sf", "Reach2_simplify100_smooth.shp");
            var sfLine = Helper.OpenShapefile(fileName);

            var numShapes = sfLine.NumShapes;
            Debug.WriteLine("numShapes: " + numShapes);

            var stopwatch = new Stopwatch();
            stopwatch.Start();
            for (var i = 0; i < numShapes; i++)
            {
                var shape = sfLine.Shape[i];
                var numPoints = shape.numPoints;
                Debug.WriteLine("");
                Debug.WriteLine("***************");
                Debug.WriteLine($"Shape {i} has {numPoints} points");
                for (var j = 0; j < numPoints; j++)
                {
                    var x = 0d;
                    var y = 0d;
                    retVal = shape.XY[j, ref x, ref y];
                    Assert.IsTrue(retVal, "Cannot get XY: " + shape.ErrorMsg[shape.LastErrorCode]);
                    Debug.WriteLine($"X: {x} Y: {y}");
                }
            }

            retVal = sfLine.Close();
            Assert.IsTrue(retVal, "Cannot close shapefile: " + sfLine.ErrorMsg[sfLine.LastErrorCode]);
            stopwatch.Stop();
            Debug.WriteLine("Looping shapes " + stopwatch.Elapsed);
        }

        [TestMethod]
        public void TimePointInShapefile()
        {
            // Open shapefiles:
            var sfLine = Helper.OpenShapefile(Path.Combine(@"sf", "Reach2_simplify100_smooth.shp"));
            var sfPolygon = Helper.OpenShapefile(Path.Combine(@"sf", "embankments_buffered_split.shp"));

            var stopwatch = new Stopwatch();
            stopwatch.Start();
            var retVal = sfPolygon.BeginPointInShapefile();
            Assert.IsTrue(retVal, "Error in BeginPointInShapefile: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);
            stopwatch.Stop();
            Debug.WriteLine("BeginPointInShapefile took " + stopwatch.Elapsed);

            var numShapes = sfLine.NumShapes;
            Debug.WriteLine("numShapes: " + numShapes);
            stopwatch.Restart();

            // Get long line:
            var shape = sfLine.Shape[16];
            var numPoints = shape.numPoints;
            Debug.WriteLine($"Shape 16 has {numPoints} points");
            for (var j = 0; j < numPoints; j++)
            {
                var x = 0d;
                var y = 0d;
                retVal = shape.XY[j, ref x, ref y];
                Assert.IsTrue(retVal, "Cannot get XY: " + shape.ErrorMsg[shape.LastErrorCode]);
                var selectedShape = sfPolygon.PointInShapefile(x, y);
                Debug.WriteLine($"point (X: {x} Y: {y}) is in shape {selectedShape}");
            }

            stopwatch.Stop();
            Debug.WriteLine("PointInShapefile took " + stopwatch.Elapsed);

            sfPolygon.EndPointInShapefile();
            retVal = sfPolygon.Close();
            Assert.IsTrue(retVal, "Cannot close polygon shapefile: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);

            retVal = sfLine.Close();
            Assert.IsTrue(retVal, "Cannot close line shapefile: " + sfLine.ErrorMsg[sfLine.LastErrorCode]);
        }

        [TestMethod]
        public void CreateAnglePolylines()
        {
            var sfLineInput = Helper.OpenShapefile(Path.Combine(@"sf", "Reach2_simplify100_smooth.shp"));
            Debug.WriteLine(sfLineInput.NumShapes);
            var utils = new Utils();

            // Create new shapefile with polylines:
            var sfNew = Helper.CreateSf(ShpfileType.SHP_POLYLINE);
            sfNew.GeoProjection = sfLineInput.GeoProjection.Clone();

            // TODO: For each shape:
            var i = 10; // One shape first
            var shape = sfLineInput.Shape[i];
            var numPoints = shape.numPoints;
            Helper.DebugMsg("Number points: " + numPoints);
            const int distance = 15;

            for (var j = 1; j < numPoints; j++)
            {
                var firstPoint = shape.Point[j - 1];
                var secondPoint = shape.Point[j];
                var angle = utils.GetAngle(firstPoint, secondPoint);
                // Get perpendicular angle:
                angle += 90;
                if (angle > 360) angle -= 360;
                Helper.DebugMsg("Angle: " + angle);

                var newLineShp = new Shape();
                newLineShp.Create(ShpfileType.SHP_POLYLINE);
                newLineShp.AddPoint(firstPoint.x, firstPoint.y);
                newLineShp.AddPoint(Math.Sin(angle) * distance + firstPoint.x, Math.Cos(angle) * distance + firstPoint.y);
                sfNew.EditAddShape(newLineShp);
            }

            // Save new shapefile:
            var tempFolder = Path.GetTempPath();
            Helper.SaveAsShapefile(sfNew, Path.Combine(tempFolder, "AngledLines.shp"));
            sfNew.Close();

            var retVal = sfLineInput.Close();
            Assert.IsTrue(retVal, "Cannot close line shapefile: " + sfLineInput.ErrorMsg[sfLineInput.LastErrorCode]);

        }
    }
}