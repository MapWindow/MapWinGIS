using System.Globalization;

namespace MapWinGisTests.UnitTests;
public class GlobalSettingsTests
{
    private readonly GlobalSettings _gs;
    private readonly ITestOutputHelper _testOutputHelper;

    public GlobalSettingsTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
        _gs = new GlobalSettings();
        _gs.ShouldNotBeNull("GlobalSettings is null");
    }

    [Fact]
    public void GlobalSettingsGdalDataPathTest()
    {
        var gdalDataPath = _gs.GdalDataPath;
        _testOutputHelper.WriteLine(gdalDataPath);
        gdalDataPath.ShouldNotBeNullOrEmpty("GdalDataPath is not set");
        gdalDataPath.EndsWith("\\gdal-data\\").ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsGdalPluginPathTest()
    {
        var gdalPluginPath = _gs.GdalPluginPath;
        _testOutputHelper.WriteLine(gdalPluginPath);
        gdalPluginPath.ShouldNotBeNullOrEmpty("GdalPluginPath is not set");
        gdalPluginPath.EndsWith("\\gdalplugins\\").ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsProjPathTest()
    {
        var projPath = _gs.ProjPath;
        _testOutputHelper.WriteLine("projPath: " + projPath);
        projPath.ShouldNotBeNullOrEmpty("ProjPath is not set");
        projPath.EndsWith("\\proj7\\share\\").ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsGdalVersionTest()
    {
        var gdalVersion = _gs.GdalVersion;
        _testOutputHelper.WriteLine(gdalVersion);
        gdalVersion.ShouldNotBeNullOrEmpty("GdalVersion is not set");
        gdalVersion.StartsWith("GDAL 3.4").ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsMinPolygonAreaTest()
    {
        var value = _gs.MinPolygonArea;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(1);
    }

    [Fact]
    public void GlobalSettingsMinAreaToPerimeterRatioTest()
    {
        var value = _gs.MinAreaToPerimeterRatio;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(0.0001);
    }

    [Fact]
    public void GlobalSettingsClipperGcsMultiplicationFactorTest()
    {
        var value = _gs.ClipperGcsMultiplicationFactor;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(100000);
    }

    [Fact]
    public void GlobalSettingsShapefileFastModeTest()
    {
        var value = _gs.ShapefileFastMode;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
    }

    [Fact]
    public void GlobalSettingsShapefileFastUnionTest()
    {
        var value = _gs.ShapefileFastUnion;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsGdalLastErrorNoTest()
    {
        var value = _gs.GdalLastErrorNo;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGdalError.GdalErrorNone);
    }

    [Fact]
    public void GlobalSettingsGdalLastErrorTypeTest()
    {
        var value = _gs.GdalLastErrorType;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGdalErrorType.GdalNone);
    }

    [Fact]
    public void GlobalSettingsGdalLastErrorMsgTest()
    {
        var value = _gs.GdalLastErrorMsg;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty();
    }

    [Fact]
    public void GlobalSettingsGdalReprojectionErrorMsgTest()
    {
        var value = _gs.GdalReprojectionErrorMsg;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty();
    }

    [Fact]
    public void GlobalSettingsLabelsSmoothingModeTest()
    {
        var value = _gs.LabelsSmoothingMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkSmoothingMode.HighQualityMode);
    }

    [Fact]
    public void GlobalSettingsLabelsCompositingQualityTest()
    {
        var value = _gs.LabelsCompositingQuality;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkCompositingQuality.HighQuality);
    }

    [Fact]
    public void GlobalSettingsLocalizedStringTest()
    {
        var value = _gs.LocalizedString[tkLocalizedStrings.lsKilometers];
        _testOutputHelper.WriteLine(value);
        value.ShouldNotBeNullOrEmpty("LocalizedString is not set");
        value.ShouldBe("km");
    }

    [Fact]
    public void GlobalSettingsZoomToFirstLayerTest()
    {
        var value = _gs.ZoomToFirstLayer;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsLabelsCollisionModeTest()
    {
        var value = _gs.LabelsCollisionMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkCollisionMode.LocalList);
    }

    [Fact]
    public void GlobalSettingsGridProxyFormatTest()
    {
        var value = _gs.GridProxyFormat;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGridProxyFormat.gpfBmpProxy);
    }

    [Fact]
    public void GlobalSettingsMaxDirectGridSizeMbTest()
    {
        var value = _gs.MaxDirectGridSizeMb;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(20);
    }

    [Fact]
    public void GlobalSettingsGridProxyModeTest()
    {
        var value = _gs.GridProxyMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGridProxyMode.gpmAuto);
    }

    [Fact]
    public void GlobalSettingsMaxUniqueValuesCountForGridSchemeTest()
    {
        var value = _gs.MaxUniqueValuesCountForGridScheme;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(100);
    }

    [Fact]
    public void GlobalSettingsRandomColorSchemeForGridsTest()
    {
        var value = _gs.RandomColorSchemeForGrids;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsDefaultColorSchemeForGridsTest()
    {
        var value = _gs.DefaultColorSchemeForGrids;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(PredefinedColorScheme.SummerMountains);
    }

    [Fact]
    public void GlobalSettingsShapeOutputValidationModeTest()
    {
        var value = _gs.ShapeOutputValidationMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkShapeValidationMode.NoValidation);
    }

    [Fact]
    public void GlobalSettingsShapeInputValidationModeTest()
    {
        var value = _gs.ShapeInputValidationMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkShapeValidationMode.NoValidation);
    }

    [Fact]
    public void GlobalSettingsGeometryEngineTest()
    {
        var value = _gs.GeometryEngine;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGeometryEngine.engineGeos);
    }

    [Fact]
    public void GlobalSettingsSaveGridColorSchemeToFileTest()
    {
        var value = _gs.SaveGridColorSchemeToFile;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsMinOverviewWidthTest()
    {
        var value = _gs.MinOverviewWidth;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(512);
    }

    [Fact]
    public void GlobalSettingsRasterOverviewCreationTest()
    {
        var value = _gs.RasterOverviewCreation;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkRasterOverviewCreation.rocAuto);
    }

    [Fact]
    public void GlobalSettingsTiffCompressionTest()
    {
        var value = _gs.TiffCompression;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkTiffCompression.tkmLZW);
    }

    [Fact]
    public void GlobalSettingsRasterOverviewResamplingTest()
    {
        var value = _gs.RasterOverviewResampling;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGDALResamplingMethod.grmNearest);
    }

    [Fact]
    public void GlobalSettingsTilesThreadPoolSizeTest()
    {
        var value = _gs.TilesThreadPoolSize;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(5);
    }

    [Fact]
    public void GlobalSettingsLoadSymbologyOnAddLayerTest()
    {
        var value = _gs.LoadSymbologyOnAddLayer;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsTilesMinZoomOnProjectionMismatchTest()
    {
        var value = _gs.TilesMinZoomOnProjectionMismatch;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(6);
    }

    [Fact]
    public void GlobalSettingsImageUpsamplingModeTest()
    {
        var value = _gs.ImageUpsamplingMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkInterpolationMode.imNone);
    }

    [Fact]
    public void GlobalSettingsImageDownsamplingModeTest()
    {
        var value = _gs.ImageDownsamplingMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkInterpolationMode.imBilinear);
    }


    [Fact]
    public void GlobalSettingsOgrStringEncodingTest()
    {
        var value = _gs.OgrStringEncoding;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkOgrEncoding.oseUtf8);
    }

    [Fact]
    public void GlobalSettingsOgrLayerMaxFeatureCountTest()
    {
        var value = _gs.OgrLayerMaxFeatureCount;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(50_000);
    }

    [Fact]
    public void GlobalSettingsAutoChooseOgrLoadingModeTest()
    {
        var value = _gs.AutoChooseOgrLoadingMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsReprojectLayersOnAddingTest()
    {
        var value = _gs.ReprojectLayersOnAdding;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void GlobalSettingsApplicationCallbackTest()
    {
        var value = _gs.ApplicationCallback;
        _testOutputHelper.WriteLine(value.ToString());
    }

    [Fact]
    public void GlobalSettingsHotTrackingMaxShapeCountTest()
    {
        var value = _gs.HotTrackingMaxShapeCount;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(200);
    }

    [Fact]
    public void GlobalSettingsAllowLayersWithoutProjectionsTest()
    {
        var value = _gs.AllowLayersWithoutProjections;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsAllowProjectionMismatchTest()
    {
        var value = _gs.AllowProjectionMismatch;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsMouseToleranceTest()
    {
        var value = _gs.MouseTolerance;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(20);
    }

    [Fact]
    public void GlobalSettingsMaxReprojectionShapeCountTest()
    {
        var value = _gs.MaxReprojectionShapeCount;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(50_000);
    }

    [Fact]
    public void GlobalSettingsPixelOffsetModeTest()
    {
        var value = _gs.PixelOffsetMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkPixelOffsetMode.pomDefault);
    }

    [Fact]
    public void GlobalSettingsAutoChooseRenderingHintForLabelsTest()
    {
        var value = _gs.AutoChooseRenderingHintForLabels;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsOgrLayerForceUpdateModeTest()
    {
        var value = _gs.OgrLayerForceUpdateMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }

    [Fact]
    public void GlobalSettingsForceHideLabelsTest()
    {
        var value = _gs.ForceHideLabels;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }

    [Fact]
    public void GlobalSettingsBingApiKeyTest()
    {
        var value = _gs.BingApiKey;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty("BingApiKey is set");
    }

    [Fact]
    public void GlobalSettingsCompressOverviewsTest()
    {
        var value = _gs.CompressOverviews;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkTiffCompression.tkmAUTO);
    }

    [Fact]
    public void GlobalSettingsGridFavorGreyscaleTest()
    {
        var value = _gs.GridFavorGreyscale;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsGridUseHistogramTest()
    {
        var value = _gs.GridUseHistogram;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsOverrideLocalCallbackTest()
    {
        var value = _gs.OverrideLocalCallback;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsCacheDbfRecordsTest()
    {
        var value = _gs.CacheDbfRecords;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsCacheShapeRenderingDataTest()
    {
        var value = _gs.CacheShapeRenderingData;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }

    [Fact]
    public void GlobalSettingsTileLogFilenameTest()
    {
        var value = _gs.TileLogFilename;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty("TileLogFilename is set");
    }

    [Fact]
    public void GlobalSettingsTileLogIsOpenedTest()
    {
        var value = _gs.TileLogIsOpened;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }

    [Fact]
    public void GlobalSettingsLogTileErrorsOnlyTest()
    {
        var value = _gs.LogTileErrorsOnly;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }

    [Fact]
    public void GlobalSettingsWmsDiskCachingTest()
    {
        var value = _gs.WmsDiskCaching;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsCallbackVerbosityTest()
    {
        var value = _gs.CallbackVerbosity;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkCallbackVerbosity.cvAll);
    }

    [Fact]
    public void GlobalSettingsOgrShareConnectionTest()
    {
        var value = _gs.OgrShareConnection;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }

    [Fact]
    public void GlobalSettingsOgrInterpretYNStringAsBooleanTest()
    {
        var value = _gs.OgrInterpretYNStringAsBoolean;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsAllowLayersWithIncompleteReprojectionTest()
    {
        var value = _gs.AllowLayersWithIncompleteReprojection;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse();
    }



    [Fact]
    public void GlobalSettingsSetHttpUserAgentTest()
    {
        _gs.SetHttpUserAgent("MyAgent");
        // TODO
    }

    [Fact]
    public void GlobalSettingsTestBingApiKeyTest()
    {
        var value = _gs.TestBingApiKey("MyKey");
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeFalse(); // Invalid key
    }

    [Fact]
    public void GlobalSettingsSetHereMapsApiKeyTest()
    {
        _gs.SetHereMapsApiKey("appId", "appCode");
        // TODO
    }

    [Fact]
    public void GlobalSettingsStartLogTileRequestsTest()
    {
        var baseFileName = Path.Combine(Path.GetTempPath(), Path.GetFileNameWithoutExtension(Path.GetRandomFileName()));
        var value = _gs.StartLogTileRequests($"{baseFileName}-TilesLog.txt");
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBeTrue();
    }

    [Fact]
    public void GlobalSettingsStopLogTileRequestsTest()
    {
        _gs.StopLogTileRequests();
        // TODO
    }

    [Fact]
    public void GlobalSettingsResetGdalErrorTest()
    {
        _gs.ResetGdalError();
    }
}
