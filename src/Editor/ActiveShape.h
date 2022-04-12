#pragma once
#include "GeoShape.h"
#include <set>
#include <gsl/util>

#include "CollisionList.h"

class ActiveShape : public GeoShape
{
public:
	ActiveShape() :
		_linePen(Gdiplus::Color::Orange, 2.0f), _bluePen(Gdiplus::Color::Blue, 1.0f),
		_redPen(Gdiplus::Color::Red, 1.0f), _fillBrush(Gdiplus::Color::Orange),
		_blueBrush(Gdiplus::Color::LightBlue), _redBrush(Gdiplus::Color::LightCoral),
		_textBrush(Gdiplus::Color::Black), _whiteBrush(Gdiplus::Color::White)
	{
		RELATIVE_HORIZONTAL_PADDING = 1.3;
		_font = Utility::GetGdiPlusFont("Times New Roman", 9);
		_fontArea = Utility::GetGdiPlusFont("Times New Roman", 11);
		_format.SetAlignment(Gdiplus::StringAlignmentCenter);
		_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		_formatLeft.SetAlignment(Gdiplus::StringAlignmentNear);
		_formatLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		_drawLabelsOnly = false;
		_selectedVertex = -1;
		_highlightedVertex = -1;
		_selectedPart = -1;
		_highlightedPart = -1;
		_inputMode = ShapeInputMode::simMeasuring;

		_showSnapPoint = false;
		_snapPointX = 0;
		_snapPointY = 0;

		AnglePrecision = 1;
		AngleFormat = afDegrees;  // In MapWinGis_i.h
		AreaDisplayMode = admMetric; // In MapWinGis_i.h
		AreaPrecision = 1;
		BearingType = btAbsolute;  // In MapWinGis_i.h
		FillColor = RGB(255, 165, 0);
		FillTransparency = 100;
		LineColor = RGB(255, 127, 0);
		LineWidth = 2.0f;
		LengthUnits = ldmMetric;
		LengthPrecision = 1;
		LineStyle = dsSolid;
		OverlayTool = false;
		PointsVisible = true;
		PointLabelsVisible = true;
		ShowBearing = false;
		ShowLength = true;
		ShowTotalLength = true;
		ShowArea = true;
	};

	virtual ~ActiveShape() {
		Clear();

		delete _font;
		delete _fontArea;
	}

	virtual void Clear();
private:
	Gdiplus::Pen _linePen;
	Gdiplus::Pen _bluePen;
	Gdiplus::Pen _redPen;
	Gdiplus::SolidBrush _fillBrush;
	Gdiplus::SolidBrush _blueBrush;
	Gdiplus::SolidBrush _redBrush;
	double RELATIVE_HORIZONTAL_PADDING;

protected:
	enum MixedShapePart
	{
		ScreenPointsAll = 0,
		LinearPart = 1,
		PolygonPart = 2,
	};

	std::set<int> _selectedParts;
	ShapeInputMode _inputMode;
	Gdiplus::Font* _fontArea;
	Gdiplus::Font* _font;
	Gdiplus::StringFormat _format;
	Gdiplus::StringFormat _formatLeft;

public:
	bool _drawLabelsOnly;

	Gdiplus::SolidBrush _textBrush; // Black;
	Gdiplus::SolidBrush _whiteBrush;// White;

	int _selectedVertex;
	int _highlightedVertex;
	int _selectedPart;
	int _highlightedPart;
	bool _showSnapPoint;
	double _snapPointX;
	double _snapPointY;

public:

	bool ShowBearing;
	bool ShowArea;
	bool ShowLength;
	bool OverlayTool;
	bool PointLabelsVisible;
	bool PointsVisible;
	bool ShowTotalLength;
	int LengthPrecision;
	int AreaPrecision;
	int AnglePrecision;
	tkLengthDisplayMode LengthUnits;
	tkAreaDisplayMode AreaDisplayMode;
	tkBearingType BearingType;
	tkAngleFormat AngleFormat;

