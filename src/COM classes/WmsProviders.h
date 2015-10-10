// WmsProviders.h : Declaration of the CWmsProviders
#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CWmsProviders :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CWmsProviders, &CLSID_WmsProviders>,
	public IDispatchImpl<IWmsProviders, &IID_IWmsProviders, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CWmsProviders()
	{
		m_pUnkMarshaler = NULL;

		_key = SysAllocString(L"");
		_lastErrorCode = tkNO_ERROR;

		gReferenceCounter.AddRef(tkInterface::idWmsProviders);
	}

	~CWmsProviders()
	{
		SysFreeString(_key);

		Clear();

		gReferenceCounter.Release(tkInterface::idWmsProviders);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_WMSPROVIDERS)

	BEGIN_COM_MAP(CWmsProviders)
		COM_INTERFACE_ENTRY(IWmsProviders)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(
			GetControllingUnknown(), &m_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		m_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> m_pUnkMarshaler;

public:
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(Add)(IWmsProvider* provider);
	STDMETHOD(Clear)();
	STDMETHOD(get_Count)(LONG* pVal);
	STDMETHOD(get_Item)(LONG Index, IWmsProvider** pVal);
	STDMETHOD(Remove)(LONG providerId, VARIANT_BOOL* retVal);

private:
	void ErrorMessage(long ErrorCode);

private:
	BSTR _key;
	long _lastErrorCode;
	vector<IWmsProvider*> _providers;
};

OBJECT_ENTRY_AUTO(__uuidof(WmsProviders), CWmsProviders)
