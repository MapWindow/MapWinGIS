#include "stdafx.h"
#include "Map.h"
#include "ShapefileHelper.h"
#include "EditorHelper.h"
#include "ShapeStyleHelper.h"
#include "SelectionHelper.h"

// ************************************************************
//		GetProjectedTolerance
// ************************************************************
double CMapView::GetProjectedTolerance(double xScreen, double yScreen, double tolerance)
{
	double x, y, x2, y2;
	this->PixelToProjection(xScreen, yScreen, x, y);
	this->PixelToProjection(xScreen + tolerance, yScreen + tolerance, x2, y2);
	return sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0));
}

// ************************************************************
//		FindSnapPointCore
// ************************************************************
VARIANT_BOOL CMapView::FindSnapPointCore(double xScreen, double yScreen, double* xFound, double* yFound)
{
	return FindSnapPoint(GetMouseTolerance(ToleranceSnap, false), xScreen, yScreen, xFound, yFound);
}

// ************************************************************
//		FindSnapPoint
// ************************************************************
VARIANT_BOOL CMapView::FindSnapPoint(double tolerance, double xScreen, double yScreen, double* xFound, double* yFound)
{
    // Get the projected coordinates of the screen coordinates:
    double x, y;
    this->PixelToProjection(xScreen, yScreen, x, y);

    // Initialize max distance variables
    double maxDist = GetProjectedTolerance(xScreen, yScreen, tolerance);

    // Outcome of this method
    VARIANT_BOOL result = VARIANT_FALSE;

    // Initialize temp variables
    double minDistance = DBL_MAX;
    double xF, yF;

    // Fire the snap point requested event
    // this allows users to override the snapping behaviour
    tkMwBoolean isFinal = blnFalse, isFound = blnFalse;
    FireSnapPointRequested(x, y, &xF, &yF, &isFound, &isFinal);

    if (isFound) {
        
        if (isFinal) { // Trust the event consumer knows what (s)he's doing...
            *xFound = xF;
            *yFound = yF;
            result = VARIANT_TRUE;
        } else { // Check if the snap point returned by the end-user is valid:
            double distance = sqrt(pow(x - xF, 2.0) + pow(y - yF, 2.0));
            if (distance <= maxDist)
            {
                minDistance = distance;
                *xFound = xF;
                *yFound = yF;
                result = VARIANT_TRUE;
            }
        }
    }

    // Try to find a better snap point if not yet final:
    if (!isFinal)
        result = DefaultSnappingAlgorithm(maxDist, minDistance, x, y, xFound, yFound);

    // Fire the snap point found event
    // this allows users to modify/process the found snapped point
    FireSnapPointFound(x, y, xFound, yFound);

	return result;
}

// ************************************************************
//		DefaultSnappingAlgorithm
// ************************************************************
VARIANT_BOOL CMapView::DefaultSnappingAlgorithm(double maxDist, double minDist, double x, double y, double *xFound, double *yFound)
{
    VARIANT_BOOL result = VARIANT_FALSE;

    // Determine which layer(s) to snap to:
    bool digitizing = EditorHelper::IsDigitizingCursor((tkCursorMode)m_cursorMode);
    tkLayerSelection behavior;
    _shapeEditor->get_SnapBehavior(&behavior);
    tkSnapMode mode;
    _shapeEditor->get_SnapMode(&mode);
    long currentHandle = -1;
    bool currentLayerOnly = behavior == lsActiveLayer && digitizing;
    if (currentLayerOnly)
        _shapeEditor->get_LayerHandle(&currentHandle);

    // Loop over layers & search snap points
    for (long i = 0; i < this->GetNumLayers(); i++)
    {
        // Get layer handle
        long layerHandle = this->GetLayerHandle(i);
        if (currentLayerOnly && layerHandle != currentHandle)
            continue;

        // Get layer & check if it was rendered
        Layer* l = GetLayer(layerHandle);
        if (!l || !l->wasRendered) 
            continue;

        // Get shapefile
        CComPtr<IShapefile> sf = NULL;
        sf.Attach(this->GetShapefile(layerHandle));
        if (!sf)
            continue;
        
        // Check if shapefile is snappable
        VARIANT_BOOL snappable;
        sf->get_Snappable(&snappable);
        if (!snappable)
            continue;
        
        // stuff we need
        long shapeIndex;
        long pointIndex;
        double distance;
        VARIANT_BOOL vb;

        // Try snapping to a vertex first:
        bool snapped = false;   
        double xF, yF;
        if (mode == smVertices || mode == smVerticesAndLines) {
            sf->GetClosestVertex(x, y, maxDist, &shapeIndex, &pointIndex, &distance, &vb);
            if (vb && distance < minDist)
            {
                IShape* shape = NULL;
                sf->get_Shape(shapeIndex, &shape);
                if (shape)
                {
                    minDist = distance;
                    shape->get_XY(pointIndex, &xF, &yF, &vb);
                    shape->Release();
                    snapped = true;
                }
            }
        }

        // If not snapped to a vertex, maybe try to snap to a segment:
        if (!snapped && (mode == smVerticesAndLines || mode == smLines)) {
            sf->GetClosestSnapPosition(x, y, maxDist, &shapeIndex, &xF, &yF, &distance, &vb);
            if (vb && distance < minDist)
            {
                minDist = distance;
                snapped = true;
            }
        }

        // If correctly snapped, set the result
        if (snapped) {
            *xFound = xF;
            *yFound = yF;
            result = VARIANT_TRUE;
        }
    }

    return result;
}

