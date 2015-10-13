#include "stdafx.h"
#include "WmsHelper.h"

// ****************************************************
//		Cast()
// ****************************************************
CWmsLayer* WmsHelper::Cast(CComPtr<IWmsLayer> p)
{
	return (CWmsLayer*)&(*p);
}


