// GdalDataset.cpp : Implementation of CGdalDataset
#include "stdafx.h"
#include "GdalDataset.h"
#include "GdalHelper.h"
#include "GdalDriver.h"
#include "OgrHelper.h"

// *************************************************************
//	  CheckState()
// *************************************************************
bool CGdalDataset::CheckState()
{
	if (!_dataset)
	{
		ErrorMessage("GdalDataset object isn't initialized.");
		return false;
	}
	return true;
}

// *************************************************************
//	  ErrorMessage()
// *************************************************************
void CGdalDataset::ErrorMessage(CString msg)
{
	CallbackHelper::ErrorMsg("GdalDataset", NULL, m_globalSettings.emptyBstr, msg);
}

// *************************************************************
//	  get_Driver()
// *************************************************************
STDMETHODIMP CGdalDataset::get_Driver(IGdalDriver** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckState())
	{
		*pVal = NULL;
		return S_OK;
	}

	GDALDriver* driver = _dataset->GetDriver();
	if (driver) 
	{
		ComHelper::CreateInstance(idGdalDriver, (IDispatch**)pVal);
		((CGdalDriver*)*pVal)->Inject(driver);
	}

	return S_OK;
}

// *************************************************************
//	  CheckOnwer()
// *************************************************************
bool CGdalDataset::CheckOnwer()
{
	if (_proxy) {
		CallbackHelper::ErrorMsg("Can not open or close the datasource since it has another owner.");
		return false;
	}

	return true;
}

// *************************************************************
//	  Open()
// *************************************************************
STDMETHODIMP CGdalDataset::Open(BSTR Filename, VARIANT_BOOL readOnly, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!CheckOnwer()) return S_OK;

	Close();

	USES_CONVERSION;
	_dataset = GdalHelper::OpenRasterDatasetW(OLE2W(Filename), readOnly ? GA_ReadOnly : GA_Update);
	
	*retVal = _dataset != NULL;
	
	return S_OK;
}

// *************************************************************
//	  Close()
// *************************************************************
STDMETHODIMP CGdalDataset::Close()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckOnwer()) return S_OK;

	if (_dataset) {
		GdalHelper::CloseDataset(_dataset);
		_dataset = NULL;
	}

	return S_OK;
}

// ********************************************************
//     SetGeoTransform
// ********************************************************
STDMETHODIMP CGdalDataset::SetGeoTransform(DOUBLE xLeft, DOUBLE dX, DOUBLE yProjOnX, DOUBLE yTop, DOUBLE xProjOnY, DOUBLE dY, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!CheckState()) {
		return S_OK;
	}

	double arr[6] = { xLeft, dX, yProjOnX, yTop, xProjOnY, dY };
	CPLErr result = _dataset->SetGeoTransform(arr);
	*retVal = result == CE_None ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// ********************************************************
//     SetProjection
// ********************************************************
STDMETHODIMP CGdalDataset::SetProjection(BSTR Projection, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!CheckState()) {
		return S_OK;
	}

	USES_CONVERSION;

	CPLErr result = _dataset->SetProjection(OLE2A(Projection));
	*retVal = result == CE_None ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// ********************************************************
//    get_SubDatasetCount
// ********************************************************
STDMETHODIMP CGdalDataset::get_SubDatasetCount(LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = 0;

	if (CheckState())
	{
		char** papszMetadata = _dataset->GetMetadata("SUBDATASETS");
		*retVal = CSLCount(papszMetadata) / 2;
	}

	return S_OK;
}

// ********************************************************
//    GetSubDatasetName
// ********************************************************
STDMETHODIMP CGdalDataset::GetSubDatasetName(int subDatasetIndex, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (CheckState())
	{
		char** papszMetadata = _dataset->GetMetadata("SUBDATASETS");
		int count = CSLCount(papszMetadata) / 2;
		if (subDatasetIndex >= 0 && subDatasetIndex < count)
		{
			CString s;
			s.Format("SUBDATASET_%d_NAME", subDatasetIndex + 1);
			const char* val = CSLFetchNameValue(papszMetadata, s);
			*retVal = W2BSTR(Utility::ConvertFromUtf8(val));
			return S_OK;
		}
		else
		{
			ErrorMessage("Invalid sub datasource index.");
		}
	}

	*retVal = m_globalSettings.CreateEmptyBSTR();
	return S_OK;
}
