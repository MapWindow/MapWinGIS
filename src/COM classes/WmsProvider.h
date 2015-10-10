// WmsProvider.h : Declaration of the CWmsProvider
#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CWmsProvider :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CWmsProvider, &CLSID_WmsProvider>,
	public IDispatchImpl<IWmsProvider, &IID_IWmsProvider, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CWmsProvider()
	{
		m_pUnkMarshaler = NULL;
		_key = SysAllocString(L"");
		_name = SysAllocString(L"");
		_layerCsv = SysAllocString(L"");
		_lastErrorCode = tkNO_ERROR; 
		_epgs = -1;

		gReferenceCounter.AddRef(tkInterface::idWmsProvider);
	}

	~CWmsProvider()
	{
		::SysFreeString(_key);
		::SysFreeString(_name);
		::SysFreeString(_layerCsv);

		gReferenceCounter.Release(tkInterface::idWmsProvider);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_WMSPROVIDER)

	BEGIN_COM_MAP(CWmsProvider)
		COM_INTERFACE_ENTRY(IWmsProvider)
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
	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(get_BoundingBox)(IExtents** pVal);
	STDMETHOD(put_BoundingBox)(IExtents* newVal);
	STDMETHOD(get_CrsEpsg)(LONG* pVal);
	STDMETHOD(put_CrsEpsg)(LONG newVal);
	STDMETHOD(get_LayersCsv)(BSTR* pVal);
	STDMETHOD(put_LayersCsv)(BSTR newVal);
	STDMETHOD(get_BaseUrl)(BSTR* pVal);
	STDMETHOD(put_BaseUrl)(BSTR newVal);

private:	
	BSTR _key;
	BSTR _name;
	BSTR _layerCsv;
	long _lastErrorCode;
	IExtents* _bounds;
	long _epgs;
	
};

OBJECT_ENTRY_AUTO(__uuidof(WmsProvider), CWmsProvider)
