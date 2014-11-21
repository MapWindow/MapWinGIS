#include "stdafx.h"
#include "Map.h"
#include "Tiles.h"
#include "TilesDrawer.h"
#include "GeoProjection.h"
#include "ShapefileDrawing.h"
#include "ImageDrawing.h"
#include "LabelDrawing.h"
#include "ChartDrawing.h"
#include "Image.h"
#include "Measuring.h"
#include "GeometryHelper.h"

// ***************************************************************
//		OnDraw()
// ***************************************************************
void CMapView::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (_isSizing)
	{
		// An option to clear the control surface with back color before redraw while sizing
		// but to leave the previous bitmap and to paint only the new regions seems nicer
		//m_layerDC->FillSolidRect(rcBounds,m_backColor);
		//ShowRedrawTime(0.0f, "Drawing...");
		//pdc->BitBlt( 0, 0,  rcBounds.Width(), rcBounds.Height(), m_layerDC, 0, 0, SRCCOPY);
		return;	  // redraw is prohibited before the sizing will be finished
	}
	
	//This line is intended to ensure proper function in MSAccess by verifying that the hWnd handle exists
	//before trying to draw. Lailin Chen - 2005/10/17
	if (this->m_hWnd == NULL)
		return;

	// no redraw is allowed when the rubber band is being dragged
	if (_rectTrackerIsActive)
		return;

	// the map is locked
	if (_lockCount > 0)
		return;

	m_drawMutex.Lock();		// TODO: perhaps use lighter CCriticalSection

	if (!_canUseMainBuffer || !_canUseVolatileBuffer || !_canUseLayerBuffer) 
	{
		bool hasMouseMoveData = HasDrawingData(tkDrawingDataAvailable::MeasuringData) || 
								HasDrawingData(tkDrawingDataAvailable::Coordinates) ||
								HasDrawingData(tkDrawingDataAvailable::ZoomBox);

		// if there is no move data, draws to output canvas directly
		this->HandleNewDrawing(pdc, rcBounds, rcInvalid, !hasMouseMoveData);
		
		if (hasMouseMoveData) 
		{
			// the main drawing will be taken from the buffer as it wasn't passed to output canvas yet
			this->DrawDynamic(pdc, rcBounds, rcInvalid, true);
		}
	}
	else 
	{
		// always draw the main buffer, even if there is no measuring data
		this->DrawDynamic(pdc, rcBounds, rcInvalid, true); 
	}
	m_drawMutex.Unlock();
}


// ***************************************************************
//		HandleNewDrawing()
// ***************************************************************
void CMapView::HandleNewDrawing(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, 
								bool drawToOutputCanvas, float offsetX, float offsetY)
{
	long alpha = (255)<<24;
	Gdiplus::Color backColor = Gdiplus::Color(alpha | BGR_TO_RGB(m_backColor));

	Gdiplus::Graphics* gBuffer = NULL;		// for control rendering
	Gdiplus::Graphics* gPrinting = NULL;	// for snapshot drawing
	Gdiplus::Graphics* g = NULL;			// the right one to draw

	// preparing graphics (for snapshot drawing to output canvas directly; 
	// for control rendering main buffer is used)
	if (_isSnapshot)
	{
		gPrinting = Gdiplus::Graphics::FromHDC(pdc->GetSafeHdc());
		gPrinting->TranslateTransform(offsetX, offsetY);
		Gdiplus::RectF clip((Gdiplus::REAL)rcInvalid.left, (Gdiplus::REAL)rcInvalid.top, (Gdiplus::REAL)rcInvalid.Width(), (Gdiplus::REAL)rcInvalid.Height());
		gPrinting->SetClip(clip);
		Gdiplus::Color color(255, 255, 255, 255);
		Gdiplus::SolidBrush brush(color);
		gPrinting->Clear(color);
		g = gPrinting;
	}
	else
	{
		gBuffer = Gdiplus::Graphics::FromImage(_bufferBitmap);
		gBuffer->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		gBuffer->Clear(backColor);
		g = gBuffer;
	}

	// the main thing is to ensure that if new tile is coming it will trigger a new redraw,
	// any tile after this moment will do it, otherwise a newcomer will be already in screen buffer
	_canUseMainBuffer = true;

	RedrawTiles(g, pdc);
	
	DWORD startTick = ::GetTickCount();

	bool layersRedraw = RedrawLayers(g, pdc, rcBounds);

	RedrawVolatileData(g, pdc, rcBounds);

	// passing main buffer to client for custom drawing
	if (m_sendOnDrawBackBuffer && !_isSnapshot)
	{
		HDC hdc = gBuffer->GetHDC();
		this->FireOnDrawBackBuffer((long)hdc);
		gBuffer->ReleaseHDC(hdc);
	}
	
	RedrawTools(g, rcBounds);

	// redraw time and logo
	DWORD endTick = GetTickCount();

	if (layersRedraw) {
		_lastRedrawTime = (float)(endTick - startTick) / 1000.0f;
	}
	this->ShowRedrawTime(g, _lastRedrawTime, layersRedraw);

	CLSID clsid;
	Utility::GetEncoderClsid(L"image/png", &clsid);
	_bufferBitmap->Save(L"D:\\buffer.png", &clsid, NULL);
	_drawingBitmap->Save(L"D:\\drawing.png", &clsid, NULL);
	_tilesBitmap->Save(L"D:\\tiles.png", &clsid, NULL);
	_layerBitmap->Save(L"D:\\layers.png", &clsid, NULL);
	_volatileBitmap->Save(L"D:\\volatile.png", &clsid, NULL);

	// passing the main buffer to the screen if no other drawing will be needed
	if (!_isSnapshot)
	{
		if (drawToOutputCanvas) 
		{
			HDC hdc = pdc->GetSafeHdc();
			Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
			g->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			g->DrawImage(_bufferBitmap, 0.0f, 0.0f);
			g->ReleaseHDC(pdc->GetSafeHdc());
			delete g;
		}
		else {
			// otherwise, there must be mouse move data and the buffer will be drawn there to avoid flickering
		}
		delete gBuffer;
	}
	else
	{
		gPrinting->ReleaseHDC(pdc->GetSafeHdc());
		delete gPrinting;
	}
}

