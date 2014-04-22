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
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 // Sergei Leschinski (lsu) 25 june 2010 - created the file.

#include "stdafx.h"
#include <io.h>
#include "Image.h"
#include "ImageDrawing.h"
#include "ImageAttributes.h"
#include "macros.h"

// ******************************************************************
//		DrawImage()
// ******************************************************************
// New implementation using GDI+
ScreenBitmap* CImageDrawer::DrawImage(const CRect & rcBounds, IImage* iimg, bool returnBitmap)
{
	if( iimg == NULL )
		return NULL;
	
	ScreenBitmap* screenBitmap = NULL;

	ImageType type;
	iimg->get_ImageType(&type);

	long imgWidth, imgHeight;
	iimg->get_Width(&imgWidth);
	iimg->get_Height(&imgHeight);

	//reloading image buffer if it was scheduled
	// or try to load image buffer in case it wasn't loaded before because of image grouping
	if (((CImageClass*)iimg)->_bufferReloadIsNeeded || ((CImageClass*)iimg)->_imageChanged || (imgWidth == 0 && imgHeight == 0 && type != BITMAP_FILE))
	{				
		long pixelsInView = long(((_extents->right - _extents->left) * _pixelPerProjectionX) * ((_extents->top - _extents->bottom)*_pixelPerProjectionY));
		iimg->SetVisibleExtents(_extents->left,_extents->bottom,_extents->right,_extents->top, pixelsInView, 0.0);
	}

	iimg->get_Width(&imgWidth);
	iimg->get_Height(&imgHeight);

	if( imgWidth <= 0 || imgHeight <= 0 )
		return NULL;

	double dx = 0, dy = 0;
	iimg->get_dX( &dx );
	iimg->get_dY( &dy );
	if( dx <= 0.0 || dy <= 0.0 )
		return NULL;
	
	double xllCorner, yllCorner, xtrCorner, ytrCorner;	// in projected coordinates
	iimg->get_XllCenter( &xllCorner );
	iimg->get_YllCenter( &yllCorner );
	xllCorner -= dx*.5;
	yllCorner -= dy*.5;
	xtrCorner = xllCorner+(double)imgWidth*dx;
	ytrCorner = yllCorner+(double)imgHeight*dy;
	
	if (!WithinVisibleExtents(xllCorner,xtrCorner,yllCorner, ytrCorner))
		return NULL;
	
	// ------------------------------------------------------------
	//	   reading properties
	// ------------------------------------------------------------
	VARIANT_BOOL inram;
	iimg->get_IsInRam(&inram);

	VARIANT_BOOL useTransparencyColor;
	iimg->get_UseTransparencyColor(&useTransparencyColor);

	OLE_COLOR imageTransparencyColor;
	iimg->get_TransparencyColor( &imageTransparencyColor );

	OLE_COLOR transpColor2 = RGB(0,0,0);
	iimg->get_TransparencyColor2( &transpColor2 );

	double transpPercent;
	iimg->get_TransparencyPercent(&transpPercent);

	VARIANT_BOOL setToGray;
	iimg->get_SetToGrey(&setToGray);

	tkInterpolationMode downsamplingMode;
	tkInterpolationMode upsamplingMode;
	iimg->get_DownsamplingMode(&downsamplingMode);
	iimg->get_UpsamplingMode(&upsamplingMode);
	
	int drawingMethod;
	iimg->get_DrawingMethod(&drawingMethod);
	bool useGDIplus = (drawingMethod & idmGDIPlusDrawing) != 0;		// set this to false to use GDI functions

	long blockSize = 4096;	//blockSize = 1024; there is better performance the first time read @ 1024
	
	// -----------------------------------------------------
	// Preparing to draw; Creating temporary bitmap and DC 
	// Graphics and ImageAttributes for GDI+ drawing
	// -----------------------------------------------------
	Gdiplus::Graphics* gr = NULL;
	Gdiplus::ImageAttributes* imgAttr= NULL;
	
	CDC * subsetDC = NULL;
	CBitmap * bmp = NULL;
	CBitmap * oldBMP = NULL;

	if (!useGDIplus)
	{
		HDC hdc =_graphics->GetHDC();
		_dc = CDC::FromHandle(hdc);
		
		subsetDC = new CDC();
		bmp = new CBitmap();
		
		if (!bmp->CreateDiscardableBitmap(_dc,imgWidth,blockSize))
		{
			((CImageClass*)iimg)->ErrorMessage(tkCANT_CREATE_DDB_BITMAP);
			return NULL;
		}
		subsetDC->CreateCompatibleDC(_dc);
		oldBMP = subsetDC->SelectObject(bmp);

		_graphics->ReleaseHDC(hdc);
	}
	else
	{
		gr = _graphics;	// new Gdiplus::Graphics(_dc->m_hDC);
		gr->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
		imgAttr = new CImageAttributesEx(transpPercent, setToGray?true:false, useTransparencyColor?true:false, imageTransparencyColor, transpColor2);
	}
	
	// ----------------------------------------------------------------- //
	//		Disk-based bitmap (BMP)										 //
	// ----------------------------------------------------------------- //
	if(!inram && type == BITMAP_FILE)   //&& imgfile != NULL )  // lsu: if there is no file the one is expected, it won't be possible to draw any thing all the same
	{
		// opening file
		FILE * imgfile = NULL;
		long fileHandle = -1;
		iimg->get_FileHandle(&fileHandle);
		if( fileHandle >= 0 )
		{
			USES_CONVERSION;
			CComBSTR fname;
			iimg->get_Filename(&fname);
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
		fread(&bmfh,sizeof(BITMAPFILEHEADER),1,imgfile);
		
		//if not a valid bitmap file, fail to load
		if (bmfh.bfType != 19778)
		{
			((CImageClass*)iimg)->ErrorMessage(tkINVALID_FILE);
			goto clear_bmp;
		}
		
		//read in BITMAPINFOHEADER
		fread(&bmif,sizeof(BITMAPINFOHEADER),1,imgfile);
		bmif.biClrUsed = ( bmfh.bfOffBits - 54 )/4;
		if( bmif.biClrUsed != 0 )
		{	
			bmiColors = new RGBQUAD[bmif.biClrUsed];
			fread( bmiColors, sizeof(RGBQUAD), bmif.biClrUsed, imgfile );
		}
		
		//Read to the beginning of the data
		int sizeof_header = 54;
		int n = bmfh.bfOffBits - (54 + bmif.biClrUsed*4);
		if( n > 0 )
			fseek(imgfile,n,SEEK_CUR);

		// Read the specific bitcount type and Create a Palette
		long rowLength = imgWidth;
		if( bmif.biBitCount == 24 )		rowLength*=3;
		else if( bmif.biBitCount == 8 )	{}
		else if( bmif.biBitCount == 1 )	rowLength = (long)ceil( ((double)imgWidth)/8 );
		else if( bmif.biBitCount == 4 )
		{	
			rowLength = int((double)rowLength * .5);
			if( imgWidth % 2 ) rowLength++;
		}
		
		//Compute the pad
		int	pad = bmif.biWidth * bmif.biBitCount;
		pad %= 32;
		if(pad != 0)
		{	
			pad = 32 - pad;
			pad /= 8;
		}
		
		// ------------------------------------------------------
		//	 Preparing variables
		// ------------------------------------------------------
		data = new unsigned char[(rowLength+pad)*blockSize];
		long numRead;				// number of rows read
		
		double mapL, mapT, mapR, mapB;	// map units w/o clipping (world coordinates to place the image)
		double scrL, scrT, scrR, scrB;	// screen units  after projection w/o clipping (area needed to draw all the image)
		int dstL, dstR, dstT, dstB;	// screen units with clipping (area needed to draw visible part)
		int imgX, imgY, imgW, imgH;	// image units with clipping (part of image to draw)

		bi = new BITMAPINFO;		//SetDIBits variable
		bi = (BITMAPINFO*)realloc(bi,sizeof(BITMAPINFO)+ sizeof(RGBQUAD)* bmif.biClrUsed);	//This adjusts the bi.bmiColors to the right size of array

		// -----------------------------------------------
		//    Start drawing
		// -----------------------------------------------
		for(int row = 0; row < imgHeight; )
		{
			numRead = blockSize;
			if( row + blockSize >= imgHeight )
				numRead = imgHeight - row;
			
			// part to draw in map units
			mapL = xllCorner;
			mapB = yllCorner + row * dy ;
			mapT = mapB + numRead * dy;
			mapR = mapL + imgWidth * dx;
			
			ProjectionToPixel(mapL,mapT,scrL,scrT);
			ProjectionToPixel(mapR,mapB,scrR,scrB);

			if (row != 0) scrB++;

			fread(data,sizeof(unsigned char),(rowLength+pad)*numRead, imgfile);
			bi->bmiHeader = bmif;
			bi->bmiHeader.biHeight = numRead;
			bi->bmiHeader.biSizeImage = (rowLength+pad)*numRead;
			memcpy(bi->bmiColors,bmiColors,sizeof(RGBQUAD)*bmif.biClrUsed);

			if (WithinVisibleExtents(mapL,mapR, mapB,mapT))
			{
				dstL = (int)scrL; dstT = (int)scrT; dstR = (int)scrR; dstB = (int)scrB;
				imgX = 0; imgY = 0; imgW = imgWidth; imgH = numRead;
				
				CalculateImageBlockSize(dstL, dstT, dstR, dstB, imgX, imgY, imgW, imgH, mapL, mapT, mapR, mapB, dx, dy, rcBounds);
				
				if (useGDIplus)
				{
					double destSize =  double(dstR - dstL) * double(dstB - dstT);
					if (destSize > (imgW * imgH))
						gr->SetInterpolationMode((Gdiplus::InterpolationMode)upsamplingMode);
					else
						gr->SetInterpolationMode((Gdiplus::InterpolationMode)downsamplingMode);
					
					Gdiplus::Bitmap imgPlus(bi, (void*)data);
					Gdiplus::RectF rect((Gdiplus::REAL)dstL, (Gdiplus::REAL)dstT, (Gdiplus::REAL)dstR - dstL, (Gdiplus::REAL)dstB - dstT);
					gr->DrawImage((Gdiplus::Image*)&imgPlus, rect, (Gdiplus::REAL)(imgX /*- 0.5*/), (Gdiplus::REAL)(imgY /*-0.5*/), 
																	(Gdiplus::REAL)imgW, (Gdiplus::REAL)imgH, Gdiplus::UnitPixel, imgAttr);
				}
				else
				{
					SetDIBitsToDevice(subsetDC->m_hDC,0,0,imgWidth,numRead,0,0,0,numRead,data,bi,DIB_RGB_COLORS);

					if( useTransparencyColor != FALSE )
						TransparentBlt(_dc->m_hDC,dstL,dstT,dstR-dstL,dstB-dstT,subsetDC->m_hDC,imgX,imgY, imgW, imgH, imageTransparencyColor);
					else
					{	
						_dc->SetStretchBltMode(COLORONCOLOR);
						StretchBlt(_dc->m_hDC,dstL,dstT,dstR-dstL,dstB-dstT,subsetDC->m_hDC, imgX,imgY,imgW,imgH,SRCCOPY);
					}
				}
			}
			row += numRead;
		}
		
		// --------------------------------------------------
		//	   Cleaning
		// --------------------------------------------------
clear_bmp:
		if (bi)
		{
			delete bi;
			bi = NULL;
		}

		if( bmiColors )	
		{
			delete [] bmiColors;
			bmiColors=NULL;
		}
		
		if( data )	
		{
			delete [] data;
			data = NULL;
		}

		if (imgfile)
		{
			fclose(imgfile);
			imgfile=NULL;
		}
		
		if (fileHandle > 0) 
		{
			_close(fileHandle);
		}
	}

	// -----------------------------------------------------------------
	//		In-memory image (GDAL-based or bitmap)
	// -----------------------------------------------------------------
    else
	{	
		IImage* iimgNew = NULL;

		// --------------------------------------------------------------
		//	Creating temporary instance of image class holding 
		//	the area to display. Don't forget to release it after!!!
		//  Is used for grouped images first of all
		// --------------------------------------------------------------
		if (type == USE_FILE_EXTENSION ) //|| type == BITMAP_FILE )
		{
			double mapL, mapT, mapR, mapB;	// map units w/o clipping (world coordinates to place the image)
			double scrL, scrT, scrR, scrB;	// screen units  after projection w/o clipping (area needed to draw all the image)
			int dstL, dstR, dstT, dstB;	// screen units with clipping (area needed to draw visible part)
			int imgX, imgY, imgW, imgH;	// image units with clipping (part of image to draw)

			// part to draw in map units
			mapL = xllCorner;
			mapB = yllCorner;
			mapT = mapB + imgHeight * dy;
			mapR = mapL + imgWidth * dx;
			
			ProjectionToPixel(mapL,mapT,scrL,scrT);
			ProjectionToPixel(mapR,mapB,scrR,scrB);

			dstL = (int)scrL; dstT = (int)scrT; dstR = (int)scrR; dstB = (int)scrB;
			imgX = imgY = 0; imgW = imgWidth; imgH = imgHeight;
				
			CalculateImageBlockSize(dstL, dstT, dstR, dstB, imgX, imgY, imgW, imgH, mapL, mapT, mapR, mapB, dx, dy, rcBounds);
			
			if (imgW * imgH * 2.0 > imgWidth * imgHeight)
			{
				// no need to copy data; we need to show almost all bitmap
			}
			else
			{
				// **************************************************
				// copying part of the bitmap which will be used
				// **************************************************
				CImageClass* img = (CImageClass*)iimg;
				unsigned char* pData = img->get_ImageData();
				if (!pData)	goto final_clear;

				unsigned char* pDataTemp = NULL;
				colour* pColours = NULL;

				try	{
					pColours = new colour[imgW * imgH];
				}
				catch(...)
				{
					//_map->ErrorMessage(tkFAILED_TO_ALLOCATE_MEMORY);
					goto final_clear;
				}
				
				pDataTemp = reinterpret_cast<unsigned char*>(pColours);
				for (int n = 0; n < imgH; n++)
				{
					memcpy(pDataTemp + (imgH - 1 - n) * imgW * 3, pData + (imgHeight - n - 1- imgY) * imgWidth * 3 + imgX * 3, imgW * 3);
				}
			
				// ---------------------------------------------------------
				//	  preparing new class to save results
				// ---------------------------------------------------------
				CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimgNew);

				VARIANT_BOOL vbretval;
				iimgNew->CreateNew(imgW, imgH, &vbretval);
				if (!vbretval)
				{
					iimgNew->Release(); iimgNew = NULL;   // error message in the image class
					delete[] pColours;
					goto final_clear;
				}
				
				// passing the data to image class
				CImageClass* imgNew = (CImageClass*)iimgNew;
				imgNew->put_ImageData(pColours);
				
				// setting the properties of the new bitmap
				double projX, projY;
				PixelToProjection(dstL, dstB, projX, projY);
				
				xllCorner = projX;
				yllCorner = projY;

				imgNew->put_XllCenter(xllCorner);
				imgNew->put_YllCenter(yllCorner);

				imgWidth = imgW;
				imgHeight = imgH;
				
				iimg = iimgNew;
			}
		}
		
		// -----------------------------------------------------------
		//		Start drawing
		// -----------------------------------------------------------
		double mapL, mapT, mapR, mapB;	// map units w/o clipping (world coordinates to place the image)
		double dstL, dstR, dstT, dstB;	// screen units with clipping (area needed to draw visible part)
		int imgX, imgY, imgW, imgH;	// image units with clipping (part of image to draw)
		
		// ------------------------------------------------------
		//  accessing data from the buffer
		// ------------------------------------------------------
		int pad;
		unsigned char* pData = NULL;
		colour* pNewData = NULL;
		
		CImageClass* img = (CImageClass*)iimg;
		pData = img->get_ImageData();
		
		// try to load the buffer, as we could fail to load it because of unsuccessful image grouping
		if (!pData && (type != BITMAP_FILE && type != USE_FILE_EXTENSION))
		{
			long pixelsInView = long(((_extents->right - _extents->left) * _pixelPerProjectionX) * ((_extents->top - _extents->bottom)* _pixelPerProjectionY));
			iimg->SetVisibleExtents(_extents->left,_extents->bottom,_extents->right,_extents->top, pixelsInView, 0.0);
		}
		
		if (!pData) return NULL;

		// width in bits must be divisible by 32
		pad = (imgWidth * 24) % 32;
		if(pad != 0)
		{	pad = 32 - pad;
			pad /= 8;
		}

		// ------------------------------------------------------------
		//    Drawing blocks of the bitmap
		// ------------------------------------------------------------
		blockSize = 8192;
		long numRead;					// number of rows read
		for(int row = 0; row < imgHeight; )
		{
			numRead = blockSize;
			if( row + blockSize >= imgHeight )
				numRead = imgHeight - row;

			// part of image to draw in map units
			mapL = xllCorner;
			mapB = yllCorner + row * dy;
			mapT = mapB + (double)numRead * dy;
			mapR = mapL + (double)imgWidth * dx;
			
			if (WithinVisibleExtents(mapL,mapR, mapB,mapT))
			{
				Extent extImage = Extent(mapL, mapR, mapB, mapT);
				CalculateImageBlockSize1(extImage, _extents->left, _extents->bottom, _extents->right, _extents->top, dx, dy, numRead, imgWidth);
				
				imgX = _visibleRect.left;
				imgY = _visibleRect.top;
				imgW = _visibleRect.right - _visibleRect.left + 1;
				imgH = _visibleRect.bottom - _visibleRect.top + 1;
				
				ProjectionToPixel(_visibleExtents.left,_visibleExtents.bottom,dstL,dstB);
				ProjectionToPixel(_visibleExtents.right,_visibleExtents.top,dstR,dstT);
				
				double dstW = imgW * dx * _pixelPerProjectionX;
				double dstH = imgH * dy * _pixelPerProjectionY;
				
				gr->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
				
				// -------------------------------------------------------
				//  preparing structure to receive the data
				// -------------------------------------------------------
				BITMAPINFOHEADER bih;
				bih.biCompression=0;
				bih.biXPelsPerMeter=0;
				bih.biYPelsPerMeter=0;
				bih.biClrUsed=0;
				bih.biClrImportant=0;
				bih.biPlanes=1;
				bih.biSize=sizeof(BITMAPINFOHEADER);

				bih.biBitCount=24;
				bih.biWidth= imgWidth;
				bih.biHeight= numRead;
				bih.biSizeImage= (imgWidth * 3 + pad) * numRead;
				
				BITMAPINFO bif;
				bif.bmiHeader = bih;
				
				unsigned char* bits = (unsigned char*)(&pData[row * imgWidth * 3]);
				int nBytesInRow = imgWidth * 3 + pad;

				if (useGDIplus)
				{
					unsigned char* bitsNew = NULL;						
					if (pad == 0)
					{
						bitsNew = bits;
					}
					else
					{
						// we can make number of image buffer pixels in row divisable by 4 them we don't need this condition
						bitsNew = new unsigned char[nBytesInRow * numRead];
						for(int i = 0; i < numRead; i++)		
							memcpy(&bitsNew[i * nBytesInRow], &bits[i * imgWidth * 3], imgWidth * 3);
					}
					
					// choosing sampling method						
					bool downsampling;
					if (type == BITMAP_FILE || type == USE_FILE_EXTENSION)
						downsampling = (double(dstR - dstL) * double(dstB - dstT) < (imgWidth * imgHeight));
					else
						downsampling = (double(dstR - dstL) * double(dstB - dstT) < double(img->get_originalBufferWidth() * img->get_originalBufferHeight()));
							
					if ( downsampling )
						gr->SetInterpolationMode((Gdiplus::InterpolationMode)downsamplingMode);	
					else
						gr->SetInterpolationMode((Gdiplus::InterpolationMode)upsamplingMode);	
					
					Gdiplus::Bitmap imgPlus(&bif, (void*)bitsNew); 

					if (returnBitmap)
					{
						screenBitmap = new ScreenBitmap();
						screenBitmap->left = int(dstL);
						screenBitmap->top = int(dstT);
						screenBitmap->pixelPerProjectionX = _pixelPerProjectionX;
						screenBitmap->pixelPerProjectionY = _pixelPerProjectionY;
						screenBitmap->viewHeight = _viewHeight;
						screenBitmap->viewWidth = _viewWidth;
						screenBitmap->extents = *_extents;
						screenBitmap->bitmap = new Gdiplus::Bitmap((INT)(dstW+1.0), (INT)(dstH+1.0));

						Gdiplus::Graphics g(screenBitmap->bitmap);
						g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

						if ( downsampling )
							g.SetInterpolationMode((Gdiplus::InterpolationMode)downsamplingMode);
						else
							g.SetInterpolationMode((Gdiplus::InterpolationMode)upsamplingMode);	

						Gdiplus::RectF rect(0.0f, 0.0f, (Gdiplus::REAL)int(dstW+1.0), (Gdiplus::REAL)int(dstH+1.0));
						g.DrawImage((Gdiplus::Image*)&imgPlus, rect, (Gdiplus::REAL)(imgX), (Gdiplus::REAL)(imgY), 
																		(Gdiplus::REAL)imgW, (Gdiplus::REAL)imgH, Gdiplus::UnitPixel, imgAttr);
						
						gr->DrawImage(screenBitmap->bitmap, (Gdiplus::REAL)screenBitmap->left, (Gdiplus::REAL)screenBitmap->top);
					}
					else
					{
						Gdiplus::RectF rect((Gdiplus::REAL)int(dstL), (Gdiplus::REAL)int(dstT), (Gdiplus::REAL)int(dstW+1.0), (Gdiplus::REAL)int(dstH+1.0));
						gr->DrawImage((Gdiplus::Image*)&imgPlus, rect, (Gdiplus::REAL)(imgX), (Gdiplus::REAL)(imgY), 
																		(Gdiplus::REAL)imgW, (Gdiplus::REAL)imgH, Gdiplus::UnitPixel, imgAttr);
					}
					if (pad!= 0)
						delete[] bitsNew;
				}
				else	// GDI drawing
				{
					if (pad == 0)
					{
						SetDIBitsToDevice(subsetDC->m_hDC,0,0,imgWidth,numRead,0,0,0,numRead,bits,&bif,DIB_RGB_COLORS);
					}
					else	// we can make number of image buffer pixels in row divisable by 4 them we don't need this condition
					{
						int nBytesInRow = imgWidth * 3 + pad;
						unsigned char* bitsNew = new unsigned char[numRead * nBytesInRow]; //new unsigned char[bih.biSizeImage - 1];
						for(int i = 0; i < numRead; i++)
						{
							memcpy(&bitsNew[i * nBytesInRow], &bits[i * imgWidth * 3], imgWidth * 3);
						}
						SetDIBitsToDevice(subsetDC->m_hDC,0,0,imgWidth,numRead,0,0,0,numRead,bitsNew,&bif,DIB_RGB_COLORS);
						delete[] bitsNew;
					}
					
					//	Blt the image
					if( useTransparencyColor != FALSE )
						TransparentBlt(_dc->m_hDC,(int)dstL,(int)dstT,(int)(dstR-dstL),(int)(dstB-dstT),subsetDC->m_hDC,imgX,imgY, imgW, imgH, imageTransparencyColor);
					else
					{	
						_dc->SetStretchBltMode(COLORONCOLOR);
						StretchBlt(_dc->m_hDC,(int)dstL,(int)dstT,(int)(dstR-dstL),(int)(dstB-dstT),subsetDC->m_hDC, imgX,imgY,imgW,imgH,SRCCOPY);
					}
				}
			}
			// going to the next block
			row += numRead;
		}
		
		// releasing the temporary image in case it was created
		if (iimgNew)
		{
			iimgNew->Release(); iimgNew = NULL;
		}
	}

	// -------------------------------------------------------
	//		Cleaning: this part is obligatory
	// -------------------------------------------------------
final_clear:

	if (useGDIplus)
	{
		//delete gr;
		delete imgAttr;
	}
	else
	{
		subsetDC->SelectObject(oldBMP);
		bmp->DeleteObject();
		delete bmp;
		
		subsetDC->DeleteDC();
		delete subsetDC;
	}

	// returning bitmap to draw in case of grouped images
	if (returnBitmap)	
	{
		return screenBitmap;
	}
	else
	{
		return NULL;
	}
}

