#include "stdafx.h"
#include "Layer.h"
#include "OgrLayer.h"
#include "OgrHelper.h"
#include "Ogr2RawData.h"
#include "ShapefileCategories.h"
#include "TableHelper.h"
#include "ShapefileHelper.h"
#include "WmsLayer.h"
#include "WmsHelper.h"

// ****************************************************
//		CloseDatasources()
// ****************************************************
void Layer::CloseDatasources()
{
	// we don't release objects here, it's done in Layer destructor
	VARIANT_BOOL vb;
	if (IsOgrLayer())
	{
		CComPtr<IOgrLayer> layer = NULL;
		if (QueryOgrLayer(&layer)) {
			layer->Close();
		}
	}
	else if (IsShapefile())
	{
		CComPtr<IShapefile> ishp = NULL;
		if (QueryShapefile(&ishp)) {
			ishp->Close(&vb);
		}
	}
	else if (IsWmsLayer())
	{
		CComPtr<IWmsLayer> wms = NULL;
		if (QueryWmsLayer(&wms)) {
			wms->Close();
		}
	}

	CComPtr<IImage> iimg = NULL;
	if (QueryImage(&iimg)) {
		iimg->Close(&vb);
	}

	CComPtr<IGrid> igrid = NULL;
	_object->QueryInterface(IID_IGrid, (void**)&igrid);
	if (igrid != NULL) {
		igrid->Close(&vb);
	}
}

// ****************************************************
//		OnRemoved()
// ****************************************************
void Layer::OnRemoved()
{ 
	if (IsWmsLayer())
	{
		CComPtr<IWmsLayer> wms = NULL;
		if (QueryWmsLayer(&wms)) 
		{
			WmsHelper::Cast(wms)->ResizeBuffer(0, 0);
		}
	}
}

// ****************************************************
//		IsInMemoryShapefile()
// ****************************************************
bool Layer::IsInMemoryShapefile()
{
	if (_type == OgrLayerSource) return true;
	if (_type == ImageLayer) return false;
	if (_type == ShapefileLayer)
	{
		CComPtr<IShapefile> sf = NULL;
		if (QueryShapefile(&sf))
		{
			tkShapefileSourceType sourceType;
			sf->get_SourceType(&sourceType);
			return sourceType == sstInMemory;
		}
	}
	return false;
}

// ****************************************************
//		IsDiskBased()
// ****************************************************
bool Layer::IsDiskBased()
{
	if (_type == ImageLayer)
		return true;

	if (_type == OgrLayerSource)
	{
		CComPtr<IOgrLayer> layer = NULL;
		if (QueryOgrLayer(&layer))
			return OgrHelper::GetSourceType(layer) == ogrFile;
	}
	else if (_type == ShapefileLayer) 
	{
		CComPtr<IShapefile> sf = NULL;
		if (QueryShapefile(&sf))
			return ShapefileHelper::GetSourceType(sf) == sstDiskBased;
	}
	return false;
}

// ****************************************************
//		IsDynamicOgrLayer()
// ****************************************************
bool Layer::IsDynamicOgrLayer()
{
	if (_type != OgrLayerSource) return false;
	CComPtr<IOgrLayer> layer = NULL;
	if (QueryOgrLayer(&layer))
	{
		VARIANT_BOOL vb;
		layer->get_DynamicLoading(&vb);
		return vb ? true : false;
	}
	return false;
}

// ****************************************************
//		GetOgrLoader()
// ****************************************************
OgrDynamicLoader* Layer::GetOgrLoader()
{
	if (_type != OgrLayerSource) return NULL;
	OgrDynamicLoader* loader = NULL;
	IOgrLayer* layer = NULL;
	if (QueryOgrLayer(&layer))
	{
		loader = ((COgrLayer*)layer)->GetDynamicLoader();
		layer->Release();
	}
	return loader;
}

