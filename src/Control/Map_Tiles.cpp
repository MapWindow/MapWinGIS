#include "stdafx.h"
#include "map.h"
#include "GeoPoint.h"
#include "TileHelper.h"
#include "ProjectionHelper.h"

// ************************************************************
//		ChooseZoom()
// ************************************************************
int CMapView::ChooseZoom(Extent ext, double scalingRatio, bool limitByProvider, BaseProvider* provider)
{
	if (!provider) return 1;

	Point2D center = ext.GetCenter();
	PointLatLng location(center.y, center.x);

	bool precise = _tileProjectionState == ProjectionMatch;
	double ratio = precise ? 0.99 : 0.90;		// 0.99 = set some error margin for rounding issues

	int bestZoom = provider->get_MinZoom();

	for (int i = provider->get_MinZoom(); i <= (limitByProvider ? provider->get_MaxZoom() : 20); i++)
	{
		VARIANT_BOOL isSame = precise ? VARIANT_TRUE : VARIANT_FALSE;

		double tileSize = TileHelper::GetTileSizeProj(isSame, provider, GetWgs84ToMapTransform(), location, i);
		if (tileSize == -1) {
			continue;
		}

		tileSize *= PixelsPerMapUnit();

		int minSize = (int)(256 * scalingRatio  * ratio);
		if (tileSize < minSize) {
			break;
		}

		bestZoom = i;
	}

	CSize s1, s2;
	provider->get_Projection()->GetTileMatrixMinXY(bestZoom, s1);
	provider->get_Projection()->GetTileMatrixMaxXY(bestZoom, s2);

	return bestZoom;
}

// ************************************************************
//		ProjectionBounds
// ************************************************************
// Returns bounds of the tile provider under current map projection
bool CMapView::get_TileProviderBounds(BaseProvider* provider, Extent& retVal)
{
	if (!provider || !provider->get_Projection())	return false;

	BaseProjection* proj = provider->get_Projection();

	double left = proj->get_MinLong();
	double right = proj->get_MaxLong();
	double top = proj->get_MaxLat();
	double bottom = proj->get_MinLat();

	if (_transformationMode == tmDoTransformation)	// i.e. map cs isn't in decimal degrees
	{
		// There is a problem if map projection isn't world wide (like Amersfoort for example).
		// Then values outside its bounds may not to be transformed correctly.
		// There is hardly any workaround here. Ideally we should know the bounds for map
		// projection and clip both by them and by bounds of server projection. Since bounds
		// of map projection aren't available partial solutions can be used:
		// - don't use clipping if map projection isn't world wide while server projection is
		// (which will obviously lead to server bounds outside transformation range).
		// Alternatives:
		// - doing some checks after transformation to make sure that calculations make sense;
		// - add a method to API to set bounds of map projection/tiles;
		// - store and update built-in database of bounds for different coordinate systems
		// and identify projection on setting it to map;
		bool supportsWorldWideTransform = ProjectionHelper::SupportsWorldWideTransform(_projection, _wgsProjection);

		if (proj->IsWorldWide() && !supportsWorldWideTransform) // server projection is world wide, map projection - not
		{
			// so far just skip it;
			// optionally possible to transform to check if the results make sense
			return false;
		}
		else
		{
			VARIANT_BOOL vb;

			_wgsProjection->Transform(&left, &top, &vb);
			if (!vb) {
				Debug::WriteLine("Failed to project: x = %f; y = %f", left, top);
				return false;
			}

			_wgsProjection->Transform(&right, &bottom, &vb);
			if (!vb) {
				Debug::WriteLine("Failed to project: x = %f; y = %f", bottom, right);
				return false;
			}
		}

		//Debug::WriteLine("Projected world bounds: left = %f; right = %f; bottom = %f; top = %f", left, right, bottom, top);
	}

	retVal.left = left;
	retVal.right = right;
	retVal.top = top;
	retVal.bottom = bottom;

	return true;
}

// ************************************************************
//		GetTilesForMap
// ************************************************************
// Returns zoom level and indices to be loaded for the specific provider, given current map extents.
bool CMapView::get_TilesForMap(void* p, CRect& indices, int& zoom)
{
	BaseProvider* provider = reinterpret_cast<BaseProvider*>(p);
	if (!provider) {
		return false;
	}

	// no need to go any further there is no projection
	if (_transformationMode == tmNotDefined) {
		return false;		
	}

	Extent clipExtents = _extents;
	bool clipForTiles = get_TileProviderBounds(provider, clipExtents);

	// TODO: is it really needed?
	provider->put_Map(this);

	// we don't want to have coordinates outside world bounds, as it breaks tiles loading
	IExtents* ext = GetGeographicExtentsCore(clipForTiles, &clipExtents);
	if (!ext) return false;

	Extent bounds(ext);
	ext->Release();

	double xMaxD, xMinD, yMaxD, yMinD, zMaxD, zMinD;
	ext->GetBounds(&xMinD, &yMinD, &zMinD, &xMaxD, &yMaxD, &zMaxD);

	double scalingRatio = 1.0;
	zoom = ChooseZoom(bounds, scalingRatio, true, provider);

	provider->get_Projection()->getTileRectXY(bounds, zoom, indices);
	
	return true;
}