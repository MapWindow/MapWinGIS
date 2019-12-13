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
#include "Templates.h"
#include "ShapefileHelper.h"
#include "TableClass.h"
#include "TableHelper.h"

// *************************************************************
//		InjectShapefile()
// *************************************************************
void COgrLayer::InjectShapefile(IShapefile* sfNew)
{ // Lock shape file
	CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
	CloseShapefile();
	_shapefile = sfNew;
}

// *************************************************************
//		InitOpenedLayer()
// *************************************************************
void COgrLayer::InitOpenedLayer()
{
	if (Utility::FileExistsW(_connectionString)) {
		_sourceType = ogrFile;
	}

	ClearCachedValues();
	VARIANT_BOOL vb;

	// Let's cache some values, as the driver can be busy with background loading
	// later on, so the value may not be readily available
	int featureCount;
	get_FeatureCount(VARIANT_FALSE, &featureCount);
	
	CComPtr<IExtents> extents = NULL;
	get_Extents(&extents, VARIANT_TRUE, &vb);

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

	if (_featureCount != -1) {
		_featureCount = -1;
	}
}

//***********************************************************************
//*		StopBackgroundLoading()
//***********************************************************************
void COgrLayer::StopBackgroundLoading()
{
	_loader.CancelAllTasks();  // notify working thread that it's time is over
	CSingleLock lock(&_loader.LoadingLock, _dynamicLoading ? TRUE : FALSE);
}

//***********************************************************************
//*		LoadShapefile()
//***********************************************************************
IShapefile* COgrLayer::LoadShapefile()
{ 
	bool isTrimmed = false;	
	IShapefile* sf = Ogr2Shape::Layer2Shapefile(_layer, _activeShapeType, _loader.GetMaxCacheCount(), isTrimmed, &_loader, _globalCallback); 
	if (isTrimmed) {
		ErrorMessage(tkOGR_LAYER_TRIMMED);
	}
	return sf;
}

//***********************************************************************
//*		UpdateShapefileFromOGRLoader()
//***********************************************************************
void COgrLayer::UpdateShapefileFromOGRLoader()
{
    CSingleLock lock(&_loader.ProviderLock, TRUE);
    if (!_shapefile) return;

    // Wait for tasks to finish loading:
    _loader.AwaitTasks();

    // Lock it all down:
    CSingleLock ldLock(&_loader.LoadingLock, TRUE);
    CSingleLock prLock(&_loader.ProviderLock, TRUE);
    CSingleLock sfLock(&_loader.ShapefileLock, TRUE);

    // Grab the loaded data:
    vector<ShapeRecordData*> data = _loader.FetchData();
    if (data.size() == 0) return;

    VARIANT_BOOL vb;
    _shapefile->EditClear(&vb);

    ShpfileType shpType;
    _shapefile->get_ShapefileType(&shpType);

    Debug::WriteWithThreadId(Debug::Format("Update shapefile: %d\n", data.size()), DebugOgrLoading);

    CComPtr<ITable> table = NULL;
    _shapefile->get_Table(&table);

    CComPtr<ILabels> labels = NULL;
    _shapefile->get_Labels(&labels);
    labels->Clear();

    if (table)
    {
        CTableClass* tbl = TableHelper::Cast(table);
        _shapefile->StartEditingShapes(VARIANT_TRUE, NULL, &vb);
        long count = 0;
        for (size_t i = 0; i < data.size(); i++)
        {
            CComPtr<IShape> shp = NULL;
            ComHelper::CreateShape(&shp);
            if (shp)
            {
                shp->Create(shpType, &vb);
                shp->ImportFromBinary(data[i]->Shape, &vb);
                _shapefile->EditInsertShape(shp, &count, &vb);

                tbl->UpdateTableRow(data[i]->Row, count);
                data[i]->Row = NULL;   // we no longer own it; it'll be cleared by Shapefile.EditClear

                count++;
            }
        }
        // inserted shapes were marked as modified, correct this
        ShapefileHelper::ClearShapefileModifiedFlag(_shapefile);

        // Stop 'fake' editing session
        _shapefile->StopEditingShapes(VARIANT_TRUE, VARIANT_TRUE, NULL, &vb);

        // Without this, categories are not correctly applied in the drawing function:
        IShapefileCategories* cat;
        _shapefile->get_Categories(&cat);
        cat->ApplyExpressions();
    }

    // clean the data
    for (size_t i = 0; i < data.size(); i++) {
        delete data[i];
    }
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
	CallbackHelper::ErrorMsg("OgrLayer", _globalCallback, _key, ErrorMsg(_lastErrorCode));
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
		if (_sourceType == ogrQuery && _layer) {
			_dataset->ReleaseResultSet(_layer);
		}

		if (m_globalSettings.ogrShareConnection)
		{
			GdalHelper::CloseSharedOgrDataset(_dataset);
		}
		else
		{
			if (!_externalDatasource)
			{
				// this will release memory for table layer as well
				GDALClose(_dataset);
			}
		}

		_dataset = NULL;
		_layer = NULL;
	}

	CloseShapefile();
	_updateErrors.clear();
	_sourceType = ogrUninitialized;
	_connectionString = L"";
	_sourceQuery = L"";
	_forUpdate = false;
	_activeShapeType = SHP_NULLSHAPE;
	_externalDatasource = VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		CloseShapefile()
