/**************************************************************************************
 * File name: ImageDrawing.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CImageDrawer
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
 **************************************************************************************/
  
#include "stdafx.h"
#include <io.h>
#include "Image.h"
#include "ImageDrawing.h"
#include "macros.h"
#include "ImageHelper.h"

// ******************************************************************
//		DrawImage()
// ******************************************************************
ScreenBitmap* CImageDrawer::DrawImage(const CRect & rcBounds, IImage* img, bool returnBitmap)
{
	if( img == NULL )
		return NULL;

	if (ImageHelper::Cast(img)->GetBufferReloadIsNeeded())
	{				
		img->SetVisibleExtents(_extents->left, _extents->bottom, _extents->right, _extents->top, GetPixelsInView(), 0.0);
	}
	
	ImageSpecs specs;

	// width, height, dx, dy, xllCorner, yllCorner
	if (!ReadImageSpecs(img, specs)) 
	{
		return NULL;
	}
	
	if (!WithinVisibleExtents(specs.xllCorner, specs.GetXtrCorner(), specs.yllCorner, specs.GetYtrCorner())) 
	{
		return NULL;
	}
	
	_graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	
	Gdiplus::ImageAttributes* imgAttr = ImageHelper::GetImageAttributes(img);

	ScreenBitmap* screenBitmap = NULL;
	if(!ImageHelper::GetIsInRam(img) && ImageHelper::GetImageType(img) == BITMAP_FILE)
	{
		DrawBmpNative(img, specs, imgAttr, rcBounds);
	}
    else
	{	
		int bitsPerPixel = 24;		

		IImage* imgTemp = CreateSmallerProxyForGdalRaster(specs, img, rcBounds, bitsPerPixel / 8);

		DrawGdalRaster(imgTemp != NULL ? imgTemp : img, specs, imgAttr, rcBounds, bitsPerPixel, returnBitmap);

		if (imgTemp) {
			imgTemp->Release();
		}
	}

	delete imgAttr;
	
	return returnBitmap ? screenBitmap : NULL;	// returning bitmap to draw in case of grouped images
}