// ****************************************************
//		QueryShapefile()
// ****************************************************
bool Layer::QueryShapefile(IShapefile** sf)
{
	if (!_object) return false;
	_object->QueryInterface(IID_IShapefile, (void**)sf);
	if (!(*sf))
	{
		// in case of OGR, we will return underlying shapefile
		CComPtr<IOgrLayer> ogr = NULL;
		_object->QueryInterface(IID_IOgrLayer, (void**)&ogr);
		if (ogr)
		{
			ogr->GetBuffer(sf);
		}
	}
	return (*sf) != NULL;
}

// ****************************************************
//		QueryImage()
// ****************************************************
bool Layer::QueryImage(IImage** img)
{
	if (!_object) return false;
	_object->QueryInterface(IID_IImage, (void**)img);
	return (*img) != NULL;
}

// ****************************************************
//		QueryOgrLayer()
// ****************************************************
bool Layer::QueryOgrLayer(IOgrLayer** ogrLayer)
{
	if (!_object) return false;
	_object->QueryInterface(IID_IOgrLayer, (void**)ogrLayer);
	return (*ogrLayer) != NULL;
}

// ****************************************************
//		QueryWmsLayer()
// ****************************************************
bool Layer::QueryWmsLayer(IWmsLayer** wmsLayer)
{
	if (!_object) return false;
	_object->QueryInterface(IID_IWmsLayer, (void**)wmsLayer);
	return (*wmsLayer) != NULL;
}

// ****************************************************
//		IsVisible()
// ****************************************************
// Returns the visibility of layer considering dynamic visibility as well
bool Layer::IsVisible(double scale, int zoom)
{
	if (this->_flags & Visible)
	{
		if (this->dynamicVisibility)
		{
			return (scale >= minVisibleScale && scale <= maxVisibleScale &&
				zoom >= minVisibleZoom && zoom <= maxVisibleZoom);
		}
		else
			return true;
	}
	else
		return false;
}

// ---------------------------------------------------------
//	 Returning labels of the underlying object
// ---------------------------------------------------------
ILabels* Layer::get_Labels()
{
	ILabels* labels = NULL;
	if (this->IsShapefile())
	{
		IShapefile * sf = NULL;
		if (this->QueryShapefile(&sf))
		{
			sf->get_Labels(&labels);
			sf->Release(); sf = NULL;
		}
	}
	else if (this->IsImage())
	{
		IImage * img = NULL;
		if (this->QueryImage(&img))
		{
			img->get_Labels(&labels);
			img->Release(); img = NULL;
		}
	}
	return labels;
};

// ---------------------------------------------------------
//	 Returning projection of underlying object
// ---------------------------------------------------------
IGeoProjection* Layer::GetGeoProjection()
{
	IGeoProjection* gp = NULL;
	if (IsShapefile())
	{
		CComPtr<IShapefile> sf = NULL;
		if (QueryShapefile(&sf))
		{
			sf->get_GeoProjection(&gp);
		}
	}
	else if (IsImage())
	{
		CComPtr<IImage> img = NULL;
		if (QueryImage(&img))
		{
			img->get_GeoProjection(&gp);
		}
	}
	else if (IsWmsLayer())
	{
		CComPtr<IWmsLayer> wms = NULL;
		if (QueryWmsLayer(&wms))
		{
			wms->get_GeoProjection(&gp);
		}
	}

	return gp;
}

