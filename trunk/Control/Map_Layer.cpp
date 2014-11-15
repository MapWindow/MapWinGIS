// Implementation of CMapView class (see other files as well)
// The properties and methods common for every layer are stored here.
// TODO: consider creation of Layers class as wrapper for this methods
// In this the members can copied to the new class. In the current class 
// simple redirections will be used. To make m_alllayers, m_activelayers available
// the variables can be initialized with the pointers to underlying data of Layers class
#pragma once
#include "stdafx.h"
#include "Map.h"
#include "Image.h"
#include "Grid.h"
#include "Shapefile.h"
#include "ImageLayerInfo.h"
#include "OgrLayer.h"

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
BSTR CMapView::GetLayerName(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( IsValidLayer(LayerHandle) )
	{	
		return W2BSTR( _allLayers[LayerHandle]->name );
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		CString result;
		return result.AllocSysString();
	}
}
void CMapView::SetLayerName(LONG LayerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( IsValidLayer(LayerHandle) )
	{	
		USES_CONVERSION;
		_allLayers[LayerHandle]->name = A2W(newVal);	// TODO: use Unicode
	}
	else
		ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ****************************************************
//		GetLayerDescription()
// ****************************************************
BSTR CMapView::GetLayerDescription(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( IsValidLayer(LayerHandle) )
	{	
		return A2BSTR( _allLayers[LayerHandle]->description );
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		CString result;
		return result.AllocSysString();
	}
}

// ****************************************************
//		SetLayerDescription()
// ****************************************************
void CMapView::SetLayerDescription(LONG LayerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		layer->description = newVal;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
}

// ************************************************************
//		LayerKey()
// ************************************************************
BSTR CMapView::GetLayerKey(long LayerHandle)
{
	USES_CONVERSION;
	if( IsValidLayer(LayerHandle) )
	{	
		return OLE2BSTR( _allLayers[LayerHandle]->key );
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		CString result;
		return result.AllocSysString();
	}
}
void CMapView::SetLayerKey(long LayerHandle, LPCTSTR lpszNewValue)
{
	USES_CONVERSION;

	if( IsValidLayer(LayerHandle) )
	{	
		::SysFreeString(_allLayers[LayerHandle]->key);
		_allLayers[LayerHandle]->key = A2BSTR(lpszNewValue);
	}
	else
		ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ************************************************************
//		LayerPosition()
// ************************************************************
long CMapView::GetLayerPosition(long LayerHandle)
{
	if( IsValidLayer(LayerHandle) )
	{
		register int i;
		long endcondition = _activeLayers.size();
		for( i = 0; i < endcondition; i++ )
		{
			if( _activeLayers[i] == LayerHandle )
				return i;
		}

		return -1;
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return -1;
	}
}

// ************************************************************
//		LayerHandle()
// ************************************************************
long CMapView::GetLayerHandle(long LayerPosition)
{
	if( LayerPosition >= 0 && LayerPosition < (long)_activeLayers.size())
	{
		return _activeLayers[LayerPosition];
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_POSITION);
		return -1;
	}
}

// ************************************************************
//		GetLayerVisible()
// ************************************************************
BOOL CMapView::GetLayerVisible(long LayerHandle)
{
	if( IsValidLayer(LayerHandle) )
	{	
		return ( _allLayers[LayerHandle]->flags & Visible );
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return FALSE;
	}
}

// ************************************************************
//		SetLayerVisible()
// ************************************************************
void CMapView::SetLayerVisible(long LayerHandle, BOOL bNewValue)
{
	if( IsValidLayer(LayerHandle) )
	{	
		if( bNewValue != FALSE )
			_allLayers[LayerHandle]->flags |= Visible;
		else
			_allLayers[LayerHandle]->flags = _allLayers[LayerHandle]->flags & ( 0xFFFFFFFF ^ Visible );

		// we need to refresh the buffer here
		if (_allLayers[LayerHandle]->IsImage())
		{
			if (_allLayers[LayerHandle]->object)
			{
				IImage * iimg = NULL;
				if (_allLayers[LayerHandle]->QueryImage(&iimg))
				{
					((CImageClass*)iimg)->_bufferReloadIsNeeded = true;
					iimg->Release();	
				}
			}
		}

		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
		{
			InvalidateControl();
		}
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return;
	}
}

// ************************************************************
//		GetGetObject()
// ************************************************************
LPDISPATCH CMapView::GetGetObject(long LayerHandle)
{
	if( IsValidLayer(LayerHandle) )
	{	
		if (_allLayers[LayerHandle]->type == OgrLayerSource)
		{
			// for OGR layers we return underlying shapefile to make it compliant with existing client code
			IShapefile* sf = NULL;
			_allLayers[LayerHandle]->QueryShapefile(&sf);
			return (IDispatch*)sf;
		}

		IDispatch* obj = _allLayers[LayerHandle]->object;
		if (obj != NULL) obj->AddRef();
		return obj;
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return NULL;
	}
}

// ***************************************************************
//		AddLayerFromFilename()
// ***************************************************************
long CMapView::AddLayerFromFilename(LPCTSTR Filename, tkFileOpenStrategy openStrategy, VARIANT_BOOL visible)
{
	USES_CONVERSION;
	IDispatch* layer = NULL;
	_fileManager->Open(A2BSTR(Filename), openStrategy, _globalCallback, &layer);
	if (layer) {
		long handle = AddLayer(layer, visible);
		layer->Release();
		return handle;
	}
	else {
		return -1;
	}
}

// ***************************************************************
//		AddLayerFromDatabase()
// ***************************************************************
long CMapView::AddLayerFromDatabase(LPCTSTR ConnectionString, LPCTSTR layerNameOrQuery, VARIANT_BOOL visible)
{
	USES_CONVERSION;
	IOgrLayer* layer = NULL;
	_fileManager->OpenFromDatabase(A2BSTR(ConnectionString), A2BSTR(layerNameOrQuery), &layer);
	if (layer) {
		long handle = AddLayer(layer, visible);
		layer->Release();
		return handle;
	}
	else {
		return -1;
	}
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

	CComPtr<IShapefile> ishp = NULL;
	CComPtr<IImage> iimg = NULL;
	CComPtr<IGrid> igrid = NULL;
	CComPtr<IOgrLayer> iogr = NULL;

	object->QueryInterface(IID_IShapefile, (void**)&ishp);
	object->QueryInterface(IID_IImage, (void**)&iimg);
	object->QueryInterface(IID_IGrid, (void**)&igrid);
	object->QueryInterface(IID_IOgrLayer, (void**)&iogr);

	CComPtr<ICallback> callback = NULL;
	
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
long CMapView::AddLayer(LPDISPATCH Object, BOOL pVisible)
{
	long layerHandle = -1;

	if( Object == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return layerHandle;
	}

	IShapefile * ishp = NULL;
	Object->QueryInterface(IID_IShapefile,(void**)&ishp);

	IImage * iimg = NULL;
	Object->QueryInterface(IID_IImage,(void**)&iimg);

	IGrid * igrid = NULL;
	Object->QueryInterface(IID_IGrid,(void**)&igrid);

	IOgrLayer * iogr = NULL;
	Object->QueryInterface(IID_IOgrLayer, (void**)&iogr);
	
	if(!igrid && !iimg && !ishp && !iogr)
	{
		AfxMessageBox("Error: Interface Not Supported");
		ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		return -1;
	}

	AttachGlobalCallbackToLayers(Object);

	LockWindow( lmLock );

	Layer * l = NULL;

	if( ishp != NULL || iogr != NULL )
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
		}
		
		l = new Layer();
		if (ishp) l->object = ishp;
		else l->object = iogr;
		l->type = ishp ? ShapefileLayer : OgrLayerSource;
		l->UpdateExtentsFromDatasource();
		l->flags = pVisible != FALSE ? l->flags | Visible : l->flags & ( 0xFFFFFFFF ^ Visible );
		
		layerHandle = AddLayerCore(l);
	}

	// grids aren't added directly; an image representation is created first 
	// using particular color scheme
	CStringW gridFilename = L"";
	IGridColorScheme* gridColorScheme = NULL;

	if (igrid != NULL)
	{
		tkGridSourceType sourceType;
		igrid->get_SourceType(&sourceType);
		if (sourceType == tkGridSourceType::gstUninitialized)
		{
			ErrorMessage(tkGRID_NOT_INITIALIZED);
			LockWindow(lmUnlock);
			return -1;
		}

		CGrid* grid = (CGrid*)igrid;
		gridFilename = grid->GetFilename();
		CStringW proxyName = grid->GetProxyName();
		CStringW legendName = grid->GetProxyLegendName();
		CStringW imageName;

		PredefinedColorScheme coloring = m_globalSettings.GetGridColorScheme();

		igrid->RetrieveOrGenerateColorScheme(gsrAuto, gsgGradient, coloring, &gridColorScheme);
		if (gridColorScheme)
		{
			// there is no proxy; either create a new one or opening directly
			ICallback* cback = NULL;
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
				CStringW legendName = isProxy ? grid->GetProxyLegendName() : grid->GetLegendName();
				IGridColorScheme* newScheme = NULL;
				GetUtils()->CreateInstance(tkInterface::idGridColorScheme, (IDispatch**)&newScheme);
				newScheme->ReadFromFile(W2BSTR(legendName), A2BSTR("GridColoringScheme"), &vb);
				if (vb)
				{
					((CImageClass*)iimg)->LoadImageAttributesFromGridColorScheme(newScheme);
				}
				newScheme->Release();
			}
		}
	}

	// it may be either directly opened image or the one created for the grid
	if( iimg != NULL )
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
		l->object = iimg;
		l->type = ImageLayer;
		l->flags = pVisible != FALSE ? l->flags | Visible : l->flags & ( 0xFFFFFFFF ^ Visible );
		l->UpdateExtentsFromDatasource();

		layerHandle = AddLayerCore(l);
		
		// try to save pixels in case image grouping is enabled
		if (_canUseImageGrouping)
		{
			if (!((CImageClass*)iimg)->SaveNotNullPixels())	// analyzing pixels...
				iimg->put_CanUseGrouping(VARIANT_FALSE);	//  don't try this image any more, before transparency values will be changed
		}
	}
	
	GrabLayerProjection(l);

	// find out filename with symbology before file was substituted by reprojection
	CStringW symbologyName;
	if (m_globalSettings.loadSymbologyOnAddLayer)
	{
		if (!l->IsInMemoryShapefile())
		{
			CComBSTR bstr;
			bstr.Attach(GetLayerFilename(layerHandle));
			symbologyName = OLE2W(bstr);
			symbologyName += L".mwsymb";
			symbologyName = Utility::fileExistsW(symbologyName) ? OLE2W(bstr) : L"";
		}
	}

	// do projection mismatch check
	if (!CheckLayerProjection(l))
	{
		RemoveLayerCore(layerHandle, false);
		LockWindow( lmUnlock );
		return -1;
	}

	// set initial extents
	if (l != NULL && m_globalSettings.zoomToFirstLayer)
	{
		if( _activeLayers.size() == 1 && pVisible)
		{
			if (!l->IsEmpty())
				SetExtentsWithPadding(l->extents);
		}
	}

	// loading symbology
	if(symbologyName.GetLength() > 0)
	{
		CComBSTR desc;
		USES_CONVERSION;
		this->LoadLayerOptionsCore(W2A(symbologyName), layerHandle, "", &desc);
	}

	if (l != NULL) FireLayersChanged();
	_canUseLayerBuffer = FALSE;
	LockWindow( lmUnlock );
	return layerHandle;
}

