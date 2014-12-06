#include "stdafx.h"
#include "OgrLayer.h"
#include "OgrHelper.h"
#include "UtilityFunctions.h"
#include "GeoProjection.h"
#include "OgrConverter.h"
#include "Shapefile.h"
#include "OgrStyle.h"
#include "Shape2Ogr.h"
#include "Ogr2Shape.h"
#include "FieldClassification.h"
#include "ShapefileCategories.h"

// *************************************************************
//		InjectShapefile()
// *************************************************************
void COgrLayer::InjectShapefile(IShapefile* sfNew)
{
	CloseShapefile();
	_shapefile = sfNew;
}

// *************************************************************
//		InitOpenedLayer()
// *************************************************************
void COgrLayer::InitOpenedLayer()
{
	ClearCachedValues();
	VARIANT_BOOL vb;

	// Let's cache some values, as the driver can be busy with background loading
	// later on, so the value may not be readily available
	int featureCount;
	get_FeatureCount(VARIANT_FALSE, &featureCount);
	
	CComPtr<IExtents> extents = NULL;
	get_Extents(&extents, VARIANT_FALSE, &vb);

	if (m_globalSettings.autoChooseOgrLoadingMode) {
		long maxCount;
		get_MaxFeatureCount(&maxCount);
		put_DynamicLoading(featureCount > maxCount ? VARIANT_TRUE : VARIANT_FALSE);
	}

	ForceCreateShapefile();
	RestartBackgroundLoader();

}

//***********************************************************************
//*		ClearCachedValues()
//***********************************************************************
void COgrLayer::ClearCachedValues()
{
	if (_envelope)
	{
		delete _envelope;
		_envelope = NULL;
	}
	if (_featureCount != -1) 
		_featureCount = -1;
}

//***********************************************************************
//*		StopBackgroundLoading()
//***********************************************************************
void COgrLayer::StopBackgroundLoading()
{
	_loader.AddWaitingTask(true);  // notify working thread that it's time is over
	_loader.LockLoading(true);     // wait until it is finished
	_loader.LockLoading(false);
}

//***********************************************************************
//*		LoadShapefile()
//***********************************************************************
IShapefile* COgrLayer::LoadShapefile()
{ 
	bool isTrimmed = false;	
	IShapefile* sf = Ogr2Shape::Layer2Shapefile(_layer, _loader.GetMaxCacheCount(), isTrimmed, &_loader, _globalCallback); 
	if (isTrimmed) {
		ErrorMessage(tkOGR_LAYER_TRIMMED);
	}
	return sf;
}

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP COgrLayer::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP COgrLayer::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	USES_CONVERSION;
	_key = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void COgrLayer::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	Utility::DisplayErrorMsg("OgrLayer", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

STDMETHODIMP COgrLayer::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

STDMETHODIMP COgrLayer::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

//***********************************************************************/
//*		get/put_GlobalCallback()
//***********************************************************************/
STDMETHODIMP COgrLayer::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _globalCallback;
	if (_globalCallback != NULL) _globalCallback->AddRef();
	return S_OK;
}

STDMETHODIMP COgrLayer::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *************************************************************
//		CheckState()
// *************************************************************
bool COgrLayer::CheckState()
{
	if (!_dataset)
	{
		ErrorMessage(tkOGR_LAYER_UNINITIALIZED);
		return false;
	}
	return true;
}

// *************************************************************
//		get_SourceType()
// *************************************************************
STDMETHODIMP COgrLayer::get_SourceType(tkOgrSourceType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _sourceType;
	return S_OK;
}

// *************************************************************
//		Close()
// *************************************************************
STDMETHODIMP COgrLayer::Close()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	StopBackgroundLoading();
	if (_dataset)
	{
		if (_sourceType == ogrQuery && _layer)
			_dataset->ReleaseResultSet(_layer);
	
		// this will release memory for table layer as well
		GDALClose(_dataset);
		_dataset = NULL;
		_layer = NULL;
	}
	CloseShapefile();
	_updateErrors.clear();
	_sourceType = ogrUninitialized;
	_connectionString = L"";
	_sourceQuery = L"";
	_forUpdate = false;
	return S_OK;
}

// *************************************************************
//		CloseShapefile()
// *************************************************************
void COgrLayer::CloseShapefile()
{
	if (_shapefile)
	{
		VARIANT_BOOL vb;
		_shapefile->Close(&vb);
		ULONG count = _shapefile->Release();
		_shapefile = NULL;
	}
}

// *************************************************************
//		OpenDataset()
// *************************************************************
GDALDataset* COgrLayer::OpenDataset(BSTR connectionString, bool forUpdate)
{
	GDALDataset* ds = GdalHelper::OpenOgrDatasetW(OLE2W(connectionString), forUpdate);
	if (!ds)
	{
		// clients should extract last GDAL error
		ErrorMessage(tkFAILED_TO_OPEN_OGR_DATASOURCE);
	}
	return ds;
}

