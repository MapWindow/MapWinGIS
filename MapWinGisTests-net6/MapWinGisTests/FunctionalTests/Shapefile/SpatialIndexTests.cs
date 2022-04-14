namespace MapWinGisTests.FunctionalTests.Shapefile;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class SpatialIndexTests: ICallback
{
    private readonly ITestOutputHelper _testOutputHelper;

    public SpatialIndexTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
        _ = new GlobalSettings
        {
            ApplicationCallback = this,
            CallbackVerbosity = tkCallbackVerbosity.cvAll
        };
    }

    [Fact]
    public void CreateSpatialIndexTest()
    {
        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Create index, should fail because in-memory shapefile:
        var retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeFalse("CreateSpatialIndex was unexpectly successful");
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("The method isn't applicable to the in-memory object");

        // Save shapefile:
        var sfFileLocation = Helpers.SaveSfToTempFile(sfPolygon, "");
        _testOutputHelper.WriteLine(sfFileLocation);
        sfFileLocation.ShouldNotBeNullOrEmpty("Could not save shapefile");

        // Create index again:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeTrue("CreateSpatialIndex failed");

        // Check files:
        CheckIndexFiles(sfFileLocation, true);
    }

    //[Fact(Skip="Unit test is created, but fix is not yet implemented")]
    [Fact]
    public void CreateSpatialIndexUnicodeTest()
    {
        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Create index, should fail because in-memory shapefile:
        var retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeFalse("CreateSpatialIndex was unexpectly successful");
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("The method isn't applicable to the in-memory object");

        // Save shapefile with Unicode name:
        var sfFileLocation = Helpers.SaveSfToTempFile(sfPolygon, "Воздух");
        _testOutputHelper.WriteLine(sfFileLocation);
        sfFileLocation.ShouldNotBeNullOrEmpty("Could not save shapefile");        
        
        // Create index again:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeFalse("CreateSpatialIndex was unexpectly successful");
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("Cannot create spatial index with Unicode name.");

        // Check files:
        // CheckIndexFiles(sfFileLocation, true);
    }

    [Fact]
    public void HasSpatialIndexTest()
    {
        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Test, shoud return false and error:
        var retVal = sfPolygon.HasSpatialIndex;
        retVal.ShouldBeFalse("Shapefile has an unexpected spatial index");
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("The method isn't applicable to the in-memory object");

        // Save shapefile:
        var sfFileLocation = Helpers.SaveSfToTempFile(sfPolygon, "");
        _testOutputHelper.WriteLine(sfFileLocation);
        sfFileLocation.ShouldNotBeNullOrEmpty("Could not save shapefile");

        // Test again, should now return false, but no error:
        retVal = sfPolygon.HasSpatialIndex;
        retVal.ShouldBeFalse("Shapefile has an unexpected spatial index");
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("No Error");

        // Create again:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeTrue("CreateSpatialIndex failed");

        // Test again, should now return true:
        retVal = sfPolygon.HasSpatialIndex;
        retVal.ShouldBeTrue("Shapefile should have a spatial index");
    }

    [Fact]
    public void CanUseSpatialIndex()
    {
        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Test, shoud return false and error:
        var retVal = sfPolygon.CanUseSpatialIndex[sfPolygon.Extents];
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("The method isn't applicable to the in-memory object");

        // Save shapefile:
        var sfFileLocation = Helpers.SaveSfToTempFile(sfPolygon, "");
        _testOutputHelper.WriteLine(sfFileLocation);
        sfFileLocation.ShouldNotBeNullOrEmpty("Could not save shapefile");

        // Test again, should now return false, but no error:
        retVal = sfPolygon.CanUseSpatialIndex[sfPolygon.Extents];
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("No Error");

        // Create index:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeTrue("CreateSpatialIndex failed");

        // Test again, should still return false:
        retVal = sfPolygon.CanUseSpatialIndex[sfPolygon.Extents];
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("The selected extents divided by the shapefile extents exceeds the SpatialIndexMaxAreaPercent setting");

        // Test again with smaller extent, should return true:
        sfPolygon.Extents.GetBounds(out var xMin, out var yMin, out var zMin, out var xMax, out var yMax, out var zMax);
        var enlargedExtent = new Extents();
        const int enlargeValue = -100;
        enlargedExtent.SetBounds(xMin - enlargeValue, yMin - enlargeValue, zMin, xMax + enlargeValue, yMax + enlargeValue, zMax);
        retVal = sfPolygon.CanUseSpatialIndex[enlargedExtent];
        retVal.ShouldBeTrue("CanUseSpatialIndex failed: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);
    }

    [Fact]
    public void IsSpatialIndexValidTest()
    {
        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Test, shoud return false and error:
        var retVal = sfPolygon.IsSpatialIndexValid();
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("The method isn't applicable to the in-memory object");

        // Save shapefile:
        var sfFileLocation = Helpers.SaveSfToTempFile(sfPolygon, "");
        _testOutputHelper.WriteLine(sfFileLocation);
        sfFileLocation.ShouldNotBeNullOrEmpty("Could not save shapefile");

        // Test again, should return false
        retVal = sfPolygon.IsSpatialIndexValid();
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("No valid spatial index can be found");

        // Create index:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeTrue("CreateSpatialIndex failed");

        // Test again, should return true:
        retVal = sfPolygon.IsSpatialIndexValid();
        retVal.ShouldBeTrue("IsSpatialIndexValid failed: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);
    }

    [Fact]
    public void RemoveSpatialIndex()
    {
        // Create shapefile:
        var sfPolygon = Helpers.CreateTestPolygonShapefile();

        // Test, shoud return false and error:
        var retVal = sfPolygon.RemoveSpatialIndex();
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("The method isn't applicable to the in-memory object");

        // Save shapefile:
        var sfFileLocation = Helpers.SaveSfToTempFile(sfPolygon, "");
        _testOutputHelper.WriteLine(sfFileLocation);
        sfFileLocation.ShouldNotBeNullOrEmpty("Could not save shapefile");

        // Test again, should return false
        retVal = sfPolygon.RemoveSpatialIndex();
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("No valid spatial index can be found");

        // Create index:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeTrue("CreateSpatialIndex failed");

        // Check files, should exist:
        CheckIndexFiles(sfFileLocation, true);


        // Test again, should return true:
        retVal = sfPolygon.RemoveSpatialIndex();
        retVal.ShouldBeTrue("RemoveSpatialIndex failed: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);

        // Check files, should be removed:
        CheckIndexFiles(sfFileLocation, false);
    }

    [Fact]
    public void InvalidIndexFilesTest()
    {
        // As mentioned in https://github.com/MapWindow/MapWinGIS/issues/216

        // ogrinfo -geom=SUMMARY -so "D:\dev\MapWindow\MapWinGIS\git\unittest-net6\TestData\Issue-216.shp" Issue-216
        //Layer name: Issue-216
        //Geometry: Line String
        //Feature Count: 13424
        //Extent: (108.722071, 34.149021) - (109.139842, 34.457816)
        //Layer SRS WKT:
        //GEOGCRS["WGS 84",
        //    DATUM["World Geodetic System 1984",
        //        ELLIPSOID["WGS 84",6378137,298.257223563,
        //            LENGTHUNIT["metre",1]]],
        //    PRIMEM["Greenwich",0,
        //        ANGLEUNIT["degree",0.0174532925199433]],
        //    CS[ellipsoidal,2],
        //    AXIS["latitude",north,
        //        ORDER[1],
        //        ANGLEUNIT["degree",0.0174532925199433]],
        //    AXIS["longitude",east,
        //        ORDER[2],
        //        ANGLEUNIT["degree",0.0174532925199433]],
        //    ID["EPSG",4326]]
        //Data axis to CRS axis mapping: 2,1
        //osm_id: String (10.0)
        //code: Integer (5.0)
        //fclass: String (28.0)
        //name: String (100.0)
        //    ref: String (20.0)
        //oneway: String (1.0)
        //maxspeed: Integer (5.0)
        //layer: Real (19.11)
        //bridge: String (1.0)
        //tunnel: String (1.0)

        var sfLocation = Helpers.GetTestFilePath("Issue-216.shp");
        var sf = Helpers.OpenShapefile(sfLocation, this);
        Helpers.CheckEpsgCode(sf.GeoProjection, 4326, true);

        // Debug: Create spatial index files:
        //var retVal = sf.CreateSpatialIndex();
        //retVal.ShouldBeTrue("CreateSpatialIndex failed");

        sf.HasSpatialIndex.ShouldBeTrue();
        sf.UseSpatialIndex.ShouldBeTrue();
        sf.IsSpatialIndexValid().ShouldBeTrue();
        sf.HasInvalidShapes().ShouldBeFalse();
        sf.NumShapes.ShouldBe(13424);
        sf.Extents.xMin.ShouldBe(108.722071, 0.00001);
        sf.Extents.yMin.ShouldBe(34.149021, 0.00001);
        sf.Extents.xMax.ShouldBe(109.139842, 0.00001);
        sf.Extents.yMax.ShouldBe(34.457816, 0.00001);

        // Open using FileManager:
        var sf2 = Helpers.LoadSfUsingFileManager(sfLocation);
        sf2.HasSpatialIndex.ShouldBeTrue();
        sf2.UseSpatialIndex.ShouldBeTrue();
        sf2.IsSpatialIndexValid().ShouldBeTrue();
        sf2.HasInvalidShapes().ShouldBeFalse();
        sf2.NumShapes.ShouldBe(13424);
        sf2.Extents.xMin.ShouldBe(108.722071, 0.00001);
        sf2.Extents.yMin.ShouldBe(34.149021, 0.00001);
        sf2.Extents.xMax.ShouldBe(109.139842, 0.00001);
        sf2.Extents.yMax.ShouldBe(34.457816, 0.00001);
    }

    private static void CheckIndexFiles(string sfFileLocation, bool shouldExists)
    {
        var datFileLocation = Path.ChangeExtension(sfFileLocation, ".dat");
        if (shouldExists)
        {
            File.Exists(datFileLocation).ShouldBeTrue("Can't find dat file");
        }
        else
        {
            File.Exists(datFileLocation).ShouldBeFalse("The dat file still exists");
        }
        var idxFileLocation = Path.ChangeExtension(sfFileLocation, ".idx");
        if (shouldExists)
        {
            File.Exists(idxFileLocation).ShouldBeTrue("Can't find idx file");
        }
        else
        {
            File.Exists(idxFileLocation).ShouldBeFalse("The idx file still exists");
        }
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
