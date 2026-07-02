using System.Diagnostics;

namespace MapWinGisTests.UnitTests;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class GdalUtilsTests : ICallback, IClassFixture<GdalUtilsTests.GdalUtilsFixture>
{
    private readonly GdalUtils _gdalUtils;
    private readonly ITestOutputHelper _testOutputHelper;
    private readonly GdalUtilsFixture _fixture;

    public GdalUtilsTests(ITestOutputHelper testOutputHelper, GdalUtilsFixture fixture)
    {
        _testOutputHelper = testOutputHelper;
        _fixture = fixture;
        _gdalUtils = new GdalUtils();

        _ = new GlobalSettings
        {
            ApplicationCallback = this,
            CallbackVerbosity = tkCallbackVerbosity.cvAll
        };
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
        GdalRasterWarpTest(_fixture.Float32_50mTiffFilename);
        GdalRasterWarpTest(_fixture.UnicodeTiffFilename);

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

        var retVal = _gdalUtils.GdalVectorTranslate(_fixture.UnitedStates_3857SfFilename, output, options, true);
        retVal.ShouldBeTrue("GdalVectorTranslate failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
        // Check if output file exists:
        File.Exists(output).ShouldBeTrue(output + " doesn't exists");
    }

    [Fact]
    public void GdalUtilsClipVectorWithVectorTest()
    {
        var output = Helpers.GetRandomFilePath("GdalVectorTranslate", ".shp");

        var borderSfFilename = Helpers.GetTestFilePath("Usa-Clip-Polygon-3857.shp");
        var retVal = _gdalUtils.ClipVectorWithVector(_fixture.UnitedStates_3857SfFilename, borderSfFilename, output);
        retVal.ShouldBeTrue("GdalVectorTranslate failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
        // Check if output file exists:
        File.Exists(output).ShouldBeTrue(output + " doesn't exists");
    }

    //[Fact(Skip = "Needs more research, causes crashes sometimes.")]
    [Fact]
    public void GdalUtilsGdalRasterTranslateTest()
    {
        GdalRasterTranslateTest(_fixture.Float32_50mTiffFilename);
        GdalRasterTranslateTest(_fixture.UnicodeTiffFilename);

        // Local function:
        void GdalRasterTranslateTest(string input)
        {
            _testOutputHelper.WriteLine("Input file: " + input);
            var output = Helpers.GetRandomFilePath("-Tiled", ".tif");
            var options = new[]
            {
                "-of", "GTiff",
                "-co",  "TILED=YES"
            };

            try
            {
                var retVal = _gdalUtils.GdalRasterTranslate(input, output, options);
                retVal.ShouldBeTrue("GdalRasterTranslate failed: " + _gdalUtils.ErrorMsg[_gdalUtils.LastErrorCode] + " Detailed error: " + _gdalUtils.DetailedErrorMsg);
            }
            catch (Exception e)
            {
                _testOutputHelper.WriteLine(e.Message);
                Debug.Fail(e.Message, e.InnerException?.Message);
            }

            // Check if output file exists:
            File.Exists(output).ShouldBeTrue(output + " doesn't exists");
        }
    }

    [Fact]
    public void GdalUtilsGdalBuildOverviewsTest()
    {
        GdalBuildOverviewsTest(_fixture.Float32_50mTiffFilename);
        GdalBuildOverviewsTest(_fixture.UnicodeTiffFilename);

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
        var retVal = _gdalUtils.GdalVectorReproject(_fixture.UnitedStates_3857SfFilename, output, 3857, 28992);
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

    public class GdalUtilsFixture : IDisposable
    {
        // ReSharper disable once InconsistentNaming
        public string Float32_50mTiffFilename { get; }
        // ReSharper disable once InconsistentNaming
        public string UnitedStates_3857SfFilename { get; }
        public string UnicodeTiffFilename { get; }
        public string UnicodeSfFilename { get; }

        public GdalUtilsFixture()
        {

            // Setup test data:
            Float32_50mTiffFilename = Helpers.GetTestFilePath("float32_50m.tif");
            // Copy tiff to filename with unicode chars:
            UnicodeTiffFilename = Float32_50mTiffFilename.Replace(".tif", "-Воздух.tif");
            if (!File.Exists(UnicodeTiffFilename))
                File.Copy(Float32_50mTiffFilename, UnicodeTiffFilename);

            UnitedStates_3857SfFilename = Helpers.GetTestFilePath("UnitedStates-3857.shp");
            UnicodeSfFilename = UnitedStates_3857SfFilename.Replace(".shp", "-Воздух.shp");
            Helpers.CopyShapefile(UnitedStates_3857SfFilename, UnicodeSfFilename);
        }

        /// <inheritdoc />
        public void Dispose()
        {
            // Clean up:
            if (File.Exists(UnicodeTiffFilename)) File.Delete(UnicodeTiffFilename);
            if (File.Exists(UnicodeSfFilename)) Helpers.DeleteShapefileFiles(UnicodeSfFilename);

            GC.SuppressFinalize(this);
        }
    }
}
