#pragma region Include
#include "stdafx.h"
#include "MapWinGis.h"
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
#include "GeometryOperations.h"
#include "Utils.h"

#pragma endregion

#pragma region OnDraw
// ***************************************************************
//		OnDraw()
// ***************************************************************
void CMapView::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (m_isSizing)
	{
		// An option to clear the control surface with back color before redraw while sizing
		// but to leave the the previous bitmap and to paint only the new regions seems nicer
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
	if (m_rectTrackerIsActive)
		return;

	bool dragging =  (m_bitbltClickDown.x != 0 || m_bitbltClickDown.y != 0 ||
					  m_bitbltClickMove.x != 0 || m_bitbltClickMove.y != 0);
	
	// for panning restore everything from the main buffer, no need to update anything until user releases the button
	if (dragging)
	{
		// background for the new area
		pdc->FillSolidRect(0, 0, m_bitbltClickMove.x - m_bitbltClickDown.x, rcBounds.Height(), m_backColor);
		pdc->FillSolidRect(rcBounds.Width() - ( m_bitbltClickDown.x - m_bitbltClickMove.x ), 0, rcBounds.Width(), rcBounds.Height(), m_backColor);
		pdc->FillSolidRect(0, 0, rcBounds.Width(), m_bitbltClickMove.y - m_bitbltClickDown.y, m_backColor);
		pdc->FillSolidRect(0, rcBounds.Height() - ( m_bitbltClickDown.y - m_bitbltClickMove.y ), rcBounds.Width(), rcBounds.Height(), m_backColor);
		
		// passing main buffer to the screen
		int x = m_bitbltClickMove.x - m_bitbltClickDown.x;
		int y = m_bitbltClickMove.y - m_bitbltClickDown.y;

		Gdiplus::Graphics* gBuffer = Gdiplus::Graphics::FromImage(m_bufferBitmap);
		
		// blit to the screen
		HDC hdc = pdc->GetSafeHdc();
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
		g->DrawImage(m_bufferBitmap, (Gdiplus::REAL)x, (Gdiplus::REAL)y);
		g->ReleaseHDC(hdc);
		delete g;
		delete gBuffer;
	}
	else
	{
		// the map is locked
		if (m_lockCount > 0)
			return;
		
		if ( m_drawMouseMoves && m_canbitblt) {	 // there was request to update measuring and no changes to other layers
			this->DrawMouseMoves(pdc, rcBounds, rcInvalid);   // drawing on the top of buffer
			m_drawMouseMoves = false;
		}
		else {
			this->HandleNewDrawing(pdc, rcBounds, rcInvalid);
			
			if (m_cursorMode == cmMeasure || ((CMeasuring*)m_measuring)->persistent)
			{
				// let it blink on each loading of tile
				this->DrawMouseMoves(pdc, rcBounds, rcInvalid);
			}
		}
	}
}

// ***************************************************************
//		OnDraw()
// ***************************************************************
void CMapView::HandleNewDrawing(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, float offsetX, float offsetY)
{
	#ifdef _DEBUG	
	CLSID pngClsid;
	Utility::GetEncoderClsid(L"image/png", &pngClsid);		
	#endif
	
	bool layersRedraw = !m_canbitblt;
	bool drawingRedraw = true;		// currently always on; can be time consuming when tiles are enabled 
	
	// background
	long alpha = (255)<<24;
	Gdiplus::Color backColor = Gdiplus::Color(alpha | BGR_TO_RGB(m_backColor));

	Gdiplus::Graphics* gBuffer = NULL;
	Gdiplus::Graphics* gPrinting = NULL;
	if (m_isSnapshot)
	{
		//SetBkMode (hdc, TRANSPARENT);
		//SetBkColor (hDC, m_backColor);
		gPrinting = Gdiplus::Graphics::FromHDC(pdc->GetSafeHdc());
		gPrinting->TranslateTransform(offsetX, offsetY);
		Gdiplus::RectF clip((Gdiplus::REAL)rcInvalid.left, (Gdiplus::REAL)rcInvalid.top, (Gdiplus::REAL)rcInvalid.Width(), (Gdiplus::REAL)rcInvalid.Height());
		gPrinting->SetClip(clip);
		
		Gdiplus::Color color(255, 255, 255, 255);
		Gdiplus::SolidBrush brush(color);
		gPrinting->Clear(color);
	}
	else
	{
		gBuffer = Gdiplus::Graphics::FromImage(m_bufferBitmap);
		gBuffer->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		gBuffer->Clear(backColor);
	}

	// tiles
	VARIANT_BOOL tilesVisible;
	m_tiles->get_Visible(&tilesVisible);

	bool tilesUpdate = false;
	// if projection isn't defined there is no way to display tiles
	if (tilesVisible && m_transformationMode != tmNotDefined)
	{
		CTiles* tiles = (CTiles*)m_tiles;
		if (m_isSnapshot)
		{
			if (((CTiles*)m_tiles)->TilesAreInScreenBuffer((void*)this))
			{
				((CTiles*)m_tiles)->MarkUndrawn();	
				CTilesDrawer drawer(gPrinting, &this->extents, this->m_pixelPerProjectionX, this->m_pixelPerProjectionY);
				if (m_transformationMode == tmDoTransformation)
					drawer.m_transfomation = ((CGeoProjection*)m_wgsProjection)->m_transformation;
				drawer.DrawTiles(this->m_tiles, this->PixelsPerMapUnit(), m_projection, tiles->m_provider->Projection, true);
				((CTiles*)m_tiles)->MarkUndrawn();
			}
		}
		else
		{
			tilesUpdate = tiles->UndrawnTilesExist();
			if (tilesUpdate)
			{
				Gdiplus::Graphics* gTiles = Gdiplus::Graphics::FromImage(m_tilesBitmap);
				if (!tiles->DrawnTilesExist())
				{
					// if it's the first tile - clear the background
					gTiles->Clear(Gdiplus::Color::Transparent);
				}
				
				// draw new tiles
				CTilesDrawer drawer(gTiles, &this->extents, this->m_pixelPerProjectionX, this->m_pixelPerProjectionY);

				if (m_transformationMode == tmDoTransformation)
					drawer.m_transfomation = ((CGeoProjection*)m_wgsProjection)->m_transformation;

				drawer.DrawTiles(this->m_tiles, this->PixelsPerMapUnit(), m_projection, tiles->m_provider->Projection, false);
			}

			if (tiles->DrawnTilesExist())
			{
				gBuffer->DrawImage(m_tilesBitmap, 0.0f, 0.0f);
			}
		}
	}
	
	// layers
	bool layersExist = m_activeLayers.size() > 0;
	if (layersExist)
	{
		if (m_isSnapshot)
		{
			this->DrawLayers(rcBounds, gPrinting);
		}
		else
		{
			if(!layersRedraw)
			{	
				// update from the layer buffer
				gBuffer->DrawImage(m_layerBitmap, 0.0f, 0.0f);
			}
			else
			{
				DWORD startTick = ::GetTickCount();
				
				Gdiplus::Graphics* gLayers = Gdiplus::Graphics::FromImage(m_layerBitmap);
				gLayers->Clear(Gdiplus::Color::Transparent);

				#ifdef _DEBUG
				//m_layerBitmap->Save(L"C:\\layers.png", &pngClsid, NULL);
				#endif

				gLayers->SetCompositingMode(Gdiplus::CompositingModeSourceOver);

				// main drawing
				//if (m_RotateAngle == 0)	// currently rotation is simply ignored
				{
					 this->DrawLayers(rcBounds, gLayers);
					
					 #pragma region Asynchronous
					 // a pointer ot member function
					 /*UINT (CMapView::* ptrDrawLayers)(LPVOID) = &CMapView::StartDrawLayers;
					 CMapView* map = this;
					 (this->*ptrDrawLayers)(NULL);*/

					 //DrawingParams* param = new  DrawingParams(this, gLayers, &rcBounds);
					 //CWinThread* thread = AfxBeginThread(&StartThread, param);
					 //delete param;
					 #pragma endregion
				}
				
				if (0)	// TODO: reimplement rotation
				{
					// TODO: reimplement using GDI+
					#pragma region Rotation
						//HDC hdcLayers = gLayers->GetHDC();
						//CDC* dcLayers = CDC::FromHandle(hdcLayers);
						//if (dcLayers)
						//{
					
						//	CDC     *tmpBackbuffer = new CDC();
						//	CRect   tmpRcBounds = new CRect();
						//	Extent  tmpExtent, saveExtent;       
						//	long    save_viewWidth, save_viewHeight;

						//	if (m_Rotate == NULL)
						//	  m_Rotate = new Rotate();

						//	tmpBackbuffer->CreateCompatibleDC(dcLayers);
						//	m_Rotate->setSize(rcBounds);
						//	m_Rotate->setupRotateBackbuffer(tmpBackbuffer->m_hDC, pdc->m_hDC, m_backColor);

						//	save_viewWidth = m_viewWidth;
						//	save_viewHeight = m_viewHeight;
						//	m_viewWidth = m_Rotate->rotatedWidth;
						//	m_viewHeight = m_Rotate->rotatedHeight;
						//	saveExtent = extents;
						//	tmpExtent = extents;
						//	tmpExtent.right += (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
						//	tmpExtent.bottom -= (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
						//	tmpExtent.left -= (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
						//	tmpExtent.top += (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
						//	extents = tmpExtent;

						//	// draw the Map
						//	//this->DrawLayers(rcBounds,tmpBackbuffer, gLayers);
						//	
						//	// Cleanup
						//	extents = saveExtent;
						//	m_viewWidth = save_viewWidth;
						//	m_viewHeight = save_viewHeight;
						//	m_Rotate->resetWorldTransform(tmpBackbuffer->m_hDC);
						//	dcLayers->BitBlt(0,0,rcBounds.Width(),rcBounds.Height(), tmpBackbuffer, 0, 0, SRCCOPY);
						//	m_Rotate->cleanupRotation(tmpBackbuffer->m_hDC);
						//	tmpBackbuffer->DeleteDC();
						//}
						//gLayers->ReleaseHDC(hdcLayers);
					#pragma endregion
				}
				
				// passing layers to the back buffer
				gBuffer->DrawImage(m_layerBitmap, 0.0f, 0.0f);

				// displaying the time
				DWORD endTick = GetTickCount();
				this->ShowRedrawTime(gBuffer, (float)(endTick - startTick)/1000.0f);
			}
		}
	}
	m_canbitblt = TRUE;
	
	// hot tracking
	if (m_hotTracking.Shapefile && !m_isSnapshot)
	{
		CShapefileDrawer drawer(gBuffer, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, &m_collisionList, 
			this->GetCurrentScale(), true);
		drawer.Draw(rcBounds, m_hotTracking.Shapefile, ((CShapefile*)m_hotTracking.Shapefile)->get_File());
	}

	// passing buffer to user (includes background, tiles, layers, spatially referenced drawing)
	if (m_sendOnDrawBackBuffer && !m_isSnapshot)
	{
		HDC hdc = gBuffer->GetHDC();
		this->FireOnDrawBackBuffer((long)hdc);
		gBuffer->ReleaseHDC(hdc);
	}
	
	#ifdef _DEBUG
	//m_layerBitmap->Save(L"C:\\layers.png", &pngClsid, NULL);
	//m_bufferBitmap->Save(L"C:\\buffer.png", &pngClsid, NULL);
	#endif

	if (m_isSnapshot)
	{
		this->DrawLists(rcBounds, gPrinting, dlSpatiallyReferencedList);
		this->DrawLists(rcBounds, gPrinting, dlScreenReferencedList);
	}
	else
	{
		// drawing layers
		Gdiplus::Graphics* gDrawing = Gdiplus::Graphics::FromImage(m_drawingBitmap);
		gDrawing->Clear(Gdiplus::Color::Transparent);

		// fire external drawing
		{
			HDC hdc = gBuffer->GetHDC();
			VARIANT_BOOL retVal = VARIANT_FALSE;
			this->FireBeforeDrawing((long)hdc, rcBounds.left, rcBounds.right, rcBounds.top, rcBounds.bottom, &retVal);
			gBuffer->ReleaseHDC(hdc);
		}

		// temp objects
		this->DrawLists(rcBounds, gDrawing, dlSpatiallyReferencedList);
		this->DrawLists(rcBounds, gDrawing, dlScreenReferencedList);
		
		// fire external drawing code
		{
			HDC hdc = gBuffer->GetHDC();
			VARIANT_BOOL retVal = VARIANT_FALSE;
			this->FireAfterDrawing((long)hdc, rcBounds.left, rcBounds.right, rcBounds.top, rcBounds.bottom, &retVal);
			gBuffer->ReleaseHDC(hdc);
		}

		// passing layers to the back buffer
		gBuffer->DrawImage(m_drawingBitmap, 0.0f, 0.0f);
		delete gDrawing;
	}

	if (m_scalebarVisible)
		this->DrawScaleBar(m_isSnapshot ? gPrinting : gBuffer);

	VARIANT_BOOL persistent;
	m_measuring->get_Persistent(&persistent);
	if (m_cursorMode == cmMeasure || persistent)
	{
		this->DrawMeasuring(m_isSnapshot ? gPrinting : gBuffer);
	}

	// passing the main buffer to the screen
	if (!m_isSnapshot)
	{
		HDC hdc = pdc->GetSafeHdc();
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
		g->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
		g->DrawImage(m_bufferBitmap, 0.0f, 0.0f);
		g->ReleaseHDC(pdc->GetSafeHdc());
		delete g;
		delete gBuffer;
	}
	else
	{
		gPrinting->ReleaseHDC(pdc->GetSafeHdc());
		delete gPrinting;
	}
}
#pragma endregion

