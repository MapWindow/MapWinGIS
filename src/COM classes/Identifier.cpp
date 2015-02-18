// Identifier.cpp : Implementation of CIdentifier

#include "stdafx.h"
#include "Identifier.h"

//****************************************************************
//			HotTracking()
//****************************************************************
STDMETHODIMP CIdentifier::get_HotTracking(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _hotTracking;
	return S_OK;
}
STDMETHODIMP CIdentifier::put_HotTracking(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_hotTracking = newVal;
	return S_OK;
}

//****************************************************************
//			IdentifierMode()
//****************************************************************
STDMETHODIMP CIdentifier::get_IdentifierMode(tkIdentifierMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _mode;
	return S_OK;
}
STDMETHODIMP CIdentifier::put_IdentifierMode(tkIdentifierMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_mode = newVal;
	return S_OK;
}

//****************************************************************
//			IdentifierColor()
//****************************************************************
STDMETHODIMP CIdentifier::get_OutlineColor(OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _color;
	return S_OK;
}
STDMETHODIMP CIdentifier::put_OutlineColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_color = newVal;
	return S_OK;
}

//****************************************************************
//			IdentifierColor()
//****************************************************************
STDMETHODIMP CIdentifier::get_ActiveLayer(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _activeLayer;
	return S_OK;
}
STDMETHODIMP CIdentifier::put_ActiveLayer(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_activeLayer = newVal;
	return S_OK;
}


