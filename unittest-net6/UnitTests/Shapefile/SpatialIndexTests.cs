namespace unittest_net6.Shapefile;
public class SpatialIndexTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public SpatialIndexTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
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

        // Create index again:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeTrue("CreateSpatialIndex failed");

        // Check files:
        var mwdFileLocation = Path.ChangeExtension(sfFileLocation, ".mwd");
        File.Exists(mwdFileLocation).ShouldBeTrue("Can't find mwd file");
        var mwxFileLocation = Path.ChangeExtension(sfFileLocation, ".mwx");
        File.Exists(mwxFileLocation).ShouldBeTrue("Can't find mwx file");
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

        // Test again, should return false
        retVal = sfPolygon.RemoveSpatialIndex();
        retVal.ShouldBeFalse();
        sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("No valid spatial index can be found");

        // Create index:
        retVal = sfPolygon.CreateSpatialIndex();
        retVal.ShouldBeTrue("CreateSpatialIndex failed");

        // Check files, should exist:
        var mwdFileLocation = Path.ChangeExtension(sfFileLocation, ".mwd");
        File.Exists(mwdFileLocation).ShouldBeTrue("Can't find mwd file");
        var mwxFileLocation = Path.ChangeExtension(sfFileLocation, ".mwx");
        File.Exists(mwxFileLocation).ShouldBeTrue("Can't find mwx file");

        // Test again, should return true:
        retVal = sfPolygon.RemoveSpatialIndex();
        retVal.ShouldBeTrue("RemoveSpatialIndex failed: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);

        // Check files, should be removed:
        File.Exists(mwdFileLocation).ShouldBeFalse("mwd file still exists.");
        File.Exists(mwxFileLocation).ShouldBeFalse("mwx file still exists.");
    }
}