// ******************************************************************
//		DrawGdalRaster()
// ******************************************************************
ScreenBitmap* CImageDrawer::DrawGdalRaster(IImage* iimg, ImageSpecs& specs, Gdiplus::ImageAttributes* imgAttr, CRect rcBounds, int bitsPerPixel, bool returnBitmap)
{
	ScreenBitmap* screenBitmap = NULL;

	ImageType type = ImageHelper::GetImageType(iimg);
	
	double mapL, mapT, mapR, mapB;	// map units w/o clipping (world coordinates to place the image)
	double dstL, dstR, dstT, dstB;	// screen units with clipping (area needed to draw visible part)
	int imgX, imgY, imgW, imgH;	// image units with clipping (part of image to draw)
	
	//  accessing data from the buffer
	unsigned char* pData = ImageHelper::GetImageData(iimg);
	colour* pNewData = NULL;

	// try to load the buffer, as we could fail to load it because of unsuccessful image grouping
	if (!pData && (type != BITMAP_FILE && type != USE_FILE_EXTENSION))
	{
		iimg->SetVisibleExtents(_extents->left, _extents->bottom, _extents->right, _extents->top, GetPixelsInView(), 0.0);
		CallbackHelper::AssertionFailed("DrawGdalRaster: no buffer was populated.");
	}

	if (!pData) return NULL;

	int pad = GetRowBytePad(specs.width, bitsPerPixel);

	// ------------------------------------------------------------
	//    Drawing blocks of the bitmap
	// ------------------------------------------------------------
	int blockSize = 8192;
	long numRead;					// number of rows read
	for (int row = 0; row < specs.height;)
	{
		numRead = blockSize;
		if (row + blockSize >= specs.height)
			numRead = specs.height - row;

		// part of image to draw in map units
		mapL = specs.xllCorner;
		mapB = specs.yllCorner + row * specs.dy;
		mapT = mapB + (double)numRead * specs.dy;
		mapR = mapL + (double)specs.width * specs.dx;

		if (WithinVisibleExtents(mapL, mapR, mapB, mapT))
		{
			Extent extImage = Extent(mapL, mapR, mapB, mapT);
			CalculateImageBlockSize(extImage, _extents->left, _extents->bottom, _extents->right, _extents->top, specs.dx, specs.dy, numRead, specs.width);

			imgX = _visibleRect.left;
			imgY = _visibleRect.top;
			imgW = _visibleRect.right - _visibleRect.left + 1;
			imgH = _visibleRect.bottom - _visibleRect.top + 1;

			ProjectionToPixel(_visibleExtents.left, _visibleExtents.bottom, dstL, dstB);
			ProjectionToPixel(_visibleExtents.right, _visibleExtents.top, dstR, dstT);

			double dstW = imgW * specs.dx * _pixelPerProjectionX;
			double dstH = imgH * specs.dy * _pixelPerProjectionY;

			_graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

			// -------------------------------------------------------
			//  preparing structure to receive the data
			// -------------------------------------------------------
			BITMAPINFO bif;
			InitBitmapHeader(specs.width, bitsPerPixel, numRead, pad, bif);

			unsigned char* bits = ReadGdalBufferBlock(pData, row, specs.width, bitsPerPixel / 8, pad, numRead);

			// choosing sampling method		
			int bmp = type == BITMAP_FILE || type == USE_FILE_EXTENSION;
			int w = bmp ? specs.width : ImageHelper::Cast(iimg)->GetOriginalBufferWidth();
			int h = bmp ? specs.height : ImageHelper::Cast(iimg)->GetOriginalBufferHeight();
			bool downsampling = double(dstR - dstL) * double(dstB - dstT) < (double)(w * h);
			_graphics->SetInterpolationMode(ImageHelper::GetInterpolationMode(iimg, !downsampling));

			Gdiplus::Bitmap imgPlus(&bif, (void*)bits);

			if (returnBitmap)
			{
				screenBitmap = CreateScreenBitmap(dstL, dstT, dstW, dstH);

				Gdiplus::Graphics g(screenBitmap->bitmap);
				g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
				g.SetInterpolationMode(ImageHelper::GetInterpolationMode(iimg, !downsampling));

				DrawGdalImage(&g, &imgPlus, imgAttr, dstL, dstT, dstW, dstH, imgX, imgY, imgH, imgW, true);

				_graphics->DrawImage(screenBitmap->bitmap, (Gdiplus::REAL)screenBitmap->left, (Gdiplus::REAL)screenBitmap->top);
			}
			else
			{
				DrawGdalImage(_graphics, &imgPlus, imgAttr, dstL, dstT, dstW, dstH, imgX, imgY, imgH, imgW, false);
			}

			if (pad != 0)	// otherwise we were using the buffer directly
			{
				delete[] bits;		
			}
		}
		
		// going to the next block
		row += numRead;
	}

	return screenBitmap;
}

// ******************************************************************
//		DrawGdalImage()
// ******************************************************************
void CImageDrawer::DrawGdalImage(Gdiplus::Graphics* g, Gdiplus::Image* img, Gdiplus::ImageAttributes* imgAttr,
								double dstL, double dstT, double dstW, double dstH, 
								int imgX, int imgY, int imgH, int imgW, bool atOrigin)
{
	if (atOrigin)
	{
		dstL = 0;
		dstT = 0;
	}

	Gdiplus::RectF rect((Gdiplus::REAL)int(dstL),
					    (Gdiplus::REAL)int(dstT), 
						(Gdiplus::REAL)int(dstW + 1.0), 
						(Gdiplus::REAL)int(dstH + 1.0));

	g->DrawImage(img, rect, 
				(Gdiplus::REAL)(imgX), 
				(Gdiplus::REAL)(imgY),
				(Gdiplus::REAL)imgW, 
				(Gdiplus::REAL)imgH, 
				Gdiplus::UnitPixel, 
				imgAttr);
}

