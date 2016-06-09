#include "stdafx.h"
#include "GeographLibHelper.h"
#include "..\Processing\GeograpicLib\PolygonArea.hpp"

// ********************************************************
//     CalcPolyGeodesicArea()
// ********************************************************
double GeographLibHelper::CalcPolyGeodesicArea(std::vector<Point2D>& points)
{
	GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
	GeographicLib::PolygonArea poly(geod);
	for(size_t i = 0; i < points.size(); i++)
	{
		poly.AddPoint(points[i].y, points[i].x);
	}
	double area = 0.0, perimeter = 0.0;
	unsigned int r = poly.Compute(true, true, perimeter, area);
	return area;
}
