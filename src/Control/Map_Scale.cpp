#include "stdafx.h"
#include "Map.h"
#include "Shapefile.h"
#include "Tiles.h"
#include "Utils.h"
#include "TileHelper.h"
#include "ShapefileHelper.h"
#include "ExtentsHelper.h"

#pragma region Scale
// ****************************************************
//	   ZoomToTileLevel()
// ****************************************************
VARIANT_BOOL CMapView::ZoomToTileLevel(int zoom)
{
	return ZoomToTileLevelCore(zoom, true);
}

// ****************************************************
//	   GetTilesZoomLevelSize()
// ****************************************************
// returns size of particular zoom level in server projection
void CMapView::GetTilesZoomLevelSize(int zoom, SizeLatLng& size)
{
	// we shall make all the calculations in server projection (either GMercator or custom)
	// and then transform bounds to the current coordinate system
	Point2D center = _extents.GetCenter();

	double minX, maxX, minY, maxY;	// size of control in pixels
	PROJECTION_TO_PIXEL(_extents.left, _extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(_extents.right, _extents.top, maxX, maxY);

	double screenHeight = abs(maxY - minY);
	double screenWidth = abs(maxX - minX);

	// multiplication ratio, to make texts more legible when projection-related scaling is under way
	double ratio;
	((CTiles*)_tiles)->get_ScalingRatio(&ratio);	// 1.0 by default

	// half of equator circumference (width and height for Mercator projection)
	double projWidth = ((CTiles*)_tiles)->get_Provider()->get_Projection()->GetWidth();
	// TODO: use height as well

	double pxWidth = ratio * 512.0 * pow(2.0, zoom - 1);	// width of map in pixels at the requested zoom

	double w = screenWidth / pxWidth * projWidth;		// requested width in server projection
	double h = screenHeight / pxWidth * projWidth;		// requested height in server projection

	size.WidthLng = w;
	size.HeightLat = h;
}

// ****************************************************
//	   ZoomToTileLevelCore()
// ****************************************************
VARIANT_BOOL CMapView::ZoomToTileLevelCore(int zoom, bool logPrevious)
{
	if (_transformationMode == tmNotDefined) {
		return VARIANT_FALSE;
	}

	if (_viewWidth == 0 || _viewHeight == 0) {
		return VARIANT_FALSE;
	}

	SizeLatLng size;
	GetTilesZoomLevelSize(zoom, size);

	if (_tileProjectionState == ProjectionMatch)
	{
		SetMapExtentsSize(size, logPrevious);
		_currentZoom = zoom;
	}
	else
	{
		Extent box;
		MapExtentsForTileBounds(size, box);

		SetExtentsCore(box, logPrevious, false, false);
		_currentZoom = zoom;
		AdjustZoom(zoom);
	}

	return VARIANT_FALSE;
}

// ****************************************************
//	   SetMapExtentsSize()
// ****************************************************
void CMapView::SetMapExtentsSize(SizeLatLng size, bool logPrevious)
{
	Point2D center = _extents.GetCenter();	

	double minX = center.x - size.WidthLng / 2.0;
	double maxX = center.x + size.WidthLng / 2.0;
	double minY = center.y - size.HeightLat / 2.0;
	double maxY = center.y + size.HeightLat / 2.0;

	SetExtentsCore(Extent(minX, maxX, minY, maxY), logPrevious, false, false);
}

// ****************************************************
//	   MapExtentsForZoomLevel()
// ****************************************************
// Converts size of new extents defined in tile server projection to map projection
bool CMapView::MapExtentsForTileBounds(SizeLatLng size, Extent& extents)
{
	IGeoProjection* tilesToMapTransform = GetTilesToMapTransform();
	if (!tilesToMapTransform) {
		ErrorMsg(tkFAILED_TO_REPROJECT);
		return false;
	}

	Point2D center = _extents.GetCenter();
	double xCent = center.x;
	double yCent = center.y;

	// bounds defined in tiles projection
	VARIANT_BOOL vb, vb2;
	GetMapToTilesTransform()->Transform(&xCent, &yCent, &vb);
	double minX = xCent - size.WidthLng / 2.0;
	double maxX = xCent + size.WidthLng / 2.0;
	double minY = yCent - size.HeightLat / 2.0;
	double maxY = yCent + size.HeightLat / 2.0;

	Extent extrapolatedBounds;
	bool extrapolation = ExtrapolateSphericalMercatorToDegrees(minX, maxX, minY, maxY, extrapolatedBounds);

	// transforming to map projection
	tilesToMapTransform->Transform(&minX, &minY, &vb);
	tilesToMapTransform->Transform(&maxX, &maxY, &vb2);

	// substitute with extrapolated values if we are outside bounds
	if (extrapolation)
	{
		if (extrapolatedBounds.left != 0.0) minX = extrapolatedBounds.left;
		if (extrapolatedBounds.right != 0.0) maxX = extrapolatedBounds.right;
		if (extrapolatedBounds.bottom != 0.0) minY = extrapolatedBounds.bottom;
		if (extrapolatedBounds.top != 0.0) maxY = extrapolatedBounds.top;
	}

	if (!vb || !vb2)
	{
		ErrorMsg(tkFAILED_TO_REPROJECT);
		return false;
	}
	
	// finally adjust the center to it's initial position
	extents = Extent(minX, maxX, minY, maxY);
	extents.MoveTo(center.x, center.y);
		
	return true;
}

// ****************************************************
//	   ExtrapolateSphericalMercatorToDegrees()
// ****************************************************
bool CMapView::ExtrapolateSphericalMercatorToDegrees(double xMin, double xMax, double yMin, double yMax, Extent& extents)
{
	VARIANT_BOOL sphericalMercator;
	_tiles->get_ProjectionIsSphericalMercator(&sphericalMercator);
	bool extrapolation = sphericalMercator && _transformationMode == tmWgs84Complied;

	double minLng = 0.0, maxLng = 0.0, minLat = 0.0, maxLat = 0.0;
	if (extrapolation)		// ding extrapolation for WGS84
	{
		// In case we are outside bounds of GMercator, results will be incorrect.
		// Let's use clipping and extrapolation as a remedy.
		// We shall assume that GMercator meters to map units ratio is constant and doesn't 
		// depend on latitude or longitude, which is not true.
		// TODO: do transformation in several points within world bounds for better extrapolation.
		double MAX_VAL = MERCATOR_MAX_VAL;
		if (xMin < -MAX_VAL)
			minLng = -MAX_LONGITUDE - abs(xMin + MAX_VAL) / MAX_VAL * MAX_LONGITUDE;

		if (xMax > MAX_VAL)
			maxLng = MAX_LONGITUDE + abs(xMax - MAX_VAL) / MAX_VAL * MAX_LONGITUDE;

		if (yMin < -MAX_VAL)
			minLat = -MAX_LATITUDE - abs(yMin + MAX_VAL) / MAX_VAL * MAX_LATITUDE;

		if (yMax > MAX_VAL)
			maxLat = MAX_LATITUDE + abs(yMax - MAX_VAL) / MAX_VAL * MAX_LATITUDE;
	}

	extents.left = minLng;
	extents.right = maxLng;
	extents.top = maxLat;
	extents.bottom = minLat;

	return extrapolation;
}

// ****************************************************
//	   GetCurrentTileSize()
// ****************************************************
double CMapView::GetCurrentTileSize( int zoom )
{
	VARIANT_BOOL vb;
	double xTemp = (_extents.left + _extents.right)/2.0;
	double yTemp = (_extents.bottom + _extents.top)/2.0;
	IGeoProjection* gp = GetMapToWgs84Transform();
	gp->Transform(&xTemp, &yTemp, &vb);
	PointLatLng loc;
	loc.Lat = yTemp;
	loc.Lng = xTemp;
	
	double size = TileHelper::GetTileSizeProj(_tileProjectionState == ProjectionMatch, 
		((CTiles*)_tiles)->get_Provider(), GetWgs84ToMapTransform(), loc, zoom);

	double pixelsPerMapUnit = PixelsPerMapUnit();
	size *= pixelsPerMapUnit;
	return size;
}

// ****************************************************
//	   AdjustZoom()
// ****************************************************
void CMapView::AdjustZoom(int zoom)
{
	if (_transformationMode == tmWgs84Complied || _tileProjectionState == ProjectionMatch)
		return;
	
	double size = GetCurrentTileSize(zoom);
	if (size < 0) return;
	Debug::WriteWithTime(Debug::Format("After zooming; tile size: %f", size), DebugTiles);

	double scalingRatio;
	((CTiles*)_tiles)->get_ScalingRatio(&scalingRatio);
	
	double targetSize = 256.0 * scalingRatio;
	if (abs(size - targetSize) > 1.0)
	{
		// let's adjust it
		double xTemp = (_extents.left + _extents.right)/2.0;
		double yTemp = (_extents.bottom + _extents.top)/2.0;
		double ratio = size / targetSize;
		double w = _extents.Width() * ratio;
		double h = _extents.Height() * ratio;
		_extents.left = xTemp - w / 2.0;
		_extents.right = xTemp + w / 2.0;
		_extents.top = yTemp - h / 2.0;
		_extents.bottom = yTemp + h / 2.0;
		this->SetExtentsCore(_extents, false, false, false);
	}
	
	size = GetCurrentTileSize(zoom);
	Debug::WriteWithTime(Debug::Format("After adjusting; tile size: %f", size), DebugTiles);
}

// ****************************************************
//	   SetNewExtentsWithZoomIn()
// ****************************************************
// Sets new extents and makes sure that map is zoomed in for at least one level
void CMapView::SetNewExtentsWithForcedZooming( Extent ext, bool zoomIn )
{
	double cLeft = ext.left;
	double cRight = ext.right;
	double cBottom = ext.bottom;
	double cTop = ext.top;
	
	// If new extents are 2 or more times smaller than current ones
	// we'll zoom in to the next level (or through several levels), otherwise return to the previous one.
	// As a user generally expects zooming, let's make extents smaller manually to force zooming
	// if they are not small enough.
	if (ForceDiscreteZoom())
	{
		int zoom, maxZoom, minZoom;
		_tiles->get_CurrentZoom(&zoom);
		GetMinMaxZoom(minZoom, maxZoom);
		if ((zoom + 1 > maxZoom && zoomIn) || (zoom - 1 < minZoom && !zoomIn)) 
			return;
		
		double ratioX = (_extents.right - _extents.left) / (cRight - cLeft);
		double ratioY = (_extents.top - _extents.bottom) / (cTop - cBottom);
		double ratio = MIN(ratioX, ratioY);
		
		double targetRatio = zoomIn ? 2.001 : 0.499;	// 2.001 = add some margin for rounding issues

		if ((zoomIn && ratio < 2.0) || !zoomIn && ratio > 0.5)
		{
			// no need to preserve the aspect ratio of new selection; control will adjust it all the same
			double w = (_extents.right - _extents.left) / targetRatio;		
			double h = (_extents.top - _extents.bottom) / targetRatio;
			double centX = (cRight + cLeft) / 2.0;
			double centY = (cTop + cBottom) / 2.0;
			cLeft = centX - w/2.0;
			cRight = centX + w/2.0;
			cTop = centY - h/2.0;
			cBottom = centY + h/2.0;
		}
	}

	SetExtentsCore(Extent(cLeft, cRight, cBottom, cTop));
}

// ****************************************************
//	   SetExtentsCore()
// ****************************************************
// adjustZoom - when we use discrete zoom levels the extents should be adjusted
// unless the call is from ZoomToTileLevel, so it's already done
void CMapView::SetExtentsCore( Extent ext, bool logExtents /*= true*/, bool mapSizeChanged /*= false*/, bool adjustZoom /*= true*/ )
{
	_knownExtents = keNone;
	_lastRedrawTime = 0.0f;

	CalculateVisibleExtents(ext, mapSizeChanged);

	if (ForceDiscreteZoom() && adjustZoom )
	{
		// Adjust it to the smaller discrete zoom level.
		// The computation overhead on calculating twice is negligible
		// In order it to work, Tiles.ChooseZoomLevel must not scale down the tiles
		// (i.e. tiles size is always > 256 (original) and the same size in chosen in ZoomToTileLevel)
		int zoom;
		_tiles->get_CurrentZoom(&zoom);
		ZoomToTileLevelCore(zoom, logExtents);
		_tiles->get_CurrentZoom(&zoom);
		return;
	}

	if (!ForceDiscreteZoom())
	{
		_currentZoom = GetCurrentZoom();
	}

	ScheduleLayerRedraw();

	if (logExtents)
	{
		LogPrevExtent();
	}

	FireExtentsChanged();

	_lastWidthMeters = 0.0;	// extents has changed it must be recalculated

	if( !_lockCount ) 
	{
		RedrawCore(RedrawAll, false, true);
	}
}

// **********************************************************
//			GetScreenInfo()
// **********************************************************
void CMapView::GetScreenInches(double& sw, double& sh)
{
	double minX, maxX, minY, maxY;	// size of map control in pixels

	ProjectionToPixel(_extents.left, _extents.top, minX, minY);
	ProjectionToPixel(_extents.right, _extents.bottom, maxX, maxY);

	sh = (maxY - minY) / 96.0;	// Number of pixels per logical inch along the screen width.
	sw = (maxX - minX) / 96.0;	// Number of pixels per logical inch along the screen height
}

// **********************************************************
//			GetMapInfo()
// **********************************************************
void CMapView::GetMapSizeInches(double& mw, double& mh)
{
	double convFact = Utility::GetConversionFactor(_unitsOfMeasure);	
	mh = _extents.Height() * convFact;
	mw = _extents.Width() * convFact;
}

// **********************************************************
//			CurrentScale()
// **********************************************************
DOUBLE CMapView::GetCurrentScale(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	 
	if (_extents.Width() == 0.0 || _extents.Height() == 0.0 || _viewWidth == 0 || _viewHeight == 0)
	{
		return 0.0;
	}

	double sw, sh, mw, mh;
	GetScreenInches(sw, sh);
	GetMapSizeInches(mw, mh);

	double md = sqrt(pow(mw, 2) + pow(mh, 2));
	double sd = sqrt(pow(sw, 2) + pow(sh, 2));

	return md / sd;
}

// **********************************************************
//			SetCurrentScale()
// **********************************************************
void CMapView::SetCurrentScale(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (newVal <= 0.0) return;
	
	double sw, sh;
	GetScreenInches(sw, sh);

	// diagonal of map extents in inches
	double sd = sqrt(pow(sw, 2) + pow(sh, 2));
	double md = newVal * sd;
	
	double a = sw / sh;								// we need width and height, but have diagonal and the sides ratio of triangle only; this makes 2 equations:
	double mh = sqrt(pow(md,2)/(pow(a,2) + 1));		// x/y = a
	double mw = mh * a;								// x^2 + y^2 = b^2		// where b - mapDiag
													// Taking x from first:
													// y^2*a^2 + y^2 = b^2
													// y = sqrt(b^2/(a^2 + 1))
													// x = y*a
	// converting to the map units
	mh /= Utility::GetConversionFactor(_unitsOfMeasure);
	mw /= Utility::GetConversionFactor(_unitsOfMeasure);
	
	Point2D center = _extents.GetCenter();	
	Extent box(center, mw, mh);

	SetExtentsCore(box);
}
#pragma endregion

#pragma region Extents

// ****************************************************
//	   GetExtents()
// ****************************************************
IExtents* CMapView::GetExtents()
{
	IExtents * box = NULL;
	ComHelper::CreateExtents(&box);
	box->SetBounds( _extents.left, _extents.bottom, 0, _extents.right, _extents.top, 0 );
	return box;
}

// ****************************************************
//	   SetExtents()
// ****************************************************
void CMapView::SetExtents(IExtents* newValue)
{
	if( !newValue )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return;
	}
	
	Extent box(newValue);

	SetExtentsCore(box);
}

