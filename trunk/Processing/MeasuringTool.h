#pragma once;
#include <vector>
using namespace std;

class MeasureDistance
{
private:
	bool isGeodesic;
	IGeoProjection* proj;
	IGeoProjection* projWGS84;
	tkTransformationMode transformationMode;
public:
	MeasureDistance(IGeoProjection* proj, IGeoProjection* projWGS84, tkTransformationMode mode): 
			isGeodesic(false), proj(proj), projWGS84(projWGS84), transformationMode(mode)
	{

	}
	
	std::vector<Point2D> points;		   // points entered by user (in map units, whatever they are)
	bool IsGeodesic() {
		return isGeodesic;
	}

	double getDistance() {
		double dist;
		isGeodesic = true;
		if (!GetGeodesicDistance(dist))	
		{
			dist = GetEuclidianDistance();	// if there us undefined or incompatible projection; return distance on plane 
			isGeodesic = false;
		}
		return dist;
	}

private:
	// in map units specified by current projection
	double GetEuclidianDistance()
	{
		double dist = 0.0;
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			dist += points[i].GetDistance(points[i + 1]);
		}
		return dist;
	}

	// trasnforms input data to decimal degrees
	bool GetTransformedPoints(vector<Point2D>& list) {
		
		VARIANT_BOOL vb;
		projWGS84->StartTransform(proj, &vb);
		if (!vb) {
			return false;
		}

		list.resize(points.size());

		for(size_t i = 0; i < list.size(); i++)
		{
			list[i].x = points[i].x;
			projWGS84->Transform(&list[i].x, &list[i].y, &vb);
			if (!vb) {
				projWGS84->StopTransform();
				return false;
			}
		}

		projWGS84->StopTransform();
		return true;
	}

	// in meters with decimal degrees as input
	bool GetGeodesicDistance(double& dist) 
	{
		vector<Point2D> list;
		
		switch (transformationMode)
		{
			case tkTransformationMode::tmNotDefined:
				return false;			// no way to calc distance
			case tkTransformationMode::tmDoTransformation:
				if (!GetTransformedPoints(list)) {
					return false;		// failed to transform
				}
				break;			
			case tkTransformationMode::tmWgs84Complied:
				break;	// do nothing
		}

		vector<Point2D>* data = transformationMode == tkTransformationMode::tmDoTransformation ? &list : &points;
		
		dist = 0.0;
		for (size_t i = 0; i < data->size(); i++)
		{
			double val;
			m_utils->GeodesicDistance((*data)[i].y, (*data)[i].x, (*data)[i + 1].y, (*data)[i + 1].x, &val);
			dist += val;
		}
		return true;
	}
};