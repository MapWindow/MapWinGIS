namespace unittest_net6;

public class MapWinGisTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public MapWinGisTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [WpfFact]
    public void VersionTest()
    {
        using var form = new WinFormsApp1.Form1();

        var version = form.GetMapWinGisVersion();
        version.ShouldNotBeNull();
        _testOutputHelper.WriteLine(version.ToString());
        version.Major.ShouldBe(5);
        version.Minor.ShouldBe(4);
        version.Build.ShouldBeGreaterThanOrEqualTo(0);
    }
}