#pragma endregion

#pragma region GeographicExtents

// *****************************************************
//		SetGeographicExtents()
// *****************************************************
VARIANT_BOOL CMapView::SetGeographicExtents(IExtents* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!pVal)
	{
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return VARIANT_FALSE;
	}
	else
	{
		this->LockWindow(tkLockMode::lmLock);
		
		if (_transformationMode == tmWgs84Complied)
		{
			this->SetExtents(pVal);
		}
		else if (_transformationMode == tmDoTransformation)
		{
			double xMin, xMax, yMin, yMax, zMin, zMax;
			pVal->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
			
			VARIANT_BOOL vbretval;
			IGeoProjection* p = GetWgs84ToMapTransform();
			if (p) {
				p->Transform(&xMin, &yMin, &vbretval);	 
				if (!vbretval) {
					this->LockWindow(tkLockMode::lmUnlock);
					this->ErrorMessage(tkFAILED_TO_REPROJECT);
					return VARIANT_FALSE;
				}
				p->Transform(&xMax, &yMax, &vbretval);	 
				if (!vbretval) {
					this->LockWindow(tkLockMode::lmUnlock);
					this->ErrorMessage(tkFAILED_TO_REPROJECT);
					return VARIANT_FALSE;
				}
			}
			else
			{
				this->LockWindow(tkLockMode::lmUnlock);
				this->ErrorMessage(tkFAILED_TO_REPROJECT);
				return VARIANT_FALSE;
			}

			pVal->SetBounds(xMin, yMin, zMin, xMax, yMax, zMax);
			this->SetExtents(pVal); 
		}

		if (ForceDiscreteZoom())
		{
			int zoom;
			_tiles->get_CurrentZoom(&zoom);
			ZoomToTileLevel(zoom);
		}
		this->LockWindow(tkLockMode::lmUnlock);
		return VARIANT_TRUE;
	}
}

// *****************************************************
//		GetMeasuring()
// *****************************************************
IMeasuring* CMapView::GetMeasuring() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(_measuring) {
		_measuring->AddRef();
	}
	return _measuring;
}

// *****************************************************
//		GetShapeEditor()
// *****************************************************
IShapeEditor* CMapView::GetShapeEditor() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(_shapeEditor) {
		_shapeEditor->AddRef();
	}
	return _shapeEditor;
}

// *****************************************************
//		GetGeographicExtents()
// *****************************************************
IExtents* CMapView::GetGeographicExtents()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return GetGeographicExtentsCore(false);
}

// ***************************************************************
//	   GetGeographicExtentsCore
// ***************************************************************
bool CMapView::GetGeographicExtentsInternal(bool clipForTiles, Extent* clipExtents, Extent& result)
{
	// we don't want to have coordinates outside world bounds, as it breaks tiles loading
	IExtents* ext = GetGeographicExtentsCore(clipForTiles, clipExtents);
	if (!ext) return false;

	Extent bounds(ext);
	ext->Release();

	result = bounds;

	return true;
}

// ***************************************************************
//	   GetGeographicExtentsCore
// ***************************************************************
IExtents* CMapView::GetGeographicExtentsCore(bool clipForTiles, Extent* clipExtents)
{
	IExtents * box = NULL;

	if (GetMapProjection())
	{
		if (_transformationMode == tkTransformationMode::tmWgs84Complied)
		{
			ComHelper::CreateExtents(&box);
			box->SetBounds( _extents.left, _extents.bottom, 0, _extents.right, _extents.top, 0 );
		}
		else if (_transformationMode == tkTransformationMode::tmDoTransformation)
		{
			VARIANT_BOOL vb;
			IGeoProjection* projTemp = GetMapToWgs84Transform();
			if (projTemp)
			{
				Extent ext;
				bool clip = clipForTiles && clipExtents;
				ext.left = clip ? MAX(_extents.left, clipExtents->left) : _extents.left;
				ext.right = clip ? MIN(_extents.right, clipExtents->right) : _extents.right;
				ext.top = clip ? MIN(_extents.top, clipExtents->top) : _extents.top;
				ext.bottom = clip ? MAX(_extents.bottom, clipExtents->bottom) : _extents.bottom;
				
				double xBL, yBL, xTL, yTL, xBR, yBR, xTR, yTR;
				
				xBL = ext.left;
				yBL = ext.bottom;
				
				xTL = ext.left;
				yTL = ext.top;

				xBR = ext.right;
				yBR = ext.bottom;
				
				xTR = ext.right;
				yTR = ext.top;

				projTemp->Transform(&xBL, &yBL, &vb);	if (!vb) goto cleaning;
				projTemp->Transform(&xTL, &yTL, &vb);	if (!vb) goto cleaning;
				projTemp->Transform(&xBR, &yBR, &vb);  if (!vb) goto cleaning;
				projTemp->Transform(&xTR, &yTR, &vb);  if (!vb) goto cleaning;
				
				double degreePerMapUnit = this->DegreesPerMapUnit();

				bool checkBounds = true;
				if (checkBounds && !clipForTiles)
				{
					double xMinTest = xTL; 
					double yMinTest = yBR;
					double xMaxTest = xBR; 
					double yMaxTest = yTL;
					
					projTemp = GetWgs84ToMapTransform();
					if (projTemp)
					{
						projTemp->Transform(&xMinTest, &yMinTest, &vb);	if (!vb) goto cleaning;
						projTemp->Transform(&xMaxTest, &yMaxTest, &vb);    if (!vb) goto cleaning;
					}
					else
						goto cleaning;
					
					double x1 = fabs(xMinTest - _extents.left);
					double x2 = fabs(xMaxTest - _extents.right);
					double y1 = fabs(yMinTest - _extents.bottom);
					double y2 = fabs(yMaxTest - _extents.top);
					
					VARIANT_BOOL projected;
					GetMapProjection()->get_IsGeographic(&projected);
					if (projected)
					{
						if (x1 > 500.0)
							xTL = -180.0;

						if (x2 > 500.0)
							xBR = 180.0;
					}
				}

				ComHelper::CreateExtents(&box);
				box->SetBounds( xTL, yBR, 0, xBR, yTL, 0 );		// TODO: return 4 point geographical extents as projections other that equirectangular can be used
			}
		}
	}
cleaning:
	return box;
}
#pragma endregion

