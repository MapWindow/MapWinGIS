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
#include "BaseDrawer.h"
#include "ImageGroup.h"

class CImageDrawer: public CBaseDrawer
{
public:
	CImageDrawer::CImageDrawer(Gdiplus::Graphics* graphics, Extent* extents, double dx, double dy, long viewWidth, long viewHeight)
	{
		_extents = extents;
		_pixelPerProjectionX = dx;
		_pixelPerProjectionY = dy;
		_viewHeight = viewHeight;
		_viewWidth = viewWidth;
		_dc = NULL;
		_graphics = graphics;
	}
	
	CImageDrawer::~CImageDrawer() {}
	
protected:	
	Extent _visibleExtents;
	CRect _visibleRect;

	long _viewWidth;
	long _viewHeight;

private:
	bool CalculateImageBlockSize(int& dstL, int& dstT, int& dstR, int& dstB, 
								   int& imgX, int& imgY, int& imgW, int& imgH, 
						           double& mapL, double& mapT, double& mapR, double& mapB,
								   const double dx, const double dy, const CRect& rcBounds);

	bool CalculateImageBlockSize(Extent extents, double MinX, double MinY, double MaxX, double MaxY,
										   const double dX, const double dY, const int height, const int width);
	void DrawBmpNative(IImage* iimg, ImageSpecs& specs, Gdiplus::ImageAttributes* imgAttr, CRect rcBounds);
	ScreenBitmap* DrawGdalRaster(IImage* iimg, ImageSpecs& specs, Gdiplus::ImageAttributes* imgAttr, CRect rcBounds, int bitsPerPixel, bool returnBitmap);
	long GetPixelsInView();
	bool WithinVisibleExtents(double xMin, double xMax, double yMin, double yMax);
	bool ReadImageSpecs(IImage* iimg, ImageSpecs& specs);
	IImage* CreateSmallerProxyForGdalRaster(ImageSpecs& specs, IImage* img, CRect rcBounds, int bytesPerPixel);
	int GetRowBytePad(int width, int bitsPerPixel);
	ScreenBitmap* CreateScreenBitmap(double dstL, double dstT, double dstW, double dstH);
	unsigned char* ReadGdalBufferBlock(unsigned char* buffer, int row, int width, int bytesPerPixel, int pad, int numRead);
	void DrawGdalImage(Gdiplus::Graphics* g, Gdiplus::Image* img, Gdiplus::ImageAttributes* imgAttr, double dstL, double dstT, double dstW, double dstH, int imgX, int imgY, int imgH, int imgW, bool atOrigin);
	int GetBmpRowLength(int width, int bitsPerPixel);

public:
	ScreenBitmap* DrawImage(const CRect & rcBounds, IImage* iimg, bool returnBitmap = false);	
	
};

