#include "stdafx.h"
#include "ShapeHelper.h"
#include "SelectionHelper.h"
#include "Shape.h"

// *****************************************************
//		GetShapeType
// *****************************************************
ShpfileType ShapeHelper::GetShapeType(IShape* shp)
{
	if (!shp) return SHP_NULLSHAPE;
	ShpfileType shpType;
	shp->get_ShapeType(&shpType);
	return shpType;
}

// *****************************************************
//		GetShapeType2D
// *****************************************************
ShpfileType ShapeHelper::GetShapeType2D(IShape* shp)
{
	if (!shp) return SHP_NULLSHAPE;
	ShpfileType shpType;
	shp->get_ShapeType2D(&shpType);
	return shpType;
}

// *****************************************************
//		PointWithinShape
// *****************************************************
bool ShapeHelper::PointInThisPoly(IShape* shp, double x, double y)
{
	if (!shp) return false;
	CComPtr<IPoint> pnt = NULL;
	ComHelper::CreatePoint(&pnt);
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
	shpType2D = ShapeUtility::Convert2D(shpType2D);

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
			ComHelper::CreatePoint(&pnt);

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

	if (ShapeUtility::Convert2D(sfType) == SHP_MULTIPOINT && ShapeUtility::Convert2D(shapeType) == SHP_POINT)
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

// *************************************************************
//     SplitByPolyline()
// *************************************************************
bool ShapeHelper::SplitByPolyline(IShape* subject, IShape* polyline, vector<IShape*>& shapes)
{
	return ((CShape*)subject)->SplitByPolylineCore(polyline, shapes);
}

// *************************************************************
//     IsEmpty()
// *************************************************************
bool ShapeHelper::IsEmpty(IShape* shp)
{
	if (!shp) return true;
	VARIANT_BOOL vb;
	shp->get_IsEmpty(&vb);
	return vb ? true: false;
}

// *************************************************************
//		Cast()
// *************************************************************
CShape* ShapeHelper::Cast(CComPtr<IShape>& shp)
{
	return (CShape*)&(*shp);
}

// *************************************************************
//		GetNumParts()
// *************************************************************
long ShapeHelper::GetNumParts(IShape* shp)
{
	if (!shp) return 0;
	long numParts;
	shp->get_NumParts(&numParts);
	return numParts;
}

// *************************************************************
//		GetLargestPart()
// *************************************************************
int ShapeHelper::GetLargestPart(IShape* shp)
{
	if (!shp) return -1;

	int maxPart = -1;
	double maxValue = 0;
	
	ShpfileType shpType = ShapeHelper::GetShapeType2D(shp);
	long numParts = ShapeHelper::GetNumParts(shp);

	for (int j = 0; j < numParts; j++)
	{
		CComPtr<IShape> shpPart = NULL;
		shp->get_PartAsShape(j, &shpPart);
		if (!shpPart) continue;

		if (shpType == SHP_POLYGON)
		{
			VARIANT_BOOL vbretval;
			shpPart->get_PartIsClockWise(0, &vbretval);
			if (!vbretval) continue;   // holes of polygons must not be labeled
		}

		// Seeking the largest part of shape
		double value = 0.0;
		if (shpType == SHP_POLYGON)
		{
			shpPart->get_Area(&value);
		}
		else if (shpType == SHP_POLYLINE)
		{
			shpPart->get_Length(&value);
		}
		if (value > maxValue)
		{
			maxValue = value;
			maxPart = j;
		}
	}
	return maxPart;
}

// *************************************************************
//		AddLabel()
// *************************************************************
void ShapeHelper::AddLabelToShape(IShape* shp, ILabels* labels, BSTR text, tkLabelPositioning method, tkLineLabelOrientation orientation, double offsetX, double offsetY)
{
	if (!shp || !labels) return;
	double x, y, rotation = 0.0;
	((CShape*)shp)->get_LabelPosition(method, x, y, rotation, orientation);
	labels->AddLabel(text, x, y, rotation, -1, offsetX, offsetY);
}

// *************************************************************
//		CenterAsShape()
// *************************************************************
IShape* ShapeHelper::CenterAsShape(IShape* shp)
{
	if (!shp) return NULL;

	CComPtr<IPoint> pnt = NULL;
	shp->get_Center(&pnt);

	VARIANT_BOOL vb;
	IShape* shpNew = NULL;
	ComHelper::CreateShape(&shp);
	shp->Create(SHP_POINT, &vb);
	
	long pointIndex = 0;
	shp->InsertPoint(pnt, &pointIndex, &vb);

	return shp;
}

// *************************************************************
//		GetContentLength()
// *************************************************************
int ShapeHelper::GetContentLength(IShape* shp)
{
	if (!shp) return -1;

	long numPoints, numParts;
	ShpfileType shpType;

	shp->get_NumPoints(&numPoints);
	shp->get_NumParts(&numParts);
	shp->get_ShapeType(&shpType);

	return ShapeUtility::get_ContentLength(shpType, numPoints, numParts);
}