// *************************************************************
//		OpenDatabaseLayer()
// *************************************************************
STDMETHODIMP COgrLayer::OpenDatabaseLayer(BSTR connectionString, int layerIndex, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	Close();

	GDALDataset* ds = OpenDataset(connectionString, forUpdate ? true: false);
	if (ds)
	{
		if (layerIndex < 0 || layerIndex >= ds->GetLayerCount())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			GDALClose(ds);
			return S_FALSE;
		}
		
		OGRLayer* layer = ds->GetLayer(layerIndex);
		if (!layer)
		{
			ErrorMessage(tkFAILED_TO_OPEN_OGR_LAYER);
			GDALClose(ds);
			return S_FALSE;
		}
	
		USES_CONVERSION;
		_connectionString = OLE2W(connectionString);
		_sourceQuery = OgrHelper::OgrString2Unicode(layer->GetName());
		_dataset = ds;
		_layer = layer;
		_forUpdate = forUpdate == VARIANT_TRUE;
		_sourceType = ogrDbTable;
		InitOpenedLayer();

		*retVal = VARIANT_TRUE;
		return S_OK;
	}
	return S_FALSE;
}

// *************************************************************
//		OpenFromQuery()
// *************************************************************
STDMETHODIMP COgrLayer::OpenFromQuery(BSTR connectionString, BSTR sql, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	Close();

	GDALDataset* ds = OpenDataset(connectionString, false);
	if (ds)
	{
		OGRLayer* layer = ds->ExecuteSQL(OgrHelper::Bstr2OgrString(sql), NULL, NULL);
		if (layer)
		{
			_connectionString = OLE2W(connectionString);
			_sourceQuery = OLE2W(sql);
			_dataset = ds;
			_layer = layer;
			_sourceType = ogrQuery;
			_forUpdate = VARIANT_FALSE;
			InitOpenedLayer();
			*retVal = VARIANT_TRUE;
			return S_OK;
		}
		else
		{
			ErrorMessage(tkOGR_QUERY_FAILED);
			GDALClose(_dataset);
		}
	}
	return S_FALSE;
}

// *************************************************************
//		OpenFromDatabase()
// *************************************************************
STDMETHODIMP COgrLayer::OpenFromDatabase(BSTR connectionString, BSTR layerName, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	Close();

	GDALDataset* ds = OpenDataset(connectionString, forUpdate ? true : false);
	if (ds)
	{
		OGRLayer* layer = ds->GetLayerByName(OgrHelper::Bstr2OgrString(layerName));
		if (layer)
		{
			_connectionString = OLE2W(connectionString);
			_sourceQuery = OLE2W(layerName);
			_sourceType = ogrDbTable;
			_dataset = ds;
			_layer = layer;
			_forUpdate = forUpdate == VARIANT_FALSE;
			InitOpenedLayer();
			*retVal = VARIANT_TRUE;
			return S_OK;
		}
		else
		{
			ErrorMessage(tkFAILED_TO_OPEN_OGR_LAYER);
			GDALClose(_dataset);
		}
	}
	return S_FALSE;
}

// *************************************************************
//		get_Name()
// *************************************************************
STDMETHODIMP COgrLayer::get_Name(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!CheckState())
	{
		*retVal = A2BSTR("");
		return S_FALSE;
	}
	else
	{
		CStringW name = OgrHelper::OgrString2Unicode(_layer->GetName());
		*retVal = W2BSTR(name);
		return S_OK;
	}
}

// *************************************************************
//		get_Data()
// *************************************************************
STDMETHODIMP COgrLayer::GetData(IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*retVal = NULL;
	if (!CheckState()) return S_FALSE;
	
	CSingleLock sfLock(&_loader.ShapefileLock);
	sfLock.Lock();
	
	if (!_shapefile)
	{
		if (_dynamicLoading) {
			CSingleLock lock(&_loader.ProviderLock);
			lock.Lock();
			_shapefile = Ogr2Shape::CreateShapefile(_layer);
		}
		else {
			_shapefile = LoadShapefile();
		}
	}
	
	sfLock.Unlock();

	if (_shapefile)
	{
		*retVal = _shapefile;
		_shapefile->AddRef();
	}
	return S_OK;
}

