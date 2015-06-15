#include "stdafx.h"
#include "Function.h"

// **********************************************************
//		Validate
// **********************************************************
bool CFunction::Validate()
{
	if (!_function)
	{
		CallbackHelper::ErrorMsg("Function class doesn't reference any function.");
		return false;
	}

	return true;
}

// **********************************************************
//		get_Name
// **********************************************************
STDMETHODIMP CFunction::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!Validate()) 
	{
		*pVal = m_globalSettings.CreateEmptyBSTR();
		return S_OK;
	}

	USES_CONVERSION;
	*pVal = A2BSTR(_function->name());

	return S_OK;
}

// **********************************************************
//		get_Alias
// **********************************************************
STDMETHODIMP CFunction::get_Alias(long aliasIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (Validate())
	{
		aliasIndex++;    // the first alias is name

		vector<CString>* aliases = _function->GetAliases();

		if (aliasIndex <= 0 || aliasIndex >= aliases->size())
		{
			CallbackHelper::ErrorMsg("Function::get_Alias: index out of bounds.");
		}
		else
		{
			*pVal = A2BSTR((*aliases)[aliasIndex]);
			return S_OK;
		}
	}

	*pVal = m_globalSettings.CreateEmptyBSTR();
	return S_OK;
}

// **********************************************************
//		get_NumAliases
// **********************************************************
STDMETHODIMP CFunction::get_NumAliases(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!Validate()) 
	{
		*pVal = 0;
		return S_OK;
	}

	vector<CString>* aliases = _function->GetAliases();

	*pVal = aliases->size() - 1;
	
	if (*pVal == -1)  {
		*pVal = 0;
	}

	return S_OK;
}

// **********************************************************
//		get_NumParameters
// **********************************************************
STDMETHODIMP CFunction::get_NumParameters(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!Validate()) 
	{
		*pVal = 0;
		return S_OK;
	}	

	*pVal = _function->numParams();

	return S_OK;
}

// **********************************************************
//		get_Group
// **********************************************************
STDMETHODIMP CFunction::get_Group(tkFunctionGroup* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!Validate()) 
	{
		*pVal = fgMath;
		return S_OK;
	}

	*pVal = _function->group();

	return S_OK;
}

