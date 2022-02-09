namespace unittest_net6.Shapefile;

public class ShapefileTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public ShapefileTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void CreatePolygonShapefileFromWktTest()
    {
        // Create shapefile:
        var sfPolygon = Helpers.MakeShapefile(ShpfileType.SHP_POLYGON);
        // Add shape to shapefile:
        Helpers.AddShape(sfPolygon, "POLYGON((330695.973322992 5914896.16305817, 330711.986129861 5914867.19586245, 330713.350435287 5914867.56644015, 330716.510827627 5914862.28973662, 330715.632568651 5914860.60107999, 330652.234582712 5914803.80510632, 330553.749382483 5914715.80328169, 330551.979355848 5914714.83347535, 330549.911988583 5914715.86502807, 330545.027807355 5914724.05916443, 330544.592985976 5914725.93531509, 330544.30963704 5914726.72754692, 330543.612620707 5914726.14904553, 330543.271515787 5914727.06633931, 330542.234090059 5914729.85597723, 330542.959654761 5914730.50411962, 330530.319252794 5914765.86064153, 330505.294840402 5914836.7930124, 330471.411812074 5914931.61558331, 330486.074748666 5914941.33795239, 330585.983154737 5915010.32749106, 330618.427962455 5915031.20447119, 330653.234601917 5914970.37328093, 330695.973322992 5914896.16305817))");
        // Check:
        sfPolygon.NumShapes.ShouldBe(1);

        // Create some random points:
        var random = new Random();
        var extents = sfPolygon.Extents;
        extents.ShouldNotBeNull();
        var width = extents.xMax - extents.xMin;
        var height = extents.yMax - extents.yMin;

        // Create shapefile:
        var sfPoint = Helpers.MakeShapefile(ShpfileType.SHP_POINT);

        const int numPoints = 50;
        for (var i = 0; i < numPoints; i++)
        {
            var x = extents.xMin + width * random.NextDouble();
            var y = extents.yMin + height * random.NextDouble();
            Helpers.AddPointShape(sfPoint, x, y);
        }

        // Checks:
        sfPoint.ShapefileType.ShouldBe(ShpfileType.SHP_POINT);
        sfPoint.NumShapes.ShouldBe(numPoints);
    }

    [Fact]
    public void CreatePolygonShapefileTest()
    {
        // Also used the check if the scalebar is correct (MWGIS-276)

        // Create shapefile:
        var sfPolygon = Helpers.MakeShapefile(ShpfileType.SHP_POLYGON);
        // Set projection:
        sfPolygon.GeoProjection = Helpers.MakeProjection(28992);
        // Add shape:
        var shp = Helpers.MakeShape(ShpfileType.SHP_POLYGON);
        // Add points to create a square:
        var pointIndex = shp.AddPoint(187810, 433910);
        pointIndex.ShouldBe(0, "Invalid point index");
        shp.AddPoint(187810, 433910 - 50);
        shp.AddPoint(187810 - 50, 433910 - 50);
        shp.AddPoint(187810 - 50, 433910);
        shp.AddPoint(187810, 433910); // Repeat first point to close
        shp.IsValid.ShouldBeTrue(shp.IsValidReason);

        var shapeIndex = sfPolygon.EditAddShape(shp);
        shapeIndex.ShouldNotBe(-1, "EditAddShape failed");

        sfPolygon.HasInvalidShapes().ShouldBeFalse("sfPolygon.HasInvalidShapes()");

        // Save shapefile:
        var baseFileName = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(Path.GetRandomFileName()));
        var fileName = Path.ChangeExtension($"{baseFileName}-28992", ".shp");
        var retVal = sfPolygon.SaveAs(fileName);
        retVal.ShouldBeTrue("sfPolygon.SaveAs failed");
        _testOutputHelper.WriteLine("Saved " + fileName);

        // Reproject this shapefile to WGS84:
        var newFileName = Path.ChangeExtension($"{baseFileName}-4326", ".shp");
        var u = new MapWinGIS.GdalUtils();
        retVal = u.GdalVectorReproject(fileName, newFileName, 28992, 4326);
        retVal.ShouldBeTrue("GdalVectorReproject failed");
        _testOutputHelper.WriteLine("Saved " + newFileName);

        // Reproject to Google Mercator:
        newFileName = Path.ChangeExtension($"{baseFileName}-3857", ".shp");
        retVal = u.GdalVectorReproject(fileName, newFileName, 28992, 3857);
        retVal.ShouldBeTrue("GdalVectorReproject failed");
        _testOutputHelper.WriteLine("Saved " + newFileName);
    }

    [Fact]
    public void SaveSfAndCheckProjection()
    {
        // Create shapefile:
        var sf = Helpers.MakeShapefile(ShpfileType.SHP_POINT);
        // Set projection:
        sf.GeoProjection = Helpers.MakeProjection(28992);
        // Add shape:
        var shp = Helpers.MakeShape(ShpfileType.SHP_POINT);
        // Add points to create a square:
        var pointIndex = shp.AddPoint(187810, 433910);
        pointIndex.ShouldBe(0, "Invalid point index");

        var shapeIndex = sf.EditAddShape(shp);
        shapeIndex.ShouldNotBe(-1, "EditAddShape failed");

        sf.HasInvalidShapes().ShouldBeFalse("sf.HasInvalidShapes()");

        // Save shapefile:
        var baseFileName = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(Path.GetRandomFileName()));
        var fileName = Path.ChangeExtension($"{baseFileName}-28992", ".shp");
        var retVal = sf.SaveAs(fileName);
        retVal.ShouldBeTrue("sf.SaveAs failed");
        _testOutputHelper.WriteLine("Saved " + fileName);

        // Close shapefile:
        retVal = sf.Close();
        retVal.ShouldBeTrue("sf.Close failed");

        // Reopen shapefile:
        var sf2 = new MapWinGIS.Shapefile();
        retVal = sf2.Open(fileName);
        retVal.ShouldBeTrue("sf2.Open failed");

        // Get EpsgCode:
        retVal = sf2.GeoProjection.TryAutoDetectEpsg(out var epsgCode);
        retVal.ShouldBeTrue("TryAutoDetectEpsg failed");
        epsgCode.ShouldBe(28992);
        _testOutputHelper.WriteLine(sf2.GeoProjection.ExportToWktEx());
    }

    [Fact]
    public void ReprojectShapefileTest()
    {
        // See also UtilsProjectionTests.UtilsReprojectShapefileTest()

        ReprojectShapefile(187816.756, 433912.801, 5.86394184, 51.89276528, 0.0000001, 28992, 4326);
        // The other way round:
        ReprojectShapefile(5.86394184, 51.89276528, 187816.756, 433912.801, 0.05, 4326, 28992);

        // Amersfoort Geodetic to ETRS89 (EPSG:4258)
        ReprojectShapefile(4.5703125, 51.944265, 4.5706292, 51.945227, 0.000001, 4258, 4289);
        // Swap:
        ReprojectShapefile(4.5706292, 51.945227, 4.5703125, 51.944265, 0.000001, 4289, 4258);
    }

    private void ReprojectShapefile(double srcX, double srcY, double dstX, double dstY, double tolerance, int srcEpsgCode, int dstEpsgCode)
    {
        // Create shapefile:
        var sfPoint = Helpers.MakeShapefile(ShpfileType.SHP_POINT);
        // Set projection
        sfPoint.GeoProjection = Helpers.MakeProjection(srcEpsgCode);
        // Add shape:
        Helpers.AddPointShape(sfPoint, srcX, srcY);

        var count = 0;
        var sfNew = sfPoint.Reproject(Helpers.MakeProjection(dstEpsgCode), ref count);

        // Checks:
        sfNew.ShouldNotBeNull("ReprojectShapefile failed");
        sfNew.ShapefileType.ShouldBe(ShpfileType.SHP_POINT);
        sfNew.NumShapes.ShouldBe(1);
        count.ShouldBe(1);
        var shpNew = sfNew.Shape[0];
        shpNew.ShouldNotBeNull();
        var pntNew = shpNew.Point[0];
        pntNew.ShouldNotBeNull();
        _testOutputHelper.WriteLine("X: {0}, Y: {1}", pntNew.x, pntNew.y);
        pntNew.x.ShouldBe(dstX, tolerance);
        pntNew.y.ShouldBe(dstY, tolerance);

    }
}