// *************************************************************
void COgrLayer::CloseShapefile()
{ // Lock shape file
	CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
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
	GDALDataset* ds = GdalHelper::OpenOgrDatasetW(OLE2W(connectionString), forUpdate, true);
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

	GDALDataset* ds = OpenDataset(connectionString, forUpdate ? true : false);

	*retVal = OpenDatabaseLayerCore(ds, OLE2W(connectionString), layerIndex, forUpdate, VARIANT_FALSE) ? VARIANT_TRUE : VARIANT_FALSE;

	if (!(*retVal))
	{
		GdalHelper::CloseSharedOgrDataset(ds);
	}

	return S_OK;
}

// *************************************************************
//		OpenDatabaseLayerCore()
// *************************************************************
bool COgrLayer::OpenDatabaseLayerCore(GDALDataset* ds, CStringW connectionString, int layerIndex, VARIANT_BOOL forUpdate, VARIANT_BOOL externalDatasource)
{
	Close();

	if (!ds) {
		return false;
	}

	if (layerIndex < 0 || layerIndex >= ds->GetLayerCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}

	OGRLayer* layer = ds->GetLayer(layerIndex);
	if (!layer)
	{
		ErrorMessage(tkFAILED_TO_OPEN_OGR_LAYER);
		return S_OK;
	}

	USES_CONVERSION;
	_connectionString = connectionString;
	_sourceQuery = OgrHelper::OgrString2Unicode(layer->GetName());
	_dataset = ds;
	_layer = layer;
	_forUpdate = forUpdate == VARIANT_TRUE;
	_sourceType = ogrDbTable;
	_externalDatasource = externalDatasource;

	InitOpenedLayer();

	return true;
}

// *************************************************************
//		InjectLayer()
// *************************************************************
bool COgrLayer::InjectLayer(GDALDataset* ds, int layerIndex, CStringW connection, VARIANT_BOOL forUpdate)
{
	return OpenDatabaseLayerCore(ds, connection, layerIndex, forUpdate, VARIANT_TRUE);
}

// *************************************************************
//		ExtendFromQuery()
// *************************************************************
STDMETHODIMP COgrLayer::ExtendFromQuery(BSTR sql, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = VARIANT_FALSE;

    GDALDataset* ds = OpenDataset(W2BSTR(_connectionString), false);
    if (!ds) {
        ErrorMessage(tkOGR_QUERY_FAILED);
        return S_OK;
    }
    
    OGRLayer* layer = ds->ExecuteSQL(OgrHelper::Bstr2OgrString(sql), NULL, NULL);
    if (!layer)
    {
        ErrorMessage(tkOGR_QUERY_FAILED);
        GdalHelper::CloseSharedOgrDataset(ds);
        return S_OK;
    }

    Ogr2Shape::ExtendShapefile(layer, _shapefile, true, _globalCallback);
    GdalHelper::CloseSharedOgrDataset(ds);
    *retVal = VARIANT_TRUE;
    return S_OK;
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
			GdalHelper::CloseSharedOgrDataset(ds);
		}
	}
	return S_OK;
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
			GdalHelper::CloseSharedOgrDataset(ds);
		}
	}
	return S_OK;
}

