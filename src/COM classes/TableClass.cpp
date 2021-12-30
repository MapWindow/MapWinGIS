/**************************************************************************************
 * File name: TableClass.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control)
 * Description: implementation of CTableClass
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** */

#include "StdAfx.h"
#include "TableClass.h"
#include <algorithm>
#include <functional>
#include "Templates.h"
#include "JenksBreaks.h"
#include "Field.h"
#include "TableHelper.h"
#include "FieldHelper.h"
#include "Shapefile.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ReSharper disable CppUseAuto

// *****************************************************
//	ParseExpressionCore()
// *****************************************************
void CTableClass::ParseExpressionCore(BSTR Expression, tkValueType returnType, CStringW& errorString, VARIANT_BOOL* retVal)
{
	*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CStringW str = OLE2CW(Expression);
	CustomExpression expr;

	if (!expr.ReadFieldNames(this))
	{
		errorString = "Failed to read field names";
		return;
	}


	if (!expr.Parse(str, true, errorString))
	{
		return;
	}

	// testing with values of the first row; there can be inconsistent data types for example
	int i = 0;

	TableHelper::SetFieldValues(this, i, expr);

	// if expression returns true for the given record we'll save the index 
	CExpressionValue* result = expr.Calculate(errorString);
	if (result)
	{
		if (result->type() != returnType)
		{
			if (returnType == vtString)
			{
				// there is no problem to convert any type to string						
				*retVal = VARIANT_TRUE;
			}
			else
			{
				errorString = "Invalid resulting type";
			}
		}

		*retVal = VARIANT_TRUE;
	}
}

// *****************************************************
//		ParseExpression()
// *****************************************************
//  Checks the correctness of the expression syntax, but doesn't check the validity of data types
STDMETHODIMP CTableClass::ParseExpression(BSTR Expression, BSTR* ErrorString, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CStringW str = OLE2CW(Expression);

	CustomExpression expr;

	if (expr.ReadFieldNames(this))
	{
		CStringW err;
		if (expr.Parse(str, true, err))
		{
			*retVal = VARIANT_TRUE;
		}
		else
		{
			*ErrorString = W2BSTR(err);
		}
	}
	else
	{
		*ErrorString = SysAllocString(L"Failed to read field names");
	}

	return S_OK;
}

// *****************************************************************
//		TestExpression()
// *****************************************************************
// Checks syntax of expression and data types based on the first record in the table
STDMETHODIMP CTableClass::TestExpression(BSTR Expression, tkValueType ReturnType, BSTR* ErrorString, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CStringW err;
	ParseExpressionCore(Expression, ReturnType, err, retVal);

	*ErrorString = W2BSTR(err);
	return S_OK;
}

// *****************************************************************
//		Query()
// *****************************************************************
STDMETHODIMP CTableClass::Query(BSTR Expression, VARIANT* Result, BSTR* ErrorString, VARIANT_BOOL* retVal)
{
	*retVal = VARIANT_FALSE;
	SysFreeString(*ErrorString);	// do we need it here?
	USES_CONVERSION;
	CStringW str = OLE2CW(Expression);

	std::vector<long> indices;
	CStringW err;
	if (QueryCore(str, indices, err))
	{
		*ErrorString = SysAllocString(L"");
		if (indices.size() == 0)
		{
			*ErrorString = SysAllocString(L"Selection is empty");
			Result = NULL;
		}
		else
		{
			bool ret = Templates::Vector2SafeArray(&indices, VT_I4, Result);
			*retVal = ret ? VARIANT_TRUE : VARIANT_FALSE;
		}
	}
	else
	{
		*ErrorString = W2BSTR(err);
	}
	return S_OK;
}

// *****************************************************************
//		Calculate()
// *****************************************************************
STDMETHODIMP CTableClass::Calculate(BSTR Expression, LONG RowIndex, VARIANT* Result, BSTR* ErrorString, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;

	*retVal = VARIANT_FALSE;
	Result->vt = VT_NULL;

	if (RowIndex < 0 || RowIndex >= RowCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

    CStringW err;
    return CalculateCoreRaw(
        Expression,
        [&](CExpressionValue* result, int rowIndex, CStringW& ErrorString) -> int {
            if (result->isBoolean())
            {
                Result->vt = VT_BOOL;
                Result->boolVal = result->bln();
            }
            else if (result->IsDouble())
            {
                Result->vt = VT_R8;
                Result->dblVal = result->dbl();
            }
            else if (result->isString())
            {
                Result->vt = VT_BSTR;
                Result->bstrVal = W2BSTR(result->str());
            }
            return true;
        },
        err,
        m_globalSettings.floatNumberFormat, RowIndex, true
    );

    *ErrorString = W2BSTR(err);
    return S_OK;
}

// ********************************************************************
//			Query_()
// ********************************************************************
bool CTableClass::QueryCore(CStringW Expression, std::vector<long>& indices, CStringW& ErrorString)
{
	indices.clear();
    return CalculateCoreRaw(
        Expression,
        [&indices](CExpressionValue* result, int rowIndex, CStringW& ErrorString) -> int {
            if (!result->isBoolean())
            {
                ErrorString = "Resulting value isn't boolean";
                return false;
            }
            
            if (result->bln())
                indices.push_back(rowIndex);

            return true;
        },
        ErrorString
    );
}

// ********************************************************************
//			CalculateCore()
// ********************************************************************
bool CTableClass::CalculateCoreRaw(CStringW Expression, 
    std::function<bool(CExpressionValue* value, int rowIndex, CStringW& ErrorString)> processValue, 
    CStringW& ErrorString, CString floatFormat, int startRowIndex, int endRowIndex, bool ignoreCalculationErrors)
{
    USES_CONVERSION;

    CustomExpression expr;
    expr.SetFloatFormat(floatFormat);
    if (!expr.ReadFieldNames(this))
    {
        ErrorString = "Failed to read field names";
        return false;
    }

    CStringW err;
    if (!expr.Parse(Expression, true, err))
    {
        ErrorString = err;
        return false;
    }

    bool error = false;
    CStringW str;

    int start = (startRowIndex == -1) ? 0 : startRowIndex;
    int end = (endRowIndex == -1) ? int(_rows.size()) : endRowIndex + 1;

    for (int i = start; i < end; i++)
    {
        TableHelper::SetFieldValues(this, i, expr);

        // calculate expression
        CExpressionValue* result = expr.Calculate(err);
        
        // check if we can ignore calculation errors:
        if (!result && ignoreCalculationErrors)
        {
            err = ""; // clear the error
            continue;
        }
            
        // if we had a result & processing went fine, continue
        if (result && processValue(result, i, err))
            continue;
        else
        {
            ErrorString = err;
            error = true;
            return false;
        }
    }

    return true;
}

// ********************************************************************
//			CalculateCore()
// ********************************************************************
bool CTableClass::CalculateCore(CStringW Expression, std::vector<CStringW>& results, CStringW& ErrorString,
	CString floatFormat, int startRowIndex, int endRowIndex)
{
	results.clear();
    return CalculateCoreRaw(
        Expression, 
        [&](CExpressionValue* result, int rowIndex, CStringW& ErrorString) -> int {
            USES_CONVERSION;
            CStringW str;
            if (result->isString())
            {
                str = result->str();
                results.push_back(str);
            }
            else if (result->isBoolean())
            {
                str = result->bln() ? "true" : "false";
                results.push_back(str);
            }
            else
            {
                str.Format(A2W(floatFormat), result->dbl());
                results.push_back(str);
            }
            return true;
        },
        ErrorString, floatFormat, startRowIndex, endRowIndex
    );
}

// ****************************************************************
//	  AnalyzeExpressions()
// ****************************************************************
// Analyzes list of expressions and returns vector of size equal to numShapes where 
// for each shape index a category is specified. If the given shape didn't fall into
// any category -1 is used. The first categories in the list have higher priority
// Results vector with certain categories can be provided by caller; those categories won't be changed
void CTableClass::AnalyzeExpressions(std::vector<CStringW>& expressions, std::vector<int>& results,
	int startRowIndex, int endRowIndex)
{
	// TODO: optimize, if all expressions have the same fields in the same positions
	// don't read the values multiple times.
	// For unique values classification it's better to store classification field,
	// but in older style files it may be not present.
	CustomExpression expr;
	if (!expr.ReadFieldNames(this)) return;

	startRowIndex = startRowIndex < 0 ? 0 : startRowIndex;

	for (unsigned int categoryId = 0; categoryId < expressions.size(); categoryId++)
	{
		if (expressions[categoryId] != "")
		{
			CStringW ErrorString;
            CalculateCoreRaw(
                expressions[categoryId],
                [&](CExpressionValue* result, int rowIndex, CStringW& ErrorString) -> int {
                    USES_CONVERSION;
                    if (result->isBoolean() && result->bln())
                    {
                        results[rowIndex-startRowIndex] = categoryId;
                    }
                    return true;
                },
                ErrorString, m_globalSettings.floatNumberFormat, startRowIndex, endRowIndex, true
            );
		}
	}
}

// ***********************************************************
//		get_Num_rows
// ***********************************************************
STDMETHODIMP CTableClass::get_NumRows(long *pVal)
{
	*pVal = RowCount();
	return S_OK;
}

// ***********************************************************
//		get_NumFields
// ***********************************************************
STDMETHODIMP CTableClass::get_NumFields(long *pVal)
{
	*pVal = FieldCount();
	return S_OK;
}

// ***********************************************************
//		get_Field
// ***********************************************************
STDMETHODIMP CTableClass::get_Field(long FieldIndex, IField **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	if (FieldIndex < 0 || FieldIndex >= (int)FieldCount())
	{
		*pVal = NULL;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		((CField*)_fields[FieldIndex]->field)->SetTable(this);
		*pVal = _fields[FieldIndex]->field;
		(*pVal)->AddRef();
	}
	return S_OK;
}

// ***********************************************************
//		get_CellValue
// ***********************************************************
STDMETHODIMP CTableClass::get_CellValue(long FieldIndex, long RowIndex, VARIANT *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		if (FieldIndex < 0 || FieldIndex >= FieldCount() || RowIndex < 0 || RowIndex >= RowCount())
		{
			VARIANT var;
			VariantInit(&var);
			var.vt = VT_EMPTY;
			pVal = &var;
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			return S_OK;
		}

	if (ReadRecord(RowIndex) && _rows[RowIndex].row != NULL)
	{
		VARIANT* var = _rows[RowIndex].row->values[FieldIndex];
		if (var != NULL)
		{
			if (var->vt != VT_NULL)
			{
				VariantCopy(pVal, _rows[RowIndex].row->values[FieldIndex]);
			}
			else
			{
				// MWGIS-128
				// no value, send back EMPTY variant
				VariantInit(pVal);
				pVal->vt = VT_EMPTY;
			}
			return S_OK;
		}
	}
	pVal = NULL;
	return S_OK;
}

// ***********************************************************
//		get_EditingTable
// ***********************************************************
STDMETHODIMP CTableClass::get_EditingTable(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _isEditingTable ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ***********************************************************
//		get_LastErrorCode
// ***********************************************************
STDMETHODIMP CTableClass::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// ***********************************************************
//		get_ErrorMsg
// ***********************************************************
STDMETHODIMP CTableClass::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ***********************************************************
//		get_CdlgFilter
// ***********************************************************
STDMETHODIMP CTableClass::get_CdlgFilter(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR("dBase Files (*.dbf)|*.dbf");
	return S_OK;
}

// ***********************************************************
//		get/put_GlobalCallback
// ***********************************************************
STDMETHODIMP CTableClass::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _globalCallback;
	if (_globalCallback)
		_globalCallback->AddRef();
	return S_OK;
}
STDMETHODIMP CTableClass::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// ***********************************************************
//		get/put_Key
// ***********************************************************
STDMETHODIMP CTableClass::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}

STDMETHODIMP CTableClass::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

// ***********************************************************
//		Open
// ***********************************************************
STDMETHODIMP CTableClass::Open(BSTR dbfFilename, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	*retval = VARIANT_FALSE;
	if ((cBack != NULL) && (_globalCallback == NULL))
	{
		_globalCallback = cBack;
		cBack->AddRef();
	}

	Close(retval);

	if (*retval)
	{
		CStringW name = OLE2CW(dbfFilename);
		if (!Utility::FileExistsW(name))
		{
			ErrorMessage(tkDBF_FILE_DOES_NOT_EXIST);
			return S_OK;
		}

		// checking writing permissions
		//LPCSTR name = OLE2CA(dbfFilename);
		DWORD dwAttrs = GetFileAttributesW(name);
		if (dwAttrs == INVALID_FILE_ATTRIBUTES)
		{
			ErrorMessage(tkCANT_OPEN_DBF);
			return S_OK;
		}
		bool readOnly = (dwAttrs & FILE_ATTRIBUTE_READONLY);

		if (!readOnly) {
			_dbfHandle = DBFOpen_MW(name, "rb+");
		}

		if (_dbfHandle == NULL) {
			_dbfHandle = DBFOpen_MW(name, "rb");
		}

		if (_dbfHandle == NULL)
		{
			ErrorMessage(tkCANT_OPEN_DBF);
			return S_OK;
		}

		_filename = name;
		*retval = VARIANT_TRUE;

		//After open the dbf file, load all _fields info and create spatial row indices 
		//with FieldWrapper and RecordWrapper help classes.
		LoadDefaultFields();
		LoadDefaultRows();
	}
	return S_OK;
}