#pragma region Unit conversion
// *****************************************************
//		GetPixelsPerDegree
// *****************************************************
DOUBLE CMapView::GetPixelsPerDegree(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	double val = 1.0;
	
	if (this->_unitsOfMeasure != umDecimalDegrees)
	{
		if (!Utility::ConvertDistance(umDecimalDegrees, this->_unitsOfMeasure, val))
		{
			return 0.0;
		}
	}

	double x, y;
    x = y = 0.0;
    double screenX = 0.0, screenY = 0.0;
    this->ProjToPixel(x, y, &screenX, &screenY);
    double x1 = screenX;
	double y1 = screenY;

    x = y = val;
    this->ProjToPixel(x, y, &screenX, &screenY);
    double result = (abs(screenX - x1) + abs(screenY - y1))/2.0;
	
	// alternative approach (results depend on latitude)
	/*double px, py, px2, py2;
	double degX, degY;
	
	PixelToDegrees(_viewWidth/2, _viewHeight/2, &degX, &degY);
	DegreesToPixel(degX, degY, &px, &py );
	DegreesToPixel(degX + 1, degY + 1, &px2, &py2 );
	double result2 = (abs(px - px2) + abs(py - py2))/2.0;*/

	return result;
}

// *****************************************************
//		GetPixelsPerMapUnit
// *****************************************************
// Without conversion to decimal degrees
DOUBLE CMapView::PixelsPerMapUnit(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	double val = 1.0;
	
	double x, y;
    x = y = 0.0;
    double screenX = 0.0, screenY = 0.0;
    this->ProjToPixel(x, y, &screenX, &screenY);
    double x1 = screenX;

    x = y = val;
    this->ProjToPixel(x, y, &screenX, &screenY);
    return abs(screenX - x1);
}

// *****************************************************
//		DegreesPerMapUnit
// *****************************************************
DOUBLE CMapView::DegreesPerMapUnit(void)
{
	double val = 1.0;
	if (_unitsOfMeasure == umDecimalDegrees)
	{
		return val;
	}
	else
	{
		if (Utility::ConvertDistance(this->_unitsOfMeasure, umDecimalDegrees, val))
		{
			return val;
		}
		else
			return 1.0;
	}
}

void CMapView::ProjToPixel(double projX, double projY, double FAR* pixelX, double FAR* pixelY)
{
	ProjectionToPixel(projX,projY,*pixelX,*pixelY);
}

void CMapView::PixelToProj(double pixelX, double pixelY, double FAR* projX, double FAR* projY)
{
	PixelToProjection(pixelX,pixelY,*projX,*projY);
}

// ***********************************************************
//		UnitsPerPixel
// ***********************************************************
// Returns number of map units per pixel of map
double CMapView::UnitsPerPixel()
{
    double minX, maxX, minY, maxY;
    PROJECTION_TO_PIXEL(_extents.left, _extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(_extents.right, _extents.top, maxX, maxY);
	if (minX == maxX && minY == maxY)
	{
		return 0.0;
	}
	else
	{
		return sqrt(pow(_extents.right - _extents.left, 2) + pow(_extents.top - _extents.bottom, 2)) / 
			   sqrt(pow(maxX - minX,2) + pow(maxY - minY,2));
	}
}
#pragma endregion

#pragma region Zooming
// *****************************************************
//		ZoomToSelected()
// *****************************************************
VARIANT_BOOL CMapView::ZoomToSelected(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long numSelected = 0;

	IShapefile* sf = this->GetShapefile(LayerHandle);
	if (sf)
	{
		sf->get_NumSelected(&numSelected);
		
		if (numSelected > 0)
		{
			double xMin, yMin, xMax, yMax;
			ShapefileHelper::GetSelectedExtents(sf, xMin, yMin, xMax, yMax);
			SetExtentsWithPadding(Extent(xMin, xMax, yMin, yMax));
		}

		sf->Release();
	}

	return numSelected > 0 ? VARIANT_TRUE : VARIANT_FALSE;

}

// ****************************************************************
//		ZoomToMaxExtents()
// ****************************************************************
void CMapView::ZoomToMaxExtents()
{
	bool extentsSet = false;

	long endcondition = _activeLayers.size();
	for(int i = 0; i < endcondition; i++ )
	{
		if (this->LayerIsEmpty(_activeLayers[i])) continue;

		Layer * l = _allLayers[_activeLayers[i]];
		this->AdjustLayerExtents(_activeLayers[i]);

		if( extentsSet == false )
		{	
			double xrange = l->extents.right - l->extents.left;
			double yrange = l->extents.top - l->extents.bottom;

			//Neio 2009-07-02 for empty layer will cause the caculation error
			if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
				continue;

			_extents.left = l->extents.left - xrange*m_extentPad;
			_extents.right = l->extents.right + xrange*m_extentPad;
			_extents.top = l->extents.top + yrange*m_extentPad;
			_extents.bottom = l->extents.bottom - yrange*m_extentPad;
			extentsSet = true;
		}
		else
		{	double xrange = l->extents.right - l->extents.left;
			double yrange = l->extents.top - l->extents.bottom;

			//Neio 2009-07-02 for empty layer will cause the caculation error
			if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
				continue;

			if( l->extents.left - xrange*m_extentPad < _extents.left )
				_extents.left = l->extents.left - xrange*m_extentPad;
			if( l->extents.right + xrange*m_extentPad > _extents.right )
				_extents.right = l->extents.right + xrange*m_extentPad;
			if( l->extents.bottom - yrange*m_extentPad < _extents.bottom )
				_extents.bottom = l->extents.bottom - yrange*m_extentPad;
			if( l->extents.top + yrange*m_extentPad > _extents.top )
				_extents.top = l->extents.top + yrange*m_extentPad;
		}
	}

	if( !extentsSet )
		_extents = Extent(0,0,0,0);

	this->SetExtentsCore(_extents);

}

// ****************************************************************
//		ZoomToMaxVisibleExtents()
// ****************************************************************
void CMapView::ZoomToMaxVisibleExtents(void)
{
	bool extentsSet = false;

	register int i;
	long endcondition = _activeLayers.size();
	for( i = 0; i < endcondition; i++ )
	{
		Layer * l = _allLayers[_activeLayers[i]];
		this->AdjustLayerExtents(_activeLayers[i]);
		if( l->get_Visible())
		{
			if( extentsSet == false )
			{	double xrange = l->extents.right - l->extents.left;
				double yrange = l->extents.top - l->extents.bottom;

				if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
					continue;

				_extents.left = l->extents.left - xrange*m_extentPad;
				_extents.right = l->extents.right + xrange*m_extentPad;
				_extents.top = l->extents.top + yrange*m_extentPad;
				_extents.bottom = l->extents.bottom - yrange*m_extentPad;
				extentsSet = true;
			}
			else
			{	double xrange = l->extents.right - l->extents.left;
				double yrange = l->extents.top - l->extents.bottom;

				if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
					continue;

				if( l->extents.left - xrange*m_extentPad < _extents.left )
					_extents.left = l->extents.left - xrange*m_extentPad;
				if( l->extents.right + xrange*m_extentPad > _extents.right )
					_extents.right = l->extents.right + xrange*m_extentPad;
				if( l->extents.bottom - yrange*m_extentPad < _extents.bottom )
					_extents.bottom = l->extents.bottom - yrange*m_extentPad;
				if( l->extents.top + yrange*m_extentPad > _extents.top )
					_extents.top = l->extents.top + yrange*m_extentPad;
			}
		}

	}

	if( ! extentsSet )
		_extents = Extent(0,0,0,0);

	this->SetExtentsCore(_extents);
}

// **************************************************************
//		ZoomToLayer()
// **************************************************************
void CMapView::ZoomToLayer(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,_allLayers) )
	{	
		AdjustLayerExtents(LayerHandle);
		
		Layer * l = _allLayers[LayerHandle];
		SetExtentsWithPadding(l->extents);
	}
	else
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ****************************************************************
//		ZoomToShape()
// ****************************************************************
void CMapView::ZoomToShape(long LayerHandle, long Shape)
{
	ZoomToShape2(LayerHandle, Shape, VARIANT_FALSE);
}

// ****************************************************************
//		SetExtentsWithPadding()
// ****************************************************************
void CMapView::SetExtentsWithPadding(Extent ext)
{
	double xrange = ext.right - ext.left;
	double yrange = ext.top - ext.bottom;

	//if the minimum and maximum extents are the same, use a range of 1 for
	// xrange and yrange to allow a point to be centered in the map window
	// when ZoomToShape is used on a point shapefile.
	if (xrange == 0)
		xrange = 1;

	if (yrange == 0)
		yrange = 1;
	
	ext.left = ext.left - xrange * m_extentPad;
	ext.right = ext.right + xrange * m_extentPad;
	ext.top = ext.top + yrange * m_extentPad;
	ext.bottom = ext.bottom - yrange * m_extentPad;

	this->SetExtentsCore(ext);
}

// ****************************************************************
//		ZoomToShape2()
// ****************************************************************
VARIANT_BOOL CMapView::ZoomToShape2(long LayerHandle, long ShapeIndex, VARIANT_BOOL ifOutsideOnly /*= VARIANT_TRUE*/)
{
	if (!IsValidShape(LayerHandle, ShapeIndex))
		return VARIANT_FALSE;

	IShapefile* sf = GetShapefile(LayerHandle);

	double left, right, top, bottom;
	((CShapefile*)sf)->QuickExtentsCore(ShapeIndex, &left, &bottom, &right, &top);
	sf->Release();

	Extent extNew(left, right, bottom, top);
	if (ifOutsideOnly && extNew.Intersects(_extents)){
		return VARIANT_FALSE;
	}

	SetExtentsWithPadding(extNew);
	return VARIANT_TRUE;
}

#pragma endregion

