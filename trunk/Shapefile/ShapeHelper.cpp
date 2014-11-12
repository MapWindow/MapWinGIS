#include "stdafx.h"
#include "ShapeHelper.h"
#include "SelectionHelper.h"



// *****************************************************
//		PointWithinShape
// *****************************************************
bool ShapeHelper::PointInThisPoly(IShape* shp, double x, double y)
{
	if (!shp) return false;
	CComPtr<IPoint> pnt = NULL;
	GetUtils()->CreateInstance(idPoint, (IDispatch**)&pnt);
	VARIANT_BOOL vb;
	shp->PointInThisPoly(pnt, &vb);
	return vb ? true : false;
}

// *****************************************************
//		PointWithinShape
// *****************************************************
bool ShapeHelper::BoundPointsInPolygon(IShape* shp, double b_minX, double b_minY, double b_maxX, double b_maxY)
{
	// test for Reverse inclusion
	if (PointInThisPoly(shp, b_minX, b_minY))
	{
		return true;
	}
	else if (PointInThisPoly(shp, b_maxX, b_maxY))
	{
		return true;
	}
	else if (PointInThisPoly(shp, b_minX, b_maxY))
	{
		return true;
	}
	else if (PointInThisPoly(shp, b_maxX, b_minY))
	{
		return true;
	}
	return false;
}

// *****************************************************
//		PointWithinShape
// *****************************************************
bool ShapeHelper::PointWithinShape(IShape* shape, double projX, double projY, double Tolerance)
{
	std::vector<double> xPts;
	std::vector<double> yPts;
	std::vector<long> parts;
	ShpfileType ShapeType;

	ShpfileType shpType2D;
	shape->get_ShapeType(&shpType2D);
	shpType2D = Utility::ShapeTypeConvert2D(shpType2D);

	double halfTolerance = Tolerance*.5;
	double b_minX = projX - halfTolerance;
	double b_minY = projY - halfTolerance;
	double b_maxX = projX + halfTolerance;
	double b_maxY = projY + halfTolerance;

	if (shpType2D == SHP_POLYLINE)
	{
		if (get_MemShapePoints(shape, ShapeType, parts, xPts, yPts) != FALSE)
		{
			if (SelectionHelper::PolylineIntersection(xPts, yPts, parts, b_minX, b_maxX, b_minY, b_maxY, Tolerance))
				return true;
		}
	}
	else if (shpType2D == SHP_POLYGON)
	{
		if (get_MemShapePoints(shape, ShapeType, parts, xPts, yPts))
		{
			if (SelectionHelper::PolygonIntersection(xPts, yPts, parts, b_minX, b_maxX, b_minY, b_maxY, Tolerance))
				return true;

			if (BoundPointsInPolygon(shape, b_minX, b_maxX, b_minY, b_maxY))
				return true;

			VARIANT_BOOL vb;
			IPoint* pnt = NULL;
			GetUtils()->CreateInstance(idPoint, (IDispatch**)&pnt);

			pnt->put_X(b_minX);
			pnt->put_Y(b_minY);
			GetUtils()->PointInPolygon(shape, pnt, &vb);

			if (!vb)
			{
				pnt->put_X(b_maxX);
				pnt->put_Y(b_maxY);
				GetUtils()->PointInPolygon(shape, pnt, &vb);
			}

			if (!vb)
			{
				pnt->put_X(b_minX);
				pnt->put_Y(b_maxY);
				GetUtils()->PointInPolygon(shape, pnt, &vb);
			}

			if (!vb)
			{
				pnt->put_X(b_maxX);
				pnt->put_Y(b_minY);
				GetUtils()->PointInPolygon(shape, pnt, &vb);
			}

			pnt->Release();

			return vb ? true : false;
		}
	}
	else if (shpType2D == SHP_MULTIPOINT)
	{
		if (get_MemShapePoints(shape, ShapeType, parts, xPts, yPts) != FALSE)
		{
			bool addShape = false;
			for (size_t j = 0; j < xPts.size(); j++)
			{
				double px = xPts[j];
				double py = yPts[j];

				if (px >= b_minX && px <= b_maxX)
				{
					if (py >= b_minY && py <= b_maxY)
					{
						addShape = true;
						break;
					}
				}
			}
			if (addShape) return true;
		}
	}
	return false;
}

