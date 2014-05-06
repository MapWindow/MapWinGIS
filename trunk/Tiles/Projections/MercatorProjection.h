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
#include "BaseProjection.h"

// commonly used Google Mercator projection; EPSG 3857
class MercatorProjection: public MercatorBase
{
private:
	double degToRad (double ang) {
		return ang * pi / 180.0;
	}
public:
	MercatorProjection()
	{
		 yMinLat = -85.05112878;
		 yMaxLat = 85.05112878;
		 xMinLng = -180;
		 xMaxLng = 180;
	};
	
	~MercatorProjection() { }

	// converts coordinates to meters in EPSG:3857
	PointLatLng TileXYToProj(CPoint &pos, int zoom, PointLatLng &ret){
		this->FromXYToLatLng(pos, zoom, ret);
		double x = this->earthRadius * degToRad (ret.Lng);
		double y = this->earthRadius * log(tan(pi/4.0 + degToRad(ret.Lat)/2 ));
		ret.Lng = x;
		ret.Lat = y;
		return ret;
	}
	
	// converts decimal degrees to tile coordinates
	void FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret)
	{
		double lat = Clip(pnt.Lat, yMinLat, yMaxLat);
		double lng = Clip(pnt.Lng, xMinLng, xMaxLng);

		double x = (lng + 180) / 360;
		double sinLatitude = sin(lat * PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * PI);

		CSize s;
		GetTileMatrixSizeXY(zoom, s);
		int mapSizeX = s.cx;
		int mapSizeY = s.cy;
		
		ret.x = (int) (x * mapSizeX);
		ret.y = (int) (y * mapSizeY);

		Clip(ret, zoom);
	}

	// converts tile coordinates to decimal degrees
	void FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret)
	{
		int x = pnt.x;
		int y = pnt.y;
		
		CSize s;
		GetTileMatrixSizeXY(zoom, s);
		double mapSizeX = s.cx;
		double mapSizeY = s.cy;

		double xx = (Clip(x, 0, mapSizeX - 1) / mapSizeX) - 0.5;	
		double yy = 0.5-(Clip(y, 0, mapSizeY - 1) / mapSizeY);

		ret.Lat = 90 - 360 * atan(exp(-yy * 2 * PI)) / PI;
		ret.Lng = 360 * xx;
	}
};
