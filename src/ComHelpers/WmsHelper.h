#pragma once
#include "WmsLayer.h"
class WmsHelper
{
public:
	static CWmsLayer* Cast(CComPtr<IWmsLayer> p);
	static bool GetServerBounds(IWmsLayer* layer, Extent& extent);
};

