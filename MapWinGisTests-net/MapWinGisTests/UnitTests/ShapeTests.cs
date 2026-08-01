using Xunit.Abstractions;

namespace MapWinGisTests.UnitTests;

[Collection(nameof(NotThreadSafeResourceCollection))]
public class ShapeTests
{
	private readonly ITestOutputHelper _testOutputHelper;
	private readonly Shape _firstShapePoint;
	private readonly Shape _firstShapePolygon;
	private readonly Shape _firstShapePolyline;

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

		var sfPolygon = Helpers.CreateTestPolygonShapefile();
		_firstShapePolygon = sfPolygon.Shape[0];

		var sfPolyline = Helpers.CreateTestPolylineShapefile();
		_firstShapePolyline = sfPolyline.Shape[0];
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
		_firstShapePolygon.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		_firstShapePolyline.ShapeType.ShouldBe(ShpfileType.SHP_POLYLINE);
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
		_firstShapePolygon.Part[0].ShouldBe(0);
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

	[Fact(Skip = "Unit test is not yet implemented, Shape.GlobalCallback is deprecated")]
	public void ShapeGlobalCallbackTest() { }

	[Fact]
	public void ShapeExtentsTest()
	{
		_firstShapePolygon.Extents.ShouldNotBeNull();
		var ext = _firstShapePolygon.Extents;

		ext.xMin.ShouldBe(330471.411812074);
		ext.xMax.ShouldBe(330716.510827627);

		ext.yMin.ShouldBe(5914714.83347535);
		ext.yMax.ShouldBe(5915031.20447119);
	}

	[Fact]
	public void ShapeCentroidTest()
	{
		_firstShapePolygon.Centroid.ShouldNotBeNull();
		var centroid = _firstShapePolygon.Centroid;
		centroid.x.ShouldBe(330588.63919048081);
		centroid.y.ShouldBe(5914879.6435981784);
	}

	[Fact]
	public void ShapeLengthTest()
	{
		_firstShapePolyline.Length.ShouldBe(673.275809807466);
	}

	[Fact]
	public void ShapePerimeterTest()
	{
		_firstShapePolygon.Perimeter.ShouldBe(828.99836248993006);
	}

	[Fact]
	public void ShapeAreaTest()
	{
		_firstShapePolygon.Area.ShouldBe(41521.544677734375);
	}

	[Fact]
	public void ShapeIsValidTest()
	{
		_firstShapePolygon.IsValid.ShouldBeTrue();
	}

	[Fact]
	public void ShapeXyTest()
	{
		var shape = _firstShapePoint.Clone();
		shape.put_XY(0, 12345, 4567).ShouldBeTrue();
		var pt = shape.Point[0];
		pt.ShouldNotBeNull();
		pt.x.ShouldBe(12345);
		pt.y.ShouldBe(4567);
	}

	[Fact]
	public void ShapePartIsClockWiseTest()
	{
		_firstShapePolygon.PartIsClockWise[0].ShouldBeTrue();
		_firstShapePolyline.PartIsClockWise[0].ShouldBeFalse();
	}

	[Fact]
	public void ShapeCenterTest()
	{
		_firstShapePolygon.Centroid.ShouldNotBeNull();
		var center = _firstShapePolygon.Center;
		center.x.ShouldBe(330593.96131985052);
		center.y.ShouldBe(5914873.01897327);
	}

	[Fact]
	public void ShapeEndOfPartTest()
	{
		_firstShapePolygon.EndOfPart[0].ShouldBe(23);
	}

	[Fact]
	public void ShapePartAsShapeTest()
	{
		var shape = _firstShapePolygon.PartAsShape[0];
		shape.ShouldNotBeNull();
		Math.Round(shape.Area, 4).ShouldBe(41521.5447);
	}

	[Fact]
	public void ShapeIsValidReasonTest()
	{
		var shape = _firstShapePolygon.Clone();
		var ret = shape.ReversePointsOrder(0);
		ret.ShouldBeTrue();
		shape.IsValid.ShouldBeFalse();
		shape.IsValidReason.ShouldNotBeEmpty();
	}

	[Fact]
	public void ShapeInteriorPointTest()
	{
		_firstShapePolygon.InteriorPoint.ShouldNotBeNull();
		var pt = _firstShapePolygon.InteriorPoint;
		_firstShapePolygon.PointInThisPoly(pt).ShouldBeTrue();
	}

