#pragma once
#include "Shape.h"

class ShapeHelper
{
public:
	static BOOL get_MemShapePoints(IShape* shape, ShpfileType & ShapeType, std::vector<long> & parts, std::vector<double> & xPts, std::vector<double> & yPts);
	static bool PointWithinShape(IShape* shape, double projX, double projY, double Tolerance);
	static bool PointInThisPoly(IShape* shp, double x, double y);
	static bool BoundPointsInPolygon(IShape* shp, double b_minX, double b_minY, double b_maxX, double b_maxY);
	static bool ForceProperShapeType(IShape* shp, ShpfileType sfType);
	static ShpfileType GetShapeType(IShape* shp);
	static ShpfileType GetShapeType2D(IShape* shp);
	static bool SplitByPolyline(IShape* subject, IShape* polyline, vector<IShape*>& shapes);
	static bool IsEmpty(IShape* shp);
	static CShape* Cast(CComPtr<IShape>& shp);
	static long GetNumParts(IShape* shp);
	static int GetLargestPart(IShape* shp);
	static void AddLabelToShape(IShape* shp, ILabels* labels, BSTR text, tkLabelPositioning method, tkLineLabelOrientation orientation, double offsetX, double offsetY);
	static IShape* CenterAsShape(IShape* shp);
	static int GetContentLength(IShape* shp);
};

