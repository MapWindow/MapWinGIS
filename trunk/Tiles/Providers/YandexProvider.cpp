#include "stdafx.h"
#include "YandexProvider.h"
#include "map.h"

// Covers the situation when some specific initialization is needed 
// like requesting parameters from client
//void YandexBaseProvider::Initialize()
//{
//	if (!m_initialized)
//	{
//		CMapView* mapView = (CMapView*)this->mapView;
//		BSTR retVal;
//		retVal = A2BSTR("");
//		CComVariant var;
//		var.vt = VT_I4;
//		var.lVal = 4;
//		//mapView->FireParameterRequested("tiles", "GoogleServer", var, &retVal);
//		m_initialized = true;
//	}
//}