// ***************************************************************
//		RedrawLayers()
// ***************************************************************
bool CMapView::RedrawLayers(Gdiplus::Graphics* g, CDC* dc, const CRect& rcBounds)
{
	bool layersRedraw = false;
	if (HasDrawingData(tkDrawingDataAvailable::LayersData))
	{
		if (_isSnapshot)
		{
			this->DrawLayers(rcBounds, g);
		}
		else
		{
			if (_canUseLayerBuffer)
			{
				bool dragging = _dragging.Operation == DragPanning &&
					(_dragging.Start.x != 0 || _dragging.Start.y != 0 ||
					_dragging.Move.x != 0 || _dragging.Move.y != 0);

				int x = dragging ? _dragging.Move.x - _dragging.Start.x : 0;
				int y = dragging ? _dragging.Move.y - _dragging.Start.y : 0;

				// update from the layer buffer
				g->DrawImage(_layerBitmap, (float)x, (float)y);
			}
			else
			{
				ClearHotTracking();

				layersRedraw = true;
				Gdiplus::Graphics* gLayers = Gdiplus::Graphics::FromImage(_layerBitmap);
				gLayers->Clear(Gdiplus::Color::Transparent);
				gLayers->SetCompositingMode(Gdiplus::CompositingModeSourceOver);

				bool useRotation = false;	// not implemented
				if (useRotation) {
					this->DrawLayersRotated(dc, gLayers, rcBounds);
				}
				else {
					this->DrawLayers(rcBounds, gLayers);
				}

				// passing layer buffer to the main buffer
				g->DrawImage(_layerBitmap, 0.0f, 0.0f);
			}
		}
	}
	_canUseLayerBuffer = TRUE;
	return layersRedraw;
}

// ***************************************************************
//		DrawTiles()
// ***************************************************************
void CMapView::RedrawTiles(Gdiplus::Graphics* g, CDC* dc)
{
	if (HasDrawingData(tkDrawingDataAvailable::TilesData))
	{
		CTiles* tiles = (CTiles*)_tiles;
		if (_isSnapshot)
		{
			if (tiles->TilesAreInScreenBuffer((void*)this))
			{
				tiles->MarkUndrawn();
				DrawTiles(g);
				tiles->MarkUndrawn();
			}
		}
		else
		{
			bool zoomingAnimation = HasDrawingData(tkDrawingDataAvailable::ZoomingAnimation);
			UpdateTileBuffer(dc, zoomingAnimation);
			g->DrawImage(_tilesBitmap, 0.0f, 0.0f);
		}
	}
}

// ***************************************************************
//		DrawVolatileData()
// ***************************************************************
void CMapView::RedrawVolatileData(Gdiplus::Graphics* g, CDC* dc, const CRect& rcBounds)
{
	bool hasVolatile = HasVolatileShapefiles();

	if (!hasVolatile && !HasDrawLists()) return;

	if (_isSnapshot) 
	{
		if (hasVolatile)
			this->DrawLayers(rcBounds, g, false);
		this->DrawLists(rcBounds, g, dlSpatiallyReferencedList);
		this->DrawLists(rcBounds, g, dlScreenReferencedList);
		return;
	}

	if (_canUseVolatileBuffer) 
	{
		g->DrawImage(_volatileBitmap, 0.0f, 0.0f);
	}
	else {
		// drawing layers
		Gdiplus::Graphics* gDrawing = Gdiplus::Graphics::FromImage(_volatileBitmap);
		gDrawing->Clear(Gdiplus::Color::Transparent);

		// rendering of volatile shapefiles
		if (HasVolatileShapefiles())
			this->DrawLayers(rcBounds, gDrawing, false);

		// fire external drawing
		HDC hdc = g->GetHDC();
		VARIANT_BOOL retVal = VARIANT_FALSE;
		this->FireBeforeDrawing((long)hdc, rcBounds.left, rcBounds.right, rcBounds.top, rcBounds.bottom, &retVal);
		g->ReleaseHDC(hdc);

		// temp objects
		this->DrawLists(rcBounds, gDrawing, dlSpatiallyReferencedList);
		this->DrawLists(rcBounds, gDrawing, dlScreenReferencedList);

		// fire external drawing code
		hdc = g->GetHDC();
		this->FireAfterDrawing((long)hdc, rcBounds.left, rcBounds.right, rcBounds.top, rcBounds.bottom, &retVal);
		g->ReleaseHDC(hdc);

		// passing layers to the main buffer
		g->DrawImage(_volatileBitmap, 0.0f, 0.0f);
		delete gDrawing;
		_canUseVolatileBuffer = TRUE;
	}
}

