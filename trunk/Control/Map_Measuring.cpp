#include "stdafx.h"
#include "map.h"
#include "Utils.h"

// ***************************************************************
//		DrawMouseMoves()
// ***************************************************************
void CMapView::DrawMouseMoves(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool isSnapShot, float offsetX, float offsetY) 
{
	HDC hdc = pdc->GetSafeHdc();
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
	g->TranslateTransform(offsetX, offsetY);

	DrawMouseMovesCore(g, isSnapShot);

	g->ReleaseHDC(pdc->GetSafeHdc());
	delete g;
}

// ***************************************************************
//		DrawMouseMovesCore()
// ***************************************************************
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

						double dist;
						double xLng, yLat;
						this->PixelToProj(m->mousePoint.x, m->mousePoint.y, &xLng, &yLat);
						if (m->TransformPoint(xLng, yLat))
						{
							GetUtils()->GeodesicDistance(m->points[m->points.size() - 1]->y, m->points[m->points.size() - 1]->x, yLat, xLng, &dist);
						}
						else
						{
							dist = m->points[m->points.size() - 1]->Proj.GetDistance(xLng, yLat);
						}
						DrawSegmentInfo(gDrawing, x, y, m->mousePoint.x, m->mousePoint.y, dist, 0.0, 1, m);

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
					if (size > 2)
					{
						IPoint* pnt = GetMeasuringPolyCenter(data, size);
						if (pnt)
						{
							if (!m->IsStopped())
							{
								double xLng, yLat;
								this->PixelToProj(m->mousePoint.x, m->mousePoint.y, &xLng, &yLat);
								m->TransformPoint(xLng, yLat);
								//if (m->TransformPoint(xLng, yLat))
								//{
								DrawMeasuringPolyArea(gDrawing, true, xLng, yLat, pnt);
								//}
							}
							else
							{
								DrawMeasuringPolyArea(gDrawing, false, 0.0, 0.0, pnt);
							}
							pnt->Release();
						}
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

				// make sure that centroid lies within extents of shapes; otherwise place it at the center
				IExtents* ext = NULL;
				shp->get_Extents(&ext);

				double x, y;
				pnt->get_X(&x);
				pnt->get_Y(&y);

				ext->PointIsWithin(x, y, &vb);
				if (!vb)
				{
					pnt->Release();
					pnt = NULL;
					ext->get_Center(&pnt);
				}
				shp->Release();
			}
		}
	}
	return pnt;
}

// ****************************************************************
//		DrawMeasuringPolyArea()
// ****************************************************************
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
			
			CStringW str;

			if (measuring->HasProjection())
			{
				// calc area
				double area = abs(CalcPolyGeodesicArea(gPoints));

				// draw the label
				if (area < 1000.0)
				{
					str.Format(L"%.1f %s", area, m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsSquareMeters));
				}
				else if(area < 10000000.0)
				{
					area /= 10000.0;
					str.Format(L"%.2f %s", area, m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsHectars));
				}
				else
				{
					area /= 1000000.0;
					str.Format(L"%.2f %s", area, m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsSquareKilometers));
				}
			}
			else
			{
				double area;
				measuring->get_AreaWithClosingVertex(lastGeogX, lastGeogY, &area);
				str.Format(L"%.2f %s", abs(area), m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsSquareMapUnits));
			}

			Gdiplus::Font* font = Utility::GetGdiPlusFont("Arial", 12);
			
			Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
			Gdiplus::SolidBrush whiteBrush(Gdiplus::Color::White);
			Gdiplus::PointF origin((Gdiplus::REAL)xOrig, (Gdiplus::REAL)yOrig);
			
			Gdiplus::StringFormat format;
			format.SetAlignment(Gdiplus::StringAlignmentCenter);
			format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

			Gdiplus::RectF box;
			g->MeasureString(str, str.GetLength(), font, origin, &format, &box);
			g->FillRectangle(&whiteBrush, box);

			g->DrawString(str, str.GetLength(), font, origin, &format, &brush);

			delete font;
		}
	}
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

					if (measuring->closedPoly && size > 2)
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
//		DrawSegmentInfo()
// ***************************************************************
void CMapView::DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, double length, 
					 double totalLength, int segmentIndex, CMeasuring* measure)
{
	CStringW s1, s2, s, sAz;

	double x = xScr - xScr2;
	double y = yScr - yScr2;
	
	double angle = 360.0 - GetPointAngle(x, y) * 180.0 / pi;							
	sAz.Format(L"%.1f°", angle);
	
	x = -x;
	y = -y;
	angle = GetPointAngle(x, y) * 180.0 / pi;							
	angle = - (angle - 90.0);

	if (measure->HasProjection())
	{
		CStringW m = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMeters);
		CStringW km = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsKilometers);

		if (length > 1000.0)
		{
			s1.Format(L"%.2f %s", length / 1000.0, km);
		}
		else
		{
			s1.Format(L"%.1f %s", length, m);
		}
		
		if (segmentIndex > 0 && totalLength != 0.0)
		{
			if (totalLength > 1000.0)
			{
				s2.Format(L"%.2f %s", totalLength / 1000.0, km);
			}
			else
			{
				s2.Format(L"%.1f %s", totalLength, m);
			}
			s.Format(L"%s (%s)", s1, s2);
		}
		else
		{
			s = s1;
		}
	}
	else
	{
		// no projection; displaye map units
		USES_CONVERSION;
		//A2W(Utility::GetUnitOfMeasureText(m_unitsOfMeasure));
		CStringW mu = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMapUnits);	
		s1.Format(L"%.1f %s", length, mu);
		if (segmentIndex > 0 && totalLength != 0.0)
		{
			s2.Format(L"%.1f %s", totalLength, mu);
			s.Format(L"%s (%s)", s1, s2);
		}
		else
		{
			s = s1;
		}
	}

	Gdiplus::RectF r1(0.0f, 0.0f, 0.0f, 0.0f);
	Gdiplus::RectF r2(0.0f, 0.0f, 0.0f, 0.0f);
	g->MeasureString(s, s.GetLength(), measure->font, Gdiplus::PointF(0.0f,0.0f), &measure->format, &r1);
	
	g->MeasureString(sAz, sAz.GetLength(), measure->font, Gdiplus::PointF(0.0f,0.0f), &measure->format, &r2);
	
	double width = sqrt(x*x + y*y);	// width must not be longer than width of segment
	if (r1.Width > width && segmentIndex > 0 && totalLength != 0.0)
	{
		// if a segment is too short, let's try to display it without total distance
		g->MeasureString(s1, s1.GetLength(), measure->font, Gdiplus::PointF(0.0f,0.0f), &measure->format, &r1);
		s = s1;
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
		g->DrawString(s, s.GetLength(), measure->font, r1, &measure->format, &measure->textBrush);
		
		if (measure->displayAngles)
		{
			r2.Y = 0.0f;
			r2.X = (Gdiplus::REAL)(width - r2.Width) / 2.0f;
			g->FillRectangle(&measure->whiteBrush, r2);
		
			r2.X = 0.0f;
			r2.Width = (Gdiplus::REAL)width;
			g->DrawString(sAz, sAz.GetLength(), measure->font, r2, &measure->format, &measure->textBrush);
		}
		g->SetTransform(&m);		// restore transform
	}
}