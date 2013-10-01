//********************************************************************************************************
//File name: Field.cpp
//Description: Implementation of CField 
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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
#include "Field.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// *********************************************************************
//		get/put_Name
// *********************************************************************
STDMETHODIMP CField::get_Name(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(name);
	return S_OK;
}

STDMETHODIMP CField::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	
	if( _tcslen( OLE2CA(newVal) ) > 0 )
	{	
		if (!CheckTableEditingState()) return S_OK;
		::SysFreeString(name);
		name = OLE2BSTR(newVal);
		isUpdated = true;
	}
	else
		ErrorMessage(tkZERO_LENGTH_STRING);
	
	return S_OK;
}

// *********************************************************************
//		get/put_Width
// *********************************************************************
STDMETHODIMP CField::get_Width(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = width;
	return S_OK;
}
STDMETHODIMP CField::put_Width(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( newVal >= 0 )
	{
		if (!CheckTableEditingState()) return S_OK;
		width = newVal;
		isUpdated = true;
	}
	else
		ErrorMessage(tkINVALID_PARAMETER_VALUE);

	return S_OK;
}

// *********************************************************************
//		get/put_Precision
// *********************************************************************
STDMETHODIMP CField::get_Precision(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = precision;
	return S_OK;
}

STDMETHODIMP CField::put_Precision(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( newVal >= 0 )
	{
		if (!CheckTableEditingState()) return S_OK;
		precision = newVal;
		isUpdated = true;
	}
	else
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	
	return S_OK;
}

// *********************************************************************
//		get/put_Type
// *********************************************************************
STDMETHODIMP CField::get_Type(FieldType *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = type;
	return S_OK;
}

STDMETHODIMP CField::put_Type(FieldType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!CheckTableEditingState()) return S_OK;
	type = newVal;
	isUpdated = true;
	return S_OK;
}

// *********************************************************************
//		CheckTableEditingState
// *********************************************************************
bool CField::CheckTableEditingState()
{
	if (_table != NULL)
	{
		VARIANT_BOOL editing;
		_table->get_EditingTable(&editing);
		if(!editing)
		{
			ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
			return false;
		}
		else
		{
			return true;
		}
	}
	else
		return true;	// if there is no parent table, we can change the settings freely
	
}

// *********************************************************************
//		get_LastErrorCode
// *********************************************************************
STDMETHODIMP CField::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = lastErrorCode;
	lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// *********************************************************************
//		get_ErrorMsg
// *********************************************************************
STDMETHODIMP CField::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// *********************************************************************
//		get/put_GlobalCallback
// *********************************************************************
STDMETHODIMP CField::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = globalCallback;
	if( globalCallback )
		globalCallback->AddRef();

	return S_OK;
}

STDMETHODIMP CField::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	return S_OK;
}

// *********************************************************************
//		get/put_Key
// *********************************************************************
STDMETHODIMP CField::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = OLE2BSTR(key);

	return S_OK;
}

STDMETHODIMP CField::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	::SysFreeString(key);
	key = OLE2BSTR(newVal);

	return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CField::ErrorMessage(long ErrorCode)
{
	lastErrorCode = ErrorCode;
	if( globalCallback != NULL) 
		globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
	return;
}

// **************************************************************
//		Clone()
// **************************************************************
STDMETHODIMP CField::Clone(/*[out, retval]*/ IField** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	IField* fld = NULL;
	CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&fld);
	fld->put_Precision(precision);
	fld->put_Type(type);
	fld->put_Name(name);
	fld->put_Width(width);
	*retVal = fld;
	return S_OK;
}