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
	*pVal = OLE2BSTR(_name);

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	::SysFreeString(_name);
	_name = OLE2BSTR(newVal);

	return S_OK;
}

// *********************************************************
//	     BoundingBox()
// *********************************************************
STDMETHODIMP CWmsProvider::get_BoundingBox(IExtents** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_BoundingBox(IExtents* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here

	return S_OK;
}

// *********************************************************
//	     CrsEpsg()
// *********************************************************
STDMETHODIMP CWmsProvider::get_CrsEpsg(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _epgs;

	return S_OK;
}

STDMETHODIMP CWmsProvider::put_CrsEpsg(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal < 0) return S_OK;

	_epgs = newVal;

	return S_OK;
}

// *********************************************************
//	     LayersCsv()
// *********************************************************
STDMETHODIMP CWmsProvider::get_LayersCsv(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*pVal = OLE2BSTR(_layerCsv);

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_LayersCsv(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	::SysFreeString(_layerCsv);
	_layerCsv = OLE2BSTR(newVal);

	return S_OK;
}

// *********************************************************
//	     BaseUrl()
// *********************************************************
STDMETHODIMP CWmsProvider::get_BaseUrl(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_BaseUrl(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here

	return S_OK;
}

// *********************************************************
//	     Id()
// *********************************************************
STDMETHODIMP CWmsProvider::get_Id(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _id;

	return S_OK;
}
STDMETHODIMP CWmsProvider::put_Id(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_id = newVal;

	return S_OK;
}