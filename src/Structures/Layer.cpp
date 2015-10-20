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

	if (IsWmsLayer())
	{
		CComPtr<IWmsLayer> wmsLayer = NULL;
		if (!QueryWmsLayer(&wmsLayer)) {
			return false;
		}
		
		WmsCustomProvider* provider = WmsHelper::Cast(wmsLayer)->get_InnerProvider();
		if (provider)
		{
			CustomProjection* p = provider->get_CustomProjection();
			if (p)
			{
				// this bounds will be update during each loading of the layer based on map projection;
				// if no loading was done, the bounds will be invalid (all values set to zero)
				this->extents = p->MapBounds;
			}
		}
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
		return 0;
	}
	
	Layer* layer = options->layer;
	if (layer)
	{
		OgrDynamicLoader* loader = layer->GetOgrLoader();
		
		Debug::WriteWithThreadId("New task", DebugOgrLoading);

		if (loader->AddWaitingTask())		// stop current loading task
		{
			loader->LockLoading(true);	// next one is allowed after previous is finished
			Debug::WriteWithThreadId("Acquired lock", DebugOgrLoading);

			loader->ReleaseWaitingTask();   // provide the way for the next one to stop it
			if (loader->HaveWaitingTasks()) 
			{
				Debug::WriteWithThreadId("Task was canceled", DebugOgrLoading);
				loader->LockLoading(false);
				delete options;
				return 0;    // more of tasks further down the road, don't even start this
			}

			layer->put_AsyncLoading(true);
			IOgrLayer* ogr = NULL;
			layer->QueryOgrLayer(&ogr);
			if (ogr) 
			{
				OGRLayer * ds = ((COgrLayer*)ogr)->GetDatasource();
				ULONG count = ogr->Release();
				IShapefile* sf = ((COgrLayer*)ogr)->GetShapefileNoRef();
					
				CComBSTR expr;
				ogr->get_LabelExpression(&expr);
				loader->LabelExpression = OLE2W(expr);
					
				bool success = Ogr2RawData::Layer2RawData(ds, &options->extents, loader, *options->categories, options->task);

				options->task->Finished = true;
				if (!success) {
					options->task->Cancelled = true;
					OgrLoadingTask* task = options->task;
					options->map->_FireBackgroundLoadingFinished(task->Id, task->LayerHandle, task->FeatureCount, 0);
				}

				// just quietly delete it for now
				if (!success) {
					loader->Clear();
				}

				if (success) {
					options->map->_Redraw(RedrawAll, false, false);
				}
			}
			loader->LockLoading(false);
			Debug::WriteWithThreadId("Lock released. \n", DebugOgrLoading);
			layer->put_AsyncLoading(false);
		}
	}
			
	delete options;
	return 1;
}

// ****************************************************
//		LoadAsync()
// ****************************************************
void Layer::LoadAsync(IMapViewCallback* mapView, Extent extents, long layerHandle)
{
	if (!IsDynamicOgrLayer())
		return;
	
	OgrDynamicLoader* loader = GetOgrLoader();
	if (!loader) return;

	if (extents == loader->LastExtents) return;   // definitely no need to run it twice
	loader->LastExtents = extents;

	// if larger extents were requested previously and features were loaded, skip the new request
	if (extents.Within(loader->LastSuccessExtents)) return;	   

	// get a copy of categories to apply them in the background thread
	vector<CategoriesData*>* data = new vector<CategoriesData*>();
		
	CComPtr<IShapefile> sf = NULL;
	this->QueryShapefile(&sf);
	if (sf) 
	{
		ShpfileType shpType = ShapefileHelper::GetShapeType(sf);
		loader->IsMShapefile = ShapeUtility::IsM(shpType);

		IShapefileCategories* categories = NULL;
		sf->get_Categories(&categories);
		if (categories) 
		{
			((CShapefileCategories*)categories)->GetCategoryData(*data);
			categories->Release();
		}
	}

	OgrLoadingTask* task = new OgrLoadingTask(layerHandle);
	loader->Queue.push(task);
	mapView->_FireBackgroundLoadingStarted(task->Id, layerHandle);

	AsyncLoadingParams* param = new AsyncLoadingParams(mapView, extents, this, data, task);
	CWinThread* thread = AfxBeginThread(OgrAsyncLoadingThreadProc, (LPVOID)param);
}

//***********************************************************************
//*		UpdateShapefile()
//***********************************************************************
void Layer::UpdateShapefile(long layerHandle)
{
	OgrDynamicLoader* loader = GetOgrLoader();
	if (!loader) return;

	// grab the data
	vector<ShapeRecordData*> data;
	loader->LockData(true);
	if (loader->Data.size() > 0) {
		data.insert(data.end(), loader->Data.begin(), loader->Data.end());
		loader->Data.clear();
	}
	loader->LockData(false);
	if (data.size() == 0) return;
	
	USES_CONVERSION;
	CComPtr<IShapefile> sf = NULL;
	if (QueryShapefile(&sf)) 
	{
		CSingleLock sfLock(&loader->ShapefileLock);
		sfLock.Lock();

		VARIANT_BOOL vb;
		sf->EditClear(&vb);
		ShpfileType shpType;
		sf->get_ShapefileType(&shpType);
		
		Debug::WriteWithThreadId(Debug::Format("Update shapefile: %d\n", data.size()), DebugOgrLoading);

		CComPtr<ITable> table = NULL;
		sf->get_Table(&table);

		CComPtr<ILabels> labels = NULL;
		sf->get_Labels(&labels);
		labels->Clear();

		if (table) 
		{
			CTableClass* tbl = TableHelper::Cast(table);

			long count = 0;
			for (size_t i = 0; i < data.size(); i++)
			{
				CComPtr<IShape> shp = NULL;
				ComHelper::CreateShape(&shp);
				if (shp)
				{
					shp->Create(shpType, &vb);
					shp->ImportFromBinary(data[i]->Shape, &vb);
					sf->EditInsertShape(shp, &count, &vb);
					sf->put_ShapeCategory(count, data[i]->CategoryIndex);
					
					tbl->UpdateTableRow(data[i]->Row, count);
					data[i]->Row = NULL;   // we no longer own it; it'll be cleared by Shapefile.EditClear
					
					if (data[i]->HasLabel()) {
						CComBSTR bstr(data[i]->LabelText);
						labels->AddLabel(bstr, data[i]->LabelX, data[i]->LabelY, data[i]->LabelRotation);
					}

					count++;
				}
			}
			ShapefileHelper::ClearShapefileModifiedFlag(sf);		// inserted shapes were marked as modified, correct this
		}

		sfLock.Unlock();
	}

	// clean the data
	for (size_t i = 0; i < data.size(); i++) {
		delete data[i];
	}
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


