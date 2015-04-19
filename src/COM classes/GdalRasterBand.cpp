// GdalRasterBand.cpp : Implementation of CGdalRasterBand

#include "stdafx.h"
#include "GdalRasterBand.h"
#include "RasterBandHelper.h"

// *************************************************************
//	  CheckBand()
// *************************************************************
bool CGdalRasterBand::CheckBand()
{
	if (!_band)
	{
		ErrorMessage("GdalRasterBand object isn't initialized.");
		return false;
	}
	return true;
}

// *************************************************************
//	  ErrorMessage()
// *************************************************************
void CGdalRasterBand::ErrorMessage(CString msg)
{
	CallbackHelper::ErrorMsg("GdalRasterBand", NULL, m_globalSettings.emptyBstr, msg);
}

// *************************************************************
//	  get_NoDataValue()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_NoDataValue(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0.0;

	if (!CheckBand()) return S_OK;

	int success;
	*pVal = _band->GetNoDataValue(&success);

	if (!success)
	{
		ErrorMessage("Failed to retrieve no data value.");
	}

	return S_OK;
}

// *************************************************************
//	  get_Minimum()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Minimum(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0.0;

	if (!CheckBand()) return S_OK;

	int success;
	*pVal = _band->GetMinimum(&success);

	if (!success)
	{
		ErrorMessage("Failed to retrieve minimum.");
	}

	return S_OK;
}

// *************************************************************
//	  get_Maximum()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Maximum(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0.0;

	if (!CheckBand()) return S_OK;

	int success;
	*pVal = _band->GetMaximum(&success);

	if (!success)
	{
		ErrorMessage("Failed to retrieve maximum.");
	}

	return S_OK;
}

// *************************************************************
//	  get_OverviewCount()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_OverviewCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0;

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetOverviewCount();

	return S_OK;
}

// *************************************************************
//	  get/put_ColorInterpretation()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_ColorInterpretation(tkColorInterpretation* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = (tkColorInterpretation)_band->GetColorInterpretation();

	return S_OK;
}

STDMETHODIMP CGdalRasterBand::put_ColorInterpretation(tkColorInterpretation newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	_band->SetColorInterpretation((GDALColorInterp)newVal);		// error will be returned by GDAL callback

	return S_OK;
}

// *************************************************************
//	  get_DataType()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_DataType(tkGdalDataType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = (tkGdalDataType)_band->GetRasterDataType();

	return S_OK;
}

// *************************************************************
//	  get_XSize()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_XSize(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetXSize();

	return S_OK;
}

// *************************************************************
//	  get_YSize()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_YSize(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetYSize();

	return S_OK;
}

// *************************************************************
//	  get_BlockSizeX()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_BlockSizeX(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!CheckBand()) return S_OK;

	int x, y;
	 _band->GetBlockSize(&x, &y);

	 *pVal = x;

	return S_OK;
}

// *************************************************************
//	  get_BlockSizeY()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_BlockSizeY(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	int x, y;
	_band->GetBlockSize(&x, &y);

	*pVal = y;

	return S_OK;
}

// *************************************************************
//	  get_UnitType()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_UnitType(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	USES_CONVERSION;
	*pVal = A2BSTR(_band->GetUnitType());

	return S_OK;
}

// *************************************************************
//	  get_Scale()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Scale(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetScale();

	return S_OK;
}

// *************************************************************
//	  get_Offset()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Offset(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetOffset();

	return S_OK;
}

// *************************************************************
//	  get_HasColorTable()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_HasColorTable(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	GDALColorTable* table = _band->GetColorTable();

	*pVal = table != NULL;

	return S_OK;
}

// *************************************************************
//	  get_MetadataCount()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_MetadataCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	char** data = _band->GetMetadata();
	*pVal = CSLCount(data);

	return S_OK;
}

// *************************************************************
//	  get_MetadataItem()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_MetadataItem(LONG itemIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = A2BSTR("");

	if (!CheckBand()) return S_OK;

	char** data = _band->GetMetadata();
	if (itemIndex < 0 || itemIndex >= CSLCount(data))
	{
		ErrorMessage("Metadata item index is out of range.");
		return S_OK;
	}
	else
	{
		char* item = const_cast<char*>(CSLGetField(data, itemIndex));
		*pVal = A2BSTR(item);
		return S_OK;
	}

	return S_OK;
}

// *************************************************************
//	  get_ColorTable()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_ColorTable(IGridColorScheme** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	RasterBandHelper::ColorTableToColorScheme(_band, pVal);

	return S_OK;
}
