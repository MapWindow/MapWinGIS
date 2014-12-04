#pragma once

#include <math.h>
#include <map>
#include <vector>
#include <deque>
#include "gdal_priv.h"
#include "geos_c.h"
#include "ogr_geometry.h"
#include "ogr_spatialref.h"
#include "MapWinGis.h"
#include "structures.h"
#include "UtilityFunctions.h"
#include "GdalHelper.h"
#include "ComHelper.h"
#include "Utilities\Debugging\ReferenceCounter.h"
#include "GlobalSettingsInfo.h"
#include "Logger.h"
#include "Extent.h"
#include "VarH.h"
#include "MapViewCallback.h"
#include "DebugHelper.h"

#pragma warning(disable:4482)	// non-standard extension used - for names of enumerations
#pragma warning(disable:4099)	// missing program database

extern GEOSContextHandle_t _geosContextHandle;
extern IUtils* m_utils;
extern IShapefile* m_shapefile;
extern ReferenceCounter gReferenceCounter;
extern GlobalSettingsInfo m_globalSettings;
extern char* UTF8_ENCODING_MARKER;

IUtils* GetUtils();
IShapefile* GetTempShapefile();
GEOSContextHandle_t getGeosHandle();

// constants to be available  in the whole program
const double pi_ = 3.1415926535897932384626433832795;
const double pi_2 = 1.5707963267948966192313216916398;
const double METERS_PER_DEGREE = 110899.999942;
const double MAX_LAYER_VISIBLE_SCALE = 100000000.0;
const double MERCATOR_MAX_VAL = 20037508.342789244;	// in GMercator for bot long and lat
const double MAX_LATITUDE = 85.05112878;		// in WGS84
const double MAX_LONGITUDE = 180.0;
const int EPSG_AMERSFOORT = 28992;
const double SNAP_TOLERANCE = 20;  // pixels
const double MOUSE_CLICK_TOLERANCE = 20;  // pixels
const double POINT_INSERT_TOLERANCE = 10;  // pixels

#define MAX_BUFFER 512

#ifdef VLD_FORCE_ENABLE
	#include <vld.h>
#endif

// comment these lines to turn off the memory leaking detection tools
#ifdef _DEBUG
	#include "../debugging/MemLeakDetect.h"
	#pragma warning(disable:4100)
	extern CMemLeakDetect gMemLeakDetect;
#endif
