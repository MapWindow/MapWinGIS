#include "stdafx.h"
#include "ActiveShape.h"
#include "GeometryOperations.h"
#include "Shape.h"
#include "GdiPlusHelper.h"
#include "..\Processing\GeograpicLib\PolygonArea.hpp"
#include "GeographLibHelper.h"
#include "CollisionList.h"

// *******************************************************
//		TestAreaCalc()
// *******************************************************
void TestAreaCalc()
{
	std::vector<Point2D> points;

	points.push_back(Point2D(5.1, 42.6));
	points.push_back(Point2D(7.3, 41.6));
	points.push_back(Point2D(4.2, 40.4));

	//if (_points.size() > 0)
	//points.push_back(Point2D(_points[0]->x, _points[0]->y));
	double area = GeographLibHelper::CalcPolyGeodesicArea(points);
	Debug::WriteLine("Area %f", area);
};

// *******************************************************
//		GetGeodesicArea()
// *******************************************************
// The behavior of GeographicLib is sometimes erratic, GeographicLib::PolygonArea::_mask 
// is initialized in constructor but is reset with no explicit call doing it. 
// I haven't been able to find the reason. Perhaps conflicts with other include directives,
// perhaps problems with memory management. Seems to be working now.
namespace MeasuringNsp
{
	GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
	GeographicLib::PolygonArea poly(geod);
}

double ActiveShape::GetGeodesicArea(bool closingPoint, double x, double y)
{
	unsigned int result;
	double perimeter = 0.0, area = 0.0;
	if (GetPolygonPointCount(closingPoint) > 1)
	{
recalc:			
		if (_areaRecalcIsNeeded || !IsDynamic())
		{
			_areaRecalcIsNeeded = false;
			
			MeasuringNsp::poly.Clear();

			for(int i = GetFirstPolyPointIndex(); i < (int)_points.size(); i++)
			{
				MeasuringNsp::poly.AddPoint(_points[i]->y, _points[i]->x);
			}

			result = MeasuringNsp::poly.Compute(true, true, perimeter, area);
			if (closingPoint)
				result = MeasuringNsp::poly.TestPoint(y, x, true, true, perimeter, area);
		}
		else
		{
			if (closingPoint)
			{
				result = MeasuringNsp::poly.TestPoint(y, x, true, true, perimeter, area);
				if (perimeter == 0 && area == 0)
				{
					_areaRecalcIsNeeded = true;
					goto recalc;
				}
			}
			else
			{
				result = MeasuringNsp::poly.Compute(true, true, perimeter, area);
			}	
		}
	}
	return abs(area);
}

// *******************************************************
//		SetDefaults()
// *******************************************************
void ActiveShape::SetDefaults()
{
	//TestAreaCalc();

	_inputMode = simMeasuring;
	DisplayAngles = false;
	_isGeodesic = false;
	_mousePoint.x = _mousePoint.y = 0;
	_areaRecalcIsNeeded = true;
	_firstPolyPointIndex = -1;
}

// *******************************************************
//		Clear()
// *******************************************************
void ActiveShape::Clear()
{
	_selectedVertex = -1;
	_firstPolyPointIndex = -1;
	for(size_t i = 0; i < _points.size(); i++)
		delete _points[i];
	_points.clear();
	MeasuringNsp::poly.Clear();
}

#pragma region MapView
// *******************************************************
//		SetMapView()
// *******************************************************
void ActiveShape::SetMapCallback(IMapViewCallback* mapView, ShapeInputMode inputMode)
{
	_mapCallback = mapView;
	_inputMode = inputMode;
}

IGeoProjection* ActiveShape::GetWgs84Projection()
{
	return _mapCallback ? _mapCallback->_GetWgs84Projection() : NULL;
}
IGeoProjection* ActiveShape::GetMapProjection()
{
	return _mapCallback ? _mapCallback->_GetMapProjection() : NULL;
}
tkTransformationMode ActiveShape::GetTransformationMode()
{
	return _mapCallback ? _mapCallback->_GetTransformationMode() : tmNotDefined;
}

