#pragma once

const double eps = 0.000001;

class GeometryHelper
{
public:
	static bool LineIntersection(const POINT& p1, const POINT& p2, const POINT& p3, const POINT& p4);
	static bool LineIntersection(const POINT& a1, const POINT& a2, const POINT& b1, const POINT& b2, POINT& pntCross);
	static bool LineIntersection1(const POINT& a1, const POINT& a2, const POINT& b1, const POINT& b2);
	static bool ExtentsIntersection(const POINT& p1, const POINT& p2, const POINT& p3, const POINT& p4);
	static tkExtentsRelation RelateExtents(IShape* shp1, IShape* shp2);
	static tkExtentsRelation RelateExtents(IExtents* ext1, IExtents* ext2);
	static tkExtentsRelation RelateExtents(CRect& r1, CRect& r2);
	static double GetPointAngle(double &x, double &y);
	static bool PointOnSegment(double x1, double y1, double x2, double y2, double pntX, double pntY);
};
