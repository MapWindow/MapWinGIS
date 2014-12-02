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
#include <set>
#include "TableRow.h"
#include "dbf.h"
#include "Expression.h"
#include "_ITableEvents_CP.H"

class ATL_NO_VTABLE CTableClass : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTableClass, &CLSID_Table>,
	public IDispatchImpl<ITable, &IID_ITable, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>,
	public IConnectionPointContainerImpl<CTableClass>,
	public CProxy_ITableEvents<CTableClass>
{
public:
	CTableClass()
	{
		_key = SysAllocString(L"");
		_globalCallback = NULL;
		_lastErrorCode = tkNO_ERROR;
		_isEditingTable = FALSE;
		_dbfHandle = NULL;
		m_maxRowId = -1;
		gReferenceCounter.AddRef(tkInterface::idTable);
	}
	~CTableClass()
	{
		VARIANT_BOOL vbretval;
		this->Close(&vbretval);

		for (int i = 0; i < (int)_tempFiles.size(); i++ )
		{
			try
			{
				CString * a = _tempFiles[i];
				_unlink(a->GetBuffer());
				delete a;
			}
			catch(...)
			{
				ASSERT(FALSE);
			}
		}
		_tempFiles.clear();

		::SysFreeString(_key);

		if( _globalCallback )
			_globalCallback->Release();
		_globalCallback = NULL;
		gReferenceCounter.Release(tkInterface::idTable);
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
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
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

	STDMETHOD(StopJoin)(int joinIndex, VARIANT_BOOL* retVal);
	STDMETHOD(StopAllJoins)();

	STDMETHOD(get_IsJoined)(VARIANT_BOOL* retVal);
	STDMETHOD(get_JoinCount)(int* retVal);

	STDMETHOD(get_JoinFilename)(int joinIndex, BSTR* retVal);
	STDMETHOD(get_JoinFromField)(int joinIndex, BSTR* retVal);
	STDMETHOD(get_JoinToField)(int joinIndex, BSTR* retVal);

	STDMETHOD(get_FieldIsJoined)(int FieldIndex, VARIANT_BOOL* retVal);
	STDMETHOD(get_FieldJoinIndex)(int FieldIndex, int* retVal);

	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);

	STDMETHOD(TryJoin)(ITable* table2, BSTR fieldTo, BSTR fieldFrom, int* rowCount, int* joinRowCount, VARIANT_BOOL* retVal);

	STDMETHOD(Join)(ITable* table2, BSTR fieldTo, BSTR fieldFrom, VARIANT_BOOL* retVal);
	STDMETHOD(Join2)(ITable* table2, BSTR fieldTo, BSTR fieldFrom, BSTR filenameToReopen, BSTR joinOptions, VARIANT_BOOL* retVal);
	STDMETHOD(Join3)(ITable* table2, BSTR fieldTo, BSTR fieldFrom, BSTR filenameToReopen, BSTR joinOptions, SAFEARRAY* filedList, VARIANT_BOOL* retVal);

	STDMETHOD(Dump)(/*[in]*/ BSTR dbfFilename, /*[in, optional]*/ ICallback * cBack, /*[out, retval]*/ VARIANT_BOOL * retval);

private:
	struct JoinInfo
	{
		CStringW filename;
		CString fieldFrom;
		CString fieldTo;
		CString options;
		int joinId;
		CString fields; // comma separated list
	};

	struct FieldMapping
	{
		int srcIndex;
		int destIndex;
	};
	
private:
	DBFInfo * _dbfHandle;	// underlying data structure
	std::vector<FieldWrapper*> _fields;
	std::vector<RecordWrapper> _rows;
	std::deque<CString *> _tempFiles;
	BSTR _key;
	long _lastErrorCode;
	ICallback * _globalCallback;
	BOOL _isEditingTable;
	CStringW _filename;
	vector<JoinInfo*> _joins;
	int _lastJoinId;

public:
	bool m_needToSaveAsNewFile;
	int m_maxRowId;	// maximum value in the MWShapeId field

private:
	bool SaveToFile(const CStringW& dbfFilename, bool updateFileInPlace, ICallback* cBack);
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
	bool DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);
	void CloseUnderlyingFile(); 
	TableRow* CloneTableRow(int rowIndex);
	bool InsertTableRow(TableRow* row, long rowIndex);
	TableRow* SwapTableRow(TableRow* row, long rowIndex);
	bool GetUids(long fieldIndex, map<long, long>& resutls);
	bool UpdateTableRow(TableRow* newRow, long rowIndex);

	void ParseExpressionCore(BSTR Expression, tkValueType returnType, CString& ErrorString, VARIANT_BOOL* retVal);

	std::vector<CategoriesData>* GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses);
	std::vector<CategoriesData>* GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses,
		CComVariant minValue, CComVariant maxValue);
	void AnalyzeExpressions(std::vector<CString>& expressions, std::vector<int>& results);
	bool QueryCore(CString Expression, std::vector<long>& indices, CString& ErrorString);
	bool CalculateCore(CString Expression, std::vector<CString>& results, CString& ErrorString, int rowIndex = -1);

	bool get_FieldValuesDouble(int FieldIndex, std::vector<double>& values);
	bool get_FieldValuesInteger(int FieldIndex, std::vector<int>& values);
	bool get_FieldValuesString(int FieldIndex, std::vector<CString>& values);

	bool set_IndexValue(int rowIndex);

	bool MakeUniqueFieldNames();

	bool CheckJoinInput(ITable* table2, CString fieldTo, CString fieldFrom, long& index1, long& index2);

	bool JoinFields(ITable* table2, std::vector<FieldMapping*>& mapping, set<CString>& fieldList);
	bool JoinInternal(ITable* table2, CString fieldTo, CString fieldFrom, CStringW filenameToReopen, CString options, set<CString>& fieldList);
	void RemoveJoinedFields();

public:
	BEGIN_CONNECTION_POINT_MAP(CTableClass)
		CONNECTION_POINT_ENTRY(__uuidof(_ITableEvents))
	END_CONNECTION_POINT_MAP()
	
};

OBJECT_ENTRY_AUTO(__uuidof(Table), CTableClass)
