using System;
using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    [DeploymentItem("Testdata")]
    public class GridTests
    {
        [TestMethod]
        public void ChangeValueGridNotInRam()
        {
            ChangeValueGrid(false);
        }

        [TestMethod]
        public void ChangeValueGridInRam()
        {
            ChangeValueGrid(true);
        }

        public void ChangeValueGrid(bool useInRam)
        {
            // https://mapwindow.atlassian.net/browse/MWGIS-94
            const string gridFilename = @"Issues\MWGIS-94\test.asc";
            Assert.IsTrue(File.Exists(gridFilename), "The input file doesn't exist!");

            // Open grid:
            var grd = new Grid();
            var retVal = grd.Open(gridFilename, GridDataType.UnknownDataType, useInRam);
            Assert.IsTrue(retVal, "Cannot open grid file. Error: " + grd.ErrorMsg[grd.LastErrorCode]);
            Console.WriteLine(grd.DataType);
            var nodataValue = (double)grd.Header.NodataValue;
            var numCols = grd.Header.NumberCols;
            var numRows = grd.Header.NumberRows;

            var col = 0;
            var row = 0;
            var found = false;
            for (var i = 0; i < numRows; i++)
            {
                if (found) break;

                for (var j = 0; j < numCols; j++)
                {
                    var value = (double)grd.Value[j, i];
                    Debug.WriteLine(value);
                    if (value.Equals(nodataValue)) continue;
                    if (value < -50) continue;

                    // Found a good value
                    col = j;
                    row = i;
                    found = true;
                    break;
                }
            }

            var gridValue = grd.Value[col, row];
            Console.WriteLine("Original grid value: " + gridValue);

            const double newValue = 3;
            grd.Value[col, row] = newValue;

            // Save grid:
            var newFileLocation = Path.Combine(Path.GetTempPath(), "ChangeValueGrid.asc");
            if (File.Exists(newFileLocation)) File.Delete(newFileLocation);
            retVal = grd.Save(newFileLocation);
            Assert.IsTrue(retVal, "Cannot save grid file. Error: " + grd.ErrorMsg[grd.LastErrorCode]);
            Assert.IsTrue(File.Exists(newFileLocation), "The output file doesn't exist!");

            retVal = grd.Close();
            Assert.IsTrue(retVal, "Cannot close grid file. Error: " + grd.ErrorMsg[grd.LastErrorCode]);

            // Reopen file
            var grd2 = new Grid();
            retVal = grd2.Open(newFileLocation, GridDataType.UnknownDataType, false);
            Assert.IsTrue(retVal, "Cannot open grid file. Error: " + grd2.ErrorMsg[grd2.LastErrorCode]);

            var updatedGridValue = grd2.Value[col, row];
            Console.WriteLine("Updated grid value: " + updatedGridValue);
            Assert.AreEqual(newValue, (double)updatedGridValue, 0.001, "The update value is not equal to the new value.");
        }
    }
}
