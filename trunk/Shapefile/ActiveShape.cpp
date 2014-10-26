#include "stdafx.h"
#include "ActiveShape.h"
#include "GeometryHelper.h"
#include "Shape.h"
#include "GdiPlusHelper.h"
#include "CollisionList.h"

// *******************************************************
//		SetMapCallback()
// *******************************************************
void ActiveShape::SetMapCallback(IMapViewCallback* mapView, ShapeInputMode inputMode)
{
	GeoShape::SetMapCallback(mapView);
	_inputMode = inputMode;
}

// *******************************************************
//		Clear()
// *******************************************************
void ActiveShape::Clear()
{
	_selectedPart = -1;
	_selectedVertex = -1;
	
	GeoShape::Clear();
}

// *******************************************************
//		get_ScreenPoints()
// *******************************************************
int ActiveShape::GetScreenPoints(int partIndex, MixedShapePart whichPoints, Gdiplus::PointF** data, 
								  bool dynamicPoint, DraggingOperation offsetType, int offsetX, int offsetY)
{
	int size = GetScreenPoints(partIndex, whichPoints, dynamicPoint, (int)_mousePoint.x, (int)_mousePoint.y, data);

	Gdiplus::PointF* points = *data;
	if (offsetType == DragMoveShape || (offsetType == DragMovePart && partIndex == _selectedPart))
	{
		for(int i= 0; i < size; i++)
		{
			points[i].X += offsetX;
			points[i].Y += offsetY;
		}
	}
	else if (offsetType == DragMovePart && partIndex == -1) 
	{
		// rendering of a poly as a whole; adjust only the necessary part
		int startIndex = GetPartStart(_selectedPart);
		int endIndex = SeekPartEnd(startIndex);

		if (startIndex != -1 && endIndex != -1)
		{
			for (int i = startIndex; i <= endIndex; i++)
			{
				points[i].X += offsetX;
				points[i].Y += offsetY;
			}
		}
	}
	return size;
}

// *******************************************************
//		GetPartStartAndEnd()
// *******************************************************
bool ActiveShape::GetPartStartAndEnd(int partIndex, MixedShapePart whichPoints, int& startIndex, int& endIndex)
{
	startIndex = 0;
	endIndex = _points.size();		// actually the next after end one

	int partCount = GetNumParts();
	if (partCount > 1 && partIndex != -1)
	{
		int count = -1;
		startIndex = -1;
		for (size_t i = 0; i < _points.size(); i++)
		{
			if (startIndex != -1 && _points[i]->Part == PartEnd) {
				endIndex = i + 1;
				break;
			}
			if (_points[i]->Part == PartBegin)
			{
				count++;
				if (count == partIndex)
					startIndex = i;
			}
		}
		return startIndex != -1;
	}
	return true;
}

// *******************************************************
//		get_ScreenPoints()
// *******************************************************
int ActiveShape::GetScreenPoints(int partIndex, MixedShapePart whichPoints, bool hasLastPoint, int lastX, int lastY, 
	Gdiplus::PointF** data)
{
	int startIndex, endIndex;
	GetPartStartAndEnd(partIndex, whichPoints, startIndex, endIndex);

	int size = endIndex - startIndex;
	int totalSize = size;
	if (hasLastPoint) totalSize++;

	if (size > 0)
	{
		(*data) = new Gdiplus::PointF[totalSize];
		Gdiplus::PointF* temp = *data;
		double x, y;
		for(int i = 0; i < size; i++) {
			ProjToPixel(_points[startIndex + i]->Proj.x, _points[startIndex + i]->Proj.y, x, y);
			temp[i].X = (Gdiplus::REAL)x;
			temp[i].Y = (Gdiplus::REAL)y;
		}
		if (hasLastPoint)
		{
			temp[totalSize - 1].X = (Gdiplus::REAL)lastX;
			temp[totalSize - 1].Y = (Gdiplus::REAL)lastY;
		}
	}
	return totalSize;
}

