// EditShape.cpp : Implementation of CEditShape

#include "stdafx.h"
#include "EditShape.h"
#include "Shape.h"
#include "map.h"

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CEditShape::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	USES_CONVERSION;
	if( _globalCallback != NULL && _lastErrorCode != tkNO_ERROR)
		_globalCallback->Error( OLE2BSTR(_key),  A2BSTR(ErrorMsg(_lastErrorCode)));
}


// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CEditShape::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CEditShape::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CEditShape::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();
	return S_OK;
}

// ************************************************************
//		put_GlobalCallback()
// ************************************************************
STDMETHODIMP CEditShape::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		Utility::put_ComReference(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}


// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CEditShape::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CEditShape::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

// *******************************************************
//		ShapeType()
// *******************************************************
STDMETHODIMP CEditShape::get_ShapeType(ShpfileType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->GetShapeType();
	return S_OK;
}

STDMETHODIMP CEditShape::put_ShapeType(ShpfileType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->SetShapeType(newVal);
	return S_OK;
}

// *******************************************************
//		RawData()
// *******************************************************
STDMETHODIMP CEditShape::get_RawData(IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	ShpfileType shpType = _editShape->GetShapeType();
	if (shpType == SHP_NULLSHAPE) return S_OK;

	GetUtils()->CreateInstance(idShape, (IDispatch**)retVal);

	VARIANT_BOOL vb;
	(*retVal)->Create(shpType, &vb);

	CopyData(0, _editShape->GetPointCount(), *retVal);

	return S_OK;
}

// *******************************************************
//		AsShape()
// *******************************************************
STDMETHODIMP CEditShape::get_Shape(VARIANT_BOOL geosFixup, IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	VARIANT_BOOL vb;
	get_HasEnoughPoints(&vb);
	if (!vb) {
		return S_OK;
	}

	get_RawData(retVal);
	ShpfileType shpType = _editShape->GetShapeType();

	// add the last point automatically
	if (shpType == SHP_POLYGON)
		((CShape*)(*retVal))->FixupShapeCore(ShapeValidityCheck::FirstAndLastPointOfPartMatch);

	if (geosFixup)
	{
		(*retVal)->get_IsValid(&vb);
		IShape* shpNew = NULL;
		if (!vb) {
			(*retVal)->FixUp(&shpNew);
			if (shpNew)
			{
				(*retVal)->Release();
				(*retVal) = shpNew;
			}
		}
	}
	return S_OK;
}

// *******************************************************
//		HasValidShape()
// *******************************************************
STDMETHODIMP CEditShape::get_HasEnoughPoints(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	ShpfileType type = Utility::ShapeTypeConvert2D(_editShape->GetShapeType());
	if (type == SHP_NULLSHAPE)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return S_FALSE;
	}

	int pointCount = _editShape->GetPointCount();
	if (pointCount == 0 ||
		(pointCount < 2 && type == SHP_POLYLINE) ||
		(pointCount < 3 && type == SHP_POLYGON))
	{
		ErrorMessage(tkNOT_ENOUGH_POINTS_FOR_SHAPE_TYPE);
		return S_FALSE;
	}

	*retVal = VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		CopyData()
// *******************************************************
void CEditShape::CopyData(int firstIndex, int lastIndex, IShape* target )
{
	long index;
	for(int i = firstIndex; i < lastIndex; i++)
	{
		MeasurePoint* pnt = _editShape->GetPoint(i);
		if (pnt) {
			target->AddPoint(pnt->Proj.x, pnt->Proj.y, &index);
		}
	}
}

