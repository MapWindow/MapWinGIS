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
#include <StdAfx.h>
#include "Shape.h"

#include <gsl/pointers>
#include <gsl/span_ext>

#include "GeometryHelper.h"
#include "Templates.h"
#include "GeosHelper.h"
#include "GeosConverter.h"
#include "LabelsHelper.h"
#include "OgrConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// **********************************************
//		Constructor
// **********************************************
CShape::CShape()
{
	_pUnkMarshaler = nullptr;

	_key = A2BSTR("");
	_globalCallback = nullptr;
	_lastErrorCode = tkNO_ERROR;
	_isValidReason = "";

	_useFastMode = m_globalSettings.shapefileFastMode;

	_shp = ShapeUtility::CreateEmptyWrapper(!_useFastMode);

	// Missing initializer:
	_labelX = 0;
	_labelY = 0;
	_labelRotation = 0;

	gReferenceCounter.AddRef(tkInterface::idShape);
}

// **********************************************
//		Destructor
// **********************************************
CShape::~CShape()
{
	SysFreeString(_key);

	if (_shp) {
		delete _shp;
		_shp = nullptr;
	}

	if (_globalCallback) {
		_globalCallback->Release(); // TODO: Fix compile warning
	}

	gReferenceCounter.Release(tkInterface::idShape); // TODO: Fix compile warning
}

#pragma region DataConversions

// **********************************************
//   put_RawData()
// **********************************************
bool CShape::put_RawData(char* data, const int recordLength)
{
	if (data == nullptr) return false;

	IShapeWrapper* wrapper = ShapeUtility::CreateWrapper(data, recordLength, !_useFastMode);
	if (!wrapper) {
		return false;
	}

	delete _shp;
	_shp = wrapper;

	ClearLabelPositionCache();
	return true;
}

// **********************************************
//		put_FastMode
// **********************************************
void CShape::put_FastMode(const bool newValue)
{
	if (newValue != _useFastMode)
	{
		const ShpfileType shpType = _shp->get_ShapeType();

		// for M and Z types the same COM point-based wrapper is used
		if (!ShapeUtility::IsM(shpType) && !ShapeUtility::IsZ(shpType))
		{
			IShapeWrapper* wrapper = ShapeUtility::CreateWrapper(shpType, !newValue);
			_shp->CopyTo(wrapper);
			delete _shp;
			_shp = wrapper;
		}

		_useFastMode = newValue;
	}
}

#pragma endregion

#pragma region Genereal

// *************************************************************
//		get_LastErrorCode
// *************************************************************
STDMETHODIMP CShape::get_LastErrorCode(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;

	return S_OK;
}

// *************************************************************
//		get_ErrorMsg
// *************************************************************
STDMETHODIMP CShape::get_ErrorMsg(const long errorCode, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = A2BSTR(ErrorMsg(errorCode));

	return S_OK;
}

