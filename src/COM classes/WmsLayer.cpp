// WmsLayer.cpp : Implementation of CWmsLayer
#include "stdafx.h"
#include "WmsLayer.h"

// *********************************************************************
//		get_LastErrorCode
// *********************************************************************
STDMETHODIMP CWmsLayer::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// *********************************************************************
//		get_ErrorMsg
// *********************************************************************
STDMETHODIMP CWmsLayer::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// *********************************************************************
//		get/put_Key
// *********************************************************************
STDMETHODIMP CWmsLayer::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);

	return S_OK;
}

STDMETHODIMP CWmsLayer::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);

	return S_OK;
}

// *********************************************************
//	     Name()
// *********************************************************
STDMETHODIMP CWmsLayer::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*pVal = A2BSTR(_provider->Name);

	return S_OK;
}
STDMETHODIMP CWmsLayer::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	_provider->Name = OLE2A(newVal);

	return S_OK;
}

// *********************************************************
//	     BoundingBox()
// *********************************************************
STDMETHODIMP CWmsLayer::get_BoundingBox(IExtents** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = NULL;

	CustomProjection* projection = _provider->get_CustomProjection();
	if (projection) 
	{
		double xMin, xMax, yMin, yMax;
		projection->get_Bounds(xMin, xMax, yMin, yMax);

		IExtents* box = NULL;
		ComHelper::CreateExtents(&box);
		
		box->SetBounds(xMin, yMin, 0.0, xMax, yMax, 0.0);

		*pVal = box;
	}

	return S_OK;
}

STDMETHODIMP CWmsLayer::put_BoundingBox(IExtents* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!newVal) 
	{
		ErrorMsg(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	CustomProjection* projection = _provider->get_CustomProjection();
	if (projection)
	{
		double xMin, yMin, zMin, xMax, yMax, zMax;
		newVal->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		projection->put_Bounds(xMin, xMax, yMin, yMax);
	}

	return S_OK;
}

// *********************************************************
//	     CrsEpsg()
// *********************************************************
STDMETHODIMP CWmsLayer::get_Epsg(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _provider->get_CustomProjection()->get_Epsg();

	return S_OK;
}

STDMETHODIMP CWmsLayer::put_Epsg(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < 0) return S_OK;

	_provider->get_CustomProjection()->put_Epsg(newVal);

	return S_OK;
}

// *********************************************************
//	     LayersCsv()
// *********************************************************
STDMETHODIMP CWmsLayer::get_Layers(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	USES_CONVERSION;
	*pVal = A2BSTR(_provider->get_Layers());

	return S_OK;
}
STDMETHODIMP CWmsLayer::put_Layers(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	_provider->set_Layers(OLE2A(newVal));

	return S_OK;
}

// *********************************************************
//	     BaseUrl()
// *********************************************************
STDMETHODIMP CWmsLayer::get_BaseUrl(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString s = _provider->get_UrlFormat();

	USES_CONVERSION;
	*pVal = A2BSTR(s);

	return S_OK;
}
STDMETHODIMP CWmsLayer::put_BaseUrl(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	CString s = OLE2A(newVal);

	_provider->put_UrlFormat(s);

	return S_OK;
}

// *********************************************************
//	     Id()
// *********************************************************
STDMETHODIMP CWmsLayer::get_Id(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _provider->Id;

	return S_OK;
}

STDMETHODIMP CWmsLayer::put_Id(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_provider->Id = newVal;

	return S_OK;
}

// *********************************************************
//	     Format()
// *********************************************************
STDMETHODIMP CWmsLayer::get_Format(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*pVal = A2BSTR(_provider->get_Format());

	return S_OK;
}

STDMETHODIMP CWmsLayer::put_Format(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	_provider->set_Format(OLE2A(newVal));

	return S_OK;
}

// *********************************************************
//	     IsEmpty()
// *********************************************************
STDMETHODIMP CWmsLayer::get_IsEmpty(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_provider->get_UrlFormat().GetLength() == 0 ||
		_provider->get_Format().GetLength() == 0 ||
		_provider->get_Layers().GetLength() == 0 ||
		_provider->get_CustomProjection()->get_Epsg() <= 0)
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}

// *********************************************************
//	     MapExtents()
// *********************************************************
STDMETHODIMP CWmsLayer::get_MapExtents(IExtents** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BaseProjection* p = _provider->get_Projection();
	if (p) 
	{
		IExtents* ext = NULL;
		ComHelper::CreateExtents(&ext);
		ext->SetBounds(p->MapBounds.left, p->MapBounds.bottom, 0.0, p->MapBounds.right, p->MapBounds.top, 0.0);
		*pVal = ext;
	}

	return S_OK;
}

// *********************************************************
//	     Close()
// *********************************************************
STDMETHODIMP CWmsLayer::Close()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: implement

	return S_OK;
}

// *********************************************************
//	     Serialize()
// *********************************************************
STDMETHODIMP CWmsLayer::Serialize()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: implement

	return S_OK;
}

// *********************************************************
//	     Deserialize()
// *********************************************************
STDMETHODIMP CWmsLayer::Deserialize(BSTR state, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: implement

	return S_OK;
}
