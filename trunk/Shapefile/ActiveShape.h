#pragma once


// TODO: perhaps add more rendering options: transparency, color, width, style, vertex size
class ActiveShape
{
public:	
	ActiveShape(): _textBrush(Gdiplus::Color::Black), _whiteBrush(Gdiplus::Color::White),
		_linePen(Gdiplus::Color::Orange, 2.0f), _fillBrush(Gdiplus::Color(100, 255, 165, 0)),
		_bluePen(Gdiplus::Color::Blue, 1.0f), _blueBrush(Gdiplus::Color::LightBlue),
		_redPen(Gdiplus::Color::Red, 1.0f), _redBrush(Gdiplus::Color::LightCoral)
	{
		_mapView = NULL;
		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&_areaCalcShape);

		_font = Utility::GetGdiPlusFont("Times New Roman", 9);
		_fontArea = Utility::GetGdiPlusFont("Arial", 12);
		_format.SetAlignment(Gdiplus::StringAlignmentCenter);
		_format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		_formatLeft.SetAlignment(Gdiplus::StringAlignmentNear);
		_formatLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);

		_mixedLinePolyMode = true;		// TODO: set externally
		_drawLineForPoly = true;
		_lengthRounding = 1;
		_areaRounding = 1;
		_angleCorrection = 0.0;
		_drawLabelsOnly = false;
		_geometryVisible = true;
		_selectedVertex = -1;
		_highlightedVertex = -1;

		AreaDisplayMode = admMetric;
		AngleDisplayMode = Azimuth;

		SetDefaults();
	};
	virtual ~ActiveShape(void) {
		Clear();
		if (_areaCalcShape) _areaCalcShape->Release();
		delete _font;
		delete _fontArea;
	};
	
	virtual void Clear();
	void SetDefaults();
	void ErrorMessage(long ErrorCode);
private:
	Gdiplus::Pen _linePen;
	Gdiplus::Pen _bluePen;
	Gdiplus::Pen _redPen;
	Gdiplus::SolidBrush _fillBrush;
	Gdiplus::SolidBrush _blueBrush;
	Gdiplus::SolidBrush _redBrush;
	

protected:
	enum ScreenPointsType
	{
		ScreenPointsAll = 0,
		ScreenPointsLinearPart = 1,
		ScreenPointsPolygonPart = 2,
	};
	
	std::vector<MeasurePoint*> _points;		   // points in decimal degrees (in case transformation to WGS84 is possible)
	ShapeInputMode _inputMode;
	bool _drawLineForPoly;
	void* _mapView;
	bool _isGeodesic;
	bool _areaRecalcIsNeeded;		// geodesic area should be recalculated a new (after a point was added or removed)
	IShape* _areaCalcShape;
	Point2D _mousePoint;			// points entered by user (in map units, whatever they are)
	Gdiplus::Font* _fontArea;
	Gdiplus::Font* _font;
	Gdiplus::StringFormat _format;
	Gdiplus::StringFormat _formatLeft;

public:
	bool _geometryVisible;
	bool _drawLabelsOnly;
	bool _mixedLinePolyMode;
	Gdiplus::SolidBrush _textBrush; // Black;
	Gdiplus::SolidBrush _whiteBrush;// White;
	int _firstPolyPointIndex;
	int _lengthRounding;
	int _areaRounding;
	double _angleCorrection;
	int _selectedVertex;
	int _highlightedVertex;
public:
	BYTE FillTransparency;
	float LineWidth;
	bool DisplayAngles;
	tkAreaDisplayMode AreaDisplayMode;
	tkAngleDisplay AngleDisplayMode;
	tkAngleFormat AngleFormat;
	OLE_COLOR FillColor;
	OLE_COLOR LineColor;

protected:

	// abstract methods
	virtual bool CloseOnPreviousVertex() = 0;
	virtual void UpdatePolyCloseState(bool close, int pointIndex = -1) = 0;
	virtual bool HasStaticPolygon() = 0;
	virtual bool HasDynamicPolygon() = 0;
	virtual bool HasStaticLine() = 0;
	virtual bool HasDynamicLine() = 0;
	virtual void ClearIfStopped() = 0;
	virtual bool SnapToPreviousVertex(int& vertexIndex, double screenX, double screenY) = 0;
	virtual bool DrawAccumalatedLength() = 0;
	virtual bool HasClosedPolygon() = 0;
public:	
	virtual bool IsDynamic() = 0;

