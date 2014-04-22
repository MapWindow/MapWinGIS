#pragma once
#include "MapWinGIS_i.h"
#include "Enumerations.h"
#include "Extent.h"

struct HotTrackingInfo
{
	IShapefile* Shapefile;
	int ShapeId;
	int LayerHandle;
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