// *************************************************************
//		ReloadFromSource()
// *************************************************************
STDMETHODIMP COgrLayer::ReloadFromSource(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_FALSE;

	if (_dynamicLoading) 
	{
		ErrorMessage(tkNOT_ALLOWED_IN_OGR_DYNAMIC_MODE);
		return S_OK;		
	}
	
	CSingleLock sfLock(&_loader.ShapefileLock);
	sfLock.Lock();

	CloseShapefile();
	_shapefile = LoadShapefile();
	
	sfLock.Unlock();
	*retVal = _shapefile ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		RedefineQuery()
// *************************************************************
STDMETHODIMP COgrLayer::RedefineQuery(BSTR newSql, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_FALSE;

	if (_dynamicLoading)
	{
		ErrorMessage(tkNOT_ALLOWED_IN_OGR_DYNAMIC_MODE);
		return S_OK;
	}

	if (_sourceType == ogrQuery)
	{
		OGRLayer* layer = _dataset->ExecuteSQL(OgrHelper::Bstr2OgrString(newSql), NULL, NULL);
		if (layer)
		{
			_dataset->ReleaseResultSet(_layer);
			_sourceQuery = OLE2W(newSql);
			_layer = layer;
			ClearCachedValues();
			CloseShapefile();
			return S_OK;
		}
		else
		{
			ErrorMessage(tkOGR_QUERY_FAILED);
			return S_FALSE;
		}
	}
	else
	{
		ErrorMessage(tkUNEXPECTED_OGR_SOURCE_TYPE);
		return S_FALSE;
	}
	return S_OK;
}

// *************************************************************
//		get_ConnectionString()
// *************************************************************
STDMETHODIMP COgrLayer::GetConnectionString(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = W2BSTR(_connectionString);
	return S_OK;
}

// *************************************************************
//		get_SourceQuery()
// *************************************************************
STDMETHODIMP COgrLayer::GetSourceQuery(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = W2BSTR(_sourceQuery);
	return S_OK;
}

// *************************************************************
//		get_GeoProjection()
// *************************************************************
STDMETHODIMP COgrLayer::get_GeoProjection(IGeoProjection** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IGeoProjection* gp = NULL;
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&gp);
	*retVal = gp;

	if (!CheckState()) return S_OK;

	CSingleLock lock(&_loader.ProviderLock);
	if (_dynamicLoading) lock.Lock();

	OGRSpatialReference* sr = _layer->GetSpatialRef();		// owned by OGRLayer
	if (sr)((CGeoProjection*)gp)->InjectSpatialReference(sr);
	return S_OK;
}

// *************************************************************
//		get_ShapeType()
// *************************************************************
STDMETHODIMP COgrLayer::get_ShapeType(ShpfileType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = SHP_NULLSHAPE;
	if (!CheckState()) return S_FALSE;
	*retVal = OgrConverter::GeometryType2ShapeType(_layer->GetGeomType());
	return S_OK;
}

// *************************************************************
//		get_ShapeType2D()
// *************************************************************
STDMETHODIMP COgrLayer::get_ShapeType2D(ShpfileType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	get_ShapeType(pVal);
	*pVal = Utility::ShapeTypeConvert2D(*pVal);
	return S_OK;
}

// *************************************************************
//		get_DataIsReprojected()
// *************************************************************
STDMETHODIMP COgrLayer::get_DataIsReprojected(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_FALSE;
	if (!_shapefile) return S_FALSE;		// data wasn't loaded yet

	CComPtr<IGeoProjection> gp = NULL;
	get_GeoProjection(&gp);
	if (!gp) return S_FALSE;

	CComPtr<IShapefile> sf = NULL;
	GetData(&sf);
	if (sf)
	{
		CComPtr<IGeoProjection> gp2 = NULL;
		sf->get_GeoProjection(&gp2);
		VARIANT_BOOL isSame;
		gp->get_IsSame(gp2, &isSame);
		*retVal = !isSame;
	}
	return S_OK;
}

// *************************************************************
//		get_FIDColumn()
// *************************************************************
STDMETHODIMP COgrLayer::get_FIDColumnName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (CheckState())
	{
		CStringW s = OgrHelper::OgrString2Unicode(_layer->GetFIDColumn());
		*retVal = W2BSTR(s);
		return S_OK;
	}
	*retVal = A2BSTR("");
	return S_FALSE;
}

// *************************************************************
//		SaveChanges()
// *************************************************************
STDMETHODIMP COgrLayer::SaveChanges(int* savedCount, tkOgrSaveType saveType, VARIANT_BOOL validateShapes, tkOgrSaveResult* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*savedCount = 0;
	*retVal = osrNoChanges;
	_updateErrors.clear();

	if (!CheckState()) return S_FALSE;
	if (!_shapefile)
	{
		ErrorMessage(tkNO_OGR_DATA_WAS_LOADED);
		return S_FALSE;
	}
	
	VARIANT_BOOL hasChanges;
	HasLocalChanges(&hasChanges);
	if (!hasChanges)
	{
		ErrorMessage(tkOGR_NO_MODIFICATIONS);
		return S_FALSE;
	}

	*retVal = osrNoneSaved;
	long shapeCmnId = GetFidForShapefile();
	if (shapeCmnId == -1)
	{
		ErrorMessage(tkFID_COLUMN_NOT_FOUND);
		return S_FALSE;
	}

	VARIANT_BOOL reprojected;
	get_DataIsReprojected(&reprojected);
	if (reprojected && saveType != tkOgrSaveType::ostAttributesOnly)
	{
		ErrorMessage(tkFID_COLUMN_NOT_FOUND);
		return S_FALSE;
	}

	CSingleLock lock(&_loader.ProviderLock);
	if (_dynamicLoading) lock.Lock();

	*savedCount = Shape2Ogr::SaveShapefileChanges(_layer, _shapefile, shapeCmnId, saveType, validateShapes ? true : false, _updateErrors);

	lock.Unlock();

	if (*savedCount == 0)
	{
		*retVal = osrNoneSaved;
	}
	else
	{
		HasLocalChanges(&hasChanges);
		*retVal = hasChanges ? osrSomeSaved : osrAllSaved;
	}
	return S_OK;
}

