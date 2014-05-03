#pragma once
#include "globalvars.h"

class GeosHelper
{
public:
	GeosHelper(void) {};
	~GeosHelper(void) {};
	
	static char* IsValidReason(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSisValidReason_r(getGeosHandle(), gsGeom);			
		#else
			return GEOSisValidReason(gsGeom);	
		#endif
	}

	static char Within(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSWithin_r(getGeosHandle(), gsBase, gsGeom);
		#else
			return GEOSWithin(gsBase, gsGeom);	
		#endif
	}

	static char Touches(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSTouches_r(getGeosHandle(), gsBase, gsGeom);
		#else
			return GEOSTouches(gsBase, gsGeom);	
		#endif
	}

	static char Overlaps(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSOverlaps_r(getGeosHandle(), gsBase, gsGeom);
		#else
			return GEOSOverlaps(gsBase, gsGeom);	
		#endif
	}

	static char Intersects(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSIntersects_r(getGeosHandle(), gsBase, gsGeom);
		#else
			return GEOSIntersects(gsBase, gsGeom);	
		#endif
	}

	static char Equals(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSEquals_r(getGeosHandle(), gsBase, gsGeom);
		#else
			return GEOSEquals(gsBase, gsGeom);	
		#endif
	}

	static char Crosses(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSCrosses_r(getGeosHandle(), gsBase, gsGeom);
		#else
			return GEOSCrosses(gsBase, gsGeom);	
		#endif
	}

	static char Contains(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSContains_r(getGeosHandle(), gsBase, gsGeom);
		#else
			return GEOSContains(gsBase, gsGeom);	
		#endif
	}

	static void Free(void* buffer)
	{
		#ifdef GEOS_NEW
			return GEOSFree_r(getGeosHandle(), buffer);
		#else
			return GEOSFree(buffer);	
		#endif
	}

	static int GetNumGeometries(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSGetNumGeometries_r(getGeosHandle(), gsGeom);
		#else
			return GEOSGetNumGeometries(gsGeom);	
		#endif
	}

	static const GEOSGeometry* GetGeometryN(GEOSGeometry* gsGeom, int n)
	{
		#ifdef GEOS_NEW
			return GEOSGetGeometryN_r(getGeosHandle(), gsGeom, n);
		#else
			return GEOSGetGeometryN(gsGeom, n);	
		#endif
	}

	static const GEOSGeometry* GetExteriorRing(const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSGetExteriorRing_r(getGeosHandle(), gsGeom);
		#else
			return GEOSGetExteriorRing(gsGeom);	
		#endif
	}

	static GEOSGeometry* CreatePolygon(GEOSGeometry* gsShell, GEOSGeometry** gsHoles, unsigned int nholes)
	{
		#ifdef GEOS_NEW
			return GEOSGeom_createPolygon_r(getGeosHandle(), gsShell, gsHoles, nholes);
		#else
			return GEOSGeom_createPolygon(gsShell, gsHoles, nholes);	
		#endif
	}

	static int GetNumInteriorRings(const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSGetNumInteriorRings_r(getGeosHandle(), gsGeom);
		#else
			return GEOSGetNumInteriorRings(gsGeom);	
		#endif
	}

	static const GEOSGeometry* GetInteriorRingN(const GEOSGeometry* gsGeom, int n)
	{
		#ifdef GEOS_NEW
			return GEOSGetInteriorRingN_r(getGeosHandle(), gsGeom, n);
		#else
			return GEOSGetInteriorRingN(gsGeom, n);	
		#endif
	}

	static GEOSGeometry* Simplify(const GEOSGeometry* gsGeom, double tolerance)
	{
		#ifdef GEOS_NEW
			return GEOSSimplify_r(getGeosHandle(), gsGeom, tolerance);
		#else
			return GEOSSimplify(gsGeom, tolerance);	
		#endif
	}

	static char* GetGeometryType(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSGeomType_r(getGeosHandle(), gsGeom);
		#else
			return GEOSGeomType(gsGeom);	
		#endif
	}

