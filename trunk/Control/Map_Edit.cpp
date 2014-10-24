#include "stdafx.h"
#include "Map.h"
#include "Shapefile.h"
#include "ShapeEditor.h"

// ************************************************************
//		HandleLeftButtonUpDragVertexOrShape
// ************************************************************
bool CMapView::HandleLButtonUpDragVertexOrShape(long nFlags)
{
	double x1, x2, y1, y2;
	PixelToProj(_dragging.Start.x, _dragging.Start.y, &x1, &y1);
	PixelToProj(_dragging.Move.x, _dragging.Move.y, &x2, &y2);

	DraggingOperation operation = _dragging.Operation;

	if (operation == DragMoveVertex && !_dragging.HasMoved) {
		((CShapeEditor*)_shapeEditor)->DiscardState();
		return false;
	}

	if (_dragging.HasMoved && operation == DragMoveVertex)
	{
		tkSnapBehavior behavior;
		if (SnappingIsOn(nFlags, behavior))
		{
			VARIANT_BOOL result = this->FindSnapPoint(GetMouseTolerance(ToleranceSnap, false), _dragging.Move.x, _dragging.Move.y, &x2, &y2);
			if ( !result && behavior == sbSnapWithShift )
				return true;		// can't proceed without snapping in this mode
		}
		GetEditorBase()->MoveVertex(x2, y2);		// don't save state; it's already saved at the beginning of operation
	}

	if (operation == DragMoveShape && (x2 - x1 != 0.0 || y2 - y1 != 0))
	{
		((CShapeEditor*)_shapeEditor)->MoveShape(x2 - x1, y2 - y1);
	}
	_dragging.Operation = DragNone;
	return true;
}

// ************************************************************
//		SnappingIsOn
// ************************************************************
bool CMapView::SnappingIsOn(long nFlags, tkSnapBehavior& behavior)
{
	if (m_cursorMode == cmMeasure) {
		behavior = tkSnapBehavior::sbSnapWithShift;
	}
	else {
		_shapeEditor->get_SnapBehavior(&behavior);
		if (behavior == sbSnapByDefault) return true;
	}
	return (nFlags & MK_SHIFT) == 0;
}

// ************************************************************
//		HandleOnMouseMoveShapeEditor
// ************************************************************
bool CMapView::HandleOnMouseMoveShapeEditor(int x, int y, long nFlags)
{
	if ((_dragging.Operation == DragMoveVertex || _dragging.Operation == DragMoveShape)
		&& (nFlags & MK_LBUTTON) && _leftButtonDown)
	{
		_dragging.Snapped = false;
		tkSnapBehavior behavior;
		if (SnappingIsOn(nFlags, behavior) && _dragging.Operation == DragMoveVertex)
		{
			double xFound, yFound;
			if (this->FindSnapPoint(GetMouseTolerance(ToleranceSnap, false), x, y, &xFound, &yFound)) {
				double xNew, yNew;
				ProjToPixel(xFound, yFound, &xNew, &yNew);
				_dragging.SetSnapped(xFound, yFound);
			}
		}
		_dragging.HasMoved = true;
		_canUseMainBuffer = false;
		return true;
	}
	else 
	{
		// highlighting of vertices
		double projX, projY;
		this->PixelToProjection(x, y, projX, projY);
		EditorBase* base = GetEditorBase();
		int pntIndex = base->SelectVertex(projX, projY, GetMouseTolerance(ToleranceSelect));
		if (pntIndex != -1)
		{
			bool changed = base->_highlightedVertex != pntIndex;
			base->_highlightedVertex = pntIndex;
			if (changed) {
				_canUseMainBuffer = false;
				return true;
			}
		}
		else {
			if (base->_highlightedVertex != -1) {
				base->_highlightedVertex = -1;
				_canUseMainBuffer = false;
				return true;
			}
		}
	}
	return false;
}

// ************************************************************
//		RemoveSelectedShape
// ************************************************************
bool CMapView::RemoveSelectedShape()
{
	VARIANT_BOOL isEmpty;
	_shapeEditor->get_IsEmpty(&isEmpty);
	if (!isEmpty) {
		int layerHandle, shapeIndex;
		_shapeEditor->get_LayerHandle(&layerHandle);
		_shapeEditor->get_ShapeIndex(&shapeIndex);
		
		CComPtr<IShapefile> sf = GetShapefile(layerHandle);
		if (sf) {
			tkMwBoolean cancel = blnFalse;
			FireBeforeShapeEdit(uoRemoveShape, layerHandle, shapeIndex, &cancel);
			if (cancel == blnTrue) return false;

			VARIANT_BOOL vb;
			_undoList->Add(uoRemoveShape, layerHandle, shapeIndex, &vb);
			if (vb) {
				sf->EditDeleteShape(shapeIndex, &vb);
				FireAfterShapeEdit(uoRemoveShape, layerHandle, shapeIndex);
				return true;
			}
			
		}
	}
	return false;
}