// *************************************************************
//		HasLocalChanges()
// *************************************************************
STDMETHODIMP COgrLayer::HasLocalChanges(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_FALSE;

	if (_shapefile)
	{
		long numShapes;
		_shapefile->get_NumShapes(&numShapes);

		CSingleLock lock(&_loader.ProviderLock);
		if (_dynamicLoading) lock.Lock();

		int featureCount = _layer->GetFeatureCount();
		if (numShapes != featureCount)	   // i.e. deleted or inserted feature
		{
			*retVal = VARIANT_TRUE;
		}
		else
		{
			VARIANT_BOOL modified;
			for (long i = 0; i < numShapes; i++)
			{
				_shapefile->get_ShapeModified(i, &modified);
				if (modified)
				{
					*retVal = VARIANT_TRUE;
					break;
				}
			}
		}
	}
	return S_OK;
}

// *************************************************************
//		GetFidMapping()
// *************************************************************
long COgrLayer::GetFidForShapefile()
{
	if (!_layer || !_shapefile) return -1;
	CComBSTR bstr;
	get_FIDColumnName(&bstr);
	CComPtr<ITable> table = NULL;
	_shapefile->get_Table(&table);
	long shapeCmnId;
	table->get_FieldIndexByName(bstr, &shapeCmnId);
	return shapeCmnId;
}

// *************************************************************
//		TestCapability()
// *************************************************************
STDMETHODIMP COgrLayer::TestCapability(tkOgrLayerCapability capability, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_FALSE;
	int val = _layer->TestCapability(OgrHelper::GetLayerCapabilityString(capability));
	*retVal = val ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		UpdateSourceErrorCount()
// *************************************************************
STDMETHODIMP COgrLayer::get_UpdateSourceErrorCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _updateErrors.size();
	return S_OK;
}

// *************************************************************
//		UpdateSourceErrorMsg()
// *************************************************************
STDMETHODIMP COgrLayer::get_UpdateSourceErrorMsg(int errorIndex, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (errorIndex < 0 || errorIndex >= (int)_updateErrors.size())
	{
		*retVal = A2BSTR("");
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	*retVal = W2BSTR(_updateErrors[errorIndex].ErrorMsg);
	return S_OK;
}

// *************************************************************
//		UpdateSourceErrorShapeIndex()
// *************************************************************
STDMETHODIMP COgrLayer::get_UpdateSourceErrorShapeIndex(int errorIndex, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (errorIndex < 0 || errorIndex >= (int)_updateErrors.size())
	{
		*retVal = -1;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	*retVal = _updateErrors[errorIndex].ShapeIndex;
	return S_OK;
}

// *************************************************************
//		get_FeatureCount()
// *************************************************************
STDMETHODIMP COgrLayer::get_FeatureCount(VARIANT_BOOL forceLoading, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = 0;
	if (!CheckState()) return S_FALSE;
	CSingleLock lock(&_loader.ProviderLock);
	if (_dynamicLoading) lock.Lock();
	if (_featureCount == -1 || forceLoading) {
		_featureCount = _layer->GetFeatureCount(forceLoading == VARIANT_TRUE);
	}
	*retVal = _featureCount;
	return S_OK;
}


// *************************************************************
//		ForceCreateShapefile()
// *************************************************************
void COgrLayer::ForceCreateShapefile()
{
	tkOgrSourceType sourceType;
	get_SourceType(&sourceType);
	if (_dynamicLoading && (!_shapefile) && (sourceType != ogrUninitialized)) {
		CSingleLock lock(&_loader.ProviderLock);
		lock.Lock();
		_shapefile = Ogr2Shape::CreateShapefile(_layer);
	}
}

// *************************************************************
//		get_Extents()
// *************************************************************
STDMETHODIMP COgrLayer::get_Extents(IExtents** extents, VARIANT_BOOL forceLoading, VARIANT_BOOL *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*extents = NULL;
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_FALSE;

	if (_envelope && forceLoading) {
		delete _envelope;
		_envelope = NULL;
	}

	if (!_envelope) {
		
		_envelope = new OGREnvelope();
		CSingleLock lock(&_loader.ProviderLock);
		if (_dynamicLoading) lock.Lock();
		_layer->GetExtent(_envelope, forceLoading == VARIANT_TRUE);
	}
	
	if (_envelope) {
		IExtents* ext = NULL;
		ComHelper::CreateInstance(idExtents, (IDispatch**)&ext);
		ext->SetBounds(_envelope->MinX, _envelope->MinY, 0.0, _envelope->MaxX, _envelope->MaxY, 0.0);
		*extents = ext;
		*retVal = VARIANT_TRUE;
		return S_OK;
	}
	
	return S_FALSE;
}

// *************************************************************
//		get_GeometryColumnName()
// *************************************************************
STDMETHODIMP COgrLayer::get_GeometryColumnName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!CheckState()) return S_FALSE;
	CStringW name = OgrHelper::OgrString2Unicode(_layer->GetGeometryColumn());
	USES_CONVERSION;
	*retVal = W2BSTR(name);
	return S_OK;
}

