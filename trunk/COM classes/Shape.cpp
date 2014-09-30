//********************************************************************************************************
//File name: Shape.cpp
//Description: Implementation of the CShape
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//27-jul-2009 Sergei Leschinski (lsu) - added centroid function; 
//										shifted area, length, perimeter properties from CUtils;
//06-aug-2009 lsu - added OGR/GEOS geoprocessing functions;
//********************************************************************************************************
#include "stdafx.h"
#include "Shape.h"
#include "GeometryOperations.h"
#include "Templates.h"
#include "Utilities\GeosHelper.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CShape::CShape()
{	
	USES_CONVERSION;
	_key = A2BSTR("");
	_globalCallback = NULL;
	_lastErrorCode = tkNO_ERROR;
	_isValidReason = "";
	
	_useFastMode = m_globalSettings.shapefileFastMode;
	if (m_globalSettings.shapefileFastMode)
	{
		_shp = new CShapeWrapper(SHP_NULLSHAPE);
	}
	else
	{
		_shp = new CShapeWrapperCOM(SHP_NULLSHAPE);
	}
	gReferenceCounter.AddRef(tkInterface::idShape);
}
	
	// destructor
CShape::~CShape()
{
	::SysFreeString(_key);			
		
	if (_shp)
	{
		if (_useFastMode)
		{
			delete (CShapeWrapper*)_shp;
		}
		else
		{
			delete (CShapeWrapperCOM*)_shp;
		}
		_shp = NULL;
	}

	if (_globalCallback)
	{
		_globalCallback->Release();
	}
	gReferenceCounter.Release(tkInterface::idShape);
}


#pragma region DataConversions

// **********************************************
//		CShape::get_fastMode
// **********************************************
bool CShape::get_fastMode()
{
	return _useFastMode;
}

void CShape::put_fastModeAdd(bool state)
{
	_useFastMode = state;
}

// **********************************************
//		InitShapeWrapper()
// **********************************************
CShapeWrapper* CShape::InitShapeWrapper(CShapeWrapperCOM* shpOld)
{
	ASSERT(shpOld != NULL);
	
	ShpfileType type = shpOld->get_ShapeType();

	bool isM = (type == SHP_MULTIPOINTM || type == SHP_POINTM || type == SHP_POLYGONM || type == SHP_POLYLINEM);
	bool isZ = (type == SHP_MULTIPOINTZ || type == SHP_POINTZ || type == SHP_POLYGONZ || type == SHP_POLYLINEZ);
	
	double x, y, z, m;

	// referencing the old one
	std::deque<IPoint*>* allPoints = &(shpOld->_allPoints);
	std::deque<long>* allParts = &(shpOld->_allParts);
	
	// creating new wrapper
	CShapeWrapper* shpNew = new CShapeWrapper(type);

	// passing points
	unsigned int size = allPoints->size();
	for (unsigned int i = 0; i < size; i++)
	{
		(*allPoints)[i]->get_X(&x);
		(*allPoints)[i]->get_Y(&y);
		shpNew->_points.push_back(pointEx(x, y));

		if (isZ)
		{ 
			(*allPoints)[i]->get_Z(&z);
			shpNew->_pointsZ.push_back(z);
		}
		else if (isM)
		{
			(*allPoints)[i]->get_Z(&z);
			(*allPoints)[i]->get_M(&m);
			shpNew->_pointsZ.push_back(z);
			shpNew->_pointsM.push_back(m);
		}
	}

	// passing parts
	for (unsigned int i = 0; i < (*allParts).size(); i++)
	{
		shpNew->_parts.push_back((*allParts)[i]);
	}

	return shpNew;
}

// **********************************************
//		InitComWrapper()
// **********************************************
CShapeWrapperCOM* CShape::InitComWrapper(CShapeWrapper* shpOld)
{
	ASSERT(shpOld != NULL);
	
	ShpfileType type = shpOld->get_ShapeType();

	bool isM = (type == SHP_MULTIPOINTM || type == SHP_POINTM || type == SHP_POLYGONM || type == SHP_POLYLINEM);
	bool isZ = (type == SHP_MULTIPOINTZ || type == SHP_POINTZ || type == SHP_POLYGONZ || type == SHP_POLYLINEZ);
	
	// creating the new wrapper
	CShapeWrapperCOM* shpNew = new CShapeWrapperCOM(type);
	std::deque<IPoint*>* allPoints = &(shpNew->_allPoints);
	std::deque<long>* allParts = &(shpNew->_allParts);
	
	// passing points
	IPoint* pnt = NULL;
	unsigned int size = shpOld->_points.size();
	
	for (unsigned int i = 0; i < size; i++)
	{
		m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
		//CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&pnt);
		pnt->put_X(shpOld->_points[i].X);
		pnt->put_Y(shpOld->_points[i].Y);
		if (isZ || isM)
		{
			pnt->put_Z(shpOld->_pointsZ[i]);
		}
		if (isM)
		{
			pnt->put_M(shpOld->_pointsM[i]);
		}
		allPoints->push_back(pnt);
	}
	
	// passing parts
	size = shpOld->_parts.size();
	for (unsigned int i = 0; i < size; i++)
	{
		allParts->push_back(shpOld->_parts[i]);
	}

	return shpNew;
}

// **********************************************
//		put_FastEditMode
// **********************************************
// Toggles between fast (CShapeWrapper) and regular mode (_allPoints)
void CShape::put_fastMode(bool state)
{
	ASSERT(_shp != NULL);
	
	if (state && !_useFastMode )
	{
		// COM wrapper -> CShapeWrapper

		// referencing the old one
		CShapeWrapperCOM* shpOld = (CShapeWrapperCOM*)_shp;
		
		// converting
		CShapeWrapper* shpNew = InitShapeWrapper(shpOld);

		// deleting the old wrapper
		delete shpOld;

		// storing the new one
		_shp = shpNew;
	}
	else if ( !state && _useFastMode )
	{
		// CShapeWrapper -> COM wrapper

		// referencing the old one
		CShapeWrapper* shpOld = (CShapeWrapper*)_shp;
		
		CShapeWrapperCOM* shpNew = InitComWrapper(shpOld);

		// deleting the old wrapper
		delete shpOld;

		// storing the new one
		_shp = shpNew;
	}
	_useFastMode = state;
}

#pragma endregion

#pragma region Genereal

// *************************************************************
//		get_LastErrorCode
// *************************************************************
STDMETHODIMP CShape::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;

	return S_OK;
}

// *************************************************************
//		get_ErrorMsg
// *************************************************************
STDMETHODIMP CShape::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

// *************************************************************
//		get/put_globalCallback
// *************************************************************
STDMETHODIMP CShape::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();
	return S_OK;
}
STDMETHODIMP CShape::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *************************************************************
//		get/put__key
// *************************************************************
STDMETHODIMP CShape::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CShape::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

//***************************************************************
//		ErrorMessage()								           
//***************************************************************
inline void CShape::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	if( _globalCallback != NULL) _globalCallback->Error(OLE2BSTR(_key),A2BSTR(ErrorMsg(_lastErrorCode)));
	return;
}

// *************************************************************
//		Create
// *************************************************************
STDMETHODIMP CShape::Create(ShpfileType ShpType, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if(	ShpType == SHP_MULTIPATCH )
	{	
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
		*retval = VARIANT_FALSE;
	}
	else
	{
		_shp->Clear();
		_shp->put_ShapeType(ShpType);		// the only error here (multipatch shape) is checked above
											// so we don't control return value of function here
		*retval = VARIANT_TRUE;
	}
	return S_OK;
}

// **********************************************************
//		get_ShapeType
// **********************************************************
STDMETHODIMP CShape::get_ShapeType(ShpfileType *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _shp->get_ShapeType();
	return S_OK;
}

