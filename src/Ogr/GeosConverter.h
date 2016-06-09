#pragma once
class GeosConverter
{
public:
	static bool GeomToShapes(GEOSGeom gsGeom, vector<IShape*>* vShapes, bool isM);
	static GEOSGeom ShapeToGeom(IShape* shp);
	static GEOSGeometry* MergeGeometries(vector<GEOSGeometry*>& data, ICallback* callback, bool deleteInput = true, bool displayProgress = true);
	static GEOSGeometry* SimplifyPolygon(const GEOSGeometry *gsGeom, double tolerance);
	static void NormalizeSplitResults(GEOSGeometry* result, GEOSGeometry* subject, ShpfileType shpType,	vector<GEOSGeometry*>& results);
};

