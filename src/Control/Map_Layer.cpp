// #pragma once
// Implementation of CMapView class (see other files as well)
// The properties and methods common for every layer are stored here.
// TODO: consider creation of Layers class as wrapper for this methods
// In this the members can copied to the new class. In the current class 
// simple redirections will be used. To make m_alllayers, m_activelayers available
// the variables can be initialized with the pointers to underlying data of Layers class

#include <StdAfx.h>
#include "Map.h"
#include "Image.h"
#include "Grid.h"
#include "Shapefile.h"
#include "OgrLayer.h"
#include "ShapefileHelper.h"
#include "OgrHelper.h"
#include "TableHelper.h"
#include "WmsHelper.h"

// ************************************************************
//		GetNumLayers()
// ************************************************************
long CMapView::GetNumLayers()
{
	return _activeLayers.size();
}

// ************************************************************
//		LayerName()
// ************************************************************
BSTR CMapView::GetLayerName(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (IsValidLayer(layerHandle))
	{
		return W2BSTR(_allLayers[layerHandle]->name);
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	const CString result;
	return result.AllocSysString();
}

void CMapView::SetLayerName(LONG layerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (IsValidLayer(layerHandle))
	{
		USES_CONVERSION;
		const WCHAR* buffer = Utility::StringToWideChar(newVal);
		_allLayers[layerHandle]->name = buffer;
		delete[] buffer;
	}
	else
		ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ****************************************************
//		GetLayerDescription()
// ****************************************************
BSTR CMapView::GetLayerDescription(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	Layer* layer = GetLayer(layerHandle);
	if (!layer) {
		const CString result;
		return result.AllocSysString();
	}

	return A2BSTR(layer->description);
}

// ****************************************************
//		SetLayerDescription()
// ****************************************************
void CMapView::SetLayerDescription(LONG layerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (Layer* layer = GetLayer(layerHandle))
	{
		layer->description = newVal;
	}
}

// ************************************************************
//		LayerKey()
// ************************************************************
BSTR CMapView::GetLayerKey(long layerHandle)
{
	USES_CONVERSION;
	if (IsValidLayer(layerHandle))
	{
		return OLE2BSTR(_allLayers[layerHandle]->key);
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	const CString result;
	return result.AllocSysString();
}

void CMapView::SetLayerKey(long layerHandle, LPCTSTR lpszNewValue)
{
	USES_CONVERSION;

	if (IsValidLayer(layerHandle))
	{
		::SysFreeString(_allLayers[layerHandle]->key);
		_allLayers[layerHandle]->key = A2BSTR(lpszNewValue);
	}
	else
		ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ************************************************************
//		LayerPosition()
// ************************************************************
long CMapView::GetLayerPosition(long layerHandle)
{
	if (IsValidLayer(layerHandle))
	{
		const long endcondition = _activeLayers.size();
		for (int i = 0; i < endcondition; i++)
		{
			if (_activeLayers[i] == layerHandle)
				return i;
		}

		return -1;
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	return -1;
}

// ************************************************************
//		LayerHandle()
// ************************************************************
long CMapView::GetLayerHandle(long layerPosition)
{
	// TODO: How to cast _activeLayers.size() to long?
	if (layerPosition >= 0 && layerPosition < (long)_activeLayers.size())
	{
		return _activeLayers[layerPosition];
	}

	ErrorMessage(tkINVALID_LAYER_POSITION);
	return -1;
}

// ************************************************************
//		GetLayerVisible()
// ************************************************************
BOOL CMapView::GetLayerVisible(long layerHandle)
{
	if (IsValidLayer(layerHandle))
	{
		return _allLayers[layerHandle]->get_Visible();
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	return FALSE;
}

// ************************************************************
//		SetLayerVisible()
// ************************************************************
void CMapView::SetLayerVisible(long layerHandle, BOOL bNewValue)
{
	if (IsValidLayer(layerHandle))
	{
		_allLayers[layerHandle]->put_Visible(bNewValue != FALSE);

		// we need to refresh the buffer here
		if (_allLayers[layerHandle]->IsImage())
		{
			if (_allLayers[layerHandle]->get_Object())
			{
				IImage* iimg;
				if (_allLayers[layerHandle]->QueryImage(&iimg))
				{
					//static_cast<CImageClass*>(iimg)->SetBufferReloadIsNeeded();
					dynamic_cast<CImageClass*>(iimg)->SetBufferReloadIsNeeded();
					iimg->Release();
				}
			}
		}

		RedrawCore(RedrawAll, false);
	}
	else
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
}

// ************************************************************
//		GetGetObject()
// ************************************************************
LPDISPATCH CMapView::GetGetObject(long layerHandle)
{
	if (IsValidLayer(layerHandle))
	{
		if (_allLayers[layerHandle]->get_LayerType() == OgrLayerSource)
		{
			// for OGR layers we return underlying shapefile to make it compliant with existing client code
			IShapefile* sf = nullptr;
			_allLayers[layerHandle]->QueryShapefile(&sf);
			return sf;
		}

		IDispatch* obj = _allLayers[layerHandle]->get_Object();
		if (obj != nullptr) obj->AddRef();
		return obj;
	}

	ErrorMessage(tkINVALID_LAYER_HANDLE);
	return nullptr;
}

// ***************************************************************
//		AddLayerFromFilename()
// ***************************************************************
long CMapView::AddLayerFromFilename(LPCTSTR filename, tkFileOpenStrategy openStrategy, VARIANT_BOOL visible)
{
	USES_CONVERSION;
	IDispatch* layer = nullptr;
	const CComBSTR bstr(filename);
	_fileManager->Open(bstr, openStrategy, _globalCallback, &layer);
	if (layer) {
		const long handle = AddLayer(layer, visible);
		layer->Release();
		return handle;
	}

	return -1;
}

// ***************************************************************
//		AddLayerFromDatabase()
// ***************************************************************
long CMapView::AddLayerFromDatabase(LPCTSTR connectionString, LPCTSTR layerNameOrQuery, VARIANT_BOOL visible)
{
	USES_CONVERSION;
	IOgrLayer* layer = nullptr;
	const CComBSTR bstrConnection(connectionString);
	const CComBSTR bstrQuery(layerNameOrQuery);
	_fileManager->OpenFromDatabase(bstrConnection, bstrQuery, &layer);
	if (layer) {
		const long handle = AddLayer(layer, visible);
		layer->Release();
		return handle;
	}
	return -1;
}

// ***************************************************************
//		AddLayerCore()
// ***************************************************************
int CMapView::AddLayerCore(Layer* layer)
{
	int layerHandle = -1;
	for (size_t i = 0; i < _allLayers.size(); i++)
	{
		if (!_allLayers[i])  // that means we can reuse it
		{
			layerHandle = i;
			_allLayers[i] = layer;
			break;
		}
	}

	if (layerHandle == -1)
	{
		layerHandle = _allLayers.size();
		_allLayers.push_back(layer);
	}

	_activeLayers.push_back(layerHandle);
	return layerHandle;
}

// ***************************************************************
//		AttachGlobalCallbackToLayers()
// ***************************************************************
void CMapView::AttachGlobalCallbackToLayers(IDispatch* object)
{
	if (!m_globalSettings.attachMapCallbackToLayers || !_globalCallback) return;

	CComPtr<IShapefile> ishp = nullptr;
	CComPtr<IImage> iimg = nullptr;
	CComPtr<IGrid> igrid = nullptr;
	CComPtr<IOgrLayer> iogr = nullptr;

	object->QueryInterface(IID_IShapefile, reinterpret_cast<void**>(&ishp));
	object->QueryInterface(IID_IImage, reinterpret_cast<void**>(&iimg));
	object->QueryInterface(IID_IGrid, reinterpret_cast<void**>(&igrid));
	object->QueryInterface(IID_IOgrLayer, reinterpret_cast<void**>(&iogr));

	CComPtr<ICallback> callback = nullptr;

	if (igrid) {
		igrid->get_GlobalCallback(&callback);
		if (!callback) {
			igrid->put_GlobalCallback(_globalCallback);
		}
	}

	if (iimg) {
		iimg->put_GlobalCallback(_globalCallback);
		if (!callback) {
			iimg->put_GlobalCallback(_globalCallback);
		}
	}

	if (ishp) {
		ishp->put_GlobalCallback(_globalCallback);
		if (!callback) {
			ishp->put_GlobalCallback(_globalCallback);
		}
	}

	if (iogr)
	{
		iogr->put_GlobalCallback(_globalCallback);
		if (!callback) {
			iogr->put_GlobalCallback(_globalCallback);
		}
	}
}

// ***************************************************************
//		AddLayer()
// ***************************************************************
long CMapView::AddLayer(LPDISPATCH object, BOOL visible)
{
	long layerHandle = -1;

	if (!object)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return layerHandle;
	}

	CComPtr<IOgrDatasource> ds = nullptr;
	object->QueryInterface(IID_IOgrDatasource, (void**)&ds); // TODO: Don't use C-style cast
	if (ds)
	{
		const bool mapIsEmpty = GetNumLayers() == 0;

		const int layerCount = OgrHelper::GetLayerCount(ds);
		if (layerCount == 0)
		{
			ErrorMessage(tkOGR_DATASOURCE_EMPTY);
			return layerHandle;
		}

		LockWindow(lmLock);

		for (int i = 0; i < layerCount; i++)
		{
			CComPtr<IOgrLayer> layer = nullptr;
			ds->GetLayer(i, VARIANT_FALSE, &layer);
			if (layer) {
				layerHandle = AddSingleLayer(layer, visible);		// recursion is here
			}
		}

		if (mapIsEmpty && GetNumLayers() > 0 && m_globalSettings.zoomToFirstLayer)
			ZoomToMaxExtents();

		LockWindow(lmUnlock);

		return layerHandle;   // returning the last layer handle
	}

	return AddSingleLayer(object, visible);
}

// ***************************************************************
//		AddSingleLayer()
// ***************************************************************
long CMapView::AddSingleLayer(LPDISPATCH object, BOOL visible)
{
	long layerHandle = -1;

	IShapefile* ishp = nullptr;
	object->QueryInterface(IID_IShapefile, (void**)&ishp); // TODO: Don't use C-style cast

	IImage* iimg = nullptr;
	object->QueryInterface(IID_IImage, (void**)&iimg); // TODO: Don't use C-style cast

	IGrid* igrid = nullptr;
	object->QueryInterface(IID_IGrid, (void**)&igrid); // TODO: Don't use C-style cast

	IOgrLayer* iogr = nullptr;
	object->QueryInterface(IID_IOgrLayer, (void**)&iogr); // TODO: Don't use C-style cast

	IWmsLayer* iwms = nullptr;
	object->QueryInterface(IID_IWmsLayer, (void**)&iwms); // TODO: Don't use C-style cast

	if (!igrid && !iimg && !ishp && !iogr && !iwms)
	{
		ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		return -1;
	}

	AttachGlobalCallbackToLayers(object);

	LockWindow(lmLock);

	Layer* l = nullptr;

	if (ishp != nullptr || iogr != nullptr)
	{
		if (ishp)
		{
			tkShapefileSourceType type;
			ishp->get_SourceType(&type);
			if (type == sstUninitialized)
			{
				ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
				LockWindow(lmUnlock);
				return -1;
			}

			CComPtr<ITable> table = nullptr;
			ishp->get_Table(&table);
			if (table)
			{
				if (ShapefileHelper::GetNumShapes(ishp) != TableHelper::GetNumRows(table))
					ErrorMessage(tkDBF_RECORDS_SHAPES_MISMATCH);   // report it but allow to proceed
			}
		}

		l = new Layer();

		if (ishp) l->set_Object(ishp);
		else l->set_Object(iogr);

		l->put_LayerType(ishp ? ShapefileLayer : OgrLayerSource);
		l->UpdateExtentsFromDatasource();
		l->put_Visible(visible != FALSE);

		layerHandle = AddLayerCore(l);
	}

	// grids aren't added directly; an image representation is created first 
	// using particular color scheme

	if (igrid != nullptr)
	{
		tkGridSourceType sourceType;
		igrid->get_SourceType(&sourceType);
		if (sourceType == tkGridSourceType::gstUninitialized)
		{
			ErrorMessage(tkGRID_NOT_INITIALIZED);
			LockWindow(lmUnlock);
			return -1;
		}

		//CGrid* grid = static_cast<CGrid*>(igrid);
		const auto grid = dynamic_cast<CGrid*>(igrid);

		CStringW gridFilename = grid->GetFilename();
		CStringW proxyName = grid->GetProxyName();
		//CStringW legendName = grid->GetProxyLegendName();
		CStringW imageName;

		const PredefinedColorScheme coloring = m_globalSettings.GetGridColorScheme();

		CComPtr<IGridColorScheme> gridColorScheme = nullptr;
		igrid->RetrieveOrGenerateColorScheme(gsrAuto, gsgGradient, coloring, &gridColorScheme);
		if (gridColorScheme)
		{
			// there is no proxy; either create a new one or opening directly
			ICallback* cback = nullptr;
			igrid->get_GlobalCallback(&cback);
			tkGridProxyMode displayMode;
			igrid->get_PreferedDisplayMode(&displayMode);
			igrid->OpenAsImage(gridColorScheme, displayMode, cback, &iimg);
			if (cback) cback->Release();

			if (iimg)
			{
				// load transparency, etc
				// update color scheme from disk, as it's the one that is actually used; 
				// and not necessarily the one we passed to Grid.OpenAsImage
				VARIANT_BOOL vb;
				VARIANT_BOOL isProxy;
				iimg->get_IsGridProxy(&isProxy);
				const CStringW legendName = isProxy ? grid->GetProxyLegendName() : grid->GetLegendName();

				CComPtr<IGridColorScheme> newScheme = nullptr;
				ComHelper::CreateInstance(tkInterface::idGridColorScheme, (IDispatch**)&newScheme); // TODO: Don't use C-style cast

				const CComBSTR bstrName(legendName);
				const CComBSTR bstrElementName("GridColoringScheme");
				newScheme->ReadFromFile(bstrName, bstrElementName, &vb);
				if (vb)
				{
					dynamic_cast<CImageClass*>(iimg)->LoadImageAttributesFromGridColorScheme(newScheme);
				}
			}
		}
	}

	// it may be either directly opened image or the one created for the grid
	if (iimg != nullptr)
	{
		tkImageSourceType type;
		iimg->get_SourceType(&type);
		if (type == istUninitialized)
		{
			ErrorMessage(tkIMAGE_UNINITIALIZED);
			LockWindow(lmUnlock);
			return -1;
		}

		l = new Layer();
		l->set_Object(iimg);
		l->put_LayerType(ImageLayer);
		l->put_Visible(visible != FALSE);
		l->UpdateExtentsFromDatasource();

		layerHandle = AddLayerCore(l);

		// try to save pixels in case image grouping is enabled
		if (_canUseImageGrouping)
		{
			if (!dynamic_cast<CImageClass*>(iimg)->SaveNotNullPixels())	// analyzing pixels...
				iimg->put_CanUseGrouping(VARIANT_FALSE);	//  don't try this image any more, before transparency values will be changed
		}
	}

	// WMS layer
	if (iwms != nullptr)
	{
		l = new Layer();
		l->set_Object(iwms);
		l->put_LayerType(WmsLayerSource);
		l->put_Visible(visible != FALSE);

		UpdateWmsLayerBounds(iwms, *l);

		layerHandle = AddLayerCore(l);
	}

	l->GrabLayerNameFromDatasource();

	GrabLayerProjection(l);

	const bool diskSymbology = m_globalSettings.loadSymbologyOnAddLayer && l->IsDiskBased();
	CStringW symbologyName;
	if (diskSymbology)
	{
		// find out filename with symbology before file was substituted by reprojection
		CComBSTR bstrFilename;
		bstrFilename.Attach(GetLayerFilename(layerHandle));
		symbologyName = OLE2W(bstrFilename);
		symbologyName += L".mwsymb";
		symbologyName = Utility::FileExistsW(symbologyName) ? OLE2W(bstrFilename) : L"";
	}

	// do projection mismatch check
	if (!CheckLayerProjection(l, layerHandle))
	{
		RemoveLayerCore(layerHandle, false, false, true);
		LockWindow(lmUnlock);
		return -1;
	}

	if (m_globalSettings.loadSymbologyOnAddLayer)
	{
		// loading symbology
		if (diskSymbology)
		{
			if (symbologyName.GetLength() > 0) {
				CComBSTR desc;
				USES_CONVERSION;
				this->LoadLayerOptionsCore(W2A(symbologyName), layerHandle, "", &desc);
			}
		}
		else {
			LoadOgrStyle(l, layerHandle, L"", false);   // perhaps it's OGR database table
		}
	}

	if (l != nullptr) {
		FireLayerAdded(layerHandle);
	}

	// grid opened event
	if (iimg)
	{
		VARIANT_BOOL rgb, isProxy;
		iimg->get_IsRgb(&rgb);
		iimg->get_IsGridProxy(&isProxy);
		if (!rgb || isProxy)
		{
			int bandIndex = -1;
			CComBSTR bstrName;
			iimg->get_SourceGridName(&bstrName);
			iimg->get_SourceGridBandIndex(&bandIndex);
			USES_CONVERSION;
			FireGridOpened(layerHandle, OLE2A(bstrName), bandIndex, isProxy);
		}
	}

	// set initial extents
	if (l != nullptr && m_globalSettings.zoomToFirstLayer)
	{
		if (_activeLayers.size() == 1 && visible)
		{
			if (!l->IsEmpty())
			{
				SetExtentsWithPadding(l->extents);
			}
		}
	}

	ScheduleLayerRedraw();
	LockWindow(lmUnlock);
	return layerHandle;
}

// ***************************************************************
//		UpdateWmsLayerBounds()
// ***************************************************************
void CMapView::UpdateWmsLayerBounds(IWmsLayer* wms, Layer& layer)
{
	CComPtr<IGeoProjection> gp = nullptr;
	wms->get_GeoProjection(&gp);

	// if there is no projection - no extents either
	VARIANT_BOOL vb;
	if (gp->get_IsEmpty(&vb) == S_OK && vb == VARIANT_TRUE)
	{
		layer.extents = Extent();
		return;
	}

	// extracted bounds in server projection
	Extent projBox;
	if (!WmsHelper::GetServerBounds(wms, projBox)) {
		return;
	}

	// same as map ?
	VARIANT_BOOL isSame;
	IExtents* mapBox = GetExtents();
	gp->get_IsSameExt(_projection, mapBox, 10, &isSame);
	mapBox->Release();

	if (isSame)
	{
		layer.extents = projBox;
	}
	else {
		// need to transform them first
		VARIANT_BOOL vb;
		gp->StartTransform(_projection, &vb);
		if (vb)
		{
			VARIANT_BOOL vb2, vb3;
			gp->Transform(&projBox.left, &projBox.top, &vb2);
			gp->Transform(&projBox.right, &projBox.bottom, &vb3);

			if (vb2 && vb3) {
				layer.extents = projBox;
			}

			// TODO: No need to call StopTransform??
		}
	}
}

// ***************************************************************
//		AdjustWmsLayerVerticalPosition()
// ***************************************************************
/// Moves new WMS layer below all data layers but above existing WMS layers.
void CMapView::AdjustWmsLayerVerticalPosition(int layerHandle)
{
	if (!GetLayer(layerHandle)) return;

	const int position = GetLayerPosition(layerHandle);

	for (long i = 0; i < GetNumLayers(); i++)
	{
		Layer* layer = get_LayerByPosition(i);

		if (!layer || layer->IsEmpty()) continue;

		if (layer->IsWmsLayer()) continue;

		MoveLayer(position, i);
		break;
	}
}

// ***************************************************************
//		LoadOgrStyle()
// ***************************************************************
VARIANT_BOOL CMapView::LoadOgrStyle(Layer* layer, long layerHandle, CStringW name, bool reportError)
{
	VARIANT_BOOL result = VARIANT_FALSE;

	CComPtr<IOgrLayer> ogrLayer = nullptr;
	layer->QueryOgrLayer(&ogrLayer);
	if (!ogrLayer)
		return result;

	if (OgrHelper::GetSourceType(ogrLayer) != ogrDbTable)
		return result;

	const CStringW xml = OgrHelper::Cast(ogrLayer)->LoadStyleXML(name);
	if (xml.GetLength() == 0)
	{
		if (reportError)
			ErrorMessage(tkOGR_STYLE_NOT_FOUND);  // TODO: Should this be an error?
		return result;
	}

	if (CPLXMLNode* root = CPLParseXMLString(Utility::ConvertToUtf8(xml))) {
		if (CPLXMLNode* node = CPLGetXMLNode(root, "Layer")) {
			result = DeserializeLayerOptionsCore(layerHandle, node);
		}
		CPLDestroyXMLNode(root);
	}
	return result;
}

// ***************************************************************
//		GrabLayerProjection()
// ***************************************************************
void CMapView::GrabLayerProjection(Layer* layer)
{
	// if we don't have a projection, let's try and grab projection from it
	if (_grabProjectionFromData && layer)
	{
		VARIANT_BOOL isEmpty = VARIANT_FALSE;
		GetMapProjection()->get_IsEmpty(&isEmpty);
		if (isEmpty)
		{
			if (IGeoProjection* gp = layer->GetGeoProjection())
			{
				gp->get_IsEmpty(&isEmpty);
				if (!isEmpty)
				{
					IGeoProjection* newProj = nullptr;
					gp->Clone(&newProj);

					if (!newProj)
					{
						ErrorMsg(tkFAILED_TO_COPY_PROJECTION);
					}
					else
					{
						SetGeoProjection(newProj);
						newProj->Release();
					}
				}
				gp->Release();
			}
		}
	}
}

// ***************************************************************
//		CheckLayerProjection()
// ***************************************************************
bool CMapView::CheckLayerProjection(Layer* layer, int layerHandle)
{
	IGeoProjection* gpMap = GetMapProjection();

	if (layer->IsWmsLayer())
	{
		// if there is valid map projection we shall be able to do transformation
		if (_transformationMode == tmNotDefined)
		{
			CallbackHelper::ErrorMsg("Can't add WMS layer: map projection if empty or doesn't support required transformation.");
			return false;
		}
	}

	CComPtr<IGeoProjection> gp = nullptr;
	gp.Attach(layer->GetGeoProjection());

	VARIANT_BOOL vb;
	if (gp->get_IsEmpty(&vb) == S_OK && vb == VARIANT_TRUE)
	{
		tkMwBoolean cancel = m_globalSettings.allowLayersWithoutProjection ? blnFalse : blnTrue;
		FireLayerProjectionIsEmpty(layerHandle, &cancel);
		if (cancel == blnTrue) {
			ErrorMessage(tkMISSING_GEOPROJECTION);
			return false;
		}

		// even if user accepted the layer but it clearly doesn't fit we will reject it
		if (gpMap->get_IsGeographic(&vb) == S_OK && vb == VARIANT_TRUE && layer->extents.OutsideWorldBounds())
		{
			ErrorMessage(tkGEOGRAPHIC_PROJECTION_EXPECTED);
			return false;
		}
		return true;
	}

	// makes no sense to do the matching
	if (gpMap->get_IsEmpty(&vb) == S_OK && vb == VARIANT_TRUE) return true;

	// mismatch testing
	CComPtr<IExtents> bounds = nullptr;
	layer->GetExtentsAsNewInstance(&bounds);

	if (gp->get_IsSameExt(gpMap, bounds, 20, &vb) == S_OK && vb == VARIANT_TRUE)
		return true;

	tkMwBoolean cancelAdding = m_globalSettings.allowProjectionMismatch ? blnFalse : blnTrue;
	tkMwBoolean reproject = m_globalSettings.reprojectLayersOnAdding ? blnTrue : blnFalse;
	FireProjectionMismatch(layerHandle, &cancelAdding, &reproject);

	if (cancelAdding)
	{
		ErrorMessage(tkPROJECTION_MISMATCH);
		return false;
	}

	if (!reproject)	return true;	//basically ignore it

	if (layer->IsImage())
	{
		ErrorMessage(tkNO_REPROJECTION_FOR_IMAGES);
		return false;
	}

	if (layer->IsShapefile())
	{
		// save reprojected state
		return ReprojectLayer(layer, layerHandle);
	}

	// for other layer types potentially added in the future
	return true;
}

// ***************************************************************
//		ReprojectLayer()
// ***************************************************************
bool CMapView::ReprojectLayer(Layer* layer, int layerHandle)
{
	// let's try to do a transformation
	CComPtr<IShapefile> sf = nullptr;
	if (!layer->QueryShapefile(&sf))
		return false;

	const long numShapes = ShapefileHelper::GetNumShapes(sf);
	if (numShapes > m_globalSettings.maxReprojectionShapeCount)
	{
		// OGR layers can potentially have millions of features, so let's be cautions not to start something too lengthy
		ErrorMessage(tkREPROJECTION_TOO_MUCH_SHAPES);
		return false;
	}

	long count;
	IShapefile* sfNew = nullptr;
	sf->Reproject(GetMapProjection(), &count, &sfNew);

	// to be considered successful, the new Shapefile (sfNew) must have been allocated AND 
	// either all shapes have been reprojected OR we are allowing layers with incomplete reprojection;
	if (!sfNew || numShapes != count && !m_globalSettings.allowLayersWithIncompleteReprojection)
	{
		// reprojection failed
		FireLayerReprojected(layerHandle, VARIANT_FALSE);
		if (sfNew) sfNew->Release();
		ErrorMessage(tkFAILED_TO_REPROJECT);
		return false;
	}

	// let's substitute original shapefile with the reprojected one
	// don't close the original shapefile; user may still want to interact with it

	if (layer->get_LayerType() == OgrLayerSource)
	{
		CComPtr<IOgrLayer> ogr = nullptr;
		layer->QueryOgrLayer(&ogr);
		if (ogr) {
			// don't need to Release original Shapefile reference here,
			// since InjectShapefile Closes/Releases original reference
			OgrHelper::Cast(ogr)->InjectShapefile(sfNew);
		}
	}
	else
	{
		// need to Release original reference for Shapefile here, 
		// since we are directly replacing old reference with new
		ShapefileHelper::Cast(sf)->Release();
		layer->set_Object(sfNew);
	}
	layer->UpdateExtentsFromDatasource();

	// if not all shapes could be reprojected, let the world know
	if (numShapes != count)
	{
		// in case this was an Ogr layer, event should be fired AFTER new Shapefile was 'injected' into OgrLayerSource
		FireLayerReprojectedIncomplete(layerHandle, count, numShapes);
	}
	// always fire LayerReprojected event
	FireLayerReprojected(layerHandle, VARIANT_TRUE);

	return true;
}

// ***************************************************************
//		RemoveLayerCore()
// ***************************************************************
void CMapView::RemoveLayerCore(long layerHandle, bool closeDatasources, bool fromRemoveAll, bool suppressEvent)
{
	try
	{
		if (!IsValidLayer(layerHandle))
		{
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return;
		}

		const bool hadLayers = _activeLayers.size() > 0;
		if (layerHandle >= (long)_allLayers.size()) return; // TODO: How to cast _activeLayers.size() to long?

		Layer* l = _allLayers[layerHandle];
		if (l == nullptr) return;

		if (closeDatasources) {
			l->CloseDatasources();
		}
		else {
			l->OnRemoved();
		}

		for (unsigned int i = 0; i < _activeLayers.size(); i++)
		{
			if (_activeLayers[i] == layerHandle)
			{
				_activeLayers.erase(_activeLayers.begin() + i);
				break;
			}
		}

		try
		{
			// This may have been deleted already.
			if (_allLayers[layerHandle] != nullptr)
			{
				delete _allLayers[layerHandle];
			}
		}
		catch (...)
		{
			Debug::WriteError("Exception during RemoveLayer.");
		}

		_allLayers[layerHandle] = nullptr;

		_identifiedShapes->RemoveByLayerHandle(layerHandle);

		if (_activeLayers.size() == 0 && hadLayers)
			ClearMapProjectionWithLastLayer();

		if (!suppressEvent)
			FireLayerRemoved(layerHandle, fromRemoveAll ? VARIANT_TRUE : VARIANT_FALSE);

		Redraw();
	}
	catch (...)
	{
		Debug::WriteError("Exception during RemoveLayer.");
	}
}

// ***************************************************************
//		RemoveLayer()
// ***************************************************************
void CMapView::RemoveLayer(long layerHandle)
{
	RemoveLayerCore(layerHandle, true);
}

// ***************************************************************
//		RemoveLayerWithoutClosing()
// ***************************************************************
void CMapView::RemoveLayerWithoutClosing(long layerHandle)
{
	RemoveLayerCore(layerHandle, false);
}

// ***************************************************************
//		RemoveAllLayers()
// ***************************************************************
void CMapView::RemoveAllLayers()
{
	LockWindow(lmLock);
	// Never used: bool hadLayers = _activeLayers.size() > 0;

	for (unsigned int i = 0; i < _allLayers.size(); i++)
	{
		if (IsValidLayer(i))
		{
			RemoveLayerCore(i, true, true);
		}
	}
	_allLayers.clear();

	LockWindow(lmUnlock);

	ClearExtentHistory();

	_activeLayerPosition = 0;

	Redraw();
}

// ***************************************************************
//		MoveLayerUp()
// ***************************************************************
BOOL CMapView::MoveLayerUp(long initialPosition)
{
	if (initialPosition >= 0 && initialPosition < (long)_activeLayers.size())   // TODO: How to cast _activeLayers.size() to long?
	{
		const long layerHandle = _activeLayers[initialPosition];

		_activeLayers.erase(_activeLayers.begin() + initialPosition);

		long newPos = initialPosition + 1;
		if (newPos > (long)_activeLayers.size())  // TODO: How to cast _activeLayers.size() to long?
			newPos = _activeLayers.size();

		_activeLayers.insert(_activeLayers.begin() + newPos, layerHandle);

		Redraw();
		return TRUE;
	}

	ErrorMessage(tkINVALID_LAYER_POSITION);
	return FALSE;
}

// ***************************************************************
//		MoveLayerDown()
// ***************************************************************
BOOL CMapView::MoveLayerDown(long initialPosition)
{
	// TODO: How to cast _activeLayers.size() to long?
	if (initialPosition >= 0 && initialPosition < (long)_activeLayers.size())
	{
		const long layerHandle = _activeLayers[initialPosition];
		_activeLayers.erase(_activeLayers.begin() + initialPosition);

		long newPos = initialPosition - 1;
		if (newPos < 0)
			newPos = 0;

		_activeLayers.insert(_activeLayers.begin() + newPos, layerHandle);

		Redraw();

		return TRUE;
	}

	ErrorMessage(tkINVALID_LAYER_POSITION);
	return FALSE;
}

// ***************************************************************
//		MoveLayer()
// ***************************************************************
BOOL CMapView::MoveLayer(long initialPosition, long targetPosition)
{
	if (initialPosition == targetPosition)
		return TRUE;

	// TODO: How to cast _activeLayers.size() to long?
	if (initialPosition >= 0 && initialPosition < (long)_activeLayers.size() &&
		targetPosition >= 0 && targetPosition < (long)_activeLayers.size())
	{
		const long layerHandle = _activeLayers[initialPosition];

		_activeLayers.erase(_activeLayers.begin() + initialPosition);
		_activeLayers.insert(_activeLayers.begin() + targetPosition, layerHandle);

		Redraw();

		return TRUE;
	}

	ErrorMessage(tkINVALID_LAYER_POSITION);
	return FALSE;
}

// ***************************************************************
//		MoveLayerTop()
// ***************************************************************
BOOL CMapView::MoveLayerTop(long initialPosition)
{
	// TODO: How to cast _activeLayers.size() to long?
	if (initialPosition >= 0 && initialPosition < (long)_activeLayers.size())
	{
		const long layerHandle = _activeLayers[initialPosition];
		_activeLayers.erase(_activeLayers.begin() + initialPosition);
		_activeLayers.push_back(layerHandle);

		Redraw();

		return TRUE;
	}

	ErrorMessage(tkINVALID_LAYER_POSITION);
	return FALSE;
}

// ***************************************************************
//		MoveLayerBottom()
// ***************************************************************
BOOL CMapView::MoveLayerBottom(long initialPosition)
{
	// TODO: How to cast _activeLayers.size() to long?
	if (initialPosition >= 0 && initialPosition < (long)_activeLayers.size())
	{
		const long layerHandle = _activeLayers[initialPosition];
		_activeLayers.erase(_activeLayers.begin() + initialPosition);
		_activeLayers.push_front(layerHandle);

		Redraw();

		return TRUE;
	}

	ErrorMessage(tkINVALID_LAYER_POSITION);
	return FALSE;
}

// ***************************************************************
//		ReSourceLayer()
// ***************************************************************
void CMapView::ReSourceLayer(long layerHandle, LPCTSTR newSrcPath)
{
	USES_CONVERSION;

	if (IsValidLayer(layerHandle))
	{
		Layer* l = _allLayers[layerHandle];
		const CString newFile = newSrcPath;
		const CComBSTR bstrName(newFile);
		VARIANT_BOOL rt;

		if (l->IsShapefile())
		{
			IShapefile* sf;
			if (!l->QueryShapefile(&sf)) return;
			sf->Resource(bstrName, &rt);

			IExtents* box;
			sf->get_Extents(&box);
			double xm, ym, zm, xM, yM, zM;
			box->GetBounds(&xm, &ym, &zm, &xM, &yM, &zM);
			l->extents = Extent(xm, xM, ym, yM);
			box->Release();
			box = nullptr;
			sf->Release();
		}
		else if (l->IsImage())
		{
			IImage* iimg;

			if (!l->QueryImage(&iimg)) return;

			iimg->Resource(bstrName, &rt);
			iimg->Release();
		}
		else
			return;

		Redraw();
	}
	else
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
}

// ***************************************************************
//		ReloadOgrLayerFromSource()
// ***************************************************************
BOOL CMapView::ReloadOgrLayerFromSource(long ogrLayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// get the layer from the specified handle
	Layer* layer = GetLayer(ogrLayerHandle);
	// failure here will have already set ErrorMessage
	if (layer == nullptr) return false;

	// make sure it's an OGR layer
	IOgrLayer* ogrLayer = nullptr;
	layer->QueryOgrLayer(&ogrLayer);
	if (!ogrLayer)
	{
		ErrorMessage(tkFAILED_TO_OPEN_OGR_LAYER);
		return false;
	}

	// reload OGR layer from source
	VARIANT_BOOL vb = VARIANT_FALSE;
	if (layer->IsDynamicOgrLayer())
	{
		vb = VARIANT_TRUE;
		RestartBackgroundLoading(ogrLayerHandle);
	}
	else
	{
		ogrLayer->ReloadFromSource(&vb);
	}

	// we can now Release the IOgrLayer reference
	ogrLayer->Release();

	// if reload failed...
	if (vb == 0)
	{
		ErrorMessage(tkNO_OGR_DATA_WAS_LOADED);
		return VARIANT_FALSE;
	}

	// following reload, and prior to reprojection, we need to update extents;
	// this is particularly to cover transitions to or from 0 shapes in the layer
	layer->UpdateExtentsFromDatasource();

	// do we need to reproject?
	// at this point, success here indicates success of function
	return CheckLayerProjection(layer, ogrLayerHandle) ? TRUE : FALSE;
}

// ***************************************************************
//		RestartBackgroundLoading()
// ***************************************************************
void CMapView::RestartBackgroundLoading(long ogrLayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// get the layer from the specified handle
	Layer* layer = GetLayer(ogrLayerHandle);
	// failure here will have already set ErrorMessage
	if (layer == nullptr) return;

	// reload layer in background thread
	if (layer->IsDynamicOgrLayer())
		layer->LoadAsync(this, _extents, ogrLayerHandle, true);
}

// ****************************************************************** 
//		LayerMaxVisibleScale
// ****************************************************************** 
DOUBLE CMapView::GetLayerMaxVisibleScale(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const Layer* const layer = GetLayer(layerHandle);
	return layer ? layer->maxVisibleScale : 0.0;
}

void CMapView::SetLayerMaxVisibleScale(LONG layerHandle, DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (Layer* layer = GetLayer(layerHandle)) {
		layer->maxVisibleScale = newVal;
	}
}

// ****************************************************************** 
//		LayerMinVisibleScale
// ****************************************************************** 
DOUBLE CMapView::GetLayerMinVisibleScale(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const Layer* const layer = GetLayer(layerHandle);
	return layer ? layer->minVisibleScale : 0.0;
}

void CMapView::SetLayerMinVisibleScale(LONG layerHandle, DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (Layer* layer = GetLayer(layerHandle)) {
		layer->minVisibleScale = newVal;
	}
}

// ****************************************************************** 
//		LayerMinVisibleZoom
// ****************************************************************** 
int CMapView::GetLayerMinVisibleZoom(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const Layer* const layer = GetLayer(layerHandle);
	return layer ? layer->minVisibleZoom : -1;
}

void CMapView::SetLayerMinVisibleZoom(LONG layerHandle, int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (Layer* layer = GetLayer(layerHandle)) {
		if (newVal < 0) newVal = 0;
		if (newVal > 18) newVal = 18;
		layer->minVisibleZoom = newVal;
	}
}

// ****************************************************************** 
//		LayerMaxVisibleZoom
// ****************************************************************** 
int CMapView::GetLayerMaxVisibleZoom(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const Layer* const layer = GetLayer(layerHandle);
	return layer ? layer->maxVisibleZoom : -1;
}

void CMapView::SetLayerMaxVisibleZoom(LONG layerHandle, int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (Layer* layer = GetLayer(layerHandle))
	{
		if (newVal < 0) newVal = 0;
		if (newVal > 100) newVal = 100;
		layer->maxVisibleZoom = newVal;
	}
}

// ****************************************************************** 
//		LayerDynamicVisibility
// ****************************************************************** 
VARIANT_BOOL CMapView::GetLayerDynamicVisibility(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const Layer* const layer = GetLayer(layerHandle);
	return layer ? static_cast<VARIANT_BOOL>(layer->dynamicVisibility) : VARIANT_FALSE;
}

void CMapView::SetLayerDynamicVisibility(LONG layerHandle, VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (Layer* layer = GetLayer(layerHandle))
	{
		layer->dynamicVisibility = newVal ? true : false;
	}
}

#pragma region Serialization
// ********************************************************
//		DeserializeLayerCore()
// ********************************************************
// Loads layer based on the filename; return layer handle
int CMapView::DeserializeLayerCore(CPLXMLNode* node, CStringW projectName, bool utf8Filenames, IStopExecution* callback)
{
	const char* nameA = CPLGetXMLValue(node, "Filename", nullptr);

	CStringW filename = Utility::XmlFilenameToUnicode(nameA, utf8Filenames);

	wchar_t buffer[4096] = L"";
	const DWORD retval = GetFullPathNameW(filename, 4096, buffer, nullptr);
	if (retval > 4096)
		return -1;

	filename = buffer;

	bool visible = false;
	CString s = CPLGetXMLValue(node, "LayerVisible", nullptr);
	if (s != "") visible = atoi(s) == 0 ? false : true;

	long layerHandle = -1;
	VARIANT_BOOL vb = VARIANT_FALSE;

	LayerType layerType = UndefinedLayer;
	s = CPLGetXMLValue(node, "LayerType", nullptr);
	if (_stricmp(s, "Shapefile") == 0)
	{
		layerType = ShapefileLayer;
	}
	else if (_stricmp(s, "Image") == 0)
	{
		layerType = ImageLayer;
	}
	else if (_stricmp(s, "OgrLayer") == 0)
	{
		layerType = OgrLayerSource;
	}

	if (layerType == ShapefileLayer)
	{
		// opening shapefile
		CComPtr<IShapefile> sf = nullptr;
		ComHelper::CreateInstance(idShapefile, (IDispatch**)&sf); // TODO: Don't use C-style cast

		if (sf)
		{
			CComBSTR bstrFilename;
			if (filename.GetLength() == 0)
			{
				// shapefile type is arbitrary; the correct one will be supplied on deserialization
				bstrFilename = L"";
				sf->CreateNew(bstrFilename, ShpfileType::SHP_POLYGON, &vb);
			}
			else
			{
				bstrFilename = filename;
				sf->Open(bstrFilename, nullptr, &vb);
			}

			if (vb)
			{
				layerHandle = this->AddLayer(sf, (BOOL)visible);

				if (CPLXMLNode* nodeShapefile = CPLGetXMLNode(node, "ShapefileClass"))
				{
					IShapefile* isf = sf;
					dynamic_cast<CShapefile*>(isf)->DeserializeCore(VARIANT_TRUE, nodeShapefile);
				}
			}
		}
	}
	else if (layerType == ImageLayer)
	{
		// opening image
		IImage* img = nullptr;
		CoCreateInstance(CLSID_Image, nullptr, CLSCTX_INPROC_SERVER, IID_IImage, (void**)&img);  // TODO: Don't use C-style cast

		if (img)
		{
			const CComBSTR name(filename);
			img->Open(name, USE_FILE_EXTENSION, VARIANT_FALSE, nullptr, &vb);
		}

		if (vb)
		{
			layerHandle = this->AddLayer(img, visible);
			img->Release();
			if (CPLXMLNode* nodeImage = CPLGetXMLNode(node, "ImageClass"))
			{
				dynamic_cast<CImageClass*>(img)->DeserializeCore(nodeImage);
			}
		}
	}
	else if (layerType == OgrLayerSource)
	{
		CComPtr<IOgrLayer> layer = nullptr;
		ComHelper::CreateInstance(idOgrLayer, (IDispatch**)&layer); // TODO: Don't use C-style cast
		if (layer)
		{
			if (CPLXMLNode* nodeOgrLayer = CPLGetXMLNode(node, "OgrLayerClass"))
			{
				OgrHelper::Cast(layer)->DeserializeCore(nodeOgrLayer);
				layerHandle = AddLayer(layer, visible);
			}
		}
	}
	else if (layerType == WmsLayerSource)
	{
		CComPtr<IWmsLayer> layer = nullptr;
		ComHelper::CreateInstance(idWmsLayer, (IDispatch**)&layer); // TODO: Don't use C-style cast
		if (layer)
		{
			if (CPLXMLNode* nodeWmsLayer = CPLGetXMLNode(node, "WmsLayerClass"))
			{
				WmsHelper::Cast(layer)->DeserializeCore(nodeWmsLayer);
				layerHandle = AddLayer(layer, visible);
			}
		}
	}
	else
	{
		// unsupported layer type
		return -1;
	}

	if (layerHandle != -1)
	{
		s = CPLGetXMLValue(node, "LayerName", nullptr);
		_allLayers[layerHandle]->name = Utility::ConvertFromUtf8(s);

		s = CPLGetXMLValue(node, "DynamicVisibility", nullptr);
		_allLayers[layerHandle]->dynamicVisibility = s != "" ? (atoi(s) == 0 ? false : true) : false;

		s = CPLGetXMLValue(node, "MaxVisibleScale", nullptr);
		_allLayers[layerHandle]->maxVisibleScale = s != "" ? Utility::atof_custom(s) : MAX_LAYER_VISIBLE_SCALE;

		s = CPLGetXMLValue(node, "MinVisibleScale", nullptr);
		_allLayers[layerHandle]->minVisibleScale = s != "" ? Utility::atof_custom(s) : 0.0;

		s = CPLGetXMLValue(node, "MaxVisibleZoom", nullptr);
		_allLayers[layerHandle]->maxVisibleZoom = s != "" ? atoi(s) : 18;

		s = CPLGetXMLValue(node, "MinVisibleZoom", nullptr);
		_allLayers[layerHandle]->minVisibleZoom = s != "" ? atoi(s) : 0;

		s = CPLGetXMLValue(node, "LayerKey", nullptr);
		this->SetLayerKey(layerHandle, s);

		s = CPLGetXMLValue(node, "LayerDescription", nullptr);
		this->SetLayerDescription(layerHandle, s);
	}

	return layerHandle;
}

// ********************************************************
//		SerializeLayer()
// ********************************************************
// Filename isn't saved
BSTR CMapView::SerializeLayer(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	CString str = "";
	if (CPLXMLNode* nodeLayer = SerializeLayerCore(layerHandle, ""))
	{
		if (char* s = CPLSerializeXMLTree(nodeLayer))
		{
			str.Append(s);
			CPLFree(s);
		}
		CPLDestroyXMLNode(nodeLayer);
	}
	return A2BSTR(str);
}

// ********************************************************
//		SerializeLayerCore()
// ********************************************************
// For map state generation
CPLXMLNode* CMapView::SerializeLayerCore(LONG layerHandle, CStringW filename)
{
	USES_CONVERSION;

	// TODO: How to cast _activeLayers.size() to long?
	if (layerHandle < 0 || layerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return nullptr;
	}

	CPLXMLNode* psLayer = CPLCreateXMLNode(nullptr, CXT_Element, "Layer");
	if (psLayer)
	{
		CString s;
		Layer* layer = _allLayers[layerHandle];
		if (layer)
		{
			switch (layer->get_LayerType())
			{
			case OgrLayerSource:
				s = "OgrLayer";
				break;
			case ImageLayer:
				s = "Image";
				break;
			case ShapefileLayer:
				s = "Shapefile";
				break;
			case WmsLayerSource:
				s = "WmsLayer";
				break;
			case UndefinedLayer:
				s = "Undefined";
				break;
			}

			Utility::CPLCreateXMLAttributeAndValue(psLayer, "LayerType", s);
			Utility::CPLCreateXMLAttributeAndValue(psLayer, "LayerName", layer->name);

			Utility::CPLCreateXMLAttributeAndValue(psLayer, "LayerVisible", CPLString().Printf("%d", static_cast<int>(layer->get_Visible())));

			if (OLE2A(layer->key) != "")
				Utility::CPLCreateXMLAttributeAndValue(psLayer, "LayerKey", OLE2CA(layer->key));

			if (layer->description != "")
				Utility::CPLCreateXMLAttributeAndValue(psLayer, "LayerDescription", layer->description);

			if (layer->dynamicVisibility != false)
				Utility::CPLCreateXMLAttributeAndValue(psLayer, "DynamicVisibility", CPLString().Printf("%d", static_cast<int>(layer->dynamicVisibility)));

			if (layer->minVisibleScale != 0.0)
				Utility::CPLCreateXMLAttributeAndValue(psLayer, "MinVisibleScale", CPLString().Printf("%f", layer->minVisibleScale));

			if (layer->maxVisibleScale != 100000000.0)
				Utility::CPLCreateXMLAttributeAndValue(psLayer, "MaxVisibleScale", CPLString().Printf("%f", layer->maxVisibleScale));

			if (layer->minVisibleZoom != 0)
				Utility::CPLCreateXMLAttributeAndValue(psLayer, "MinVisibleZoom", CPLString().Printf("%d", layer->minVisibleZoom));

			if (layer->maxVisibleZoom != 18)
				Utility::CPLCreateXMLAttributeAndValue(psLayer, "MaxVisibleZoom", CPLString().Printf("%d", layer->maxVisibleZoom));

			CComPtr<IWmsLayer> wms = nullptr;
			CComPtr<IOgrLayer> ogr = nullptr;
			layer->QueryOgrLayer(&ogr);
			layer->QueryWmsLayer(&wms);

			if (ogr)
			{
				if (CPLXMLNode* node = OgrHelper::Cast(ogr)->SerializeCore("OgrLayerClass"))
				{
					CPLAddXMLChild(psLayer, node);
				}
			}
			else if (wms)
			{
				if (CPLXMLNode* node = WmsHelper::Cast(wms)->SerializeCore("WmsLayerClass"))
				{
					CPLAddXMLChild(psLayer, node);
				}
			}
			else
			{
				// retrieving filename
				IImage* img = nullptr;
				CComPtr<IShapefile> sf = nullptr;
				layer->QueryShapefile(&sf);
				layer->QueryImage(&img);

				if (sf || img)
				{
					CPLXMLNode* node;

					if (sf)
					{
						IShapefile* isf = sf;
						node = dynamic_cast<CShapefile*>(isf)->SerializeCore(VARIANT_TRUE, "ShapefileClass", true);
					}
					else
					{
						CComBSTR bstr;
						img->get_SourceFilename(&bstr);
						const CStringW sourceName = Utility::GetNameFromPath(OLE2W(bstr));
						Utility::CPLCreateXMLAttributeAndValue(psLayer, "SourceName", sourceName);

						node = dynamic_cast<CImageClass*>(img)->SerializeCore(VARIANT_FALSE, "ImageClass");

						img->Release();
					}

					Utility::CPLCreateXMLAttributeAndValue(psLayer, "Filename", filename);
					if (node)
					{
						CPLAddXMLChild(psLayer, node);
					}
				}
			}
		}
	}

	return psLayer;
}

// ********************************************************
//		DeserializeLayer()
// ********************************************************
// Restores options, but doesn't add layer
VARIANT_BOOL CMapView::DeserializeLayer(LONG layerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	const CString s = newVal;
	VARIANT_BOOL retval = VARIANT_FALSE;
	if (CPLXMLNode* node = CPLParseXMLString(s.GetString()))
	{
		retval = DeserializeLayerOptionsCore(layerHandle, node);
		CPLDestroyXMLNode(node);
	}
	return retval;
}

// ********************************************************
//		DeserializeLayerOptionsCore()
// ********************************************************
VARIANT_BOOL CMapView::DeserializeLayerOptionsCore(LONG layerHandle, CPLXMLNode* node)
{
	if (layerHandle < 0 || layerHandle >= static_cast<long>(_allLayers.size()))
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return VARIANT_FALSE;
	}

	if (!node)
	{
		ErrorMessage(tkINVALID_FILE);
		return VARIANT_FALSE;
	}

	node = CPLGetXMLNode(node, "=Layer");
	if (!node)
	{
		ErrorMessage(tkINVALID_FILE);
		return VARIANT_FALSE;
	}

	// layer type in the file
	LayerType layerType = UndefinedLayer;
	CString s = CPLGetXMLValue(node, "LayerType", nullptr);

	if (_stricmp(s.GetString(), "Shapefile") == 0)
		layerType = ShapefileLayer;
	else if (_stricmp(s.GetString(), "Image") == 0)
		layerType = ImageLayer;
	else if (_stricmp(s.GetString(), "OgrLayer") == 0)
		layerType = OgrLayerSource;
	else if (_stricmp(s.GetString(), "WmsLayer") == 0)
		layerType = WmsLayerSource;

	if (layerType == UndefinedLayer)
	{
		ErrorMessage(tkINVALID_FILE);
		return VARIANT_FALSE;
	}

	// actual layer type
	//bool injectShapefileToOgr = false;
	Layer* layer = _allLayers[layerHandle];

	if (layer->IsOgrLayer() && layerType == ShapefileLayer)
	{
		//injectShapefileToOgr = true; // TODO: Never used??
	}
	else if (layer->get_LayerType() != layerType)
	{
		ErrorMessage(tkINVALID_FILE);
		return VARIANT_FALSE;
	}

	// layer options
	s = CPLGetXMLValue(node, "LayerVisible", nullptr);
	if (s != "")
	{
		_allLayers[layerHandle]->put_Visible(atoi(s) != 0);
	}

	s = CPLGetXMLValue(node, "DynamicVisibility", nullptr);
	_allLayers[layerHandle]->dynamicVisibility = s != "" ? (atoi(s) == 0 ? false : true) : false;

	s = CPLGetXMLValue(node, "MaxVisibleScale", nullptr);
	_allLayers[layerHandle]->maxVisibleScale = s != "" ? Utility::atof_custom(s) : 100000000.0;	// TODO: use constant

	s = CPLGetXMLValue(node, "MinVisibleScale", nullptr);
	_allLayers[layerHandle]->minVisibleScale = s != "" ? Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue(node, "MinVisibleZoom", nullptr);
	_allLayers[layerHandle]->minVisibleZoom = s != "" ? atoi(s) : 0;

	s = CPLGetXMLValue(node, "MaxVisibleZoom", nullptr);
	_allLayers[layerHandle]->maxVisibleZoom = s != "" ? atoi(s) : 18;


	s = CPLGetXMLValue(node, "LayerKey", nullptr);
	this->SetLayerKey(layerHandle, s);

	s = CPLGetXMLValue(node, "LayerDescription", nullptr);
	this->SetLayerDescription(layerHandle, s);

	bool retVal = false;
	if (layerType == OgrLayerSource)
	{
		IOgrLayer* ogr = nullptr;
		if (layer->QueryOgrLayer(&ogr))
		{
			node = CPLGetXMLNode(node, "OgrLayerClass");
			if (node)
			{
				retVal = dynamic_cast<COgrLayer*>(ogr)->DeserializeOptions(node);
			}
			ogr->Release();
		}
	}
	else if (layerType == WmsLayerSource)
	{
		CComPtr<IWmsLayer> wms = nullptr;
		if (layer->QueryWmsLayer(&wms))
		{
			node = CPLGetXMLNode(node, "WmsLayerClass");
			if (node)
			{
				WmsHelper::Cast(wms)->DeserializeCore(node);
			}
		}
	}
	else if (layerType == ShapefileLayer)
	{
		CComPtr<IShapefile> sf = nullptr;
		if (layer->QueryShapefile(&sf))
		{
			node = CPLGetXMLNode(node, "ShapefileClass");
			if (node)
			{
				IShapefile* isf = sf;
				retVal = dynamic_cast<CShapefile*>(isf)->DeserializeCore(VARIANT_TRUE, node);
			}
		}
	}
	else if (layerType == ImageLayer)
	{
		IImage* img = nullptr;
		if (layer->QueryImage(&img))
		{
			node = CPLGetXMLNode(node, "ImageClass");
			if (node)
			{
				retVal = dynamic_cast<CImageClass*>(img)->DeserializeCore(node);
			}
			img->Release();
		}
	}
	else
	{
		return VARIANT_FALSE;
	}

	FireMapState(layerHandle);

	return retVal ? VARIANT_TRUE : VARIANT_FALSE;
}

// *********************************************************
//		GetLayerFilename()
// *********************************************************
BSTR CMapView::GetLayerFilename(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BSTR filename;

	// TODO: How to cast _activeLayers.size() to long?
	if (layerHandle < 0 || layerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		filename = SysAllocString(L"");
		return filename;
	}

	if (Layer* layer = _allLayers[layerHandle])
	{
		return layer->GetFilename();
	}

	filename = SysAllocString(L"");
	return filename;
}

// *********************************************************
//		RemoveLayerOptions()
// *********************************************************
VARIANT_BOOL CMapView::RemoveLayerOptions(LONG layerHandle, LPCTSTR optionsName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const CString name = get_OptionsFilename(layerHandle, optionsName);
	if (Utility::FileExists(name))
	{
		if (remove(name) != 0)
		{
			ErrorMessage(tkCANT_DELETE_FILE);
			return VARIANT_FALSE;
		}
		return VARIANT_TRUE;
	}
	ErrorMessage(tkINVALID_FILENAME);
	return VARIANT_FALSE;
}

// *********************************************************
//		get_OptionsFilename()
// *********************************************************
CString CMapView::get_OptionsFilename(LONG layerHandle, LPCTSTR optionsName)
{
	CComBSTR filename;
	filename.Attach(this->GetLayerFilename(layerHandle));

	USES_CONVERSION;
	CString name = OLE2CA(filename);

	// constructing name
	const CString dot = _stricmp(optionsName, "") == 0 ? "" : ".";
	name += dot;
	name += optionsName;
	name += ".mwsymb";
	return name;
}

// *********************************************************
//		SaveLayerOptions()
// *********************************************************
VARIANT_BOOL CMapView::SaveLayerOptions(LONG layerHandle, LPCTSTR optionsName, VARIANT_BOOL overwrite, LPCTSTR description)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	bool result = false;

	if (Layer* layer = GetLayer(layerHandle))
	{
		if (layer->IsDiskBased())
		{
			const CString name = get_OptionsFilename(layerHandle, optionsName);

			if (Utility::FileExists(name))
			{
				if (!overwrite)
				{
					ErrorMessage(tkCANT_CREATE_FILE);
					return VARIANT_FALSE;
				}

				if (remove(name) != 0)
				{
					ErrorMessage(tkCANT_DELETE_FILE);
					return VARIANT_FALSE;
				}
			}

			if (CPLXMLNode* psTree = LayerOptionsToXmlTree(layerHandle))
			{
				USES_CONVERSION;
				result = GdalHelper::SerializeXMLTreeToFile(psTree, A2W(name)) != 0;		// TODO: use Unicode
				CPLDestroyXMLNode(psTree);
			}
		}
		else if (layer->IsOgrLayer())
		{
			if (CPLXMLNode* psTree = LayerOptionsToXmlTree(layerHandle))
			{
				char* buffer = CPLSerializeXMLTree(psTree);
				CPLDestroyXMLNode(psTree);

				if (!buffer) return VARIANT_FALSE;
				const CStringW xml = Utility::ConvertFromUtf8(buffer);
				CPLFree(buffer);

				IOgrLayer* ogrLayer = nullptr;
				layer->QueryOgrLayer(&ogrLayer);
				if (ogrLayer)
				{
					VARIANT_BOOL vb;
					const CComBSTR bstr(optionsName);
					dynamic_cast<COgrLayer*>(ogrLayer)->SaveStyle(bstr, xml, &vb);
					if (vb) result = true;
				}
			}
		}
	}
	return result ? VARIANT_TRUE : VARIANT_FALSE;
}

// *********************************************************
//		LayerOptionsToXmlTree()
// *********************************************************
CPLXMLNode* CMapView::LayerOptionsToXmlTree(long layerHandle)
{
	if (CPLXMLNode* node = this->SerializeLayerCore(layerHandle, ""))
	{
		if (CPLXMLNode* psTree = CPLCreateXMLNode(nullptr, CXT_Element, "MapWinGIS"))
		{
			Utility::WriteXmlHeaderAttributes(psTree, "LayerFile");
			USES_CONVERSION;
			CPLAddXMLChild(psTree, node);
			return psTree;
		}
		CPLDestroyXMLNode(node);
	}
	return nullptr;
}

// *********************************************************
//		LoadLayerOptionsCore()
// *********************************************************
VARIANT_BOOL CMapView::LoadLayerOptionsCore(CString baseName, LONG layerHandle, LPCTSTR optionsName, BSTR* description)
{
	if (_stricmp(baseName, "") == 0)
	{
		return VARIANT_FALSE;		// error code is in the function
	}

	if (_stricmp(optionsName, "") == 0)
	{
		optionsName = "default";
	}

	// shp.view-default.mwsymb
	CString name = baseName;
	name += ".";  //view-";
	name += optionsName;
	name += ".mwsymb";

	if (!Utility::FileExists(name))
	{
		// shp.mwsymb
		name = baseName + ".mwsymb";
		if (!Utility::FileExists(name))
		{
			ErrorMessage(tkINVALID_FILENAME);
			return VARIANT_FALSE;
		}
	}

	VARIANT_BOOL success = VARIANT_FALSE;
	if (CPLXMLNode* root = CPLParseXMLFile(name))
	{
		if (_stricmp(root->pszValue, "MapWinGIS") != 0)
		{
			ErrorMessage(tkINVALID_FILE);
		}
		else
		{
			const CString s = CPLGetXMLValue(root, "Description", nullptr);
			*description = A2BSTR(s);
			CPLXMLNode* node = CPLGetXMLNode(root, "Layer");
			success = DeserializeLayerOptionsCore(layerHandle, node);
		}
		CPLDestroyXMLNode(root);
	}
	return success;
}

// *********************************************************
//		LoadLayerOptions()
// *********************************************************
VARIANT_BOOL CMapView::LoadLayerOptions(LONG layerHandle, LPCTSTR optionsName, BSTR* description)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());;

	if (Layer* l = GetLayer(layerHandle))
	{
		if (l->IsOgrLayer())
		{
			USES_CONVERSION;
			return LoadOgrStyle(l, layerHandle, A2W(optionsName), true);
		}
	}

	CComBSTR filename;
	filename.Attach(this->GetLayerFilename(layerHandle));

	// constructing name
	USES_CONVERSION;
	const CString baseName = OLE2CA(filename);

	return LoadLayerOptionsCore(baseName, layerHandle, optionsName, description);
}
#pragma endregion

// *******************************************************
//		GetLayerSkipOnSaving
// *******************************************************
VARIANT_BOOL CMapView::GetLayerSkipOnSaving(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: How to cast _activeLayers.size() to long?
	if (layerHandle < 0 || layerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return VARIANT_FALSE;
	}

	if (const Layer* const layer = _allLayers[layerHandle])
	{
		return layer->skipOnSaving;
	}

	return VARIANT_FALSE;
}

// *******************************************************
//		SetLayerSkipOnSaving
// *******************************************************
void CMapView::SetLayerSkipOnSaving(LONG layerHandle, VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: How to cast _activeLayers.size() to long?
	if (layerHandle < 0 || layerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return;
	}

	if (Layer* layer = _allLayers[layerHandle])
	{
		layer->skipOnSaving = newVal;
	}
}

// *******************************************************
//		GetLayerExtents
// *******************************************************
IExtents* CMapView::GetLayerExtents(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!IsValidLayer(layerHandle))
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return nullptr;
	}

	if (const Layer* const layer = _allLayers[layerHandle])
	{
		IExtents* box;
		ComHelper::CreateExtents(&box);
		box->SetBounds(layer->extents.left, layer->extents.bottom, 0.0, layer->extents.right, layer->extents.top, 0.0);
		return box;
	}

	return nullptr;
}

// ************************************************************
//		get_LayerByPosition
// ************************************************************
Layer* CMapView::get_LayerByPosition(int position)
{
	const long layerHandle = GetLayerHandle(position);
	return layerHandle != -1 ? GetLayer(layerHandle) : nullptr;
}
