#include "stdafx.h"
#include "measuring.h"

// *******************************************************
//		get_Length()
// *******************************************************
STDMETHODIMP CMeasuring::get_Length(double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = this->getDistance();
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
		points.resize(points.size() - 1);
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *******************************************************
//		UndoPoint()
// *******************************************************
STDMETHODIMP CMeasuring::get_numPoints(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = points.size();
	return S_OK;
}

// returns distance
double CMeasuring::getDistance() {
	double dist;
	isGeodesic = true;
	if (!GetGeodesicDistance(dist))	
	{
		dist = GetEuclidianDistance();	// if there us undefined or incompatible projection; return distance on plane 
		isGeodesic = false;
	}
	return dist;
}

// in map units specified by current projection
double CMeasuring::GetEuclidianDistance()
{
	double dist = 0.0;
	if (points.size() > 0) 
	{
		for (size_t i = 0; i < points.size() - 1; i++)
		{
			dist += points[i].GetDistance(points[i + 1]);
		}
	}
	return dist;
}

// trasnforms input data to decimal degrees
bool CMeasuring::GetTransformedPoints(vector<Point2D>& list) {
	
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
bool CMeasuring::GetGeodesicDistance(double& dist) 
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
	
	if (!m_utils)
		CoCreateInstance(CLSID_Utils,NULL,CLSCTX_INPROC_SERVER,IID_IUtils,(void**)&m_utils);

	dist = 0.0;
	if (data->size() > 0) 
	{
		for (size_t i = 0; i < data->size() - 1; i++)
		{
			double val;
			m_utils->GeodesicDistance((*data)[i].y, (*data)[i].x, (*data)[i + 1].y, (*data)[i + 1].x, &val);
			dist += val;
		}
	}
	return true;
}
