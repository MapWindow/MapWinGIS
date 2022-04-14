#include "StdAfx.h"
#include "ActiveShape.h"

#include <gsl/util>

#include "GeometryHelper.h"
#include "CollisionList.h"
#include "AngleHelper.h"
#include "GraphicsStateHelper.h"

// *******************************************************
//		SetMapCallback()
// *******************************************************
void ActiveShape::SetMapCallback(IMapViewCallback* mapView, const ShapeInputMode inputMode)
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
int ActiveShape::GetScreenPoints(const int partIndex, const MixedShapePart whichPoints, Gdiplus::PointF** data,
	const bool dynamicPoint, const DraggingOperation offsetType, const int offsetX, const int offsetY)
{
	const int size = GetScreenPoints(partIndex, whichPoints, dynamicPoint, static_cast<int>(_mousePoint.x), static_cast<int>(_mousePoint.y), data);

	//gsl::not_null<Gdiplus::PointF*> points = *data;
	Gdiplus::PointF* points = *data; // TODO: Fix compile warning
	if (offsetType == DragMoveShape || offsetType == DragMovePart && PartIsSelected(partIndex))
	{
		for (int i = 0; i < size; i++)
		{
			points[i].X += offsetX; // TODO: Fix compile warning
			points[i].Y += offsetY;
		}
	}
	else if (offsetType == DragMovePart && partIndex == -1)
	{
		// rendering of a poly as a whole; adjust only the necessary part
		const int startIndex = GetPartStart(_selectedPart);
		const int endIndex = SeekPartEnd(startIndex);

		if (startIndex != -1 && endIndex != -1)
		{
			for (int i = startIndex; i <= endIndex; i++)
			{
				points[i].X += offsetX; // TODO: Fix compile warning
				points[i].Y += offsetY;
			}
		}
	}
	return size;
}

// *******************************************************
//		GetPartStartAndEnd()
// *******************************************************
bool ActiveShape::GetPartStartAndEnd(const int partIndex, MixedShapePart whichPoints, int& startIndex, int& endIndex)
{
	startIndex = 0;
	endIndex = GetPointCount();		// actually the next after end one

	if ( GetNumParts() > 1 && partIndex != -1)
	{
		int count = -1;
		startIndex = -1;
		for (int i = 0; i < GetPointCount(); i++)
		{
			if (startIndex != -1 && gsl::at(_points, i)->Part == PartEnd) {
				endIndex = gsl::narrow_cast<int>(i) + 1;
				break;
			}
			if (gsl::at(_points, i)->Part == PartBegin)
			{
				count++;
				if (count == partIndex)
					startIndex = gsl::narrow_cast<int>(i);
			}
		}
		return startIndex != -1;
	}
	return true;
}

// *******************************************************
//		get_ScreenPoints()
// *******************************************************
int ActiveShape::GetScreenPoints(const int partIndex, const MixedShapePart whichPoints, const bool hasLastPoint, const int lastX, const int lastY,
	Gdiplus::PointF** data)
{
	int startIndex, endIndex;
	GetPartStartAndEnd(partIndex, whichPoints, startIndex, endIndex);

	const int size = endIndex - startIndex;
	int totalSize = size;
	if (hasLastPoint) totalSize++;

	if (size > 0)
	{
		*data = new Gdiplus::PointF[totalSize];
		Gdiplus::PointF* temp = *data; // TODO: Fix compile warning
		double x, y;
		for (int i = 0; i < size; i++) {
			ProjToPixel(gsl::at(_points, startIndex + i)->Proj.x, gsl::at(_points, startIndex + i)->Proj.y, x, y);
			temp[i].X = (Gdiplus::REAL)x; // TODO: Fix compile warning
			temp[i].Y = (Gdiplus::REAL)y;
		}
		if (hasLastPoint)
		{
			temp[totalSize - 1].X = (Gdiplus::REAL)lastX; // TODO: Fix compile warning
			temp[totalSize - 1].Y = (Gdiplus::REAL)lastY;
		}
	}
	return totalSize;
}

