#include "stdafx.h"
#include "map.h"
#include "TileProviders.h"
#include "GdiPlusHelper.h"
#include "GraphicsStateHelper.h"

// ****************************************************************
//		DrawStringWithShade()
// ****************************************************************
void CMapView::DrawStringWithShade(Gdiplus::Graphics* g, CStringW s, Gdiplus::Font *font, Gdiplus::PointF &point, 
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
	if (power >= 1)
	{
		s.Format(L"%d",(int)(step * count));
	}
	else
	{
		CStringW sFormat;
		if (power == 0.0) power = -1;
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
			unitsPerPixel *= 5280.0;		// feet in mile
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
//		DrawScaleBar()
// ****************************************************************
void CMapView::DrawScaleBar(Gdiplus::Graphics* g)
{
	if (!_scalebarVisible) return;

	int barWidth = 140;
	int barHeight = 30;
	int yPadding = 10;
	int xPadding = 10;
	int xOffset = 20;
	int yOffset = 10;
	int segmHeight = 5;

	if (_viewWidth <= barWidth + xOffset || _viewHeight <= barHeight + yOffset)		// control must be big enough
		return;
	
	if (_transformationMode != tkTransformationMode::tmNotDefined)
	{
		int zoom = GetCurrentZoom();
		if (zoom >= 0 && zoom < 3) {
			// lsu: there are some problems with displaying scale bar at such zoom levels: 
			// - there are areas outside the globe where coordinate transformations may fail;
			// - the points at the left and right sides of the screen may lie on the same meridian
			// so geodesic distance across the screen will be 0;
			// - finally projection distortions change drastically by Y axis across map so
			// the scalebar will be virtually meaningless;
			// The easy solution will be simply not to show scale bar at such small scales
			return;
		}
	}
	
	double minX, maxX, minY, maxY;	// size of map control in pixels
    PROJECTION_TO_PIXEL(_extents.left, _extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(_extents.right, _extents.top, maxX, maxY);

	double xMin = _extents.left;
	double yMin = _extents.top;
	double xMax = _extents.right;
	double yMax = _extents.bottom;
	
	double width = _extents.right - _extents.left;
	tkUnitsOfMeasure units = _unitsOfMeasure;

	// ----------------------------------------------------------
	//    Geodesic distance calculation
	// ----------------------------------------------------------
	if (_transformationMode != tkTransformationMode::tmNotDefined)
	{
		// skip calculations when extents haven't changed
		if (this->_lastWidthMeters == 0.0)
		{
			bool skipTransform = false;
			if (_transformationMode == tkTransformationMode::tmDoTransformation)
			{
				VARIANT_BOOL vb1, vb2;
				IGeoProjection* projTemp = GetMapToWgs84Transform();
				if (projTemp)
				{
					projTemp->Transform(&xMin, &yMin, &vb1);
					projTemp->Transform(&xMax, &yMax, &vb2);
					if (!vb1 || !vb2)
					{
						skipTransform = true;
					}
				}
				else {
					skipTransform = true;
				}
			}
		
			if ( !skipTransform)
			{
				GetUtils()->GeodesicDistance((yMax + yMin)/2, xMin, (yMax + yMin)/2, xMax, &width);
				_lastWidthMeters = width;
				units = tkUnitsOfMeasure::umMeters;
			}
		}
		else
		{
			width = _lastWidthMeters;
			units = tkUnitsOfMeasure::umMeters;
		}
	}

	if (width <= 0.0) return;

	
	// ----------------------------------------------------------
	//    Choosing units
	// ----------------------------------------------------------
	double unitsPerPixel, unitsPerPixel2;
	double step, power; int count;
	double step2, power2; int count2;
	double newWidth = width;

	// metric calculations are performed for mixed mode as well
	tkUnitsOfMeasure targetUnits = _scalebarUnits == American ? umMiles : umMeters;		
	Utility::ConvertDistance(units, targetUnits, newWidth);
	unitsPerPixel = newWidth/(maxX - minX);	  // target units on screen size
	double distance = (barWidth - xPadding * 2) * unitsPerPixel;
	ChooseScalebarUnits(units, targetUnits, distance, unitsPerPixel, step, count, power);

	tkUnitsOfMeasure targetUnits2 = umMiles;
	if (_scalebarUnits == GoogleStyle)
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
	mtx.Translate((float)5, (float)_viewHeight - barHeight - yOffset);
	g->SetTransform(&mtx);
	Gdiplus::PixelOffsetMode pixelOffsetMode = g->GetPixelOffsetMode();
	g->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);

	Gdiplus::RectF rect(0.0f, 0.0f, (Gdiplus::REAL)barWidth, (Gdiplus::REAL)barHeight);

	// ----------------------------------------------------------
	//    Drawing of segments
	// ----------------------------------------------------------
	std::vector<Gdiplus::Rect*> parts;
	int length = (int)(step * count / unitsPerPixel + xPadding);

	if (_scalebarUnits != GoogleStyle)
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
		g->FillRectangle(_brushWhite, parts[i]->X - 2, parts[i]->Y - 2, parts[i]->Width + 4, parts[i]->Height + 4);
	}

	for(size_t i = 0; i < parts.size(); i++)				
	{
		g->FillRectangle(_brushBlack, parts[i]->X - 1, parts[i]->Y - 1, parts[i]->Width + 2, parts[i]->Height + 2);
	}

	for(size_t i = 0; i < parts.size(); i++)				
	{
		delete parts[i];
	}
	parts.clear();

	g->SetPixelOffsetMode(pixelOffsetMode);

	// ----------------------------------------------------------
	//    Drawing of text
	// ----------------------------------------------------------
	Gdiplus::TextRenderingHint hint = g->GetTextRenderingHint();
	g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
	Gdiplus::FontFamily family(L"Arial");
	Gdiplus::REAL fontSize = _scalebarUnits != GoogleStyle? 10.0f : 8.0f;
	Gdiplus::Font font(&family, fontSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
	CStringW s;
	Gdiplus::StringFormat format; 

	if (_scalebarUnits != GoogleStyle)
	{
		s.Format(L"0");
		Gdiplus::PointF point(xPadding + 4.0f, -4.0f);
		DrawStringWithShade(g, s, &font, point, _brushBlack, _brushWhite);
			
		// max
		FormatUnits(s, step, power, count);
		point.X = (Gdiplus::REAL)(step * count/unitsPerPixel + xPadding + 3 + 1.0f);
		point.Y = -4.0f;
		DrawStringWithShade(g, s, &font, point, _brushBlack, _brushWhite);
			
		// units
		s = Utility::GetLocalizedUnitsText(targetUnits);
		point.X = (Gdiplus::REAL)(step * count/unitsPerPixel + xPadding + 3 + 1.0f);
		point.Y = (Gdiplus::REAL)(barHeight - yPadding - 12 + 1.0f);
		DrawStringWithShade(g, s, &font, point, _brushBlack, _brushWhite);
	}
	else
	{
		// metric
		USES_CONVERSION;
		s.Format(L"%s %s", FormatUnits(s, step, power, count), Utility::GetLocalizedUnitsText(targetUnits));
		Gdiplus::PointF point(xPadding + 8.0f, -10.0f);
		DrawStringWithShade(g, s, &font, point, _brushBlack, _brushWhite);
			
		// miles
		s.Format(L"%s %s", FormatUnits(s, step2, power2, count2), Utility::GetLocalizedUnitsText(targetUnits2));
		point.Y = 8.0f;
		DrawStringWithShade(g, s, &font, point, _brushBlack, _brushWhite);
	}
	g->SetTextRenderingHint(hint);
	g->ResetTransform();
}

// ****************************************************************
//		ShowRedrawTime()
// ****************************************************************
// Displays redraw time in the bottom right corner
void CMapView::ShowRedrawTime(Gdiplus::Graphics* g, float time, bool layerRedraw, CStringW message )
{
	_copyrightRect = Gdiplus::RectF(0.0f, 0.0f, 0.0f, 0.0f);

	bool showRedrawTime = _showRedrawTime && time > 0.01 && !_isSnapshot;
	bool showVersionNumber = _showVersionNumber && !_isSnapshot;
	
	CStringW s;
	tkTileProvider provider = GetTileProvider();
	if (provider != tkTileProvider::ProviderNone && _transformationMode != tmNotDefined)
	{
		CComPtr<ITileProviders> providers = NULL;
		_tiles->get_Providers(&providers);
		s = ((CTileProviders*)&(*providers))->get_CopyrightNotice(provider);
	}

	if (!showRedrawTime && !showVersionNumber && s.GetLength() == 0) return;

	POINT mousePnt;
	if (GetCursorPos(&mousePnt))
		ScreenToClient(&mousePnt);

	Gdiplus::GraphicsStateHelper gstate;
	gstate.SetTextRenderingHint(g, Gdiplus::TextRenderingHintAntiAliasGridFit);
	Gdiplus::PointF point(0.0f, 0.0f);
	Gdiplus::StringFormat format; 

	if (s.GetLength() > 0)
	{
		g->MeasureString(s, s.GetLength(), _fontCourierLink, point, &format, &_copyrightRect);
		_copyrightRect.Height += 5;
		_copyrightRect.Width += 5;
		
		if (_copyrightRect.Width < _viewWidth)		// control must be big enough to host the string
		{
			point.X = (float)(_viewWidth - _copyrightRect.Width);
			point.Y = (float)(_viewHeight - _copyrightRect.Height);
			_copyrightRect.X += point.X;
			_copyrightRect.Y += point.Y;

			//bool active = _copyrightRect.Contains((Gdiplus::REAL)mousePnt.x, (Gdiplus::REAL)mousePnt.y);
			Gdiplus::SolidBrush* textBrush = _copyrightLinkActive ? _brushBlue : _brushBlack;
			Gdiplus::Font* font = _copyrightLinkActive ? _fontCourierLink : _fontCourierSmall;
			
			format.SetAlignment(Gdiplus::StringAlignmentCenter);
			format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
			g->FillRectangle(_brushLightGray, _copyrightRect);
			g->DrawString(s.GetString(), s.GetLength(), font, _copyrightRect, &format, textBrush);
		}
	}

	if (showVersionNumber)
	{
		USES_CONVERSION;
		Gdiplus::RectF rect;
		s.Format(L"MapWinGIS %s", OLE2W(GetVersionNumber()));
		g->MeasureString(s, s.GetLength(), _fontCourierSmall, point, &format, &rect);
		
		if (rect.Width < _viewWidth)	  	// 	control must be	big	enough to host the string
		{
			point.X = (float)(_viewWidth - rect.Width);
			point.Y = (float)(_viewHeight - rect.Height - _copyrightRect.Height - 3);
			DrawStringWithShade(g, s, _fontCourierSmall, point, _brushBlack, _brushWhite);
		}
	}

	if (showRedrawTime)
	{
		if (message.GetLength() != 0)
		{
			s = message;
		}
		else
		{
			s.Format(L"Redraw time: %.3fs", time);
		}

		Gdiplus::RectF rect;
		g->MeasureString(s, s.GetLength(), _fontCourier, point, &format, &rect);
		if (rect.Width + 15 < _viewWidth)		// control must be big enough to host the string
		{
			point.X = (float)(_viewWidth - rect.Width - 10);
			point.Y = _showCoordinates != cdmNone ? 10.0f + rect.Height : 10.0f;
			DrawStringWithShade(g, s, _fontCourier, point, _brushBlack, _brushWhite);
		}
	}
	gstate.RestoreTextRenderingHint(g);
}

#pragma region Zoombar

// ****************************************************************
//		ZoombarHitTest()
// ****************************************************************
ZoombarPart CMapView::ZoombarHitTest(int x, int y)
{
	if (!_zoombarVisible || _transformationMode == tmNotDefined)
		return ZoombarPart::ZoombarNone;
	
	POINT pnt;
	pnt.x = x; pnt.y = y;
	
	// plus button
	if (_zoombarParts.PlusButton.PtInRect(pnt))
		return ZoombarPart::ZoombarPlus;

	// minus button
	if (_zoombarParts.MinusButton.PtInRect(pnt))
		return ZoombarPart::ZoombarMinus;

	// handle
	if (_zoombarParts.Handle.PtInRect(pnt))
		return ZoombarPart::ZoombarHandle;

	// bar
	if (_zoombarParts.Bar.PtInRect(pnt))
		return ZoombarPart::ZoombarBar;

	return ZoombarPart::ZoombarNone;
}

// ****************************************************************
//		DrawGradientShadowForPath()
// ****************************************************************
void DrawGradientShadowForPath(Gdiplus::GraphicsPath& path, Gdiplus::Graphics* g)
{
	bool enableShadow = false;   // doesn't look well; skip it for now
	if (enableShadow)
	{
		float shadowOffset = 2.0f;
		g->TranslateTransform(0.0f, shadowOffset);

		// drawing shadow
		Gdiplus::GraphicsPath* outline = path.Clone();
		Gdiplus::Pen p2(Gdiplus::Color::Gray, 4.0f);
		outline->Widen(&p2);
		Gdiplus::PathGradientBrush pathBrush(outline);
		pathBrush.SetWrapMode(Gdiplus::WrapMode::WrapModeClamp);
		Gdiplus::Color colors[3];
		colors[0] = Gdiplus::Color::Transparent;
		colors[1] = Gdiplus::Color(180, 0, 0, 0);
		colors[2] = Gdiplus::Color(180, 0, 0, 0);

		float positions[3];
		positions[0] = 0.0f;
		positions[1] = 0.7f;
		positions[2] = 1.0f;
		pathBrush.SetInterpolationColors(&(colors[0]), &(positions[0]), 3);
		g->FillPath(&pathBrush, outline);

		g->TranslateTransform(0, -shadowOffset);
	}
}

// ****************************************************************
//		DrawZoombar()
// ****************************************************************
void CMapView::DrawZoombar(Gdiplus::Graphics* g) 
{
	if (!_zoombarVisible || _transformationMode == tmNotDefined)
		return;

	//  mouse position
	POINT pnt;
	if (GetCursorPos(&pnt))
		ScreenToClient(&pnt);

	float boxSize = 17.0f;
	float cornerRadius = 5.0f;
	
	Gdiplus::GraphicsPath path;
    Gdiplus::RectF bounds(0.0f,0.0f,boxSize,boxSize);
    path.AddArc(bounds.X + bounds.Width - cornerRadius, bounds.Y, cornerRadius, cornerRadius, 270.0f, 90.0f);
    path.AddArc(bounds.X + bounds.Width - cornerRadius, bounds.Y + bounds.Height - cornerRadius, cornerRadius, cornerRadius, 0.0f, 90.0f);
    path.AddArc(bounds.X, bounds.Y + bounds.Height - cornerRadius, cornerRadius, cornerRadius, 90.0f, 90.0f);
	path.AddArc(bounds.X, bounds.Y, cornerRadius, cornerRadius, 180.0f, 90.0f);
    path.CloseAllFigures();

	int maxZoom, minZoom;
	GetMinMaxZoom(minZoom, maxZoom);

	float notchStep = 7;
	float lineOffset = 8.0f;
	float lineHeight = (maxZoom - minZoom) * notchStep;
	float x = 15.0f, y = 15.0f;
	float height =lineHeight + boxSize + 2 * lineOffset;
	
	Gdiplus::Matrix m;
	g->GetTransform(&m);

	// ---------------------------------------------------
	//	 Plus/minus sings
	// ---------------------------------------------------
	Gdiplus::PixelOffsetMode pixelOffsetMode = g->GetPixelOffsetMode();

	// upper plus button
	_zoombarParts.PlusButton.SetRect((int)x, (int)y, (int)(x + boxSize), (int)(y + boxSize));
	BOOL highlight = _zoombarParts.PlusButton.PtInRect(pnt);
	g->TranslateTransform(x, y);
	DrawGradientShadowForPath(path, g);
	g->FillPath(_brushWhite, &path);
	g->DrawPath( highlight ? _penDarkGray : _penGray, &path);
	g->SetTransform(&m);

	// lower minus button
	_zoombarParts.MinusButton.SetRect((int)x, (int)(y + height), (int)(x + boxSize), (int)(y + height + boxSize));
	highlight = _zoombarParts.MinusButton.PtInRect(pnt);
	g->TranslateTransform(x, y + height);
	DrawGradientShadowForPath(path, g);
	g->FillPath(_brushWhite, &path);
	g->DrawPath(highlight ? _penDarkGray : _penGray, &path);
	g->SetTransform(&m);

	// plus sign
	g->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
	g->FillRectangle(_brushGray, x + 5.0f, y + 8.0f, 8.0f, 2.0f);
	g->FillRectangle(_brushGray, x + 8.0f, y + 5.0f, 2.0f, 8.0f);

	// minus sign
	g->FillRectangle(_brushGray, x + 5.0f, y + height + 8.0f, 8.0f, 2.0f);

	g->SetPixelOffsetMode(pixelOffsetMode);

	// ---------------------------------------------------
	//	 Handle position
	// ---------------------------------------------------
	float lineWidth = 5.0f;
	
	float handleY = 0.0f;
	bool handleHighlight = false;
	int zoom = this->GetCurrentZoom();
	int targetZoom = -1;					// for handle dragging

	bool zooming = _dragging.Operation == DragZoombarHandle;
	if (zooming)
	{
		// take y from current mouse position
		handleY = (float)_dragging.Move.y;
		
		// make sure that we are within bar
		float minY = y + boxSize + lineOffset;
		float maxY = minY + lineHeight;
		if (handleY < minY) handleY = minY;
		if (handleY > maxY) handleY = maxY;

		// calculate current zoom
		float ratio = (handleY - minY) / (maxY - minY);
		targetZoom = (int)Utility::Rint(maxZoom - (maxZoom - minZoom) * ratio);
		_zoombarTargetZoom = targetZoom;

		// backward calculation to snap to the level
		float position = (float)((maxZoom - targetZoom) / ((double)(maxZoom - minZoom)) * lineHeight);
		handleY = (float)(int)(y + boxSize + lineOffset + position - boxSize/4.0f + 0.5);

		// adjust for the height of handle
		//handleY -= boxSize/4.0f;
		handleY = (float)Utility::Rint(handleY);
		
		handleHighlight = true;
		_zoombarParts.Handle.SetRect((int)x, (int)handleY, (int)(x + boxSize), (int)(handleY + bounds.Height));
	}
	else
	{
		// display current zoom
		if (zoom > maxZoom) zoom = maxZoom;
		if (zoom < minZoom) zoom = minZoom;
		float position = (float)((maxZoom - zoom) / ((double)(maxZoom - minZoom)) * lineHeight);
		handleY = (float)(int)(y + boxSize + lineOffset + position - boxSize/4.0f + 0.5);
		_zoombarParts.Handle.SetRect((int)x, (int)handleY, (int)(x + boxSize), (int)(handleY + bounds.Height));
		handleHighlight = _zoombarParts.Handle.PtInRect(pnt) ? true : false;
	}

	// ---------------------------------------------------
	//	 Tooltip drawing
	// ---------------------------------------------------
	// if handle is highlighted (i.e. mouse cursor is within it, draw a tooltip)
	if (handleHighlight)
	{
		if (_zoomBarVerbosity != zbvNone)
		{
			double scale = GetCurrentScale();
			if (zooming)
			{
				// it's troublesome to calculate target scale directly, therefore use simple proportion
				double ratio = pow(2.0, (double)(zoom - targetZoom));
				scale = scale * ratio;
			}
			double resoultion = scale / 96.0 * 0.0254;	// meters per pixel
			
			CStringW s;
			switch(_zoomBarVerbosity)
			{
				case zbvFull:
					s.Format(L"Zoom: %d\nScale: 1:%.0f\nResolution: %.2f", zooming ? targetZoom : zoom, scale, resoultion);
					break;
				case zbvZoomOnly:
					s.Format(L"Zoom: %d", zooming ? targetZoom : zoom);
					break;
			}
			
			float tooltipOffset = 15.0f;
			Gdiplus::PointF tooltipOrigin(x + boxSize + tooltipOffset, handleY);
			Gdiplus::RectF tooltipBox;
			g->MeasureString(s, s.GetLength(), _fontArial, tooltipOrigin, &tooltipBox);
			Gdiplus::SolidBrush tooltipBrush(Gdiplus::Color(255, 255, 255)/*Gdiplus::Color(246, 212, 178 )*/);
			tooltipBox.Inflate(9.0f, 5.0f);
			g->FillRectangle(&tooltipBrush, tooltipBox);
			g->DrawRectangle(_penGray, tooltipBox);
			tooltipBox.Inflate(-9.0f, -5.0f);
			g->DrawString(s, s.GetLength(), _fontArial, tooltipOrigin, _brushBlack );
		}
	}

	// ---------------------------------------------------
	//	 Bar with notches
	// ---------------------------------------------------
	Gdiplus::RectF line(x + boxSize/2.0f - lineWidth/2.0f, y + boxSize + lineOffset, lineWidth, lineHeight);
	_zoombarParts.Bar.SetRect((int)x, (int)line.Y, (int)(x + boxSize), (int)(line.Y + line.Height));  // include the width of handle; not the actual line only
	highlight = _zoombarParts.Bar.PtInRect(pnt);
	
	Gdiplus::GraphicsPath path3;
	path3.AddRectangle(line);
	DrawGradientShadowForPath(path3, g);
	
	g->FillRectangle(_brushWhite, line);
	g->DrawRectangle(highlight && !handleHighlight ? _penDarkGray : _penGray, line);

	
	// notches for zoom levels
	g->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
	float step = lineHeight / (maxZoom - minZoom);
	float tempX  = x + boxSize/2.0f - lineWidth/2.0f + 1;
	for (int i = 1; i < maxZoom - minZoom; i++)
	{
		float tempY = (float)(int)(y + boxSize + lineOffset + step * i + 0.5);
		g->FillRectangle(_brushGray, tempX, tempY, 4.0f, 1.0f);
	}
	g->SetPixelOffsetMode(pixelOffsetMode);

	// ---------------------------------------------------
	//	 Drawing of handle
	// ---------------------------------------------------
	// shape of handle
	Gdiplus::GraphicsPath path2;
	bounds.Height = 9.0f;
	path2.AddArc(bounds.X, bounds.Y, cornerRadius, cornerRadius, 180, 90);
    path2.AddArc(bounds.X + bounds.Width - cornerRadius, bounds.Y, cornerRadius, cornerRadius, 270, 90);
    path2.AddArc(bounds.X + bounds.Width - cornerRadius, bounds.Y + bounds.Height - cornerRadius, cornerRadius, cornerRadius, 0, 90);
    path2.AddArc(bounds.X, bounds.Y + bounds.Height - cornerRadius, cornerRadius, cornerRadius, 90, 90);
    path2.CloseAllFigures();

	// drawing the handle
	g->TranslateTransform(x, handleY);		// boxSize/4.0 = center of handle is the position
	DrawGradientShadowForPath(path2, g);
	g->FillPath(_brushWhite, &path2);
	g->DrawPath(handleHighlight ? _penDarkGray : _penGray, &path2);
	g->SetTransform(&m);

	// minus sign
	g->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
	g->FillRectangle(_brushGray, x + 5.0f, handleY + 4.0f, 8.0f, 2.0f);
	g->SetPixelOffsetMode(pixelOffsetMode);
	_penDarkGray->SetWidth(1.0f);
}

// ****************************************************************
//		GetMinMaxZoom()
// ****************************************************************
bool CMapView::GetMinMaxZoom( int& minZoom, int& maxZoom )
{
	_tiles->get_MinZoom(&minZoom);
	_tiles->get_MaxZoom(&maxZoom);

	if (_zoomBarMinZoom + 1 <= _zoomBarMaxZoom)
	{
		if (_zoomBarMinZoom != -1) minZoom = _zoomBarMinZoom;
		if (_zoomBarMaxZoom != -1) maxZoom = _zoomBarMaxZoom;
	}

	int min;
	if (GetTileMismatchMinZoom(min)) {
		minZoom = min;
	}
	return true;
}

// ****************************************************************
//		GetTileMismatchMinZoom()
// ****************************************************************
bool CMapView::GetTileMismatchMinZoom( int& minZoom )
{
	VARIANT_BOOL sphericalMercator;
	_tiles->get_ProjectionIsSphericalMercator(&sphericalMercator);

	if (_tileProjectionState == ProjectionDoTransform && sphericalMercator)
	{
		minZoom = m_globalSettings.tilesMaxZoomOnProjectionMismatch;
		return true;
	}
	else 
	{
		return false;
	}
}
#pragma endregion
