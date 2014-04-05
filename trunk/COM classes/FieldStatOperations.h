// FieldStatOperations.h : Declaration of the CFieldStatOperations
#pragma once
#include "MapWinGIS.h"

//#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
//#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
//#endif

struct FieldOperation
{
	int targetIndex;
	int fieldIndex;
	CStringW fieldName;
	bool hasName;
	tkFieldStatOperation operation;
	bool valid;
	FieldType targetFieldType;
	tkFieldOperationValidity isValidReason;
};

// CFieldStatOperations
class ATL_NO_VTABLE CFieldStatOperations :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFieldStatOperations, &CLSID_FieldStatOperations>,
	public IDispatchImpl<IFieldStatOperations, &IID_IFieldStatOperations, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CFieldStatOperations()
	{
		USES_CONVERSION;
		m_lastErrorCode = tkNO_ERROR;
		m_key = A2BSTR("stuff");
	}

	~CFieldStatOperations()
	{
		Clear();
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_FIELDSTATOPERATIONS)

	BEGIN_COM_MAP(CFieldStatOperations)
		COM_INTERFACE_ENTRY(IFieldStatOperations)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(AddFieldIndex)(int fieldIndex, tkFieldStatOperation operation);
	STDMETHOD(AddFieldName)(BSTR fieldIndex, tkFieldStatOperation operation);
	STDMETHOD(Remove)(int operationIndex, VARIANT_BOOL* retVal);
	STDMETHOD(Clear)();
	STDMETHOD(get_Count)(int* retVal);
	STDMETHOD(get_Operation)(int operationIndex, tkFieldStatOperation* retVal);
	STDMETHOD(get_FieldIndex)(int operationIndex, int* retVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_FieldName)(int operationIndex, BSTR* retVal);
	STDMETHOD(Validate)(IShapefile* sf, VARIANT_BOOL* retVal);
	STDMETHOD(get_OperationIsValid)(int operationIndex, VARIANT_BOOL* retVal);
	STDMETHOD(get_OperationIsValidReason)(int operationIndex, tkFieldOperationValidity* retVal);
private:
	void ErrorMessage(long ErrorCode);
	long m_lastErrorCode;
	BSTR m_key;
public:
	std::vector<FieldOperation*> _operations;
};

OBJECT_ENTRY_AUTO(__uuidof(FieldStatOperations), CFieldStatOperations)
