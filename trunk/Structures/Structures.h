#pragma once
#include "Enumerations.h"
#include "Extent.h"

struct Poly
{	
public:
	Poly(){}
	std::vector<double> polyX;
	std::vector<double> polyY;
};

class HotTrackingInfo
{
public:	
	IShapefile* Shapefile;
	IShape* Shape;
	int ShapeId;
	int LayerHandle;
	HotTrackingInfo(): Shape(NULL), Shapefile(NULL), ShapeId(-1), LayerHandle(-1) {}
	~HotTrackingInfo()
	{
		if (Shape)
			Shape->Release();
	}
};

struct TileBuffer
{
	bool Initialized;  
	int Provider;	// id of provider in buffer
	int Zoom;		// zoom at which current tile buffer was drawn
	Extent Extents;	
};

class TimedPoint
{
public:	
	int x;
	int y;
	DWORD time;

	TimedPoint(int x, int y, DWORD time) :
	x(x), y(y), time(time)	{}
};

// some classes as well but never mind
class ZoombarParts
{
public:
	CRect PlusButton;
	CRect MinusButton;
	CRect Handle;
	CRect Bar;

	ZoombarParts(): 
	PlusButton(0, 0, 0, 0), MinusButton(0, 0, 0, 0), Handle(0, 0, 0, 0), Bar(0, 0, 0, 0){ }

	double GetRelativeZoomFromClick(int y)
	{
		double val = (y - Bar.top) /(double)(Bar.bottom - Bar.top);
		if (val < 0.0) val = 0.0;
		if (val > 1.0) val = 1.0;
		return 1 - val;
	}
};

class DraggingState
{
public:	
	DraggingState() : Start(0, 0), Move(0, 0), Operation(DragNone), HasMoved(false), Snapped(false){};
	DraggingOperation Operation;
	CPoint Start;
	CPoint Move;
	bool HasMoved;
	bool Snapped;
	Point2D Proj;
	bool HasRectangle()
	{
		return !(abs(Start.x - Move.x) < 10 && abs(Start.y - Move.y) < 10);
	}
	CRect GetRectangle()
	{
		CRect r = CRect(Start.x, Start.y, Move.x, Move.y);
		r.NormalizeRect();
		return r;
	};
	void Clear()
	{
		Proj.x = Proj.y = Start.x = Start.y = Move.x = Move.y = 0;
		Operation = DragNone;
		Snapped = false;
		HasMoved = false;
	}
	void SetSnapped(double xProj, double yProj){
		Snapped = true;
		Proj.x = xProj;
		Proj.y = yProj;
	}
};

struct MeasurePoint
{
	PointPart Part;
	Point2D Proj;
	double x;		// in decimal degrees
	double y;
	void CopyTo(MeasurePoint& pnt2) {
		pnt2.x = x;
		pnt2.y = y;
		pnt2.Proj = Proj;
	}
	MeasurePoint() : Part(PartNone) {}
};

struct OgrUpdateError
{
	int ShapeIndex;
	CStringW ErrorMsg;
	OgrUpdateError(int shapeIndex, CStringW msg) : ShapeIndex(shapeIndex), ErrorMsg(msg) {}
};