// ***************************************************************
//		RedrawTools()
// ***************************************************************
void CMapView::RedrawTools(Gdiplus::Graphics* g, const CRect& rcBounds)
{
	DrawShapeEditor(g, false);

	DrawScaleBar(g);

	DrawZoombar(g);

	// distance measuring or persistent measuring
	if (HasDrawingData(tkDrawingDataAvailable::MeasuringData))
	{
		GetMeasuringBase()->DrawData(g, false, DragNone);
	}

	// hot tracking
	if (HasDrawingData(tkDrawingDataAvailable::HotTracking))
	{
		CShapefileDrawer drawer(g, &_extents, _pixelPerProjectionX, _pixelPerProjectionY, &_collisionList,
			this->GetCurrentScale(), true);
		drawer.Draw(rcBounds, _hotTracking.Shapefile);
	}
}

// ***************************************************************
//	UpdateTileBuffer
// ***************************************************************
void CMapView::UpdateTileBuffer( CDC* dc, bool zoomingAnimation )
{
	CTiles* tiles = (CTiles*)_tiles;
	Gdiplus::Graphics* gTiles = Gdiplus::Graphics::FromImage(_tilesBitmap);
	int tileProvider = GetTileProvider();
	
	bool initialization = !_tileBuffer.Initialized;
	if (!_tileBuffer.Initialized)
	{
		// it's the first tile for current extents, we need to initialize the buffer
		bool canReuseBuffer = /*ForceDiscreteZoom() &&*/
			GetTileProvider() == _tileBuffer.Provider &&			   
			_currentZoom != _tileBuffer.Zoom && abs(_currentZoom - _tileBuffer.Zoom) <= 4;		// for larger difference it's not practical

		if (canReuseBuffer && (_zoomAnimation || _reuseTileBuffer))
		{
			// reuse existing buffer
			Gdiplus::Graphics* gTemp = Gdiplus::Graphics::FromImage(_tempBitmap);
			
			Extent match;
			if (_extents.getIntersection(_tileBuffer.Extents, match))
			{
				Gdiplus::RectF source, target;
				DrawZoomingAnimation(match, gTemp, dc, source, target, zoomingAnimation);

				if (_reuseTileBuffer)
				{
					Gdiplus::ImageAttributes attr;
					gTemp->Clear(Gdiplus::Color::Transparent);
					gTemp->Flush();
					gTemp->DrawImage(_tilesBitmap, target, source.X, source.Y, source.Width, source.Height, Gdiplus::Unit::UnitPixel, &attr);
					gTemp->Flush();
					gTiles->Clear(Gdiplus::Color::Transparent);
					gTiles->DrawImage(_tempBitmap, 0, 0 );
					gTiles->Flush();
					_tileBuffer.Initialized = true;
				}
				delete gTemp;
			}
		}
		
		if (!_tileBuffer.Initialized) 
		{
			gTiles->Clear(Gdiplus::Color::Transparent);
		}

		// save extents to reuse the buffer
		_tileBuffer.Extents = _extents;
		_tileBuffer.Zoom = _currentZoom;
		_tileBuffer.Provider = tileProvider;
		_tileBuffer.Initialized = true;
	}

	// drawing new tiles
	if (initialization || tiles->UndrawnTilesExist())
		DrawTiles(gTiles);		
}

