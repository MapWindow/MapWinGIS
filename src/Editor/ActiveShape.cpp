#include "stdafx.h"
#include "ActiveShape.h"
#include "GeometryHelper.h"
#include "Shape.h"
#include "GdiPlusHelper.h"
#include "CollisionList.h"
#include "AngleHelper.h"
#include "GraphicsStateHelper.h"

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
	if (offsetType == DragMoveShape || (offsetType == DragMovePart && PartIsSelected(partIndex)))
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
	int partCount = GetNumParts();

	if (hasPolygon)
	{
		if (!_drawLabelsOnly)
		{
			Gdiplus::GraphicsPath path;
			path.SetFillMode(Gdiplus::FillModeWinding);
			
			for (int n = 0; n < partCount; n++)
			{
				polySize = GetScreenPoints(n, PolygonPart, &polyData, dynamicBuffer, offsetType, screenOffsetX, screenOffsetY);
				if (polySize > 1)
				{
					path.AddPolygon(polyData, polySize);
				}

				// for a single part shapes we potentially want to display area
				if (partCount > 1 && polySize > 0) {
					delete[] polyData;
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

		CCollisionList collisionList;

		for (int n = 0; n < partCount; n++)
		{
			Gdiplus::PointF* data = NULL;
			int size = GetScreenPoints(n, LinearPart, &data, false, offsetType, screenOffsetX, screenOffsetY);
			if (size > 0)
			{
				DrawLines(g, size, data, dynamicBuffer, n, collisionList);
			}
		}

		state.RestoreSmoothingMode(g);
		state.RestoreTextRenderingHint(g);
	}
	
	// draw area atop of everything else
	if (partCount == 1 && polySize > 0)
	{
		DrawPolygonArea(g, polyData, polySize, dynamicBuffer);
		delete[] polyData;
	}

	// finally draw the snap point position if needed
	if (_showSnapPoint) {
		g->DrawRectangle(&_redPen, (int)(_snapPointX - 3.0f + 0.5f), (int)(_snapPointY - 3.0f + 0.5f), 6, 6);
	}
}

// ****************************************************************
//		DrawLines()
// ****************************************************************
void ActiveShape::DrawLines(Gdiplus::Graphics* g, int size, Gdiplus::PointF* data, bool dynamicBuffer, int partIndex, CCollisionList& collisionList)
{
	double length = 0.0, totalLength = 0.0;

	int startIndex = GetPartStart(partIndex);
	bool editing = this->GetInputMode() == simEditing;
	bool multiPoint = GetShapeType2D() == SHP_MULTIPOINT;
	long errorCode = tkNO_ERROR;
	
	if (!multiPoint) 
	{
		for (int i = 0; i < size - 1; i++) 
		{
			int realIndex = startIndex + i;
			length = GetSegmentLength(realIndex, errorCode);
			totalLength += length;
			DrawSegmentInfo(g, data[i].X, data[i].Y, data[i + 1].X, data[i + 1].Y, length, totalLength, i);
		}
	}

	Gdiplus::Pen* pen = partIndex != -1 && (_selectedPart == partIndex || _highlightedPart == partIndex ) ? &_redPen : &_linePen;

	if (OverlayTool)
	{
		_linePen.SetDashStyle(Gdiplus::DashStyleCustom);
		Gdiplus::REAL dashValues[4] = { 8, 8 };
		_linePen.SetDashPattern(dashValues, 2);
	}
	else
	{
		_linePen.SetDashStyle((Gdiplus::DashStyle)LineStyle);
	}

	if (!_drawLabelsOnly && !multiPoint) 
	{
		g->DrawLines(pen, data, size);
	}

	// drawing points
	
	CStringW s;
	for (int i = 0; i < size; i++)
	{
		int realIndex = startIndex + i;

		if (VerticesAreVisible())
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
		if (PointLabelsAreVisible())
		{
			int index = realIndex + 1;
			Gdiplus::RectF bounds;
			s.Format(L"%d", index);
			Gdiplus::PointF pntTemp(data[i].X + 10, data[i].Y);
			g->MeasureString(s, s.GetLength(), _font, pntTemp, &bounds);

			if (i > 0 && i < size - 1)
			{
				double angle = GetBearingLabelAngle(realIndex, BearingType == btLeftAngle);

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
	if (dynamicBuffer && !multiPoint)
	{
		double x, y;
		ProjToPixel(_points[_points.size() - 1]->Proj.x, _points[_points.size() - 1]->Proj.y, x, y);

		if (_points.size() > 0 && (_mousePoint.x != x || _mousePoint.y != y))
		{
			double dist = GetDynamicLineDistance();
			DrawSegmentInfo(g, x, y, _mousePoint.x, _mousePoint.y, dist, 0.0, -1);
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
	if (!GetShowArea()) return;

	IPoint* pnt = GetPolygonCenter(data, size);
	if (pnt)
	{
		double projX, projY;
		PixelToProj(_mousePoint.x, _mousePoint.y, projX, projY);
		double area = GetArea(dynamicPoly, projX, projY);
		DrawPolygonArea(g, pnt, area);
		pnt->Release();
	}
}

// ****************************************************************
//		DrawPolygonArea()
// ****************************************************************
void ActiveShape::DrawPolygonArea(Gdiplus::Graphics* g, IPoint* pnt, double area)
{
	double xOrig, yOrig;
	pnt->get_X(&xOrig);
	pnt->get_Y(&yOrig);

	CStringW sArea = Utility::FormatArea(area, !HasProjection(), AreaDisplayMode, AreaPrecision);

	Gdiplus::PointF origin((Gdiplus::REAL)xOrig, (Gdiplus::REAL)yOrig);

	Gdiplus::RectF box;
	g->MeasureString(sArea, sArea.GetLength(), _fontArea, origin, &_format, &box);

	g->FillRectangle(&_whiteBrush, box);
	g->DrawString(sArea, sArea.GetLength(), _fontArea, origin, &_format, &_textBrush);
}

// ****************************************************************************/
//   GetAzimuth()
// ****************************************************************************/
double ActiveShape::GetAzimuth(MeasurePoint* pnt1, MeasurePoint* pnt2)
{
	double x = pnt2->Proj.x - pnt1->Proj.x;
	double y = pnt2->Proj.y - pnt1->Proj.y;
	return GeometryHelper::GetPointAngleDeg(x, y);
}

// *****************************************************************
//   GetInnerAngle()
// *****************************************************************
double ActiveShape::GetInnerAngle(int vertexIndex, bool clockwise)
{
	double az1 = GetAzimuth(_points[vertexIndex - 1], _points[vertexIndex]);
	double az2 = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
	if (az2 - 180 > az1) az1 = az1 + 360;
	if (az1 - 180 > az2) az2 = az2 + 360;
	return clockwise ? 180 + (az1 - az2) : 180 + (az2 - az1);
}

// *****************************************************************
//   GetRelativeBearing()
// *****************************************************************
double ActiveShape::GetRelativeBearing(int vertexIndex, bool clockwise)
{
	double az1 = GetAzimuth(_points[vertexIndex - 1], _points[vertexIndex]);
	double az2 = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
	if (az2 - 180 > az1) az1 = az1 + 360;
	if (az1 - 180 > az2) az2 = az2 + 360;
	return clockwise ? (az2 - az1) : az1 - az2;
}

// ***************************************************************
//		FormatLength()
// ***************************************************************
CStringW ActiveShape::FormatLength(double length, CStringW format, bool unknownUnits)
{
	CStringW s;

	if (unknownUnits)
	{
		CStringW mu = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMapUnits);
		s.Format(format, length, mu);
		return s;
	}

	tkUnitsOfMeasure units = umMeters;

	switch (LengthUnits)
	{
		case ldmMetric:
			if (length > 1000.0)
			{
				Utility::ConvertDistance(units, umKilometers, length);
				units = umKilometers;
			}
			else if (length < 1.0)
			{
				Utility::ConvertDistance(units, umCentimeters, length);
				units = umCentimeters;
			}
			break;
		case ldmAmerican:
			Utility::ConvertDistance(units, umMiles, length);
			units = umMiles;

			if (length < 1.0)
			{
				Utility::ConvertDistance(umMiles, umFeets, length);
				units = umFeets;
			}
			break;
	}
	
	s.Format(format, length, Utility::GetLocalizedUnitsText(units));
	return s;	
}

// ***************************************************************
//		DrawSegmentInfo()
// ***************************************************************
void ActiveShape::PrepareSegmentLength(Gdiplus::Graphics* g, double length, double totalLength, double screenLength, int segmentIndex, CStringW& sLength, Gdiplus::RectF& rect)
{
	bool unknownUnits = !HasProjection();
	CStringW format = Utility::GetUnitsFormat(LengthPrecision);
	Gdiplus::PointF pnt(0.0f, 0.0f);

	sLength = FormatLength(length, format, unknownUnits);

	bool allowTotalLength = segmentIndex != 0 && totalLength != 0.0 && DrawAccumalatedLength();
	if (allowTotalLength)
	{
		CStringW sLength2 = FormatLength(totalLength, format, unknownUnits);
		CStringW sLengthTotal;
		sLengthTotal.Format(L"%s (%s)", sLength, sLength2);
		
		g->MeasureString(sLengthTotal, sLengthTotal.GetLength(), _font, pnt, &_format, &rect);
		if (rect.Width * RELATIVE_HORIZONTAL_PADDING <= screenLength) {
			sLength = sLengthTotal;
			return;
		}
	}

	// if a segment is too short, let's try to display it without total distance
	g->MeasureString(sLength, sLength.GetLength(), _font, pnt, &_format, &rect);
}

// ***************************************************************
//		IsRelativeBearing()
// ***************************************************************
bool ActiveShape::IsRelativeBearing()
{
	return BearingType == btRelative || BearingType == btLeftAngle || BearingType == btRightAngle;
}

// ***************************************************************
//		PrepareSegmentBearing()
// ***************************************************************
void ActiveShape::PrepareSegmentBearing(Gdiplus::Graphics* g, int segmentIndex, double dx, double dy, CStringW& sBearing, Gdiplus::RectF& rect)
{
	double az = 360.0 - GeometryHelper::GetPointAngleDeg(-dx, -dy);

	if (IsRelativeBearing())
	{
		if (segmentIndex > 0 && segmentIndex < (int)_points.size() - 1)
		{
			if (BearingType == btRelative)
			{
				az = GetRelativeBearing(segmentIndex, true);		// relative bearing is always clockwise
			}
			else
			{
				az = GetInnerAngle(segmentIndex, BearingType == btRightAngle);
			}
		}
		else {
			sBearing = L"";
			return;
		}
	}

	sBearing = AngleHelper::FormatBearing(az, BearingType, AngleFormat, AnglePrecision);
	
	g->MeasureString(sBearing, sBearing.GetLength(), _font, Gdiplus::PointF(0.0f, 0.0f), &_format, &rect);
}

// ***************************************************************
//		DrawSegmentLabel()
// ***************************************************************
void ActiveShape::DrawSegmentLabel(Gdiplus::Graphics* g, CStringW text, Gdiplus::RectF rect, double screenLength, bool aboveLine)
{
	rect.X = (Gdiplus::REAL)(screenLength - rect.Width) / 2.0f;
	rect.Y = aboveLine ? -rect.Height - 2.0f : 2.0f;
	g->FillRectangle(&_whiteBrush, rect);

	rect.X = 0.0f;
	rect.Width = (Gdiplus::REAL)screenLength;
	g->DrawString(text, text.GetLength(), _font, rect, &_format, &_textBrush);
}

// ***************************************************************
//		DrawSegmentInfo()
// ***************************************************************
void ActiveShape::DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, double length, double totalLength, int segmentIndex)
{
	if (!ShowBearing && !ShowLength) {
		return;
	}

	// screen length of segment
	double dx = xScr2 - xScr;
	double dy = yScr2 - yScr;
	double screenLength = sqrt(dx*dx + dy*dy);
	bool relativeBearing = IsRelativeBearing();

	// rotation angle for labels
	double angle = GeometryHelper::GetPointAngleDeg(dx, dy);
	angle = -(angle - 90.0);

	// length string
	CStringW sLength;
	Gdiplus::RectF rectLength;
	if (ShowLength) {
		PrepareSegmentLength(g, length, totalLength, screenLength, segmentIndex, sLength, rectLength);
	}

	// bearing string
	CStringW sBearing;
	Gdiplus::RectF rectBearing;
	if (ShowBearing) {
		PrepareSegmentBearing(g, segmentIndex, dx, dy, sBearing, rectBearing);
	}

	Gdiplus::Matrix m;
	g->GetTransform(&m);
	g->TranslateTransform((Gdiplus::REAL)xScr, (Gdiplus::REAL)yScr);

	bool upsideDown = false;
	if (angle < -90.0)
	{
		g->RotateTransform((Gdiplus::REAL)(angle + 180.0));
		g->TranslateTransform((Gdiplus::REAL)-screenLength, (Gdiplus::REAL)0.0f);
		upsideDown = true;
	}
	else
	{
		g->RotateTransform((Gdiplus::REAL)angle);
	}

	bool hasBearing = ShowBearing && !relativeBearing;

	if (ShowLength && rectLength.Width * RELATIVE_HORIZONTAL_PADDING < screenLength)
	{
		// TODO: add parameter to change above / below position
		DrawSegmentLabel(g, sLength, rectLength, screenLength, hasBearing ? false : true);	
	}
		
	if (hasBearing && rectBearing.Width * RELATIVE_HORIZONTAL_PADDING < screenLength)
	{
		DrawSegmentLabel(g, sBearing, rectBearing, screenLength, true);
	}

	g->SetTransform(&m);		// restore transform

	if (ShowBearing && relativeBearing)
	{
		DrawRelativeBearing(g, segmentIndex, xScr, yScr, rectBearing, sBearing);
	}
}

// ***************************************************************
//		DrawRelativeBearing()
// ***************************************************************
// Draws the relative bearing(inner or outer angle for 2 segments).
void ActiveShape::DrawRelativeBearing(Gdiplus::Graphics* g, int segmentIndex, double xScr, double yScr, Gdiplus::RectF r2, CStringW sBearing)
{
	if (segmentIndex < 1 || segmentIndex >= (int)_points.size() - 1) {
		return;
	}

	double angle = GetBearingLabelAngle(segmentIndex, BearingType != btLeftAngle) - 90.0;

	bool upsideDown = false;

	Gdiplus::Matrix m;
	g->GetTransform(&m);
	g->TranslateTransform((Gdiplus::REAL)xScr, (Gdiplus::REAL)yScr);
	if (angle > 90.0)
	{
		g->RotateTransform((Gdiplus::REAL)(angle - 180.0));
		g->TranslateTransform((Gdiplus::REAL) - r2.Width, (Gdiplus::REAL)0.0f);
		upsideDown = true;
	}
	else {
		g->RotateTransform((Gdiplus::REAL)angle);
	}

	r2.X = upsideDown ? -r2.Height : r2.Height;
	r2.Y = -r2.Height / 2.0f;
	g->FillRectangle(&_whiteBrush, r2);
	g->DrawString(sBearing, sBearing.GetLength(), _font, r2, &_format, &_textBrush);

	g->SetTransform(&m);		// restore transform
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

	SetModified();
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
		SetModified();
	}
	UpdatePolyCloseState(false);
	return result;
}

// **************************************************************
//		PartIsSelected()
// **************************************************************
bool ActiveShape::PartIsSelected(int partIndex)
{
	return _selectedParts.find(partIndex) != _selectedParts.end();
}

bool ActiveShape::VerticesAreVisible()
{
	return PointsVisible && !OverlayTool;
}

bool ActiveShape::PointLabelsAreVisible()
{
	return PointLabelsVisible && !OverlayTool;
}
