using System.Management;
using Shouldly;

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

	[Fact]
	public void ShapeExtentsTest()
    {
	    _firstShapePoint.ShouldNotBeNull();
		var ext = _firstShapePoint.Extents;

	    var xMin = double.MaxValue;
        var xMax = double.MinValue;
        var yMin = double.MaxValue;
        var yMax = double.MinValue;
	    for(var i = 0; i < _firstShapePoint.NumPoints; i++)
	    {
		    var pt = _firstShapePoint.Point[i];
            if(pt.x < xMin)
                xMin = pt.x;
            if(pt.x > xMax)
                xMax = pt.x;
            if(pt.y < yMin)
                yMin = pt.y;
            if(pt.y > yMax)
                yMax = pt.y;
	    }

        ext.xMin.ShouldBe(xMin);
        ext.xMax.ShouldBe(xMax);
        ext.yMin.ShouldBe(yMin);
        ext.yMax.ShouldBe(yMax);
	}

    [Fact]
	public void ShapeCentroidTest()
	{
		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		sfPolygon.ShouldNotBeNull();

		var shape = sfPolygon.Shape[0];
		var centroid = shape.Centroid;
		centroid.ShouldNotBeNull();

		var centroidShp = Helpers.MakeShape(centroid);
		shape.Contains(centroidShp).ShouldBeTrue();
	}

    [Fact]
	public void ShapeLengthTest()
	{
		var sfPolyline = Helpers.CreateTestPolylineShapefile();
		sfPolyline.ShouldNotBeNull();

		var expectedLength = 828.9983624899301;
		var shape = sfPolyline.Shape[0];

		((shape.Length - expectedLength) < 0.000001).ShouldBeTrue();
	}

	[Fact]
	public void ShapePerimeterTest()
	{
		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		sfPolygon.ShouldNotBeNull();

		var expectedPerimeter = 828.9983624899301;
		var shape = sfPolygon.Shape[0];
		((shape.Perimeter - expectedPerimeter) < 0.000001).ShouldBeTrue();
	}

    [Fact]
	public void ShapeAreaTest()
	{
		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		sfPolygon.ShouldNotBeNull();

		var expectedArea = 41521.544653236866;
		var shape = sfPolygon.Shape[0];
        var diff = Math.Abs(shape.Area - expectedArea);
        diff.ShouldBeLessThan(0.0001);
	}

	[Fact]
	public void ShapeIsValidTest()
	{
		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		sfPolygon.ShouldNotBeNull();

		var shape = sfPolygon.Shape[0];
        shape.IsValid.ShouldBeTrue();
	}

	[Fact]
	public void ShapeXyTest()
	{
		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		sfPolygon.ShouldNotBeNull();

		var shape = sfPolygon.Shape[0];
		var pt = shape.Point[0];

		double x = 0, y = 0;
		shape.get_XY(0, ref x, ref y);
		x.ShouldBe(pt.x);
		y.ShouldBe(pt.y);

		x += 15;
		y += 10;

		shape.put_XY(0, x, y);
		pt = shape.Point[0];
		x.ShouldBe(pt.x);
		y.ShouldBe(pt.y);
	}

    [Fact]
	public void ShapePartIsClockWiseTest()
	{
		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		sfPolygon.ShouldNotBeNull();

		var shape = sfPolygon.Shape[0];

		shape.PartIsClockWise[0].ShouldBeTrue();
	}

    [Fact]
    public void ShapeCenterTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];

	    var center = shape.Center;
		center.ShouldNotBeNull();

		var centerShp = Helpers.MakeShape(center);
		shape.Contains(centerShp).ShouldBeTrue();
	}

    [Fact]
    public void ShapeEndOfPartTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
        shape.EndOfPart[0].ShouldBeGreaterThan(0);
	}

    [Fact]
    public void ShapePartAsShapeTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
	    var partShape = shape.PartAsShape[0];
        partShape.ShouldNotBeNull();
    }

    [Fact]
    public void ShapeIsValidReasonTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
	    shape.DeletePoint(shape.NumPoints - 1);
        shape.IsValid.ShouldBeFalse();
        if(!shape.IsValid)
	        shape.IsValidReason.ShouldNotBeEmpty();
	}

    [Fact]
    public void ShapeInteriorPointTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
	    var x = (shape.Extents.xMax - shape.Extents.xMin) / 2.0 + shape.Extents.xMin;
        var y = (shape.Extents.yMax - shape.Extents.yMin) / 2.0 + shape.Extents.yMin;

        var interiorPoint = shape.InteriorPoint;
        interiorPoint.ShouldNotBeNull();
        (Math.Abs(interiorPoint.x - x) < 10).ShouldBeTrue();
        (Math.Abs(interiorPoint.y - y) < 10).ShouldBeTrue();
    }

    [Fact]
    public void ShapeShapeType2DTest()
    {
	    var shape = Helpers.MakeShape(ShpfileType.SHP_POLYLINEZ);
        shape.ShapeType2D.ShouldBe(ShpfileType.SHP_POLYLINE);

        shape = Helpers.MakeShape(ShpfileType.SHP_POLYGONZ);
        shape.ShapeType2D.ShouldBe(ShpfileType.SHP_POLYGON);

        shape = Helpers.MakeShape(ShpfileType.SHP_POINTZ);
        shape.ShapeType2D.ShouldBe(ShpfileType.SHP_POINT);
	}

    [Fact]
    public void ShapeIsEmptyTest()
    {
	    var shp = Helpers.MakeShapefile(ShpfileType.SHP_POLYLINEZ);
        shp.ShouldNotBeNull();

        var shape = Helpers.MakeShape(ShpfileType.SHP_POLYLINEZ);
		shape.IsEmpty.ShouldBeTrue();

        var sfPolygon = Helpers.CreateTestPolygonShapefile();
        sfPolygon.ShouldNotBeNull();
        shape = sfPolygon.Shape[0];
        shape.IsEmpty.ShouldBeFalse();
	}

    [Fact]
    public void ShapePut_ZTest()
    {
		Shape shape = new Shape();
		shape.ShapeType = ShpfileType.SHP_POINTZ;
		shape.AddPoint(100, 100);

		var z = 3.14;
		shape.put_Z(0, z).ShouldBeTrue();

		shape.get_Z(0, out var zValue);
		zValue.ShouldBe(z);
    }

    [Fact]
    public void ShapeMTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];

	    var m = 3.1415;
	    shape.put_M(0, m).ShouldBeTrue();
        shape.get_M(0, out var mValue).ShouldBeTrue();
        mValue.ShouldBe(m);
    }

    [Fact]
    public void ShapeZTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];

	    var z = 3.1415;
	    shape.put_Z(0, z).ShouldBeTrue();
	    shape.get_Z(0, out var zValue).ShouldBeTrue();
	    zValue.ShouldBe(z);
	}

    [Fact]
    public void ShapeBufferWithParamsTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
	    var dist = 10.0;
	    var newShape = shape.BufferWithParams(dist, 30, false, tkBufferCap.bcROUND, tkBufferJoin.bjROUND, 5.0);
        newShape.ShouldNotBeNull();
        (shape.Area < newShape.Area).ShouldBeTrue();
    }

    [Fact]
    public void ShapeMoveTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
	    var xCenter = shape.Center.x;
        var yCenter = shape.Center.y;

	    var xOffset = 15;
	    var yOffset = 20;
        shape.Move(xOffset, yOffset);

        var center = shape.Center;
        var xDiff = center.x - xCenter;
        var yDiff = center.y - yCenter;
        xDiff.ShouldBe(xOffset);
        yDiff.ShouldBe(yOffset);
	}

    [Fact]
    public void ShapeRotateTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
	    var orgShape = shape.Clone();

	    var center = shape.Center;
        shape.Rotate(center.x, center.y, 360);

        shape.Area.ShouldBe(orgShape.Area);
        shape.NumPoints.ShouldBe(orgShape.NumPoints);

		for (var i = 0; i < shape.NumPoints; i++)
        {
			var pt0 = orgShape.Point[i];
			var pt1 = shape.Point[i];
			pt0.ShouldNotBeNull();
			pt1.ShouldNotBeNull();
			pt0.x.ShouldBe(pt1.x);
			pt0.y.ShouldBe(pt1.y);
		}
	}

    [Fact]
    public void ShapeSplitByPolylineTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];

	    var polyline = Helpers.MakeShape(ShpfileType.SHP_POLYLINE);
        polyline.ShouldNotBeNull();
        var wkt = "LineString (330431.80617637105751783 5914860.0969890458509326, 330766.5170847776462324 5914971.03695115447044373, 330766.5170847776462324 5914971.03695115447044373)";
        polyline.ImportFromWKT(wkt).ShouldBeTrue();

		object result = null;
		shape.SplitByPolyline(polyline, ref result).ShouldBeTrue();

        result.ShouldBeOfType(typeof(object[]));
        var array = result as object[];
        array.ShouldNotBeNull();
        array.Length.ShouldBe(2);

		IShape shape0 = array[0] as Shape;
		shape0.ShouldNotBeNull();
        shape0.IsValid.ShouldBeTrue();

		IShape shape1 = array[1] as Shape;
		shape1.ShouldNotBeNull();
		shape1.IsValid.ShouldBeTrue();

		var diff = Math.Abs((shape0.Area + shape1.Area) - shape.Area);
        diff.ShouldBeLessThan(0.001);
    }

    [Fact]
    public void ShapeClearTest()
    {
	    var sfPolygon = Helpers.CreateTestPolygonShapefile();
	    sfPolygon.ShouldNotBeNull();

	    var shape = sfPolygon.Shape[0];
        shape.ShouldNotBeNull();
        shape.IsEmpty.ShouldBeFalse();

        shape.Clear();
        shape.IsEmpty.ShouldBeTrue();
	}

	[Fact]
	public void ShapeFixUp2Test()
	{
		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		sfPolygon.ShouldNotBeNull();

		var shape = sfPolygon.Shape[0];
		var newShape = shape.FixUp2(tkUnitsOfMeasure.umMeters);
		newShape.ShouldNotBeNull();

		shape.DeletePoint(shape.NumPoints - 1);
		shape.IsValid.ShouldBeFalse();

		newShape = shape.FixUp2(tkUnitsOfMeasure.umMeters);
		newShape.ShouldNotBeNull();
		newShape.IsValid.ShouldBeTrue();
	}

	[Fact]
	public void ShapeInterpolatePointTest()
	{
		var sfPolyline = Helpers.CreateTestPolylineShapefile();
		sfPolyline.ShouldNotBeNull();

		var shape = sfPolyline.Shape[0];
		var pt = shape.InterpolatePoint(shape.Point[0], 0.5);
		pt.ShouldNotBeNull();

		var expectedX = 330696.21521950705;
		var expectedY = 5914895.72546695;
		ShouldBeExtensions.ShouldBeEqualWithin(pt.x, expectedX, 0.0001);
		ShouldBeExtensions.ShouldBeEqualWithin(pt.y, expectedY, 0.0001);
	}

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

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeExportToBinaryTest() { }

    [Fact(Skip = "Unit test is not yet implemented")]
    public void ShapeImportFromBinaryTest() { }

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
}