// ***************************************************************
//	DrawZoomingAnimation
// ***************************************************************
void CMapView::DrawZoomingAnimation( Extent match, Gdiplus::Graphics* gTemp, CDC* dc, Gdiplus::RectF& source, Gdiplus::RectF& target, bool zoomingAnimation )
{
	// target rectangle (current screen buffer)	
	double tx = (match.left - _extents.left)/_extents.Width();
	double tx2 = 1.0f - (_extents.right - match.right)/_extents.Width();
	double ty = (_extents.top - match.top)/_extents.Height();
	double ty2 = 1.0f - (match.bottom - _extents.bottom)/_extents.Height();

	target.X = (float)(tx * _viewWidth);
	target.Y = (float)(ty * _viewHeight);
	target.Width = (float)((tx2 - tx) * _viewWidth);
	target.Height = (float)((ty2 - ty) * _viewHeight);

	// source rectangle (cached tile buffer)	
	Extent buffer = _tileBuffer.Extents;
	double sx = (match.left - buffer.left)/buffer.Width();
	double sx2 =  1.0f - (buffer.right - match.right)/buffer.Width();
	double sy = (buffer.top - match.top)/buffer.Height();
	double sy2 = 1.0f - (match.bottom - buffer.bottom)/buffer.Height();

	source.X = (float)(sx * _tilesBitmap->GetWidth());
	source.Y = (float)(sy * _tilesBitmap->GetHeight());
	source.Width = (float)((sx2 - sx) * _tilesBitmap->GetWidth());
	source.Height = (float)((sy2 - sy) * _tilesBitmap->GetHeight());

	double x, x2, y, y2;

	// -----------------------------------------------
	// zooming animation
	// -----------------------------------------------
	if (zoomingAnimation)
	{
		HDC hdc = dc->GetSafeHdc();
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
		g->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
		Gdiplus::Color backColor = Gdiplus::Color((255)<<24 | BGR_TO_RGB(m_backColor));
		Gdiplus::ImageAttributes attr;
		gTemp->Clear(backColor);

		double steps = ((_viewHeight * _viewWidth > 1e6) ? 4 : 5) * abs(_currentZoom - _tileBuffer.Zoom);
		DWORD lastTime, time;
		DWORD redrawDelay = 40;

		bool zoomingIn = _currentZoom > _tileBuffer.Zoom;
		if (zoomingIn)
		{
			// portion of buffer is gradually scaled to the whole screen
			for(int i = 1; i <= steps; i++)
			{
				x = i / steps * sx ;
				x2 = sx2 + (1 - sx2) * (1 - i/steps);
				y = i / steps * sy;
				y2 = sy2 + (1 - sy2) * (1 - i/steps);

				//Debug::WriteLine("Zoom in animation: x=%f; x2=%f; y=%f; y2=%f", x, x2, y, y2 );

				x *= _tilesBitmap->GetWidth();
				x2 *= _tilesBitmap->GetWidth();
				y *= _tilesBitmap->GetHeight();
				y2 *= _tilesBitmap->GetHeight();

				lastTime = GetTickCount();

				// render to temp bitmap first as we need background color
				gTemp->DrawImage(_tilesBitmap, target, (float)x, (float)y, (float)(x2 - x), (float)(y2 - y), Gdiplus::Unit::UnitPixel, &attr);
				g->DrawImage(_tempBitmap, 0, 0 );

				time = GetTickCount();
				if (time - lastTime < redrawDelay )
					Sleep(redrawDelay - time + lastTime);
			}
		}
		else
		{
			// the whole buffer is gradually scale to the portion of the screen
			Gdiplus::Color clr = Gdiplus::Color(backColor);
			Gdiplus::SolidBrush brush(clr);
			Gdiplus::RectF _lastTargetRect(target.X, target.Y, target.Width, target.Height);
			Gdiplus::RectF tRect;

			for(int i = 1; i <= steps ; i++)
			{
				x = i / steps * tx;
				x2 = tx2 + (1 - tx2) * (1 - i/steps);
				y = i / steps * ty;
				y2 = ty2 + (1 - ty2) * (1 - i/steps);

				Debug::WriteLine("Zoom out animation: x=%f; x2=%f; y=%f; y2=%f", x, x2, y, y2 );

				tRect.X = (float)(x * _viewWidth);
				tRect.Width = (float)(x2 * _viewWidth - tRect.X);
				tRect.Y = (float)(y * _viewHeight);
				tRect.Height =(float)(y2 * _viewHeight - tRect.Y);

				lastTime = GetTickCount();

				gTemp->FillRectangle(&brush, _lastTargetRect);
				gTemp->DrawImage(_tilesBitmap, tRect, source.X, source.Y, source.Width, source.Height, Gdiplus::Unit::UnitPixel, &attr);
				g->DrawImage(_tempBitmap, 0, 0 );
				
				time = GetTickCount();
				if (time - lastTime < redrawDelay )
					Sleep(redrawDelay - time + lastTime);

				_lastTargetRect.X = tRect.X;	
				_lastTargetRect.Y = tRect.Y;
				_lastTargetRect.Width = tRect.Width;
				_lastTargetRect.Height = tRect.Height;
			}
		}

		g->ReleaseHDC(dc->GetSafeHdc());
		delete g;

		Debug::WriteLine("End animation");
	}
}

// ***************************************************************
//		DrawTiles()
// ***************************************************************
void CMapView::DrawTiles(Gdiplus::Graphics* g) 
{
	int minZoom;
	if ( GetTileMismatchMinZoom(minZoom) )
	{
		if (_currentZoom < minZoom) return;
	}
	
	CTilesDrawer drawer(g, &this->_extents, _pixelPerProjectionX, _pixelPerProjectionY);
	if (_transformationMode == tmDoTransformation)
	{
		drawer.m_transfomation = GetWgs84ToMapTransform(); 
	}
	drawer.DrawTiles(_tiles, this->PixelsPerMapUnit(), GetMapProjection(), ((CTiles*)_tiles)->m_provider->Projection, _isSnapshot, _projectionChangeCount);
}

#pragma region Draw layers

