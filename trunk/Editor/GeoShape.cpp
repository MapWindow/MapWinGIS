#include "stdafx.h"
#include "GeoShape.h"
#include "GeographLibHelper.h"
#include "GeometryHelper.h"

// *******************************************************
//		TestAreaCalc()
// *******************************************************
void TestAreaCalc()
{
	std::vector<Point2D> points;

	points.push_back(Point2D(5.1, 42.6));
	points.push_back(Point2D(7.3, 41.6));
	points.push_back(Point2D(4.2, 40.4));

	//if (_points.size() > 0)
	//points.push_back(Point2D(_points[0]->x, _points[0]->y));
	double area = GeographLibHelper::CalcPolyGeodesicArea(points);
	Debug::WriteLine("Area %f", area);
};

// *******************************************************
//		GetGeodesicArea()
// *******************************************************
double GeoShape::GetGeodesicArea(bool closingPoint, double x, double y)
{
	unsigned int result;
	double perimeter = 0.0, area = 0.0;
	if (GetPolyPointCount(closingPoint) > 1)
	{
	recalc:
		if (_areaRecalcIsNeeded || !IsDynamic())
		{
			_areaRecalcIsNeeded = false;

			poly.Clear();

			for (int i = GetFirstPolyPointIndex(); i < (int)_points.size(); i++)
			{
				poly.AddPoint(_points[i]->y, _points[i]->x);
			}

			result = poly.Compute(true, true, perimeter, area);
			if (closingPoint)
				result = poly.TestPoint(y, x, true, true, perimeter, area);
		}
		else
		{
			if (closingPoint)
			{
				result = poly.TestPoint(y, x, true, true, perimeter, area);
				if (perimeter == 0 && area == 0)
				{
					_areaRecalcIsNeeded = true;
					goto recalc;
				}
			}
			else
			{
				result = poly.Compute(true, true, perimeter, area);
			}
		}
	}
	return abs(area);
}

// ****************************************************************
//		GetMeasuringPolyCenter()
// ****************************************************************
IPoint* GeoShape::GetPolygonCenter(Gdiplus::PointF* data, int length)
{
	IPoint* pnt = NULL;
	if (HasPolygon() && length > 2)
	{
		// let's draw fill and area
		Gdiplus::PointF* polyData = &data[0];

		// find position for label
		CComPtr<IShape> shp = NULL;
		GetUtils()->CreateInstance(idShape, (IDispatch**)&shp);
		if (shp)
		{
			long pointIndex;
			VARIANT_BOOL vb;
			shp->Create(ShpfileType::SHP_POLYGON, &vb);

			int sz = length;
			for (int i = 0; i < sz; i++)	{
				shp->AddPoint(polyData[i].X, polyData[i].Y, &pointIndex);
			}
			shp->AddPoint(polyData->X, polyData->Y, &pointIndex);	// close it

			shp->get_Centroid(&pnt);

			// make sure that centroid lies within extents of shapes; otherwise place it at the center
			CComPtr<IExtents> ext = NULL;
			shp->get_Extents(&ext);

			double x, y;
			pnt->get_X(&x);
			pnt->get_Y(&y);

			ext->PointIsWithin(x, y, &vb);
			if (!vb)
			{
				pnt->Release();
				ext->get_Center(&pnt);
			}
		}
	}
	return pnt;
}

// ***************************************************************
//		GetWgs84Projection()
// ***************************************************************
IGeoProjection* GeoShape::GetWgs84Projection()
{
	return _mapCallback ? _mapCallback->_GetWgs84Projection() : NULL;
}

// ***************************************************************
//		GetMapProjection()
// ***************************************************************
IGeoProjection* GeoShape::GetMapProjection()
{
	return _mapCallback ? _mapCallback->_GetMapProjection() : NULL;
}
// ***************************************************************
//		GetTransformationMode()
// ***************************************************************
tkTransformationMode GeoShape::GetTransformationMode()
{
	return _mapCallback ? _mapCallback->_GetTransformationMode() : tmNotDefined;
}

