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
        bool alt = GetKeyState(VK_MENU) < 0 ? true : false;

		if (SnappingIsOn(shift))
		{
			VARIANT_BOOL result = this->FindSnapPointCore(_dragging.Move.x, _dragging.Move.y, &x2, &y2);
			if ((result == VARIANT_FALSE) && shift)
				return true;		// can't proceed without snapping in this mode
		}

        if (alt) { // user wants to intercept coordinates and possibly modify them
            this->FireBeforeVertexDigitized(&x2, &y2);
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
		if (behavior == lsAllLayers || behavior == lsActiveLayer) return true;
	}
	return shift;
}

// ************************************************************
//		HandleOnMouseMoveShapeEditor
// ************************************************************
bool CMapView::HandleOnMouseMoveShapeEditor(int x, int y, long nFlags)
{

	tkLayerSelection snapBehavior;
	// get Snap setting
	_shapeEditor->get_SnapBehavior(&snapBehavior);

	double projX, projY;
    VARIANT_BOOL snapped = FindSnapPointCore(x, y, &projX, &projY);
	if (!snapped) {
		PixelToProjection(x, y, projX, projY);
		GetEditorBase()->ClearSnapPoint();
	}
	else {
        double sX, sY;
        ProjToPixel(projX, projY, &sX, &sY);
        GetEditorBase()->SetSnapPoint(sX, sY);
    }
		

	if ((_dragging.Operation == DragMoveVertex || 
		 _dragging.Operation == DragMoveShape || 
		_dragging.Operation == DragMovePart))      // && (nFlags & MK_LBUTTON) && _leftButtonDown
	{
		_dragging.Snapped = false;
		if (snapBehavior == lsAllLayers && _dragging.Operation == DragMoveVertex && snapped)
			_dragging.SetSnapped(projX, projY);
		else
			GetEditorBase()->ClearSnapPoint();
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
		EditorBase* base = GetEditorBase();
		bool handled = false;

		tkEditorBehavior behavior;
		// get Editor setting
		_shapeEditor->get_EditorBehavior(&behavior);

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
	CComPtr<IShapefile> source = NULL;
	source.Attach(GetShapefile(layerHandle));
	if (source) 
	{
		vector<int>* selection = ShapefileHelper::GetSelectedIndices(source);
		if (!selection) return;

		// mark them for OGR saving
		for (size_t i = 0; i < selection->size(); i++)
		{
			source->put_ShapeModified((*selection)[i], VARIANT_TRUE);
		}

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
	if (!_dragging.Shapefile && _dragging.LayerHandle != -1)
	{
		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(_dragging.LayerHandle));
		if (sf)
		{
			IShapefile* sfNew = ShapefileHelper::CloneSelection(sf);
			ShpfileType shpType;
			sf->get_ShapefileType(&shpType);
			shpType = ShapeUtility::Convert2D(shpType);
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
    bool selectingSelectable = false;
    CComPtr<IShapefile> sf = NULL;

    FireChooseLayer(0, 0, &layerHandle);
    // if none specified, consider all 'selectable' layers
    if (layerHandle == -1)
    {
        // only in 'selection' mode is -1 acceptable
        if (m_cursorMode == cmSelectByPolygon)
        {
            // select based on all selectable layers
            selectingSelectable = true;
        }
        else
        {
            // any other mode, layerHandle must be specified
            return;
        }
    }
    else
    {
        // single layer specified for selection
        sf.Attach(GetShapefile(layerHandle));
        if (!sf)
        {
            ErrorMessage(tkINVALID_LAYER_HANDLE);
            return;
        }
    }

	bool editing = m_cursorMode == cmSplitByPolyline ||
		m_cursorMode == cmSplitByPolygon ||
		m_cursorMode == cmEraseByPolygon ||
		m_cursorMode == cmClipByPolygon;

	if (editing)
	{
		if (!ShapefileHelper::InteractiveEditing(sf))
		{
			ErrorMessage(tkNO_INTERACTIVE_EDITING);
			return;
		}
	}

    if (m_cursorMode == cmSplitByPolyline)
    {
        ShpfileType shpType;
        sf->get_ShapefileType2D(&shpType);
        if (shpType != SHP_POLYGON && shpType != SHP_POLYLINE)
        {
            ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
            return;
        }
    }

	bool redrawNeeded = false;
	int errorCode = tkNO_ERROR;

	if (m_cursorMode == cmSelectByPolygon)
	{
        // if single layer selection
        if (sf)
        {
            SelectionHelper::SelectByPolygon(sf, shp, errorCode);
            // selection has (likely) changed for this layer
            FireSelectionChanged(layerHandle);
            redrawNeeded = true;
        }
        else if (selectingSelectable)
        {
            // iterate all layers
            for (int layerPosition = 0; layerPosition < GetNumLayers(); layerPosition++)
            {
				layerHandle = GetLayerHandle(layerPosition);
                sf.Attach(GetShapefile(layerHandle));
                if (sf)
                {
                    // is layer selectable?
                    VARIANT_BOOL isSelectable = VARIANT_FALSE;
                    sf->get_Selectable(&isSelectable);
                    if (isSelectable == VARIANT_TRUE)
                    {
                        int tempError = tkNO_ERROR;
                        SelectionHelper::SelectByPolygon(sf, shp, tempError);
                        // save error code if any layer returns an error
                        if (tempError != tkNO_ERROR) errorCode = tempError;
                        // selection has changed for this layer
                        FireSelectionChanged(layerHandle);
                        redrawNeeded = true;
                    }
                }
            }
        }
    }
	else
    {
		redrawNeeded = GroupOperation::Run((tkCursorMode)m_cursorMode, layerHandle, sf, shp, _undoList, errorCode);
	}

	if (errorCode != tkNO_ERROR)
    {
		ErrorMessage(errorCode);
	}

	if (redrawNeeded)
		Redraw();
}

// ***************************************************************
//	StartNewBoundShape
// ***************************************************************
VARIANT_BOOL CMapView::StartNewBoundShape(DOUBLE x, DOUBLE y)
{
	if (m_cursorMode == cmAddShape && EditorHelper::IsEmpty(_shapeEditor))
	{
		long layerHandle = -1;
		FireEvent(eventidChooseLayer, EVENT_PARAM(VTS_R8 VTS_R8 VTS_PI4), x, y, &layerHandle);
		if (layerHandle == -1) 
			return VARIANT_FALSE;
		else
			return StartNewBoundShapeEx(layerHandle);
	}
	return VARIANT_TRUE;   // no need to choose
}

// ***************************************************************
//	StartNewBoundShape
// ***************************************************************
VARIANT_BOOL CMapView::StartNewBoundShapeEx(long LayerHandle)
{
	if (m_cursorMode == cmAddShape && EditorHelper::IsEmpty(_shapeEditor))
	{
		if (LayerHandle == -1) return VARIANT_FALSE;

		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(LayerHandle));
		if (!sf) {
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return VARIANT_FALSE;
		}

        if (!ShapefileHelper::InteractiveEditing(sf))
        {
            ErrorMessage(tkNO_INTERACTIVE_EDITING);
            return VARIANT_FALSE;
        }

        // Get shape type
        ShpfileType shpType;
        sf->get_ShapefileType(&shpType);

        // Get drawing options
        CComPtr<IShapeDrawingOptions> options = NULL;
        sf->get_DefaultDrawingOptions(&options);

        // Check if this is an OGR layer & try get active shape type
        CComPtr<IOgrLayer> ogrLayer = NULL;
        ogrLayer.Attach(GetOgrLayer(LayerHandle));
        if (ogrLayer != nullptr)
            ogrLayer->get_ActiveShapeType(&shpType);

		Digitizer::StartNewBoundShape(_shapeEditor, shpType, options, LayerHandle);
		return VARIANT_TRUE;
	}
	return VARIANT_TRUE;   // no need to choose
}
