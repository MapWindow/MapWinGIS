#include "stdafx.h"
#include "Map.h"
#include "Shapefile.h"

// ************************************************************
//		UndoEdit
// ************************************************************
VARIANT_BOOL CMapView::UndoEdit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL vb;
	_editShape->Undo(&vb);
	if (vb) return vb;

	_editShape->Clear();
	return RunShapefileUndoList(true) ?  VARIANT_TRUE: VARIANT_FALSE;
}

// ************************************************************
//		RedoEdit
// ************************************************************
VARIANT_BOOL CMapView::RedoEdit()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL vb;
	_editShape->Redo(&vb);
	if (vb) return vb;

	_editShape->Clear();
	return RunShapefileUndoList(false) ? VARIANT_TRUE : VARIANT_FALSE;
}

// ************************************************************
//		RunShapefileUndoList
// ************************************************************
bool CMapView::RunShapefileUndoList(bool undo)
{
	vector<int> layers;
	SelectLayerHandles(slctInteractiveEditing, layers);
	
	int layerHandle = -1;
	int maxId = -1;
	for (int i = 0; i < layers.size(); i++) 
	{
		CComPtr<IShapefile> sf = GetShapefile(layers[i]);
		if (sf) 
		{
			CComPtr<IUndoList> list;
			sf->get_UndoList(&list);
			if (list) 
			{
				long id = -1;
				list->GetLastId(&id);
				if (id > maxId) {
					maxId = id;
					layerHandle = layers[i];
				}
			}
		}
	}

	if (layerHandle != -1) 
	{
		CComPtr<IShapefile> sf = GetShapefile(layerHandle);
		CComPtr<IUndoList> list;
		sf->get_UndoList(&list);

		VARIANT_BOOL vb;
		if (undo) {
			list->Undo(&vb);
		}
		else {
			list->Redo(&vb);
		}
		return true;
	}
	return false;
}

// ************************************************************
//		GetEditShapeShapefile
// ************************************************************
IShapefile* CMapView::GetEditShapeShapefile()
{
	int layerHandle;
	_editShape->get_LayerHandle(&layerHandle);
	return GetShapefile(layerHandle);
}

// ************************************************************
//		MoveShapeVertex
// ************************************************************
void CMapView::MoveShapeVertex(int offsetX, int offsetY)
{
	if (offsetX == 0 && offsetY == 0)
		return;

	// register with undo list
	CComPtr<IShapefile> sf = GetEditShapeShapefile();
	if (sf) {
		CComPtr<IUndoList> undoList = NULL;
		sf->get_UndoList(&undoList);

		CComPtr<IShape> shp = NULL;
		_editShape->get_RawData(&shp);

		int shapeIndex;
		_editShape->get_ShapeIndex(&shapeIndex);

		VARIANT_BOOL vb;
		undoList->AddSubOperation(uoSubEditShape, (long)shapeIndex,  shp, &vb);

		GetEditShapeBase()->MoveVertex(offsetX, offsetY);
	}
}

// ************************************************************
//		HandleOnLButtonDownEditShape
// ************************************************************
void CMapView::HandleOnLButtonDownEditShape(int x, int y, bool ctrl)
{
	long layerHandle = -1, shapeIndex = -1;
	bool handled = false;
	bool hasEditShape = !GetEditShapeBase()->IsEmpty();
	
	if (hasEditShape)
	{
		IShapefile* sf = GetTempShapefile();
		if (sf != NULL)
		{
			IShape* shp = NULL;
			_editShape->get_Shape(VARIANT_TRUE, &shp);

			double projX, projY;
			PixelToProj(x, y, &projX, &projY);

			if (m_cursorMode == cmMoveShape)
			{
				if (((CShapefile*)sf)->PointWithinShape(shp, projX, projY, GetMouseTolerance(ToleranceSelect)))
				{
					this->SetCapture();
					_dragging.Operation = DragMoveShape;
					handled = true;
				}
			}
			else if (m_cursorMode == cmEditShape)
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
					EditShapeBase* base = GetEditShapeBase();
					int pntIndex = base->SelectVertex(projX, projY, GetMouseTolerance(ToleranceSelect));
					if (pntIndex != -1)
					{
						bool changed = base->_selectedVertex != pntIndex;
						base->_selectedVertex = pntIndex;

						this->SetCapture();
						_dragging.Operation = DragMoveVertex;
						handled = true;

						if (changed)
							RedrawCore(tkRedrawType::RedrawSkipDataLayers, false, true);
					}
				}
			}
		}
	}
	
	if (handled)
		return;

	if (hasEditShape)
	{
		if (!TrySaveEditShape())
			return;
	}
	
	ClearEditShape();

	if (SelectSingleShape(x, y, layerHandle, shapeIndex))
		SetEditShape(layerHandle, shapeIndex);

	RedrawCore(RedrawAll, false, false);
}

// ************************************************************
//		ClearEditShape
// ************************************************************
void CMapView::ClearEditShape() 
{
	int layerHandle, shapeIndex;
	_editShape->get_LayerHandle(&layerHandle);
	_editShape->get_ShapeIndex(&shapeIndex);
	if (shapeIndex != -1) {
		CComPtr<IShapefile> sf = GetShapefile(layerHandle);
		if (sf) {
			sf->put_ShapeVisible(shapeIndex, VARIANT_TRUE);
		}
	}
	_editShape->Clear();
}

