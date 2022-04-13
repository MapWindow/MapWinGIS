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
        private static readonly GlobalSettings _settings = new GlobalSettings();

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
                var numPoints = shape.NumPoints;
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
            var numPoints = shape.NumPoints;
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
        public void TestPointInSimplePolygon()
        {
            RunPointInPolygonTests(Path.Combine(@"sf", "SHP_POLYGON.shp"),
                Helper.MakePoint(6.37464034466582, 52.4102411020024),
                Helper.MakePoint(6.37448647420611, 52.410194349055));
        }

        [TestMethod]
        public void TestPointInPolygonZ()
        {
            RunPointInPolygonTests(Path.Combine(@"sf", "SHP_POLYGONZ.shp"),
                Helper.MakePoint(-102.519749544454, 46.8429408895592),
                Helper.MakePoint(-102.51917417091, 46.8431631610016));
        }

        [TestMethod]
        public void TestPointInPolygonM()
        {
            RunPointInPolygonTests(Path.Combine(@"sf", "SHP_POLYGONM.shp"),
                Helper.MakePoint(-51.9355346789247, -23.4136868836559),
                Helper.MakePoint(-51.9147302015054, -23.4096941051613));
        }

        [TestMethod]
        public void TestPointInLotsOfPolygons()
        {
            RunPointInPolygonTests(Path.Combine(@"sf", "LotsOfPolygons.shp"),
                Helper.MakePoint(321273.225450182, 5857301.96614617),
                Helper.MakePoint(321451.30034346, 5857310.8255936));
        }

        [TestMethod]
        public void TestPointInPolygonWithHole()
        {
            RunPointInPolygonTests(Path.Combine(@"sf", "polygonWithHole.shp"),
                Helper.MakePoint(265273.647149635, 500006.809897344),
                Helper.MakePoint(265559.247149635, 500252.089897344));
        }

        [TestMethod]
        public void TestPointInMultiPolygon()
        {
            RunPointInPolygonTests(Path.Combine(@"sf", "MultiPolygon.shp"),
                Helper.MakePoint(268484.127149635, 499948.009897344),
                Helper.MakePoint(267684.447149635, 500418.409897344));
        }


        private static void RunPointInPolygonTests(string sfPolygonFilename, Point pointInside, Point pointOutside)
        {
            // Shapefile.PointInShape, Shapefile.PointInShapefile, Utils.PointInPolygon, and Shapefile.SelectByShapefile (using srContains or srWithin).
            var sfPolygon = Helper.OpenShapefile(sfPolygonFilename);

            var stopwatch = new Stopwatch();
            stopwatch.Start();
            var numErrors = UsePointInShapefile(sfPolygon, pointInside, pointOutside);
            stopwatch.Stop();
            Debug.WriteLine("UsePointInShapefile took " + stopwatch.Elapsed);

            stopwatch.Restart();
            numErrors += UsePointInShape(sfPolygon, pointInside, pointOutside);
            stopwatch.Stop();
            Debug.WriteLine("UsePointInShape took " + stopwatch.Elapsed);

            stopwatch.Restart();
            numErrors += UsePointInPolygon(sfPolygon, pointInside, pointOutside);
            stopwatch.Stop();
            Debug.WriteLine("UsePointInPolygon took " + stopwatch.Elapsed);

            stopwatch.Restart();
            numErrors += UseGetRelatedShapes2(sfPolygon, pointInside, pointOutside);
            stopwatch.Stop();
            Debug.WriteLine("UseGetRelatedShapes2 took " + stopwatch.Elapsed);

            stopwatch.Restart();
            numErrors += UseSelectByShapefile(sfPolygon, pointInside, pointOutside);
            stopwatch.Stop();
            Debug.WriteLine("UseSelectByShapefile took " + stopwatch.Elapsed);

            if (!sfPolygon.Close())
            {
                Debug.WriteLine("Error! Cannot close polygon shapefile: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);
            }

            Assert.IsTrue(numErrors == 0, "Errors were found");
        }

        private static int UseSelectByShapefile(IShapefile sfPolygon, IPoint pointInside, IPoint pointOutside)
        {
            var numErrors = 0;
            object resultArray = null;
            var sfInside = new Shapefile();
            sfInside.CreateNewWithShapeID("", ShpfileType.SHP_POINT);
            var shpInside = new Shape();
            shpInside.Create(ShpfileType.SHP_POINT);
            shpInside.AddPoint(pointInside.x, pointInside.y);
            sfInside.EditAddShape(shpInside);
            
            if (!sfPolygon.SelectByShapefile(sfInside, tkSpatialRelation.srContains, false, ref resultArray))
            {
                Debug.WriteLine("Error! No shapes found");
                numErrors++;
            }

            var sfOutside = new Shapefile();
            sfOutside.CreateNewWithShapeID("", ShpfileType.SHP_POINT);
            var shpOutside = new Shape();
            shpOutside.Create(ShpfileType.SHP_POINT);
            shpOutside.AddPoint(pointOutside.x, pointOutside.y);
            sfOutside.EditAddShape(shpOutside);
            
            if (sfPolygon.SelectByShapefile(sfOutside, tkSpatialRelation.srContains, false, ref resultArray))
            {
                Debug.WriteLine("Error! Shapes found, this is unexpected");
                numErrors++;
            }

            return numErrors;
        }

        private static int UseGetRelatedShapes2(IShapefile sfPolygon, IPoint pointInside, IPoint pointOutside)
        {
            var numErrors = 0;
            object resultArray = null;

            var shpInside = new Shape();
            shpInside.Create(ShpfileType.SHP_POINT);
            shpInside.AddPoint(pointInside.x, pointInside.y);
            if (!sfPolygon.GetRelatedShapes2(shpInside, tkSpatialRelation.srIntersects, ref resultArray))
            {
                Debug.WriteLine("Error! No shapes found");
                numErrors++;
            }
            
            var shpOutside = new Shape();
            shpOutside.Create(ShpfileType.SHP_POINT);
            shpOutside.AddPoint(pointOutside.x, pointOutside.y);
            if (sfPolygon.GetRelatedShapes2(shpOutside, tkSpatialRelation.srIntersects, ref resultArray))
            {
                Debug.WriteLine("Error! Shapes found, this is unexpected");
                numErrors++;
            }

            return numErrors;
        }

        private static int UsePointInPolygon(IShapefile sfPolygon, Point pointInside, Point pointOutside)
        {
            Debug.WriteLine("Testing UsePointInPolygon");
            var numErrors = 0;
            var numShapes = sfPolygon.NumShapes;
            var foundInside = false;
            var utils = new Utils();
            var stopwatch = new Stopwatch();
            stopwatch.Start();

            for (var i = 0; i < numShapes; i++)
            {
                var shp = sfPolygon.Shape[i];
                if (!utils.PointInPolygon(shp, pointInside)) continue;

                foundInside = true;
                Debug.WriteLine("Inside point is in shape " + i);
                break;
            }
            stopwatch.Stop();
            Debug.WriteLine("Searching for inside point took " + stopwatch.Elapsed);

            if (!foundInside)
            {
                Debug.WriteLine("Error! Inside point is not in any shape");
                numErrors++;
            }

            stopwatch.Restart();
            for (var i = 0; i < numShapes; i++)
            {
                var shp = sfPolygon.Shape[i];
                if (!utils.PointInPolygon(shp, pointOutside)) continue;

                Debug.WriteLine("Error! Outside point is in shape " + i);
                numErrors++;
                break;
            }
            stopwatch.Stop();
            Debug.WriteLine("Searching for outside point took " + stopwatch.Elapsed);            

            return numErrors;
        }

        private static int UsePointInShapefile(IShapefile sfPolygon,  IPoint pointInside, IPoint pointOutside)
        {
            Debug.WriteLine("Testing PointInShapefile");
            var numErrors = 0;
            var stopwatch = new Stopwatch();
            stopwatch.Start();
            if (!sfPolygon.BeginPointInShapefile())
            {
                Debug.WriteLine("Error in BeginPointInShapefile: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);
                numErrors++;
            }

            stopwatch.Stop();
            Debug.WriteLine("BeginPointInShapefile took " + stopwatch.Elapsed);

            stopwatch.Restart();
            var selectedShapeInside = sfPolygon.PointInShapefile(pointInside.x, pointInside.y);
            stopwatch.Stop();
            Debug.WriteLine("PointInShapefile inside point took " + stopwatch.Elapsed);
            if (selectedShapeInside == -1)
            {
                Debug.WriteLine("Error! No shape found with inside point");
                numErrors++;
            }
            else
            {
                Debug.WriteLine("Point is in shape " + selectedShapeInside);
            }

            stopwatch.Restart();
            var selectedShapeOutside = sfPolygon.PointInShapefile(pointOutside.x, pointOutside.y);
            stopwatch.Stop();
            Debug.WriteLine("PointInShapefile outside point took " + stopwatch.Elapsed);
            if (selectedShapeOutside != -1)
            {
                Debug.WriteLine("Error! Shape found with outside point, this is unexpexted");
                numErrors++;
            }

            sfPolygon.EndPointInShapefile();

            return numErrors;
        }

        private static int UsePointInShape(IShapefile sfPolygon, IPoint pointInside, IPoint pointOutside)
        {
            Debug.WriteLine("Testing PointInShape");
            var numErrors = 0;
            var numShapes = sfPolygon.NumShapes;
            var foundInside = false;
            var stopwatch = new Stopwatch();
            stopwatch.Start();

            for (var i = 0; i < numShapes; i++)
            {
                if (!sfPolygon.PointInShape(i, pointInside.x, pointInside.y)) continue;

                foundInside = true;
                Debug.WriteLine("Inside point is in shape " + i);
                break;
            }
            stopwatch.Stop();
            Debug.WriteLine("Searching for inside point took " + stopwatch.Elapsed);

            if (!foundInside)
            {
                Debug.WriteLine("Error! Inside point is not in any shape");
                numErrors++;
            }

            stopwatch.Restart();
            for (var i = 0; i < numShapes; i++)
            {
                if (!sfPolygon.PointInShape(i, pointOutside.x, pointOutside.y)) continue;

                Debug.WriteLine("Error! Outside point is in shape " + i);
                numErrors++;
                break;
            }
            stopwatch.Stop();
            Debug.WriteLine("Searching for outside point took " + stopwatch.Elapsed);

            return numErrors;
        }

    }
}