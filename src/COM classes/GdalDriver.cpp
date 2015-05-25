// GdalDriver.cpp : Implementation of CGdalDriver

#include "stdafx.h"
#include "GdalDriver.h"
#include "GdalDriverHelper.h"
#include "ogrsf_frmts.h"

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
STDMETHODIMP CGdalDriver::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	if (CheckState())
	{
		CString s = _driver->GetDescription();
		*pVal = A2BSTR(s);
		return S_OK;
	}

	*pVal = A2BSTR("");
	return S_OK;
}

// *************************************************************
//		get_IsVector()
// *************************************************************
STDMETHODIMP CGdalDriver::get_IsVector(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckState())	return S_OK;

	CComBSTR bstr;
	get_Metadata(dmdVector, &bstr);

	*pVal = bstr.Length() > 0;

	return S_OK;
}

// *************************************************************
//		get_IsVector()
// *************************************************************
STDMETHODIMP CGdalDriver::get_IsRaster(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckState())	return S_OK;

	CComBSTR bstr;
	get_Metadata(dmdRaster, &bstr);

	*pVal = bstr.Length() > 0;

	return S_OK;
}

// *************************************************************
//		get_MetadataItemKey()
// *************************************************************
STDMETHODIMP CGdalDriver::get_MetadataItemKey(int metadataIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckState())	return S_OK;

	CString s = GdalDriverHelper::get_MetadataItem(_driver, metadataIndex);
	if (s.GetLength() == 0)
	{
		*pVal = m_globalSettings.CreateEmptyBSTR();
		return S_OK;
	}

	int index = s.Find('=');

	if (index <= 0)
	{
		CallbackHelper::ErrorMsg(Debug::Format("Failed to parser driver metadata string: %s", s));
		*pVal = m_globalSettings.CreateEmptyBSTR();
		return S_OK;
	}

	*pVal = A2BSTR(s.Mid(0, index));

	return S_OK;
}

// *************************************************************
//		get_MetadataItemValue()
// *************************************************************
STDMETHODIMP CGdalDriver::get_MetadataItemValue(int metadataIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckState())	return S_OK;

	CString s = GdalDriverHelper::get_MetadataItem(_driver, metadataIndex);
	if (s.GetLength() == 0)
	{
		*pVal = m_globalSettings.CreateEmptyBSTR();
		return S_OK;
	}

	int index = s.Find('=');

	if (index <= 0 || index >= s.GetLength() - 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format("Failed to parser driver metadata string: %s", s));
		*pVal = m_globalSettings.CreateEmptyBSTR();
		return S_OK;
	}

	*pVal = A2BSTR(s.Mid(index + 1));

	return S_OK;
}

// *************************************************************
//		get_MetadataItemType()
// *************************************************************
STDMETHODIMP CGdalDriver::get_MetadataItemType(int metadataIndex, tkGdalDriverMetadata* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = dmdUnknown;

	CComBSTR bstr;
	get_MetadataItemKey(metadataIndex, &bstr);

	if (bstr.Length() > 0)
	{
		USES_CONVERSION;
		*pVal = GdalHelper::GetMetadataType(OLE2A(bstr));
		return S_OK;
	}

	return S_OK;
}