#pragma region Calculate visible extents
// ***************************************************************
//		CalculateVisibleExtents()
// ***************************************************************
void CMapView::CalculateVisibleExtents( Extent e, bool MapSizeChanged )
{
	double left = MIN( e.left, e.right );
	double right = MAX( e.left, e.right );
	double bottom = MIN( e.bottom, e.top );
	double top = MAX( e.bottom, e.top );
	
	if (left == right)	// lsu 26 jul 2009 for zooming to single point
	{	
		left -= 0.5;  
		right +=0.5;
	}
	if (bottom == top)
	{	
		bottom -=0.5;
		top +=0.5;
	}
	double xrange = right - left;
	double yrange = top - bottom;

	double yextent, xextent;
	double xadjust = 0.0, yadjust = 0.0;
	xextent = yextent = 0.0;
	
	if (!MapSizeChanged && _mapResizeBehavior != rbWarp)
	{
		// size of control is the same, we need just to apply new extents
		if (_viewHeight!= 0 && _viewWidth != 0 && xrange != 0.0 && yrange != 0.0)
		{	 
			// make extents proportional to screen sides ratio
			double ratio = ((double)_viewWidth/(double)_viewHeight)/(xrange/yrange);
			if (ratio > 1 )
			{	
				left = left - xrange * (ratio - 1)/2;
				right = left + xrange * (ratio);
				xrange *= ratio;
			}
			else if (ratio < 1)
			{	
				top = top + yrange * ((1/ratio) - 1)/2;
				bottom = top - yrange * (1/ratio);
				yrange *= (1/ratio);	
			}
		}
	}
	else
	{	
		// size of control changed; we need to calculate new extents
		if ( (_mapResizeBehavior == rbClassic) || (_mapResizeBehavior == rbIntuitive) )
		{
			if( xrange > yrange )
			{
				yextent = (xrange / _viewWidth) * _viewHeight;
				xextent = xrange;
				yadjust = (yextent - yrange) * .5;
				xadjust = 0;
				
				if (_mapResizeBehavior == rbClassic)
				{
					if( yextent < yrange )
					{
						yextent = yrange;
						xextent = (yrange / _viewHeight) * _viewWidth;
						yadjust = 0;
						xadjust = (xextent - xrange) * .5;
					}
				}
			}
			else
			{
				xextent = (yrange / _viewHeight) * _viewWidth;
				yextent = yrange;
				xadjust = (xextent - xrange) * .5;
				yadjust = 0;
				
				if (_mapResizeBehavior == rbClassic)
				{
					if( xextent < xrange )
					{
						xextent = xrange;
						yextent = (xrange / _viewWidth) * _viewHeight;
						xadjust = 0;
						yadjust = (yextent - yrange) * .5;
					}
				}
			}
		}
		else if (_mapResizeBehavior == rbModern)
		{   
			//sizeOption is "modern" - this leaves scale on dX but adjusts scale on dY
			xextent = (yrange / _viewHeight) * _viewWidth;
		    yextent = yrange;
		}
		
		else if (_mapResizeBehavior == rbKeepScale)
		{   
			// lsu (07/03/09) sizeOption is "keep scale", no scale adjustments for both axes
			if (_pixelPerProjectionX == 0 || _pixelPerProjectionY == 0)
			{	xextent = xrange;
				yextent = yrange;
			}
			else
			{	xextent = _viewWidth/_pixelPerProjectionX;
				yextent = _viewHeight/_pixelPerProjectionY;
			}
		}
		else if (_mapResizeBehavior == rbWarp)
		{
			//sizeOption is "Warp" - this does not scale
			xextent = xrange;
			yextent = yrange;
			xadjust = 0;
			yadjust = 0;
		}
		
		left = left - xadjust;
		right = left + xextent;
		bottom = bottom - yadjust;
		if (_mapResizeBehavior == rbKeepScale)
			bottom = top - yextent;
		else
			top = bottom + yextent;	

		xrange = right - left;
		yrange = top - bottom;
	}
	
	// save new extents and recalculate scale
	_extents.left = left;
	_extents.right = right;
	_extents.bottom = bottom;
	_extents.top = top;

	if (xrange == 0 || _viewWidth == 0)
	{
		_pixelPerProjectionX = 0;
		_inversePixelPerProjectionX = 0;
	}
	else
	{
		_pixelPerProjectionX = _viewWidth/xrange;
		_inversePixelPerProjectionX = 1.0/_pixelPerProjectionX;
	}

	if (yrange == 0 || _viewHeight == 0)
	{
		_pixelPerProjectionY = 0;
		_inversePixelPerProjectionY = 0;
	}
	else
	{
		_pixelPerProjectionY = _viewHeight/yrange;
		_inversePixelPerProjectionY = 1.0/_pixelPerProjectionY;
	}
}
#pragma endregion

#pragma region Max extents
// ****************************************************************
//		GetMaxExtents()
// ****************************************************************
IExtents* CMapView::GetMaxExtents(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	bool extentsSet = false;
	Extent maxExtents;

	for(size_t i = 0; i <  _activeLayers.size(); i++ )
	{
		if (this->LayerIsEmpty(_activeLayers[i])) continue;
		
		Layer * l = _allLayers[_activeLayers[i]];
		this->AdjustLayerExtents(_activeLayers[i]);

		if( !extentsSet )
		{	
			maxExtents.left = l->extents.left;
			maxExtents.right = l->extents.right;
			maxExtents.top = l->extents.top;
			maxExtents.bottom = l->extents.bottom;
			extentsSet = true;
		}
		else
		{	

			if( l->extents.left < maxExtents.left )
				maxExtents.left = l->extents.left;
			if( l->extents.right > maxExtents.right )
				maxExtents.right = l->extents.right;
			if( l->extents.bottom < maxExtents.bottom )
				maxExtents.bottom = l->extents.bottom;
			if( l->extents.top > maxExtents.top )
				maxExtents.top = l->extents.top;
		}
	}

	if( !extentsSet )
	{
		maxExtents = Extent(0,0,0,0);
	}
	
	IExtents* ext = NULL;
	ComHelper::CreateExtents(&ext);
	ext->SetBounds(maxExtents.left, maxExtents.bottom, 0.0, maxExtents.right, maxExtents.top, 0.0);
	return ext;
}
#pragma endregion

#pragma region Zoom in/zoom out

// ***************************************************
//		ZoomIn()
// ***************************************************
void CMapView::ZoomIn(double Percent)
{
	if (ForceDiscreteZoom())
	{
		// An attempt to use discrete zoom levels from tiles; unfinished
		int zoom  = GetCurrentZoom();
		int maxZoom, minZoom;
		GetMinMaxZoom(minZoom, maxZoom);
		if (zoom + 1 <= maxZoom) {
			this->ZoomToTileLevel(zoom + 1);
		}
	}
	else {
		double xzin = ((_extents.right - _extents.left)*( 1.0 - Percent ))*.5;
		double xmiddle = _extents.left + (_extents.right - _extents.left)*.5;

		double cLeft = xmiddle - xzin;
		double cRight = xmiddle + xzin;

		double yzin = ((_extents.top - _extents.bottom)*( 1.0 - Percent ))*.5;
		double ymiddle = _extents.bottom + (_extents.top - _extents.bottom)*.5;

		double cBottom = ymiddle - yzin;
		double cTop = ymiddle + yzin;

		this->SetExtentsCore(Extent(cLeft, cRight, cBottom, cTop), true);
	}
}

// ***************************************************
//		ZoomOut()
// ***************************************************
void CMapView::ZoomOut(double Percent)
{
	if (ForceDiscreteZoom())
	{
		int zoom = GetCurrentZoom();
		int minZoom, maxZoom;
		GetMinMaxZoom(minZoom, maxZoom);
		if (zoom - 1 >= minZoom) {
			this->ZoomToTileLevel(zoom - 1);
		}
	}
	else 
	{
		double xzout = ((_extents.right - _extents.left)*( 1.0 + Percent ))*.5;
		double xmiddle = _extents.left + (_extents.right - _extents.left)*.5;

		double cLeft = xmiddle - xzout;
		double cRight = xmiddle + xzout;

		double yzout = ((_extents.top - _extents.bottom)*( 1.0 + Percent ))*.5;
		double ymiddle = _extents.bottom + (_extents.top - _extents.bottom)*.5;

		double cBottom = ymiddle - yzout;
		double cTop = ymiddle + yzout;

		this->SetExtentsCore(Extent( cLeft, cRight, cBottom, cTop ), true);
	}
}

// ***************************************************
//		ValidatePreviousExtent()
// ***************************************************
bool CMapView::ValidatePreviousExtent()
{
	bool result = _prevExtentsIndex >= 0 && _prevExtentsIndex < (int)_prevExtents.size();

	if (!result && _prevExtents.size() > 0)
	{
		CallbackHelper::AssertionFailed("Previous extent index out of bounds");
	}

	return result;
}

// ***************************************************
//		MapIsEmpty()
// ***************************************************
bool CMapView::MapIsEmpty()
{
	return (_transformationMode == tmNotDefined || GetTileProvider() == tkTileProvider::ProviderNone) && GetNumLayers() == 0;
}

// ***************************************************
//		LogPrevExtent()
// ***************************************************
void CMapView::LogPrevExtent()
{
	if (MapIsEmpty()) return;

	// maybe they are the same extents
	if (ValidatePreviousExtent())
	{
		if (_extents == _prevExtents[_prevExtentsIndex]) return;
	}

	// let's discard part of the history that was reverted with ZoomToPrev
	int removeCount = _prevExtents.size() - 1 - _prevExtentsIndex;
	if (removeCount > 0)
	{
		int count = 0;
		do
		{
			_prevExtents.pop_back();
			count++;
		} 
		while (count < removeCount);
	}

	// add new extent
	_prevExtents.push_back( _extents );

	// make sure that it's no longer than expected
	if (_extentHistoryCount > 0 && (long)_prevExtents.size() > _extentHistoryCount)
	{
		_prevExtents.erase(_prevExtents.begin());
	}

	_prevExtentsIndex = _prevExtents.size() - 1;
}

// ***************************************************
//		ZoomToPrev()
// ***************************************************
long CMapView::ZoomToPrev()
{
	if( _prevExtents.size() > 0 && _prevExtentsIndex > 0)
	{	
		_prevExtentsIndex--;
		
		if (!ValidatePreviousExtent()) 
		{
			return 0;
		}

		Extent ext = _prevExtents[_prevExtentsIndex];
		SetExtentsCore(ext, false);

		//CallbackHelper::ErrorMsg("Zoom prev: " + ext.ToString());
	}

	return GetExtentHistoryUndoCount();
}

// ***************************************************
//		ZoomToNext()
// ***************************************************
long CMapView::ZoomToNext()
{
	if (_prevExtents.size() == 0)
	{
		return 0;
	}

	if (_prevExtentsIndex < (int)(_prevExtents.size() - 1))
	{
		_prevExtentsIndex++;
		Extent ext = _prevExtents[_prevExtentsIndex];

		if (!ValidatePreviousExtent())
		{
			return 0;
		}

		SetExtentsCore(ext, false);
		//CallbackHelper::ErrorMsg("Zoom next: " + ext.ToString());
	}

	return GetExtentHistoryRedoCount();
}

// ***************************************************
//		GetExtentHistoryUndoCount()
// ***************************************************
long CMapView::GetExtentHistoryUndoCount()
{
	return _prevExtentsIndex;
}

// ***************************************************
//		GetExtentHistoryRedoCount()
// ***************************************************
long CMapView::GetExtentHistoryRedoCount()
{
	if (_prevExtents.size() == 0)
	{
		return 0;
	}

	return (_prevExtents.size() - 1) - _prevExtentsIndex;
}