// **************************************************************
//	  CreateNew()
// **************************************************************
STDMETHODIMP CTableClass::CreateNew(BSTR dbfFilename, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	// closing the existing table
	this->Close(retval);

	if (*retval == VARIANT_FALSE)
	{
		return S_OK;
	}
	else
	{
		if (Utility::FileExistsUnicode(OLE2A(dbfFilename)))
		{
			*retval = VARIANT_FALSE;
			ErrorMessage(tkDBF_FILE_EXISTS);
			return S_OK;
		}

		_filename = OLE2CA(dbfFilename);
		_isEditingTable = TRUE;
		*retval = VARIANT_TRUE;
	}
	return S_OK;
}

// ********************************************************
//     CloseUnderlyingFile()
// ********************************************************
void CTableClass::CloseUnderlyingFile()
{
	_filename = L"";
	if (_dbfHandle != NULL)
	{
		DBFClose(_dbfHandle);
		_dbfHandle = NULL;
	}
}

// ********************************************************
//     WriteAppendedRow()
// ********************************************************
bool CTableClass::WriteAppendedRow()
{
	if (_rows.size() == 0 || _rows.size() == _appendStartShapeCount) return false;

	int rowIndex = _rows.size() - 1;

	if (!WriteRecord(_dbfHandle, rowIndex, rowIndex))
	{
		ErrorMessage(tkDBF_CANT_WRITE_ROW);
		return false;
	}

	if (!m_globalSettings.cacheDbfRecords)
	{
		ClearRow(rowIndex);
	}
	else {
		_rows[rowIndex].row->SetDirty(TableRow::DATA_CLEAN);
	}

	return true;
}

// ********************************************************
//     StopAppendMode()
// ********************************************************
void CTableClass::StopAppendMode()
{
	if (_appendMode)
	{
		WriteAppendedRow();

		CStringW filename = _filename;

		VARIANT_BOOL vb;
		Close(&vb);		// _appendMode will be set to false here

		CComBSTR bstr(filename);
		Open(bstr, NULL, &vb);
	}
}

// **************************************************************
//	  SaveToFile()
// **************************************************************
bool CTableClass::SaveToFile(const CStringW& dbfFilename, bool updateFileInPlace, ICallback* cBack)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	if (_globalCallback == NULL && cBack != NULL)
	{
		_globalCallback = cBack;
		cBack->AddRef();
	}

	if (_dbfHandle == NULL && _isEditingTable == FALSE)
	{
		ErrorMessage(_lastErrorCode);
		return false;
	}

	if (Utility::FileExistsW(dbfFilename) != FALSE)
	{
		ErrorMessage(tkDBF_FILE_EXISTS);
		return false;
	}

	DBFInfo * newdbfHandle = DBFCreate_MW(dbfFilename);
	if (newdbfHandle == NULL)
	{
		ErrorMessage(tkCANT_CREATE_DBF);
		return false;
	}

	// is the Shapefile UTF-8 (if unspecified or specified as UTF-8)
	bool isUTF8 = (DBFGetCodePage(_dbfHandle) == nullptr || strcmp(DBFGetCodePage(_dbfHandle), "UTF-8") == 0);

	// joined fields must be removed; they will be restored in the process of reopening table
	// after saving operation
	this->RemoveJoinedFields();

	for (int i = 0; i < FieldCount(); i++)
	{
		IField * field = NULL;
		this->get_Field(i, &field);
		CComBSTR fname;
		FieldType type;
		long width, precision;
		field->get_Name(&fname);
		field->get_Type(&type);
		field->get_Width(&width);
		field->get_Precision(&precision);

		if (type == DOUBLE_FIELD)
		{
			// width and precision from OGR datasource come in as zero
			if (width == 0)
			{
				// don't know what we're getting; set to max of 17
				width = 17;
				// split down the middle (considering point and sign)
				precision = 8;
			}
			// leaving previous logic alone, setting to 1, perhaps assuming 
			// that the callers intent was to minimize the significand
			if (precision <= 0)
				precision = 1;
		}
		else
		{
			// all non-doubles have precision of zero
			precision = 0;
		}

		if (type == INTEGER_FIELD)
		{
			if (width > 9)
			{
				width = 9;  // otherwise it will be reopened as double
			}
		}

		if (type == DATE_FIELD)
		{
			if (width > 8)
			{
				width = 8;  // fixed-width YYYYMMDD
			}
		}

		if (type == BOOLEAN_FIELD)
		{
			if (width > 1)
			{
				width = 1;  // fixed-width of 1
			}
		}

		DBFAddField(newdbfHandle, OLE2CA(fname), (DBFFieldType)type, width, precision);
		field->Release();
	}

	long percent = 0, newpercent = 0;
	long currentRowIndex = -1;
	long rowCount = RowCount();

	for (long rowIndex = 0; rowIndex < rowCount; rowIndex++)
	{
		CallbackHelper::Progress(cBack, rowIndex, rowCount, "Writing .dbf", _key, percent);

		//if updating existing file, only write out modified records
		if (updateFileInPlace &&
			(_rows[rowIndex].row == NULL || _rows[rowIndex].row->status() != TableRow::DATA_MODIFIED))
		{
			currentRowIndex++;
			continue;
		}

		if (!WriteRecord(newdbfHandle, rowIndex, ++currentRowIndex, isUTF8))
		{
			ErrorMessage(tkDBF_CANT_WRITE_ROW);
			return false;
		}

		if (updateFileInPlace) {
			_rows[rowIndex].row->SetDirty(TableRow::DATA_CLEAN);
		}
		else {
			ClearRow(rowIndex);		// it will break join
		}
	}

	CallbackHelper::ProgressCompleted(cBack, _key);

	//Flush all of the records
	if (!updateFileInPlace)
		DBFClose(newdbfHandle);

	return true;
}

