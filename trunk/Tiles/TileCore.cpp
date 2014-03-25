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
 // lsu 17 apr 2012 - created the file

#include "stdafx.h"
#include "TileCore.h"

// gets relative path of tile disk storage
CStringW TileCore::getPath(CStringW root, CStringW ext)
{
	CStringW path;
	path.Format(L"%s%d\\%d\\%d%s", root, this->m_scale, this->m_tileX, this->m_tileY, ext);
	return path;
}

// in case map projection and tiles projection are the same
bool TileCore::UpdateProjection(CustomProjection* proj)
{
	PointLatLng pnt;
	proj->FromXYToProj(CPoint(this->m_tileX, this->m_tileY + 1), this->m_scale, pnt);
	CSize size;
	proj->GetTileSizeProj(this->m_scale, size);
	Proj.xLng = pnt.Lng;
	Proj.yLat = pnt.Lat;
	Proj.WidthLng = size.cx;
	Proj.HeightLat = size.cy;
	m_projectionOk = true;
	return true;
}

// in case maps and tiles projection aren't the same
bool TileCore::UpdateProjection(OGRCoordinateTransformation* transformation)
{
	if (transformation)
	{
		double xMin = Geog.xLng; 
		double yMax = Geog.yLat;
		double xMax = Geog.MaxLng(); 
		double yMin = Geog.MinLat();
		
		BaseProjection* pr = this->m_projection;
		if (pr)
		{
			xMin = MAX(xMin, pr->MinLongitude);
			xMax = MIN(xMax, pr->MaxLongitude);
			yMin = MAX(yMin, pr->MinLatitude);
			yMax = MIN(yMax, pr->MaxLatitude);
		}

		double xTL, xTR, xBL, xBR;
		double yTL, yTR, yBL, yBR;

		xTL = xBL = xMin;
		xTR = xBR = xMax;
		yTL = yTR = yMax;
		yBL = yBR = yMin;

		if (!transformation->Transform(1, &xTL, &yTL))
			return false;

		if (!transformation->Transform(1, &xTR, &yTR))
			return false;

		if (!transformation->Transform(1, &xBL, &yBL))
			return false;

		if (!transformation->Transform(1, &xBR, &yBR))
			return false;

		Proj.xLng = (xBL + xTL)/2.0;
		Proj.yLat = (yTL + yTR)/2.0;
		Proj.WidthLng = (xTR + xBR)/2.0 - Proj.xLng;
		Proj.HeightLat = Proj.yLat - (yBR + yBL)/2.0;
		
		m_projectionOk = true;
		return true;
	}
	return false;
}