// **************************************************************
//		get_MemShapePoints
// **************************************************************
BOOL ShapeHelper::get_MemShapePoints(IShape* shape, ShpfileType & ShapeType, std::vector<long> & parts, std::vector<double> & xPts, std::vector<double> & yPts)
{
	double x, y;
	long part;

	long numPoints;
	long numParts;
	VARIANT_BOOL vbretval;

	ShpfileType shapetype;
	shape->get_ShapeType(&shapetype);

	parts.clear();
	xPts.clear();
	yPts.clear();

	if (shapetype == SHP_NULLSHAPE)
	{
		ShapeType = shapetype;
		return FALSE;
	}
	else if (shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM)
	{
		shape->get_NumPoints(&numPoints);
		if (numPoints != 1) return FALSE;

		shape->get_XY(0, &x, &y, &vbretval);
		xPts.push_back(x);
		yPts.push_back(y);
		ShapeType = shapetype;
	}
	else if (shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM)
	{
		shape->get_NumParts(&numParts);
		shape->get_NumPoints(&numPoints);
		if (numPoints < 2)	return FALSE;

		// fill up parts: polyline must have at least 1 part	
		if (numParts > 0)
		{
			for (int p = 0; p < numParts; p++)
			{
				shape->get_Part(p, &part);
				parts.push_back(part);
			}
		}
		else
			parts.push_back(0);

		// fill up xPts and yPts
		VARIANT_BOOL vbretval;
		for (int i = 0; i < numPoints; i++)
		{
			shape->get_XY(i, &x, &y, &vbretval);
			xPts.push_back(x);
			yPts.push_back(y);
		}
		ShapeType = shapetype;
	}
	else if (shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM)
	{
		shape->get_NumParts(&numParts);
		shape->get_NumPoints(&numPoints);
		if (numPoints < 2) return FALSE;

		// fill up parts: polygon must have at least 1 part	
		if (numParts > 0)
		{
			for (int p = 0; p < numParts; p++)
			{
				shape->get_Part(p, &part);
				parts.push_back(part);
			}
		}
		else
			parts.push_back(0);

		// fill up xPts and yPts
		for (int i = 0; i < numPoints; i++)
		{
			shape->get_XY(i, &x, &y, &vbretval);
			xPts.push_back(x);
			yPts.push_back(y);
		}

		ShapeType = shapetype;
	}
	else if (shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM)
	{
		shape->get_NumPoints(&numPoints);
		if (numPoints < 1)	return FALSE;

		// fill up xPts and yPts
		for (int i = 0; i < numPoints; i++)
		{
			shape->get_XY(i, &x, &y, &vbretval);
			xPts.push_back(x);
			yPts.push_back(y);
		}
		ShapeType = shapetype;
	}
	else
		return FALSE;

	return TRUE;
}

// *************************************************************
//     ForceProperShapeType()
// *************************************************************
// Checks if shape type match shapefile type and doing fixes if possible
bool ShapeHelper::ForceProperShapeType(IShape* shp, ShpfileType sfType)
{
	ShpfileType shapeType;
	shp->get_ShapeType(&shapeType);

	if (Utility::ShapeTypeConvert2D(sfType) == SHP_MULTIPOINT && Utility::ShapeTypeConvert2D(shapeType) == SHP_POINT)
	{
		VARIANT_BOOL vb;
		CComPtr<IPoint> pnt = NULL;
		shp->get_Point(0, &pnt);
		shp->Create(sfType, &vb);
		if (vb)
		{
			long pointIndex = 0;
			shp->InsertPoint(pnt, &pointIndex, &vb);
		}
		// extract it once more to make sure it has worked
		shp->get_ShapeType(&shapeType);
	}
	return sfType == shapeType;
}