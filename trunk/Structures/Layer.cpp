#include "stdafx.h"
#include "Layer.h"
#include "OgrLayer.h"
#include "OgrHelper.h"

// ****************************************************
//		IsInMemoryShapefile()
// ****************************************************
bool Layer::IsInMemoryShapefile()
{
	if (type == OgrLayerSource) return true;
	if (type == ImageLayer) return false;
	if (type == ShapefileLayer)
	{
		CComPtr<IShapefile> sf = NULL;
		QueryShapefile(&sf);
		if (sf)
		{
			tkShapefileSourceType sourceType;
			sf->get_SourceType(&sourceType);
			return sourceType == sstInMemory;
		}
		return false;
	}
	return false;
}

// ****************************************************
//		IsDynamicOgrLayer()
// ****************************************************
bool Layer::IsDynamicOgrLayer()
{
	if (type != OgrLayerSource) return false;
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
//		QueryShapefile()
// ****************************************************
bool Layer::QueryShapefile(IShapefile** sf)
{
	if (!this->object) return false;
	this->object->QueryInterface(IID_IShapefile, (void**)sf);
	if (!(*sf))
	{
		// in case of OGR, we will return underlying shapefile
		IOgrLayer* ogr = NULL;
		this->object->QueryInterface(IID_IOgrLayer, (void**)&ogr);
		if (ogr)
		{
			ogr->GetData(sf);
			ogr->Release();
		}
	}
	return (*sf) != NULL;
}

// ****************************************************
//		QueryImage()
// ****************************************************
bool Layer::QueryImage(IImage** img)
{
	if (!this->object) return false;
	this->object->QueryInterface(IID_IImage, (void**)img);
	return (*img) != NULL;
}

// ****************************************************
//		QueryOgrLayer()
// ****************************************************
bool Layer::QueryOgrLayer(IOgrLayer** ogrLayer)
{
	if (!this->object) return false;
	this->object->QueryInterface(IID_IOgrLayer, (void**)ogrLayer);
	return (*ogrLayer) != NULL;
}


// ****************************************************
//		IsVisible()
// ****************************************************
// Returns the visibility of layer considering dynamic visibility as well
bool Layer::IsVisible(double scale, int zoom)
{
	if (this->flags & Visible)
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
	if (this->IsShapefile())
	{
		IShapefile * sf = NULL;
		if (this->QueryShapefile(&sf))
		{
			// simply grab the object from sf
			sf->get_GeoProjection(&gp);
			sf->Release(); sf = NULL;
		}
	}
	else if (this->IsImage())
	{
		IImage * img = NULL;
		if (this->QueryImage(&img))
		{
			// there is no GeoProjection object; so create it from the string
			CComBSTR bstr;
			img->GetProjection(&bstr);
			if (bstr.Length() > 0)
			{
				VARIANT_BOOL vb;
				GetUtils()->CreateInstance(tkInterface::idGeoProjection, (IDispatch**)&gp);
				gp->ImportFromAutoDetect(bstr, &vb);
				if (!vb)
				{
					gp->Release();
					gp = NULL;
				}
			}
			img->Release(); img = NULL;
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
		IImage * iimg = NULL;
		if (!this->QueryImage(&iimg)) return false;
		double xllCenter = 0, yllCenter = 0, dx = 0, dy = 0;
		long width = 0, height = 0;

		iimg->get_OriginalXllCenter(&xllCenter);
		iimg->get_OriginalYllCenter(&yllCenter);
		iimg->get_OriginalDX(&dx);
		iimg->get_OriginalDY(&dy);
		iimg->get_OriginalWidth(&width);
		iimg->get_OriginalHeight(&height);
		this->extents = Extent(xllCenter, xllCenter + dx*width, yllCenter, yllCenter + dy*height);
		iimg->Release();
		iimg = NULL;
		return TRUE;
	}
	else if (IsDynamicOgrLayer())
	{
		// regular OGR layers will use underlying shapefile to get extents
		CComPtr<IOgrLayer> ogr = NULL;
		if (!this->QueryOgrLayer(&ogr)) return FALSE;
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
	else if (IsShapefile())
	{
		IShapefile * ishp = NULL;
		if (!this->QueryShapefile(&ishp)) return FALSE;
		IExtents * box = NULL;
		ishp->get_Extents(&box);
		double xm, ym, zm, xM, yM, zM;
		box->GetBounds(&xm, &ym, &zm, &xM, &yM, &zM);
		this->extents = Extent(xm, xM, ym, yM);
		box->Release();
		box = NULL;
		ishp->Release();
		ishp = NULL;
		return TRUE;
	}
	else return FALSE;
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
		return 0;
	}
	else
	{
		Layer* layer = options->layer;
		if (layer)
		{
			OgrDynamicLoader* loader = &layer->_loader;
		
			Debug::WriteWithThreadId("New task");
			loader->AddWaitingTask();		// stop current loading task

			loader->LockLoading(true);	// next one is allowed after previous is finished
			Debug::WriteWithThreadId("Acquired lock");

			loader->ReleaseWaitingTask();   // provide the way for the next one to stop it
			if (loader->HaveWaitingTasks()) 
			{
				Debug::WriteWithThreadId("Task was canceled");
				loader->LockLoading(false);
				return 0;    // more of tasks further down the road, don't even start this
			}

			layer->_asyncLoading = true;
			IOgrLayer* ogr = NULL;
			layer->QueryOgrLayer(&ogr);
			if (ogr) 
			{
				OGRLayer * ds = ((COgrLayer*)ogr)->GetDatasource();
				bool success = OgrHelper::Layer2RawData(ds, &options->extents, &options->layer->_loader);

				// just quietly delete it for now
				if (!success) {
					options->layer->_loader.Clear();
				}

				if (success) {
					options->map->_Redraw(RedrawAll, false, false);
				}
			}
			loader->LockLoading(false);
			Debug::WriteWithThreadId("Lock released. \n");
			layer->_asyncLoading = false;
		}
		
		delete options;
		return 1;
	}
}

// ****************************************************
//		LoadAsync()
// ****************************************************
void Layer::LoadAsync(IMapViewCallback* callback, Extent extents)
{
	if (IsDynamicOgrLayer()) 
	{
		if (extents == _loader.LastExtents) return;   // definitely no need to run it twice
		_loader.LastExtents = extents;

		// if larger extents were requested previously and features were loaded, skip the new request
		if (extents.Within(_loader.LastSuccessExtents)) return;	   
		
		AsyncLoadingParams* param = new AsyncLoadingParams(callback, extents, this);
		_thread = AfxBeginThread(OgrAsyncLoadingThreadProc, (LPVOID)param);
	}
}

//***********************************************************************
//*		UpdateShapefile()
//***********************************************************************
void Layer::UpdateShapefile()
{
	if (_loader.Data.size() > 0) 
	{
		CComPtr<IShapefile> sf = NULL;
		if (QueryShapefile(&sf)) 
		{
			VARIANT_BOOL vb;
			sf->EditClear(&vb);
			ShpfileType shpType;
			sf->get_ShapefileType(&shpType);
		
			Debug::WriteWithThreadId("Update shapefile: %d\n", _loader.Data.size());
			
			// copy to temp vector to minimize locking time			
			_loader.LockData(true);
			vector<ShapeRecordData*> data;
			data.insert(data.end(), _loader.Data.begin(), _loader.Data.end());
			_loader.Data.clear();
			_loader.LockData(false);

			ITable* itable = NULL;
			sf->get_Table(&itable);

			if (itable) {
				CTableClass* tbl = (CTableClass*)itable;
				long count = 0;
				for (size_t i = 0; i < data.size(); i++)
				{
					IShape* shp = NULL;
					GetUtils()->CreateInstance(tkInterface::idShape, (IDispatch**)&shp);
					if (shp)
					{
						shp->Create(shpType, &vb);
						shp->ImportFromBinary(data[i]->Shape, &vb);
						sf->EditInsertShape(shp, &count, &vb);
						tbl->UpdateTableRow(data[i]->Row, count);
						data[i]->Row = NULL;   // we no longer own it; it'll be cleared by Shapefile.EditClear
						count++;
					}
				}
				itable->Release();
			}

			// clean the data
			for (size_t i = 0; i < data.size(); i++) {
				delete data[i];
			}
		}
	}
}