// ******************************************************************
//		CreateScreenBitmap()
// ******************************************************************
ScreenBitmap* CImageDrawer::CreateScreenBitmap(double dstL, double dstT, double dstW, double dstH)
{
	ScreenBitmap* screenBitmap = new ScreenBitmap();		// TODO: where is it released?
	screenBitmap->left = int(dstL);
	screenBitmap->top = int(dstT);
	screenBitmap->pixelPerProjectionX = _pixelPerProjectionX;
	screenBitmap->pixelPerProjectionY = _pixelPerProjectionY;
	screenBitmap->viewHeight = _viewHeight;
	screenBitmap->viewWidth = _viewWidth;
	screenBitmap->extents = *_extents;
	screenBitmap->bitmap = new Gdiplus::Bitmap((INT)(dstW + 1.0), (INT)(dstH + 1.0));
	return screenBitmap;
}

// ******************************************************************
//		ReadGdalBlock()
// ******************************************************************
// allocated new buffer in case padding isn't 0
unsigned char* CImageDrawer::ReadGdalBufferBlock(unsigned char* buffer, int row, int width, int bytesPerPixel, int pad, int numRead)
{
	unsigned char* bits = (unsigned char*)(&buffer[row * width * bytesPerPixel]);

	if (pad == 0) {
		return bits;
	}
	
	// we can make number of image buffer pixels in row dividable by 4 them we don't need this condition
	int nBytesInRow = width * bytesPerPixel + pad;
	
	unsigned char* bitsNew = new unsigned char[nBytesInRow * numRead];
	for (int i = 0; i < numRead; i++) 
	{
		memcpy(&bitsNew[i * nBytesInRow], &bits[i * width * bytesPerPixel], width * bytesPerPixel);
	}

	return bitsNew;
}

// ******************************************************************
//		GetWidthPad()
// ******************************************************************
// width in bits must be divisible by 32
int CImageDrawer::GetRowBytePad(int width, int bitsPerPixel)
{
	int pad = (width * bitsPerPixel) % 32;
	if (pad != 0)
	{
		pad = 32 - pad;
		pad /= 8;
	}

	return pad;
}

// ******************************************************************
//		InitBitmapInfo()
// ******************************************************************
void CImageDrawer::InitBitmapHeader(int width, int bitsPerPixel, int height, int pad, BITMAPINFO& bif)
{
	BITMAPINFOHEADER bih;
	bih.biCompression = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);

	bih.biBitCount = bitsPerPixel;
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biSizeImage = (width * bitsPerPixel / 8 + pad) * height;

	bif.bmiHeader = bih;
}