// ***************************************************************
//		ProjToPixel()
// ***************************************************************
void ActiveShape::ProjToPixel(double projX, double projY, double& pixelX, double& pixelY)
{
	if (_mapCallback) {
		_mapCallback->_ProjectionToPixel(projX, projY, &pixelX, &pixelY);
	}
	else {
		Debug::WriteError("Measuring: map pointer is not initialized");
	}
}

// ***************************************************************
//		PixelToProj()
// ***************************************************************
void ActiveShape::PixelToProj(double pixelX, double pixelY, double& projX, double& projY)
{
	if (_mapCallback) {
		_mapCallback->_PixelToProjection(pixelX, pixelY, &projX, &projY);
	}
	else {
		Debug::WriteError("Measuring: map pointer is not initialized");
	}
}
#pragma endregion

// *******************************************************
//		get_ScreenPoints()
// *******************************************************
int ActiveShape::get_ScreenPoints(int partIndex, ScreenPointsType whichPoints, Gdiplus::PointF** data, 
								  bool dynamicPoint, OffsetType offsetType, int offsetX, int offsetY)
{
	int size = get_ScreenPoints(partIndex, whichPoints, dynamicPoint, (int)_mousePoint.x, (int)_mousePoint.y, data);

	Gdiplus::PointF* points = *data;
	if (offsetType == OffsetShape)
	{
		for(int i= 0; i < size; i++)
		{
			points[i].X += offsetX;
			points[i].Y += offsetY;
		}
	}
	return size;
}

// *******************************************************
//		GetPartStartAndEnd()
// *******************************************************
bool ActiveShape::GetPartStartAndEnd(int partIndex, ScreenPointsType whichPoints, int& startIndex, int& endIndex)
{
	startIndex = 0;
	endIndex = _points.size();		// actually the next after end one

	if (_mixedLinePolyMode)
	{
		switch (whichPoints)
		{
			case ScreenPointsLinearPart:
			{
				if (!_drawLineForPoly) {
					if (_firstPolyPointIndex != -1 && _firstPolyPointIndex < (int)_points.size())
						endIndex = _firstPolyPointIndex + 1;
				}
				break;
			}
			case ScreenPointsPolygonPart:
			{
				if (_firstPolyPointIndex == -1)
					return 0;
				if (_firstPolyPointIndex >(int)_points.size() - 1)
				{
					Debug::WriteError("Active shape; First poly point index outside bounds");
					return 0;
				}
				startIndex = _firstPolyPointIndex;
				endIndex = (int)_points.size();
				break;
			}
		}
	}
	else {
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
	}
	return true;
}

// *******************************************************
//		get_ScreenPoints()
// *******************************************************
int ActiveShape::get_ScreenPoints(int partIndex, ScreenPointsType whichPoints, bool hasLastPoint, int lastX, int lastY, Gdiplus::PointF** data)
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
//		TryInsertVertex()
// *******************************************************
bool ActiveShape::TryInsertVertex(double xProj, double yProj)
{
	int pntIndex = FindSegmentWithPoint(xProj, yProj);
	if (pntIndex != - 1)
	{
		MeasurePoint* pnt = new MeasurePoint();
		pnt->Proj.x = xProj;
		pnt->Proj.y = yProj;
		_points.insert(_points.begin() + pntIndex + 1, pnt);
		UpdateLatLng(pntIndex + 1);
		UpdateLatLng(pntIndex + 2);

		_areaRecalcIsNeeded = true;
		return true;
	}
	return false;
}

// *******************************************************
//		FindSegementWithPoint()
// *******************************************************
int ActiveShape::FindSegmentWithPoint(double xProj, double yProj)
{
	// TODO: should we consider segment from the last to the first
	for(size_t i = 0; i < _points.size() - 1; i++)
	{
		if (PointOnSegment(_points[i]->Proj.x, _points[i]->Proj.y, 
			_points[i + 1]->Proj.x, _points[i + 1]->Proj.y, xProj, yProj))
			return i;
	}
	return -1;
}

