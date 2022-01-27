namespace unittest_net6;

public class MapWinGisTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public MapWinGisTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void VersionTest()
    {
        _testOutputHelper.WriteLine("TODO");
        // TODO
        //var map = new MapWinGIS.Map();
        //var versionNumber = map.VersionNumber;
        //_testOutputHelper.WriteLine(@"MapWinGIS.VersionNumber: " + versionNumber);
        //Assert.NotNull(versionNumber);
    }
}
