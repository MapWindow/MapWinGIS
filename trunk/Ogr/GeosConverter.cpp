#include "stdafx.h"
#include "GeosConverter.h"
#include "GeosHelper.h"
#include "OgrConverter.h"


// *********************************************************************
//		DoBuffer()
// *********************************************************************
GEOSGeometry* DoBuffer(DOUBLE distance, long nQuadSegments, GEOSGeometry* gsGeom)
{
	__try
	{
		return GeosHelper::Buffer(gsGeom, distance, nQuadSegments);
	}
	__except (1)
	{
		return NULL;
	}
}

// *********************************************************************
//		GEOSGeom2Shape()
// *********************************************************************
bool GeosConverter::GeomToShapes(GEOSGeom gsGeom, vector<IShape*>* vShapes, bool isM)
{
	bool substitute = false;
	if (!GeosHelper::IsValid(gsGeom))
	{
		GEOSGeometry* gsNew = DoBuffer(m_globalSettings.invalidShapesBufferDistance, 30, gsGeom);
		if (GeosHelper::IsValid(gsNew))
		{
			//GEOSGeom_destroy(gsGeom);   // it should be deleted by caller as it can be a part of larger geometry
			gsGeom = gsNew;
			substitute = true;
		}
	}

	OGRGeometry* oGeom = GeosHelper::CreateFromGEOS(gsGeom);
	if (oGeom)
	{
		char* type = GeosHelper::GetGeometryType(gsGeom);
		CString s = type;
		GeosHelper::Free(type);

		OGRwkbGeometryType oForceType = wkbNone;
		if (s == "LinearRing" && oGeom->getGeometryType() != wkbLinearRing)
			oForceType = wkbLinearRing;

		bool result = OgrConverter::GeometryToShapes(oGeom, vShapes, isM);
		OGRGeometryFactory::destroyGeometry(oGeom);

		if (substitute)
			GeosHelper::DestroyGeometry(gsGeom);
		return result;
	}
	else
	{
		if (substitute)
			GeosHelper::DestroyGeometry(gsGeom);

		return false;
	}
}

// *********************************************************************
//			Shape2GEOSGeom()
// *********************************************************************
//  Converts MapWinGis shape to GEOS geometry
GEOSGeom GeosConverter::ShapeToGeom(IShape* shp)
{
	OGRGeometry* oGeom = OgrConverter::ShapeToGeometry(shp);
	if (oGeom != NULL)
	{
		GEOSGeometry* result = GeosHelper::ExportToGeos(oGeom);
		OGRGeometryFactory::destroyGeometry(oGeom);
		return result;
	}
	else
		return NULL;
}

// *****************************************************
//		SimplifyPolygon()
// *****************************************************
// A polygon is expected as input; multi-polygons should be split into parts before treating with this routine
GEOSGeometry* GeosConverter::SimplifyPolygon(const GEOSGeometry *gsGeom, double tolerance)
{
	const GEOSGeometry* gsRing = GeosHelper::GetExteriorRing(gsGeom);	// no memory is allocated there
	GEOSGeom gsPoly = GeosHelper::TopologyPreserveSimplify(gsRing, tolerance);		// memory allocation

	if (!gsPoly)
		return NULL;

	std::vector<GEOSGeom> holes;
	for (int n = 0; n < GeosHelper::GetNumInteriorRings(gsGeom); n++)
	{
		gsRing = GeosHelper::GetInteriorRingN(gsGeom, n);				// no memory is allocated there
		if (gsRing)
		{
			GEOSGeom gsOut = GeosHelper::TopologyPreserveSimplify(gsRing, tolerance);	// memory allocation
			if (gsOut)
			{
				char* type = GeosHelper::GetGeometryType(gsOut);
				CString s = type;
				GeosHelper::Free(type);
				if (s == "LinearRing")
					holes.push_back(gsOut);
			}
		}
	}

	GEOSGeometry *gsNew = NULL;
	if (holes.size() > 0)
	{
		gsNew = GeosHelper::CreatePolygon(gsPoly, &(holes[0]), holes.size()); // memory allocation (should be released by caller)
	}
	else
	{
		gsNew = GeosHelper::CreatePolygon(gsPoly, NULL, 0);
	}
	return gsNew;

	/*GEOSGeometry* result = GeosHelper::Simplify(gsGeom, tolerance);
	char* type = GeosHelper::GetGeometryType(result);
	return result;*/
}