// *******************************************************
//		get_PointXY()
// *******************************************************
STDMETHODIMP CEditShape::get_PointXY(long pointIndex, double* x, double* y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pointIndex < 0 || pointIndex >= (long)_editShape->GetPointCount())
	{
		ErrorMsg(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		MeasurePoint* pnt = _editShape->GetPoint(pointIndex);
		if (pnt)
		{
			*x = pnt->Proj.x;
			*y = pnt->Proj.y;
			*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// *******************************************************
//		put_PointXY()
// *******************************************************
STDMETHODIMP CEditShape::put_PointXY(long pointIndex, double x, double y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pointIndex < 0 || pointIndex >= (long)_editShape->GetPointCount())
	{
		ErrorMsg(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		MeasurePoint* pnt = _editShape->GetPoint(pointIndex);
		if (pnt)
		{
			pnt->Proj.x = x;
			pnt->Proj.y = y;
			_editShape->UpdateLatLng(pointIndex);
			*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// *******************************************************
//		get_PointCount()
// *******************************************************
STDMETHODIMP CEditShape::get_numPoints(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->GetPointCount();
	return S_OK;
}

// *******************************************************
//		Clear()
// *******************************************************
STDMETHODIMP CEditShape::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->Clear();
	for (size_t i = 0; i < _undoList.size(); i++)
		_undoList[i]->Release();
	_undoList.clear();
	return S_OK;
}

// *******************************************************
//		AddPoint()
// *******************************************************
STDMETHODIMP CEditShape::AddPoint(double xProj, double yProj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->AddPoint(xProj, yProj);
	return S_OK;
}

// ***************************************************************
//		SetShape()
// ***************************************************************
STDMETHODIMP CEditShape::SetShape( IShape* shp )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!shp) return S_FALSE;

	_editShape->Clear();

	ShpfileType shpType;
	shp->get_ShapeType(&shpType);
	_editShape->SetShapeType(shpType);
	_editShape->SetCreationMode(false);

	VARIANT_BOOL vb;
	double x, y;
	long numPoints;
	shp->get_NumPoints(&numPoints);
	for(long i = 0; i < numPoints; i++)
	{
		shp->get_XY(i, &x, &y, &vb);
		_editShape->AddPoint(x, y, -1, -1);
	}
	return S_OK;
}

// *******************************************************
//		SegmentLength()
// *******************************************************
STDMETHODIMP CEditShape::get_SegmentLength(int segmentIndex, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->GetSegmentLength(segmentIndex);
	return S_OK;
}

// *******************************************************
//		SegmentAngle()
// *******************************************************
STDMETHODIMP CEditShape::get_SegmentAngle(int segmentIndex, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->GetSegmentAngle(segmentIndex);
	return S_OK;
}

// *******************************************************
//		AreaDisplayMode()
// *******************************************************
STDMETHODIMP CEditShape::get_AreaDisplayMode(tkAreaDisplayMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->AreaDisplayMode;
	return S_OK;
}

STDMETHODIMP CEditShape::put_AreaDisplayMode(tkAreaDisplayMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->AreaDisplayMode = newVal;
	return S_OK;
}

// *******************************************************
//		AngleDisplayMode()
// *******************************************************
STDMETHODIMP CEditShape::get_AngleDisplayMode(tkAngleDisplay* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->AngleDisplayMode;
	return S_OK;
}

STDMETHODIMP CEditShape::put_AngleDisplayMode(tkAngleDisplay newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->AngleDisplayMode = newVal;
	return S_OK;
}

// *******************************************************
//		CreationMode()
// *******************************************************
STDMETHODIMP CEditShape::get_CreationMode(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->GetCreationMode() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CEditShape::put_CreationMode(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->SetCreationMode(newVal ? true: false);
	return S_OK;
}

// *******************************************************
//		get_Area()
// *******************************************************
STDMETHODIMP CEditShape::get_Area(double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->GetArea(false, 0.0, 0.0);
	return S_OK;
}

// *******************************************************
//		LayerHandle()
// *******************************************************
STDMETHODIMP CEditShape::get_LayerHandle(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _layerHandle;
	return S_OK;
}

STDMETHODIMP CEditShape::put_LayerHandle(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_layerHandle = newVal;
	return S_OK;
}

// *******************************************************
//		ShapeIndex()
// *******************************************************
STDMETHODIMP CEditShape::get_ShapeIndex(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _shapeIndex;
	return S_OK;
}

STDMETHODIMP CEditShape::put_ShapeIndex(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_shapeIndex = newVal;
	return S_OK;
}

// *******************************************************
//		Visible()
// *******************************************************
STDMETHODIMP CEditShape::get_Visible(VARIANT_BOOL* val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*val = _editShape->_geometryVisible;
	return S_OK;
}
STDMETHODIMP CEditShape::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->_geometryVisible = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		LabelsOnly()
// *******************************************************
STDMETHODIMP CEditShape::get_DrawLabelsOnly(VARIANT_BOOL* val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*val = _editShape->_drawLabelsOnly;
	return S_OK;
}
STDMETHODIMP CEditShape::put_DrawLabelsOnly(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->_drawLabelsOnly = newVal ? true : false;
	return S_OK;
}

// *******************************************************
//		SelectedVertex()
// *******************************************************
STDMETHODIMP CEditShape::get_SelectedVertex(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->_selectedVertex;
	return S_OK;
}

STDMETHODIMP CEditShape::put_SelectedVertex(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->_selectedVertex = newVal;
	return S_OK;
}

// *******************************************************
//		CopyOptionsFrom()
// *******************************************************
STDMETHODIMP CEditShape::CopyOptionsFrom(IShapeDrawingOptions* options)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	float lineWidth, tranparency;
	OLE_COLOR fillColor, lineColor;
	options->get_FillColor(&fillColor);
	options->get_LineColor(&lineColor);
	options->get_LineWidth(&lineWidth);
	options->get_FillTransparency(&tranparency);

	_editShape->FillColor = fillColor;
	_editShape->LineColor = lineColor;
	_editShape->LineWidth = lineWidth;
	_editShape->FillTransparency = (BYTE)tranparency;
	return S_OK;
}

// *******************************************************
//		FillColor()
// *******************************************************
STDMETHODIMP CEditShape::get_FillColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _editShape->FillColor;
	return S_OK;
}
STDMETHODIMP CEditShape::put_FillColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->FillColor = newVal;
	return S_OK;
}

// *******************************************************
//		FillTransparency()
// *******************************************************
STDMETHODIMP CEditShape::get_FillTransparency(BYTE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _editShape->FillTransparency;
	return S_OK;
}
STDMETHODIMP CEditShape::put_FillTransparency(BYTE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->FillTransparency = newVal;
	return S_OK;
}

// *******************************************************
//		LineColor()
// *******************************************************
STDMETHODIMP CEditShape::get_LineColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _editShape->LineColor;
	return S_OK;
}
STDMETHODIMP CEditShape::put_LineColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->LineColor = newVal;
	return S_OK;
}

