//********************************************************************************************************
//File name: Layer.h
//Description:  Header for layer.cpp.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//3-28-2005 dpa - Identical to public domain version.
//5-2-2005 dpa Added scale labels, shadow labels and visible labels
//9-30-2006 Stan - splitting Labels to separate class LabelLayer -> Label.h
//********************************************************************************************************
# ifndef LAYER_H
# define LAYER_H

# include "Label.h"
# include "BaseLayerInfo.h"
# include "OgrLoader.h"

class Layer;

// **************************************************
//		AsyncLoadingParams
// **************************************************
// A structure to pass parameters to the background thread
struct AsyncLoadingParams : CObject
{
	AsyncLoadingParams(IMapViewCallback* m, Extent e, Layer* l, vector<CategoriesData*>* ct, OgrLoadingTask* cback)
	{
		map = m;
		extents = e;
		layer = l;
		categories = ct;
		task = cback;
	};

	~AsyncLoadingParams()
	{
		if (categories)
		{
			for (size_t i = 0; i < categories->size(); i++) {
				delete (*categories)[i];
			}
			delete categories;
		}
	}

public:
	IMapViewCallback* map;
	Extent extents;
	Layer* layer;
	vector<CategoriesData*>* categories;
	OgrLoadingTask* task;  // not owned by this object
};

// **************************************************
//		Layer
// **************************************************
class Layer
{	
public:
	Layer()
		: _type(UndefinedLayer), _object(NULL), _flags(0), _asyncLoading(false)
	{	
		key = SysAllocString(L"");
		dynamicVisibility = false;
		maxVisibleScale = 100000000.0;
		minVisibleScale = 0.0;
		maxVisibleZoom = 20;
		minVisibleZoom = 0;
		description = "";
		skipOnSaving = VARIANT_FALSE;
		wasRendered = false;
	}

	~Layer()
	{
		::SysFreeString(key);

		if (_object != NULL) 
		{
			ULONG refcnt = _object->Release();
			//Debug::WriteLine("Releasing layer; Remaining ref count: %d", refcnt);
		}
	}	

private:
	long _flags;
	IDispatch * _object;
	bool _asyncLoading;
	LayerType _type;

public:	
	BSTR key;
	CStringW name;
	CString description;
	Extent extents;
	bool dynamicVisibility;
	double maxVisibleScale;
	double minVisibleScale;
	int maxVisibleZoom;
	int minVisibleZoom;
	VARIANT_BOOL skipOnSaving;
	bool wasRendered;

public:	
	// properties
	void put_Visible(bool value) { _flags = value ? _flags | Visible : _flags & (0xFFFFFFFF ^ Visible); }
	bool get_Visible() { return _flags & Visible; }
	IDispatch* get_Object() { return _object; }
	void set_Object(IDispatch* value)  { _object = value; }
	LayerType get_LayerType() { return _type; }
	void put_LayerType(LayerType value) { _type = value; }
	void put_AsyncLoading(bool value) { _asyncLoading = value; }
	bool IsAsyncLoading() { return _asyncLoading; }
	bool IsShapefile() { return(_type == ShapefileLayer || _type == OgrLayerSource); }
	bool IsImage() { return _type == ImageLayer; }
	bool IsOgrLayer() { return _type == OgrLayerSource; }
	bool IsWmsLayer() { return _type == WmsLayerSource; }
	bool IsInMemoryShapefile();
	bool IsDynamicOgrLayer();
	ILabels* get_Labels();
	IGeoProjection* GetGeoProjection();
	OgrDynamicLoader* GetOgrLoader();

public:
	// methods
	bool QueryShapefile(IShapefile** sf);
	bool QueryImage(IImage** img);
	bool QueryOgrLayer(IOgrLayer** ogrLayer);
	bool QueryWmsLayer(IWmsLayer** wmsLayer);
	bool IsVisible(double scale, int zoom);
	void GetExtentsAsNewInstance(IExtents** extents);
	bool UpdateExtentsFromDatasource();
	void LoadAsync(IMapViewCallback * mapView, Extent extents, long layerHandle, bool bForce);
	void UpdateShapefile(long layerHandle);
	void CloseDatasources();
	bool IsEmpty();
	bool IsDiskBased();
	void GrabLayerNameFromDatasource();
	BSTR GetFilename();
	bool PointWithinExtents(double projX, double projY);
	void OnRemoved();
};
# endif