// *************************************************************
//		get_SupportsEditing()
// *************************************************************
STDMETHODIMP COgrLayer::get_SupportsEditing(tkOgrSaveType editingType, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_FALSE;
	
	// is it supported by driver?
	VARIANT_BOOL randomWrite;
	TestCapability(tkOgrLayerCapability::olcRandomWrite, &randomWrite);
	if (!randomWrite)
	{
		ErrorMessage(tkOGR_RANDOM_WRITE_NOT_SUPPORTED);
		return S_OK;
	}
		
	// do we have FID column?
	if (_shapefile)
	{
		long fid = GetFidForShapefile();
		if (fid == -1)
		{
			ErrorMessage(tkFID_COLUMN_NOT_FOUND);
			return S_OK;
		}
	}
	else
	{
		CString cmn = _layer->GetFIDColumn();
		if (cmn.GetLength() == 0)
		{
			ErrorMessage(tkFID_COLUMN_NOT_FOUND);
			return S_OK;
		}
	}

	// do we have reprojected data?
	VARIANT_BOOL reprojected;
	get_DataIsReprojected(&reprojected);
	if (reprojected && editingType != ostAttributesOnly)
	{
		ErrorMessage(tkCANT_SAVE_REPROJECTED_GEOMETRIES);
		return S_OK;
	}
	*retVal = VARIANT_TRUE;
	return S_OK;
}

// *************************************************************
//		Serialize()
// *************************************************************
STDMETHODIMP COgrLayer::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CPLXMLNode* psTree = this->SerializeCore("OgrLayerClass");
	Utility::SerializeAndDestroyXmlTree(psTree, retVal);
	return S_OK;
}