// *************************************************************
//		OpenFromFile()
// *************************************************************
STDMETHODIMP COgrLayer::OpenFromFile(BSTR Filename, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;
	Close();

	GDALDataset* ds = OpenDataset(Filename, forUpdate ? true : false);
	if (ds)
	{
		OGRLayer* layer = ds->GetLayer(0);
		if (layer)
		{
			_connectionString = OLE2W(Filename);
			_sourceQuery = OgrHelper::OgrString2Unicode(layer->GetName());
			_sourceType = ogrFile;
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
			GdalHelper::CloseSharedOgrDataset(ds);
		}
	}
	return S_OK;
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
		return S_OK;
	}
	else
	{
		CStringW name = OgrHelper::OgrString2Unicode(_layer->GetName());
		*retVal = W2BSTR(name);
		return S_OK;
	}
}

// *************************************************************
//		GetBuffer()
// *************************************************************
STDMETHODIMP COgrLayer::GetBuffer(IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*retVal = NULL;
	if (!CheckState()) return S_OK;
	
	// Lock shape file
	CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
	if (!_shapefile)
	{
		if (_dynamicLoading)
		{ // Lock provider
			CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);
			_shapefile = Ogr2Shape::CreateShapefile(_layer, _activeShapeType);
		}
		else {
			_shapefile = LoadShapefile();
	}
	}

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
	if (!CheckState()) return S_OK;

	if (_dynamicLoading) 
	{
		ErrorMessage(tkNOT_ALLOWED_IN_OGR_DYNAMIC_MODE);
		return S_OK;		
	}
	
	// Lock shape file
	CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);

    // prior to close, see if we can save visibility flags
    VARIANT_BOOL hasOgrFidMapping, isSelectable;
    map<long, BYTE> visibilityFlags;
    bool haveFlags = false;
    // it is required to have an OGR_FID field mapping since Shape ID's 
    // are not guaranteed to be the same following the reload
    _shapefile->get_HasOgrFidMapping(&hasOgrFidMapping);
    if (hasOgrFidMapping == VARIANT_TRUE)
    {
        // get 'selectable' attribute
        _shapefile->get_Selectable(&isSelectable);
        // get relevant visibility flags
        haveFlags = (dynamic_cast<CShapefile*>(_shapefile))->GetVisibilityFlags(visibilityFlags);
    }

    // close and reload Shapefile
	CloseShapefile();
	_shapefile = LoadShapefile();

    // try to restore visibility flags
    if (hasOgrFidMapping == VARIANT_TRUE && haveFlags)
    {
        // restore 'selectable' attribute
        _shapefile->put_Selectable(isSelectable);
        // restore relevant visibility flags
        (dynamic_cast<CShapefile*>(_shapefile))->SetVisibilityFlags(visibilityFlags);
    }

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
	if (!CheckState()) return S_OK;

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
			return S_OK;
		}
	}
	else
	{
		ErrorMessage(tkUNEXPECTED_OGR_SOURCE_TYPE);
		return S_OK;
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

	// Locking provider here
	CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE: FALSE);

	OGRSpatialReference* sr = _layer->GetSpatialRef();		// owned by OGRLayer
	if (sr) ((CGeoProjection*)gp)->InjectSpatialReference(sr);
	return S_OK;
}

// *************************************************************
//		get_ShapeType()
// *************************************************************
STDMETHODIMP COgrLayer::get_ShapeType(ShpfileType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = SHP_NULLSHAPE;
	if (!CheckState()) return S_OK;
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
	*pVal = ShapeUtility::Convert2D(*pVal);
	return S_OK;
}