// ************************************************************
//		UndoEdit
// ************************************************************
VARIANT_BOOL CMapView::UndoEdit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL vb;
	_shapeEditor->Undo(&vb);
	if (vb) return vb;
	
	// one call to clear the edit shape
	VARIANT_BOOL isEmpty;
	_shapeEditor->get_IsEmpty(&isEmpty);
	if (!isEmpty) {
		_shapeEditor->Clear();
		return VARIANT_TRUE;
	}
	
	return VARIANT_TRUE;
	//return RunShapefileUndoList(true) ?  VARIANT_TRUE: VARIANT_FALSE;
}

// ************************************************************
//		RedoEdit
// ************************************************************
VARIANT_BOOL CMapView::RedoEdit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL vb;
	_shapeEditor->Redo(&vb);
	if (vb) return vb;

	// one call to clear the edit shape
	VARIANT_BOOL isEmpty;
	_shapeEditor->get_IsEmpty(&isEmpty);
	if (!isEmpty) {
		_shapeEditor->Clear();
		return VARIANT_TRUE;
	}
	
	return VARIANT_TRUE;
	//return RunShapefileUndoList(false) ? VARIANT_TRUE : VARIANT_FALSE;
}

// ************************************************************
//		GetUndoList
// ************************************************************
IUndoList* CMapView::GetUndoList()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_undoList) {
		_undoList->AddRef();
	}
	return _undoList;
}

// ************************************************************
//		RunShapefileUndoList
// ************************************************************
//bool CMapView::RunShapefileUndoList(bool undo)
//{
//	vector<int> layers;
//	SelectLayerHandles(slctInteractiveEditing, layers);
//	
//	int layerHandle = -1;
//	int maxId = -1;
//	for (size_t i = 0; i < layers.size(); i++) 
//	{
//		CComPtr<IShapefile> sf = GetShapefile(layers[i]);
//		if (sf) 
//		{
//			CComPtr<IUndoList> list;
//			sf->get_UndoList(&list);
//			if (list) 
//			{
//				long id = -1;
//				list->GetLastId(&id);
//				if (id > maxId) {
//					maxId = id;
//					layerHandle = layers[i];
//				}
//			}
//		}
//	}
//
//	if (layerHandle != -1) 
//	{
//		CComPtr<IShapefile> sf = GetShapefile(layerHandle);
//		CComPtr<IUndoList> list;
//		sf->get_UndoList(&list);
//
//		VARIANT_BOOL vb;
//		if (undo) {
//			list->Undo(&vb);
//		}
//		else {
//			list->Redo(&vb);
//		}
//		return true;
//	}
//	return false;
//}

// ************************************************************
//		GetShapeEditorShapefile
// ************************************************************
IShapefile* CMapView::GetShapeEditorShapefile()
{
	int layerHandle;
	_shapeEditor->get_LayerHandle(&layerHandle);
	return GetShapefile(layerHandle);
}

// ************************************************************
//		HandleOnLButtonDownShapeEditor
// ************************************************************
void CMapView::HandleOnLButtonDownShapeEditor(int x, int y, bool ctrl)
{
	long layerHandle = -1, shapeIndex = -1;
	bool handled = false;
	bool hasShapeEditor = !GetEditorBase()->IsEmpty();
	
	if (hasShapeEditor)
	{
		IShapefile* sf = GetTempShapefile();
		if (sf != NULL)
		{
			CComPtr<IShape> shp = NULL;
			_shapeEditor->get_RawData(&shp);
			double projX, projY;
			PixelToProj(x, y, &projX, &projY);
		
			if (m_cursorMode == cmEditShape)
			{
				if (ctrl)
				{
					// let's try to add vertex
					if (TryAddVertex(projX, projY)) {
						handled = true;
						RedrawCore(tkRedrawType::RedrawSkipDataLayers, false, true);
					}
				}
				else
				{
					EditorBase* base = GetEditorBase();
					int pntIndex = base->SelectVertex(projX, projY, GetMouseTolerance(ToleranceSelect));
					if (pntIndex != -1)
					{
						// start vertex moving
						bool changed = base->_selectedVertex != pntIndex;
						base->_selectedVertex = pntIndex;

						this->SetCapture();
						_dragging.Operation = DragMoveVertex;
						handled = true;

						((CShapeEditor*)_shapeEditor)->SaveState();

						if (changed)
							RedrawCore(tkRedrawType::RedrawSkipDataLayers, false, true);
					}
					else {
						// start shape moving
						if (((CShapefile*)sf)->PointWithinShape(shp, projX, projY, GetMouseTolerance(ToleranceSelect)))
						{
							this->SetCapture();
							_dragging.Operation = DragMoveShape;
							handled = true;
						}
					}
				}
			}
		}
	}
	
	if (handled)
		return;

	if (hasShapeEditor)
	{
		if (!TrySaveShapeEditor())
			return;
	}
	
	_shapeEditor->Clear();

	if (SelectSingleShape(x, y, layerHandle, shapeIndex)) {
		VARIANT_BOOL vb;
		_shapeEditor->StartEdit(layerHandle, shapeIndex, &vb);
	}

	RedrawCore(RedrawAll, false, false);
}

