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

// A base class for projected coordinate systems, i.e. specified in meters rather than decimal degrees
// Additional transformation with proj4.dll is carries out for them
class CustomProjection: public BaseProjection
{
protected:	
	IGeoProjection* _projWGS84;
	IGeoProjection* _projCustom;
	long _epsg = -1;
	bool _boundsChanged = true;
	
public:
	CustomProjection()
	{
		_projected = true;

		ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_projWGS84);
		ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&_projCustom);

		VARIANT_BOOL vb;
		_projWGS84->ImportFromEPSG(4326, &vb);
	};

	virtual ~CustomProjection()
	{
		_projWGS84->StopTransform();
		_projCustom->StopTransform();
		_projWGS84->Release();
		_projCustom->Release();
	}

public:
	IGeoProjection* get_ServerProjection() { return _projCustom; }
	void FromLatLngToXY(PointLatLng pnt, int zoom, CPoint &ret);
	void FromProjToXY(double lat, double lng, int zoom, CPoint &ret);
	void FromXYToLatLng(CPoint pnt, int zoom, PointLatLng &ret);
	void FromXYToProj(CPoint pnt, int zoom, PointLatLng &ret);
	double GetWidth() {	return _xMax - _xMin;	}
	void GetTileSizeProj(int zoom, CSize &size);
	void GetTileSizeProj(int zoom, SizeLatLng &size);
	long get_Epsg(){ return _epsg; }
	void put_Epsg(long epsg);
	
	void put_Bounds(double xMin, double xMax, double yMin, double yMax);
	void get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax);

	// make sure that UpdateBounds is called before the first use of the projection
	// starts coordinate transformation and update bounds of projection in WGS84 degrees
	bool UpdateBounds();
};