	[Fact]
	public void ShapeShapeType2DTest()
	{
		_firstShapePoint.ShapeType.ShouldBe(_firstShapePoint.ShapeType2D);
		_firstShapePolyline.ShapeType.ShouldBe(_firstShapePolyline.ShapeType2D);
		_firstShapePolygon.ShapeType.ShouldBe(_firstShapePolygon.ShapeType2D);
	}

	[Fact]
	public void ShapeIsEmptyTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.IsEmpty.ShouldBeFalse();
		shape.Clear();
		shape.IsEmpty.ShouldBeTrue();
	}

	[Fact]
	public void ShapePut_ZTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.put_Z(0, 99.9).ShouldBeTrue();
		shape.get_Z(0, out var zValue).ShouldBeTrue();
		zValue.ShouldBe(99.9);
	}

	[Fact]
	public void ShapeMTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.put_M(0, 123.4).ShouldBeTrue();
		shape.get_M(0, out var mValue).ShouldBeTrue();
		mValue.ShouldBe(123.4);
	}

	[Fact]
	public void ShapeZTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.put_Z(0, 99.9).ShouldBeTrue();
		shape.get_Z(0, out var zValue).ShouldBeTrue();
		zValue.ShouldBe(99.9);
	}

	[Fact]
	public void ShapeBufferWithParamsTest()
	{
		var bufferShape = _firstShapePolygon.BufferWithParams(1.0, 10, true, tkBufferCap.bcFLAT, tkBufferJoin.bjROUND, 5);
		bufferShape.ShouldNotBeNull();
		bufferShape.Area.ShouldBeGreaterThan(_firstShapePolygon.Area);
	}

	[Fact]
	public void ShapeMoveTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.Move(10, 10);

		var oldArea = Math.Round(_firstShapePolygon.Area, 2);
		var newArea = Math.Round(shape.Area, 2);
		newArea.ShouldBe(oldArea);
	}

	[Fact]
	public void ShapeRotateTest()
	{
		var shape = _firstShapePolygon.Clone();
		var center = shape.Center;
		shape.Rotate(center.x, center.y, 90);

		var oldArea = Math.Round(_firstShapePolygon.Area, 2);
		var newArea = Math.Round(shape.Area, 2);
		newArea.ShouldBe(oldArea);
	}

	[Fact]
	public void ShapeSplitByPolylineTest()
	{
		var wkt = "LINESTRING (330452.29615390772232786 5914925.46296628564596176, 330753.68315174279268831 5914844.20796951279044151, 330753.68315174279268831 5914844.20796951279044151)";
		var sfSplitLine = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
		var splitLine = sfSplitLine.Shape[0];

		var gs = new GlobalSettings();
		gs.ApplicationCallback = MapCallback.Instance;
		var shape = _firstShapePolygon.Clone();
		shape.GlobalCallback = MapCallback.Instance;
		var result = new object();
		shape.SplitByPolyline(splitLine, ref result).ShouldBeTrue();
		if(result is Array array)
		{
			array.Length.ShouldBe(2);

			var obj1 = array.GetValue(0);
			var obj2 = array.GetValue(1);
			(obj1 is Shape).ShouldBeTrue();
			(obj2 is Shape).ShouldBeTrue();
			var shape1 = (Shape)obj1;
			var shape2 = (Shape)obj2;
			shape1.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
			shape2.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
			Math.Round(shape1.Area, 5).ShouldBe(21938.30627);
			Math.Round(shape2.Area, 5).ShouldBe(19583.23840);
		}
	}

	[Fact]
	public void ShapeClearTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.NumParts.ShouldBe(1);
		shape.NumPoints.ShouldBe(24);
		shape.Clear();
		shape.NumParts.ShouldBe(0);
		shape.NumPoints.ShouldBe(0);
	}

	[Fact]
	public void ShapeFixUp2Test()
	{
		var shape = _firstShapePolygon.Clone();
		var ret = shape.ReversePointsOrder(0);
		ret.ShouldBeTrue();

		shape.IsValid.ShouldBeFalse();

		var fixedShape = shape.FixUp2(tkUnitsOfMeasure.umMeters);
		fixedShape.ShouldNotBeNull();
		fixedShape.IsValid.ShouldBeTrue();
	}

	[Fact]
	public void ShapeInterpolatePointTest()
	{
		var startPt = _firstShapePolyline.Point[0];
		var pt = _firstShapePolyline.InterpolatePoint(startPt, 100);
		Math.Round(pt.x, 5).ShouldBe(330672.85460);
		Math.Round(pt.y, 5).ShouldBe(5914822.27784);

		pt = _firstShapePolyline.InterpolatePoint(startPt, _firstShapePolyline.Length);
		var endPt = _firstShapePolyline.Point[_firstShapePolyline.NumPoints - 1];
		Math.Round(pt.x, 5).ShouldBe(Math.Round(endPt.x, 5));
		Math.Round(pt.y, 5).ShouldBe(Math.Round(endPt.y, 5));
	}

	[Fact]
	public void ShapeProjectDistanceToTest()
	{
		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330672.85460291471 5914822.2778402278)", 28992);
		var ptShape = ptShapefile.Shape[0];

		var distance = _firstShapePolyline.ProjectDistanceTo(ptShape);	// Testing with point on the polyline
		Math.Round(distance, 5).ShouldBe(100);

		ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330481.80419807904399931 5914854.63038331270217896)");
		ptShape = ptShapefile.Shape[0];
		distance = _firstShapePolyline.ProjectDistanceTo(ptShape);		// Testing with point away from the polyline
		Math.Round(distance, 5).ShouldBe(419.69482);
	}

	[Fact]
	public void ShapeCoversTest()
	{
		var wkt = "LINESTRING (803330.82357044471427798 10894657.53224020265042782, 803573.5578525101300329 10894530.3940687570720911, 803573.5578525101300329 10894530.3940687570720911)";
		var sfSplitLine = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
		var splitLine = sfSplitLine.Shape[0];
		_firstShapePolygon.Covers(splitLine).ShouldBeFalse();

		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330575.24080588802462444 5914889.61517359130084515)", 28992);
		var ptShape = ptShapefile.Shape[0];
		_firstShapePolygon.Covers(ptShape).ShouldBeTrue();
	}

	[Fact]
	public void ShapeCoveredByTest()
	{
		var wkt = "LINESTRING (803330.82357044471427798 10894657.53224020265042782, 803573.5578525101300329 10894530.3940687570720911, 803573.5578525101300329 10894530.3940687570720911)";
		var sfSplitLine = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
		var splitLine = sfSplitLine.Shape[0];
		_firstShapePolygon.CoveredBy(splitLine).ShouldBeFalse();

		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330552.00612543808529153 5914719.5373126994818449)", 28992);
		var ptShape = ptShapefile.Shape[0];
		ptShape.CoveredBy(_firstShapePolygon).ShouldBeTrue();
	}

	[Fact]
	public void ShapeCreateTest()
	{
		var shape = new Shape();
		shape.Create(ShpfileType.SHP_POINTZ).ShouldBeTrue();
		shape.ShapeType2D.ShouldBe(ShpfileType.SHP_POINT);
	}

	[Fact]
	public void ShapeInsertPointTest()
	{
		var shape = _firstShapePolygon.Clone();
		var pointIndex = 10;
		var newPt = new Point { x = 330600, y = 5914800 };
		shape.InsertPoint(newPt, ref pointIndex).ShouldBeTrue();
		shape.NumPoints.ShouldBe(25);
	}

	[Fact]
	public void ShapeDeletePointTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.NumPoints.ShouldBe(24);
		shape.DeletePoint(5).ShouldBeTrue();
		shape.NumPoints.ShouldBe(23);
	}

	[Fact]
	public void ShapeInsertPartTest()
	{
		/*var wkt = "POLYGON ((330655.56641544314334169 5915024.34312780015170574, 330712.39180534332990646 5915044.78964659571647644, 330729.12077526719076559 5914998.32028569653630257, 330678.13724788010586053 5914978.13930610567331314, 330678.13724788010586053 5914978.13930610567331314, 330655.56641544314334169 5915024.34312780015170574))";
		var sfPolygon = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);
		var polygon = sfPolygon.Shape[0].Clone(); */
		var shape = _firstShapePolygon.Clone();
		var partIndex = 0;
		shape.InsertPart(shape.NumPoints, ref partIndex).ShouldBeTrue();
	}

	[Fact]
	public void ShapeDeletePartTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.DeletePart(0).ShouldBeTrue();
		shape.IsValid.ShouldBeFalse();
	}

	[Fact]
	public void ShapeSerializeToStringTest()
	{
		var text = _firstShapePolygon.SerializeToString();
		text.ShouldNotBeNullOrEmpty();

		var shape = new Shape();
		shape.CreateFromString(text).ShouldBeTrue();
		shape.IsValid.ShouldBeTrue();
	}

	[Fact]
	public void ShapeCreateFromStringTest()
	{
		var shape = new Shape();
		var text = "5;0;330695.973323|5914896.163058|330711.986130|5914867.195862|330713.350435|5914867.566440|330716.510828|5914862.289737|330715.632569|5914860.601080|330652.234583|5914803.805106|330553.749382|5914715.803282|330551.979356|5914714.833475|330549.911989|5914715.865028|330545.027807|5914724.059164|330544.592986|5914725.935315|330544.309637|5914726.727547|330543.612621|5914726.149046|330543.271516|5914727.066339|330542.234090|5914729.855977|330542.959655|5914730.504120|330530.319253|5914765.860642|330505.294840|5914836.793012|330471.411812|5914931.615583|330486.074749|5914941.337952|330585.983155|5915010.327491|330618.427962|5915031.204471|330653.234602|5914970.373281|330695.973323|5914896.163058|";
		shape.CreateFromString(text).ShouldBeTrue();
		shape.IsValid.ShouldBeTrue();
	}

	[Fact]
	public void ShapePointInThisPolyTest()
	{
		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330552.00612543808529153 5914719.5373126994818449)", null);
		var pt = ptShapefile.Shape[0].Point[0];
		_firstShapePolygon.PointInThisPoly(pt).ShouldBeTrue();
	}

	[Fact]
	public void ShapeRelatesTest()
	{
		var wkt = "LINESTRING (330452.29615390772232786 5914925.46296628564596176, 330753.68315174279268831 5914844.20796951279044151, 330753.68315174279268831 5914844.20796951279044151)";
		var sfSplitLine = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
		var splitLine = sfSplitLine.Shape[0];
		var crosses = splitLine.Crosses(_firstShapePolygon);
		splitLine.Relates(_firstShapePolygon, tkSpatialRelation.srCrosses).ShouldBe(crosses);

		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330575.24080588802462444 5914889.61517359130084515)", 28992);
		var ptShape = ptShapefile.Shape[0];
		var covers = _firstShapePolygon.Covers(ptShape);
		_firstShapePolygon.Relates(ptShape, tkSpatialRelation.srCovers).ShouldBe(covers);
	}

	[Fact]
	public void ShapeDistanceTest()
	{
		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330552.00612543808529153 5914719.5373126994818449)", null);
		var pt = ptShapefile.Shape[0];
		_firstShapePolygon.Distance(pt).ShouldBe(0);

		ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330481.80419807904399931 5914854.63038331270217896)");
		pt = ptShapefile.Shape[0];
		var distance = _firstShapePolygon.Distance(pt);
		Math.Round(distance, 5).ShouldBe(16.11865);
	}

	[Fact]
	public void ShapeBufferTest()
	{
		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330552.00612543808529153 5914719.5373126994818449)", null);
		var pt = ptShapefile.Shape[0];
		var shape = pt.Buffer(10, 100);
		shape.ShouldNotBeNull();
		shape.IsValid.ShouldBeTrue();
		shape.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		Math.Round(shape.Area, 5).ShouldBe(314.14575);
	}

	[Fact]
	public void ShapeClipTest()
	{
		var wkt = "LINESTRING (330452.29615390772232786 5914925.46296628564596176, 330753.68315174279268831 5914844.20796951279044151, 330753.68315174279268831 5914844.20796951279044151)";
		var sfSplitLine = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
		var splitLine = sfSplitLine.Shape[0];
		var clipResult = _firstShapePolygon.Clip(splitLine, tkClipOperation.clClip);
		clipResult.ShapeType.ShouldBe(ShpfileType.SHP_POLYLINE);
		Math.Round(clipResult.Length, 4).ShouldBe(242.8589);

		wkt = "POLYGON ((330496.242892358568497 5914947.07121910341084003, 330477.25683919101720676 5914958.3566353227943182, 330456.94308999768691137 5914937.91011652629822493, 330462.51941330567933619 5914901.79678462725132704, 330505.13845573080470785 5914918.12744574341922998, 330505.13845573080470785 5914918.12744574341922998, 330496.242892358568497 5914947.07121910341084003))";
		var sfMiniPolygon = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);
		var miniPolygon = sfMiniPolygon.Shape[0];
		clipResult = _firstShapePolygon.Clip(miniPolygon, tkClipOperation.clUnion);
		clipResult.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		Math.Round(clipResult.Area, 4).ShouldBe(42477.6892);

		clipResult = _firstShapePolygon.Clip(miniPolygon, tkClipOperation.clIntersection);
		clipResult.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		Math.Round(clipResult.Area, 4).ShouldBe(777.1583);

		clipResult = _firstShapePolygon.Clip(miniPolygon, tkClipOperation.clDifference);
		clipResult.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		Math.Round(clipResult.Area, 4).ShouldBe(40744.3864);
	}

	[Fact]
	public void ShapeContainsTest()
	{
		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330552.00612543808529153 5914719.5373126994818449)", null);
		var pt = ptShapefile.Shape[0];
		_firstShapePolygon.Contains(pt).ShouldBeTrue();

		ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330481.80419807904399931 5914854.63038331270217896)");
		pt = ptShapefile.Shape[0];
		_firstShapePolygon.Contains(pt).ShouldBeFalse();
	}

	[Fact]
	public void ShapeCrossesTest()
	{
		var wkt = "LINESTRING (330452.29615390772232786 5914925.46296628564596176, 330753.68315174279268831 5914844.20796951279044151, 330753.68315174279268831 5914844.20796951279044151)";
		var sfSplitLine = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
		var splitLine = sfSplitLine.Shape[0];
		splitLine.Crosses(_firstShapePolygon).ShouldBeTrue();
	}

	[Fact]
	public void ShapeDisjointTest()
	{
		_firstShapePolygon.Disjoint(_firstShapePolyline).ShouldBeFalse();

		var shape = _firstShapePolygon.Clone();
		var polygon = shape.Buffer(-10, 50);
		polygon.Disjoint(_firstShapePolyline).ShouldBeTrue();
	}

	[Fact]
	public void ShapeEqualsTest()
	{
		var shape = _firstShapePolygon.Clone();
		_firstShapePolygon.Equals(shape).ShouldBeTrue();

		var pointIndex = 10;
		var newPt = new Point { x = 330600, y = 5914800 };
		shape.InsertPoint(newPt, ref pointIndex).ShouldBeTrue();
		_firstShapePolygon.Equals(shape).ShouldBeFalse();
	}

	[Fact]
	public void ShapeIntersectsTest()
	{
		var wkt = "LINESTRING (330452.29615390772232786 5914925.46296628564596176, 330753.68315174279268831 5914844.20796951279044151, 330753.68315174279268831 5914844.20796951279044151)";
		var sfSplitLine = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
		var splitLine = sfSplitLine.Shape[0];
		splitLine.Intersects(_firstShapePolygon).ShouldBeTrue();
	}

	[Fact]
	public void ShapeOverlapsTest()
	{
		var shape = _firstShapePolygon.Clone();
		shape.Move(10, 10);
		_firstShapePolygon.Overlaps(shape).ShouldBeTrue();
	}

	[Fact]
	public void ShapeTouchesTest()
	{
		_firstShapePolygon.Touches(_firstShapePolyline).ShouldBeTrue();
		var shape = _firstShapePolygon.Clone();
		var polygon = shape.Buffer(-10, 50);
		polygon.Touches(_firstShapePolyline).ShouldBeFalse();
	}

	[Fact]
	public void ShapeWithinTest()
	{
		var ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330552.00612543808529153 5914719.5373126994818449)", null);
		var pt = ptShapefile.Shape[0];
		pt.Within(_firstShapePolygon).ShouldBeTrue();

		ptShapefile = Helpers.CreateTestShapefile(ShpfileType.SHP_POINT, "POINT (330481.80419807904399931 5914854.63038331270217896)");
		pt = ptShapefile.Shape[0];
		pt.Within(_firstShapePolygon).ShouldBeFalse();
	}

	[Fact]
	public void ShapeBoundaryTest()
	{
		var boundary = _firstShapePolygon.Boundary();
		boundary.ShouldNotBeNull();
		boundary.ShapeType.ShouldBe(ShpfileType.SHP_POLYLINE);
		boundary.Length.ShouldBe(_firstShapePolygon.Perimeter);
	}

	[Fact]
	public void ShapeConvexHullTest()
	{
		var shape = _firstShapePolygon.ConvexHull();
		shape.ShouldNotBeNull();
		Math.Round(shape.Area, 5).ShouldBe(41752.61182);
		_firstShapePolygon.Area.ShouldBeLessThan(shape.Area);
	}

	[Fact]
	public void ShapeReversePointsOrderTest()
	{
		var shape = _firstShapePolygon.Clone();
		var ret = shape.ReversePointsOrder(0);
		ret.ShouldBeTrue();
	}

	[Fact]
	public void ShapeGetIntersectionTest()
	{
		var wkt = "POLYGON ((330496.242892358568497 5914947.07121910341084003, 330477.25683919101720676 5914958.3566353227943182, 330456.94308999768691137 5914937.91011652629822493, 330462.51941330567933619 5914901.79678462725132704, 330505.13845573080470785 5914918.12744574341922998, 330505.13845573080470785 5914918.12744574341922998, 330496.242892358568497 5914947.07121910341084003))";
		var sfMiniPolygon = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);
		var miniPolygon = sfMiniPolygon.Shape[0];

		var results = new object();
		_firstShapePolygon.GetIntersection(miniPolygon, ref results).ShouldBeTrue();
		(results is Array).ShouldBeTrue();
		var array = (Array)results;
		array.Length.ShouldBe(1);
		var obj1 = array.GetValue(0);
		(obj1 is Shape).ShouldBeTrue();
		var shape1 = (Shape)obj1;
		shape1.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		Math.Round(shape1.Area, 5).ShouldBe(777.15833);
	}

	[Fact]
	public void ShapeCloneTest()
	{
		var shape = _firstShapePolygon.Clone();
		_firstShapePolygon.Equals(shape).ShouldBeTrue();
	}

	[Fact]
	public void ShapeExplodeTest()
	{
		var wkt = "MULTIPOLYGON (((330695.973322992 5914896.16305817, 330711.986129861 5914867.19586245, 330713.350435287 5914867.56644015, 330716.510827627 5914862.28973662, 330715.632568651 5914860.60107999, 330652.234582712 5914803.80510632, 330553.749382483 5914715.80328169, 330551.979355848 5914714.83347535, 330549.911988583 5914715.86502807, 330545.027807355 5914724.05916443, 330544.592985976 5914725.93531509, 330544.30963704 5914726.72754692, 330543.612620707 5914726.14904553, 330543.271515787 5914727.06633931, 330542.234090059 5914729.85597723, 330542.959654761 5914730.50411962, 330530.319252794 5914765.86064153, 330505.294840402 5914836.7930124, 330471.411812074 5914931.61558331, 330486.074748666 5914941.33795239, 330585.983154737 5915010.32749106, 330618.427962455 5915031.20447119, 330653.234601917 5914970.37328093, 330695.973322992 5914896.16305817)), ((330655.56641544314 5915024.3431278, 330712.39180534333 5915044.789646596, 330729.1207752672 5914998.320285697, 330678.1372478801 5914978.139306106, 330678.1372478801 5914978.139306106, 330655.56641544314 5915024.3431278)))";
		var shape = new Shape();
		shape.ImportFromWKT(wkt);
		shape.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		shape.NumParts.ShouldBe(2);

		var results = new object();
		shape.Explode(ref results).ShouldBeTrue();

		(results is Array).ShouldBeTrue();
		var array = (Array)results;
		array.Length.ShouldBe(2);
		var obj1 = array.GetValue(0);
		var obj2 = array.GetValue(1);
		(obj1 is Shape).ShouldBeTrue();
		(obj2 is Shape).ShouldBeTrue();
		var shape1 = (Shape)obj1;
		var shape2 = (Shape)obj2;
		shape1.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		shape2.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		Math.Round(shape1.Area, 5).ShouldBe(41521.54468);
		Math.Round(shape2.Area, 5).ShouldBe(2896.91199);
	}

	[Fact]
	public void Shapeput_XYTest()
	{
		var shape = _firstShapePoint.Clone();
		shape.put_XY(0, 12345, 4567).ShouldBeTrue();
		var pt = shape.Point[0];
		pt.ShouldNotBeNull();
		pt.x.ShouldBe(12345);
		pt.y.ShouldBe(4567);
	}

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

	[Fact]
	public void ShapeFixUpTest()
	{
		var shape = _firstShapePolygon.Clone();
		var partIndex = 0;
		shape.InsertPart(shape.NumPoints, ref partIndex).ShouldBeTrue();
		shape.IsValid.ShouldBeFalse();
		shape.FixUp(out var fixedUp);
		fixedUp.ShouldNotBeNull();
		fixedUp.IsValid.ShouldBeTrue();
		fixedUp.ShapeType.ShouldBe(ShpfileType.SHP_POLYGON);
		Math.Round(fixedUp.Area, 5).ShouldBe(41521.54468);
	}

	[Fact]
	public void ShapeAddPointTest()
	{
		var shape = _firstShapePolygon.Clone();
		var newPt = new Point { x = 330600, y = 5914800 };
		var idx = shape.AddPoint(newPt.x, newPt.y);
		shape.NumPoints.ShouldBe(25);
	}

	[Fact]
	public void ShapeExportToWktTest()
	{
		var wkt = _firstShapePolygon.ExportToWKT();
		var shape = new Shape();
		shape.ImportFromWKT(wkt);
		_firstShapePolygon.ShapeType.ShouldBe(shape.ShapeType);
		_firstShapePolygon.Area.ShouldBe(shape.Area);
	}

	[Fact]
	public void ShapeImportFromWktTest()
	{
		var shape = new Shape();
		var wkt = "LINESTRING (330452.29615390772232786 5914925.46296628564596176, 330753.68315174279268831 5914844.20796951279044151, 330753.68315174279268831 5914844.20796951279044151)";
		shape.ImportFromWKT(wkt);
		shape.ShapeType.ShouldBe(ShpfileType.SHP_POLYLINE);
		shape.IsValid.ShouldBeTrue();
	}

	[Fact]
	public void ShapeCopyFromTest()
	{
		var shape = new Shape();
		shape.CopyFrom(_firstShapePolygon).ShouldBeTrue();
		CompareShapes(shape, _firstShapePolygon).ShouldBeTrue();
		shape.Equals(_firstShapePolygon).ShouldBeTrue();
	}

	[Fact]
	public void ShapeClosestPointsTest()
	{
		var wkt = "POLYGON ((330655.56641544314334169 5915024.34312780015170574, 330712.39180534332990646 5915044.78964659571647644, 330729.12077526719076559 5914998.32028569653630257, 330678.13724788010586053 5914978.13930610567331314, 330678.13724788010586053 5914978.13930610567331314, 330655.56641544314334169 5915024.34312780015170574))";
		var sfMiniPolygon = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);
		var miniPolygon = sfMiniPolygon.Shape[0];

		var shape = _firstShapePolygon.ClosestPoints(miniPolygon);
		shape.ShouldNotBeNull();
		shape.ShapeType.ShouldBe(ShpfileType.SHP_POLYLINE);
		Math.Round(shape.Length, 5).ShouldBe(25.45549);
	}

	[Fact]
	public void ShapePut_MTest()
	{
		var shape = _firstShapePoint.Clone();
		shape.put_M(0, 1234).ShouldBeTrue();
		var pt = shape.Point[0];
		pt.ShouldNotBeNull();
		pt.M.ShouldBe(1234);
	}

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

public class MapCallback : ICallback
{
	private MapCallback() { }

	public static MapCallback Instance { get; } = new MapCallback();

	public void Progress(string keyOfSender, int percent, string message)
	{
		System.Diagnostics.Debug.WriteLine($"Progress({percent}% Msg: {message})");
	}

	public void Error(string keyOfSender, string errorMsg)
	{

		if(string.IsNullOrWhiteSpace(keyOfSender))
			System.Diagnostics.Debug.WriteLine($"Error: {errorMsg}");
		else
			System.Diagnostics.Debug.WriteLine($"Source: {keyOfSender} Error: {errorMsg}");
	}
}
