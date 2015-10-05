#include "StdAfx.h"
#include "ShapeUtility.h"

// **************************************************************
//		get_ContentLength 
// **************************************************************
int ShapeUtility::get_ContentLength(ShpfileType shptype, int numPoints, int numParts)
{
	int contentLength;
	if (shptype == SHP_NULLSHAPE)
		contentLength = sizeof(int);		// type is stored
	else if (shptype == SHP_POINT)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 2;
	}
	else if (shptype == SHP_POINTZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4;
	}
	else if (shptype == SHP_POINTM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 3;
	}
	else if (shptype == SHP_POLYLINE)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_POLYLINEZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_POLYLINEM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_POLYGON)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_POLYGONZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_POLYGONM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_MULTIPOINT)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_MULTIPOINTZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_MULTIPOINTM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else
	{
		return 0;
	}
	return contentLength;
}