// ***************************************************************
//		GrabLayerProjection()
// ***************************************************************
void CMapView::GrabLayerProjection( Layer* layer )
{
	// if we don't have a projection, let's try and grab projection from it
	if (_grabProjectionFromData && layer)
	{
		VARIANT_BOOL isEmpty = VARIANT_FALSE;
		GetMapProjection()->get_IsEmpty(&isEmpty);
		if (isEmpty)
		{
			IGeoProjection* gp =  layer->GetGeoProjection();
			if (gp)
			{
				gp->get_IsEmpty(&isEmpty);
				if (!isEmpty)
				{
					IGeoProjection* newProj = NULL;
					gp->Clone(&newProj);
					if (!newProj)
					{
						ErrorMsg(tkFAILED_TO_COPY_PROJECTION);
					}
					else
					{
						this->SetGeoProjection(newProj);
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
bool CMapView::CheckLayerProjection( Layer* layer )
{
	if (_projectionMismatchBehavior == mbIgnore)
		return true;

	IGeoProjection* mapProj = GetMapProjection();
	VARIANT_BOOL isGeographic;
	mapProj->get_IsGeographic(&isGeographic);

	// compare projection of layer with one for the map
	VARIANT_BOOL isEmpty = VARIANT_FALSE;
	IGeoProjection* gp =  layer->GetGeoProjection();
	if (gp)
	{
		gp->get_IsEmpty(&isEmpty);
	}
	
	bool result = false;
	if (isEmpty)
	{
		switch (_projectionMismatchBehavior)
		{
			case mbCheckLoose:
			case mbCheckLooseAndReproject:
				if (isGeographic &&
					(layer->extents.left < -180.0 || layer->extents.right > 180.0 || 
					layer->extents.top > 90.0 || layer->extents.bottom < -90.0))
				{
					ErrorMessage(tkGEOGRAPHIC_PROJECTION_EXPECTED);
					result = false;
				}
				else {
					// nothing else to check; since it's loose check - let it be
					result = true;
				}
				break;
			case mbCheckStrict:
			case mbCheckStrictAndReproject:
				// since it's strict, we want to be sure
				ErrorMessage(tkMISSING_GEOPROJECTION);
				result = false;
				break;
		}
		if (gp)
			gp->Release();
		return result;
	}
	else
	{
		IExtents* ext = NULL;
		GetUtils()->CreateInstance(idExtents, (IDispatch**)&ext);
		ext->SetBounds(layer->extents.left, layer->extents.bottom, 0.0, layer->extents.right, layer->extents.top, 0.0);

		IGeoProjection* gpMap = GetMapProjection();
		VARIANT_BOOL isEmpty;
		gpMap->get_IsEmpty(&isEmpty);
		if (isEmpty && 
			(_projectionMismatchBehavior == mbCheckLoose || _projectionMismatchBehavior == mbCheckLooseAndReproject)) 
		{
			return true;
		}

		VARIANT_BOOL match;
		GetMapProjection()->get_IsSameExt(gp, ext, 20, &match);
		ext->Release();
		if (gp) {
			gp->Release();
			gp = NULL;
		}

		if (match)
		{
			return true;
		}
		else
		{
			if (_projectionMismatchBehavior == mbCheckStrict || 
				_projectionMismatchBehavior == mbCheckLoose)
			{
				// no transformation option is chosen, so give up
				ErrorMessage(tkPROJECTION_MISMATCH);
				return false;
			}
			
			if (layer->IsImage())
			{
				ErrorMessage(tkNO_REPROJECTION_FOR_IMAGES);
				return false;
			}
			
			// let's try to do a transformation
			if (layer->IsShapefile())
			{
				IShapefile* sf = NULL;
				layer->QueryShapefile(&sf);
				if (sf)
				{
					long numShapes;
					sf->get_NumShapes(&numShapes);
					
					long count;
					IShapefile* sfNew = NULL;
					sf->Reproject(GetMapProjection(), &count, &sfNew);

					result = false;
					if (!sfNew || 
						(numShapes != count && _projectionMismatchBehavior == mbCheckStrictAndReproject))
					{
						if (sfNew)
							sfNew->Release();
						ErrorMessage(tkFAILED_TO_REPROJECT);
						return false;
					}
					else
					{
						// let's substitute original file with this one
						// don't close the original shapefile; use may still want to interact with it
						sf->Release();				// release the original reference
						if (layer->type == OgrLayerSource)
						{
							IOgrLayer* ogr;
							layer->QueryOgrLayer(&ogr);
							if (ogr) {
								((COgrLayer*)ogr)->InjectShapefile(sfNew);
								ogr->Release();
							}
						}
						else
						{
							layer->object = sfNew;		// TODO: save it to the disk as an option
						}
						layer->UpdateExtentsFromDatasource();
						return true;
					}

					sf->Release();
					return result;
				}
			}

			// in case of some omissions or new layer types
			return _projectionMismatchBehavior == mbCheckLooseAndReproject ? true : false;		
		}
	}
}

// ***************************************************************
//		RemoveLayerCore()
// ***************************************************************
void CMapView::RemoveLayerCore(long LayerHandle, bool closeDatasources)
{
	try
	{
		if( IsValidLayer(LayerHandle) )
		{
			bool hadLayers = _activeLayers.size() > 0;
			if (LayerHandle >= (long)_allLayers.size()) return;
			Layer * l = _allLayers[LayerHandle];
			if (l == NULL) return;

			if (closeDatasources)
				l->CloseDatasources();
			
			for(unsigned int i = 0; i < _activeLayers.size(); i++ )
			{	
				if( _activeLayers[i] == LayerHandle )
				{	
					_activeLayers.erase( _activeLayers.begin() + i );
					break;
				}
			}

			try
			{
				// This may have been deleted already.
				if (_allLayers[LayerHandle] != NULL)
				{
					delete _allLayers[LayerHandle];
				}
			}
			catch(...)
			{
				#ifdef _DEBUG
						AfxMessageBox("CMapView::RemoveLayer: Exception while deleting Layer.");
				#endif
			}

			_allLayers[LayerHandle] = NULL;

			if (_activeLayers.size() == 0 && hadLayers)
				ClearMapProjectionWithLastLayer();

			FireLayersChanged();

			_canUseLayerBuffer = FALSE;
			if( !_lockCount )
				InvalidateControl();
		}
		else
			ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	catch(...)
	{}
}

// ***************************************************************
//		RemoveLayer()
// ***************************************************************
void CMapView::RemoveLayer(long LayerHandle)
{
	RemoveLayerCore(LayerHandle, true);
}

// ***************************************************************
//		RemoveLayerWithoutClosing()
// ***************************************************************
void CMapView::RemoveLayerWithoutClosing(long LayerHandle)
{
	RemoveLayerCore(LayerHandle, false);
}

// ***************************************************************
//		RemoveAllLayers()
// ***************************************************************
void CMapView::RemoveAllLayers()
{
	LockWindow( lmLock );
	bool hadLayers = _activeLayers.size() > 0;
	
	for(unsigned int i = 0; i < _allLayers.size(); i++ )
	{
		if( IsValidLayer(i) )
		{
			RemoveLayer(i);
		}
	}
	_allLayers.clear();

	LockWindow( lmUnlock );

	_activeLayerPosition = 0;
	_canUseLayerBuffer = FALSE;

	if( !_lockCount )
		InvalidateControl();
}

// ***************************************************************
//		MoveLayerUp()
// ***************************************************************
BOOL CMapView::MoveLayerUp(long InitialPosition)
{
	if( InitialPosition >= 0 && InitialPosition < (long)_activeLayers.size() )
	{	
		long layerHandle = _activeLayers[InitialPosition];

		_activeLayers.erase( _activeLayers.begin() + InitialPosition );

		long newPos = InitialPosition + 1;
		if( newPos > (long)_activeLayers.size() )
			newPos = _activeLayers.size();

		_activeLayers.insert( _activeLayers.begin() + newPos, layerHandle );

		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
			InvalidateControl();
		return TRUE;
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_POSITION);
		return FALSE;
	}
}

// ***************************************************************
//		MoveLayerDown()
// ***************************************************************
BOOL CMapView::MoveLayerDown(long InitialPosition)
{
	if( InitialPosition >= 0 && InitialPosition < (long)_activeLayers.size() )
	{	
		long layerHandle = _activeLayers[InitialPosition];
		_activeLayers.erase( _activeLayers.begin() + InitialPosition );

		long newPos = InitialPosition - 1;
		if( newPos < 0 )
			newPos = 0;

		_activeLayers.insert( _activeLayers.begin() + newPos, layerHandle );

		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
			InvalidateControl();

		return TRUE;
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_POSITION);
		return FALSE;
	}
}

// ***************************************************************
//		MoveLayer()
// ***************************************************************
BOOL CMapView::MoveLayer(long InitialPosition, long TargetPosition)
{
	if(InitialPosition == TargetPosition)
		return TRUE;

	if( InitialPosition >= 0 && InitialPosition < (long)_activeLayers.size() &&  
		TargetPosition >= 0 && TargetPosition < (long)_activeLayers.size())
	{
		long layerHandle = _activeLayers[InitialPosition];

		_activeLayers.erase( _activeLayers.begin() + InitialPosition );
		_activeLayers.insert( _activeLayers.begin() + TargetPosition, layerHandle );

		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
			InvalidateControl();

		return TRUE;
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_POSITION);
		return FALSE;
	}
}

// ***************************************************************
//		MoveLayerTop()
// ***************************************************************
BOOL CMapView::MoveLayerTop(long InitialPosition)
{
	if( InitialPosition >= 0 && InitialPosition < (long)_activeLayers.size() )
	{	
		long layerHandle = _activeLayers[InitialPosition];
		_activeLayers.erase( _activeLayers.begin() + InitialPosition );
		_activeLayers.push_back(layerHandle);

		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
			InvalidateControl();
		return TRUE;
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_POSITION);
		return FALSE;
	}
}

// ***************************************************************
//		MoveLayerBottom()
// ***************************************************************
BOOL CMapView::MoveLayerBottom(long InitialPosition)
{
	if( InitialPosition >= 0 && InitialPosition < (long)_activeLayers.size() )
	{	
		long layerHandle = _activeLayers[InitialPosition];
		_activeLayers.erase( _activeLayers.begin() + InitialPosition );
		_activeLayers.push_front(layerHandle);

		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
			InvalidateControl();
		return TRUE;
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_POSITION);
		return FALSE;
	}
}

// ***************************************************************
//		ReSourceLayer()
// ***************************************************************
void CMapView::ReSourceLayer(long LayerHandle, LPCTSTR newSrcPath)
{
	USES_CONVERSION;

	if (IsValidLayer(LayerHandle))
	{	
		Layer * l = _allLayers[LayerHandle];
		CString newFile = newSrcPath;
		VARIANT_BOOL rt;
		if (l->IsShapefile())
		{
			IShapefile * sf = NULL;
			//l->object->QueryInterface(IID_IShapefile, (void**)&sf);
			//if (sf == NULL) return;
			if (!l->QueryShapefile(&sf)) return;
			sf->Resource(newFile.AllocSysString(), &rt);

			IExtents * box = NULL;
			sf->get_Extents(&box);
			double xm,ym,zm,xM,yM,zM;
			box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
			l->extents = Extent(xm,xM,ym,yM);
			box->Release();
			box = NULL;
			sf->Release();

			/*if (l->file != NULL) fclose(l->file);
			l->file = ::fopen(newFile.GetBuffer(),"rb");*/
		}
		else if(l->IsImage())
		{
			IImage * iimg = NULL;
			
			//if (iimg == NULL) return;
			if (!l->QueryImage(&iimg)) return;
			iimg->Resource(newFile.AllocSysString(), &rt);
			iimg->Release();

			/*if (l->file != NULL) fclose(l->file);
			l->file = ::fopen(newFile.GetBuffer(),"rb");*/
		}
		else
			return;

		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
			InvalidateControl();
	}
	else
	{	_lastErrorCode = tkINVALID_LAYER_HANDLE;
		if( _globalCallback != NULL )
			_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(_lastErrorCode)));
	}
}

