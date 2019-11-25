#include "stdafx.h"
#include "OgrDatasource.h"
#include "OgrHelper.h"
#include "OgrLayer.h"
#include "GeoProjection.h"
#include "OgrConverter.h"
#include "Shapefile.h"
#include "OgrStyle.h"
#include "Shape2Ogr.h"
#include "Templates.h"

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP COgrDatasource::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP COgrDatasource::put_Key(BSTR newVal)
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
void COgrDatasource::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("OgrDatasource", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

STDMETHODIMP COgrDatasource::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

STDMETHODIMP COgrDatasource::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

//***********************************************************************/
//*		get/put_GlobalCallback()
//***********************************************************************/
STDMETHODIMP COgrDatasource::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _globalCallback;
	if (_globalCallback != NULL) _globalCallback->AddRef();
	return S_OK;
}

STDMETHODIMP COgrDatasource::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *************************************************************
//		Open()
// *************************************************************
STDMETHODIMP COgrDatasource::Open(BSTR connectionString, VARIANT_BOOL* retVal)
{
	return Open2(connectionString, VARIANT_FALSE, retVal);
}

// *************************************************************
//		Open2()
// *************************************************************
STDMETHODIMP COgrDatasource::Open2(BSTR connectionString, VARIANT_BOOL forUpdate, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Close();
	*retVal = VARIANT_FALSE;

	GDALDataset* ds = GdalHelper::OpenOgrDatasetW(OLE2W(connectionString), forUpdate ? true : false, true);
	if (!ds)
	{
		// clients should extract last GDAL error
		ErrorMessage(tkFAILED_TO_OPEN_OGR_DATASOURCE);
		return S_OK;
	}
	else
	{
		_connectionString = connectionString;
		_dataset = ds;
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *************************************************************
//		Close()
// *************************************************************
STDMETHODIMP COgrDatasource::Close()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_dataset)
	{
		GdalHelper::CloseSharedOgrDataset(_dataset);
		_dataset = NULL;
	}
	return S_OK;
}

// *************************************************************
//		get_LayerCount()
// *************************************************************
STDMETHODIMP COgrDatasource::get_LayerCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _dataset ? _dataset->GetLayerCount() : 0;
	return S_OK;
}

// *************************************************************
//		GetLayerByName()
// *************************************************************
STDMETHODIMP COgrDatasource::GetLayerByName(BSTR name, VARIANT_BOOL forUpdate, IOgrLayer** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	if (!CheckState()) return S_OK;
	
	IOgrLayer* layer = NULL;
	ComHelper::CreateInstance(idOgrLayer, (IDispatch**)&layer);

	VARIANT_BOOL vb;
	CComBSTR bstrConnection(_connectionString);
	((COgrLayer*)layer)->OpenFromDatabase(bstrConnection, name, forUpdate, &vb);
	if (!vb)
	{
		long errorCode;
		layer->get_LastErrorCode(&errorCode);
		ErrorMessage(errorCode);
		layer->Release();
	}
	else
	{
		*retVal = layer;
	}
	return S_OK;
}

// *************************************************************
//		get_Layer()
// *************************************************************
STDMETHODIMP COgrDatasource::GetLayer(int index, VARIANT_BOOL forUpdate, IOgrLayer** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	return GetLayer2(index, forUpdate, VARIANT_TRUE, retVal);
}