// ****************************************************************
//		DrawLayers()
// ****************************************************************
void CMapView::DrawLayers(const CRect & rcBounds, Gdiplus::Graphics* graphics, bool layerBuffer)
{
	if (_lockCount > 0 && !_isSnapshot)
	{
		return;
	}
	
	HCURSOR oldCursor;
	if (layerBuffer) 
		oldCursor = this->SetWaitCursor();

	// clear extents of drawn labels and charts
	this->ClearLabelFrames();
	
	register int i;
	long startcondition = 0;
	long endcondition = _activeLayers.size();

	// ---------------------------------------------------
	//	perhaps there is nothing to draw
	// ---------------------------------------------------
	if (endcondition == 0)
	{
	     if (layerBuffer && oldCursor != NULL) 
			 ::SetCursor(oldCursor);
		 return;
	}
	
	// ------------------------------------------------------------------
	//	Check whether some layers are completely concealed by images 
	//	no need to draw them then
	// ------------------------------------------------------------------
	bool * isConcealed = NULL;
	isConcealed = new bool[endcondition];
	memset(isConcealed,0,endcondition*sizeof(bool));

	double scale = this->GetCurrentScale();
	int zoom;
	_tiles->get_CurrentZoom(&zoom);

	if (layerBuffer)
		CheckForConcealedImages(isConcealed, startcondition, endcondition, scale, zoom);

	// do we have shapefiles with hot tracking? check it once here and don't check on mouse move
	//_hasHotTracking = HasHotTracking();

	// ---------------------------------------------------
	//	Drawing grouped images
	// ---------------------------------------------------
	if (layerBuffer)
		DrawImageGroups();
	
	// ---------------------------------------------------
	//	Prepare for drawing
	// ---------------------------------------------------
	double currentScale = this->GetCurrentScale();

	bool useCommonCollisionListForCharts = true;
	bool useCommonCollisionListForLabels = true;

	// collision avoidance
	_collisionList.Clear();
	CCollisionList collisionListLabels;
	CCollisionList collisionListCharts;
	
	CCollisionList* chosenListLabels = NULL;
	CCollisionList* chosenListCharts = NULL;
	
	chosenListLabels = useCommonCollisionListForLabels?(&_collisionList):(&collisionListLabels);
	chosenListCharts = useCommonCollisionListForCharts?(&_collisionList):(&collisionListCharts);

	// initializing classes for drawing
	bool forceGdiplus = this->_rotateAngle != 0.0f || _isSnapshot;
	
	CShapefileDrawer sfDrawer(graphics, &_extents, _pixelPerProjectionX, _pixelPerProjectionY, &_collisionList, this->GetCurrentScale(), forceGdiplus);
	CImageDrawer imgDrawer(graphics, &_extents, _pixelPerProjectionX, _pixelPerProjectionY, _viewWidth, _viewHeight);
	CLabelDrawer lblDrawer(graphics, &_extents, _pixelPerProjectionX, _pixelPerProjectionY, currentScale, chosenListLabels, _rotateAngle, _isSnapshot);
	CChartDrawer chartDrawer(graphics, &_extents, _pixelPerProjectionX, _pixelPerProjectionY, currentScale, chosenListCharts);

	// ---------------------------------------------------
	//	Run drawing
	// ---------------------------------------------------
	int shapeCount = 0;
	for(int i = startcondition; i < endcondition; i++)
	{
		if( isConcealed[i] == false )
		{
			Layer * l = _allLayers[_activeLayers[i]];
			if( l != NULL )
			{	
				bool visible = l->IsVisible(scale, zoom);
				l->wasRendered = visible;

				if (visible)
				{
					if (l->IsDynamicOgrLayer())
					{
						l->UpdateShapefile();
					}
		
					if(l->IsImage())
					{
						if (!layerBuffer) continue;

						if(l->object == NULL ) continue;
						IImage * iimg = NULL;
						if (!l->QueryImage(&iimg)) continue;
						
						CImageClass* img = (CImageClass*)iimg;
						
						if (_canUseImageGrouping && img->m_groupID != -1)
						{
							// this is grouped image, if this is the first image of group, we shall draw the whole group
							if (!(*_imageGroups)[img->m_groupID]->wasDrawn)
							{
								this->DrawImageGroups(rcBounds, graphics, img->m_groupID);
								(*_imageGroups)[img->m_groupID]->wasDrawn = true;
							}
						}
						else
						{
							bool saveBitmap = true;
							
							if (saveBitmap)
							{
								CImageClass* img = (CImageClass*)iimg;
								ScreenBitmap* bmp = img->_screenBitmap;
								bool wasDrawn = false;

								// in case we have saved bitmap and map position is the same we shall draw it
								if (bmp)
								{
									if (bmp->extents == _extents &&
										bmp->pixelPerProjectionX == _pixelPerProjectionX &&
										bmp->pixelPerProjectionY == _pixelPerProjectionY &&
										bmp->viewWidth == _viewWidth &&
										bmp->viewHeight == _viewHeight && !((CImageClass*)iimg)->_imageChanged )
									{
										graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
										
										// TODO: choose interpolation mode more precisely
										// TODO: set image attributes
										
										graphics->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);	
										graphics->SetSmoothingMode(Gdiplus::SmoothingModeDefault);
										graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
										graphics->DrawImage(bmp->bitmap, Gdiplus::REAL(bmp->left), Gdiplus::REAL(bmp->top));
										wasDrawn = true;
									}
								}
								
								if (!wasDrawn)
								{
									long width, height;
									iimg->get_OriginalWidth(&width);
									iimg->get_OriginalHeight(&height);

									if ((width == 256 && height == 256) || _isSnapshot)
									{
										// it's tiles, I don't want to cache bitmap here to avoid seams
										// the same thing with Snapshot calls
										bmp = imgDrawer.DrawImage(rcBounds, iimg);
									}
									else
									{
										// image hasn't been saved so far
										bmp = imgDrawer.DrawImage(rcBounds, iimg, true);
										
										if (img->_screenBitmap)
										{
											delete img->_screenBitmap;
											img->_screenBitmap = NULL;
										}

										img->_screenBitmap = bmp;
									}
								}
							}
							else
							{
								imgDrawer.DrawImage(rcBounds, iimg);
							}
						}

						img->Release();
						
						// drawing labels for images
						ILabels* labels = l->get_Labels();
						if(labels != NULL)
						{
							tkVerticalPosition vertPos;
							labels->get_VerticalPosition(&vertPos);
							if (vertPos == vpAboveParentLayer)		
							{
								lblDrawer.DrawLabels(labels);
							}
							labels->Release();
							labels = NULL;
						}
					}
					else if( l->IsShapefile() )
					{
						// grab extents from shapefile in case they have changed
						this->AdjustLayerExtents(_activeLayers[i]);

						if (l->extents.right < _extents.left || l->extents.left > _extents.right ||
							l->extents.top < _extents.bottom || l->extents.bottom > _extents.top) continue;
					
						if( l->object == NULL )
						{
							Debug::WriteError("Layer object is missing");
							continue;
						}
						
						IShapefile* sf = NULL;
						if (l->QueryShapefile(&sf))
						{
							VARIANT_BOOL isVolatile;
							sf->get_Volatile(&isVolatile);
							if ((isVolatile ? true : false) != layerBuffer)
							{
								sfDrawer.Draw(rcBounds, sf);

								// for old modes we shall mark all the shapes of shapefile as visible as no visibility expressions were analyzed
								if (_shapeDrawingMethod != dmNewSymbology)
								{
									std::vector<ShapeData*>* shapeData = ((CShapefile*)sf)->get_ShapeVector();
									if (shapeData)
									{
										for (size_t n = 0; n < shapeData->size(); n++)
										{
											(*shapeData)[n]->isVisible = true;
										}
									}
								}

								// labels
								ILabels* labels = l->get_Labels();
								if(labels != NULL)
								{
									tkVerticalPosition vertPos;
									labels->get_VerticalPosition(&vertPos);
									if (vertPos == vpAboveParentLayer)		
									{
										lblDrawer.DrawLabels(labels);
									}
									labels->Release();
									labels = NULL;
								}

								// charts: available for all modes
								ICharts* charts = NULL;
								sf->get_Charts(&charts);
								if (charts)
								{
									tkVerticalPosition vertPosition;
									charts->get_VerticalPosition(&vertPosition);
									if (vertPosition == vpAboveParentLayer )
									{
										chartDrawer.DrawCharts(sf);
									}
									charts->Release();
									charts = NULL;
								}
							}
							sf->Release();
						}
					}
				}
			}
		}
	}

	shapeCount = sfDrawer.GetShapeCount();

	if (layerBuffer)
		_shapeCountInView = shapeCount;

	if (!layerBuffer && shapeCount > _shapeCountInView)
		_shapeCountInView = shapeCount;

	// -------------------------------------------------
	//	Drawing labels and charts above the layers
	// -------------------------------------------------
	for (i = 0; i < (int)_activeLayers.size(); i++)
	{
		Layer * l = _allLayers[_activeLayers[i]];
		if( l != NULL )
		{	
			if (l->IsVisible(scale, zoom))
			{
				IShapefile* sf = NULL;
				if (l->QueryShapefile(&sf))
				{
					VARIANT_BOOL isVolatile;
					sf->get_Volatile(&isVolatile);
					sf->Release();
					if ((isVolatile ? true : false) == layerBuffer)
						continue;
				}

				//  labels: for the new modes only
				if (_shapeDrawingMethod == dmNewWithLabels || _shapeDrawingMethod == dmNewSymbology || 
					l->IsImage() || FORCE_NEW_LABELS)
				{
					ILabels* labels = l->get_Labels();
					if ( labels )
					{
						tkVerticalPosition vertPos;
						labels->get_VerticalPosition(&vertPos);
						if (vertPos == vpAboveAllLayers)
						{
							lblDrawer.DrawLabels(labels);
						}
						labels->Release(); 
						labels = NULL;
					}
				}
				
				// charts: for all modes
				if (l->QueryShapefile(&sf))
				{
					ICharts* charts = NULL;
					sf->get_Charts(&charts);
					if (charts)
					{
						tkVerticalPosition vertPosition;
						charts->get_VerticalPosition(&vertPosition);
						if (vertPosition == vpAboveAllLayers )
						{
							chartDrawer.DrawCharts(sf);
						}
						charts->Release();
						charts = NULL;
					}
					
					sf->Release();
					sf = NULL;
				}
			}
		}
	}
	
   if (layerBuffer && oldCursor != NULL)
      ::SetCursor(oldCursor);

   if (layerBuffer)
		delete[] isConcealed;
}

