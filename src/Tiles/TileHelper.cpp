#include "stdafx.h"
#include "tilehelper.h"
#include "CustomProjection.h"

// ************************************************************
//		GetTileSizeProj()
// ************************************************************
double TileHelper::GetTileSizeProj(VARIANT_BOOL isSameProjection, BaseProvider* provider, IGeoProjection* wgsToMapTransformation, 
								   PointLatLng& location, int zoom)
{
	CustomProjection* customProj = dynamic_cast<CustomProjection*>(provider->get_Projection());

	if (isSameProjection && customProj)
	{
		SizeLatLng size;
		customProj->GetTileSizeProj(zoom, size);
		return size.WidthLng;
	}
	
	CPoint point;
	provider->get_Projection()->FromLatLngToXY(location, zoom, point);
	SizeLatLng size;
	provider->get_Projection()->GetTileSizeLatLon(point, zoom, size);

	PointLatLng newLoc;
	provider->get_Projection()->FromXYToLatLng(point, zoom, newLoc);

	double xMin, xMax, yMin, yMax;
	xMin = newLoc.Lng;
	xMax = xMin + size.WidthLng;
	if (abs(xMax) > 180.0)
		xMax = xMin - size.WidthLng;

	yMin = newLoc.Lat;
	yMax = yMin + size.HeightLat;
	if (abs(yMax) > 90.0) {
		yMax = yMin - size.HeightLat;
	}

	if (wgsToMapTransformation)
	{
		VARIANT_BOOL vb1, vb2;
		m_globalSettings.suppressGdalErrors = true;
		wgsToMapTransformation->Transform(&xMin, &yMin, &vb1);
		wgsToMapTransformation->Transform(&xMax, &yMax, &vb2);
		m_globalSettings.suppressGdalErrors = false;

		if (vb1 && vb2)
		{
			double val = xMax - xMin;
			return val > 0 ? val : -1.0;
		}
		return -1.0;
	}
	else
	{
		return xMax - xMin;
	}
}

// ************************************************************
//		Transform()
// ************************************************************
// Transforms tile bounds to map projection.
bool TileHelper::Transform(TileCore* tile, IGeoProjection* mapProjection, bool isSameProjection, RectLatLng& result)
{
	if (isSameProjection)
	{
		// projection for tiles matches map projection (most often it's Google Mercator; EPSG:3857)
		PointLatLng pnt;
		CustomProjection* customProj = dynamic_cast<CustomProjection*>(tile->get_Projection());
		
		if (customProj)
		{
			customProj->FromXYToProj(CPoint(tile->tileX(), tile->tileY() + 1), tile->zoom(), pnt);
			SizeLatLng size;
			customProj->GetTileSizeProj(tile->zoom(), size);
			result.xLng = pnt.Lng;
			result.yLat = pnt.Lat;
			result.WidthLng = size.WidthLng;
			result.HeightLat = size.HeightLat;
			return true;
		}
	}
	else
	{
		if (mapProjection)
		{
			RectLatLng* bounds = tile->get_GeographicBounds();
			double xMin = bounds->xLng; 
			double yMax = bounds->yLat;
			double xMax = bounds->MaxLng();
			double yMin = bounds->MinLat();

			BaseProjection* pr = tile->get_Projection();
			if (pr)
			{
				xMin = MAX(xMin, pr->get_MinLong());
				xMax = MIN(xMax, pr->get_MaxLong());
				yMin = MAX(yMin, pr->get_MinLat());
				yMax = MIN(yMax, pr->get_MaxLat());
			}

			double xTL, xTR, xBL, xBR;
			double yTL, yTR, yBL, yBR;

			xTL = xBL = xMin;
			xTR = xBR = xMax;
			yTL = yTR = yMax;
			yBL = yBR = yMin;

			VARIANT_BOOL vb;
			mapProjection->Transform(&xTL, &yTL, &vb);
			if (!vb) return false;

			mapProjection->Transform(&xTR, &yTR, &vb);
			if (!vb) return false;

			mapProjection->Transform(&xBL, &yBL, &vb);
			if (!vb) return false;

			mapProjection->Transform(&xBR, &yBR, &vb);
			if (!vb) return false;

			result.xLng = (xBL + xTL)/2.0;
			result.yLat = (yTL + yTR)/2.0;
			result.WidthLng = (xTR + xBR)/2.0 - result.xLng;
			result.HeightLat = result.yLat - (yBR + yBL)/2.0;
			return true;
		}
		else
		{
			// we are working with WGS84 decimal degrees
			RectLatLng* bounds = tile->get_GeographicBounds();
			result.xLng = bounds->xLng;
			result.yLat = bounds->yLat;
			result.WidthLng = bounds->WidthLng;
			result.HeightLat = bounds->HeightLat;
			return true;
		}
	}

	return false;
}

// ************************************************************
//		GetTileSize()
// ************************************************************
// Returns tiles size of the specified zoom level under current map scale
double TileHelper::GetTileSize(BaseProjection* proj, PointLatLng& location, int zoom, double pixelPerDegree)
{
	CPoint point;
	proj->FromLatLngToXY(location, zoom, point);
	SizeLatLng size;
	proj->GetTileSizeLatLon(point, zoom, size);
	return (size.WidthLng + size.HeightLat) * pixelPerDegree / 2.0;
}

// ************************************************************
//		GetTileSizeByWidth()
// ************************************************************
// Returns tiles size of the specified zoom level under current map scale
double TileHelper::GetTileSizeByWidth(BaseProjection* proj, PointLatLng& location, int zoom, double pixelPerDegree)
{
	CPoint point;
	proj->FromLatLngToXY(location, zoom, point);
	SizeLatLng size;
	proj->GetTileSizeLatLon(point, zoom, size);
	return size.WidthLng * pixelPerDegree;
}