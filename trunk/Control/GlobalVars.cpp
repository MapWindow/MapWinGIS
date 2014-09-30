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
GEOSContextHandle_t _geosContextHandle = NULL;
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
		CoCreateInstance(CLSID_Utils,NULL,CLSCTX_INPROC_SERVER,IID_IUtils,(void**)&m_utils);
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
		CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)&m_shapefile);
	}
	return m_shapefile;
}