// ************************************************************
//		HandleOnLButtonShapeAddMode
// ************************************************************
void CMapView::HandleOnLButtonShapeAddMode(int x, int y, double projX, double projY, bool ctrl)
{
	EditShapeBase* editShape = GetEditShapeBase();
	editShape->SetCreationMode(true);

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
		succeed = TrySaveEditShape();
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
		succeed = TrySaveEditShape();
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

	SetEditShape(layerHandle);
	
	return true;
}

// ************************************************************
//		SetEditShape
// ************************************************************
// the case of new shape
bool CMapView::SetEditShape(long layerHandle)
{
	CComPtr<IShapefile> sf = GetShapefile(layerHandle);
	if (!sf) {
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return false;
	}

	ShpfileType shpType;
	sf->get_ShapefileType(&shpType);

	_editShape->Clear();
	_editShape->put_ShapeType(shpType);
	_editShape->put_LayerHandle(layerHandle);
	
	CComPtr<IShapeDrawingOptions> options = NULL;
	sf->get_DefaultDrawingOptions(&options);
	_editShape->CopyOptionsFrom(options);
}

// ************************************************************
//		SetEditShape
// ************************************************************
// the case of exiting shape
void CMapView::SetEditShape(long layerHandle, long shapeIndex)
{
	if (layerHandle != -1 && shapeIndex != -1)
	{
		CComPtr<IShapefile> sf = GetShapefile(layerHandle);
		if (sf)
		{
			CComPtr<IShape> shp = NULL;
			sf->get_Shape(shapeIndex, &shp);
			if (shp)
			{
				_editShape->SetShape(shp);
				_editShape->put_LayerHandle(layerHandle);
				_editShape->put_ShapeIndex(shapeIndex);
				sf->put_ShapeVisible(shapeIndex, VARIANT_FALSE);

				CComPtr<IShapeDrawingOptions> options = NULL;
				sf->get_DefaultDrawingOptions(&options);
				_editShape->CopyOptionsFrom(options);
			}
		}
	}
}

// ************************************************************
//		TrySaveEditShape
// ************************************************************
bool CMapView::TrySaveEditShape()
{
	int layerHandle, shapeIndex;
	_editShape->get_LayerHandle(&layerHandle);
	_editShape->get_ShapeIndex(&shapeIndex);
	bool newShape = shapeIndex == -1;

	// first check if it's enough points
	VARIANT_BOOL enoughPoints;
	_editShape->get_HasEnoughPoints(&enoughPoints);
	if (!enoughPoints) {
		FireValidationResults(VARIANT_FALSE, "The shape doesn't have enough points");		// TODO: localize
		return false;
	}

	CComPtr<IShape> shp = NULL;
	VARIANT_BOOL valid = VARIANT_TRUE;
	_editShape->get_Shape(VARIANT_FALSE, &shp);
	
	// does user want to validate with GEOS?
	tkMwBoolean geosCheck = blnTrue, tryFix = blnTrue;
	FireValidationMode(&geosCheck, &tryFix);
	
	// if so validate and optionally fix
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

	VARIANT_BOOL vb;
	if (newShape)    // TODO: perhaps register add editing as well
	{
		CComPtr<IUndoList> undoList = NULL;
		sf->get_UndoList(&undoList);

		// register with undo list
		undoList->Add(newShape ? uoAddShape : uoEditShape, (long)shapeIndex, &vb);
	}

	// add new shape
	if (newShape) {
		long numShapes;
		sf->get_NumShapes(&numShapes);
		sf->EditInsertShape(shp, &numShapes, &vb);
		shapeIndex = numShapes;
	}
	else {
		sf->EditUpdateShape(shapeIndex, shp, &vb);
	}

	ClearEditShape();

	// let the user set new attributes
	FireAfterShapeEdit((tkCursorMode)m_cursorMode, layerHandle, shapeIndex);
	return true;
}

// ************************************************************
//		DoFireAfterShapeEdit
// ************************************************************
void CMapView::DoFireAfterShapeEdit()
{
	int layerHandle, shapeIndex;
	_editShape->get_LayerHandle(&layerHandle);
	_editShape->get_ShapeIndex(&shapeIndex);
	FireAfterShapeEdit((tkCursorMode)m_cursorMode, layerHandle, shapeIndex);
}

// ************************************************************
//		TryAddVertex
// ************************************************************
bool CMapView::TryAddVertex(double projX, double projY)
{
	ShpfileType shpType;
	_editShape->get_ShapeType(&shpType);
	shpType = Utility::ShapeTypeConvert2D(shpType);

	if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
		return false;				// TODO: multi points should be supported

	bool success = false;
	IShape* shp = NULL;
	_editShape->get_Shape(VARIANT_TRUE, &shp);
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
		IShape* shp2 = NULL;
		GetUtils()->CreateInstance(idShape, (IDispatch**)&shp2);
		shp2->Create(SHP_POINT, &vb);
		shp2->AddPoint(projX, projY, &pointIndex);

		IShape* result = NULL;
		shp->ClosestPoints(shp2, &result);
		if (result != NULL)
		{
			double x, y;
			result->get_XY(0, &x, &y, &vb);		// 0 = point lying on the line

			double dist = sqrt(pow(x - projX, 2.0) + pow(y - projY, 2.0));
			if (dist < GetMouseTolerance(ToleranceSelect))
			{
				success = GetEditShapeBase()->TryInsertVertex(x, y);
			}
			result->Release();
		}
		shp->Release();
		shp2->Release();
	}
	return success;
}


