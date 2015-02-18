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
#include "BaseProvider.h"

class WMSGMercProvider: public BaseProvider
{
CString url2;
public:
	~WMSGMercProvider(void){};

	WMSGMercProvider() 
	{
		this->Projection = new MercatorProjection();
	}
	// gets bounding box in Google mercator projection (meters; EPSG:3857)
	CString GetBoundingBox(CPoint &pos, int zoom)
	{
		PointLatLng pnt1;
		((MercatorProjection*)this->Projection)->TileXYToProj(pos, zoom, pnt1);

		PointLatLng pnt2;
		pos.x++;
		pos.y++;
		((MercatorProjection*)this->Projection)->TileXYToProj(pos, zoom, pnt2);
		
		CString s;
		s.Format("%f,%f,%f,%f", pnt1.Lng, pnt2.Lat, pnt2.Lng, pnt1.Lat);
		return s;
	}
};