// *************************************************************
//		get_DataIsReprojected()
// *************************************************************
STDMETHODIMP COgrLayer::get_DataIsReprojected(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;
	if (!_shapefile) return S_OK;		// data wasn't loaded yet

	CComPtr<IGeoProjection> gp = NULL;
	get_GeoProjection(&gp);
	if (!gp) return S_OK;

	CComPtr<IShapefile> sf = NULL;
	GetBuffer(&sf);
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
	return S_OK;
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

	if (!CheckState()) return S_OK;
	if (!_shapefile)
	{
		ErrorMessage(tkNO_OGR_DATA_WAS_LOADED);
		return S_OK;
	}
	
	VARIANT_BOOL hasChanges;
	HasLocalChanges(&hasChanges);
	if (!hasChanges)
	{
		ErrorMessage(tkOGR_NO_MODIFICATIONS);
		return S_OK;
	}

	*retVal = osrNoneSaved;
	long shapeCmnId = GetFidForShapefile();
	if (shapeCmnId == -1)
	{
		ErrorMessage(tkFID_COLUMN_NOT_FOUND);
		return S_OK;
	}

	VARIANT_BOOL reprojected;
	get_DataIsReprojected(&reprojected);
	if (reprojected && saveType != tkOgrSaveType::ostAttributesOnly)
	{
		ErrorMessage(tkCANT_SAVE_REPROJECTED_GEOMETRIES);
		return S_OK;
	}

	{ // Locking provider & shapefile here
		CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);
		CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
		*savedCount = Shape2Ogr::SaveShapefileChanges(_layer, _shapefile, shapeCmnId, saveType, validateShapes ? true : false, _updateErrors);
	}

	HasLocalChanges(&hasChanges);

	*retVal = (*savedCount == 0) ? osrNoneSaved : osrSomeSaved;
	
	if (!hasChanges) {
		*retVal = osrAllSaved;
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

	if (!CheckState() || !_shapefile) return S_OK;

	{ // Locking provider & shapefile here
		CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);
		CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);

		long numShapes;
		_shapefile->get_NumShapes(&numShapes);

		int featureCount = static_cast<int>(_layer->GetFeatureCount());
		if (numShapes != featureCount)	   // i.e. deleted or inserted feature
		{
			*retVal = VARIANT_TRUE;
			return S_OK;
		}

		CComPtr<ITable> table = NULL;
		_shapefile->get_Table(&table);

		long numFields = ShapefileHelper::GetNumFields(_shapefile);
		long sourceFieldCount = _layer->GetLayerDefn()->GetFieldCount();

		// removed fields
		if (sourceFieldCount > numFields - 1)
		{
			// if there are both removed and created fields, the total
			// count may remain the same, but the will be modified records
			*retVal = VARIANT_TRUE;
			return S_OK;
		}

		// modified fields
		VARIANT_BOOL modified, rowModified;

		for (long i = 1; i < numFields; i++)
		{
			CComPtr<IField> field = NULL;
			_shapefile->get_Field(i, &field);
			field->get_Modified(&modified);

			if (modified) {
				*retVal = VARIANT_TRUE;
				return S_OK;
			}
		}

		// modified rows and shapes
		for (long i = 0; i < numShapes; i++)
		{
			_shapefile->get_ShapeModified(i, &modified);
			table->get_RowIsModified(i, &rowModified);

			if (modified || rowModified)
			{
				*retVal = VARIANT_TRUE;
				break;
			}
		}

		return S_OK;
	}
}

// *************************************************************
//		GetFidMapping()
// *************************************************************
long COgrLayer::GetFidForShapefile()
{
	if (!_layer || !_shapefile) return -1;
	// Locking shapefile here
	CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
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
	if (!CheckState()) return S_OK;
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
		return S_OK;
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
		return S_OK;
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
	
	if (!CheckState()) return S_OK;

	{ // Locking provider here
		CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);

	if (_featureCount == -1 || forceLoading)
	{
		_featureCount = static_cast<int>(_layer->GetFeatureCount(forceLoading == VARIANT_TRUE));
	}
	*retVal = _featureCount;
	return S_OK;
}
}