// *************************************************************
//		get/put_globalCallback
// *************************************************************
STDMETHODIMP CShape::get_GlobalCallback(ICallback** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _globalCallback;
	if (_globalCallback != nullptr)
		_globalCallback->AddRef();

	return S_OK;
}
STDMETHODIMP CShape::put_GlobalCallback(ICallback* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *************************************************************
//		get/put__key
// *************************************************************
STDMETHODIMP CShape::get_Key(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CShape::put_Key(const BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

//***************************************************************
//		ErrorMessage()								           
//***************************************************************
inline void CShape::ErrorMessage(const long errorCode)
{
	_lastErrorCode = errorCode;
	CallbackHelper::ErrorMsg("Shape", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// *************************************************************
//		Create
// *************************************************************
STDMETHODIMP CShape::Create(const ShpfileType shpType, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;

	if (shpType == SHP_MULTIPATCH)
	{
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
		*retval = VARIANT_FALSE;
		// TODO: Shouldn't we exit: return S_OK;  ??
	}

	// we shall create a new instance of wrapper each and every time
	// old data is expected to be cleared all the same
	IShapeWrapper* wrapper = ShapeUtility::CreateWrapper(shpType, !_useFastMode);
	if (!wrapper) {
		return S_OK;
	}

	if (_shp) {
		_shp->Clear();
		delete _shp;
		_shp = nullptr;
	}

	_shp = wrapper;
	ClearLabelPositionCache();

	*retval = VARIANT_TRUE;
	return S_OK;
}

// **********************************************************
//		get_ShapeType
// **********************************************************
STDMETHODIMP CShape::get_ShapeType(ShpfileType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _shp->get_ShapeType();
	return S_OK;
}

// *************************************************************
//		put_ShapeType
// *************************************************************
STDMETHODIMP CShape::put_ShapeType(const ShpfileType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const ShapeWrapperType type = _shp->get_WrapperType();
	const ShapeWrapperType newType = ShapeUtility::GetShapeWrapperType(newVal, !_useFastMode);

	if (type == newType)
	{
		if (!_shp->put_ShapeType(newVal)) {
			ErrorMessage(_shp->get_LastErrorCode());
		}
		else {
			ClearLabelPositionCache();
		}
	}
	else
	{
		// change wrapper type if needed
		IShapeWrapper* shpNew = ShapeUtility::CreateWrapper(newVal, !_useFastMode);

		if (!shpNew->put_ShapeType(newVal)) {
			ErrorMessage(shpNew->get_LastErrorCode());
		}
		else {
			delete _shp;
			_shp = shpNew;
			ClearLabelPositionCache();
		}
	}

	return S_OK;
}

//*****************************************************************
//*		ValidateBasics()
//*****************************************************************
bool CShape::ValidateBasics(ShapeValidityCheck& failedCheck, CString& errMsg)
{
	if (_shp->get_PointCount() == 0)
	{
		errMsg = "Shape hasn't got points";
		failedCheck = ShapeValidityCheck::NoPoints;
		return false;
	}

	const ShpfileType shptype = ShapeUtility::Convert2D(_shp->get_ShapeType());

	if (shptype == SHP_POLYGON || shptype == SHP_POLYLINE)
	{
		if (_shp->get_PartCount() == 0)
		{
			errMsg = "Shape hasn't got parts";
			failedCheck = ShapeValidityCheck::NoParts;
			return false;
		}
	}

	int minPointCount = 0;
	if (shptype == SHP_POLYLINE) minPointCount = 2;
	if (shptype == SHP_POLYGON) minPointCount = 4;   // including closing one

	if (shptype == SHP_POLYLINE || shptype == SHP_POLYGON)
	{
		if (_shp->get_PointCount() < minPointCount)
		{
			errMsg = "Shape doesn't have enough points for its type.";
			failedCheck = ShapeValidityCheck::NotEnoughPoints;
			return false;
		}
		for (long i = 0; i < _shp->get_PartCount(); i++)
		{
			const int begPart = _shp->get_PartStartPoint(i);
			const int endPart = _shp->get_PartEndPoint(i);
			const int count = endPart - begPart + 1;
			if (count < minPointCount)
			{
				errMsg = "A part doesn't have enough points for a given shape type.";
				failedCheck = ShapeValidityCheck::EmptyParts;
				return false;
			}
		}
	}

	if (shptype == SHP_POLYGON)
	{
		double x1, x2, y1, y2;

		for (long i = 0; i < _shp->get_PartCount(); i++)
		{
			const int begPart = _shp->get_PartStartPoint(i);
			const int endPart = _shp->get_PartEndPoint(i);

			_shp->get_PointXY(begPart, x1, y1);
			_shp->get_PointXY(endPart, x2, y2);

			if (x1 != x2 || y1 != y2)
			{
				errMsg = "The first and the last point of the polygon part must be the same";
				failedCheck = ShapeValidityCheck::FirstAndLastPointOfPartMatch;
				return false;
			}
		}
	}

	// checking the clockwise-order
	if (shptype == SHP_POLYGON)
	{
		VARIANT_BOOL isClockwise;
		const int partCount = _shp->get_PartCount();
		if (partCount == 1)
		{
			this->get_PartIsClockWise(0, &isClockwise);
			if (!isClockwise)
			{
				VARIANT_BOOL ret;
				double x, y;
				this->get_XY(0, &x, &y, &ret);
				errMsg.Format("Polygon must be clockwise [%f %f]", x, y);
				failedCheck = ShapeValidityCheck::DirectionOfPolyRings;
				return false;
			}
		}
	}
	return true;
}

// *************************************************************
//			get_IsValid	
// *************************************************************
// Checking validity of the geometry
STDMETHODIMP CShape::get_IsValid(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;

	ShapeValidityCheck validityCheck;
	if (!ValidateBasics(validityCheck, _isValidReason)) {
		return S_OK;
	}

	// -----------------------------------------------
	//  check through GEOS (common for both modes)
	// -----------------------------------------------
	const GEOSGeom hGeosGeom = GeosConverter::ShapeToGeom(this);
	if (hGeosGeom == nullptr)
	{
		_isValidReason = "Failed to convert to GEOS geometry";
		return S_OK;
	}

	if (!GeosHelper::IsValid(hGeosGeom))
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
STDMETHODIMP CShape::get_NumParts(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _shp->get_PartCount();
	return S_OK;
}

// *************************************************************
//		get_Part
// *************************************************************
STDMETHODIMP CShape::get_Part(const long partIndex, long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _shp->get_PartStartPoint(partIndex);
	if (*pVal == -1)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//		put_Part
// *************************************************************
STDMETHODIMP CShape::put_Part(const long partIndex, const long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!_shp->put_PartStartPoint(partIndex, newVal)) {
		ErrorMessage(_shp->get_LastErrorCode());
	}
	else {
		ClearLabelPositionCache();
	}
	return S_OK;
}

//***********************************************************************
//*		get_EndOfPart()
//***********************************************************************
//  Returns last point of the part
STDMETHODIMP CShape::get_EndOfPart(const long partIndex, long* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = _shp->get_PartEndPoint(partIndex);
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
};

// *************************************************************
//		InsertPart
// *************************************************************
STDMETHODIMP CShape::InsertPart(const long pointIndex, long* partIndex, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = static_cast<VARIANT_BOOL>(_shp->InsertPart(*partIndex, pointIndex));
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	ClearLabelPositionCache();
	return S_OK;
}

// *************************************************************
//		DeletePart
// *************************************************************
STDMETHODIMP CShape::DeletePart(const long partIndex, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = static_cast<VARIANT_BOOL>(_shp->DeletePart(partIndex));
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	ClearLabelPositionCache();
	return S_OK;
}

//*********************************************************************
//		CShape::PartIsClockWise()									
//*********************************************************************
//  Returns true if points of shape's part are in clockwise direction
//  and false otherwise
STDMETHODIMP CShape::get_PartIsClockWise(const long partIndex, VARIANT_BOOL* retval)
{
	*retval = VARIANT_FALSE;

	long numParts, numPoints;
	this->get_NumParts(&numParts);

	if (partIndex >= numParts || partIndex < 0)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	long begPart, endPart;
	this->get_NumPoints(&numPoints);
	this->get_Part(partIndex, &begPart);

	if (numParts - 1 > partIndex)
		this->get_Part(partIndex + 1, &endPart);
	else
		endPart = numPoints;

	//lsu: we need to calcuate area of part to determine clockwiseness
	double x1, x2, y1, y2;
	VARIANT_BOOL vbretval;
	double area = 0;

	for (long i = begPart; i < endPart - 1; i++)
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
STDMETHODIMP CShape::ReversePointsOrder(const long partIndex, VARIANT_BOOL* retval)
{
	*retval = VARIANT_FALSE;

	long numParts, numPoints;
	this->get_NumParts(&numParts);

	if (partIndex >= numParts || partIndex < 0)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	long begPart, endPart;
	this->get_NumPoints(&numPoints);
	this->get_Part(partIndex, &begPart);

	if (numParts - 1 > partIndex)
	{
		this->get_Part(partIndex + 1, &endPart);
	}
	else
	{
		endPart = numPoints;
	}

	_shp->ReversePoints(begPart, endPart);

	*retval = VARIANT_TRUE;
	ClearLabelPositionCache();
	return S_OK;
}

// ***************************************************************
//		PartAsShape()
// ***************************************************************
//  Returns part of the shape as new shape; new points are created
STDMETHODIMP CShape::get_PartAsShape(const long partIndex, IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long begPart, endPart;
	this->get_Part(partIndex, &begPart);
	this->get_EndOfPart(partIndex, &endPart);

	if (begPart == -1 || endPart == -1)
	{
		*retval = nullptr;
		return S_OK;
	}

	IShape* shp;
	ComHelper::CreateShape(&shp);

	const ShpfileType shptype = _shp->get_ShapeType();
	shp->put_ShapeType(shptype);

	long part = 0;
	VARIANT_BOOL vbretval;
	shp->InsertPart(0, &part, &vbretval);

	long cnt = 0;
	IPoint* pntOld;
	IPoint* pntNew;
	for (int i = begPart; i <= endPart; i++)
	{
		this->get_Point(i, &pntOld);
		pntOld->Clone(&pntNew);
		shp->InsertPoint(pntNew, &cnt, &vbretval);
		pntOld->Release();
		pntNew->Release();
		cnt++;
	}
	*retval = shp;
	return S_OK;
}

#pragma endregion	

#pragma region Points
// **********************************************************
//		get_NumPoints
// **********************************************************
STDMETHODIMP CShape::get_NumPoints(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _shp->get_PointCount();
	return S_OK;
}

// *************************************************************
//		get_Point
// *************************************************************
STDMETHODIMP CShape::get_Point(const long pointIndex, IPoint** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _shp->get_Point(pointIndex);
	if (*pVal == nullptr)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	return S_OK;
}

// *************************************************************
//		put_Point
// *************************************************************
STDMETHODIMP CShape::put_Point(const long pointIndex, IPoint* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal == nullptr)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
	}
	else
	{
		double x, y;
		newVal->get_X(&x);
		newVal->get_Y(&y);
		newVal->Release();
		if (!_shp->put_PointXY(pointIndex, x, y))
		{
			ErrorMessage(_shp->get_LastErrorCode());
		}
		ClearLabelPositionCache();
	}
	return S_OK;
}

// *************************************************************
//		InsertPoint
// *************************************************************
STDMETHODIMP CShape::InsertPoint(IPoint* newPoint, long* pointIndex, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = static_cast<VARIANT_BOOL>(_shp->InsertPoint(*pointIndex, newPoint));
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	ClearLabelPositionCache();
	return S_OK;
}

// *************************************************************
//		DeletePoint
// *************************************************************
STDMETHODIMP CShape::DeletePoint(const long pointIndex, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = static_cast<VARIANT_BOOL>(_shp->DeletePoint(pointIndex));
	if (*retval == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	ClearLabelPositionCache();
	return S_OK;
}

// *************************************************************
//		get_XY
// *************************************************************
STDMETHODIMP CShape::get_XY(const long pointIndex, double* x, double* y, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = get_XY(pointIndex, x, y) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// **********************************************
//   put_XY()
// **********************************************
STDMETHODIMP CShape::put_XY(const LONG pointIndex, const double x, const double y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = _shp->put_PointXY(pointIndex, x, y) ? VARIANT_TRUE : VARIANT_FALSE;
	if (*retVal == VARIANT_FALSE)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	//else
	ClearLabelPositionCache();
	return S_OK;
}

// **********************************************
//   put_M()
// **********************************************
STDMETHODIMP CShape::put_M(const LONG pointIndex, const double m, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = _shp->put_PointM(pointIndex, m) ? VARIANT_TRUE : VARIANT_FALSE;
	if (!*retVal)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	//else
	ClearLabelPositionCache();
	return S_OK;
}

// **********************************************
//   put_Z()
// **********************************************
STDMETHODIMP CShape::put_Z(const LONG pointIndex, const double z, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = _shp->put_PointZ(pointIndex, z) ? VARIANT_TRUE : VARIANT_FALSE;
	if (!*retVal)
	{
		ErrorMessage(_shp->get_LastErrorCode());
	}
	//else
	ClearLabelPositionCache();
	return S_OK;
}

// *************************************************************
//		get_Z
// *************************************************************
STDMETHODIMP CShape::get_Z(const long pointIndex, double* z, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = get_Z(pointIndex, z) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		get_M
// *************************************************************
STDMETHODIMP CShape::get_M(const long pointIndex, double* m, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = get_M(pointIndex, m) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

#pragma endregion	

#pragma region SpatialProperties

// *****************************************************************
//			get_Center()
// *****************************************************************
//  Returns center of shape (crossing of diagonals of bounding box)
STDMETHODIMP CShape::get_Center(IPoint** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
	double x, y;

	//CShapeWrapper* shp = (CShapeWrapper*)_shp;
	//std::vector<pointEx>* points = &shp->_points;

	const int pointCount = _shp->get_PointCount();
	for (int i = 0; i < pointCount; i++)
	{
		_shp->get_PointXY(i, x, y);

		if (i == 0)
		{
			xMin = x; xMax = x;
			yMin = y; yMax = y;
		}
		else
		{
			if (x < xMin) xMin = x;
			else if (x > xMax) xMax = x;
			if (y < yMin) yMin = y;
			else if (y > yMax) yMax = y;
		}
	}

	x = xMin + (xMax - xMin) / 2;
	y = yMin + (yMax - yMin) / 2;

	IPoint* pnt = nullptr;
	CoCreateInstance(CLSID_Point, nullptr, CLSCTX_INPROC_SERVER, IID_IPoint, reinterpret_cast<void**>(&pnt));
	pnt->put_X(x); pnt->put_Y(y);
	*pVal = pnt;

	return S_OK;
}

// *************************************************************
//		get_Length
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Length(double* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShpfileType shptype;
	this->get_ShapeType(&shptype);

	if (shptype != SHP_POLYLINE && shptype != SHP_POLYLINEZ && shptype != SHP_POLYLINEM)
	{
		*pVal = 0.0;
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	double length = 0.0;
	long numParts = 0, numPoints = 0;
	this->get_NumParts(&numParts);
	this->get_NumPoints(&numPoints);

	long begPart = 0;
	long endPart = 0;
	for (int j = 0; j < numParts; j++)
	{
		this->get_Part(j, &begPart);
		if (numParts - 1 > j)
		{
			this->get_Part(j + 1, &endPart);
		}
		else
		{
			endPart = numPoints;
		}

		double oneX, oneY;
		double twoX, twoY;
		VARIANT_BOOL vbretval;

		for (int i = begPart; i < endPart - 1; i++)
		{
			this->get_XY(i, &oneX, &oneY, &vbretval);
			this->get_XY(i + 1, &twoX, &twoY, &vbretval);
			length += sqrt(pow(twoX - oneX, 2) + pow(twoY - oneY, 2));
		}
	}

	*pVal = length;
	return S_OK;
}

// *************************************************************
//		get_Perimeter
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Perimeter(double* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShpfileType shptype;
	this->get_ShapeType(&shptype);

	if (shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM)
	{
		*pVal = 0.0;
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	double perimeter = 0.0;
	long numParts = 0, numPoints = 0;
	this->get_NumParts(&numParts);
	this->get_NumPoints(&numPoints);

	long begPart = 0;
	long endPart = 0;
	for (int j = 0; j < numParts; j++)
	{
		this->get_Part(j, &begPart);
		if (numParts - 1 > j)
			this->get_Part(j + 1, &endPart);
		else
			endPart = numPoints;

		double px1, py1, px2, py2;
		VARIANT_BOOL vbretval;
		for (int i = begPart; i < endPart - 1; i++)
		{
			this->get_XY(i, &px1, &py1, &vbretval);
			this->get_XY(i + 1, &px2, &py2, &vbretval);
			perimeter += sqrt(pow(px2 - px1, 2) + pow(py2 - py1, 2));
		}
	}

	*pVal = perimeter;
	return S_OK;
}

// *************************************************************
//		get_Extents
// *************************************************************
STDMETHODIMP CShape::get_Extents(IExtents** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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

	IExtents* bBox;
	ComHelper::CreateExtents(&bBox);
	bBox->SetBounds(xMin, yMin, zMin, xMax, yMax, zMax);
	bBox->SetMeasureBounds(mMin, mMax);
	*pVal = bBox;
	return S_OK;
}

// *************************************************************
//		get_Area
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Area(double* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShpfileType shptype;
	this->get_ShapeType(&shptype);

	if (shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM)
	{
		*pVal = 0.0;
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	double totalArea = 0.0;

	long numParts = 0, numPoints = 0;
	this->get_NumParts(&numParts);
	this->get_NumPoints(&numPoints);

	if (numParts > 1)
	{
		//Create new polygons from the different parts (simplified on 2/10/06 by Angela Hillier)
		std::deque<Poly> allPolygons;

		long begPart = 0;
		long endPart = 0;
		for (int j = 0; j < numParts; j++)
		{
			this->get_Part(j, &begPart);
			if (numParts - 1 > j)
				this->get_Part(j + 1, &endPart);
			else
				endPart = numPoints;

			double px, py;
			Poly polygon;
			for (int i = begPart; i < endPart; i++)
			{
				VARIANT_BOOL vbretval;
				this->get_XY(i, &px, &py, &vbretval);
				polygon.polyX.push_back(px);
				polygon.polyY.push_back(py);
			}
			allPolygons.push_back(polygon);
		}

		const int allPolygonsSize = gsl::narrow_cast<int>(allPolygons.size());
		for (int p = 0; p < allPolygonsSize; p++)
		{
			double indivArea = 0.0;

			//Calculate individual area of each part
			const int polyXSize = gsl::narrow_cast<int>(gsl::at(allPolygons, p).polyX.size());
			for (int a = 0; a < polyXSize - 1; a++)
			{
				//const double oneX = allPolygons[p].polyX[a];
				//const double oneY = allPolygons[p].polyY[a];
				//const double twoX = allPolygons[p].polyX[a + 1];
				//const double twoY = allPolygons[p].polyY[a + 1];
				const double oneX = gsl::at(allPolygons[p].polyX, a);
				const double oneY = gsl::at(allPolygons[p].polyY, a);
				const double twoX = gsl::at(allPolygons[p].polyX, static_cast<gsl::index>(a) + 1);
				const double twoY = gsl::at(allPolygons[p].polyY, static_cast<gsl::index>(a) + 1);

				const double trapArea = (oneX * twoY) - (twoX * oneY);
				indivArea += trapArea;
			}

			totalArea += indivArea;
		}
		totalArea = fabs(totalArea) * .5;
		allPolygons.clear();
	}
	else
	{
		double oneX, oneY, twoX, twoY;
		VARIANT_BOOL vbretval;
		for (int i = 0; i <= numPoints - 2; i++)
		{
			this->get_XY(i, &oneX, &oneY, &vbretval);
			this->get_XY(i + 1, &twoX, &twoY, &vbretval);
			const double trapArea = (oneX * twoY) - (twoX * oneY);
			totalArea += trapArea;
		}
		totalArea = fabs(totalArea) * .5;
	}

	*pVal = totalArea;
	return S_OK;
}

// *************************************************************
//		get_Centroid
// *************************************************************
// TODO: it's possible to optimize it for fast mode
STDMETHODIMP CShape::get_Centroid(IPoint** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const ShpfileType shptype = _shp->get_ShapeType();
	if (shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM)
	{
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	long numPoints, numParts;
	long begPart, endPart;
	double totalArea = 0.0;
	double xSum, ySum;
	double xShift = 0.0;
	double yShift = 0.0;
	double xPart = 0, yPart = 0;	// centroid of part
	double x = 0.0;			// final centroid
	double y = 0.0;

	double xMin, xMax, yMin, yMax;
	this->get_ExtentsXY(xMin, yMin, xMax, yMax);
	if (xMin < 0 && xMax > 0) xShift = xMax - xMin;
	if (yMin < 0 && yMax > 0) yShift = yMax - yMin;

	this->get_NumPoints(&numPoints);
	this->get_NumParts(&numParts);

	for (int j = 0; j < numParts; j++)
	{
		double area = xSum = ySum = 0.0;

		this->get_Part(j, &begPart);
		if (numParts - 1 > j)
			this->get_Part(j + 1, &endPart);
		else
			endPart = numPoints;

		double oneX, oneY, twoX, twoY;
		VARIANT_BOOL vbretval;
		for (int i = begPart; i < endPart - 1; i++)
		{
			this->get_XY(i, &oneX, &oneY, &vbretval);
			this->get_XY(i + 1, &twoX, &twoY, &vbretval);

			const double cProduct = (oneX + xShift) * (twoY + yShift) - (twoX + xShift) * (oneY + yShift);
			xSum += ((oneX + xShift) + (twoX + xShift)) * cProduct;
			ySum += ((oneY + yShift) + (twoY + yShift)) * cProduct;

			area += (oneX * twoY) - (twoX * oneY);
		}

		area = fabs(area) * .5;
		if (area != 0)
		{
			xPart = xSum / (6 * area);
			yPart = ySum / (6 * area);
		}

		// corrects for shapes in quadrants other than 1 or clockwise/counter-clocwise sign errors
		if (xMax + xShift < 0 && xPart > 0)  xPart = -1 * xPart;
		if (xMin + xShift > 0 && xPart < 0)  xPart = -1 * xPart;
		if (yMax + yShift < 0 && yPart > 0)  yPart = -1 * yPart;
		if (yMin + yShift > 0 && yPart < 0)  yPart = -1 * yPart;

		// Adjust centroid if we calculated it using an X or Y shift
		xPart -= xShift;
		yPart -= yShift;

		x += xPart * area;
		y += yPart * area;
		totalArea += area;
	}
	if (totalArea != 0)
	{
		x = x / totalArea;
		y = y / totalArea;
	}

	IPoint* pnt;
	ComHelper::CreatePoint(&pnt);

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
STDMETHODIMP CShape::Relates(IShape* shape, const tkSpatialRelation relation, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;

	if (shape == nullptr)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	// if extents don't cross, no need to seek further
	if (!(relation == srDisjoint))
	{
		if (GeometryHelper::RelateExtents(this, shape) == tkExtentsRelation::erNone)
			return S_OK;
	}

	OGRGeometry* oGeom1 = OgrConverter::ShapeToGeometry(this);
	if (oGeom1 == nullptr) return S_OK;

	OGRGeometry* oGeom2 = OgrConverter::ShapeToGeometry(shape);
	if (oGeom2 == nullptr)
	{
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}

	OGRBoolean res = 0;

	switch (relation)
	{
	case srContains:	res = oGeom1->Contains(oGeom2); break;
	case srCrosses:		res = oGeom1->Crosses(oGeom2); break;
	case srDisjoint:	res = oGeom1->Disjoint(oGeom2); break;
	case srEquals:		res = oGeom1->Equals(oGeom2); break;
	case srIntersects:	res = oGeom1->Intersects(oGeom2); break;
	case srOverlaps:	res = oGeom1->Overlaps(oGeom2); break;
	case srTouches:		res = oGeom1->Touches(oGeom2); break;
	case srWithin:		res = oGeom1->Within(oGeom2); break;
	case srCovers: break; // TODO: Implement (MWGIS-307)
	case srCoveredBy: break; // TODO: Implement (MWGIS-308)
	}

	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);

	*retval = res == 0 ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *************************************************************
//		Relations
// *************************************************************
STDMETHODIMP CShape::Contains(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srContains, retval);
	return S_OK;
}
STDMETHODIMP CShape::Crosses(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srCrosses, retval);
	return S_OK;
}
STDMETHODIMP CShape::Disjoint(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srDisjoint, retval);
	return S_OK;
}
STDMETHODIMP CShape::Equals(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srEquals, retval);
	return S_OK;
}
STDMETHODIMP CShape::Intersects(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srIntersects, retval);
	return S_OK;
}
STDMETHODIMP CShape::Overlaps(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srOverlaps, retval);
	return S_OK;
}
STDMETHODIMP CShape::Touches(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srTouches, retval);
	return S_OK;
}
STDMETHODIMP CShape::Within(IShape* shape, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srWithin, retval);
	return S_OK;
}
STDMETHODIMP CShape::Covers(IShape* shape, VARIANT_BOOL* retval)
{
	// TODO: MWGIS-307
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srCovers, retval);
	return S_OK;
}
STDMETHODIMP CShape::CoveredBy(IShape* shape, VARIANT_BOOL* retval)
{
	// TODO: MWGIS-308
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->Relates(shape, srCoveredBy, retval);
	return S_OK;
}

// *************************************************************
//		Clip
// *************************************************************
STDMETHODIMP CShape::Clip(IShape* shape, const tkClipOperation operation, IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = nullptr;

	if (shape == nullptr)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	if (operation == clClip || operation == clIntersection)
	{
		if (GeometryHelper::RelateExtents(this, shape) == tkExtentsRelation::erNone)
			return S_OK;
	}

	OGRGeometry* oGeom1 = OgrConverter::ShapeToGeometry(this);
	if (oGeom1 == nullptr)
		return S_OK;

	const OGRwkbGeometryType oReturnType = oGeom1->getGeometryType();

	OGRGeometry* oGeom2 = OgrConverter::ShapeToGeometry(shape);
	if (oGeom2 == nullptr)
	{
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}

	OGRGeometry* oGeom3 = nullptr;

	switch (operation)
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
		oGeom3 = oGeom1->SymDifference(oGeom2);
		break;
	}

	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);

	if (oGeom3 == nullptr)
		return S_OK;

	ShpfileType shpType;
	this->get_ShapeType(&shpType);
	IShape* shp = OgrConverter::GeometryToShape(oGeom3, ShapeUtility::IsM(shpType), oReturnType);

	OGRGeometryFactory::destroyGeometry(oGeom3);

	*retval = shp;
	return S_OK;
}

// *************************************************************
//		Distance
// *************************************************************
STDMETHODIMP CShape::Distance(IShape* shape, double* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = 0.0;

	if (shape == nullptr)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	OGRGeometry* oGeom1 = OgrConverter::ShapeToGeometry(this);
	if (oGeom1 == nullptr) return S_OK;

	OGRGeometry* oGeom2 = OgrConverter::ShapeToGeometry(shape);
	if (oGeom2 == nullptr)
	{
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}

	*retval = oGeom1->Distance(oGeom2);


	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);
	return S_OK;
}

OGRGeometry* DoBuffer(const DOUBLE distance, const long nQuadSegments, const gsl::not_null<OGRGeometry*> geomSource)
{
	// TODO: Fix compile warnings:
	__try
	{
		return geomSource->Buffer(distance, nQuadSegments);
	}
	__except (1)
	{
		return nullptr;
	}
}

// *************************************************************
//		Buffer
// *************************************************************
STDMETHODIMP CShape::Buffer(const DOUBLE distance, const long nQuadSegments, IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = nullptr;

	OGRGeometry* oGeom1 = OgrConverter::ShapeToGeometry(this);
	if (oGeom1 == nullptr) return S_OK;

	OGRGeometry* oGeom2 = DoBuffer(distance, nQuadSegments, oGeom1);

	OGRGeometryFactory::destroyGeometry(oGeom1);
	if (oGeom2 == nullptr)	return S_OK;

	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	IShape* shp = OgrConverter::GeometryToShape(oGeom2, ShapeUtility::IsM(shpType));

	*retval = shp;
	OGRGeometryFactory::destroyGeometry(oGeom2);

	return S_OK;
}

// *************************************************************
//		BufferWithParams
// *************************************************************
STDMETHODIMP CShape::BufferWithParams(const DOUBLE distance, const LONG numSegments, const VARIANT_BOOL singleSided,
	const tkBufferCap capStyle, const tkBufferJoin joinStyle, const DOUBLE mitreLimit, IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = nullptr;

	const GEOSGeom gs = GeosConverter::ShapeToGeom(this);
	if (!gs)
	{
		ErrorMessage(tkCANT_CONVERT_SHAPE_GEOS);
		return S_OK;
	}

	GEOSBufferParams* params = GeosHelper::BufferParams_create();
	if (!params) {
		GeosHelper::DestroyGeometry(gs);
		ErrorMessage(tkFAILED_CREATE_BUFFER_PARAMS);
		return S_OK;
	}

	GeosHelper::BufferParams_setQuadrantSegments(params, numSegments);
	GeosHelper::BufferParams_setMitreLimit(params, mitreLimit);
	GeosHelper::BufferParams_setEndCapStyle(params, capStyle);
	GeosHelper::BufferParams_setJoinStyle(params, joinStyle);
	GeosHelper::BufferParams_setSingleSided(params, singleSided ? true : false);

	GEOSGeometry* gsNew = GeosHelper::BufferWithParams(gs, params, distance);

	GeosHelper::DestroyGeometry(gs);
	GeosHelper::BufferParams_destroy(params);

	if (gsNew)
	{
		ShpfileType shpType;
		get_ShapeType(&shpType);

		vector<IShape*> shapes;
		if (GeosConverter::GeomToShapes(gsNew, &shapes, ShapeUtility::IsM(shpType)))
		{
			if (!shapes.empty()) {
				*retVal = gsl::at(shapes, 0);
				const int numShapes = shapes.size();
				for (size_t i = 1; i < numShapes; i++) {
					gsl::at(shapes, i)->Release();
				}
			}
		}
		GeosHelper::DestroyGeometry(gsNew);
	}
	return S_OK;
}

// *************************************************************
//		Boundary
// *************************************************************
STDMETHODIMP CShape::Boundary(IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = nullptr;

	OGRGeometry* oGeom1 = OgrConverter::ShapeToGeometry(this);
	if (oGeom1 == nullptr) return S_OK;

	OGRGeometry* oGeom2 = oGeom1->Boundary();
	OGRGeometryFactory::destroyGeometry(oGeom1);

	if (oGeom2 == nullptr)	return S_OK;

	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	IShape* shp = OgrConverter::GeometryToShape(oGeom2, ShapeUtility::IsM(shpType));

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

	*retval = nullptr;

	OGRGeometry* oGeom1 = OgrConverter::ShapeToGeometry(this);
	if (oGeom1 == nullptr) return S_OK;

	OGRGeometry* oGeom2 = oGeom1->ConvexHull();
	OGRGeometryFactory::destroyGeometry(oGeom1);

	if (oGeom2 == nullptr)	return S_OK;

	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	IShape* shp = OgrConverter::GeometryToShape(oGeom2, ShapeUtility::IsM(shpType));

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
 *      Returns intersection of 2 shapes as an safearray of shapes
/***********************************************************************/
STDMETHODIMP CShape::GetIntersection(IShape* shape, VARIANT* results, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;

	if (shape == nullptr)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	if (GeometryHelper::RelateExtents(this, shape) == tkExtentsRelation::erNone)
		return S_OK;

	OGRGeometry* oGeom1 = OgrConverter::ShapeToGeometry(this);
	if (oGeom1 == nullptr) return S_OK;

	OGRGeometry* oGeom2 = OgrConverter::ShapeToGeometry(shape);
	if (oGeom2 == nullptr)
	{
		OGRGeometryFactory::destroyGeometry(oGeom1);
		return S_OK;
	}

	OGRGeometry* oGeom3 = oGeom1->Intersection(oGeom2);

	OGRGeometryFactory::destroyGeometry(oGeom1);
	OGRGeometryFactory::destroyGeometry(oGeom2);

	if (oGeom3 == nullptr) return S_OK;

	ShpfileType shpType;
	this->get_ShapeType(&shpType);

	std::vector<IShape*> vShapes;
	if (!OgrConverter::GeometryToShapes(oGeom3, &vShapes, ShapeUtility::IsM(shpType)))
		return S_OK;  // TODO Should oGeom3 not be destroyed?

	OGRGeometryFactory::destroyGeometry(oGeom3);

	if (!vShapes.empty())
	{
		if (Templates::Vector2SafeArray(&vShapes, results))
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
	*retval = nullptr;

	double xMin, xMax, yMin, yMax;
	this->get_ExtentsXY(xMin, yMin, xMax, yMax);

	OGRGeometry* oGeom = OgrConverter::ShapeToGeometry(this);
	if (oGeom == nullptr)
	{
		ErrorMessage(tkCANT_CONVERT_SHAPE_GEOS);
		return S_OK;
	}

	const auto oLine = static_cast<OGRLineString*>(OGRGeometryFactory::createGeometry(wkbLineString));
	oLine->addPoint(xMin, (yMax + yMin) / 2);
	oLine->addPoint(xMax, (yMax + yMin) / 2);

	OGRGeometry* oResult = oGeom->Intersection(oLine);
	if (oResult == nullptr)
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

	const ShpfileType shptype = ShapeUtility::Convert2D(_shp->get_ShapeType());

	if (shptype == SHP_POLYLINE)
	{
		ErrorMessage(tkMETHOD_NOT_IMPLEMENTED);
	}
	if (shptype == SHP_POLYGON)
	{
		const OGRwkbGeometryType oType = oResult->getGeometryType();
		if (oType == wkbLineString || oType == wkbLineString25D)
		{
			const OGRLineString* oSubLine = static_cast<OGRLineString*>(oResult);
			x = (oSubLine->getX(0) + oSubLine->getX(1)) / 2;
			y = (oSubLine->getY(0) + oSubLine->getY(1)) / 2;
		}
		else if (oType == wkbGeometryCollection || oType == wkbGeometryCollection25D ||
			oType == wkbMultiLineString || oType == wkbMultiLineString25D)
		{
			double maxLength = -1;

			const auto oColl = static_cast<OGRGeometryCollection*>(oResult);
			for (long i = 0; i < oColl->getNumGeometries(); i++)
			{
				OGRGeometry* oPart = oColl->getGeometryRef(i);
				if (oPart->getGeometryType() == wkbLineString || oPart->getGeometryType() == wkbLineString25D)
				{
					const auto oSubLine = static_cast<OGRLineString*>(oPart);
					const double length = oSubLine->get_Length();
					if (length > maxLength)
					{
						x = (oSubLine->getX(0) + oSubLine->getX(1)) / 2;
						y = (oSubLine->getY(0) + oSubLine->getY(1)) / 2;
						maxLength = length;
					}
				}
			}
		}
	}

	if (x != 0 || y != 0)
	{
		CoCreateInstance(CLSID_Point, nullptr, CLSCTX_INPROC_SERVER, IID_IPoint, reinterpret_cast<void**>(retval));
		(*retval)->put_X(x);
		(*retval)->put_Y(y);
	}

	// cleaning
	if (oLine)	 OGRGeometryFactory::destroyGeometry(oLine);
	if (oGeom)	 OGRGeometryFactory::destroyGeometry(oGeom);
	if (oResult) OGRGeometryFactory::destroyGeometry(oResult);
	return S_OK;
}
#pragma endregion

#pragma region Serialization
// *************************************************************
//		SerializeToString
// *************************************************************
STDMETHODIMP CShape::SerializeToString(BSTR* serialized)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//	fast editing mode
	const ShpfileType shptype = _shp->get_ShapeType();

	CString builder = "";
	char cbuf[20];
	double dbuf;
	double dbuf1;
	_itoa(shptype, cbuf, 10); // TODO: Fix compile warning
	builder.Append(cbuf);
	builder.Append(";");

	const int numParts = _shp->get_PartCount();
	for (int i = 0; i < numParts; i++)
	{
		sprintf(cbuf, "%d;", _shp->get_PartStartPoint(i));  // TODO: Fix compile warning
		builder.Append(cbuf);
	}

	const int numPoints = _shp->get_PointCount();
	for (int i = 0; i < numPoints; i++)
	{
		_shp->get_PointXY(i, dbuf, dbuf1);

		sprintf(cbuf, "%f|", dbuf); // TODO: Fix compile warning
		builder.Append(cbuf);

		sprintf(cbuf, "%f|", dbuf1); // TODO: Fix compile warning
		builder.Append(cbuf);

		if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM ||
			shptype == SHP_MULTIPOINTZ || shptype == SHP_POLYGONZ || shptype == SHP_POLYLINEZ)
		{
			_shp->get_PointZ(i, dbuf);
			sprintf(cbuf, "%f|", dbuf); // TODO: Fix compile warning
			builder.Append(cbuf);
		}
		if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM)
		{
			_shp->get_PointM(i, dbuf);
			sprintf(cbuf, "%f|", dbuf); // TODO: Fix compile warning
			builder.Append(cbuf);
		}
	}
	*serialized = builder.AllocSysString();

	return S_OK;
}

// *************************************************************
//		CreateFromString
// *************************************************************
STDMETHODIMP CShape::CreateFromString(BSTR serialized, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	VARIANT_BOOL rt;
	// convert BSTR to CString
	const CString ser(serialized);
	CString next = ser.Mid(ser.Find(";") + 1);

	const auto newShpType = static_cast<ShpfileType>(atoi(ser.Left(ser.Find(";")))); // TODO: Fix compile warning

	//Test the ShpType
	if (newShpType == SHP_MULTIPATCH)
	{
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	//Discard the old shape information
	_shp->Clear();

	_shp->put_ShapeType(newShpType);

	while (next.Find(";") != -1)
	{
		long nextId = _shp->get_PartCount();
		this->InsertPart(atol(next.Left(next.Find(";"))), &nextId, &rt);
		next = next.Mid(next.Find(";") + 1);
	}

	while (next.Find("|") != -1)
	{
		const double x = Utility::atof_custom(next.Left(next.Find("|")));
		next = next.Mid(next.Find("|") + 1);
		const double y = Utility::atof_custom(next.Left(next.Find("|")));

		const long nextID = _shp->get_PointCount();
		_shp->InsertPointXY(nextID, x, y);

		const ShpfileType shptype = _shp->get_ShapeType();

		// Z
		if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM || shptype == SHP_MULTIPOINTZ || shptype == SHP_POLYGONZ || shptype == SHP_POLYLINEZ)
		{
			next = next.Mid(next.Find("|") + 1);
			const double z = Utility::atof_custom(next.Left(next.Find("|")));
			_shp->put_PointZ(nextID, z);
		}
		// M
		if (shptype == SHP_MULTIPOINTM || shptype == SHP_POLYGONM || shptype == SHP_POLYLINEM)
		{
			next = next.Mid(next.Find("|") + 1);
			const double m = Utility::atof_custom(next.Left(next.Find("|")));
			_shp->put_PointM(nextID, m);
		}

		next = next.Mid(next.Find("|") + 1);
	}

	ClearLabelPositionCache();
	*retval = VARIANT_TRUE;
	return S_OK;
}
#pragma endregion

// *****************************************************************
//		PointInThisPoly()
// *****************************************************************
STDMETHODIMP CShape::PointInThisPoly(IPoint* pt, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GetUtils()->PointInPolygon(this, pt, retval);

	return S_OK;
}

//*******************************************************************
//		Get_SegmentAngle()
//*******************************************************************
// returns angle in degrees
double CShape::get_SegmentAngle(const long segmentIndex)
{
	double x1, y1, x2, y2;
	VARIANT_BOOL vbretval;
	long numPoints;
	this->get_NumPoints(&numPoints);
	if (segmentIndex > numPoints - 2)
	{
		return 0.0;
	}

	this->get_XY(segmentIndex, &x1, &y1, &vbretval);
	this->get_XY(segmentIndex + 1, &x2, &y2, &vbretval);
	double dx = x2 - x1; double dy = y2 - y1;
	return GeometryHelper::GetPointAngle(dx, dy) / pi_ * 180.0;
}

// **********************************************
//   Clone()
// **********************************************
STDMETHODIMP CShape::Clone(IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = nullptr;

	IShape* shp = nullptr;
	ComHelper::CreateShape(&shp);
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
			CComPtr<IPoint> pnt = nullptr;
			this->get_Point(i, &pnt);
			CComPtr<IPoint> pntNew = nullptr;
			pnt->Clone(&pntNew);
			shp->InsertPoint(pntNew, &i, &vbretval);
		}

		//shp->put_Key(_key);
		*retval = shp;
	}
	return S_OK;
}

