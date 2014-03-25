#include "stdafx.h"
#include "map.h"
#include "GeoProjection.h"

// ****************************************************************
//		DrawStringWithShade()
// ****************************************************************
void DrawStringWithShade(Gdiplus::Graphics* g, CStringW s, Gdiplus::Font *font, Gdiplus::PointF &point, 
						 Gdiplus::Brush *brush, Gdiplus::Brush *brushOutline)
{
	g->DrawString(s.GetString(), s.GetLength(), font, point, brushOutline);
	point.X -= 1.0f;
	point.Y -= 1.0f;
	g->DrawString(s.GetString(), s.GetLength(), font, point, brush);
}

// ****************************************************************
//		FormatUnits()
// ****************************************************************
CStringW FormatUnits(CStringW& s, double step, double power, int count)
{
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
	return s;
}

// ****************************************************************
//		ChooseScalebarUnits()
// ****************************************************************
void ChooseScalebarUnits(tkUnitsOfMeasure sourceUnits, tkUnitsOfMeasure& targetUnits, double distance, 
						 double& unitsPerPixel, double& step, int& count, double& power)
{
	if (targetUnits == umMiles)
	{
		if (distance < 1)
		{
			targetUnits = umFeets;
			unitsPerPixel *= 5280.0;		// feets in mile
			distance *= 5280.0;
		}
	}
	else	// meters
	{
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
	}
	
	// ----------------------------------------------------------
	//    Choosing a step
	// ----------------------------------------------------------
	power = floor(log10(distance));
	step = pow(10, floor(log10(distance)));
	count = (int)floor(distance/step);

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

	if (count > 5)
	{
		step *= 2.5;
		count = (int)floor(distance/step);
	}
}

// ****************************************************************
//		GetLocalizedUnitsText()
// ****************************************************************
CStringW GetLocalizedUnitsText(tkUnitsOfMeasure units)
{
	switch(units)
	{
		case umMiles:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMiles);
		case umFeets:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsFeet);
		case umMeters:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMeters);
		case umKilometers:
			return m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsKilometers);
		default:
			USES_CONVERSION;
			return A2W(Utility::GetUnitOfMeasureText(units));
	}
}

