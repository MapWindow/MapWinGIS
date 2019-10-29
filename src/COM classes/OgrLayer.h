// OgrLayer.h : Declaration of the COgrLayer

#pragma once
#include "ogrsf_frmts.h"
#include "afxmt.h"
#include "OgrLoader.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE COgrLayer :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<COgrLayer, &CLSID_OgrLayer>,
	public IDispatchImpl<IOgrLayer, &IID_IOgrLayer, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	COgrLayer()
	{
		_pUnkMarshaler = NULL;
		_globalCallback = NULL;
		_key = SysAllocString(L"");
		_lastErrorCode = tkNO_ERROR;
		_dataset = NULL;
		_layer = NULL;
		_shapefile = NULL;
		_sourceType = ogrUninitialized;
		_forUpdate = false;
		_encoding = m_globalSettings.ogrEncoding;
		_dynamicLoading = VARIANT_FALSE;
		_envelope = NULL;
		_featureCount = -1;
		_activeShapeType = SHP_NULLSHAPE;
		_externalDatasource = VARIANT_FALSE;
		_loader.SetMaxCacheCount(m_globalSettings.ogrLayerMaxFeatureCount);
		gReferenceCounter.AddRef(tkInterface::idOgrLayer);
	}
	~COgrLayer()
	{
		SysFreeString(_key);
		if (_envelope)
			delete _envelope;
		Close();
		if (_globalCallback)
			_globalCallback->Release();
		gReferenceCounter.Release(tkInterface::idOgrLayer);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_OGRLAYER)

	BEGIN_COM_MAP(COgrLayer)
		COM_INTERFACE_ENTRY(IOgrLayer)
		COM_INTERFACE_ENTRY(IDispatch)
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

public:
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(OpenDatabaseLayer)(BSTR connectionString, int layerIndex, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal);   // not in public API
	STDMETHOD(OpenFromQuery)(BSTR connectionString, BSTR sql, VARIANT_BOOL* retVal);
    STDMETHOD(ExtendFromQuery)(BSTR sql, VARIANT_BOOL* retVal);
	STDMETHOD(OpenFromDatabase)(BSTR connectionString, BSTR layerName, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal);
	STDMETHOD(Close)();
	STDMETHOD(get_Name)(BSTR* retVal);
	STDMETHOD(GetBuffer)(IShapefile** retVal);
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
	STDMETHOD(get_DynamicLoading)(VARIANT_BOOL* pVal);
	STDMETHOD(put_DynamicLoading)(VARIANT_BOOL newVal);
	STDMETHOD(get_MaxFeatureCount)(LONG* pVal);
	STDMETHOD(put_MaxFeatureCount)(LONG newVal);
	STDMETHOD(SaveStyle)(BSTR Name, CStringW xml, VARIANT_BOOL* retVal);
	STDMETHOD(get_SupportsStyles)(VARIANT_BOOL* pVal);
	STDMETHOD(GetNumStyles)(LONG* pVal);
	STDMETHOD(get_StyleName)(LONG styleIndex, BSTR* pVal);
	STDMETHOD(ClearStyles)(VARIANT_BOOL* retVal);
	STDMETHOD(RemoveStyle)(BSTR styleName, VARIANT_BOOL* retVal);
	STDMETHOD(GenerateCategories)(BSTR FieldName, tkClassificationType classificationType, long numClasses, tkMapColor colorStart, tkMapColor colorEnd, tkColorSchemeType schemeType, VARIANT_BOOL* retVal);
	STDMETHOD(get_ShapeType2D)(ShpfileType* pVal);
	STDMETHOD(get_DriverName)(BSTR* pVal);
	STDMETHOD(OpenFromFile)(BSTR Filename, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal);
	STDMETHOD(get_AvailableShapeTypes)(VARIANT* pVal);
	STDMETHOD(get_ActiveShapeType)(ShpfileType* pVal);
	STDMETHOD(put_ActiveShapeType)(ShpfileType newVal);
	STDMETHOD(get_IsExternalDatasource)(VARIANT_BOOL* pVal);

private:
	VARIANT_BOOL _externalDatasource;	// we don't own this datasource, so must not close in destructor
	VARIANT_BOOL _dynamicLoading;
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
	OGREnvelope* _envelope;
	int _featureCount;
	OgrDynamicLoader _loader;
	ShpfileType _activeShapeType;		// this type will be used in case layer holds geometries of various types
	
private:
	bool CheckState();
	void ErrorMessage(long ErrorCode);
	void CloseShapefile();
	GDALDataset* OpenDataset(BSTR connectionString, bool forUpdate);
	long GetFidForShapefile();
	IShapefile* LoadShapefile();
	void InitOpenedLayer();
	void ForceCreateShapefile();
	void StopBackgroundLoading();
	void RestartBackgroundLoader() { _loader.Restart(); }
	void ClearCachedValues();
	bool HasStyleTable();
	CStringW GetDbSchemeName(bool withTrailingPoint);
	CStringW GetLayerName();
	CStringW GetStyleTableName();
	void GetFieldValues(OGRFieldType fieldType, BSTR& fieldName, vector<VARIANT*>& values);
	bool OpenDatabaseLayerCore(GDALDataset* ds, CStringW connectionString, int layerIndex, VARIANT_BOOL forUpdate, 
							   VARIANT_BOOL externalDatasource);

public:

	void InjectShapefile(IShapefile* sfNew);
    void UpdateShapefileFromOGRLoader();
	OGRLayer* GetDatasource() { return _layer; }
	CPLXMLNode* SerializeCore(CString ElementName);
	bool DeserializeCore(CPLXMLNode* node);
	OgrDynamicLoader* GetDynamicLoader() { return &_loader; }
	CStringW LoadStyleXML(CStringW name);
	GDALDataset* GetDataset() { return _dataset; }
	bool DeserializeOptions(CPLXMLNode* node);
	bool InjectLayer(GDALDataset* ds, int layerIndex, CStringW connection, VARIANT_BOOL forUpdate);
	void GetMsSqlShapeTypes(vector<ShpfileType>& types);
};
OBJECT_ENTRY_AUTO(__uuidof(OgrLayer), COgrLayer)