// *************************************************************
//		GetLayer2()
// *************************************************************
STDMETHODIMP COgrDatasource::GetLayer2(LONG index, VARIANT_BOOL forUpdate, VARIANT_BOOL newConnection, IOgrLayer** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = NULL;
	if (!CheckState()) return S_OK;

	if (index < 0 && index >= _dataset->GetLayerCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
	
	IOgrLayer* layer = NULL;
	ComHelper::CreateInstance(idOgrLayer, (IDispatch**)&layer);

	if (newConnection)
	{
		VARIANT_BOOL vb;
		CComBSTR bstrConnection(_connectionString);

		((COgrLayer*)layer)->OpenDatabaseLayer(bstrConnection, index, forUpdate, &vb);
		if (!vb)
		{
			long errorCode;
			layer->get_LastErrorCode(&errorCode);
			ErrorMessage(errorCode);
			layer->Release();
		}
		else
		{
			*retVal = layer;
		}
	}
	else
	{
		((COgrLayer*)layer)->InjectLayer(_dataset, index, _connectionString, forUpdate);
		*retVal = layer;
	}

	return S_OK;
}

// *************************************************************
//		RunQuery()
// *************************************************************
STDMETHODIMP COgrDatasource::RunQuery(BSTR sql, IOgrLayer** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	if (CheckState())
	{
		IOgrLayer* layer = NULL;
		ComHelper::CreateInstance(idOgrLayer, (IDispatch**)&layer);

		VARIANT_BOOL vb;
		CComBSTR bstr(_connectionString);
		((COgrLayer*)layer)->OpenFromQuery(bstr, sql, &vb);
		if (!vb)
		{
			long errorCode;
			layer->get_LastErrorCode(&errorCode);
			ErrorMessage(errorCode);
			layer->Release();
		}
		else
		{
			*retVal = layer;
		}
	}
	return S_OK;
}

// *************************************************************
//		CheckState()
// *************************************************************
bool COgrDatasource::CheckState()
{
	if (!_dataset)
	{
		ErrorMessage(tkOGR_DATASOURCE_UNINITIALIZED);
		return false;
	}
	return true;
}

// *************************************************************
//		GetDriverName()
// *************************************************************
STDMETHODIMP COgrDatasource::get_DriverName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!CheckState())
	{
		*retVal = A2BSTR("");
		return S_OK;
	}
	else
	{
		*retVal = A2BSTR(_dataset->GetDriverName());   // no need to convert from UTF-8: it's in ASCII
		return S_OK;
	}
}

// *************************************************************
//		GetLayerName()
// *************************************************************
STDMETHODIMP COgrDatasource::GetLayerName(int index, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (CheckState())
	{
		if (index >= 0 || index < _dataset->GetLayerCount())
		{
			OGRLayer* layer = _dataset->GetLayer(index);
			if (layer)
			{
				CStringW name = OgrHelper::OgrString2Unicode(layer->GetName());
				*retVal = W2BSTR(name);
				return S_OK;
			}
		}
		else
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
	}
	*retVal = A2BSTR("");
	return S_OK;
}

// *************************************************************
//		DeleteLayer()
// *************************************************************
STDMETHODIMP COgrDatasource::DeleteLayer(int layerIndex, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (CheckState())
	{
		if (layerIndex >= 0 || layerIndex < _dataset->GetLayerCount())
		{
			OGRErr result = _dataset->DeleteLayer(layerIndex);
			*retVal = (result == OGRERR_NONE) ? VARIANT_TRUE : VARIANT_FALSE;
		}
		else
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
	}
	return S_OK;
}

