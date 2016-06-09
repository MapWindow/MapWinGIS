#include "stdafx.h"
#include "GdalDriverManager.h"
#include "GdalDriver.h"

// ********************************************************
//	  get_DriverCount
// ********************************************************
STDMETHODIMP CGdalDriverManager::get_DriverCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _manager->GetDriverCount();

	return S_OK;
}

// ********************************************************
//	  get_Driver
// ********************************************************
STDMETHODIMP CGdalDriverManager::get_Driver(LONG driverIndex, IGdalDriver** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = NULL;

	// NULL will be returned on invalid index,
	// let GDAL return the error message
	GDALDriver* driver = _manager->GetDriver(driverIndex);		

	CreateDriverInstance(driver, pVal);

	return S_OK;
}

// ********************************************************
//	  get_DriverByName
// ********************************************************
STDMETHODIMP CGdalDriverManager::get_DriverByName(BSTR driverName, IGdalDriver** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	
	GDALDriver* driver = _manager->GetDriverByName(OLE2A(driverName));

	CreateDriverInstance(driver, pVal);

	return S_OK;
}

// ********************************************************
//	  CreateDriverInstance
// ********************************************************
void CGdalDriverManager::CreateDriverInstance(GDALDriver* driver, IGdalDriver** pVal)
{
	if (driver)
	{
		ComHelper::CreateInstance(idGdalDriver, (IDispatch**)pVal);
		((CGdalDriver*)*pVal)->Inject(driver);
	}
}