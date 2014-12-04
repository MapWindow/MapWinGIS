#pragma once
class SelectionHelper
{
public:
	static bool PolylineIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts, double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance);
	static bool PolygonIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts,
		double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance);
	static bool SelectWithShapeBounds(IShapefile* sf, IShape* shp, vector<long>& indices);
	static int SelectByPolygon(IShapefile* sf, IShape* polygon, int& errorCode);
	static bool SelectByPoint(IShapefile* sf, Extent& box, bool clearPrevious);
	static bool SelectByRectangle(IShapefile* sf, Extent& box);
	static bool SelectSingleShape(IShapefile* sf, Extent& box, long& shapeIndex);
	static bool SelectSingleShape(IShapefile* sf, Extent& box, SelectMode mode, long& shapeIndex);
private:
	static bool SelectShapes(IShapefile* sf, Extent& extents, double Tolerance, SelectMode SelectMode, std::vector<long>& selectResult);
	
};

