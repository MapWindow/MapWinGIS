#include "stdafx.h"
#include "tilehelper.h"

// ************************************************************
//		GetTileSizeProj()
// ************************************************************
double TileHelper::GetTileSizeProj(VARIANT_BOOL isSameProjection, BaseProvider* provider, IGeoProjection* wgsToMapTransformation, 
								   PointLatLng& location, int zoom)
{
	CustomProjection* customProj = dynamic_cast<CustomProjection*>(provider->Projection);
	if (isSameProjection && customProj)
	{
		CSize size;
		customProj->GetTileSizeProj(zoom, size);
		return size.cx;
	}
	else
	{
		CPoint point;
		provider->Projection->FromLatLngToXY(location, zoom, point);
		SizeLatLng size;
		provider->Projection->GetTileSizeLatLon(point, zoom, size);

		PointLatLng newLoc;
		provider->Projection->FromXYToLatLng(point, zoom, newLoc);

		double xMin, xMax, yMin, yMax;
		xMin = newLoc.Lng;
		xMax = xMin + size.WidthLng;
		if (abs(xMax) > 180.0)
			xMax = xMin - size.WidthLng;

		yMin = newLoc.Lat;
		yMax = yMin + size.HeightLat;
		if (abs(yMax) > 90.0)
			yMax = yMin - size.HeightLat;

		if (wgsToMapTransformation)
		{
			VARIANT_BOOL vb1, vb2;
			wgsToMapTransformation->Transform(&xMin, &yMin, &vb1);
			wgsToMapTransformation->Transform(&xMax, &yMax, &vb2);
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
	return false;
}

// ************************************************************
//		Transform()
// ************************************************************
bool TileHelper::Transform(RectLatLng& Geog, IGeoProjection* proj, BaseProjection* baseProj, 
							bool isSameProjection, int tileX, int tileY, int zoom, RectLatLng& result)
{
	if (isSameProjection)
	{
		// projection for tiles matches map projection (most often it's Google Mercator; EPSG:3857)
		PointLatLng pnt;
		CustomProjection* customProj = (CustomProjection*)baseProj;
		if (customProj)
		{
			customProj->FromXYToProj(CPoint(tileX, tileY + 1), zoom, pnt);
			CSize size;
			customProj->GetTileSizeProj(zoom, size);
			result.xLng = pnt.Lng;
			result.yLat = pnt.Lat;
			result.WidthLng = size.cx;
			result.HeightLat = size.cy;
			return true;
		}
	}
	else
	{
		if (proj)
		{
			double xMin = Geog.xLng; 
			double yMax = Geog.yLat;
			double xMax = Geog.MaxLng(); 
			double yMin = Geog.MinLat();

			BaseProjection* pr = baseProj;
			if (pr)
			{
				xMin = MAX(xMin, pr->GetMinLongitude());
				xMax = MIN(xMax, pr->GetMaxLongitude());
				yMin = MAX(yMin, pr->GetMinLatitude());
				yMax = MIN(yMax, pr->GetMaxLatitude());
			}

			double xTL, xTR, xBL, xBR;
			double yTL, yTR, yBL, yBR;

			xTL = xBL = xMin;
			xTR = xBR = xMax;
			yTL = yTR = yMax;
			yBL = yBR = yMin;

			VARIANT_BOOL vb;
			proj->Transform(&xTL, &yTL, &vb);
			if (!vb) return false;

			proj->Transform(&xTR, &yTR, &vb);
			if (!vb) return false;

			proj->Transform(&xBL, &yBL, &vb);
			if (!vb) return false;

			proj->Transform(&xBR, &yBR, &vb);
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
			result.xLng = Geog.xLng;
			result.yLat = Geog.yLat;
			result.WidthLng = Geog.WidthLng;
			result.HeightLat = Geog.HeightLat;
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