// ---------------------------------------------------------
//	 UpdateExtentsFromDatasource
// ---------------------------------------------------------
bool Layer::UpdateExtentsFromDatasource()
{
	if (IsImage())
	{
		CComPtr<IImage> iimg = NULL;
		if (!this->QueryImage(&iimg)) return false;

		double xllCenter = 0, yllCenter = 0, dx = 0, dy = 0;
		long width = 0, height = 0;

		iimg->get_OriginalXllCenter(&xllCenter);
		iimg->get_OriginalYllCenter(&yllCenter);
		iimg->get_OriginalDX(&dx);
		iimg->get_OriginalDY(&dy);
		iimg->get_OriginalWidth(&width);
		iimg->get_OriginalHeight(&height);
		this->extents = Extent(xllCenter - dx / 2, xllCenter - dx / 2 + dx*width, yllCenter - dy /2, yllCenter - dy / 2 + dy*height);
		
		return true;
	}
	
	if (IsDynamicOgrLayer())
	{
		// regular OGR layers will use underlying shapefile to get extents
		CComPtr<IOgrLayer> ogr = NULL;
		if (!this->QueryOgrLayer(&ogr)) return false;
		
		CComPtr<IExtents> box = NULL;
		VARIANT_BOOL vb;
		ogr->get_Extents(&box, VARIANT_FALSE, &vb);
		if (vb && box) {
			double xm, ym, zm, xM, yM, zM;
			box->GetBounds(&xm, &ym, &zm, &xM, &yM, &zM);
			this->extents = Extent(xm, xM, ym, yM);
		}

		return true;
	}

	if (IsShapefile())
	{
		CComPtr<IShapefile> ishp = NULL;
		if (!this->QueryShapefile(&ishp)) return false;

		CComPtr<IExtents> box = NULL;
		ishp->get_Extents(&box);
		double xm, ym, zm, xM, yM, zM;
		box->GetBounds(&xm, &ym, &zm, &xM, &yM, &zM);
		this->extents = Extent(xm, xM, ym, yM);

		return true;
	}


	return FALSE;
}

// ****************************************************
//		OgrAsyncLoadingThreadProc()
// ****************************************************
UINT OgrAsyncLoadingThreadProc(LPVOID pParam)
{
	UINT res = 0;
	AsyncLoadingParams* options = (AsyncLoadingParams*)pParam;
	if (!options || !options->IsKindOf(RUNTIME_CLASS(AsyncLoadingParams)))
	{
		// if pObject is not valid; return 0
		if (options) delete options;
		return 0;
	}
	
	Layer* layer = options->layer;
	if (!layer)
	{
		delete options;
		return 0;
	}
	OgrDynamicLoader* loader = layer->GetOgrLoader();
		
	Debug::WriteWithThreadId("Processing new loading task", DebugOgrLoading);
	if (loader->SignalWaitingTask())		// signal previously started thread
	{
		// Locking loading in this section
		CSingleLock lock(&loader->LoadingLock, TRUE);
		Debug::WriteWithThreadId("Acquired loading lock - ", DebugOgrLoading);

		loader->ReleaseWaitingTask();   // provide the way for the next one to stop it
		if (loader->HaveWaitingTasks()) // check if there is a new thread waiting
		{
			Debug::WriteWithThreadId("Task was canceled", DebugOgrLoading);
			delete options; // clear memory
			return 0;    // more of tasks further down the road, don't even start this
		}

		layer->put_AsyncLoading(true);
		IOgrLayer* ogr = NULL;
		layer->QueryOgrLayer(&ogr);
		if (ogr)
		{
			OGRLayer * ds = ((COgrLayer*)ogr)->GetDatasource();
			ULONG count = ogr->Release();

			bool success = Ogr2RawData::Layer2RawData(ds, &options->extents, loader, options->task);

            // Fire event for this task:
            OgrLoadingTask* task = options->task;
            task->Finished = true;
            task->Cancelled = !success;
            options->map->_FireBackgroundLoadingFinished(task->Id, task->LayerHandle, task->FeatureCount, 0);

			loader->ClearFinishedTasks();

		}
		layer->put_AsyncLoading(false);
		Debug::WriteWithThreadId("Releasing loading lock. \n", DebugOgrLoading);
	}
			
	delete options;
	return 1;
}

