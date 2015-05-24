// GdalDriver.cpp : Implementation of CGdalDriver

#include "stdafx.h"
#include "GdalDriver.h"
#include "GdalDriverHelper.h"

// *************************************************************
//	  CheckBand()
// *************************************************************
bool CGdalDriver::CheckState()
{
	if (!_driver)
	{
		ErrorMessage("GdalDataset object isn't initialized.");
		return false;
	}
	return true;
}

// *************************************************************
//	  ErrorMessage()
// *************************************************************
void CGdalDriver::ErrorMessage(CString msg)
{
	CallbackHelper::ErrorMsg("GdalDriver", NULL, m_globalSettings.emptyBstr, msg);
}

// *************************************************************
//		get_DriverMetadata()
// *************************************************************
STDMETHODIMP CGdalDriver::get_Metadata(tkGdalDriverMetadata metadata, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if (CheckState())
	{
		CString s = GdalDriverHelper::GetMetadata(_driver, metadata);
		*retVal = A2BSTR(s);
		return S_OK;
	}

	*retVal = A2BSTR("");
	return S_OK;
}

// *************************************************************
//		get_DriverMetadataCount()
// *************************************************************
STDMETHODIMP CGdalDriver::get_MetadataCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*retVal = CheckState() ? GdalDriverHelper::get_MetadataCount(_driver) : 0;

	return S_OK;
}

// *************************************************************
//		get_DriverMetadata()
// *************************************************************
STDMETHODIMP CGdalDriver::get_MetadataItem(int metadataIndex, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if (CheckState())
	{
		CString s = GdalDriverHelper::get_MetadataItem(_driver, metadataIndex);
		*retVal = A2BSTR(s);
		return S_OK;
	}

	*retVal = A2BSTR("");
	return S_OK;
}

// *************************************************************
//		get_Description()
// *************************************************************
STDMETHODIMP CGdalDriver::get_Description(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	if (CheckState())
	{
		CString s =_driver->GetDescription();
		*pVal = A2BSTR(s);
		return S_OK;
	}

	*pVal = A2BSTR("");
	return S_OK;
}