// ****************************************************************
//		DrawData()
// ****************************************************************
void ActiveShape::DrawData(Gdiplus::Graphics* g, const bool dynamicBuffer,
	const DraggingOperation offsetType, const int screenOffsetX /*= 0*/, const int screenOffsetY /*= 0*/)
{
	const bool hasLine = HasLine(dynamicBuffer);
	const bool hasPolygon = HasPolygon(dynamicBuffer);
	if (!hasLine && !hasPolygon && !_showSnapPoint) return;

	_fillBrush.SetColor(Utility::OleColor2GdiPlus(FillColor, FillTransparency));
	_linePen.SetWidth(LineWidth);
	_linePen.SetColor(Utility::OleColor2GdiPlus(LineColor, 255));


	Gdiplus::PointF* polyData = nullptr;
	int polySize = 0;
	const int partCount = GetNumParts();

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
					delete[] polyData; // TODO: Fix compile warning
				}
			}
			g->FillPath(&_fillBrush, &path);

			if (_inputMode == ShapeInputMode::simMeasuring) {   // to display closing line
				g->DrawPath(&_linePen, &path);
			}
		}
	}

	if (hasLine)
	{
		Gdiplus::GraphicsStateHelper state;
		state.SetSmoothingMode(g, Gdiplus::SmoothingModeHighQuality);
		state.SetTextRenderingHint(g, Gdiplus::TextRenderingHintAntiAlias);

		const int partCountLine = GetNumParts();

		CCollisionList collisionList;

		for (int n = 0; n < partCountLine; n++)
		{
			Gdiplus::PointF* data = nullptr;
			const int size = GetScreenPoints(n, LinearPart, &data, false, offsetType, screenOffsetX, screenOffsetY);
			if (size > 0)
			{
				DrawLines(g, size, data, dynamicBuffer, n, collisionList);
			}
		}

		state.RestoreSmoothingMode(g);
		state.RestoreTextRenderingHint(g);
	}

	// draw area atop of everything else
	// TODO: april 2022: is partCount correct or should it be partCountLine which was previously also named partCount
	if (partCount == 1 && polySize > 0)
	{
		DrawPolygonArea(g, polyData, polySize, dynamicBuffer);
		delete[] polyData; // TODO: Fix compile warning
	}

	// finally draw the snap point position if needed
	if (_showSnapPoint) {
		//  TODO: Fix compile warning:
		g->DrawRectangle(&_redPen, gsl::narrow_cast<int>(_snapPointX - 3.0f + 0.5f), gsl::narrow_cast<int>(_snapPointY - 3.0f + 0.5f), 6, 6);
	}
}