// ****************************************************************** 
//		LayerMaxVisibleScale
// ****************************************************************** 
DOUBLE CMapView::GetLayerMaxVisibleScale(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		return layer->maxVisibleScale;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return 0.0;
	}
}

void CMapView::SetLayerMaxVisibleScale(LONG LayerHandle, DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		layer->maxVisibleScale = newVal;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}	
}

// ****************************************************************** 
//		LayerMinVisibleScale
// ****************************************************************** 
DOUBLE CMapView::GetLayerMinVisibleScale(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		return layer->minVisibleScale;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return 0.0;
	}
}

void CMapView::SetLayerMinVisibleScale(LONG LayerHandle, DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		layer->minVisibleScale = newVal;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
}

// ****************************************************************** 
//		LayerMinVisibleZoom
// ****************************************************************** 
int CMapView::GetLayerMinVisibleZoom(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		return layer->minVisibleZoom;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return -1;
	}
}

void CMapView::SetLayerMinVisibleZoom(LONG LayerHandle, int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		if (newVal < 0) newVal = 0;
		if (newVal > 18) newVal = 18;
		layer->minVisibleZoom = newVal;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
}

// ****************************************************************** 
//		LayerMaxVisibleScale
// ****************************************************************** 
int CMapView::GetLayerMaxVisibleZoom(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		return layer->maxVisibleZoom;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return -1;
	}
}