// *************************************************************
//		put_ShapeType
// *************************************************************
STDMETHODIMP CShape::put_ShapeType(ShpfileType newVal)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!_shp->put_ShapeType(newVal))
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//			get_IsValid	
// *************************************************************
// Checking validity of the geometry
STDMETHODIMP CShape::get_IsValid(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
		
	if(_shp->get_PointCount() == 0)
	{
		_isValidReason = "Shape hasn't got points";
		return S_OK;
	}
	
	ShpfileType shptype = Utility::ShapeTypeConvert2D(_shp->get_ShapeType());

	if (shptype == SHP_POLYGON || shptype == SHP_POLYLINE)
	{
		if (_shp->get_PartCount() == 0)
		{
			_isValidReason = "Shape hasn't got parts";
			return S_OK;
		}
	}
	
	if (shptype == SHP_POLYGON)
	{
		int beg_part, end_part;
		double x1, x2, y1, y2;

		for(long i = 0; i < _shp->get_PartCount(); i++)
		{
			beg_part = _shp->get_PartStartPoint(i);
			end_part = _shp->get_PartEndPoint(i);
			
			_shp->get_PointXY(beg_part, x1, y1);
			_shp->get_PointXY(end_part, x2, y2);

			if (x1 != x2 || y1!= y2)
			{
				_isValidReason = "The first and the last point of the polygon part must be the same";
				return S_OK;
			}
		}
	}
	
	// checking the clockwise-order
	if (shptype == SHP_POLYGON)
	{
		VARIANT_BOOL isClockwise;
		int partCount = _shp->get_PartCount();
		if (partCount == 1)
		{
			this->get_PartIsClockWise(0, &isClockwise);
			if (!isClockwise)
			{
				VARIANT_BOOL ret;
				double x, y;
				this->get_XY(0, &x, &y, &ret);
				_isValidReason.Format("Polygon must be clockwise [%f %f]", x, y);
				//_isValidReason = "";
				return S_OK;
			}
		}
	}

	// -----------------------------------------------
	//  check through GEOS (common for both modes)
	// -----------------------------------------------
	OGRGeometry* oGeom = GeometryConverter::ShapeToGeometry(this);
	if (oGeom == NULL) 
	{
		_isValidReason = "Failed to convert to OGR geometry";
		return S_OK;
	}

	// added code
	GEOSGeom hGeosGeom = NULL;	
	
	hGeosGeom = GeosHelper::ExportToGeos(oGeom);

	OGRGeometryFactory::destroyGeometry(oGeom);

	if (hGeosGeom == NULL)
	{
		_isValidReason = "Failed to convert to GEOS geometry";
		return S_OK;
	}

	if (!GeosHelper::IsValid(hGeosGeom ))
	{
		char* buffer = GeosHelper::IsValidReason(hGeosGeom);
		_isValidReason = buffer;
		GeosHelper::Free(buffer);
	}
	else
	{
		*retval = VARIANT_TRUE;
	}
	GeosHelper::DestroyGeometry(hGeosGeom);
	
	return S_OK;
}
#pragma endregion

#pragma region Parts
// **********************************************************
//		get_NumParts
// **********************************************************
STDMETHODIMP CShape::get_NumParts(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _shp->get_PartCount();
	return S_OK;
}