// ******************************************************************
//		CreateSmallerProxyForGdalRaster()
// ******************************************************************
//	Creating temporary instance of image class holding 
//	the area to display. Don't forget to release it after!!!
//  Is used for grouped images first of all
IImage* CImageDrawer::CreateSmallerProxyForGdalRaster(ImageSpecs& specs, IImage* img, CRect rcBounds, int bytesPerPixel)
{
	double mapL, mapT, mapR, mapB;	// map units w/o clipping (world coordinates to place the image)
	double scrL, scrT, scrR, scrB;	// screen units  after projection w/o clipping (area needed to draw all the image)
	int dstL, dstR, dstT, dstB;	// screen units with clipping (area needed to draw visible part)
	int imgX, imgY, imgW, imgH;	// image units with clipping (part of image to draw)

	// part to draw in map units
	mapL = specs.xllCorner;
	mapB = specs.yllCorner;
	mapT = mapB + specs.height * specs.dy;
	mapR = mapL + specs.width * specs.dx;

	ProjectionToPixel(mapL, mapT, scrL, scrT);
	ProjectionToPixel(mapR, mapB, scrR, scrB);

	dstL = (int)scrL;
	dstT = (int)scrT;
	dstR = (int)scrR; dstB = (int)scrB;

	imgX = imgY = 0;
	imgW = specs.width;
	imgH = specs.height;

	CalculateImageBlockSize(dstL, dstT, dstR, dstB, imgX, imgY, imgW, imgH, mapL, mapT, mapR, mapB, specs.dx, specs.dy, rcBounds);

	if (imgW * imgH * 2.0 > specs.width * specs.height)
	{
		// no need to copy data; we need to show almost all bitmap
		return NULL;
	}
	
	// **************************************************
	// copying part of the bitmap which will be used
	// **************************************************
	unsigned char* pData = ImageHelper::GetImageData(img);
	if (!pData)	{
		return NULL;
	}

	unsigned char* pDataTemp = NULL;
	colour* pColours = NULL;

	try	{
		pColours = new colour[imgW * imgH];
	}
	catch (...)
	{
		CallbackHelper::ErrorMsg("Drawing GDAL raster: failed to allocated memory buffer.");
		return NULL;
	}

	pDataTemp = reinterpret_cast<unsigned char*>(pColours);
	for (int n = 0; n < imgH; n++)
	{
		memcpy(pDataTemp + (imgH - 1 - n) * imgW * bytesPerPixel, 
			   pData + (specs.height - n - 1 - imgY) * specs.width * bytesPerPixel + imgX * bytesPerPixel, 
			   imgW * bytesPerPixel);
	}

	// ---------------------------------------------------------
	//	  preparing new class to save results
	// ---------------------------------------------------------
	IImage* imgNew = NULL;
	CoCreateInstance(CLSID_Image, NULL, CLSCTX_INPROC_SERVER, IID_IImage, (void**)&imgNew);

	VARIANT_BOOL vb;
	imgNew->CreateNew(imgW, imgH, &vb);
	if (!vb)
	{
		imgNew->Release();
		delete[] pColours;
		return NULL;
	}

	// passing the data to image class
	ImageHelper::PutImageData(imgNew, pColours);

	// setting the properties of the new bitmap
	double projX, projY;
	PixelToProjection(dstL, dstB, projX, projY);

	specs.xllCorner = projX;
	specs.yllCorner = projY;

	imgNew->put_XllCenter(specs.xllCorner);
	imgNew->put_YllCenter(specs.yllCorner);

	specs.width = imgW;
	specs.height = imgH;

	return imgNew;
}

// ******************************************************************
//		GetPixelsInView()
// ******************************************************************
long CImageDrawer::GetPixelsInView()
{
	return static_cast<long>(_extents->Width() * _pixelPerProjectionX * _extents->Height() * _pixelPerProjectionY);
}

