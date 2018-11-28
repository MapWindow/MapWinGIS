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
            var tempFolder = Path.GetTempPath();
            var sfLineInput = Helper.OpenShapefile(Path.Combine(@"sf", "Reach2_simplify100_smooth.shp"));
            var utils = new Utils();

            // Create new shapefile with polylines:
            var sfNew = Helper.CreateSf(ShpfileType.SHP_POLYLINE);
            sfNew.GeoProjection = sfLineInput.GeoProjection.Clone();

            var numShapesInput = sfLineInput.NumShapes;
            for (var i = 0; i < numShapesInput; i++)
            {
                var shape = sfLineInput.Shape[i];
                var numPoints = shape.numPoints;
                const int distance = 20;

                // Skip the first point:
                for (var j = 0; j < numPoints; j++)
                {
                    var firstPoint = shape.Point[j];
                    var secondPoint = shape.Point[j + 1];
                    if (j == numPoints - 1) secondPoint = shape.Point[j - 1];

                    var angle = utils.GetAngle(firstPoint, secondPoint);
                    // convert to proper Cartesian angle of rotation
                    angle = (450 - angle) % 360;
                    // convert to Radians
                    angle = angle * Math.PI / 180.0;

                    // Get perpendicular angle:
                    angle += 90 * Math.PI / 180.0; ;
                    if (angle > 2 * Math.PI) angle -= 2 * Math.PI;

                    var newLineShp = new Shape();
                    newLineShp.Create(ShpfileType.SHP_POLYLINE);
                    newLineShp.AddPoint(firstPoint.x, firstPoint.y);
                    newLineShp.AddPoint(Math.Cos(angle) * distance + firstPoint.x,
                        Math.Sin(angle) * distance + firstPoint.y);
                    sfNew.EditAddShape(newLineShp);
                    // Add line to other side:
                    newLineShp = new Shape();
                    newLineShp.Create(ShpfileType.SHP_POLYLINE);
                    newLineShp.AddPoint(firstPoint.x, firstPoint.y);
                    newLineShp.AddPoint(Math.Cos(angle) * -1 * distance + firstPoint.x,
                        Math.Sin(angle) * -1 * distance + firstPoint.y);
                    sfNew.EditAddShape(newLineShp);
                }
            }

            // Save new shapefile:
            Helper.SaveAsShapefile(sfNew, Path.Combine(tempFolder, "AngledLines.shp"));
            sfNew.Close();

            var retVal = sfLineInput.Close();
            Assert.IsTrue(retVal, "Cannot close line shapefile: " + sfLineInput.ErrorMsg[sfLineInput.LastErrorCode]);

        }
    }
}