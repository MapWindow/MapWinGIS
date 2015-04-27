// GdalDataset.cpp : Implementation of CGdalDataset
#include "stdafx.h"
#include "GdalDataset.h"
#include "GdalHelper.h"
#include "GdalDriver.h"

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

