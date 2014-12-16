// FileManager.cpp : Implementation of CFileManager
#include "stdafx.h"
#include "FileManager.h"
#include "GridManager.h"
#include "Grid.h"
#include "OgrDatasource.h"
#include "OgrHelper.h"

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP CFileManager::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CFileManager::put_Key(BSTR newVal)
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
void CFileManager::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("FileManager", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

STDMETHODIMP CFileManager::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
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
	*pVal = _globalCallback;
	if( _globalCallback != NULL ) _globalCallback->AddRef();
	return S_OK;
}

STDMETHODIMP CFileManager::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

//****************************************************************
//			get_OpenStrategyCore()
//****************************************************************
tkFileOpenStrategy CFileManager::get_OpenStrategyCore(BSTR Filename)
{
	// shapefile
	CStringW filenameW = OLE2W(Filename);
	if (IsShapefile(Filename))
	{
		return tkFileOpenStrategy::fosVectorLayer;
	}
	
	// GDAL
	GdalSupport support = GdalHelper::TryOpenWithGdal(filenameW);
	if (support != GdalSupport::GdalSupportNone) {
		if (support == GdalSupportRgb)
		{
			return tkFileOpenStrategy::fosRgbImage;
		}
		else
		{
			return GridManager::NeedProxyForGrid(filenameW, m_globalSettings.gridProxyMode) ? tkFileOpenStrategy::fosProxyForGrid : tkFileOpenStrategy::fosDirectGrid;
		}
	}

	// it can be binary grid, handled by our own classes
	USES_CONVERSION;
	GridManager gm;
	DATA_TYPE dType = gm.getGridDataType(W2A(filenameW), USE_EXTENSION);
	if (dType != INVALID_DATA_TYPE)
	{
		return tkFileOpenStrategy::fosProxyForGrid;
	}

	// OGR vector
	tkFileOpenStrategy strategy = fosNotSupported;
	GDALDataset* dt = GdalHelper::OpenOgrDatasetW(filenameW, false);
	if (dt)
	{
		int layerCount = dt->GetLayerCount();
		if (layerCount > 0) {
			strategy = layerCount == 1? fosVectorLayer : fosVectorDatasource;
		}
		dt->Dereference();
		delete dt;
	}
	return strategy;
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
			*retVal = GdalHelper::CanOpenAsGdalRaster(OLE2W(Filename)) ? VARIANT_TRUE: VARIANT_FALSE;
			return S_OK;
		case stGdalOverviews:
			*retVal = GdalHelper::SupportsOverviews(OLE2W(Filename), _globalCallback) ? VARIANT_TRUE : VARIANT_FALSE;
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
	GDALDataset* dt = GdalHelper::OpenRasterDatasetW(OLE2W(Filename));
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
	*retVal = VARIANT_FALSE;

	CStringW filenameW = OLE2W(Filename);
	if (IsShapefile(filenameW)) 
	{
		*retVal = VARIANT_TRUE;
		return S_OK;
	}

	if (GdalHelper::CanOpenAsOgrDataset(filenameW)) {
		*retVal = VARIANT_TRUE;
		return S_OK;
	}
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
		// direct grids must be supported by GDAL
		USES_CONVERSION;
		*retVal = GdalHelper::CanOpenAsGdalRaster(OLE2W(Filename));
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

	CStringW filenameW = OLE2W(Filename);

	_lastOpenIsSuccess = false;
	_lastOpenFilename = filenameW;
	_lastOpenStrategy = openStrategy;

	USES_CONVERSION;
	if (!Utility::FileExistsW(filenameW))
	{
		ErrorMessage(tkFILE_NOT_EXISTS);
		return S_OK;
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
			if (IsShapefile(filenameW)) {
				OpenShapefile(Filename, NULL, (IShapefile**)retVal);
			}
			else {
				OpenVectorLayer(Filename, SHP_NULLSHAPE, VARIANT_FALSE, (IOgrLayer**)retVal);
			}
			break;
		case fosVectorDatasource:
			OpenVectorDatasource(Filename, (IOgrDatasource**)retVal);
			break;
		default:
			OpenRaster(Filename, openStrategy, NULL, (IImage**)retVal);
	}
	return S_OK;
}

