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

class Layer
{	
public:
	Layer()
	{	USES_CONVERSION;

		type = UndefinedLayer;
		object = NULL;
		//addInfo = NULL;		
		flags = 0;
		key = A2BSTR("");
		dynamicVisibility = false;
		maxVisibleScale = 100000000.0;
		minVisibleScale = 0.0;
		maxVisibleZoom = 18;
		minVisibleZoom = 0;
		description = "";
		skipOnSaving = VARIANT_FALSE;
	}

	~Layer()
	{
		::SysFreeString(key);
		if( object != NULL )
			int refcnt = object->Release();
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

	bool IsInMemoryShapefile()
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

	bool IsShapefile()
	{
		return(type == ShapefileLayer || type == OgrLayerSource);
	}

	bool IsImage()
	{
		return type == ImageLayer;
	}

	bool QueryShapefile(IShapefile** sf)
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

	bool QueryImage(IImage** img)
	{
		if (!this->object) return false;
		this->object->QueryInterface(IID_IImage, (void**)img);
		return (*img) != NULL;
	}

	bool QueryOgrLayer(IOgrLayer** ogrLayer)
	{
		if (!this->object) return false;
		this->object->QueryInterface(IID_IOgrLayer, (void**)ogrLayer);
		return (*ogrLayer) != NULL;
	}

	// Returns the visibility of layer considering dynamic visibility as well
	bool IsVisible(double scale, int zoom)
	{
		if (this->flags & Visible)
		{
			if (this->dynamicVisibility)
			{
				return (scale >= minVisibleScale && scale <= maxVisibleScale &&
						zoom >= minVisibleZoom && zoom <= maxVisibleZoom );
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
	ILabels* get_Labels()
	{
		ILabels* labels = NULL;
		if(this->IsShapefile())
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
	IGeoProjection* GetGeoProjection()
	{
		IGeoProjection* gp = NULL;
		if(this->IsShapefile())
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
	void UpdateExtentsFromDatasource()
	{
		if(this->IsShapefile())
		{
			IShapefile * sf = NULL;
			if (this->QueryShapefile(&sf))
			{
				IExtents * box = NULL;
				sf->get_Extents(&box);
				double xm,ym,zm,xM,yM,zM;
				box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
				this->extents = Extent(xm,xM,ym,yM);
				box->Release();
				box = NULL;
				sf->Release();
			}
		}
		else if (this->IsImage())
		{
			IImage * img = NULL;
			if (this->QueryImage(&img))
			{
				double xllCenter = 0, yllCenter = 0, dx = 0, dy = 0;
				long height = 0, width = 0;
				img->get_OriginalXllCenter(&xllCenter);
				img->get_OriginalYllCenter(&yllCenter);
				img->get_OriginalHeight(&height);
				img->get_OriginalWidth(&width);
				img->get_OriginalDX(&dx);
				img->get_OriginalDY(&dy);
				this->extents = Extent( xllCenter, xllCenter + dx*width, yllCenter, yllCenter + dy*height );
				img->Release();
			}
		}
	}
};


# endif