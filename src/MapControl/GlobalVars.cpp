#include "stdafx.h"
#include "GlobalVars.h"

#ifdef _DEBUG
CMemLeakDetect gMemLeakDetect;		// comment this line to turn off the memory leaking detection tools	
#endif

GlobalSettingsInfo m_globalSettings;
ReferenceCounter gReferenceCounter;
IUtils* m_utils;
IShapefile* m_shapefile;
char* UTF8_ENCODING_MARKER = "utf8";

// *****************************************************************
//		getGeosHandle
// *****************************************************************
#ifdef GEOS_NEW
GEOSContextHandle_t _geosContextHandle = nullptr;
GEOSContextHandle_t getGeosHandle()
{
	if (!_geosContextHandle)
		_geosContextHandle = OGRGeometry::createGEOSContext();
	return _geosContextHandle;
}
#endif

// *****************************************************************
//		GetUtils
// *****************************************************************
IUtils* GetUtils()
{
	if (!m_utils)
	{
		HRESULT hr = CoCreateInstance(CLSID_Utils, nullptr, CLSCTX_INPROC_SERVER, IID_IUtils, reinterpret_cast<void**>(&m_utils));
		if (FAILED(hr))
		{
			m_utils = nullptr;
		}
	}
	return m_utils;
}

// *****************************************************************
//		GetTempShapefile
// *****************************************************************
IShapefile* GetTempShapefile()
{
	if (!m_shapefile)
	{
		HRESULT hr = CoCreateInstance(CLSID_Shapefile, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefile, reinterpret_cast<void**>(&m_shapefile));
		if (FAILED(hr))
		{
			m_shapefile = nullptr;
		}
	}
	return m_shapefile;
}