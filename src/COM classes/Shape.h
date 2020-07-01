// ********************************************************************************************************
// File name: Shape.h
// Description: Declaration of the CShape
// ********************************************************************************************************
// The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at 
// http://www.mozilla.org/MPL/ 
// Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND, either express or implied. See the License for the specific language governing rights and 
// limitations under the License. 
//
// The Original Code is MapWindow Open Source. 
//
// The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
// Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
// public domain in March 2004.  
//
// -------------------------------------------------------------------------------------------------------
// Contributor(s): (Open source contributors should list themselves and their modifications here). 

#pragma once
//
#include "ShapeInterfaces.h"
#include "OgrConverter.h"
#include "ShapeWrapper.h"
#include "ShapeWrapperCOM.h"
#include "ShapePoint.h"

// ******************************************************
//	 CShape declaration
// ******************************************************
class ATL_NO_VTABLE CShape : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CShape, &CLSID_Shape>,
	public IDispatchImpl<IShape, &IID_IShape, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShape();

	~CShape();

	DECLARE_REGISTRY_RESOURCEID(IDR_SHAPE)

	DECLARE_NOT_AGGREGATABLE(CShape)

	BEGIN_COM_MAP(CShape)
		COM_INTERFACE_ENTRY(IShape)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
		return S_OK;
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;

// ******************************************************
//	 IShape interface
// ******************************************************
public:
	STDMETHOD(get_Extents)(/*[out, retval]*/ IExtents * *pVal);
	STDMETHOD(DeletePart)(/*[in]*/ long PartIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(InsertPart)(/*[in]*/ long PointIndex, /*[in, out]*/ long * PartIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(DeletePoint)(/*[in]*/ long PointIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(InsertPoint)(/*[in]*/ IPoint * NewPoint, /*[in, out]*/long * PointIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Create)(/*[in]*/ ShpfileType _shptype, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Part)(/*[in]*/ long PartIndex, /*[out, retval]*/ long *pVal);
	STDMETHOD(put_Part)(/*[in]*/ long PartIndex, /*[in]*/ long newVal);
	STDMETHOD(get_Point)(/*[in]*/ long PointIndex, /*[out, retval]*/ IPoint * *pVal);
	STDMETHOD(put_Point)(/*[in]*/ long PointIndex, /*[in]*/ IPoint * newVal);
	STDMETHOD(get_ShapeType)(/*[out, retval]*/ ShpfileType *pVal);
	STDMETHOD(put_ShapeType)(/*[in]*/ ShpfileType newVal);
	STDMETHOD(get_NumParts)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_NumPoints)(/*[out, retval]*/ long *pVal);
	STDMETHOD(SerializeToString)(/*[out, retval]*/ BSTR * Serialized);
	STDMETHOD(CreateFromString)(/*[in]*/ BSTR Serialized, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(PointInThisPoly)(/*[in]*/ IPoint * pt, /*[out, retval]*/ VARIANT_BOOL *retval);
	STDMETHOD(get_Centroid)(IPoint** pVal);
	STDMETHOD(get_Length)(double *pVal);
	STDMETHOD(get_Perimeter)(double *pVal);
	STDMETHOD(get_Area)(double *pVal);
	// OGRGeometry methods and properties (lsu 06-aug-2009)
	STDMETHOD(Relates)(IShape* Shape, tkSpatialRelation Relation, VARIANT_BOOL* retval);
	STDMETHOD(Clip)(IShape* Shape, tkClipOperation Operation, IShape** retval);
	STDMETHOD(Distance)(IShape* Shape, DOUBLE* retval);
	STDMETHOD(Buffer)(DOUBLE Distance, long nQuadSegments, IShape** retval);
	STDMETHOD(Contains)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Crosses)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Disjoint)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Equals)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Intersects)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Overlaps)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Touches)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Within)(IShape* Shape, VARIANT_BOOL* retval);
    STDMETHOD(Covers)(IShape* Shape, VARIANT_BOOL* retval);
    STDMETHOD(CoveredBy)(IShape* Shape, VARIANT_BOOL* retval);
	STDMETHOD(Boundary)(IShape** retval);
	STDMETHOD(ConvexHull)(IShape** retval);
	STDMETHOD(get_IsValid)(VARIANT_BOOL* retval);
	STDMETHOD(get_XY)(long PointIndex, double* x, double* y, VARIANT_BOOL* retval);
	STDMETHOD(get_PartIsClockWise)(long PartIndex, VARIANT_BOOL* retval);
	STDMETHOD(ReversePointsOrder)(long PartIndex, VARIANT_BOOL* retval);
	STDMETHOD(GetIntersection)(IShape* Shape, VARIANT* Results, VARIANT_BOOL* retval);
	STDMETHOD(get_Center)(IPoint **pVal);
	STDMETHOD(get_EndOfPart)(long PartIndex, long* retval);	
	STDMETHOD(get_PartAsShape)(long PartIndex, IShape** retval);
	STDMETHOD(get_IsValidReason)(BSTR* retval);
	STDMETHOD(get_InteriorPoint)(IPoint** retval);
	STDMETHOD(Clone)(IShape** retval);
	STDMETHOD(Explode)(VARIANT* Results, VARIANT_BOOL* retval);
	STDMETHOD(put_XY)(LONG pointIndex, DOUBLE x, DOUBLE y, VARIANT_BOOL* retVal);
	STDMETHOD(ExportToBinary)(VARIANT* bytesArray, VARIANT_BOOL* retVal);
	STDMETHOD(ImportFromBinary)(VARIANT bytesArray, VARIANT_BOOL* retVal);
	STDMETHOD(FixUp)(IShape** retval);
	STDMETHOD(AddPoint)(double x, double y, long* pointIndex);
	STDMETHOD(ExportToWKT)(BSTR* retVal);
	STDMETHOD(ImportFromWKT)(BSTR Serialized, VARIANT_BOOL *retVal);
	STDMETHOD(CopyFrom)(IShape* sourceShape, VARIANT_BOOL* retVal);
	STDMETHOD(ClosestPoints)(IShape* shape2, IShape** result);
	STDMETHOD(put_M)(LONG pointIndex, double m, VARIANT_BOOL* retVal);
	STDMETHOD(put_Z)(LONG pointIndex, double z, VARIANT_BOOL* retVal);
	STDMETHOD(get_M)(LONG pointIndex, double* m, VARIANT_BOOL* retVal);
	STDMETHOD(get_Z)(LONG pointIndex, double* z, VARIANT_BOOL* retVal);
	STDMETHOD(BufferWithParams)(DOUBLE Ditances, LONG numSegments, VARIANT_BOOL singleSided, tkBufferCap capStyle, tkBufferJoin joinStyle, DOUBLE mitreLimit, IShape** retVal);
	STDMETHOD(Move)(DOUBLE xProjOffset, DOUBLE yProjOffset);
	STDMETHOD(Rotate)(DOUBLE originX, DOUBLE originY, DOUBLE angle);
	STDMETHOD(get_ShapeType2D)(ShpfileType* pVal);
	STDMETHOD(SplitByPolyline)(IShape* polyline, VARIANT* results, VARIANT_BOOL* retVal);
	STDMETHOD(get_IsEmpty)(VARIANT_BOOL* pVal);
	STDMETHOD(Clear)();
	STDMETHOD(FixUp2)(tkUnitsOfMeasure units, IShape** retVal);
	STDMETHOD(InterpolatePoint)(IPoint* startPoint, double distance, VARIANT_BOOL normalized, IPoint **retVal);
	STDMETHOD(ProjectDistanceTo)(IShape* referenceShape, double* distance);

