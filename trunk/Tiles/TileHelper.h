#pragma once
#include "BaseProvider.h"

class TileHelper
{
public:
	static double GetTileSizeProj(VARIANT_BOOL isSameProjection, BaseProvider* provider, IGeoProjection* wgsToMapTransformation, PointLatLng& location, int zoom);
	static bool Transform(RectLatLng& Geog, IGeoProjection* proj, BaseProjection* baseProj, 
		bool isSameProjection, int tileX, int tileY, int zoom, RectLatLng& result);
	static double GetTileSize(BaseProjection* proj, PointLatLng& location, int zoom, double pixelPerDegree);
	static double GetTileSizeByWidth(BaseProjection* proj, PointLatLng& location, int zoom, double pixelPerDegree);
};