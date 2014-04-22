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
#include "Utilities\Debugging\ReferenceCounter.h"
#include "GlobalSettingsInfo.h"
#include "Logger.h"
#include "Extent.h"
#include "VarH.h"

#pragma warning(disable:4482)	// non-standard extension used - for names of enumerations
#pragma warning(disable:4099)	// missing program database

extern GEOSContextHandle_t _geosContextHandle;
extern IUtils* m_utils;
extern ReferenceCounter gReferenceCounter;
extern GlobalSettingsInfo m_globalSettings;

IUtils* GetUtils();
GEOSContextHandle_t getGeosHandle();

// constants to be available  in the whole program
const double pi = 3.1415926535897932384626433832795;
const double pi_2 = 1.5707963267948966192313216916398;

// comment these lines to turn off the memory leaking detection tools
#ifdef _DEBUG
	#include "../debugging/MemLeakDetect.h"
	#pragma warning(disable:4100)
	extern CMemLeakDetect gMemLeakDetect;
#endif
