using System.Globalization;

namespace MapWinGisTests.UnitTests;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class GlobalSettingsTests : ICallback
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
        // Read:
        var gdalDataPath = _gs.GdalDataPath;
        _testOutputHelper.WriteLine(gdalDataPath);
        gdalDataPath.ShouldNotBeNullOrEmpty("GdalDataPath is not set");
        gdalDataPath.EndsWith("\\gdal-data\\").ShouldBeTrue();
        // Change:
        var newpath = Path.Combine(Path.GetTempPath(), "new-gdal-path-Воздух");
        _testOutputHelper.WriteLine(newpath);
        _gs.GdalDataPath = newpath;
        // Check:
        _gs.GdalDataPath.ShouldBe(newpath);
        // Reset:
        _gs.GdalDataPath = gdalDataPath;
        _gs.GdalDataPath.ShouldBe(gdalDataPath);
    }

    [Fact]
    public void GlobalSettingsGdalPluginPathTest()
    {
        // Read:
        var gdalPluginPath = _gs.GdalPluginPath;
        _testOutputHelper.WriteLine(gdalPluginPath);
        gdalPluginPath.ShouldNotBeNullOrEmpty("GdalPluginPath is not set");
        gdalPluginPath.EndsWith("\\gdalplugins\\").ShouldBeTrue();
        // Change:
        var newpath = Path.Combine(Path.GetTempPath(), "new-gdal-plugin-Воздух");
        _testOutputHelper.WriteLine(newpath);
        _gs.GdalPluginPath = newpath;
        // Check:
        _gs.GdalPluginPath.ShouldBe(newpath);
        // Reset:
        _gs.GdalPluginPath = gdalPluginPath;
        _gs.GdalPluginPath.ShouldBe(gdalPluginPath);
    }

    [Fact]
    public void GlobalSettingsProjPathTest()
    {
        // Read:
        var projPath = _gs.ProjPath;
        _testOutputHelper.WriteLine("projPath: " + projPath);
        projPath.ShouldNotBeNullOrEmpty("ProjPath is not set");
        projPath.EndsWith("\\proj7\\share\\").ShouldBeTrue();
        // Change:
        var newpath = Path.Combine(Path.GetTempPath(), "new-proj-Воздух");
        _testOutputHelper.WriteLine(newpath);
        _gs.ProjPath = newpath;
        // Check:
        _gs.ProjPath.ShouldBe(newpath);
        // Reset:
        _gs.ProjPath = projPath;
        _gs.ProjPath.ShouldBe(projPath);
    }

    [Fact]
    public void GlobalSettingsGdalVersionTest()
    {
        // Read:
        var gdalVersion = _gs.GdalVersion;
        _testOutputHelper.WriteLine(gdalVersion);
        gdalVersion.ShouldNotBeNullOrEmpty("GdalVersion is not set");
        gdalVersion.StartsWith("GDAL 3.4").ShouldBeTrue();
        // Change:
        // GdalVersion is read-only
    }

    [Fact]
    public void GlobalSettingsMinPolygonAreaTest()
    {
        // Read:
        var value = _gs.MinPolygonArea;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(1);
        // Change:
        _gs.MinPolygonArea = value * 2.5;
        // Check:
        _gs.MinPolygonArea.ShouldBe(value * 2.5);
        // Reset:
        _gs.MinPolygonArea = value;
        _gs.MinPolygonArea.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsMinAreaToPerimeterRatioTest()
    {
        // Read:
        var value = _gs.MinAreaToPerimeterRatio;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(0.0001);
        // Change:
        _gs.MinAreaToPerimeterRatio = value * 2.5;
        // Check:
        _gs.MinAreaToPerimeterRatio.ShouldBe(value * 2.5);
        // Reset:
        _gs.MinAreaToPerimeterRatio = value;
        _gs.MinAreaToPerimeterRatio.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsClipperGcsMultiplicationFactorTest()
    {
        // Read:
        var value = _gs.ClipperGcsMultiplicationFactor;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(100000);
        // Change:
        _gs.ClipperGcsMultiplicationFactor = value * 2.5;
        // Check:
        _gs.ClipperGcsMultiplicationFactor.ShouldBe(value * 2.5);
        // Reset:
        _gs.ClipperGcsMultiplicationFactor = value;
        _gs.ClipperGcsMultiplicationFactor.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsShapefileFastModeTest()
    {
        // Read:
        var value = _gs.ShapefileFastMode;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.ShapefileFastMode = !value;
        // Check:
        _gs.ShapefileFastMode.ShouldBe(!value);
        // Reset:
        _gs.ShapefileFastMode = value;
        _gs.ShapefileFastMode.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsShapefileFastUnionTest()
    {
        // Read:
        var value = _gs.ShapefileFastUnion;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.ShapefileFastUnion = !value;
        // Check:
        _gs.ShapefileFastUnion.ShouldBe(!value);
        // Reset:
        _gs.ShapefileFastUnion = value;
        _gs.ShapefileFastUnion.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsGdalLastErrorNoTest()
    {
        // Read:
        var value = _gs.GdalLastErrorNo;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGdalError.GdalErrorNone);
        // Change:
        // _gs.GdalLastErrorNo is read-only
    }

    [Fact]
    public void GlobalSettingsGdalLastErrorTypeTest()
    {
        // Read
        var value = _gs.GdalLastErrorType;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGdalErrorType.GdalNone);
        // Change:
        // _gs.GdalLastErrorType is read-only
    }

    [Fact]
    public void GlobalSettingsGdalLastErrorMsgTest()
    {
        // Read:
        var value = _gs.GdalLastErrorMsg;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty();
        // Change:
        // _gs.GdalLastErrorMsg is read-only
    }

    [Fact]
    public void GlobalSettingsGdalReprojectionErrorMsgTest()
    {
        // Read:
        var value = _gs.GdalReprojectionErrorMsg;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty();
        // Change:
        // _gs.GdalReprojectionErrorMsg is read-only
    }

    [Fact]
    public void GlobalSettingsLabelsSmoothingModeTest()
    {
        // Read:
        var value = _gs.LabelsSmoothingMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkSmoothingMode.HighQualityMode);
        // Loop:
        foreach (tkSmoothingMode enumValue in Enum.GetValues(typeof(tkSmoothingMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.LabelsSmoothingMode = enumValue;
            // Check:
            _gs.LabelsSmoothingMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.LabelsSmoothingMode = value;
        _gs.LabelsSmoothingMode.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsLabelsCompositingQualityTest()
    {
        var value = _gs.LabelsCompositingQuality;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkCompositingQuality.HighQuality);
        // Loop:
        foreach (tkCompositingQuality enumValue in Enum.GetValues(typeof(tkCompositingQuality)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.LabelsCompositingQuality = enumValue;
            // Check:
            _gs.LabelsCompositingQuality.ShouldBe(enumValue);
        }
        // Reset:
        _gs.LabelsCompositingQuality = value;
        _gs.LabelsCompositingQuality.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsLocalizedStringTest()
    {
        // Read:
        var value = _gs.LocalizedString[tkLocalizedStrings.lsKilometers];
        _testOutputHelper.WriteLine(value);
        value.ShouldNotBeNullOrEmpty("LocalizedString is not set");
        value.ShouldBe("km");
        // Change:
        const string newValue = "Воздух";
        _gs.LocalizedString[tkLocalizedStrings.lsKilometers] = newValue;
        // Check:
        _gs.LocalizedString[tkLocalizedStrings.lsKilometers].ShouldBe(newValue);
        // Reset:
        _gs.LocalizedString[tkLocalizedStrings.lsKilometers] = value;
        _gs.LocalizedString[tkLocalizedStrings.lsKilometers].ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsZoomToFirstLayerTest()
    {
        // Read:
        var value = _gs.ZoomToFirstLayer;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.ZoomToFirstLayer = !value;
        // Check:
        _gs.ZoomToFirstLayer.ShouldBe(!value);
        // Reset:
        _gs.ZoomToFirstLayer = value;
        _gs.ZoomToFirstLayer.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsLabelsCollisionModeTest()
    {
        // Read:
        var value = _gs.LabelsCollisionMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkCollisionMode.LocalList);
        // Loop:
        foreach (tkCollisionMode enumValue in Enum.GetValues(typeof(tkCollisionMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.LabelsCollisionMode = enumValue;
            // Check:
            _gs.LabelsCollisionMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.LabelsCollisionMode = value;
        _gs.LabelsCollisionMode.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsGridProxyFormatTest()
    {
        // Read:
        var value = _gs.GridProxyFormat;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGridProxyFormat.gpfBmpProxy);
        // Loop:
        foreach (tkGridProxyFormat enumValue in Enum.GetValues(typeof(tkGridProxyFormat)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.GridProxyFormat = enumValue;
            // Check:
            _gs.GridProxyFormat.ShouldBe(enumValue);
        }
        // Reset:
        _gs.GridProxyFormat = value;
        _gs.GridProxyFormat.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsMaxDirectGridSizeMbTest()
    {
        //Read:
        var value = _gs.MaxDirectGridSizeMb;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(20);
        // Change:
        _gs.MaxDirectGridSizeMb = value * 2.5;
        // Check:
        _gs.MaxDirectGridSizeMb.ShouldBe(value * 2.5);
        // Reset:
        _gs.MaxDirectGridSizeMb = value;
        _gs.MaxDirectGridSizeMb.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsGridProxyModeTest()
    {
        //Read:
        var value = _gs.GridProxyMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGridProxyMode.gpmAuto);
        // Loop:
        foreach (tkGridProxyMode enumValue in Enum.GetValues(typeof(tkGridProxyMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.GridProxyMode = enumValue;
            // Check:
            _gs.GridProxyMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.GridProxyMode = value;
        _gs.GridProxyMode.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsRandomColorSchemeForGridsTest()
    {
        //Read:
        var value = _gs.RandomColorSchemeForGrids;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.RandomColorSchemeForGrids = !value;
        // Check:
        _gs.RandomColorSchemeForGrids.ShouldBe(!value);
        // Reset:
        _gs.RandomColorSchemeForGrids = value;
        _gs.RandomColorSchemeForGrids.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsShapeOutputValidationModeTest()
    {
        //Read:
        var value = _gs.ShapeOutputValidationMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkShapeValidationMode.NoValidation);
        // Loop:
        foreach (tkShapeValidationMode enumValue in Enum.GetValues(typeof(tkShapeValidationMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.ShapeOutputValidationMode = enumValue;
            // Check:
            _gs.ShapeOutputValidationMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.ShapeOutputValidationMode = value;
        _gs.ShapeOutputValidationMode.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsShapeInputValidationModeTest()
    {
        //Read:
        var value = _gs.ShapeInputValidationMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkShapeValidationMode.NoValidation);
        // Loop:
        foreach (tkShapeValidationMode enumValue in Enum.GetValues(typeof(tkShapeValidationMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.ShapeInputValidationMode = enumValue;
            // Check:
            _gs.ShapeInputValidationMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.ShapeInputValidationMode = value;
        _gs.ShapeInputValidationMode.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsGeometryEngineTest()
    {
        //Read:
        var value = _gs.GeometryEngine;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGeometryEngine.engineGeos);
        // Loop:
        foreach (tkGeometryEngine enumValue in Enum.GetValues(typeof(tkGeometryEngine)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.GeometryEngine = enumValue;
            // Check:
            _gs.GeometryEngine.ShouldBe(enumValue);
        }
        // Reset:
        _gs.GeometryEngine = value;
        _gs.GeometryEngine.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsSaveGridColorSchemeToFileTest()
    {
        //Read:
        var value = _gs.SaveGridColorSchemeToFile;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.SaveGridColorSchemeToFile = !value;
        // Check:
        _gs.SaveGridColorSchemeToFile.ShouldBe(!value);
        // Reset:
        _gs.SaveGridColorSchemeToFile = value;
        _gs.SaveGridColorSchemeToFile.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsRasterOverviewCreationTest()
    {
        //Read:
        var value = _gs.RasterOverviewCreation;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkRasterOverviewCreation.rocAuto);
        // Loop:
        foreach (tkRasterOverviewCreation enumValue in Enum.GetValues(typeof(tkRasterOverviewCreation)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.RasterOverviewCreation = enumValue;
            // Check:
            _gs.RasterOverviewCreation.ShouldBe(enumValue);
        }
        // Reset:
        _gs.RasterOverviewCreation = value;
        _gs.RasterOverviewCreation.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsTiffCompressionTest()
    {
        //Read:
        var value = _gs.TiffCompression;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkTiffCompression.tkmLZW);
        // Loop:
        foreach (tkTiffCompression enumValue in Enum.GetValues(typeof(tkTiffCompression)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.TiffCompression = enumValue;
            // Check:
            _gs.TiffCompression.ShouldBe(enumValue);
        }
        // Reset:
        _gs.TiffCompression = value;
        _gs.TiffCompression.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsRasterOverviewResamplingTest()
    {
        //Read:
        var value = _gs.RasterOverviewResampling;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkGDALResamplingMethod.grmNearest);
        // Loop:
        foreach (tkGDALResamplingMethod enumValue in Enum.GetValues(typeof(tkGDALResamplingMethod)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.RasterOverviewResampling = enumValue;
            // Check:
            _gs.RasterOverviewResampling.ShouldBe(enumValue);
        }
        // Reset:
        _gs.RasterOverviewResampling = value;
        _gs.RasterOverviewResampling.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsLoadSymbologyOnAddLayerTest()
    {
        //Read:
        var value = _gs.LoadSymbologyOnAddLayer;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.LoadSymbologyOnAddLayer = !value;
        // Check:
        _gs.LoadSymbologyOnAddLayer.ShouldBe(!value);
        // Reset:
        _gs.LoadSymbologyOnAddLayer = value;
        _gs.LoadSymbologyOnAddLayer.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsImageUpsamplingModeTest()
    {
        //Read:
        var value = _gs.ImageUpsamplingMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkInterpolationMode.imNone);
        // Loop:
        foreach (tkInterpolationMode enumValue in Enum.GetValues(typeof(tkInterpolationMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.ImageUpsamplingMode = enumValue;
            // Check:
            _gs.ImageUpsamplingMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.ImageUpsamplingMode = value;
        _gs.ImageUpsamplingMode.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsImageDownsamplingModeTest()
    {
        //Read:
        var value = _gs.ImageDownsamplingMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkInterpolationMode.imBilinear);
        // Loop:
        foreach (tkInterpolationMode enumValue in Enum.GetValues(typeof(tkInterpolationMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.ImageDownsamplingMode = enumValue;
            // Check:
            _gs.ImageDownsamplingMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.ImageDownsamplingMode = value;
        _gs.ImageDownsamplingMode.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsOgrStringEncodingTest()
    {
        //Read:
        var value = _gs.OgrStringEncoding;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkOgrEncoding.oseUtf8);
        // Loop:
        foreach (tkOgrEncoding enumValue in Enum.GetValues(typeof(tkOgrEncoding)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.OgrStringEncoding = enumValue;
            // Check:
            _gs.OgrStringEncoding.ShouldBe(enumValue);
        }
        // Reset:
        _gs.OgrStringEncoding = value;
        _gs.OgrStringEncoding.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsAutoChooseOgrLoadingModeTest()
    {
        //Read:
        var value = _gs.AutoChooseOgrLoadingMode;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.AutoChooseOgrLoadingMode = !value;
        // Check:
        _gs.AutoChooseOgrLoadingMode.ShouldBe(!value);
        // Reset:
        _gs.AutoChooseOgrLoadingMode = value;
        _gs.AutoChooseOgrLoadingMode.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsReprojectLayersOnAddingTest()
    {
        //Read:
        var value = _gs.ReprojectLayersOnAdding;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.ReprojectLayersOnAdding = !value;
        // Check:
        _gs.ReprojectLayersOnAdding.ShouldBe(!value);
        // Reset:
        _gs.ReprojectLayersOnAdding = value;
        _gs.ReprojectLayersOnAdding.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsAllowLayersWithoutProjectionsTest()
    {
        //Read:
        var value = _gs.AllowLayersWithoutProjections;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.AllowLayersWithoutProjections = !value;
        // Check:
        _gs.AllowLayersWithoutProjections.ShouldBe(!value);
        // Reset:
        _gs.AllowLayersWithoutProjections = value;
        _gs.AllowLayersWithoutProjections.ShouldBe(value);

    }


    [Fact]
    public void GlobalSettingsAllowProjectionMismatchTest()
    {

        //Read:
        var value = _gs.AllowProjectionMismatch;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.AllowProjectionMismatch = !value;
        // Check:
        _gs.AllowProjectionMismatch.ShouldBe(!value);
        // Reset:
        _gs.AllowProjectionMismatch = value;
        _gs.AllowProjectionMismatch.ShouldBe(value);

    }


    [Fact]
    public void GlobalSettingsMouseToleranceTest()
    {
        //Read:
        var value = _gs.MouseTolerance;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(20);
        // Change:
        _gs.MouseTolerance = value * 2.5;
        // Check:
        _gs.MouseTolerance.ShouldBe(value * 2.5);
        // Reset:
        _gs.MouseTolerance = value;
        _gs.MouseTolerance.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsPixelOffsetModeTest()
    {

        //Read:
        var value = _gs.PixelOffsetMode;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkPixelOffsetMode.pomDefault);
        // Loop:
        foreach (tkPixelOffsetMode enumValue in Enum.GetValues(typeof(tkPixelOffsetMode)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.PixelOffsetMode = enumValue;
            // Check:
            _gs.PixelOffsetMode.ShouldBe(enumValue);
        }
        // Reset:
        _gs.PixelOffsetMode = value;
        _gs.PixelOffsetMode.ShouldBe(value);

    }


    [Fact]
    public void GlobalSettingsAutoChooseRenderingHintForLabelsTest()
    {

        //Read:
        var value = _gs.AutoChooseRenderingHintForLabels;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.AutoChooseRenderingHintForLabels = !value;
        // Check:
        _gs.AutoChooseRenderingHintForLabels.ShouldBe(!value);
        // Reset:
        _gs.AutoChooseRenderingHintForLabels = value;
        _gs.AutoChooseRenderingHintForLabels.ShouldBe(value);

    }

    [Fact]
    public void GlobalSettingsOgrLayerForceUpdateModeTest()
    {
        //Read:
        var value = _gs.OgrLayerForceUpdateMode;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.OgrLayerForceUpdateMode = !value;
        // Check:
        _gs.OgrLayerForceUpdateMode.ShouldBe(!value);
        // Reset:
        _gs.OgrLayerForceUpdateMode = value;
        _gs.OgrLayerForceUpdateMode.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsForceHideLabelsTest()
    {
        //Read:
        var value = _gs.ForceHideLabels;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.ForceHideLabels = !value;
        // Check:
        _gs.ForceHideLabels.ShouldBe(!value);
        // Reset:
        _gs.ForceHideLabels = value;
        _gs.ForceHideLabels.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsBingApiKeyTest()
    {
        //Read:
        var value = _gs.BingApiKey;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty("BingApiKey should be null");
        // Change:
        const string newValue = "Foo-bar";
        _gs.BingApiKey = newValue;
        // Check:
        _gs.BingApiKey.ShouldBe(newValue);
        // Reset:
        _gs.BingApiKey = value;
        _gs.BingApiKey.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsCompressOverviewsTest()
    {
        //Read:
        var value = _gs.CompressOverviews;
        _testOutputHelper.WriteLine("Initial value " + value);
        value.ShouldBe(tkTiffCompression.tkmAUTO);
        // Loop:
        foreach (tkTiffCompression enumValue in Enum.GetValues(typeof(tkTiffCompression)))
        {
            _testOutputHelper.WriteLine("Value to check: " + enumValue.ToString());
            // Change:
            _gs.CompressOverviews = enumValue;
            // Check:
            _gs.CompressOverviews.ShouldBe(enumValue == tkTiffCompression.tkmAUTO
                ? tkTiffCompression.tkmJPEG // MapWinGIS custom default value
                : enumValue);
        }
        // Reset:
        _gs.CompressOverviews = value;
        _gs.CompressOverviews.ShouldBe(value == tkTiffCompression.tkmAUTO
            ? tkTiffCompression.tkmJPEG // MapWinGIS custom default value
            : value);
    }


    [Fact]
    public void GlobalSettingsGridFavorGreyscaleTest()
    {

        //Read:
        var value = _gs.GridFavorGreyscale;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.GridFavorGreyscale = !value;
        // Check:
        _gs.GridFavorGreyscale.ShouldBe(!value);
        // Reset:
        _gs.GridFavorGreyscale = value;
        _gs.GridFavorGreyscale.ShouldBe(value);

    }


    [Fact]
    public void GlobalSettingsGridUseHistogramTest()
    {

        //Read:
        var value = _gs.GridUseHistogram;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.GridUseHistogram = !value;
        // Check:
        _gs.GridUseHistogram.ShouldBe(!value);
        // Reset:
        _gs.GridUseHistogram = value;
        _gs.GridUseHistogram.ShouldBe(value);

    }


    [Fact]
    public void GlobalSettingsOverrideLocalCallbackTest()
    {

        //Read:
        var value = _gs.OverrideLocalCallback;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.OverrideLocalCallback = !value;
        // Check:
        _gs.OverrideLocalCallback.ShouldBe(!value);
        // Reset:
        _gs.OverrideLocalCallback = value;
        _gs.OverrideLocalCallback.ShouldBe(value);

    }


    [Fact]
    public void GlobalSettingsCacheDbfRecordsTest()
    {

        //Read:
        var value = _gs.CacheDbfRecords;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.CacheDbfRecords = !value;
        // Check:
        _gs.CacheDbfRecords.ShouldBe(!value);
        // Reset:
        _gs.CacheDbfRecords = value;
        _gs.CacheDbfRecords.ShouldBe(value);

    }


    [Fact]
    public void GlobalSettingsCacheShapeRenderingDataTest()
    {
        //Read:
        var value = _gs.CacheShapeRenderingData;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.CacheShapeRenderingData = !value;
        // Check:
        _gs.CacheShapeRenderingData.ShouldBe(!value);
        // Reset:
        _gs.CacheShapeRenderingData = value;
        _gs.CacheShapeRenderingData.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsTileLogFilenameTest()
    {
        //Read:
        var value = _gs.TileLogFilename;
        _testOutputHelper.WriteLine(value);
        value.ShouldBeNullOrEmpty("TileLogFilename should not be set");
        // Change:
        // TileLogFilename is read-only
    }


    [Fact]
    public void GlobalSettingsTileLogIsOpenedTest()
    {
        //Read:
        var value = _gs.TileLogIsOpened;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        // TileLogIsOpened is read-only
    }


    [Fact]
    public void GlobalSettingsLogTileErrorsOnlyTest()
    {
        //Read:
        var value = _gs.LogTileErrorsOnly;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.LogTileErrorsOnly = !value;
        // Check:
        _gs.LogTileErrorsOnly.ShouldBe(!value);
        // Reset:
        _gs.LogTileErrorsOnly = value;
        _gs.LogTileErrorsOnly.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsWmsDiskCachingTest()
    {

        //Read:
        var value = _gs.WmsDiskCaching;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.WmsDiskCaching = !value;
        // Check:
        _gs.WmsDiskCaching.ShouldBe(!value);
        // Reset:
        _gs.WmsDiskCaching = value;
        _gs.WmsDiskCaching.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsCallbackVerbosityTest()
    {
        //Read:
        var value = _gs.CallbackVerbosity;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(tkCallbackVerbosity.cvAll);
        // Loop:
        foreach (tkCallbackVerbosity enumValue in Enum.GetValues(typeof(tkCallbackVerbosity)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.CallbackVerbosity = enumValue;
            // Check:
            _gs.CallbackVerbosity.ShouldBe(enumValue);
        }
        // Reset:
        _gs.CallbackVerbosity = value;
        _gs.CallbackVerbosity.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsOgrShareConnectionTest()
    {
        //Read:
        var value = _gs.OgrShareConnection;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.OgrShareConnection = !value;
        // Check:
        _gs.OgrShareConnection.ShouldBe(!value);
        // Reset:
        _gs.OgrShareConnection = value;
        _gs.OgrShareConnection.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsOgrInterpretYnStringAsBooleanTest()
    {
        //Read:
        var value = _gs.OgrInterpretYNStringAsBoolean;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeTrue();
        // Change:
        _gs.OgrInterpretYNStringAsBoolean = !value;
        // Check:
        _gs.OgrInterpretYNStringAsBoolean.ShouldBe(!value);
        // Reset:
        _gs.OgrInterpretYNStringAsBoolean = value;
        _gs.OgrInterpretYNStringAsBoolean.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsAllowLayersWithIncompleteReprojectionTest()
    {
        //Read:
        var value = _gs.AllowLayersWithIncompleteReprojection;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBeFalse();
        // Change:
        _gs.AllowLayersWithIncompleteReprojection = !value;
        // Check:
        _gs.AllowLayersWithIncompleteReprojection.ShouldBe(!value);
        // Reset:
        _gs.AllowLayersWithIncompleteReprojection = value;
        _gs.AllowLayersWithIncompleteReprojection.ShouldBe(value);

    }

    [Fact]
    public void GlobalSettingsMaxUniqueValuesCountForGridSchemeTest()
    {
        //Read:
        var value = _gs.MaxUniqueValuesCountForGridScheme;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(100);
        // Change:
        _gs.MaxUniqueValuesCountForGridScheme = value * 2;
        // Check:
        _gs.MaxUniqueValuesCountForGridScheme.ShouldBe(value * 2);
        // Reset:
        _gs.MaxUniqueValuesCountForGridScheme = value;
        _gs.MaxUniqueValuesCountForGridScheme.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsDefaultColorSchemeForGridsTest()
    {
        //Read:
        var value = _gs.DefaultColorSchemeForGrids;
        _testOutputHelper.WriteLine(value.ToString());
        value.ShouldBe(PredefinedColorScheme.SummerMountains);
        // Loop:
        foreach (PredefinedColorScheme enumValue in Enum.GetValues(typeof(PredefinedColorScheme)))
        {
            _testOutputHelper.WriteLine(enumValue.ToString());
            // Change:
            _gs.DefaultColorSchemeForGrids = enumValue;
            // Check:
            _gs.DefaultColorSchemeForGrids.ShouldBe(enumValue);
        }
        // Reset:
        _gs.DefaultColorSchemeForGrids = value;
        _gs.DefaultColorSchemeForGrids.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsMinOverviewWidthTest()
    {
        //Read:
        var value = _gs.MinOverviewWidth;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(512);
        // Change:
        _gs.MinOverviewWidth = value * 2;
        // Check:
        _gs.MinOverviewWidth.ShouldBe(value * 2);
        // Reset:
        _gs.MinOverviewWidth = value;
        _gs.MinOverviewWidth.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsTilesThreadPoolSizeTest()
    {
        //Read:
        var value = _gs.TilesThreadPoolSize;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(5);
        // Change:
        _gs.TilesThreadPoolSize = value * 2;
        // Check:
        _gs.TilesThreadPoolSize.ShouldBe(value * 2);
        // Reset:
        _gs.TilesThreadPoolSize = value;
        _gs.TilesThreadPoolSize.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsTilesMinZoomOnProjectionMismatchTest()
    {
        //Read:
        var value = _gs.TilesMinZoomOnProjectionMismatch;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(6);
        // Change:
        _gs.TilesMinZoomOnProjectionMismatch = value * 2;
        // Check:
        _gs.TilesMinZoomOnProjectionMismatch.ShouldBe(value * 2);
        // Reset:
        _gs.TilesMinZoomOnProjectionMismatch = value;
        _gs.TilesMinZoomOnProjectionMismatch.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsOgrLayerMaxFeatureCountTest()
    {
        //Read:
        var value = _gs.OgrLayerMaxFeatureCount;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(50_000);
        // Change:
        _gs.OgrLayerMaxFeatureCount = value * 2;
        // Check:
        _gs.OgrLayerMaxFeatureCount.ShouldBe(value * 2);
        // Reset:
        _gs.OgrLayerMaxFeatureCount = value;
        _gs.OgrLayerMaxFeatureCount.ShouldBe(value);
    }

    [Fact]
    public void GlobalSettingsApplicationCallbackTest()
    {
        //Read:
        // Is already set in other unit test class:
        // var value = _gs.ApplicationCallback;
        // value.ShouldBeNull();
        // Change:
        _gs.ApplicationCallback = this;
        // Check:
        _gs.ApplicationCallback.ShouldNotBeNull();
        _gs.ApplicationCallback.ShouldBe(this);
        // Reset:
        _gs.ApplicationCallback = null;
    }

    [Fact]
    public void GlobalSettingsHotTrackingMaxShapeCountTest()
    {
        //Read:
        var value = _gs.HotTrackingMaxShapeCount;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(200);
        // Change:
        _gs.HotTrackingMaxShapeCount = value * 2;
        // Check:
        _gs.HotTrackingMaxShapeCount.ShouldBe(value * 2);
        // Reset:
        _gs.HotTrackingMaxShapeCount = value;
        _gs.HotTrackingMaxShapeCount.ShouldBe(value);
    }


    [Fact]
    public void GlobalSettingsSetHttpUserAgentTest()
    {
        //Read:
        _gs.SetHttpUserAgent("MyAwesomeApplication/v9.8");
        // Nothing to unit test
    }


    [Fact]
    public void GlobalSettingsTestBingApiKeyTest()
    {
        //Read:
        var value = _gs.TestBingApiKey("Wrong key");
        value.ShouldBeFalse();
    }


    [Fact]
    public void GlobalSettingsSetHereMapsApiKeyTest()
    {
        //Read:
        _gs.SetHereMapsApiKey("MyAppId", "MyAppCode");
        // Nothing to unit test
    }


    [Fact]
    public void GlobalSettingsStartLogTileRequestsTest()
    {
        //Read:
        var location = Path.Combine(Path.GetTempPath(), "tiles-log-Воздух.txt");
        var value = _gs.StartLogTileRequests(location);
        value.ShouldBeTrue();
        _gs.TileLogFilename.ShouldBe(location);
    }


    [Fact]
    public void GlobalSettingsStopLogTileRequestsTest()
    {
        //Read:
        _gs.StopLogTileRequests();
        // Nothing to unit test
    }


    [Fact]
    public void GlobalSettingsResetGdalErrorTest()
    {
        //Read:
        _gs.ResetGdalError();
        // Nothing to unit test
    }

    [Fact]
    public void GlobalSettingsMaxReprojectionShapeCountTest()
    {
        //Read:
        var value = _gs.MaxReprojectionShapeCount;
        _testOutputHelper.WriteLine(value.ToString(CultureInfo.InvariantCulture));
        value.ShouldBe(50_000);
        // Change:
        _gs.ClipperGcsMultiplicationFactor = value * 2;
        // Check:
        _gs.ClipperGcsMultiplicationFactor.ShouldBe(value * 2);
        // Reset:
        _gs.ClipperGcsMultiplicationFactor = value;
        _gs.ClipperGcsMultiplicationFactor.ShouldBe(value);
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