// ************************************************************
//		HandleOnLButtonShapeAddMode
// ************************************************************
void CMapView::HandleOnLButtonShapeAddMode(int x, int y, double projX, double projY, bool ctrl)
{
	EditorBase* editShape = GetEditorBase();

	// it's the first point; shape type and layer
	ShpfileType shpType = editShape->GetShapeType();
	if (shpType == SHP_NULLSHAPE)
	{
		if (!ChooseEditLayer(x, y)) return;
		shpType = Utility::ShapeTypeConvert2D(editShape->GetShapeType());
	}

	// an attempt to finish shape
	bool succeed = false;
	if (ctrl)
	{
		succeed = TrySaveShapeEditor();
		if (succeed) {
			RedrawCore(RedrawSkipDataLayers, false, true);
			return;
		}
	}

	// add another point
	editShape->HandleProjPointAdd(projX, projY);

	// for point layer it's also a shape
	bool isPoint = shpType == SHP_POINT;
	if (isPoint)
	{
		succeed = TrySaveShapeEditor();
		if (succeed) {
			RedrawCore(RedrawAll, false, true);
		}
		return;
	}

	// otherwise update just the layer
	// TODO: fire vertex added event
	RedrawCore(RedrawSkipDataLayers, false, true);
}

// ************************************************************
//		ChooseEditLayer
// ************************************************************
bool CMapView::ChooseEditLayer(long x, long y)
{
	tkMwBoolean cancel = blnFalse;
	long layerHandle = -1;
	if (GetInteractiveShapefileCount(layerHandle) > 0)
	{
		FireNewShape(x, y, &layerHandle, &cancel);
		if (cancel == blnTrue) return false;
	}
	else {
		ErrorMessage(tkNO_INTERACTIVE_SHAPEFILES);
		return false;
	}

	SetShapeEditor(layerHandle);
	
	return true;
}

// ************************************************************
//		SetShapeEditor
// ************************************************************
// the case of new shape
bool CMapView::SetShapeEditor(long layerHandle)
{
	CComPtr<IShapefile> sf = GetShapefile(layerHandle);
	if (!sf) {
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return false;
	}

	ShpfileType shpType;
	sf->get_ShapefileType(&shpType);

	_shapeEditor->Clear();
	_shapeEditor->put_ShapeType(shpType);
	_shapeEditor->put_LayerHandle(layerHandle);
	
	CComPtr<IShapeDrawingOptions> options = NULL;
	sf->get_DefaultDrawingOptions(&options);
	_shapeEditor->CopyOptionsFrom(options);
	return true;
}

