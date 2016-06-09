// FieldStatOperations.cpp : Implementation of CFieldStatOperations

#include "stdafx.h"
#include "FieldStatOperations.h"

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP CFieldStatOperations::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CFieldStatOperations::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	USES_CONVERSION;
	_key = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CFieldStatOperations::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	return;
}

STDMETHODIMP CFieldStatOperations::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

STDMETHODIMP CFieldStatOperations::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

//****************************************************************
//		get_FieldIndex()
//****************************************************************
STDMETHODIMP CFieldStatOperations::get_FieldIndex(int index, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( index < 0 || index >= (long)_operations.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = -1;
	}
	else
	{
		*retVal = _operations[index]->fieldIndex;
	}
	return S_OK;
};

//****************************************************************
//		get_FieldName()
//****************************************************************
STDMETHODIMP CFieldStatOperations::get_FieldName(int index, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( index < 0 || index >= (long)_operations.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else
	{
		*retVal = W2BSTR(_operations[index]->fieldName);
	}
	return S_OK;
};

//****************************************************************
//			get_Operation()
//****************************************************************
STDMETHODIMP CFieldStatOperations::get_Operation(int index, tkFieldStatOperation* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( index < 0 || index >= (long)_operations.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = (tkFieldStatOperation)0;
	}
	else
	{
		*retVal = _operations[index]->operation;
	}
	return S_OK;
};

//****************************************************************
//			get_Count()
//****************************************************************
STDMETHODIMP CFieldStatOperations::get_Count(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _operations.size();
	return S_OK;
};

// ***************************************************************
//		AddFieldIndex()
// ***************************************************************
STDMETHODIMP CFieldStatOperations::AddFieldIndex(int fieldIndex, tkFieldStatOperation operation)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	FieldOperation* op = new FieldOperation();
	op->fieldIndex = fieldIndex;
	op->operation = operation;
	op->valid = true;
	op->hasName = false;
	_operations.push_back(op);
	return S_OK;
}

// ***************************************************************
//		AddFieldName()
// ***************************************************************
STDMETHODIMP CFieldStatOperations::AddFieldName(BSTR fieldName, tkFieldStatOperation operation)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	FieldOperation* op = new FieldOperation();
	op->fieldName = OLE2W(fieldName);
	op->operation = operation;
	op->valid = true;
	op->hasName = true;
	_operations.push_back(op);
	return S_OK;
}

// ***************************************************************
//		Remove()
// ***************************************************************
STDMETHODIMP CFieldStatOperations::Remove(int index, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	if( index < 0 || index >= (long)_operations.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		delete _operations[index];
		_operations.erase(_operations.begin() + index);
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// ***************************************************************
//		Clear()
// ***************************************************************
STDMETHODIMP CFieldStatOperations::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	for(size_t i = 0; i < _operations.size(); i++)
	{
		delete _operations[i];
	}
	_operations.clear();
	return S_OK;
}

//***********************************************************************/
//*			Validate()
//***********************************************************************/
STDMETHODIMP CFieldStatOperations::Validate(IShapefile* sf, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (!sf)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
	}
	else
	{
		long numFields;
		sf->get_NumFields(&numFields);

		for(size_t i = 0; i < _operations.size(); i++)
		{
			FieldOperation* op = _operations[i];
			op->valid = false;
			op->isValidReason = fovValid;

			// searching index for name
			if (op->hasName)
			{
				CComPtr<ITable> table = NULL;
				sf->get_Table(&table);
				if (table)
				{
					long fieldIndex;
					CComBSTR bstrName(op->fieldName);
					table->get_FieldIndexByName(bstrName, &fieldIndex);
					op->fieldIndex = fieldIndex;
				}
			}
			
			IField* field = NULL;
			if (op->fieldIndex >= 0 && op->fieldIndex < numFields)
			{
				sf->get_Field(op->fieldIndex, &field);
				if (field)
				{
					FieldType type;
					field->get_Type(&type);
					op->valid = !((op->operation == fsoSum || op->operation == fsoAvg || op->operation == fsoWeightedAvg) 
									&& type == STRING_FIELD);
					if (op->operation == fsoMode && type != STRING_FIELD)
						op->valid = false;
					field->Release();
					op->isValidReason = op->valid ? fovValid : fovNotSupported;
				}
			}
			else
			{
				op->isValidReason = fovFieldNotFound;
			}
		}

		*retVal = VARIANT_TRUE;
		for(size_t i = 0; i < _operations.size(); i++)
		{
			if (!_operations[i]->valid)
			{
				*retVal = VARIANT_FALSE;
				break;
			}
		}
	}
	return S_OK;
}

//****************************************************************
//			get_OperationIsValid
//****************************************************************
STDMETHODIMP CFieldStatOperations::get_OperationIsValid(int index, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( index < 0 || index >= (long)_operations.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		*retVal = _operations[index]->valid;
	}
	return S_OK;
}

//****************************************************************
//			get_OperationIsValidReason
//****************************************************************
STDMETHODIMP CFieldStatOperations::get_OperationIsValidReason(int index, tkFieldOperationValidity* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( index < 0 || index >= (long)_operations.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = fovValid;
	}
	else
	{
		*retVal = _operations[index]->isValidReason;
	}
	return S_OK;
}