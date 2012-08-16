/**************************************************************************************
 * File name: Table.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CTableClass
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
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */

#pragma once
#include "MapWinGis.h"
#include <comsvcs.h>

#include <vector>
#include <deque>
#include <map>
#include <direct.h>

#include "Field.h"
#include "TableRow.h"
#include "dbf.h"
#include "Expression.h"

using namespace std;

# define MAX_BUFFER 512

struct FieldWrapper
{
    IField* field;
    long oldIndex;
	bool joined;

	FieldWrapper()		// do we need to add destructor here?
	{
		field = NULL;
		joined = false;
	}

	~FieldWrapper()
	{
		//field->Release();
		field = NULL;
	}
};

struct RecordWrapper
{
    TableRow* row;
    long oldIndex;

	RecordWrapper()
	{
		row = NULL;
	}
};

struct CategoriesData
{
public:
	CComVariant minValue;
	CComVariant maxValue;
	CString expression;
	CString name;
};

// -------------------------------------------------------
// CTableClass
// -------------------------------------------------------
class ATL_NO_VTABLE CTableClass : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTableClass, &CLSID_Table>,
	public IDispatchImpl<ITable, &IID_ITable, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CTableClass()
	{
		USES_CONVERSION;
		key = A2BSTR("");
		globalCallback = NULL;
		lastErrorCode = tkNO_ERROR;
		isEditingTable = FALSE;
		dbfHandle = NULL;
		m_maxRowId = -1;
	}
	~CTableClass()
	{
		VARIANT_BOOL vbretval;
		this->Close(&vbretval);
		for (int i = 0; i < (int)tempFiles.size(); i++ )
		{
			try
			{
				CString * a = tempFiles[i];
				_unlink(a->GetBuffer());
				delete a;
			}
			catch(...)
			{}
		}
		tempFiles.clear();

		::SysFreeString(key);

		if( globalCallback )
			globalCallback->Release();
		globalCallback = NULL;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TABLE)

	DECLARE_NOT_AGGREGATABLE(CTableClass)

	BEGIN_COM_MAP(CTableClass)
		COM_INTERFACE_ENTRY(ITable)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()


// ITable
public:
	STDMETHOD(EditDeleteRow)(/*[in]*/ long RowIndex, /*[out,retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(StopEditingTable)(/*[in, optional, defaultvalue(TRUE)]*/ VARIANT_BOOL ApplyChanges, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(StartEditingTable)(/*[in, optional]*/  ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditCellValue)(/*[in]*/ long FieldIndex, /*[in]*/ long RowIndex, /*[in]*/ VARIANT newVal, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditInsertRow)(/*[in,out]*/ long * RowIndex, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditDeleteField)(/*[in]*/ long FieldIndex, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditReplaceField)(/*[in]*/ long FieldIndex, /*[in]*/ IField * newField, /*[in, optional]*/ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditInsertField)(/*[in]*/ IField * Field, /*[in, out]*/ long * FieldIndex, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(EditClear)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Close)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(SaveAs)(/*[in]*/ BSTR dbfFilename, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(CreateNew)(/*[in]*/ BSTR dbfFilename, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(Open)(/*[in]*/ BSTR dbfFilename, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_CdlgFilter)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_EditingTable)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(get_CellValue)(/*[in]*/ long FieldIndex, /*[in]*/ long RowIndex, /*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(get_Field)(/*[in]*/ long FieldIndex, /*[out, retval]*/ IField * *pVal);
	STDMETHOD(get_NumFields)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_NumRows)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_MinValue)(long FieldIndex, VARIANT* retval);
	STDMETHOD(get_MaxValue)(long FieldIndex, VARIANT* retval);
	STDMETHOD(get_MeanValue)(long FieldIndex, double* retVal);
	STDMETHOD(get_StandardDeviation)(long FieldIndex, double* retVal);
	STDMETHOD(ParseExpression)(BSTR Expression, BSTR* ErrorString, VARIANT_BOOL* retVal);
	STDMETHOD(Query)(BSTR Expression, VARIANT* Result, BSTR* ErrorString, VARIANT_BOOL* retval);

	STDMETHOD(Save)(/*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(get_FieldIndexByName)(BSTR FieldName, long* retval);
	STDMETHOD(TestExpression)(BSTR Expression, tkValueType ReturnType, BSTR* ErrorString, VARIANT_BOOL* retVal);
	STDMETHOD(Calculate)(BSTR Expression, LONG RowIndex, VARIANT* Result, BSTR* ErrorString, VARIANT_BOOL* retVal);
	STDMETHOD(EditAddField)(BSTR name, FieldType type, int precision, int width, long* fieldIndex);

	STDMETHOD(Join)(ITable* table2, BSTR field1, BSTR field2, VARIANT_BOOL* retVal);
	STDMETHOD(StopJoin)();
	STDMETHOD(get_IsJoined)(VARIANT_BOOL* retVal);
	STDMETHOD(get_FieldIsJoined)(int fieldIndex, VARIANT_BOOL* retVal);

	void CTableClass::ParseExpressionCore(BSTR Expression, tkValueType returnType, BSTR* ErrorString, VARIANT_BOOL* retVal);
	
	std::vector<CategoriesData>* CTableClass::GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses);
	std::vector<CategoriesData>* CTableClass::GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses, 
																  CComVariant minValue, CComVariant maxValue);
	void CTableClass::AnalyzeExpressions(std::vector<CString>& expressions, std::vector<int>& results );
	bool CTableClass::Query_(CString Expression, std::vector<long>& indices, CString& ErrorString);
	bool CTableClass::Calculate_(CString Expression, std::vector<CString>& results, CString& ErrorString, int rowIndex = -1);
	
	bool get_FieldValuesDouble(int FieldIndex, std::vector<double>& values);
	bool get_FieldValuesInteger(int FieldIndex, std::vector<int>& values);
	bool get_FieldValuesString(int FieldIndex, std::vector<CString>& values);

	bool set_IndexValue(int rowIndex);

	struct FieldMapping
	{
		int srcIndex;
		int destIndex;
	};

	bool CTableClass::CopyFields(ITable* table2, std::vector<FieldMapping*>& mapping);

public:	
	bool needToSaveAsNewFile;
	int m_maxRowId;	// maximum value in the MWShapeId field
	
private:
    DBFInfo * dbfHandle;	// underlying data structure
	std::vector<FieldWrapper> _fields;
	std::vector<RecordWrapper> _rows;
	std::deque<CString *> tempFiles;
	BSTR key;
	long lastErrorCode;
	ICallback * globalCallback;
	BOOL isEditingTable;
	CString filename;
	
	bool SaveToFile(const CString& dbfFilename, bool updateFileInPlace, ICallback* cBack);
	void LoadDefault_fields();
    void LoadDefault_rows();
    long RowCount();
    long FieldCount();
	bool ReadRecord(long RowIndex);
    bool WriteRecord(DBFInfo* dbfHandle, long fromRowIndex, long toRowIndex);
    void ClearRow(long rowIndex);
	FieldType GetFieldType(long fieldIndex);
    long GetFieldPrecision(long fieldIndex);
	inline void ErrorMessage(long ErrorCode);
	std::vector<CString>* get_FieldNames();
	
	
public:
	
	//STDMETHOD(CalculateStat)(LONG FieldIndex, tkGroupOperation Statistic, BSTR Expression, VARIANT* Result, VARIANT_BOOL* retVal);
};

OBJECT_ENTRY_AUTO(__uuidof(Table), CTableClass)