// ************************************************************
//		SelectLayerHandles
// ************************************************************
bool CMapView::SelectLayerHandles(LayerSelector selector, std::vector<int>& layers)
{
	if (selector == slctIdentify)
	{
		// for single layer identification, request the selected layer from the client
		tkIdentifierMode mode;
		_identifier->get_IdentifierMode(&mode);
		if (mode == imSingleLayer)
		{
			long layerHandle = -1;
			FireChooseLayer(0, 0, &layerHandle);
			if (layerHandle != -1)
			{
				Layer* layer = GetLayer(layerHandle);
				if (layer && layer->wasRendered)
				{
					layers.push_back(layerHandle);
				}
				return true;
			}
			return false;
		}
	}

	IShapefile * sf = NULL;
	for (int i = 0; i < (int)_activeLayers.size(); i++)
	{
		int handle = GetLayerHandle(i);
		bool result = CheckLayer(selector, handle);
		if (result) {
			layers.push_back(handle);
		}
	}
	return layers.size() > 0;
}

// ************************************************************
//		LayerIsIdentifiable
// ************************************************************
VARIANT_BOOL CMapView::LayerIsIdentifiable(long layerHandle, IShapefile* sf)
{
	VARIANT_BOOL result = VARIANT_FALSE;
	tkIdentifierMode mode;
	_identifier->get_IdentifierMode(&mode);
	sf->get_Identifiable(&result);
	return result;
}

// ************************************************************
//		CheckShapefileLayer
// ************************************************************
bool CMapView::CheckShapefileLayer(LayerSelector selector, int layerHandle, IShapefile* sf)
{
	VARIANT_BOOL result = VARIANT_FALSE;
	switch (selector)
	{
		case slctShapefiles:
			return true;
		case slctIdentify:
			result = LayerIsIdentifiable(layerHandle, sf);
			break;
		case slctHotTracking:
			if (m_cursorMode == cmIdentify)
			{
				VARIANT_BOOL hotTracking;
				_identifier->get_HotTracking(&hotTracking);
				if (hotTracking)
				{
					result = LayerIsIdentifiable(layerHandle, sf);
				}
			}
			else
			{
				VARIANT_BOOL interactive;
				sf->get_InteractiveEditing(&interactive);

				tkLayerSelection highlighting;
				_shapeEditor->get_HighlightVertices(&highlighting);

				bool editorEmpty = EditorHelper::IsEmpty(_shapeEditor);
				bool editing = m_cursorMode == cmEditShape && !editorEmpty;
				bool selection = m_cursorMode == cmEditShape && editorEmpty;

				if (selection)
				{
					// we are looking for new shape to select which can be at any editable layer
					if (interactive && highlighting != lsNoLayer)
						result = VARIANT_TRUE;
				}
				else if ((!editorEmpty && m_cursorMode == cmEditShape) ||
					EditorHelper::IsDigitizingCursor((tkCursorMode)m_cursorMode))
				{
					// highlight vertices for easier snapping
					switch (highlighting) {
						case lsAllLayers:
							return true;
						case lsActiveLayer:
							long handle;
							_shapeEditor->get_LayerHandle(&handle);
							return handle == layerHandle;
						case lsNoLayer:
							return false;
					}
				}
			}
			break;
		case slctInMemorySf:
			sf->get_EditingShapes(&result);
			break;
		case slctInteractiveEditing:
			sf->get_InteractiveEditing(&result);
			break;
	}
	return result ? true : false;
}

