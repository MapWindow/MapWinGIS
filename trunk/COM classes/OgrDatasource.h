// OgrDatasource.h : Declaration of the COgrDatasource
#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// COgrDatasource
class ATL_NO_VTABLE COgrDatasource :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COgrDatasource, &CLSID_OgrDatasource>,
	public IDispatchImpl<IOgrDatasource, &IID_IOgrDatasource, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	COgrDatasource()
	{
		_dataset = NULL;
		_key = SysAllocString(L"");
		_lastErrorCode = tkNO_ERROR;
		_globalCallback = NULL;
		_encoding = m_globalSettings.ogrEncoding;
		gReferenceCounter.AddRef(tkInterface::idOgrDatasource);
	}
	~COgrDatasource()
	{
		SysFreeString(_key);
		Close();
		if (_globalCallback)
			_globalCallback->Release();
		gReferenceCounter.Release(tkInterface::idOgrDatasource);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_OGRDATASOURCE)

	BEGIN_COM_MAP(COgrDatasource)
		COM_INTERFACE_ENTRY(IOgrDatasource)
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
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(Close)();
	STDMETHOD(Open)(BSTR connectionString, VARIANT_BOOL* retVal);
	STDMETHOD(get_LayerCount)(int* retVal);
	STDMETHOD(GetLayer)(int index, VARIANT_BOOL forUpdate, IOgrLayer** retVal);
	STDMETHOD(get_DriverName)(BSTR* retVal);
	STDMETHOD(GetLayerName)(int index, BSTR* retVal);
	STDMETHOD(GetLayerByName)(BSTR name, VARIANT_BOOL forUpdate, IOgrLayer** retVal);
	STDMETHOD(RunQuery)(BSTR sql, IOgrLayer** retVal);
	STDMETHOD(DeleteLayer)(int layerIndex, VARIANT_BOOL* retVal);
	STDMETHOD(CreateLayer)(BSTR layerName, ShpfileType shpType, IGeoProjection* projection, BSTR creationOptions, VARIANT_BOOL* retVal);
	STDMETHOD(ImportShapefile)(IShapefile* shapefile, BSTR newLayerName, BSTR creationOptions, 
		tkShapeValidationMode validationMode, VARIANT_BOOL* retVal);
	STDMETHOD(TestCapability)(tkOgrDSCapability capability, VARIANT_BOOL* retVal);
	STDMETHOD(LayerIndexByName)(BSTR layerName, int* retVal);
	STDMETHOD(get_DriverMetadata)(tkGdalDriverMetadata metadata, BSTR* retVal);
	STDMETHOD(get_DriverMetadataCount)(int* retVal);
	STDMETHOD(get_DriverMetadataItem)(int metadataIndex, BSTR* retVal);
	STDMETHOD(ExecuteSQL)(BSTR sql, BSTR* errorMessage, VARIANT_BOOL* retVal);
	STDMETHOD(get_GdalLastErrorMsg)(BSTR* pVal);

private:
	ICallback * _globalCallback;
	long _lastErrorCode;
	BSTR _key;
	GDALDataset* _dataset;
	CStringW _connectionString;
	tkOgrEncoding _encoding;

private:
	bool CheckState();
	void DumpDriverInfo();
	void ErrorMessage(long ErrorCode);
	void GetMetaData(GDALDriver* driver);
	char** ParseLayerCreationOptions(BSTR creationOptions);
};

OBJECT_ENTRY_AUTO(__uuidof(OgrDatasource), COgrDatasource)
