namespace MapWinGisTests.FunctionalTests;

public class GlobalSettingsTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public GlobalSettingsTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void GdalDataPathTest()
    {
        var gs = new GlobalSettings();
        gs.ShouldNotBeNull("GlobalSettings is null");
        var gdalDataPath = gs.GdalDataPath;
        _testOutputHelper.WriteLine(gdalDataPath);
        gdalDataPath.ShouldNotBeNullOrEmpty("GdalDataPath is not set");
        gdalDataPath.EndsWith("\\gdal-data\\").ShouldBeTrue();
    }

    [Fact]
    public void GdalPluginPathTest()
    {
        var gs = new GlobalSettings();
        gs.ShouldNotBeNull("GlobalSettings is null");
        var gdalPluginPath = gs.GdalPluginPath;
        _testOutputHelper.WriteLine(gdalPluginPath);
        gdalPluginPath.ShouldNotBeNullOrEmpty("GdalPluginPath is not set");
        gdalPluginPath.EndsWith("\\gdalplugins\\").ShouldBeTrue();
    }

    [Fact]
    public void ProjPathTest()
    {
        var gs = new GlobalSettings();
        gs.ShouldNotBeNull();
        var projPath = gs.ProjPath;
        _testOutputHelper.WriteLine("projPath: " + projPath);
        projPath.ShouldNotBeNullOrEmpty("ProjPath is not set");
        projPath.EndsWith("\\proj7\\share\\").ShouldBeTrue();
    }

    [Fact]
    public void GdalVersionTest()
    {
        var gs = new GlobalSettings();
        gs.ShouldNotBeNull();
        var gdalVersion = gs.GdalVersion;
        _testOutputHelper.WriteLine(gdalVersion);
        gdalVersion.ShouldNotBeNullOrEmpty("GdalVersion is not set");
        gdalVersion.StartsWith("GDAL 3.4").ShouldBeTrue();
    }
}