// *************************************************************
//		get_Part
// *************************************************************
STDMETHODIMP CShape::get_Part(long PartIndex, long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _shp->get_PartStartPoint(PartIndex);
	if (*pVal == -1)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//		put_Part
// *************************************************************
STDMETHODIMP CShape::put_Part(long PartIndex, long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	
	if (!_shp->put_PartStartPoint(PartIndex, newVal))
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

/***********************************************************************/
/*		get_EndOfPart()
/***********************************************************************/
//  Returns last point of the part
STDMETHODIMP CShape::get_EndOfPart(long PartIndex, long* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = _shp->get_PartEndPoint(PartIndex);
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
};

// *************************************************************
//		InsertPart
// *************************************************************
STDMETHODIMP CShape::InsertPart(long PointIndex, long *PartIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*retval = (VARIANT_BOOL)_shp->InsertPart(*PartIndex , PointIndex);
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}
	
// *************************************************************
//		DeletePart
// *************************************************************
STDMETHODIMP CShape::DeletePart(long PartIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*retval = (VARIANT_BOOL)_shp->DeletePart(PartIndex);
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

//*********************************************************************
//		CShape::PartIsClockWise()									
//*********************************************************************
//  Returns true if points of shape's part are in clockwise direction
//  and false otherwise
STDMETHODIMP CShape::get_PartIsClockWise(long PartIndex, VARIANT_BOOL* retval)
{
	*retval = VARIANT_FALSE;
	
	long numParts, numPoints;
	this->get_NumParts(&numParts);

	if ((PartIndex >= numParts) || (PartIndex < 0))
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
	
	long beg_part, end_part;
	this->get_NumPoints(&numPoints);
	this->get_Part(PartIndex,&beg_part);
	
	if( numParts - 1 > PartIndex )	
		this->get_Part(PartIndex+1, &end_part);
	else							
		end_part = numPoints;
	
	//lsu: we need to calcuate area of part to determine clockwiseness
	double x1, x2, y1, y2;
	VARIANT_BOOL vbretval;
	double area = 0;

	for(long i = beg_part; i < end_part - 1; i++)
	{
		this->get_XY(i, &x1, &y1, &vbretval);
		this->get_XY(i + 1, &x2, &y2, &vbretval);
		area += (x1 * y2) - (x2 * y1);
	}
    
    if (area < 0) *retval = VARIANT_TRUE;
	
	return S_OK;
}

//***********************************************************************/
//		CShape::ReversePointsOrder()
//***********************************************************************/
//  Changes the order of points for shape's part
STDMETHODIMP CShape::ReversePointsOrder(long PartIndex, VARIANT_BOOL* retval)
{
	*retval = VARIANT_FALSE;
	
	long numParts, numPoints;
	this->get_NumParts(&numParts);

	if ((PartIndex >= numParts) || (PartIndex < 0))
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
	
	long beg_part, end_part;
	this->get_NumPoints(&numPoints);
	this->get_Part(PartIndex,&beg_part);
	
	if( numParts - 1 > PartIndex )	
	{
		this->get_Part(PartIndex+1, &end_part);
	}
	else							
	{
		end_part = numPoints;
	}
	
	if (_useFastMode)
	{
		CShapeWrapper* shp = (CShapeWrapper*) _shp;
		if (shp->_points.size() > 1)
		{
			std::vector<pointEx>::iterator iter1 = shp->_points.begin();
			std::vector<pointEx>::iterator iter2 = shp->_points.begin();
			iter1 +=beg_part;
			iter2 +=end_part;
			reverse( iter1, iter2);
		}
		
		if (shp->_pointsZ.size() > 1)
		{
			std::vector<double>::iterator iterZ1 = shp->_pointsZ.begin();
			std::vector<double>::iterator iterZ2 = shp->_pointsZ.begin();
			iterZ1 +=beg_part;
			iterZ2 +=end_part;
			reverse( iterZ1, iterZ2);
		}
		
		if (shp->_pointsM.size() > 1)
		{
			std::vector<double>::iterator iterM1 = shp->_pointsM.begin();
			std::vector<double>::iterator iterM2 = shp->_pointsM.begin();
			iterM1 +=beg_part;
			iterM2 +=end_part;
			reverse( iterM1, iterM2);
		}
	}
	else
	{
		CShapeWrapperCOM* shp = (CShapeWrapperCOM*) _shp;
		std::deque<IPoint* >* allPoints = &shp->_allPoints;
		deque<IPoint *>::iterator iter1 = allPoints->begin();
		deque<IPoint *>::iterator iter2 = allPoints->begin();
		iter1 +=beg_part;
		iter2 +=end_part;
		reverse( iter1, iter2);
	}

	*retval = VARIANT_TRUE;
	return S_OK;
}

// ***************************************************************
//		PartAsShape()
// ***************************************************************
//  Returns part of the shape as new shape; new points are created
STDMETHODIMP CShape::get_PartAsShape(long PartIndex, IShape **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	long beg_part, end_part;
	this->get_Part(PartIndex, &beg_part);
	this->get_EndOfPart(PartIndex, &end_part);
	
	if (beg_part == -1 || end_part == -1) 
	{
		*pVal = NULL; 
		return S_OK;
	}
	
	IShape * shp = NULL;
	CoCreateInstance( CLSID_Shape, NULL, CLSCTX_INPROC_SERVER, IID_IShape, (void**)&shp );
	
	ShpfileType shptype = _shp->get_ShapeType();
	shp->put_ShapeType(shptype);

	long part  = 0; 
	VARIANT_BOOL vbretval;
	shp->InsertPart(0, &part, &vbretval);
	
	long cnt = 0;
	IPoint* pntOld = NULL;
	IPoint* pntNew = NULL;
	for (int i = beg_part; i <=end_part; i++)
	{
		this->get_Point(i, &pntOld);
		pntOld->Clone(&pntNew);
		shp->InsertPoint(pntNew, &cnt, &vbretval);
		pntOld->Release();
		pntNew->Release();
		cnt++;
	}
	*pVal = shp;
	return S_OK;
}

#pragma endregion	

#pragma region Points
// **********************************************************
//		get_NumPoints
// **********************************************************
STDMETHODIMP CShape::get_NumPoints(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _shp->get_PointCount();
	return S_OK;
}

// *************************************************************
//		get_Point
// *************************************************************
STDMETHODIMP CShape::get_Point(long PointIndex, IPoint **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _shp->get_Point(PointIndex);
	if ((*pVal) == NULL)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//		put_Point
// *************************************************************
STDMETHODIMP CShape::put_Point(long PointIndex, IPoint *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( newVal == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
	}
	else
	{
		double x, y;	
		newVal->get_X(&x);
		newVal->get_Y(&y);
		newVal->Release();
		if (!_shp->put_PointXY(PointIndex, x, y))
		{
			ErrorMessage(_shp->get_LastErrorCode());
		}
	}
	return S_OK;
}

// *************************************************************
//		InsertPoint
// *************************************************************
STDMETHODIMP CShape::InsertPoint(IPoint *NewPoint, long *PointIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*retval = (VARIANT_BOOL)_shp->InsertPoint(*PointIndex, NewPoint);
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//		DeletePoint
// *************************************************************
STDMETHODIMP CShape::DeletePoint(long PointIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*retval = (VARIANT_BOOL)_shp->DeletePoint(PointIndex);
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//		get_XY
// *************************************************************
STDMETHODIMP CShape::get_XY(long PointIndex, double* x, double* y, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = get_XY(PointIndex, x, y);
	return S_OK;
}

// **********************************************
//   put_XY()
// **********************************************
STDMETHODIMP CShape::put_XY(LONG pointIndex, double x, double y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = (VARIANT_BOOL)_shp->put_PointXY(pointIndex, x, y);
	if (*retVal == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// **********************************************
//   put_M()
// **********************************************
STDMETHODIMP CShape::put_M(LONG pointIndex, double m, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = (VARIANT_BOOL)_shp->put_PointM(pointIndex, m);
	if (!(*retVal))
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// **********************************************
//   put_Z()
// **********************************************
STDMETHODIMP CShape::put_Z(LONG pointIndex, double z, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = (VARIANT_BOOL)_shp->put_PointZ(pointIndex, z);
	if (!(*retVal))
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//		get_Z
// *************************************************************
STDMETHODIMP CShape::get_Z(long PointIndex, double* z, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = get_Z(PointIndex, z);
	return S_OK;
}

// *************************************************************
//		get_M
// *************************************************************
STDMETHODIMP CShape::get_M(long PointIndex, double* m, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = get_M(PointIndex, m);
	return S_OK;
}

#pragma endregion	

#pragma region SpatialProperties

// *****************************************************************
//			get_Center()
// *****************************************************************
//  Returns center of shape (crossing of diagonals of bounding box)
STDMETHODIMP CShape::get_Center(IPoint **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	double xMin, xMax, yMin, yMax;
	double x, y;
	
	//CShapeWrapper* shp = (CShapeWrapper*)_shp;
	//std::vector<pointEx>* points = &shp->_points;

	for( int i = 0; i < (int)_shp->get_PointCount(); i++ )
	{	
		_shp->get_PointXY(i, x, y);

		if( i == 0 )
		{	
			xMin = x; xMax = x;
			yMin = y; yMax = y;
		}
		else
		{	
			if		(x < xMin)	xMin = x;
			else if (x > xMax)	xMax = x;
			if		(y < yMin)	yMin = y;
			else if (y > yMax)	yMax = y;
		}
	}	
	
	x = xMin + (xMax - xMin)/2;
	y = yMin + (yMax - yMin)/2;

	IPoint * pnt = NULL;
	CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&pnt );
	pnt->put_X(x);pnt->put_Y(y);
	*pVal = pnt;
	
	return S_OK;
}

// *************************************************************
//		get_Length
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Length(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	ShpfileType shptype;
	this->get_ShapeType(&shptype);

	if( shptype != SHP_POLYLINE && shptype != SHP_POLYLINEZ && shptype != SHP_POLYLINEM )
	{	
		*pVal = 0.0;
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	double length = 0.0;
	long numParts = 0, numPoints = 0;
	this->get_NumParts(&numParts);
	this->get_NumPoints(&numPoints);
	
	long beg_part = 0;
	long end_part = 0;
	for( int j = 0; j < numParts; j++ )
	{	
		this->get_Part(j,&beg_part);
		if( numParts - 1 > j )
		{
			this->get_Part(j+1,&end_part);
		}
		else
		{
			end_part = numPoints;
		}

		double oneX, oneY;
		double twoX, twoY;
		VARIANT_BOOL vbretval;

		for( int i = beg_part; i < end_part-1; i++ )
		{	
			this->get_XY(i, &oneX, &oneY, &vbretval);
			this->get_XY(i + 1, &twoX, &twoY, &vbretval);
			length += sqrt( pow( twoX - oneX, 2 ) + pow( twoY - oneY, 2 ) );
		}
	}
	
	*pVal = length;
	return S_OK;
}

// *************************************************************
//		get_Perimeter
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Perimeter(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	ShpfileType shptype;
	this->get_ShapeType(&shptype);

	if( shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM )
	{	
		*pVal = 0.0;
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	double perimeter = 0.0;
	long numParts = 0, numPoints = 0;
	this->get_NumParts(&numParts);
	this->get_NumPoints(&numPoints);

	long beg_part = 0;
	long end_part = 0;
	for( int j = 0; j < numParts; j++ )
	{	
		this->get_Part(j,&beg_part);
		if( numParts - 1 > j )
			this->get_Part(j+1,&end_part);
		else
			end_part = numPoints;
		
		double px1, py1, px2, py2;
		VARIANT_BOOL vbretval;
		for( int i = beg_part; i < end_part-1; i++ )
		{	
			this->get_XY(i, &px1, &py1, &vbretval);
			this->get_XY(i + 1, &px2, &py2, &vbretval);
			perimeter += sqrt( pow(px2 - px1, 2) + pow(py2 - py1,2));
		}
		
	}

	*pVal = perimeter;
	return S_OK;
}

// *************************************************************
//		get_Extents
// *************************************************************
STDMETHODIMP CShape::get_Extents(IExtents **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	double xMin = 0.0, xMax = 0.0,
		   yMin = 0.0, yMax = 0.0,
		   zMin = 0.0, zMax = 0.0,
		   mMin = 0.0, mMax = 0.0;
		
	if (_shp->get_ShapeType2D() == SHP_POINT)
	{
		_shp->get_PointXY(0, xMin, yMin);
		xMax = xMin; yMax = yMin;
	}
	else
	{
		_shp->get_Bounds(xMin, xMax, yMin, yMax, zMin, zMax, mMin, mMax);
	}

	IExtents * bBox = NULL;
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&bBox );
	bBox->SetBounds(xMin,yMin,zMin,xMax,yMax,zMax);
	bBox->SetMeasureBounds(mMin,mMax);
	*pVal = bBox;
	return S_OK;
}

// *************************************************************
//		get_Area
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Area(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	ShpfileType shptype;
	this->get_ShapeType(&shptype);

	if( shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM )
	{	
		*pVal = 0.0;
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	double total_area = 0.0;
	double indiv_area = 0.0;

	long numParts = 0, numPoints = 0;
	this->get_NumParts(&numParts);
	this->get_NumPoints(&numPoints);
	
	if(numParts > 1)
	{
		//Create new polygons from the different parts (simplified on 2/10/06 by Angela Hillier)
		std::deque<Poly> all_polygons;

		long beg_part = 0;
		long end_part = 0;
		for( int j = 0; j < numParts; j++ )
		{	
			this->get_Part(j, &beg_part);
			if( numParts - 1 > j )
				this->get_Part(j+1, &end_part);
			else
				end_part = numPoints;
			
			double px, py;
			Poly polygon;
			for( int i = beg_part; i < end_part; i++ )
			{	
				VARIANT_BOOL vbretval;
				this->get_XY(i, &px, &py, &vbretval);
				polygon.polyX.push_back(px);
				polygon.polyY.push_back(py);			
			}
			all_polygons.push_back( polygon );		
		}
		
		for( int p = 0; p < (int)all_polygons.size(); p++ )
		{	
			indiv_area = 0.0;

			//Calculate individual area of each part
			for( int a = 0; a < (int)all_polygons[p].polyX.size() - 1; a++)
			{	
				double oneX = all_polygons[p].polyX[a];
				double oneY = all_polygons[p].polyY[a];
				double twoX = all_polygons[p].polyX[a+1];		
				double twoY = all_polygons[p].polyY[a+1];
			
				double trap_area = ((oneX * twoY) - (twoX * oneY));
				indiv_area += trap_area;
			}		
			
			total_area += indiv_area;
		}
		total_area = fabs(total_area) * .5;
		all_polygons.clear();
	}
	else
	{
		double oneX, oneY, twoX, twoY;
		VARIANT_BOOL vbretval;
		for(int i= 0; i <= numPoints-2; i++)
		{
			this->get_XY(i, &oneX, &oneY, &vbretval);
			this->get_XY(i + 1, &twoX, &twoY, &vbretval);
			double trap_area = ((oneX * twoY) - (twoX * oneY));
			total_area += trap_area;
		}
		total_area = fabs(total_area) * .5;
	}

	*pVal = total_area;
	return S_OK;
}

// *************************************************************
//		get_Centroid
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Centroid(IPoint** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	ShpfileType shptype = _shp->get_ShapeType();
	if( shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM )
	{	
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}
	
	long numPoints, numParts;
	long beg_part, end_part;
	double totalArea = 0.0;
    double area;
	double xSum, ySum;
	double XShift = 0.0;
	double YShift = 0.0; 
	double xPart, yPart;	// centroid of part
	double x = 0.0;			// final centroid
	double y = 0.0;
	
	double xMin, xMax, yMin, yMax;
	this->get_ExtentsXY(xMin, yMin, xMax, yMax);
	if (xMin < 0 && xMax > 0) XShift = xMax - xMin;
	if (yMin < 0 && yMax > 0) YShift = yMax - yMin;

	this->get_NumPoints(&numPoints);
	this->get_NumParts(&numParts);
	
	for( int j = 0; j < numParts; j++ )
	{	
		area = xSum = ySum = 0.0;
		
		this->get_Part(j, &beg_part);
		if( numParts - 1 > j )
			this->get_Part(j+1, &end_part);
		else
			end_part = numPoints;

		double oneX, oneY, twoX, twoY;
		VARIANT_BOOL vbretval;
		for (int i = beg_part; i < end_part -1; i++)
		{
			this->get_XY(i, &oneX, &oneY, &vbretval);
			this->get_XY(i + 1, &twoX, &twoY, &vbretval);

			double cProduct = ((oneX + XShift) * (twoY + YShift)) - ((twoX + XShift) * (oneY + YShift));
			xSum += ((oneX + XShift) + (twoX + XShift)) * cProduct;
			ySum += ((oneY + YShift) + (twoY + YShift)) * cProduct;
			
			area += (oneX * twoY) - (twoX * oneY);
		}

		area = fabs(area) * .5;
		if (area!=0)
		{	
			xPart = xSum / (6 * area);
			yPart = ySum / (6 * area);
		}

		// corrects for shapes in quadrants other than 1 or clockwise/counterclocwise sign errors
		if (xMax + XShift < 0 && xPart > 0)  xPart = -1 * xPart;
		if (xMin + XShift > 0 && xPart < 0)  xPart = -1 * xPart;
		if (yMax + YShift < 0 && yPart > 0)  yPart = -1 * yPart;
		if (yMin + YShift > 0 && yPart < 0)  yPart = -1 * yPart;
	    
		// Adjust centroid if we calculated it using an X or Y shift
		xPart -= XShift;
		yPart -= YShift;

		x += xPart * area;
        y += yPart * area;
		totalArea += area;
	}
	if (totalArea != 0)
	{	
		x = x / totalArea;
		y = y / totalArea;
	}
	
	IPoint* pnt = NULL;
	m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
	//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);	
	pnt->put_X(x);
	pnt->put_Y(y);
	*pVal = pnt;
	return S_OK;
}
#pragma endregion

#pragma region GEOSGeoprocessing

// *************************************************************
//		Relates()
// *************************************************************
STDMETHODIMP CShape::Relates(IShape* Shape, tkSpatialRelation Relation, VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;	

	if( Shape == NULL)
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	} 
	
	// if extents don't cross, no need to seek further
	if (!(Relation == srDisjoint))	
	{	
		if (RelateExtents(this, Shape) == erNone)		
			return S_OK;
	}

	OGRGeometry* oGeom1 = NULL;
	OGRGeometry* oGeom2 = NULL;
	
	oGeom1 = GeometryConverter::ShapeToGeometry(this);
	if (oGeom1 == NULL) return S_OK;

	oGeom2 = GeometryConverter::ShapeToGeometry(Shape);
	if (oGeom2 == NULL) 
	{	
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}
	
	OGRBoolean res = 0;
	
	switch (Relation)
	{
		case srContains:	res = oGeom1->Contains(oGeom2); break;
		case srCrosses:		res = oGeom1->Crosses(oGeom2); break;
		case srDisjoint:	res = oGeom1->Disjoint(oGeom2); break;
		case srEquals:		res = oGeom1->Equals(oGeom2); break;
		case srIntersects:	res = oGeom1->Intersect(oGeom2); break;
		case srOverlaps:	res = oGeom1->Overlaps(oGeom2); break;
		case srTouches:		res = oGeom1->Touches(oGeom2); break;
		case srWithin:		res = oGeom1->Within(oGeom2); break;
	}
	
	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);

	*retval = res;
	return S_OK;
}

// *************************************************************
//		Relations
// *************************************************************
STDMETHODIMP CShape::Contains(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srContains, retval);
	return S_OK;
}
STDMETHODIMP CShape::Crosses(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srCrosses, retval);
	return S_OK;
}
STDMETHODIMP CShape::Disjoint(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srDisjoint, retval);
	return S_OK;
}
STDMETHODIMP CShape::Equals(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srEquals, retval);
	return S_OK;
}
STDMETHODIMP CShape::Intersects(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srIntersects, retval);
	return S_OK;
}
STDMETHODIMP CShape::Overlaps(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srOverlaps, retval);
	return S_OK;
}
STDMETHODIMP CShape::Touches(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srTouches, retval);
	return S_OK;
}
STDMETHODIMP CShape::Within(IShape* Shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->Relates(Shape, srWithin, retval);
	return S_OK;
}

// *************************************************************
//		Clip
// *************************************************************
STDMETHODIMP CShape::Clip(IShape* Shape, tkClipOperation Operation, IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = NULL;

	if( Shape == NULL)
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	} 
	
	if (Operation == clClip || Operation == clIntersection)
	{
		if (RelateExtents(this, Shape) == erNone)	
			return S_OK;
	}

	OGRGeometry* oGeom1 = NULL;
	OGRGeometry* oGeom2 = NULL;

	oGeom1 = GeometryConverter::ShapeToGeometry(this);
	if (oGeom1 == NULL) 
		return S_OK;

	OGRwkbGeometryType oReturnType = oGeom1->getGeometryType();
	
	oGeom2 = GeometryConverter::ShapeToGeometry(Shape);
	if (oGeom2 == NULL) 
	{	
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}
	
	OGRGeometry* oGeom3 = NULL;

	switch (Operation)
	{
		case clUnion:			
			oGeom3 = oGeom1->Union(oGeom2);
			break;
		case clDifference:
			oGeom3 = oGeom1->Difference(oGeom2);
			break;
		case clIntersection:
		case clClip:
			oGeom3 = oGeom1->Intersection(oGeom2);
			break;
		case clSymDifference:
			oGeom3 = oGeom1->SymmetricDifference(oGeom2);
			break;
		default:
			break;
	}
	
	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);
	
	if (oGeom3 == NULL) 
		return S_OK;

	IShape* shp;
	ShpfileType shpType;
	this->get_ShapeType(&shpType);
	shp = GeometryConverter::GeometryToShape(oGeom3, Utility::ShapeTypeIsM(shpType), oReturnType);
	
	OGRGeometryFactory::destroyGeometry(oGeom3);

	*retval = shp;
	return S_OK;
}

