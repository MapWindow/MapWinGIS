#pragma once

class GeographLibHelper
{
public:
	GeographLibHelper(void) {};
	~GeographLibHelper(void) {};

	static double CalcPolyGeodesicArea(std::vector<Point2D>& points);
};