// *****************************************************
//		NormalizeSplitResults()
// *****************************************************
void GeosConverter::NormalizeSplitResults(GEOSGeometry* result, GEOSGeometry* subject, ShpfileType shpType, vector<GEOSGeometry*>& results)
{
	if (!result) return;

	int numGeoms = GeosHelper::GetNumGeometries(result);
	if (numGeoms > 1)
	{
		if (shpType == SHP_POLYGON)
		{
			for (int i = 0; i < numGeoms; i++)
			{
				const GEOSGeometry* polygon = GeosHelper::GetGeometryN(result, i);
				GEOSGeometry* intersect = GeosHelper::Intersection(subject, polygon);

				if (!intersect)
					continue;

				double intersectArea;
				GeosHelper::Area(intersect, &intersectArea);
				GeosHelper::DestroyGeometry(intersect);

				double polyArea;
				GeosHelper::Area(polygon, &polyArea);

				double areaRatio = intersectArea / polyArea;
				if (areaRatio > 0.99 && areaRatio < 1.01) {
					GEOSGeometry* clone = GeosHelper::CloneGeometry(polygon);
					if (clone) {
						results.push_back(clone);
					}
				}
			}

		}
		else {
			for (int i = 0; i < numGeoms; i++) {
				const GEOSGeometry* line = GeosHelper::GetGeometryN(result, i);
				GEOSGeometry* clone = GeosHelper::CloneGeometry(line);
				if (clone) {
					results.push_back(clone);
				}
			}
		}
	}
}

// ********************************************************************
//		MergeGeosGeometries
// ********************************************************************
// Returns GEOS geometry which is result of the union operation for the geometries passed
GEOSGeometry* GeosConverter::MergeGeometries(std::vector<GEOSGeometry*>& data, ICallback* callback, bool deleteInput)
{
	if (data.size() == 0)
		return NULL;

	USES_CONVERSION;
	GEOSGeometry* g1 = NULL;
	GEOSGeometry* g2 = NULL;

	bool stop = false;
	int count = 0;	// number of union operation performed
	long percent = 0;

	int size = data.size();
	int depth = 0;

	if (size == 1)
	{
		// no need for calculation
		if (deleteInput)
			return data[0];	 // no need to clone; it will be exactly the same
		else
		{
			GEOSGeometry* geomTemp = GeosHelper::CloneGeometry(data[0]);
			return geomTemp;
		}
	}

	while (!stop)
	{
		stop = true;

		for (int i = 0; i < size; i++)
		{
			if (data[i] != NULL)
			{
				if (!g1)
				{
					g1 = data[i];
					data[i] = NULL;
				}
				else
				{
					g2 = data[i];
					data[i] = NULL;
				}

				if (g2 != NULL)
				{
					GEOSGeometry* geom = GeosHelper::Union(g1, g2);
					data[i] = geom;		// placing the resulting geometry back for further processing

					if (deleteInput || depth > 0)	// in clipping operation geometries are used several times
						// so the intial geometries should be intact (depth == 0)
						// in other cases (Buffer, Dissolve) the geometries can be deleted in place
					{
						GeosHelper::DestroyGeometry(g1);
						GeosHelper::DestroyGeometry(g2);
					}

					g1 = NULL;
					g2 = NULL;
					count++;
					stop = false;		// in case there is at least one union occurred, we shall run once more

					Utility::DisplayProgress(callback, count, size, "Merging shapes...", percent);
				}

				// it is the last geometry, unpaired one, not the only one, it's the initial and must not be deleted
				if (i == size - 1 && stop == false && g2 == NULL && g1 != NULL && depth == 0 && !deleteInput)
				{
					// we need to clone it, to be able to apply unified memory management afterwards
					// when depth > 0 all interim geometries are deleted, while this one should be preserved
					GEOSGeometry* geomTemp = GeosHelper::CloneGeometry(g1);
					g1 = geomTemp;
				}
			}
		}
		depth++;
	}
	return g1;
}