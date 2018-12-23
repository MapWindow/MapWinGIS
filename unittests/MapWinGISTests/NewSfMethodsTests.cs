using System;
using System.CodeDom;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class NewSfMethodsTests
    {
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
                    angle += 90 * Math.PI / 180.0;
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


        [TestMethod]
        public void InterpolateTest()
        {
            var sfTracks = Helper.OpenShapefile("sf/Tracks.shp");
            var sfPoints = Helper.CreateSf(ShpfileType.SHP_POINT);
            // Set projection:
            sfPoints.GeoProjection = sfTracks.GeoProjection.Clone();

            // Get the first shape:
            var shp = sfTracks.Shape[0];
            var trackLength = shp.Length;
            // Get the first point:
            var firstPoint = shp.Point[0];
            // Save first point to result shapefile:
            Helper.AddPointToPointSf(sfPoints, firstPoint);
            var segmentLength = 0d;
            const int distance = 10;
            while (trackLength > segmentLength)
            {
                var lastShape = sfPoints.Shape[sfPoints.NumShapes - 1];
                var nextPoint = shp.InterpolatePoint(lastShape.Point[0], distance);
                Helper.AddPointToPointSf(sfPoints, nextPoint);
                segmentLength += distance;
            }

            sfTracks.Close();
            Helper.SaveAsShapefile(sfPoints, Path.Combine(Path.GetTempPath(), "InterpolatedPoints.shp"));
        }
    }
}