//*****************************************************************
//*		CopyFrom()
//*****************************************************************
STDMETHODIMP CShape::CopyFrom(IShape* source, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!source)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		*retVal = VARIANT_FALSE;
		return S_OK;
	}

	IShape* target = this; // TODO: Fix compile warning
	ShpfileType shpType;
	source->get_ShapeType(&shpType);
	VARIANT_BOOL vb;
	target->Create(shpType, &vb);

	long numParts;
	source->get_NumParts(&numParts);
	for (int i = 0; i < numParts; i++)
	{
		long part, newIndex;
		source->get_Part(i, &part);
		target->InsertPart(part, &newIndex, &vb);
	}

	const bool hasM = shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ;
	const bool hasZ = shpType == SHP_POLYGONZ;

	long numPoints;
	source->get_NumPoints(&numPoints);
	double x, y, m, z;
	long pointIndex;
	for (int i = 0; i < numPoints; i++)
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
	ClearLabelPositionCache();
	*retVal = VARIANT_TRUE;
	return S_OK;
}

// **********************************************
//   Explode()
// **********************************************
// Splits multi-part shapes in the single part ones
STDMETHODIMP CShape::Explode(VARIANT* results, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = VARIANT_FALSE;

	std::vector<IShape*> vShapes;
	if (ExplodeCore(vShapes))
	{
		// no need to release objects as we return them to the caller
		if (Templates::Vector2SafeArray(&vShapes, results))
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
		IShape* shp = nullptr;
		this->Clone(&shp);
		vShapes.push_back(shp);
	}
	else if (shpType != SHP_POLYGON && shpType != SHP_POLYGONM && shpType != SHP_POLYGONZ)
	{
		// for every feature except polygons we just return the parts of shapes
		for (long i = 0; i < numParts; i++)
		{
			IShape* part = nullptr;
			this->get_PartAsShape(i, &part);
			vShapes.push_back(part);
		}
	}
	else
	{
		const bool isM = ShapeUtility::IsM(shpType);

		// for polygons holes should be treated, the main problem here is to determine 
		// to which part the hole belong; OGR will be used for this
		if (OGRGeometry* geom = OgrConverter::ShapeToGeometry(this))
		{
			const OGRwkbGeometryType type = geom->getGeometryType();
			if (type == wkbMultiPolygon || type == wkbMultiPolygon25D)
			{
				std::vector<OGRGeometry*> polygons;		// polygons shouldn't be deleted as they are only 
														// references to the parts of init multipolygon
				if (OgrConverter::MultiPolygon2Polygons(geom, &polygons))
				{
					for (const auto& polygon : polygons)
					{
						if (IShape* poly = OgrConverter::GeometryToShape(polygon, isM))
						{
							vShapes.push_back(poly);
						}
					}
				}
			}
			else
			{
				if (IShape* shp = OgrConverter::GeometryToShape(geom, isM))
				{
					vShapes.push_back(shp);
				}
			}
			OGRGeometryFactory::destroyGeometry(geom);
		}
	}
	return !vShapes.empty();
}

