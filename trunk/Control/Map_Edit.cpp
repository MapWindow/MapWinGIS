#include "stdafx.h"
#include "Map.h"
#include "Shapefile.h"
#include "ShapeEditor.h"
#include "UndoList.h"
#include "GeometryHelper.h"

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
		_shapeEditor->DiscardState();
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

	if (x2 - x1 != 0.0 || y2 - y1 != 0)
	{
		if (operation == DragMovePart)
		{
			_shapeEditor->MovePart(x2 - x1, y2 - y1);
		}
		if (operation == DragMoveShape)
		{
			_shapeEditor->MoveShape(x2 - x1, y2 - y1);
		}
	}
	return true;
}

// ************************************************************
//		SnappingIsOn
// ************************************************************
bool CMapView::SnappingIsOn(long nFlags, tkSnapBehavior& behavior)
{
	if (m_cursorMode == cmMeasure) {
		return false;
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
	if ((_dragging.Operation == DragMoveVertex || 
		 _dragging.Operation == DragMoveShape || 
		_dragging.Operation == DragMovePart))      // && (nFlags & MK_LBUTTON) && _leftButtonDown
	{
		_dragging.Snapped = false;
		tkSnapBehavior behavior;
		if (SnappingIsOn(nFlags, behavior) && behavior == sbSnapByDefault && _dragging.Operation == DragMoveVertex)
		{
			double xFound, yFound;
			if (this->FindSnapPoint(GetMouseTolerance(ToleranceSnap, false), x, y, &xFound, &yFound)) {
				double xNew, yNew;
				ProjToPixel(xFound, yFound, &xNew, &yNew);
				_dragging.SetSnapped(xFound, yFound);
			}
		}
		_dragging.HasMoved = true;

		// in case of vertex moving underlying data is changed in the process (to update displayed length);
		// for other types of moving we simply display the shape with offset and modify the data when dragging
		// has finished, i.e. LButtonMouseUp event
		if (_dragging.Operation == DragMoveVertex) 
		{
			EditorBase* edit = GetEditorBase();
			MeasurePoint* pnt = edit->GetPoint(edit->_selectedVertex);
			if (pnt) {
				if (_dragging.Snapped) {
					edit->MoveVertex(_dragging.Proj.x, _dragging.Proj.y);
				}
				else {
					if (_dragging.HasMoved) {
						double x1, x2, y1, y2;
						PixelToProj(_dragging.Start.x, _dragging.Start.y, &x1, &y1);
						PixelToProj(_dragging.Move.x, _dragging.Move.y, &x2, &y2);
						edit->MoveVertex(x2, y2);
					}
				}
				_dragging.Start = _dragging.Move;
			}
		}
		_canUseMainBuffer = false;
		return true;
	}
	else 
	{
		// highlighting of vertices
		bool handled = false;
		double projX, projY;
		this->PixelToProjection(x, y, projX, projY);
		EditorBase* base = GetEditorBase();
		int pntIndex = base->GetClosestVertex(projX, projY, GetMouseTolerance(ToleranceSelect));
		if (pntIndex != -1)
		{
			if (base->SetHighlightedVertex(pntIndex)) {
				_canUseMainBuffer = false;
				return true;
			}
			return false;
		}
		else {
			if (base->ClearHighlightedVertex()) {
				_canUseMainBuffer = false;
				return true;
			}
		}
		
		// highlighting parts
		if (nFlags & MK_CONTROL) {
			int partIndex = _shapeEditor->GetClosestPart(projX, projY, GetMouseTolerance(ToleranceSelect));
			if (partIndex != -1) 
			{
				if (base->SetHighlightedPart(partIndex)) {
					_canUseMainBuffer = false;
				}
				return true;
			}
		}
		
		if (base->ClearHighlightedPart()){
			_canUseMainBuffer = false;
			handled = true;
		}
	}
	return false;
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
				// select vertex
				if (!ctrl) {
					EditorBase* base = GetEditorBase();
					int pntIndex = base->GetClosestVertex(projX, projY, GetMouseTolerance(ToleranceSelect));
					if (pntIndex != -1)
					{
						// start vertex moving
						bool changed = base->SetSelectedVertex(pntIndex);

						this->SetCapture();
						_dragging.Operation = DragMoveVertex;
						_shapeEditor->SaveState();

						if (changed)
							RedrawCore(tkRedrawType::RedrawSkipDataLayers, false, true);

						return;
					}
				}

				// add vertex or select part
				double x, y;
				if (_shapeEditor->GetClosestPoint(projX, projY, x, y))
				{
					double dist = sqrt(pow(x - projX, 2.0) + pow(y - projY, 2.0));
					if (dist < GetMouseTolerance(ToleranceSelect))
					{
						EditorBase* base = GetEditorBase();
						if (ctrl)
						{
							// select part
							int partIndex = base->SelectPart(x, y);
							if (partIndex != -1) 
							{
								if (base->SetSelectedPart(partIndex)) {
									this->SetCapture();
									_dragging.Operation = DragMovePart;
								}
								RedrawCore(tkRedrawType::RedrawSkipDataLayers, false, true);
								return;
							}
						}
						if (base->HasSelectedPart()) {
							this->SetCapture();
							_dragging.Operation = DragMovePart;
							return;
						}
					}
				}

				// start shape moving
				if (((CShapefile*)sf)->PointWithinShape(shp, projX, projY, GetMouseTolerance(ToleranceSelect)))
				{
					// it's confusing to have both part and shape move depending on where you clicked
					if (GetEditorBase()->HasSelectedPart()) return;   

					this->SetCapture();
					_dragging.Operation = DragMoveShape;
					return;
				}
			}
		}

		if (!_shapeEditor->TrySave())
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
		succeed = _shapeEditor->TrySave();
		if (succeed) {
			RedrawCore(RedrawSkipDataLayers, false, true);
		}
		return;
	}

	// add another point
	_shapeEditor->HandleProjPointAdd(projX, projY);

	// for point layer it's also a shape
	bool isPoint = shpType == SHP_POINT;
	if (isPoint)
	{
		succeed = _shapeEditor->TrySave();
		if (succeed) {
			RedrawCore(RedrawAll, false, true);
		}
		return;
	}

	// otherwise update just the layer
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
		FireChooseLayer(x, y, &layerHandle, &cancel);
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
//		HandleLButtonSubjectCursor
// ************************************************************
void CMapView::HandleLButtonSubjectCursor(int x, int y, double projX, double projY, bool ctrl)
{
	long numShapes;
	_shapeEditor->get_NumSubjectShapes(&numShapes);
	if (numShapes == 0) 
	{
		// let's choose the subject
		long shapeIndex = -1, layerHandle = -1;
		if (SelectSingleShape(x, y, layerHandle, shapeIndex)) 
		{
			VARIANT_BOOL vb;
			_shapeEditor->AddSubjectShape(layerHandle, shapeIndex, VARIANT_TRUE, &vb);
			RedrawCore(RedrawAll, false, true);
		}
	}
	else {
		tkShapeEditorState state;
		_shapeEditor->get_EditorState(&state);
		if (state != EditorCreationUnbound)
		{
			VARIANT_BOOL vb;
			ShpfileType shpType = _shapeEditor->GetOverlayTypeForSubjectOperation((tkCursorMode)m_cursorMode);
			_shapeEditor->StartUnboundShape(shpType, &vb);

			OLE_COLOR color;
			GetUtils()->ColorByName(m_cursorMode == cmAddPart ? Green : Red, &color);
			_shapeEditor->put_FillColor(color);
			if (m_cursorMode == cmSplitByPolyline) {
				_shapeEditor->put_LineColor(RGB(255,0,0));
			}
		}
		HandleOnLButtonShapeAddMode(x, y, projX, projY, ctrl);
	}
}