// ****************************************************************
//		DrawData()
// ****************************************************************
void ActiveShape::DrawData( Gdiplus::Graphics* g, bool dynamicBuffer, 
		DraggingOperation offsetType, int screenOffsetX /*= 0*/, int screenOffsetY /*= 0*/ )
{
	bool hasLine = HasLine(dynamicBuffer);
	bool hasPolygon = HasPolygon(dynamicBuffer); 
	if (!hasLine && !hasPolygon) return;

	_fillBrush.SetColor(Utility::OleColor2GdiPlus(FillColor, FillTransparency)) ;
	_linePen.SetWidth(LineWidth);
	_linePen.SetColor(Utility::OleColor2GdiPlus(LineColor, 255));

	Gdiplus::PointF* polyData = NULL;
	int polySize = 0;

	if (hasPolygon)
	{
		if (!_drawLabelsOnly)
		{
			Gdiplus::GraphicsPath path;
			path.SetFillMode(Gdiplus::FillModeWinding);
			
			int partCount = GetNumParts();
			for (int n = 0; n < partCount; n++)
			{
				polySize = GetScreenPoints(n, PolygonPart, &polyData, dynamicBuffer, offsetType, screenOffsetX, screenOffsetY);
				if (polySize > 1)
				{
					path.AddPolygon(polyData, polySize);
				}
			}
			g->FillPath(&_fillBrush, &path);
			
			if (_inputMode == simMeasuring) {   // to display closing line
				g->DrawPath(&_linePen, &path);
			}
		}
	}

	if (hasLine)
	{
		Gdiplus::GraphicsStateHelper state;
		state.SetSmoothingMode(g, Gdiplus::SmoothingModeHighQuality);
		state.SetTextRenderingHint(g, Gdiplus::TextRenderingHintAntiAlias);
	
		int partCount = GetNumParts();

		for (int n = 0; n < partCount; n++)
		{
			Gdiplus::PointF* data = NULL;
			int size = GetScreenPoints(n, LinearPart, &data, false, offsetType, screenOffsetX, screenOffsetY);
			if (size > 0)
			{
				DrawLines(g, size, data, dynamicBuffer, n);
			}
		}

		state.RestoreSmoothingMode(g);
		state.RestoreTextRenderingHint(g);
	}
	
	// draw area atop of everything else
	if (polySize > 0)
	{
		DrawPolygonArea(g, polyData, polySize, dynamicBuffer);
		delete[] polyData;
	}
}

