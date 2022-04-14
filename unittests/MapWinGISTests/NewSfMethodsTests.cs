using System;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Point = MapWinGIS.Point;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class NewSfMethodsTests : ICallback
    {
        [TestInitialize]
        public void Start()
        {
            var utils = new Utils();
            var version = utils.GDALInfo("", "--version");
            Debug.WriteLine(version);
            var axMap1 = Helper.GetAxMap();
            axMap1.BackColor = Color.Aquamarine;
            
            axMap1.BeforeVertexDigitized += AxMap1_BeforeVertexDigitized;
        }

        private void AxMap1_BeforeVertexDigitized(object sender, AxMapWinGIS._DMapEvents_BeforeVertexDigitizedEvent e)
        {
            throw new NotImplementedException();
        }

        [TestMethod]
        public void CreateAnglePolylines()
        {
            var tempFolder = Path.GetTempPath();
            var sfLineInput = Helper.OpenShapefile(Path.Combine(@"sf", "Tracks.shp"));
            var utils = new Utils();

            // Create new shapefile with polylines:
            var sfNew = Helper.CreateSf(ShpfileType.SHP_POLYLINE);
            sfNew.GeoProjection = sfLineInput.GeoProjection.Clone();

            var numShapesInput = sfLineInput.NumShapes;
            for (var i = 0; i < numShapesInput; i++)
            {
                var shape = sfLineInput.Shape[i];
                var numPoints = shape.NumPoints;
                const int distance = 20;

                // Skip the first point:
                for (var j = 0; j < numPoints; j++)
                {
                    var firstPoint = shape.Point[j];
                    var secondPoint = shape.Point[j + 1];
                    if (j == numPoints - 1) secondPoint = shape.Point[j - 1];

                    var angle = GetAngle(utils, firstPoint, secondPoint);

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
            var sfTracks = Helper.OpenShapefile(Path.Combine(@"sf", "Tracks.shp"));
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

        [TestMethod]
        public void GetIntersectionTest()
        {
            var result = GetIntersection(new Helper.Coordinate(1, 1), new Helper.Coordinate(5, 7),
                new Helper.Coordinate(1, 6), new Helper.Coordinate(2, 5));
            Debug.WriteLine(result);
            Assert.AreEqual(3, result.X);
            Assert.AreEqual(4, result.Y);
        }


        [TestMethod]
        public void DifferenceTest1()
        {
            // 145, 154
            GetDifference("POLYGON ((259449.071118579 543868.469050065,259460.460699968 543878.23006619,259463.245769758 543873.768560297,259455.578068317 543860.876455946,259449.071118579 543868.469050065))",
                "POLYGON ((259463.243735243 543856.317227775,259458.0148573 543864.973555723,259462.906856719 543873.198728218,259471.741696109 543861.450443661,259463.243735243 543856.317227775))");
        }

        [TestMethod]
        public void DifferenceTest2()
        {
            // 24, 32
            GetDifference("POLYGON ((259073.014081524 544320.092508488,259085.267633063 544321.677731439,259085.942168897 544320.308101652,259074.192475622 544310.983687036,259073.014081524 544320.092508488))",
                "POLYGON ((259087.677538158 544333.393525226,259097.260135269 544321.853428493,259090.546766392 544316.354731347,259079.984140336 544327.005127152,259087.677538158 544333.393525226))");
        }

        private static void GetDifference(string wktShp1, string wktShp2)
        {
            var sf = Helper.CreateSf(ShpfileType.SHP_POLYGON);
            var shp = new Shape();
            shp.ImportFromWKT(wktShp1);
            Assert.IsTrue(shp.IsValid);
            var shpIndex = sf.EditAddShape(shp);
            Assert.IsTrue(shpIndex >= 0);

            var shp2 = new Shape();
            shp2.ImportFromWKT(wktShp2);
            Assert.IsTrue(shp2.IsValid);
            shpIndex = sf.EditAddShape(shp2);
            Assert.IsTrue(shpIndex >= 0);

            if (shp.Overlaps(shp2))
            {
                Debug.WriteLine("Shapes overlap and touches: " + shp.Touches(shp2));

                var shp3 = shp2.Clip(shp, tkClipOperation.clDifference);
                Debug.WriteLine(shp3.ExportToWKT());
                Assert.IsTrue(shp3.IsValid);
                Assert.IsTrue(shp3.PartIsClockWise[0]);
                Assert.IsFalse(shp.Overlaps(shp3), "New shape still overlaps");
                // sf.EditDeleteShape(1); // Remove shp2
                shpIndex = sf.EditAddShape(shp3);
                Assert.IsTrue(shpIndex >= 0);
                Debug.WriteLine($"Clipped shape overlaps: {shp.Overlaps(shp3)} and touches: {shp.Touches(shp3)}");
                Debug.WriteLine($"shp2.Perimeter: {shp2.Perimeter} shp3.Perimeter: {shp3.Perimeter}");
                Debug.WriteLine($"shp2.Area: {shp2.Area} shp3.Area: {shp3.Area}");
                Assert.IsFalse(Math.Abs(shp2.Area - shp3.Area) < double.Epsilon, "Area are still the same");
            }

            Helper.SaveAsShapefile(sf, Path.Combine(Path.GetTempPath(), "TestClippingMethods.shp"));
        }

        [TestMethod]
        public void CreateSprayAreas()
        {
            var sfTracks = Helper.OpenShapefile(Path.Combine(@"sf", "Tracks.shp"));
            var sfPoints = Helper.CreateSf(ShpfileType.SHP_POINT);
            var sfLines = Helper.CreateSf(ShpfileType.SHP_POLYLINE);
            var sfPolygons = Helper.CreateSf(ShpfileType.SHP_POLYGON);

            // Set projection:
            sfPoints.GeoProjection = sfTracks.GeoProjection.Clone();
            sfLines.GeoProjection = sfTracks.GeoProjection.Clone();
            sfPolygons.GeoProjection = sfTracks.GeoProjection.Clone();

            var tempPath = Path.GetTempPath();
            var utils = new Utils();

            // Debug first track point:
            var debugPoint = sfTracks.Shape[0].Point[0];
            Debug.WriteLine(debugPoint.x);
            Debug.WriteLine(debugPoint.y);

            // Get the first shape:
            var shp = sfTracks.Shape[0];
            var trackLength = shp.Length;
            // Get the first point:
            var firstPoint = shp.Point[0];
            // Save first point to result shapefile:
            Helper.AddPointToPointSf(sfPoints, firstPoint);
            var segmentLength = 0d; // Total lenght between the points
            const int distance = 10; // Distance between points
            const int length = 15; // Length of perpendicular lines
            var skip = 1;
            while (trackLength > segmentLength)
            {
                // Create next point on line:
                var lastShape = sfPoints.Shape[sfPoints.NumShapes - 1];
                var previousPoint = lastShape.Point[0];
                var nextPoint = shp.InterpolatePoint(previousPoint, distance * skip);

                Helper.AddPointToPointSf(sfPoints, nextPoint);
                skip = 1; // reset

                // Create perpendicular line
                var angle = GetAngle(utils, previousPoint, nextPoint);
                var previousPointCoordinate = new Helper.Coordinate(previousPoint.x, previousPoint.y);
                var perpendicularCoordinate1 = new Helper.Coordinate(Math.Cos(angle) * length + previousPoint.x,
                    Math.Sin(angle) * length + previousPoint.y);
                AddCoordinatesToLineSf(sfLines, previousPointCoordinate, perpendicularCoordinate1);
                // Add line to other side:
                var perpendicularCoordinate2 = new Helper.Coordinate(Math.Cos(angle) * -1 * length + previousPoint.x,
                    Math.Sin(angle) * -1 * length + previousPoint.y);
                AddCoordinatesToLineSf(sfLines, previousPointCoordinate, perpendicularCoordinate2);

                // Create polygon from two perpendicular lines:
                var numShapes = sfLines.NumShapes;
                if (numShapes >= 4)
                {
                    Debug.WriteLine("numShapes: " + numShapes);
                    // Get first perpendicular line:
                    var shpLine1 = sfLines.Shape[numShapes - 4];
                    var point1_1 = shpLine1.Point[0];
                    var point1_2 = shpLine1.Point[1];
                    var shpLine3 = sfLines.Shape[numShapes - 2];
                    var point3_1 = shpLine3.Point[0];
                    var point3_2 = shpLine3.Point[1];

                    AddToPolygonSf(sfPolygons,
                        new Helper.Coordinate(point1_1.x, point1_1.y),
                        new Helper.Coordinate(point1_2.x, point1_2.y),
                        new Helper.Coordinate(point3_2.x, point3_2.y),
                        new Helper.Coordinate(point3_1.x, point3_1.y));

                    // Other side:
                    var shpLine2 = sfLines.Shape[numShapes - 3];
                    var point2_1 = shpLine2.Point[0];
                    var point2_2 = shpLine2.Point[1];
                    var shpLine4 = sfLines.Shape[numShapes - 1];
                    var point4_1 = shpLine4.Point[0];
                    var point4_2 = shpLine4.Point[1];
                    AddToPolygonSf(sfPolygons,
                        new Helper.Coordinate(point2_1.x, point2_1.y),
                        new Helper.Coordinate(point2_2.x, point2_2.y),
                        new Helper.Coordinate(point4_2.x, point4_2.y),
                        new Helper.Coordinate(point4_1.x, point4_1.y));
                }

                segmentLength += distance;

                // Debug:
                if (sfPolygons.NumShapes >= 33) break;
            }

            // TODO: Check last point

            sfTracks.Close();
            Helper.SaveAsShapefile(sfPoints, Path.Combine(tempPath, "InterpolatedPoints.shp"));
            Helper.SaveAsShapefile(sfLines, Path.Combine(tempPath, "PerpendicularLines.shp"));
            Helper.SaveAsShapefile(sfPolygons, Path.Combine(tempPath, "SprayAreas.shp"));
        }

        [TestMethod]
        public void CreateRepeatingLabels()
        {
            const string filename = @"D:\dev\GIS-data\Joe Rose\Labeling\Contours.shp";
            var sf = Helper.OpenShapefile(filename, true, this);
            sf.Labels.SavingMode = tkSavingMode.modeXMLOverwrite;
            // To test, save new points to new shapefile as well:
            var sfPoints = Helper.CreateSf(ShpfileType.SHP_POINT);

            // To test, get a long shape (long polyline)
            var shp = sf.Shape[291];
            // Get first point
            var firstPoint = shp.Point[0];
            const double distance = 0.005; // Without knowing the projection it is hard to set a correct distance
            var numLabels = (int)Math.Floor(shp.Length / distance);
            Debug.WriteLine($"Making {numLabels} labels for this shape");
            for (var i = 0; i < numLabels; i++)
            {
                // Get next point on the line 
                var nextPoint = shp.InterpolatePoint(firstPoint, distance * (i + 1));
                Helper.AddPointToPointSf(sfPoints, nextPoint);
                sf.Labels.AddLabel("MyLabel " + i, nextPoint.x, nextPoint.y);
            }
            sf.Labels.SaveToXML(Path.ChangeExtension(filename, ".lbl"));
            Helper.SaveAsShapefile(sfPoints, Path.ChangeExtension(filename, "points.shp"));
        }

        private static Extents PointToExtent(IPoint nextPoint, double enlargeValue)
        {
            var extent = new Extents();
            extent.SetBounds(nextPoint.x - enlargeValue, nextPoint.y - enlargeValue, 0,
                nextPoint.x + enlargeValue, nextPoint.y + enlargeValue, 0);
            return extent;
        }

        private static double GetAngle(IUtils utils, Point firstPoint, Point secondPoint)
        {
            var angle = utils.GetAngle(firstPoint, secondPoint);
            // convert to proper Cartesian angle of rotation
            angle = (450 - angle) % 360;
            // convert to Radians
            angle = angle * Math.PI / 180.0;

            // Get perpendicular angle:
            angle += 90 * Math.PI / 180.0;
            if (angle > 2 * Math.PI) angle -= 2 * Math.PI;

            return angle;
        }

        public static void AddCoordinatesToLineSf(Shapefile sfLines, Helper.Coordinate coordinate1,
            Helper.Coordinate coordinate2)
        {
            var shp = new Shape();
            if (!shp.Create(ShpfileType.SHP_POLYLINE))
                throw new Exception("Error in creating shape. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            if (shp.AddPoint(coordinate1.X, coordinate1.Y) < 0)
                throw new Exception("Error in adding point. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            if (shp.AddPoint(coordinate2.X, coordinate2.Y) < 0)
                throw new Exception("Error in adding point. Error: " + shp.ErrorMsg[shp.LastErrorCode]);

            // Check if this line intersects other lines, if so shorten to intersection point:
            var numShapes = sfLines.NumShapes;
            for (var i = 0; i < numShapes; i++)
            {
                var shpTesting = sfLines.Shape[i];
                if (!shpTesting.Crosses(shp)) continue;

                var newCoordinate = GetIntersection(Helper.PointToCoordinate(shp.Point[0]),
                    Helper.PointToCoordinate(shp.Point[1]), Helper.PointToCoordinate(shpTesting.Point[0]),
                    Helper.PointToCoordinate(shpTesting.Point[1]));
                // Replace point:
                shp.Point[1] = Helper.CoordinateToPoint(newCoordinate);
            }

            if (sfLines.EditAddShape(shp) < 0)
                throw new Exception("Error in adding shape. Error: " + sfLines.ErrorMsg[sfLines.LastErrorCode]);
        }

        private static void AddToPolygonSf(IShapefile sfPolygons, Helper.Coordinate coordinate1_1,
            Helper.Coordinate coordinate1_2, Helper.Coordinate coordinate2_1, Helper.Coordinate coordinate2_2)
        {
            var shp = new Shape();
            if (!shp.Create(ShpfileType.SHP_POLYGON))
                throw new Exception("Error in creating shape. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            if (shp.AddPoint(coordinate1_1.X, coordinate1_1.Y) < 0)
                throw new Exception("Error in adding point. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            if (shp.AddPoint(coordinate1_2.X, coordinate1_2.Y) < 0)
                throw new Exception("Error in adding point. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            if (shp.AddPoint(coordinate2_1.X, coordinate2_1.Y) < 0)
                throw new Exception("Error in adding point. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            if (shp.AddPoint(coordinate2_2.X, coordinate2_2.Y) < 0)
                throw new Exception("Error in adding point. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            // Closing:
            if (shp.AddPoint(coordinate1_1.X, coordinate1_1.Y) < 0)
                throw new Exception("Error in adding point. Error: " + shp.ErrorMsg[shp.LastErrorCode]);
            if (!shp.PartIsClockWise[0]) shp.ReversePointsOrder(0);
            if (!shp.IsValid)
            {
                shp = shp.FixUp2(tkUnitsOfMeasure.umMeters);
                if (shp == null) return;
                if (!shp.IsValid)
                    throw new Exception("Error: shape is not valid. " + shp.IsValidReason);
            }

            // Check if this new shape is overlapping other shapes, 
            // if so clip and add that version instead:
            var numShapes = sfPolygons.NumShapes;
            for (var i = 0; i < numShapes; i++)
            {
                var shpTesting = sfPolygons.Shape[i];
                // If within, don't add again:
                if (shp.Within(shpTesting))
                {
                    Debug.WriteLine("Shape is within " + i);
                    return;
                }
                // If overlaps, add only new part:
                if (shp.Overlaps(shpTesting))
                {
                    Debug.WriteLine(i + " overlaps. Touches: " + shp.Touches(shpTesting));
                    // TODO: Returns wrong part:
                    //shp = shpTesting.Clip(shp, tkClipOperation.clDifference);
                    //if (shp == null) return;
                }
            }

            if (shp.ShapeType2D != ShpfileType.SHP_POLYGON) return;

            if (!shp.PartIsClockWise[0]) shp.ReversePointsOrder(0);
            if (!shp.IsValid)
            {
                shp = shp.FixUp2(tkUnitsOfMeasure.umMeters);
                if (shp == null) return;
                if (!shp.IsValid)
                    throw new Exception("Error: shape is not valid. " + shp.IsValidReason);
            }


            if (shp.ShapeType2D != ShpfileType.SHP_POLYGON) return;
            if (sfPolygons.EditAddShape(shp) < 0)
                throw new Exception("Error in adding shape. Error: " + sfPolygons.ErrorMsg[sfPolygons.LastErrorCode]);

        }

        private static Helper.Coordinate GetIntersection(
            Helper.Coordinate p1,
            Helper.Coordinate p2,
            Helper.Coordinate p3,
            Helper.Coordinate p4)
        {
            // http://csharphelper.com/blog/2014/08/determine-where-two-lines-intersect-in-c/

            // Get the segments' parameters.
            var dx12 = p2.X - p1.X;
            var dy12 = p2.Y - p1.Y;
            var dx34 = p4.X - p3.X;
            var dy34 = p4.Y - p3.Y;

            // Solve for t1 and t2
            var denominator = dy12 * dx34 - dx12 * dy34;

            var t1 = ((p1.X - p3.X) * dy34 + (p3.Y - p1.Y) * dx34) / denominator;

            if (double.IsInfinity(t1))
            {
                // The lines are parallel (or close enough to it).
                throw new ArgumentException("Lines are parallel");
            }

            // Find the point of intersection.
            return new Helper.Coordinate(p1.X + dx12 * t1, p1.Y + dy12 * t1);
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
