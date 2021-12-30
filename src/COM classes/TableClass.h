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
#include <functional>
#include "TableRow.h"
#include "dbf.h"
#include "CustomExpression.h"
#include "_ITableEvents_CP.H"

class ATL_NO_VTABLE CTableClass : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CTableClass, &CLSID_Table>,
	public IDispatchImpl<ITable, &IID_ITable, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>,
	public IConnectionPointContainerImpl<CTableClass>,
	public CProxy_ITableEvents<CTableClass>
{
public:
	CTableClass()
		:_shapefile(NULL), _globalCallback(NULL), _lastErrorCode(tkNO_ERROR),
		_isEditingTable(FALSE), _dbfHandle(NULL), m_maxRowId(-1), _appendMode(false),
		_appendStartShapeCount(-1)
	{
		_pUnkMarshaler = NULL;
		_key = SysAllocString(L"");
		_lastRecordIndex = -1;
		gReferenceCounter.AddRef(tkInterface::idTable);
	}

	~CTableClass()
	{
		VARIANT_BOOL vbretval;
		this->Close(&vbretval);

		RemoveTempFiles();

		::SysFreeString(_key);

		if( _globalCallback )
			_globalCallback->Release();
		
		gReferenceCounter.Release(tkInterface::idTable);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TABLE)

	DECLARE_NOT_AGGREGATABLE(CTableClass)

	BEGIN_COM_MAP(CTableClass)
		COM_INTERFACE_ENTRY(ITable)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
		return S_OK;
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;

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

	STDMETHOD(get_JoinFields)(LONG joinIndex, BSTR* pVal);
	STDMETHOD(get_Filename)(BSTR* pVal);
	STDMETHOD(get_JoinOptions)(LONG joinIndex, BSTR* pVal);
	STDMETHOD(ClearCache)();
	STDMETHOD(get_RowIsModified)(LONG RowIndex, VARIANT_BOOL* pVal);

private:
	struct JoinInfo
	{
		CStringW filename;
		CStringW fieldFrom;
		CStringW fieldTo;
		CString options;
		int joinId;
		CStringW fields; // comma separated list
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
	IShapefile* _shapefile;
	int _lastRecordIndex;    // last index accessed with get_CellValue
	bool _appendMode;
	int _appendStartShapeCount;

public:
	bool m_needToSaveAsNewFile;
	int m_maxRowId;	// maximum value in the MWShapeId field

private:
	bool SaveToFile(const CStringW& dbfFilename, bool updateFileInPlace, ICallback* cBack);
	void LoadDefaultFields();
	void LoadDefaultRows();
	long RowCount() { return _rows.size(); }
	long FieldCount() { return _fields.size(); }
	bool ReadRecord(long RowIndex);
	bool WriteRecord(DBFInfo* dbfHandle, long fromRowIndex, long toRowIndex, bool isUTF8 = false);
	void ClearRow(long rowIndex);
	FieldType GetFieldType(long fieldIndex);
	long GetFieldPrecision(long fieldIndex);
	inline void ErrorMessage(long ErrorCode);
	std::vector<CStringW>* get_FieldNames();
	void RestoreJoins(CPLXMLNode* node);
	void RestoreFields(CPLXMLNode* node);
	void ClearFieldCustomizations();
	void RemoveTempFiles();
	void ClearRows();
	void TryClearLastRecord(long rowIndex);
	void ClearFields();
	bool ValidateFieldIndex(long fieldIndex);
	bool ValidateRowIndex(long rowIndex);

public:	
	void DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);
	void CloseUnderlyingFile(); 
	TableRow* CloneTableRow(int rowIndex);
	bool InsertTableRow(TableRow* row, long rowIndex);
	TableRow* SwapTableRow(TableRow* row, long rowIndex);
	bool GetUids(long fieldIndex, map<long, long>& resutls);
	bool UpdateTableRow(TableRow* newRow, long rowIndex);

	void ParseExpressionCore(BSTR Expression, tkValueType returnType, CStringW& ErrorString, VARIANT_BOOL* retVal);

	std::vector<CategoriesData>* GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses);
	std::vector<CategoriesData>* GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses,
		CComVariant minValue, CComVariant maxValue);
	void AnalyzeExpressions(std::vector<CStringW>& expressions, std::vector<int>& results,
		int startRowIndex = -1, int endRowIndex = -1);
	bool QueryCore(CStringW Expression, std::vector<long>& indices, CStringW& ErrorString);
	bool CalculateCore(CStringW Expression, std::vector<CStringW>& results, CStringW& ErrorString, CString floatFormat,
		int startRowIndex = -1, int endRowIndex = -1);
    bool CalculateCoreRaw(CStringW Expression, std::function<bool(CExpressionValue* value, int rowIndex, CStringW& ErrorString)> processValue, CStringW& ErrorString, CString floatFormat = m_globalSettings.floatNumberFormat,
		int startRowIndex = -1, int endRowIndex = -1, bool ignoreCalculationErrors = false);

	bool get_FieldValuesDouble(int FieldIndex, std::vector<double>& values);
	bool get_FieldValuesInteger(int FieldIndex, std::vector<int>& values);
	bool get_FieldValuesString(int FieldIndex, std::vector<CString>& values);

	bool set_IndexValue(int rowIndex);
	bool MakeUniqueFieldNames();
	bool CheckJoinInput(ITable* table2, CStringW fieldTo, CStringW fieldFrom, long& index1, long& index2);
	bool JoinFields(ITable* table2, std::vector<FieldMapping*>& mapping, set<CStringW>& fieldList);
	bool JoinInternal(ITable* table2, CStringW fieldTo, CStringW fieldFrom, CStringW filenameToReopen, CString options, set<CStringW>& fieldList);
	void RemoveJoinedFields();
	bool HasFieldChanges();
	void MarkFieldsAsUnchanged();
	void InjectShapefile(IShapefile* sf) { _shapefile = sf;	}
	IShapefile* GetParentShapefile() { return _shapefile; }
	bool GetSorting(long fieldIndex, vector<long>& indices);
	bool GetRelativeValues(long fieldIndex, bool logScale, vector<double>& values);
	bool WriteAppendedRow();
	void StartAppendMode() { _appendMode = true; _appendStartShapeCount = _rows.size(); };
	void StopAppendMode();
	void MarkRowIsClean(long rowIndex);
	void MarkFieldsAreClean();
	int GetFieldSourceIndex(int fieldIndex);
	void SetFieldSourceIndex(int fieldIndex, int sourceIndex);

public:
	BEGIN_CONNECTION_POINT_MAP(CTableClass)
		CONNECTION_POINT_ENTRY(__uuidof(_ITableEvents))
	END_CONNECTION_POINT_MAP()
	
};

OBJECT_ENTRY_AUTO(__uuidof(Table), CTableClass)
