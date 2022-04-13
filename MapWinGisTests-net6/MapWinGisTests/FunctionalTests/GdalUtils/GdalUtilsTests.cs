namespace MapWinGisTests.FunctionalTests.GdalUtils;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class GdalUtilsTests : ICallback
{
    private readonly ITestOutputHelper _testOutputHelper;

    public GdalUtilsTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
        _ = new GlobalSettings
        {
            ApplicationCallback = this,
            CallbackVerbosity = tkCallbackVerbosity.cvAll
        };
    }

    [Theory]
    [InlineData(true)]
    [InlineData(false)]
    public void GdalVectorTranslateReproject(bool useGdalVectorReproject)
    {
        // See also UtilsProjectionTests.UtilsReprojectShapefileTest() and ShapefileTests.ReprojectShapefileTest()

        ReprojectShapefile(187816.756, 433912.801, 5.86394184, 51.89276528, 0.0000001, 28992, 4326, useGdalVectorReproject);
        // The other way round:
        ReprojectShapefile(5.86394184, 51.89276528, 187816.756, 433912.801, 0.05, 4326, 28992, useGdalVectorReproject);

        // Amersfoort Geodetic to ETRS89 (EPSG:4258)
        ReprojectShapefile(4.5703125, 51.944265, 4.5706292, 51.945227, 0.000001, 4258, 4289, useGdalVectorReproject);
        // Swap:
        ReprojectShapefile(4.5706292, 51.945227, 4.5703125, 51.944265, 0.000001, 4289, 4258, useGdalVectorReproject);
    }

    private void ReprojectShapefile(double srcX, double srcY, double dstX, double dstY, double tolerance, int srcEpsgCode, int dstEpsgCode, bool useGdalVectorReproject)
    {
        _testOutputHelper.WriteLine("Use GdalVectorReproject: " + useGdalVectorReproject);
        // Create shapefile:
        var sfPoint = Helpers.MakeShapefile(ShpfileType.SHP_POINT);
        // Add shape:
        Helpers.AddPointShape(sfPoint, srcX, srcY);
        // Save shapefile to disk:
        var fileLocation = Helpers.SaveSfToTempFile(sfPoint, "");
        _testOutputHelper.WriteLine("fileLocation: " + fileLocation);
        fileLocation.ShouldNotBeNullOrEmpty("Could not save shapefile");

        // New file:
        var newFileLocation = Path.ChangeExtension(Path.Combine(Path.GetTempPath(), Path.GetRandomFileName()), ".shp");
        _testOutputHelper.WriteLine("newFileLocation: " + newFileLocation);

        var gdalUtils = new MapWinGIS.GdalUtils { GlobalCallback = this };
        if (useGdalVectorReproject)
        {
            var retVal = gdalUtils.GdalVectorReproject(fileLocation, newFileLocation, srcEpsgCode, dstEpsgCode);
            retVal.ShouldBeTrue("GdalVectorReproject failed: " + gdalUtils.DetailedErrorMsg);
        }
        else
        {
            var options = new[]
            {
                "-s_srs", "EPSG:" + srcEpsgCode,
                "-t_srs", "EPSG:" + dstEpsgCode
            };
            var retVal = gdalUtils.GdalVectorTranslate(fileLocation, newFileLocation, options);
            retVal.ShouldBeTrue("GdalVectorTranslate failed");
        }

        var sfNew = Helpers.OpenShapefile(newFileLocation, this);

        // Checks:
        sfNew.ShouldNotBeNull("ReprojectShapefile failed");
        sfNew.ShapefileType.ShouldBe(ShpfileType.SHP_POINT);
        sfNew.NumShapes.ShouldBe(1);
        var shpNew = sfNew.Shape[0];
        shpNew.ShouldNotBeNull();
        var pntNew = shpNew.Point[0];
        pntNew.ShouldNotBeNull();
        _testOutputHelper.WriteLine("X: {0}, Y: {1}", pntNew.x, pntNew.y);
        pntNew.x.ShouldBe(dstX, tolerance);
        pntNew.y.ShouldBe(dstY, tolerance);

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