#pragma region Draw layers
// ****************************************************************
//		DrawLayers()
// ****************************************************************
void CMapView::DrawLayers(const CRect & rcBounds, Gdiplus::Graphics* graphics)
{
	if (m_lockCount > 0 && !m_isSnapshot)
	{
		return;
	}
	
	HCURSOR oldCursor = this->SetWaitCursor();

	// clear extents of drawn labels and charts
	this->ClearLabelFrames();
	
	m_drawMutex.Lock();

	//dc->FillSolidRect(rcBounds,m_backColor);
	register int i, j;
	long startcondition = 0;
	long endcondition = m_activeLayers.size();

	if (endcondition == 0)
	{
		m_drawMutex.Unlock();

      if (oldCursor != NULL)
         ::SetCursor(oldCursor);
		return;
	}
	
	// ------------------------------------------------------------------
	//	Check whether some layers are completely concealed by images 
	//	no need to draw them then
	// ------------------------------------------------------------------
	bool * isConcealed = NULL;
	if( endcondition )
	{
		isConcealed = new bool[endcondition];
		memset(isConcealed,0,endcondition*sizeof(bool));
	}
	
	double scale = this->GetCurrentScale();
	int zoom;
	this->m_tiles->get_CurrentZoom(&zoom);

	if( m_numImages > 0 && !_canUseImageGrouping )
	{
		for( i = endcondition - 1; i >= 0; i-- )
		{
			Layer * l = m_allLayers[m_activeLayers[i]];
			if( IS_VALID_PTR(l) )
			{
				if( l->type == ImageLayer && l->IsVisible(scale, zoom)) 
				{
					IImage * iimg = NULL;
					if (!l->QueryImage(&iimg)) continue;
					//l->object->QueryInterface(IID_IImage,(void**)&iimg);
					//if( iimg == NULL )continue;
					
					this->AdjustLayerExtents(i);

					VARIANT_BOOL useTransparencyColor;
					iimg->get_UseTransparencyColor(&useTransparencyColor);
					iimg->Release();
					iimg = NULL;

					if( useTransparencyColor == FALSE )
					{
						//Check if this is the end condition layer
						if( l->extents.left <= extents.left && 
							l->extents.right >= extents.right &&
							l->extents.bottom <= extents.bottom && 
							l->extents.top >= extents.top )
						{	
							startcondition = i;
							break;
						}
						//Check if this layer conceals any others
						else if( isConcealed[i] == false )
						{
							for( j = i - 1; j >= 0; j-- )
							{
								Layer * l2 = m_allLayers[m_activeLayers[j]];
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
	
	// ------------------------------------------------------------------
	//		Drawing of grouped image layers
	// ------------------------------------------------------------------
	if ( _canUseImageGrouping )
	{
		std::vector<ImageGroup*>* newGroups = new std::vector<ImageGroup*>;

		// building groups
		this->BuildImageGroups(*newGroups);
		
		// comparing them with the old list
		if (m_imageGroups != NULL)
		{
			if (this->ImageGroupsAreEqual(*m_imageGroups, *newGroups))
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
				if (m_imageGroups != NULL)
				{
					for (size_t i = 0; i < m_imageGroups->size(); i++)
					{
						delete (*m_imageGroups)[i];
					}
					
					m_imageGroups->clear();
					delete m_imageGroups;
					m_imageGroups = NULL;
				}
				m_imageGroups = newGroups;
			}
		}
		else
		{
			m_imageGroups = newGroups;
		}
		
		// mark all images as undrawn
		for (size_t i = 0; i < m_imageGroups->size(); i++)
		{
			(*m_imageGroups)[i]->wasDrawn = false;
		}
	}
	
	// ------------------------------------------------------------------
	//		Actual drawing
	// ------------------------------------------------------------------
	double currentScale = this->GetCurrentScale();

	bool useCommonCollisionListForCharts = true;
	bool useCommonCollisionListForLabels = true;

	// collision avoidance
	m_collisionList.Clear();
	CCollisionList collisionListLabels;
	CCollisionList collisionListCharts;
	
	CCollisionList* chosenListLabels = NULL;
	CCollisionList* chosenListCharts = NULL;
	
	chosenListLabels = useCommonCollisionListForLabels?(&m_collisionList):(&collisionListLabels);
	chosenListCharts = useCommonCollisionListForCharts?(&m_collisionList):(&collisionListCharts);

	// initializing classes for drawing
	bool forceGdiplus = this->m_RotateAngle != 0.0f || m_isSnapshot;
	
	CShapefileDrawer sfDrawer(graphics, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, &m_collisionList, 
						   this->GetCurrentScale(), forceGdiplus);

	CImageDrawer imgDrawer(graphics, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, m_viewWidth, m_viewHeight);
	CLabelDrawer lblDrawer(graphics, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, currentScale, 
		chosenListLabels, m_RotateAngle, m_isSnapshot);
	CChartDrawer chartDrawer(graphics, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, currentScale, chosenListCharts);

	for(int i = startcondition; i < endcondition; i++)
	{
		//CString str;
		//str.Format("Drawing layer %d", i);
		//timer.PrintTime(str.GetBuffer());
		
		if( isConcealed[i] == false )
		{
			Layer * l = m_allLayers[m_activeLayers[i]];
			if( l != NULL )
			{	
				if (l->IsVisible(scale, zoom))
				{
					if(l->type == ImageLayer)
					{
						if(l->object == NULL ) continue;
						IImage * iimg = NULL;
						if (!l->QueryImage(&iimg)) continue;
						//l->object->QueryInterface(IID_IImage,(void**)&iimg);
						//if( iimg == NULL ) continue;
						
						CImageClass* img = (CImageClass*)iimg;
						
						if (_canUseImageGrouping && img->m_groupID != -1)
						{
							// this is grouped image, if this is the first image of group, we shall draw the whole group
							if (!(*m_imageGroups)[img->m_groupID]->wasDrawn)
							{
								this->DrawImageGroups(rcBounds, graphics, img->m_groupID);
								(*m_imageGroups)[img->m_groupID]->wasDrawn = true;
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
									if (bmp->extents == extents &&
										bmp->pixelPerProjectionX == m_pixelPerProjectionX &&
										bmp->pixelPerProjectionY == m_pixelPerProjectionY &&
										bmp->viewWidth == m_viewWidth &&
										bmp->viewHeight == m_viewHeight && !((CImageClass*)iimg)->_imageChanged )
									{
										//Gdiplus::Graphics g(dc->m_hDC);
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

									if ((width == 256 && height == 256) || m_isSnapshot)
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
					else if( l->type == ShapefileLayer )
					{
						// grab extents from shapefile in case they've changed
						this->AdjustLayerExtents(m_activeLayers[i]);

						if( l->extents.left   < extents.left   && l->extents.right < extents.left )		continue;
						if( l->extents.left   > extents.right  && l->extents.right > extents.right )	continue;
						if( l->extents.bottom < extents.bottom && l->extents.top   < extents.bottom )	continue;
						if( l->extents.bottom > extents.top    && l->extents.top   > extents.top )		continue;
					
						if( l->object == NULL )
						{
							continue;	// TODO: report the error?
						}
						
						IShapefile* sf = NULL;
						//l->object->QueryInterface(IID_IShapefile,(void**)&sf);
						//if( sf )
						if (l->QueryShapefile(&sf))
						{
							sfDrawer.Draw(rcBounds, sf, ((CShapefile*)sf)->get_File());

							// for old modes we shall mark all the shapes of shapefile as visible as no visiblity expressions were analyzed
							if (m_ShapeDrawingMethod != dmNewSymbology)
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
							sf->Release();
						}
					}
				}
			}
		}
	}
	
	// -------------------------------------------------
	//	Drawing labels and charts above the layers
	// -------------------------------------------------
	for (i = 0; i < (int)m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			if (l->IsVisible(scale, zoom))
			{
				//  labels: for the new modes only
				if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology || l->type == ImageLayer || FORCE_NEW_LABELS)
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
				IShapefile* sf = NULL;
				//l->object->QueryInterface(IID_IShapefile,(void**)&sf);
				//if ( sf )
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

	
	
	m_drawMutex.Unlock();

   if (oldCursor != NULL)
      ::SetCursor(oldCursor);

	delete[] isConcealed;
}
#pragma endregion

#pragma region ImageGrouping
// *****************************************************************
//		BuildImageGroups
// *****************************************************************
// Here we'll make groups from the images with the same size and positions
// Group number will be written to the each image groupID property
void CMapView::BuildImageGroups(std::vector<ImageGroup*>& imageGroups)
{
	imageGroups.clear();

	for(size_t i = 0; i < m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			if(l->type == ImageLayer)
			{
				IImage* iimg = NULL;
				//l->object->QueryInterface(IID_IImage, (void**)&iimg);
				//if ( iimg != NULL )
				if (l->QueryImage(&iimg))
				{
					CImageClass* img = (CImageClass*)iimg;
					img->m_groupID = -1;
					
					if (l->flags & Visible)
					{
						if ( img->_canUseGrouping)
						{
							double dx, dy, xllCenter, yllCenter;
							LONG width, height;

							img->get_OriginalHeight(&height);
							img->get_OriginalWidth(&width);
							
							img->get_OriginalDX(&dx);
							img->get_OriginalDY(&dy);
							img->get_OriginalXllCenter(&xllCenter);
							img->get_OriginalYllCenter(&yllCenter);

							//img->GetOriginal_dX(&dx);
							//img->GetOriginal_dY(&dy);
							//img->GetOriginalXllCenter(&xllCenter);
							//img->GetOriginalYllCenter(&yllCenter);

							bool groupFound = false;
							for(size_t j = 0; j < imageGroups.size(); j++)
							{
								ImageGroup* group = imageGroups[j];
								
								if ((group->dx == dx) && 
									(group->dy == dy) && 
									(group->width == width) && 
									(group->height == height) &&
									(group->xllCenter == xllCenter) && 
									(group->yllCenter == yllCenter))
								{
									groupFound = true;
									group->imageIndices.push_back(i);
									break;
								}
							}
							
							if (! groupFound )
							{
								// adding new group
								ImageGroup* group = new ImageGroup(dx, dy, xllCenter, yllCenter, width, height);
								imageGroups.push_back(group);
								imageGroups[imageGroups.size() - 1]->imageIndices.push_back(i);
							}
						}
					}
				}
			}
		}
	}

	// now we'll check whether the pixels of image are scarce enough for us
	// the group wil work only in case there is more then 1 suitable image
	int groupId = 0;
	IImage* iimg = NULL;
	for (size_t i = 0; i < imageGroups.size(); i++)
	{
		std::vector<int>* indices = &imageGroups[i]->imageIndices;
		int groupSize = indices->size();

		if (groupSize > 1)
		{
			for (size_t j = 0; j < indices->size(); j++ )
			{
				Layer * l = m_allLayers[m_activeLayers[(*indices)[j]]];
				//l->object->QueryInterface(IID_IImage, (void**)&iimg);
				//if (iimg)
				if (l->QueryImage(&iimg))
				{
					CImageClass* img = (CImageClass*)iimg;
					
					if (!img->_pixelsSaved)				// it's the first time we try to draw image or transparency color chnaged
					{
						if (!img->SaveNotNullPixels())	// analysing pixels...
						{
							(*indices)[j] = -1;
							img->put_CanUseGrouping(VARIANT_FALSE);	//  don't try this image any more - there are to many data pixels in it
							groupSize--;
						}
					}
					iimg->Release();
				}
			}
		}
		
		// saving the valid groups
		if (groupSize > 1)
		{
			imageGroups[i]->isValid = true;
			for (size_t i = 0; i< indices->size(); i++)
			{
				int imageIndex = (*indices)[i];
				if (imageIndex != -1)
				{
					Layer * l = m_allLayers[m_activeLayers[imageIndex]];
					//l->object->QueryInterface(IID_IImage, (void**)&iimg);
					//if (iimg)
					if (l->QueryImage(&iimg))
					{
						CImageClass* img = (CImageClass*)iimg;
						img->m_groupID = groupId;
						iimg->Release();
					}
				}
			}
			groupId++;
		}
		else
		{
			imageGroups[i]->isValid = false;
		}
	}
}

// *****************************************************************
//		ChooseInterpolationMode
// *****************************************************************
// Choosing the mode with better quality from the pair
tkInterpolationMode CMapView::ChooseInterpolationMode(tkInterpolationMode mode1, tkInterpolationMode mode2)
{
	if (mode1 == imHighQualityBicubic || mode2 == imHighQualityBicubic )
	{
		return imHighQualityBicubic;
	}
	else if (mode1 == imHighQualityBilinear || mode2 == imHighQualityBilinear )
	{
		return imHighQualityBilinear;
	}
	else if (mode1 == imBicubic || mode2 == imBicubic )
	{
		return imBicubic;
	}
	else if (mode1 == imBilinear || mode2 == imBilinear )
	{
		return imBilinear;
	}
	else
	{
		return imNone;
	}
}

// *****************************************************************
//		DrawImageGroups
// *****************************************************************
// groupIndex - index of group that should be drawn
void CMapView::DrawImageGroups(const CRect& rcBounds, Gdiplus::Graphics* graphics, int groupIndex)
{
	CImageDrawer imgDrawer(graphics, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, m_viewWidth, m_viewHeight);
	IImage* iimg = NULL;

	ImageGroup* group = (*m_imageGroups)[groupIndex];
	if (! group->isValid ) 
		return;
	
	// in case the image was drawn at least once at current resolution, we can use screenBitmap
	ScreenBitmap* bmp = NULL;
	bmp = group->screenBitmap;
	if (bmp != NULL)
	{
		if (bmp->extents == extents &&
			bmp->pixelPerProjectionX == m_pixelPerProjectionX &&
			bmp->pixelPerProjectionY == m_pixelPerProjectionY &&
			bmp->viewWidth == m_viewWidth &&
			bmp->viewHeight == m_viewHeight)
		{
			//Gdiplus::Graphics g(dc->m_hDC);
			graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
			
			graphics->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			graphics->SetSmoothingMode(Gdiplus::SmoothingModeDefault);
			graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			graphics->DrawImage(bmp->bitmap, Gdiplus::REAL(bmp->left), Gdiplus::REAL(bmp->top));
			return;
		}
	}
	
	double scale = GetCurrentScale();
	int zoom;
	this->m_tiles->get_CurrentZoom(&zoom);

	if(group->image == NULL)
	{
		// creating a new temporary image		
		IImage* imgGroup = NULL;
		VARIANT_BOOL vbretval;
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&imgGroup);
		imgGroup->CreateNew(group->width, group->height, &vbretval);

		if ( !vbretval )
		{
			return;
		}
		else
		{
			// setting it's position
			imgGroup->put_dX(group->dx);
			imgGroup->put_dY(group->dy);
			imgGroup->put_XllCenter(group->xllCenter);
			imgGroup->put_YllCenter(group->yllCenter);
			
			tkInterpolationMode downsamplingMode = imNone;
			tkInterpolationMode upsamplingMode = imNone;
			
			// acquiring reference to the destination color array
			unsigned char* data = ((CImageClass*)imgGroup)->get_ImageData();
			colour* dstData = reinterpret_cast<colour*>(data);
			
			// passing the data from all images
			bool visibleLayerExists = false;
			bool useTransparencyColor = true;		
			for(size_t j = 0; j < m_activeLayers.size(); j++)
			{
				Layer * l = m_allLayers[m_activeLayers[j]];
				if( l != NULL )
				{	
					//if(l->type == ImageLayer && (l->flags & Visible))
					if(l->type == ImageLayer && l->IsVisible(scale, zoom))
					{
						//l->object->QueryInterface(IID_IImage, (void**)&iimg);
						//if (iimg)
						if (l->QueryImage(&iimg))
						{
							CImageClass* img = (CImageClass*)iimg;

							if ( img )
							{
								if (img->m_groupID == groupIndex)
								{
									tkInterpolationMode downMode;
									tkInterpolationMode upMode;
									img->get_DownsamplingMode(&downMode);
									img->get_UpsamplingMode(&upMode);
									
									// in case at least one image don't use transparency the grouped bitmap will have white background
									VARIANT_BOOL transp;
									img->get_UseTransparencyColor(&transp);
									if (!transp) 
										useTransparencyColor = false;

									downsamplingMode = ChooseInterpolationMode(downsamplingMode, downMode);
									upsamplingMode = ChooseInterpolationMode(upsamplingMode, upMode);
									
									visibleLayerExists = true;

									DataPixels* pixels = img->m_pixels;
									int pixelsCount = img->m_pixelsCount;

									// passing data
									DataPixels* val;
									for (int p = 0; p < pixelsCount; p++ )
									{
										val = pixels + p;
										memcpy(&(dstData[val->position]), &val->value, sizeof(colour));
										//dstData[val->position] = val->value;
									}
								}
							}
							iimg->Release();
						}
					}
				}
			}
			
			if (useTransparencyColor)
			{
				imgGroup->put_TransparencyColor(RGB(255, 255, 255));
				imgGroup->put_TransparencyColor2(RGB(255, 255, 255));
				imgGroup->put_UseTransparencyColor(VARIANT_TRUE);
			}
			else
			{
				imgGroup->put_UseTransparencyColor(VARIANT_FALSE);
			}

			if (!visibleLayerExists)
			{
				return;
			}
			else
			{
				// setting sampling mode
				imgGroup->put_UpsamplingMode(upsamplingMode);
				imgGroup->put_DownsamplingMode(downsamplingMode);
				group->image = imgGroup;
			}
		}
	}
	
	// drawing; in case we draw it first time screen bitmap will be saved, for not doing resampling when loading each new tile
	/* ScreenBitmap*  */
	bmp = imgDrawer.DrawImage(rcBounds, group->image, true);
	if (bmp)
	{
		if (group->screenBitmap != NULL)
		{
			delete group->screenBitmap;
			group->screenBitmap = NULL;
		}
		
		int width = bmp->bitmap->GetWidth();
		int height = bmp->bitmap->GetHeight();
		
		group->screenBitmap = bmp;	// saving bitmap in screen resolution
	}
}

// *****************************************************************
//		ImageGroupsAreEqual()
// *****************************************************************
bool CMapView::ImageGroupsAreEqual(std::vector<ImageGroup*>& groups1, std::vector<ImageGroup*>& groups2)
{
	if (groups1.size() != groups2.size())
	{
		return false;
	}
	else
	{
		for (size_t i = 0; i < groups1.size(); i++)
		{
			if (!(groups1[i] == groups2[i]))
			{
				return false;
			}
		}
	}
	return true;
}
#pragma endregion

#pragma region REGION SnapShots

// *********************************************************
//		SnapShot()
// *********************************************************
LPDISPATCH CMapView::SnapShot(LPDISPATCH BoundBox)
{
	if( BoundBox == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return NULL;
	}

	IExtents * box = NULL;
	BoundBox->QueryInterface(IID_IExtents,(void**)&box);

	if( box == NULL )
	{	
		ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		return NULL;
	}

	double left, right, bottom, top, nv;
	box->GetBounds(&left,&bottom,&nv,&right,&top,&nv);
	box->Release();
	box = NULL;
	
	return SnapShotCore(left, right, bottom, top, m_viewWidth, m_viewHeight);
}

// *********************************************************
//		SnapShot2()
// *********************************************************
// use the indicated layer and zoom/width to determine the output size and clipping
IDispatch* CMapView::SnapShot2(LONG ClippingLayerNbr, DOUBLE Zoom, long pWidth)
{   
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long Width, Height;
	double left, right, bottom, top;

	Layer * l = m_allLayers[ClippingLayerNbr];
	if( !IS_VALID_PTR(l) )
	{
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Cannot clip to selected layer"));
		return NULL;
	}
	else
	{	
		this->AdjustLayerExtents(ClippingLayerNbr);
		left = l->extents.left;
		right = l->extents.right;
		top = l->extents.top;
		bottom = l->extents.bottom;

		if( l->type == ShapefileLayer )
		{
			double ar = (right-left)/(top-bottom);
			Width = (long) (pWidth == 0 ? ((right - left) * Zoom) : pWidth);
			Height = (long)((double)pWidth / ar);
		}
		else if(l->type == ImageLayer)
		{
			Width = (long)(right - left);
			Height = (long)(top - bottom);
			if (Zoom > 0)
			{
				Width *= (long)Zoom;
				Height *= (long)Zoom;
			}
		}
		else
		{
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Cannot clip to selected layer type"));
			return NULL;
		}
	}

	if (Width <= 0 || Height <= 0)
	{
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Invalid Width and/or Zoom"));
		return NULL;
	}

	return this->SnapShotCore(left, right, top, bottom, Width, Height);
}

//Created a new snapshot method which works a bit better specifically for the printing engine
//1. Draw to a back buffer, 2. Populate an Image object
LPDISPATCH CMapView::SnapShot3(double left, double right, double top, double bottom, long Width)
{   
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long Height = (long)((double)Width / ((right-left)/(top-bottom)));
	if (Width <= 0 || Height <= 0)
	{
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Invalid Width and/or Zoom"));
		return NULL;
	}

	return this->SnapShotCore(left, right, top, bottom, Width, Height);
}

// *********************************************************************
//    TilesAreInCache()
// *********************************************************************
INT CMapView::TilesAreInCache(IExtents* Extents, LONG WidthPixels, tkTileProvider provider)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	
	if (Extents)
	{
		// Get the image height based on the box aspect ratio
		double xMin, xMax, yMin, yMax, zMin, zMax;
		Extents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		
		// Make sure that the width and height are valid
		long Height = static_cast<long>((double)WidthPixels *(yMax - yMin) / (xMax - xMin));
		if (WidthPixels <= 0 || Height <= 0)
		{
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(), A2BSTR("Invalid Width and/or Zoom"));
		}
		else
		{
			SetTempExtents(xMin, xMax, yMin, yMax, WidthPixels, Height);
			bool tilesInCache =((CTiles*)m_tiles)->TilesAreInCache((void*)this, provider);
			RestoreExtents();
			return tilesInCache ? 1 : 0;
		}
	}
	return -1;	// error
}

// *********************************************************************
//    LoadTiles()
// *********************************************************************
// Loads tiles for specified extents
void CMapView::LoadTiles(IExtents* Extents, LONG WidthPixels, LPCTSTR Key, tkTileProvider provider)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	
	if (Extents)
	{
		// Get the image height based on the box aspect ratio
		double xMin, xMax, yMin, yMax, zMin, zMax;
		Extents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		
		// Make sure that the width and height are valid
		long Height = static_cast<long>((double)WidthPixels *(yMax - yMin) / (xMax - xMin));
		if (WidthPixels <= 0 || Height <= 0)
		{
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(), A2BSTR("Invalid Width and/or Zoom"));
		}
		else
		{
			CString key = (char*)Key;
			SetTempExtents(xMin, xMax, yMin, yMax, WidthPixels, Height);
			bool tilesInCache =((CTiles*)m_tiles)->TilesAreInCache((void*)this, provider);
			if (!tilesInCache)
			{
				((CTiles*)m_tiles)->LoadTiles((void*)this, true, (int)provider, key);
				RestoreExtents();
			}
			else
			{
				// they are already here, no loading is needed
				RestoreExtents();
				FireTilesLoaded(m_tiles, NULL, true, key);
			}
		}
	}
}

BOOL CMapView::SnapShotToDC2(PVOID hdc, IExtents* Extents, LONG Width, float OffsetX, float OffsetY,
							 float ClipX, float ClipY, float clipWidth, float clipHeight)
{
	if(!Extents || !hdc) 
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return FALSE;
	}
	// getting DC to draw
	HDC dc = reinterpret_cast<HDC>(hdc);
	CDC * tempDC = CDC::FromHandle(dc);

	// Get the image height based on the box aspect ration
	double xMin, xMax, yMin, yMax, zMin, zMax;
	Extents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
	
	// Make sure that the width and height are valid
	long Height = static_cast<long>((double)Width *(yMax - yMin) / (xMax - xMin));
	if (Width <= 0 || Height <= 0)
	{
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(), A2BSTR("Invalid Width and/or Zoom"));
		return FALSE;
	}
	
	SnapShotCore(xMin, xMax, yMin, yMax, Width, Height, tempDC, OffsetX, OffsetY, ClipX, ClipY, clipWidth, clipHeight);
	return TRUE;
}

// *********************************************************************
//    SnapShotToDC()
// *********************************************************************
// Draws the specified extents of map at given DC.
BOOL CMapView::SnapShotToDC(PVOID hdc, IExtents* Extents, LONG Width)
{
	return this->SnapShotToDC2(hdc, Extents, Width, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

#pragma region Swtich Extents
void CMapView::SetTempExtents(double left, double right, double top, double bottom, long Width, long Height)
{
	mm_viewWidth = m_viewWidth;
	mm_viewHeight = m_viewHeight;
	mm_pixelPerProjectionX = m_pixelPerProjectionX;
	mm_pixelPerProjectionY = m_pixelPerProjectionY;
	mm_inversePixelPerProjectionX = m_inversePixelPerProjectionX;
	mm_inversePixelPerProjectionY = m_inversePixelPerProjectionY;
	mm_aspectRatio = m_aspectRatio;
	mm_left = extents.left;
	mm_right = extents.right;
	mm_bottom = extents.bottom;
	mm_top = extents.top;

	mm_newExtents = (Width != m_viewWidth || Height != m_viewWidth ||
						left != extents.left || right !=  extents.right ||
						top != extents.top || bottom != extents.bottom);

	if (mm_newExtents)
	{
		m_viewWidth=Width;
		m_viewHeight=Height;
		//ResizeBuffers(m_viewWidth, m_viewHeight);
		m_aspectRatio = (double)Width / (double)Height; 

		double xrange = right - left;
		double yrange = top - bottom;
		m_pixelPerProjectionX = m_viewWidth/xrange;
		m_inversePixelPerProjectionX = 1.0/m_pixelPerProjectionX;
		m_pixelPerProjectionY = m_viewHeight/yrange;
		m_inversePixelPerProjectionY = 1.0/m_pixelPerProjectionY;
		
		extents.left = left;
		extents.right = right - m_inversePixelPerProjectionX;
		extents.bottom = bottom;
		extents.top = top - m_inversePixelPerProjectionY;

		CalculateVisibleExtents(Extent(left,right,bottom,top));
	}
}

void CMapView::RestoreExtents()
{
	if (mm_newExtents)
	{
		m_viewWidth = mm_viewWidth;
		m_viewHeight = mm_viewHeight;
		//ResizeBuffers(m_viewWidth, m_viewHeight);
		m_aspectRatio = mm_aspectRatio; 
		m_pixelPerProjectionX = mm_pixelPerProjectionX;
		m_pixelPerProjectionY = mm_pixelPerProjectionY;
		m_inversePixelPerProjectionX = mm_inversePixelPerProjectionX;
		m_inversePixelPerProjectionY = mm_inversePixelPerProjectionY;
		extents.left = mm_left;
		extents.right = mm_right;
		extents.bottom = mm_bottom;
		extents.top = mm_top;
	}
}
#pragma endregion

void CMapView::Log(CString message)
{
	if (tilesLogger.is_open() && tilesLogger.good())
	{
		tilesLogger << message << endl;    tilesLogger.flush();
	}
}

// *********************************************************
//		SnapShotCore()
// *********************************************************
// first 4 paramters - extents in map units; last 2 - the size of bitmap to draw this extents on
IDispatch* CMapView::SnapShotCore(double left, double right, double top, double bottom, long Width, long Height, CDC* snapDC,
								  float offsetX, float offsetY, float clipX, float clipY, float clipWidth, float clipHeight)
{
	bool createDC = (snapDC == NULL);
	CBitmap * bmp = NULL;
	
	if (createDC)
	{
		bmp = new CBitmap();
		if (!bmp->CreateDiscardableBitmap(GetDC(), Width, Height))
		{
			delete bmp;
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Failed to create bitmap; not enough memory?"));
			return NULL;
		}
	}

	LockWindow( lmLock );

	SetTempExtents(left, right, top, bottom, Width, Height);

	if (mm_newExtents)
	{
		ReloadImageBuffers();
		((CTiles*)m_tiles)->MarkUndrawn();		// otherwise they will be taken from screen buffer
	}

	IImage * iimg = NULL;
	bool tilesInCache = false;

	// create canvas
	CBitmap * oldBMP = NULL;
	if (createDC)
	{
		snapDC = new CDC();
		snapDC->CreateCompatibleDC(GetDC());
		oldBMP = snapDC->SelectObject(bmp);
	}
	
	// do the drawing
	m_canbitblt=FALSE;
	m_isSnapshot = true;

	
	tilesInCache =((CTiles*)m_tiles)->TilesAreInCache((void*)this);
	if (tilesInCache)
	{
		((CTiles*)m_tiles)->LoadTiles((void*)this, true);		// simply move the to the screen buffer (is performed synchronously)
	}

	CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
	CRect rcClip((int)clipX, (int)clipY, (int)clipWidth, (int)clipHeight);
	CRect* r = clipWidth != 0.0 && clipHeight != 0.0 ? &rcClip : &rcBounds;
	
	HandleNewDrawing(snapDC, rcBounds, *r, offsetX, offsetY);
	
	CMeasuring* m = (CMeasuring*)m_measuring;
	if (m->persistent && m->IsStopped() && m->measuringType == tkMeasuringType::MeasureArea)
	{
		DrawMouseMoves(snapDC, rcBounds, *r, true, offsetX, offsetY);
	}

	m_canbitblt=FALSE;
	m_isSnapshot = false;

	if (createDC)
	{
		// create output
		VARIANT_BOOL retval;
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimg);
		iimg->SetImageBitsDC((long)snapDC->m_hDC,&retval);

		double dx = (right-left)/(double)(m_viewWidth);
		double dy = (top-bottom)/(double)(m_viewHeight);
		iimg->put_dX(dx);
		iimg->put_dY(dy);
		iimg->put_XllCenter(left + dx*.5);
		iimg->put_YllCenter(bottom + dy*.5);
	
		// dispose the canvas
		snapDC->SelectObject(oldBMP);
		bmp->DeleteObject();
		snapDC->DeleteDC();
		delete bmp;
		delete snapDC;
	}

	RestoreExtents();

	if (mm_newExtents)
	{
		this->ReloadImageBuffers();
		mm_newExtents = false;
	}

	if (tilesInCache)
	{
		// restore former list of tiles in the buffer
		((CTiles*)m_tiles)->LoadTiles((void*)this, false);	  
	}

	LockWindow( lmUnlock );
	return iimg;
}

// ********************************************************************
//		DrawBackBuffer()
// ********************************************************************
// Draws the backbuffer to the specified DC (probably external)
void CMapView::DrawBackBuffer(int** hdc, int ImageWidth, int ImageHeight)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!hdc)
	{
		m_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return;
	}
	
	CDC* dc = CDC::FromHandle((HDC)hdc);
	CRect rect(0,0, ImageWidth, ImageHeight);
	OnDraw(dc, rect, rect);
}
#pragma endregion

#pragma region Scalebar + maptext
// ****************************************************************
//		GetUnitOfMeasureText
// ****************************************************************
// Returns the short namó for units of measure
CString GetUnitOfMeasureText(tkUnitsOfMeasure units)
{
	switch(units)
	{
		case umDecimalDegrees:
			return "deg.";
		case umMiliMeters:
			return "mm";
		case umCentimeters:
			return "cm";
		case umInches:
			return "inches";
		case umFeets:
			return "feet";
		case umYards:
			return "yards";
		case umMeters:
			return "m";
		case umMiles:
			return "miles";
		case umKilometers:
			return "km";
		default:
			return "units";
	}
}

// ****************************************************************
//		DrawScaleBar()
// ****************************************************************
void CMapView::DrawScaleBar(Gdiplus::Graphics* g)
{
	if (m_transformationMode != tkTransformationMode::tmNotDefined)
	{
		int zoom = -1;
		m_tiles->get_CurrentZoom(&zoom);
		if (zoom >= 0 && zoom < 3) {
			// lsu: there are some problems with displaying scalebar at such zoom levels: 
			// - there are areas outside the globe where coordinate transformations may fail;
			// - the points at the left and right sides of the screen may lie on the same meridian
			// so geodesic distance across the screen will be 0;
			// - finally projection distortions change drastically by Y axis across map so
			// the scalebar will be virtually meaningless;
			// The easy solution will be simply not to show scalebar at such small scales
			return;
		}
	}
	
	double minX, maxX, minY, maxY;	// size of ap control in pixels
    PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	
	int barWidth = 140;
	int barHeight = 30;
	int yPadding = 10;
	int xPadding = 10;
	int xOffset = 20;
	int yOffset = 10;
	int segmHeight = 7;

	Gdiplus::SmoothingMode smoothing = g->GetSmoothingMode();
	Gdiplus::TextRenderingHint hint = g->GetTextRenderingHint();
	g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	tkUnitsOfMeasure targetUnits = umMeters;

	double xMin = extents.left;
	double yMin = extents.top;
	double xMax = extents.right;
	double yMax = extents.bottom;
	
	double width = extents.right - extents.left; // maxX - minX;
	tkUnitsOfMeasure units = m_unitsOfMeasure;

	// run calculations on ellipsoid
	if (m_transformationMode != tkTransformationMode::tmNotDefined)
	{
		// skip calculations when extents haven't changed
		if (this->m_lastWidthMeters == 0.0)
		{
			bool skipTransform = false;
			if (m_transformationMode == tkTransformationMode::tmDoTransformation)
			{
				VARIANT_BOOL vb;
				m_projection->get_HasTransformation(&vb);
				if (!vb) {
					m_projection->StartTransform(m_wgsProjection, &vb);
				}
				if (vb)
				{
					m_projection->Transform(&xMin, &yMin, &vb);
					m_projection->Transform(&xMax, &yMax, &vb);
				}
				else {
					skipTransform = true;
				}
			}
		
			if ( !skipTransform)
			{
				GetUtils()->GeodesicDistance((yMax + yMin)/2, xMin, (yMax + yMin)/2, xMax, &width);
				m_lastWidthMeters = width;
				units = tkUnitsOfMeasure::umMeters;
			}
		}
		else
		{
			width = m_lastWidthMeters;
			units = tkUnitsOfMeasure::umMeters;
		}
	}

	if (width > 0.0)
	{
		if( Utility::ConvertDistance(units, targetUnits, width))
		{
			double unitsPerPixel = width/(maxX - minX);	  // target units on screen size
			double distance = (barWidth - xPadding * 2) * unitsPerPixel;

			if (distance > 1000)
			{
				targetUnits = umKilometers;
				unitsPerPixel /= 1000.0;
				distance /= 1000.0;
			}

			if (distance < 1)
			{
				targetUnits = umCentimeters;
				unitsPerPixel *= 100.0;
				distance *= 100.0;
			}
		
			double power = floor(log10(distance));
			double step = pow(10, floor(log10(distance)));
			int count = (int)floor(distance/step);

			if (count == 1)
			{
				step /= 4;	// steps like 25-50-75
				count = (int)floor(distance/step);
			}

			if (count == 2)
			{
				step /= 2;	// steps like 0-50-100
				count = (int)floor(distance/step);
			}

			if (count > 8)
			{
				step *= 2.5;
				count = (int)floor(distance/step);
			}
			
			//Gdiplus::SolidBrush brush(Gdiplus::Color::White);
			Gdiplus::Pen pen(Gdiplus::Color::Black, 1.5f);
			Gdiplus::Pen penOutline(Gdiplus::Color::White, 3.0f);
			pen.SetLineJoin(Gdiplus::LineJoinRound);
			penOutline.SetLineJoin(Gdiplus::LineJoinRound);
			Gdiplus::Matrix mtx;
			Gdiplus::RectF rect(0.0f, 0.0f, (Gdiplus::REAL)barWidth, (Gdiplus::REAL)barHeight );

			// initializing font
			Gdiplus::FontFamily family(L"Arial");
			Gdiplus::Font font(&family, (Gdiplus::REAL)12.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
			
			CStringW s;
			Gdiplus::StringFormat format; 
			Gdiplus::GraphicsPath path;

			if (m_viewWidth > barWidth + xOffset &&
				m_viewHeight > barHeight + yOffset)		// control must be big enough
			{
				mtx.Translate((float)5, (float)m_viewHeight - barHeight - yOffset);
				g->SetTransform(&mtx);
				
				int length = (int)(step * count / unitsPerPixel + xPadding);

				// horizontal line
				g->DrawLine(&penOutline, xPadding,  barHeight - yPadding, length, barHeight - yPadding);
				g->DrawLine(&pen, xPadding,  barHeight - yPadding, length, barHeight - yPadding);
				
				// inner measures (shorter)
				for (int i = 0; i <= count; i++ )
				{
					length = (int)(step * i / unitsPerPixel + xPadding);
					int valHeight = (i == 0 || i == count) ? segmHeight * 2 : segmHeight;	// the height of the mark; side marks are longer
					g->DrawLine(&penOutline, length,  barHeight - yPadding - valHeight, length, barHeight - yPadding);
					g->DrawLine(&pen, length,  barHeight - yPadding - valHeight, length, barHeight - yPadding);
				}

				Gdiplus::Pen penText(Gdiplus::Color::White, 3.0f);
				Gdiplus::SolidBrush brushText(Gdiplus::Color::Black);

				s.Format(L"0");
				Gdiplus::PointF point(xPadding + 3.0f, -2.0f);
				path.StartFigure();
				path.AddString(s.GetString(), wcslen(s), &family, font.GetStyle(), font.GetSize(), point, &format);
				path.CloseFigure();
				g->DrawPath(&penText, &path);
				g->FillPath(&brushText, &path);

				if (power >= 0)
				{
					s.Format(L"%d",(int)(step * count));
				}
				else
				{
					CStringW sFormat;
					sFormat.Format(L"%%.%df", (int)-power);
					s.Format(sFormat,(float)step * count);
				}
				
				point.X = (Gdiplus::REAL)(step * count/unitsPerPixel + xPadding + 3);
				point.Y = -2.0f;	//yPadding
				path.StartFigure();
				path.AddString(s.GetString(), wcslen(s), &family, font.GetStyle(), font.GetSize(), point, &format);
				path.CloseFigure();
				g->DrawPath(&penText, &path);
				g->FillPath(&brushText, &path);

				s = GetUnitOfMeasureText(targetUnits);
				
				point.X = (Gdiplus::REAL)(step * count/unitsPerPixel + xPadding + 3);
				point.Y = (Gdiplus::REAL)(barHeight - yPadding - 12);
				path.StartFigure();
				path.AddString(s.GetString(), wcslen(s), &family, font.GetStyle(), font.GetSize(), point, &format);
				path.CloseFigure();
				g->DrawPath(&penText, &path);
				g->FillPath(&brushText, &path);

				g->ResetTransform();
			}
		}
	}

	g->SetTextRenderingHint(hint);
	g->SetSmoothingMode(smoothing);
}

// ***************************************************************
//		DrawSegmentInfo()
// ***************************************************************
void CMapView::DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, double length, 
					 double totalLength, int segmentIndex, CMeasuring* measure)
{
	CString s1, s2, s, sAz;

	double x = xScr - xScr2;
	double y = yScr - yScr2;
	
	double angle = 360.0 - GetPointAngle(x, y) * 180.0 / pi;							
	sAz.Format("%.1f°", angle);
	
	x = -x;
	y = -y;
	angle = GetPointAngle(x, y) * 180.0 / pi;							
	angle = - (angle - 90.0);

	CString m = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMeters);
	CString km = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsKilometers);

	if (length > 1000.0)
	{
		s1.Format("%.2f %s", length / 1000.0, km);
	}
	else
	{
		s1.Format("%.1f %s", length, m);
	}
	
	if (segmentIndex > 0 && totalLength != 0.0)
	{
		if (totalLength > 1000.0)
		{
			s2.Format("%.2f %s", totalLength / 1000.0, km);
		}
		else
		{
			s2.Format("%.1f %s", totalLength, m);
		}
		s.Format("%s (%s)", s1, s2);
	}
	else
	{
		s = s1;
	}

	WCHAR* wStr = Utility::StringToWideChar(s);
	WCHAR* wAz = Utility::StringToWideChar(sAz);
	
	Gdiplus::RectF r1(0.0f, 0.0f, 0.0f, 0.0f);
	Gdiplus::RectF r2(0.0f, 0.0f, 0.0f, 0.0f);
	g->MeasureString(wStr, wcslen(wStr), measure->font, Gdiplus::PointF(0.0f,0.0f), &measure->format, &r1);
	g->MeasureString(wAz, wcslen(wAz), measure->font, Gdiplus::PointF(0.0f,0.0f), &measure->format, &r2);
	
	double width = sqrt(x*x + y*y);	// width must not be longer than width of segment
	if (r1.Width > width && segmentIndex > 0 && totalLength != 0.0)
	{
		// if a segment is too short, let's try to display it without total distance
		delete wStr;
		wStr = Utility::StringToWideChar(s1);
		g->MeasureString(wStr, wcslen(wStr), measure->font, Gdiplus::PointF(0.0f,0.0f), &measure->format, &r1);
	}
	
	if (width >= r1.Width)
	{
		Gdiplus::Matrix m;
		g->GetTransform(&m);
		g->TranslateTransform((Gdiplus::REAL)xScr, (Gdiplus::REAL)yScr);
		
		bool upsideDown = false;
		if (angle < -90.0)
		{
			g->RotateTransform((Gdiplus::REAL)(angle + 180.0));
			g->TranslateTransform((Gdiplus::REAL)-width, (Gdiplus::REAL)0.0f);
			upsideDown = true;
		}
		else
		{
			g->RotateTransform((Gdiplus::REAL)angle);
			
		}
		
		// draw black text
		r1.X = (Gdiplus::REAL)(width - r1.Width) / 2.0f;
		r1.Y = -r1.Height;
		g->FillRectangle(&measure->whiteBrush, r1);
		
		r1.X = 0.0f;
		r1.Width = (Gdiplus::REAL)width;
		g->DrawString(wStr, wcslen(wStr), measure->font, r1, &measure->format, &measure->textBrush);
		
		r2.Y = 0.0f;
		r2.X = (Gdiplus::REAL)(width - r2.Width) / 2.0f;
		g->FillRectangle(&measure->whiteBrush, r2);
		
		r2.X = 0.0f;
		r2.Width = (Gdiplus::REAL)width;
		g->DrawString(wAz, wcslen(wAz), measure->font, r2, &measure->format, &measure->textBrush);
		g->SetTransform(&m);		// restore transform
	}
	delete wStr;
	delete wAz;
}

