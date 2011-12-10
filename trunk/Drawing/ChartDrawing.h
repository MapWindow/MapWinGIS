/**************************************************************************************
 * File name: ChartDrawing.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CChartDrawer
 *
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
 // Sergei Leschinski (lsu) 25 june 2010 - created the file.

#pragma once
#include "MapWinGis.h"
#include <gdiplus.h>
#include "Extent.h"
#include "CollisionList.h"

class CChartDrawer
{
public:

	// constructor
	CChartDrawer::CChartDrawer(CDC* dc, Extent* extents, double dx, double dy, double currentScale, CCollisionList* collisionList)
	{
		_dc = dc;
		_extents = extents;
		_pixelPerProjectionX = dx;
		_pixelPerProjectionY = dy;
		_collisionList = collisionList;
		_currentScale = currentScale;
	}
	
	// destructor
	CChartDrawer::~CChartDrawer(void)
	{
	}

// member variables
protected:	
	double _pixelPerProjectionX;//_pixelPerProjectionX;
	double _pixelPerProjectionY;
	double _currentScale;
	CCollisionList* _collisionList;
	CDC* _dc;
	Extent* _extents; 

private:
	bool WithinVisibleExtents(double xMin, double xMax, double yMin, double yMax)
	{
		return 	
			!(xMin > _extents->right || xMax < _extents->left || yMin > _extents->top || yMax < _extents->bottom);
	};

	void PixelToProjection( double piX, double piY, double& prX, double& prY )
	{
		prX = _extents->left + piX* 1.0/_pixelPerProjectionX;
		prY = _extents->top - piY*1.0/_pixelPerProjectionY;
	}

	void ProjectionToPixel( double prX, double prY, double& piX, double& piY )
	{
		piX = (prX - _extents->left)* _pixelPerProjectionX;
		piY = (_extents->top - prY) * _pixelPerProjectionY;
	}

	//CString CChartDrawer::Format(double val, CString& sFormat);

public:
	void CChartDrawer::DrawCharts(IShapefile* sf);
};


