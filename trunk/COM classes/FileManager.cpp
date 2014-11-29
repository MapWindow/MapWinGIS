// FileManager.cpp : Implementation of CFileManager
#include "stdafx.h"
#include "FileManager.h"
#include "GridManager.h"
#include "Grid.h"

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP CFileManager::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(m_key);
	return S_OK;
}
STDMETHODIMP CFileManager::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(m_key);
	USES_CONVERSION;
	m_key = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CFileManager::ErrorMessage(long ErrorCode)
{
	m_lastErrorCode = ErrorCode;
	USES_CONVERSION;
	if( m_globalCallback != NULL && m_lastErrorCode != tkNO_ERROR)
		m_globalCallback->Error( OLE2BSTR(m_key),  A2BSTR(ErrorMsg(m_lastErrorCode) ) );
}

STDMETHODIMP CFileManager::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_lastErrorCode;
	m_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

STDMETHODIMP CFileManager::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

//***********************************************************************/
//*		get/put_GlobalCallback()
//***********************************************************************/
STDMETHODIMP CFileManager::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_globalCallback;
	if( m_globalCallback != NULL ) m_globalCallback->AddRef();
	return S_OK;
}

STDMETHODIMP CFileManager::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&m_globalCallback);
	return S_OK;
}

//****************************************************************
//			get_OpenStrategyCore()
//****************************************************************
tkFileOpenStrategy CFileManager::get_OpenStrategyCore(BSTR Filename)
{
	// shapefile
	VARIANT_BOOL vb;
	get_IsVectorLayer(Filename, &vb);
	if (vb)
	{
		return tkFileOpenStrategy::fosVectorLayer;
	}
	
	// gdal
	CStringW name = OLE2W(Filename);
	GdalSupport support = GdalHelper::TryOpenWithGdal(name);
	if (support != GdalSupport::GdalSupportNone) {
		if (support == GdalSupportRgb)
		{
			return tkFileOpenStrategy::fosRgbImage;
		}
		else
		{
			return GridManager::NeedProxyForGrid(name, m_globalSettings.gridProxyMode) ? tkFileOpenStrategy::fosProxyForGrid : tkFileOpenStrategy::fosDirectGrid;
		}
	}

	// it can be binary grid, handled by our own classes
	USES_CONVERSION;
	GridManager gm;
	DATA_TYPE dType = gm.getGridDataType( W2A(name), USE_EXTENSION );
	if (dType != INVALID_DATA_TYPE)
	{
		return tkFileOpenStrategy::fosProxyForGrid;
	}

	return tkFileOpenStrategy::fosNotSupported;
}

