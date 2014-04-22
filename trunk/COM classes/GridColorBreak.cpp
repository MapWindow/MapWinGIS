//********************************************************************************************************
//File name: GridColorBreak.cpp
//Description:  Implementation of CGridColorBreak.
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
#include "GridColorBreak.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CGridColorBreak
STDMETHODIMP CGridColorBreak::get_HighColor(OLE_COLOR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*pVal = HighColor;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_HighColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	HighColor = newVal;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_LowColor(OLE_COLOR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	*pVal = LowColor;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_LowColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	LowColor = newVal;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_HighValue(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	*pVal = HighValue;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_HighValue(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	HighValue = newVal;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_LowValue(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	*pVal = LowValue;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_LowValue(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	LowValue = newVal;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_ColoringType(ColoringType *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	*pVal = ColorType;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_ColoringType(ColoringType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	ColorType = newVal;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_GradientModel(GradientModel *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	*pVal = GradModel;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_GradientModel(GradientModel newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	
	GradModel = newVal;

	return S_OK;
}


STDMETHODIMP CGridColorBreak::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = lastErrorCode;
	lastErrorCode = tkNO_ERROR;

	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = globalCallback;
	if( globalCallback )
	{	
		globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	return S_OK;
}

STDMETHODIMP CGridColorBreak::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	*pVal = OLE2BSTR(key);

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	::SysFreeString(key);
	key = OLE2BSTR(newVal);

	return S_OK;
}


STDMETHODIMP CGridColorBreak::get_Caption(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = m_Caption.AllocSysString();

	return S_OK;
}

STDMETHODIMP CGridColorBreak::put_Caption(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_Caption = newVal;

	return S_OK;
}