// *************************************************************
//		ForceCreateShapefile()
// *************************************************************
void COgrLayer::ForceCreateShapefile()
{
	tkOgrSourceType sourceType;
	get_SourceType(&sourceType);

	if (_dynamicLoading && !_shapefile && sourceType != ogrUninitialized) 
	{ // Lock the provider & shapefile
		CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);
		CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
		_shapefile = Ogr2Shape::CreateShapefile(_layer, _activeShapeType);
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
	if (!CheckState()) return S_OK;

	if (_envelope && forceLoading) {
		delete _envelope;
		_envelope = NULL;
	}

	if (!_envelope)
	{ // Locking provider here
		CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);
		_envelope = new OGREnvelope();
		_layer->GetExtent(_envelope, forceLoading == VARIANT_TRUE);
	}
	
	if (_envelope) {
		IExtents* ext = NULL;
		ComHelper::CreateExtents(&ext);
		ext->SetBounds(_envelope->MinX, _envelope->MinY, 0.0, _envelope->MaxX, _envelope->MaxY, 0.0);
		*extents = ext;
		*retVal = VARIANT_TRUE;
		return S_OK;
	}
	
	return S_OK;
}

// *************************************************************
//		get_GeometryColumnName()
// *************************************************************
STDMETHODIMP COgrLayer::get_GeometryColumnName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!CheckState()) return S_OK;
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
	if (!CheckState()) return S_OK;
	
	// is it supported by driver?
	VARIANT_BOOL randomWrite;
	TestCapability(tkOgrLayerCapability::olcRandomWrite, &randomWrite);
	if (!randomWrite)
	{
		ErrorMessage(tkOGR_RANDOM_WRITE_NOT_SUPPORTED);
		return S_OK;
	}
		
	// do we have FID column?
	{ // Locking shapefile here
		CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
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
	}

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

	{ // Lock shape file
		CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);
	if (_shapefile)
	{
		CPLXMLNode* sfNode = ((CShapefile*)_shapefile)->SerializeCore(VARIANT_FALSE, "ShapefileData", true);
		CPLAddXMLChild(psTree, sfNode);
	}
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
	if (sourceType == ogrDbTable || sourceType == ogrFile)
	{
		OpenFromDatabase(bstrConnection, bstrQuery, forUpdate ? VARIANT_TRUE : VARIANT_FALSE, &vb);
	}
	else if (sourceType == ogrQuery)
	{
		OpenFromQuery(bstrConnection, bstrQuery, &vb);
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
		{ // Lock shape file
			CSingleLock sfLock(&_loader.ShapefileLock, _dynamicLoading ? TRUE : FALSE);
            if (!_shapefile) {
                IShapefile * sf = LoadShapefile();
                _shapefile = sf;
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
	CSingleLock lock(&_loader.ProviderLock, _dynamicLoading ? TRUE : FALSE);
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
			OgrHelper::GetFieldValues(layer, static_cast<int>( _layer->GetFeatureCount()), fieldType, values, _globalCallback);
			_dataset->ReleaseResultSet(layer);
		}
	}
	else
	{
		// the column can be a computed one, so have to load feature as a whole
		OgrHelper::GetFieldValues(_layer, static_cast<int>(_layer->GetFeatureCount()), fieldType, values, _globalCallback);
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
	GetBuffer(&sf);
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

// *************************************************************
//		DriverName()
// *************************************************************
STDMETHODIMP COgrLayer::get_DriverName(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!CheckState())
	{
		*pVal = A2BSTR("");
		return S_OK;
	}
	else
	{
		*pVal = A2BSTR(_dataset->GetDriverName());   // no need to convert from UTF-8: it's in ASCII
		return S_OK;
	}
	return S_OK;
}

// *************************************************************
//		AvailableShapeTypes()
// *************************************************************
STDMETHODIMP COgrLayer::get_AvailableShapeTypes(VARIANT* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	vector<int> result;
	if (!CheckState()) 
	{
		// return empty array
	}
	else
	{
		ShpfileType shpType = OgrConverter::GeometryType2ShapeType(_layer->GetGeomType());
		if (shpType != SHP_NULLSHAPE)
		{
			// return a single type
			result.push_back(shpType);
		}
		else
		{
			vector<ShpfileType> shapeTypes;

            // querying the MSSQL datasource for available types doesn't work if
            // we're looking at a query result set rather than an actual table. 
            // instead, we'll let it fall into the 'else' logic, which iterates the
            // local layer instead of the database to determine which types are present.
            CStringW layerName = Utility::ConvertFromUtf8(_layer->GetName());

			if (m_globalSettings.ogrListAllGeometryTypes && 
				OgrHelper::IsMsSqlDatasource(_dataset) &&
                (layerName != "SELECT" && layerName != "UPDATE"))
			{
				GetMsSqlShapeTypes(shapeTypes);
			}
			else
			{
				// read and return all the types
				set<OGRwkbGeometryType> types;
				Ogr2Shape::ReadGeometryTypes(_layer, types, m_globalSettings.ogrListAllGeometryTypes);
				Ogr2Shape::GeometryTypesToShapeTypes(types, shapeTypes);
			}

			for (size_t i = 0; i < shapeTypes.size(); i++)
			{
				result.push_back((int)shapeTypes[i]);
			}
		}
	}

	Templates::Vector2SafeArray(&result, VT_I4, pVal);

	return S_OK;
}

// *************************************************************
//		GetMsSqlGeometryTypes()
// *************************************************************
void COgrLayer::GetMsSqlShapeTypes(vector<ShpfileType>& result)
{
	CStringW cmnName = Utility::ConvertFromUtf8(_layer->GetGeometryColumn());
	CStringW layerName = Utility::ConvertFromUtf8(_layer->GetName());
	CStringW sql;

	sql.Format(L"SELECT DISTINCT [%s].STGeometryType() FROM %s", cmnName, layerName);

	set<ShpfileType> types;

	OGRLayer* lyr = _dataset->ExecuteSQL(Utility::ConvertToUtf8(sql), NULL, NULL);
	if (lyr)
	{
		lyr->ResetReading();

		OGRFeature* ft = NULL;
		while ((ft = lyr->GetNextFeature()) != NULL)
		{
			CStringW s = Utility::ConvertFromUtf8(ft->GetFieldAsString(0));
			ShpfileType shpType = OgrHelper::OgcType2ShapeType(s);

			if (types.find(shpType) == types.end())
			{
				types.insert(shpType);
			}

			OGRFeature::DestroyFeature(ft);
		}

		_dataset->ReleaseResultSet(lyr);
	}

	Templates::SetToVector(types, result);

	if (CPLGetLastErrorNo() != OGRERR_NONE)
	{
		CallbackHelper::ErrorMsg("Failed to retrieve the list of geometry types for MS SQL layer.");
	}
}

// *************************************************************
//		ActiveShapeType()
// *************************************************************
STDMETHODIMP COgrLayer::get_ActiveShapeType(ShpfileType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShpfileType shpType;
	get_ShapeType(&shpType);
	
	*pVal = shpType != SHP_NULLSHAPE ? shpType : _activeShapeType;

	return S_OK;
}

STDMETHODIMP COgrLayer::put_ActiveShapeType(ShpfileType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShpfileType shpType;
	get_ShapeType(&shpType);

    // SHP_NULLSHAPE indicates multiple shape types
	if (shpType == SHP_NULLSHAPE)
	{
        // if re-assigning the active type
        if (_activeShapeType != newVal)
        {
            // clear current buffer to force new
            CloseShapefile();
        }
        // assign active type
        _activeShapeType = newVal;
    }
    else
	{
		CallbackHelper::ErrorMsg("OGR layer has single geometry type. ActiveShapeType provided will be ignored.");
	}

	return S_OK;
}

// *************************************************************
//		IsExternalDatasource()
// *************************************************************
STDMETHODIMP COgrLayer::get_IsExternalDatasource(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _externalDatasource;

	return S_OK;
}