// ******************************************************************
//		CalculateImageBlock()
// ******************************************************************
// Calculates the part of image to draw (img) and the screen rectangular to draw (dst)
// uses dimensions of block in map coordinates (map)
bool CImageDrawer::CalculateImageBlockSize(int& dstL, int& dstT, int& dstR, int& dstB, 
								   int& imgX, int& imgY, int& imgW, int& imgH, 
						           double& mapL, double& mapT, double& mapR, double& mapB,
								   const double dx, const double dy, const CRect& rcBounds)
{
	double fraction = 0.0;
	double imgCellsHidden;
	int offset;
	double hiddenRange;

	if( dstL < -1 )			//adjust the left side
	{
		hiddenRange = _extents->left - mapL;								// in world coordinates
		imgCellsHidden = hiddenRange/dx;								//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;				//fraction of cell off screen (in img cells)
		offset = int(fraction * dx * _pixelPerProjectionX);					//screen pixels needed off screen
		dstL = static_cast<int>(rcBounds.left - offset + 0.5);			//adjusted drawing area
		imgX = (int)imgCellsHidden;										//new img starting index
		imgW -= (int)imgCellsHidden;									//chop off hidden cells from width
	}
	if( dstT < -1 )			//adjust the top
	{
		hiddenRange = mapT - _extents->top;					// in world coordinates
		imgCellsHidden = hiddenRange/dy;					//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;	//fraction of cell off screen (in img cells)
		offset = int(fraction * dy * _pixelPerProjectionY);		//screen pixels needed off screen
		dstT = static_cast<int>(rcBounds.top - offset + 0.5);						//adjusted drawing area
		imgY = (int)imgCellsHidden;							//new img starting index
		imgH -= (int)imgCellsHidden;							//chop off hidden cells from width
	}
	if( dstR > _viewWidth )	//adjust the right side
	{
		hiddenRange = mapR - _extents->right;					//in world coordinates
		imgCellsHidden = hiddenRange /dx;					//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;	//fraction of cell off screen
		offset = int(fraction * dx * _pixelPerProjectionX);		//screen pixels needed off screen
		dstR = static_cast<int>(rcBounds.right + offset + 0.5);						//adjusted drawing area
		imgW -=(int)imgCellsHidden;
	}
	if( dstB > _viewHeight )	//adjust the bottom side
	{
		hiddenRange = _extents->bottom - mapB;					//in world coordinates
		imgCellsHidden = hiddenRange /dy;					//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;	//the fraction of a cell that is hidden
		offset = int(fraction * dy * _pixelPerProjectionY);		//screen pixels out of view
		dstB = static_cast<int>(rcBounds.bottom + offset + 0.5);					//adjust the drawing area
		imgH -= (int)imgCellsHidden;
	}
	
	return true;
}

// ***********************************************************
//		CalculateImageBlockSize()
// ***********************************************************
// extent - extents of the image block in map coordinates;
// minX, etc - visible extents in map coordinates
// Goal - to find which pixels of the image will be visible (_visibleRect in image coordinates)
// and to what map coordinates they correspond (_visibleExtents in map coordinates)
// The conversion to the screen coordinates can be made with standard means
bool CImageDrawer::CalculateImageBlockSize(Extent extents, double MinX, double MinY, double MaxX, double MaxY,
										   const double dX, const double dY, const int height, const int width)
{
	int hiddenPixels;
	
	_visibleExtents.left = extents.left;
	_visibleRect.left = 0;
	if (extents.left < MinX) 
	{	
		hiddenPixels = int((MinX - extents.left)/dX);		// how many pixels are completely hidden: rounding in the lower side
		_visibleExtents.left += (double)hiddenPixels * dX;
		_visibleRect.left += hiddenPixels;
	}
    
	_visibleExtents.bottom = extents.bottom;
	_visibleRect.bottom = height - 1;
	if (extents.bottom < MinY)
	{	
		hiddenPixels = int((MinY - extents.bottom)/dY);	// how many pixels are completely hidden: rounding in the lower side
		_visibleExtents.bottom +=  (double)hiddenPixels * dY;
		_visibleRect.bottom -= hiddenPixels;
	}
	
	_visibleExtents.right = extents.right;
	_visibleRect.right = width - 1;
	if (extents.right > MaxX) 
	{	
		hiddenPixels = int((extents.right - MaxX)/dX);		// how many pixels are completely hidden: rounding in the lower side
		_visibleExtents.right -= (double)hiddenPixels * dX;
		_visibleRect.right -= hiddenPixels;
	}

	_visibleExtents.top = extents.top;
	_visibleRect.top = 0;
	if (extents.top > MaxY)
	{	
		hiddenPixels = int((extents.top-MaxY)/ dY);		// how many pixels are completely hidden: rounding in the lower side
		_visibleExtents.top -=  (double)hiddenPixels * dY;
		_visibleRect.top += hiddenPixels;
	}

	return true;
}