// ***************************************************
//		ClearExtentsHistory()
// ***************************************************
void CMapView::ClearExtentHistory()
{
	_prevExtents.clear();
	_prevExtentsIndex = 0;
}

// ****************************************************************
//		ZoomToWorld()
// ****************************************************************
VARIANT_BOOL CMapView::ZoomToWorld()
{
	VARIANT_BOOL vb;
	GetMapProjection()->get_IsEmpty(&vb);
	if (!vb) {
		CComPtr<IExtents> box = NULL;
		box.Attach(ExtentsHelper::GetWorldBounds());
		vb = this->SetGeographicExtents(box);
		if (vb)  {
			Redraw();
		}
		return vb;
	}
	else {
		ErrorMessage(tkMAP_PROJECTION_NOT_SET, cvAll);
	}
	return false;
}

// ****************************************************************
//		SetGeographicExtents2()
// ****************************************************************
VARIANT_BOOL CMapView::SetGeographicExtents2(double xLongitude, double yLatitude, double widthKilometers)
{
	if (abs(xLongitude) > 179.5 || abs(yLatitude) > 89.5) {
		// TODO: accept values in [-90, 90] [-180, 180]
		ErrorMessage(tkINVALID_GEOGRAPHIC_COORDINATES);
		return VARIANT_FALSE;
	}
	else 
	{
		IExtents* box = NULL;
		ComHelper::CreateExtents(&box);

		double dy = 0.0, dx = 0.0;				// meters per degree
		GetUtils()->GeodesicDistance(yLatitude - 0.5, xLongitude, yLatitude + 0.5, xLongitude, &dy);
		GetUtils()->GeodesicDistance(yLatitude, xLongitude - 0.5, yLatitude, xLongitude + 0.5, &dx);

		double distortion = dy / dx;
		
		dx = widthKilometers * 1000.0 / dx;  	// in degrees
		dy = dx / (_viewHeight / _viewWidth) / distortion;
		
		box->SetBounds(xLongitude - dx / 2.0, yLatitude - dy / 2.0, 0.0, xLongitude + dx / 2.0, yLatitude + dy / 2.0, 0.0);
		VARIANT_BOOL vb = this->SetGeographicExtents(box);
		box->Release();
		return vb;
	}
	return VARIANT_FALSE;
}

