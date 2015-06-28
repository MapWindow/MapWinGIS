#pragma once
#include "ExpressionParts.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CFunction :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CFunction, &CLSID_Function>,
	public IDispatchImpl<IFunction, &IID_IFunction, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CFunction()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_FUNCTION)

	BEGIN_COM_MAP(CFunction)
		COM_INTERFACE_ENTRY(IFunction)
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
	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(get_Alias)(long aliasIndex, BSTR* pVal);
	STDMETHOD(get_NumAliases)(long* pVal);
	STDMETHOD(get_NumParameters)(long* pVal);
	STDMETHOD(get_Group)(tkFunctionGroup* pVal);
	STDMETHOD(get_Description)(BSTR* pVal);
	STDMETHOD(put_Description)(BSTR newVal);
	STDMETHOD(get_ParameterName)(LONG parameterIndex, BSTR* pVal);
	STDMETHOD(put_ParameterName)(LONG parameterIndex, BSTR newVal);
	STDMETHOD(get_ParameterDescription)(LONG parameterIndex, BSTR* pVal);
	STDMETHOD(put_ParameterDescription)(LONG parameterIndex, BSTR newVal);
	STDMETHOD(get_Signature)(BSTR* pVal);

private:
	CustomFunction* _function;

public:
	void Inject(CustomFunction* fn) { _function = fn; }
	bool Validate();
	
};

OBJECT_ENTRY_AUTO(__uuidof(Function), CFunction)
