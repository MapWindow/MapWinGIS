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
protected:
	double MinLatitude;
	double MaxLatitude;
	double MinLongitude;
	double MaxLongitude;
public:
    CSize tileSize;
	double yMinLat;		// in decimal degrees for Mercator projection or projected units for custom projections
	double yMaxLat;
	double xMinLng;
	double xMaxLng;
	bool yInverse;
	double PI;
	bool projected;	  // it's projected coordinate system; direct calculations of tile positions will be attempted
	double earthRadius;
	bool worldWide;
	tkTileProjection serverProjection;

	double GetMinLatitude() { 
		return projected ? MinLatitude : yMinLat; 
	}
	double GetMaxLatitude() { 
		return projected ? MaxLatitude : yMaxLat; }
	double GetMinLongitude() { 
		return projected ? MinLongitude : xMinLng; 
	}
	double GetMaxLongitude() { 
		return projected ? MaxLongitude : xMaxLng; 
	}

	virtual void FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret) = 0;
	virtual void FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret) = 0;

	BaseProjection()
	{
		 tileSize = CSize(256, 256);
		 PI = 3.1415926535897932384626433832795;
		 yInverse = false;
		 earthRadius = 6378137.0;
		 worldWide = true;
		 serverProjection = tkTileProjection::SphericalMercator;
		 yMinLat = yMaxLat = xMinLng = xMaxLng = 0.0;
		 projected = false;
	};
	
	virtual ~BaseProjection() {}

	void GetTileMatrixMinXY(int zoom, CSize &size)
	{
		size.cx = 0;
		size.cy = 0;
	}

	void GetTileMatrixMaxXY(int zoom, CSize &size)
	{
		int xy = (1 << zoom);
		size.cx = xy-1;
		size.cy = xy-1;
	}

	static double Clip(double n, double minValue, double maxValue)
	{
	   return MIN(MAX(n, minValue), maxValue);
    }

	void Clip(CPoint& tilePnt, int zoom)
	{
		CSize size; 
		GetTileMatrixMaxXY(zoom, size);
		tilePnt.x = MIN(MAX(tilePnt.x, 0), size.cx);
		tilePnt.y = MIN(MAX(tilePnt.y, 0), size.cy);
    }

	void GetTileSizeLatLon(PointLatLng point, int zoom, SizeLatLng &ret);
	void GetTileSizeLatLon(CPoint point, int zoom, SizeLatLng &ret);
	void GetTileMatrixSizeXY(int zoom, CSize &ret);
};
