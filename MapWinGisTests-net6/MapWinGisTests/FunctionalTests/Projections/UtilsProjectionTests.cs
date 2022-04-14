namespace MapWinGisTests.FunctionalTests.Projections;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class UtilsProjectionTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public UtilsProjectionTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void UtilsReprojectShapefileTest()
    {
        // RD (Amersfoort, The Neterlands) to WGS84:
        // https://geodata.nationaalgeoregister.nl/locatieserver/v3/free?q=a325
        // "centroide_rd": "POINT(187816.756 433912.801)",
        // "centroide_ll": "POINT(5.86394184 51.89276528)"

        UtilsReprojectPointShapefile(187816.756, 433912.801, 5.86394184, 51.89276528, 0.0000001, 28992, 4326);
        // The other way round:
        UtilsReprojectPointShapefile(5.86394184, 51.89276528, 187816.756, 433912.801, 0.05, 4326, 28992);

        // Amersfoort Geodetic to ETRS89 (EPSG:4258)
        UtilsReprojectPointShapefile(4.5703125, 51.944265, 4.5706292, 51.945227, 0.000001, 4258, 4289);
        // Swap:
        UtilsReprojectPointShapefile(4.5706292, 51.945227, 4.5703125, 51.944265, 0.000001, 4289, 4258);
    }

    private void UtilsReprojectPointShapefile(double srcX, double srcY, double dstX, double dstY, double tolerance, int srcEpsgCode, int dstEpsgCode)
    {
        // Create shapefile:
        var sfPoint = Helpers.MakeShapefile(ShpfileType.SHP_POINT);

        // Add shape:
        Helpers.AddPointShape(sfPoint, srcX, srcY);

        var geoProjectionSource = Helpers.MakeProjection(srcEpsgCode);
        var geoProjectionTarget = Helpers.MakeProjection(dstEpsgCode);

        // Test:
        var utils = new Utils();
        var sfNew = utils.ReprojectShapefile(sfPoint, geoProjectionSource, geoProjectionTarget);

        // Check
        sfNew.ShouldNotBeNull("ReprojectShapefile failed");
        sfNew.ShapefileType.ShouldBe(ShpfileType.SHP_POINT);
        sfNew.NumShapes.ShouldBe(1);
        var shpNew = sfNew.Shape[0];
        shpNew.ShouldNotBeNull();
        var pntNew = shpNew.Point[0];
        pntNew.ShouldNotBeNull();
        _testOutputHelper.WriteLine("X: {0}, Y: {1}", pntNew.x, pntNew.y);
        pntNew.x.ShouldBe(dstX, tolerance);
        pntNew.y.ShouldBe(dstY, tolerance);
    }
}