// *************************************************************
//		TestCapability()
// *************************************************************
STDMETHODIMP COgrDatasource::TestCapability(tkOgrDSCapability capability, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;
	int val = _dataset->TestCapability(OgrHelper::GetDsCapabilityString(capability));
	*retVal = val ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		CreateLayer()
// *************************************************************
STDMETHODIMP COgrDatasource::CreateLayer(BSTR layerName, ShpfileType shpType, IGeoProjection* projection, BSTR creationOptions, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;
	
	if (shpType == SHP_NULLSHAPE)
	{
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
		return S_OK;
	}

	OGRSpatialReference* ref = projection ? ((CGeoProjection*)projection)->get_SpatialReference() : NULL;

	char** options = ParseLayerCreationOptions(creationOptions);
	bool multiPart = CSLFetchBoolean(options, "MULTI_PART", 1) != 0;

	OGRLayer* layer = _dataset->CreateLayer(OgrHelper::Bstr2OgrString(layerName), ref,
		OgrConverter::ShapeType2GeometryType(shpType, multiPart), options);

	_dataset->FlushCache();

	CSLDestroy(options);

	*retVal = layer != NULL ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		LayerIndexByName()
// *************************************************************
STDMETHODIMP COgrDatasource::LayerIndexByName(BSTR layerName, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = -1;
	if (!CheckState()) return S_OK;

	CStringA name = OgrHelper::Bstr2OgrString(layerName);
	for (int i = 0; i < _dataset->GetLayerCount(); i++)
	{
		OGRLayer* layer = _dataset->GetLayer(i);
		if (layer && _stricmp(layer->GetName(), name) == 0)
		{
			*retVal = i;
			break;
		}
	}
	return S_OK;
}

// *************************************************************
//		ImportShapefile()
// *************************************************************
STDMETHODIMP COgrDatasource::ImportShapefile(IShapefile* shapefile, BSTR newLayerName, BSTR creationOptions, 
	tkShapeValidationMode validationMode, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	if (!CheckState()) return S_OK;

	if (!shapefile)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	CStringA name = OgrHelper::Bstr2OgrString(newLayerName);
	if (name.GetLength() == 0)
	{
		ErrorMessage(tkINVALID_LAYER_NAME);
		return S_OK;
	}

	if (validationMode != NoValidation)
	{
		IShapeValidationInfo* info = ((CShapefile*)shapefile)->ValidateInputCore(shapefile, "ImportShapefile", "shapefile", VARIANT_FALSE, validationMode, "OgrDatasource");
		if (info)
		{
			tkShapeValidationStatus status;
			info->get_Status(&status);
			info->Release();
			if (status == OperationAborted)
			{
				ErrorMessage(tkOGR_ABORTED_INVALID_SHAPES);
				goto cleaning;
			}
		}
	}

	// creating empty layer
	IGeoProjection* gp = NULL;
	ShpfileType shpType;
	shapefile->get_ShapefileType(&shpType);
	shapefile->get_GeoProjection(&gp);
	OGRSpatialReference* ref = gp ? ((CGeoProjection*)gp)->get_SpatialReference() : NULL;
	
	char** options = ParseLayerCreationOptions(creationOptions);
	bool multiPart = CSLFetchBoolean(options, "MW_MULTI_PART", 1) != 0;
	bool vacuum = CSLFetchBoolean(options, "MW_POSTGIS_VACUUM", 1) != 0;
	bool saveStyles = CSLFetchBoolean(options, "MW_SAVE_STYLE", 1) != 0;

	OGRLayer* layer = _dataset->CreateLayer(name, ref, OgrConverter::ShapeType2GeometryType(shpType, multiPart), options);
	
	gp->Release();
	CSLDestroy(options);

	if (!layer)
	{
		ErrorMessage(tkFAILED_TO_CREATE_OGR_LAYER);
		goto cleaning;
	}

	// copying fields and shapes
	bool result = Shape2Ogr::Shapefile2OgrLayer(shapefile, layer, m_globalSettings.saveOgrLabels, _globalCallback);
	if (result)
	{
		OGRErr err = layer->SyncToDisk();
		if (err != OGRERR_NONE)
		{
			CallbackHelper::ErrorMsg("Failed to synchronize imported layer to disk / database.");
		}
	}

	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;

	// saving current style as a default one
	if (saveStyles)
	{
		CStringW layerName = OLE2W(newLayerName);
		if (OgrStyleHelper::SupportsStyles(_dataset, layerName)) 
		{
			OgrStyleHelper::RemoveStyle(_dataset, OgrStyleHelper::GetStyleTableName(layerName), layerName, "" );
			OgrStyleHelper::SaveStyle(_dataset, shapefile, layerName, "");
		}
	}

	// run vacuum command for PostGis unless user canceled it explicitly
	if (vacuum)
	{
		if (OgrHelper::IsPostGisDatasource(_dataset))
		{
			// if it fails just ignore it as it's not critical
			name = "VACUUM FULL ANALYZE " + name;
			_dataset->ExecuteSQL(name, NULL, NULL);
		}
	}

cleaning:
	return S_OK;
}

// *************************************************************
//		ExecuteSQL()
// *************************************************************
STDMETHODIMP COgrDatasource::ExecuteSQL(BSTR sql, BSTR* errorMessage, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CPLErrorReset();
	
	OGRLayer* lyr = _dataset->ExecuteSQL(OgrHelper::Bstr2OgrString(sql), NULL, NULL);
	if (lyr)_dataset->ReleaseResultSet(lyr);

	if (CPLGetLastErrorNo() != OGRERR_NONE)
	{
		CStringW s = OgrHelper::OgrString2Unicode(CPLGetLastErrorMsg());
		*errorMessage = W2BSTR(s);
		*retVal = VARIANT_FALSE;
		return S_OK;
	}
	else
	{
		*errorMessage = A2BSTR("");
		*retVal = VARIANT_TRUE;
		return S_OK;
	}
}

// *************************************************************
//		ParseLayerCreationOptions()
// *************************************************************
char** COgrDatasource::ParseLayerCreationOptions(BSTR creationOptions)
{
	USES_CONVERSION;
	CString options = OLE2A(creationOptions);
	char** list = NULL;
	int pos = 0;
	do
	{
		CString s = options.Tokenize(";", pos);
		list = CSLAddString(list, s);
	} while (pos < options.GetLength() - 1);
	return list;
}

// *************************************************************
//		get_DriverMetadata()
// *************************************************************
STDMETHODIMP COgrDatasource::get_DriverMetadata(tkGdalDriverMetadata metadata, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	if (CheckState())
	{
		CString s = GdalHelper::GetDriverMetadata(_dataset, metadata);
		*retVal = A2BSTR(s);		// no need to treat it as UTF-8: it's in ASCII
	}

	*retVal = A2BSTR("");
	return S_OK;
}

// *************************************************************
//		get_DriverMetadataCount()
// *************************************************************
STDMETHODIMP COgrDatasource::get_DriverMetadataCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*retVal = CheckState() ? GdalHelper::get_DriverMetadataCount(_dataset) : 0;

	return S_OK;
}

// *************************************************************
//		get_DriverMetadata()
// *************************************************************
STDMETHODIMP COgrDatasource::get_DriverMetadataItem(int metadataIndex, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	if (CheckState())
	{
		CString s = GdalHelper::get_DriverMetadataItem(_dataset, metadataIndex);
		*retVal = A2BSTR(s);
	}

	*retVal = A2BSTR("");
	return S_OK;
}

// *************************************************************
//		get_GdalLastErrorMsg()
// *************************************************************
STDMETHODIMP COgrDatasource::get_GdalLastErrorMsg(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CStringW s = OgrHelper::OgrString2Unicode(CPLGetLastErrorMsg());
	*pVal = W2BSTR(s);
	return S_OK;
}

// *************************************************************
//		GetSchemas()
// *************************************************************
CStringW COgrDatasource::GetSchemaSql()
{
	CStringA driver = _dataset->GetDriverName();
	if (driver.CompareNoCase("PostgreSQL") == 0)
	{
		return L"select * from pg_namespace; ";
	}
	else if (driver.CompareNoCase("MSSqlSpatial") == 0)
	{
		return L"SELECT name FROM sys.schemas";
	}
	
	Debug::WriteError("Schema retrieval isn't supported for this driver.");
	return L"";
	
}

// *************************************************************
//		GetSchemas()
// *************************************************************
STDMETHODIMP COgrDatasource::GetSchemas(VARIANT* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CStringW sql = GetSchemaSql();
	if (sql.GetLength() == 0)
	{
		VariantInit(retVal);
		retVal->vt = VT_NULL;
		return S_OK;
	}

	OGRLayer* layer = _dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	
	if (!layer)
	{
		VariantInit(retVal);
		retVal->vt = VT_NULL;
		return S_OK;
	}

	vector<BSTR> schemas;
	do 
	{
		OGRFeature* ft = layer->GetNextFeature();
		if (ft) {
			const char* s = ft->GetFieldAsString(0);
			if (s) {
				BSTR bstr = W2BSTR(OgrHelper::OgrString2Unicode(s));
				schemas.push_back(bstr);
			}
			OGRFeature::DestroyFeature(ft);
		}
		else {
			break;
		}
	} while (1);
	

	_dataset->ReleaseResultSet(layer);

	Templates::Vector2SafeArray(&schemas, VT_BSTR, retVal);

	//for (size_t i = 0; i < schemas.size(); i++)
	//{
	//	SysFreeString(schemas[i]);
	//}

	return S_OK;
}
