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
class MercatorProjection: public BaseProjection
{
public:
	MercatorProjection()
	{
		 _minLat = -85.05112878;
		 _maxLat = 85.05112878;
		 _minLng = -180;
		 _maxLng = 180;
	};
	
	virtual ~MercatorProjection() 
	{
	}

private:
	static IGeoProjection* _geoProjeciton;
	static ::CCriticalSection _lock;

private:
	double GetWidth() { return MERCATOR_MAX_VAL * 2.0; }

public:
	static void ReleaseGeoProjection();

public:
	virtual bool IsSphericalMercator() { return true; }
	
	IGeoProjection* get_ServerProjection();

	// converts coordinates to meters in EPSG:3857
	void FromXYToProj(CPoint pos, int zoom, PointLatLng &ret);
	
	// converts decimal degrees to tile coordinates
	void FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret);

	// converts tile coordinates to decimal degrees
	void FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret);
	
	// override BaseProjection's member to check result
	virtual RectLatLng CalculateGeogBounds(CPoint pnt, int zoom);
};
