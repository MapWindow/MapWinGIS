#include "stdafx.h"
#include "WmsHelper.h"

// ****************************************************
//		Cast()
// ****************************************************
CWmsLayer* WmsHelper::Cast(CComPtr<IWmsLayer> p)
{
	return (CWmsLayer*)&(*p);
}

// ****************************************************
//		GetServerBounds()
// ****************************************************
bool WmsHelper::GetServerBounds(IWmsLayer* layer, Extent& extent)
{
	WmsCustomProvider* provider = WmsHelper::Cast(layer)->get_InnerProvider();
	if (provider)
	{
		CustomProjection* p = provider->get_CustomProjection();
		if (p)
		{
			// this bounds will be update during each loading of the layer based on map projection;
			// if no loading was done, the bounds will be invalid (all values set to zero)
			extent = p->MapBounds;
			return true;
		}
	}

	return false;
}