// *******************************************************
//		Move()
// *******************************************************
void ActiveShape::Move( double offsetXProj, double offsetYProj )
{
	for(size_t i = 0; i < _points.size(); i++)
	{
		_points[i]->Proj.x += offsetXProj;
		_points[i]->Proj.y += offsetYProj;
		UpdateLatLng(i);
	}
	_areaRecalcIsNeeded = true;
}

// *******************************************************
//		MoveVertex()
// *******************************************************
void ActiveShape::MoveVertex( double xProj, double yProj )
{
	int index = _selectedVertex;
	int closeIndex = GetCloseIndex(index);

	if (index >= 0 && index < (int)_points.size())
	{
		_points[index]->Proj.x = xProj;
		_points[index]->Proj.y = yProj;
		UpdateLatLng(index);
		
		// coordinates of the first and last point of polygon must be the same
		if (closeIndex >= 0 && closeIndex < (int)_points.size()) {
			_points[closeIndex]->Proj.x = _points[index]->Proj.x;
			_points[closeIndex]->Proj.y = _points[index]->Proj.y;
			UpdateLatLng(closeIndex);
		}
	}
	_areaRecalcIsNeeded = true;
}

// *******************************************************
//		UpdateLatLng()
// *******************************************************
void ActiveShape::UpdateLatLng( int pointIndex )
{
	if (pointIndex < 0 || pointIndex >= (int)_points.size() ) return;

	MeasurePoint* pnt = _points[pointIndex];
	double x = pnt->Proj.x, y = pnt->Proj.y;
	if (TransformPoint(x, y))
	{
		pnt->x = x;
		pnt->y = y;
	}
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


#pragma region Drawing
// ****************************************************************
//		DrawMeasuringToScreenBuffer()
// ****************************************************************
void ActiveShape::DrawData( Gdiplus::Graphics* g, bool dynamicBuffer, OffsetType offsetType, int screenOffsetX, int screenOffsetY )
{
	bool hasLine = (HasDynamicLine() || HasStaticLine()); //(HasDynamicLine() && dynamicBuffer) || (HasStaticLine() && !dynamicBuffer);
	bool hasPolygon = (HasDynamicPolygon() && dynamicBuffer) || (HasStaticPolygon() && !dynamicBuffer);

	_fillBrush.SetColor(Utility::OleColor2GdiPlus(FillColor, FillTransparency)) ;
	_linePen.SetWidth(LineWidth);
	_linePen.SetColor(Utility::OleColor2GdiPlus(LineColor, 255));

	bool editing = this->GetInputMode() == simEditing;

	Gdiplus::PointF* polyData = NULL;
	int polySize = 0;

	if (hasPolygon)
	{
		polySize = get_ScreenPoints(-1, ScreenPointsPolygonPart, &polyData, dynamicBuffer, offsetType, screenOffsetX, screenOffsetY);
		if (polySize > 1)
		{
			_linePen.SetLineJoin(Gdiplus::LineJoinRound);
			if (!_drawLabelsOnly)
			{
				g->FillPolygon(&_fillBrush, polyData, polySize);
				//g->DrawPolygon(&_linePen, polyData, polySize);
			}
		}
	}

	if (hasLine)
	{
		Gdiplus::GraphicsStateHelper state;
		state.SetSmoothingMode(g, Gdiplus::SmoothingModeHighQuality);
		state.SetTextRenderingHint(g, Gdiplus::TextRenderingHintAntiAlias);
	
		if (HasDynamicLine() == dynamicBuffer)
		{
			int partCount = GetNumParts();

			for (int n = 0; n < partCount; n++) 
			{
				Gdiplus::PointF* data = NULL;
				int size = get_ScreenPoints(n, ScreenPointsLinearPart, &data, false, offsetType, screenOffsetX, screenOffsetY);
				if (size > 0)
				{
					DrawLines(g, size, data, editing, dynamicBuffer, n);
				}
			}
		}

		state.RestoreSmoothingMode(g);
		state.RestoreTextRenderingHint(g);
	}
	
	// draw area atop of everything else
	if (polySize > 0)
	{
		DisplayPolygonArea(g, polyData, polySize, dynamicBuffer);
		delete[] polyData;
	}
}

// ****************************************************************
//		DrawLines()
// ****************************************************************
void ActiveShape::DrawLines(Gdiplus::Graphics* g, int size, Gdiplus::PointF* data, bool editing, bool dynamicBuffer, int partIndex)
{
	double length = 0.0, totalLength = 0.0;

	int startIndex = GetPartStart(partIndex);

	for (int i = 0; i < size - 1; i++) {
		int realIndex = startIndex + i;
		length = GetSegmentLength(realIndex);
		totalLength += length;
		DrawSegmentInfo(g, data[i].X, data[i].Y, data[i + 1].X, data[i + 1].Y, length, totalLength, i, false);
	}

	if (!_drawLabelsOnly)
		g->DrawLines(&_linePen, data, size);

	bool closedPoly = false;
	if (_mixedLinePolyMode && _firstPolyPointIndex != -1 && _firstPolyPointIndex < size)
	{
		double POLYGON_CLOSE_TOLERANCE = 1.0;
		closedPoly = (abs(data[_firstPolyPointIndex].X - data[size - 1].X) < POLYGON_CLOSE_TOLERANCE &&
			abs(data[_firstPolyPointIndex].Y - data[size - 1].Y) < POLYGON_CLOSE_TOLERANCE);
		if (_firstPolyPointIndex == size - 1) closedPoly = false;
	}

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
				g->DrawRectangle(&_redPen, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
			}
			else
			{
				Gdiplus::SolidBrush* brush = realIndex == _selectedVertex ? &_redBrush : &_blueBrush;
				Gdiplus::Pen* pen = realIndex == _selectedVertex ? &_redPen : &_bluePen;

				if (i == 0 || (_drawLineForPoly && i == _firstPolyPointIndex) || editing)
				{
					//g->FillRectangle(&_blueBrush, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
					g->DrawRectangle(&_bluePen, (int)(data[i].X - 3.0f + 0.5), int(data[i].Y - 3.0f + 0.5), 6, 6);
				}
				else
				{
					g->FillEllipse(&_blueBrush, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
					g->DrawEllipse(&_bluePen, data[i].X - 3.0f, data[i].Y - 3.0f, 6.0f, 6.0f);
				}
			}
		}

		if (closedPoly && i == size - 1) continue;	// don't display the last one for the closed poly

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
//		GetMeasuringPolyCenter()
// ****************************************************************
IPoint* ActiveShape::GetPolygonCenter(Gdiplus::PointF* data, int length)
{
	IPoint* pnt = NULL;
	if ( HasPolygon() && length > 2 )
	{
		// let's draw fill and area
		Gdiplus::PointF* polyData = &data[0]; //&(data[_firstPolyPointIndex]);

		// find position for label
		IShape* shp = NULL;
		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
		if (shp)
		{
			long pointIndex;
			VARIANT_BOOL vb;
			shp->Create(ShpfileType::SHP_POLYGON, &vb);

			//int sz = _measuring.Type == tkMeasuringType::MeasureDistance ? _points.size() - 1 - _firstPolyPointIndex : length;
			int sz = length;			

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
	return pnt;
}

// ****************************************************************
//		DisplayPolygonArea()
// ****************************************************************
void ActiveShape::DisplayPolygonArea(Gdiplus::Graphics* g, Gdiplus::PointF* data, int size, bool dynamicPoly)
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
	if (!_geometryVisible) return;
	
	CStringW s1, s2, s, sAz;

	double x = xScr - xScr2;
	double y = yScr - yScr2;

	double angle = 360.0 - GetPointAngle(x, y) * 180.0 / pi_;
	angle += _angleCorrection;							
	
	x = -x;
	y = -y;
	angle = GetPointAngle(x, y) * 180.0 / pi_;							
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
#pragma endregion

// *******************************************************
//		GetTransformedPoints()
// *******************************************************
// transforms input data to decimal degrees
bool ActiveShape::TransformPoint(double& x, double& y) {

	VARIANT_BOOL vb;
	switch (GetTransformationMode())
	{
	case tkTransformationMode::tmDoTransformation:
		GetMapProjection()->Transform(&x, &y, &vb);
		return true;
	case tkTransformationMode::tmWgs84Complied:	
		return true;
	}
	return false;
}

// ***************************************************************
//		HandleProjPointAdd()
// ***************************************************************
void ActiveShape::HandleProjPointAdd( double projX, double projY )
{
	double pixelX, pixelY;
	ProjToPixel(projX, projY, pixelX, pixelY);
	AddPoint(projX, projY, pixelX, pixelY);
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

#pragma region Calculate area

// *******************************************************
//		GetArea()
// *******************************************************
double ActiveShape::GetArea(bool closingPoint, double x, double y)
{
	if (GetPolygonPointCount(closingPoint) < 3)
		return false;

	if (GetTransformationMode() == tmNotDefined)
	{
		return GetEuclidianArea(closingPoint, x, y);   // x, y are projected
	}
	else
	{
		double xDeg = x, yDeg = y;
		if (closingPoint) TransformPoint(xDeg, yDeg);
		return GetGeodesicArea(closingPoint, xDeg, yDeg);	  // x, y are decimal degrees
	}
}




// *******************************************************
//		GetEuclidianArea()
// *******************************************************
double ActiveShape::GetEuclidianArea(bool closingPoint, double x, double y) 
{
	double val = 0.0;
	if (GetPolygonPointCount(closingPoint) > 2)
	{
		VARIANT_BOOL vb;
		_areaCalcShape->Create(ShpfileType::SHP_POLYGON, &vb);		// this will clear points

		long pointIndex = -1;

		for (size_t i = GetFirstPolyPointIndex(); i < _points.size(); i++)
		{
			_areaCalcShape->AddPoint(_points[i]->Proj.x, _points[i]->Proj.y, &pointIndex);
		}

		if (closingPoint)
			_areaCalcShape->AddPoint(x, y, &pointIndex);

		_areaCalcShape->AddPoint(_points[0]->Proj.x, _points[0]->Proj.y, &pointIndex);   // we need to close the poly
		_areaCalcShape->get_Area(&val);
	}
	return val;
}
#pragma endregion


#pragma region Distance calculation
// *******************************************************
//		getDistance()
// *******************************************************
double ActiveShape::GetDistance() 
{
	if (GetTransformationMode() == tmNotDefined)
	{
		return GetEuclidianDistance();	// if there us undefined or incompatible projection; return distance on plane 
	}
	else
	{
		return GetGeodesicDistance();
	}
}

// *******************************************************
//		GetEuclidianDistance()
// *******************************************************
// in map units specified by current projection
double ActiveShape::GetEuclidianDistance()
{
	double dist = 0.0;
	if (_points.size() > 0) 
	{
		for (size_t i = 0; i < _points.size() - 1; i++)
		{
			dist += _points[i]->Proj.GetDistance(_points[i + 1]->Proj);
		}
	}
	return dist;
}

// *******************************************************
//		GetGeodesicDistance()
// *******************************************************
// in meters with decimal degrees as input
double ActiveShape::GetGeodesicDistance() 
{
	IUtils* utils = GetUtils();
	double dist = 0.0;
	if (_points.size() > 0) 
	{
		for (size_t i = 0; i < _points.size() - 1; i++)
		{
			double val;
			utils->GeodesicDistance(_points[i]->y, _points[i]->x, _points[i + 1]->y, _points[i + 1]->x, &val);
			dist += val;
		}
	}
	return dist;
}
#pragma endregion

// **************************************************************
//		ErrorMessage()
// **************************************************************
void ActiveShape::ErrorMessage(long ErrorCode)
{
	// TODO!!!: report error through callback function
}

// **************************************************************
//		UndoPoint()
// **************************************************************
bool ActiveShape::UndoPoint()
{
	bool result = false;
	if (_points.size() > 0) {
		delete _points[_points.size() - 1];
		result = true;
		_points.pop_back();
		_areaRecalcIsNeeded = true;
	}
	UpdatePolyCloseState(false);
	return result;
}

// ***************************************************************
//		GetSegmentAngle()
// ***************************************************************
double ActiveShape::GetSegmentAngle(int segmentIndex)
{
	if (segmentIndex < 0 || segmentIndex >= (long)_points.size() - 1)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return 0;
	}
	else
	{
		int i = segmentIndex;
		double xScr, yScr, xScr2, yScr2;
		ProjToPixel(_points[i]->Proj.x, _points[i]->Proj.y, xScr, yScr);
		ProjToPixel(_points[i + 1]->Proj.x, _points[i + 1]->Proj.y, xScr2, yScr2);
		double x = xScr - xScr2;
		double y = yScr - yScr2;
		double angle = 360.0 - GetPointAngle(x, y) * 180.0 / pi_;							
		if (angle == 360.0) angle = 0.0;
		return angle;
	}
}

// **************************************************************
//		GetSegmentLength()
// **************************************************************
double ActiveShape::GetSegmentLength(int segmentIndex)
{
	if (segmentIndex < 0 || segmentIndex >= (long)_points.size() - 1)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return 0;
	}
	else
	{
		double result;
		int i = segmentIndex;
		if (GetTransformationMode() == tmNotDefined)
		{
			result = _points[i]->Proj.GetDistance(_points[i + 1]->Proj);;
		}
		else
		{
			IUtils* utils = GetUtils();
			utils->GeodesicDistance(_points[i]->y, _points[i]->x, _points[i + 1]->y, _points[i + 1]->x, &result);
		}
		return result;
	}
}

// **************************************************************
//		GetDymanicLineDistance()
// **************************************************************
double ActiveShape::GetDynamicLineDistance()
{
	double dist;
	double xLng, yLat;
	PixelToProj(_mousePoint.x, _mousePoint.y, xLng, yLat);
	if (TransformPoint(xLng, yLat))
	{
		GetUtils()->GeodesicDistance(_points[_points.size() - 1]->y, _points[_points.size() - 1]->x, yLat, xLng, &dist);
	}
	else
	{
		dist = _points[_points.size() - 1]->Proj.GetDistance(xLng, yLat);
	}
	return dist;
}

// ****************************************************************************/
//   GetAzimuth()
// ****************************************************************************/
double GetAzimuth(MeasurePoint* pnt1, MeasurePoint* pnt2)
{
	double x = pnt2->Proj.x - pnt1->Proj.x;
	double y = pnt2->Proj.y - pnt1->Proj.y;
	return GetPointAngle(x, y)/pi_*180.0;
}

// ****************************************************************************/
//   GetBearing()
// ****************************************************************************/
double ActiveShape::GetBearing( int vertexIndex, bool clockwise )
{
	double az1 = GetAzimuth(_points[vertexIndex - 1], _points[vertexIndex]);
	double az2 = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
	if (az2 - 180 > az1) az1 = az1 + 360;
	if (az1 - 180 > az2) az2 = az2 + 360;
	return clockwise ? 180 + (az1 - az2) : 180 + (az2 - az1);
}

// ****************************************************************************/
//   GetBearingLabelAngle()
// ****************************************************************************/
double ActiveShape::GetBearingLabelAngle( int vertexIndex, bool clockwise )
{
	double az;
	double bearing = GetBearing(vertexIndex, clockwise);
	if (clockwise)
	{
		az = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
		az += bearing / 2.0;
	}
	else
	{
		az = GetAzimuth(_points[vertexIndex], _points[vertexIndex + 1]);
		az -= bearing / 2.0;
	}
	if (az > 360.0) az -= 360.0;
	if (az < 0.0) az += 360.0;
	return az;
}

// ****************************************************************************/
//   GetExtents()
// ****************************************************************************/
void ActiveShape::GetExtents( Extent& extent )
{
	double xMin = DBL_MAX, xMax = -DBL_MAX, yMin = DBL_MAX, yMax = -DBL_MAX;
	for(size_t i = 0; i < _points.size(); i++)
	{
		if (_points[i]->Proj.x > xMax) xMax = _points[i]->Proj.x;
		if (_points[i]->Proj.x < xMin) xMin = _points[i]->Proj.x;
		if (_points[i]->Proj.y > yMax) yMax = _points[i]->Proj.y;
		if (_points[i]->Proj.y > yMin) yMin = _points[i]->Proj.y;
	}
	extent.left = xMin;
	extent.right = xMax;
	extent.top = yMax;
	extent.bottom = yMin;
}

// ****************************************************************************/
//   GetPartCount()
// ****************************************************************************/
int ActiveShape::GetNumParts()
{
	int count = 0;
	for (size_t i = 0; i < _points.size(); i++) {
		if (_points[i]->Part == PartBegin)
			count++;
	}
	if (count == 0) count = 1;
	return count;
}

// *******************************************************
//		GetPartStart()
// *******************************************************
int ActiveShape::GetPartStart(int partIndex) {
	int count = -1;
	for (size_t i = 0; i < _points.size(); i++) {
		if (_points[i]->Part == PartBegin) 
			count++;
		if (count == partIndex)
			return i;
	}
	return 0;
}

// *******************************************************
//		GetPartStart()
// *******************************************************
int ActiveShape::SeekPartEnd(int startSearchFrom) {
	for (size_t i = startSearchFrom; i < _points.size(); i++) {
		if (_points[i]->Part == PartEnd)
			return i;
	}
	return _points.size() - 1;
}

// *******************************************************
//		GetPartStart()
// *******************************************************
int ActiveShape::SeekPartStart(int startSearchFrom) {
	for (size_t i = startSearchFrom; i >= 0; i--) {
		if (_points[i]->Part == PartBegin)
			return i;
	}
	return 0;
}

// *******************************************************
//		GetCloseIndex()
// *******************************************************
int ActiveShape::GetCloseIndex(int startIndex)
{
	bool polygon = Utility::ShapeTypeConvert2D(GetShapeType()) == SHP_POLYGON;
	int closeIndex = -1;
	if (polygon) {
		if (_points[startIndex]->Part == PartBegin) closeIndex = SeekPartEnd(startIndex);
		if (_points[startIndex]->Part == PartEnd) closeIndex = SeekPartStart(startIndex);
	}
	return closeIndex;
}

// *******************************************************
//		RemoveVertex()
// *******************************************************
bool ActiveShape::RemoveVertex(int vertexIndex)
{
	if (vertexIndex < 0 && vertexIndex >= (int)_points.size())
		return false;

	bool polygon = GetShapeType() == SHP_POLYGON;
	bool polyline = GetShapeType() == SHP_POLYLINE;
	
	int startIndex = SeekPartStart(vertexIndex);
	int numPoints = SeekPartEnd(startIndex) - startIndex + 1;

	if (polygon && numPoints <= 4) return false;
	if (polyline && numPoints <= 2) return false;

	PointPart part = _points[vertexIndex]->Part;

	delete _points[vertexIndex];
	_points.erase(_points.begin() + vertexIndex);

	if (vertexIndex == _selectedVertex)
		_selectedVertex = -1;

	if (part == PartEnd) vertexIndex--;
	if (part != PartNone)
		_points[vertexIndex]->Part = part;

	// make sure that first and last points of poly are still the same		
	int closeIndex = GetCloseIndex(vertexIndex);
	
	if (polygon && part != PartNone) {
		MeasurePoint* source = _points[vertexIndex];
		MeasurePoint* target = _points[closeIndex];
		source->CopyTo(*target);
	}
	return true;
}
