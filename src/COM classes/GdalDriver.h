// GdalDriver.h : Declaration of the CGdalDriver
#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CGdalDriver :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CGdalDriver, &CLSID_GdalDriver>,
	public IDispatchImpl<IGdalDriver, &IID_IGdalDriver, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGdalDriver() : _driver(NULL)
	{
		_pUnkMarshaler = NULL;
		gReferenceCounter.AddRef(tkInterface::idGdalDriver);
	}

	~CGdalDriver()
	{
		gReferenceCounter.Release(tkInterface::idGdalDriver);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GDALDRIVER)

	BEGIN_COM_MAP(CGdalDriver)
		COM_INTERFACE_ENTRY(IGdalDriver)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;

public:
	STDMETHOD(get_Metadata)(tkGdalDriverMetadata metadata, BSTR* retVal);
	STDMETHOD(get_MetadataCount)(int* retVal);
	STDMETHOD(get_MetadataItem)(int metadataIndex, BSTR* retVal);
	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(get_IsVector)(VARIANT_BOOL* pVal);
	STDMETHOD(get_IsRaster)(VARIANT_BOOL* pVal);
	STDMETHOD(get_MetadataItemType)(int metadataIndex, tkGdalDriverMetadata* pVal);
	STDMETHOD(get_MetadataItemValue)(int metadataIndex, BSTR* pVal);
	STDMETHOD(get_MetadataItemKey)(int metadataIndex, BSTR* pVal);

private:
	GDALDriver* _driver;

private:
	bool CheckState();
	void ErrorMessage(CString msg);

public:
	void Inject(GDALDriver* driver) { _driver = driver; }

	
};

OBJECT_ENTRY_AUTO(__uuidof(GdalDriver), CGdalDriver)