// *************************************************************
//		SerializeCore()
// *************************************************************
CPLXMLNode* COgrLayer::SerializeCore(CString ElementName)
{
	CPLXMLNode* psTree = CPLCreateXMLNode(NULL, CXT_Element, ElementName);

	USES_CONVERSION;
	CStringW skey = OLE2W(_key);
	if (skey.GetLength() != 0) Utility::CPLCreateXMLAttributeAndValue(psTree, "Key", skey);
	Utility::CPLCreateXMLAttributeAndValue(psTree, "ConnectionString", Utility::ConvertToUtf8(_connectionString));
	Utility::CPLCreateXMLAttributeAndValue(psTree, "SourceQuery", Utility::ConvertToUtf8(_sourceQuery));
	Utility::CPLCreateXMLAttributeAndValue(psTree, "SourceType", CPLString().Printf("%d", (int)_sourceType));
	Utility::CPLCreateXMLAttributeAndValue(psTree, "ForUpdate", CPLString().Printf("%d", (int)_forUpdate));
	if (_loader.LabelExpression.GetLength() > 0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LabelExpression", Utility::ConvertToUtf8(_loader.LabelExpression));
	if (_loader.LabelPosition != lpNone)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LabelPosition", CPLString().Printf("%d", (int)_loader.LabelPosition));
	if (_loader.LabelOrientation != lorParallel)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LabelOrientation", CPLString().Printf("%d", (int)_loader.LabelOrientation));
	if (_loader.GetMaxCacheCount() != m_globalSettings.ogrLayerMaxFeatureCount)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxFeatureCount", CPLString().Printf("%d", (int)_loader.GetMaxCacheCount()));
	
	if (_shapefile)
	{
		CSingleLock sfLock(&_loader.ShapefileLock);
		sfLock.Lock();
		CPLXMLNode* sfNode = ((CShapefile*)_shapefile)->SerializeCore(VARIANT_FALSE, "ShapefileData", true);
		sfLock.Unlock();
		CPLAddXMLChild(psTree, sfNode);
	}
	return psTree;
}

// *************************************************************
//		Deserialize()
// *************************************************************
STDMETHODIMP COgrLayer::Deserialize(BSTR newVal, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeLayer = CPLGetXMLNode(node, "=OgrLayerClass");
		if (nodeLayer)
		{
			if (DeserializeCore(nodeLayer))
				*retVal = VARIANT_TRUE;
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

// *************************************************************
//		DeserializeCore()
// *************************************************************
bool COgrLayer::DeserializeCore(CPLXMLNode* node)
{
	if (!node) return false;

	Close();

	CString s = CPLGetXMLValue(node, "SourceType", NULL);
	tkOgrSourceType sourceType = (s != "") ? (tkOgrSourceType)atoi(s.GetString()) : ogrUninitialized;

	CStringW connectionString = Utility::ConvertFromUtf8(CPLGetXMLValue(node, "ConnectionString", ""));
	CStringW sourceQuery = Utility::ConvertFromUtf8(CPLGetXMLValue(node, "SourceQuery", ""));

	s = CPLGetXMLValue(node, "ForUpdate", NULL);
	bool forUpdate = (s != "") ? (atoi(s.GetString()) == 0 ? false : true) : false;

	CComBSTR bstrConnection(connectionString);
	CComBSTR bstrQuery(sourceQuery);

	VARIANT_BOOL vb = VARIANT_FALSE;
	if (sourceType == ogrDbTable)
	{
		OpenFromDatabase(bstrConnection, bstrQuery, forUpdate ? VARIANT_TRUE : VARIANT_FALSE, &vb);
	}
	else if (sourceType == ogrQuery)
	{
		OpenFromQuery(bstrConnection, bstrQuery, &vb);
	}
	else if (sourceType == ogrFile)
	{
		// TODO: implement
	}
	
	vb = DeserializeOptions(node) ? VARIANT_TRUE : VARIANT_FALSE;

	return vb == VARIANT_TRUE;
}

// *************************************************************
//		DeserializeOptions()
// *************************************************************
bool COgrLayer::DeserializeOptions(CPLXMLNode* node)
{
	bool success = true;
	_loader.LabelExpression = Utility::ConvertFromUtf8(CPLGetXMLValue(node, "LabelExpression", ""));

	CString s = CPLGetXMLValue(node, "LabelPosition", NULL);
	_loader.LabelPosition = (s != "") ? (tkLabelPositioning)atoi(s.GetString()) : lpNone;

	s = CPLGetXMLValue(node, "LabelOrientation", NULL);
	_loader.LabelOrientation = (s != "") ? (tkLineLabelOrientation)atoi(s.GetString()) : lorParallel;

	s = CPLGetXMLValue(node, "MaxFeatureCount", NULL);
	_loader.SetMaxCacheCount((s != "") ? atoi(s.GetString()) : m_globalSettings.ogrLayerMaxFeatureCount);

	// let's populate data (in case it was populated before serialization)
	if (_sourceType != ogrUninitialized && _layer != NULL)
	{
		CPLXMLNode* psChild = CPLGetXMLNode(node, "ShapefileData");
		if (psChild)
		{
			if (!_shapefile) {
				IShapefile* sf = LoadShapefile();
				CSingleLock sfLock(&_loader.ShapefileLock);
				sfLock.Lock();
				_shapefile = sf;
				sfLock.Unlock();
			}
			bool result = ((CShapefile*)_shapefile)->DeserializeCore(VARIANT_FALSE, psChild);
			if (!result) success = false;
		}
	}

	CString key = CPLGetXMLValue(node, "Key", "");
	if (key.GetLength() > 0)
	{
		SysFreeString(_key);
		_key = A2BSTR(key);
	}
	return success;
}

// *************************************************************
//		get_GdalLastErrorMsg()
// *************************************************************
STDMETHODIMP COgrLayer::get_GdalLastErrorMsg(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CStringW s = OgrHelper::OgrString2Unicode(CPLGetLastErrorMsg());
	*pVal = W2BSTR(s);
	return S_OK;
}

// *************************************************************
//		DynamicLoading()
// *************************************************************
STDMETHODIMP COgrLayer::get_DynamicLoading(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _dynamicLoading;
	return S_OK;
}
STDMETHODIMP COgrLayer::put_DynamicLoading(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_dynamicLoading = newVal;
	if (newVal) {
		ForceCreateShapefile();
	}
	return S_OK;
}

// *************************************************************
//		MaxFeatureCount()
// *************************************************************
STDMETHODIMP COgrLayer::get_MaxFeatureCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _loader.GetMaxCacheCount();
	return S_OK;
}
STDMETHODIMP COgrLayer::put_MaxFeatureCount(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_loader.SetMaxCacheCount(newVal <= 0 ? m_globalSettings.ogrLayerMaxFeatureCount : newVal);
	return S_OK;
}

// *************************************************************
//		HasStyleTable()
// *************************************************************
bool COgrLayer::HasStyleTable()
{
	if (!CheckState()) return false;
	return OgrStyleHelper::HasStyleTable(_dataset, GetLayerName());
}

// *************************************************************
//		GetLayerName()
// *************************************************************
CStringW COgrLayer::GetLayerName()
{
	return OgrHelper::OgrString2Unicode(_layer->GetName());
}

// *************************************************************
//		GetStyleTableName()
// *************************************************************
CStringW COgrLayer::GetStyleTableName()
{
	return OgrStyleHelper::GetStyleTableName(GetLayerName());
}

// *************************************************************
//		SupportsStyles()
// *************************************************************
STDMETHODIMP COgrLayer::get_SupportsStyles(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = VARIANT_FALSE;

	if (!CheckState()) return S_OK;

	if (_sourceType == ogrQuery) 
	{
		ErrorMessage(tkOGR_NO_STYLE_FOR_QUERIES);
		return S_OK;
	}

	if (HasStyleTable()){
		*pVal = VARIANT_TRUE;
		return S_OK;
	}
	
	OgrStyleHelper::CreateStyleTable(_dataset, GetLayerName());
	return S_OK;
}

// *************************************************************
//		SaveStyle()
// *************************************************************
STDMETHODIMP COgrLayer::SaveStyle(BSTR Name, CStringW xml, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	if (!_shapefile) {
		ErrorMessage(tkNO_OGR_DATA_WAS_LOADED);
		return S_OK;
	}

	VARIANT_BOOL vb;
	get_SupportsStyles(&vb);
	if (!vb) {
		return S_OK;
	}

	RemoveStyle(Name, &vb);
	CPLErrorReset();

	CStringW styleName = OLE2W(Name);

	bool result = OgrStyleHelper::SaveStyle(_dataset, xml, GetLayerName(), styleName);
	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		StyleCount()
// *************************************************************
STDMETHODIMP COgrLayer::GetNumStyles(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = -1;

	if (!CheckState()) return S_OK;

	CStringW sql;
	sql.Format(L"SELECT COUNT(*) FROM %s WHERE layername = '%s'", GetStyleTableName(), GetLayerName());

	CPLErrorReset();
	OGRLayer* layer = _dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	if (layer) {
		layer->ResetReading();
		OGRFeature* ft = layer->GetNextFeature();
		if (ft) {
			*pVal = ft->GetFieldAsInteger(0);
			OGRFeature::DestroyFeature(ft);
		}
		_dataset->ReleaseResultSet(layer);
	}
	return S_OK;
}

// *************************************************************
//		StyleName()
// *************************************************************
STDMETHODIMP COgrLayer::get_StyleName(LONG styleIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckState()) return S_OK;

	CStringW sql;
	sql.Format(L"SELECT styleName FROM %s WHERE layername = '%s'", GetStyleTableName(), GetLayerName());

	bool found = false;
	CPLErrorReset();
	OGRLayer* layer = _dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	if (layer) {
		layer->ResetReading();
		OGRFeature* ft = NULL;
		int count = 0;
		while ((ft = layer->GetNextFeature()) != NULL)
		{
			if (count == styleIndex) {
				CStringW name = OgrHelper::OgrString2Unicode(ft->GetFieldAsString(0));
				*pVal = W2BSTR(name);
				found = true;
			}
			count++;
			OGRFeature::DestroyFeature(ft);
		}
		_dataset->ReleaseResultSet(layer);
	}

	if (!found) {
		*pVal = SysAllocString(L"");
	}
	return S_OK;
}


// *************************************************************
//		LoadStyleXML()
// *************************************************************
CStringW COgrLayer::LoadStyleXML(CStringW name)
{
	if (!CheckState()) return L"";
	CSingleLock lock(&_loader.ProviderLock);
	lock.Lock();
	return OgrStyleHelper::LoadStyle(_dataset, GetStyleTableName(), GetLayerName(), name);
}

// *************************************************************
//		ClearStyles()
// *************************************************************
STDMETHODIMP COgrLayer::ClearStyles(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckState() || !HasStyleTable())
	{
		*retVal = VARIANT_TRUE;
		return S_OK;
	}
		
	USES_CONVERSION;
	CStringW sql;
	sql.Format(L"DELETE FROM %s WHERE layername = '%s'", GetStyleTableName(), GetLayerName());

	CPLErrorReset();
	_dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	*retVal = CPLGetLastErrorNo() == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// *************************************************************
//		RemoveStyle()
// *************************************************************
STDMETHODIMP COgrLayer::RemoveStyle(BSTR styleName, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (!CheckState()) return S_OK;

	CStringW name = OLE2W(styleName);
	bool result = OgrStyleHelper::RemoveStyle(_dataset, GetStyleTableName(), GetLayerName(), name);
	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		LabelExpression()
// *************************************************************
STDMETHODIMP COgrLayer::get_LabelExpression(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*pVal = W2BSTR(_loader.LabelExpression);
	return S_OK;
}
STDMETHODIMP COgrLayer::put_LabelExpression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	_loader.LabelExpression = OLE2W(newVal);
	return S_OK;
}

// *************************************************************
//		LabelPosition()
// *************************************************************
STDMETHODIMP COgrLayer::get_LabelPosition(tkLabelPositioning* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _loader.LabelPosition;
	return S_OK;
}
STDMETHODIMP COgrLayer::put_LabelPosition(tkLabelPositioning newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_loader.LabelPosition = newVal;
	return S_OK;
}

// *************************************************************
//		LabelOrientation()
// *************************************************************
STDMETHODIMP COgrLayer::get_LabelOrientation(tkLineLabelOrientation* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _loader.LabelOrientation;
	return S_OK;
}
STDMETHODIMP COgrLayer::put_LabelOrientation(tkLineLabelOrientation newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_loader.LabelOrientation = newVal;
	return S_OK;
}

// *************************************************************
//		GetFieldValues()
// *************************************************************
void COgrLayer::GetFieldValues(OGRFieldType fieldType, BSTR& fieldName, vector<VARIANT*>& values)
{
	if (_sourceType == ogrDbTable || _sourceType == ogrFile) 
	{
		// load only the necessary column
		CStringW sql;
		sql.Format(L"SELECT %s FROM %s;", fieldName, GetLayerName());
		OGRLayer* layer = _dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
		if (layer)
		{
			OgrHelper::GetFieldValues(layer, _layer->GetFeatureCount(), fieldType, values, _globalCallback);
			_dataset->ReleaseResultSet(layer);
		}
	}
	else
	{
		// the column can be a computed one, so have to load feature as a whole
		OgrHelper::GetFieldValues(_layer, _layer->GetFeatureCount(), fieldType, values, _globalCallback);
	}
}

// *************************************************************
//		GenerateCategories()
// *************************************************************
STDMETHODIMP COgrLayer::GenerateCategories(BSTR FieldName, tkClassificationType classificationType, 
				long numClasses, tkMapColor colorStart, tkMapColor colorEnd, 
				tkColorSchemeType schemeType, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	OGRFeatureDefn* fields = _layer->GetLayerDefn();
	int fieldIndex = fields->GetFieldIndex(OgrHelper::Bstr2OgrString(FieldName));
	if (fieldIndex == -1) {
		ErrorMessage(tkOGR_INVALID_FIELD_NAME);
		return S_OK;
	}

	CStringW fid = OgrHelper::OgrString2Unicode(_layer->GetFIDColumn());
	bool hasFid = fid.GetLength() > 0;

	CComPtr<IShapefile> sf = NULL;
	GetData(&sf);
	if (!sf) {
		ErrorMessage(tkOGR_NO_SHAPEFILE);
		return S_OK;
	}

	OGRFieldDefn* fld = fields->GetFieldDefn(fieldIndex);
	OGRFieldType ogrType = fld->GetType();
	FieldType fieldType = OgrHelper::GetFieldType(ogrType);
	
	vector<VARIANT*>* values = new vector<VARIANT*>();
	GetFieldValues(ogrType, FieldName, *values);
	
	long errorCode = tkNO_ERROR;
	USES_CONVERSION;
	vector<CategoriesData>* categories = FieldClassification::GenerateCategories(OLE2A(FieldName), 
									fieldType, *values, classificationType, numClasses, errorCode);
	
	for (size_t i = 0; i < values->size(); i++) {
		VariantClear((*values)[i]);
		delete (*values)[i];
	}
	delete values;

	if (!categories) {
		ErrorMessage(errorCode);
		return S_OK;
	}

	IShapefileCategories* ct = NULL;
	sf->get_Categories(&ct);
	if (ct)
	{
		if (hasFid)
			fieldIndex++;   // there will be the first FID column in the data

		VARIANT_BOOL vb;
		((CShapefileCategories*)ct)->GenerateCore(categories, fieldIndex, classificationType, &vb);
		if (vb) {
			*retVal = VARIANT_TRUE;
		}

		CComPtr<IColorScheme> scheme = NULL;
		ComHelper::CreateInstance(idColorScheme, (IDispatch**)&scheme);
		if (scheme) {
			scheme->SetColors2(colorStart, colorEnd);
			ct->ApplyColorScheme(schemeType, scheme);
		}

		long numShapes;
		sf->get_NumShapes(&numShapes);
		if (numShapes > 0)
		{
			ct->ApplyExpressions();
		}
		ct->Release();
	}
	delete categories;
	return S_OK;
}