// ****************************************************************
//		DrawLines()
// ****************************************************************
void ActiveShape::DrawLines(Gdiplus::Graphics* g, int size, Gdiplus::PointF* data, bool dynamicBuffer, int partIndex)
{
	double length = 0.0, totalLength = 0.0;

	int startIndex = GetPartStart(partIndex);
	bool editing = this->GetInputMode() == simEditing;

	long errorCode = tkNO_ERROR;
	for (int i = 0; i < size - 1; i++) 
	{
		int realIndex = startIndex + i;
		length = GetSegmentLength(realIndex, errorCode);
		totalLength += length;
		DrawSegmentInfo(g, data[i].X, data[i].Y, data[i + 1].X, data[i + 1].Y, length, totalLength, i, false);
	}

	Gdiplus::Pen* pen = partIndex != -1 && (_selectedPart == partIndex || _highlightedPart == partIndex ) ? &_redPen : &_linePen;
	if (!_drawLabelsOnly)
		g->DrawLines(pen, data, size);

	// drawing points
	CCollisionList collisionList;
	CStringW s;
	for (int i = 0; i < size; i++)
	{
		int realIndex = startIndex + i;

		if (_verticesVisible)
		{
			if (realIndex == _selectedVertex || realIndex == _highlightedVertex)
			{
				g->DrawRectangle(&_redPen, (int)(data[i].X - 3.0f + 0.5f), (int)(data[i].Y - 3.0f + 0.5f), 6, 6);
			}
			else
			{
				Gdiplus::SolidBrush* brush = realIndex == _selectedVertex ? &_redBrush : &_blueBrush;
				Gdiplus::Pen* pen = realIndex == _selectedVertex ? &_redPen : &_bluePen;

				if (i == 0 || editing)
				{
					g->DrawRectangle(&_bluePen, (int)(data[i].X - 3.0f + 0.5f), (int)(data[i].Y - 3.0f + 0.5f), 6, 6);
				}
				else
				{
					g->FillEllipse(&_blueBrush, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
					g->DrawEllipse(&_bluePen, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
				}
			}
		}

		// --------------------------------------
		// display vertex index
		// --------------------------------------
		if (_pointLabelsVisible)
		{
			int index = realIndex + 1;
			Gdiplus::RectF bounds;
			s.Format(L"%d", index);
			Gdiplus::PointF pntTemp(data[i].X + 10, data[i].Y);
			g->MeasureString(s, s.GetLength(), _font, pntTemp, &bounds);

			if (i > 0 && i < size - 1)
			{
				double angle = GetBearingLabelAngle(realIndex, AngleDisplayMode == CounterClockwiseBearing);

				double dx = sin(angle / 180.0 * pi_) * 15;
				double dy = -cos(angle / 180.0 * pi_) * 15;

				bounds.X = (Gdiplus::REAL)(data[i].X + dx - bounds.Width / 2);
				bounds.Y = (Gdiplus::REAL)(data[i].Y + dy - bounds.Height / 2);
			}

			CRect r((int)bounds.GetLeft(), (int)bounds.GetTop(), (int)bounds.GetRight(), (int)bounds.GetBottom());
			if (!collisionList.HaveCollision(r)) {
				g->FillRectangle(&_whiteBrush, bounds);
				g->DrawString(s, s.GetLength(), _font, bounds, &_format, &_textBrush);
				collisionList.AddRectangle(&r, 10, 10);
			}
		}
	}

	// drawing the last segment only
	if (dynamicBuffer)
	{
		double x, y;
		ProjToPixel(_points[_points.size() - 1]->Proj.x, _points[_points.size() - 1]->Proj.y, x, y);

		if (_points.size() > 0 && (_mousePoint.x != x || _mousePoint.y != y))
		{
			double dist = GetDynamicLineDistance();
			DrawSegmentInfo(g, x, y, _mousePoint.x, _mousePoint.y, dist, 0.0, -1, false);
			g->DrawLine(&_linePen, (Gdiplus::REAL)x, (Gdiplus::REAL)y,
				(Gdiplus::REAL)_mousePoint.x, (Gdiplus::REAL)_mousePoint.y);
		}
	}

	delete[] data;
}

// ****************************************************************
//		DrawPolygonArea()
// ****************************************************************
void ActiveShape::DrawPolygonArea(Gdiplus::Graphics* g, Gdiplus::PointF* data, int size, bool dynamicPoly)
{
	IPoint* pnt = GetPolygonCenter(data, size);
	if (pnt)
	{
		double projX, projY;
		PixelToProj(_mousePoint.x, _mousePoint.y, projX, projY);
		double area = GetArea(dynamicPoly, projX, projY);
		DrawMeasuringPolyArea(g, pnt, area);
		pnt->Release();
	}
}

// ****************************************************************
//		DrawMeasuringPolyArea()
// ****************************************************************
void ActiveShape::DrawMeasuringPolyArea(Gdiplus::Graphics* g, IPoint* pnt, double area)
{
	double xOrig, yOrig;
	pnt->get_X(&xOrig);
	pnt->get_Y(&yOrig);

	CStringW str;

	if (HasProjection())
	{
		// draw the label
		switch(AreaDisplayMode)
		{
			case admMetric:
				{
					if (area < 1000.0)
					{
						str.Format(L"%.1f %s", area, m_globalSettings.GetLocalizedString(lsSquareMeters));
					}
					else if(area < 10000000.0)
					{
						area /= 10000.0;
						str.Format(L"%.2f %s", area, m_globalSettings.GetLocalizedString(lsHectars));
					}
					else
					{
						area /= 1000000.0;
						str.Format(L"%.2f %s", area, m_globalSettings.GetLocalizedString(lsSquareKilometers));
					}
				}
				break;
			case admHectars:
				{
					area /= 10000.0;
					/*CString sArea = Utility::FormatNumber(area, _areaRounding);
					USES_CONVERSION;
					str.Format(L"%s %s", A2W(sArea), m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsHectars));*/
				}
				break;
			case admNone:
				return;
		}
	}
	else
	{
		str.Format(L"%.2f %s", abs(area), m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsSquareMapUnits));
	}
	
	Gdiplus::PointF origin((Gdiplus::REAL)xOrig, (Gdiplus::REAL)yOrig);

	Gdiplus::RectF box;
	g->MeasureString(str, str.GetLength(), _fontArea, origin, &_format, &box);

	g->FillRectangle(&_whiteBrush, box);
	g->DrawString(str, str.GetLength(), _fontArea, origin, &_format, &_textBrush);
}

// ***************************************************************
//		DrawSegmentInfo()
// ***************************************************************
void ActiveShape::DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, double length, 
								 double totalLength, int segmentIndex, bool rumbOnly)
{
	CStringW s1, s2, s, sAz;

	double x = xScr - xScr2;
	double y = yScr - yScr2;

	double angle = 360.0 - GeometryHelper::GetPointAngle(x, y) * 180.0 / pi_;
	
	x = -x;
	y = -y;
	angle = GeometryHelper::GetPointAngle(x, y) * 180.0 / pi_;
	angle = - (angle - 90.0);

	if (HasProjection())
	{
		CStringW m = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMeters);
		CStringW km = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsKilometers);

		CStringW format = L"%.";
		CStringW temp;
		temp.Format(L"%d", _lengthRounding);
		format += temp;
		format += "f %s";

		if (length > 1000.0)
		{
			s1.Format(format, length / 1000.0, km);
		}
		else
		{
			s1.Format(format, length, m);
		}

		if (segmentIndex != 0 && totalLength != 0.0 && DrawAccumalatedLength())
		{
			if (totalLength > 1000.0)
			{
				s2.Format(format, totalLength / 1000.0, km);
			}
			else
			{
				s2.Format(format, totalLength, m);
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
		if (segmentIndex != 0 && totalLength != 0.0)
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
	g->MeasureString(s, s.GetLength(), _font, Gdiplus::PointF(0.0f,0.0f), &_format, &r1);
	g->MeasureString(sAz, sAz.GetLength(), _font, Gdiplus::PointF(0.0f,0.0f), &_format, &r2);

	double width = sqrt(x*x + y*y);	// width must not be longer than width of segment
	if (r1.Width > width && segmentIndex != 0 && totalLength != 0.0)
	{
		// if a segment is too short, let's try to display it without total distance
		g->MeasureString(s1, s1.GetLength(), _font, Gdiplus::PointF(0.0f,0.0f), &_format, &r1);
		s = s1;
	}

	if (width >= r1.Width * 1.5)		// TODO: 1.5 = padding; use parameter
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

		// ------------------------------------
		// drawing length
		// ------------------------------------
		if (!rumbOnly && _lengthDisplayMode != ldmNone)
		{
			r1.X = (Gdiplus::REAL)(width - r1.Width) / 2.0f;
			r1.Y = -r1.Height - 2;
			//r1.Y = upsideDown ? -r1.Height : 0;
			//r1.Y =  upsideDown ? 0.0f : -r1.Height;
			g->FillRectangle(&_whiteBrush, r1);

			r1.X = 0.0f;
			r1.Width = (Gdiplus::REAL)width;
			g->DrawString(s, s.GetLength(), _font, r1, &_format, &_textBrush);
		}
	
		// ------------------------------------
		// drawing directional angles
		// ------------------------------------
		bool bearing = (AngleDisplayMode == ClockwiseBearing || AngleDisplayMode == CounterClockwiseBearing);
		if (DisplayAngles && !bearing)
		{
			r2.X = (Gdiplus::REAL)(width - r2.Width) / 2.0f;
			r2.Y = 2.0f;
			//r2.Y =  upsideDown ? 0.0f : -r1.Height;
			//r2.Y = upsideDown ? -r2.Height : 0;
			g->FillRectangle(&_whiteBrush, r2);

			r2.X = 0.0f;
			r2.Width = (Gdiplus::REAL)width;
			g->DrawString(sAz, sAz.GetLength(), _font, r2, &_format, &_textBrush);
		}
		g->SetTransform(&m);		// restore transform

		// ------------------------------------
		// drawing bearings
		// ------------------------------------
		if (DisplayAngles && bearing && segmentIndex > 0 && segmentIndex < (int)_points.size() - 1)
		{
			angle = GetBearingLabelAngle(segmentIndex, AngleDisplayMode == ClockwiseBearing) - 90.0;

			upsideDown = false;
			g->GetTransform(&m);
			g->TranslateTransform((Gdiplus::REAL)xScr, (Gdiplus::REAL)yScr);
			if (angle > 90.0)
			{
				g->RotateTransform((Gdiplus::REAL)(angle - 180.0));
				g->TranslateTransform((Gdiplus::REAL)-r2.Width, (Gdiplus::REAL)0.0f);
				upsideDown = true;
			}
			else {
				g->RotateTransform((Gdiplus::REAL)angle);
			}

			r2.X = upsideDown ? - r2.Height : r2.Height;
			r2.Y = -r2.Height / 2.0f; 
			g->FillRectangle(&_whiteBrush, r2);
			g->DrawString(sAz, sAz.GetLength(), _font, r2, &_format, &_textBrush);

			g->SetTransform(&m);		// restore transform
		}
	}
}

