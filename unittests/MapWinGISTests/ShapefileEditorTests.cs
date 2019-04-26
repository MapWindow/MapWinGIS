using System.Diagnostics;
using System.IO;
using MapWinGIS;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MapWinGISTests
{
    [TestClass]
    public class ShapefileEditorTests
    {
        private const string WorkingFolder = @"D:\dev\GIS-data\Joe Rose\Ayrshire\";

        [TestMethod]
        public void Simplify()
        {
            var utils = new Utils();

            var sfOriginal = Helper.OpenShapefile(Path.Combine(WorkingFolder, "Parish_Kirkoswald.shp"));
            Debug.WriteLine($"Original has {sfOriginal.NumShapes} shapes");
            Debug.WriteLine($"Original first shape has {sfOriginal.Shape[0].numPoints} points.");

            var area = sfOriginal.Shape[0].Area;
            utils.ConvertDistance(sfOriginal.GeoProjection.LinearUnits, tkUnitsOfMeasure.umMeters, ref area);
            Debug.WriteLine($"Original first shape has an area of {area}");

            var sfSimplifyLines = sfOriginal.SimplifyLines(0d, false);
            Debug.WriteLine($"Simplified (0 tolerance) has {sfSimplifyLines.NumShapes} shapes");
            Debug.WriteLine($"Simplified (0 tolerance) first shape has {sfSimplifyLines.Shape[0].numPoints} points.");
            sfSimplifyLines.Close();


            var tolerance = 1d;
            utils.ConvertDistance(tkUnitsOfMeasure.umMeters, sfOriginal.GeoProjection.LinearUnits, ref tolerance);
            Debug.WriteLine("Tolerance in map units: " + tolerance);

            sfSimplifyLines = sfOriginal.SimplifyLines(tolerance, false);
            Debug.WriteLine($"Simplified (1m tolerance) has {sfSimplifyLines.NumShapes} shapes");
            Debug.WriteLine($"Simplified (1m tolerance) first shape has {sfSimplifyLines.Shape[0].numPoints} points.");

            area = sfOriginal.Shape[0].Area;
            utils.ConvertDistance(sfOriginal.GeoProjection.LinearUnits, tkUnitsOfMeasure.umMeters, ref area);
            Debug.WriteLine($"Simplified (1m tolerance) has an area of {area}");

            // sfSimplifyLines.SaveAs(Path.Combine(WorkingFolder, "Simplified-1m.shp"));
            sfSimplifyLines.Close();

            sfOriginal.Close();
        }
    }
}
