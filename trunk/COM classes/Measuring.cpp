#include "stdafx.h"
#include "measuring.h"
#include "..\Processing\GeograpicLib\PolygonArea.hpp"
#include "map.h"
GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
GeographicLib::PolygonArea poly(geod);

// *******************************************************
//		get_ScreenPoints()
// *******************************************************
int CMeasuring::get_ScreenPoints(void* map, bool hasLastPoint, int lastX, int lastY, Gdiplus::PointF** data)
{
	CMapView* mapView = (CMapView*)map;
	int size = this->points.size();
	if (hasLastPoint) size++;
	if (size > 0)
	{
		(*data) = new Gdiplus::PointF[size];
		Gdiplus::PointF* temp = *data;
		double x, y;
		for(size_t i = 0; i < this->points.size(); i++) {
			mapView->ProjectionToPixel(this->points[i]->Proj.x, this->points[i]->Proj.y, x, y);
			temp[i].X = (Gdiplus::REAL)x;
			temp[i].Y = (Gdiplus::REAL)y;
		}
		if (hasLastPoint)
		{
			temp[size - 1].X = (Gdiplus::REAL)lastX;
			temp[size - 1].Y = (Gdiplus::REAL)lastY;
		}
	}
	return size;
}

// *******************************************************
//		get_Length()
// *******************************************************
STDMETHODIMP CMeasuring::get_IsStopped(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = this->IsStopped() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		UndoPoint()
// *******************************************************
STDMETHODIMP CMeasuring::get_PointCount(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = points.size();
	return S_OK;
}

// *******************************************************
//		get_PointXY()
// *******************************************************
STDMETHODIMP CMeasuring::get_PointXY(long pointIndex, double* x, double* y, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (pointIndex < 0 || pointIndex >= (long)points.size())
	{
		ErrorMsg(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		*x = points[pointIndex]->Proj.x;
		*y = points[pointIndex]->Proj.y;
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *******************************************************
//		MeasuringType()
// *******************************************************
STDMETHODIMP CMeasuring::get_MeasuringType(tkMeasuringType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = this->measuringType;
	return S_OK;
}
STDMETHODIMP CMeasuring::put_MeasuringType(tkMeasuringType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	this->measuringType = newVal;
	return S_OK;
}

// *******************************************************
//		MeasuringType()
// *******************************************************
STDMETHODIMP CMeasuring::get_Persistent(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = this->persistent;
	return S_OK;
}
STDMETHODIMP CMeasuring::put_Persistent(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	this->persistent = newVal;
	return S_OK;
}

// *******************************************************
//		get_Length()
// *******************************************************
STDMETHODIMP CMeasuring::get_Length(double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = this->GetDistance();
	return S_OK;
}

// *******************************************************
//		get_AreaWithClosingVertex()
// *******************************************************
STDMETHODIMP CMeasuring::get_AreaWithClosingVertex(double lastPointProjX, double lastPointProjY, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = this->GetArea(true, lastPointProjX, lastPointProjY);
	return S_OK;
}

// *******************************************************
//		get_Area()
// *******************************************************
STDMETHODIMP CMeasuring::get_Area(double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = this->GetArea(false, 0.0, 0.0);
	return S_OK;
}

// *******************************************************
//		UndoPoint()
// *******************************************************
STDMETHODIMP CMeasuring::UndoPoint(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (points.size() > 0) {
		delete points[points.size() - 1];
		points.pop_back();
		*retVal = VARIANT_TRUE;
		areaRecalcIsNeeded = true;
	}
	stopped = false;
	closedPoly = false;
	return S_OK;
}

// *******************************************************
//		Stop()
// *******************************************************
STDMETHODIMP CMeasuring::FinishMeasuring()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	this->stopped = true;
	return S_OK;
}

// *******************************************************
//		Clear()
// *******************************************************
STDMETHODIMP CMeasuring::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	this->firstPointIndex = -1;
	for(size_t i = 0; i < points.size(); i++)
		delete points[i];
	this->points.clear();
	poly.Clear();
	return S_OK;
}

// *******************************************************
//		SegementLength()
// *******************************************************
STDMETHODIMP CMeasuring::get_SegementLength(int segmentIndex, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (segmentIndex < 0 || segmentIndex >= (long)points.size() - 1)
	{
		*retVal = 0.0;
		ErrorMsg(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		int i = segmentIndex;
		if (GetTransformationMode() == tmNotDefined)
		{
			*retVal = points[i]->Proj.GetDistance(points[i + 1]->Proj);
		}
		else
		{
			IUtils* utils = GetUtils();
			utils->GeodesicDistance(points[i]->y, points[i]->x, points[i + 1]->y, points[i + 1]->x, retVal);
		}
	}
	return S_OK;
}

#pragma region Distance calculation
// *******************************************************
//		getDistance()
// *******************************************************
double CMeasuring::GetDistance() 
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
double CMeasuring::GetEuclidianDistance()
{
	double dist = 0.0;
	if (points.size() > 0) 
	{
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			dist += points[i]->Proj.GetDistance(points[i + 1]->Proj);
		}
	}
	return dist;
}

// *******************************************************
//		GetGeodesicDistance()
// *******************************************************
// in meters with decimal degrees as input
double CMeasuring::GetGeodesicDistance() 
{
	IUtils* utils = GetUtils();
	double dist = 0.0;
	if (points.size() > 0) 
	{
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			double val;
			utils->GeodesicDistance(points[i]->y, points[i]->x, points[i + 1]->y, points[i + 1]->x, &val);
			dist += val;
		}
	}
	return dist;
}
#pragma endregion

#pragma region Calculate area

// *******************************************************
//		GetArea()
// *******************************************************
double CMeasuring::GetArea(bool closingPoint, double x, double y)
{
	if (GetTransformationMode() == tmNotDefined)
	{
		return GetEuclidianArea(closingPoint, x, y);   // x, y are projected
	}
	else
	{
		double xDeg = x, yDeg = y;
		if (closingPoint)
			TransformPoint(xDeg, yDeg);
		return GetGeodesicArea(closingPoint, xDeg, yDeg);	  // x, y are decimal degrees
	}
}

// *******************************************************
//		GetGeodesicArea()
// *******************************************************
// coordinates in decimal degress are expected
double CMeasuring::GetGeodesicArea(bool closingPoint, double x, double y)
{
	unsigned int result;
	double perimeter = 0.0, area = 0.0;
	if (points.size() > 1)
	{
		if (areaRecalcIsNeeded)
		{
			areaRecalcIsNeeded = false;
			poly.Clear();
			for(size_t i = 0; i < points.size(); i++)
			{
				poly.AddPoint(points[i]->y, points[i]->x);
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
			}
			else
			{
				result = poly.Compute(true, true, perimeter, area);
			}	
		}
	}
	return area;
}

// *******************************************************
//		GetEuclidianArea()
// *******************************************************
double CMeasuring::GetEuclidianArea(bool closingPoint, double x, double y) 
{
	double val = 0.0;
	if (points.size() > 1)
	{
		VARIANT_BOOL vb;
		shape->Create(ShpfileType::SHP_POLYGON, &vb);		// this will clear points

		long pointIndex = -1;
		
		for (size_t i = 0; i < points.size(); i++)
		{
			shape->AddPoint(points[i]->Proj.x, points[i]->Proj.y, &pointIndex);
		}
		
		if (closingPoint)
			shape->AddPoint(x, y, &pointIndex);

		shape->AddPoint(points[0]->Proj.x, points[0]->Proj.y, &pointIndex);   // we need to close the poly
		shape->get_Area(&val);
	}
	return val;
}
#pragma endregion

// *******************************************************
//		SetMapView()
// *******************************************************
void CMeasuring::SetMapView(void* mapView)
{
	_mapView = mapView;
}

IGeoProjection* CMeasuring::GetWgs84Projection()
{
	return _mapView ? ((CMapView*)_mapView)->m_wgsProjection : NULL;
}
IGeoProjection* CMeasuring::GetMapProjection()
{
	return _mapView ? ((CMapView*)_mapView)->m_projection : NULL;
}
tkTransformationMode CMeasuring::GetTransformationMode()
{
	return _mapView ? ((CMapView*)_mapView)->m_transformationMode : tmNotDefined;
}

// *******************************************************
//		AddPoint()
// *******************************************************
void CMeasuring::AddPoint(double xProj, double yProj, double xScreen, double yScreen) 
{
	closedPoly = false;
	if (stopped)
	{
		this->points.clear();
		this->stopped = false;
	}
	this->mousePoint.x = xScreen;
	this->mousePoint.y = yScreen;
	
	MeasurePoint* pnt = new MeasurePoint();
	pnt->Proj.x = xProj;
	pnt->Proj.y = yProj;
	points.push_back(pnt);	

	double x = xProj, y = yProj;
	if (TransformPoint(x, y))
	{
		pnt->x = x;
		pnt->y = y;
	}
	areaRecalcIsNeeded = true;
}

// *******************************************************
//		GetTransformedPoints()
// *******************************************************
// trasnforms input data to decimal degrees
bool CMeasuring::TransformPoint(double& x, double& y) {
	
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
//		DisplayAngles()
// *******************************************************
STDMETHODIMP CMeasuring::get_DisplayAngles(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = displayAngles ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CMeasuring::put_DisplayAngles(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	displayAngles = newVal ? true: false;
	return S_OK;
}


	