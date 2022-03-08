namespace MapWinGisTests.UnitTests;
public class GdalUtilsTests : ICallback, IDisposable
{
    private readonly GdalUtils _gdalUtils;
    private readonly ITestOutputHelper _testOutputHelper;
    // ReSharper disable once InconsistentNaming
    private readonly string _float32_50mTiffFilename;
    // ReSharper disable once InconsistentNaming
    private readonly string _unitedStates_3857SfFilename;
    private readonly string _unicodeTiffFilename;
    private readonly string _unicodeSfFilename;

    public GdalUtilsTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
        _gdalUtils = new GdalUtils();

        // Setup test data:
        _float32_50mTiffFilename = Helpers.GetTestFilePath("float32_50m.tif");
        // Copy tiff to filename with unicode chars:
        _unicodeTiffFilename = _float32_50mTiffFilename.Replace(".tif", "-Воздух.tif");
        if (!File.Exists(_unicodeTiffFilename))
            File.Copy(_float32_50mTiffFilename, _unicodeTiffFilename);

        _unitedStates_3857SfFilename = Helpers.GetTestFilePath("UnitedStates-3857.shp");
        _unicodeSfFilename = _unitedStates_3857SfFilename.Replace(".shp", "-Воздух.shp");
        Helpers.CopyShapefile(_unitedStates_3857SfFilename, _unicodeSfFilename);
    }

    /// <inheritdoc />
    public void Dispose()
    {
        // Clean up:
        if (File.Exists(_unicodeTiffFilename)) File.Delete(_unicodeTiffFilename);
        if (File.Exists(_unicodeSfFilename)) Helpers.DeleteShapefileFiles(_unicodeSfFilename);

        GC.SuppressFinalize(this);
    }

    [Fact]
    public void GdalUtilsLastErrorCodeTest()
    {
        var errorCode = _gdalUtils.LastErrorCode;
        errorCode.ShouldBe(0);
        // Trigger error:
        var retVal = _gdalUtils.GdalRasterWarp("NoFileName", null, null);
        retVal.ShouldBeFalse();
        errorCode = _gdalUtils.LastErrorCode;
        errorCode.ShouldBe(4);
        _testOutputHelper.WriteLine(_gdalUtils.ErrorMsg[errorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
    }

    [Fact]
    public void GdalUtilsErrorMsgTest()
    {
        var errorMsg = _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode];
        errorMsg.ShouldBe("No Error");
        // Trigger error:
        var retVal = _gdalUtils.GdalRasterWarp("NoFileName", null, null);
        retVal.ShouldBeFalse();
        errorMsg = _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode];
        errorMsg.ShouldBe("Invalid Filename");
    }

    [Fact]
    public void GdalUtilsDetailedErrorMsgTest()
    {
        var errorMsg = _gdalUtils.DetailedErrorMsg;
        errorMsg.ShouldBeEmpty();
        // Trigger error:
        var retVal = _gdalUtils.GdalRasterWarp("NoFileName", null, null);
        retVal.ShouldBeFalse();
        errorMsg = _gdalUtils.DetailedErrorMsg;
        errorMsg.ShouldBe("Source file NoFileName does not exists.");
    }

    [Fact]
    public void GdalUtilsGlobalCallbackTest()
    {
        //Read:
        var retVal = _gdalUtils.GlobalCallback;
        retVal.ShouldBeNull();
        // Change:
        _gdalUtils.GlobalCallback = this;
        // Check:
        _gdalUtils.GlobalCallback.ShouldNotBeNull();
    }

    [Fact]
    public void GdalUtilsKeyTest()
    {
        //Read:
        var retVal = _gdalUtils.Key;
        retVal.ShouldBeNullOrEmpty();
        // Change:
        const string keyString = "This is my key";
        _gdalUtils.Key = keyString;
        // Check:
        _gdalUtils.Key.ShouldBe(keyString);
    }

    [Fact]
    public void GdalUtilsGdalRasterWarpTest()
    {
        GdalRasterWarpTest(_float32_50mTiffFilename);
        GdalRasterWarpTest(_unicodeTiffFilename);

        // Local function:
        void GdalRasterWarpTest(string input)
        {
            _testOutputHelper.WriteLine("Input file: " + input);
            var output = Helpers.GetRandomFilePath("GdalWarp", ".vrt");
            var options = new[]
            {
                "-of", "vrt",
                "-overwrite"
            };

            var retVal = _gdalUtils.GdalRasterWarp(input, output, options);
            retVal.ShouldBeTrue("GdalRasterWarp failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] +
                                " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            // Check if output file exists:
            File.Exists(output).ShouldBeTrue(output + " doesn't exists");
        }
    }

    [Fact]
    public void GdalUtilsGdalVectorTranslateTest()
    {
        var output = Helpers.GetRandomFilePath("translated", ".gml");
        var options = new[]
        {
            "-f", "GML"
        };

        var retVal = _gdalUtils.GdalVectorTranslate(_unitedStates_3857SfFilename, output, options, true);
        retVal.ShouldBeTrue("GdalVectorTranslate failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
        // Check if output file exists:
        File.Exists(output).ShouldBeTrue(output + " doesn't exists");
    }

    [Fact]
    public void GdalUtilsClipVectorWithVectorTest()
    {
        var output = Helpers.GetRandomFilePath("GdalVectorTranslate", ".shp");

        var borderSfFilename = Helpers.GetTestFilePath("Usa-Clip-Polygon-3857.shp");
        var retVal = _gdalUtils.ClipVectorWithVector(_unitedStates_3857SfFilename, borderSfFilename, output);
        retVal.ShouldBeTrue("GdalVectorTranslate failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
        // Check if output file exists:
        File.Exists(output).ShouldBeTrue(output + " doesn't exists");
    }

    [Fact]
    public void GdalUtilsGdalRasterTranslateTest()
    {
        GdalRasterTranslateTest(_float32_50mTiffFilename);
        GdalRasterTranslateTest(_unicodeTiffFilename);

        // Local function:
        void GdalRasterTranslateTest(string input)
        {
            _testOutputHelper.WriteLine("Input file: " + input);
            var output = Helpers.GetRandomFilePath("ChangedResolution", ".tif");
            var options = new[]
            {
                "-ot", "Float32",
                "-tr", "0.2", "0.2",
                "-r", "average",
                "-projwin", "-180", "90", "180", "-90"
            };

            var retVal = _gdalUtils.GdalRasterTranslate(input, output, options);
            retVal.ShouldBeTrue("GdalRasterTranslate failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] +
                                " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            // Check if output file exists:
            File.Exists(output).ShouldBeTrue(output + " doesn't exists");
        }
    }

    [Fact]
    public void GdalUtilsGdalBuildOverviewsTest()
    {
        GdalBuildOverviewsTest(_float32_50mTiffFilename);
        GdalBuildOverviewsTest(_unicodeTiffFilename);

        // Local function:
        void GdalBuildOverviewsTest(string input)
        {
            _testOutputHelper.WriteLine("Input file: " + input);
            // Removing overviews:
            var ovrFilename = $"{input}.ovr";
            if (File.Exists(ovrFilename)) File.Delete(ovrFilename);

            // Example of creating overviews for all bands with autogenerated levels:
            var configOptions = new[] { "COMPRESS_OVERVIEW ZSTD", "INTERLEAVE_OVERVIEW PIXEL" };
            var retVal = _gdalUtils.GdalBuildOverviews(input, tkGDALResamplingMethod.grmCubic, null,
                null, configOptions);
            retVal.ShouldBeTrue("GdalBuildOverviews failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] +
                                " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            // Check:
            File.Exists(ovrFilename).ShouldBeTrue(ovrFilename + " doesn't exists.");
            // Removing overviews:
            if (File.Exists(ovrFilename)) File.Delete(ovrFilename);
        }
    }

    [Fact]
    public void GdalUtilsGdalVectorReprojectTest()
    {
        var output = Helpers.GetRandomFilePath("translated-28992", ".shp");
        var retVal = _gdalUtils.GdalVectorReproject(_unitedStates_3857SfFilename, output, 3857, 28992);
        retVal.ShouldBeTrue("GdalVectorReproject failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
        // Check if output file exists:
        File.Exists(output).ShouldBeTrue(output + " doesn't exists");
    }

    #region Implementation of ICallback
#pragma warning disable xUnit1013

    public void Progress(string keyOfSender, int percent, string message)
    {
        _testOutputHelper.WriteLine($"Progress of {keyOfSender}: {percent}. Msg: {message}");
    }

    public void Error(string keyOfSender, string errorMsg)
    {
        _testOutputHelper.WriteLine($"Error of {keyOfSender}: {errorMsg}");
    }

#pragma warning restore xUnit1013
    #endregion
}
