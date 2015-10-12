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

// *******************************************************
//		GetTileMatrixMinXY
// *******************************************************
void BaseProjection::GetTileMatrixMinXY(int zoom, CSize &size)
{
	size.cx = 0;
	size.cy = 0;
}

// *******************************************************
//		GetTileMatrixMaxXY
// *******************************************************
void BaseProjection::GetTileMatrixMaxXY(int zoom, CSize &size)
{
	int xy = (1 << zoom);
	size.cx = xy - 1;
	size.cy = xy - 1;
}

// *******************************************************
//		Clip
// *******************************************************
void BaseProjection::Clip(CPoint& tilePnt, int zoom)
{
	CSize size;
	GetTileMatrixMaxXY(zoom, size);
	tilePnt.x = MIN(MAX(tilePnt.x, 0), size.cx);
	tilePnt.y = MIN(MAX(tilePnt.y, 0), size.cy);
}

// *******************************************************
//		GetTileSizeLatLon
// *******************************************************
// gets matrix size in decimal degrees
void BaseProjection::GetTileSizeLatLon(PointLatLng point, int zoom, SizeLatLng &ret)
{
	CPoint temp;
	this->FromLatLngToXY(point, zoom, temp);
	this->GetTileSizeLatLon(temp, zoom, ret);
}

// *******************************************************
//		GetTileSizeLatLon
// *******************************************************
// gets matrix size in decimal degrees
void BaseProjection::GetTileSizeLatLon(CPoint point, int zoom, SizeLatLng &ret)
{
	PointLatLng pnt1;
	this->FromXYToLatLng(point, zoom, pnt1);

	// moving to the neighboring tile
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

// *******************************************************
//		GetTileMatrixSizeXY
// *******************************************************
// gets matrix size in tile coordinates
void BaseProjection::GetTileMatrixSizeXY(int zoom, CSize &ret)
{
	CSize sMin, sMax;
	GetTileMatrixMinXY(zoom, sMin);
	GetTileMatrixMaxXY(zoom, sMax);
	ret.cx = sMax.cx - sMin.cx + 1;
	ret.cy = sMax.cy - sMin.cy + 1;
}

// *******************************************************
//		CalculateGeogBounds
// *******************************************************
RectLatLng BaseProjection::CalculateGeogBounds(CPoint pnt, int zoom)
{
	// calculating geographic coordinates
	SizeLatLng size;
	this->GetTileSizeLatLon(pnt, zoom, size);

	if (size.WidthLng == 0.0 || size.HeightLat == 0.0) {
		CallbackHelper::AssertionFailed("Invalid tile size on calculating geographic bounds.");
	}

	if (this->_yInverse)
	{
		PointLatLng geoPnt;
		CPoint pnt2 = pnt;
		pnt2.y++;			// y corresponds to the bottom of tile as the axis is directed up
		// while the drawing position is defined by its top-left corner
		// so the calculation is made by the upper tile

		this->FromXYToLatLng(pnt2, zoom, geoPnt);
		return RectLatLng(geoPnt.Lng, geoPnt.Lat, size.WidthLng, size.HeightLat);
	}
	else
	{
		PointLatLng geoPnt;
		this->FromXYToLatLng(pnt, zoom, geoPnt);
		return RectLatLng(geoPnt.Lng, geoPnt.Lat, size.WidthLng, size.HeightLat);
	}
}

// *******************************************************
//		getTileRectXY
// *******************************************************
void BaseProjection::getTileRectXY(Extent extentsWgs84, int zoom, CRect &rect)
{
	CPoint p1, p2;

	FromLatLngToXY(PointLatLng(extentsWgs84.top, extentsWgs84.left), zoom, p1);
	FromLatLngToXY(PointLatLng(extentsWgs84.bottom, extentsWgs84.right), zoom, p2);

	rect.left = p1.x;
	rect.right = p2.x;
	rect.top = p1.y;
	rect.bottom = p2.y;
}