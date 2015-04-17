// GdalRasterBand.cpp : Implementation of CGdalRasterBand

#include "stdafx.h"
#include "GdalRasterBand.h"

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