	static int GetGeometryTypeId(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSGeomTypeId_r(getGeosHandle(), gsGeom);
		#else
			return GEOSGeomTypeId(gsGeom);	
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

	static GEOSGeometry* Union(GEOSGeometry* gsGeom1, GEOSGeometry* gsGeom2)
	{
		#ifdef GEOS_NEW
			return GEOSUnion_r(getGeosHandle(), gsGeom1, gsGeom2);
		#else
			return GEOSUnion(gsGeom1, gsGeom2);
		#endif
	}

	static GEOSGeometry* CloneGeometry(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSGeom_clone_r(getGeosHandle(), gsGeom);
		#else
			return GEOSGeom_clone(gsGeom);
		#endif
	}

	static GEOSGeometry* Intersection(GEOSGeometry* gsGeom1, GEOSGeometry* gsGeom2)
	{
		#ifdef GEOS_NEW
			return GEOSIntersection_r(getGeosHandle(), gsGeom1, gsGeom2);
		#else
			return GEOSIntersection(gsGeom1, gsGeom2);
		#endif
	}

	static GEOSGeometry* Difference(GEOSGeometry* gsGeom1, GEOSGeometry* gsGeom2)
	{
		#ifdef GEOS_NEW
			return GEOSDifference_r(getGeosHandle(), gsGeom1, gsGeom2);
		#else
			return GEOSDifference(gsGeom1, gsGeom2);
		#endif
	}

	static bool Intersects(GEOSGeometry* gsGeom1, GEOSGeometry* gsGeom2)
	{
		#ifdef GEOS_NEW
			return gsGeom1 && gsGeom2 && GEOSIntersects_r(getGeosHandle(), gsGeom1, gsGeom2);
		#else
			return gsGeom1 && gsGeom2 && GEOSIntersects(gsGeom1, gsGeom2);
		#endif
	}

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

	static void DestroyGeometry(GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			GEOSGeom_destroy_r(getGeosHandle(), gsGeom);
		#else
			GEOSGeom_destroy(gsGeom);
		#endif
	}

	static GEOSCoordSequence* ClosestPoints(GEOSGeometry* g1, GEOSGeometry* g2)
	{
		#ifdef GEOS_NEW
			return GEOSNearestPoints_r(getGeosHandle(), g1, g2);
		#else
			return GEOSNearestPoints(g1, g2);
		#endif
	}

	static void DestroyCoordinateSequence(GEOSCoordSequence* coords)
	{
		#ifdef GEOS_NEW
			GEOSCoordSeq_destroy_r(	getGeosHandle(), coords);
		#else
			GEOSCoordSeq_destroy(coords);	
		#endif
	}

	static unsigned int CoordinateSequenceSize(GEOSCoordSequence* coords)
	{
		unsigned int numPoints = 0;
		#ifdef GEOS_NEW
			GEOSCoordSeq_getSize_r(getGeosHandle(), coords, &numPoints);
		#else
			GEOSCoordSeq_getSize(coords, &numPoints);
		#endif
		return numPoints;
	}

	static bool CoordinateSequenceGetXY(GEOSCoordSequence* coords, unsigned int index, double& x, double& y)
	{
		#ifdef GEOS_NEW
			int val1 = GEOSCoordSeq_getX_r(getGeosHandle(), coords, index, &x);
			int val2 = GEOSCoordSeq_getY_r(getGeosHandle(), coords, index, &y);
		#else
			int val1 = GEOSCoordSeq_getX( coords, index, &x );
			int val2 = GEOSCoordSeq_getY( coords, index, &y );
		#endif
		return val1 && val2;
	}

	static GEOSGeometry* GetCentroid(GEOSGeometry* gs)
	{
		#ifdef GEOS_NEW
			return GEOSGetCentroid_r(getGeosHandle(), gs);
		#else
			return GEOSGetCentroid(gs);	
		#endif
	}

	static const GEOSCoordSequence* GetCoordinatesSeq(GEOSGeometry* gs)
	{
		#ifdef GEOS_NEW
			return GEOSGeom_getCoordSeq_r(getGeosHandle(), gs);
		#else
			return GEOSGeom_getCoordSeq(gs);	
		#endif
	}
};
