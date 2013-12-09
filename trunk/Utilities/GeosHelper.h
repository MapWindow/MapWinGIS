#pragma once
#include "ogr_spatialref.h"

class GeosHelper
{
public:
	GeosHelper(void) {};
	~GeosHelper(void) {};
	
	static bool IsValid(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return (gsGeom && GEOSisValid_r(getGeosHandle(), gsGeom));
		#else
			return (gsGeom && GEOSisValid(gsGeom));
		#endif
	}

	static GEOSGeometry* ExportToGeos(OGRGeometry* oGeom)
	{
		#ifdef GEOS_NEW
			return oGeom->exportToGEOS(getGeosHandle());
		#else
			return oGeom->exportToGEOS();
		#endif
	}

	static OGRGeometry* CreateFromGEOS(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return OGRGeometryFactory::createFromGEOS(getGeosHandle(), gsGeom);
		#else
			return OGRGeometryFactory::createFromGEOS(gsGeom);
		#endif
	}

	static GEOSGeometry* Buffer(GEOSGeometry* gsGeom, double Distance, int nSegments)
	{
		#ifdef GEOS_NEW
			return GEOSBuffer_r(getGeosHandle(), gsGeom, Distance, (int)nSegments);
		#else
			return GEOSBuffer( gsGeom, Distance, (int)nSegments);
		#endif
	}
	
	static void DestroyGeometry(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			GEOSGeom_destroy_r(getGeosHandle(), gsGeom);
		#else
			GEOSGeom_destroy(gsGeom);
		#endif
	}


};