public:
	/*void SetBlueFillColor()
	{
		_orangePen.SetColor(Gdiplus::Color(255, 30, 144, 255));
		_orangeBrush.SetColor(Gdiplus::Color(100, 173, 216, 230));
	}*/

	// -------------------------------------------------
	//   Exposing properties
	// -------------------------------------------------
	virtual ShpfileType GetShapeType() { return SHP_POLYLINE; }
	ShapeInputMode GetInputMode() { return _inputMode; }
	bool HasPolygon() { return _firstPolyPointIndex != -1; }
	bool HasLine() { return _firstPolyPointIndex == -1 || _firstPolyPointIndex > 0; }
	int GetFirstPolyPointIndex()
	{
		return _firstPolyPointIndex == -1 ? 0 : _firstPolyPointIndex;
	}
	bool IsEmpty() 	{ return _points.size() == 0;	}
	bool HasSelectedVertex()
	{
		return _selectedVertex >= 0 && _selectedVertex < (int)_points.size();
	}

	// -------------------------------------------------
	//    Map projection and coordinate transformations
	// -------------------------------------------------
	IGeoProjection* GetMapProjection();
	IGeoProjection* GetWgs84Projection();
	tkTransformationMode GetTransformationMode();
	bool IsGeodesic() { return _isGeodesic;}
	bool HasProjection() { return GetTransformationMode() != tmNotDefined; }
	void SetMapView(void* mapView, ShapeInputMode inputMode);
	void ProjToPixel(double projX, double projY, double& pixelX, double& pixelY);
	void PixelToProj(double pixelX, double pixelY, double& projX, double& projY);
	void* GetMapView();
	
	// -------------------------------------------------
	//    Distance and area calculation
	// -------------------------------------------------
	double GetDistance();
	double GetEuclidianDistance();
	double GetGeodesicDistance();
	double GetArea(bool closingPoint, double x, double y);
	double GetGeodesicArea(bool closingPoint, double x, double y);
	double GetEuclidianArea(bool closingPoint, double x, double y) ;
	double GetSegmentLength(int segmentIndex);
	double GetSegmentAngle(int segmentIndex);

	// -------------------------------------------------
	//	   Interaction
	// -------------------------------------------------
	// projection should be specified before any calculations are possible
	void AddPoint(double xProj, double yProj, double xScreen, double yScreen);
	void AddPoint(double xProj, double yProj);
	bool TryInsertVertex(double xProj, double yProj);
	int FindSegmentWithPoint(double xProj, double yProj);

	virtual bool UndoPoint();
	bool TransformPoint(double& x, double& y);
	bool HandlePointAdd( double screenX, double screenY, bool ctrl);
	void SetMousePosition(double xScreen, double yScreen)
	{
		_mousePoint.x = xScreen;
		_mousePoint.y = yScreen;
	}
	void Move( double offsetXProj, double offsetYProj );
	void MoveVertex( double offsetXProj, double offsetYProj, bool offset = true );

	int get_ScreenPoints(ScreenPointsType whichPoints, Gdiplus::PointF** data, bool dynamicPoint, OffsetType offsetType, int offsetX, int offsetY);
	int get_ScreenPoints(ScreenPointsType whichPoints, bool hasLastPoint, int lastX, int lastY, Gdiplus::PointF** data);
	
	// TODO: merge
	//int GetPolygonPoints(Gdiplus::PointF** data, bool dynamicPoly);
	//int GetLinePoints(Gdiplus::PointF** data, bool dynamicLine);
	
	int GetPointCount() { return (int)_points.size(); }
	MeasurePoint* GetPoint(int index) { 
		return index >= 0 && index < (int)_points.size() ? _points[index] : NULL;
	}

	int GetPolygonPointCount(bool dynamicPoly) 
	{ 
		if (_firstPolyPointIndex == -1) return 0;
		int size = _points.size() - GetFirstPolyPointIndex();	
		if (dynamicPoly) size++;
		return size;
	}

	bool RemoveSelectedVertex() {
		return RemoveVertex(_selectedVertex);
	}

	bool RemoveVertex(int vertexIndex)
	{
		if (vertexIndex < 0 && vertexIndex >= (int)_points.size())
			return false;

		bool polygon = GetShapeType() == SHP_POLYGON;
		bool polyline = GetShapeType() == SHP_POLYLINE;

		if (polygon && _points.size() <= 4) return false;
		if (polyline && _points.size() <= 2) return false;

		delete _points[vertexIndex];
		_points.erase(_points.begin() + vertexIndex);
		
		// make sure that first and last points of poly are still the same		
		if (polygon) {
			MeasurePoint* target = NULL, *source = NULL;
			if (vertexIndex == 0) {
				 target = _points[_points.size() - 1];
				 source = _points[0];
			}
			else if (vertexIndex == _points.size()) {
				target = _points[0];
				source = _points[_points.size() - 1];
			}
			if (target) {
				source->CopyTo(*target);
			}
		}

		if (vertexIndex == _selectedVertex)
			_selectedVertex = -1;

		return true;
	}

	// -------------------------------------------------
	//	   Drawing
	// -------------------------------------------------
	void DrawData(Gdiplus::Graphics* g, bool dynamicBuffer, OffsetType offsetType, int screenOffsetX = 0, int screenOffsetY = 0);
	void DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, 
		double length, double totalLength, int segmentIndex, bool rumbOnly);
	void DrawMeasuringPolyArea(Gdiplus::Graphics* g, IPoint* pnt, double area);
	IPoint* GetPolygonCenter(Gdiplus::PointF* data, int length);
	void DisplayPolygonArea(Gdiplus::Graphics* g, Gdiplus::PointF* data, int size, bool dynamicPoly);
	void HandleProjPointAdd( double projX, double projY );
	double GetDynamicLineDistance();
	void UpdateLatLng( int pointIndex );
	double GetBearing( int vertexIndex, bool clockwise );
	double GetBearingLabelAngle( int vertexIndex, bool clockwise );
	void GetExtents( Extent& extent );
};
