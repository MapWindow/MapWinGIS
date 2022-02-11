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

        var retVal = form.OpenFile(Path.Combine(Helpers.GetTestDataLocation(), "UnitedStates-3857.shp"));
        retVal.ShouldBeTrue("form.OpenFile failed");

        var epsgCode = form.GetMapProjectionAsEpsgCode();
        epsgCode.ShouldBe(3857);
    }
}
