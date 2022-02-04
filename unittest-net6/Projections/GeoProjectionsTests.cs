using System.IdentityModel.Tokens;

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
        retVal.ShouldBeTrue();
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
        retVal.ShouldBeTrue();
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
        retVal.ShouldBeTrue();
        geoProjection.IsProjected.ShouldBeTrue();
        geoProjection.IsEmpty.ShouldBeFalse();

        // Reset:
        retVal = geoProjection.Clear();
        retVal.ShouldBeTrue();
        geoProjection.IsEmpty.ShouldBeTrue();

        // Import geographic:
        retVal = geoProjection.ImportFromEPSG(4326);
        retVal.ShouldBeTrue();
        geoProjection.IsGeographic.ShouldBeTrue();
        geoProjection.IsEmpty.ShouldBeFalse();
    }

    [Fact]
    public void ImportFromAutoDetectTest()
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();

        // Use string from https://epsg.io/4326

        // Use proj.4 string:
        var retVal = geoProjection.ImportFromAutoDetect("+proj=longlat +datum=WGS84 +no_defs");
        retVal.ShouldBeTrue();
        _testOutputHelper.WriteLine(geoProjection.ExportToWktEx());

        // Use OGC WKT string:
        retVal = geoProjection.ImportFromAutoDetect("GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]");
        retVal.ShouldBeTrue();
        var wkt = geoProjection.ExportToWktEx();
        _testOutputHelper.WriteLine(wkt);
        wkt.StartsWith("GEOGCRS[\"WGS 84\",").ShouldBeTrue();
    }

    [Fact]
    public void TryAutoDetectEpsgTest()
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();

        // Projected
        var retVal = geoProjection.ImportFromEPSG(3857);
        retVal.ShouldBeTrue();
        CheckEpsgCode(geoProjection, 3857, false);

        // Reset:
        retVal = geoProjection.Clear();
        retVal.ShouldBeTrue();
        geoProjection.IsEmpty.ShouldBeTrue();
        retVal = geoProjection.TryAutoDetectEpsg(out var epsgCode2);
        retVal.ShouldBeFalse();
        epsgCode2.ShouldBe(-1);

        // Geopgraphic
        retVal = geoProjection.ImportFromEPSG(4326);
        retVal.ShouldBeTrue();
        CheckEpsgCode(geoProjection, 4326);

        // Reset:
        retVal = geoProjection.Clear();
        retVal.ShouldBeTrue();
        geoProjection.IsEmpty.ShouldBeTrue();

        // Using Well known enums:
        retVal = geoProjection.SetWellKnownGeogCS(tkCoordinateSystem.csWGS_84);
        retVal.ShouldBeTrue();
        CheckEpsgCode(geoProjection, 4326);

        // Using OGR WKT from https://epsg.io/4326
        retVal = geoProjection.ImportFromAutoDetect("GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]]");
        retVal.ShouldBeTrue();
        CheckEpsgCode(geoProjection, 4326);

        // Amersfoort:
        retVal = geoProjection.ImportFromEPSG(28992);
        retVal.ShouldBeTrue();
        CheckEpsgCode(geoProjection, 28992, false);

    }

    // [Fact]
    public void ImportFromEsriTest()
    {
        // As mentioned in https://mapwindow.discourse.group/t/importfromesri-crash-in-64-bits/984
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();

        const string proj = "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";
        var retVal = geoProjection.ImportFromESRI(proj);
        retVal.ShouldBeTrue();
        _testOutputHelper.WriteLine(geoProjection.ExportToWktEx());
        CheckEpsgCode(geoProjection, 4326);
        _testOutputHelper.WriteLine(geoProjection.ExportToWktEx());
    }

    private static void CheckEpsgCode(IGeoProjection geoProjection, int epsgCodeToCheck, bool isGeographic = true)
    {
        geoProjection.ShouldNotBeNull();
        geoProjection.IsEmpty.ShouldBeFalse();
        if (isGeographic)
        {
            geoProjection.IsGeographic.ShouldBeTrue();
        }
        else
        {
            geoProjection.IsProjected.ShouldBeTrue(); 
        }
        var retVal = geoProjection.TryAutoDetectEpsg(out var epsgCode);
        retVal.ShouldBeTrue();
        epsgCode.ShouldBe(epsgCodeToCheck);
    }
}