private:
	BSTR _key;
	long _lastErrorCode;
	ICallback * _globalCallback;
	CString _isValidReason;

	// either CShapeWrapper or CShapeWrapperCOM
	IShapeWrapper* _shp;

	tkLabelPositioning labelPositioning = tkLabelPositioning::lpNone;
	tkLineLabelOrientation labelOrientation = tkLineLabelOrientation::lorParallel;
	double labelX;
	double labelY;
	double labelRotation;

	// forces to use fast shape wrapper class to hold points information
	bool _useFastMode;							
	
private:
	void ClearLabelPositionCache();
	void ErrorMessage(long ErrorCode);
	bool PointInThisPolyFast(IPoint * pt);
	bool PointInThisPolyRegular(IPoint * pt);
	double get_SegmentAngle(long segementIndex);
	IShape* FixupByBuffer(tkUnitsOfMeasure units);

public:
	bool ExplodeCore(std::vector<IShape*>& vShapes);
	bool FixupShapeCore(ShapeValidityCheck validityCheck);
	bool put_RawData(char* data, int recordLength);
	IShapeWrapper* get_ShapeWrapper() { return _shp; }
	void put_FastMode(bool state);
	bool get_fastMode() { return _useFastMode; }
	void get_LabelPosition(tkLabelPositioning method, double& x, double& y, double& rotation, tkLineLabelOrientation orientation);
	bool get_Z(long PointIndex, double* z);
	bool get_M(long PointIndex, double* m);
	bool get_XY(long PointIndex, double* x, double* y);
	bool get_XYM(long PointIndex, double* x, double* y, double* m);
	bool get_XYZ(long PointIndex, double* x, double* y, double* z);
	bool get_XYZM(long PointIndex, double& x, double& y, double& z, double& m);
	bool get_ExtentsXY(double& xMin, double& yMin, double& xMax, double& yMax);
	bool get_ExtentsXYZM(double& xMin, double& yMin, double& xMax, double& yMax, double& zMin, double& zMax, double& mMin, double& mMax);
	bool ValidateBasics(ShapeValidityCheck& failedCheck, CString& err);
	void get_LabelPositionAuto(tkLabelPositioning method, double& x, double& y, double& rotation, tkLineLabelOrientation orientation);
	bool SplitByPolylineCore(IShape* polyline, vector<IShape*>& results);
};

OBJECT_ENTRY_AUTO(__uuidof(Shape), CShape)