// ***************************************************************
//		ProjToPixel()
// ***************************************************************
void GeoShape::ProjToPixel(double projX, double projY, double& pixelX, double& pixelY)
{
	if (_mapCallback) {
		_mapCallback->_ProjectionToPixel(projX, projY, &pixelX, &pixelY);
	}
	else {
		Debug::WriteError("Measuring: map pointer is not initialized");
	}
}

// ***************************************************************
//		PixelToProj()
// ***************************************************************
void GeoShape::PixelToProj(double pixelX, double pixelY, double& projX, double& projY)
{
	if (_mapCallback) {
		_mapCallback->_PixelToProjection(pixelX, pixelY, &projX, &projY);
	}
	else {
		Debug::WriteError("Measuring: map pointer is not initialized");
	}
}

// *******************************************************
//		UpdateLatLng()
// *******************************************************
void GeoShape::UpdateLatLng(int pointIndex)
{
	if (pointIndex < 0 || pointIndex >= (int)_points.size()) return;

	MeasurePoint* pnt = _points[pointIndex];
	double x = pnt->Proj.x, y = pnt->Proj.y;
	if (TransformPoint(x, y))
	{
		pnt->x = x;
		pnt->y = y;
	}
}

// *******************************************************
//		Clear()
// *******************************************************
void GeoShape::Clear()
{
	for (size_t i = 0; i < _points.size(); i++)
		delete _points[i];
	_points.clear();
	poly.Clear();
}

// *******************************************************
//		GetTransformedPoints()
// *******************************************************
// transforms input data to decimal degrees
bool GeoShape::TransformPoint(double& x, double& y) {

	VARIANT_BOOL vb;
	switch (GetTransformationMode())
	{
		case tkTransformationMode::tmDoTransformation:
			GetMapProjection()->Transform(&x, &y, &vb);
			return true;
		case tkTransformationMode::tmWgs84Complied:
			return true;
	}
	return false;
}

// *******************************************************
//		GetArea()
// *******************************************************
double GeoShape::GetArea(bool closingPoint, double x, double y)
{
	if (GetPolyPointCount(closingPoint) < 3)
		return false;

	if (GetTransformationMode() == tmNotDefined)
	{
		return GetEuclidianArea(closingPoint, x, y);   // x, y are projected
	}
	else
	{
		double xDeg = x, yDeg = y;
		if (closingPoint) TransformPoint(xDeg, yDeg);
		return GetGeodesicArea(closingPoint, xDeg, yDeg);	  // x, y are decimal degrees
	}
}

// *******************************************************
//		GetEuclidianArea()
// *******************************************************
double GeoShape::GetEuclidianArea(bool closingPoint, double x, double y)
{
	double val = 0.0;
	if (GetPolyPointCount(closingPoint) > 2)
	{
		VARIANT_BOOL vb;
		_areaCalcShape->Create(ShpfileType::SHP_POLYGON, &vb);		// this will clear points

		long pointIndex = -1;

		for (size_t i = GetFirstPolyPointIndex(); i < _points.size(); i++)
		{
			_areaCalcShape->AddPoint(_points[i]->Proj.x, _points[i]->Proj.y, &pointIndex);
		}

		if (closingPoint)
			_areaCalcShape->AddPoint(x, y, &pointIndex);

		_areaCalcShape->AddPoint(_points[0]->Proj.x, _points[0]->Proj.y, &pointIndex);   // we need to close the poly
		_areaCalcShape->get_Area(&val);
	}
	return val;
}

// *******************************************************
//		GetNumParts()
// *******************************************************
int GeoShape::GetNumParts()
{
	int count = 0;
	for (size_t i = 0; i < _points.size(); i++) {
		if (_points[i]->Part == PartBegin)
			count++;
	}
	if (count == 0) count = 1;
	return count;
}

// *******************************************************
//		GetPartForPoint()
// *******************************************************
int GeoShape::GetPartForPoint(int pointIndex) {
	int part = -1;
	for (int i = 0; i <= pointIndex; i++) {
		if (_points[i]->Part == PartBegin)
			part++;
	}
	return part;
}

