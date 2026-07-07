namespace MapWinGisTests.UnitTests;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class ShapeTests
{
	private readonly ITestOutputHelper _testOutputHelper;
    private readonly Shape _firstShapePoint;

    private readonly Extents _sfPointExtents;

    //private readonly Shape _firstShapePointZ;
    //private readonly Shape _firstShapePointM;
    //private readonly Shape _firstShapeMultiPoint;
    //private readonly Shape _firstShapePolyline;
    // etc
    public ShapeTests(ITestOutputHelper testOutputHelper)
    {
        _testOutputHelper = testOutputHelper;
        var sfPoint = Helpers.CreateRandomPointShapefile(50);
        _sfPointExtents = sfPoint.Extents;
        _firstShapePoint = sfPoint.Shape[0];
    }

    [Fact]
    public void ShapeNumPointsTest()
    {
        // Point shape
        _firstShapePoint.NumPoints.ShouldBe(1);
    }

    [Fact]
    public void ShapeNumPartsTest()
    {
        // Point shape
        _firstShapePoint.NumParts.ShouldBe(0);
    }

    [Fact]
    public void ShapeShapeTypeTest()
    {
        // Point shape
        _firstShapePoint.ShapeType.ShouldBe(ShpfileType.SHP_POINT);
        // TODO: Check other shapefile types as well
    }

    [Fact]
    public void ShapePointTest()
    {
        // Point shape
        _firstShapePoint.Point[0].ShouldNotBeNull();
        _firstShapePoint.Point[0].x.ShouldBeInRange(_sfPointExtents.xMin, _sfPointExtents.xMax);
        _firstShapePoint.Point[0].y.ShouldBeInRange(_sfPointExtents.yMin, _sfPointExtents.yMax);
    }

    [Fact]
    public void ShapePartTest()
    {
        // Point shape
        _firstShapePoint.Part[0].ShouldBe(-1);
    }

    [Fact]
    public void ShapeLastErrorCodeTest()
    {
        // Point shape
        _firstShapePoint.LastErrorCode.ShouldBe(0);
        // Trigger an error:
        _firstShapePoint.Part[0].ShouldBe(-1);
        _firstShapePoint.LastErrorCode.ShouldBe(1);
    }

    [Fact]
    public void ShapeErrorMsgTest()
    {
        var errorCodes = new Dictionary<int, string>
        {
            [-1] = "Invalid Error Code",
            [0] = "No Error",
            [1] = "Index Out of Bounds",
            [2] = "Parameter was NULL",
            [22]= "The property you called is not implemented",
            [201] = "Unsupported Shapefile Type"
        };

        foreach (var (key, value) in errorCodes)
        {
            _firstShapePoint.ErrorMsg[key].ShouldBe(value);
        }
    }
    
    [Fact]
    public void ShapeKeyTest()
    {
        const string keyString = "This is my key";
        _firstShapePoint.Key = keyString;
        _firstShapePoint.Key.ShouldBe(keyString);
    }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeGlobalCallbackTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeExtentsTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCentroidTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeLengthTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapePerimeterTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeAreaTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeIsValidTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeXyTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapePartIsClockWiseTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCenterTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeEndOfPartTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapePartAsShapeTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeIsValidReasonTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeInteriorPointTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeShapeType2DTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeIsEmptyTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapePut_ZTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeMTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeZTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeBufferWithParamsTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeMoveTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeRotateTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeSplitByPolylineTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeClearTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeFixUp2Test() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeInterpolatePointTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeProjectDistanceToTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCoversTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCoveredByTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCreateTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeInsertPointTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeDeletePointTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeInsertPartTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeDeletePartTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeSerializeToStringTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCreateFromStringTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapePointInThisPolyTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeRelatesTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeDistanceTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeBufferTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeClipTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeContainsTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCrossesTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeDisjointTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeEqualsTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeIntersectsTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeOverlapsTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeTouchesTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeWithinTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeBoundaryTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeConvexHullTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeReversePointsOrderTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeGetIntersectionTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCloneTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeExplodeTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void Shapeput_XYTest() { }

	[Fact]
	public void ShapeExportToBinaryTest()
    {
		var mPartPolyWktFileName = Path.Combine(Helpers.GetTestDataLocation(), "MultiPartPolygon.wkt");
        File.Exists(mPartPolyWktFileName).ShouldBeTrue("TestData file: MultiPartPolygon.wkt is missing!");
        var mPartPolyWkt = File.ReadAllText(mPartPolyWktFileName);

        var shape1 = new ShapeClass();
        var impOk = shape1.ImportFromWKT(mPartPolyWkt);
        impOk.ShouldBeTrue();

        var bytesArray = new object();
        var ret = shape1.ExportToBinary(ref bytesArray);
        ret.ShouldBeTrue();
        var shape2 = new ShapeClass();
        var impBinOk = shape2.ImportFromBinary(bytesArray);
        impBinOk.ShouldBeTrue();
		CompareShapes(shape1, shape2).ShouldBeTrue();
	}

    [Fact]
    public void ShapeImportFromBinaryTest()
    {
	    var polyShpDataFileName = Path.Combine(Helpers.GetTestDataLocation(), "polyShapeData.bin");
        File.Exists(polyShpDataFileName).ShouldBeTrue("TestData file: polyShapeData.bin is missing!");
	    var polyShpData = File.ReadAllBytes(polyShpDataFileName);

	    var shape1 = new ShapeClass();
	    shape1.ImportFromBinary(polyShpData);
	    var wkt = shape1.ExportToWKT();

		var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    var shape2 = sfPolygon.Shape[0];

		CompareShapes(shape1, shape2).ShouldBeTrue();
	}

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeFixUpTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeAddPointTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeExportToWktTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeImportFromWktTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeCopyFromTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeClosestPointsTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapePut_MTest() { }



    internal static bool CompareShapes(IShape shp1, IShape shp2, int decimals = 8)
    {
        if(shp1.ShapeType != shp2.ShapeType) return false;
        if(shp1.ShapeType2D != shp2.ShapeType2D) return false;
        if(shp1.NumParts != shp2.NumParts) return false;
		if(shp1.NumPoints != shp2.NumPoints) return false;

		for(var i = 0;i < shp1.NumParts;++i)
		{
			var pt1 = shp1.Point[i];
			var pt2 = shp2.Point[i];

			if(Math.Abs(pt1.x - pt2.x) > Math.Pow(10, -decimals)) return false;
			if(Math.Abs(pt1.y - pt2.y) > Math.Pow(10, -decimals)) return false;

			if(HaveZ(shp1.ShapeType))
			{
				if(Math.Abs(pt1.Z - pt2.Z) > Math.Pow(10, -decimals)) return false;
			}

			if(HaveM(shp1.ShapeType))
			{
				if(Math.Abs(pt1.M - pt2.M) > Math.Pow(10, -decimals)) return false;
			}
		}
		return true;
    }

 
    private static bool HaveZ(ShpfileType shpType)
    {
		return shpType is ShpfileType.SHP_POINTZ or ShpfileType.SHP_MULTIPOINTZ or ShpfileType.SHP_POLYLINEZ or ShpfileType.SHP_POLYGONZ;
	}

    private static bool HaveM(ShpfileType shpType)
    {
	    return shpType is ShpfileType.SHP_POINTM or ShpfileType.SHP_POINTZ or ShpfileType.SHP_MULTIPOINTM or ShpfileType.SHP_MULTIPOINTZ
		    or ShpfileType.SHP_POLYLINEM or ShpfileType.SHP_POLYLINEZ or ShpfileType.SHP_POLYGONM or ShpfileType.SHP_POLYGONZ;
	}

}
