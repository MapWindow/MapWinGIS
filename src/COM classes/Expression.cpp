#include "stdafx.h"
#include "Expression.h"
#include "Functions.h"
#include "Function.h"
#include "TableHelper.h"

// **********************************************************
//		Clear
// **********************************************************
void CExpression::Clear()
{
	if (_table)
	{
		_table->Release();
		_table = NULL;
	}
}

// **********************************************************
//		ValidateExpression
// **********************************************************
bool CExpression::ValidateExpression()
{
	if (_expression.IsEmpty())
	{
		CallbackHelper::ErrorMsg("Expression::Calculate: expression is empty. Call Expression.Parse first.");
		return false;
	}

	return true;
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

// **********************************************************
//		Parse
// **********************************************************
STDMETHODIMP CExpression::Parse(BSTR expr, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	Clear();

	USES_CONVERSION;
	*retVal = _expression.Parse(OLE2A(expr), true, _lastErrorMessage) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// **********************************************************
//		ParseForTable
// **********************************************************
STDMETHODIMP CExpression::ParseForTable(BSTR expr, ITable* table, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!table)
	{
		CallbackHelper::ErrorMsg("CExpression::ParseForTable: unexpected NULL parameter - table");
		return S_OK;
	}

	Clear();

	ComHelper::SetRef(table, (IDispatch**)&_table);

	_expression.ReadFieldNames(table);

	USES_CONVERSION;
	*retVal = _expression.Parse(OLE2A(expr), true, _lastErrorMessage) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// **********************************************************
//		Calculate
// **********************************************************
STDMETHODIMP CExpression::Calculate(VARIANT* result, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!ValidateExpression()) 
	{
		return S_OK;
	}

	*retVal = CalculateCore(result) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// **********************************************************
//		CalculateForTableRow
// **********************************************************
STDMETHODIMP CExpression::CalculateForTableRow2(LONG rowIndex, VARIANT* result, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!ValidateExpression()) 
	{
		return S_OK;
	}	

	if (!_table)
	{
		CallbackHelper::ErrorMsg("Expression::CalculateForTableRow: there is no table associated with expression. Call Expression.ParseForTable first.");
		return S_OK;
	}

	TableHelper::SetFieldValues(_table, rowIndex, _expression);

	*retVal = CalculateCore(result) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// **********************************************************
//		CalculateForTableRow2
// **********************************************************
STDMETHODIMP CExpression::CalculateForTableRow(LONG rowIndex, LONG targetFieldIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!ValidateExpression())
	{
		return S_OK;
	}

	if (!_table)
	{
		CallbackHelper::ErrorMsg("Expression::CalculateForTableRow: there is no table associated with expression. Call Expression.ParseForTable first.");
		return S_OK;
	}

	if (targetFieldIndex < 0 || targetFieldIndex >= TableHelper::GetNumRows(_table))
	{
		CallbackHelper::ErrorMsg("CExpression::CalculateForTableRow: targetFieldIndex is out of bounds.");
		return S_OK;
	}

	TableHelper::SetFieldValues(_table, rowIndex, _expression);

	CExpressionValue*value = _expression.Calculate(_lastErrorMessage);

	if (value)
	{
		CComVariant var;
		SetVariant(value, &var);
		
		_table->EditCellValue(targetFieldIndex, rowIndex, var, retVal);		// *retVal = VARIANT_TRUE will be set here
	}

	return S_OK;
}

// **********************************************************
//		CalculateCore
// **********************************************************
bool CExpression::CalculateCore(VARIANT* result)
{
	CExpressionValue* value = _expression.Calculate(_lastErrorMessage);
	if (value)
	{
		SetVariant(value, result);
		return true;
	}

	return false;
}

// **********************************************************
//		SetVariant
// **********************************************************
void CExpression::SetVariant(CExpressionValue* value, VARIANT* result)
{
	if (!result) return;

	if (value->isBoolean())
	{
		result->vt = VT_BOOL;
		result->boolVal = value->bln();
	}
	else if (value->IsDouble())
	{
		result->vt = VT_R8;
		result->dblVal = value->dbl();
	}
	else if (value->isString())
	{
		result->vt = VT_BSTR;
		result->bstrVal = W2BSTR(value->str());
	}
}

// **********************************************************
//		get_Table
// **********************************************************
STDMETHODIMP CExpression::get_Table(ITable** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_table)	{
		_table->AddRef();
	}

	*pVal = _table;

	return S_OK;
}