	OLE_COLOR FillColor;
	OLE_COLOR LineColor;
	BYTE FillTransparency;
	tkDashStyle LineStyle;
	float LineWidth;

protected:

	// abstract methods
	virtual bool CloseOnPreviousVertex() = 0;
	virtual void UpdatePolyCloseState(bool close, int pointIndex = -1) = 0;
	virtual bool HasPolygon(bool dynamicBuffer) = 0; // TODO: Fix compile warning
	virtual bool HasLine(bool dynamicBuffer) = 0;
	virtual void ClearIfStopped() = 0;
	virtual bool SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY) = 0;
	virtual bool DrawAccumalatedLength() = 0;
	virtual bool HasClosedPolygon() = 0;
	virtual bool GetShowArea() = 0;

	bool VerticesAreVisible();
	bool PointLabelsAreVisible();
	bool PartIsSelected(int partIndex);
	double GetAzimuth(const MeasurePoint* pnt1, const MeasurePoint* pnt2);
	double GetInnerAngle(int vertexIndex, bool clockwise);
	double GetRelativeBearing(int vertexIndex, bool clockwise);
	bool IsRelativeBearing();
public:

	void SetMapCallback(IMapViewCallback* mapView, ShapeInputMode inputMode);

	// -------------------------------------------------
	//   Exposing properties
	// -------------------------------------------------
	virtual bool GetMixedMode() { return false; }
	virtual bool UndoPoint();
	virtual bool GetPartStartAndEnd(int partIndex, MixedShapePart whichPoints, int& startIndex, int& endIndex);
	ShapeInputMode GetInputMode() const { return _inputMode; }
	void AddPoint(double xProj, double yProj, double xScreen, double yScreen, PointPart part = PartNone);
	void AddPoint(double xProj, double yProj);
	bool HandlePointAdd(double screenX, double screenY, bool ctrl);
	int GetPointCount() const { return gsl::narrow_cast<int>(_points.size()); }
	MeasurePoint* GetPoint(const int index) { return index >= 0 && index < gsl::narrow_cast<int>(_points.size()) ? gsl::at(_points, index) : nullptr; }

	// -------------------------------------------------
	//	 Drawing
	// -------------------------------------------------
	int GetScreenPoints(int partIndex, MixedShapePart whichPoints, Gdiplus::PointF** data, bool dynamicPoint,
		DraggingOperation offsetType, int offsetX, int offsetY);
	int GetScreenPoints(int partIndex, MixedShapePart whichPoints, bool hasLastPoint, int lastX, int lastY, Gdiplus::PointF** data);
	void DrawData(Gdiplus::Graphics* g, bool dynamicBuffer, DraggingOperation offsetType, int screenOffsetX = 0, int screenOffsetY = 0);
	void DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2,
		double length, double totalLength, int segmentIndex);
	void DrawPolygonArea(Gdiplus::Graphics* g, Gdiplus::PointF* data, int size, bool dynamicPoly);
	void DrawPolygonArea(Gdiplus::Graphics* g, IPoint* pnt, double area);
	void DrawLines(Gdiplus::Graphics* g, int size, const Gdiplus::PointF* data, bool dynamicBuffer, int partIndex, CCollisionList& collisionList);
	void DrawRelativeBearing(Gdiplus::Graphics* g, int segmentIndex, double xScr, double yScr, Gdiplus::RectF r2, CStringW sBearing);
	void PrepareSegmentLength(const Gdiplus::Graphics* g, double length, double totalLength, double screenLength, int segmentIndex, CStringW& sLength, Gdiplus::RectF& rect);
	CStringW FormatLength(double length, CStringW format, bool unknownUnits);
	void PrepareSegmentBearing(const Gdiplus::Graphics* g, int segmentIndex, double dx, double dy, CStringW& sBearing, Gdiplus::RectF& rect);
	void DrawSegmentLabel(Gdiplus::Graphics* g, CStringW text, Gdiplus::RectF rect, double screenLength, bool aboveLine);
	int GetSelectedVertex() const { return _selectedVertex; }
};
