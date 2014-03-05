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

// Converts from decimal degrees to tile coordinates
void CustomProjection::FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret)
{
	double lat = pnt.Lat;
	double lng = pnt.Lng;
	
	VARIANT_BOOL vb;
	projWGS84->Transform(&lng, &lat, &vb);
	
	this->FromProjToXY(lat, lng, zoom, ret);
}

// Converts from projected units to tile coordinates
void CustomProjection::FromProjToXY(double lat, double lng, int zoom, CPoint &ret)
{
	lat = Clip(lat, MinLatitude, MaxLatitude);
	lng = Clip(lng, MinLongitude, MaxLongitude);

	double y = (lat - MinLatitude)/(MaxLatitude - MinLatitude);
	double x = (lng - MinLongitude)/(MaxLongitude - MinLongitude);
	
	CSize s;
	GetTileMatrixSizeXY(zoom, s);
	int mapSizeX = s.cx;
	int mapSizeY = s.cy;

	ret.x = (int) Clip(x * mapSizeX, 0, mapSizeX);
	ret.y = (int) Clip(y * mapSizeY, 0, mapSizeY);

	Clip(ret, zoom);
}

// Converts tile coordinates to decimal degrees
void CustomProjection::FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret)
{
	FromXYToProj(pnt, zoom, ret);

	VARIANT_BOOL vb;
	projCustom->Transform(&ret.Lng, &ret.Lat, &vb);

	ret.Lat = ret.Lat;
	ret.Lng = ret.Lng;
}

// Converts from tile coordinates to projected coordinates
void CustomProjection::FromXYToProj(CPoint pnt, int zoom, PointLatLng &ret)
{
	CSize s;
	GetTileMatrixSizeXY(zoom, s);
	double mapSizeX = s.cx;
	double mapSizeY = s.cy;

	double x = Clip(pnt.x, 0, mapSizeX) / mapSizeX;
	double y = Clip(pnt.y, 0, mapSizeY) / mapSizeY;

	x = MinLongitude + x * (MaxLongitude - MinLongitude);
	y = MinLatitude + y * (MaxLatitude - MinLatitude);

	ret.Lat = y;
	ret.Lng = x;
}
