// WmsProvider.cpp : Implementation of CWmsProvider
#include "stdafx.h"
#include "WmsProvider.h"

// *********************************************************************
//		get_LastErrorCode
// *********************************************************************
STDMETHODIMP CWmsProvider::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// *********************************************************************
//		get_ErrorMsg
// *********************************************************************
STDMETHODIMP CWmsProvider::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// *********************************************************************
//		get/put_Key
// *********************************************************************
STDMETHODIMP CWmsProvider::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);

	return S_OK;
}

STDMETHODIMP CWmsProvider::put_Key(BSTR newVal)
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
STDMETHODIMP CWmsProvider::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*pVal = A2BSTR(_provider->Name);

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	_provider->Name = OLE2A(newVal);

	return S_OK;
}

// *********************************************************
//	     BoundingBox()
// *********************************************************
STDMETHODIMP CWmsProvider::get_BoundingBox(IExtents** pVal)
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

STDMETHODIMP CWmsProvider::put_BoundingBox(IExtents* newVal)
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
STDMETHODIMP CWmsProvider::get_CrsEpsg(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _provider->get_CustomProjection()->get_Epsg();

	return S_OK;
}

STDMETHODIMP CWmsProvider::put_CrsEpsg(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < 0) return S_OK;

	_provider->get_CustomProjection()->put_Epsg(newVal);

	return S_OK;
}

// *********************************************************
//	     LayersCsv()
// *********************************************************
STDMETHODIMP CWmsProvider::get_LayersCsv(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	USES_CONVERSION;
	*pVal = A2BSTR(_provider->get_Layers());

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_LayersCsv(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	_provider->set_Layers(OLE2A(newVal));

	return S_OK;
}

// *********************************************************
//	     BaseUrl()
// *********************************************************
STDMETHODIMP CWmsProvider::get_BaseUrl(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString s = _provider->get_UrlFormat();

	USES_CONVERSION;
	*pVal = A2BSTR(s);

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_BaseUrl(BSTR newVal)
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
STDMETHODIMP CWmsProvider::get_Id(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _provider->Id;

	return S_OK;
}

STDMETHODIMP CWmsProvider::put_Id(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_provider->Id = newVal;

	return S_OK;
}

// *********************************************************
//	     Id()
// *********************************************************
STDMETHODIMP CWmsProvider::get_Format(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*pVal = A2BSTR(_provider->get_Format());

	return S_OK;
}

STDMETHODIMP CWmsProvider::put_Format(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	_provider->set_Format(OLE2A(newVal));

	return S_OK;
}