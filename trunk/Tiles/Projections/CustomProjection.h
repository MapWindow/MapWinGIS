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

// A base class for projected coordinte systems, i.e. specified in meters rather than decimal degrees
// Additional transformation with proj4.dll is carries out for them
class CustomProjection: public BaseProjection
{
protected:	
	IGeoProjection* projWGS84;
public:
	IGeoProjection* projCustom;
	
	CustomProjection()
	{
		projected = true;
		CoCreateInstance(CLSID_GeoProjection,NULL,CLSCTX_INPROC_SERVER,IID_IGeoProjection,(void**)&projWGS84);
		CoCreateInstance(CLSID_GeoProjection,NULL,CLSCTX_INPROC_SERVER,IID_IGeoProjection,(void**)&projCustom);
	};

	virtual ~CustomProjection()
	{
		projWGS84->StopTransform();
		projCustom->StopTransform();
		projWGS84->Release();
		projCustom->Release();
	}

	void FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret);
	void FromProjToXY(double lat, double lng, int zoom, CPoint &ret);
	void FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret);
	void FromXYToProj(CPoint pnt, int zoom, PointLatLng &ret);

	void GetTileSizeProj(int zoom, CSize &size)
	{
		this->GetTileMatrixSizeXY(zoom, size);
		size.cx = (long)((xMaxLng - xMinLng)/(double)size.cx);
		size.cy = (long)((yMaxLat - yMinLat)/(double)size.cy);
	}
};