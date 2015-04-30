#include "stdafx.h"
#include "measuring.h"
#include "MeasuringHelper.h"

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CMeasuring::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CMeasuring::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CMeasuring::get_GlobalCallback(ICallback **pVal)
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
STDMETHODIMP CMeasuring::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}


// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CMeasuring::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CMeasuring::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

// *******************************************************
//		get_Length()
// *******************************************************
STDMETHODIMP CMeasuring::get_IsStopped(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->IsStopped() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		get_PointCount()
// *******************************************************
STDMETHODIMP CMeasuring::get_PointCount(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->GetPointCount();
	return S_OK;
}

// *******************************************************
//		get_PointXY()
// *******************************************************
STDMETHODIMP CMeasuring::get_PointXY(long pointIndex, double* x, double* y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pointIndex < 0 || pointIndex >= (long)_measuring->GetPointCount())
	{
		ErrorMsg(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		MeasurePoint* pnt = _measuring->GetPoint(pointIndex);
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
//		MeasuringType()
// *******************************************************
STDMETHODIMP CMeasuring::get_MeasuringType(tkMeasuringType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->GetMeasuringType();
	return S_OK;
}
STDMETHODIMP CMeasuring::put_MeasuringType(tkMeasuringType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->SetMeasuringType(newVal);
	return S_OK;
}

// *******************************************************
//		MeasuringType()
// *******************************************************
STDMETHODIMP CMeasuring::get_Persistent(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->Persistent;
	return S_OK;
}
STDMETHODIMP CMeasuring::put_Persistent(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->Persistent = newVal;
	return S_OK;
}

// *******************************************************
//		get_Length()
// *******************************************************
STDMETHODIMP CMeasuring::get_Length(double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->GetDistance();
	return S_OK;
}

// *******************************************************
//		get_AreaWithClosingVertex()
// *******************************************************
STDMETHODIMP CMeasuring::get_AreaWithClosingVertex(double lastPointProjX, double lastPointProjY, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->GetArea(true, lastPointProjX, lastPointProjY);
	return S_OK;
}

// *******************************************************
//		get_Area()
// *******************************************************
STDMETHODIMP CMeasuring::get_Area(double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->GetArea(false, 0.0, 0.0);
	return S_OK;
}

// *******************************************************
//		UndoPoint()
// *******************************************************
STDMETHODIMP CMeasuring::UndoPoint(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	*retVal = _measuring->UndoPoint() ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		Stop()
// *******************************************************
STDMETHODIMP CMeasuring::FinishMeasuring()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->Stop();
	return S_OK;
}

// *******************************************************
//		Clear()
// *******************************************************
STDMETHODIMP CMeasuring::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->Clear();
	return S_OK;
}

// *******************************************************
//		SegmentLength()
// *******************************************************
STDMETHODIMP CMeasuring::get_SegmentLength(int segmentIndex, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long errorCode = tkNO_ERROR;
	*retVal = _measuring->GetSegmentLength(segmentIndex, errorCode);
	return S_OK;
}

// *******************************************************
//		ShowBearing()
// *******************************************************
STDMETHODIMP CMeasuring::get_ShowBearing(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->ShowBearing ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_ShowBearing(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->ShowBearing = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		IsUsingEllipsoid()
// *******************************************************
STDMETHODIMP CMeasuring::get_IsUsingEllipsoid(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->GetTransformationMode() != tmNotDefined;
	return S_OK;
}

// *******************************************************
//		AreaDisplayMode()
// *******************************************************
STDMETHODIMP CMeasuring::get_AreaUnits(tkAreaDisplayMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _measuring->AreaDisplayMode;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_AreaUnits(tkAreaDisplayMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->AreaDisplayMode = newVal;
	return S_OK;
}

// *******************************************************
//		get_IsEmpty()
// *******************************************************
STDMETHODIMP CMeasuring::get_IsEmpty(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->GetPointCount() == 0 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		BearingType()
// *******************************************************
STDMETHODIMP CMeasuring::get_BearingType(tkBearingType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->BearingType;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_BearingType(tkBearingType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->BearingType = newVal;
	return S_OK;
}

// *******************************************************
//		ShowLength()
// *******************************************************
STDMETHODIMP CMeasuring::get_ShowLength(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->ShowLength ? VARIANT_TRUE: VARIANT_FALSE;
 	return S_OK;
}

STDMETHODIMP CMeasuring::put_ShowLength(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->ShowLength = newVal ? true : false;
	return S_OK;
}

// *******************************************************
//		LengthDisplayMode()
// *******************************************************
STDMETHODIMP CMeasuring::get_LengthUnits(tkLengthDisplayMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->LengthUnits;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_LengthUnits(tkLengthDisplayMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->LengthUnits = newVal;
	return S_OK;
}

// *******************************************************
//		AngleFormat()
// *******************************************************
STDMETHODIMP CMeasuring::get_AngleFormat(tkAngleFormat* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->AngleFormat;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_AngleFormat(tkAngleFormat newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->AngleFormat = newVal;
	return S_OK;
}

// *******************************************************
//		AnglePrecision()
// *******************************************************
STDMETHODIMP CMeasuring::get_AnglePrecision(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->AnglePrecision;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_AnglePrecision(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->AnglePrecision = newVal;
	return S_OK;
}

// *******************************************************
//		AreaPrecision()
// *******************************************************
STDMETHODIMP CMeasuring::get_AreaPrecision(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->AreaPrecision;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_AreaPrecision(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->AreaPrecision = newVal;
	return S_OK;
}

// *******************************************************
//		LengthPrecision()
// *******************************************************
STDMETHODIMP CMeasuring::get_LengthPrecision(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->LengthPrecision;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_LengthPrecision(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->LengthPrecision = newVal;
	return S_OK;
}

// *******************************************************
//		PointsVisible()
// *******************************************************
STDMETHODIMP CMeasuring::get_PointsVisible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->PointsVisible ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_PointsVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->PointsVisible = newVal ? true : false;
	return S_OK;
}

// *******************************************************
//		LineColor()
// *******************************************************
STDMETHODIMP CMeasuring::get_LineColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->LineColor;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_LineColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->LineColor = newVal;
	return S_OK;
}

// *******************************************************
//		FillColor()
// *******************************************************
STDMETHODIMP CMeasuring::get_FillColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->FillColor;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_FillColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->FillColor = newVal;
	return S_OK;
}

// *******************************************************
//		FillTransparency()
// *******************************************************
STDMETHODIMP CMeasuring::get_FillTransparency(BYTE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->FillTransparency;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_FillTransparency(BYTE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->FillTransparency = newVal;
	return S_OK;
}

// *******************************************************
//		LineWidth()
// *******************************************************
STDMETHODIMP CMeasuring::get_LineWidth(FLOAT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->LineWidth;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_LineWidth(FLOAT newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->LineWidth = newVal;
	return S_OK;
}

// *******************************************************
//		LineStipple()
// *******************************************************
STDMETHODIMP CMeasuring::get_LineStyle(tkDashStyle* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->LineStyle;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_LineStyle(tkDashStyle newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->LineStyle = newVal;
	return S_OK;
}

// *******************************************************
//		PointLabelsVisible()
// *******************************************************
STDMETHODIMP CMeasuring::get_PointLabelsVisible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->PointLabelsVisible ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_PointLabelsVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->PointLabelsVisible = newVal ? true: false;
	return S_OK;
}

// *******************************************************
//		ShowTotalLength()
// *******************************************************
STDMETHODIMP CMeasuring::get_ShowTotalLength(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _measuring->ShowTotalLength ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_ShowTotalLength(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_measuring->ShowTotalLength = newVal ? true : false;
	return S_OK;
}

// *******************************************************
//		UndoButton()
// *******************************************************
STDMETHODIMP CMeasuring::get_UndoButton(tkUndoShortcut* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _undoButton;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_UndoButton(tkUndoShortcut newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_undoButton = newVal;
	return S_OK;
}

// *******************************************************
//		Serialize()
// *******************************************************
STDMETHODIMP CMeasuring::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CPLXMLNode* node = MeasuringHelper::Serialize(_measuring, "MeasuringClass");
	Utility::SerializeAndDestroyXmlTree(node, retVal);

	return S_OK;
}

// *******************************************************
//		Deserialize()
// *******************************************************
STDMETHODIMP CMeasuring::Deserialize(BSTR state, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*retVal = VARIANT_FALSE;

	CString s = OLE2CA(state);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeMeasuring = CPLGetXMLNode(node, "=MeasuringClass");
		if (nodeMeasuring)
		{
			MeasuringHelper::Deserialize(_measuring, nodeMeasuring);
			*retVal = VARIANT_TRUE;
		}
		CPLDestroyXMLNode(node);
	}

	return S_OK;
}