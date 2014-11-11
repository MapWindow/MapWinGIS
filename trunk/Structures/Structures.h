#pragma once
#include "Enumerations.h"
#include "Extent.h"
#include "TableRow.h"

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
	void UpdateShapefile()
	{
		VARIANT_BOOL vb;
		if (!Shapefile) {
			CoCreateInstance(CLSID_Shapefile, NULL, CLSCTX_INPROC_SERVER, IID_IShapefile, (void**)&Shapefile);
		}
		else {
			Shapefile->Close(&vb);
		}
	}
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
	DraggingState() : Start(0, 0), Move(0, 0), Operation(DragNone), HasMoved(false), Snapped(false), 
		LayerHandle(-1), Shapefile(NULL), InitAngle(0.0){};
	DraggingOperation Operation;
	CPoint Start;
	CPoint Move;
	bool HasMoved;
	bool Snapped;
	Point2D Proj;
	Point2D RotateCenter;
	int LayerHandle;
	IShapefile* Shapefile;
	double InitAngle;   // degrees
	void CloseShapefile()
	{
		if (!Shapefile) return;
		VARIANT_BOOL vb;
		Shapefile->Close(&vb);
		Shapefile->Release();
		Shapefile = NULL;
	}
	void SetShapefile(IShapefile* sfNew)
	{
		CloseShapefile();
		Shapefile = sfNew;
		SetShapefileColors();
	}
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
		RotateCenter.x = RotateCenter.y = 0.0;
		Operation = DragNone;
		Snapped = false;
		HasMoved = false;
		CloseShapefile();
		LayerHandle = -1;
		InitAngle = 0.0;
	}
	void SetSnapped(double xProj, double yProj){
		Snapped = true;
		Proj.x = xProj;
		Proj.y = yProj;
	}
	int GetOffsetX() { return Move.x - Start.x; }
	int GetOffsetY() { return Move.y - Start.y; }
private:
	void SetShapefileColors()
	{
		if (!Shapefile) return;
		CComPtr<IShapeDrawingOptions> options = NULL;
		Shapefile->get_DefaultDrawingOptions(&options);
		if (options) {
			options->put_FillColor(RGB(255, 0, 0));
			options->put_FillTransparency(100.0f);
			options->put_LineWidth(2.0f);
			options->put_LineColor(RGB(255, 0, 0));
		}
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

struct ShapeRecordData
{
	VARIANT Shape;
	TableRow* Row;
	double LabelX;
	double LabelY;
	double LabelRotation;
	CStringW LabelText;
	int CategoryIndex;
	
	bool HasLabel() { return LabelX != 0 && LabelY != 0 && LabelText.GetLength() > 0;}

	ShapeRecordData() : LabelX(0.0), LabelY(0.0), LabelRotation(0.0), CategoryIndex(-1)
	{
		this->Row = new TableRow();
		VariantInit(&Shape);
	}
	~ShapeRecordData()
	{
		VariantClear(&Shape);
		if (Row)
			delete Row;
	}
};

struct CategoriesData
{
public:
	CComVariant minValue;
	CComVariant maxValue;
	CString expression;
	CString name;
	tkCategoryValue valueType;
	int classificationField;
	bool skip;
};