// *******************************************************
//		LineWidth()
// *******************************************************
STDMETHODIMP CEditShape::get_LineWidth(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _editShape->LineWidth;
	return S_OK;
}
STDMETHODIMP CEditShape::put_LineWidth(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->LineWidth = newVal;
	return S_OK;
}

// *******************************************************
//		MoveShape()
// *******************************************************
void CEditShape::MoveShape(double offsetX, double offsetY)
{
	SaveState();
	_editShape->Move(offsetX, offsetY);
}

// *******************************************************
//		MoveVertex()
// *******************************************************
void CEditShape::MoveVertex(double offsetX, double offsetY, bool offset)
{
	SaveState();
	_editShape->MoveVertex(offsetX, offsetY, offset);
}

// *******************************************************
//		InsertVertex()
// *******************************************************
bool CEditShape::InsertVertex(double xProj, double yProj)
{
	IShape* shp = NULL;
	get_RawData(&shp);
	if (!shp) return false;
		
	if (_editShape->TryInsertVertex(xProj, yProj)) {
		_undoList.push_back(shp);
		return true;
	}
	shp->Release();
	return false;
}

// *******************************************************
//		RemoveVertex()
// *******************************************************
bool CEditShape::RemoveVertex()
{
	IShape* shp = NULL;
	get_RawData(&shp);
	if (!shp) return false;
	
	if (_editShape->RemoveSelectedVertex()) {
		_undoList.push_back(shp);
		return true;
	}
	shp->Release();
	return false;
}

// *******************************************************
//		SaveState()
// *******************************************************
void CEditShape::SaveState()
{
	IShape* shp = NULL;
	get_RawData(&shp);
	_undoList.push_back(shp);
}

// *******************************************************
//		DiscardState()
// *******************************************************
void CEditShape::DiscardState()
{
	if (_undoList.size() > 0) {
		IShape* shp = _undoList[_undoList.size() - 1];
		if (shp) {
			shp->Release();
		}
		_undoList.pop_back();
	}
}


// *******************************************************
//		Undo()
// *******************************************************
STDMETHODIMP CEditShape::Undo(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (_editShape->GetCreationMode()) {
		*retVal = _editShape->UndoPoint() ? VARIANT_TRUE : VARIANT_FALSE;
	}
	else {
		if (_undoList.size() > 0) {
			IShape* shp = _undoList[_undoList.size() - 1];
			this->SetShape(shp);
			shp->Release();
			_undoList.pop_back();
			*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// *******************************************************
//		Redo()
// *******************************************************
STDMETHODIMP CEditShape::Redo(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_editShape->GetCreationMode()) {
		// TODO: consider whether to implement this
	}
	else {
		// TODO: implement
	}
	*retVal = VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		get_IsEmpty()
// *******************************************************
STDMETHODIMP CEditShape::get_IsEmpty(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _editShape->IsEmpty();
	return S_OK;
}

// *******************************************************
//		SnapTolerance()
// *******************************************************
STDMETHODIMP CEditShape::get_SnapTolerance(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _snapTolerance;
	return S_OK;
}
STDMETHODIMP CEditShape::put_SnapTolerance(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0.0 && newVal <= 100.0) {
		_snapTolerance = newVal;
	}
	else {
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// *******************************************************
//		get_HotTracking()
// *******************************************************
STDMETHODIMP CEditShape::get_HotTracking(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _hotTracking;
	return S_OK;
}
STDMETHODIMP CEditShape::put_HotTracking(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_hotTracking = newVal;
	return S_OK;
}

// *******************************************************
//		get_SnapBehavior()
// *******************************************************
STDMETHODIMP CEditShape::get_SnapBehavior(tkSnapBehavior* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _snapBehavior;
	return S_OK;
}
STDMETHODIMP CEditShape::put_SnapBehavior(tkSnapBehavior newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_snapBehavior = newVal;
	return S_OK;
}
