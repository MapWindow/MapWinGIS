namespace MapWinGisTests.FunctionalTests.Projections;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class EpsgTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public EpsgTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void AmersfoortTest()
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();
        var retVal = geoProjection.ImportFromEPSG(28992);
        retVal.ShouldBe(true);

        // Checks
        geoProjection.Checks(true);
        geoProjection.ProjectionName.ShouldBe("Amersfoort / RD New");
        geoProjection.InverseFlattening.ShouldBe(299.1528128);
        geoProjection.SemiMajor.ShouldBe(6377397.155); // Bessel 1841
        // TODO: Not sure what value this should be: geoProjection.SemiMinor

        var esri = geoProjection.ExportToEsri();
        _testOutputHelper.WriteLine(esri);
        esri.ShouldNotBeNull();
        esri.StartsWith("PROJCS[\"RD_New\"").ShouldBeTrue();
        var proj4 = geoProjection.ExportToProj4();
        _testOutputHelper.WriteLine(proj4);
        proj4.ShouldNotBeNull();
        proj4.ShouldBe("+proj=sterea +lat_0=52.1561605555556 +lon_0=5.38763888888889 +k=0.9999079 +x_0=155000 +y_0=463000 +ellps=bessel +units=m +no_defs");
        var wkt = geoProjection.ExportToWKT();
        _testOutputHelper.WriteLine(wkt);
        wkt.ShouldNotBeNull();
        wkt.StartsWith("PROJCS[\"Amersfoort / RD New\",GEOGCS[\"Amersfoort\",DATUM[\"Amersfoort\"").ShouldBeTrue();
        var wktEx = geoProjection.ExportToWktEx();
        _testOutputHelper.WriteLine(wktEx);
        wktEx.ShouldNotBeNull();
        wktEx.StartsWith("PROJCRS[\"Amersfoort / RD New\",").ShouldBeTrue();
    }

    [Fact]
    public void Wgs84Test()
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();
        var retVal = geoProjection.ImportFromEPSG(4326);
        retVal.ShouldBe(true);

        // Checks
        geoProjection.Checks(false);
        geoProjection.GeogCSName.ShouldBe("WGS 84");
        geoProjection.InverseFlattening.ShouldBe(298.257223563);
        geoProjection.SemiMajor.ShouldBe(6378137); // Bessel 1841
        // TODO: Not sure what value this should be: geoProjection.SemiMinor

        var esri = geoProjection.ExportToEsri();
        _testOutputHelper.WriteLine(esri);
        esri.ShouldNotBeNull();
        esri.StartsWith("GEOGCS[\"GCS_WGS_1984\"").ShouldBeTrue();
        var proj4 = geoProjection.ExportToProj4();
        _testOutputHelper.WriteLine(proj4);
        proj4.ShouldNotBeNull();
        proj4.ShouldBe("+proj=longlat +datum=WGS84 +no_defs");
        var wkt = geoProjection.ExportToWKT();
        _testOutputHelper.WriteLine(wkt);
        wkt.ShouldNotBeNull();
        wkt.StartsWith("GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\"").ShouldBeTrue();
        var wktEx = geoProjection.ExportToWktEx();
        _testOutputHelper.WriteLine(wktEx);
        wktEx.ShouldNotBeNull();
        wktEx.StartsWith("GEOGCRS[\"WGS 84\",").ShouldBeTrue();
    }

    [Fact]
    public void GoogleMercatorTest()
    {
        var geoProjection = new GeoProjection();
        geoProjection.ShouldNotBeNull();
        var retVal = geoProjection.ImportFromEPSG(3857);
        retVal.ShouldBe(true);

        // Checks
        geoProjection.Checks(true);
        geoProjection.GeogCSName.ShouldBe("WGS 84");
        geoProjection.InverseFlattening.ShouldBe(298.257223563);
        geoProjection.SemiMajor.ShouldBe(6378137); // Bessel 1841
        // TODO: Not sure what value this should be: geoProjection.SemiMinor

        var esri = geoProjection.ExportToEsri();
        _testOutputHelper.WriteLine(esri);
        esri.ShouldNotBeNull();
        esri.StartsWith("PROJCS[\"WGS_1984_Web_Mercator_Auxiliary_Sphere\",GEOGCS[\"GCS_WGS_1984\",").ShouldBeTrue();
        var proj4 = geoProjection.ExportToProj4();
        _testOutputHelper.WriteLine(proj4);
        proj4.ShouldNotBeNull();
        proj4.ShouldBe("+proj=merc +a=6378137 +b=6378137 +lat_ts=0 +lon_0=0 +x_0=0 +y_0=0 +k=1 +units=m +nadgrids=@null +wktext +no_defs");
        var wkt = geoProjection.ExportToWKT();
        _testOutputHelper.WriteLine(wkt);
        wkt.ShouldNotBeNull();
        wkt.StartsWith("PROJCS[\"WGS 84 / Pseudo-Mercator\",GEOGCS[\"WGS 84\",").ShouldBeTrue();
        var wktEx = geoProjection.ExportToWktEx();
        _testOutputHelper.WriteLine(wktEx);
        wktEx.ShouldNotBeNull();
        wktEx.StartsWith("PROJCRS[\"WGS 84 / Pseudo-Mercator\",").ShouldBeTrue();
    }
}

public static class GeoProjectionChecks
{
    public static void Checks(this GeoProjection geoProjection, bool isProjected)
    {
        geoProjection.LastErrorCode.ShouldBe(0);
        geoProjection.IsEmpty.ShouldBeFalse();
        geoProjection.IsLocal.ShouldBeFalse();
        geoProjection.IsProjected.ShouldBe(isProjected);
        geoProjection.IsGeographic.ShouldBe(!isProjected);
        geoProjection.Name.ShouldBe(isProjected ? geoProjection.ProjectionName : geoProjection.GeogCSName);
        geoProjection.InverseFlattening.ShouldBeGreaterThan(0d);
        geoProjection.SemiMajor.ShouldBeGreaterThan(0d);
    }
}
