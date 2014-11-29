//********************************************************************************************************
//File name: ESRIGridManager.cpp
//Description:  Implementation of CESRIGridManager.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//3-28-2005 dpa - Identical to public domain version.
//********************************************************************************************************

#include "stdafx.h"
#include "ESRIGridManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CESRIGridManager
STDMETHODIMP CESRIGridManager::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = tkNO_ERROR;

	return S_OK;
}

STDMETHODIMP CESRIGridManager::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CESRIGridManager::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _globalCallback;
	_globalCallback->AddRef();

	return S_OK;
}

STDMETHODIMP CESRIGridManager::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

STDMETHODIMP CESRIGridManager::CanUseESRIGrids(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_gm.canUseEsriGrids())
		*retval = VARIANT_TRUE;
	else
        *retval = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CESRIGridManager::DeleteESRIGrids(BSTR Filename, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	
	if (_gm.deleteGrid(OLE2CA(Filename),ESRI_GRID))
        *retval = VARIANT_TRUE;
	else
		*retval = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CESRIGridManager::IsESRIGrid(BSTR Filename, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	USES_CONVERSION;
	if( _gm.getGridType(OLE2CA(Filename)) == ESRI_GRID ) 
		*retval = VARIANT_TRUE;
	else
		*retval = VARIANT_FALSE;

	return S_OK;
}