// ****************************************************************
//		HaveDataLayersWithinView()
// ****************************************************************
bool CMapView::HaveDataLayersWithinView()
{
	double scale = this->GetCurrentScale();
	int zoom = _currentZoom;
	
	for(size_t i = 0; i < _activeLayers.size(); i++)
	{
		Layer * l = _allLayers[_activeLayers[i]];
		if( l != NULL )
		{	
			if (l->IsVisible(scale, zoom))
			{
				Extent result;
				if (l->extents.getIntersection(_extents, result))
					return true;
			}
		}
	}
	return false;	
}

// ****************************************************************
//		DrawImageGroups()
// ****************************************************************
void CMapView::DrawImageGroups()
{
	if ( _canUseImageGrouping )
	{
		std::vector<ImageGroup*>* newGroups = new std::vector<ImageGroup*>;

		// building groups
		this->BuildImageGroups(*newGroups);
		
		// comparing them with the old list
		if (_imageGroups != NULL)
		{
			if (this->ImageGroupsAreEqual(*_imageGroups, *newGroups))
			{
				// groups are the same so we can continue to use them
				for (size_t i = 0; i < newGroups->size(); i++)
				{
					delete (*newGroups)[i];
				}
				newGroups->clear();
				delete newGroups;
				newGroups = NULL;
			}
			else
			{
				// groups has changed, swapping pointers
				if (_imageGroups != NULL)
				{
					for (size_t i = 0; i < _imageGroups->size(); i++)
					{
						delete (*_imageGroups)[i];
					}
					
					_imageGroups->clear();
					delete _imageGroups;
					_imageGroups = NULL;
				}
				_imageGroups = newGroups;
			}
		}
		else
		{
			_imageGroups = newGroups;
		}
		
		// mark all images as undrawn
		for (size_t i = 0; i < _imageGroups->size(); i++)
		{
			(*_imageGroups)[i]->wasDrawn = false;
		}
	}
}

