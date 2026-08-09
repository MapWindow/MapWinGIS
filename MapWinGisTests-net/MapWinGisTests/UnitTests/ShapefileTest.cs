using Xunit.Abstractions;
using SelectMode = MapWinGIS.SelectMode;

namespace MapWinGisTests.UnitTests
{
	[Collection(nameof(NotThreadSafeResourceCollection))]
	public class ShapefileTest : IDisposable
	{
		private readonly ITestOutputHelper _testOutputHelper;
		private readonly GlobalSettings _gs;
		private readonly Shapefile _shapefile;
		private readonly Shapefile _polyShapefile;

		public ShapefileTest(ITestOutputHelper testOutputHelper)
		{
			_testOutputHelper = testOutputHelper;
			_gs = new GlobalSettings();
			var shapefileName = Helpers.GetTestFilePath("Issue-216.shp");
			_shapefile = new Shapefile();
			_shapefile.Open(shapefileName);

			shapefileName = Helpers.GetTestFilePath("UnitedStates-3857.shp");
			_polyShapefile = new Shapefile();
			_polyShapefile.Open(shapefileName);
		}

		[Fact]
		public void ShapefileNumShapesTest()
		{
			_shapefile.NumShapes.ShouldBe(13424);
		}

		[Fact]
		public void ShapefileNumFieldsTest()
		{
			_shapefile.NumFields.ShouldBe(10);
		}

		[Fact]
		public void ShapefileExtentsTest()
		{
			_shapefile.Extents.ShouldNotBeNull();
			var ext = _shapefile.Extents;

			ext.xMin.ShouldBe(108.7220712);
			ext.xMax.ShouldBe(109.1398415);
			ext.yMin.ShouldBe(34.1490212);
			ext.yMax.ShouldBe(34.4578165);
		}

		[Fact]
		public void ShapefileShapefileTypeTest()
		{
			_shapefile.ShapefileType.ShouldBe(ShpfileType.SHP_POLYLINE);
		}

		[Fact]
		public void ShapefileShapeTest()
		{
			_shapefile.Shape[0].ShouldNotBeNull();
			var shape = _shapefile.Shape[0];
			shape.ShapeType.ShouldBe(ShpfileType.SHP_POLYLINE);
		}