// **************************************************************
//	  Dump()
// **************************************************************
// Saves file to new path without 
STDMETHODIMP CTableClass::Dump(BSTR dbfFilename, ICallback *cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;
	*retval = SaveToFile(OLE2W(dbfFilename), false, cBack) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// **************************************************************
//	  SaveToFile()
// **************************************************************
STDMETHODIMP CTableClass::SaveAs(BSTR dbfFilename, ICallback *cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;
	if (!SaveToFile(OLE2W(dbfFilename), false, cBack))
	{
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	CComBSTR state;
	this->Serialize(&state);
	this->Open(dbfFilename, cBack, retval);
	this->Deserialize(state);

	*retval = VARIANT_TRUE;

	return S_OK;
}

// **************************************************************
//	  ClearFields()
// **************************************************************
void CTableClass::ClearFields()
{
	for (int i = 0; i < FieldCount(); i++)
	{
		if (_fields[i]->field != NULL)
		{
			// if the field is used somewhere else, we must not refer to this table - is it really needed ?
			((CField*)_fields[i]->field)->SetTable(NULL);
		}
		delete _fields[i];
	}
	_fields.clear();
}

// **************************************************************
//	  Close()
// **************************************************************
STDMETHODIMP CTableClass::Close(VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		*retval = VARIANT_TRUE;

	StopAllJoins();

	ClearFields();

	ClearRows();

	if (_appendMode)
	{
		WriteAppendedRow();
		_appendMode = false;
		_appendStartShapeCount = -1;
	}

	_filename = L"";

	if (_dbfHandle != NULL)
	{
		DBFClose(_dbfHandle);
		_dbfHandle = NULL;
	}

	return S_OK;
}

// **************************************************************
//	  LoadDefaultFields()
// **************************************************************
void CTableClass::LoadDefaultFields()
{
	USES_CONVERSION;

	if (_dbfHandle == NULL) return;

	for (size_t i = 0; i < _fields.size(); i++)	// clear only for disk-based table; otherwise there is no way to restore them
		delete _fields[i];
	_fields.clear();

	long num_fields = DBFGetFieldCount(_dbfHandle);
	char * fname = new char[MAX_BUFFER];
	int fwidth, fdecimals;
	DBFFieldType type;
	IField * field = NULL;

	for (long i = 0; i < num_fields; i++)
	{
		type = DBFGetFieldInfo(_dbfHandle, i, fname, &fwidth, &fdecimals);

		CoCreateInstance(CLSID_Field, NULL, CLSCTX_INPROC_SERVER, IID_IField, (void**)&field);
		field->put_GlobalCallback(_globalCallback);
		CComBSTR bstrName(fname);
		field->put_Name(bstrName);
		field->put_Width(fwidth);
		field->put_Precision(fdecimals);
		field->put_Type((FieldType)type);
		//// field has not really been modified
		//((CField*)field)->SetIsUpdated(false);

		FieldWrapper* fw = new FieldWrapper();
		fw->oldIndex = i;
		fw->field = field;
		_fields.push_back(fw);
		((CField*)field)->SetTable(this);
	}

	if (fname != NULL) delete[] fname;
	fname = NULL;
}

// **************************************************************
//	  ClearRows()
// **************************************************************
void CTableClass::ClearRows()
{
	for (int j = 0; j < RowCount(); j++)
	{
		if (_rows[j].row != NULL) {
			delete _rows[j].row;
		}
	}

	_rows.clear();
}

// **************************************************************
//	  LoadDefaultRows()
// **************************************************************
//Initialize RecordWrapper array and set the TableRow pointer to NULL
void CTableClass::LoadDefaultRows()
{
	ClearRows();

	if (_dbfHandle == NULL) return;

	long num_rows = DBFGetRecordCount(_dbfHandle);

	for (long i = 0; i < num_rows; i++)
	{
		RecordWrapper rw;
		rw.oldIndex = i;
		rw.row = NULL;
		_rows.push_back(rw);
	}
}

// **************************************************************
//	  EditClear()
// **************************************************************
STDMETHODIMP CTableClass::EditClear(VARIANT_BOOL *retval)
{
	//Reset all editing bits and reload original _fields info and reinitialize the RowWrapper array
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		LoadDefaultFields();
	LoadDefaultRows();

	m_needToSaveAsNewFile = false;
	*retval = VARIANT_TRUE;
	return S_OK;

}

// **************************************************************
//	  EditInsertField()
// **************************************************************
STDMETHODIMP CTableClass::EditInsertField(IField *Field, long *FieldIndex, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retval = VARIANT_FALSE;

	if (cBack != NULL && _globalCallback == NULL)
	{
		_globalCallback = cBack;
		cBack->AddRef();
	}

	if (_isEditingTable == FALSE)
	{
		ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		return S_OK;
	}

	// we'll help the user and correct invalid index
	if (*FieldIndex < 0)
	{
		*FieldIndex = 0;
	}
	else if (*FieldIndex > FieldCount())
	{
		*FieldIndex = _fields.size();
	}

	FieldType type;
	Field->get_Type(&type);

	for (long i = 0; i < RowCount(); i++)
	{
		if (_rows[i].row == NULL)
			ReadRecord(i);

		VARIANT * val = NULL;
		val = new VARIANT;
		VariantInit(val);

		if (type == STRING_FIELD)
		{
			val->vt = VT_BSTR;
			val->bstrVal = A2BSTR("");
		}
		else
			val->vt = VT_NULL;

		_rows[i].row->values.insert(_rows[i].row->values.begin() + *FieldIndex, val);
		_rows[i].row->SetDirty(TableRow::DATA_MODIFIED);
	}

	FieldWrapper* fw = new FieldWrapper();
	fw->oldIndex = -1;
	fw->field = Field;

	_fields.insert(_fields.begin() + *FieldIndex, fw);
	_fields[*FieldIndex]->field->AddRef();
	*retval = VARIANT_TRUE;

	// in the field class we should know about table editing state
	((CField*)_fields[*FieldIndex]->field)->SetTable(this);

	m_needToSaveAsNewFile = true;
	return S_OK;
}

// **************************************************************
//	  EditInsertField()
// **************************************************************
STDMETHODIMP CTableClass::EditReplaceField(long FieldIndex, IField *newField, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		if (FieldIndex < 0 || FieldIndex >= (int)_fields.size())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			return S_OK;
		}

	if (_fields[FieldIndex]->field == newField)
	{
		// it's the same field; no need to do anything
		return S_OK;
	}

	// deleting field
	EditDeleteField(FieldIndex, cBack, retval);

	// inserting the new one
	if (retval)
	{
		EditInsertField(newField, &FieldIndex, cBack, retval);
	}

	return S_OK;
}

// *******************************************************************
//		EditDeleteField()
// *******************************************************************
STDMETHODIMP CTableClass::EditDeleteField(long FieldIndex, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retval = VARIANT_FALSE;

	if (_globalCallback == NULL && cBack != NULL)
	{
		_globalCallback = cBack;
		_globalCallback->AddRef();
	}

	if (_isEditingTable == FALSE)
	{
		ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		return S_OK;
	}

	if (FieldIndex < 0 || FieldIndex >= (int)_fields.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	for (long i = 0; i < RowCount(); i++)
	{
		if (_rows[i].row != NULL && _rows[i].row->values[FieldIndex] != NULL)
		{
			VariantClear(_rows[i].row->values[FieldIndex]);
			_rows[i].row->values[FieldIndex] = NULL;
			_rows[i].row->values.erase(_rows[i].row->values.begin() + FieldIndex);
		}
	}

	delete _fields[FieldIndex];
	_fields.erase(_fields.begin() + FieldIndex);

	//DeleteField operation can't be saved into the original DBF file.
	m_needToSaveAsNewFile = true;

	*retval = VARIANT_TRUE;
	return S_OK;
}

// *******************************************************************
//		EditInsertRow()
// *******************************************************************
STDMETHODIMP CTableClass::EditInsertRow(long * RowIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		*retval = VARIANT_FALSE;

	bool canAppend = _appendMode && *RowIndex >= (long)_rows.size();

	if (!_isEditingTable && !canAppend)
	{
		ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		return S_OK;
	}

	// will make users life easy and correct wrong value
	if (*RowIndex < 0)
	{
		*RowIndex = 0;
	}
	else if (*RowIndex > RowCount())
	{
		*RowIndex = RowCount();
	}

	TableRow * tr = new TableRow();
	tr->SetDirty(TableRow::DATA_INSERTED);

	for (long i = 0; i < FieldCount(); i++)
	{
		VARIANT * val = NULL;
		val = new VARIANT();
		VariantInit(val);

		FieldType type = GetFieldType(i);
		if (type == STRING_FIELD)
		{
			val->vt = VT_BSTR;
			val->bstrVal = A2BSTR("");
		}
		else
			val->vt = VT_NULL;

		tr->values.push_back(val);
	}

	RecordWrapper rw;
	rw.row = tr;
	rw.oldIndex = -1;
	_rows.insert(_rows.begin() + *RowIndex, rw);

	m_needToSaveAsNewFile = true;

	*retval = VARIANT_TRUE;
	return S_OK;
}

// ********************************************************
//     CloneTableRow()
// ********************************************************
TableRow* CTableClass::CloneTableRow(int rowIndex)
{
	if (ReadRecord(rowIndex)) {
		TableRow* row = _rows[rowIndex].row;
		if (row) {
			return row->Clone();
		}
	}
	return NULL;
}

// ********************************************************
//     InsertTableRow()
// ********************************************************
bool CTableClass::InsertTableRow(TableRow* row, long rowIndex)
{
	if (!row) return false;
	if (rowIndex < 0 || rowIndex >= (long)_rows.size())
		return false;

	row->SetDirty(TableRow::DATA_INSERTED);
	RecordWrapper rw;
	rw.row = row;

	_rows.insert(_rows.begin() + rowIndex, rw);

	m_needToSaveAsNewFile = true;
	return true;
}

// ********************************************************
//     SwapTableRow()
// ********************************************************
TableRow* CTableClass::SwapTableRow(TableRow* newRow, long rowIndex)
{
	if (!newRow) return NULL;

	if (rowIndex < 0 || rowIndex >= (long)_rows.size())
		return NULL;

	if (ReadRecord(rowIndex)) {
		TableRow* oldRow = _rows[rowIndex].row;
		_rows[rowIndex].row = newRow;
		newRow->SetDirty(TableRow::DATA_MODIFIED);
		return oldRow;
	}

	return NULL;
}

// ********************************************************
//     UpdateTableRow()
// ********************************************************
bool CTableClass::UpdateTableRow(TableRow* newRow, long rowIndex)
{
	if (!newRow) return false;
	if (rowIndex < 0 || rowIndex >= (long)_rows.size())
		return false;

	if (_rows[rowIndex].row)
		delete _rows[rowIndex].row;

	_rows[rowIndex].row = newRow;
	newRow->SetDirty(TableRow::DATA_MODIFIED);
	return true;
}

// *******************************************************************
//		TryClearLastRecord()
// *******************************************************************
void CTableClass::TryClearLastRecord(long rowIndex)
{
	if (!m_globalSettings.cacheDbfRecords && _dbfHandle != NULL)
	{
		if (_lastRecordIndex != rowIndex &&
			_lastRecordIndex >= 0 && _lastRecordIndex < RowCount() &&
			_rows[_lastRecordIndex].row != NULL && !_rows[_lastRecordIndex].row->IsModified() &&
			_joins.size() == 0)
		{
			// make sure that only one row in a time can be read
			ClearRow(_lastRecordIndex);
		}
	}
}

// *******************************************************************
//		ReadRecord()
// *******************************************************************
//Read one row values and cached into the RecordWrapper array.
bool CTableClass::ReadRecord(long RowIndex)
{
	USES_CONVERSION;

	if (RowIndex < 0 && RowIndex >= RowCount()) {
		return false;
	}

	TryClearLastRecord(RowIndex);

	_lastRecordIndex = RowIndex;

	if (_rows[RowIndex].row != NULL)
		return true;

	if (_dbfHandle == NULL)
	{
		ErrorMessage(tkFILE_NOT_OPEN);
		return false;
	}

	long percent = 0, newpercent = 0;
	_rows[RowIndex].row = new TableRow();

	for (int i = 0; i < FieldCount(); i++)
	{
		FieldType type = GetFieldType(i);
		VARIANT * val = NULL;

		if (_fields[i]->oldIndex != -1)
		{
			val = new VARIANT;
			VariantInit(val);
			bool isNull = false;

			//Rob Cairns 14/2/2006
			if (DBFIsAttributeNULL(_dbfHandle, _rows[RowIndex].oldIndex, _fields[i]->oldIndex) == 1)
			{
				isNull = true;
			}

			if (type == STRING_FIELD)
			{
				if (isNull)
				{
					val->vt = VT_BSTR;
					val->bstrVal = A2BSTR("");
				}
				else
				{
					val->vt = VT_BSTR;
					const char* v = DBFReadStringAttribute(_dbfHandle, _rows[RowIndex].oldIndex, _fields[i]->oldIndex);
					// MWGIS-72: Support Russian encoding
					//WCHAR *buffer = Utility::StringToWideChar(v);
					//val->bstrVal = W2BSTR(buffer);
					//delete[] buffer;

					// jf, 12-22-2018
					// if code page was not provided (via .CPG file), OR if code page is specified UTF-8,
					// then we will assume we have to unpack it as UTF-8, else we assume the string is 
					// already encoded properly, or is in the Windows multi-byte charset, and take it as is.
					// Further reading: https://support.esri.com/en/technical-article/000013192
					// NOTE that this code differs from shapefiles being read from OGR, which are always UTF-8.
					if (DBFGetCodePage(_dbfHandle) == nullptr || strcmp(DBFGetCodePage(_dbfHandle), "UTF-8") == 0)
						// assume UTF-8
						val->bstrVal = W2BSTR(Utility::ConvertFromUtf8(v));
					else
						// else assume it's already interpreted by associated code page
						val->bstrVal = A2BSTR(v);
				}
			}
			else if (type == INTEGER_FIELD)
			{
				if (isNull)
				{
					val->vt = VT_NULL;
				}
				else
				{
					int res = DBFReadIntegerAttribute(_dbfHandle, _rows[RowIndex].oldIndex, _fields[i]->oldIndex);
					val->vt = VT_I4;
					val->lVal = res;
				}
			}
			else if (type == DOUBLE_FIELD)
			{
				if (isNull)
				{
					val->vt = VT_NULL;
				}
				else
				{
					double res = DBFReadDoubleAttribute(_dbfHandle, _rows[RowIndex].oldIndex, _fields[i]->oldIndex);
					val->vt = VT_R8;
					val->dblVal = res;
				}
			}
			else if (type == BOOLEAN_FIELD)
			{
				if (isNull)
				{
					val->vt = VT_NULL;
				}
				else
				{
					const char* v = DBFReadLogicalAttribute(_dbfHandle, _rows[RowIndex].oldIndex, _fields[i]->oldIndex);
					val->vt = VT_BOOL;
					// depending on who wrote the record, we will accept any of 'Y', 'y', 'T', or 't'
					val->boolVal = (v[0] == 'Y' || v[0] == 'y' || v[0] == 'T' || v[0] == 't') ? VARIANT_TRUE : VARIANT_FALSE;
				}
			}
			else if (type == DATE_FIELD)
			{
				if (isNull)
				{
					val->vt = VT_NULL;
				}
				else
				{
					int nFullDate = DBFReadIntegerAttribute(_dbfHandle, _rows[RowIndex].oldIndex, _fields[i]->oldIndex);
					// first we need to parse out the date from the integer
					int m, d, y;
					y = (nFullDate / 10000);
					m = ((nFullDate / 100) % 100);
					d = (nFullDate % 100);
					COleDateTime dt(y, m, d, 0, 0, 0);
					val->vt = VT_DATE;
					val->date = dt.m_dt;
				}
			}

			_rows[RowIndex].row->SetDirty(TableRow::DATA_CLEAN);
		}
		_rows[RowIndex].row->values.push_back(val);
	}

	return (_rows[RowIndex].row != NULL);
}

// *******************************************************************
//		WriteRecord()
// *******************************************************************
//Write a cached RecordWrapper into dbf file 
bool CTableClass::WriteRecord(DBFInfo* dbfHandle, long fromRowIndex, long toRowIndex, bool isUTF8)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	if (dbfHandle == NULL)
	{
		ErrorMessage(tkFILE_NOT_OPEN);
		return false;
	}

	if (fromRowIndex < 0 || fromRowIndex >= RowCount())
		return false;

	const char * nonstackString = NULL;

	for (long i = 0; i < FieldCount(); i++)
	{
		FieldType type = GetFieldType(i);
		long precision = GetFieldPrecision(i);

		VARIANT val;
		VariantInit(&val);
		get_CellValue(i, fromRowIndex, &val);
		if (type == FTString)
		{
			if (val.vt == VT_BSTR)
			{
				nonstackString = Utility::ConvertBSTRToLPSTR(val.bstrVal, (isUTF8 ? CP_UTF8 : CP_ACP)); // ((LPCSTR)Utility::ConvertToUtf8(val.bstrVal)); // Utility::SYS2A(val.bstrVal);
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, nonstackString);
				delete[] nonstackString;
				nonstackString = NULL;
			}
			else if (val.vt == VT_I4)
			{
				CString cval;
				cval.Format("%d", val.dblVal);
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, cval);
			}
			else if (val.vt == VT_R8)
			{
				CString cval;
				cval.Format("%i", val.lVal);
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, cval);
			}
			else if (val.vt == VT_BOOL)
			{
				CString cval;
				cval = (val.boolVal == VARIANT_TRUE) ? "Y" : "N";
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, cval);
			}
			else if (val.vt == VT_DATE)
			{
				CString cval;
				COleDateTime dt(val.date);
				cval.Format("%4d%2d%2d", dt.GetYear(), dt.GetMonth(), dt.GetDay());
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, cval);
			}
			else
			{
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, "");
			}
		}
		else if (type == FTInteger)
		{
			if (val.vt == VT_BSTR)
			{
				nonstackString = Utility::SYS2A(val.bstrVal);
				long lval = atoi(nonstackString);
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, lval);
				delete[] nonstackString;
				nonstackString = NULL;
			}
			else if (val.vt == VT_I4)
			{
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, val.lVal);
			}
			else if (val.vt == VT_R8)
			{
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, (int)val.dblVal);
			}
			else if (val.vt == VT_DATE)
			{
				int ival;
				COleDateTime dt(val.date);
				ival = dt.GetYear() * 10000 + dt.GetMonth() * 100 + dt.GetDay();
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, ival);
			}
			else if (val.vt == VT_NULL)
			{
				DBFWriteNULLAttribute(dbfHandle, toRowIndex, i);
			}
			else
			{
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, 0);
			}
		}
		else if (type == FTDouble)
		{
			if (val.vt == VT_BSTR)
			{
				nonstackString = Utility::SYS2A(val.bstrVal);
				double dblval = Utility::atof_custom(nonstackString);
				DBFWriteDoubleAttribute(dbfHandle, toRowIndex, i, dblval);
				delete[] nonstackString;
				nonstackString = NULL;
			}
			else if (val.vt == VT_I4)
			{
				DBFWriteDoubleAttribute(dbfHandle, toRowIndex, i, val.lVal);
			}
			else if (val.vt == VT_R8)
			{
				DBFWriteDoubleAttribute(dbfHandle, toRowIndex, i, val.dblVal);
			}
			else if (val.vt == VT_NULL)
			{
				DBFWriteNULLAttribute(dbfHandle, toRowIndex, i);
			}
			else
			{
				DBFWriteDoubleAttribute(dbfHandle, toRowIndex, i, 0.0);
			}
		}
		else if (type == FTDate)
		{
			if (val.vt == VT_BSTR)
			{
				// still a string? not likely, but the two GDAL formats
				// would either be 'YYYYMMDD' or 'MM/DD/YYYY'
				CString cval = OLE2CA(val.bstrVal);
				if (cval.GetLength() == 8 && cval.Find('/') < 0)
				{
					// already formatted properly, just write it back out
					DBFWriteStringAttribute(dbfHandle, toRowIndex, i, (LPCSTR)cval);
				}
				else if (cval.GetLength() == 10 && cval[2] == '/' && cval[5] == '/')
				{
					// reformat the way it should be...
					CString newVal = cval.Right(4) + cval.Left(2) + cval.Mid(3, 2);
					DBFWriteStringAttribute(dbfHandle, toRowIndex, i, (LPCSTR)newVal);
				}
				else
				{
					// don't know what we've got
					DBFWriteNULLAttribute(dbfHandle, toRowIndex, i);
				}
			}
			else if (val.vt == VT_I4)
			{
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, val.lVal);
			}
			else if (val.vt == VT_R8)
			{
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, (int)val.dblVal);
			}
			else if (val.vt == VT_DATE)
			{
				int ival;
				COleDateTime dt(val.date);
				ival = dt.GetYear() * 10000 + dt.GetMonth() * 100 + dt.GetDay();
				DBFWriteIntegerAttribute(dbfHandle, toRowIndex, i, ival);
			}
			else // if (val.vt == VT_NULL)
			{
				DBFWriteNULLAttribute(dbfHandle, toRowIndex, i);
			}
		}
		else if (type == FTLogical)
		{
			if (val.vt == VT_BSTR)
			{
				CString cval = OLE2CA(val.bstrVal);
				// if no string, assume False
				if (cval.GetLength() == 0) cval = "F";
				// make sure to only write a single character
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, cval.Left(1));
			}
			else if (val.vt == VT_BOOL)
			{
				CString cval;
				cval = (val.boolVal == VARIANT_TRUE) ? "T" : "F";
				//DBFWriteStringAttribute(dbfHandle, toRowIndex, i, cval);
				DBFWriteLogicalAttribute(dbfHandle, toRowIndex, i, cval[0]);
			}
			else
			{
				// default False
				DBFWriteStringAttribute(dbfHandle, toRowIndex, i, "F");
			}
		}
		VariantClear(&val);
	}
	return true;
}