// *************************************************************
//		Distance
// *************************************************************
STDMETHODIMP CShape::Distance(IShape* Shape, double* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	*retval = 0.0;

	if( Shape == NULL)
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	} 
	
	OGRGeometry* oGeom1 = NULL;
	OGRGeometry* oGeom2 = NULL;
	
	oGeom1 = GeometryConverter::ShapeToGeometry(this);
	if (oGeom1 == NULL) return S_OK;

	oGeom2 = GeometryConverter::ShapeToGeometry(Shape);
	if (oGeom2 == NULL) 
	{	
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}

	*retval = oGeom1->Distance(oGeom2);
	

	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);
	return S_OK;
}

OGRGeometry* DoBuffer(DOUBLE Distance, long nQuadSegments, OGRGeometry* geomSource)
{
	__try
	{
		return geomSource->Buffer(Distance, nQuadSegments);
	}
	__except(1)
	{
		return NULL;
	}
}

// *************************************************************
//		Buffer
// *************************************************************
STDMETHODIMP CShape::Buffer(DOUBLE Distance, long nQuadSegments, IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = NULL;

	OGRGeometry* oGeom1 = NULL;
	OGRGeometry* oGeom2 = NULL;

	oGeom1 = GeometryConverter::ShapeToGeometry(this);
	if (oGeom1 == NULL) return S_FALSE;
		
	oGeom2 = DoBuffer(Distance, nQuadSegments, oGeom1);

	OGRGeometryFactory::destroyGeometry(oGeom1);
	if (oGeom2 == NULL)	return S_FALSE;
	
	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	IShape* shp;
	shp = GeometryConverter::GeometryToShape(oGeom2, Utility::ShapeTypeIsM(shpType));

	*retval = shp;
	OGRGeometryFactory::destroyGeometry(oGeom2);
	
	return S_OK;
}

