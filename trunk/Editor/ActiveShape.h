#pragma once
#include "GeoShape.h"
#include <set>
#include "CollisionList.h"

class ActiveShape: public GeoShape
{
public:	
	ActiveShape(): _textBrush(Gdiplus::Color::Black), _whiteBrush(Gdiplus::Color::White),
		_bluePen(Gdiplus::Color::Blue, 1.0f), _blueBrush(Gdiplus::Color::LightBlue),
		_linePen(Gdiplus::Color::Orange, 2.0f), _fillBrush(Gdiplus::Color::Orange),
		_redPen(Gdiplus::Color::Red, 1.0f), _redBrush(Gdiplus::Color::LightCoral)
	{
		_font = Utility::GetGdiPlusFont("Times New Roman", 9);
		_fontArea = Utility::GetGdiPlusFont("Arial", 12);
		_format.SetAlignment(Gdiplus::StringAlignmentCenter);
		_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		_formatLeft.SetAlignment(Gdiplus::StringAlignmentNear);
		_formatLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		_verticesVisible = true;
		//_drawLineForPoly = true;
		_lengthRounding = 1;
		_areaRounding = 1;
		_drawLabelsOnly = false;
		_selectedVertex = -1;
		_highlightedVertex = -1;
		_pointLabelsVisible = true;
		_lengthDisplayMode = ldmMetric;
		_selectedPart = -1;
		_highlightedPart = -1;
		_inputMode = simMeasuring;
		_isGeodesic = false;
		OverlayerTool = false;
		FillTransparency = 100;
		FillColor = RGB(255, 165, 0);
		LineColor = RGB(255, 127, 0);
		LineWidth = 2.0f;
		AreaDisplayMode = admMetric;
		AngleDisplayMode = Azimuth;
		DisplayAngles = false;

		
	};

	virtual ~ActiveShape(void) {
		Clear();
		
		delete _font;
		delete _fontArea;
	};
	
	virtual void Clear();
private:
	Gdiplus::Pen _linePen;
	Gdiplus::Pen _bluePen;
	Gdiplus::Pen _redPen;
	Gdiplus::SolidBrush _fillBrush;
	Gdiplus::SolidBrush _blueBrush;
	Gdiplus::SolidBrush _redBrush;

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
	bool _pointLabelsVisible;
	bool _verticesVisible;
	
	tkLengthDisplayMode _lengthDisplayMode;
	
	Gdiplus::SolidBrush _textBrush; // Black;
	Gdiplus::SolidBrush _whiteBrush;// White;
	
	int _selectedVertex;
	int _highlightedVertex;
	int _selectedPart;
	int _highlightedPart;

	int _lengthRounding;
	int _areaRounding;

public:
	BYTE FillTransparency;
	float LineWidth;
	bool DisplayAngles;
	tkAreaDisplayMode AreaDisplayMode;
	tkAngleDisplay AngleDisplayMode;
	tkAngleFormat AngleFormat;
	OLE_COLOR FillColor;
	OLE_COLOR LineColor;
	bool OverlayerTool;
protected:

	// abstract methods
	virtual bool CloseOnPreviousVertex() = 0;
	virtual void UpdatePolyCloseState(bool close, int pointIndex = -1) = 0;
	virtual bool HasPolygon(bool dynamicBuffer) = 0;
	virtual bool HasLine(bool dynamicBuffer) = 0;
	virtual void ClearIfStopped() = 0;
	virtual bool SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY) = 0;
	virtual bool DrawAccumalatedLength() = 0;
	virtual bool HasClosedPolygon() = 0;
	
public:	
	
	void SetMapCallback(IMapViewCallback* mapView, ShapeInputMode inputMode);

	// -------------------------------------------------
	//   Exposing properties
	// -------------------------------------------------
	virtual bool GetMixedMode() { return false; }
	virtual bool UndoPoint();
	virtual bool GetPartStartAndEnd(int partIndex, MixedShapePart whichPoints, int& begin, int& end);
	ShapeInputMode GetInputMode() { return _inputMode; }
	void AddPoint(double xProj, double yProj, double xScreen, double yScreen, PointPart part = PartNone);
	void AddPoint(double xProj, double yProj);
	bool HandlePointAdd( double screenX, double screenY, bool ctrl);
	int GetPointCount() { return (int)_points.size(); }
	MeasurePoint* GetPoint(int index) { return index >= 0 && index < (int)_points.size() ? _points[index] : NULL; }
	
	// -------------------------------------------------
	//	 Drawing
	// -------------------------------------------------
	int GetScreenPoints(int partIndex, MixedShapePart whichPoints, Gdiplus::PointF** data, bool dynamicPoint,
		DraggingOperation offsetType, int offsetX, int offsetY);
	int GetScreenPoints(int partIndex, MixedShapePart whichPoints, bool hasLastPoint, int lastX, int lastY, Gdiplus::PointF** data);
	void DrawData(Gdiplus::Graphics* g, bool dynamicBuffer, DraggingOperation offsetType, int screenOffsetX = 0, int screenOffsetY = 0);
	void DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, 
		double length, double totalLength, int segmentIndex, bool rumbOnly);
	void DrawMeasuringPolyArea(Gdiplus::Graphics* g, IPoint* pnt, double area);
	void DrawPolygonArea(Gdiplus::Graphics* g, Gdiplus::PointF* data, int size, bool dynamicPoly);
	void DrawLines(Gdiplus::Graphics* g, int size, Gdiplus::PointF* data, bool dynamicBuffer, int partIndex, CCollisionList& collisionList);
	bool PartIsSelected(int partIndex);
	bool VerticesAreVisible();
	bool PointLabelsAreVisible();
};
