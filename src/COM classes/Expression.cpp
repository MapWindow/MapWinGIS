#include "stdafx.h"
#include "Expression.h"
#include "Functions.h"
#include "Function.h"

// **********************************************************
//		Parse
// **********************************************************
STDMETHODIMP CExpression::Parse(BSTR expr, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*retVal  = _expression.Parse(OLE2A(expr), true, _lastErrorMessage) ? VARIANT_TRUE: VARIANT_FALSE;

	return S_OK;
}

// **********************************************************
//		Evaluate
// **********************************************************
STDMETHODIMP CExpression::Evaluate(VARIANT* result, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CExpressionValue* value = _expression.Calculate(_lastErrorMessage);
	if (!value)
	{
		*retVal = VARIANT_FALSE;
		return S_OK;
	}

	// TODO: populate result with value

	return S_OK;
}

// **********************************************************
//		get_LastErrorMessage
// **********************************************************
STDMETHODIMP CExpression::get_LastErrorMessage(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = A2BSTR(_lastErrorMessage);

	return S_OK;
}

// **********************************************************
//		get_LastErrorPosition
// **********************************************************
STDMETHODIMP CExpression::get_LastErrorPosition(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _lastErrorPosition;

	return S_OK;
}

// **********************************************************
//		get_NumSupportedFunctions
// **********************************************************
STDMETHODIMP CExpression::get_NumSupportedFunctions(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = parser::functions.size();

	return S_OK;
}

// **********************************************************
//		get_SupportedFunction
// **********************************************************
STDMETHODIMP CExpression::get_SupportedFunction(LONG functionIndex, IFunction** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = NULL;

	vector<CustomFunction*> list = parser::functions;

	if (functionIndex < 0 || functionIndex >= (int)list.size())
	{
		CallbackHelper::ErrorMsg("CExpression::get_SupportedFunction: index out of bounds");
		return S_OK;
	}

	IFunction* fn = NULL;
	ComHelper::CreateInstance(idFunction, (IDispatch**)&fn);
	((CFunction*)fn)->Inject(list[functionIndex]);

	*pVal = fn;

	return S_OK;
}