void CMapView::SetLayerMaxVisibleZoom(LONG LayerHandle, int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		if (newVal < 0) newVal = 0;
		if (newVal > 100) newVal = 100;
		layer->maxVisibleZoom = newVal;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}	
}

// ****************************************************************** 
//		LayerDynamicVisibility
// ****************************************************************** 
VARIANT_BOOL CMapView::GetLayerDynamicVisibility(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		return layer->dynamicVisibility;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return VARIANT_FALSE;
	}
}

void CMapView::SetLayerDynamicVisibility(LONG LayerHandle, VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[LayerHandle];
		layer->dynamicVisibility = newVal?true:false;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
}

#pragma region Serialization
// ********************************************************
//		DeserializeLayerCore()
// ********************************************************
// Loads layer based on the filename; return layer handle
int CMapView::DeserializeLayerCore(CPLXMLNode* node, CStringW ProjectName, bool utf8Filenames, IStopExecution* callback)
{
	const char* nameA = CPLGetXMLValue( node, "Filename", NULL );
	
	CStringW filename = Utility::XmlFilenameToUnicode(nameA, utf8Filenames);

	wchar_t buffer[4096] = L""; 
    DWORD retval = GetFullPathNameW(filename, 4096, buffer, NULL);
	if (retval > 4096)
		return -1;
	
	filename = buffer;
	
	bool visible = false;
	CString s = CPLGetXMLValue( node, "LayerVisible", NULL );
	if (s != "") visible = atoi(s) == 0 ? false : true;

	long layerHandle = -1;
	VARIANT_BOOL vb = VARIANT_FALSE;

	LayerType layerType = UndefinedLayer;
	s = CPLGetXMLValue( node, "LayerType", NULL );
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
		IShapefile* sf = NULL;
		CoCreateInstance( CLSID_Shapefile, NULL, CLSCTX_INPROC_SERVER, IID_IShapefile, (void**)&sf );
		
		if (sf) 
		{
			if (filename.GetLength() == 0)
			{
				// shapefile type is arbitrary; the correct one will be supplied on deserialization
				sf->CreateNew(A2BSTR(""), ShpfileType::SHP_POLYGON, &vb);    
			}
			else
			{
				sf->Open(W2BSTR(filename), NULL, &vb);
			}
		
			if (vb)
			{
				layerHandle = this->AddLayer(sf, (BOOL)visible);
				sf->Release();	// Map.AddLayer added second reference
				
				CPLXMLNode* nodeShapefile = CPLGetXMLNode(node, "ShapefileClass");
				if (nodeShapefile)
				{
					((CShapefile*)sf)->DeserializeCore(VARIANT_TRUE, nodeShapefile);
				}
			}
		}
	}
	else if (layerType == ImageLayer)
	{
		// opening image
		IImage* img = NULL;
		CoCreateInstance( CLSID_Image, NULL, CLSCTX_INPROC_SERVER, IID_IImage, (void**)&img );
		
		if (img) 
		{
			img->Open(W2BSTR(filename), USE_FILE_EXTENSION, VARIANT_FALSE, NULL, &vb);
		}
		
		if (vb)
		{
			layerHandle = this->AddLayer(img, (BOOL)visible);
			img->Release();
			CPLXMLNode* nodeImage = CPLGetXMLNode(node, "ImageClass");
			if (nodeImage)
			{
				((CImageClass*)img)->DeserializeCore(nodeImage);
			}
		}
	}
	else if (layerType == OgrLayerSource)
	{
		IOgrLayer* layer = NULL;
		GetUtils()->CreateInstance(idOgrLayer, (IDispatch**)&layer);
		if (layer) 
		{
			CPLXMLNode* nodeOgrLayer = CPLGetXMLNode(node, "OgrLayerClass");
			if (nodeOgrLayer)
			{
				((COgrLayer*)layer)->DeserializeCore(nodeOgrLayer);
				layerHandle = AddLayer(layer, (BOOL)visible);
			}
			layer->Release();
		}
	}
	else
	{
		// unsupported layer type
		return -1;
	}

	if(layerHandle != -1) 
	{
		s = CPLGetXMLValue( node, "LayerName", NULL );
		_allLayers[layerHandle]->name = Utility::ConvertFromUtf8(s);

		s = CPLGetXMLValue( node, "DynamicVisibility", NULL );
		_allLayers[layerHandle]->dynamicVisibility = (s != "") ? (atoi(s) == 0 ? false : true) : false;
		
		s = CPLGetXMLValue( node, "MaxVisibleScale", NULL );
		_allLayers[layerHandle]->maxVisibleScale = (s != "") ? Utility::atof_custom (s) : MAX_LAYER_VISIBLE_SCALE;

		s = CPLGetXMLValue( node, "MinVisibleScale", NULL );
		_allLayers[layerHandle]->minVisibleScale = (s != "") ? Utility::atof_custom (s) : 0.0;

		s = CPLGetXMLValue( node, "MaxVisibleZoom", NULL );
		_allLayers[layerHandle]->maxVisibleZoom = (s != "") ? atoi(s) : 18;

		s = CPLGetXMLValue( node, "MinVisibleZoom", NULL );
		_allLayers[layerHandle]->minVisibleZoom = (s != "") ? atoi(s) : 0;

		s = CPLGetXMLValue( node, "LayerKey", NULL );
		this->SetLayerKey(layerHandle, s);

		s = CPLGetXMLValue( node, "LayerDescription", NULL );
		this->SetLayerDescription(layerHandle, s);
	}

	return layerHandle;
}