//****************************************************************
//			OpenVectorLayer()
//****************************************************************
STDMETHODIMP CFileManager::OpenVectorLayer(BSTR Filename, ShpfileType preferedShapeType, VARIANT_BOOL forUpdate, IOgrLayer** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	CStringW filenameW = OLE2W(Filename);
	_lastOpenFilename = filenameW;
	_lastOpenStrategy = fosVectorLayer;

	CComPtr<IOgrDatasource> ds = NULL;
	ComHelper::CreateInstance(idOgrDatasource, (IDispatch**)&ds);
	if (!ds)  return S_OK;
		
	VARIANT_BOOL vb;
	ds->Open(Filename, &vb);		// error will be reported in the class

	if (!vb) return S_OK;

	int layerCount;
	ds->get_LayerCount(&layerCount);
	if (layerCount == 0)
	{
		ds->Close();
		ErrorMessage(tkOGR_DATASOURCE_EMPTY);
		return S_OK;
	}
	
	IOgrLayer* layer = NULL;
	int layerIndex = 0;
	if (layerCount > 1 || preferedShapeType != SHP_NULLSHAPE)
	{
		// choose layer with proper type (for KML for example)
		layer = OgrHelper::ChooseLayerByShapeType(ds, preferedShapeType, forUpdate);
	}

	if (!layer) {
		ds->GetLayer(0, forUpdate, &layer);		// simply grab the first one
	}

	if (layer) {
		*retVal = layer;
		_lastOpenIsSuccess = true;
	}
	
	ds->Close();
	return S_OK;
}