// ************************************************************
//		TrySaveShapeEditor
// ************************************************************
bool CMapView::TrySaveShapeEditor()
{
	int layerHandle, shapeIndex;
	_shapeEditor->get_LayerHandle(&layerHandle);
	_shapeEditor->get_ShapeIndex(&shapeIndex);
	bool newShape = shapeIndex == -1;

	// first check if it's enough points
	VARIANT_BOOL enoughPoints;
	_shapeEditor->get_HasEnoughPoints(&enoughPoints);
	if (!enoughPoints) {
		FireValidationResults(VARIANT_FALSE, "The shape doesn't have enough points");		// TODO: localize
		return false;
	}

	CComPtr<IShape> shp = NULL;
	if (m_cursorMode == cmAddPart || m_cursorMode == cmRemovePart) {
		SubjectOperation op = m_cursorMode == cmAddPart ? SubjectAddPart: SubjectClip;
		shp = ((CShapeEditor*)_shapeEditor)->ApplyOperation(op, layerHandle, shapeIndex);
		newShape = false;
	}
	else {
		_shapeEditor->get_Shape(VARIANT_FALSE, &shp);
	}

	// does user want to validate with GEOS?
	tkMwBoolean geosCheck = blnTrue, tryFix = blnTrue;
	FireValidationMode(&geosCheck, &tryFix);
	
	// if so validate and optionally fix
	VARIANT_BOOL valid = VARIANT_TRUE;
	if (geosCheck == blnTrue) 
	{
		shp->get_IsValid(&valid);
		if (!valid && tryFix == blnTrue) 
		{
			IShape* shpNew = NULL;
			shp->FixUp(&shpNew);
			if (shpNew) {
				shp = NULL;				// TODO: will it release the shape?
				shp = shpNew;
				valid = true;
			}
		}
		
		// report results back to user
		CComBSTR reason;
		if (!valid) {
			shp->get_IsValidReason(&reason);
		}
		else {
			reason = ::SysAllocString(L"");
		}
		USES_CONVERSION;
		FireValidationResults(valid, OLE2A(reason));
	}
	
	if (!valid) {
		return false;
	}

	// now let the user check custom validation rules
	if (valid) {
		tkMwBoolean cancel = blnFalse;
		FireValidateShape((tkCursorMode)m_cursorMode, layerHandle, shp, &cancel);
		if (cancel == blnTrue) {
			return false;
		}
	}
	
	//finally ready to save	
	CComPtr<IShapefile> sf = NULL;
	sf = GetShapefile(layerHandle);

	if (!sf) {
		// TODO: check it earlier on
		return false;
	}

	long numShapes;
	sf->get_NumShapes(&numShapes);

	VARIANT_BOOL vb;
	if (newShape)
	{
		// register with undo list
		_undoList->Add(uoAddShape, (long)layerHandle, (long)numShapes, &vb);
	}
	else  {
		// copy all the actions from ShapeEditor undo list
		vector<IShape*> list = ((CShapeEditor*)_shapeEditor)->GetUndoList();
		for (size_t i = 0; i < list.size(); i++) {
			_undoList->AddSubOperation(uoSubShapeEditor, (long)layerHandle, (long)shapeIndex, list[i], &vb);
		}
	}

	// add new shape
	if (newShape) {
		
		sf->EditInsertShape(shp, &numShapes, &vb);
		shapeIndex = numShapes;
	}
	else {
		sf->EditUpdateShape(shapeIndex, shp, &vb);
	}

	_shapeEditor->Clear();

	// let the user set new attributes
	FireAfterShapeEdit(uoEditingShape, layerHandle, shapeIndex);
	return true;
}

// ************************************************************
//		DoFireAfterShapeEdit
// ************************************************************
void CMapView::DoFireAfterShapeEdit()
{
	int layerHandle, shapeIndex;
	_shapeEditor->get_LayerHandle(&layerHandle);
	_shapeEditor->get_ShapeIndex(&shapeIndex);
	FireAfterShapeEdit(uoEditingShape, layerHandle, shapeIndex);
}

// ************************************************************
//		TryAddVertex
// ************************************************************
bool CMapView::TryAddVertex(double projX, double projY)
{
	ShpfileType shpType;
	_shapeEditor->get_ShapeType(&shpType);
	shpType = Utility::ShapeTypeConvert2D(shpType);

	if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
		return false;				// TODO: multi points should be supported

	bool success = false;
	CComPtr<IShape> shp = NULL;
	_shapeEditor->get_Shape(VARIANT_TRUE, &shp);
	if (shp)
	{
		if (shpType == SHP_POLYGON)
		{
			// we need ClosestPoints method to return points on contour, and not inner points
			shp->put_ShapeType(SHP_POLYLINE);
		}

		// creating temp edit shape
		VARIANT_BOOL vb;
		long pointIndex = 0;
		CComPtr<IShape> shp2 = NULL;
		GetUtils()->CreateInstance(idShape, (IDispatch**)&shp2);
		shp2->Create(SHP_POINT, &vb);
		shp2->AddPoint(projX, projY, &pointIndex);

		CComPtr<IShape> result = NULL;
		shp->ClosestPoints(shp2, &result);
		if (result)
		{
			double x, y;
			result->get_XY(0, &x, &y, &vb);		// 0 = point lying on the line

			double dist = sqrt(pow(x - projX, 2.0) + pow(y - projY, 2.0));
			if (dist < GetMouseTolerance(ToleranceSelect))
			{
				success = ((CShapeEditor*)_shapeEditor)->InsertVertex(x, y);
			}
		}
	}
	return success;
}


