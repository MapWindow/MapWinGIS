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

// A structure to pass parameters to the background thread
struct AsyncLoadingParams : CObject
{
	IMapViewCallback* map;
	Extent extents;
	Layer* layer;
	vector<CategoriesData*>* categories;

	AsyncLoadingParams(IMapViewCallback* m, Extent e, Layer* l, vector<CategoriesData*>* ct)
	{
		map = m;
		extents = e;
		layer = l;
		categories = ct;
	};
};

class Layer
{	
public:
	Layer()
	{	
		USES_CONVERSION;
		type = UndefinedLayer;
		object = NULL;
		flags = 0;
		key = A2BSTR("");
		dynamicVisibility = false;
		maxVisibleScale = 100000000.0;
		minVisibleScale = 0.0;
		maxVisibleZoom = 18;
		minVisibleZoom = 0;
		description = "";
		skipOnSaving = VARIANT_FALSE;
		wasRendered = false;
		_asyncLoading = false;
	}

	~Layer()
	{
		::SysFreeString(key);
		if (object != NULL) 
		{
			ULONG refcnt = object->Release();
			Debug::WriteLine("Releasing layer datasource; Remaining ref count: %d", refcnt);
		}
		object = NULL;
	}	
	
	CString description;
	LayerType type;
	IDispatch * object;
	Extent extents;
	long flags;
	BSTR key;
	CStringW name;
	bool dynamicVisibility;
	double maxVisibleScale;
	double minVisibleScale;
	int maxVisibleZoom;
	int minVisibleZoom;
	VARIANT_BOOL skipOnSaving;
	bool wasRendered;
	bool _asyncLoading;
	vector<CWinThread*> _threads;
	
	OgrDynamicLoader* GetOgrLoader();
	bool IsAsyncLoading() {return _asyncLoading; }
	LayerType GetLayerType() { return type; }
	bool IsShapefile() { return(type == ShapefileLayer || type == OgrLayerSource);	}
	bool IsImage() { return type == ImageLayer;	}
	bool IsOgrLayer() { return type == OgrLayerSource; }
	bool IsInMemoryShapefile();
	bool IsDynamicOgrLayer();
	bool QueryShapefile(IShapefile** sf);
	bool QueryImage(IImage** img);
	bool QueryOgrLayer(IOgrLayer** ogrLayer);
	bool IsVisible(double scale, int zoom);
	ILabels* get_Labels();
	IGeoProjection* GetGeoProjection();
	bool UpdateExtentsFromDatasource();
	void LoadAsync(IMapViewCallback* mapView, Extent extents);
	void UpdateShapefile();
	void CloseDatasources();
	bool IsEmpty();
};
# endif