// ********************************************************
//		SerializeLayer()
// ********************************************************
// Filename isn't saved
BSTR CMapView::SerializeLayerOptions(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	CString str = "";
	CPLXMLNode* nodeLayer = SerializeLayerCore(LayerHandle, "");
	if (nodeLayer)
	{
		char* s = CPLSerializeXMLTree(nodeLayer);
		if (s)
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
CPLXMLNode* CMapView::SerializeLayerCore(LONG LayerHandle, CStringW Filename)
{
	USES_CONVERSION;
	
	if (LayerHandle < 0 || LayerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return NULL;
	}
		
	CPLXMLNode* psLayer = CPLCreateXMLNode(NULL, CXT_Element, "Layer");
	if (psLayer)
	{
		CString s;
		Layer* layer = _allLayers[LayerHandle];
		if (layer)
		{
			switch (layer->type)
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
				case UndefinedLayer:
					s = "Undefined";
					break;
			}
			Utility::CPLCreateXMLAttributeAndValue( psLayer, "LayerType", s);
			Utility::CPLCreateXMLAttributeAndValue( psLayer, "LayerName", layer->name);
			
			Utility::CPLCreateXMLAttributeAndValue( psLayer, "LayerVisible", CPLString().Printf("%d", (int)(layer->flags & Visible) ));
			
			if (OLE2A(layer->key) != "")
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "LayerKey", OLE2CA(layer->key));

			if (layer->description != "")
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "LayerDescription", layer->description);

			if (layer->dynamicVisibility != false)
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "DynamicVisibility", CPLString().Printf("%d", (int)layer->dynamicVisibility));

			if (layer->minVisibleScale != 0.0)
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "MinVisibleScale", CPLString().Printf("%f", layer->minVisibleScale));
			
			if (layer->maxVisibleScale != 100000000.0)
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "MaxVisibleScale", CPLString().Printf("%f", layer->maxVisibleScale));
			
			if (layer->minVisibleZoom != 0)
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "MinVisibleZoom", CPLString().Printf("%d", layer->minVisibleZoom));

			if (layer->maxVisibleZoom != 18)
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "MaxVisibleZoom", CPLString().Printf("%d", layer->maxVisibleZoom));

			IOgrLayer* ogr = NULL;
			layer->QueryOgrLayer(&ogr);
			if (ogr)
			{
				CPLXMLNode* node = ((COgrLayer*)ogr)->SerializeCore("OgrLayerClass");
				ogr->Release();
				if (node != NULL)
				{
					CPLAddXMLChild(psLayer, node);
				}
			}
			else
			{
				// retrieving filename
				IImage* img = NULL;
				IShapefile* sf = NULL;
				layer->QueryShapefile(&sf);
				layer->QueryImage(&img);

				if (sf || img)
				{
					CPLXMLNode* node = NULL;

					if (sf)
					{
						node = ((CShapefile*)sf)->SerializeCore(VARIANT_TRUE, "ShapefileClass", true);
						sf->Release();
					}
					else
					{
						BSTR bstr;
						img->get_SourceFilename(&bstr);
						CStringW sourceName = Utility::GetNameFromPath(OLE2W(bstr));
						Utility::CPLCreateXMLAttributeAndValue(psLayer, "SourceName", sourceName);

						node = ((CImageClass*)img)->SerializeCore(VARIANT_FALSE, "ImageClass");

						img->Release();
					}

					Utility::CPLCreateXMLAttributeAndValue(psLayer, "Filename", Filename);
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
//		DeserializeLayerOptions()
// ********************************************************
// Restores options, but doesn't add layer
VARIANT_BOOL CMapView::DeserializeLayerOptions(LONG LayerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	CString s = newVal;
	VARIANT_BOOL retval = VARIANT_FALSE;
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		retval = DeserializeLayerOptionsCore(LayerHandle, node);
		CPLDestroyXMLNode( node );
	}
	return retval;
}

// ********************************************************
//		DeserializeLayerOptionsCore()
// ********************************************************
VARIANT_BOOL CMapView::DeserializeLayerOptionsCore(LONG LayerHandle, CPLXMLNode* node)
{
	if (LayerHandle < 0 || LayerHandle >= (long)_allLayers.size())
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
	CString s = CPLGetXMLValue( node, "LayerType", NULL );
	
	if (_stricmp(s.GetString(), "Shapefile") == 0) 
		layerType = ShapefileLayer;
	else if (_stricmp(s.GetString(), "Image") == 0)
		layerType = ImageLayer;

	if (layerType == UndefinedLayer)
	{
		ErrorMessage(tkINVALID_FILE);
		return VARIANT_FALSE;
	}

	// actual layer type
	Layer* layer = _allLayers[LayerHandle];
	if (layer->type != layerType)
	{
		ErrorMessage(tkINVALID_FILE);
		return VARIANT_FALSE;
	}

	// layer options
	s = CPLGetXMLValue( node, "LayerVisible", NULL );
	if (s != "") 
	{
		BOOL val = atoi(s);
		if( val )
			_allLayers[LayerHandle]->flags |= Visible;
		else
			_allLayers[LayerHandle]->flags = _allLayers[LayerHandle]->flags & ( 0xFFFFFFFF ^ Visible );
	}

	s = CPLGetXMLValue( node, "DynamicVisibility", NULL );
	_allLayers[LayerHandle]->dynamicVisibility = (s != "") ? (atoi(s) == 0 ? false : true) : false;
	
	s = CPLGetXMLValue( node, "MaxVisibleScale", NULL );
	_allLayers[LayerHandle]->maxVisibleScale = (s != "") ? Utility::atof_custom(s) : 100000000.0;	// todo use constant

	s = CPLGetXMLValue( node, "MinVisibleScale", NULL );
	_allLayers[LayerHandle]->minVisibleScale = (s != "") ? Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue( node, "LayerKey", NULL );
	this->SetLayerKey(LayerHandle, s);

	s = CPLGetXMLValue( node, "LayerDescription", NULL );
	this->SetLayerDescription(LayerHandle, s);

	bool retVal = false;
	if (layerType == ShapefileLayer)
	{
		IShapefile* sf = NULL;
		if (layer->QueryShapefile(&sf))
		{
			node = CPLGetXMLNode(node, "ShapefileClass");
			if (node)
			{
				retVal = ((CShapefile*)sf)->DeserializeCore(VARIANT_TRUE, node);
			}
			sf->Release();
		}
	}
	else if (layerType == ImageLayer )
	{
		IImage* img = NULL;
		if (layer->QueryImage(&img))
		{
			node = CPLGetXMLNode(node, "ImageClass");
			if (node)
			{
				retVal =((CImageClass*)img)->DeserializeCore(node);
			}
			img->Release();
		}
	}
	else
	{
		return VARIANT_FALSE;
	}
	return retVal ? VARIANT_TRUE : VARIANT_FALSE;
}

// *********************************************************
//		GetLayerFilename()
// *********************************************************
BSTR CMapView::GetLayerFilename(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BSTR filename;

	if (layerHandle < 0 || layerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	else
	{
		// extracting object
		Layer* layer = _allLayers[layerHandle];
		if (layer  )
		{
			switch (layer->type)
			{
				case ShapefileLayer:
				{
					CComPtr<IShapefile> sf = NULL;
					layer->QueryShapefile(&sf);
					if (sf) {
						tkShapefileSourceType shpSource;
						sf->get_SourceType(&shpSource);
						sf->get_Filename(&filename);
						return filename;
					}
					break;
				}
				case ImageLayer:
				{
					CComPtr<IImage> img = NULL;
					layer->QueryImage(&img);
					if (img != NULL)
					{
						tkImageSourceType imgSource;
						img->get_SourceType(&imgSource);
						img->get_Filename(&filename);
						return filename;
					}
					break;
				}
				case OgrLayerSource:
					break;
			}
		}
	}
	
	filename = SysAllocString(L"");
	return filename;
}

// *********************************************************
//		RemoveLayerOptions()
// *********************************************************
VARIANT_BOOL CMapView::RemoveLayerOptions(LONG LayerHandle, LPCTSTR OptionsName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString name = get_OptionsFilename(LayerHandle, OptionsName);
	if (Utility::fileExists(name))
	{
		if( remove( name ) != 0 )
		{
			ErrorMessage(tkCANT_DELETE_FILE);
			return VARIANT_FALSE;
		}
		else
		{
			return VARIANT_TRUE;
		}
	}
	else
	{
		ErrorMessage(tkINVALID_FILENAME);
		return VARIANT_FALSE;
	}
}

// *********************************************************
//		get_OptionsFilename()
// *********************************************************
CString CMapView::get_OptionsFilename(LONG LayerHandle, LPCTSTR OptionsName)
{
	CComBSTR filename;
	filename.Attach(this->GetLayerFilename(LayerHandle));
	
	USES_CONVERSION;
	CString name = OLE2CA(filename);
	//if (_stricmp(OptionsName, "") == 0)
	//{
	//	return "";		// error code is in the function
	//}

	// constructing name
	CString dot = (_stricmp(OptionsName, "") == 0) ? "" : ".";
	name += dot;
	name += OptionsName;
	name += ".mwsymb";
	return name;
}

// *********************************************************
//		SaveLayerOptions()
// *********************************************************
VARIANT_BOOL CMapView::SaveLayerOptions(LONG LayerHandle, LPCTSTR OptionsName, VARIANT_BOOL Overwrite, LPCTSTR Description)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString name = get_OptionsFilename(LayerHandle, OptionsName);

	if (Utility::fileExists(name))
	{
		if (!Overwrite)
		{
			ErrorMessage(tkCANT_CREATE_FILE);
			return VARIANT_FALSE;
		}
		else
		{
			if( remove( name ) != 0 )
			{
				ErrorMessage(tkCANT_DELETE_FILE);
				return VARIANT_FALSE;
			}
		}
	}
	
	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, "MapWinGIS");
	if (psTree) 
	{
		Utility::WriteXmlHeaderAttributes(psTree, "LayerFile");

		CPLXMLNode* node = this->SerializeLayerCore(LayerHandle, "");
		if (node)
		{
			USES_CONVERSION;
			CPLAddXMLChild(psTree, node);
			bool result = GdalHelper::SerializeXMLTreeToFile(psTree, A2W(name)) != 0;		// TODO: use Unicode
			CPLDestroyXMLNode(psTree);
			return result ? VARIANT_TRUE : VARIANT_FALSE;
		}
	}
	
	return VARIANT_FALSE;
}