// *********************************************************
//		ResizeBuffers()
// *********************************************************
void CMapView::ResizeBuffers(int cx, int cy)
{
	if (_bufferBitmap)
	{
		delete _bufferBitmap;
		_bufferBitmap = NULL;
	}
	_bufferBitmap = new Gdiplus::Bitmap(cx, cy);

	if (_tilesBitmap)
	{
		delete _tilesBitmap;
		_tilesBitmap = NULL;
	}
	_tilesBitmap = new Gdiplus::Bitmap(cx, cy);
	_tileBuffer.Provider = tkTileProvider::ProviderNone;		// buffer can't be reused

	if (_tempBitmap)
	{
		delete _tempBitmap;
		_tempBitmap = NULL;
	}
	_tempBitmap = new Gdiplus::Bitmap(cx, cy);

	if (_drawingBitmap)
	{
		delete _drawingBitmap;
		_drawingBitmap = NULL;
	}
	_drawingBitmap = new Gdiplus::Bitmap(cx, cy);

	if (_volatileBitmap)
	{
		delete _volatileBitmap;
		_volatileBitmap = NULL;
	}
	_volatileBitmap = new Gdiplus::Bitmap(cx, cy);

	if (_layerBitmap)
	{
		delete _layerBitmap;
		_layerBitmap = NULL;
	}
	_layerBitmap = new Gdiplus::Bitmap(cx, cy);
}
#pragma endregion

#pragma region Utilities
// ***************************************************************
//		HasDrawingData()
// ***************************************************************
bool CMapView::HasDrawingData(tkDrawingDataAvailable type) 
{
	switch(type) 
	{
		case MovingShapes:
			{
				return _dragging.Operation == DragMoveShapes || _dragging.Operation == DragRotateShapes;
			}
		case ActShape:	
			{
				VARIANT_BOOL isEmpty;
				_shapeEditor->get_IsEmpty(&isEmpty);
				return !isEmpty ? true : false;
			}	
		case ZoomBox:
			{
				return _dragging.Operation == DragZoombox || _dragging.Operation == DragSelectionBox;
			}
		case ShapeEditing:
			{
				if (!GetEditorBase()->GetCreationMode())
					return false;
				return GetEditorBase()->GetPointCount() > 0;
			}
		case tkDrawingDataAvailable::LayersData:	
			{
				return _activeLayers.size() > 0;
			}
		case tkDrawingDataAvailable::MeasuringData:
			{
				return GetMeasuringBase()->NeedsDrawing() || (m_cursorMode == cmMeasure && GetMeasuringBase()->GetPointCount() > 0);
			}
		case tkDrawingDataAvailable::Coordinates:
			{
				return _showCoordinates != cdmNone && !_isSnapshot;
			}
		case tkDrawingDataAvailable::TilesData:
			{
				// if projection isn't defined there is no way to display tiles
				VARIANT_BOOL tilesVisible;
				_tiles->get_Visible(&tilesVisible);
				return tilesVisible && _transformationMode != tmNotDefined;
			}
		case tkDrawingDataAvailable::HotTracking:
			{
				if (_hotTracking.Shapefile && !_isSnapshot) {
					long numShapes;
					_hotTracking.Shapefile->get_NumShapes(&numShapes);
					return numShapes > 0;
				}
				return false;
			}
		case tkDrawingDataAvailable::ZoomingAnimation:
		case tkDrawingDataAvailable::PanningInertia:
			{
				VARIANT_BOOL visible;
				_tiles->get_Visible(&visible);
				if (!visible) {
					return false;
				}
				else
				{
					tkCustomState state = (type == ZoomingAnimation) ? _zoomAnimation : _panningInertia;
					if (state == csAuto) {
						return !HaveDataLayersWithinView();
					}
					else {
						return state == csTrue;  
					}
				}
			}
			break;
		default:
			return false;
	}
}

// ****************************************************************
//		HasImages()
// ****************************************************************
bool CMapView::HasImages() 
{
	for(long i = _activeLayers.size() - 1; i >= 0; i-- )
	{
		Layer * l = _allLayers[_activeLayers[i]];
		if( IS_VALID_PTR(l) )
		{
			if( l->IsImage())
				return true;
		}
	}
	return false;
}