// ************************************************************
//		CheckLayer
// ************************************************************
bool CMapView::CheckLayer(LayerSelector selector, int layerHandle)
{
	CComPtr<IShapefile> sf = NULL;
	Layer* layer = GetLayer(layerHandle);

	if (!layer || !layer->wasRendered) return false;

	if (layer->IsShapefile())
	{
		if (layer->QueryShapefile(&sf))
		{
			return CheckShapefileLayer(selector, layerHandle, sf);
		}
	}
	else if (layer->IsImage())
	{
		// it's raster		
		switch (selector)
		{
			case slctIdentify:
				return true;			// TODO: check layer is identifiable
		}

		return false;
	}

	return false;
}

// ************************************************************
//		GetMouseTolerance
// ************************************************************
// Mouse tolerance in projected units
double CMapView::GetMouseTolerance(MouseTolerance tolerance, bool proj)
{
	double tol = 0;
	switch (tolerance)
	{
		case ToleranceSnap:
			_shapeEditor->get_SnapTolerance(&tol);
			break;
		case ToleranceSelect:
			tol = m_globalSettings.mouseTolerance;
			break;
		case ToleranceInsert:
			tol = 10;
			break;
	}

	if (proj) 
	{
		tol /= this->PixelsPerMapUnit();
	}

	return tol;
}

// ************************************************************
//		DrillDownSelect
// ************************************************************
bool CMapView::DrillDownSelect(double projX, double projY, long& layerHandle, long& shapeIndex)
{
	vector<int> handles;
	SelectLayerHandles(slctIdentify, handles);
	for (int i = handles.size() - 1; i >= 0; i--)
	{
		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(handles[i]));
		if (sf) {
			Extent box = GetPointSelectionBox(sf, projX, projY);

			if (SelectionHelper::SelectSingleShape(sf, box, shapeIndex))
			{
				layerHandle = handles[i];
				return true;
			}
		}
	}
	return false;
}

// ************************************************************
//		DrillDownSelect
// ************************************************************
bool CMapView::DrillDownSelect(double projX, double projY, ISelectionList* list, bool stopOnFirst, bool ctrl)
{
	vector<int> handles;
	SelectLayerHandles(slctIdentify, handles);

	vector<long> results;

	for (int i = handles.size() - 1; i >= 0; i--)
	{
		Layer* layer = GetLayer(handles[i]);

		if (!layer)
		{
			CallbackHelper::AssertionFailed("Invalid layer handler in DrillDownSelect.");
			continue;
		}

		// ************************************************************************************
		// jfaust: until tolerance is determined/implemented, I am commenting out this pre-test
		// since it often results in the exclusion of points that define the edge of the extent
		//if (!layer->PointWithinExtents(projX, projY))		// TODO: use tolerance
		//{
		//	continue;
		//}
		// ************************************************************************************

		if (layer->IsShapefile())
		{
			CComPtr<IShapefile> sf = NULL;
			sf.Attach(GetShapefile(handles[i]));

			if (sf)
			{
				Extent box = GetPointSelectionBox(sf, projX, projY);

				results.clear();

				if (SelectionHelper::SelectShapes(sf, box, SelectMode::INTERSECTION, results))
				{
					for (size_t j = 0; j < results.size(); j++)
					{
						VARIANT_BOOL visible;
						sf->get_ShapeVisible(results[j], &visible);

						if (visible)
						{
							list->AddShape(handles[i], results[j]);
						}
					}

					if (results.size() > 0 && stopOnFirst) {
						return true;
					}
				}
			}
		}
		else if (layer->IsImage())
		{
			CComPtr<IImage> img = NULL;
			img.Attach(GetImage(handles[i]));
			if (img)
			{
				long row, column;
				img->ProjectionToImage(projX, projY, &column, &row);
				
				if (ctrl) {
					list->TogglePixel(handles[i], column, row);
				}
				else {
					list->AddPixel(handles[i], column, row);
				}
			}
		}
	}

	long numLayers;
	list->get_Count(&numLayers);
	return numLayers > 0;
}