//***********************************************************************
//*			get_LabelPositionAutoChooseMethod()
//***********************************************************************
void CShape::get_LabelPositionAuto(tkLabelPositioning method, double& x, double& y, double& rotation, const tkLineLabelOrientation orientation)
{
	if (method == lpNone)
	{
		ShpfileType type;
		get_ShapeType(&type);
		method = LabelsHelper::LabelPositionForShapeType(type);
	}

	get_LabelPosition(method, x, y, rotation, orientation);
}

//***********************************************************************
//*			get_LabelPosition()
//***********************************************************************
// sub-function for GenerateLabels
// returns coordinates of label and angle of segment rotation for polylines
void CShape::get_LabelPosition(const tkLabelPositioning method, double& x, double& y, double& rotation, const tkLineLabelOrientation orientation)
{
	x = y = rotation = 0.0;
	if (method == lpNone)
		return;

	// If previous call was cached, return those values
	if (method == _labelPositioning && orientation == _labelOrientation)
	{
		x = _labelX;
		y = _labelY;
		rotation = _labelRotation;
		return;
	}

	IPoint* pnt = nullptr;
	ShpfileType shpType;
	this->get_ShapeType(&shpType);
	VARIANT_BOOL vbretval;
	int segmentIndex = -1;	// for polylines

	shpType = ShapeUtility::Convert2D(shpType);

	if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
	{
		//VARIANT_BOOL vbretval;
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
			pnt->Release();
			pnt = nullptr;
		}
	}
	else if (shpType == SHP_POLYLINE)
	{
		long numPoints;
		this->get_NumPoints(&numPoints);
		if (numPoints < 2) return;

		double x1, y1, x2, y2;

		if (method == lpFirstSegment)
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
			double halfLength;
			this->get_Length(&halfLength);
			halfLength /= 2.0;

			long i;
			for (i = 0; i < numPoints; i++)
			{
				this->get_XY(i, &x1, &y1, &vbretval);
				this->get_XY(i + 1, &x2, &y2, &vbretval);
				const double delta = sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));
				if (length + delta < halfLength)
					length += delta;
				else if (length + delta > halfLength)
				{
					const double ratio = (halfLength - length) / (delta);
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
			for (long i = 0; i < numPoints - 1; i++)
			{
				this->get_XY(i, &x1, &y1, &vbretval);
				this->get_XY(i + 1, &x2, &y2, &vbretval);
				const double length = sqrt(pow(x1 - x2, 2.0) + pow(y1 - y2, 2.0));
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
			x = (x1 + x2) / 2.0;
			y = (y1 + y2) / 2.0;
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

	// Cache values:
	_labelPositioning = method;
	_labelOrientation = orientation;
	_labelX = x;
	_labelY = y;
	_labelRotation = rotation;
}


// ********************************************************************
//		ClearLabelPositionCache()  
// ********************************************************************
void CShape::ClearLabelPositionCache()
{
	if (_labelPositioning == tkLabelPositioning::lpNone)
		return;

	_labelPositioning = tkLabelPositioning::lpNone;
	_labelOrientation = tkLineLabelOrientation::lorParallel;
	_labelX = 0;
	_labelY = 0;
	_labelRotation = 0;
}

// ********************************************************************
//		Bytes2SafeArray()				               
// ********************************************************************
//  Creates safe array with numbers of shapes as long values
//  Returns true when created safe array has elements, and false otherwise
bool Bytes2SafeArray(const unsigned char* data, const int size, VARIANT* arr)
{
	SAFEARRAY FAR* psa;
	SAFEARRAYBOUND rgsabound[1]{};
	rgsabound[0].lLbound = 0;

	if (size > 0)
	{
		rgsabound[0].cElements = size;
		psa = SafeArrayCreate(VT_UI1, 1, rgsabound); // TODO: Fix compile warning

		if (psa)
		{
			unsigned char* pchar = nullptr;
			//SafeArrayAccessData(psa, (void HUGEP * FAR*)(&pchar));
			SafeArrayAccessData(psa, reinterpret_cast<void**>(&pchar));

			if (pchar)
				memcpy(pchar, &data[0], sizeof(unsigned char) * size);

			SafeArrayUnaccessData(psa);

			arr->vt = VT_ARRAY | VT_UI1;
			arr->parray = psa;
			return true;
		}
	}
	else
	{
		rgsabound[0].cElements = 0;
		psa = SafeArrayCreate(VT_UI1, 1, rgsabound); // TODO: Fix compile warning
		arr->vt = VT_ARRAY | VT_UI1;
		arr->parray = psa;
	}
	return false;
}

//***********************************************************************
//			ExportToBinary()
//***********************************************************************
STDMETHODIMP CShape::ExportToBinary(VARIANT* bytesArray, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int* data = _shp->get_RawData();
	const int contentLength = _shp->get_ContentLength();

	if (data)
	{
		const auto buffer = (unsigned char*)data;  // TODO: Fix compile warning
		// *retVal = Bytes2SafeArray(buffer, contentLength, bytesArray);
		Bytes2SafeArray(buffer, contentLength, bytesArray) ? *retVal = VARIANT_TRUE : *retVal = VARIANT_FALSE;
		delete[] data;  // TODO: Fix compile warning
	}
	else
	{
		*retVal = NULL;  // TODO: Should this be NULL?
	}

	return S_OK;
}

//********************************************************************
//*		ImportFromBinary()
//********************************************************************
STDMETHODIMP CShape::ImportFromBinary(const VARIANT bytesArray, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (bytesArray.vt != (VT_ARRAY | VT_UI1))
		return S_OK;

	unsigned char* p = nullptr;
	//SafeArrayAccessData(bytesArray.parray, (void HUGEP * FAR*)(&p));
	SafeArrayAccessData(bytesArray.parray, reinterpret_cast<void**>(&p));

	auto data = (char*)p; // TODO: Fix compile warning

	const int recordLength = gsl::narrow_cast<int>(bytesArray.parray->cbElements);
	const bool result = _shp->put_RawData(data, recordLength);
	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;

	SafeArrayUnaccessData(bytesArray.parray);
	ClearLabelPositionCache();
	return S_OK;
}

//*****************************************************************
//*		FixupShapeCore()
//*****************************************************************
bool CShape::FixupShapeCore(const ShapeValidityCheck validityCheck)
{
	VARIANT_BOOL vb;

	switch (validityCheck)
	{
	case ShapeValidityCheck::NoParts:
	{
		long partIndex = 0;
		InsertPart(0, &partIndex, &vb);
		return vb ? true : false;
	}
	case ShapeValidityCheck::DirectionOfPolyRings:
	{
		ReversePointsOrder(0, &vb);
		return vb ? true : false;
	}
	case ShapeValidityCheck::FirstAndLastPointOfPartMatch:
	{
		const bool hasM = _shp->get_ShapeType() == SHP_POLYGONM || _shp->get_ShapeType() == SHP_POLYGONZ;
		const bool hasZ = _shp->get_ShapeType() == SHP_POLYGONZ;

		const ShpfileType shptype = ShapeUtility::Convert2D(_shp->get_ShapeType());
		if (shptype == SHP_POLYGON)
		{
			double x1, x2, y1, y2, m;

			for (long i = 0; i < _shp->get_PartCount(); i++)
			{
				const int begPart = _shp->get_PartStartPoint(i);
				const int endPart = _shp->get_PartEndPoint(i);

				_shp->get_PointXY(begPart, x1, y1);
				_shp->get_PointXY(endPart, x2, y2);
				if (x1 != x2 || y1 != y2)
				{
					_shp->InsertPointXY(endPart + 1, x1, y1);

					if (hasM)
					{
						_shp->get_PointM(begPart, m);
						_shp->put_PointM(endPart + 1, m);
					}
					if (hasZ)
					{
						_shp->get_PointZ(begPart, m);
						_shp->put_PointZ(endPart + 1, m);
					}

					// the next parts should be moved a step forward
					for (long part = i + 1; part < _shp->get_PartCount(); part++)
					{
						_shp->put_PartStartPoint(part, _shp->get_PartStartPoint(part));
					}
				}
			}
		}
	}
	ClearLabelPositionCache();
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

	FixUp2(umMeters, retval);

	return S_OK;
}

//*****************************************************************
//*		FixUp2()
//*****************************************************************
STDMETHODIMP CShape::FixUp2(const tkUnitsOfMeasure units, IShape** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = nullptr;

	// no points? nothing we can do.
	if (_shp->get_PointCount() <= 0)
	{
		return S_OK;
	}

	CString errMessage;
	ShapeValidityCheck validityCheck;
	if (!this->ValidateBasics(validityCheck, errMessage))
	{
		switch (validityCheck)
		{
		case ShapeValidityCheck::FirstAndLastPointOfPartMatch:
		case ShapeValidityCheck::DirectionOfPolyRings:
		case ShapeValidityCheck::NoParts:
			IShape* shp = nullptr;
			this->Clone(&shp);

			// try some basic fixing
			static_cast<CShape*>(shp)->FixupShapeCore(validityCheck);

			// run the core routine
			*retVal = static_cast<CShape*>(shp)->FixupByBuffer(units);

			shp->Release();
			return S_OK;
		}
	}

	*retVal = FixupByBuffer(units);

	return S_OK;
}

//*****************************************************************
//*		FixupByBuffer()
//*****************************************************************
IShape* CShape::FixupByBuffer(tkUnitsOfMeasure units)
{
	VARIANT_BOOL vb = VARIANT_FALSE;

	IShape* result = nullptr;

	// valid shape? just copy it.
	this->get_IsValid(&vb);
	if (vb)
	{
		Clone(&result);
	}
	else
	{
		// Fixing MWGIS-59: Fixup makes new shape larger: 
		// Buffer(m_globalSettings.GetInvalidShapeBufferDistance(units), 30, &result);
		// Copied from GeosConverter.GeomToShapes():
		Buffer(0, 30, &result);
	}

	// did we allocate memory for the fixed shape?
	if (result)
	{
		result->get_IsValid(&vb);
		if (!vb) {
			result->Release();
			result = nullptr;
		}
	}

	return result;
}

//*****************************************************************
//*		AddPoint()
//*****************************************************************
STDMETHODIMP CShape::AddPoint(const double x, const double y, long* pointIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const bool success = _shp->InsertPointXY(_shp->get_PointCount(), x, y);
	*pointIndex = success ? _shp->get_PointCount() - 1 : -1;
	ClearLabelPositionCache();
	return S_OK;
}

#pragma region Get point

// XY coordinates
bool CShape::get_XY(const long pointIndex, double* x, double* y)
{
	if (!_shp->get_PointXY(pointIndex, *x, *y))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	return true;
}

bool CShape::get_Z(const long pointIndex, double* z)
{
	if (!_shp->get_PointZ(pointIndex, *z))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	return true;
}

bool CShape::get_M(const long pointIndex, double* m)
{
	if (!_shp->get_PointM(pointIndex, *m))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	return true;
}

// XYZ coordinates
bool CShape::get_XYM(const long pointIndex, double* x, double* y, double* m)
{
	if (!_shp->get_PointXY(pointIndex, *x, *y) ||
		!_shp->get_PointM(pointIndex, *m))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	return true;
}

bool CShape::get_XYZ(const long pointIndex, double* x, double* y, double* z)
{
	if (!_shp->get_PointXY(pointIndex, *x, *y))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	_shp->get_PointZ(pointIndex, *z);   // ignore possible error for 2D type
	return true;
}

// XYZM coordinates
bool CShape::get_XYZM(const long pointIndex, double& x, double& y, double& z, double& m)
{
	if (!_shp->get_PointXY(pointIndex, x, y))
	{
		this->ErrorMessage(_shp->get_LastErrorCode());
		return false;
	}
	_shp->get_PointZ(pointIndex, z);   // ignore possible error for 2D type
	_shp->get_PointM(pointIndex, m);
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
STDMETHODIMP CShape::ExportToWKT(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	OGRGeometry* geom = OgrConverter::ShapeToGeometry(this);
	if (geom != nullptr)
	{
		char* s;
		geom->exportToWkt(&s, OGRwkbVariant::wkbVariantIso);
		(*retVal) = A2BSTR(s);
		OGRGeometryFactory::destroyGeometry(geom);
		// allocated in GDAL; free using CPLFree
		CPLFree(s);
	}
	else
	{
		*retVal = A2BSTR("");
	}
	return S_OK;
}

//*****************************************************************
//*		ImportFromWKT()
//*****************************************************************
STDMETHODIMP CShape::ImportFromWKT(BSTR serialized, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	// convert BSTR to CString
	CString ser(serialized);
	// get char* buffer from CString
	const char* buffer = ser.GetBuffer();

	OGRGeometry* oGeom = nullptr;
	const OGRErr err = OGRGeometryFactory::createFromWkt(&buffer, nullptr, &oGeom);
	if (err != OGRERR_NONE || !oGeom)
	{
		ErrorMessage(tkINVALID_SHAPE);
		return S_OK;
	}

	// if there is a geometry collection only the first shape will be taken
	std::vector<IShape*> shapes;
	// in case geometry is both measured and 3D, let 3D govern
	if (OgrConverter::GeometryToShapes(oGeom, &shapes, oGeom->IsMeasured() && !oGeom->Is3D()))
	{
		if (!shapes.empty() && gsl::at(shapes, 0))
		{
			IShape* result = gsl::at(shapes, 0);

			// it was an impression that polygons are imported as non-closed
			//((CShape*)result)->FixupShapeCore(ShapeValidityCheck::FirstAndLastPointOfPartMatch);

			VARIANT_BOOL vb;
			this->CopyFrom(result, &vb);
			*retVal = VARIANT_TRUE;
		}

		for (const auto& shape : shapes)
		{
			if (shape) shape->Release();
		}
	}
	else
	{
		ErrorMessage(tkINVALID_SHAPE);
	}
	return S_OK;
}

//*****************************************************************
//*		ClosestPoints()
//*****************************************************************
STDMETHODIMP CShape::ClosestPoints(IShape* shape2, IShape** result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	GEOSGeometry* g1 = GeosConverter::ShapeToGeom(this);
	GEOSGeometry* g2 = GeosConverter::ShapeToGeom(shape2);
	if (g1 && g2)
	{
		GEOSCoordSequence* coords = GeosHelper::ClosestPoints(g1, g2);
		if (coords)
		{
			const int size = GeosHelper::CoordinateSequenceSize(coords);
			if (size > 0)
			{
				std::vector<Point2D> points;
				double x, y;
				for (int i = 0; i < size; i++) {
					if (GeosHelper::CoordinateSequenceGetXY(coords, i, x, y))
					{
						// points.push_back(Point2D(x, y));
						points.emplace_back(x, y);
					}
				}
				if (points.size() > 1)
				{
					VARIANT_BOOL vb;
					ComHelper::CreateShape(result);
					(*result)->Create(ShpfileType::SHP_POLYLINE, &vb);
					long pointIndex;
					for (const auto& point : points)
					{
						(*result)->AddPoint(point.x, point.y, &pointIndex);
					}
				}
			}
			GeosHelper::DestroyCoordinateSequence(coords);
		}
		if (!*result)
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

//*****************************************************************
//*		Move()
//*****************************************************************
STDMETHODIMP CShape::Move(const DOUBLE xProjOffset, const DOUBLE yProjOffset)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long numPoints;
	get_NumPoints(&numPoints);

	VARIANT_BOOL vb;
	double x, y;
	for (long i = 0; i < numPoints; i++)
	{
		if (get_XY(i, &x, &y)) {
			put_XY(i, x + xProjOffset, y + yProjOffset, &vb);
		}
	}
	ClearLabelPositionCache();
	return S_OK;
}

//*****************************************************************
//*		Rotate()
//*****************************************************************
STDMETHODIMP CShape::Rotate(const DOUBLE originX, const DOUBLE originY, DOUBLE angle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (angle == 0.0) return S_OK;

	angle *= -1;
	const double angleRad = angle / 180.0 * pi_;
	const double sine = sin(angleRad);
	const double cosine = cos(angleRad);

	VARIANT_BOOL vb;
	double x, y;
	long numPoints;
	get_NumPoints(&numPoints);
	for (long i = 0; i < numPoints; i++)
	{
		if (get_XY(i, &x, &y))
		{
			const double dx = x - originX;
			const double dy = y - originY;
			x = originX + cosine * dx - sine * dy;
			y = originY + sine * dx + cosine * dy;
			put_XY(i, x, y, &vb);
		}
	}
	ClearLabelPositionCache();
	return S_OK;
}

//*****************************************************************
//*		ShapeType2D()
//*****************************************************************
STDMETHODIMP CShape::get_ShapeType2D(ShpfileType* pVal)
{
	*pVal = ShapeUtility::Convert2D(_shp->get_ShapeType());
	return S_OK;
}

//*****************************************************************
//*		SplitByPolyline()
//*****************************************************************
STDMETHODIMP CShape::SplitByPolyline(IShape* polyline, VARIANT* results, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	vector<IShape*>	shapes;

	if (!SplitByPolylineCore(polyline, shapes))
		return S_OK;

	if (!shapes.empty() && Templates::Vector2SafeArray(&shapes, results))
	{
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

//*****************************************************************
//*		SplitByPolyline()
//*****************************************************************
// Based on QGis implementation: https_://github.com/qgis/QGIS/blob/master/src/core/qgsgeometry.cpp
bool CShape::SplitByPolylineCore(IShape* polyline, vector<IShape*>& shapes)
{
	if (!shapes.empty()) return false;

	ShpfileType shpType;
	get_ShapeType2D(&shpType);
	if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return true;
	}

	int numSourceGeom = 0;
	vector<GEOSGeometry*> results;

	GEOSGeometry* result = nullptr;
	GEOSGeometry* line = GeosConverter::ShapeToGeom(polyline);
	GEOSGeometry* s = GeosConverter::ShapeToGeom(this);
	if (s && line)
	{
		if (GeosHelper::Intersects(s, line))
		{
			if (shpType == SHP_POLYGON)
			{
				if (GEOSGeometry* b = GeosHelper::Boundary(s))
				{
					if (GEOSGeometry* un = GeosHelper::Union(line, b)) {
						result = GeosHelper::Polygonize(un);
						GeosHelper::DestroyGeometry(un);
					}
					GeosHelper::DestroyGeometry(b);
				}
			}
			else {
				const int linearIntersect = GeosHelper::RelatePattern(s, line, "1********");
				if (linearIntersect > 0) {
					ErrorMessage(tkSPLIT_LINEAR_INTERSECTION);
					goto cleaning; // TODO: Fix compile warning
				}
				result = GeosHelper::Difference(s, line);
			}
		}
		numSourceGeom = GeosHelper::GetNumGeometries(s);
	}

	if (!result) goto cleaning; // TODO: Fix compile warning

	const int numGeoms = GeosHelper::GetNumGeometries(result);
	if (numGeoms > 1)
	{
		GeosConverter::NormalizeSplitResults(result, s, shpType, results);
		GeosHelper::DestroyGeometry(result);
	}
	else {
		results.push_back(result);
	}

	// we expect to have more parts then initially
	if (static_cast<int>(results.size()) > numSourceGeom)
	{
		for (const auto& value : results)
		{
			vector<IShape*> shapesTemp;
			GeosConverter::GeomToShapes(value, &shapesTemp, false);
			shapes.insert(shapes.end(), shapesTemp.begin(), shapesTemp.end());
		}
	}

cleaning:
	for (const auto& value : results)
	{
		GeosHelper::DestroyGeometry(value);
	}
	if (s)	GeosHelper::DestroyGeometry(s);
	if (line) GeosHelper::DestroyGeometry(line);

	return !shapes.empty();
}

//*****************************************************************
//*		get_IsEmpty()
//*****************************************************************
STDMETHODIMP CShape::get_IsEmpty(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _shp->get_PointCount() == 0 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

//*****************************************************************
//*		Clear()
//*****************************************************************
STDMETHODIMP CShape::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_shp->Clear();
	ClearLabelPositionCache();
	return S_OK;
}

//*****************************************************************
//*		InterpolatePoint()
//*****************************************************************
STDMETHODIMP CShape::InterpolatePoint(IPoint* startPoint, const double distance, const VARIANT_BOOL normalized, IPoint** retVal)
{
	// simply call Utility function
	return GetUtils()->LineInterpolatePoint(this, startPoint, distance, normalized, retVal);
}

//*****************************************************************
//*		ProjectDistanceTo()
//*****************************************************************
STDMETHODIMP CShape::ProjectDistanceTo(IShape* referenceShape, double* distance)
{
	// call Utility function
	return GetUtils()->LineProjectDistanceTo(this, referenceShape, distance);
}
