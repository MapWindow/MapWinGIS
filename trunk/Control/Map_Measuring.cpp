#include "stdafx.h"
#include "map.h"
#include "Utils.h"

#pragma region DrawMouseMoves
// ***************************************************************
//		DrawMouseMoves()
// ***************************************************************
void CMapView::DrawMouseMoves(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool drawBackBuffer, float offsetX, float offsetY) 
{
	HDC hdc = pdc->GetSafeHdc();
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
	g->TranslateTransform(offsetX, offsetY);

	Gdiplus::Graphics* gTemp = NULL;
	if (drawBackBuffer)
	{
		// creating a temp buffer, and passing content of main buffer to it;
		// so that all the resulting stuff can passed to screen in one take, without flickering
		gTemp = Gdiplus::Graphics::FromImage(m_drawingBitmap);
		gTemp->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
		gTemp->DrawImage(m_bufferBitmap, 0.0f, 0.0f);
	}
	else
	{
		// main buffer was already drawn, so we don't care about flickering (snapshot operation),
		// so can draw on the output canvas directly
		gTemp = g;
	}
	
	DrawMeasuringToScreenBuffer(gTemp);

	DrawCoordinatesToScreenBuffer(gTemp);
	
	if (drawBackBuffer)
	{
		// it's not snapshot so we are using temp buffer to avoid flickering;
		// let's pass it to the screen here;
		g->DrawImage(m_drawingBitmap, 0.0f, 0.0f);
		delete gTemp;
	}

	g->ReleaseHDC(pdc->GetSafeHdc());
	delete g;
}

// ****************************************************************
//		DrawCoordinatesToScreenBuffer()
// ****************************************************************
void CMapView::DrawCoordinatesToScreenBuffer(Gdiplus::Graphics* g) 
{
	if(HasDrawingData(tkDrawingDataAvailable::Coordinates)) 
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(&p);
			double x = p.x, y = p.y;
			double prX, prY;
			PixelToProjection(x, y, prX, prY);

			bool canUseDegrees = m_transformationMode == tmWgs84Complied;
			if (m_transformationMode == tmDoTransformation && (_showCoordinates == cdmDegrees || _showCoordinates == cdmAuto))
			{
				IGeoProjection* p = GetMapToWgs84Transform();
				if (p) {
					VARIANT_BOOL vb;
					p->Transform(&prX, &prY, &vb);
					if (vb) canUseDegrees = true;
				}
			}

			if (_showCoordinates == cdmDegrees && !canUseDegrees) {
				// can't display degrees
			}
			else {
				CStringW s;
				if (canUseDegrees) {
					s.Format(L"%s: %.3f; %s: %.3f", m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsLatitude), prY, 
													m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsLongitude), prX);
				}
				else
				{
					s.Format(L"x=%.2f; y=%.2f", prX, prY);
				}
				Gdiplus::PointF point(0.0f, 0.0f);
				Gdiplus::RectF rect;
				
				g->MeasureString(s, s.GetLength(), _font, point, Gdiplus::StringFormat::GenericDefault(), &rect);
				if (rect.Width + 15 < m_viewWidth)		// control must be big enough to host the string
				{
					point.X = 7.0f;
					point.Y = 7.0f;
					DrawStringWithShade(g, s, _font, point, &((CMeasuring*)m_measuring)->textBrush, &((CMeasuring*)m_measuring)->whiteBrush);
				}
			}
		}
	}
}

// ****************************************************************
//		DrawMeasuringToScreenBuffer()
// ****************************************************************
// - last point under mouse cursor for distance measuring is drawn here
// - all points of area measuring
// All the data will be redrawn on each mouse move
void CMapView::DrawMeasuringToScreenBuffer(Gdiplus::Graphics* g) 
{
	if (HasDrawingData(tkDrawingDataAvailable::MeasuringData))
	{
		CMeasuring* m =(CMeasuring*)m_measuring;
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
					g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
					g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

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
					DrawSegmentInfo(g, x, y, m->mousePoint.x, m->mousePoint.y, dist, 0.0, 1, m);

					g->SetSmoothingMode(prevMode);
					g->SetTextRenderingHint(prevHint);
					
					Gdiplus::Pen pen(Gdiplus::Color::Orange, 2.0f);
					g->DrawLine(&pen, (Gdiplus::REAL)x, (Gdiplus::REAL)y, (Gdiplus::REAL)m->mousePoint.x, (Gdiplus::REAL)m->mousePoint.y);
				}
			}
		}
		else	
		{
			// area measuring; drawing the whole shape
			Gdiplus::PointF* data = NULL;
			int size = m->get_ScreenPoints((void*)this, !m->IsStopped(), (int)m->mousePoint.x, (int)m->mousePoint.y, &data);
			if (size > 0 )
			{
				// drawing points
				Gdiplus::Pen pen(Gdiplus::Color::Orange, 2.0f);

				Gdiplus::Color color(100, 255, 165, 0);
				Gdiplus::SolidBrush brush(color);

				g->FillPolygon(&brush, data, size);
				g->DrawPolygon(&pen, data, size);

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
							DrawMeasuringPolyArea(g, true, xLng, yLat, pnt);
						}
						else
						{
							DrawMeasuringPolyArea(g, false, 0.0, 0.0, pnt);
						}
						pnt->Release();
					}
				}
				delete[] data;
			}
		}
	}
}

// ****************************************************************
//		DrawMeasuringToMainBuffer()
// ****************************************************************
// - distance measuring is drawn here (all points but the last under mouse cursor); it won't be readrawn on mouse move
void CMapView::DrawMeasuringToMainBuffer(Gdiplus::Graphics* g )
{
	// TODO: perhaps add more rendering options: transparency, color, width, style, vertex size
	if (HasDrawingData(tkDrawingDataAvailable::MeasuringData))
	{
		CMeasuring* measuring = ((CMeasuring*)m_measuring);
		switch(measuring->measuringType)	
		{
			case tkMeasuringType::MeasureArea:
				// it's drawn outside the main buffer (DrawMouseMoves)
				break;
			case tkMeasuringType::MeasureDistance:
				// all but the last segment are drawn here
				Gdiplus::PointF* data = NULL;
				int size = measuring->get_ScreenPoints((void*)this, false, 0, 0, &data);
				if (size > 0)
				{
					Gdiplus::SmoothingMode prevMode = g->GetSmoothingMode();
					Gdiplus::TextRenderingHint prevHint = g->GetTextRenderingHint();
					g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
					g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
					
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

					g->SetSmoothingMode(prevMode);
					g->SetTextRenderingHint(prevHint);
				}
				break;
		}
	}
}
#pragma endregion

#pragma region Drawing utilities

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
		// no projection; display map units
		USES_CONVERSION;
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
#pragma endregion