using System.Security.Policy;

namespace unittest_net6.Shapefile;
public class SpatialIndexTests
{
    private readonly ITestOutputHelper _testOutputHelper;

    public SpatialIndexTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
    }

    [Fact]
    public void CreateIndexTest()
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
    public void HasIndexTest()
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


        // TODO:
        //// Test again, should now return false, but no error:
        //retVal = sfPolygon.CanUseSpatialIndex[sfPolygon.Extents];
        //retVal.ShouldBeFalse();
        //sfPolygon.ErrorMsg[sfPolygon.LastErrorCode].ShouldBe("No Error");

        //// Create again:
        //retVal = sfPolygon.CreateSpatialIndex();
        //retVal.ShouldBeTrue("CreateSpatialIndex failed");

        //sfPolygon.EditingShapes.ShouldBeFalse();
        //sfPolygon.InteractiveEditing.ShouldBeFalse();

        //sfPolygon.UseSpatialIndex = true;

        //// Test again, should now return true:
        //retVal = sfPolygon.CanUseSpatialIndex[sfPolygon.Extents];
        //retVal.ShouldBeTrue("CanUseSpatialIndex failed: " + sfPolygon.ErrorMsg[sfPolygon.LastErrorCode]);
    }
}
