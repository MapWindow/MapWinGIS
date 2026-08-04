using Xunit.Abstractions;

namespace MapWinGisTests.UnitTests
{
	[Collection(nameof(NotThreadSafeResourceCollection))]
	public class ShapefileTest
	{
		private readonly ITestOutputHelper _testOutputHelper;
		private readonly GlobalSettings _gs;
		private readonly Shapefile _shapefile;

		public ShapefileTest(ITestOutputHelper testOutputHelper)
		{
			_testOutputHelper = testOutputHelper;
			_gs = new GlobalSettings();
			var shapefileName = Helpers.GetTestFilePath("Issue-216.shp");
			_shapefile = new Shapefile();
			_shapefile.Open(shapefileName);
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

		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileStopEditingTableTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileQuickPointTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileQuickExtentsTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileQuickPointsTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefilePointInShapeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefilePointInShapefileTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileBeginPointInShapefileTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileEndPointInShapefileTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileCreateNewWithShapeIDTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileCreateSpatialIndexTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileResourceTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileRefreshExtentsTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileRefreshShapeExtentsTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSaveTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileIsSpatialIndexValidTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGetIntersectionTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSelectByShapefileTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSelectAllTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSelectNoneTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileInvertSelectionTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileDissolveTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGenerateLabelsTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileCloneTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileBufferByDistanceTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileDifferenceTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileClipTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSymmDifferenceTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileUnionTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileExplodeShapesTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileAggregateShapesTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileExportSelectionTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSortTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileMergeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSerializeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileDeserializeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileReprojectTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileReprojectInPlaceTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSimplifyLinesTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileFixUpShapesTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileEditAddShapeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileEditAddFieldTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGetRelatedShapesTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGetRelatedShapes2Test() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSegmentizeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGetClosestVertexTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileHasInvalidShapesTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileDumpTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileLoadDataFromTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileClearCachedGeometriesTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileAggregateShapesWithStatsTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileDissolveWithStatsTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileEditUpdateShapeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSerialize2Test() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileMoveTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileRemoveSpatialIndexTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileUpdateSortFieldTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileSaveAsExTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileFixUpShapes2Test() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileStartAppendModeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileStopAppendModeTest() { }
		[Fact(Skip = "Unit test is not yet implemented")]
		public void ShapefileGetClosestSnapPositionTest() { }

		[Fact]
		public void ShapefileOgrFid2ShapeIndexTest()
		{
			var shapefile = _shapefile.Clone();
			if(shapefile.HasOgrFidMapping)
				shapefile.OgrFid2ShapeIndex(0).ShouldBe(0);
			else
				shapefile.OgrFid2ShapeIndex(0).ShouldBe(-1);
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
