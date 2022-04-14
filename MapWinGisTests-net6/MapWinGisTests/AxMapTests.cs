namespace MapWinGisTests;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class AxMapTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public AxMapTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }
    
    [WpfFact]
    public void VersionTest()
    {
        using var form = new WinFormsApp1.Form1();
        form.ShouldNotBeNull();

        var version = form.GetMapWinGisVersion();
        version.ShouldNotBeNull();
        version.Major.ShouldBe(5);
        version.Minor.ShouldBe(4);
        version.Build.ShouldBeGreaterThanOrEqualTo(0);
        _testOutputHelper.WriteLine("Version: {0}", version);
    }

    [WpfFact]
    public void MapProjectionTest()
    {
        using var form = new WinFormsApp1.Form1();
        form.ShouldNotBeNull();

        var sfLocation = Helpers.GetTestFilePath("UnitedStates-3857.shp");
        var layerHandle = form.OpenFile(sfLocation);
        layerHandle.ShouldNotBe(-1, "form.OpenFile failed");

        var epsgCode = form.GetMapProjectionAsEpsgCode();
        epsgCode.ShouldBe(3857);
    }

    [WpfFact]
    public void OpenShapefileWithInvalidSpatialIndex()
    {
        using var form = new WinFormsApp1.Form1();
        form.ShouldNotBeNull();

        var sfLocation = Helpers.GetTestFilePath("Issue-216.shp");
        var layerHandle = form.OpenFile(sfLocation);
        layerHandle.ShouldNotBe(-1, "form.OpenFile failed");

        var sf = form.GetShapefileFromLayer(layerHandle);
        sf.ShouldNotBeNull("Could not get shapefile from layer");

        // Test
        sf.HasSpatialIndex.ShouldBeTrue();
        sf.UseSpatialIndex.ShouldBeTrue();
        sf.IsSpatialIndexValid().ShouldBeTrue();
        sf.HasInvalidShapes().ShouldBeFalse();
        sf.NumShapes.ShouldBe(13424);
        sf.Extents.xMin.ShouldBe(108.722071, 0.00001);
        sf.Extents.yMin.ShouldBe(34.149021, 0.00001);
        sf.Extents.xMax.ShouldBe(109.139842, 0.00001);
        sf.Extents.yMax.ShouldBe(34.457816, 0.00001);
    }

    [WpfFact]
    public void ShapefileKeyTest()
    {
        // AS mentioned at https://mapwindow.discourse.group/t/key-property-of-shape-object-not-work/1250

        using var form = new WinFormsApp1.Form1();
        form.ShouldNotBeNull();

        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Set key for shapefile:
        const string sfKeyValue = "This is my sf key";
        sfPolygon.Key = sfKeyValue;
        // Check:
        sfPolygon.Key.ShouldBe(sfKeyValue);

        // Set key for first shape:
        const string shpKeyValue = "This is my shp key";
        sfPolygon.Shape[0].Key = shpKeyValue;
        // Check:
        sfPolygon.Shape[0].Key.ShouldBe(shpKeyValue);
        var shp = sfPolygon.Shape[0];
        shp.Key.ShouldBe(shpKeyValue);

        // Re-check:
        sfPolygon.Key.ShouldBe(sfKeyValue);

        // Add shapefile to map and test again:
        var layerHandle = form.AddShapefileToMap(sfPolygon);
        // Get sf back:
        var sf = form.GetShapefileFromLayer(layerHandle);
        sf.Key.ShouldBe(sfKeyValue);
        sf.Shape[0].Key.ShouldBe(shpKeyValue);
    }
}
