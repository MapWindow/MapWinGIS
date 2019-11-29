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
#include "MercatorProjection.h"
#include "AngleHelper.h"

IGeoProjection* MercatorProjection::_geoProjeciton = NULL;
::CCriticalSection MercatorProjection::_lock;

// ****************************************************
//		get_ServerProjection
// ****************************************************
void MercatorProjection::ReleaseGeoProjection()
{
	_lock.Lock();

	if (_geoProjeciton) {
		_geoProjeciton->Release();
		_geoProjeciton = NULL;
	}

	_lock.Unlock();
}

// ****************************************************
//		get_ServerProjection
// ****************************************************
IGeoProjection* MercatorProjection::get_ServerProjection()
{
	CSingleLock lock(&_lock, TRUE);

	if (!_geoProjeciton) 
	{
		ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_geoProjeciton);

		VARIANT_BOOL vb;
		_geoProjeciton->SetGoogleMercator(&vb);
	}

	return _geoProjeciton;
}

// ****************************************************
//		TileXYToProj
// ****************************************************
void MercatorProjection::FromXYToProj(CPoint pos, int zoom, PointLatLng &ret)
{
	FromXYToLatLng(pos, zoom, ret);

	double x = _earthRadius * AngleHelper::ToRad(ret.Lng);
	double y = _earthRadius * log(tan(pi_ / 4.0 + AngleHelper::ToRad(ret.Lat) / 2));

	ret.Lng = x;
	ret.Lat = y;
}

// ****************************************************
//		FromLatLngToXY
// ****************************************************
// converts decimal degrees to tile coordinates
void MercatorProjection::FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret)
{
	double lat = Clip(pnt.Lat, _minLat, _maxLat);
	double lng = Clip(pnt.Lng, _minLng, _maxLng);

	double x = (lng + 180) / 360;
	double sinLatitude = sin(lat * pi_ / 180);
	double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * pi_);

	CSize s;
	GetTileMatrixSizeXY(zoom, s);
	int mapSizeX = s.cx;
	int mapSizeY = s.cy;

	ret.x = (int)(x * mapSizeX);
	ret.y = (int)(y * mapSizeY);

	Clip(ret, zoom);
}

// ****************************************************
//		FromXYToLatLng
// ****************************************************
// converts tile coordinates to decimal degrees
void MercatorProjection::FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret)
{
	int x = pnt.x;
	int y = pnt.y;

	CSize s;
	GetTileMatrixSizeXY(zoom, s);
	double mapSizeX = s.cx;
	double mapSizeY = s.cy;

	double xx = (Clip(x, 0, mapSizeX - 1) / mapSizeX) - 0.5;
	double yy = 0.5 - (Clip(y, 0, mapSizeY - 1) / mapSizeY);

	ret.Lat = 90 - 360 * atan(exp(-yy * 2 * pi_)) / pi_;
	ret.Lng = 360 * xx;
}

// ****************************************************
//		CalculateGeogBounds
// ****************************************************
// check result from BaseProjection against min/max lat/lon, because BaseProjection don't 
// have lat/lon limits, but Mercator do
RectLatLng MercatorProjection::CalculateGeogBounds(CPoint pnt, int zoom)
{
	auto res = BaseProjection::CalculateGeogBounds(pnt, zoom);

	//sanity check

	if (res.yLat > _maxLat)
		res.yLat = _maxLat;

	if (res.yLat < _minLat)
		res.yLat = _minLat;

	if (res.xLng > _maxLng)
		res.xLng = _maxLng;

	if (res.xLng < _minLng)
		res.xLng = _minLng;

	if ((res.xLng + res.WidthLng) > _maxLng)
		res.WidthLng = _maxLng - res.xLng;

	if ((res.yLat - res.HeightLat) < _minLat)
		res.HeightLat = res.yLat - _minLat;

	return res;
}