// ****************************************************************
//		ZoomToKnownExtents()
// ****************************************************************
IExtents* CMapView::GetKnownExtents(tkKnownExtents extents)
{
	IExtents* box = NULL;
	ComHelper::CreateExtents(&box);

	// generated from MW4 projection database
	switch(extents) {
		case keWorld: box->SetBounds(-179.0, -85.0, 0.0, 179.0, 85.0, 0.0);	break;
		case keAfghanistan: box->SetBounds( 60.504166,  29.406105, 0.0,  74.915741,  38.472115, 0.0); break;
		case keAland_Islands: box->SetBounds( 19.510555,  59.976944, 0.0,  20.442497,  60.40361, 0.0); break;
		case keAlbania: box->SetBounds( 19.282497,  39.644722, 0.0,  21.054165,  42.661942, 0.0); break;
		case keAlgeria: box->SetBounds(-8.667223,  18.976387, 0.0,  11.986475,  37.091385, 0.0); break;
		case keAmerican_Samoa: box->SetBounds(-170.82611, -14.375555, 0.0, -169.43832, -14.166389, 0.0); break;
		case keAndorra: box->SetBounds( 1.421389,  42.436104, 0.0,  1.78172,  42.656387, 0.0); break;
		case keAngola: box->SetBounds( 11.663332, -18.016392, 0.0,  24.084442, -4.388991, 0.0); break;
		case keAnguilla: box->SetBounds(-63.167778,  18.164444, 0.0, -62.969452,  18.276665, 0.0); break;
		case keAntigua: box->SetBounds(-61.891113,  16.989719, 0.0, -61.666389,  17.724998, 0.0); break;
		case keArgentina: box->SetBounds(-73.583618, -55.051674, 0.0, -53.649727, -21.780521, 0.0); break;
		case keArmenia: box->SetBounds( 43.453888,  38.841148, 0.0,  46.62249,  41.29705, 0.0); break;
		case keAruba: box->SetBounds(-70.063339,  12.41111, 0.0, -69.873337,  12.631109, 0.0); break;
		case keAustralia: box->SetBounds( 112.907211, -54.753891, 0.0,  159.101898, -10.05139, 0.0); break;
		case keAustria: box->SetBounds( 9.533569,  46.407494, 0.0,  17.166386,  49.018883, 0.0); break;
		case keAzerbaijan: box->SetBounds( 44.778862,  38.389153, 0.0,  50.374992,  41.897057, 0.0); break;
		case keBahamas: box->SetBounds(-78.978897,  20.915276, 0.0, -72.737503,  26.929165, 0.0); break;
		case keBahrain: box->SetBounds( 50.453049,  25.571941, 0.0,  50.822495,  26.288887, 0.0); break;
		case keBangladesh: box->SetBounds( 88.04332,  20.738049, 0.0,  92.669342,  26.631939, 0.0); break;
		case keBarbados: box->SetBounds(-59.659447,  13.050554, 0.0, -59.426949,  13.337221, 0.0); break;
		case keBelarus: box->SetBounds( 23.1654,  51.251846, 0.0,  32.741379,  56.16777, 0.0); break;
		case keBelgium: box->SetBounds( 2.541667,  49.504166, 0.0,  6.398204,  51.503609, 0.0); break;
		case keBelize: box->SetBounds(-89.2164,  15.889851, 0.0, -87.7789,  18.489902, 0.0); break;
		case keBenin: box->SetBounds( .776667,  6.218721, 0.0,  3.855,  12.396658, 0.0); break;
		case keBermuda: box->SetBounds(-64.876114,  32.260551, 0.0, -64.638626,  32.382217, 0.0); break;
		case keBhutan: box->SetBounds( 88.751938,  26.703049, 0.0,  92.115265,  28.325275, 0.0); break;
		case keBolivia: box->SetBounds(-69.656189, -22.901112, 0.0, -57.521118, -9.679195, 0.0); break;
		case keBosnia_and_Herzegovina: box->SetBounds( 15.736387,  42.565826, 0.0,  19.621765,  45.265945, 0.0); break;
		case keBotswana: box->SetBounds( 19.996109, -26.875557, 0.0,  29.373623, -17.781391, 0.0); break;
		case keBrazil: box->SetBounds(-74.010559, -33.743896, 0.0, -29.84,  5.273889, 0.0); break;
		case keBritish_Virgin_Islands: box->SetBounds(-64.700287,  18.383888, 0.0, -64.26918,  18.748608, 0.0); break;
		case keBrunei: box->SetBounds( 114.095078,  4.017499, 0.0,  115.36026,  5.053054, 0.0); break;
		case keBulgaria: box->SetBounds( 22.365276,  41.24305, 0.0,  28.606384,  44.224716, 0.0); break;
		case keBurkina_Faso: box->SetBounds(-5.521111,  9.393888, 0.0,  2.397925,  15.082777, 0.0); break;
		case keBurundi: box->SetBounds( 28.983887, -4.448056, 0.0,  30.853886, -2.298056, 0.0); break;
		case keCambodia: box->SetBounds( 102.345543,  10.422739, 0.0,  107.636383,  14.708618, 0.0); break;
		case keCameroon: box->SetBounds( 8.502222,  1.654166, 0.0,  16.207222,  13.085278, 0.0); break;
		case keCanada: box->SetBounds(-141.00299,  41.675552, 0.0, -52.614449,  83.113876, 0.0); break;
		case keCape_Verde: box->SetBounds(-25.360558,  14.81111, 0.0, -22.665836,  17.193054, 0.0); break;
		case keCayman_Islands: box->SetBounds(-81.401123,  19.264721, 0.0, -79.732788,  19.762218, 0.0); break;
		case keCentral_African_Republic: box->SetBounds( 14.41861,  2.220833, 0.0,  27.460278,  11.001389, 0.0); break;
		case keChad: box->SetBounds( 13.461666,  7.457777, 0.0,  24.002747,  23.450554, 0.0); break;
		case keChile: box->SetBounds(-109.44917, -55.919724, 0.0, -66.419174, -17.50528, 0.0); break;
		case keChina: box->SetBounds( 73.617203,  18.168884, 0.0,  134.77359,  53.554436, 0.0); break;
		case keCocos_Islands: box->SetBounds( 96.81749, -12.199999, 0.0,  96.924423, -12.128332, 0.0); break;
		case keColombia: box->SetBounds(-81.722778, -4.236874, 0.0, -66.871887,  13.378611, 0.0); break;
		case keComoros: box->SetBounds( 43.213608, -12.383057, 0.0,  44.53083, -11.366945, 0.0); break;
		case keCongo: box->SetBounds( 11.140661, -5.019444, 0.0,  18.643608,  3.713055, 0.0); break;
		case keCook_Islands: box->SetBounds(-165.85028, -21.940834, 0.0, -157.30587, -8.948057, 0.0); break;
		case keCosta_Rica: box->SetBounds(-85.911392,  8.025669, 0.0, -82.561401,  11.21361, 0.0); break;
		case keCroatia: box->SetBounds( 13.496387,  42.39666, 0.0,  19.426109,  46.535828, 0.0); break;
		case keCuba: box->SetBounds(-84.953339,  19.821941, 0.0, -74.130844,  23.204166, 0.0); break;
		case keCyprus: box->SetBounds( 32.269165,  34.56255, 0.0,  34.590553,  35.690277, 0.0); break;
		case keCzech_Republic: box->SetBounds( 12.093704,  48.581379, 0.0,  18.852219,  51.053604, 0.0); break;
		case keDenmark: box->SetBounds( 8.087221,  54.561661, 0.0,  15.15,  57.746666, 0.0); break;
		case keDjibouti: box->SetBounds( 41.75972,  10.941944, 0.0,  43.42083,  12.708332, 0.0); break;
		case keDominica: box->SetBounds(-61.491394,  15.198055, 0.0, -61.250557,  15.631943, 0.0); break;
		case keDominican_Republic: box->SetBounds(-72.003067,  17.540276, 0.0, -68.322235,  19.93111, 0.0); break;
		case keDR_Congo: box->SetBounds( 12.214552, -13.458057, 0.0,  31.302776,  5.381389, 0.0); break;
		case keEcuador: box->SetBounds(-91.663895, -5.009132, 0.0, -75.21608,  1.437778, 0.0); break;
		case keEgypt: box->SetBounds( 24.706665,  21.994164, 0.0,  36.898331,  31.646942, 0.0); break;
		case keEl_Salvador: box->SetBounds(-90.108337,  13.156387, 0.0, -87.684723,  14.431982, 0.0); break;
		case keEquatorial_Guinea: box->SetBounds( 5.615277, -1.479445, 0.0,  11.353888,  3.763333, 0.0); break;
		case keEritrea: box->SetBounds( 36.443283,  12.363888, 0.0,  43.121384,  17.994881, 0.0); break;
		case keEstonia: box->SetBounds( 21.83194,  57.522217, 0.0,  28.195274,  59.668327, 0.0); break;
		case keEthiopia: box->SetBounds( 32.991104,  3.406389, 0.0,  47.988243,  14.88361, 0.0); break;
		case keFaeroe_Islands: box->SetBounds(-7.435,  61.388329, 0.0, -6.388612,  62.396942, 0.0); break;
		case keFalkland_Islands: box->SetBounds(-61.315834, -52.343056, 0.0, -57.731392, -51.249451, 0.0); break;
		case keFiji: box->SetBounds(-180, -20.674442, 0.0,  180, -12.481943, 0.0); break;
		case keFinland: box->SetBounds( 20.580929,  59.804993, 0.0,  31.588928,  70.088882, 0.0); break;
		case keFrance: box->SetBounds(-5.134723,  41.364166, 0.0,  9.562222,  51.09111, 0.0); break;
		case keFrench_Guiana: box->SetBounds(-54.603783,  2.112222, 0.0, -51.647781,  5.755555, 0.0); break;
		case keFrench_Polynesia: box->SetBounds(-152.87973, -27.915554, 0.0, -134.9414, -7.888333, 0.0); break;
		case keGabon: box->SetBounds( 8.698332, -3.925277, 0.0,  14.520555,  2.317898, 0.0); break;
		case keGambia: box->SetBounds(-16.821667,  13.059977, 0.0, -13.798613,  13.826387, 0.0); break;
		case keGeorgia: box->SetBounds( 40.002968,  41.046097, 0.0,  46.710815,  43.584717, 0.0); break;
		case keGermany: box->SetBounds( 5.864166,  47.274719, 0.0,  15.038887,  55.056664, 0.0); break;
		case keGhana: box->SetBounds(-3.249167,  4.726388, 0.0,  1.202778,  11.166666, 0.0); break;
		case keGibraltar: box->SetBounds(-5.35624,  36.112175, 0.0, -5.334508,  36.163307, 0.0); break;
		case keGreat_Britain: box->SetBounds(-8.621389,  49.911659, 0.0,  1.749444,  60.844444, 0.0); break;
		case keGreece: box->SetBounds( 19.37611,  34.808884, 0.0,  28.238049,  41.748322, 0.0); break;
		case keGreenland: box->SetBounds(-73.053604,  59.790276, 0.0, -12.155001,  83.623596, 0.0); break;
		case keGrenada: box->SetBounds(-61.789726,  11.996387, 0.0, -61.418617,  12.529165, 0.0); break;
		case keGuadeloupe: box->SetBounds(-62.873062,  15.869999, 0.0, -60.988617,  17.930275, 0.0); break;
		case keGuam: box->SetBounds( 144.634155,  13.234997, 0.0,  144.953308,  13.65361, 0.0); break;
		case keGuatemala: box->SetBounds(-92.24678,  13.745832, 0.0, -88.214737,  17.82111, 0.0); break;
		case keGuernsey: box->SetBounds(-2.670278,  49.422493, 0.0, -2.500278,  49.508888, 0.0); break;
		case keGuinea: box->SetBounds(-15.081112,  7.198889, 0.0, -7.646536,  12.6775, 0.0); break;
		case keGuinea_Bissau: box->SetBounds(-16.71777,  10.922777, 0.0, -13.643057,  12.684721, 0.0); break;
		case keGuyana: box->SetBounds(-61.389725,  1.185555, 0.0, -56.470634,  8.535276, 0.0); break;
		case keHaiti: box->SetBounds(-74.467789,  18.022778, 0.0, -71.628891,  20.09222, 0.0); break;
		case keHonduras: box->SetBounds(-89.351959,  12.979721, 0.0, -83.131851,  17.420277, 0.0); break;
		case keHungary: box->SetBounds( 16.111805,  45.748329, 0.0,  22.894804,  48.57666, 0.0); break;
		case keIceland: box->SetBounds(-24.542225,  63.389999, 0.0, -13.499445,  66.536102, 0.0); break;
		case keIndia: box->SetBounds( 68.139435,  6.745554, 0.0,  97.380539,  35.506104, 0.0); break;
		case keIndonesia: box->SetBounds( 95.008026, -10.93, 0.0,  141.007019,  5.913888, 0.0); break;
		case keIran: box->SetBounds( 44.034157,  25.075275, 0.0,  63.341934,  39.78054, 0.0); break;
		case keIraq: box->SetBounds( 38.794701,  29.061661, 0.0,  48.563881,  37.38472, 0.0); break;
		case keIreland: box->SetBounds(-10.474724,  51.445549, 0.0, -6.013056,  55.380272, 0.0); break;
		case keIsle_of_Man: box->SetBounds(-4.788611,  54.05555, 0.0, -4.307501,  54.416664, 0.0); break;
		case keIsrael: box->SetBounds( 34.267578,  29.486706, 0.0,  35.683052,  33.270271, 0.0); break;
		case keItaly: box->SetBounds( 6.61976,  36.649162, 0.0,  18.514999,  47.094719, 0.0); break;
		case keIvory_Coast: box->SetBounds(-8.606384,  4.344722, 0.0, -2.487778,  10.735256, 0.0); break;
		case keJamaica: box->SetBounds(-78.373901,  17.696663, 0.0, -76.221115,  18.522499, 0.0); break;
		case keJapan: box->SetBounds( 122.935257,  24.250832, 0.0,  153.96579,  45.486382, 0.0); break;
		case keJersey: box->SetBounds(-2.2475,  49.16777, 0.0, -2.015,  49.261108, 0.0); break;
		case keJordan: box->SetBounds( 34.959999,  29.188889, 0.0,  39.301109,  33.377594, 0.0); break;
		case keKazakhstan: box->SetBounds( 46.499161,  40.594437, 0.0,  87.348206,  55.44471, 0.0); break;
		case keKenya: box->SetBounds( 33.907219, -4.669618, 0.0,  41.905167,  4.622499, 0.0); break;
		case keKiribati: box->SetBounds(-172.23333, -11.466665, 0.0,  176.85025,  4.725832, 0.0); break;
		case keKuwait: box->SetBounds( 46.546944,  28.538883, 0.0,  48.416588,  30.084438, 0.0); break;
		case keKyrgyzstan: box->SetBounds( 69.248871,  39.191856, 0.0,  80.283325,  43.216904, 0.0); break;
		case keLaos: box->SetBounds( 100.09137,  13.926664, 0.0,  107.695251,  22.500832, 0.0); break;
		case keLatvia: box->SetBounds( 20.968605,  55.674835, 0.0,  28.237774,  58.084435, 0.0); break;
		case keLebanon: box->SetBounds( 35.10083,  33.061943, 0.0,  36.623741,  34.647499, 0.0); break;
		case keLesotho: box->SetBounds( 27.011108, -30.650528, 0.0,  29.456108, -28.569447, 0.0); break;
		case keLiberia: box->SetBounds(-11.492331,  4.343333, 0.0, -7.366667,  8.512777, 0.0); break;
		case keLibya: box->SetBounds( 9.303888,  19.499065, 0.0,  25.152775,  33.171135, 0.0); break;
		case keLiechtenstein: box->SetBounds( 9.474637,  47.057457, 0.0,  9.63611,  47.274544, 0.0); break;
		case keLithuania: box->SetBounds( 20.942833,  53.888046, 0.0,  26.819717,  56.450829, 0.0); break;
		case keLuxembourg: box->SetBounds( 5.734444,  49.448326, 0.0,  6.524722,  50.18222, 0.0); break;
		case keMacao: box->SetBounds( 113.531372,  22.183052, 0.0,  113.556374,  22.214439, 0.0); break;
		case keMacedonia: box->SetBounds( 20.457775,  40.855888, 0.0,  23.032776,  42.361382, 0.0); break;
		case keMadagascar: box->SetBounds( 43.236824, -25.588337, 0.0,  50.501389, -11.945557, 0.0); break;
		case keMalawi: box->SetBounds( 32.678886, -17.135281, 0.0,  35.924164, -9.373335, 0.0); break;
		case keMalaysia: box->SetBounds( 99.640823,  .852778, 0.0,  119.275818,  7.35361, 0.0); break;
		case keMaldives: box->SetBounds( 72.687759, -.690833, 0.0,  73.753601,  7.096388, 0.0); break;
		case keMali: box->SetBounds(-12.244833,  10.141109, 0.0,  4.2525,  25.000275, 0.0); break;
		case keMalta: box->SetBounds( 14.180832,  35.799995, 0.0,  14.57,  36.074997, 0.0); break;
		case keMarshall_Islands: box->SetBounds( 162.323578,  5.600277, 0.0,  172.090515,  14.598331, 0.0); break;
		case keMartinique: box->SetBounds(-61.231674,  14.402777, 0.0, -60.816673,  14.880278, 0.0); break;
		case keMauritania: box->SetBounds(-17.075558,  14.725321, 0.0, -4.806111,  27.290459, 0.0); break;
		case keMauritius: box->SetBounds( 56.507217, -20.520557, 0.0,  63.498604, -10.316668, 0.0); break;
		case keMayotte: box->SetBounds( 45.039162, -12.9925, 0.0,  45.293327, -12.6625, 0.0); break;
		case keMexico: box->SetBounds(-118.40416,  14.550547, 0.0, -86.701401,  32.718456, 0.0); break;
		case keMicronesia: box->SetBounds( 138.058319,  5.261666, 0.0,  163.043304,  9.589441, 0.0); break;
		case keMoldova: box->SetBounds( 26.634995,  45.448647, 0.0,  30.133228,  48.468323, 0.0); break;
		case keMonaco: box->SetBounds( 7.386389,  43.727547, 0.0,  7.439293,  43.773048, 0.0); break;
		case keMongolia: box->SetBounds( 87.758331,  41.581383, 0.0,  119.934982,  52.143608, 0.0); break;
		case keMontenegro: box->SetBounds( 18.453331,  41.848999, 0.0,  20.382774,  43.556107, 0.0); break;
		case keMontserrat: box->SetBounds(-62.237228,  16.671387, 0.0, -62.137505,  16.81361, 0.0); break;
		case keMorocco: box->SetBounds(-13.174961,  27.664238, 0.0, -1.010278,  35.919167, 0.0); break;
		case keMozambique: box->SetBounds( 30.213017, -26.860279, 0.0,  40.846107, -10.471111, 0.0); break;
		case keNamibia: box->SetBounds( 11.716389, -28.962502, 0.0,  25.264431, -16.952778, 0.0); break;
		case keNauru: box->SetBounds( 166.904419, -.552222, 0.0,  166.958588, -.493333, 0.0); break;
		case keNepal: box->SetBounds( 80.0522,  26.364719, 0.0,  88.195816,  30.424995, 0.0); break;
		case keNetherlands: box->SetBounds( 3.370866,  50.753883, 0.0,  7.211666,  53.511383, 0.0); break;
		case keNew_Caledonia: box->SetBounds( 159.922211, -22.694164, 0.0,  171.313873, -19.114445, 0.0); break;
		case keNew_Zealand: box->SetBounds(-178.61306, -52.578056, 0.0,  179.082733, -29.223057, 0.0); break;
		case keNicaragua: box->SetBounds(-87.693069,  10.708611, 0.0, -82.72139,  15.022221, 0.0); break;
		case keNiger: box->SetBounds( .166667,  11.693274, 0.0,  15.996666,  23.522305, 0.0); break;
		case keNigeria: box->SetBounds( 2.6925,  4.272499, 0.0,  14.658054,  13.891499, 0.0); break;
		case keNiue: box->SetBounds(-169.95306, -19.145557, 0.0, -169.7814, -18.963333, 0.0); break;
		case keNorfolk_Island: box->SetBounds( 167.909424, -29.081112, 0.0,  168, -29.000557, 0.0); break;
		case keNorth_Korea: box->SetBounds( 124.322769,  37.671379, 0.0,  130.697418,  43.008324, 0.0); break;
		case keNorthern_Mariana_Islands: box->SetBounds( 144.89859,  14.105276, 0.0,  145.870789,  20.556385, 0.0); break;
		case keNorway: box->SetBounds( 4.62,  57.987778, 0.0,  31.078053,  71.154709, 0.0); break;
		case keOccupied_Palestinian_Territory: box->SetBounds( 34.21666,  31.216541, 0.0,  35.573296,  32.546387, 0.0); break;
		case keOman: box->SetBounds( 51.99929,  16.642778, 0.0,  59.847221,  26.382389, 0.0); break;
		case kePakistan: box->SetBounds( 60.866302,  23.688049, 0.0,  77.823929,  37.062592, 0.0); break;
		case kePalau: box->SetBounds( 132.208313,  5.292221, 0.0,  134.658875,  7.729444, 0.0); break;
		case kePanama: box->SetBounds(-83.030289,  7.206111, 0.0, -77.198334,  9.620277, 0.0); break;
		case kePapua_New_Guinea: box->SetBounds( 140.858856, -11.6425, 0.0,  159.523041, -1.098333, 0.0); break;
		case keParaguay: box->SetBounds(-62.643768, -27.588337, 0.0, -54.243896, -19.296669, 0.0); break;
		case kePeru: box->SetBounds(-81.3564, -18.348545, 0.0, -68.673904, -.031389, 0.0); break;
		case kePhilippines: box->SetBounds( 116.949997,  4.641388, 0.0,  126.598038,  21.118053, 0.0); break;
		case kePitcairn: box->SetBounds(-130.10748, -25.082226, 0.0, -124.77113, -24.325005, 0.0); break;
		case kePoland: box->SetBounds( 14.145555,  49.001938, 0.0,  24.144718,  54.836937, 0.0); break;
		case kePortugal: box->SetBounds(-31.290001,  32.637497, 0.0, -6.187222,  42.15274, 0.0); break;
		case kePuerto_Rico: box->SetBounds(-67.938339,  17.922222, 0.0, -65.241959,  18.519444, 0.0); break;
		case keQatar: box->SetBounds( 50.751938,  24.556042, 0.0,  51.615829,  26.15361, 0.0); break;
		case keReunion_Island: box->SetBounds( 55.219719, -21.37389, 0.0,  55.85305, -20.856392, 0.0); break;
		case keRomania: box->SetBounds( 20.261024,  43.622437, 0.0,  29.672497,  48.263885, 0.0); break;
		case keRussia: box->SetBounds(-180,  41.196091, 0.0,  180,  81.851929, 0.0); break;
		case keRwanda: box->SetBounds( 28.853333, -2.826667, 0.0,  30.894444, -1.053889, 0.0); break;
		case keSaint_Barthelemy: box->SetBounds(-63.139839,  18.015553, 0.0, -63.010284,  18.070366, 0.0); break;
		case keSaint_Martin_French_part: box->SetBounds(-63.146667,  18.058601, 0.0, -63.006393,  18.121944, 0.0); break;
		case keSamoa: box->SetBounds(-172.7806, -14.057503, 0.0, -171.42865, -13.460556, 0.0); break;
		case keSan_Marino: box->SetBounds( 12.403889,  43.895554, 0.0,  12.511665,  43.989166, 0.0); break;
		case keSao_Tome_and_Principe: box->SetBounds( 6.464444,  .018333, 0.0,  7.464167,  1.701944, 0.0); break;
		case keSaudi_Arabia: box->SetBounds( 34.492218,  15.616943, 0.0,  55.666107,  32.154942, 0.0); break;
		case keSenegal: box->SetBounds(-17.537224,  12.301748, 0.0, -11.3675,  16.693054, 0.0); break;
		case keSerbia: box->SetBounds( 18.81702,  41.855827, 0.0,  23.004997,  46.181389, 0.0); break;
		case keSeychelles: box->SetBounds( 46.204163, -9.755001, 0.0,  56.28611, -4.280001, 0.0); break;
		case keSierra_Leone: box->SetBounds(-13.29561,  6.923611, 0.0, -10.264168,  9.997499, 0.0); break;
		case keSingapore: box->SetBounds( 103.640808,  1.258889, 0.0,  103.998863,  1.445277, 0.0); break;
		case keSlovakia: box->SetBounds( 16.839996,  47.737221, 0.0,  22.558052,  49.60083, 0.0); break;
		case keSlovenia: box->SetBounds( 13.383055,  45.425819, 0.0,  16.607872,  46.876663, 0.0); break;
		case keSolomon_Islands: box->SetBounds( 155.507477, -11.845833, 0.0,  167.209961, -5.293056, 0.0); break;
		case keSomalia: box->SetBounds( 40.986595, -1.674868, 0.0,  51.412636,  11.979166, 0.0); break;
		case keSouth_Africa: box->SetBounds( 16.48333, -46.969727, 0.0,  37.981667, -22.136391, 0.0); break;
		case keSouth_Korea: box->SetBounds( 124.609711,  33.190269, 0.0,  130.924133,  38.625244, 0.0); break;
		case keSpain: box->SetBounds(-18.170559,  27.637497, 0.0,  4.316944,  43.772217, 0.0); break;
		case keSri_Lanka: box->SetBounds( 79.651932,  5.917777, 0.0,  81.891663,  9.828331, 0.0); break;
		case keSt_Helena: box->SetBounds(-14.416113, -40.403893, 0.0, -5.645278, -7.883056, 0.0); break;
		case keSt_Kitts_and_Nevis: box->SetBounds(-62.863892,  17.091663, 0.0, -62.534172,  17.410831, 0.0); break;
		case keSt_Lucia: box->SetBounds(-61.079727,  13.709444, 0.0, -60.878059,  14.109444, 0.0); break;
		case keSt_Pierre_and_Miquelon: box->SetBounds(-56.398056,  46.747215, 0.0, -56.144165,  47.136658, 0.0); break;
		case keSt_Vincent: box->SetBounds(-61.45417,  12.584444, 0.0, -61.120285,  13.384165, 0.0); break;
		case keSudan: box->SetBounds( 21.827774,  3.493394, 0.0,  38.607498,  22.23222, 0.0); break;
		case keSuriname: box->SetBounds(-58.071396,  1.835556, 0.0, -53.984169,  6.003055, 0.0); break;
		case keSvalbard_and_Jan_Mayen: box->SetBounds(-9.120058,  70.803864, 0.0,  36.853325,  80.76416, 0.0); break;
		case keSwaziland: box->SetBounds( 30.798332, -27.316669, 0.0,  32.1334, -25.728336, 0.0); break;
		case keSweden: box->SetBounds( 11.106943,  55.339165, 0.0,  24.16861,  69.060303, 0.0); break;
		case keSwitzerland: box->SetBounds( 5.96611,  45.829437, 0.0,  10.488913,  47.806938, 0.0); break;
		case keSyria: box->SetBounds( 35.614464,  32.313606, 0.0,  42.379166,  37.290543, 0.0); break;
		case keTajikistan: box->SetBounds( 67.3647,  36.671844, 0.0,  75.187485,  41.050224, 0.0); break;
		case keTanzania: box->SetBounds( 29.340832, -11.740835, 0.0,  40.436813, -.997222, 0.0); break;
		case keThailand: box->SetBounds( 97.345261,  5.63111, 0.0,  105.639427,  20.455273, 0.0); break;
		case keTimor_Leste: box->SetBounds( 124.046161, -9.4633795, 0.0,  127.308594, -8.324444, 0.0); break;
		case keTogo: box->SetBounds(-.149762,  6.100546, 0.0,  1.799327,  11.13854, 0.0); break;
		case keTokelau: box->SetBounds(-172.50033, -9.381111, 0.0, -171.21142, -8.553614, 0.0); break;
		case keTonga: box->SetBounds(-175.68472, -21.454166, 0.0, -173.90615, -15.56028, 0.0); break;
		case keTrinidad_and_Tobago: box->SetBounds(-61.924446,  10.037498, 0.0, -60.520561,  11.346109, 0.0); break;
		case keTunisia: box->SetBounds( 7.491666,  30.23439, 0.0,  11.583332,  37.539444, 0.0); break;
		case keTurkey: box->SetBounds( 25.663883,  35.817497, 0.0,  44.822762,  42.109993, 0.0); break;
		case keTurkmenistan: box->SetBounds( 52.440071,  35.141663, 0.0,  66.672485,  42.797775, 0.0); break;
		case keTurks_and_Caicos_Islands: box->SetBounds(-72.468063,  21.430275, 0.0, -71.127792,  21.957775, 0.0); break;
		case keTuvalu: box->SetBounds( 176.066376, -8.561292, 0.0,  179.232285, -5.657778, 0.0); break;
		case keUganda: box->SetBounds( 29.570831, -1.47611, 0.0,  35.00972,  4.222777, 0.0); break;
		case keUkraine: box->SetBounds( 22.151442,  44.37915, 0.0,  40.179718,  52.379715, 0.0); break;
		case keUnited_Arab_Emirates: box->SetBounds( 51.583328,  22.633329, 0.0,  56.38166,  26.08416, 0.0); break;
		case keUruguay: box->SetBounds(-58.438614, -34.948891, 0.0, -53.093056, -30.096668, 0.0); break;
		case keUS_Virgin_Islands: box->SetBounds(-65.026947,  17.676666, 0.0, -64.560287,  18.377777, 0.0); break;
		case keUSA: box->SetBounds(-124.731422,  24.955967, 0.0,  -66.969849,  49.371735, 0.0); break;
		case keUzbekistan: box->SetBounds( 55.99749,  37.183876, 0.0,  73.173035,  45.571106, 0.0); break;
		case keVanuatu: box->SetBounds( 166.516663, -20.254169, 0.0,  170.235229, -13.070555, 0.0); break;
		case keVenezuela: box->SetBounds(-73.378067,  .648611, 0.0, -59.801392,  12.198889, 0.0); break;
		case keVietnam: box->SetBounds( 102.140747,  8.558609, 0.0,  109.466377,  23.334721, 0.0); break;
		case keWallis_and_Futuna: box->SetBounds(-178.1911, -14.323891, 0.0, -176.12109, -13.213614, 0.0); break;
		case keWestern_Sahara: box->SetBounds(-17.105278,  20.764095, 0.0, -8.666389,  27.666958, 0.0); break;
		case keYemen: box->SetBounds( 42.555832,  12.10611, 0.0,  54.476944,  18.999344, 0.0); break;
		case keZambia: box->SetBounds( 21.996387, -18.076126, 0.0,  33.702278, -8.191668, 0.0); break;
		case keZimbabwe: box->SetBounds( 25.236664, -22.414764, 0.0,  33.073051, -15.616112, 0.0); break;
		case keGreenwich: box->SetBounds( -0.1, 51.4791 - 0.1, 0.0,  0.1, 51.4791 + 0.1, 0.0); break;
		case keSiliconValley: box->SetBounds( -122.44, 36.97, 0.0, -122.04, 37.77, 0.0); break;
	}
	//VARIANT_BOOL vb = this->SetGeographicExtents(box);
	return box;
}