// *******************************************************
//		GetPartStart()
// *******************************************************
int GeoShape::GetPartStart(int partIndex) {
	int count = -1;
	for (size_t i = 0; i < _points.size(); i++) {
		if (_points[i]->Part == PartBegin)
			count++;
		if (count == partIndex)
			return i;
	}
	return 0;
}

// *******************************************************
//		GetPartStart()
// *******************************************************
int GeoShape::SeekPartEnd(int startSearchFrom) {
	for (int i = startSearchFrom; i < (int)_points.size(); i++) {
		if (_points[i]->Part == PartEnd)
			return i;
	}
	return _points.size() - 1;
}

// *******************************************************
//		GetPartStart()
// *******************************************************
int GeoShape::SeekPartStart(int startSearchFrom) {
	for (int i = startSearchFrom; i >= 0; i--) {
		if (_points[i]->Part == PartBegin)
			return i;
	}
	return 0;
}

// *******************************************************
//		GetCloseIndex()
// *******************************************************
int GeoShape::GetCloseIndex(int startIndex)
{
	bool polygon = Utility::ShapeTypeConvert2D(GetShapeType2D()) == SHP_POLYGON;
	int closeIndex = -1;
	if (polygon) {
		if (_points[startIndex]->Part == PartBegin) closeIndex = SeekPartEnd(startIndex);
		if (_points[startIndex]->Part == PartEnd) closeIndex = SeekPartStart(startIndex);
	}
	return closeIndex;
}

// ****************************************************************************/
//   GetExtents()
// ****************************************************************************/
void GeoShape::GetExtents(Extent& extent)
{
	double xMin = DBL_MAX, xMax = -DBL_MAX, yMin = DBL_MAX, yMax = -DBL_MAX;
	for (size_t i = 0; i < _points.size(); i++)
	{
		if (_points[i]->Proj.x > xMax) xMax = _points[i]->Proj.x;
		if (_points[i]->Proj.x < xMin) xMin = _points[i]->Proj.x;
		if (_points[i]->Proj.y > yMax) yMax = _points[i]->Proj.y;
		if (_points[i]->Proj.y > yMin) yMin = _points[i]->Proj.y;
	}
	extent.left = xMin;
	extent.right = xMax;
	extent.top = yMax;
	extent.bottom = yMin;
}

// *******************************************************
//		GetDistance()
// *******************************************************
double GeoShape::GetDistance()
{
	if (GetTransformationMode() == tmNotDefined)
	{
		return GetEuclidianDistance();	// if there us undefined or incompatible projection; return distance on plane 
	}
	else
	{
		return GetGeodesicDistance();
	}
}

// *******************************************************
//		GetEuclidianDistance()
// *******************************************************
// in map units specified by current projection
double GeoShape::GetEuclidianDistance()
{
	double dist = 0.0;
	if (_points.size() > 0)
	{
		for (size_t i = 0; i < _points.size() - 1; i++)
		{
			dist += _points[i]->Proj.GetDistance(_points[i + 1]->Proj);
		}
	}
	return dist;
}

// *******************************************************
//		GetGeodesicDistance()
// *******************************************************
// in meters with decimal degrees as input
double GeoShape::GetGeodesicDistance()
{
	IUtils* utils = GetUtils();
	double dist = 0.0;
	if (_points.size() > 0)
	{
		for (size_t i = 0; i < _points.size() - 1; i++)
		{
			double val;
			utils->GeodesicDistance(_points[i]->y, _points[i]->x, _points[i + 1]->y, _points[i + 1]->x, &val);
			dist += val;
		}
	}
	return dist;
}

// ***************************************************************
//		GetSegmentAngle()
// ***************************************************************
double GeoShape::GetSegmentAngle(int segmentIndex, long& errorCode)
{
	if (segmentIndex < 0 || segmentIndex >= (long)_points.size() - 1)
	{
		errorCode = tkINDEX_OUT_OF_BOUNDS;
		return 0.0;
	}
	else
	{
		int i = segmentIndex;
		double xScr, yScr, xScr2, yScr2;
		ProjToPixel(_points[i]->Proj.x, _points[i]->Proj.y, xScr, yScr);
		ProjToPixel(_points[i + 1]->Proj.x, _points[i + 1]->Proj.y, xScr2, yScr2);
		double x = xScr - xScr2;
		double y = yScr - yScr2;
		double angle = 360.0 - GeometryHelper::GetPointAngle(x, y) * 180.0 / pi_;
		if (angle == 360.0) angle = 0.0;
		return angle;
	}
}

