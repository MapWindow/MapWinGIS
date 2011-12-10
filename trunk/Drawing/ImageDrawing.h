/**************************************************************************************
 * File name: ImageDrawing.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CImageDrawer
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
#include "ImageGroup.h"

class CImageDrawer
{
public:
	// constructor
	CImageDrawer::CImageDrawer(CDC* dc, Extent* extents, double dx, double dy, long viewWidth, long viewHeight)
	{
		_extents = extents;
		_pixelPerProjectionX = dx;
		_pixelPerProjectionY = dy;
		_viewHeight = viewHeight;
		_viewWidth = viewWidth;
		_dc = dc;
		//_graphics = new Gdiplus::Graphics(dc->m_hDC);
	}
	
	// destructor
	CImageDrawer::~CImageDrawer(void)
	{
	}
	
	// member variables
protected:	
	Extent _visibleExtents;
	CRect _visibleRect;

	Extent* _extents; 
	double _pixelPerProjectionX;
	double _pixelPerProjectionY;
	long _viewWidth;
	long _viewHeight;
	CDC* _dc;
	//Gdiplus::Graphics* _graphics;

public:
	//void CImageDrawer::DrawImage(const CRect & rcBounds, IImage* iimg);
	ScreenBitmap* CImageDrawer::DrawImage(const CRect & rcBounds, IImage* iimg, bool returnBitmap = false);	
private:
	bool CImageDrawer::CalculateImageBlockSize(int& dstL, int& dstT, int& dstR, int& dstB, 
								   int& imgX, int& imgY, int& imgW, int& imgH, 
						           double& mapL, double& mapT, double& mapR, double& mapB,
								   const double dx, const double dy, const CRect& rcBounds);

	bool CImageDrawer::CalculateImageBlockSize1(Extent extents, double MinX, double MinY, double MaxX, double MaxY,
										   const double dX, const double dY, const int height, const int width);

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

	

};

