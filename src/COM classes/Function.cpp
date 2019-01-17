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
	*pVal = W2BSTR(_function->GetName());

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

		vector<CStringW>* aliases = _function->getAliases();

		if (aliasIndex <= 0 || aliasIndex >= (long)aliases->size())
		{
			CallbackHelper::ErrorMsg("Function::get_Alias: index out of bounds.");
		}
		else
		{
			*pVal = W2BSTR((*aliases)[aliasIndex]);
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

	vector<CStringW>* aliases = _function->getAliases();

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

// **********************************************************
//		Description
// **********************************************************
STDMETHODIMP CFunction::get_Description(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (Validate())
	{
		*pVal = OLE2BSTR(_function->description());
	}
	else
	{
		*pVal = m_globalSettings.CreateEmptyBSTR();
	}

	return S_OK;
}

STDMETHODIMP CFunction::put_Description(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// may be useful for localization, currently not exposed to API

	return S_OK;
}

// **********************************************************
//		ParameterName
// **********************************************************
STDMETHODIMP CFunction::get_ParameterName(LONG parameterIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!Validate())
	{
		return S_OK;
	}

	if (parameterIndex < 0 || parameterIndex >= _function->numParams())
	{
		CallbackHelper::ErrorMsg("CFunction::get_ParameterName: parameter index out of bounds.");
		return S_OK;
	}

	FunctionParameter* p = _function->getParameter(parameterIndex);
	if (p)
	{
		USES_CONVERSION;
		*pVal = OLE2BSTR(p->name);
		return S_OK;
	}

	*pVal = m_globalSettings.CreateEmptyBSTR();

	return S_OK;
}

STDMETHODIMP CFunction::put_ParameterName(LONG parameterIndex, BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// may be useful for localization, currently not exposed to API

	return S_OK;
}

// **********************************************************
//		ParameterDescription
// **********************************************************
STDMETHODIMP CFunction::get_ParameterDescription(LONG parameterIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!Validate())
	{
		return S_OK;
	}

	if (parameterIndex < 0 || parameterIndex >= _function->numParams())
	{
		CallbackHelper::ErrorMsg("CFunction::get_ParameterDescription: parameter index out of bounds.");
		return S_OK;
	}

	FunctionParameter* p = _function->getParameter(parameterIndex);
	if (p)
	{
		USES_CONVERSION;
		*pVal = OLE2BSTR(p->description);
		return S_OK;
	}

	*pVal = m_globalSettings.CreateEmptyBSTR();

	return S_OK;
}

STDMETHODIMP CFunction::put_ParameterDescription(LONG parameterIndex, BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// may be useful for localization, currently not exposed to API

	return S_OK;
}

// **********************************************************
//		get_Signature
// **********************************************************
STDMETHODIMP CFunction::get_Signature(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (Validate())
	{
		USES_CONVERSION;
		*pVal = OLE2BSTR(_function->GetSignature());
	}
	else
	{
		*pVal = m_globalSettings.CreateEmptyBSTR();
	}

	return S_OK;
}
