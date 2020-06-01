#pragma once
#include "globalvars.h"

class GeosHelper
{
public:
	static bool HasZ(const GEOSGeometry* g)
	{
		#ifdef GEOS_NEW
			return GEOSHasZ_r(getGeosHandle(), g) != 0;
		#else
			return GEOSHasZ(g) != 0;
		#endif
	}

	static int RelatePattern(const GEOSGeometry* g1, const GEOSGeometry* g2, const char *pat)
	{
		
		#ifdef GEOS_NEW
			return GEOSRelatePattern_r(getGeosHandle(), g1, g2, pat);
		#else
			return GEOSRelatePattern(g1, g2, pat);
		#endif
	}

	static int Area(const GEOSGeometry* g, double *area)
	{
		#ifdef GEOS_NEW
			return GEOSArea_r(getGeosHandle(), g, area);
		#else
			return GEOSArea(g, area);
		#endif
	}

	static GEOSGeometry* Boundary(const GEOSGeometry* g)
	{
		#ifdef GEOS_NEW
			return GEOSBoundary_r(getGeosHandle(), g);
		#else
			return GEOSBoundary(g);
		#endif
	}

	static int BufferParams_setEndCapStyle(GEOSBufferParams* p, tkBufferCap style)
	{
		#ifdef GEOS_NEW
			return GEOSBufferParams_setEndCapStyle_r(getGeosHandle(), p, (int)style);
		#else
			return GEOSBufferParams_setEndCapStyle(p, (int)style);
		#endif
	}

	static int BufferParams_setJoinStyle(GEOSBufferParams* p, tkBufferJoin joinStyle)
	{
		#ifdef GEOS_NEW
			return GEOSBufferParams_setJoinStyle_r(getGeosHandle(), p, (int)joinStyle);
		#else
			return GEOSBufferParams_setJoinStyle(p, (int)joinStyle);
		#endif
	}

	static int BufferParams_setMitreLimit(GEOSBufferParams* p, double mitreLimit)
	{
		#ifdef GEOS_NEW
			return GEOSBufferParams_setMitreLimit_r(getGeosHandle(), p, mitreLimit);
		#else
			return GEOSBufferParams_setMitreLimit(p, mitreLimit);
		#endif
	}

	static int BufferParams_setQuadrantSegments(GEOSBufferParams* p, int quadSegs)
	{
		#ifdef GEOS_NEW
			return GEOSBufferParams_setQuadrantSegments_r(getGeosHandle(), p, quadSegs);
		#else
			return GEOSBufferParams_setQuadrantSegments(p, quadSegs);
		#endif
	}

	static int BufferParams_setSingleSided(GEOSBufferParams* p, bool singleSided)
	{
		#ifdef GEOS_NEW
			return GEOSBufferParams_setSingleSided_r(getGeosHandle(), p, singleSided ? 1 : 0);
		#else
			return GEOSBufferParams_setSingleSided(p, singleSided ? 1 : 0);
		#endif
	}

	static GEOSGeometry* BufferWithParams(const GEOSGeometry* g, const GEOSBufferParams* p, double width)
	{
		#ifdef GEOS_NEW
			return GEOSBufferWithParams_r(getGeosHandle(), g, p, width);
		#else
			return GEOSBufferWithParams(g, p, width);
		#endif
	}

	static GEOSBufferParams* BufferParams_create()
	{
		#ifdef GEOS_NEW
			return GEOSBufferParams_create_r(getGeosHandle());
		#else
			return GEOSBufferParams_create();
		#endif
	}

	static void BufferParams_destroy(GEOSBufferParams* params)
	{
		#ifdef GEOS_NEW
			return GEOSBufferParams_destroy_r(getGeosHandle(), params);
		#else
			return GEOSBufferParams_destroy(params);
		#endif
	}

	static GEOSGeometry* Polygonize(const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSPolygonize_r(getGeosHandle(), &gsGeom, 1);
		#else
			return GEOSPolygonize(&gsGeom, 1);
		#endif
	}

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

    static char Disjoint(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
    {
        #ifdef GEOS_NEW
                return GEOSDisjoint_r(getGeosHandle(), gsBase, gsGeom);
        #else
                return GEOSDisjoint(gsBase, gsGeom);
        #endif
    }

    static char Covers(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
    {
        #ifdef GEOS_NEW
                return GEOSCovers_r(getGeosHandle(), gsBase, gsGeom);
        #else
                return GEOSCovers(gsBase, gsGeom);
        #endif
    }

    static char CoveredBy(const GEOSGeometry* gsBase, const GEOSGeometry* gsGeom)
    {
        #ifdef GEOS_NEW
                return GEOSCoveredBy_r(getGeosHandle(), gsBase, gsGeom);
        #else
                return GEOSCoveredBy(gsBase, gsGeom);
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

	static GEOSGeometry* TopologyPreserveSimplify(const GEOSGeometry* gsGeom, double tolerance)
	{
		#ifdef GEOS_NEW
			return GEOSTopologyPreserveSimplify_r(getGeosHandle(), gsGeom, tolerance);
		#else
			return GEOSTopologyPreserveSimplify(gsGeom, tolerance);
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

	static GEOSGeometry* CloneGeometry(const GEOSGeometry* gsGeom)
	{
		#ifdef GEOS_NEW
			return GEOSGeom_clone_r(getGeosHandle(), gsGeom);
		#else
			return GEOSGeom_clone(gsGeom);
		#endif
	}

	static GEOSGeometry* Intersection(const GEOSGeometry* gsGeom1, const GEOSGeometry* gsGeom2)
	{
		#ifdef GEOS_NEW
			return GEOSIntersection_r(getGeosHandle(), gsGeom1, gsGeom2);
		#else
			return GEOSIntersection(gsGeom1, gsGeom2);
		#endif
	}

	static GEOSGeometry* Difference(const GEOSGeometry* gsGeom1, const GEOSGeometry* gsGeom2)
	{
		#ifdef GEOS_NEW
			return GEOSDifference_r(getGeosHandle(), gsGeom1, gsGeom2);
		#else
			return GEOSDifference(gsGeom1, gsGeom2);
		#endif
	}

	static GEOSGeometry* SymDifference(const GEOSGeometry* gsGeom1, const GEOSGeometry* gsGeom2)
	{
		#ifdef GEOS_NEW
			return GEOSSymDifference_r(getGeosHandle(), gsGeom1, gsGeom2);
		#else
			return GEOSSymDifference(gsGeom1, gsGeom2);
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

	static GEOSGeometry* Interpolate(GEOSGeometry* gs, double d, bool normalized = false)
	{
#ifdef GEOS_NEW
		if (normalized)
			return GEOSInterpolateNormalized_r(getGeosHandle(), gs, d);
		else
			return GEOSInterpolate_r(getGeosHandle(), gs, d);
#else
		if (normalized)
			return GEOSInterpolateNormalized(gs, d);
		else
			return GEOSInterpolate(gs, d);
#endif
	}

	static double Project(GEOSGeometry* g1, GEOSGeometry* g2)
	{
#ifdef GEOS_NEW
		return GEOSProject_r(getGeosHandle(), g1, g2);
#else
		return GEOSProject(g1, g2);
#endif
	}
    static GEOSGeometry* Snap(GEOSGeometry* g1, GEOSGeometry* g2, double tolerance = 1.0)
    {
        #ifdef GEOS_NEW
            return GEOSSnap_r(getGeosHandle(), g1, g2, tolerance);
        #else
            return GEOSSnap(g1, g2, tolerance);
        #endif
    }
};
