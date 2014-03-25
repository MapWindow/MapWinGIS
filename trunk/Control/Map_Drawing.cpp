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
