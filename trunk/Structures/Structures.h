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

struct LayerShape
{
	long LayerHandle;
	long ShapeIndex;
	LayerShape() : LayerHandle(-1), ShapeIndex(-1) {}
	LayerShape(long layerHandle, long shapeIndex) : LayerHandle(layerHandle), ShapeIndex(shapeIndex){}
	bool IsEmpty() {
		return LayerHandle == -1 || ShapeIndex == -1;
	}
};