// ****************************************************************
//		DrawMeasuring()
// ****************************************************************
void CMapView::DrawMeasuring(Gdiplus::Graphics* g )
{
	// transparency
	// color
	// width
	// style
	// vertex size

	CMeasuring* measuring = ((CMeasuring*)m_measuring);
	if (measuring)
	{
		int size = measuring->points.size();
		if (size > 0 )
		{
			Gdiplus::SmoothingMode prevMode = g->GetSmoothingMode();
			Gdiplus::TextRenderingHint prevHint = g->GetTextRenderingHint();
			g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
			g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

			// drawing the last segment only
			if (measuring->measuringType == tkMeasuringType::MeasureDistance)  
			{
				/*Gdiplus::PointF* data = new Gdiplus::PointF[size];
				double x, y;
				for(size_t i = 0; i < measuring->points.size(); i++) {
					this->ProjectionToPixel(measuring->points[i]->Proj.x, measuring->points[i]->Proj.y, x, y);
					data[i].X = (Gdiplus::REAL)x;
					data[i].Y = (Gdiplus::REAL)y;
				}*/
				
				Gdiplus::PointF* data = NULL;
				int size = measuring->get_ScreenPoints((void*)this, false, 0, 0, &data);
				if (size > 0)
				{
					if (measuring->closedPoly)
					{
						int sz = measuring->points.size() - 1 - measuring->firstPointIndex;
						if (sz > 2 && measuring->firstPointIndex >= 0)
						{
							// let's draw fill and area
							Gdiplus::PointF* polyData = &(data[measuring->firstPointIndex]);
							Gdiplus::Color color(100, 255, 165, 0);
							Gdiplus::SolidBrush brush(color);
							g->FillPolygon(&brush, polyData, sz);
						}
					}

					double length, totalLength = 0.0;
					for(int i = 0; i < size - 1; i++) {
						measuring->get_SegementLength(i, &length);
						totalLength += length;
						DrawSegmentInfo(g, data[i].X, data[i].Y, data[i + 1].X, data[i + 1].Y, length, totalLength, i, measuring);
					}

					Gdiplus::Pen pen(Gdiplus::Color::Orange, 2.0f);
					g->DrawLines(&pen, data, size);
					
					// drawing points
					Gdiplus::Pen penPoints(Gdiplus::Color::Blue, 1.0f);
					Gdiplus::SolidBrush brush(Gdiplus::Color::LightBlue);
					for(int i = 0; i < size; i++) {
						g->FillEllipse(&brush, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
						g->DrawEllipse(&penPoints, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
					}

					if (measuring->closedPoly)
					{
						IPoint* pnt = GetMeasuringPolyCenter(data, size);
						if (pnt)
						{
							DrawMeasuringPolyArea(g, false, 0.0, 0.0, pnt);
							pnt->Release();
						}
					}
					delete[] data;
				}
			}
			else
			{
				// do nothing; it's being drawn in DrawMouseMoves
			}

			g->SetSmoothingMode(prevMode);
			g->SetTextRenderingHint(prevHint);
		}
	}
}

// ***************************************************************
//		DrawMouseMoves()
// ***************************************************************
void CMapView::DrawMouseMoves(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool isSnapShot, float offsetX, float offsetY) {
	HDC hdc = pdc->GetSafeHdc();
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
	g->TranslateTransform(offsetX, offsetY);
	//g->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);	

	DrawMouseMovesCore(g, isSnapShot);

	g->ReleaseHDC(pdc->GetSafeHdc());
	delete g;
}

void CMapView::DrawMouseMovesCore(Gdiplus::Graphics* g, bool isSnapShot) 
{
	// update measuring
	CMeasuring* m =(CMeasuring*)m_measuring;
	if( m_cursorMode == cmMeasure || m->persistent ) {
		Gdiplus::Graphics* gDrawing = NULL;
		if (!isSnapShot)
		{
			// drawing layers
			gDrawing = Gdiplus::Graphics::FromImage(m_drawingBitmap);			// allocate another bitmap for this purpose
			gDrawing->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
			gDrawing->DrawImage(m_bufferBitmap, 0.0f, 0.0f);
		}
		else
		{
			gDrawing = g;
		}

		if (m->points.size() > 0) 
		{
			double x, y;
			if (!m->IsStopped())
			{
				this->ProjectionToPixel(m->points[m->points.size() - 1]->Proj.x, m->points[m->points.size() - 1]->Proj.y, x, y);
			}
			
			if (m->measuringType == tkMeasuringType::MeasureDistance)  // drawing the last segment only
			{
				if (!m->IsStopped())
				{
					if (m->points.size() > 0 && (m->mousePoint.x != x || m->mousePoint.y != y)) 
					{
						Gdiplus::SmoothingMode prevMode = g->GetSmoothingMode();
						Gdiplus::TextRenderingHint prevHint = g->GetTextRenderingHint();
						gDrawing->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
						gDrawing->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

						double xLng, yLat;
						this->PixelToProj(m->mousePoint.x, m->mousePoint.y, &xLng, &yLat);
						if (m->TransformPoint(xLng, yLat))
						{
							double dist;
							IUtils* utils = GetUtils();
							utils->GeodesicDistance(m->points[m->points.size() - 1]->y, m->points[m->points.size() - 1]->x, yLat, xLng, &dist);
							DrawSegmentInfo(gDrawing, x, y, m->mousePoint.x, m->mousePoint.y, dist, 0.0, 1, m);
						}

						gDrawing->SetSmoothingMode(prevMode);
						gDrawing->SetTextRenderingHint(prevHint);
						
						Gdiplus::Pen pen(Gdiplus::Color::Orange, 2.0f);
						gDrawing->DrawLine(&pen, (Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)m->mousePoint.x, (Gdiplus::REAL)m->mousePoint.y);
					}
				}
			}
			else	// area measuring; drawing the whole shape
			{
				Gdiplus::PointF* data = NULL;
				int size = m->get_ScreenPoints((void*)this, !m->IsStopped(), (int)m->mousePoint.x, (int)m->mousePoint.y, &data);
				if (size > 0 )
				{
					// drawing points
					Gdiplus::Pen pen(Gdiplus::Color::Orange, 2.0f);

					Gdiplus::Color color(100, 255, 165, 0);
					Gdiplus::SolidBrush brush(color);

					gDrawing->FillPolygon(&brush, data, size);
					gDrawing->DrawPolygon(&pen, data, size);

					m->firstPointIndex = 0;		// the first one is actual start of the poly
					IPoint* pnt = GetMeasuringPolyCenter(data, size);
					if (pnt)
					{
						if (!m->IsStopped())
						{
							double xLng, yLat;
							this->PixelToProj(m->mousePoint.x, m->mousePoint.y, &xLng, &yLat);
							if (m->TransformPoint(xLng, yLat))
							{
								DrawMeasuringPolyArea(gDrawing, true, xLng, yLat, pnt);
							}
						}
						else
						{
							DrawMeasuringPolyArea(gDrawing, false, 0.0, 0.0, pnt);
						}
						pnt->Release();
					}
					delete[] data;
				}
			}
		}
		
		if (!isSnapShot)
		{
			g->DrawImage(m_drawingBitmap, 0.0f, 0.0f);
			delete gDrawing;
		}
	}
}

// ****************************************************************
//		GetMeasuringPolyCenter()
// ****************************************************************
IPoint* CMapView::GetMeasuringPolyCenter(Gdiplus::PointF* data, int length)
{
	IPoint* pnt = NULL;
	CMeasuring* measuring = ((CMeasuring*)m_measuring);
	if (measuring)
	{
		if (length > 2 && measuring->firstPointIndex >= 0)
		{
			// let's draw fill and area
			Gdiplus::PointF* polyData = &(data[measuring->firstPointIndex]);
		
			// find position for label
			IShape* shp = NULL;
			CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
			if (shp)
			{
				long pointIndex;
				VARIANT_BOOL vb;
				shp->Create(ShpfileType::SHP_POLYGON, &vb);
				
				int sz = measuring->measuringType == tkMeasuringType::MeasureDistance ? measuring->points.size() - 1 - measuring->firstPointIndex : length;
				for(int i = 0; i < sz; i++)
				{
					shp->AddPoint(polyData[i].X, polyData[i].Y, &pointIndex);
				}
				shp->AddPoint(polyData->X, polyData->Y, &pointIndex);	// close it
				
				shp->get_Centroid(&pnt);
				shp->Release();
			}
		}
	}
	return pnt;
}

void CMapView::DrawMeasuringPolyArea(Gdiplus::Graphics* g, bool lastPoint, double lastGeogX, double lastGeogY, IPoint* pnt)
{
	CMeasuring* measuring = ((CMeasuring*)m_measuring);
	if (measuring)
	{
		double xOrig, yOrig;
		pnt->get_X(&xOrig);
		pnt->get_Y(&yOrig);
		
		int sz = measuring->points.size() - 1 - measuring->firstPointIndex;
		if (lastPoint)
			sz++;
		
		if (sz > 1)
		{
			// copy geog coordinates
			int count = 0;
			std::vector<Point2D> gPoints;
			for(size_t i = measuring->firstPointIndex; i < measuring->points.size(); i++)
			{
				gPoints.push_back(Point2D(measuring->points[i]->x, measuring->points[i]->y));
				count++;
			}
			if (lastPoint)
				gPoints.push_back(Point2D(lastGeogX, lastGeogY));
			
			// calc area
			double area = abs(CalcPolyGeodesicArea(gPoints));

			// draw the label 
			CString str;
			str.Format("%.2f %s", area / 10000.0, m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsHectars));

			WCHAR* wStr = Utility::StringToWideChar(str);
			Gdiplus::Font* font = Utility::GetGdiPlusFont("Arial", 12);
			
			Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
			Gdiplus::SolidBrush whiteBrush(Gdiplus::Color::White);
			Gdiplus::PointF origin((Gdiplus::REAL)xOrig, (Gdiplus::REAL)yOrig);
			
			Gdiplus::StringFormat format;
			format.SetAlignment(Gdiplus::StringAlignmentCenter);
			format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

			Gdiplus::RectF box;
			g->MeasureString(wStr, wcslen(wStr), font, origin, &format, &box);
			g->FillRectangle(&whiteBrush, box);

			g->DrawString(wStr, wcslen(wStr), font, origin, &format, &brush);
			
			delete font;
			delete wStr;
		}
	}
}

// ****************************************************************
//		ShowRedrawTime()
// ****************************************************************
// Displays redraw time in the bottom left corner
void CMapView::ShowRedrawTime(Gdiplus::Graphics* g, float time, CStringW message )
{
	if (!m_ShowRedrawTime && !m_ShowVersionNumber)	return;

	// preparing canvas
	Gdiplus::SmoothingMode smoothing = g->GetSmoothingMode();
	Gdiplus::TextRenderingHint hint = g->GetTextRenderingHint();
	g->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	// initializing brushes
	Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
	Gdiplus::Pen pen(Gdiplus::Color::White, 3.0f);
	pen.SetLineJoin(Gdiplus::LineJoinRound);

	// initializing font
	Gdiplus::FontFamily family(L"Arial");
	Gdiplus::Font font(&family, (Gdiplus::REAL)12.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
	Gdiplus::PointF point(0.0f, 0.0f);
	Gdiplus::StringFormat format; 
	
	// vars
	CStringW s;
	Gdiplus::GraphicsPath path;
	Gdiplus::RectF rect;
	Gdiplus::Matrix mtx;
	
	Gdiplus::REAL width = (Gdiplus::REAL)m_viewWidth; //r.right - r.left;
	Gdiplus::REAL height;

	if (m_ShowVersionNumber)
	{
		s.Format(L"MapWinGIS %d.%d", _wVerMajor, _wVerMinor);
		path.StartFigure();
		path.AddString(s.GetString(), wcslen(s), &family, font.GetStyle(), font.GetSize(), point, &format);
		path.CloseFigure();
		path.GetBounds(&rect);
		height = rect.Height;
		
		if (rect.Width + 10 < width)		// control must be big enough to host the string
		{
			mtx.Translate((float)(m_viewWidth - rect.Width - 10), (float)(m_viewHeight - height - 10));
			path.Transform(&mtx);
			g->DrawPath(&pen, &path);
			g->FillPath(&brush, &path);
			width -= (rect.Width);
		}
	}
	
	path.Reset();
	mtx.Reset();

	if (m_ShowRedrawTime)
	{
		if (wcslen(message) != 0)
		{
			s = message;
		}
		else
		{
			s.Format(L"Redraw time: %.3f sec", time);
		}
		path.StartFigure();
		path.AddString(s.GetString(), wcslen(s), &family, font.GetStyle(), font.GetSize(), point, &format);
		path.CloseFigure();
		path.GetBounds(&rect);
		height = m_ShowVersionNumber?height:rect.Height + 3;
		if (rect.Width + 15 < width)		// control must be big enough to host the string
		{
			mtx.Translate(5.0f, (float)(m_viewHeight - height - 10));
			path.Transform(&mtx);
			g->DrawPath(&pen, &path);
			g->FillPath(&brush, &path);
			width -= (rect.Width + 15);
		}
	}
	
	g->SetTextRenderingHint(hint);
	g->SetSmoothingMode(smoothing);
}
#pragma endregion

#pragma region Multithreading
// ******************************************************************
//		InitMapRotation()
// ******************************************************************
//	Chnages the nessary variables to perform drawing with rotation
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

// A structure to pass parameters to the background thread
struct DrawingParams: CObject 
{
	Gdiplus::Graphics* graphics;
	const CRect* bounds;
	CMapView* map;

	DrawingParams(CMapView* m, Gdiplus::Graphics* g, const CRect* b)
	{
		graphics = g;
		bounds = b;
		map = m;
	};
};

// ***************************************************************
//		StartDrawLayers()
// ***************************************************************
// Starts drawing in the background thread
UINT CMapView::StartDrawLayers(LPVOID pParam)
{
	DrawingParams* options = (DrawingParams*)pParam;
	if (!options || !options->IsKindOf(RUNTIME_CLASS(DrawingParams)))
	{
		return 0;   // if pObject is not valid
	}
	else
	{
		this->DrawLayers(options->bounds, options->graphics);
		return 1;   // thread completed successfully
	}
}


UINT StartThread(LPVOID pParam)
{
	DrawingParams* options = (DrawingParams*)pParam;
	if (!options || !options->IsKindOf(RUNTIME_CLASS(DrawingParams)))
	{
		return 0;   // if pObject is not valid
	}
	else
	{
		CMapView* map = options->map;
		map->DrawLayers(options->bounds, options->graphics);
		return 1;   // thread completed successfully
	}
}
#pragma endregion
