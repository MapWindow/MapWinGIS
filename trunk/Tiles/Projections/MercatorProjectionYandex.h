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

// Mercator projection used by Yandex
class MercatorProjectionYandex: public BaseProjection
{
	double RAD_DEG;
    double DEG_RAD;
    double piDiv4;
public:
	MercatorProjectionYandex()
	{
		yMinLat = -85.05112878;
		yMaxLat = 85.05112878;
		xMinLng = -180;
		xMaxLng = 180;
		RAD_DEG = 180 / PI;
		DEG_RAD = PI / 180;
		piDiv4 = PI / 4;
	};
	
	virtual bool CoordinatesInDegrees()	{ return true; }

	void FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret)
	{
		FromLatLngToXY(pnt.Lat, pnt.Lng, zoom, ret);
	}
	void FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret) 
	{
		FromXYToLatLng(pnt.x, pnt.y, zoom, ret);
	}

	void FromLatLngToXY(double lat, double lng, int zoom, CPoint &ret)
	{
		lat = Clip(lat, yMinLat, yMaxLat);
        lng = Clip(lng, xMinLng, xMaxLng);

        double rLon = lng * DEG_RAD; 
        double rLat = lat * DEG_RAD;

        double a = 6378137;
        double k = 0.0818191908426;

		double z = tan(piDiv4 + rLat / 2) / pow((tan(piDiv4 + asin(k * sin(rLat)) / 2)), k);
		double z1 = (double)pow(2.0, (int)(23 - zoom));

        double dx =  ((20037508.342789 + a * rLon) * 53.5865938 /  z1);
        double dy = ((20037508.342789 - a * log(z)) * 53.5865938 / z1);

        ret.x = (int) (dx/256.0);
        ret.y = (int) (dy/256.0);

		this->Clip(ret, zoom);
	}

	void FromXYToLatLng(int x, int y, int zoom, PointLatLng &ret)
	{
		x *= 256;
		y *= 256;

        double a = 6378137;
        double c1 = 0.00335655146887969;
        double c2 = 0.00000657187271079536;
        double c3 = 0.00000001764564338702;
        double c4 = 0.00000000005328478445;
        double z1 = (23 - zoom);
		double mercX = (x * pow(2, z1)) / 53.5865938 - 20037508.342789;
		double mercY = 20037508.342789 - (y * pow(2, z1)) / 53.5865938;

		double g = PI /2 - 2 * atan(1 / exp(mercY /a));
        double z = g + c1 * sin(2 * g) + c2 * sin(4 * g) + c3 * sin(6 * g) + c4 * sin(8 * g);

        ret.Lat = z * RAD_DEG;
        ret.Lng = mercX / a * RAD_DEG;
	}
};
