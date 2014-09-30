#ifndef GEOMETRY_OPERATIONS_H
#define GEOMETRY_OPERATIONS_H

const double eps = 0.000001;
	// TODO: wrap in namespace or static class
	bool LineIntersection(const POINT& p1, const POINT& p2, const POINT& p3, const POINT& p4);
	bool LineIntersection(const POINT& a1, const POINT& a2, const POINT& b1, const POINT& b2, POINT& pntCross);
	bool LineIntersection1(const POINT& a1, const POINT& a2, const POINT& b1, const POINT& b2);
	bool ExtentsIntersection(const POINT& p1, const POINT& p2, const POINT& p3, const POINT& p4);
	tkExtentsRelation RelateExtents(IShape* shp1, IShape* shp2);
	tkExtentsRelation RelateExtents(IExtents* ext1, IExtents* ext2);
	tkExtentsRelation RelateExtents(CRect& r1, CRect& r2);
	double GetPointAngle(double &x, double &y);
	bool PointOnSegment(double x1, double y1, double x2, double y2, double pntX, double pntY);
#endif