// ****************************************************
//		LoadAsync()
// ****************************************************
void Layer::LoadAsync(IMapViewCallback* mapView, Extent extents, long layerHandle, bool bForce = false)
{
	if (!IsDynamicOgrLayer())
		return;
	
	OgrDynamicLoader* loader = GetOgrLoader();
	if (!loader) return;

	if (!bForce && extents == loader->LastExtents) return;   // definitely no need to run it twice
	loader->LastExtents = extents;

	// if larger extents were requested previously and features were loaded, skip the new request
	if (!bForce && extents.Within(loader->LastSuccessExtents)) return;

	// Prepare a new OgrLoadingTask & queue it for execution
	OgrLoadingTask* task = new OgrLoadingTask(layerHandle);
	loader->EnqueueTask(task);

	// First fire the event, then start the thread. 
	// This prevents race condition between the started & completed event.
	AsyncLoadingParams* param = new AsyncLoadingParams(mapView, extents, this, task);
	mapView->_FireBackgroundLoadingStarted(task->Id, layerHandle);
    OgrAsyncLoadingThreadProc(param);

	//CWinThread* thread = AfxBeginThread(OgrAsyncLoadingThreadProc, (LPVOID)param);
}

//***********************************************************************
//*		UpdateShapefile()
//***********************************************************************
void Layer::UpdateShapefile()
 {
	if (!IsDynamicOgrLayer())
		return;

    IOgrLayer* layer = NULL;
	if (!QueryOgrLayer(&layer))
		return;
    ((COgrLayer*)layer)->UpdateShapefileFromOGRLoader();
}

//****************************************************
//*		IsEmpty()
//****************************************************
bool Layer::IsEmpty()
{
	if (_object == NULL) return true;
	if (this->IsDynamicOgrLayer())return false;
	if (this->IsShapefile())
	{
		IShapefile * ishp = NULL;
		if (!this->QueryShapefile(&ishp)) return true;

		long numShapes;
		ishp->get_NumShapes(&numShapes);
		ishp->Release();
		if (numShapes == 0) return true;
	}
	return false;
}

//****************************************************
//*		GetExtentsAsNewInstance()
//****************************************************
void Layer::GetExtentsAsNewInstance(IExtents** box)
{
	ComHelper::CreateExtents(box);
	(*box)->SetBounds(extents.left, extents.bottom, 0.0, extents.right, extents.top, 0.0);
}

//****************************************************
//*		GrabLayerNameFromDatasource()
//****************************************************
void Layer::GrabLayerNameFromDatasource()
{
	if (this->name.GetLength() > 0)	return;

	if (IsOgrLayer())
	{
		CComPtr<IOgrLayer> layer = NULL;
		if (QueryOgrLayer(&layer))
		{
			CComBSTR name;
			layer->get_Name(&name);
			this->name = OLE2W(name);
		}
	}
	else
	{
		CComBSTR bstr;
		bstr.Attach(GetFilename());
		CStringW path = OLE2W(bstr);
		name = Utility::GetNameFromPathWoExtension(path);
	}
}

//****************************************************
//*		GetFilename()
//****************************************************
BSTR Layer::GetFilename()
{
	BSTR filename;	
	switch (this->_type)
	{
		case ShapefileLayer:
		{
			CComPtr<IShapefile> sf = NULL;
			if (QueryShapefile(&sf))
			{
				sf->get_Filename(&filename);
				return filename;
			}
			break;
		}
		case ImageLayer:
		{
			CComPtr<IImage> img = NULL;
			if (QueryImage(&img))
			{
				img->get_Filename(&filename);
				return filename;
			}
			break;
		}
		case OgrLayerSource:
		{
			CComPtr<IOgrLayer> ogr = NULL;
			if (QueryOgrLayer(&ogr))
			{
				if (OgrHelper::GetSourceType(ogr) == ogrFile)
				{
					ogr->GetConnectionString(&filename);
					return filename;
				}
			}
			break;
		}
	}
	return SysAllocString(L"");
}

//****************************************************
//*		PointWithinExtents()
//****************************************************
bool Layer::PointWithinExtents(double projX, double projY)
{
	return !(projX < extents.left || projX > extents.right || projY < extents.bottom || projY > extents.top);
}