// *******************************************************************
//		EditCellValue()
// *******************************************************************
STDMETHODIMP CTableClass::EditCellValue(long FieldIndex, long RowIndex, VARIANT newVal, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		*retval = VARIANT_FALSE;

	bool canAppend = _appendMode && _rows.size() > 0 && RowIndex == _rows.size() - 1;

	if (_isEditingTable == FALSE && !canAppend)
	{
		ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		return S_OK;
	}

	if (FieldIndex < 0 || FieldIndex >= FieldCount() || RowIndex < 0 || RowIndex >= RowCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	USES_CONVERSION;

	if (newVal.vt == VT_I2)
	{
		long val = newVal.iVal;
		newVal.vt = VT_I4;
		newVal.lVal = val;
	}
	else if (newVal.vt == VT_I8)
	{
		long val = newVal.lVal;
		newVal.vt = VT_I4;
		newVal.lVal = val;
	}
	else if (newVal.vt == (VT_BYREF | VT_I8))
	{
		LONGLONG val = *(newVal.pllVal);
		newVal.vt = VT_I4;
		newVal.lVal = (long)val;
	}
	else if (newVal.vt == (VT_BYREF | VT_I2))
	{
		long val = *(newVal.piVal);
		newVal.vt = VT_I4;
		newVal.lVal = val;
	}
	else if (newVal.vt == (VT_BYREF | VT_I4))
	{
		long val = *(newVal.plVal);
		newVal.vt = VT_I4;
		newVal.lVal = val;
	}
	else if (newVal.vt == VT_R4)
	{
		double val = newVal.fltVal;
		newVal.vt = VT_R8;
		newVal.dblVal = val;
	}
	else if (newVal.vt == (VT_BYREF | VT_R4))
	{
		double val = *(newVal.pfltVal);
		newVal.vt = VT_R8;
		newVal.dblVal = val;
	}
	else if (newVal.vt == (VT_BYREF | VT_R8))
	{
		double val = *(newVal.pdblVal);
		newVal.vt = VT_R8;
		newVal.dblVal = val;
	}
	else if (newVal.vt == (VT_BYREF | VT_BSTR))
	{

		BSTR val = OLE2BSTR(*(newVal.pbstrVal));
		newVal.vt = VT_BSTR;
		newVal.bstrVal = val;
	}
	// jf, 2/17/2018, no longer want to write a string, we can now process dates as dates
	//else if( newVal.vt == VT_DATE )
	//{
	//	BSTR val;
	//	LCID localeID = MAKELCID(MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US), SORT_DEFAULT);
	//	VarBstrFromDate(newVal.date,localeID,LOCALE_NOUSEROVERRIDE,&val);
	//	newVal.vt = VT_BSTR;
	//	newVal.bstrVal = val;
	//	SysFreeString(val);
	//}

	/*
	VT_UI1;
	VT_I2;
	VT_I4;
	VT_R4;
	VT_R8;
	VT_BOOL;
	VT_ERROR;
	VT_CY;
	VT_DATE;
	VT_BSTR;
	VT_UNKNOWN;
	VT_DISPATCH;
	VT_ARRAY;
	VT_BYREF|VT_UI1;
	VT_BYREF|VT_I2;
	VT_BYREF|VT_I4;
	VT_BYREF|VT_R4;
	VT_BYREF|VT_R8;
	VT_BYREF|VT_BOOL;
	VT_BYREF|VT_ERROR;
	VT_BYREF|VT_CY;
	VT_BYREF|VT_DATE;
	VT_BYREF|VT_BSTR;
	VT_BYREF|VT_UNKNOWN;
	VT_BYREF|VT_DISPATCH;
	VT_ARRAY;
	VT_BYREF|VT_VARIANT;
	*/

	// Darrel Brown, 10/16/2003 Added support for null cell values
	// jf, 2/17/2018, added support for Dates and Booleans
	if (newVal.vt != VT_I4 && newVal.vt != VT_R8 && newVal.vt != VT_BSTR && newVal.vt != VT_DATE && newVal.vt != VT_BOOL && newVal.vt != VT_NULL)
	{
		ErrorMessage(tkINCORRECT_VARIANT_TYPE);
		return S_OK;
	}

	if (_rows[RowIndex].row == NULL)
	{
		ReadRecord(RowIndex);
	}

	if (_rows[RowIndex].row != NULL)
	{
		if (_rows[RowIndex].row->values[FieldIndex] == NULL)
		{
			// tws  6/7/7 : VariantInit DOES NOT free any old value, but VariantClear and VariantCopy DO 
			// so only use VariantInit when it is NEW, otherwise it will leak BSTRs
			// this pair of braces saved 80MB of mem leak on table edit 50k shapes with 40 _fields
			_rows[RowIndex].row->values[FieldIndex] = new VARIANT;
			VariantInit(_rows[RowIndex].row->values[FieldIndex]);
		}
		VariantCopy(_rows[RowIndex].row->values[FieldIndex], &newVal);

		//Change the width of the field
		IField * field = NULL;
		this->get_Field(FieldIndex, &field);
		FieldType type;
		long precision, width;
		field->get_Type(&type);
		field->get_Width(&width);
		field->get_Precision(&precision);

		long valWidth = 0;
		if (newVal.vt == VT_BSTR)
		{
			CString cval(OLE2CA(newVal.bstrVal));
			valWidth = cval.GetLength();
		}
		else if (newVal.vt == VT_I4)
		{
			CString cval;
			cval.Format("%i", newVal.lVal);
			valWidth = cval.GetLength();
		}
		else if (newVal.vt == VT_R8)
		{
			CString cval;
			CString fmat;
			fmat.Format("%ld", precision);
			cval.Format("%." + fmat + "d", precision, newVal.dblVal);
			valWidth = cval.GetLength();
		}
		else if (newVal.vt == VT_DATE)
		{
			valWidth = 8;
		}
		else if (newVal.vt == VT_BOOL)
		{
			valWidth = 1;
		}

		if (valWidth > width)
			field->put_Width(valWidth);

		field->Release();
		_rows[RowIndex].row->SetDirty(TableRow::DATA_MODIFIED);

		*retval = VARIANT_TRUE;
	}
	return S_OK;
}

// *********************************************************
//		StartEditingTable ()
// *********************************************************
STDMETHODIMP CTableClass::StartEditingTable(ICallback *cBack, VARIANT_BOOL *retval)
{
	// StartEditingTable now just simply set the editing flag, not read all records into memory
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	if (_isEditingTable)
	{
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	if (_dbfHandle == NULL)
	{
		ErrorMessage(tkFILE_NOT_OPEN);
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	if (_appendMode)
	{
		ErrorMessage(tkDBF_NO_EDIT_MODE_WHEN_APPENDING);
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	if (_isEditingTable != TRUE)
	{
		_isEditingTable = TRUE;
		m_needToSaveAsNewFile = false;
	}

	*retval = VARIANT_TRUE;
	return S_OK;
}

// *****************************************************************
//		StopEditingTable()
// *****************************************************************
bool CTableClass::HasFieldChanges()
{
	for (int i = 0; i < (int)_fields.size(); i++)
	{
		CField* fld = (CField*)_fields[i]->field;
		if (fld->GetIsUpdated())
		{
			return true;
		}
	}
	return false;
}

// *****************************************************************
//		MarkFieldsAsUnchanged()
// *****************************************************************
void CTableClass::MarkFieldsAsUnchanged()
{
	for (int i = 0; i < (int)_fields.size(); i++)
	{
		CField* fld = (CField*)_fields[i]->field;
		fld->SetIsUpdated(false);
	}
}

// *****************************************************************
//		StopEditingTable()
// *****************************************************************
STDMETHODIMP CTableClass::StopEditingTable(VARIANT_BOOL ApplyChanges, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		*retval = VARIANT_FALSE;

	if (!_globalCallback  && cBack)
	{
		_globalCallback = cBack;
		_globalCallback->AddRef();
	}

	if (!_dbfHandle)
	{
		if (_isEditingTable)
		{
			if (ApplyChanges)
			{
				CComBSTR bstr(_filename);
				SaveAs(bstr, cBack, retval);
			}
			else
				EditClear(retval);

			// Note that we are no longer editing the table
			_isEditingTable = FALSE;
		}
		else
		{
			ErrorMessage(tkFILE_NOT_OPEN);
		}
		return S_OK;
	}

	if (!_isEditingTable)
	{
		*retval = VARIANT_TRUE;
		return S_OK;
	}

	if (ApplyChanges)
	{
		if (m_needToSaveAsNewFile || HasFieldChanges())
		{
			// file should be rewritten completely; writing to temp file first
			CString tempFilename = Utility::GetTempFilename(".dbf");

			_tempFiles.push_back(new CString(tempFilename));		// will be released in destructor

			USES_CONVERSION;
			if (SaveToFile(A2W(tempFilename), false, cBack))
			{
				BOOL result = CopyFile(tempFilename, W2A(_filename), FALSE);
				_unlink(tempFilename);
			}
			else
			{
				ErrorMessage(tkCANT_CREATE_DBF);
			}
		}
		else
		{
			// can edit the file in place, no need to save to a temporary file
			if (!SaveToFile(_filename, true, cBack))
			{
				ErrorMessage(tkCANT_CREATE_DBF);
			}
		}
	}

	MarkFieldsAsUnchanged();

	_isEditingTable = FALSE;

	CComBSTR state;
	CComBSTR bstrFilename(_filename);
	this->Serialize(&state);
	this->Open(bstrFilename, cBack, retval);
	this->Deserialize(state);	// restores joins

	return S_OK;
}

// *******************************************************************
//		EditDeleteRow()
// *******************************************************************
STDMETHODIMP CTableClass::EditDeleteRow(long RowIndex, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*retval = VARIANT_FALSE;

	((CShapefile*) _shapefile)->MarkShapeDeleted(RowIndex);

	if (_isEditingTable == FALSE)
	{
		ErrorMessage(tkDBF_NOT_IN_EDIT_MODE);
		return S_OK;
	}

	if (RowIndex < 0 || RowIndex >= RowCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	if (_rows[RowIndex].row != NULL)
		delete _rows[RowIndex].row;
	_rows.erase(_rows.begin() + RowIndex);

	*retval = VARIANT_TRUE;
	m_needToSaveAsNewFile = true;
	return S_OK;
}

// *********************************************************************
//		Save()
// *********************************************************************
//Neio July 28,2009: this method is simple,but low efficiency
STDMETHODIMP CTableClass::Save(ICallback *cBack, VARIANT_BOOL *retval)
{
	StopEditingTable(VARIANT_TRUE, cBack, retval);
	StartEditingTable(cBack, retval);
	return S_OK;
}

// *********************************************************************
//		GetFieldType()
// *********************************************************************
FieldType CTableClass::GetFieldType(long fieldIndex)
{
	IField * field = NULL;
	this->get_Field(fieldIndex, &field);
	FieldType type;
	field->get_Type(&type);
	field->Release();
	return type;
}

// *********************************************************************
//		GetFieldPrecision()
// *********************************************************************
long CTableClass::GetFieldPrecision(long fieldIndex)
{
	IField * field = NULL;
	get_Field(fieldIndex, &field);
	long precision;
	field->get_Precision(&precision);
	field->Release();
	return precision;
}

// *********************************************************************
//		ClearRow()
// *********************************************************************
void CTableClass::ClearRow(long rowIndex)
{
	if (rowIndex >= 0 && rowIndex < RowCount())
	{
		if (_rows[rowIndex].row)
		{
			delete _rows[rowIndex].row;
			_rows[rowIndex].row = NULL;
		}
	}
}

// *********************************************************************
//		get_MinValue
// *********************************************************************
STDMETHODIMP CTableClass::get_MinValue(long FieldIndex, VARIANT* retval)
{
	if (FieldIndex < 0 || FieldIndex >= (long)_fields.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		retval = NULL;
	}
	else
	{
		CComVariant min, val;
		for (unsigned long i = 0; i < _rows.size(); i++)
		{
			if (ReadRecord(i) && _rows[i].row != NULL)
				val.Copy(_rows[i].row->values[FieldIndex]);
			if (i == 0)	min = val;
			else if (val < min)	min = val;
			val.Clear();
		}
		VariantCopy(retval, &min);
	}
	return S_OK;
}

// *********************************************************************
//		get_MaxValue()
// *********************************************************************
STDMETHODIMP CTableClass::get_MaxValue(long FieldIndex, VARIANT* retval)
{
	if (FieldIndex < 0 || FieldIndex >= (long)_fields.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		retval = NULL;
	}
	else
	{
		CComVariant max, val;
		for (long i = 0; i < (long)_rows.size(); i++)
		{
			if (ReadRecord(i) && _rows[i].row != NULL)
				val.Copy(_rows[i].row->values[FieldIndex]);
			if (i == 0)	max = val;
			else if (val > max)	max = val;
			val.Clear();
		}
		VariantCopy(retval, &max);
	}
	return S_OK;
}

// *********************************************************************
//		get_MaxValue()
// *********************************************************************
STDMETHODIMP CTableClass::get_MeanValue(long FieldIndex, double* retval)
{
	if (FieldIndex < 0 || FieldIndex >= (long)_fields.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retval = 0.0;
	}
	else
	{
		FieldType type;
		_fields[FieldIndex]->field->get_Type(&type);
		if (type == DOUBLE_FIELD || type == INTEGER_FIELD)
		{
			double sum = 0;
			for (unsigned long i = 0; i < _rows.size(); i++)
			{
				if (ReadRecord(i) && _rows[i].row != NULL)
				{
					if (type == DOUBLE_FIELD)	sum += _rows[i].row->values[FieldIndex]->dblVal;
					else						sum += _rows[i].row->values[FieldIndex]->lVal;
				}
			}
			*retval = sum / (double)_rows.size();
		}
		else
		{
			//ErrorMessage(InvalidFieldType);
			*retval = 0.0;
		}
	}
	return S_OK;
}

// *********************************************************************
//		get_MaxValue()
// *********************************************************************
STDMETHODIMP CTableClass::get_StandardDeviation(long FieldIndex, double* retval)
{
	if (FieldIndex < 0 || FieldIndex >= (long)_fields.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		retval = NULL;
	}
	else
	{
		FieldType type;
		_fields[FieldIndex]->field->get_Type(&type);
		if (type == DOUBLE_FIELD || type == INTEGER_FIELD)
		{
			double mean;
			get_MeanValue(FieldIndex, &mean);
			double std = 0.0;
			for (unsigned long i = 0; i < _rows.size(); i++)
			{
				if (ReadRecord(i) && _rows[i].row != NULL)
				{
					if (type == DOUBLE_FIELD)	std += pow(_rows[i].row->values[FieldIndex]->dblVal - mean, 2);
					else						std += pow((double)_rows[i].row->values[FieldIndex]->lVal - mean, 2);
				}
			}
			*retval = sqrt(std / (_rows.size() - 1));
		}
		else
		{
			//ErrorMessage(InvalidFieldType);
			*retval = 0.0;
		}
	}
	return S_OK;
}

// *****************************************************************
//			ErrorMessage()
// *****************************************************************
inline void CTableClass::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("Table", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// *****************************************************************
//			GenerateCategories()
// *****************************************************************
// Overload for passing empty range
vector<CategoriesData>* CTableClass::GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses)
{
	CComVariant minVal, maxVal;
	minVal.vt = VT_EMPTY;
	maxVal.vt = VT_EMPTY;
	return GenerateCategories(FieldIndex, ClassificationType, numClasses, minVal, maxVal);
}

// *****************************************************************
//			GenerateCategories()
// *****************************************************************
vector<CategoriesData>* CTableClass::GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses,
	CComVariant minValue, CComVariant maxValue)
{
	if (FieldIndex < 0 || FieldIndex >= this->FieldCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	long numShapes = this->RowCount();

	// getting field type
	IField* fld = NULL;
	this->get_Field(FieldIndex, &fld);
	FieldType fieldType;
	fld->get_Type(&fieldType);
	CComBSTR str;
	fld->get_Name(&str);
	USES_CONVERSION;
	CStringW fieldName = OLE2CW(str);
	fld->Release(); fld = NULL;

	/* we won't define intervals for string values */
	if (ClassificationType != ctUniqueValues && fieldType == STRING_FIELD)
	{
		CallbackHelper::ErrorMsg("String fields support only unique values classification for categories.");
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	if ((numClasses <= 0 || numClasses > 1000) && (ClassificationType != ctUniqueValues))
	{
		CallbackHelper::ErrorMsg("Too many categories for non-unique values classification type.");
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	// natural breaks aren't designed to work otherwise
	if (numShapes < numClasses && ClassificationType == ctNaturalBreaks)
	{
		numClasses = numShapes;
	}

	// values in specified range should be classified
	bool useRange = minValue.vt != VT_EMPTY && maxValue.vt != VT_EMPTY && fieldType == DOUBLE_FIELD;

	if (useRange) //fieldType == DOUBLE_FIELD)
	{
		double max, min;
		dVal(minValue, min);
		dVal(maxValue, max);
		minValue.vt = VT_R8;
		maxValue.vt = VT_R8;
		minValue.dblVal = min;
		maxValue.dblVal = max;
	}

	//bool useRange = minValue.vt == VT_R8 && maxValue.vt == VT_R8 && fieldType != STRING_FIELD;

	std::vector<CategoriesData>* result = new std::vector<CategoriesData>;
	if (ClassificationType == ctUniqueValues)
	{
		std::set<CComVariant> dict;
		CComVariant val;

		for (long i = 0; i < numShapes; i++)
		{
			this->get_CellValue(FieldIndex, i, &val);
			if (useRange && (val.dblVal < minValue.dblVal || val.dblVal > maxValue.dblVal))
				continue;

			if (dict.find(val) == dict.end())
				dict.insert(val);
		}
		/* creating categories */
		std::vector<CComVariant> values;
		copy(dict.begin(), dict.end(), inserter(values, values.end()));

		for (int i = 0; i < (int)values.size(); i++)
		{
			CategoriesData data;
			data.minValue = values[i];
			data.maxValue = values[i];
			result->push_back(data);
		}
		dict.clear();
		values.clear();
	}
	else if (ClassificationType == ctEqualSumOfValues)
	{
		CComVariant val;

		// sorting the values
		std::vector<double> values;
		double totalSum = 0, dValue;
		for (int i = 0; i < numShapes; i++)
		{
			this->get_CellValue(FieldIndex, i, &val);

			if (useRange && (val.dblVal < minValue.dblVal || val.dblVal > maxValue.dblVal))
				continue;

			dVal(val, dValue); val.Clear();
			values.push_back(dValue);
			totalSum += dValue;
		}
		sort(values.begin(), values.end());

		double step = totalSum / (double)numClasses;
		int index = 1;
		double sum = 0;

		for (int i = 0; i < (int)values.size(); i++)
		{
			sum += values[i];
			if (sum >= step * (double)index || i == numShapes - 1)
			{
				CategoriesData data;

				if (index == numClasses)
					data.maxValue = values[values.size() - 1];
				else if (i != 0)
					data.maxValue = (values[i] + values[i - 1]) / 2;
				else
					data.maxValue = values[0];

				if (index == 1)
					data.minValue = values[0];
				else
					data.minValue = (*result)[result->size() - 1].maxValue;

				result->push_back(data);
				index++;
			}
		}
	}
	else if (ClassificationType == ctEqualIntervals)
	{
		CComVariant vMin, vMax;

		if (useRange)
		{
			vMin = minValue;
			vMax = maxValue;
		}
		else
		{
			this->get_MinValue(FieldIndex, &vMin);
			this->get_MaxValue(FieldIndex, &vMax);
		}

		double dMin, dMax;
		dVal(vMin, dMin); dVal(vMax, dMax);
		vMin.Clear(); vMax.Clear();

		/*	creating classes */
		double dStep = (dMax - dMin) / (double)numClasses;
		while (dMin < dMax)
		{
			CategoriesData data;
			data.minValue = dMin;
			data.maxValue = dMin + dStep;
			result->push_back(data);

			dMin += dStep;
		}
	}
	else if (ClassificationType == ctEqualCount)
	{
		CComVariant vMin, vMax;
		if (useRange)
		{
			vMin = minValue;
			vMax = maxValue;
		}
		else
		{
			this->get_MinValue(FieldIndex, &vMin);
			this->get_MaxValue(FieldIndex, &vMax);
		}

		double dMin, dMax;
		dVal(vMin, dMin); dVal(vMax, dMax);
		vMin.Clear(); vMax.Clear();

		// sorting the values
		std::vector<double> values;
		for (int i = 0; i < numShapes; i++)
		{
			this->get_CellValue(FieldIndex, i, &vMin);
			dVal(vMin, dMin); vMin.Clear();
			values.push_back(dMin);
		}
		sort(values.begin(), values.end());

		/*	creating classes */
		int i = 0;
		int count = numShapes / numClasses;

		for (int i = 0; i < numShapes; i += count)
		{
			dMin = values[i];
			if (i + count < numShapes)
				dMax = values[i + count];
			else
				dMax = values[numShapes - 1];

			CategoriesData data;
			data.minValue = dMin;
			data.maxValue = dMax;
			result->push_back(data);
		}
		values.clear();
	}
	else if (ClassificationType == ctNaturalBreaks)
	{
		CComVariant vMin; double dMin;
		// sorting the values
		std::vector<double> values;
		for (int i = 0; i < numShapes; i++)
		{
			this->get_CellValue(FieldIndex, i, &vMin);
			if (useRange && (vMin.dblVal < minValue.dblVal || vMin.dblVal > maxValue.dblVal))
				continue;

			dVal(vMin, dMin); vMin.Clear();
			values.push_back(dMin);
		}
		sort(values.begin(), values.end());

		CJenksBreaks breaks(&values, numClasses);
		if (breaks.Initialized())
		{
			breaks.Optimize();
			std::vector<long>* startIndices = breaks.get_Results();
			//std::vector<int>* startIndices = breaks.TestIt(&values, numClasses);

			if (startIndices)
			{
				for (unsigned int i = 0; i < startIndices->size(); i++)
				{
					CategoriesData data;
					data.minValue = values[(*startIndices)[i]];
					if (i == startIndices->size() - 1)
						data.maxValue = values[values.size() - 1];
					else
						data.maxValue = values[(*startIndices)[i + 1]];

					result->push_back(data);
				}
				delete startIndices;
			}
		}
	}
	else if (ClassificationType == ctStandardDeviation)
	{
		double mean, sdo;
		this->get_MeanValue(FieldIndex, &mean);
		this->get_StandardDeviation(FieldIndex, &sdo);

		for (int i = -3; i <= 2; i++)
		{
			CategoriesData data;
			data.minValue = mean + sdo * i;
			data.maxValue = mean + sdo * (i + 1);
			result->push_back(data);
		}
	}

	// ------------------------------------------------------
	//		generating text expressions
	// ------------------------------------------------------
	if (ClassificationType == ctUniqueValues)
	{
		for (int i = 0; i < (int)result->size(); i++)
		{
			//CString strExpression;
			CStringW strValue;
			CComVariant* val = &(*result)[i].minValue;
			// TODO: MWGIS-72: Support Russian encoding
			// https://stackoverflow.com/questions/45484130/how-to-convert-ccomvariant-bstr-to-cstring
			switch (val->vt)
			{
			case VT_BSTR:
				strValue = OLE2W(val->bstrVal);
				(*result)[i].name = strValue;
				(*result)[i].expression = "[" + fieldName + "] = \"" + strValue + "\"";
				break;
			case VT_R8:
				strValue.Format(L"%g", val->dblVal);
				(*result)[i].name = strValue;
				(*result)[i].expression = "[" + fieldName + "] = " + strValue;
				break;
			case VT_I4:
				strValue.Format(L"%i", val->lVal);
				(*result)[i].name = strValue;
				(*result)[i].expression = "[" + fieldName + "] = " + strValue;
				break;
			}
		}
	}
	else //if (ClassificationType == ctEqualIntervals || ClassificationType == ctEqualCount)
	{
		// in case % is present, we need to put to double it for proper formatting
		fieldName.Replace(L"%", L"%%");

		for (int i = 0; i < (int)result->size(); i++)
		{
			CategoriesData* data = &((*result)[i]);

			CString strExpression, strName, sFormat;

			if (i == 0)
			{
				data->minValue.dblVal = floor(data->minValue.dblVal);
			}
			else if (i == result->size() - 1)
			{
				data->maxValue.dblVal = ceil(data->maxValue.dblVal);
			}

			CStringW upperBound = (i == result->size() - 1) ? "<=" : "<";

			switch (data->minValue.vt)
			{
			case VT_R8:
				sFormat = "%g";
				data->name = Utility::FormatNumber(data->minValue.dblVal, sFormat) + " - " + Utility::FormatNumber(data->maxValue.dblVal, sFormat);
				data->expression.Format("[" + fieldName + "] >= %f AND [" + fieldName + "] " + upperBound + " %f", data->minValue.dblVal, data->maxValue.dblVal);
				break;
			case VT_I4:
				sFormat = "%i";
				data->name = Utility::FormatNumber(data->minValue.dblVal, sFormat) + " - " + Utility::FormatNumber(data->maxValue.dblVal, sFormat);
				data->expression.Format("[" + fieldName + "] >= %i AND [" + fieldName + "] " + upperBound + " %i", data->minValue.lVal, data->maxValue.lVal);
				break;
			}
		}
	}

	if (result->size() > 0)
	{
		return result;
	}
	else
	{
		delete result;
		return NULL;
	}
}

// *****************************************************************
//		FieldNames()
// *****************************************************************
std::vector<CStringW>* CTableClass::get_FieldNames()
{
	std::vector<CStringW>* names = new std::vector<CStringW>;
	for (unsigned int i = 0; i < _fields.size(); i++)
	{
		CComBSTR bstr;
		_fields[i]->field->get_Name(&bstr);
		USES_CONVERSION;
		CStringW str = OLE2CW(bstr);
		names->push_back(str);
	}
	return names;
}

// *****************************************************************
//		FieldIndexByName()
// *****************************************************************
STDMETHODIMP CTableClass::get_FieldIndexByName(BSTR FieldName, long* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		*retval = -1;
	USES_CONVERSION;
	CString searchName = OLE2CA(FieldName);
	for (unsigned int i = 0; i < _fields.size(); i++)
	{
		IField* field = _fields[i]->field;
		if (field)
		{
			CComBSTR name;
			field->get_Name(&name);
			if (_stricmp(OLE2CA(name), searchName.GetString()) == 0)
			{
				*retval = i;
				break;
			}
		}
	}

	return S_OK;
}

// *****************************************************************
//		get_FieldValues()
// *****************************************************************
bool CTableClass::get_FieldValuesDouble(int FieldIndex, std::vector<double>& values)
{
	values.clear();

	if (FieldIndex < 0 || FieldIndex >= FieldCount())
	{
		return false;
	}

	FieldType type;
	_fields[FieldIndex]->field->get_Type(&type);

	int index = _fields[FieldIndex]->oldIndex;	// real index of the field

	values.reserve(_rows.size());
	for (unsigned int i = 0; i < _rows.size(); i++)
	{
		if (_rows[i].row)
		{
			VARIANT* var = _rows[i].row->values[FieldIndex];
			if (var && var->vt != VT_NULL)
			{
				values.push_back(var->dblVal);
			}
			else
			{
				values.push_back(0.0);
			}
		}
		else
		{
			values.push_back(DBFReadDoubleAttribute(_dbfHandle, _rows[i].oldIndex, index));
		}
	}
	return true;
}

// *****************************************************************
//		get_FieldValues()
// *****************************************************************
bool CTableClass::get_FieldValuesInteger(int FieldIndex, std::vector<int>& values)
{
	values.clear();

	if (FieldIndex < 0 || FieldIndex >= FieldCount())
	{
		return false;
	}

	FieldType type;
	_fields[FieldIndex]->field->get_Type(&type);

	int index = _fields[FieldIndex]->oldIndex;	// real index of the field

	values.reserve(_rows.size());
	for (unsigned int i = 0; i < _rows.size(); i++)
	{
		if (_rows[i].row)
		{
			// TODO: use getValue
			VARIANT* var = _rows[i].row->values[FieldIndex];
			if (var && var->vt != VT_NULL)
			{
				values.push_back(var->lVal);
			}
			else
			{
				values.push_back(0);
			}
		}
		else
		{
			values.push_back(DBFReadIntegerAttribute(_dbfHandle, _rows[i].oldIndex, index));
		}
	}
	return true;
}

// *****************************************************************
//		get_FieldValues()
// *****************************************************************
bool CTableClass::get_FieldValuesString(int FieldIndex, std::vector<CString>& values)
{
	USES_CONVERSION;

	values.clear();

	if (FieldIndex < 0 || FieldIndex >= FieldCount())
	{
		return false;
	}

	FieldType type;
	_fields[FieldIndex]->field->get_Type(&type);

	int index = _fields[FieldIndex]->oldIndex;	// real index of the field

	values.reserve(_rows.size());
	for (unsigned int i = 0; i < _rows.size(); i++)
	{
		if (_rows[i].row)
		{
			VARIANT* var = _rows[i].row->values[FieldIndex];
			if (var && var->vt != VT_NULL)
			{

				values.push_back(OLE2CA(var->bstrVal));
			}
			else
			{
				values.push_back("");
			}
		}
		else
		{
			values.push_back(DBFReadStringAttribute(_dbfHandle, _rows[i].oldIndex, index));
		}
	}
	return true;
}

// *****************************************************************
//		set_IndexValue()
// *****************************************************************
bool CTableClass::set_IndexValue(int rowIndex)
{
	if (!_isEditingTable)
		return false;

	if (rowIndex < 0 || rowIndex >(int)_rows.size())
		return false;

	long fieldIndex = -1;

	CComBSTR fieldName("MWShapeID");

	this->get_FieldIndexByName(fieldName, &fieldIndex);
	if (fieldIndex == -1)
		return false;

	IField* field = NULL;
	this->get_Field(fieldIndex, &field);
	FieldType type;
	field->get_Type(&type);
	field->Release();

	if (type != INTEGER_FIELD)
		return false;

	// there is no cached value;  a full search is needed
	CComVariant val;
	if (m_maxRowId == -1)
	{
		for (unsigned int i = 0; i < _rows.size(); i++)
		{
			if (i != rowIndex)
			{
				this->get_CellValue(fieldIndex, i, &val);
				long value;
				lVal(val, value);
				if (value > m_maxRowId)
					m_maxRowId = value;
			}
		}
	}

	// setting the value
	val.vt = VT_I4;
	val.lVal = m_maxRowId + 1;
	VARIANT_BOOL vbretval;
	this->EditCellValue(fieldIndex, rowIndex, val, &vbretval);
	if (vbretval)
	{
		m_maxRowId++;

	}
	return vbretval == VARIANT_FALSE ? false : true;
}

// *****************************************************************
//		EditAddField()
// *****************************************************************
STDMETHODIMP CTableClass::EditAddField(BSTR name, FieldType type, int precision, int width, long* fieldIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		// MWGIS-55: Check inputs:
		if (width < 1)
		{
			*fieldIndex = -1;
			ErrorMessage(tkDBF_WIDTH_TOO_SMALL);
			return S_OK;
		}

	// width and precision of doubles
	if (type == DOUBLE_FIELD)
	{
		if (precision < 1)
		{
			*fieldIndex = -1;
			ErrorMessage(tkDBF_PRECISION_TOO_SMALL);
			return S_OK;
		}
		else if (precision > (width - 2))
		{
			// have to allow for point and sign
			*fieldIndex = -1;
			ErrorMessage(tkDBF_WIDTH_TOO_SMALL);
			return S_OK;
		}
	}

	// the following are fixed-width fields
	else if (type == DATE_FIELD)
	{
		width = 8;
		precision = 0;
	}
	else if (type == BOOLEAN_FIELD)
	{
		width = 1;
		precision = 0;
	}
	else if (type == INTEGER_FIELD && width > 9)
	{
		width = 9;
		precision = 0;
	}

	IField* field = NULL;
	CoCreateInstance(CLSID_Field, NULL, CLSCTX_INPROC_SERVER, IID_IField, (void**)&field);
	field->put_Name(name);
	field->put_Width(width);
	field->put_Precision(precision);
	field->put_Type(type);
	*fieldIndex = (long)_fields.size();
	VARIANT_BOOL vbretval;
	this->EditInsertField(field, fieldIndex, NULL, &vbretval);
	field->Release();		// the reference was added in previous call
	if (vbretval == VARIANT_FALSE)
		*fieldIndex = -1;
	return S_OK;
}

//*********************************************************************
//*						UniqueFieldNames()				              
//*********************************************************************
// Makes name of fields in dbf table unique. In case of duplicated names adds _# to them	 
bool CTableClass::MakeUniqueFieldNames()
{
	VARIANT_BOOL editing;
	USES_CONVERSION;

	// Do we need edit mode for editing of the field names?
	// Yes we do, shapelib doesn't allow it otherwise ;)
	this->get_EditingTable(&editing);
	if (!editing)
		return false;

	long numFields;
	this->get_NumFields(&numFields);

	set<CString> fields;

	for (long i = 0; i < numFields; i++)
	{
		CComBSTR name;
		IField* fld;
		this->get_Field(i, &fld);
		fld->get_Name(&name);

		CString temp = OLE2CA(name);

		if (fields.find(temp) == fields.end())
		{
			fields.insert(temp);
		}
		else
		{
			for (int j = 1; ; j++)
			{
				CString newName = temp;

				int maxLength = j > 10 ? 7 : 8;		//ensure that length of name is not longer than 10 characters
				if (newName.GetLength() > maxLength) {
					newName = newName.Left(maxLength);
				}

				newName.AppendFormat("_%d", j);
				if (fields.find(newName) == fields.end())
				{
					fields.insert(newName);
					CComBSTR bstrNewName(newName);
					fld->put_Name(bstrNewName);
					break;
				}
			}
		}
		fld->Release();
	}
	fields.clear();
	return true;
}

// *************************************************************
//		JoinFields()
// *************************************************************
bool CTableClass::JoinFields(ITable* table2, std::vector<FieldMapping*>& mapping, set<CStringW>& fieldList)
{
	USES_CONVERSION;

	long numFields;
	table2->get_NumFields(&numFields);
	for (long i = 0; i < numFields; i++)
	{
		IField* fld = NULL;
		table2->get_Field(i, &fld);
		CComBSTR name;
		fld->get_Name(&name);

		// either take all or take those that are in the list provided by user; comparison is case sensitive
		if (fieldList.size() == 0 || fieldList.find(OLE2W(name)) != fieldList.end())
		{
			IField* fldNew;
			fld->Clone(&fldNew);

			long index;
			VARIANT_BOOL vbretval;
			this->get_NumFields(&index);
			this->EditInsertField(fldNew, &index, NULL, &vbretval);
			_fields[index]->SetJoinId(_lastJoinId);
			fldNew->Release();

			FieldMapping* fm = new FieldMapping();
			fm->srcIndex = i;
			fm->destIndex = index;
			mapping.push_back(fm);
		}

		fld->Release();
	}

	this->MakeUniqueFieldNames();
	return true;
}


// *****************************************************
//		Join()
// *****************************************************
STDMETHODIMP CTableClass::Join(ITable* table2, BSTR fieldTo, BSTR fieldFrom, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	set<CStringW> fields;
	bool res = this->JoinInternal(table2, OLE2W(fieldTo), OLE2W(fieldFrom), "", "", fields);
	*retVal = res ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *****************************************************
//		Join2()
// *****************************************************
STDMETHODIMP CTableClass::Join2(ITable* table2, BSTR fieldTo, BSTR fieldFrom, BSTR filenameToReopen, BSTR joinOptions, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	set<CStringW> fields;
	bool res = this->JoinInternal(table2, OLE2W(fieldTo), OLE2W(fieldFrom), OLE2W(filenameToReopen), OLE2A(joinOptions), fields);
	*retVal = res ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *****************************************************
//		Join3()
// *****************************************************
STDMETHODIMP CTableClass::Join3(ITable* table2, BSTR fieldTo, BSTR fieldFrom, BSTR filenameToReopen, BSTR joinOptions, SAFEARRAY* filedList, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	// TODO: Looks very similar to CGdalUtils::ConvertSafeArray

	// Check dimensions of the array.
	set<CStringW> fields;
	if (SafeArrayGetDim(filedList) == 1)
	{
		LONG lLBound, lUBound;
		BSTR HUGEP *pbstr;
		const HRESULT hr1 = SafeArrayGetLBound(filedList, 1, &lLBound);
		const HRESULT hr2 = SafeArrayGetUBound(filedList, 1, &lUBound);
		const HRESULT hr3 = SafeArrayAccessData(filedList, reinterpret_cast<void HUGEP* FAR*>(&pbstr));
		if (!FAILED(hr1) && !FAILED(hr2) && !FAILED(hr3))
		{
			const long count = lUBound - lLBound + 1;
			for (int i = 0; i < count; i++) {
				fields.insert(OLE2W(pbstr[i]));
			}

			// if safeArray was successfully locked, unlock it
			if (!FAILED(hr3))
			{
				SafeArrayUnaccessData(filedList);
			}
		}
	}

	const bool res = this->JoinInternal(table2, OLE2W(fieldTo), OLE2W(fieldFrom), OLE2W(filenameToReopen),
	                                    OLE2A(joinOptions), fields);
	*retVal = res ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *****************************************************
//		TryJoin()
// *****************************************************
STDMETHODIMP CTableClass::TryJoin(ITable* table2, BSTR fieldTo, BSTR fieldFrom, int* rowCount, int* joinRowCount, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	*rowCount = -1;
	*joinRowCount = -1;

	long index1, index2;

	USES_CONVERSION;
	if (this->CheckJoinInput(table2, OLE2W(fieldTo), OLE2W(fieldFrom), index1, index2))
	{
		// building a maps for field of target table
		std::map<CComVariant, int> vals;
		long numRows;
		table2->get_NumRows(&numRows);
		for (long i = 0; i < numRows; i++)
		{
			CComVariant v;
			table2->get_CellValue(index2, i, &v);
			vals[v] = 0;
		}

		int count = 0;
		int countJoin = 0;	// number of unique rows from the external table
		for (size_t i = 0; i < _rows.size(); i++)
		{
			CComVariant v;
			this->get_CellValue(index1, i, &v);

			std::map<CComVariant, int>::iterator it = vals.find(v);
			if (it != vals.end())
			{
				if (it->second == 0)
				{
					countJoin++;
					it->second = 1;
				}
				count++;
			}
		}
		*rowCount = count;
		*joinRowCount = countJoin;
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *****************************************************
//		CheckJoinInput()
// *****************************************************
bool CTableClass::CheckJoinInput(ITable* table2, CStringW fieldTo, CStringW fieldFrom, long& index1, long& index2)
{
	if (!table2)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return false;
	}

	CComBSTR bstrTo(fieldTo);
	CComBSTR bstrFrom(fieldFrom);
	this->get_FieldIndexByName(bstrTo, &index1);
	table2->get_FieldIndexByName(bstrFrom, &index2);

	if (index1 == -1 || index2 == -1)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return false;
	}

	CComPtr<IField> fld1 = NULL;
	CComPtr<IField> fld2 = NULL;
	this->get_Field(index1, &fld1);
	table2->get_Field(index2, &fld2);

	FieldType type1;
	FieldType type2;
	fld1->get_Type(&type1);
	fld2->get_Type(&type2);

	if (type1 != type2)
	{
		ErrorMessage(tkSAME_FIELD_TYPE_EXPECTED);
		return false;
	}
	return true;
}

// *****************************************************
//		JoinInternal()
// *****************************************************
bool CTableClass::JoinInternal(ITable* table2, CStringW fieldTo, CStringW fieldFrom, CStringW filenameToReopen, CString options, set<CStringW>& fieldList)
{
	long index1, index2;
	if (!this->CheckJoinInput(table2, fieldTo, fieldFrom, index1, index2))
		return false;

	// if edit mode isn't open, will use this hack to get through; rather that calling Table.StartEditMode
	VARIANT_BOOL editing;
	this->get_EditingTable(&editing);
	if (!editing) {
		this->_isEditingTable = TRUE;
	}

	// reading list of fields specified by user
	CString csvFields;
	set<CStringW>::iterator it = fieldList.begin();
	while (it != fieldList.end())
	{
		csvFields += *it + ",";
		++it;
	}
	if (csvFields.GetLength() > 0) {
		csvFields = csvFields.Left(csvFields.GetLength() - 1);
	}

	// saving the info about join operation to be able to restore it later
	JoinInfo* info = new JoinInfo();
	info->filename = filenameToReopen;
	info->fieldTo = fieldTo;
	info->fieldFrom = fieldFrom;
	info->options = options;
	info->fields = csvFields;
	info->joinId = ++_lastJoinId;
	_joins.push_back(info);

	// joining fields
	std::vector<FieldMapping*> mapping;
	this->JoinFields(table2, mapping, fieldList);

	// building a maps for field of target table
	std::map<CComVariant, int> vals;
	long numRows;
	table2->get_NumRows(&numRows);
	for (long i = 0; i < numRows; i++)
	{
		CComVariant v;
		table2->get_CellValue(index2, i, &v);
		vals[v] = i;
	}

	int count = 0;
	VARIANT_BOOL vb;
	for (size_t i = 0; i < _rows.size(); i++)
	{
		CComVariant v;
		this->get_CellValue(index1, i, &v);

		std::map<CComVariant, int>::iterator it = vals.find(v);
		if (it != vals.end())
		{
			for (size_t j = 0; j < mapping.size(); j++)
			{
				table2->get_CellValue(mapping[j]->srcIndex, it->second, &v);
				this->EditCellValue(mapping[j]->destIndex, i, v, &vb);
				if (vb) {
					count++;
				}
			}
		}
	}

	for (size_t i = 0; i < mapping.size(); i++) {
		delete mapping[i];
	}

	if (!editing) {
		this->_isEditingTable = FALSE;
	}

	return true;
}

// *****************************************************
//		RemoveJoinedFields()
// *****************************************************
// Should be used before saving takes place, while _joins remain untouched so that 
// they can be restored via serialization/deserialization
void CTableClass::RemoveJoinedFields()
{
	VARIANT_BOOL editing;
	this->get_EditingTable(&editing);
	if (!editing) {
		this->_isEditingTable = TRUE;
	}

	VARIANT_BOOL vb;
	for (int i = _fields.size() - 1; i >= 0; i--)
	{
		if (_fields[i]->Joined()) {
			this->EditDeleteField(i, NULL, &vb);
		}
	}

	if (!editing) {
		this->_isEditingTable = FALSE;
	}
}

// *****************************************************
//		StopAllJoins()
// *****************************************************
STDMETHODIMP CTableClass::StopAllJoins()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->RemoveJoinedFields();

	for (size_t i = 0; i < _joins.size(); i++) {
		delete _joins[i];
	}
	_joins.clear();
	return S_OK;
}

// *****************************************************
//		StopJoin()
// *****************************************************
STDMETHODIMP CTableClass::StopJoin(int joinIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (joinIndex < 0 || joinIndex >= (int)_joins.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		int id = _joins[joinIndex]->joinId;

		VARIANT_BOOL editing;
		this->get_EditingTable(&editing);
		if (!editing) {
			this->_isEditingTable = TRUE;
		}

		// remove all fields which belong to this join
		VARIANT_BOOL vb;
		for (int i = _fields.size() - 1; i >= 0; i--)
		{
			if (_fields[i]->GetJoinId() == id)
				this->EditDeleteField(i, NULL, &vb);
		}

		if (!editing)
			this->_isEditingTable = FALSE;

		delete _joins[joinIndex];
		_joins.erase(_joins.begin() + joinIndex);

		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *****************************************************
//		get_IsJoined()
// *****************************************************
STDMETHODIMP CTableClass::get_IsJoined(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for (size_t i = _fields.size() - 1; i >= 0; i--)
	{
		if (_fields[i]->Joined()) {
			*retVal = VARIANT_TRUE;
			return S_OK;
		}
	}
	*retVal = VARIANT_FALSE;
	return S_OK;
}

// *****************************************************
//		get_JoinCount()
// *****************************************************
STDMETHODIMP CTableClass::get_JoinCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _joins.size();
	return S_OK;
}


// *****************************************************
//		get_FieldIsJoined()
// *****************************************************
STDMETHODIMP CTableClass::get_FieldIsJoined(int fieldIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (fieldIndex < 0 || fieldIndex >= (int)_fields.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else {
		*retVal = _fields[fieldIndex]->Joined() ? VARIANT_TRUE : VARIANT_FALSE;
	}
	return S_OK;
}

// *****************************************************
//		get_FieldJoinIndex()
// *****************************************************
STDMETHODIMP CTableClass::get_FieldJoinIndex(int fieldIndex, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = -1;
	if (fieldIndex < 0 || fieldIndex >= (int)_fields.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		int id = _fields[fieldIndex]->GetJoinId();
		for (size_t i = 0; i < _joins.size(); i++)
		{
			if (_joins[i]->joinId == id)
			{
				*retVal = i;
				break;
			}
		}
	}
	return S_OK;
}

// *****************************************************
//		get_JoinFilename()
// *****************************************************
STDMETHODIMP CTableClass::get_JoinFilename(int joinIndex, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (joinIndex < 0 || joinIndex >= (int)_joins.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = W2BSTR(L"");
	}
	else
	{
		*retVal = W2BSTR(_joins[joinIndex]->filename);
	}
	return S_OK;
}

// *****************************************************
//		get_JoinFromField()
// *****************************************************
STDMETHODIMP CTableClass::get_JoinFromField(int joinIndex, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (joinIndex < 0 || joinIndex >= (int)_joins.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = W2BSTR(L"");
	}
	else
	{
		*retVal = W2BSTR(_joins[joinIndex]->fieldFrom);
	}
	return S_OK;
}


// *****************************************************
//		get_JoinToField()
// *****************************************************
STDMETHODIMP CTableClass::get_JoinToField(int joinIndex, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (joinIndex < 0 || joinIndex >= (int)_joins.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = W2BSTR(L"");
	}
	else
	{
		*retVal = W2BSTR(_joins[joinIndex]->fieldTo);
	}
	return S_OK;
}

// ********************************************************
//     get_JoinFields()
// ********************************************************
STDMETHODIMP CTableClass::get_JoinFields(LONG joinIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (joinIndex < 0 || joinIndex >= (int)_joins.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = W2BSTR(L"");
	}
	else
	{
		*pVal = W2BSTR(_joins[joinIndex]->fields);
	}

	return S_OK;
}

// *****************************************************
//		Serialize()
// *****************************************************
STDMETHODIMP CTableClass::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPLXMLNode* psTree = this->SerializeCore("TableClass");
	Utility::SerializeAndDestroyXmlTree(psTree, retVal);
	return S_OK;
}

// *****************************************************
//		Deserialize()
// *****************************************************
STDMETHODIMP CTableClass::Deserialize(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeTable = CPLGetXMLNode(node, "=TableClass");
		if (nodeTable)
		{
			this->DeserializeCore(nodeTable);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
CPLXMLNode* CTableClass::SerializeCore(CString ElementName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	CPLXMLNode* psTree = CPLCreateXMLNode(NULL, CXT_Element, ElementName);

	CPLXMLNode* psFields = CPLCreateXMLNode(psTree, CXT_Element, "Fields");
	if (psFields)
	{
		for (size_t i = 0; i < _fields.size(); i++)
		{
			IField* fld = _fields[i]->field;
			if (fld && FieldHelper::NeedsSerialization(fld))
			{
				CPLXMLNode* nodeField = CPLCreateXMLNode(psFields, CXT_Element, "Field");

				VARIANT_BOOL visible;
				CComBSTR name, alias, expression;
				fld->get_Name(&name);
				fld->get_Alias(&alias);
				fld->get_Expression(&expression);
				fld->get_Visible(&visible);

				CStringA utf8Name = Utility::ConvertToUtf8(OLE2W(name));
				CStringA utf8Alias = Utility::ConvertToUtf8(OLE2W(alias));
				CStringA utf8Expression = Utility::ConvertToUtf8(OLE2W(expression));

				Utility::CPLCreateXMLAttributeAndValue(nodeField, "Name", CPLString().Printf(utf8Name));
				Utility::CPLCreateXMLAttributeAndValue(nodeField, "Alias", CPLString().Printf(utf8Alias));
				Utility::CPLCreateXMLAttributeAndValue(nodeField, "Expression", CPLString().Printf(utf8Expression));
				Utility::CPLCreateXMLAttributeAndValue(nodeField, "Visible", CPLString().Printf("%d", (int)visible));
				Utility::CPLCreateXMLAttributeAndValue(nodeField, "Index", CPLString().Printf("%d", i));
			}
		}
	}

	if (_joins.size() > 0)
	{
		CPLXMLNode* psJoins = CPLCreateXMLNode(psTree, CXT_Element, "Joins");
		if (psJoins)
		{
			for (size_t i = 0; i < _joins.size(); i++)
			{
				CStringW name = _joins[i]->filename;
				if (this->_filename.GetLength() > 0)
				{
					name = Utility::GetRelativePath(this->_filename, name);
				}

				CPLXMLNode* psNode = CPLCreateXMLNode(psJoins, CXT_Element, "Join");
				Utility::CPLCreateXMLAttributeAndValue(psNode, "Filename", CPLString().Printf(Utility::ConvertToUtf8(name)));
				Utility::CPLCreateXMLAttributeAndValue(psNode, "FieldTo", CPLString().Printf(Utility::ConvertToUtf8(_joins[i]->fieldTo)));
				Utility::CPLCreateXMLAttributeAndValue(psNode, "FieldFrom", CPLString().Printf(Utility::ConvertToUtf8(_joins[i]->fieldFrom)));
				Utility::CPLCreateXMLAttributeAndValue(psNode, "Fields", CPLString().Printf(Utility::ConvertToUtf8(_joins[i]->fields)));
				Utility::CPLCreateXMLAttributeAndValue(psNode, "Options", CPLString().Printf(_joins[i]->options));
			}
		}
	}

	return psTree;
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
void CTableClass::DeserializeCore(CPLXMLNode* node)
{
	if (!node) return;

	ClearFieldCustomizations();

	CPLXMLNode* nodeFields = CPLGetXMLNode(node, "Fields");
	if (nodeFields)
	{
		RestoreFields(nodeFields);
	}

	this->StopAllJoins();	// joins are stored in the serialized state, so they would be replicated here

	CPLXMLNode* nodeJoins = CPLGetXMLNode(node, "Joins");
	if (nodeJoins)
	{
		RestoreJoins(nodeJoins);
	}
}

// ********************************************************
//     ClearFieldCustomizations()
// ********************************************************
void CTableClass::ClearFieldCustomizations()
{
	for (size_t i = 0; i < _fields.size(); i++)
	{
		IField* fld = _fields[i]->field;
		fld->put_Visible(VARIANT_TRUE);
		fld->put_Alias(m_globalSettings.emptyBstr);
	}
}

// ********************************************************
//     RestoreFields()
// ********************************************************
void CTableClass::RestoreFields(CPLXMLNode* node)
{
	node = node->psChild;
	while (node)
	{
		if (strcmp(node->pszValue, "Field") == 0)
		{
			long index = -1;

			CString s = CPLGetXMLValue(node, "Index", "");
			if (s != "") index = atoi(s);

			if (index >= 0 && index < (long)_fields.size())
			{
				s = CPLGetXMLValue(node, "Name", "");
				if (s != "")
				{
					CStringW name = Utility::ConvertFromUtf8(s);
					CComBSTR bstrName(name);

					long index2;
					this->get_FieldIndexByName(bstrName, &index2);

					if (index == index2)
					{
						CComPtr<IField> fld = NULL;
						get_Field(index, &fld);

						if (fld)
						{
							s = CPLGetXMLValue(node, "Alias", "");
							if (s != "")
							{
								CStringW alias = Utility::ConvertFromUtf8(s);
								CComBSTR bstrAlias(alias);
								fld->put_Alias(bstrAlias);
							}

							s = CPLGetXMLValue(node, "Expression", "");
							if (s != "")
							{
								CStringW expr = Utility::ConvertFromUtf8(s);
								CComBSTR bstrExpression(expr);
								fld->put_Expression(bstrExpression);
							}

							CString s = CPLGetXMLValue(node, "Visible", "-1");
							if (s != "")
							{
								VARIANT_BOOL visible = (VARIANT_BOOL)atoi(s);
								fld->put_Visible(visible);
							}
						}
					}
				}
			}
		}

		node = node->psNext;
	}
}

// ********************************************************
//     RestoreJoins()
// ********************************************************
void CTableClass::RestoreJoins(CPLXMLNode* node)
{
	CStringW folderName = L"";
	wchar_t* cwd = NULL;
	if (this->_filename != L"")
	{
		cwd = new wchar_t[4096];
		_wgetcwd(cwd, 4096);

		folderName = Utility::GetFolderFromPath(this->_filename);
		_wchdir(folderName);
	}

	node = node->psChild;
	while (node)
	{
		if (strcmp(node->pszValue, "Join") == 0)
		{
			CStringW filename = Utility::ConvertFromUtf8(CPLGetXMLValue(node, "Filename", NULL)).MakeLower();
			CString fieldTo = CPLGetXMLValue(node, "FieldTo", NULL);
			CString fieldFrom = CPLGetXMLValue(node, "FieldFrom", NULL);
			CString fields = CPLGetXMLValue(node, "Fields", NULL);
			CString options = CPLGetXMLValue(node, "Options", NULL);

			if (filename.GetLength() > 0 && fieldTo.GetLength() > 0 && fieldFrom.GetLength() > 0)
			{
				// ask client to provide the data once more
				VARIANT_BOOL vb;
				CComPtr<ITable> tableToFill = NULL;
				ComHelper::CreateInstance(idTable, (IDispatch**)&tableToFill);

				CComBSTR bstrFilename(filename);
				if (filename.GetLength() > 4 && filename.Right(4) == ".dbf")
				{
					tableToFill->Open(bstrFilename, NULL, &vb);
				}
				else
				{
					// let the client handle all the rest formats
					tableToFill->CreateNew(m_globalSettings.emptyBstr, &vb);

					CComBSTR bstrFields(fields);
					CComBSTR bstrOptions(options);
					Fire_OnUpdateJoin(bstrFilename, bstrFields, bstrOptions, tableToFill);
				}

				long numRows, numCols;
				tableToFill->get_NumRows(&numRows);
				tableToFill->get_NumFields(&numCols);

				if (numRows > 0 && numCols > 0)
				{
					set<CStringW> fieldList;
					int pos = 0;
					CStringW field = fields.Tokenize(",", pos);
					while (field.GetLength() != 0)
					{
						fieldList.insert(field);
						field = fields.Tokenize(",", pos);
					}

					USES_CONVERSION;
					this->JoinInternal(tableToFill, A2W(fieldTo), A2W(fieldFrom), filename, options, fieldList);
				}
				tableToFill->Close(&vb);
			}
		}
		node = node->psNext;
	}

	if (this->_filename != "")
	{
		_wchdir(cwd);
	}
}

// ********************************************************
//     GetUids()
// ********************************************************
bool CTableClass::GetUids(long fieldIndex, map<long, long>& results)
{
	CComPtr<IField> fld = NULL;
	get_Field(fieldIndex, &fld);
	if (!fld) return false;

	if (fld) {
		FieldType type;
		fld->get_Type(&type);
		if (type != INTEGER_FIELD) return false;
	}

	results.clear();

	long numRows;
	get_NumRows(&numRows);
	CComVariant var;
	for (long i = 0; i < numRows; i++)
	{
		get_CellValue(fieldIndex, i, &var);
		long val;
		lVal(var, val);
		if (results.find(val) == results.end()) {
			results[val] = i;
		}
		else {
			results.clear();
			return false;
		}
	}
	return true;
}

// ********************************************************
//     get_Filename()
// ********************************************************
STDMETHODIMP CTableClass::get_Filename(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	USES_CONVERSION;
	*pVal = OLE2BSTR(_filename);

	return S_OK;
}

// ********************************************************
//     get_JoinOptions()
// ********************************************************
STDMETHODIMP CTableClass::get_JoinOptions(LONG joinIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (joinIndex < 0 || joinIndex >= (int)_joins.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = A2BSTR("");
	}
	else
	{
		*pVal = A2BSTR(_joins[joinIndex]->options);
	}

	return S_OK;
}

// ********************************************************
//     RemoveTempFiles()
// ********************************************************
void CTableClass::RemoveTempFiles()
{
	for (int i = 0; i < (int)_tempFiles.size(); i++)
	{
		try
		{
			CString * a = _tempFiles[i];
			_unlink(a->GetBuffer());
			delete a;
		}
		catch (...)
		{
			ASSERT(FALSE);
		}
	}
	_tempFiles.clear();
}

// ********************************************************
//     GetSorting()
// ********************************************************
bool CTableClass::GetSorting(long fieldIndex, vector<long>& indices)
{
	if (fieldIndex < 0 || fieldIndex >= (long)_fields.size())
	{
		CallbackHelper::ErrorMsg("Invalid field index for sorting");
		return false;
	}

	multimap <CComVariant, long> map;
	CComVariant val;

	long percent = 0;
	for (size_t i = 0; i < _rows.size(); i++)
	{
		this->get_CellValue(fieldIndex, i, &val);
		pair<CComVariant, long> myPair(val, (long)i);
		map.insert(myPair);
	}

	multimap <CComVariant, long>::iterator p;
	p = map.begin();

	indices.clear();
	indices.reserve(map.size());

	while (p != map.end())
	{
		indices.push_back(p->second);
		++p;
	}

	return true;
}

// ********************************************************
//     GetRelativeValues()
// ********************************************************
bool CTableClass::GetRelativeValues(long fieldIndex, bool logScale, vector<double>& values)
{
	values.clear();

	if (fieldIndex < 0 || fieldIndex >= (long)_fields.size())
	{
		CallbackHelper::ErrorMsg("Invalid field index for sorting.");
		return false;
	}

	IField* fld = _fields[fieldIndex]->field;
	FieldType fldType;
	fld->get_Type(&fldType);

	if (fldType != DOUBLE_FIELD && fldType != INTEGER_FIELD) {
		CallbackHelper::ErrorMsg("Invalid field type for sorting.");
		return false;
	}

	CComVariant min, max;
	get_MinValue(fieldIndex, &min);
	get_MaxValue(fieldIndex, &max);

	double dmin, dmax;
	dVal(min, dmin);
	dVal(max, dmax);

	CComVariant value;
	double dval;

	values.reserve(_rows.size());

	if (dmin == dmax)
	{
		for (size_t i = 0; i < _rows.size(); i++) {
			values.push_back(0.0);
		}
		return true;
	}

	if (logScale) {
		// if minimum is below 1.0, move it there;
		// get rid of unused part of the scale for other values
		dmax = dmin < 1.0 ? dmax + 1.0 - dmin : dmax / dmin;
	}

	double range = logScale ? log10(dmax) : dmax - dmin;

	for (size_t i = 0; i < _rows.size(); i++)
	{
		get_CellValue(fieldIndex, (long)i, &value);
		dVal(value, dval);

		if (logScale)
		{
			dval = dmin < 1.0 ? dval + 1.0 - dmin : dval / dmin;
			values.push_back(log10(dval) / range);
		}
		else {
			values.push_back((dval - dmin) / (dmax - dmin));
		}
	}

	return true;
}

// ********************************************************
//     ClearCache()
// ********************************************************
STDMETHODIMP CTableClass::ClearCache()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for (int j = 0; j < RowCount(); j++)
	{
		if (_rows[j].row != NULL && !_rows[j].row->IsModified())
		{
			delete _rows[j].row;
			_rows[j].row = NULL;
		}
	}

	_lastRecordIndex = -1;

	return S_OK;
}

// ********************************************************
//     ValidateRowIndex()
// ********************************************************
bool CTableClass::ValidateRowIndex(long rowIndex)
{
	if (rowIndex < 0 || rowIndex >= RowCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return false;
	}

	return true;
}

// ********************************************************
//     ValidateFieldIndex()
// ********************************************************
bool CTableClass::ValidateFieldIndex(long fieldIndex)
{
	if (fieldIndex < 0 || fieldIndex >= FieldCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return false;
	}

	return true;
}

// ********************************************************
//     RowIsModified()
// ********************************************************
STDMETHODIMP CTableClass::get_RowIsModified(LONG RowIndex, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!ValidateRowIndex(RowIndex)) {
		return S_OK;
	}

	bool modified = _rows[RowIndex].row != NULL && _rows[RowIndex].row->IsModified();
	*pVal = modified ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}


// ********************************************************
//     MarkRowIsClean()
// ********************************************************
void CTableClass::MarkRowIsClean(long rowIndex)
{
	if (!ValidateRowIndex(rowIndex)) {
		return;
	}

	if (_rows[rowIndex].row) {
		_rows[rowIndex].row->SetDirty(TableRow::DATA_CLEAN);
	}
}

// ********************************************************
//     MarkFieldsClean()
// ********************************************************
void CTableClass::MarkFieldsAreClean()
{
	for (size_t i = 0; i < _fields.size(); i++)
	{
		if (_fields[i]) {
			_fields[i]->oldIndex = i;
		}
	}
}

// ********************************************************
//     GetFieldOriginalIndex()
// ********************************************************
int CTableClass::GetFieldSourceIndex(int fieldIndex)
{
	if (!ValidateFieldIndex(fieldIndex)) {
		return -1;
	}

	return _fields[fieldIndex]->oldIndex;
}

// ********************************************************
//     GetFieldOriginalIndex()
// ********************************************************
void CTableClass::SetFieldSourceIndex(int fieldIndex, int sourceIndex)
{
	if (!ValidateFieldIndex(fieldIndex)) {
		return;
	}

	_fields[fieldIndex]->oldIndex = sourceIndex;
}

// ReSharper restore CppUseAuto