//****************************************************************
//			OpenShapefile()
//****************************************************************
STDMETHODIMP CFileManager::OpenShapefile(BSTR Filename, ICallback* callback, IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = NULL;

	CStringW filenameW = OLE2W(Filename);
	_lastOpenFilename = filenameW;
	_lastOpenStrategy = fosVectorLayer;

	if (callback) {
		put_GlobalCallback(callback);
	}

	if (!Utility::FileExistsW(filenameW))
	{
		ErrorMessage(tkFILE_NOT_EXISTS);
		return S_OK;
	}

	VARIANT_BOOL vb;
	if (!IsShapefile(filenameW))
		return S_OK;
	
	IShapefile* sf = NULL;
	ComHelper::CreateInstance(idShapefile, (IDispatch**)&sf);
	sf->Open(Filename, _globalCallback, &vb);
	if (!vb)
	{
		sf->get_LastErrorCode(&_lastErrorCode);
		ErrorMessage(_lastErrorCode);
		sf->Release();
	}
	else
	{
		_lastOpenIsSuccess = true;
		*retVal = sf;
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
		return S_OK;
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
				ComHelper::CreateInstance(idImage, (IDispatch**)&img);
				if (img)
				{
					img->Open( Filename, ImageType::USE_FILE_EXTENSION, VARIANT_FALSE, _globalCallback, &vb );
					if (!vb)
					{
						img->get_LastErrorCode(&_lastErrorCode);
						ErrorMessage(_lastErrorCode);
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
				CComPtr<IGrid> grid = NULL;
				ComHelper::CreateInstance(idGrid, (IDispatch**)&grid);
				if (grid)
				{
					// TODO: choose inRam mode
					grid->Open(Filename, GridDataType::UnknownDataType, VARIANT_FALSE, GridFileType::UseExtension, _globalCallback, &vb);
					if (!vb) {
						grid->get_LastErrorCode(&_lastErrorCode);
						ErrorMessage(_lastErrorCode);
					}
					else
					{
						PredefinedColorScheme coloring = m_globalSettings.GetGridColorScheme();
						CComPtr<IGridColorScheme> scheme = NULL;
						
						grid->RetrieveOrGenerateColorScheme(tkGridSchemeRetrieval::gsrAuto, 
															tkGridSchemeGeneration::gsgGradient, coloring, &scheme);
						
						tkGridProxyMode mode = openStrategy == fosDirectGrid ? gpmNoProxy : gpmUseProxy;
						
						IImage* img = NULL;
						grid->OpenAsImage(scheme, mode, _globalCallback, &img);
						
						if (!img) {
							// TODO: perhaps use another mode on failure
							grid->get_LastErrorCode(&_lastErrorCode);
							ErrorMessage(_lastErrorCode);
						}
						else {
							_lastOpenIsSuccess = true;
							*retVal = img;
						}
						grid->Close(&vb);
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
	*retVal = GdalHelper::BuildOverviewsIfNeeded(OLE2W(Filename), true, _globalCallback) ? VARIANT_TRUE : VARIANT_FALSE;
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
	ComHelper::CreateInstance(idOgrDatasource, (IDispatch**)&source);
	VARIANT_BOOL vb;
	source->Open(connectionString, &vb);
	if (!vb)
	{
		long errorCode;
		source->get_LastErrorCode(&errorCode);
		ErrorMessage(errorCode);
		return S_OK;
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

//****************************************************************
//			IsShapefile()
//****************************************************************
bool CFileManager::IsShapefile(CStringW filename)
{
	return Utility::EndsWith(filename, L"shp");
}

//****************************************************************
//			OpenVectorDatasource()
//****************************************************************
STDMETHODIMP CFileManager::OpenVectorDatasource(BSTR Filename, IOgrDatasource** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = NULL;

	CStringW filenameW = OLE2W(Filename);
	_lastOpenFilename = filenameW;
	_lastOpenStrategy = fosVectorLayer;	    // TODO: perhaps fosVectorDatasource is more appropriate

	IOgrDatasource* ds = NULL;
	ComHelper::CreateInstance(idOgrDatasource, (IDispatch**)&ds);
	if (!ds)  return S_OK;

	VARIANT_BOOL vb;
	ds->Open(Filename, &vb);		// error will be reported in the class
	
	if (vb) {
		*retVal = ds;
	}

	return S_OK;
}

//****************************************************************
//			GetFilter()
//****************************************************************
CString CFileManager::GetFilter(OpenFileDialogFilter filter)
{
	switch (filter)
	{
		case FilterImage:
			return "Image Formats|hdr.adf;*.asc;*.bt;*.bil;*.bmp;*.dem;*.ecw;*.img;*.gif;*.map;*.jp2;*.jpg;*.sid;*.pgm;*.pnm;*.png;*.ppm;*.vrt;*.tif;*.ntf|";
		case FilterGrid:
			return "Grid Formats|sta.adf;*.bgd;*.asc;*.tif;????cel0.ddf;*.arc;*.aux;*.pix;*.dem;*.dhm;*.dt0;*.img;*.dt1;*.bil;*.nc|";
		case FilterOgr:
			// TODO: add more formats; this list is just for a start;
			return "Vector formats|*.dgn;*.dxf;*.gml;*.kml;*.mif;*.tab;*.shp|";
		case FilterShapefile:
			return "ESRI Shapefiles (*.shp)|*.shp|";
		case FilterAll:
			return "All files|*.*";
	}
	return "";
}

//****************************************************************
//			get_CdlgFilter()
//****************************************************************
STDMETHODIMP CFileManager::get_CdlgFilter(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString s = GetFilter(FilterShapefile) + 
				GetFilter(FilterOgr) +
				GetFilter(FilterImage) +
				GetFilter(FilterGrid) +
				GetFilter(FilterAll);
	*pVal = A2BSTR(s);
	return S_OK;
}

//****************************************************************
//			get_CdlgRasterFilter()
//****************************************************************
STDMETHODIMP CFileManager::get_CdlgRasterFilter(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString s = GetFilter(FilterImage) +
				GetFilter(FilterGrid) +
				GetFilter(FilterAll);
	*pVal = A2BSTR(s);
	return S_OK;
}

//****************************************************************
//			get_CdlgVectorFilter()
//****************************************************************
STDMETHODIMP CFileManager::get_CdlgVectorFilter(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString s = GetFilter(FilterShapefile) +
				GetFilter(FilterOgr) +
				GetFilter(FilterAll);
	*pVal = A2BSTR(s);
	return S_OK;
}

//****************************************************************
//			get_SupportedGdalFormats()
//****************************************************************
STDMETHODIMP CFileManager::get_SupportedGdalFormats(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CComBSTR bstr("");
	CComBSTR bstrOptions("--formats");
	GetUtils()->GDALInfo(bstr, bstrOptions, NULL, pVal);
	return S_OK;
}