//****************************************************************
//			get_IsSupportedBy()
//****************************************************************
STDMETHODIMP CFileManager::get_IsSupportedBy(BSTR Filename, tkSupportType supportType, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	switch(supportType)
	{
		case stGdal:
			*retVal = GdalHelper::CanOpenWithGdal(OLE2W(Filename)) ? VARIANT_TRUE: VARIANT_FALSE;
			return S_OK;
		case stGdalOverviews:
			*retVal = GdalHelper::SupportsOverviews(OLE2W(Filename), m_globalCallback) ? VARIANT_TRUE : VARIANT_FALSE;
			return S_OK;
	}
	*retVal = VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			get_IsSupported()
//****************************************************************
STDMETHODIMP CFileManager::get_IsSupported(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	tkFileOpenStrategy strategy = get_OpenStrategyCore(Filename);
	*retVal = strategy != fosNotSupported ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			get_LastOpenStrategy()
//****************************************************************
STDMETHODIMP CFileManager::get_LastOpenStrategy(tkFileOpenStrategy* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _lastOpenStrategy;
	return S_OK;
}

//****************************************************************
//			get_LastOpenFilename()
//****************************************************************
STDMETHODIMP CFileManager::get_LastOpenFilename(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = W2BSTR(_lastOpenFilename);
	return S_OK;
}

//****************************************************************
//			get_LastOpenIsSuccess()
//****************************************************************
STDMETHODIMP CFileManager::get_LastOpenIsSuccess(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _lastOpenIsSuccess ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			get_IsRgbImage()
//****************************************************************
STDMETHODIMP CFileManager::get_IsRgbImage(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = VARIANT_FALSE;
	GDALDataset* dt = GdalHelper::OpenDatasetW(OLE2W(Filename));
	if (dt)
	{
		*retVal = GdalHelper::IsRgb(dt) ? VARIANT_TRUE: VARIANT_FALSE;
		GdalHelper::CloseDataset(dt);
	}
	return S_OK;
}

//****************************************************************
//			get_IsGrid()
//****************************************************************
STDMETHODIMP CFileManager::get_IsGrid(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	
	CStringW name = OLE2W(Filename);
	GdalSupport support = GdalHelper::TryOpenWithGdal(name);
	if (support != GdalSupport::GdalSupportNone) {
		*retVal = support != GdalSupportRgb ? VARIANT_TRUE: VARIANT_FALSE;
	}
	else
	{
		// it can be binary grid, handled by our own driver
		USES_CONVERSION;
		GridManager gm;
		DATA_TYPE dType = gm.getGridDataType( W2A(name), USE_EXTENSION );
		if (dType != INVALID_DATA_TYPE)
		{
			*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

//****************************************************************
//			get_IsVectorLayer()
//****************************************************************
STDMETHODIMP CFileManager::get_IsVectorLayer(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = Utility::EndsWith(OLE2W(Filename), L"shp") ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			get_OpenStrategy()
//****************************************************************
STDMETHODIMP CFileManager::get_OpenStrategy(BSTR Filename, tkFileOpenStrategy* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = get_OpenStrategyCore(Filename);
	return S_OK;
}

//****************************************************************
//			get_CanOpenAs()
//****************************************************************
STDMETHODIMP CFileManager::get_CanOpenAs(BSTR Filename, tkFileOpenStrategy strategy, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (strategy == fosDirectGrid)
	{
		// directs grids must be supported by gdal
		USES_CONVERSION;
		*retVal = GdalHelper::CanOpenWithGdal(OLE2W(Filename));
		return S_OK;
	}
		
	tkFileOpenStrategy format = get_OpenStrategyCore(Filename);
	switch(format) {
		case fosNotSupported:
			*retVal = VARIANT_FALSE;
			break;
		case fosRgbImage:
			*retVal = strategy == fosRgbImage || strategy == fosDirectGrid || strategy == fosProxyForGrid;
			break;
		case fosVectorLayer:
			*retVal = strategy == fosVectorLayer ? VARIANT_TRUE : VARIANT_FALSE;
			break;
		case fosProxyForGrid:
		case fosDirectGrid:
			*retVal = strategy == fosProxyForGrid;
			break;
	}
	return S_OK;
}

//****************************************************************
//			Open()
//****************************************************************
STDMETHODIMP CFileManager::Open(BSTR Filename, tkFileOpenStrategy openStrategy, ICallback* callback, IDispatch** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	
	if (callback) {
		put_GlobalCallback(callback);
	}

	_lastOpenIsSuccess = false;
	_lastOpenFilename = Filename;
	_lastOpenStrategy = openStrategy;

	USES_CONVERSION;
	if (!Utility::FileExistsW(OLE2W(Filename)))
	{
		ErrorMessage(tkFILE_NOT_EXISTS);
		return S_FALSE;
	}

	if (openStrategy == fosAutoDetect) {
		openStrategy = get_OpenStrategyCore(Filename);
	}
	
	switch(openStrategy) 
	{
		case fosNotSupported:
			ErrorMessage(tkUNSUPPORTED_FORMAT);
			break;
		case fosVectorLayer:
			OpenShapefile(Filename, NULL, (IShapefile**)retVal);
			break;
		default:
			OpenRaster(Filename, openStrategy, NULL, (IImage**)retVal);
	}
	return S_OK;
}

//****************************************************************
//			OpenShapefile()
//****************************************************************
STDMETHODIMP CFileManager::OpenShapefile(BSTR Filename, ICallback* callback, IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;

	_lastOpenFilename = Filename;
	_lastOpenStrategy = fosVectorLayer;

	if (callback) {
		put_GlobalCallback(callback);
	}

	if (!Utility::FileExistsW(OLE2W(Filename)))
	{
		ErrorMessage(tkFILE_NOT_EXISTS);
		return S_FALSE;
	}

	VARIANT_BOOL vb;
	get_IsVectorLayer(Filename, &vb);
	if (vb) {
		VARIANT_BOOL vb;
		IShapefile* sf = NULL;
		GetUtils()->CreateInstance(idShapefile, (IDispatch**)&sf);
		sf->Open(Filename, m_globalCallback, &vb);
		if (!vb)
		{
			
			sf->get_LastErrorCode(&m_lastErrorCode);
			ErrorMessage(m_lastErrorCode);
			sf->Release();
			sf = NULL;
		}
		else
		{
			_lastOpenIsSuccess = true;
			*retVal = sf;
		}
	}
	return S_OK;
}

//****************************************************************
//			OpenRaster()
//****************************************************************
STDMETHODIMP CFileManager::OpenRaster(BSTR Filename, tkFileOpenStrategy openStrategy, ICallback* callback, IImage** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;

	_lastOpenFilename = Filename;
	_lastOpenStrategy = openStrategy;

	if (callback) {
		put_GlobalCallback(callback);
	}

	if (!Utility::FileExistsW(OLE2W(Filename)))
	{
		ErrorMessage(tkFILE_NOT_EXISTS);
		return S_FALSE;
	}

	VARIANT_BOOL vb;
	if (openStrategy == fosAutoDetect) {
		openStrategy = get_OpenStrategyCore(Filename);
	}

	_lastOpenStrategy = openStrategy;

	switch(openStrategy) 
	{
		case fosNotSupported:
			ErrorMessage(tkUNSUPPORTED_FORMAT);
			break;
		case fosRgbImage:
			{
				IImage* img = NULL;
				GetUtils()->CreateInstance(idImage, (IDispatch**)&img);
				if (img)
				{
					img->Open( Filename, ImageType::USE_FILE_EXTENSION, VARIANT_FALSE, m_globalCallback, &vb );
					if (!vb)
					{
						img->get_LastErrorCode(&m_lastErrorCode);
						ErrorMessage(m_lastErrorCode);
						img->Release();
						img = NULL;
					}
					else
					{
						// check that is is actually RGB image
						img->get_IsRgb(&vb);
						if (!vb) {
							ErrorMessage(tkINVALID_OPEN_STRATEGY);
							img->Close(&vb);
							img->Release();
							img = NULL;
						}
						_lastOpenIsSuccess = true;
						*retVal = img;
					}
				}
			}
			break;
		case fosDirectGrid:
		case fosProxyForGrid:
			{
				IGrid* grid = NULL;
				GetUtils()->CreateInstance(idGrid, (IDispatch**)&grid);
				if (grid)
				{
					// TODO: choose inRam mode
					grid->Open(Filename, GridDataType::UnknownDataType, VARIANT_FALSE, GridFileType::UseExtension, m_globalCallback, &vb);
					if (!vb) {
						grid->get_LastErrorCode(&m_lastErrorCode);
						ErrorMessage(m_lastErrorCode);
						grid->Release();
						grid = NULL;
					}
					else
					{
						PredefinedColorScheme coloring = m_globalSettings.GetGridColorScheme();
						IGridColorScheme* scheme = NULL;
						
						grid->RetrieveOrGenerateColorScheme(tkGridSchemeRetrieval::gsrAuto, 
															tkGridSchemeGeneration::gsgGradient, coloring, &scheme);
						
						tkGridProxyMode mode = openStrategy == fosDirectGrid ? gpmNoProxy : gpmUseProxy;
						
						IImage* img = NULL;
						grid->OpenAsImage(scheme, mode, m_globalCallback, &img);
						
						if (!img) {
							// TODO: perhaps use another mode on failure
							grid->get_LastErrorCode(&m_lastErrorCode);
							ErrorMessage(m_lastErrorCode);
						}
						else {
							_lastOpenIsSuccess = true;
							*retVal = img;
						}
						grid->Close(&vb);
						grid->Release();
					}
				}
			}
			break;
	}
	return S_OK;
}

#pragma region Projection
//****************************************************************
//			get_HasProjection()
//****************************************************************
STDMETHODIMP CFileManager::get_HasProjection(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	// TODO: implement
	return S_OK;
}

//****************************************************************
//			get_GeoProjection()
//****************************************************************
STDMETHODIMP CFileManager::get_GeoProjection(BSTR Filename, IGeoProjection** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	// TODO: implement
	return S_OK;
}

//****************************************************************
//			get_IsSameProjection()
//****************************************************************
STDMETHODIMP CFileManager::get_IsSameProjection(BSTR Filename, IGeoProjection* projection, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	// TODO: implement
	return S_OK;
}
#pragma endregion

//****************************************************************
//			DeleteDatasource()
//****************************************************************
STDMETHODIMP CFileManager::DeleteDatasource(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	return S_OK;
}

//****************************************************************
//			HasGdalOverviews()
//****************************************************************
STDMETHODIMP CFileManager::get_HasGdalOverviews(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = GdalHelper::HasOverviews(OLE2W(Filename)) ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			ClearGdalOverviews()
//****************************************************************
STDMETHODIMP CFileManager::ClearGdalOverviews(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = GdalHelper::RemoveOverviews(OLE2W(Filename)) ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			CreateGdalOverviews()
//****************************************************************
STDMETHODIMP CFileManager::BuildGdalOverviews(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = GdalHelper::BuildOverviewsIfNeeded(OLE2W(Filename), true, m_globalCallback) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			NeedsGdalOverviews()
//****************************************************************
STDMETHODIMP CFileManager::get_NeedsGdalOverviews(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	return S_OK;
}

//****************************************************************
//			RemoveProxyImages()
//****************************************************************
STDMETHODIMP CFileManager::RemoveProxyForGrid(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL isGrid;
	this->get_IsGrid(Filename, &isGrid);
	if (isGrid) {
		*retVal = GridManager::RemoveImageProxy(OLE2W(Filename)) ? VARIANT_TRUE: VARIANT_FALSE;
	}
	*retVal = VARIANT_FALSE;
	return S_OK;
}

//****************************************************************
//			HasValidProxyForGrid()
//****************************************************************
STDMETHODIMP CFileManager::get_HasValidProxyForGrid(BSTR Filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = GridManager::HasValidProxy(OLE2W(Filename));
	return S_OK;
}

//****************************************************************
//			OpenFromDatabase()
//****************************************************************
STDMETHODIMP CFileManager::OpenFromDatabase(BSTR connectionString, BSTR layerNameOrQuery, IOgrLayer** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;
	
	IOgrDatasource* source = NULL;
	GetUtils()->CreateInstance(idOgrDatasource, (IDispatch**)&source);
	VARIANT_BOOL vb;
	source->Open(connectionString, &vb);
	if (!vb)
	{
		long errorCode;
		source->get_LastErrorCode(&errorCode);
		ErrorMessage(errorCode);
		return S_FALSE;
	}
	
	source->GetLayerByName(layerNameOrQuery, VARIANT_FALSE, retVal);
	if (*retVal == NULL)
	{
		source->RunQuery(layerNameOrQuery, retVal);
	}
	source->Close();
	source->Release();
	return S_OK;
}

