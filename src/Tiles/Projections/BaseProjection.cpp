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
#include "BaseProjection.h"

// gets matrix size in decimal degrees
void BaseProjection::GetTileSizeLatLon(PointLatLng point, int zoom, SizeLatLng &ret)
{
	CPoint temp;
	this->FromLatLngToXY(point, zoom, temp);
	this->GetTileSizeLatLon(temp, zoom, ret);
}

// gets matrix size in decimal degrees
void BaseProjection::GetTileSizeLatLon(CPoint point, int zoom, SizeLatLng &ret)
{
	PointLatLng pnt1;
	this->FromXYToLatLng(point, zoom, pnt1);

	// moving to the neighbouring tile
	CPoint newPoint(point.x, point.y);

	CSize size;
	GetTileMatrixMaxXY(zoom, size);
	if (newPoint.x < size.cx)	newPoint.x++;
	else						newPoint.x--;

	if (newPoint.y < size.cy)	newPoint.y++;
	else						newPoint.y--;

	PointLatLng pnt2;
	this->FromXYToLatLng(newPoint, zoom, pnt2);
    
	// size
	ret.WidthLng = fabs( pnt2.Lng - pnt1.Lng);
	ret.HeightLat = fabs(pnt2.Lat - pnt1.Lat);
}

// gets matrix size in tile coordinates
void BaseProjection::GetTileMatrixSizeXY(int zoom, CSize &ret)
{
	CSize sMin, sMax;
	GetTileMatrixMinXY(zoom, sMin);
	GetTileMatrixMaxXY(zoom, sMax);
	ret.cx = sMax.cx - sMin.cx + 1;
	ret.cy = sMax.cy - sMin.cy + 1;
}