// ****************************************************************
//		DrawLines()
// ****************************************************************
void ActiveShape::DrawLines(Gdiplus::Graphics* g, const int size, const Gdiplus::PointF* data, const bool dynamicBuffer, const int partIndex, CCollisionList& collisionList)
{

	// TODO: Lost of compile warnings

	const int startIndex = GetPartStart(partIndex);
	const bool editing = this->GetInputMode() == ShapeInputMode::simEditing;
	const bool multiPoint = GetShapeType2D() == SHP_MULTIPOINT;

	if (!multiPoint)
	{
		long errorCode = tkNO_ERROR;
		double totalLength = 0.0;
		for (int i = 0; i < size - 1; i++)
		{
			const int realIndex = startIndex + i;
			const double length = GetSegmentLength(realIndex, errorCode);
			totalLength += length;
			// TODO: Fix compile warning:
			DrawSegmentInfo(g, data[i].X, data[i].Y, data[i + 1].X, data[i + 1].Y, length, totalLength, i);
		}
	}

	const Gdiplus::Pen* pen = partIndex != -1 && (_selectedPart == partIndex || _highlightedPart == partIndex) ? &_redPen : &_linePen;

	if (OverlayTool)
	{
		_linePen.SetDashStyle(Gdiplus::DashStyleCustom);
		constexpr Gdiplus::REAL dashValues[4] = { 8, 8 };
		_linePen.SetDashPattern(dashValues, 2);
	}
	else
	{
		_linePen.SetDashStyle(static_cast<Gdiplus::DashStyle>(LineStyle));
	}

	if (!_drawLabelsOnly && !multiPoint)
	{
		g->DrawLines(pen, data, size);
	}

	// drawing points

	CStringW s;
	for (int i = 0; i < size; i++)
	{
		const int realIndex = startIndex + i;

		if (VerticesAreVisible())
		{
			if (realIndex == _selectedVertex || realIndex == _highlightedVertex)
			{
				g->DrawRectangle(&_redPen, static_cast<int>(data[i].X - 3.0f + 0.5f), static_cast<int>(data[i].Y - 3.0f + 0.5f), 6, 6);
			}
			else
			{
				//Gdiplus::SolidBrush* brush = realIndex == _selectedVertex ? &_redBrush : &_blueBrush;
				//Gdiplus::Pen* pen = realIndex == _selectedVertex ? &_redPen : &_bluePen;

				if (i == 0 || editing)
				{
					g->DrawRectangle(&_bluePen, static_cast<int>(data[i].X - 3.0f + 0.5f), static_cast<int>(data[i].Y - 3.0f + 0.5f), 6, 6);
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
			const int index = realIndex + 1;
			Gdiplus::RectF bounds;
			s.Format(L"%d", index);
			const Gdiplus::PointF pntTemp(data[i].X + 10, data[i].Y);
			g->MeasureString(s, s.GetLength(), _font, pntTemp, &bounds);

			if (i > 0 && i < size - 1)
			{
				const double angle = GetBearingLabelAngle(realIndex, BearingType == btLeftAngle);

				const double dx = sin(angle / 180.0 * pi_) * 15;
				const double dy = -cos(angle / 180.0 * pi_) * 15;

				bounds.X = static_cast<Gdiplus::REAL>(data[i].X + dx - bounds.Width / 2);
				bounds.Y = static_cast<Gdiplus::REAL>(data[i].Y + dy - bounds.Height / 2);
			}

			CRect r((bounds.GetLeft()), (bounds.GetTop()), (bounds.GetRight()), (bounds.GetBottom()));
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
		ProjToPixel(gsl::at(_points, static_cast<gsl::index>(GetPointCount()) - 1)->Proj.x,
			gsl::at(_points, static_cast<gsl::index>(GetPointCount()) - 1)->Proj.y, x, y);

		if (!_points.empty() && (_mousePoint.x != x || _mousePoint.y != y))
		{
			const double dist = GetDynamicLineDistance();
			DrawSegmentInfo(g, x, y, _mousePoint.x, _mousePoint.y, dist, 0.0, -1);
			g->DrawLine(&_linePen, static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y),
				static_cast<Gdiplus::REAL>(_mousePoint.x), static_cast<Gdiplus::REAL>(_mousePoint.y));
		}
	}

	delete[] data; // TODO: Fix compile warning
}

// ****************************************************************
//		DrawPolygonArea()
// ****************************************************************
void ActiveShape::DrawPolygonArea(Gdiplus::Graphics* g, Gdiplus::PointF* data, const int size, const bool dynamicPoly)
{
	if (!GetShowArea()) return;

	if (IPoint* pnt = GetPolygonCenter(data, size))
	{
		double projX, projY;
		PixelToProj(_mousePoint.x, _mousePoint.y, projX, projY);
		const double area = GetArea(dynamicPoly, projX, projY);
		DrawPolygonArea(g, pnt, area);
		pnt->Release();
	}
}

// ****************************************************************
//		DrawPolygonArea()
// ****************************************************************
void ActiveShape::DrawPolygonArea(Gdiplus::Graphics* g, IPoint* pnt, const double area)
{
	double xOrig, yOrig;
	pnt->get_X(&xOrig);
	pnt->get_Y(&yOrig);

	const CStringW sArea = Utility::FormatArea(area, !HasProjection(), AreaDisplayMode, AreaPrecision);

	const Gdiplus::PointF origin((Gdiplus::REAL)xOrig, (Gdiplus::REAL)yOrig);

	Gdiplus::RectF box;
	g->MeasureString(sArea, sArea.GetLength(), _fontArea, origin, &_format, &box);

	g->FillRectangle(&_whiteBrush, box);
	g->DrawString(sArea, sArea.GetLength(), _fontArea, origin, &_format, &_textBrush);
}

// ****************************************************************************/
//   GetAzimuth()
// ****************************************************************************/
double ActiveShape::GetAzimuth(const MeasurePoint* pnt1, const MeasurePoint* pnt2)
{
	const double x = pnt2->Proj.x - pnt1->Proj.x;
	const double y = pnt2->Proj.y - pnt1->Proj.y;
	return GeometryHelper::GetPointAngleDeg(x, y);
}

// *****************************************************************
//   GetInnerAngle()
// *****************************************************************
double ActiveShape::GetInnerAngle(const int vertexIndex, const bool clockwise)
{
	double az1 = GetAzimuth(gsl::at(_points, static_cast<gsl::index>(vertexIndex) - 1), gsl::at(_points, vertexIndex));
	double az2 = GetAzimuth(gsl::at(_points, vertexIndex), gsl::at(_points, static_cast<gsl::index>(vertexIndex) + 1));
	if (az2 - 180 > az1) az1 = az1 + 360;
	if (az1 - 180 > az2) az2 = az2 + 360;
	return clockwise ? 180 + (az1 - az2) : 180 + (az2 - az1);
}

// *****************************************************************
//   GetRelativeBearing()
// *****************************************************************
double ActiveShape::GetRelativeBearing(const int vertexIndex, const bool clockwise)
{
	double az1 = GetAzimuth(gsl::at(_points, static_cast<gsl::index>(vertexIndex) - 1), gsl::at(_points, vertexIndex));
	double az2 = GetAzimuth(gsl::at(_points, vertexIndex), gsl::at(_points, static_cast<gsl::index>(vertexIndex) + 1));
	if (az2 - 180 > az1) az1 = az1 + 360;
	if (az1 - 180 > az2) az2 = az2 + 360;
	return clockwise ? az2 - az1 : az1 - az2;
}

// ***************************************************************
//		FormatLength()
// ***************************************************************
CStringW ActiveShape::FormatLength(double length, const CStringW format, const bool unknownUnits)
{
	CStringW s;

	if (unknownUnits)
	{
		const CStringW mu = m_globalSettings.GetLocalizedString(tkLocalizedStrings::lsMapUnits);
		s.Format(format, length, mu); // TODO: Fix compile warning
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
void ActiveShape::PrepareSegmentLength(const Gdiplus::Graphics* g, const double length, const double totalLength,
	const double screenLength, const int segmentIndex, CStringW& sLength,
	Gdiplus::RectF& rect)
{
	const bool unknownUnits = !HasProjection();
	CStringW format = Utility::GetUnitsFormat(LengthPrecision);
	const Gdiplus::PointF pnt(0.0f, 0.0f);

	sLength = FormatLength(length, format, unknownUnits);

	if (segmentIndex != 0 && totalLength != 0.0 && DrawAccumalatedLength())
	{
		const CStringW sLength2 = FormatLength(totalLength, format, unknownUnits);
		CStringW sLengthTotal;
		sLengthTotal.Format(L"%s (%s)", sLength, sLength2);  // TODO: Fix compile warning

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
void ActiveShape::PrepareSegmentBearing(const Gdiplus::Graphics* g, const int segmentIndex, const double dx, const double dy, CStringW& sBearing, Gdiplus::RectF& rect)
{
	double az = 360.0 - GeometryHelper::GetPointAngleDeg(-dx, -dy);

	if (IsRelativeBearing())
	{
		if (segmentIndex > 0 && segmentIndex < GetPointCount() - 1)
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
void ActiveShape::DrawSegmentLabel(Gdiplus::Graphics* g, const CStringW text, Gdiplus::RectF rect, const double screenLength, const bool aboveLine)
{
	rect.X = static_cast<Gdiplus::REAL>(screenLength - rect.Width) / 2.0f; // TODO: Fix compile warning
	rect.Y = aboveLine ? -rect.Height - 2.0f : 2.0f;
	g->FillRectangle(&_whiteBrush, rect);

	rect.X = 0.0f;
	rect.Width = static_cast<Gdiplus::REAL>(screenLength);
	g->DrawString(text, text.GetLength(), _font, rect, &_format, &_textBrush);
}

// ***************************************************************
//		DrawSegmentInfo()
// ***************************************************************
void ActiveShape::DrawSegmentInfo(Gdiplus::Graphics* g, const double xScr, const double yScr, const double xScr2,
	const double yScr2, const double length, const double totalLength,
	const int segmentIndex)
{
	if (!ShowBearing && !ShowLength) {
		return;
	}

	// screen length of segment
	const double dx = xScr2 - xScr;
	const double dy = yScr2 - yScr;
	const double screenLength = sqrt(dx * dx + dy * dy);
	const bool relativeBearing = IsRelativeBearing();

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
	g->TranslateTransform(static_cast<Gdiplus::REAL>(xScr), static_cast<Gdiplus::REAL>(yScr));

	if (angle < -90.0)
	{
		g->RotateTransform(static_cast<Gdiplus::REAL>(angle + 180.0));
		g->TranslateTransform(static_cast<Gdiplus::REAL>(-screenLength), 0.0f);
	}
	else
	{
		g->RotateTransform(static_cast<Gdiplus::REAL>(angle));
	}

	const bool hasBearing = ShowBearing && !relativeBearing;

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
void ActiveShape::DrawRelativeBearing(Gdiplus::Graphics* g, const int segmentIndex, const double xScr, const double yScr, Gdiplus::RectF r2, const CStringW sBearing)
{
	if (segmentIndex < 1 || segmentIndex >= GetPointCount() - 1) {
		return;
	}

	const double angle = GetBearingLabelAngle(segmentIndex, BearingType != btLeftAngle) - 90.0;

	bool upsideDown = false;

	Gdiplus::Matrix m;
	g->GetTransform(&m);
	g->TranslateTransform(static_cast<Gdiplus::REAL>(xScr), static_cast<Gdiplus::REAL>(yScr));
	if (angle > 90.0)
	{
		g->RotateTransform(static_cast<Gdiplus::REAL>(angle - 180.0));
		g->TranslateTransform(-r2.Width, 0.0f);
		upsideDown = true;
	}
	else {
		g->RotateTransform(static_cast<Gdiplus::REAL>(angle));
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
bool ActiveShape::HandlePointAdd(const double screenX, const double screenY, const bool ctrl)
{
	double projX, projY;

	int closePointIndex = -1;

	if (ctrl && CloseOnPreviousVertex())
	{
		// closing to show area
		if (SnapToPreviousVertex(closePointIndex, screenX, screenY))
		{
			if (closePointIndex != -1)
			{
				projX = gsl::at(_points, closePointIndex)->Proj.x;
				projY = gsl::at(_points, closePointIndex)->Proj.y;
			}
			else
			{
				// for types other than polygons we add another point rather than closing
				PixelToProj(screenX, screenY, projX, projY);
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
		PixelToProj(screenX, screenY, projX, projY);
	}

	AddPoint(projX, projY, screenX, screenY);

	UpdatePolyCloseState(true, closePointIndex);

	return true;
}

// *******************************************************
//		AddPoint()
// *******************************************************
void ActiveShape::AddPoint(const double xProj, const double yProj, const double xScreen, const double yScreen, const PointPart part)
{
	ClearIfStopped();

	_mousePoint.x = xScreen;
	_mousePoint.y = yScreen;

	MeasurePoint* pnt = new MeasurePoint();  // TODO: Fix compile warning
	pnt->Proj.x = xProj;
	pnt->Proj.y = yProj;
	pnt->Part = part;
	_points.push_back(pnt);

	UpdateLatLng(GetPointCount() - 1);

	SetModified();
}

// *******************************************************
//		AddPoint()
// *******************************************************
void ActiveShape::AddPoint(const double xProj, const double yProj)
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
	if (!_points.empty())
	{
		delete gsl::at(_points, GetPointCount() - 1); // TODO: Fix compile warning
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
bool ActiveShape::PartIsSelected(const int partIndex)
{
	return _selectedParts.find(partIndex) != _selectedParts.end(); // TODO: Fix compile warning
}

bool ActiveShape::VerticesAreVisible()
{
	return PointsVisible && !OverlayTool;
}

bool ActiveShape::PointLabelsAreVisible()
{
	return PointLabelsVisible && !OverlayTool;
}