		[Fact]
		public void ShapefileEditingShapesTest()
		{
			_shapefile.EditingShapes.ShouldBeFalse();
			var shapefile = _shapefile.Clone();
			shapefile.EditingShapes.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileLastErrorCodeTest()
		{
			_shapefile.LastErrorCode.ShouldBe(0);
		}

		[Fact]
		public void ShapefileCdlgFilterTest()
		{
			_shapefile.CdlgFilter.ShouldNotBeEmpty();
			_shapefile.CdlgFilter.ShouldEndWith(".shp");
		}

		[Fact(Skip = "Unit test is not yet implemented, Shape.GlobalCallback is deprecated")]
		public void ShapefileGlobalCallbackTest() { }

		[Fact]
		public void ShapefileFieldTest()
		{
			_shapefile.Field[0].ShouldNotBeNull();
			var field = _shapefile.Field[0];
			field.Type.ShouldBe(FieldType.STRING_FIELD);
			field.Alias.ShouldBeEmpty();
			field.Name.ShouldBe("osm_id");
			field.Precision.ShouldBe(0);
		}

		[Fact]
		public void ShapefileCellValueTest()
		{
			var oValue = _shapefile.CellValue[0, 1];
			oValue.ShouldNotBeNull();
			oValue.ToString().ShouldBe("26173997");
		}

		[Fact]
		public void ShapefileEditingTableTest()
		{
			_shapefile.EditingTable.ShouldBeFalse();
			var shapefile = _shapefile.Clone();
			shapefile.EditingTable.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileErrorMsgTest()
		{
			_shapefile.ErrorMsg[0].ShouldBe("No Error");
			_shapefile.ErrorMsg[1].ShouldBe("Index Out of Bounds");
			_shapefile.ErrorMsg[15].ShouldBe("Invalid File");
		}

		[Fact]
		public void ShapefileFileHandleTest()
		{
			_shapefile.FileHandle.ShouldNotBe(-1);
		}

		[Fact]
		public void ShapefileFilenameTest()
		{
			_shapefile.Filename.ShouldNotBeEmpty();
		}

		[Fact]
		public void ShapefileProjectionTest()
		{
			_shapefile.Projection.ShouldBe("+proj=longlat +datum=WGS84 +no_defs");
		}

		[Fact]
		public void ShapefileFieldByNameTest()
		{
			_shapefile.FieldByName["osm_id"].ShouldNotBeNull();
		}

		[Fact]
		public void ShapefileNumPointsTest()
		{
			_shapefile.NumPoints[0].ShouldBe(2);
		}

		[Fact]
		public void ShapefileUseSpatialIndexTest()
		{
			_shapefile.UseSpatialIndex.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileHasSpatialIndexTest()
		{
			_shapefile.HasSpatialIndex.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileCacheExtentsTest()
		{
			_shapefile.CacheExtents.ShouldBeFalse();
			var shapefile = _shapefile.Clone();
			shapefile.CacheExtents.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileUseQTreeTest()
		{
			_shapefile.UseQTree.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileSpatialIndexMaxAreaPercentTest()
		{
			_shapefile.SpatialIndexMaxAreaPercent.ShouldBe(0.5);
		}

		[Fact]
		public void ShapefileCanUseSpatialIndexTest()
		{
			var shape = _shapefile.Shape[_shapefile.NumShapes / 2];
			_shapefile.CanUseSpatialIndex[shape.Extents].ShouldBeTrue();
		}

		[Fact]
		public void ShapefileNumSelectedTest()
		{
			_shapefile.NumSelected.ShouldBe(0);
			_shapefile.ShapeSelected[0] = true;
			_shapefile.ShapeSelected[2] = true;
			_shapefile.NumSelected.ShouldBe(2);
			_shapefile.SelectNone();
		}

		[Fact]
		public void ShapefileShapeSelectedTest()
		{
			_shapefile.NumSelected.ShouldBe(0);
			_shapefile.ShapeSelected[0].ShouldBeFalse();
			_shapefile.ShapeSelected[0] = true;
			_shapefile.ShapeSelected[0].ShouldBeTrue();
			_shapefile.ShapeSelected[0] = false;
			_shapefile.ShapeSelected[0].ShouldBeFalse();
		}

		[Fact]
		public void ShapefileSelectionDrawingOptionsTest()
		{
			_shapefile.SelectionDrawingOptions.ShouldNotBeNull();
			_shapefile.SelectionDrawingOptions.LineVisible.ShouldBeTrue();
			_shapefile.SelectionDrawingOptions.FillColor.ShouldBe(255u);
		}

		[Fact]
		public void ShapefileKeyTest()
		{
			var shapefile = _shapefile.Clone();
			shapefile.Key.ShouldBe("");
			shapefile.Key = "1234";
			shapefile.Key.ShouldBe("1234");
		}

		[Fact]
		public void ShapefileLabelsTest()
		{
			_shapefile.Labels.ShouldNotBeNull();
			_shapefile.Labels.Count.ShouldBe(0);
		}

		[Fact]
		public void ShapefileDefaultDrawingOptionsTest()
		{
			_shapefile.DefaultDrawingOptions.ShouldNotBeNull();
			_shapefile.DefaultDrawingOptions.LineVisible.ShouldBeTrue();
			_shapefile.DefaultDrawingOptions.FillColor.ShouldBe(16777215u);
		}

		[Fact]
		public void ShapefileCategoriesTest()
		{
			_shapefile.Categories.ShouldNotBeNull();
			_shapefile.Categories.Count.ShouldBe(0);
			_shapefile.Categories.Key.ShouldBeEmpty();
		}

		[Fact]
		public void ShapefileChartsTest()
		{
			_shapefile.Charts.ShouldNotBeNull();
			_shapefile.Charts.Count.ShouldBe(0);
		}

		[Fact]
		public void ShapefileShapeCategoryTest()
		{
			_shapefile.ShapeCategory[0].ShouldBe(-1);
		}

		[Fact]
		public void ShapefileTableTest()
		{
			_shapefile.Table.ShouldNotBeNull();
			_shapefile.Table.NumRows.ShouldBe(_shapefile.NumShapes);
		}

		[Fact]
		public void ShapefileVisibilityExpressionTest()
		{
			_shapefile.VisibilityExpression.ShouldBeNullOrWhiteSpace();
			_shapefile.VisibilityExpression = "[osm_id] <> \"\"";
			_shapefile.LastErrorCode.ShouldBe(0);
			_shapefile.VisibilityExpression = null;
		}

		[Fact]
		public void ShapefileFastModeTest()
		{
			_shapefile.FastMode.ShouldBeFalse();
			_shapefile.FastMode = true;
			if(_shapefile.EditingShapes)
				_shapefile.FastMode.ShouldBeTrue();
			else
				_shapefile.FastMode.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileMinDrawingSizeTest()
		{
			_shapefile.MinDrawingSize.ShouldBe(1);
		}

		[Fact]
		public void ShapefileSourceTypeTest()
		{
			_shapefile.SourceType.ShouldBe(tkShapefileSourceType.sstDiskBased);
		}

		[Fact]
		public void ShapefileGeometryEngineTest()
		{
			_shapefile.GeometryEngine.ShouldBe(tkGeometryEngine.engineGeos);
		}

		[Fact]
		public void ShapefileSelectionColorTest()
		{
			_shapefile.SelectionColor.ShouldBe(65535u);
		}

		[Fact]
		public void ShapefileSelectionAppearanceTest()
		{
			_shapefile.SelectionAppearance.ShouldBe(tkSelectionAppearance.saSelectionColor);
		}

		[Fact]
		public void ShapefileCollisionModeTest()
		{
			_shapefile.CollisionMode.ShouldBe(tkCollisionMode.LocalList);
		}

		[Fact]
		public void ShapefileSelectionTransparencyTest()
		{
			_shapefile.SelectionTransparency.ShouldBe((byte)180);
		}

		[Fact]
		public void ShapefileStopExecutionTest()
		{
			_shapefile.StopExecution = null;
			_shapefile.LastErrorCode.ShouldBe(0);
			_shapefile.StopExecution = new StopExecution();
			_shapefile.LastErrorCode.ShouldBe(0);
		}

		[Fact]
		public void ShapefileGeoProjectionTest()
		{
			_shapefile.GeoProjection.ShouldNotBeNull();
			_shapefile.GeoProjection.Name.ShouldBe("WGS 84");
		}

		[Fact]
		public void ShapefileIdentifiableTest()
		{
			_shapefile.Identifiable.ShouldBeTrue();
			_shapefile.Identifiable = false;
			_shapefile.Identifiable.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileShapeCategory2Test()
		{
			_shapefile.ShapeCategory2[0].ShouldBeEmpty();
			_shapefile.ShapeCategory2[10].ShouldBeEmpty();
		}

		[Fact]
		public void ShapefileShapeCategory3Test()
		{
			_shapefile.ShapeCategory3[0].ShouldBeNull();
			_shapefile.ShapeCategory3[10].ShouldBeNull();
		}

		[Fact]
		public void ShapefileLastInputValidationTest()
		{
			_shapefile.LastInputValidation.ShouldBeNull();
			//_gs.ShapeOutputValidationMode = tkShapeValidationMode.TryFixProceedOnFailure;
			/// TOOD: Implement a test that triggers input validation and checks LastInputValidation
		}

		[Fact]
		public void ShapefileLastOutputValidationTest()
		{
			_shapefile.LastOutputValidation.ShouldBeNull();
			/// TOOD: Implement a test that triggers output validation and checks LastOutputValidation
		}

		[Fact]
		public void ShapefileShapeVisibleTest()
		{
			_shapefile.ShapeVisible[0].ShouldBeFalse();
			_shapefile.ShapeVisible[10].ShouldBeFalse();
		}

		[Fact]
		public void ShapefileShapeRotationTest()
		{
			_shapefile.ShapeRotation[0].ShouldBe(0.0);
			_shapefile.ShapeRotation[10].ShouldBe(0.0);
		}

		[Fact]
		public void ShapefileVolatileTest()
		{
			_shapefile.Volatile.ShouldBeFalse();
			_shapefile.Volatile = true;
			_shapefile.Volatile.ShouldBeTrue();
			_shapefile.Volatile = false;
		}

		[Fact]
		public void ShapefileShapeModifiedTest()
		{
			_shapefile.ShapeModified[0].ShouldBeFalse();
			_shapefile.ShapeModified[10].ShouldBeFalse();
		}

		[Fact]
		public void ShapefileInteractiveEditingTest()
		{
			_shapefile.InteractiveEditing.ShouldBeFalse();
			_shapefile.InteractiveEditing = true;
			_shapefile.InteractiveEditing.ShouldBeTrue();
			_shapefile.InteractiveEditing = false;
			_shapefile.InteractiveEditing.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileShapeIsHiddenTest()
		{
			_shapefile.ShapeIsHidden[0].ShouldBeFalse();
			_shapefile.ShapeIsHidden[10].ShouldBeFalse();
		}

		[Fact]
		public void ShapefileSnappableTest()
		{
			_shapefile.Snappable.ShouldBeTrue();
			_shapefile.Snappable = false;
			_shapefile.Snappable.ShouldBeFalse();
			_shapefile.Snappable = true;
		}

		[Fact]
		public void ShapefileShapefileType2DTest()
		{
			_shapefile.ShapefileType2D.ShouldBe(ShpfileType.SHP_POLYLINE);
		}

		[Fact]
		public void ShapefileFieldIndexByNameTest()
		{
			for(var idx = 0;idx < _shapefile.NumFields; idx++)
			{
				var field = _shapefile.Field[idx];
				_shapefile.FieldIndexByName[field.Name].ShouldBe(idx);
			}
		}

		[Fact]
		public void ShapefileShapeRenderedTest()
		{
			_shapefile.ShapeRendered[0].ShouldBeFalse();
			_shapefile.ShapeRendered[10].ShouldBeFalse();
		}

		[Fact]
		public void ShapefileSortFieldTest()
		{
			_shapefile.SortField.ShouldBeEmpty();
			_shapefile.LastErrorCode.ShouldBe(0);
			_shapefile.SortField = "osm_id";
			_shapefile.LastErrorCode.ShouldBe(0);
			_shapefile.SortField = "";
		}

		[Fact]
		public void ShapefileSortAscendingTest()
		{
			_shapefile.SortAscending.ShouldBeFalse();
			_shapefile.SortAscending = true;
			_shapefile.SortAscending.ShouldBeTrue();
			_shapefile.SortAscending = false;
		}

		[Fact]
		public void ShapefileAppendModeTest()
		{
			_shapefile.AppendMode.ShouldBeFalse();
			_shapefile.StartAppendMode();
			_shapefile.AppendMode.ShouldBeTrue();
			_shapefile.StopAppendMode();
		}

		[Fact]
		public void ShapefileIsGeographicProjectionTest()
		{
			_shapefile.GeoProjection.IsGeographic.ShouldBe(_shapefile.IsGeographicProjection);
		}

		[Fact]
		public void ShapefileSelectableTest()
		{
			_shapefile.Selectable.ShouldBeFalse();
			_shapefile.Selectable = true;
			_shapefile.Selectable.ShouldBeTrue();
			_shapefile.Selectable = false;
			_shapefile.Selectable.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileHasOgrFidMappingTest()
		{
			_shapefile.HasOgrFidMapping.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileOpenTest()
		{
			var shapefile = new Shapefile();
			var fileName = Helpers.GetTestFilePath("Issue-216.shp");
			shapefile.Open(fileName).ShouldBeTrue();
			shapefile.Close();
		}

		[Fact]
		public void ShapefileCreateNewTest()
		{
			var shapefile = new Shapefile();
			shapefile.CreateNew("", ShpfileType.SHP_POINT).ShouldBeTrue();
		}

		[Fact]
		public void ShapefileSaveAsTest()
		{
			var fileName = Helpers.GetRandomFilePath("_SaveTestFile", ".shp");
			_shapefile.SaveAs(fileName).ShouldBeTrue();
			//Helpers.DeleteShapefileFiles(fileName);
		}

		[Fact]
		public void ShapefileCloseTest()
		{
			var fileName = _shapefile.Filename;
			_shapefile.Close().ShouldBeTrue();
			_shapefile.Open(fileName).ShouldBeTrue();
		}

		[Fact]
		public void ShapefileEditClearTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 20);
			shapefile.NumShapes.ShouldBeGreaterThan(10);
			shapefile.NumFields.ShouldBeGreaterThan(5);
			var numFields = shapefile.NumFields;
			shapefile.StartEditingShapes();
			shapefile.StartEditingTable();
			shapefile.EditClear().ShouldBeTrue();
			shapefile.NumShapes.ShouldBe(0);
			shapefile.NumFields.ShouldBe(numFields);
		}

		[Fact]
		public void ShapefileEditInsertShapeTest()
		{
			var shapefile = _shapefile.Clone();
			shapefile.StartEditingShapes();
			var shape = _shapefile.Shape[0];
			shape.ShouldNotBeNull();
			var shpIndex = 0;
			shapefile.NumShapes.ShouldBe(0);
			shapefile.Table.NumRows.ShouldBe(0);
			shapefile.EditInsertShape(shape, ref shpIndex).ShouldBeTrue();
			shapefile.NumShapes.ShouldBe(1);
			shapefile.Table.NumRows.ShouldBe(1);
		}

		[Fact]
		public void ShapefileEditDeleteShapeTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			shapefile.NumShapes.ShouldBe(10);
			shapefile.Table.NumRows.ShouldBe(10);
			shapefile.EditDeleteShape(5).ShouldBeTrue();
			shapefile.NumShapes.ShouldBe(9);
			shapefile.Table.NumRows.ShouldBe(9);
		}

		[Fact]
		public void ShapefileSelectShapesTest()
		{
			var shape = _shapefile.Shape[10];
			var result = new object();
			_shapefile.SelectShapes(shape.Extents, 0D, SelectMode.INTERSECTION, ref result).ShouldBeTrue();
			(result is Array).ShouldBeTrue();
			var array = (Array)result;
			array.Length.ShouldBeGreaterThan(10);
		}

		[Fact]
		public void ShapefileStartEditingShapesTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_StartEditingShapes_TestFile", ".shp");
			shapefile.SaveAs(fileName).ShouldBeTrue();
			shapefile.Close().ShouldBeTrue();
			shapefile.Open(fileName).ShouldBeTrue();
			shapefile.EditingShapes.ShouldBeFalse();
			shapefile.EditingTable.ShouldBeFalse();
			shapefile.StartEditingShapes().ShouldBeTrue();
			shapefile.EditingShapes.ShouldBeTrue();
			shapefile.EditingTable.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileStopEditingShapesTest()
		{
			_shapefile.StartEditingShapes().ShouldBeTrue();
			_shapefile.EditingShapes.ShouldBeTrue();
			_shapefile.EditingTable.ShouldBeTrue();
			_shapefile.StopEditingShapes(false).ShouldBeTrue();
			_shapefile.EditingShapes.ShouldBeFalse();
			_shapefile.EditingTable.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileEditInsertFieldTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var newField = new Field();
			newField.Name = "Text01";
			newField.Type = FieldType.STRING_FIELD;
			newField.Width = 10;
			var fieldIndex = 0;
			shapefile.EditInsertField(newField, ref fieldIndex).ShouldBeTrue();
		}

		[Fact]
		public void ShapefileEditDeleteFieldTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			shapefile.NumFields.ShouldBe(10);
			shapefile.EditDeleteField(4).ShouldBeTrue();
			shapefile.NumFields.ShouldBe(9);
		}

		[Fact]
		public void ShapefileEditCellValueTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var value = shapefile.CellValue[2, 0];
			var newValue = $"{value}_0";
			shapefile.EditCellValue(2, 0, $"{value}_0").ShouldBeTrue();
			value = shapefile.CellValue[2, 0];
			value.ShouldBe(newValue);
		}

		[Fact]
		public void ShapefileStartEditingTableTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_StartEditingTable_TestFile", ".shp");
			shapefile.SaveAs(fileName).ShouldBeTrue();
			shapefile.Close().ShouldBeTrue();
			shapefile.Open(fileName).ShouldBeTrue();
			shapefile.EditingTable.ShouldBeFalse();
			shapefile.StartEditingTable().ShouldBeTrue();
			shapefile.EditingTable.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileStopEditingTableTest()
		{
			_shapefile.StartEditingTable().ShouldBeTrue();
			_shapefile.EditingShapes.ShouldBeFalse();
			_shapefile.EditingTable.ShouldBeTrue();
			_shapefile.StopEditingTable(false).ShouldBeTrue();
			_shapefile.EditingShapes.ShouldBeFalse();
			_shapefile.EditingTable.ShouldBeFalse();
		}

		[Fact]
		public void ShapefileQuickPointTest()
		{
			var pt = _shapefile.QuickPoint(10, 2);
			pt.ShouldNotBeNull();
			_shapefile.QuickPoint(_shapefile.NumShapes+1, 0).ShouldBeNull();
		}

		[Fact]
		public void ShapefileQuickExtentsTest()
		{
			var quickExtent = _shapefile.QuickExtents(10);
			quickExtent.ShouldNotBeNull();

			var shape = _shapefile.Shape[10];
			shape.ShouldNotBeNull();
			var ext = shape.Extents;
			ext.ShouldNotBeNull();
			quickExtent.xMin.ShouldBe(ext.xMin);
			quickExtent.xMax.ShouldBe(ext.xMax);
			quickExtent.yMin.ShouldBe(ext.yMin);
			quickExtent.yMax.ShouldBe(ext.yMax);
		}

		[Fact]
		public void ShapefileQuickPointsTest()
		{
			var arrPoints = _shapefile.QuickPoints(10, 5);
			arrPoints.ShouldNotBeNull();

			var shape = _shapefile.Shape[10];
			shape.ShouldNotBeNull();

			var n = 0;
			for(var i = 0;i < shape.NumPoints; i++)
			{
				var pt = shape.Point[i];
				pt.x.ShouldBe(arrPoints.GetValue(n++));
				pt.y.ShouldBe(arrPoints.GetValue(n++));
			}
		}

		[Fact]
		public void ShapefilePointInShapeTest()
		{
			_polyShapefile.PointInShape(39, -11010847.02158054895699024, 3687674.63619555253535509).ShouldBeTrue();
			_polyShapefile.PointInShape(40, -11010847.02158054895699024, 3687674.63619555253535509).ShouldBeFalse();
		}

		[Fact]
		public void ShapefilePointInShapefileTest()
		{
			/// TODO: Fix MapWinGIS to not require BeginPointInShapefile() for PointInShapefile() to work.
			_polyShapefile.BeginPointInShapefile().ShouldBeTrue();
			_polyShapefile.PointInShapefile(-11010847.02158054895699024, 3687674.63619555253535509).ShouldBe(39);
			_polyShapefile.PointInShapefile(-11326538.29784329049289227, 3332521.95039996830746531).ShouldBe(-1);
			_polyShapefile.EndPointInShapefile();
		}

		[Fact]
		public void ShapefileBeginPointInShapefileTest()
		{
			_polyShapefile.BeginPointInShapefile().ShouldBeTrue();
			_polyShapefile.EndPointInShapefile();
		}

		[Fact]
		public void ShapefileEndPointInShapefileTest()
		{
			_polyShapefile.BeginPointInShapefile().ShouldBeTrue();
			_polyShapefile.EndPointInShapefile();
		}

		[Fact]
		public void ShapefileCreateNewWithShapeIDTest()
		{
			var shapefile = new Shapefile();
			shapefile.CreateNewWithShapeID("", ShpfileType.SHP_POINT);
			shapefile.NumFields.ShouldBe(1);
			var field = shapefile.Field[0];
			field.ShouldNotBeNull();
			field.Name.ShouldBe("MWShapeID");
			field.Key.ShouldBe("");
			field.Alias.ShouldBe("");
			field.Type.ShouldBe(FieldType.INTEGER_FIELD);
		}

		[Fact]
		public void ShapefileCreateSpatialIndexTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_CreateSpatialIndex_TestFile", ".shp");
			shapefile.SaveAs(fileName).ShouldBeTrue();
			shapefile.CreateSpatialIndex().ShouldBeTrue();
		}

		[Fact]
		public void ShapefileResourceTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_Resource_TestFile", ".shp");
			shapefile.SaveAs(fileName).ShouldBeTrue();

			shapefile = Helpers.DeepClone(_polyShapefile);
			shapefile.NumShapes.ShouldBe(49);
			shapefile.ShapefileType.ShouldBe(ShpfileType.SHP_POLYGON);
			shapefile.Resource(fileName).ShouldBeTrue();
			shapefile.NumShapes.ShouldBe(10);
			shapefile.ShapefileType.ShouldBe(ShpfileType.SHP_POLYLINE);
		}

		[Fact]
		public void ShapefileRefreshExtentsTest()
		{
			_shapefile.RefreshExtents().ShouldBeTrue();
		}

		[Fact]
		public void ShapefileRefreshShapeExtentsTest()
		{
			_shapefile.RefreshShapeExtents(10).ShouldBeTrue();
		}

		[Fact]
		public void ShapefileSaveTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_Resource_TestFile", ".shp");
			shapefile.SaveAs(fileName).ShouldBeTrue();

			//var value = shapefile.CellValue[0, 1];
			shapefile.EditCellValue(0, 1, "10000");

			var shape = shapefile.Shape[5];
			var pt = shape.Point[0];
			var currX = pt.x;
			var currY = pt.y;
			shape.Point[0] = new Point { x = currX + 1, y = currY + 1 };

			shapefile.Save().ShouldBeTrue();
			shapefile.Close().ShouldBeTrue();

			shapefile.Open(fileName);
			var value = shapefile.CellValue[0, 1];
			value.ShouldBe("10000");
			shape = shapefile.Shape[5];
			pt = shape.Point[0];
			pt.x.ShouldBe(currX  + 1);
			pt.y.ShouldBe(currY + 1);
		}

		[Fact]
		public void ShapefileIsSpatialIndexValidTest()
		{
			_shapefile.IsSpatialIndexValid().ShouldBeTrue();
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			shapefile.IsSpatialIndexValid().ShouldBeFalse();
		}

		[Fact]
		public void ShapefileGetIntersectionTest()
		{
			// Miss test
			var wkt = "POLYGON ((-9710414.20743004977703094 5390344.77251605130732059, -9645840.99183085188269615 5329358.9577834764495492, -9655706.3442140631377697 5276444.79500080179423094, -9705033.10613011568784714 5269269.99326755758374929, -9725660.66111319325864315 5307834.55258374474942684, -9732835.46284643746912479 5349986.51276655308902264, -9732835.46284643746912479 5349986.51276655308902264, -9710414.20743004977703094 5390344.77251605130732059))";
			var sfPolygon = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);

			var result = _polyShapefile.GetIntersection(false, sfPolygon, false, ShpfileType.SHP_NULLSHAPE);
			result.ShouldBeNull();

			// Hit test
			wkt = "POLYGON ((-10984838.36529753357172012 3877358.45701819472014904, -10935511.60338148102164268 3819960.04315224196761847, -10970488.76183104515075684 3746418.32538648974150419, -11042236.77916348725557327 3745521.47516983421519399, -11070039.13587980717420578 3808300.99033572059124708, -11029680.87613030895590782 3872874.2059349175542593, -11029680.87613030895590782 3872874.2059349175542593, -10984838.36529753357172012 3877358.45701819472014904))";
			sfPolygon = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);
			result = _polyShapefile.GetIntersection(false, sfPolygon, false, ShpfileType.SHP_NULLSHAPE);
			result.ShouldNotBeNull();

			var testShape = sfPolygon.Shape[0];
			testShape.ShouldNotBeNull();

			var resultShape = result.Shape[0];
			resultShape.ShouldNotBeNull();
			Math.Round(testShape.Area - resultShape.Area, 3).ShouldBe(0.000);
		}

		[Fact]
		public void ShapefileSelectByShapefileTest()
		{
			// Miss test
			var wkt = "POLYGON ((-9710414.20743004977703094 5390344.77251605130732059, -9645840.99183085188269615 5329358.9577834764495492, -9655706.3442140631377697 5276444.79500080179423094, -9705033.10613011568784714 5269269.99326755758374929, -9725660.66111319325864315 5307834.55258374474942684, -9732835.46284643746912479 5349986.51276655308902264, -9732835.46284643746912479 5349986.51276655308902264, -9710414.20743004977703094 5390344.77251605130732059))";
			var sfPolygon = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);
			var result = new object();
			_polyShapefile.SelectByShapefile(sfPolygon, tkSpatialRelation.srOverlaps, false, ref result).ShouldBeFalse();
			(result is Array).ShouldBeTrue();
			((Array)result).Length.ShouldBe(0);

			// Hit test
			wkt = "LINESTRING (-10176776.32009091414511204 3580501.03530521970242262, -10987528.91594749875366688 3763458.4795029447413981, -11737295.6970715094357729 4244170.19563029985874891, -11737295.6970715094357729 4244170.19563029985874891)";
			var sfPolyline = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
			_polyShapefile.SelectByShapefile(sfPolyline, tkSpatialRelation.srCrosses, false, ref result).ShouldBeTrue();
			(result is Array).ShouldBeTrue();
			((Array)result).Length.ShouldBe(3);
		}

