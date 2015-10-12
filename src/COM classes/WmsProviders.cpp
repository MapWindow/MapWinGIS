// WmsProviders.cpp : Implementation of CWmsProviders

#include "stdafx.h"
#include "WmsProviders.h"

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CWmsProviders::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("WmsProviders", m_globalSettings.callback, _key, ErrorMsg(_lastErrorCode));
}

// *********************************************************************
//		get_LastErrorCode
// *********************************************************************
STDMETHODIMP CWmsProviders::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// *********************************************************************
//		get_ErrorMsg
// *********************************************************************
STDMETHODIMP CWmsProviders::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

// *********************************************************************
//		get/put_Key
// *********************************************************************
STDMETHODIMP CWmsProviders::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);

	return S_OK;
}

STDMETHODIMP CWmsProviders::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);

	return S_OK;
}

// *************************************************************
//		Add
// *************************************************************
STDMETHODIMP CWmsProviders::Add(IWmsProvider* provider)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (!provider) {
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	_providers.push_back(provider);

	provider->AddRef();

	return S_OK;
}

// *************************************************************
//		Clear
// *************************************************************
STDMETHODIMP CWmsProviders::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	for (size_t i = 0; i < _providers.size(); i++) {
		_providers[i]->Release();
	}
	
	_providers.clear();

	return S_OK;
}

// *************************************************************
//		Count
// *************************************************************
STDMETHODIMP CWmsProviders::get_Count(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = static_cast<long>(_providers.size());

	return S_OK;
}

// *************************************************************
//		Item
// *************************************************************
STDMETHODIMP CWmsProviders::get_Item(LONG Index, IWmsProvider** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (Index < 0 || Index >= static_cast<long>(_providers.size()))
	{
		*pVal = NULL;
		//ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	(*pVal) = _providers[Index];
	(*pVal)->AddRef();

	return S_OK;
}

// *************************************************************
//		Remove
// *************************************************************
STDMETHODIMP CWmsProviders::Remove(LONG providerId, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	for (size_t i = 0; i < _providers.size(); i++)
	{
		long id;
		_providers[i]->get_Id(&id);

		if (providerId == id)
		{
			_providers[i]->Release();
			_providers.erase(_providers.begin() + i);

			*retVal = VARIANT_TRUE;
			return S_OK;
		}
	}

	*retVal = VARIANT_FALSE;

	return S_OK;
}
