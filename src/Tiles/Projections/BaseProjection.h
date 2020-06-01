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

#pragma once
#include "geopoint.h"

// A base abstract class to handle transformation between map coordinates (decimal degrees)
// and tile coordinates (indices of tiles at particular zoom level)
class BaseProjection
{
public:
	BaseProjection()
		: _yMin(0.0), _yMax(0.0), _xMin(0.0), _xMax(0.0), _mapBounds(0.0, 0.0, 0.0, 0.0)
	{
		_tileSize = CSize(256, 256);
		_yInverse = false;
		_earthRadius = 6378137.0;
		_worldWide = true;
		_projected = false;
		_clipBoundsChanged = true;
		_mapProjectionCount = -1;
	};

	virtual ~BaseProjection() 
	{
	}

protected:
	bool _projected;	  // it's projected coordinate system; direct calculations of tile positions will be attempted
	bool _yInverse;
	CSize _tileSize;
	double _earthRadius;
	bool _worldWide;

	// clipping bounds in decimal degrees
	double _minLat;
	double _maxLat;
	double _minLng;
	double _maxLng;
	bool _clipBoundsChanged;

	// bounds in projected coordinates (server projection)
	double _yMin;
	double _yMax;
	double _xMin;
	double _xMax;

	Extent _mapBounds;        // bounds of the projection in map coordinate system
	int _mapProjectionCount;  // the index of the map projection under which the bounds were calculated	

public:
	virtual void FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret) = 0;
	virtual void FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret) = 0;
	virtual void FromXYToProj(CPoint pnt, int zoom, PointLatLng &ret) = 0;
	virtual double GetWidth() = 0;
	virtual IGeoProjection* get_ServerProjection() = 0;

	virtual bool IsSphericalMercator() { return false; }
	bool IsWorldWide() { return _worldWide; }
	double get_MinLat() { return _minLat; }
	double get_MaxLat() { return _maxLat; }
	double get_MinLong() { return _minLng; }
	double get_MaxLong() { return _maxLng; }
	bool get_ClipBoundsChanged() { return _clipBoundsChanged;}
	Extent get_MapBounds() {return _mapBounds; }
	int get_MapProjectionCount() { return _mapProjectionCount; }

	void SetMapBounds(Extent bounds, int projectionCount);

	void GetTileMatrixMinXY(int zoom, CSize &size);
	void GetTileMatrixMaxXY(int zoom, CSize &size);
	void GetTileSizeLatLon(PointLatLng point, int zoom, SizeLatLng &ret);
	void GetTileSizeLatLon(CPoint point, int zoom, SizeLatLng &ret);
	void GetTileMatrixSizeXY(int zoom, CSize &ret);
	void getTileRectXY(Extent extentsWgs84, int zoom, CRect &rect);

	virtual RectLatLng CalculateGeogBounds(CPoint pnt, int zoom);

	void Clip(CPoint& tilePnt, int zoom);

	Extent GetClipBounds();
	void SetClipBounds(Extent& extents);

	static double Clip(double n, double minValue, double maxValue) { return MIN(MAX(n, minValue), maxValue); }
};