// ***********************************************************
//		WithinVisibleExtents()
// ***********************************************************
bool CImageDrawer::WithinVisibleExtents(double xMin, double xMax, double yMin, double yMax)
{
	return 	!(xMin > _extents->right || xMax < _extents->left || yMin > _extents->top || yMax < _extents->bottom);
}

// ******************************************************************
//		ReadImageSpecs()
// ******************************************************************
bool CImageDrawer::ReadImageSpecs(IImage* img, ImageSpecs& specs)
{
	long width, height;
	img->get_Width(&width);
	img->get_Height(&height);

	if (width <= 0 || height <= 0)
		return false;

	double dx = 0, dy = 0;
	img->get_dX(&dx);
	img->get_dY(&dy);
	if (dx <= 0.0 || dy <= 0.0)
		return false;

	double xllCorner, yllCorner;	// in projected coordinates
	img->get_XllCenter(&xllCorner);
	img->get_YllCenter(&yllCorner);
	xllCorner -= dx*.5;
	yllCorner -= dy*.5;

	specs.dx = dx;
	specs.dy = dy;
	specs.xllCorner = xllCorner;
	specs.yllCorner = yllCorner;
	specs.width = width;
	specs.height = height;

	return true;
}

// ******************************************************************
//		DrawBmpNative()
// ******************************************************************
void CImageDrawer::DrawBmpNative(IImage* img, ImageSpecs& specs, Gdiplus::ImageAttributes* imgAttr, CRect rcBounds)
{
	long blockSize = 4096;	//blockSize = 1024; there is better performance the first time read @ 1024

	FILE * imgfile = NULL;
	long fileHandle = -1;
	img->get_FileHandle(&fileHandle);
	if (fileHandle >= 0)
	{
		USES_CONVERSION;
		CComBSTR fname;
		img->get_Filename(&fname);
		imgfile = ::_wfopen(OLE2CW(fname), L"rb");
	}

	unsigned char * data = NULL;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmif;
	RGBQUAD * bmiColors = NULL;
	BITMAPINFO * bi = NULL;

	if (!imgfile)
		goto clear_bmp;

	//read in BITMAPFILEHEADER
	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, imgfile);

	//if not a valid bitmap file, fail to load
	if (bmfh.bfType != 19778)
	{
		CallbackHelper::ErrorMsg("Invalid BMP file. bmfh.bfType = 19778 was expected.");
		goto clear_bmp;
	}

	//read in BITMAPINFOHEADER
	fread(&bmif, sizeof(BITMAPINFOHEADER), 1, imgfile);
	bmif.biClrUsed = (bmfh.bfOffBits - 54) / 4;
	if (bmif.biClrUsed != 0)
	{
		bmiColors = new RGBQUAD[bmif.biClrUsed];
		fread(bmiColors, sizeof(RGBQUAD), bmif.biClrUsed, imgfile);
	}

	//Read to the beginning of the data
	int sizeof_header = 54;
	int n = bmfh.bfOffBits - (54 + bmif.biClrUsed * 4);
	if (n > 0) {
		fseek(imgfile, n, SEEK_CUR);
	}

	// Read the specific bit count type
	long rowLength = GetBmpRowLength(specs.width, bmif.biBitCount);
	
	//Compute the pad
	int pad = GetRowBytePad(bmif.biWidth, bmif.biBitCount);

	// ------------------------------------------------------
	//	 Preparing variables
	// ------------------------------------------------------
	data = new unsigned char[(rowLength + pad)*blockSize];
	long numRead;				// number of rows read

	double mapL, mapT, mapR, mapB;	// map units w/o clipping (world coordinates to place the image)
	double scrL, scrT, scrR, scrB;	// screen units  after projection w/o clipping (area needed to draw all the image)
	int dstL, dstR, dstT, dstB;	// screen units with clipping (area needed to draw visible part)
	int imgX, imgY, imgW, imgH;	// image units with clipping (part of image to draw)

	bi = new BITMAPINFO;		//SetDIBits variable
	bi = (BITMAPINFO*)realloc(bi, sizeof(BITMAPINFO) + sizeof(RGBQUAD)* bmif.biClrUsed);	//This adjusts the bi.bmiColors to the right size of array

	// -----------------------------------------------
	//    Start drawing
	// -----------------------------------------------
	for (int row = 0; row < specs.height;)
	{
		numRead = blockSize;
		if (row + blockSize >= specs.height)
			numRead = specs.height - row;

		// part to draw in map units
		mapL = specs.xllCorner;
		mapB = specs.yllCorner + row * specs.dy;
		mapT = mapB + numRead * specs.dy;
		mapR = mapL + specs.width * specs.dx;

		ProjectionToPixel(mapL, mapT, scrL, scrT);
		ProjectionToPixel(mapR, mapB, scrR, scrB);

		if (row != 0) scrB++;

		fread(data, sizeof(unsigned char), (rowLength + pad)*numRead, imgfile);
		bi->bmiHeader = bmif;
		bi->bmiHeader.biHeight = numRead;
		bi->bmiHeader.biSizeImage = (rowLength + pad)*numRead;
		memcpy(bi->bmiColors, bmiColors, sizeof(RGBQUAD)*bmif.biClrUsed);

		if (WithinVisibleExtents(mapL, mapR, mapB, mapT))
		{
			dstL = (int)scrL;
			dstT = (int)scrT;
			dstR = (int)scrR;
			dstB = (int)scrB;

			imgX = 0;
			imgY = 0;
			imgW = specs.width;
			imgH = numRead;

			CalculateImageBlockSize(dstL, dstT, dstR, dstB, imgX, imgY, imgW, imgH, mapL, mapT, mapR, mapB, specs.dx, specs.dy, rcBounds);

			double destSize = double(dstR - dstL) * double(dstB - dstT);

			Gdiplus::InterpolationMode mode = ImageHelper::GetInterpolationMode(img, destSize > (imgW * imgH));
			_graphics->SetInterpolationMode(mode);

			Gdiplus::Bitmap imgPlus(bi, (void*)data);
			Gdiplus::RectF rect((Gdiplus::REAL)dstL, (Gdiplus::REAL)dstT, (Gdiplus::REAL)dstR - dstL, (Gdiplus::REAL)dstB - dstT);
			_graphics->DrawImage((Gdiplus::Image*)&imgPlus, rect, (Gdiplus::REAL)(imgX /*- 0.5*/), (Gdiplus::REAL)(imgY /*-0.5*/),
				(Gdiplus::REAL)imgW, (Gdiplus::REAL)imgH, Gdiplus::UnitPixel, imgAttr);
		}
		row += numRead;
	}

	// --------------------------------------------------
	//	   Cleaning
	// --------------------------------------------------
clear_bmp:
	if (bi) {
		delete bi;
	}

	if (bmiColors) {
		delete[] bmiColors;
	}

	if (data) {
		delete[] data;
	}

	if (imgfile) {
		fclose(imgfile);
	}

	if (fileHandle > 0)	{
		_close(fileHandle);
	}
}

// ******************************************************************
//		GetBmpRowLength()
// ******************************************************************
int CImageDrawer::GetBmpRowLength(int width, int bitsPerPixel)
{
	long rowLength = width;
	
	if (bitsPerPixel == 24)	
	{
		rowLength *= 3;
	}
	else if (bitsPerPixel == 8)	
	{
		// do nothing
	}
	else if (bitsPerPixel == 1)	
	{
		rowLength = (long)ceil(((double)width) / 8);
	}
	else if (bitsPerPixel == 4)
	{
		rowLength = int((double)rowLength * .5);
		if (width % 2) {
			rowLength++;
		}
	}
	
	return rowLength;
}