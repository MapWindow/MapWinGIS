// Implementation of CMapView class (see other files as well)
// The properties and methods common for every layer are stored here.
// TODO: consider creation of Layers class as wrapper for this methods
// In this the members can copied to the new class. In the current class 
// simple redirections will be used. To make m_alllayers, m_activelayers available
// the variables can be initialized with the pointers to underlying data of Layers class

#pragma once

#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"

#include "xtiffio.h"  /* for TIFF */
#include "geotiffio.h" /* for GeoTIFF */
#include "tiff.h"
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"
#include "tiffiop.h"
#include <fstream>
#include <vector>
#include <atlsafe.h>
#include "IndexSearching.h"

#include "Enumerations.h"

#include "LabelCategory.h"
#include "Labels.h"
#include "Image.h"
#include "Grid.h"

#include "ShapefileDrawing.h"
#include "ImageDrawing.h"
#include "LabelDrawing.h"
#include "ChartDrawing.h"

#include "Projections.h"
#include "cpl_minixml.h"
#include "gdalhelper.h"

long CMapView::GetNumLayers()
{
	return m_activeLayers.size();
}

// ************************************************************
//		LayerName()
// ************************************************************
BSTR CMapView::GetLayerName(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if( IsValidLayer(LayerHandle) )
	{	
		return W2BSTR( m_allLayers[LayerHandle]->name );
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
		m_allLayers[LayerHandle]->name = A2W(newVal);	// TODO: use Unicode
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
		return A2BSTR( m_allLayers[LayerHandle]->description );
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
		return OLE2BSTR( m_allLayers[LayerHandle]->key );
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
		::SysFreeString(m_allLayers[LayerHandle]->key);
		m_allLayers[LayerHandle]->key = A2BSTR(lpszNewValue);
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
		long endcondition = m_activeLayers.size();
		for( i = 0; i < endcondition; i++ )
		{
			if( m_activeLayers[i] == LayerHandle )
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
	if( LayerPosition >= 0 && LayerPosition < (long)m_activeLayers.size())
	{
		return m_activeLayers[LayerPosition];
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
		return ( m_allLayers[LayerHandle]->flags & Visible );
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
			m_allLayers[LayerHandle]->flags |= Visible;
		else
			m_allLayers[LayerHandle]->flags = m_allLayers[LayerHandle]->flags & ( 0xFFFFFFFF ^ Visible );

		// we need to refresh the buffer here
		if (m_allLayers[LayerHandle]->type == ImageLayer)
		{
			if (m_allLayers[LayerHandle]->object)
			{
				//iimg = (IImage*) m_allLayers[LayerHandle]->object;
				IImage * iimg = NULL;
				//m_allLayers[LayerHandle]->object->QueryInterface(IID_IImage,(void**)&iimg);
				//if (iimg != NULL) 
				if (m_allLayers[LayerHandle]->QueryImage(&iimg))
				{
					((CImageClass*)iimg)->_bufferReloadIsNeeded = true;
					iimg->Release();	
				}
			}
		}

		_canUseLayerBuffer = FALSE;
		if( !m_lockCount )
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
		if (m_allLayers[LayerHandle]->object != NULL)
			m_allLayers[LayerHandle]->object->AddRef();
		return m_allLayers[LayerHandle]->object;
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
	_fileManager->Open(A2BSTR(Filename), openStrategy, m_globalCallback, &layer);
	if (layer) {
		return AddLayer(layer, visible);
	}
	else {
		return -1;
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
	
	if(!igrid && !iimg && !ishp)
	{
		AfxMessageBox("Error: Interface Not Supported");
		ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		return -1;
	}

	LockWindow( lmLock );

	Layer * l = NULL;

	if( ishp != NULL )
	{
		tkShapefileSourceType type;
		ishp->get_SourceType(&type);
		if (type == sstUninitialized)
		{
			ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
			LockWindow(lmUnlock);
			return -1;
		}
		
		l = new Layer();
		l->object = ishp;

		IExtents * box = NULL;
		ishp->get_Extents(&box);
		double xm,ym,zm,xM,yM,zM;
		box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
		l->extents = Extent(xm,xM,ym,yM);
		box->Release();
		box = NULL;
		
		l->flags = pVisible != FALSE ? l->flags | Visible : l->flags & ( 0xFFFFFFFF ^ Visible );
		l->type = ShapefileLayer;
		
		for(size_t i = 0; i < m_allLayers.size(); i++ )
		{
			if( !m_allLayers[i] )  // that means we can reuse it
			{	
				layerHandle = i;
				m_allLayers[i] = l;
				break;
			}
		}
		
		if( layerHandle == -1)
		{
			layerHandle = m_allLayers.size();
			m_allLayers.push_back(l);
		}

		m_activeLayers.push_back(layerHandle);

		//ShpfileType type;
		//ishp->get_ShapefileType(&type);
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

		bool inserted = false;
		for(unsigned int i = 0; i < m_allLayers.size() && !inserted; i++ )
		{	
			if( m_allLayers[i] == NULL )
			{	
				layerHandle = i;
				m_allLayers[i] = l;
				inserted = true;
			}
		}
		if( inserted == false )
		{	
			layerHandle = m_allLayers.size();
			m_allLayers.push_back(l);
		}

		m_activeLayers.push_back(layerHandle);

		l->flags = pVisible != FALSE ? l->flags | Visible : l->flags & ( 0xFFFFFFFF ^ Visible );
		l->type = ImageLayer;

		double xllCenter = 0, yllCenter = 0, dx = 0, dy = 0;
		long height = 0, width = 0;
		iimg->get_OriginalXllCenter(&xllCenter);
		iimg->get_OriginalYllCenter(&yllCenter);
		iimg->get_OriginalHeight(&height);
		iimg->get_OriginalWidth(&width);
		iimg->get_OriginalDX(&dx);
		iimg->get_OriginalDY(&dy);

		l->extents = Extent( xllCenter, xllCenter + dx*width, yllCenter, yllCenter + dy*height );

		ImageLayerInfo * ili = new ImageLayerInfo();
		l->addInfo = ili;

		// try to save pixels in case image grouping is enabled
		if (_canUseImageGrouping)
		{
			if (!((CImageClass*)iimg)->SaveNotNullPixels())	// analysing pixels...
				iimg->put_CanUseGrouping(VARIANT_FALSE);	//  don't try this image any more, before transparency values will be changed
		}
	}
	
	// if we don't have a projection, let's try and grab projection from it
	if (_grabProjectionFromData && (ishp || iimg))
	{
		VARIANT_BOOL isEmpty = VARIANT_FALSE;
		GetMapProjection()->get_IsEmpty(&isEmpty);
		if (isEmpty)
		{
			IGeoProjection* proj = NULL;
			if (ishp)
			{
				// simply grab the object from sf
				ishp->get_GeoProjection(&proj);
			}
			else
			{
				// there is no GeoProjection object; so create it from the string
				CComBSTR bstr;
				iimg->GetProjection(&bstr);
				if (bstr.Length() > 0)
				{
					VARIANT_BOOL vb;
					GetUtils()->CreateInstance(tkInterface::idGeoProjection, (IDispatch**)&proj);
					proj->ImportFromAutoDetect(bstr, &vb);
					if (!vb)
					{
						proj->Release();
						proj = NULL;
					}
				}
			}
			
			if (proj)
			{
				proj->get_IsEmpty(&isEmpty);
				if (!isEmpty)
				{
					IGeoProjection* newProj = NULL;
					proj->Clone(&newProj);
					if (!newProj)
					{
						ErrorMsg(tkFAILED_TO_COPY_PROJECTION);
					}
					else
					{
						this->SetGeoProjection(newProj);
					}
				}
				proj->Release();
			}
		}
	}

	// set initial extents
	if (l != NULL && m_globalSettings.zoomToFirstLayer)
	{
		if( m_activeLayers.size() == 1 && pVisible)
		{	
			double xrange = l->extents.right - l->extents.left;
			double yrange = l->extents.top - l->extents.bottom;
			extents.left = l->extents.left - xrange*m_extentPad;
			extents.right = l->extents.right + xrange*m_extentPad;
			extents.top = l->extents.top + yrange*m_extentPad;
			extents.bottom = l->extents.bottom - yrange*m_extentPad;

			SetExtentsCore(extents);
		}
	}

	if (l != NULL) FireLayersChanged();
	LockWindow( lmUnlock );
	return layerHandle;
}

// ***************************************************************
//		RemoveLayer()
// ***************************************************************
void CMapView::RemoveLayerCore(long LayerHandle, bool closeDatasources)
{
	try
	{
		if( IsValidLayer(LayerHandle) )
		{
			bool hadLayers = m_activeLayers.size() > 0;
			
			IShapefile * ishp = NULL;
			IImage * iimg = NULL;
			IGrid * igrid = NULL;

			if (LayerHandle >= (long)m_allLayers.size()) return;
			Layer * l = m_allLayers[LayerHandle];
			if (l == NULL) return;

			l->QueryShapefile(&ishp);
			l->QueryImage(&iimg);
			l->object->QueryInterface(IID_IGrid, (void**)&igrid);

			VARIANT_BOOL vb;
			if (ishp != NULL)
			{
				if (closeDatasources)
					ishp->Close(&vb);
				ishp->Release();		// we release only once because one more release is in Layer destructor
			}
			if (iimg != NULL)
			{
				if (closeDatasources)
					iimg->Close(&vb);
				iimg->Release();		// we release only once because one more release is in Layer destructor
			}
			if (igrid != NULL)
			{	
				if (closeDatasources)
					igrid->Close(&vb);
				igrid->Release();
			}

			for(unsigned int i = 0; i < m_activeLayers.size(); i++ )
			{	
				if( m_activeLayers[i] == LayerHandle )
				{	
					m_activeLayers.erase( m_activeLayers.begin() + i );
					break;
				}
			}

			try
			{
				// This may have been deleted already.
				if (m_allLayers[LayerHandle] != NULL)
				{
					delete m_allLayers[LayerHandle];
				}
			}
			catch(...)
			{
				#ifdef _DEBUG
						AfxMessageBox("CMapView::RemoveLayer: Exception while deleting Layer.");
				#endif
			}

			m_allLayers[LayerHandle] = NULL;

			if (m_activeLayers.size() == 0 && hadLayers)
				ClearMapProjectionWithLastLayer();

			FireLayersChanged();

			_canUseLayerBuffer = FALSE;
			if( !m_lockCount )
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
	bool hadLayers = m_activeLayers.size() > 0;
	
	for(unsigned int i = 0; i < m_allLayers.size(); i++ )
	{
		if( IsValidLayer(i) )
		{
			RemoveLayer(i);
		}
	}
	m_allLayers.clear();
	//FireLayersChanged();

	LockWindow( lmUnlock );

	//if (hadLayers)
	//	ClearMapProjectionWithLastLayer();		// it is removed in RemoveLayerCore

	_activeLayerPosition = 0;
	_canUseLayerBuffer = FALSE;

	if( !m_lockCount )
		InvalidateControl();
}

// ***************************************************************
//		MoveLayerUp()
// ***************************************************************
BOOL CMapView::MoveLayerUp(long InitialPosition)
{
	if( InitialPosition >= 0 && InitialPosition < (long)m_activeLayers.size() )
	{	
		long layerHandle = m_activeLayers[InitialPosition];

		m_activeLayers.erase( m_activeLayers.begin() + InitialPosition );

		long newPos = InitialPosition + 1;
		if( newPos > (long)m_activeLayers.size() )
			newPos = m_activeLayers.size();

		m_activeLayers.insert( m_activeLayers.begin() + newPos, layerHandle );

		_canUseLayerBuffer = FALSE;
		if( !m_lockCount )
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
	if( InitialPosition >= 0 && InitialPosition < (long)m_activeLayers.size() )
	{	
		long layerHandle = m_activeLayers[InitialPosition];
		m_activeLayers.erase( m_activeLayers.begin() + InitialPosition );

		long newPos = InitialPosition - 1;
		if( newPos < 0 )
			newPos = 0;

		m_activeLayers.insert( m_activeLayers.begin() + newPos, layerHandle );

		_canUseLayerBuffer = FALSE;
		if( !m_lockCount )
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

	if( InitialPosition >= 0 && InitialPosition < (long)m_activeLayers.size() &&  
		TargetPosition >= 0 && TargetPosition < (long)m_activeLayers.size())
	{
		long layerHandle = m_activeLayers[InitialPosition];

		m_activeLayers.erase( m_activeLayers.begin() + InitialPosition );
		m_activeLayers.insert( m_activeLayers.begin() + TargetPosition, layerHandle );

		_canUseLayerBuffer = FALSE;
		if( !m_lockCount )
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
	if( InitialPosition >= 0 && InitialPosition < (long)m_activeLayers.size() )
	{	
		long layerHandle = m_activeLayers[InitialPosition];
		m_activeLayers.erase( m_activeLayers.begin() + InitialPosition );
		m_activeLayers.push_back(layerHandle);

		_canUseLayerBuffer = FALSE;
		if( !m_lockCount )
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
	if( InitialPosition >= 0 && InitialPosition < (long)m_activeLayers.size() )
	{	
		long layerHandle = m_activeLayers[InitialPosition];
		m_activeLayers.erase( m_activeLayers.begin() + InitialPosition );
		m_activeLayers.push_front(layerHandle);

		_canUseLayerBuffer = FALSE;
		if( !m_lockCount )
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
		Layer * l = m_allLayers[LayerHandle];
		CString newFile = newSrcPath;
		VARIANT_BOOL rt;
		if (l->type == ShapefileLayer)
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
		else if(l->type == ImageLayer)
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
		if( !m_lockCount )
			InvalidateControl();
	}
	else
	{	m_lastErrorCode = tkINVALID_LAYER_HANDLE;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
	}
}

// ****************************************************************** 
//		LayerMaxVisibleScale
// ****************************************************************** 
DOUBLE CMapView::GetLayerMaxVisibleScale(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
		if (newVal < 0) newVal = 0;
		if (newVal > 18) newVal = 18;
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer* layer = m_allLayers[LayerHandle];
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
int CMapView::DeserializeLayerCore(CPLXMLNode* node, CStringW ProjectName, IStopExecution* callback)
{
	const char* nameA = CPLGetXMLValue( node, "Filename", NULL );
	
	CStringW filename = Utility::ConvertFromUtf8(nameA);

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
	else
	{
		// unsupported layer type
		return -1;
	}

	if(layerHandle != -1) 
	{
		s = CPLGetXMLValue( node, "LayerName", NULL );
		m_allLayers[layerHandle]->name = Utility::ConvertFromUtf8(s);

		s = CPLGetXMLValue( node, "DynamicVisibility", NULL );
		m_allLayers[layerHandle]->dynamicVisibility = (s != "") ? (atoi(s) == 0 ? false : true) : false;
		
		s = CPLGetXMLValue( node, "MaxVisibleScale", NULL );
		m_allLayers[layerHandle]->maxVisibleScale = (s != "") ? Utility::atof_custom (s) : 100000000.0;	// TODO: use constant

		s = CPLGetXMLValue( node, "MinVisibleScale", NULL );
		m_allLayers[layerHandle]->minVisibleScale = (s != "") ? Utility::atof_custom (s) : 0.0;

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
	
	if (LayerHandle < 0 || LayerHandle >= (long)m_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return NULL;
	}
		
	CPLXMLNode* psLayer = CPLCreateXMLNode(NULL, CXT_Element, "Layer");
	if (psLayer)
	{
		CString s;
		Layer* layer = m_allLayers[LayerHandle];
		if (layer)
		{
			switch (layer->type)
			{
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
					node = ((CShapefile*)sf)->SerializeCore(VARIANT_TRUE, "ShapefileClass");
					sf->Release();
				}
				else
				{
					BSTR bstr;
					img->get_SourceFilename(&bstr);
					CStringW sourceName = Utility::GetNameFromPath(OLE2W(bstr));
					Utility::CPLCreateXMLAttributeAndValue( psLayer, "SourceName", sourceName);

					node = ((CImageClass*)img)->SerializeCore(VARIANT_FALSE, "ImageClass");
					
					img->Release();
				}
				
				Utility::CPLCreateXMLAttributeAndValue( psLayer, "Filename", Filename);
				if (node)
				{
					CPLAddXMLChild(psLayer, node);
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
	if (LayerHandle < 0 || LayerHandle >= (long)m_allLayers.size())
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
	Layer* layer = m_allLayers[LayerHandle];
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
			m_allLayers[LayerHandle]->flags |= Visible;
		else
			m_allLayers[LayerHandle]->flags = m_allLayers[LayerHandle]->flags & ( 0xFFFFFFFF ^ Visible );
	}

	s = CPLGetXMLValue( node, "DynamicVisibility", NULL );
	m_allLayers[LayerHandle]->dynamicVisibility = (s != "") ? (atoi(s) == 0 ? false : true) : false;
	
	s = CPLGetXMLValue( node, "MaxVisibleScale", NULL );
	m_allLayers[LayerHandle]->maxVisibleScale = (s != "") ? Utility::atof_custom(s) : 100000000.0;	// todo use constant

	s = CPLGetXMLValue( node, "MinVisibleScale", NULL );
	m_allLayers[LayerHandle]->minVisibleScale = (s != "") ? Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue( node, "LayerKey", NULL );
	this->SetLayerKey(LayerHandle, s);

	s = CPLGetXMLValue( node, "LayerDescription", NULL );
	this->SetLayerDescription(LayerHandle, s);

	bool retVal = false;
	if (layerType == ShapefileLayer)
	{
		IShapefile* sf = NULL;
		//layer->object->QueryInterface(IID_IShapefile,(void**)&sf);
		//if (sf)
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
		//layer->object->QueryInterface(IID_IImage,(void**)&img);
		//if (img)
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
//		get_LayerName()
// *********************************************************
BSTR CMapView::GetLayerFilename(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BSTR layerName = A2BSTR("");;

	if (layerHandle < 0 || layerHandle >= (long)m_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	else
	{
		// extracting object
		CComBSTR filename;
		Layer* layer = m_allLayers[layerHandle];
		if (layer)
		{
			IShapefile* sf = NULL;
			IImage* img = NULL;
			layer->QueryShapefile(&sf);
			layer->QueryImage(&img);

			if (sf)
			{
				tkShapefileSourceType shpSource;
				sf->get_SourceType(&shpSource);
				sf->get_Filename(&layerName);
				sf->Release();

				if (shpSource != sstDiskBased)
				{
					this->ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
				}
			}
			else if (img)
			{
				tkImageSourceType imgSource;
				img->get_SourceType(&imgSource);
				img->get_Filename(&layerName);
				img->Release();
				
				if (imgSource != istDiskBased && imgSource != istGDALBased)
				{
					this->ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
				}
			}
			else
			{
				this->ErrorMessage(tkINVALID_FOR_INMEMORY_OBJECT);
			}
		}
	}
	return layerName;
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
	CComBSTR filename("");
	filename = this->GetLayerFilename(LayerHandle);
	
	USES_CONVERSION;
	CString name = OLE2CA(filename);
	if (_stricmp(OptionsName, "") == 0)
	{
		return "";		// error code is in the function
	}

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
//		LoadLayerOptions()
// *********************************************************
VARIANT_BOOL CMapView::LoadLayerOptions(LONG LayerHandle, LPCTSTR OptionsName, BSTR* Description)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CComBSTR filename;
	filename = this->GetLayerFilename(LayerHandle);
	
	// constructing name
	USES_CONVERSION;
	CString baseName = OLE2CA(filename);
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
#pragma endregion

// *******************************************************
//		GetLayerSkipOnSaving
// *******************************************************
VARIANT_BOOL CMapView::GetLayerSkipOnSaving(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (LayerHandle < 0 || LayerHandle >= (long)m_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return VARIANT_FALSE;
	}
	
	Layer* layer = m_allLayers[LayerHandle];
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

	if (LayerHandle < 0 || LayerHandle >= (long)m_allLayers.size())
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return;
	}

	Layer* layer = m_allLayers[LayerHandle];
	if (layer)
	{
		layer->skipOnSaving = newVal;
	}
}
