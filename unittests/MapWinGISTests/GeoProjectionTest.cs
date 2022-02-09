using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class GeoProjectionTest
    {
        [TestMethod]
        public void TransformToFourFigureGridReference()
        {
            // Source projection:
            var sourceProjection = new GeoProjection();
            sourceProjection.ImportFromEPSG(4326); // WGS84

            // Destination projection:
            var destProjection = new GeoProjection();
            destProjection.ImportFromEPSG(27700); // OSGB 1936 / British National Grid

            // Source coordinates:
            var x = -2.02903211116781;
            var y = 53.4040442788744;

            Assert.IsTrue(sourceProjection.StartTransform(destProjection), "Cannot start transform");
            sourceProjection.Transform(ref x, ref y);
            sourceProjection.StopTransform();

            Assert.AreEqual(398167.22598, x, 0.01);
            Assert.AreEqual(389691.93091, y, 0.01);

            var gridReference = ConvertToFourFigureGridReference(x, y);
            Assert.AreEqual("SJ981896", gridReference);
            Debug.WriteLine("gridReference: " + gridReference);
        }

        [TestMethod]
        public void ImportFromEsriTest()
        {
            // As mentioned in https://mapwindow.discourse.group/t/importfromesri-crash-in-64-bits/984
            var geoProjection = new GeoProjection();
            Assert.IsNotNull(geoProjection);

            const string proj = "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";
            var retVal = geoProjection.ImportFromESRI(proj);
            Assert.IsTrue(retVal);
        }

        [TestMethod]
        public void ImportFromFile()
        {
            var geoProjection = new GeoProjection();
            Assert.IsNotNull(geoProjection);

            var prjFileLocationAmersfoort = "sf/Amersfoort.prj";
            var retVal = File.Exists(prjFileLocationAmersfoort);
            Assert.IsTrue(retVal, "prjFileLocationAmersfoort doesn't exists.");

            retVal = geoProjection.ReadFromFile(prjFileLocationAmersfoort);
            Assert.IsTrue(retVal, "geoProjection.ReadFromFile failed");
            geoProjection.TryAutoDetectEpsg(out var epsgCode);
            Assert.AreEqual(28992, epsgCode);
        }

        private string ConvertToFourFigureGridReference(double x, double y)
        {
            // Input coordinates should be in OSGB 1936 / British National Grid (EPSG:27700)

            // Converting to UK Ordnance Survey four-figure grid reference:
            // Step 1: Drop sub meter values:
            var x1 = (int)x;
            var y1 = (int)y;
            // Step 2: Get the first digit:
            var squareX = int.Parse(x1.ToString().Substring(0, 1));
            var squareY = int.Parse(y1.ToString().Substring(0, 1));
            Assert.AreEqual(3, squareX);
            Assert.AreEqual(3, squareY);
            // and the 2nd, 3rd and 4th:
            var secondPartX = x1.ToString().Substring(1, 3);
            var secondPartY = y1.ToString().Substring(1, 3);
            Assert.AreEqual("981", secondPartX);
            Assert.AreEqual("896", secondPartY);
            // Step 2: Convert the square coordinates to two character string:
            var gridLetters = GetGridReferenceLetters(squareX, squareY);
            Assert.AreEqual("SJ", gridLetters);
            // SJ 981 896
            // Step 3: Combine:
            return $"{gridLetters}{secondPartX}{secondPartY}";
        }

        private string GetGridReferenceLetters(int squareX, int squareY)
        {
            string firstLetter;
            if (squareX < 5)
            {
                firstLetter = squareY < 5 ? "S" : "T";
            }
            else if (squareX < 10)
            {
                firstLetter = squareY < 5 ? "N" : "O";
            }
            else
            {
                firstLetter = "H";
            }

            var index = 65 + ((4 - (squareX % 5)) * 5) + (squareY % 5);

            if (index >= 73)
                index++;
            var secondLetter = ((char)index).ToString();

            return firstLetter + secondLetter;
        }

    }
}

