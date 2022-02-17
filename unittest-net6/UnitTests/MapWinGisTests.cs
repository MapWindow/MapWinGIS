namespace unittest_net6;

public class MapWinGisTests
{
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
    public void OpenInvalidShapefile()
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
    }
}