// ************************************************************
//		HandleOnLButtonMoveOrRotate
// ************************************************************
void CMapView::HandleOnLButtonMoveOrRotate(long x, long y)
{
	long layerHandle = -1;
	tkMwBoolean cancel = tkMwBoolean::blnFalse;
	FireChooseLayer(x, y, &layerHandle, &cancel);
	if (layerHandle != -1)
	{
		CComPtr<IShapefile> sf = GetShapefile(layerHandle);
		if (sf) 
		{
			VARIANT_BOOL editing;
			sf->get_InteractiveEditing(&editing);
			if (!editing) return;

			long numSelected;
			sf->get_NumSelected(&numSelected);
			if (numSelected > 0)
			{
				SetCapture();
				_dragging.LayerHandle = layerHandle;
				_dragging.Operation = m_cursorMode == cmMoveShapes ? DragMoveShapes : DragRotateShapes;
				_dragging.InitAngle = GetDraggingRotationAngle(_dragging.Start.x, _dragging.Start.y);
			}
		}
	}
}

// ************************************************************
//		RegisterGroupOperation
// ************************************************************
void CMapView::RegisterGroupOperation(DraggingOperation operation)
{
	if (operation != DragMoveShapes && operation != DragRotateShapes)
		return;

	int layerHandle = _dragging.LayerHandle;
	IShapefile* source = GetShapefile(layerHandle);
	if (source) 
	{
		vector<int>* selection = ((CShapefile*)source)->GetSelectedIndices();
		if (!selection) return;

		bool added = false;
		switch (operation) 
		{
			case DragMoveShapes:
			{
				Point2D pnt = GetDraggingProjOffset();
				added = ((CUndoList*)_undoList)->AddMoveOperation(_dragging.LayerHandle, selection, -pnt.x, -pnt.y);
				break;
			}
			case DragRotateShapes:
			{
				double angle = GetDraggingRotationAngle();
				added = ((CUndoList*)_undoList)->AddRotateOperation(_dragging.LayerHandle, selection, 
						_dragging.RotateCenter.x, _dragging.RotateCenter.y, angle);
				break;
			}
		}
		if (!added) delete selection;
	}
}