// *************************************************************
//		Boundary
// *************************************************************
STDMETHODIMP CShape::Boundary(IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = NULL;

	OGRGeometry* oGeom1 = NULL;
	OGRGeometry* oGeom2 = NULL;

	oGeom1 = GeometryConverter::ShapeToGeometry(this);
	if (oGeom1 == NULL) return S_OK;
	
	oGeom2 = oGeom1->getBoundary();
	OGRGeometryFactory::destroyGeometry(oGeom1);

	if (oGeom2 == NULL)	return S_OK;
	
	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	IShape* shp;
	shp = GeometryConverter::GeometryToShape(oGeom2, Utility::ShapeTypeIsM(shpType));

	*retval = shp;
	OGRGeometryFactory::destroyGeometry(oGeom2);
	
	return S_OK;
}

// *************************************************************
//		ConvexHull
// *************************************************************
STDMETHODIMP CShape::ConvexHull(IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = NULL;

	OGRGeometry* oGeom1 = NULL;
	OGRGeometry* oGeom2 = NULL;

	oGeom1 = GeometryConverter::ShapeToGeometry(this);
	if (oGeom1 == NULL) return S_OK;
	
	oGeom2 = oGeom1->ConvexHull();
	OGRGeometryFactory::destroyGeometry(oGeom1);

	if (oGeom2 == NULL)	return S_OK;
	
	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	IShape* shp;
	shp = GeometryConverter::GeometryToShape(oGeom2, Utility::ShapeTypeIsM(shpType));

	*retval = shp;
	OGRGeometryFactory::destroyGeometry(oGeom2);
	
	return S_OK;
}

// *************************************************************
//		get_isValidReason
// *************************************************************
STDMETHODIMP CShape::get_IsValidReason(BSTR* retval)
{
	*retval = A2BSTR(_isValidReason);
	_isValidReason = "";
	return S_OK;
};

/***********************************************************************/
/*		CShape::GetIntersection()
/***********************************************************************/
//  Returns intersection of 2 shapes as an safearray of shapes
 STDMETHODIMP CShape::GetIntersection(IShape* Shape, VARIANT* Results, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;

	if( Shape == NULL)
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	} 
	
	if (RelateExtents(this, Shape) == erNone)	
		return S_OK;

	OGRGeometry* oGeom1 = NULL;
	OGRGeometry* oGeom2 = NULL;

	oGeom1 = GeometryConverter::ShapeToGeometry(this);
	if (oGeom1 == NULL) return S_OK;
	
	oGeom2 = GeometryConverter::ShapeToGeometry(Shape);
	if (oGeom2 == NULL) 
	{	
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}
	
	OGRGeometry* oGeom3 = NULL;
	oGeom3 = oGeom1->Intersection(oGeom2);

	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);
	
	if (oGeom3 == NULL) return S_OK;
	
	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	std::vector<IShape*> vShapes;
	if (!GeometryConverter::GeometryToShapes(oGeom3, &vShapes, Utility::ShapeTypeIsM(shpType)))return S_OK;
	OGRGeometryFactory::destroyGeometry(oGeom3);

	if (vShapes.size()!=0) 
	{
		if (Templates::Vector2SafeArray(&vShapes, Results))
			*retval = VARIANT_TRUE;
		vShapes.clear();
	}
	return S_OK;
}

// ***************************************************************
//		get_InteriorPoint
// ***************************************************************
// Returns interior point - located on the horizontal line which goes
// through vertical center of the shapes extents.
STDMETHODIMP CShape::get_InteriorPoint(IPoint** retval)
{
	*retval = NULL;

	double xMin, xMax, yMin, yMax;
	this->get_ExtentsXY(xMin, yMin, xMax, yMax);
	
	OGRGeometry* oGeom = GeometryConverter::ShapeToGeometry(this);
	if (oGeom == NULL)
	{
		ErrorMessage(tkCANT_CONVERT_SHAPE_GEOS);
		return S_OK;
	}

	OGRLineString* oLine = (OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
	oLine->addPoint(xMin,(yMax + yMin)/2);
	oLine->addPoint(xMax,(yMax + yMin)/2);
	
	OGRGeometry* oResult = oGeom->Intersection(oLine);
	if (oResult == NULL)
	{
		ErrorMessage(tkSPATIAL_OPERATION_FAILED);
		OGRGeometryFactory::destroyGeometry(oLine); 
		return S_OK;
	}
	
	// Intersection can be line or point; for polygons we are interested
	//   in the longest line, and for polylines - in point which is the closest
	//   to the center of extents
	double x = DBL_MIN;
	double y = DBL_MIN;
	
	ShpfileType shptype = Utility::ShapeTypeConvert2D(_shp->get_ShapeType());

	if( shptype == SHP_POLYLINE)
	{
		ErrorMessage(tkMETHOD_NOT_IMPLEMENTED);
	}
	if( shptype == SHP_POLYGON)
	{
		OGRwkbGeometryType oType = oResult->getGeometryType();
		if (oType == wkbLineString || oType == wkbLineString25D)
		{
			OGRLineString* oSubLine = (OGRLineString*) oResult;
			x = (oSubLine->getX(0) + oSubLine->getX(1))/2;
			y = (oSubLine->getY(0) + oSubLine->getY(1))/2;
		}
		else if (oType == wkbGeometryCollection || oType == wkbGeometryCollection25D ||
				 oType == wkbMultiLineString || oType == wkbMultiLineString25D)
		{
			double maxLength = -1;

			OGRGeometryCollection* oColl = (OGRGeometryCollection *) oResult;
			for (long i=0; i < oColl->getNumGeometries(); i++)
			{	
				OGRGeometry* oPart = oColl->getGeometryRef(i);
				if (oPart->getGeometryType() == wkbLineString || oPart->getGeometryType() == wkbLineString25D)
				{
					
					OGRLineString* oSubLine = (OGRLineString*)oPart;
					double length = oSubLine->get_Length();
					if (length > maxLength)
					{
						x = (oSubLine->getX(0) + oSubLine->getX(1))/2;
						y = (oSubLine->getY(0) + oSubLine->getY(1))/2;
						maxLength = length;
					}
				}
			}
		}
	}
	
	if (x != 0 || y !=0 )
	{
		CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)retval );
		(*retval)->put_X(x);
		(*retval)->put_Y(y);
	}
	
	// cleaning
	if (oLine)	OGRGeometryFactory::destroyGeometry(oLine); 
	if (oGeom)	OGRGeometryFactory::destroyGeometry(oGeom); 
	if (oResult)OGRGeometryFactory::destroyGeometry(oResult);
	return S_OK;
}
#pragma endregion

#pragma region Serialization
// *************************************************************
//		SerializeToString
// *************************************************************
STDMETHODIMP CShape::SerializeToString(BSTR * Serialized)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	//	fast editing mode
	ShpfileType shptype = _shp->get_ShapeType();

	CString builder = "";
	char cbuf[20];
	double dbuf;
	double dbuf1;
	_itoa(shptype, cbuf, 10);
	builder.Append(cbuf);
	builder.Append(";");

	for(int i = 0; i < (int)_shp->get_PartCount(); i++ )
	{
		sprintf(cbuf, "%d;", _shp->get_PartStartPoint(i));
		builder.Append(cbuf);
	}
	
	for(int i = 0; i < (int)_shp->get_PointCount(); i++ )
	{	
		_shp->get_PointXY(i, dbuf, dbuf1);
		
		sprintf(cbuf, "%f|", dbuf);
		builder.Append(cbuf);

		sprintf(cbuf, "%f|", dbuf1);
		builder.Append(cbuf);

		if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM || 
			shptype == SHP_MULTIPOINTZ || shptype == SHP_POLYGONZ || shptype == SHP_POLYLINEZ)
		{
			_shp->get_PointZ(i, dbuf);
			sprintf(cbuf, "%f|", dbuf);
			builder.Append(cbuf);
		}
		if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM)
		{
			_shp->get_PointM(i, dbuf);
			sprintf(cbuf, "%f|", dbuf);
			builder.Append(cbuf);
		}
	}
	*Serialized = builder.AllocSysString();
	
	return S_OK;
}

