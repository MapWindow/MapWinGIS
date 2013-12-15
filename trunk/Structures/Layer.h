//********************************************************************************************************
//File name: Layer.h
//Description:  Header for layer.cpp.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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

# include <deque>
# include "enumerations.h"
# include "Label.h"
# include "BaseLayerInfo.h"

// Chris Michaelis Oct 24 2005
# include "extent.h"

//dpa 3/28/2005 added rotation and offset
class Layer //: public LabelLayer
{	
public:
	Layer()
	{	USES_CONVERSION;

		type = UndefinedLayer;
		object = NULL;
		addInfo = NULL;		
		flags = 0;
		key = A2BSTR("");
		name = A2BSTR("");
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
		::SysFreeString(name);
		if( object != NULL )
			int refcnt = object->Release();
		object = NULL;
		delete addInfo; // Was: casting and deleting depending on LayerType...
	}	
	
	CString description;
	LayerType type;
	IDispatch * object;
	//FILE * file;
	BaseLayerInfo* addInfo;
	Extent extents;
	long flags;
	BSTR key;
	BSTR name;
	bool dynamicVisibility;
	double maxVisibleScale;
	double minVisibleScale;
	int maxVisibleZoom;
	int minVisibleZoom;
	VARIANT_BOOL skipOnSaving;

	bool QueryShapefile(IShapefile** sf)
	{
		this->object->QueryInterface(IID_IShapefile, (void**)sf);
		return (*sf) != NULL;
	}

	bool QueryImage(IImage** img)
	{
		this->object->QueryInterface(IID_IImage, (void**)img);
		return (*img) != NULL;
	}

	// Returns the visiblity of layer considering dynamic visibility as well
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
		if(this->type == ShapefileLayer)
		{
			IShapefile * sf = NULL;
			//this->object->QueryInterface(IID_IShapefile, (void**)&sf);
			//if (sf != NULL)
			if (this->QueryShapefile(&sf))
			{
				sf->get_Labels(&labels);
				sf->Release(); sf = NULL;
			}
		}
		else if (this->type == ImageLayer)
		{
			IImage * img = NULL;
			//this->object->QueryInterface(IID_IImage, (void**)&img);
			//if (img != NULL)
			if (this->QueryImage(&img))
			{
				img->get_Labels(&labels);
				img->Release(); img = NULL;
			}
		}
		return labels;
	};
};


# endif