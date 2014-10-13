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
//		AsShape()
// *******************************************************
STDMETHODIMP CEditShape::get_AsShape(IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;
	
	ShpfileType shpType = _editShape->GetShapeType();
	if (shpType == SHP_NULLSHAPE) return S_OK;

	GetUtils()->CreateInstance(idShape, (IDispatch**)retVal);

	VARIANT_BOOL vb;
	(*retVal)->Create(shpType, &vb);

	CopyData(0, _editShape->GetPointCount(), *retVal);

	// let's close the poly
	bool doFixup = true;		// TODO: make a parameter
	if (doFixup)
	{
		if (shpType == SHP_POLYGON)
			((CShape*)(*retVal))->FixupShapeCore(ShapeValidityCheck::FirstAndLastPointOfPartMatch);
		
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
STDMETHODIMP CEditShape::get_HasValidShape(VARIANT_BOOL* retVal)
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

	// TODO: check validity with GEOS

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
STDMETHODIMP CEditShape::get_NumPoints(long* retVal)
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
	return S_OK;
}

// *******************************************************
//		UndoPoint()
// *******************************************************
STDMETHODIMP CEditShape::UndoPoint(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->UndoPoint() ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		UndoPoint()
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

// ***************************************************************
//		FinishShape()
// ***************************************************************
STDMETHODIMP CEditShape::FinishShape(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	// check whether there is a shape
	IShape* shp = NULL;
	get_AsShape(&shp);
	if (shp) {
		shp->Release();
	}
	else {
		return S_FALSE;
	}

	CMapView* map = (CMapView*)_editShape->GetMapView();
	if (map)
	{
		tkMwBoolean cancel = blnFalse;
		map->FireShapeEditing(this, eaShapeCreated, &cancel);

		if (!cancel) 
			Clear();
	}

	*retVal = VARIANT_TRUE;
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
//		AngleRounding()
// *******************************************************
STDMETHODIMP CEditShape::get_AngleFormat(tkAngleFormat* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->AngleFormat;
	return S_OK;
}

STDMETHODIMP CEditShape::put_AngleFormat(tkAngleFormat newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->AngleFormat = newVal;
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
//		LengthRounding()
// *******************************************************
STDMETHODIMP CEditShape::get_LengthRounding(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->_lengthRounding;
	return S_OK;
}

STDMETHODIMP CEditShape::put_LengthRounding(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->_lengthRounding = newVal;
	return S_OK;
}

// *******************************************************
//		AreaRounding()
// *******************************************************
STDMETHODIMP CEditShape::get_AreaRounding(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _editShape->_areaRounding;
	return S_OK;
}

STDMETHODIMP CEditShape::put_AreaRounding(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->_areaRounding = newVal;
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
//		AngleCorrection()
// *******************************************************
STDMETHODIMP CEditShape::get_AngleCorrection(double* val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*val = _editShape->_angleCorrection;
	return S_OK;
}
STDMETHODIMP CEditShape::put_AngleCorrection(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_editShape->_angleCorrection = newVal;
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