// *************************************************************
//		CreateFromString
// *************************************************************
STDMETHODIMP CShape::CreateFromString(BSTR Serialized, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	
	VARIANT_BOOL rt;
	CString ser = OLE2A(Serialized);
	CString next;
	next = ser.Mid(ser.Find(";")+1);

	ShpfileType newShpType = (ShpfileType)atoi(ser.Left(ser.Find(";")));

	//Test the ShpType
	if(	newShpType == SHP_MULTIPATCH )
	{	
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
		*retval = VARIANT_FALSE;
	}
	else
	{
		//Discard the old shape information
		_shp->Clear();
	
		_shp->put_ShapeType(newShpType);

		while (next.Find(";") != -1)
		{
			long nextID = _shp->get_PartCount();
			this->InsertPart(atol(next.Left(next.Find(";"))), &nextID, &rt);
			next = next.Mid(next.Find(";")+1);
		}
		
		double x,y,z,m;
		while (next.Find("|") != -1)
		{
			x = Utility::atof_custom(next.Left(next.Find("|")));
			next = next.Mid(next.Find("|")+1);
			y = Utility::atof_custom(next.Left(next.Find("|")));
			
			long nextID = _shp->get_PointCount();
			_shp->InsertPointXY(nextID, x, y);
			
			ShpfileType shptype = _shp->get_ShapeType();

			// Z
			if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM || shptype == SHP_MULTIPOINTZ || shptype == SHP_POLYGONZ || shptype == SHP_POLYLINEZ)
			{
				next = next.Mid(next.Find("|")+1);
				z = Utility::atof_custom(next.Left(next.Find("|")));
				_shp->put_PointZ(nextID, z);
			}
			// M
			if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM)
			{
				next = next.Mid(next.Find("|")+1);
				m = Utility::atof_custom(next.Left(next.Find("|")));
				_shp->put_PointM(nextID, m);
			}

			next = next.Mid(next.Find("|")+1);
		}
		*retval = VARIANT_TRUE;
	}
	return S_OK;
}
#pragma endregion

// *****************************************************************
//		PointInThisPoly()
// *****************************************************************
STDMETHODIMP CShape::PointInThisPoly(IPoint * pt, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	GetUtils()->PointInPolygon(this, pt, retval);

	//bool result = _useFastMode ? PointInThisPolyFast(pt) : PointInThisPolyRegular(pt);
	return S_OK;
}

//*******************************************************************
//		Get_SegmentAngle()
//*******************************************************************
// returns angle in degrees
double CShape::get_SegmentAngle( long segementIndex)
{
	double x1, y1, x2, y2, dx, dy;
	VARIANT_BOOL vbretval;
	long numPoints;
	this->get_NumPoints(&numPoints);
	if (segementIndex > numPoints - 2)
	{
		return 0.0;
	}
	
	this->get_XY(segementIndex, &x1, &y1, &vbretval);
	this->get_XY(segementIndex + 1, &x2, &y2, &vbretval);
	dx = x2 -x1; dy = y2 - y1;
	return GetPointAngle(dx, dy) / pi_ * 180.0;
}

// **********************************************
//   get_ShapeWrapper()
// **********************************************
IShapeWrapper* CShape::get_ShapeWrapper()
{ 
	return _shp; 
}

// **********************************************
//   put_ShapeWrapper()
// **********************************************
bool CShape::put_ShapeWrapper(CShapeWrapper* data)
{
	if (data == NULL )		// fast mode should be set explicitly before using wrapper
	{
		return false;
	}
	else
	{
		if (_shp)
		{
			if (_useFastMode)
			{
				delete (CShapeWrapper*)_shp;
			}
			else
			{
				delete (CShapeWrapperCOM*)_shp;
			}
			_shp = NULL;
		}
		_shp = data;		// shp must never be null
		_useFastMode = true;
		return true;
	}
}


// **********************************************
//   Clone()
// **********************************************
STDMETHODIMP CShape::Clone(IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	(*retval) = NULL;

	IShape* shp = NULL;
	CoCreateInstance( CLSID_Shape, NULL, CLSCTX_INPROC_SERVER, IID_IShape, (void**)&shp);
	if (shp)
	{
		VARIANT_BOOL vbretval;
		ShpfileType shpType;
		this->get_ShapeType(&shpType);
		shp->Create(shpType, &vbretval);
		
		// we shall do all the work through interface; some optimizations for the fast mode - later
		long numParts;
		long numPoints;
		this->get_NumParts(&numParts);
		this->get_NumPoints(&numPoints);

		// copying parts
		for (long i = 0; i < numParts; i++)
		{
			long index;
			this->get_Part(i, &index);
			shp->InsertPart(index, &i, &vbretval);
		}

		// copying points
		for (long i = 0; i < numPoints; i++)
		{
			IPoint* pnt = NULL;
			this->get_Point(i, &pnt);
			shp->InsertPoint(pnt, &i, &vbretval);
			pnt->Release();
		}
		
		shp->put_Key(_key);
		(*retval) = shp;
	}
	return S_OK;
}

//*****************************************************************
//*		CopyFrom()
//*****************************************************************
STDMETHODIMP CShape::CopyFrom(IShape* source, VARIANT_BOOL* retVal)   
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!source)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		IShape* target = this;
		ShpfileType shpType;
		source->get_ShapeType(&shpType);
		VARIANT_BOOL vb;
		target->Create(shpType, &vb);
		
		long numParts;
		source->get_NumParts(&numParts);
		for(int i = 0; i < numParts; i++ )
		{
			long part, newIndex;
			source->get_Part(i, &part);
			target->InsertPart(part, &newIndex, &vb);
		}

		bool hasM = shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ;
		bool hasZ = shpType == SHP_POLYGONZ;

		long numPoints;
		source->get_NumPoints(&numPoints);
		double x, y, m, z;
		long pointIndex;
		for(int i = 0; i < numPoints; i++ )
		{
			source->get_XY(i, &x, &y, &vb);
			target->AddPoint(x, y, &pointIndex);
			if (hasM)
			{
				source->get_M(i, &m, &vb);
				target->put_M(i, m, &vb);
			}
			else if (hasZ)
			{
				source->get_Z(i, &z, &vb);
				target->put_Z(i, z, &vb);
			}
		}
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// **********************************************
//   Explode()
// **********************************************
// Splits multi-part shapes in the single part ones
STDMETHODIMP CShape::Explode(VARIANT* Results, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	
	std::vector<IShape*> vShapes;
	if (ExplodeCore(vShapes))
	{
		// no need to release objects as we return them to the caller
		if (Templates::Vector2SafeArray(&vShapes, Results))
			*retval = VARIANT_TRUE;
	}
	return S_OK;
}

// **********************************************
//   ExplodeCore()
// **********************************************
bool CShape::ExplodeCore(std::vector<IShape*>& vShapes)
{
	vShapes.clear();
	
	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	LONG numParts;
	this->get_NumParts(&numParts);
	if (numParts <= 1)
	{
		IShape* shp = NULL;
		this->Clone(&shp);
		vShapes.push_back(shp);
	}
	else if (shpType != SHP_POLYGON && shpType != SHP_POLYGONM && shpType != SHP_POLYGONZ)
	{
		// for every feature except polygons we just return the parts of shapes
		for (long i = 0; i < numParts; i++)
		{
			IShape* part = NULL;
			this->get_PartAsShape(i, &part);
			vShapes.push_back(part);
		}
	}
	else
	{
		bool isM = Utility::ShapeTypeIsM(shpType);
		
		// for polygons holes should be treated, the main problem here is to determine 
		// to which part the hole belong; OGR will be used for this
		OGRGeometry* geom = GeometryConverter::ShapeToGeometry(this);
		if (geom)
		{
			OGRwkbGeometryType type = geom->getGeometryType();
			if (type == wkbMultiPolygon || type == wkbMultiPolygon25D)
			{
				std::vector<OGRGeometry*> polygons;		// polygons shouldn't be deleted as they are only 
														// references to the parts of init multipolygon
				if (GeometryConverter::MultiPolygon2Polygons(geom, &polygons))
				{
					for (unsigned int i = 0; i < polygons.size(); i++)
					{
						IShape* poly = GeometryConverter::GeometryToShape(polygons[i], isM);
						if (poly)
						{
							vShapes.push_back(poly);
						}
					}
				}
			}
			else
			{
				IShape* shp = GeometryConverter::GeometryToShape(geom, isM);
				if (shp)
				{
					vShapes.push_back(shp);
				}
			}
			OGRGeometryFactory::destroyGeometry(geom);
		}
	}
	return vShapes.size() > 0;
}