// ***************************************************************
//		HandlePointAdd()
// ***************************************************************
bool ActiveShape::HandlePointAdd( double screenX, double screenY, bool ctrl )
{
	double projX, projY;

	int closePointIndex= -1;
	bool closeOnPreviousVertex = ctrl && CloseOnPreviousVertex();
	
	if ( closeOnPreviousVertex )
	{
		// closing to show area
		if (SnapToPreviousVertex(closePointIndex, screenX, screenY))
		{
			if (closePointIndex != -1)
			{
				projX = _points[closePointIndex]->Proj.x;
				projY = _points[closePointIndex]->Proj.y;
			}
			else
			{
				// for types other than polygons we add another point rather than closing
				PixelToProj( screenX, screenY, projX, projY );
			}
		}
		else
		{
			// no points to close near by
			return false;
		}
	}
	else
	{
		PixelToProj( screenX, screenY, projX, projY );
	}

	AddPoint(projX, projY, screenX, screenY);
	
	UpdatePolyCloseState(true, closePointIndex);

	return true;
}

// *******************************************************
//		AddPoint()
// *******************************************************
void ActiveShape::AddPoint(double xProj, double yProj, double xScreen, double yScreen, PointPart part)
{
	ClearIfStopped();

	_mousePoint.x = xScreen;
	_mousePoint.y = yScreen;

	MeasurePoint* pnt = new MeasurePoint();
	pnt->Proj.x = xProj;
	pnt->Proj.y = yProj;
	pnt->Part = part;
	_points.push_back(pnt);

	UpdateLatLng(_points.size() - 1);

	_areaRecalcIsNeeded = true;
}

// *******************************************************
//		AddPoint()
// *******************************************************
void ActiveShape::AddPoint(double xProj, double yProj)
{
	double xScreen, yScreen;
	ProjToPixel(xProj, yProj, xScreen, yScreen);
	AddPoint(xProj, yProj, xScreen, yScreen);
}

// **************************************************************
//		UndoPoint()
// **************************************************************
bool ActiveShape::UndoPoint()
{
	bool result = false;
	if (_points.size() > 0) 
	{
		delete _points[_points.size() - 1];
		result = true;
		_points.pop_back();
		_areaRecalcIsNeeded = true;
	}
	UpdatePolyCloseState(false);
	return result;
}