// **************************************************************
//		GetSegmentLength()
// **************************************************************
double GeoShape::GetSegmentLength(int segmentIndex, long& errorCode)
{
	if (segmentIndex < 0 || segmentIndex >= (long)_points.size() - 1)
	{
		errorCode = tkINDEX_OUT_OF_BOUNDS;
		return 0.0;
	}
	else
	{
		double result;
		int i = segmentIndex;
		if (GetTransformationMode() == tmNotDefined)
		{
			result = _points[i]->Proj.GetDistance(_points[i + 1]->Proj);;
		}
		else
		{
			IUtils* utils = GetUtils();
			utils->GeodesicDistance(_points[i]->y, _points[i]->x, _points[i + 1]->y, _points[i + 1]->x, &result);
		}
		return result;
	}
}

// ****************************************************************************/
//   GetAzimuth()
// ****************************************************************************/
double GetAzimuth(MeasurePoint* pnt1, MeasurePoint* pnt2)
{
	double x = pnt2->Proj.x - pnt1->Proj.x;
	double y = pnt2->Proj.y - pnt1->Proj.y;
	return GeometryHelper::GetPointAngle(x, y) / pi_*180.0;
}

// ****************************************************************************/
//   GetBearing()
// ****************************************************************************/
double GeoShape::GetBearing(int vertexIndex, bool clockwise)
{
	double az1 = GetAzimuth(_points[vertexIndex - 1], _points[vertexIndex]);
	double az2 = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
	if (az2 - 180 > az1) az1 = az1 + 360;
	if (az1 - 180 > az2) az2 = az2 + 360;
	return clockwise ? 180 + (az1 - az2) : 180 + (az2 - az1);
}

// ****************************************************************************/
//   GetBearingLabelAngle()
// ****************************************************************************/
double GeoShape::GetBearingLabelAngle(int vertexIndex, bool clockwise)
{
	double az;
	double bearing = GetBearing(vertexIndex, clockwise);
	if (clockwise)
	{
		az = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
		az += bearing / 2.0;
	}
	else
	{
		az = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
		az -= bearing / 2.0;
	}
	if (az > 360.0) az -= 360.0;
	if (az < 0.0) az += 360.0;
	return az;
}

// **************************************************************
//		GetDymanicLineDistance()
// **************************************************************
double GeoShape::GetDynamicLineDistance()
{
	double dist;
	double xLng, yLat;
	PixelToProj(_mousePoint.x, _mousePoint.y, xLng, yLat);
	if (TransformPoint(xLng, yLat))
	{
		GetUtils()->GeodesicDistance(_points[_points.size() - 1]->y, _points[_points.size() - 1]->x, yLat, xLng, &dist);
	}
	else
	{
		dist = _points[_points.size() - 1]->Proj.GetDistance(xLng, yLat);
	}
	return dist;
}

// *******************************************************
//		SetMousePosition()
// *******************************************************
void GeoShape::SetMousePosition(double xScreen, double yScreen)
{
	_mousePoint.x = xScreen;
	_mousePoint.y = yScreen;
}

// *******************************************************
//		FindSegementWithPoint()
// *******************************************************
int GeoShape::FindSegmentWithPoint(double xProj, double yProj)
{
	for (size_t i = 0; i < _points.size() - 1; i++)
	{
		if (GeometryHelper::PointOnSegment(_points[i]->Proj.x, _points[i]->Proj.y,
			_points[i + 1]->Proj.x, _points[i + 1]->Proj.y, xProj, yProj))
			return i;
	}
	return -1;
}

// *******************************************************
//		GetPolygonPointCount()
// *******************************************************
int GeoShape::GetPolyPointCount(bool dynamicPoly)
{
	int size = _points.size() - GetFirstPolyPointIndex();
	if (dynamicPoly) size++;
	return size;
}