/***********************************************************************/
/*			get_LabelPosition()
/***********************************************************************/
// sub-function for GenerateLabels
// returns coordinates of label and angle of segment rotation for polylines
void CShape::get_LabelPosition(tkLabelPositioning method, double& x, double& y, double& rotation, tkLineLabelOrientation orientation)
{
	x = y = 0.0;
	if (method == lpNone)
		return;

	IPoint* pnt = NULL;
	ShpfileType shpType;
	this->get_ShapeType(&shpType);
	VARIANT_BOOL vbretval;
	int segmentIndex = -1;	// for polylines

	shpType = Utility::ShapeTypeConvert2D(shpType);
	
	if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
	{
		VARIANT_BOOL vbretval;
		this->get_XY(0, &x, &y, &vbretval);
	}
	if (shpType == SHP_POLYGON)
	{
		if (method == lpCenter)				this->get_Center(&pnt);
		else if (method == lpCentroid)		this->get_Centroid(&pnt);
		else if (method == lpInteriorPoint)	this->get_InteriorPoint(&pnt);
		else
		{	
			ErrorMessage(tkINVALID_PARAMETER_VALUE); 
			return;
		}
		
		if (!pnt && method == lpInteriorPoint)
		{
			// let's calculate centroid instead
			this->get_Centroid(&pnt);
		}

		if (pnt)
		{
			pnt->get_X(&x);
			pnt->get_Y(&y);
			pnt->Release(); pnt = NULL;
		}
	}
	else if (shpType == SHP_POLYLINE)
	{
		long numPoints;
		this->get_NumPoints(&numPoints);
		if (numPoints < 2) return;

		double x1, y1, x2, y2;

		if	(method == lpFirstSegment)
		{
			this->get_XY(0, &x, &y, &vbretval);
			segmentIndex = 0;
		}
		else if (method == lpLastSegment)
		{
			this->get_XY(numPoints - 1, &x, &y, &vbretval);
			segmentIndex = numPoints - 2;
		}
		else if (method == lpMiddleSegment)
		{
			double length = 0.0;
			double halfLength, delta;
			this->get_Length(&halfLength);
			halfLength /= 2.0;
			
			long i;
			for (i =0; i < numPoints; i++)
			{
				this->get_XY(i, &x1, &y1, &vbretval);
				this->get_XY(i + 1, &x2, &y2, &vbretval);
				delta = sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));
				if (length + delta < halfLength)
					length += delta;
				else if (length + delta > halfLength)
				{
					double ratio = (halfLength - length)/ (delta);
					x = x1 + (x2 - x1) * ratio;
					y = y1 + (y2 - y1) * ratio;
					break;
				}
				else
				{
					x = x2; y = y2; break;
				}
			}
			segmentIndex = i;
		}
		else if (method == lpLongestSegement)
		{
			double maxLength = 0;
			double length;
			for (long i =0; i < numPoints - 1; i++)	
			{
				this->get_XY(i, &x1, &y1, &vbretval);
				this->get_XY(i + 1, &x2, &y2, &vbretval);
				length = sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));
				if (length > maxLength)
				{
					maxLength = length;
					segmentIndex = i;
				}
			}
			if (segmentIndex != -1)
			{
				this->get_XY(segmentIndex, &x1, &y1, &vbretval);
				this->get_XY(segmentIndex + 1, &x2, &y2, &vbretval);
			}
			x = (x1 + x2)/2.0;
			y = (y1 + y2)/2.0;
		}
		else
		{	// the method is unsupported
			ErrorMessage(tkINVALID_PARAMETER_VALUE); return;
		}

		// defining angle for a segment with label
		if (orientation == lorHorizontal)
		{
			rotation = 0.0;
		}
		else
		{
			rotation = this->get_SegmentAngle(segmentIndex) - 90;
			if (orientation == lorPerpindicular)
			{
				rotation += 90.0;
			}
		}
	}
	else if (shpType == SHP_POINT)
	{
		// just return the point itself in spite of method
		this->get_XY(0, &x, &y, &vbretval);
	}
	else if (shpType == SHP_MULTIPOINT)
	{
		// TODO: return the first point for now; maybe implement several behaviours:
		// first point, last point, point closest to center of mass;
		this->get_XY(0, &x, &y, &vbretval);
	}
	return;
}

// ********************************************************************
//		Bytes2SafeArray()				               
// ********************************************************************
//  Creates safe array with numbers of shapes as long values
//  Returns true when created safe array has elements, and false otherwise
bool Bytes2SafeArray(unsigned char* data, int size, VARIANT* arr)
{
	SAFEARRAY FAR* psa = NULL;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;	

	if( size > 0 )
	{
		rgsabound[0].cElements = size;
		psa = SafeArrayCreate( VT_UI1, 1, rgsabound);
    			
		if( psa )
		{
			unsigned char* pchar = NULL;
			SafeArrayAccessData(psa,(void HUGEP* FAR*)(&pchar));
			
			if (pchar)
				memcpy(pchar,&(data[0]),sizeof(unsigned char)*size);
			
			SafeArrayUnaccessData(psa);
			
			arr->vt = VT_ARRAY|VT_UI1;
			arr->parray = psa;
			return true;
		}
	}
	else
	{
		rgsabound[0].cElements = 0;
		psa = SafeArrayCreate( VT_UI1, 1, rgsabound);
		arr->vt = VT_ARRAY|VT_UI1;
		arr->parray = psa;
	}
	return false;
}

/***********************************************************************/
/*			ExportToBinary()
/***********************************************************************/
STDMETHODIMP CShape::ExportToBinary(VARIANT* bytesArray, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int* data = NULL;
	int contentLength;
	
	if (_useFastMode)
	{
		data = ((CShapeWrapper*)_shp)->get_ShapeData();
		contentLength = ((CShapeWrapper*)_shp)->get_ContentLength();
	}
	else
	{
		CShapeWrapperCOM* shp = ((CShapeWrapperCOM*)_shp);
		CShapeWrapper* shpNew = this->InitShapeWrapper(shp);
		data = shpNew->get_ShapeData();
		contentLength = shpNew->get_ContentLength();
		delete shpNew;
	}

	if (data)
	{
		unsigned char* buffer = reinterpret_cast<unsigned char*>(data);
		*retVal = Bytes2SafeArray(buffer, contentLength, bytesArray);
		delete[] data;
		return S_OK;
	}
	else
	{
		*retVal = NULL;
		return S_FALSE;
	}
}

//********************************************************************
//*		ImportFromBinary()
//********************************************************************
STDMETHODIMP CShape::ImportFromBinary(VARIANT bytesArray, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	
	if (bytesArray.vt != (VT_ARRAY|VT_UI1))
		return S_FALSE;
	
	unsigned char* p = NULL;
	SafeArrayAccessData(bytesArray.parray,(void HUGEP* FAR*)(&p));
	char* data = reinterpret_cast<char*>(p);

	bool result = false;
	if (_useFastMode)
	{
		 result = ((CShapeWrapper*)_shp)->put_ShapeData(data);
	}
	else
	{
		CShapeWrapperCOM* shpOld = (CShapeWrapperCOM*)_shp;
		CShapeWrapper* shp = InitShapeWrapper(shpOld);
		if (shp)
		{
			if (shp->put_ShapeData(data))
			{
				CShapeWrapperCOM* shpNew = InitComWrapper(shp);
				if (shpNew)
				{
					_shp = shpNew;
					delete shpOld;
					result = true;
				}
			}
			delete shp;
		}
	}
	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;
	SafeArrayUnaccessData(bytesArray.parray);
	return S_OK;
}

//*****************************************************************
//*		FixupShapeCore()
//*****************************************************************
bool CShape::FixupShapeCore(ShapeValidityCheck validityCheck)
{
	switch(validityCheck)	
	{
		case FirstAndLastPointOfPartMatch:
			{
				bool hasM = _shp->get_ShapeType() == SHP_POLYGONM || _shp->get_ShapeType() == SHP_POLYGONZ;
				bool hasZ = _shp->get_ShapeType() == SHP_POLYGONZ;

				ShpfileType shptype = Utility::ShapeTypeConvert2D(_shp->get_ShapeType());
				if (shptype == SHP_POLYGON)
				{
					int beg_part, end_part;
					double x1, x2, y1, y2, m;

					for(long i = 0; i < _shp->get_PartCount(); i++)
					{
						beg_part = _shp->get_PartStartPoint(i);
						end_part = _shp->get_PartEndPoint(i);
						
						_shp->get_PointXY(beg_part, x1, y1);
						_shp->get_PointXY(end_part, x2, y2);
						if (x1 != x2 || y1 != y2)
						{
							_shp->InsertPointXY(end_part + 1, x1, y1);
							
							if (hasM)
							{
								_shp->get_PointM(beg_part, m);
								_shp->put_PointM(end_part + 1, m);
							}
							if (hasZ)
							{
								_shp->get_PointZ(beg_part, m);
								_shp->put_PointZ(end_part + 1, m);
							}
							
							// the next parts should be moved a step forward
							for(long part = i + 1; part < _shp->get_PartCount(); part++)
							{
								_shp->put_PartStartPoint(part, _shp->get_PartStartPoint(part));
							}
						}
					}
				}
			}
			return true;
		default: 
			return false;		// not implemented
	}
}

