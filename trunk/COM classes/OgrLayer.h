// OgrLayer.h : Declaration of the COgrLayer

#pragma once
#include "ogrsf_frmts.h"
#include "afxmt.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE COgrLayer :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COgrLayer, &CLSID_OgrLayer>,
	public IDispatchImpl<IOgrLayer, &IID_IOgrLayer, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	COgrLayer()
	{
		_globalCallback = NULL;
		_key = A2BSTR("");
		_lastErrorCode = tkNO_ERROR;
		_dataset = NULL;
		_layer = NULL;
		_shapefile = NULL;
		_sourceType = ogrUninitialized;
		_forUpdate = false;
		_dataLoadingLock.Unlock();
		_encoding = m_globalSettings.ogrEncoding;
		gReferenceCounter.AddRef(tkInterface::idOgrLayer);
	}
	~COgrLayer()
	{
		Close();
		if (_globalCallback)
			_globalCallback->Release();
		gReferenceCounter.Release(tkInterface::idOgrLayer);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_OGRLAYER)

	BEGIN_COM_MAP(COgrLayer)
		COM_INTERFACE_ENTRY(IOgrLayer)
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
	STDMETHOD(OpenDatabaseLayer)(BSTR connectionString, int layerIndex, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal);   // not in public API
	STDMETHOD(OpenFromQuery)(BSTR connectionString, BSTR sql, VARIANT_BOOL* retVal);
	STDMETHOD(OpenFromDatabase)(BSTR connectionString, BSTR layerName, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal);
	STDMETHOD(Close)();
	STDMETHOD(get_Name)(BSTR* retVal);
	STDMETHOD(GetData)(IShapefile** retVal);
	STDMETHOD(get_SourceType)(tkOgrSourceType* retVal);
	STDMETHOD(ReloadFromSource)(VARIANT_BOOL* retVal);
	STDMETHOD(RedefineQuery)(BSTR newSql, VARIANT_BOOL* retVal);
	STDMETHOD(GetConnectionString)(BSTR* retVal);
	STDMETHOD(GetSourceQuery)(BSTR* retVal);
	STDMETHOD(get_GeoProjection)(IGeoProjection** retVal);
	STDMETHOD(get_ShapeType)(ShpfileType* retVal);
	STDMETHOD(get_DataIsReprojected)(VARIANT_BOOL* retVal);
	STDMETHOD(get_FIDColumnName)(BSTR* retVal);
	STDMETHOD(SaveChanges)(int* savedCount, tkOgrSaveType saveType, VARIANT_BOOL validateShapes,  tkOgrSaveResult* retVal);
	STDMETHOD(HasLocalChanges)(VARIANT_BOOL* retVal);
	STDMETHOD(TestCapability)(tkOgrLayerCapability capability, VARIANT_BOOL* retVal);
	STDMETHOD(get_UpdateSourceErrorCount)(int* retVal);
	STDMETHOD(get_UpdateSourceErrorMsg)(int errorIndex, BSTR* retVal);
	STDMETHOD(get_UpdateSourceErrorShapeIndex)(int errorIndex, int* retVal);
	STDMETHOD(get_FeatureCount)(VARIANT_BOOL forceLoading, int* retVal);
	STDMETHOD(get_Extents)(IExtents** extents, VARIANT_BOOL forceLoading, VARIANT_BOOL *retVal);
	STDMETHOD(get_GeometryColumnName)(BSTR* retVal);
	STDMETHOD(get_SupportsEditing)(tkOgrSaveType editingType, VARIANT_BOOL* retVal);
	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal, VARIANT_BOOL* retVal);
	STDMETHOD(get_GdalLastErrorMsg)(BSTR* pVal);

public:
	void InjectShapefile(IShapefile* sfNew)
	{
		CloseShapefile();
		_shapefile = sfNew;
	}
	CPLXMLNode* SerializeCore(CString ElementName);
	bool DeserializeCore(CPLXMLNode* node);

private:
	CCriticalSection _dataLoadingLock;
	tkOgrSourceType _sourceType;
	IShapefile* _shapefile;
	GDALDataset* _dataset;
	OGRLayer* _layer;		// it's owned by dataset object
	ICallback * _globalCallback;
	long _lastErrorCode;
	BSTR _key;
	bool _forUpdate;
	CStringW _connectionString;
	CStringW _sourceQuery;
	vector<OgrUpdateError> _updateErrors;
	tkOgrEncoding _encoding;

	bool CheckState();
	void ErrorMessage(long ErrorCode);
	void CloseShapefile();
	GDALDataset* OpenDataset(BSTR connectionString, bool forUpdate);
	long GetFidForShapefile();
	IShapefile* LoadShapefile();
};

OBJECT_ENTRY_AUTO(__uuidof(OgrLayer), COgrLayer)
