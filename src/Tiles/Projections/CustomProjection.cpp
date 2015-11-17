/**************************************************************************************
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
  
#include "stdafx.h"
#include "CustomProjection.h"

// *******************************************************
//		FromLatLngToXY
// *******************************************************
// Converts from decimal degrees to tile coordinates
void CustomProjection::FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret)
{
	double lat = pnt.Lat;
	double lng = pnt.Lng;
	
	VARIANT_BOOL vb;
	_projWGS84->Transform(&lng, &lat, &vb);
	
	FromProjToXY(lat, lng, zoom, ret);
}

// *******************************************************
//		FromProjToXY
// *******************************************************
// Converts from projected units to tile coordinates
void CustomProjection::FromProjToXY(double lat, double lng, int zoom, CPoint &ret)
{
	lat = Clip(lat, _yMin, _yMax);
	lng = Clip(lng, _xMin, _xMax);

	double y = (lat - _yMin)/(_yMax - _yMin);
	double x = (lng - _xMin)/(_xMax - _xMin);
	
	CSize s;
	GetTileMatrixSizeXY(zoom, s);
	int mapSizeX = s.cx;
	int mapSizeY = s.cy;

	ret.x = (int) Clip(x * mapSizeX, 0, mapSizeX);
	ret.y = (int) Clip(y * mapSizeY, 0, mapSizeY);

	Clip(ret, zoom);
}

// *******************************************************
//		FromXYToLatLng
// *******************************************************
// Converts tile coordinates to decimal degrees
void CustomProjection::FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret)
{
	FromXYToProj(pnt, zoom, ret);

	VARIANT_BOOL vb;
	_projCustom->Transform(&ret.Lng, &ret.Lat, &vb);

	ret.Lat = ret.Lat;
	ret.Lng = ret.Lng;
}

// *******************************************************
//		FromXYToProj
// *******************************************************
// Converts from tile coordinates to projected coordinates
void CustomProjection::FromXYToProj(CPoint pnt, int zoom, PointLatLng &ret)
{
	CSize s;
	GetTileMatrixSizeXY(zoom, s);
	double mapSizeX = s.cx;
	double mapSizeY = s.cy;

	double x = Clip(pnt.x, 0, mapSizeX) / mapSizeX;
	double y = Clip(pnt.y, 0, mapSizeY) / mapSizeY;

	x = _xMin + x * (_xMax - _xMin);
	y = _yMin + y * (_yMax - _yMin);

	ret.Lat = y;
	ret.Lng = x;
}

// *******************************************************
//		GetTileSizeProj
// *******************************************************
void CustomProjection::GetTileSizeProj(int zoom, SizeLatLng &size)
{
	CSize sizeInt;
	GetTileMatrixSizeXY(zoom, sizeInt);
	size.WidthLng = (_xMax - _xMin) / (double)sizeInt.cx;
	size.HeightLat = (_yMax - _yMin) / (double)sizeInt.cy;
}

// ******************************************************
//    put_Bounds()
// ******************************************************
void CustomProjection::put_Bounds(double xMin, double xMax, double yMin, double yMax)
{
	_xMin = xMin;
	_xMax = xMax;
	_yMin = yMin;
	_yMax = yMax;
	_boundsChanged = true;

	// let's assign bounds in server projection to bounds in map projection,
	// when WMS layer is the first one added to the map, both projection 
	// and bounds will be grabbed from it; in other cases bounds will be 
	// recalculate on first data loading
	_mapBounds.left = xMin;
	_mapBounds.right = xMax;
	_mapBounds.bottom = yMin;
	_mapBounds.top = yMax;
	_mapProjectionCount = -1;	  // bounds should be update on first redraw
}

// ******************************************************
//    get_Bounds()
// ******************************************************
void CustomProjection::get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax)
{
	xMin = _xMin;
	xMax = _xMax;
	yMin = _yMin;
	yMax = _yMax;
}

// ******************************************************
//    UpdateBounds()
// ******************************************************
bool CustomProjection::UpdateBounds()
{
	if (!_boundsChanged) return true;

	VARIANT_BOOL vb, vb2;

	_projCustom->get_IsEmpty(&vb);
	if (vb) {
		CallbackHelper::ErrorMsg("Projection for WMS provider isn't set.");
		return false;
	}
	
	// existing transformation will be stopped within the call
	_projWGS84->StartTransform(_projCustom, &vb);
	if (!vb) {
		CallbackHelper::ErrorMsg("Failed to initialize coordinate transformation for WMS provider.");
		return false;
	}

	_projCustom->StartTransform(_projWGS84, &vb);
	if (!vb) {
		CallbackHelper::ErrorMsg("Failed to initialize coordinate transformation for WMS provider.");
	}

	_minLat = _yMin;
	_maxLat = _yMax;
	_minLng = _xMin;
	_maxLng = _xMax;

	_projCustom->Transform(&_minLng, &_minLat, &vb);
	_projCustom->Transform(&_maxLng, &_maxLat, &vb2);

	_boundsChanged = false;

	return vb && vb2;
}

// ******************************************************
//    put_Epsg()
// ******************************************************
void CustomProjection::put_Epsg(long epsg)
{
	VARIANT_BOOL vb;
	_projCustom->ImportFromEPSG(epsg, &vb);

	if (vb) 
	{ 
		_epsg = epsg; 
		_boundsChanged = true;
	}
}