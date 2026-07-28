/**************************************************************************************
 * File name: Map_Snapshot.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of the snapshot functions. 
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
// Paul Meems sept. 2019 - MWGIS-183: Merge .NET and VB drawing functions

#include "stdafx.h"
#include "map.h"
#include "Tiles.h"

#pragma region REGION SnapShots

// *********************************************************
//		SnapShot()
// *********************************************************
LPDISPATCH CMapView::SnapShot(IExtents* BoundBox)
{
	if( BoundBox == nullptr)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return nullptr;
	}

	IExtents * box = nullptr;
	BoundBox->QueryInterface(IID_IExtents, (void**)&box);

	if( box == nullptr)
	{	
		ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		return nullptr;
	}

	double left, right, bottom, top, nv;
	box->GetBounds(&left,&bottom,&nv,&right,&top,&nv);
	box->Release();

	return SnapShotCore(left, right, bottom, top, _viewWidth, _viewHeight);
}

// *********************************************************
//		SnapShot2()
// *********************************************************
// use the indicated layer and zoom/width to determine the output size and clipping
IDispatch* CMapView::SnapShot2(LONG clippingLayerNbr, DOUBLE zoom, long pWidth)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	long Width, Height;
	double left, right, bottom, top;

	Layer * l = _allLayers[clippingLayerNbr];
	if( !IS_VALID_PTR(l) )
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return nullptr;
	}
	else
	{
		this->AdjustLayerExtents(clippingLayerNbr);
		left = l->extents.left;
		right = l->extents.right;
		top = l->extents.top;
		bottom = l->extents.bottom;

		if( l->IsShapefile() )
		{
			double ar = (right-left)/(top-bottom);
			Width = static_cast<long>(pWidth == 0 ? ((right - left) * zoom) : pWidth);
			Height = static_cast<long>(static_cast<double>(pWidth) / ar);
		}
		else if(l->IsImage())
		{
			Width = static_cast<long>(right - left);
			Height = static_cast<long>(top - bottom);
			if (zoom > 0)
			{
				Width *= static_cast<long>(zoom);
				Height *= static_cast<long>(zoom);
			}
		}
		else
		{
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return nullptr;
		}
	}

	if (Width <= 0 || Height <= 0)
	{
		ErrorMessage(tkINVALID_WIDTH_OR_HEIGHT);
		return nullptr;
	}

	return this->SnapShotCore(left, right, top, bottom, Width, Height);
}

// *********************************************************
//		SnapShot3()
// *********************************************************
//A new snapshot method which works a bit better specifically for the printing engine
//1. Draw to a back buffer, 2. Populate an Image object
LPDISPATCH CMapView::SnapShot3(double left, double right, double top, double bottom, long width)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	long Height = static_cast<long>(static_cast<double>(width) / ((right - left) / (top - bottom)));
	if (width <= 0 || Height <= 0)
	{
		ErrorMessage(tkINVALID_WIDTH_OR_HEIGHT);
		return nullptr;
	}

	return this->SnapShotCore(left, right, top, bottom, width, Height);
}

// *********************************************************************
//    LoadTilesForSnapshot()
// *********************************************************************
// Loads tiles for specified extents
BOOL CMapView::LoadTilesForSnapshot(IExtents* extents, LONG widthPixels, LPCTSTR key)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!extents)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return FALSE;
	}

	// Get the image height based on the box aspect ratio
	double xMin, xMax, yMin, yMax, zMin, zMax;
	extents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		
	// Make sure that the width and height are valid
	long Height = static_cast<long>(static_cast<double>(widthPixels) *(yMax - yMin) / (xMax - xMin));
	if (widthPixels <= 0 || Height <= 0)
	{
		ErrorMessage(tkINVALID_WIDTH_OR_HEIGHT);
		return FALSE;
	}

	//CString key = (char*)key;
	SetTempExtents(xMin, xMax, yMin, yMax, widthPixels, Height);

	bool tilesInCache = TilesAreInCache();
	if (!tilesInCache) {
		ReloadTiles(true, true, key);
	}

	RestoreExtents();

	return tilesInCache;
}

// *********************************************************************
//    SnapShotToDC2()
// *********************************************************************
BOOL CMapView::SnapShotToDC2(PVOID hdc, IExtents* extents, LONG width, float offsetX, float offsetY,
							 float clipX, float clipY, float clipWidth, float clipHeight)
{
	if(!extents || !hdc)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return FALSE;
	}
	// getting DC to draw
	HDC dc = static_cast<HDC>(hdc);
	CDC * tempDC = CDC::FromHandle(dc);

	// Get the image height based on the box aspect ration
	double xMin, xMax, yMin, yMax, zMin, zMax;
	extents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

	// Make sure that the width and height are valid
	long Height = static_cast<long>(static_cast<double>(width) *(yMax - yMin) / (xMax - xMin));
	if (width <= 0 || Height <= 0)
	{
		ErrorMessage(tkINVALID_WIDTH_OR_HEIGHT);
		return FALSE;
	}

	SnapShotCore(xMin, xMax, yMin, yMax, width, Height, tempDC, offsetX, offsetY, clipX, clipY, clipWidth, clipHeight);
	return TRUE;
}

// *********************************************************************
//    SnapShotToDC()
// *********************************************************************
// Draws the specified extents of map at given DC.
BOOL CMapView::SnapShotToDC(PVOID hdc, IExtents* extents, LONG width)
{
	return this->SnapShotToDC2(hdc, extents, width, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

// *********************************************************************
//    SetTempExtents()
// *********************************************************************
void CMapView::SetTempExtents(double left, double right, double top, double bottom, long Width, long Height)
{
	mm_viewWidth = _viewWidth;
	mm_viewHeight = _viewHeight;
	mm_pixelPerProjectionX = _pixelPerProjectionX;
	mm_pixelPerProjectionY = _pixelPerProjectionY;
	mm_inversePixelPerProjectionX = _inversePixelPerProjectionX;
	mm_inversePixelPerProjectionY = _inversePixelPerProjectionY;
	mm_aspectRatio = _aspectRatio;
	mm_left = _extents.left;
	mm_right = _extents.right;
	mm_bottom = _extents.bottom;
	mm_top = _extents.top;

	mm_newExtents = (Width != _viewWidth || Height != _viewWidth ||
						left != _extents.left || right !=  _extents.right ||
						top != _extents.top || bottom != _extents.bottom);

	if (mm_newExtents)
	{
		_viewWidth=Width;
		_viewHeight=Height;
		//ResizeBuffers(m_viewWidth, m_viewHeight);
		_aspectRatio = static_cast<double>(Width) / static_cast<double>(Height); 

		double xrange = right - left;
		double yrange = top - bottom;
		_pixelPerProjectionX = _viewWidth/xrange;
		_inversePixelPerProjectionX = 1.0/_pixelPerProjectionX;
		_pixelPerProjectionY = _viewHeight/yrange;
		_inversePixelPerProjectionY = 1.0/_pixelPerProjectionY;
		
		_extents.left = left;
		_extents.right = right - _inversePixelPerProjectionX;
		_extents.bottom = bottom;
		_extents.top = top - _inversePixelPerProjectionY;

		CalculateVisibleExtents(Extent(left,right,bottom,top));
	}
}

// *********************************************************************
//    RestoreExtents()
// *********************************************************************
void CMapView::RestoreExtents()
{
	if (mm_newExtents)
	{
		_viewWidth = mm_viewWidth;
		_viewHeight = mm_viewHeight;
		//ResizeBuffers(m_viewWidth, m_viewHeight);
		_aspectRatio = mm_aspectRatio; 
		_pixelPerProjectionX = mm_pixelPerProjectionX;
		_pixelPerProjectionY = mm_pixelPerProjectionY;
		_inversePixelPerProjectionX = mm_inversePixelPerProjectionX;
		_inversePixelPerProjectionY = mm_inversePixelPerProjectionY;
		_extents.left = mm_left;
		_extents.right = mm_right;
		_extents.bottom = mm_bottom;
		_extents.top = mm_top;
	}
}

// *********************************************************
//		SnapShotCore()
// *********************************************************
// first 4 parameters - extents in map units; last 2 - the size of bitmap to draw this extents on
IDispatch* CMapView::SnapShotCore(double left, double right, double top, double bottom, long Width, long Height, CDC* snapDC,
								  float offsetX, float offsetY, float clipX, float clipY, float clipWidth, float clipHeight)
{
	if (left == right || top == bottom)
	{
		return nullptr;
	}

	// PM dec 2017
	if (Width == 0) Width = 100;
	if (Height == 0) Height = 100;

	bool createDC = (snapDC == nullptr);
	CBitmap * bmp = nullptr;
	
	if (createDC)
	{
		bmp = new CBitmap();
		if (!bmp->CreateDiscardableBitmap(GetDC(), Width, Height))
		{
			delete bmp;
			ErrorMessage(tkFAILED_TO_ALLOCATE_MEMORY);
			return nullptr;
		}
	}

	LockWindow( lmLock );

	SetTempExtents(left, right, top, bottom, Width, Height);

	// create canvas
	CBitmap * oldBMP = nullptr;
	if (createDC)
	{
		snapDC = new CDC();
		snapDC->CreateCompatibleDC(GetDC());
		oldBMP = snapDC->SelectObject(bmp);
	}

	// do the drawing
	ScheduleLayerRedraw();
	_isSnapshot = true;

	if (mm_newExtents) {
		ReloadBuffers();
	}

	ReloadTiles(true,true);		// simply move them to the screen buffer (is performed synchronously)

	CRect rcBounds(0,0,_viewWidth,_viewHeight);
	CRect rcClip(static_cast<int>(clipX), static_cast<int>(clipY), static_cast<int>(clipWidth), static_cast<int>(clipHeight));
	CRect* r = clipWidth != 0.0 && clipHeight != 0.0 ? &rcClip : &rcBounds;
	
	// draws to output canvas directly because of m_isSnapshot parameter
	HandleNewDrawing(snapDC, rcBounds, *r, true, offsetX, offsetY);
	
	// drawing on the output canvas atop the previous drawing, naturally we don't care about flickering here
	DrawDynamic(snapDC, rcBounds, *r, false, offsetX, offsetY);

	ScheduleLayerRedraw();

	_isSnapshot = false;
	IImage * iImg = nullptr;

	if (createDC)
	{
		// create output
		VARIANT_BOOL retval;
		ComHelper::CreateInstance(idImage, reinterpret_cast<IDispatch**>(&iImg));
		iImg->SetImageBitsDC(static_cast<long>(reinterpret_cast<LONG_PTR>(snapDC->m_hDC)), &retval);

		double dx = (right-left)/static_cast<double>(_viewWidth);
		double dy = (top-bottom)/static_cast<double>(_viewHeight);
		iImg->put_dX(dx);
		iImg->put_dY(dy);
		iImg->put_XllCenter(left + dx*.5);
		iImg->put_YllCenter(bottom + dy*.5);
	
		// dispose the canvas
		snapDC->SelectObject(oldBMP);
		bmp->DeleteObject();
		snapDC->DeleteDC();
		delete bmp;
	}

	RestoreExtents();

	if (mm_newExtents)
	{
		ReloadBuffers();
		mm_newExtents = false;
	}

	// restore former list of tiles in the buffer
	ReloadTiles(true,true);

	LockWindow( lmUnlock );
	return iImg;
}

// ********************************************************************
//		DrawBackBuffer()
// ********************************************************************
// Draws the back buffer to the specified DC (probably external)
void CMapView::DrawBackBuffer(int hdc, int imageWidth, int imageHeight)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!hdc)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return;
	}
	
	CDC* dc = CDC::FromHandle(reinterpret_cast<HDC>(static_cast<INT_PTR>(hdc)));
	CRect rect(0,0, imageWidth, imageHeight);
	OnDraw(dc, rect, rect);
}