#include "stdafx.h"
#include "Map.h"
#include "Shapefile.h"
#include "ShapeEditor.h"
#include "UndoList.h"
#include "GeometryHelper.h"
#include "Shape.h"
#include "ShapefileHelper.h"
#include "GroupOperation.h"
#include "ShapeHelper.h"
#include "SelectionHelper.h"
#include "EditorHelper.h"
#include "Digitizer.h"

// ************************************************************
//		HandleLeftButtonUpDragVertexOrShape
// ************************************************************
bool CMapView::HandleLButtonUpDragVertexOrShape(UINT nFlags)
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
		bool shift = (nFlags & MK_SHIFT) != 0;
		if (SnappingIsOn(shift))
		{
			VARIANT_BOOL result = this->FindSnapPointCore(_dragging.Move.x, _dragging.Move.y, &x2, &y2);
			if (!result && shift)
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
bool CMapView::SnappingIsOn(UINT flags)
{
	return SnappingIsOn((flags & MK_SHIFT) != 0);
}

bool CMapView::SnappingIsOn(bool shift)
{
	if (EditorHelper::IsSnappableCursor((tkCursorMode)m_cursorMode)) 
	{
		tkLayerSelection behavior = EditorHelper::GetSnappingBehavior(_shapeEditor);
		if (behavior == lsAllLayers || behavior == lsCurrentLayer) return true;
	}
	return shift;
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
		tkLayerSelection behavior;
		_shapeEditor->get_SnapBehavior(&behavior);
		if (behavior == lsAllLayers && _dragging.Operation == DragMoveVertex)
		{
			double xFound, yFound;
			if (this->FindSnapPointCore(x, y, &xFound, &yFound)) {
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
		tkEditorBehavior behavior;
		_shapeEditor->get_EditorBehavior(&behavior);
		EditorBase* base = GetEditorBase();
		bool handled = false;
		double projX, projY;
		this->PixelToProjection(x, y, projX, projY);

		// highlighting of vertices
		if (behavior == ebVertexEditor)
		{
			if (base->ClearHighlightedPart())
				_canUseMainBuffer = false;

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
		}
		
		// highlighting parts
		if (behavior == ebPartEditor)	//if (nFlags & MK_CONTROL) {
		{
			if (base->ClearHighlightedVertex())
				_canUseMainBuffer = false;

			int partIndex = _shapeEditor->GetClosestPart(projX, projY, GetMouseTolerance(ToleranceSelect));
			if (partIndex != -1) 
			{
				if (base->SetHighlightedPart(partIndex)) {
					_canUseMainBuffer = false;
				}
				return true;
			}

			if (base->ClearHighlightedPart()){
				_canUseMainBuffer = false;
				handled = true;
			}
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
//		HandleOnLButtonMoveOrRotate
// ************************************************************
void CMapView::HandleOnLButtonMoveOrRotate(long x, long y)
{
	long layerHandle = -1;
	FireChooseLayer(x, y, &layerHandle);
	if (layerHandle == -1) return;
	
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(GetShapefile(layerHandle));
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
		vector<int>* selection = ShapefileHelper::GetSelectedIndices(source);
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
	FireChooseLayer(0, 0, &layerHandle);
	if (layerHandle == -1) return false;

	bool success = false;
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(GetShapefile(layerHandle));
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
				if (ShapefileHelper::GetSelectedExtents(sf, xMin, yMin, xMax, yMax))
				{
					_dragging.RotateCenter.x = (xMax + xMin) / 2.0;
					_dragging.RotateCenter.y = (yMax + yMin) / 2.0;
					success = true;
				}
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
		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(_dragging.LayerHandle));
		if (sf)
		{
			IShapefile* sfNew = ShapefileHelper::CloneSelection(sf);
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

// ***************************************************************
//	_UnboundShapeFinished
// ***************************************************************
void CMapView::_UnboundShapeFinished(IShape* shp)
{
	if (!shp) return;

	_shapeEditor->Clear();
	_shapeEditor->StartUnboundShape((tkCursorMode)m_cursorMode);

	long layerHandle = -1;
	FireChooseLayer(0, 0, &layerHandle);
	if (layerHandle == -1) return;

	CComPtr<IShapefile> sf = NULL;
	sf.Attach(GetShapefile(layerHandle));
	if (!sf) {
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return;
	}

	bool editing = m_cursorMode == cmSplitByPolyline ||
		m_cursorMode == cmSplitByPolygon ||
		m_cursorMode == cmEraseByPolygon ||
		m_cursorMode == cmClipByPolygon;

	if (editing)
	{
		VARIANT_BOOL editing;
		sf->get_InteractiveEditing(&editing);
		if (!editing) {
			ErrorMessage(tkSHPFILE_NOT_IN_EDIT_MODE);
			return;
		}
	}

	ShpfileType shpType;
	sf->get_ShapefileType2D(&shpType);
	if (m_cursorMode == cmSplitByPolyline && shpType != SHP_POLYGON && shpType != SHP_POLYLINE)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return;
	}

	bool redrawNeeded = false;
	int errorCode = tkNO_ERROR;

	if (m_cursorMode == cmSelectByPolygon)
	{
		SelectionHelper::SelectByPolygon(sf, shp, errorCode);
		FireSelectionChanged(layerHandle);
		redrawNeeded = true;
	}
	else {
		redrawNeeded = GroupOperation::Run((tkCursorMode)m_cursorMode, layerHandle, sf, shp, _undoList, errorCode);
	}

	if (errorCode != tkNO_ERROR) {
		ErrorMessage(errorCode);
	}

	if (redrawNeeded)
		Redraw();
}

// ***************************************************************
//	StartNewBoundShape
// ***************************************************************
bool CMapView::StartNewBoundShape(long x, long y)
{
	if (m_cursorMode == cmAddShape && EditorHelper::IsEmpty(_shapeEditor))
	{
		long layerHandle = -1;
		FireChooseLayer(x, y, &layerHandle);
		if (layerHandle == -1) return false;

		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(layerHandle));
		if (!sf) {
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return false;
		}
		else {
			Digitizer::StartNewBoundShape(_shapeEditor, sf, layerHandle);
			return true;
		}
		return false;
	}
	return true;   // no need to choose
}