// ****************************************************************
//		GetGeoPosition()
// ****************************************************************
bool CMapView::GetGeoPosition(double& x, double& y)
{
	double centerX = 0.0;
	double centerY = 0.0;

	PixelToProjection( _viewWidth / 2.0,  _viewHeight / 2.0, centerX, centerY);
	
	switch(_transformationMode)
	{
		case tmWgs84Complied:
			break;
		case tmDoTransformation:
			{
				VARIANT_BOOL vb;
				IGeoProjection* p = GetMapToWgs84Transform();
				if (p) {
					p->Transform(&centerX, &centerY, &vb);
					if (!vb) {
						return false;
					}
				}
			}
			break;
		case tmNotDefined:
			return false;
	}
	x = centerX;
	y = centerY;
	return true;
}

// ****************************************************************
//		GetGeoPosition()
// ****************************************************************
bool CMapView::SetGeoPosition(double x, double y)
{
	double dx = _extents.right - _extents.left;
	double dy = _extents.top - _extents.bottom;

	switch(_transformationMode)
	{
		case tmNotDefined:
			return false;
		case tmWgs84Complied:
			break;
		case tmDoTransformation:
			IGeoProjection* p = GetWgs84ToMapTransform();
			if (p) {
				VARIANT_BOOL vb;
				p->Transform(&x, &y, &vb);
				if (!vb)
					return false;
			}
			else
				return false;
	}
	Extent ext(x - dx/ 2.0, x + dx / 2.0, y - dy / 2.0, y + dy / 2.0);
	SetExtentsCore(ext, true, false);
	return true;
}