		[Fact]
		public void ShapefileSelectAllTest()
		{
			_shapefile.NumSelected.ShouldBe(0);
			_shapefile.SelectAll();
			_shapefile.NumSelected.ShouldBe(_shapefile.NumShapes);
			_shapefile.SelectNone();
		}

		[Fact]
		public void ShapefileSelectNoneTest()
		{
			_shapefile.SelectAll();
			_shapefile.NumSelected.ShouldBe(_shapefile.NumShapes);
			_shapefile.SelectNone();
			_shapefile.NumSelected.ShouldBe(0);
		}

		[Fact]
		public void ShapefileInvertSelectionTest()
		{
			_shapefile.NumSelected.ShouldBe(0);
			_shapefile.InvertSelection();
			_shapefile.NumSelected.ShouldBe(_shapefile.NumShapes);
			_shapefile.SelectNone();
		}

		[Fact]
		public void ShapefileDissolveTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 10);
			var name1 = shapefile.CellValue[1, 1];
			var name2 = shapefile.CellValue[1, 3];

			var newName = $"[{name1}-{name2}]";
			shapefile.EditCellValue(1, 1, newName).ShouldBeTrue();
			shapefile.EditCellValue(1, 3, newName).ShouldBeTrue();

			var dissolvedShapefile = shapefile.Dissolve(1, false);
			dissolvedShapefile.ShouldNotBeNull();
			dissolvedShapefile.NumShapes.ShouldBe(9);
		}

		[Fact]
		public void ShapefileGenerateLabelsTest()
		{
			_polyShapefile.Labels.Count.ShouldBe(0);
			var result = _polyShapefile.GenerateLabels(1, tkLabelPositioning.lpCentroid);
			result.ShouldBe(_polyShapefile.NumShapes);
			_polyShapefile.Labels.Count.ShouldBe(result);
			_polyShapefile.Labels.Clear();
		}

		[Fact]
		public void ShapefileCloneTest()
		{
			var shapefile = _shapefile.Clone();
			shapefile.NumShapes.ShouldBe(0);
			shapefile.NumFields.ShouldBe(_shapefile.NumFields);
		}

		[Fact]
		public void ShapefileBufferByDistanceTest()
		{
			var wkt = "LINESTRING (-10176776.32009091414511204 3580501.03530521970242262, -10987528.91594749875366688 3763458.4795029447413981, -11737295.6970715094357729 4244170.19563029985874891, -11737295.6970715094357729 4244170.19563029985874891)";
			var sfPolyline = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
			var sfResult = sfPolyline.BufferByDistance(10, 30, false, true);
			sfResult.ShouldNotBeNull();
			sfResult.ShapefileType.ShouldBe(ShpfileType.SHP_POLYGON);
			sfResult.NumShapes.ShouldBe(1);
		}

		[Fact]
		public void ShapefileDifferenceTest()
		{
			var shapefile1 = Helpers.DeepClone(_polyShapefile, 10);
			var shapefile2 = Helpers.DeepClone(_polyShapefile, 5);
			var sfResult = shapefile1.Difference(false, shapefile2, false);
			sfResult.ShouldNotBeNull();
		}

		[Fact]
		public void ShapefileClipTest()
		{
			var wkt = "POLYGON ((-10984838.36529753357172012 3877358.45701819472014904, -10935511.60338148102164268 3819960.04315224196761847, -10970488.76183104515075684 3746418.32538648974150419, -11042236.77916348725557327 3745521.47516983421519399, -11070039.13587980717420578 3808300.99033572059124708, -11029680.87613030895590782 3872874.2059349175542593, -11029680.87613030895590782 3872874.2059349175542593, -10984838.36529753357172012 3877358.45701819472014904))";
			var sfOverlay = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);
			var sfResult = _polyShapefile.Clip(false, sfOverlay, false);
			sfResult.ShouldNotBeNull();


			var testShape = sfOverlay.Shape[0];
			testShape.ShouldNotBeNull();

			var resultShape = sfResult.Shape[0];
			resultShape.ShouldNotBeNull();
			Math.Round(testShape.Area - resultShape.Area, 3).ShouldBe(0.000);
		}

		[Fact]
		public void ShapefileSymmDifferenceTest()
		{
			var wkt = "POLYGON ((-13413508.75200067460536957 6135178.87744845822453499, -13295124.5234021469950676 6144147.37961501348763704, -13257456.81430261395871639 6067018.26098263915628195, -13318442.62903518974781036 6009619.84711668640375137, -13379428.44376776367425919 6018588.34928324166685343, -13422477.25416722893714905 6083161.56488243862986565, -13422477.25416722893714905 6083161.56488243862986565, -13413508.75200067460536957 6135178.87744845822453499))";
			var sfOverlay = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);

			var shapefile = Helpers.DeepClone(_polyShapefile, 1);
			var sfResult = shapefile.SymmDifference(false, sfOverlay, false);
			sfResult.ShouldNotBeNull();

			var orgShape = sfOverlay.Shape[0];
			orgShape.ShouldNotBeNull();
			var resultShape = sfResult.Shape[0];
			resultShape.ShouldNotBeNull();
			resultShape.NumParts.ShouldBe(4);
			var part1 = resultShape.PartAsShape[0];
			var part2 = resultShape.PartAsShape[1];
			//var part3 = resultShape.PartAsShape[2];
			//var part4 = resultShape.PartAsShape[3];
			part1.ShouldNotBeNull();
			part2.ShouldNotBeNull();
			Math.Round(orgShape.Area - part2.Area, 3).ShouldBe(0.000);
		}

		[Fact]
		public void ShapefileUnionTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 1);

			var wkt = "POLYGON ((-13146247.38743733428418636 6402440.24201180040836334, -13151628.48873726651072502 6153115.88178156688809395, -13504987.47409954108297825 6163878.08438143320381641, -13497812.6723662968724966 6395265.4402785561978817, -13497812.6723662968724966 6395265.4402785561978817, -13146247.38743733428418636 6402440.24201180040836334))";
			var sfOverlay = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);

			var sfResult = shapefile.Union(false, sfOverlay, false);
			sfResult.ShouldNotBeNull();
			sfResult.NumShapes.ShouldBe(3);
			var resultShape = sfResult.Shape[0];
			Math.Round(resultShape.Area, 3).ShouldBe(41086641019.383);
		}

		[Fact]
		public void ShapefileExplodeShapesTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 1);
			shapefile.NumShapes.ShouldBe(1);
			var sfResult = shapefile.ExplodeShapes(true);
			sfResult.ShouldBeNull();
			sfResult = shapefile.ExplodeShapes(false);
			sfResult.ShouldNotBeNull();
			sfResult.NumShapes.ShouldBe(3);
		}


		[Fact]
		public void ShapefileAggregateShapesTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 10);
			var name1 = shapefile.CellValue[1, 1];
			var name2 = shapefile.CellValue[1, 3];

			var newName = $"[{name1}-{name2}]";
			shapefile.EditCellValue(1, 1, newName).ShouldBeTrue();
			shapefile.EditCellValue(1, 3, newName).ShouldBeTrue();

			var sfResult = shapefile.AggregateShapes(false, 1);
			sfResult.NumShapes.ShouldBe(9);
		}

		[Fact]
		public void ShapefileExportSelectionTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 10);

			var sfResult = shapefile.ExportSelection();
			sfResult.ShouldBeNull();

			shapefile.SelectAll();
			sfResult = shapefile.ExportSelection();
			sfResult.ShouldNotBeNull();
			sfResult.NumShapes.ShouldBe(10);
		}

		[Fact]
		public void ShapefileSortTest()
		{
			var value = _polyShapefile.CellValue[1, 0];
			value.ShouldBe("Washington");

			var shapefile = _polyShapefile.Sort(1, true);
			value =  shapefile.CellValue[1, 0];
			value.ShouldBe("Alabama");
		}

		[Fact]
		public void ShapefileMergeTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 1);

			var wkt = "POLYGON ((-13623371.70269806869328022 6370153.63421220146119595, -13522924.47843265160918236 6151322.18134825583547354, -13542655.1831990722566843 5893029.31895146705210209, -13757899.23519639670848846 5841012.00638544745743275, -13896014.16856134496629238 5830249.80378558114171028, -13955206.28286060877144337 6239213.5025804964825511, -13955206.28286060877144337 6239213.5025804964825511, -13623371.70269806869328022 6370153.63421220146119595))";
			var sfOverlay = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYGON, wkt, 28992);

			var sfResult = shapefile.Merge(false, sfOverlay, false);
			sfResult.ShouldNotBeNull();
		}

		[Fact]
		public void ShapefileSerializeTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 1);
			var serializedText = shapefile.Serialize(true);
			serializedText.ShouldNotBeNull();
			serializedText.ShouldStartWith("<ShapefileClass");
			serializedText.ShouldContain("ShpType=\"3\"");
			serializedText.ShouldContain("</ShapefileClass>");
		}

		[Fact]
		public void ShapefileDeserializeTest()
		{
			var text = "<ShapefileClass ShpType=\"3\">\r\n  <DefaultDrawingOptions FillColor=\"16777215\" LineColor=\"8949349\" RotationExpression=\"\" />\r\n  <ShapefileCategoriesClass ClassificationField=\"-1\" />\r\n  <LabelsClass Generated=\"0\" TextRenderingHint=\"3\" />\r\n  <ChartsClass />\r\n  <TableClass>\r\n    <Fields />\r\n  </TableClass>\r\n</ShapefileClass>";
			var shapefile = new Shapefile();
			shapefile.CreateNew("", ShpfileType.SHP_NULLSHAPE);
			shapefile.ShapefileType.ShouldBe(ShpfileType.SHP_NULLSHAPE);
			shapefile.Deserialize(false, text);
			shapefile.ShapefileType.ShouldBe(ShpfileType.SHP_POLYLINE);
		}

		[Fact]
		public void ShapefileReprojectTest()
		{
			var geoProj = _shapefile.GeoProjection;
			var newProjection = Helpers.MakeProjection(28992);
			int reprojectedCount = 0;
			var shapefile = _shapefile.Reproject(newProjection, ref reprojectedCount);
			shapefile.ShouldNotBeNull();
			shapefile.NumShapes.ShouldBe(13424);
			var ext = shapefile.Extents;
			Math.Round(ext.xMin - 7861961.7160907984, 5).ShouldBe(0);
			Math.Round(ext.xMax - 7912482.6353602549, 5).ShouldBe(0);
			Math.Round(ext.yMin - 5241600.7226124881, 5).ShouldBe(0);
			Math.Round(ext.yMax - 5301001.6926183095, 5).ShouldBe(0);
		}

		[Fact]
		public void ShapefileReprojectInPlaceTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 100);
			var newProjection = Helpers.MakeProjection(28992);
			int reprojectedCount = 0;
			shapefile.ReprojectInPlace(newProjection, ref reprojectedCount).ShouldBeTrue();
			shapefile.NumShapes.ShouldBe(100);
			var ext = shapefile.Extents;
			Math.Round(ext.xMin - 7873923.7115875864, 5).ShouldBe(0);
			Math.Round(ext.xMax - 7906269.68945167, 5).ShouldBe(0);
			Math.Round(ext.yMin - 5253933.4381718114, 5).ShouldBe(0);
			Math.Round(ext.yMax - 5294477.8762830561, 5).ShouldBe(0);
		}

		[Fact]
		public void ShapefileSimplifyLinesTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var shape1 = shapefile.Shape[1];
			//var wkt1 = shape1.ExportToWKT();
			shape1.NumPoints.ShouldBe(11);

			var simplShapefile = shapefile.SimplifyLines(0.001, false);
			var shape2 = simplShapefile.Shape[1];
			//var wkt2 = shape2.ExportToWKT();
			shape2.NumPoints.ShouldBe(2);
			Math.Round(shape1.Length - shape2.Length, 3).ShouldBe(0.000);
		}

		[Fact]
		public void ShapefileFixUpShapesTest()
		{
			var sfPolygon = Helpers.CreateTestPolygonShapefile();
			var shape = sfPolygon.Shape[0].Clone();
			shape.ReversePointsOrder(0).ShouldBeTrue();
			var shapefile = new Shapefile();
			shapefile.CreateNew("", ShpfileType.SHP_POLYGON);
			shapefile.HasInvalidShapes().ShouldBeFalse();
			shapefile.EditAddShape(shape);
			shapefile.HasInvalidShapes().ShouldBeTrue();
			shapefile.FixUpShapes(out var resultShapefile).ShouldBeTrue();
			resultShapefile.ShouldNotBeNull();
			resultShapefile.HasInvalidShapes().ShouldBeFalse();
		}

		[Fact]
		public void ShapefileEditAddShapeTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var shape = shapefile.Shape[1].Clone();
			shape.Move(shape.Length / 10, shape.Length / 10);
			var ret = shapefile.EditAddShape(shape);
			ret.ShouldBe(10);
			shapefile.NumShapes.ShouldBe(11);
		}

		[Fact]
		public void ShapefileEditAddFieldTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var idx = shapefile.EditAddField("NewName", FieldType.STRING_FIELD, 0, 10);
			idx.ShouldBe(10);
			shapefile.NumFields.ShouldBe(11);
			shapefile.Field[idx].Name.ShouldBe("NewName");
		}

		[Fact]
		public void ShapefileGetRelatedShapesTest()
		{
			var resultArray = new object();
			_shapefile.GetRelatedShapes(1, tkSpatialRelation.srTouches, ref resultArray).ShouldBeTrue();
			(resultArray is Array).ShouldBeTrue();
			((Array)resultArray).Length.ShouldBe(2);
		}

		[Fact]
		public void ShapefileGetRelatedShapes2Test()
		{
			var resultArray = new object();
			var shape = _shapefile.Shape[1];
			_shapefile.GetRelatedShapes2(shape, tkSpatialRelation.srTouches, ref resultArray).ShouldBeTrue();
			(resultArray is Array).ShouldBeTrue();
			((Array)resultArray).Length.ShouldBe(2);
		}

		[Fact]
		public void ShapefileSegmentizeTest()
		{
			/// TODO: Needd to verify what the expected result should be for this test.

			//var shapeIndexes = new long[] { 422, 510, 5041, 5141 };
			var shapeIndexes = new long[] { 422, 510 };
			var shapefile = Helpers.DeepClone(_shapefile, shapeIndexes);
			shapefile.NumShapes.ShouldBe(shapeIndexes.Length);
			//shapefile.SaveAs(@"C:\Temp\shapefile_before_Segmentize.shp");
			//var newShapefile = shapefile.Segmentize(100);
			var newShapefile = shapefile.Segmentize(2);
			newShapefile.ShouldNotBeNull();
			//newShapefile.SaveAs(@"C:\Temp\newShapefile.shp");
		}

		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGetClosestVertexTest()
		{
			/// TODO: It seams that GetClosestVertex only works on shapes that is visible according to ShapeVisible, and a shapefile that is not added to a map is never visible. Is that correct behavioure ?

			var wkt = "LINESTRING (-10176776.32009091414511204 3580501.03530521970242262, -10987528.91594749875366688 3763458.4795029447413981, -11737295.6970715094357729 4244170.19563029985874891, -11737295.6970715094357729 4244170.19563029985874891)";
			var sfPolyline = Helpers.CreateTestShapefile(ShpfileType.SHP_POLYLINE, wkt, 28992);
			sfPolyline.VisibilityExpression = "";
			sfPolyline.DefaultDrawingOptions.Visible = true;
			var isVisible = sfPolyline.ShapeVisible[0];
			var x = -10999398.10846591927111149;
			var y = 3771373.48348145373165607;
			var ret = sfPolyline.GetClosestVertex(x, y, 0, out int shapeIndex, out var pointIndex, out var distance);
			//distance.ShouldBeLessThan(200);
		}

		[Fact]
		public void ShapefileHasInvalidShapesTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			shapefile.HasInvalidShapes().ShouldBeFalse();

			var sfPolygon = Helpers.CreateTestPolygonShapefile();
			var shape = sfPolygon.Shape[0].Clone();
			shape.ReversePointsOrder(0).ShouldBeTrue();
			shapefile = new Shapefile();
			shapefile.CreateNew("", ShpfileType.SHP_POLYGON);
			shapefile.HasInvalidShapes().ShouldBeFalse();
			shapefile.EditAddShape(shape);
			shapefile.HasInvalidShapes().ShouldBeTrue();
		}

		[Fact]
		public void ShapefileDumpTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_Dump_TestFile", ".shp");
			shapefile.Dump(fileName).ShouldBeTrue();

			var newShapefile = new Shapefile();
			newShapefile.Open(fileName).ShouldBeTrue();
			newShapefile.ShapefileType.ShouldBe(shapefile.ShapefileType);
			newShapefile.NumShapes.ShouldBe(shapefile.NumShapes);
		}

		[Fact]
		public void ShapefileLoadDataFromTest()
		{
			var shapefile = new Shapefile();
			var fileName = Helpers.GetTestFilePath("UnitedStates-3857.shp");
			shapefile.LoadDataFrom(fileName).ShouldBeFalse();
			shapefile.CreateNew("", ShpfileType.SHP_NULLSHAPE);
			shapefile.LoadDataFrom(fileName).ShouldBeTrue();
			shapefile.SourceType.ShouldBe(tkShapefileSourceType.sstInMemory);
			shapefile.Filename.ShouldBeNullOrWhiteSpace();
			shapefile.NumShapes.ShouldBeGreaterThan(0);
		}

		[Fact]
		public void ShapefileClearCachedGeometriesTest()
		{
			_shapefile.ClearCachedGeometries();
		}

		[Fact]
		public void ShapefileAggregateShapesWithStatsTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 10);
			shapefile.NumShapes.ShouldBe(10);
			var name1 = shapefile.CellValue[1, 1];
			var name2 = shapefile.CellValue[1, 3];

			var newName = $"[{name1}-{name2}]";
			shapefile.EditCellValue(1, 1, newName).ShouldBeTrue();
			shapefile.EditCellValue(1, 3, newName).ShouldBeTrue();

			var statOperations = new FieldStatOperationsClass();
			var sfResult = shapefile.AggregateShapesWithStats(false, 1, statOperations);
			sfResult.ShouldNotBeNull();
			sfResult.NumShapes.ShouldBe(9);
			var shape = sfResult.Shape[0];
			shape.NumParts.ShouldBe(2);
		}

		[Fact]
		public void ShapefileDissolveWithStatsTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 10);
			var name1 = shapefile.CellValue[1, 1];
			var name2 = shapefile.CellValue[1, 3];

			var newName = $"[{name1}-{name2}]";
			shapefile.EditCellValue(1, 1, newName).ShouldBeTrue();
			shapefile.EditCellValue(1, 3, newName).ShouldBeTrue();

			var statOperations = new FieldStatOperationsClass();
			/// TODO: Use FieldStatOperations
			var sfResult = shapefile.DissolveWithStats(1, false, statOperations);
			sfResult.ShouldNotBeNull();
			sfResult.NumShapes.ShouldBe(9);
			var newShape = sfResult.Shape[0];
			newShape.NumParts.ShouldBe(1);

			var shape1 = shapefile.Shape[1];
			var shape2 = shapefile.Shape[3];
			Math.Round((shape1.Area + shape2.Area) - newShape.Area, 4).ShouldBe(0.000);
		}

		[Fact]
		public void ShapefileEditUpdateShapeTest()
		{
			var shapefile = Helpers.DeepClone(_polyShapefile, 10);
			var newShape = shapefile.Shape[3].Clone();
			var orgExtCenter = newShape.Extents.Center.Clone();
			orgExtCenter.ShouldNotBeNull();
			newShape.Move(10, 10);
			shapefile.EditUpdateShape(3, newShape);
			var newExt = shapefile.Shape[3].Extents;
			newExt.Center.x.ShouldBe(orgExtCenter.x + 10);
			newExt.Center.y.ShouldBe(orgExtCenter.y + 10);
		}

		[Fact]
		public void ShapefileSerialize2Test()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var category = shapefile.Categories.Add("Test_100");
			category.Name = "Test_100";
			category.MinValue = 100;
			category.MaxValue = 100;
			var field = shapefile.Field[0];
			category.Expression = $"[{field.Name}]";
			category.DrawingOptions.Visible = true;
			category.DrawingOptions.LineWidth = 2;
			category.DrawingOptions.LineColor = 0;

			var text = shapefile.Serialize2(true, true);
			text.ShouldNotBeNullOrEmpty();

			var newShapefile = new Shapefile();
			newShapefile.Deserialize(true, text);
			newShapefile.Categories.Count.ShouldBe(1);
			var cat = newShapefile.Categories.Item[0];
			cat.Name.ShouldBe(category.Name);
		}

		[Fact]
		public void ShapefileMoveTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var orgCenter = shapefile.Extents.Center.Clone();
			orgCenter.ShouldNotBeNull();

			shapefile.Move(-100, -100).ShouldBeTrue();
			var newExt = shapefile.Extents;
			newExt.Center.x.ShouldBe(orgCenter.x - 100);
			newExt.Center.y.ShouldBe(orgCenter.y - 100);
		}

		[Fact]
		public void ShapefileRemoveSpatialIndexTest()
		{
			//Thread.Sleep(TimeSpan.FromSeconds(30));
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_RemoveSpatialIndex_File", ".shp");
			var ret1 = shapefile.SaveAs(fileName);
			shapefile.HasSpatialIndex.ShouldBeFalse();
			shapefile.CreateSpatialIndex(fileName).ShouldBeTrue();
			shapefile.RemoveSpatialIndex().ShouldBeTrue();
		}

		[Fact]
		public void ShapefileUpdateSortFieldTest()
		{
			var field = _shapefile.Field[0];
			_shapefile.SortField = field.Name;
			_shapefile.UpdateSortField();
			_shapefile.LastErrorCode.ShouldBe(0);
		}

		[Fact]
		public void ShapefileSaveAsExTest()
		{
			//Thread.Sleep(TimeSpan.FromSeconds(30));
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_SaveAsEx_File", ".shp");
			shapefile.SaveAsEx(fileName, true, true).ShouldBeTrue();
			shapefile.Filename.ShouldBeNullOrWhiteSpace();
			Helpers.DeleteShapefileFiles(fileName);
			var ret2 = shapefile.SaveAsEx(fileName, true, false);
			System.Diagnostics.Debug.WriteLine($"ret2: {ret2}");
			shapefile.Filename.ShouldNotBeNullOrEmpty();
		}

		[Fact]
		public void ShapefileFixUpShapes2Test()
		{

		}

		[Fact]
		public void ShapefileStartAppendModeTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_StartAppendMode_File", ".shp");
			shapefile.SaveAs(fileName).ShouldBeTrue();
			shapefile.AppendMode.ShouldBeFalse();
			shapefile.StartAppendMode().ShouldBeTrue();
			shapefile.AppendMode.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileStopAppendModeTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var fileName = Helpers.GetRandomFilePath("_StartAppendMode_File", ".shp");
			shapefile.SaveAs(fileName).ShouldBeTrue();
			shapefile.AppendMode.ShouldBeFalse();
			shapefile.StartAppendMode().ShouldBeTrue();
			shapefile.AppendMode.ShouldBeTrue();
			shapefile.StopAppendMode();
			shapefile.AppendMode.ShouldBeFalse();
		}

		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGetClosestSnapPositionTest()
		{
			var shapefile = Helpers.DeepClone(_shapefile, 10);
			var x = 108.89293929165357611;
			var y = 34.20425385854753131;

			/// TODO: It seams that GetClosestSnapPosition only works on shapes that is visible according to ShapeVisible, and a shapefile that is not added to a map is never visible. Is that correct behavioure ?
			var ret = shapefile.GetClosestSnapPosition(x, y, 0.0, out var shpIndex, out var fx, out var fy, out var distance);
			ret.ShouldBeTrue();
		}

		[Fact]
		public void ShapefileOgrFid2ShapeIndexTest()
		{
			var shapefile = _shapefile.Clone();
			if(shapefile.HasOgrFidMapping)
				shapefile.OgrFid2ShapeIndex(0).ShouldBe(0);
			else
				shapefile.OgrFid2ShapeIndex(0).ShouldBe(-1);
		}

		/// <summary>
		/// xUnit creates a new <see cref="ShapefileTest"/> instance per test method, and the
		/// constructor opens the shared TestData shapefiles (e.g. <c>UnitedStates-3857.*</c>).
		/// The native MapWinGIS <see cref="Shapefile"/> keeps OS file handles on the .shp/.shx/.dbf
		/// open (the .dbf without a read-share mode). Without deterministic cleanup those handles
		/// linger until the object is finalized by the GC, locking the TestData files and causing
		/// other tests/fixtures (such as GdalUtilsFixture copying UnitedStates-3857) to fail with
		/// "The process cannot access the file ... because it is being used by another process".
		/// Closing them here releases the handles as soon as each test finishes.
		/// </summary>
		public void Dispose()
		{
			_shapefile?.Close();
			_polyShapefile?.Close();
			GC.SuppressFinalize(this);
		}

	}

	internal class StopExecution : IStopExecution
	{
		public bool StopFunction()
		{
			return true;
		}
	}
}