// ****************************************************************
//		DrawScaleBar()
// ****************************************************************
void CMapView::DrawScaleBar(Gdiplus::Graphics* g)
{
	int barWidth = 140;
	int barHeight = 30;
	int yPadding = 10;
	int xPadding = 10;
	int xOffset = 20;
	int yOffset = 10;
	int segmHeight = 5;

	if (m_viewWidth <= barWidth + xOffset || m_viewHeight <= barHeight + yOffset)		// control must be big enough
		return;
	
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

	double xMin = extents.left;
	double yMin = extents.top;
	double xMax = extents.right;
	double yMax = extents.bottom;
	
	double width = extents.right - extents.left;
	tkUnitsOfMeasure units = m_unitsOfMeasure;

	// ----------------------------------------------------------
	//    Geodesic distance calculation
	// ----------------------------------------------------------
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
					((CGeoProjection*)m_projection)->SetIsFrozen(false);
					m_projection->StartTransform(m_wgsProjection, &vb);
					((CGeoProjection*)m_projection)->SetIsFrozen(true);
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
		// ----------------------------------------------------------
		//    Choosing units
		// ----------------------------------------------------------
		double unitsPerPixel, unitsPerPixel2;
		double step, power; int count;
		double step2, power2; int count2;
		double newWidth = width;

		// metric calculations are performed for mixed mode as well
		tkUnitsOfMeasure targetUnits = m_scalebarUnits == American ? umMiles : umMeters;		
		Utility::ConvertDistance(units, targetUnits, newWidth);
		unitsPerPixel = newWidth/(maxX - minX);	  // target units on screen size
		double distance = (barWidth - xPadding * 2) * unitsPerPixel;
		ChooseScalebarUnits(units, targetUnits, distance, unitsPerPixel, step, count, power);
		
		tkUnitsOfMeasure targetUnits2 = umMiles;
		if (m_scalebarUnits == GoogleStyle)
		{
			// now do calculation for miles
			newWidth = width;
			Utility::ConvertDistance(units, umMiles, newWidth);
			unitsPerPixel2 = newWidth/(maxX - minX);	  // target units on screen size
			distance = (barWidth - xPadding * 2) * unitsPerPixel2;
			ChooseScalebarUnits(units, targetUnits2, distance, unitsPerPixel2, step2, count2, power2);
		}
		
		// ----------------------------------------------------------
		//    Initialize drawing
		// ----------------------------------------------------------
		Gdiplus::Matrix mtx;
		mtx.Translate((float)5, (float)m_viewHeight - barHeight - yOffset);
		g->SetTransform(&mtx);

		Gdiplus::SolidBrush brushBlack(Gdiplus::Color::Black);
		Gdiplus::SolidBrush brushOutline(Gdiplus::Color::White);

		Gdiplus::RectF rect(0.0f, 0.0f, (Gdiplus::REAL)barWidth, (Gdiplus::REAL)barHeight );

		// ----------------------------------------------------------
		//    Drawing of segments
		// ----------------------------------------------------------
		std::vector<Gdiplus::Rect*> parts;
		int length = (int)(step * count / unitsPerPixel + xPadding);

		if (m_scalebarUnits != GoogleStyle)
		{
			// horizontal line
			parts.push_back(new Gdiplus::Rect(xPadding, barHeight - yPadding, length - xPadding, 0));
			
			// inner measures (shorter)
			for (int i = 0; i <= count; i++ )
			{
				length = (int)(step * i / unitsPerPixel + xPadding);
				int valHeight = (i == 0 || i == count) ? segmHeight * 2 : segmHeight;	// the height of the mark; side marks are longer
				parts.push_back(new Gdiplus::Rect(length, barHeight - yPadding - valHeight, 0, valHeight));
			}
		}
		else
		{
			int length2 = (int)(step2 * count2 / unitsPerPixel2 + xPadding);
			
			// horizontal line
			parts.push_back(new Gdiplus::Rect(xPadding, barHeight/2 - yPadding, length - xPadding, 0));
			parts.push_back(new Gdiplus::Rect(xPadding, barHeight/2 - yPadding, length2 - xPadding, 0));

			// vertical lines for metric
			int valHeight = segmHeight * 2;
			parts.push_back(new Gdiplus::Rect(xPadding, barHeight/2 - yPadding - valHeight, 0, valHeight));
			parts.push_back(new Gdiplus::Rect(length,   barHeight/2 - yPadding - valHeight, 0, valHeight));

			// vertical lines for USA
			parts.push_back(new Gdiplus::Rect(xPadding,	barHeight/2 - yPadding, 0, valHeight));
			parts.push_back(new Gdiplus::Rect(length2,  barHeight/2 - yPadding, 0, valHeight));
		}

		for(size_t i = 0; i < parts.size(); i++)				
		{
			g->FillRectangle(&brushOutline, parts[i]->X - 2, parts[i]->Y - 2, parts[i]->Width + 4, parts[i]->Height + 4);
		}

		for(size_t i = 0; i < parts.size(); i++)				
		{
			g->FillRectangle(&brushBlack, parts[i]->X - 1, parts[i]->Y - 1, parts[i]->Width + 2, parts[i]->Height + 2);
		}

		for(size_t i = 0; i < parts.size(); i++)				
		{
			delete parts[i];
		}
		parts.clear();

		// ----------------------------------------------------------
		//    Drawing of text
		// ----------------------------------------------------------
		Gdiplus::TextRenderingHint hint = g->GetTextRenderingHint();
		g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
		Gdiplus::FontFamily family(L"Arial");
		Gdiplus::REAL fontSize = m_scalebarUnits != GoogleStyle? 10.0f : 8.0f;
		Gdiplus::Font font(&family, fontSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
		CStringW s;
		Gdiplus::StringFormat format; 

		if (m_scalebarUnits != GoogleStyle)
		{
			s.Format(L"0");
			Gdiplus::PointF point(xPadding + 4.0f, -4.0f);
			DrawStringWithShade(g, s, &font, point, &brushBlack, &brushOutline);
			
			// max
			FormatUnits(s, step, power, count);
			point.X = (Gdiplus::REAL)(step * count/unitsPerPixel + xPadding + 3 + 1.0f);
			point.Y = -4.0f;
			DrawStringWithShade(g, s, &font, point, &brushBlack, &brushOutline);
			
			// units
			s = GetLocalizedUnitsText(targetUnits);
			point.X = (Gdiplus::REAL)(step * count/unitsPerPixel + xPadding + 3 + 1.0f);
			point.Y = (Gdiplus::REAL)(barHeight - yPadding - 12 + 1.0f);
			DrawStringWithShade(g, s, &font, point, &brushBlack, &brushOutline);
		}
		else
		{
			// metric
			USES_CONVERSION;
			s.Format(L"%s %s", FormatUnits(s, step, power, count), GetLocalizedUnitsText(targetUnits));
			Gdiplus::PointF point(xPadding + 8.0f, -10.0f);
			DrawStringWithShade(g, s, &font, point, &brushBlack, &brushOutline);
			
			// miles
			s.Format(L"%s %s", FormatUnits(s, step2, power2, count2), GetLocalizedUnitsText(targetUnits2));
			point.Y = 8.0f;
			DrawStringWithShade(g, s, &font, point, &brushBlack, &brushOutline);
		}
		g->SetTextRenderingHint(hint);
		g->ResetTransform();
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