// *********************************************************
//		LoadLayerOptionsCore()
// *********************************************************
VARIANT_BOOL CMapView::LoadLayerOptionsCore(CString baseName, LONG LayerHandle, LPCTSTR OptionsName, BSTR* Description)
{
	if (_stricmp(baseName, "") == 0) 
	{
		return VARIANT_FALSE;		// error code is in the function
	}

	CString name;
	if (_stricmp(OptionsName, "") == 0) 
	{
		OptionsName = "default";
	}
	
	// shp.view-default.mwsymb
	name = baseName;
	name += ".";  //view-";
	name += OptionsName;
	name += ".mwsymb";
	
	if (!Utility::fileExists(name))
	{
		// shp.mwsymb
		name = baseName + ".mwsymb";
		if (!Utility::fileExists(name))
		{
			ErrorMessage(tkINVALID_FILENAME);
			return VARIANT_FALSE;
		}
	}

	CPLXMLNode* node = CPLParseXMLFile(name);		// TODO: use Unicode
	if (node)
	{
		if (_stricmp( node->pszValue, "MapWinGIS") != 0)
		{
			ErrorMessage(tkINVALID_FILE);
		}
		else
		{
			CString s = CPLGetXMLValue(node, "Description", NULL);
			*Description = A2BSTR(s);
			//*Description = s;
			node = CPLGetXMLNode(node, "Layer");
			return DeserializeLayerOptionsCore(LayerHandle, node);
		}
	}
	return VARIANT_FALSE;
}