//*****************************************************************
//*		FixUp()
//*****************************************************************
STDMETHODIMP CShape::FixUp(IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = NULL;
	VARIANT_BOOL vbretval = VARIANT_FALSE;

	// no points? nothing we can do.
	if (this->_shp->get_PointCount() <= 0)
	{
		return S_OK;
	}

	// polygon or polyline?
	ShpfileType ShapeType = this->_shp->get_ShapeType2D();
	if (ShapeType == SHP_POLYGON || ShapeType == SHP_POLYLINE)
	{
		// no parts? add one part.
		if (this->_shp->get_PartCount() <= 0)
		{
			IShape* TempShape = NULL;
			this->Clone(&TempShape);

			long PartIndex = 0;
			TempShape->InsertPart(0, &PartIndex, &vbretval);

            // ensure the new shape is completed fixed.
			TempShape->FixUp(retval);
			TempShape->Release();
			return S_OK;
		}
	}

	// valid shape? just copy it.
	this->get_IsValid(&vbretval);
	if (vbretval)
	{
		this->Clone(retval);
	}
	else
	{
		this->Buffer(m_globalSettings.invalidShapesBufferDistance, 30, retval);
	}

    // did we allocate memory for the fixed shape?
	if (*retval)
	{
		(*retval)->get_IsValid(&vbretval);
		if (!vbretval)
		{
			(*retval)->Release();
			(*retval) = NULL;
		}
	}
	return S_OK;
}

//*****************************************************************
//*		AddPoint()
//*****************************************************************
STDMETHODIMP CShape::AddPoint(double x, double y, long* pointIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	bool success = _shp->InsertPointXY(_shp->get_PointCount(), x, y);
	*pointIndex = success ? _shp->get_PointCount() - 1 : -1;
	return S_OK;
}

#pragma region Get point

// XY coordinates
bool CShape::get_XY(long PointIndex, double* x, double* y)
{
	if(!_shp->get_PointXY(PointIndex, *x, *y))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	else
		return true;
}

bool CShape::get_Z(long PointIndex, double* z)
{
	if(!_shp->get_PointZ(PointIndex, *z))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	else
		return true;
}

bool CShape::get_M(long PointIndex, double* m)
{
	if(!_shp->get_PointM(PointIndex, *m))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	else
		return true;
}

// XYZ coordinates
bool CShape::get_XYM(long PointIndex, double* x, double* y, double* m)
{
	if(!_shp->get_PointXY(PointIndex, *x, *y) ||
	   !_shp->get_PointM(PointIndex, *m))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	else
		return true;
}

bool CShape::get_XYZ(long PointIndex, double* x, double* y, double* z)
{
	if(!_shp->get_PointXY(PointIndex, *x, *y) ||
	   !_shp->get_PointZ(PointIndex, *z))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	else
		return true;
}

// XYZM coordinates
bool CShape::get_XYZM(long PointIndex, double& x, double& y, double& z, double& m)
{
	if(!_shp->get_PointXY(PointIndex, x, y) ||
	   !_shp->get_PointZ(PointIndex, z) ||
	   !_shp->get_PointM(PointIndex, m))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	else
		return true;
}

// extents XY
bool CShape::get_ExtentsXY(double& xMin, double& yMin, double& xMax, double& yMax)
{
	if (_shp->get_ShapeType2D() == SHP_POINT)
	{
		_shp->get_PointXY(0, xMin, yMin);
		xMax = xMin; yMax = yMin;
	}
	else
	{
		_shp->get_BoundsXY(xMin, xMax, yMin, yMax);
	}
	return true;
}

// extents XYZM
bool CShape::get_ExtentsXYZM(double& xMin, double& yMin, double& xMax, double& yMax, double& zMin, double& zMax, double& mMin, double& mMax)
{
	if (_shp->get_ShapeType2D() == SHP_POINT)
	{
		this->get_XYZM(0, xMin, yMin, zMin, mMin);
		xMax = xMin; yMax = yMin; 
		zMax = zMin; mMax = mMin;
	}
	else
	{
		_shp->get_Bounds(xMin, xMax, yMin, yMax, zMin, zMax, mMin, mMax);
	}
	return true;
}
#pragma endregion

//*****************************************************************
//*		ExportToWKT()
//*****************************************************************
STDMETHODIMP CShape::ExportToWKT(BSTR * retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	OGRGeometry* geom = GeometryConverter::ShapeToGeometry(this);
	if (geom != NULL) 
	{
		char* s;
		geom->exportToWkt(&s);
		(*retVal) = A2BSTR(s);
		OGRGeometryFactory::destroyGeometry(geom);
		delete[] s;
	}
	else {
		(*retVal) = A2BSTR("");
	}
	return S_OK;
}

//*****************************************************************
//*		ImportFromWKT()
//*****************************************************************
STDMETHODIMP CShape::ImportFromWKT(BSTR Serialized, VARIANT_BOOL *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CString ser = OLE2A(Serialized);

	OGRGeometry* oGeom = NULL;
	char* buffer = ser.GetBuffer();
	OGRErr err = OGRGeometryFactory::createFromWkt(&buffer, NULL, &oGeom);
	if (err != OGRERR_NONE || !oGeom)
	{
		ErrorMessage(tkINVALID_SHAPE);
	}
	else
	{
		// if there is a geometry collection only the first shape will be taken
		std::vector<IShape*> shapes;
		if (GeometryConverter::GeometryToShapes(oGeom, &shapes, true))
		{
			if (shapes.size() > 0 && shapes[0])
			{
				IShape* result = shapes[0];
				
				// it was an impression that polygons are imported as non-closed
				//((CShape*)result)->FixupShapeCore(ShapeValidityCheck::FirstAndLastPointOfPartMatch);

				VARIANT_BOOL vb;
				this->CopyFrom(result, &vb);
				*retVal = VARIANT_TRUE;	
			}
			for(size_t i = 0; i < shapes.size(); i++)
			{
				if (shapes[i]) shapes[i]->Release();
			}
		}
		else
		{
			ErrorMessage(tkINVALID_SHAPE);
		}
	}
	return S_OK;
}

//*****************************************************************
//*		ClosestPoints()
//*****************************************************************
STDMETHODIMP CShape::ClosestPoints(IShape* shape2, IShape** result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	GEOSGeometry* g1 = GeometryConverter::Shape2GEOSGeom(this);
	GEOSGeometry* g2 = GeometryConverter::Shape2GEOSGeom(shape2);
	if (g1 && g2)
	{
		GEOSCoordSequence* coords = GeosHelper::ClosestPoints(g1, g2);
		if (coords)
		{
			int size = GeosHelper::CoordinateSequenceSize(coords);
			if (size > 0)
			{
				std::vector<Point2D> points;
				double x, y;
				for (int i = 0; i < size; i++) {
					if (GeosHelper::CoordinateSequenceGetXY(coords, i, x, y))
						points.push_back(Point2D(x, y));
				}
				if (points.size() > 1) 
				{
					VARIANT_BOOL vb;
					CoCreateInstance( CLSID_Shape, NULL, CLSCTX_INPROC_SERVER, IID_IShape, (void**)result );
					(*result)->Create(ShpfileType::SHP_POLYLINE, &vb);
					long pointIndex;
					for(size_t i = 0; i < points.size(); i++)
					{
						(*result)->AddPoint(points[i].x, points[i].y, &pointIndex);
					}
				}
			}
			GeosHelper::DestroyCoordinateSequence(coords);
		}
		if (!(*result))
		{
			// TODO: report GEOS error code
		}
	}
	else
	{
		ErrorMessage(tkCANT_CONVERT_SHAPE_GEOS);
	}
	if (g1)	GeosHelper::DestroyGeometry(g1);
	if (g2) GeosHelper::DestroyGeometry(g2);
	return S_OK;
}

