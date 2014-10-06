#include "stdafx.h"
#include "OgrLayer.h"
#include "OgrHelper.h"
#include "UtilityFunctions.h"
#include "GeoProjection.h"
#include "GeometryConverter.h"
#include "Shapefile.h"

//***********************************************************************
//*		LoadShapefile()
//***********************************************************************
IShapefile* COgrLayer::LoadShapefile()
{ 
	return OgrHelper::Layer2Shapefile(_layer, _globalCallback); 
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
	USES_CONVERSION;
	if (_globalCallback != NULL && _lastErrorCode != tkNO_ERROR)
		_globalCallback->Error(OLE2BSTR(_key), A2BSTR(ErrorMsg(_lastErrorCode)));
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
	Utility::put_ComReference(newVal, (IDispatch**)&_globalCallback);
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
		_shapefile->Release();
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
//		OpenFromDatabase()
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
		_sourceQuery = Utility::ConvertFromUtf8(layer->GetName());
		_dataset = ds;
		_layer = layer;
		_forUpdate = forUpdate == VARIANT_TRUE;
		_sourceType = ogrDbTable;
		
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
		OGRLayer* layer = ds->ExecuteSQL(Utility::Bstr2Char(sql), NULL, NULL);
		if (layer)
		{
			_connectionString = OLE2W(connectionString);
			_sourceQuery = OLE2W(sql);
			_dataset = ds;
			_layer = layer;
			_sourceType = ogrQuery;
			_forUpdate = VARIANT_FALSE;
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
//		OpenByLayerName()
// *************************************************************
STDMETHODIMP COgrLayer::OpenFromDatabase(BSTR connectionString, BSTR layerName, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	Close();

	GDALDataset* ds = OpenDataset(connectionString, forUpdate ? true : false);
	if (ds)
	{
		OGRLayer* layer = ds->GetLayerByName(Utility::Bstr2Char(layerName));
		if (layer)
		{
			_connectionString = OLE2W(connectionString);
			_sourceQuery = OLE2W(layerName);
			_sourceType = ogrDbTable;
			_dataset = ds;
			_layer = layer;
			_forUpdate = forUpdate == VARIANT_FALSE;
			*retVal = VARIANT_TRUE;
			return S_OK;
		}
		else
		{
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
		CString cmnName = _layer->GetFIDColumn();
		Debug::WriteLine("FID column: %s", cmnName);

		CStringW name = Utility::ConvertFromUtf8(_layer->GetName());
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
	
	_dataLoadingLock.Lock();
	if (!_shapefile)
	{
		_shapefile = LoadShapefile();
	}
	_dataLoadingLock.Unlock();

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
	_dataLoadingLock.Lock();
	CloseShapefile();
	_shapefile = LoadShapefile();
	_dataLoadingLock.Unlock();
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
	if (_sourceType == ogrQuery)
	{
		OGRLayer* layer = _dataset->ExecuteSQL(Utility::Bstr2Char(newSql), NULL, NULL);
		if (layer)
		{
			_dataset->ReleaseResultSet(_layer);
			_sourceQuery = OLE2W(newSql);
			_layer = layer;
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
STDMETHODIMP COgrLayer::get_ConnectionString(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = W2BSTR(_connectionString);
	return S_OK;
}

// *************************************************************
//		get_SourceQuery()
// *************************************************************
STDMETHODIMP COgrLayer::get_SourceQuery(BSTR* retVal)
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
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&gp);
	*retVal = gp;

	if (!CheckState()) return S_OK;
	
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
	*retVal = GeometryConverter::GeometryType2ShapeType(_layer->GetGeomType());
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
STDMETHODIMP COgrLayer::get_FidColumnName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (CheckState())
	{
		char* s = const_cast<char*>(_layer->GetFIDColumn());
		*retVal = A2BSTR(s);
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

	*savedCount = OgrHelper::SaveShapefileChanges(_layer, _shapefile, shapeCmnId, saveType, validateShapes ? true : false, _updateErrors);
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
	char* fidName = const_cast<char*>(_layer->GetFIDColumn());
	CComPtr<ITable> table = NULL;
	_shapefile->get_Table(&table);
	long shapeCmnId;
	table->get_FieldIndexByName(A2BSTR(fidName), &shapeCmnId);
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
	*retVal = A2BSTR(_updateErrors[errorIndex].ErrorMsg);
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
	*retVal = _layer->GetFeatureCount(forceLoading == VARIANT_TRUE);
	return S_OK;
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
	
	OGREnvelope env;
	if (_layer->GetExtent(&env, forceLoading == VARIANT_TRUE) == OGRERR_NONE)
	{
		IExtents* ext = NULL;
		GetUtils()->CreateInstance(idExtents, (IDispatch**)&ext);
		ext->SetBounds(env.MinX, env.MinY, 0.0, env.MaxX, env.MaxY, 0.0);
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
	const char* name = _layer->GetGeometryColumn();
	USES_CONVERSION;
	*retVal = A2BSTR(name);
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
	if (!psTree)
	{
		*retVal = A2BSTR("");
	}
	else
	{
		CString str = CPLSerializeXMLTree(psTree);
		CPLDestroyXMLNode(psTree);
		*retVal = A2BSTR(str);
	}
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
	
	if (_shapefile)
	{
		_dataLoadingLock.Lock();
		CPLXMLNode* sfNode = ((CShapefile*)_shapefile)->SerializeCore(VARIANT_FALSE, "ShapefileData");
		_dataLoadingLock.Unlock();
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
	
	VARIANT_BOOL vb = VARIANT_FALSE;
	if (sourceType == ogrDbTable)
	{
		OpenFromDatabase(W2BSTR(connectionString), W2BSTR(sourceQuery), forUpdate ? VARIANT_TRUE: VARIANT_FALSE, &vb);
	}
	else if (sourceType == ogrQuery)
	{
		OpenFromQuery(W2BSTR(connectionString), W2BSTR(sourceQuery), &vb);
	}
	else if (sourceType == ogrFile)
	{
		// TODO: implement
	}
	
	// let's populate data (in case it was populated before serialization)
	if (vb && _sourceType != ogrUninitialized && _layer != NULL)
	{
		CPLXMLNode* psChild = CPLGetXMLNode(node, "ShapefileData");
		if (psChild)
		{
			_shapefile = LoadShapefile();
			bool result = ((CShapefile*)_shapefile)->DeserializeCore(VARIANT_FALSE, psChild);
			if (!result) vb = VARIANT_FALSE;
		}
	}

	CString key = CPLGetXMLValue(node, "Key", "");
	if (key.GetLength() > 0)
	{
		SysFreeString(_key);
		_key = A2BSTR(key);
	}
	return vb == VARIANT_TRUE;
}