// *********************************************************
//		LoadLayerOptions()
// *********************************************************
VARIANT_BOOL CMapView::LoadLayerOptions(LONG LayerHandle, LPCTSTR OptionsName, BSTR* Description)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CComBSTR filename;
	filename.Attach(this->GetLayerFilename(LayerHandle));
	
	// constructing name
	USES_CONVERSION;
	CString baseName = OLE2CA(filename);

	return LoadLayerOptionsCore(baseName, LayerHandle, OptionsName, Description);
}
#pragma endregion

// *******************************************************
//		GetLayerSkipOnSaving
// *******************************************************
VARIANT_BOOL CMapView::GetLayerSkipOnSaving(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (LayerHandle < 0 || LayerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return VARIANT_FALSE;
	}
	
	Layer* layer = _allLayers[LayerHandle];
	if (layer)
	{
		return layer->skipOnSaving;
	}
	else
	{
		return VARIANT_FALSE;
	}
}

// *******************************************************
//		SetLayerSkipOnSaving
// *******************************************************
void CMapView::SetLayerSkipOnSaving(LONG LayerHandle, VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (LayerHandle < 0 || LayerHandle >= (long)_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return;
	}

	Layer* layer = _allLayers[LayerHandle];
	if (layer)
	{
		layer->skipOnSaving = newVal;
	}
}
