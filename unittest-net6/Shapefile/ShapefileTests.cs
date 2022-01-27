namespace unittest_net6.Shapefile;

public class ShapefileTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public ShapefileTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void CreatePolygonShapefileTest()
    {
        var sfPolygon = new MapWinGIS.Shapefile();
        Assert.NotNull(sfPolygon);
        var retVal = sfPolygon.CreateNewWithShapeID("", ShpfileType.SHP_POLYGON);
        Assert.True(retVal, "sfPolygon.CreateNewWithShapeID() failed");

        // Check:
        Assert.Equal(ShpfileType.SHP_POLYGON, sfPolygon.ShapefileType);

        // Create shape
        var shp = new Shape();
        Assert.NotNull(shp);
        retVal = shp.ImportFromWKT(
            "POLYGON((330695.973322992 5914896.16305817, 330711.986129861 5914867.19586245, 330713.350435287 5914867.56644015, 330716.510827627 5914862.28973662, 330715.632568651 5914860.60107999, 330652.234582712 5914803.80510632, 330553.749382483 5914715.80328169, 330551.979355848 5914714.83347535, 330549.911988583 5914715.86502807, 330545.027807355 5914724.05916443, 330544.592985976 5914725.93531509, 330544.30963704 5914726.72754692, 330543.612620707 5914726.14904553, 330543.271515787 5914727.06633931, 330542.234090059 5914729.85597723, 330542.959654761 5914730.50411962, 330530.319252794 5914765.86064153, 330505.294840402 5914836.7930124, 330471.411812074 5914931.61558331, 330486.074748666 5914941.33795239, 330585.983154737 5915010.32749106, 330618.427962455 5915031.20447119, 330653.234601917 5914970.37328093, 330695.973322992 5914896.16305817))");
        Assert.True(retVal, "shp.ImportFromWKT() failed");

        // Add shape to shapefile:
        var newIndex = sfPolygon.EditAddShape(shp);
        Assert.NotEqual(-1, newIndex);

        // Checks:
        Assert.Equal(ShpfileType.SHP_POLYGON, shp.ShapeType);
        Assert.Equal(1, shp.NumParts);
        Assert.Equal(1, sfPolygon.NumShapes);

        // Create some random points:
        var random = new Random();
        var extents = sfPolygon.Extents;
        Assert.NotNull(extents);
        var width = extents.xMax - extents.xMin;
        var height = extents.yMax - extents.yMin;

        var sfPoint = new MapWinGIS.Shapefile();
        Assert.NotNull(sfPoint);
        retVal = sfPoint.CreateNewWithShapeID("", ShpfileType.SHP_POINT);
        Assert.True(retVal, "sfPoint.CreateNewWithShapeID failed");

        const int numPoints = 50;
        for (var i = 0; i < numPoints; i++)
        {
            var x = extents.xMin + width * random.NextDouble();
            var y = extents.yMin + height * random.NextDouble();
            var shpPoint = new Shape();
            Assert.NotNull(shpPoint);
            retVal = shpPoint.Create(ShpfileType.SHP_POINT);
            Assert.True(retVal);
            newIndex = shpPoint.AddPoint(x, y);
            Assert.NotEqual(-1, newIndex);
            newIndex = sfPoint.EditAddShape(shpPoint);
            Assert.NotEqual(-1, newIndex);
        }

        // Checks:
        Assert.Equal(ShpfileType.SHP_POINT, sfPoint.ShapefileType);
        Assert.Equal(numPoints, sfPoint.NumShapes);

        _testOutputHelper.WriteLine("CreatePolygonShapefileTest finished");
    }
}
