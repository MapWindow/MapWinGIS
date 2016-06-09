#pragma once
#include "BaseProvider.h"

class TileHelper
{
public:
	static double GetTileSizeProj(VARIANT_BOOL isSameProjection, BaseProvider* provider, IGeoProjection* wgsToMapTransformation, PointLatLng& location, int zoom);
	static bool Transform(TileCore* tile, IGeoProjection* proj, bool isSameProjection, RectLatLng& result);
	static double GetTileSize(BaseProjection* proj, PointLatLng& location, int zoom, double pixelPerDegree);
	static double GetTileSizeByWidth(BaseProjection* proj, PointLatLng& location, int zoom, double pixelPerDegree);
};