// ****************************************************************
//		SetLatitude()
// ****************************************************************
void CMapView::SetLatitude(float latitude)
{
	if (abs(latitude) > 90.0) {
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return;
	}

	SetGeoPosition(GetLongitude(), latitude);
}

float CMapView::GetLatitude()
{
	double x, y;
	if (GetGeoPosition(x, y))
		return (float)y;
	return 0.0;
}

// ****************************************************************
//		SetLongitude()
// ****************************************************************
void CMapView::SetLongitude(float longitude)
{
	if (abs(longitude) > 180.0) {
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return;
	}
	SetGeoPosition(longitude, GetLatitude());
}

float CMapView::GetLongitude()
{
	double x, y;
	if (GetGeoPosition(x, y))
		return (float)x;
	return 0.0;
}

// ****************************************************************
//		SetLatitudeLongitude()
// ****************************************************************
void CMapView::SetLatitudeLongitude(double latitude, double longitude)
{
	if (abs(latitude) > 90.0 || abs(longitude) > 180.0)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return;
	}

	SetGeoPosition(longitude, latitude);
}


// ****************************************************************
//		CurrentZoom()
// ****************************************************************
void CMapView::SetCurrentZoomCore(int zoom, bool forceUpdate) 
{
	if (_transformationMode != tmNotDefined)
	{
		if (zoom < 0 || zoom > 25) {
			ErrorMessage(tkINVALID_PARAMETER_VALUE);
			return;
		}

		int oldZoom;
		_tiles->get_CurrentZoom(&oldZoom);

		if (oldZoom != zoom || forceUpdate) {
			ZoomToTileLevel(zoom);
			Redraw();
		}
	}
	else
	{
		ErrorMessage(tkMAP_PROJECTION_NOT_SET, cvAll);
	}
}

// ****************************************************************
//		CurrentZoom()
// ****************************************************************
void CMapView::SetCurrentZoom(int zoom)
{
	SetCurrentZoomCore(zoom, false);
}

int CMapView::GetCurrentZoom()
{
	if (_transformationMode != tmNotDefined) 
	{
		if (ForceDiscreteZoom() && _currentZoom != -1)
		{
			return _currentZoom;
		}
		else 
		{
			int val;
			_tiles->get_CurrentZoom(&val);
			return val;
		}
	}
	
	return -1;
}

// ****************************************************************
//		SetInitExtents()
// ****************************************************************
void CMapView::SetInitGeoExtents() 
{
	SetKnownExtentsCore(keUSA);
}

// ****************************************************************
//		Projection()
// ****************************************************************
void CMapView::SetProjection(tkMapProjection projection)
{
	if (projection == PROJECTION_CUSTOM)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return;
	}

	tkTransformationMode prevMode = _transformationMode;

	IGeoProjection* p = NULL;
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&p);
	if (p)
	{
		VARIANT_BOOL vb;
		tkMapProjection oldProjection = GetProjection();
		if(projection != oldProjection) {
			bool preserveExtents = _activeLayers.size() == 0;
			IExtents* ext = GetGeographicExtents();	// try to preserve extents

			switch(projection) {
				case PROJECTION_WGS84:
					p->SetWgs84(&vb);
					SetGeoProjection(p);
					if (prevMode == tmNotDefined)
						SetInitGeoExtents();
					break;
				case PROJECTION_GOOGLE_MERCATOR:
					p->SetGoogleMercator(&vb);
					SetGeoProjection(p);
					if (prevMode == tmNotDefined)
						SetInitGeoExtents();
					break;
				case PROJECTION_NONE:
					// simply set an empty one
					SetGeoProjection(p);
					break;
			}
			if (ext) {
				if (preserveExtents)
					SetGeographicExtents(ext);
				ext->Release();
			}
			
			Redraw();
		}
		p->Release();
	}
}
tkMapProjection CMapView::GetProjection()
{
	VARIANT_BOOL vb;
	_projection->get_IsEmpty(&vb);
	if (vb) {
		return PROJECTION_NONE;
	}

	GetWgs84Projection()->get_IsSame(GetMapProjection(), &vb);
	if (vb) {
		return PROJECTION_WGS84;
	}

	GetGMercProjection()->get_IsSame(GetMapProjection(), &vb);
	if (vb) {
		return PROJECTION_GOOGLE_MERCATOR;
	}
	return PROJECTION_CUSTOM;
}

// ****************************************************************
//		KnownExtents()
// ****************************************************************
tkKnownExtents CMapView::GetKnownExtentsCore()
{
	if (_transformationMode != tmNotDefined) {
		return _knownExtents;
	}
	else {
		return keNone;
	}
}
void CMapView::SetKnownExtentsCore(tkKnownExtents extents)
{
	if (_transformationMode != tmNotDefined) 
	{
		IExtents* ext = GetKnownExtents(extents);
		if (ext) {
			SetGeographicExtents(ext);
			_knownExtents = extents;
			ext->Release();
		}
	}
	else {
		ErrorMessage(tkMAP_PROJECTION_NOT_SET, cvAll);
	}
}

// ****************************************************************
//		CoordinatesDisplay()
// ****************************************************************
tkCoordinatesDisplay CMapView::GetShowCoordinates()
{
	return _showCoordinates;
}
void CMapView::SetShowCoordinates(tkCoordinatesDisplay value)
{
	_showCoordinates = value;
}

// ****************************************************************
//		GrabProjectionFromData()
// ****************************************************************
VARIANT_BOOL CMapView::GetGrabProjectionFromData()
{
	return _grabProjectionFromData ? VARIANT_TRUE : VARIANT_FALSE;
}
void CMapView::SetGrabProjectionFromData(VARIANT_BOOL value)
{
	_grabProjectionFromData = value;
}

// ****************************************************************
//		ProjToDegrees()
// ****************************************************************
VARIANT_BOOL CMapView::ProjToDegrees(double projX, double projY, double* degreesLngX, double * degreesLatY)
{
	*degreesLngX = 0.0;
	*degreesLatY = 0.0;
	switch(_transformationMode)
	{
		case tmNotDefined:
			return VARIANT_FALSE;
		case tmWgs84Complied:
			*degreesLngX = projX;
			*degreesLatY = projY;
			return VARIANT_TRUE;
		case tmDoTransformation:
			IGeoProjection* gp = GetMapToWgs84Transform();
			VARIANT_BOOL vb;
			gp->Transform(&projX, &projY, &vb);
			*degreesLngX = projX;
			*degreesLatY = projY;
			return vb;
	}
	return VARIANT_FALSE;
}
VARIANT_BOOL CMapView::DegreesToProj(double degreesLngX, double degreesLatY, double* projX, double* projY)
{
	*projX = 0.0;
	*projY = 0.0;
	switch(_transformationMode)
	{
		case tmNotDefined:
			return VARIANT_FALSE;
		case tmWgs84Complied:
			*projX = degreesLngX;
			*projY = degreesLatY;
			return VARIANT_TRUE;
		case tmDoTransformation:
			IGeoProjection* gp = GetWgs84ToMapTransform();
			VARIANT_BOOL vb;
			gp->Transform(&degreesLngX, &degreesLatY, &vb);
			*projX = degreesLngX;
			*projY = degreesLatY;
			return vb;
	}
	return VARIANT_FALSE;
}

// ****************************************************************
//		ProjToDegrees()
// ****************************************************************
VARIANT_BOOL CMapView::PixelToDegrees(double pixelX, double pixelY, double* degreesLngX, double * degreesLatY)
{
	*degreesLngX = 0.0;
	*degreesLatY = 0.0;
	double projX, projY;
	PixelToProjection(pixelX,pixelY,projX,projY);
	return ProjToDegrees(projX, projY, degreesLngX, degreesLatY);
}
VARIANT_BOOL CMapView::DegreesToPixel(double degreesLngX, double degreesLatY, double* pixelX, double* pixelY)
{
	*pixelX = 0.0;
	*pixelY = 0.0;
	double projX, projY;
	if (DegreesToProj(degreesLngX, degreesLatY, &projX, &projY ))
	{
		ProjectionToPixel(projX,projY,*pixelX,*pixelY);
		return VARIANT_TRUE;
	}
	return VARIANT_FALSE;
}