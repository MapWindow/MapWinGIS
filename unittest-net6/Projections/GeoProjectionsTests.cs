namespace unittest_net6.Projections;

public class GeoProjectionsTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public GeoProjectionsTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void CloneTest()
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();
        var retVal = geoProjection.ImportFromEPSG(28992);
        retVal.ShouldBe(true);
        var original = geoProjection.ExportToWktEx();
        _testOutputHelper.WriteLine("Original:");
        _testOutputHelper.WriteLine(original);

        var geoProjectionCloned = geoProjection.Clone();
        var cloned = geoProjectionCloned.ExportToWktEx();
        _testOutputHelper.WriteLine("Cloned:");
        _testOutputHelper.WriteLine(cloned);

        cloned.ShouldBe(original);

        // Change original:
        retVal = geoProjection.ImportFromEPSG(4326);
        retVal.ShouldBe(true);
        var original2 = geoProjection.ExportToWktEx();
        _testOutputHelper.WriteLine("Original2:");
        _testOutputHelper.WriteLine(original2);
        // use the same instance:
        var cloned2 = geoProjectionCloned.ExportToWktEx();
        _testOutputHelper.WriteLine("Cloned2:");
        _testOutputHelper.WriteLine(cloned2);

        original2.ShouldNotBe(cloned2);
    }

    [Fact]
    public void IsEmptyTest()
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();
        geoProjection.IsEmpty.ShouldBeTrue();

        // Import projected:
        var retVal = geoProjection.ImportFromEPSG(3857);
        retVal.ShouldBe(true);
        geoProjection.IsProjected.ShouldBeTrue();
        geoProjection.IsEmpty.ShouldBeFalse();

        // Reset:
        retVal = geoProjection.Clear();
        retVal.ShouldBe(true);
        geoProjection.IsEmpty.ShouldBeTrue();

        // Import geographic:
        retVal = geoProjection.ImportFromEPSG(4326);
        retVal.ShouldBe(true);
        geoProjection.IsGeographic.ShouldBeTrue();
        geoProjection.IsEmpty.ShouldBeFalse();
    }
}