// ****************************************************************
//		HasHotTracking()
// ****************************************************************
bool CMapView::HasHotTracking() 
{
	for(long i = _activeLayers.size() - 1; i >= 0; i-- )
		if (CheckLayer(slctHotTracking, _activeLayers[i]))
			return true;
	return false;
}

// ****************************************************************
//		HasVolatileShapefiles()
// ****************************************************************
bool CMapView::HasVolatileShapefiles() 
{
	for(long i = _activeLayers.size() - 1; i >= 0; i-- )
	{
		Layer * l = _allLayers[_activeLayers[i]];
		if( IS_VALID_PTR(l) )
		{
			if( l->IsShapefile())
			{
				IShapefile* sf = NULL;
				l->QueryShapefile(&sf);
				if (sf) {
					VARIANT_BOOL vb;
					sf->get_Volatile(&vb);
					sf->Release();
					if (vb) return true;
				}
			}
		}
	}
	return false;
}

// ****************************************************************
//		CheckForConcealedImages()
// ****************************************************************
void CMapView::CheckForConcealedImages(bool* isConcealed, long& startcondition, long& endcondition, double scale, int zoom) 
{
	if( HasImages() && !_canUseImageGrouping )
	{
		register int i, j;

		for( i = endcondition - 1; i >= 0; i-- )
		{
			Layer * l = _allLayers[_activeLayers[i]];
			if( IS_VALID_PTR(l) )
			{
				if( l->IsImage() && l->IsVisible(scale, zoom)) 
				{
					IImage * iimg = NULL;
					if (!l->QueryImage(&iimg)) continue;
					
					this->AdjustLayerExtents(i);

					VARIANT_BOOL useTransparencyColor;
					iimg->get_UseTransparencyColor(&useTransparencyColor);
					iimg->Release();
					iimg = NULL;

					if( useTransparencyColor == FALSE )
					{
						//Check if this is the end condition layer
						if( l->extents.left <= _extents.left && 
							l->extents.right >= _extents.right &&
							l->extents.bottom <= _extents.bottom && 
							l->extents.top >= _extents.top )
						{	
							startcondition = i;
							break;
						}
						//Check if this layer conceals any others
						else if( isConcealed[i] == false )
						{
							for( j = i - 1; j >= 0; j-- )
							{
								Layer * l2 = _allLayers[_activeLayers[j]];
								if( IS_VALID_PTR(l2) )
								{
									if( l->extents.left <= l2->extents.left && 
										l->extents.right >= l2->extents.right &&
										l->extents.bottom <= l2->extents.bottom && 
										l->extents.top >= l2->extents.top )
									{
										isConcealed[j] = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
#pragma endregion

#pragma region Rotation (unused)
// ***************************************************************
//		DrawLayersRotated()
// ***************************************************************
void CMapView::DrawLayersRotated(CDC* pdc, Gdiplus::Graphics* gLayers, const CRect& rcBounds)
{
	// TODO: implement rotation using GDI+
	return;

	HDC hdcLayers = gLayers->GetHDC();
	CDC* dcLayers = CDC::FromHandle(hdcLayers);
	if (dcLayers)
	{
		CDC     *tmpBackbuffer = new CDC();
		CRect   tmpRcBounds = new CRect();
		Extent  tmpExtent, saveExtent;       
		long    save_viewWidth, save_viewHeight;

		if (_rotate == NULL)
		  _rotate = new Rotate();

		tmpBackbuffer->CreateCompatibleDC(dcLayers);
		_rotate->setSize(rcBounds);
		_rotate->setupRotateBackbuffer(tmpBackbuffer->m_hDC, pdc->m_hDC, m_backColor);

		save_viewWidth = _viewWidth;
		save_viewHeight = _viewHeight;
		_viewWidth = _rotate->rotatedWidth;
		_viewHeight = _rotate->rotatedHeight;
		saveExtent = _extents;
		tmpExtent = _extents;
		tmpExtent.right += (_rotate->xAxisDiff * _inversePixelPerProjectionX);
		tmpExtent.bottom -= (_rotate->yAxisDiff * _inversePixelPerProjectionY);
		tmpExtent.left -= (_rotate->xAxisDiff * _inversePixelPerProjectionX);
		tmpExtent.top += (_rotate->yAxisDiff * _inversePixelPerProjectionY);
		_extents = tmpExtent;

		// draw the Map
		//this->DrawLayers(rcBounds,tmpBackbuffer, gLayers);
		
		// Cleanup
		_extents = saveExtent;
		_viewWidth = save_viewWidth;
		_viewHeight = save_viewHeight;
		_rotate->resetWorldTransform(tmpBackbuffer->m_hDC);
		dcLayers->BitBlt(0,0,rcBounds.Width(),rcBounds.Height(), tmpBackbuffer, 0, 0, SRCCOPY);
		_rotate->cleanupRotation(tmpBackbuffer->m_hDC);
		tmpBackbuffer->DeleteDC();
	}
	gLayers->ReleaseHDC(hdcLayers);
}
#pragma endregion

// ******************************************************************
//		InitMapRotation()
// ******************************************************************
//	Changes the necessary variables to perform drawing with rotation
void InitMapRotation()
{
	// TODO: implement
}

// ******************************************************************
//		CloseMapRotation()
// ******************************************************************
// Restores the variables back after the drawing
void CloseMapRotation()
{
	// TODO: implement
}





