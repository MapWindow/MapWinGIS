using System;
using System.Diagnostics;
using System.IO;
using AxMapWinGIS;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class ShapefileEditorTests
    {
        private AxMap _axMap1;

        [TestInitialize]
        public void Init()
        {
            Helper.DebugMsg("Test was run at " + DateTime.Now);
            _axMap1 = Helper.GetAxMap(false);
        }

        [TestMethod]
        public void Simplify()
        {
            var utils = new Utils();
            const string workingFolder = @"D:\dev\GIS-data\Joe Rose\Ayrshire\";

            var sfOriginal = Helper.OpenShapefile(Path.Combine(workingFolder, "Parish_Kirkoswald.shp"));
            Debug.WriteLine($"Original has {sfOriginal.NumShapes} shapes");
            Debug.WriteLine($"Original first shape has {sfOriginal.Shape[0].NumPoints} points.");

            var area = sfOriginal.Shape[0].Area;
            utils.ConvertDistance(sfOriginal.GeoProjection.LinearUnits, tkUnitsOfMeasure.umMeters, ref area);
            Debug.WriteLine($"Original first shape has an area of {area}");

            var sfSimplifyLines = sfOriginal.SimplifyLines(0d, false);
            Debug.WriteLine($"Simplified (0 tolerance) has {sfSimplifyLines.NumShapes} shapes");
            Debug.WriteLine($"Simplified (0 tolerance) first shape has {sfSimplifyLines.Shape[0].NumPoints} points.");
            sfSimplifyLines.Close();


            var tolerance = 1d;
            utils.ConvertDistance(tkUnitsOfMeasure.umMeters, sfOriginal.GeoProjection.LinearUnits, ref tolerance);
            Debug.WriteLine("Tolerance in map units: " + tolerance);

            sfSimplifyLines = sfOriginal.SimplifyLines(tolerance, false);
            Debug.WriteLine($"Simplified (1m tolerance) has {sfSimplifyLines.NumShapes} shapes");
            Debug.WriteLine($"Simplified (1m tolerance) first shape has {sfSimplifyLines.Shape[0].NumPoints} points.");

            area = sfOriginal.Shape[0].Area;
            utils.ConvertDistance(sfOriginal.GeoProjection.LinearUnits, tkUnitsOfMeasure.umMeters, ref area);
            Debug.WriteLine($"Simplified (1m tolerance) has an area of {area}");

            // sfSimplifyLines.SaveAs(Path.Combine(WorkingFolder, "Simplified-1m.shp"));
            sfSimplifyLines.Close();

            sfOriginal.Close();
        }

        [TestMethod]
        public void EditAddShapeTest()
        {
            // https://mapwindow.discourse.group/t/system-accessviolationexception-after-editaddshape/196
            var pnt = new Point { x = 6.7369281, y = 53.1603648 };

            var shp = new Shape();
            shp.Create(ShpfileType.SHP_POLYGON);
            shp.InsertPoint(pnt, 0);

            var sf = new Shapefile();
            var result = sf.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);
            Assert.IsTrue(result, "CreateNewWithShapeID failed: " + sf.ErrorMsg[sf.LastErrorCode]);

            var shpIndex = sf.EditAddShape(shp);
            Assert.IsTrue(shpIndex > -1, "EditAddShape failed: " + sf.ErrorMsg[sf.LastErrorCode]);

            sf.InteractiveEditing = true;
            Assert.IsTrue(sf.InteractiveEditing, "InteractiveEditing failed: " + sf.ErrorMsg[sf.LastErrorCode]);

            var layerIndex = _axMap1.AddLayer(sf, true);
            Assert.IsTrue(layerIndex > -1, "AddLayer failed: " + sf.ErrorMsg[sf.LastErrorCode]);

            result = _axMap1.ShapeEditor.StartEdit(layerIndex, shpIndex);
            Assert.IsTrue(result, "ShapeEditor.StartEdit failed: " + sf.ErrorMsg[sf.LastErrorCode]);
        }
    }
}