// ************************************************************
//		FindShapeAtScreenPoint
// ************************************************************
LayerShape CMapView::FindShapeAtScreenPoint(CPoint point, LayerSelector selector)
{
	double pixX = point.x;
	double pixY = point.y;
	double prjX, prjY;
	this->PixelToProj(pixX, pixY, &prjX, &prjY);

	std::vector<int> layers;
	if (SelectLayerHandles(selector, layers))
	{
		return FindShapeAtProjPoint(prjX, prjY, layers);
	}
	return LayerShape();
}

// ************************************************************
//		FindShapeCore
// ************************************************************
LayerShape CMapView::FindShapeAtProjPoint(double prjX, double prjY, std::vector<int>& layers)
{
	IShapefile * sf = NULL;
	for (int i = (int)layers.size() - 1; i >= 0; i--)
	{
		CComPtr<IShapefile> sf = NULL;
		sf.Attach(GetShapefile(layers[i]));
		if (sf) {
			double tol = 0.0;
			ShpfileType type = ShapefileHelper::GetShapeType2D(sf);
			if (type == SHP_MULTIPOINT || type == SHP_POINT || type == SHP_POLYLINE)
				tol = GetMouseTolerance(ToleranceSelect);

			SelectMode mode = type == SHP_POLYGON ? INCLUSION : INTERSECTION;

			long shapeIndex;
			if (SelectionHelper::SelectSingleShape(sf, Extent(prjX, prjY, tol), mode, shapeIndex)) {
				return LayerShape(layers[i], shapeIndex);
			}
		}
	}
	return LayerShape();
}

// ************************************************************
//		SelectShapeForEditing
// ************************************************************
bool CMapView::SelectShapeForEditing(int x, int y, long& layerHandle, long& shapeIndex)
{
	LayerShape info = FindShapeAtScreenPoint(CPoint(x, y), slctInteractiveEditing);
	if (!info.IsEmpty())
	{
		tkMwBoolean cancel = blnFalse;
		FireBeforeShapeEdit(info.LayerHandle, info.ShapeIndex, &cancel);
		layerHandle = info.LayerHandle;
		shapeIndex = info.ShapeIndex;
		return cancel == blnFalse;
	}
	return false;
}

// ************************************************************
//		RecalcHotTracking
// ************************************************************
HotTrackingResult CMapView::RecalcHotTracking(CPoint point, LayerShape& result)
{
	bool cursorCheck = EditorHelper::IsSnappableCursor((tkCursorMode)m_cursorMode) || m_cursorMode == cmEditShape || m_cursorMode == cmIdentify;
	if (!cursorCheck) return NoShape;

	if (_shapeCountInView < m_globalSettings.hotTrackingMaxShapeCount && HasHotTracking())
	{
		result = FindShapeAtScreenPoint(point, slctHotTracking);
		if (!result.IsEmpty()) {
			return _hotTracking.IsSame(result) ? SameShape : NewShape;
		}
	}
	return HotTrackingResult::NoShape;
}

// ************************************************************
//		ClearHotTracking
// ************************************************************
void CMapView::ClearHotTracking()
{
	if (!_hotTracking.IsEmpty())
	{
		_canUseMainBuffer = false;
		_hotTracking.Clear();
		this->FireShapeHighlighted(-1, -1);
	}
}

// ************************************************************
//		UpdateHotTracking
// ************************************************************
void CMapView::UpdateHotTracking(LayerShape info, bool fireEvent)
{
	CComPtr<IShapefile> sf = NULL;
	sf.Attach(GetShapefile(info.LayerHandle));
	if (sf) {
		CComPtr<IShape> shape = NULL;
		sf->get_Shape(info.ShapeIndex, &shape);
		if (shape)
		{
			CComPtr<IShape> shpClone = NULL;
			shape->Clone(&shpClone);
			_hotTracking.Update(sf, shpClone, info.LayerHandle, info.ShapeIndex);

			OLE_COLOR color;
			_identifier->get_OutlineColor(&color);

			CComPtr<IShapeDrawingOptions> options = NULL;
			options.Attach(ShapeStyleHelper::GetHotTrackingStyle(sf, color, m_cursorMode == cmIdentify));
			if (options) {
				_hotTracking.UpdateStyle(options);
			}

			if (fireEvent)
				this->FireShapeHighlighted(_hotTracking.LayerHandle, _hotTracking.ShapeIndex);
			_canUseMainBuffer = false;
		}
	}
}