// ******************************************************************
//		CalculateImageBlock()
// ******************************************************************
// Calculates the part of image to draw (img) and the screen rectangular to draw (dst)
// uses dimensions of blcok in map coordinates (map)
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
		hiddenRange = _extents->left - mapL;								// in world coords
		imgCellsHidden = hiddenRange/dx;								//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;				//fraction of cell off screen (in img cells)
		offset = int(fraction * dx * _pixelPerProjectionX);					//screen pixels needed off screen
		dstL = static_cast<int>(rcBounds.left - offset + 0.5);			//adjusted drawing area
		imgX = (int)imgCellsHidden;										//new img starting index
		imgW -= (int)imgCellsHidden;									//chop off hidden cells from width
	}
	if( dstT < -1 )			//adjust the top
	{
		hiddenRange = mapT - _extents->top;					// in world coords
		imgCellsHidden = hiddenRange/dy;					//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;	//fraction of cell off screen (in img cells)
		offset = int(fraction * dy * _pixelPerProjectionY);		//screen pixels needed off screen
		dstT = static_cast<int>(rcBounds.top - offset + 0.5);						//adjusted drawing area
		imgY = (int)imgCellsHidden;							//new img starting index
		imgH -= (int)imgCellsHidden;							//chop off hidden cells from width
	}
	if( dstR > _viewWidth )	//adjust the right side
	{
		hiddenRange = mapR - _extents->right;					//in world coords
		imgCellsHidden = hiddenRange /dx;					//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;	//fraction of cell off screen
		offset = int(fraction * dx * _pixelPerProjectionX);		//screen pixels needed off screen
		dstR = static_cast<int>(rcBounds.right + offset + 0.5);						//adjusted drawing area
		imgW -=(int)imgCellsHidden;
	}
	if( dstB > _viewHeight )	//adjust the bottom side
	{
		hiddenRange = _extents->bottom - mapB;					//in world coords
		imgCellsHidden = hiddenRange /dy;					//in img cells
		fraction = imgCellsHidden - (int)imgCellsHidden;	//the fraction of a cell that is hidden
		offset = int(fraction * dy * _pixelPerProjectionY);		//screen pixels out of view
		dstB = static_cast<int>(rcBounds.bottom + offset + 0.5);					//adjust the drawing area
		imgH -= (int)imgCellsHidden;
	}
	
	
	return true;
}

// ***********************************************************
//		CalculateImageBlockSize1()
// ***********************************************************
// extent - extents of the image block in map coordinates;
// minX, etc - visible extents in map coordinates
// Goal - to find which pixels of the image will be visible (_visibleRect in image coordinates)
// and to what map coordinates they correspond (_visibleExtents in map coordinates)
// The conversion to the screen coordinates can be made with standard means
bool CImageDrawer::CalculateImageBlockSize1(Extent extents, double MinX, double MinY, double MaxX, double MaxY,
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