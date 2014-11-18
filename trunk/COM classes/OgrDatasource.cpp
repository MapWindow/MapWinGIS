#include "stdafx.h"
#include "OgrDatasource.h"
#include "OgrHelper.h"
#include "OgrLayer.h"
#include "GeoProjection.h"
#include "OgrConverter.h"
#include "Shapefile.h"
#include "OgrStyle.h"
#include "Shape2Ogr.h"

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
	USES_CONVERSION;
	if (_globalCallback != NULL && _lastErrorCode != tkNO_ERROR)
		_globalCallback->Error(OLE2BSTR(_key), A2BSTR(ErrorMsg(_lastErrorCode)));
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
		Utility::put_ComReference(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *************************************************************
//		Open()
// *************************************************************
STDMETHODIMP COgrDatasource::Open(BSTR connectionString, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Close();
	*retVal = VARIANT_FALSE;

	GDALDataset* ds = GdalHelper::OpenOgrDatasetW(OLE2W(connectionString), false);
	if (!ds)
	{
		// clients should extract last GDAL error
		ErrorMessage(tkFAILED_TO_OPEN_OGR_DATASOURCE);
		return S_FALSE;
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
		GDALClose(_dataset);
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
	if (!CheckState()) return S_FALSE;
	
	IOgrLayer* layer = NULL;
	GetUtils()->CreateInstance(idOgrLayer, (IDispatch**)&layer);

	VARIANT_BOOL vb;
	((COgrLayer*)layer)->OpenFromDatabase(W2BSTR(_connectionString), name, forUpdate, &vb);
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
	*retVal = NULL;
	if (!CheckState()) return S_FALSE;
	if (index < 0 && index >= _dataset->GetLayerCount())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	else
	{
		IOgrLayer* layer = NULL;
		GetUtils()->CreateInstance(idOgrLayer, (IDispatch**)&layer);
		
		VARIANT_BOOL vb;
		((COgrLayer*)layer)->OpenDatabaseLayer(W2BSTR(_connectionString), index, forUpdate, &vb);
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
		GetUtils()->CreateInstance(idOgrLayer, (IDispatch**)&layer);

		VARIANT_BOOL vb;
		((COgrLayer*)layer)->OpenFromQuery(W2BSTR(_connectionString), sql, &vb);
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
		return S_FALSE;
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
	return S_FALSE;
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
	if (!CheckState()) return S_FALSE;
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
	if (!CheckState()) return S_FALSE;
	
	if (shpType == SHP_NULLSHAPE)
	{
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
		return S_FALSE;;
	}

	OGRSpatialReference* ref = projection ? ((CGeoProjection*)projection)->get_SpatialReference() : NULL;

	char** options = ParseLayerCreationOptions(creationOptions);
	bool multiPart = CSLFetchBoolean(options, "MULTI_PART", 1) != 0;

	OGRLayer* layer = _dataset->CreateLayer(OgrHelper::Bstr2OgrString(layerName), ref,
		OgrConverter::ShapeType2GeometryType(shpType, multiPart), options);
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
	if (!CheckState()) return S_FALSE;

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
	if (!CheckState()) return S_FALSE;

	if (!shapefile)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}

	CStringA name = OgrHelper::Bstr2OgrString(newLayerName);
	if (name.GetLength() == 0)
	{
		ErrorMessage(tkINVALID_LAYER_NAME);
		return S_FALSE;
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
		layer->SyncToDisk();
	}

	*retVal = result ? VARIANT_TRUE : VARIANT_FALSE;

	// saving current style as a default one
	if (m_globalSettings.ogrUseStyles) 
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
	((CShapefile*)shapefile)->ClearValidationList();
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
		*errorMessage =W2BSTR(s);
		*retVal = VARIANT_FALSE;
		return S_FALSE;
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
		GDALDriver* driver = _dataset->GetDriver();
		if (driver)
		{
			char* val = const_cast<char*>(driver->GetMetadataItem(GdalHelper::GetMetadataNameString(metadata)));
			*retVal = A2BSTR(val);		// no need to treat it as utf-8: it's in ASCII
			return S_OK;
		}
	}
	*retVal = A2BSTR("");
	return S_FALSE;
}

// *************************************************************
//		get_DriverMetadataCount()
// *************************************************************
STDMETHODIMP COgrDatasource::get_DriverMetadataCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (CheckState())
	{
		GDALDriver* driver = _dataset->GetDriver();
		if (driver)
		{
			char** data = driver->GetMetadata();
			*retVal = CSLCount(data);
			return S_OK;
		}
	}
	*retVal = 0;
	return S_FALSE;
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
		GDALDriver* driver = _dataset->GetDriver();
		if (driver)
		{
			char** data = driver->GetMetadata();
			if (metadataIndex < 0 || metadataIndex >= CSLCount(data))
			{
				ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
				return S_FALSE;
			}
			else
			{
				char* item = const_cast<char*>(CSLGetField(data, metadataIndex));
				*retVal = A2BSTR(item);    // no need to treat it as utf-8: it's in ASCII
				return S_OK;
			}
		}
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