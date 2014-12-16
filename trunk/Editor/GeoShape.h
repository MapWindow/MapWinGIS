#pragma once
#include "..\Processing\GeograpicLib\PolygonArea.hpp"

class GeoShape
{
public:
	GeoShape(): 
		geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f()),
		poly(geod)
	{
		_mapCallback = NULL;
		ComHelper::CreateShape(&_areaCalcShape);
		_mousePoint.x = _mousePoint.y = 0;
		_areaRecalcIsNeeded = true;
		
	}
	virtual ~GeoShape() {
		if (_areaCalcShape) _areaCalcShape->Release();
	}

protected:
	GeographicLib::Geodesic geod;
	GeographicLib::PolygonArea poly;
	std::vector<MeasurePoint*> _points;		   // points in decimal degrees (in case transformation to WGS84 is possible)	
	
	IMapViewCallback* _mapCallback;
	bool _areaRecalcIsNeeded;		// geodesic area should be recalculated a new (after a point was added or removed)
	IShape* _areaCalcShape;
	Point2D _mousePoint;			// points entered by user (in map units, whatever they are)
	
	IGeoProjection* GetMapProjection();
	IGeoProjection* GetWgs84Projection();
	bool TransformPoint(double& x, double& y);
	bool HasProjection() { return GetTransformationMode() != tmNotDefined; }
	void SetModified() { _areaRecalcIsNeeded = true; }
	void ProjToPixel(double projX, double projY, double& pixelX, double& pixelY);
	void PixelToProj(double pixelX, double pixelY, double& projX, double& projY);
	double GetGeodesicArea(bool closingPoint, double x, double y);
	IPoint* GetPolygonCenter(Gdiplus::PointF* data, int length);
	void Clear();
	void UpdateLatLng(int pointIndex);
	double GetEuclidianArea(bool closingPoint, double x, double y);
	void GetExtents(Extent& extent);
	double GetEuclidianDistance();
	double GetGeodesicDistance();
	double GetBearing(int vertexIndex, bool clockwise);
	double GetBearingLabelAngle(int vertexIndex, bool clockwise);
	double GetDynamicLineDistance();
	int FindSegmentWithPoint(double xProj, double yProj);

	// Parts
	int GetPartStart(int partIndex);
	int SeekPartEnd(int startSearchFrom);
	int SeekPartStart(int startSearchFrom);
	int GetCloseIndex(int startIndex);
	int GetPartForPoint(int pointIndex);
	int GetNumParts();

public:
	void SetMapCallback(IMapViewCallback* mapView) { _mapCallback = mapView; }
	tkTransformationMode GetTransformationMode();
	double GetArea(bool closingPoint, double x, double y);
	double GetDistance();
	double GetSegmentLength(int segmentIndex, long& errorCode);
	double GetSegmentAngle(int segmentIndex, long& errorCode);
	void SetMousePosition(double xScreen, double yScreen);
	bool IsEmpty() 	{ return _points.size() == 0; }
	int GetPolyPointCount(bool dynamicPoly);

	virtual bool IsDynamic() = 0;
	virtual bool HasPolygon() = 0;
	virtual int GetFirstPolyPointIndex() { return 0; };
	virtual ShpfileType GetShapeType2D() { return SHP_POLYLINE; }
};