// ***************************************************************
//	GetDraggingProjOffset
// ***************************************************************
Point2D CMapView::GetDraggingProjOffset()
{
	double x1, x2, y1, y2;
	PixelToProj(_dragging.Start.x, _dragging.Start.y, &x1, &y1);
	PixelToProj(_dragging.Move.x, _dragging.Move.y, &x2, &y2);
	return Point2D(x2 - x1, y2 - y1);
}

// ***************************************************************
//	InitRotationTool
// ***************************************************************
bool CMapView::InitRotationTool()
{
	long layerHandle = -1;
	tkMwBoolean cancel = blnFalse;
	FireChooseLayer(0, 0, &layerHandle, &cancel);

	bool success = false;
	if (layerHandle != -1)
	{
		IShapefile* sf = GetShapefile(layerHandle);
		if (sf) 
		{
			VARIANT_BOOL editing;
			sf->get_InteractiveEditing(&editing);
			if (editing)
			{
				long numSelected;
				sf->get_NumSelected(&numSelected);
				if (numSelected > 0)
				{
					double xMin, yMin, xMax, yMax;
					if (((CShapefile*)sf)->GetSelectedExtents(xMin, yMin, xMax, yMax))
					{
						_dragging.RotateCenter.x = (xMax + xMin) / 2.0;
						_dragging.RotateCenter.y = (yMax + yMin) / 2.0;
						success = true;
					}
				}
				sf->Release();
			}
		}
	}
	return success;
}

// ***************************************************************
//	InitDraggingShapefile
// ***************************************************************
bool CMapView::InitDraggingShapefile()
{
	if (!_dragging.Shapefile)
	{
		IShapefile* sf = GetShapefile(_dragging.LayerHandle);
		if (sf)
		{
			IShapefile* sfNew = ((CShapefile*)sf)->CloneSelection();
			ShpfileType shpType;
			sf->get_ShapefileType(&shpType);
			shpType = Utility::ShapeTypeConvert2D(shpType);
			if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
			{
				CComPtr<IShapeDrawingOptions> options = NULL;
				sf->get_DefaultDrawingOptions(&options);
				if (options) {
					CComPtr<IShapeDrawingOptions> newOptions = NULL;
					options->Clone(&newOptions);
					sfNew->put_DefaultDrawingOptions(newOptions);
				}
			}
			_dragging.SetShapefile(sfNew);
			sf->Release();
			return true;
		}
	}
	return false;
}

// ***************************************************************
//	GetDraggingRotationAngle
// ***************************************************************
double CMapView::GetDraggingRotationAngle()
{
	return GetDraggingRotationAngle(_dragging.Move.x, _dragging.Move.y) - _dragging.InitAngle;
}
double CMapView::GetDraggingRotationAngle(long screenX, long screenY)
{
	double x, y;
	PixelToProj(screenX, screenY, &x, &y);
	x -= _dragging.RotateCenter.x;
	y -= _dragging.RotateCenter.y;
	double angle = GeometryHelper::GetPointAngle(x, y);
	return angle / pi_ * 180.0;
}