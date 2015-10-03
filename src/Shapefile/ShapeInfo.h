#pragma once
#include "ShapeData.h"
#include "geos_c.h"

struct CChartInfo
{	
public:
	CChartInfo()
	{
		visible	= true;
		x = y = 0.0;
		frame = NULL;
		isDrawn = VARIANT_FALSE;
	}
	~CChartInfo()
	{
		// don't forget to set pointer to NULL after the use of frame
		// or it will be the problems here
		if (frame) 
		{
			delete frame;
			frame = NULL;
		}
	}
	
	VARIANT_BOOL isDrawn;
	CRect* frame;		// and are deleted on the next redraw
	bool visible;
	double x;
	double y;
};

// Stores all the information about a single shape from the shapefile
struct ShapeData
{
	ShapeData()
	{
		shape = NULL;
		fixedShape = NULL;
		fastData = NULL;
		chart = NULL;
		geosGeom = NULL;

		category = -1;
		originalIndex = -1;
		size = 0;

		rotation = 0.0;

		_flags = 0;

		status = ShapeValidationStatus::Original;
	}

	~ShapeData()
	{
		if (shape) {
			shape->Release();
		}

		if (chart) {
			delete chart;
		}

		if (fastData){
			delete fastData;
		}

		if (geosGeom) {
			GEOSGeom_destroy(geosGeom);
		}

		if (fixedShape) {
			fixedShape->Release();
		}
	}
	
private:
	BYTE _flags;

public:
	IShape* shape;
	CChartInfo* chart;
	GEOSGeom geosGeom;		// caches geometry
	IShape* fixedShape;				// will be used as substitute of original in input shapefile
	
	// TODO: remove
	CShapeData* fastData;	// fast non-edit mode	
	ShapeValidationStatus status;
	
	int originalIndex;		// the unique handle of shape, assigned in the increasing order
	long category;			// the ShapefileCategory to which the shape belongs: < 0 - default options
	short size;				// size in pixels at current scale; the shape will be labeled or not considering this size
	
	float rotation;         // for point icons only;

private:
	void setVisibilityFlag(tkShapeRecordFlags flag, bool value) {
		if (value) 
			_flags |= flag;
		else    
			_flags &= (0xFF ^ flag);
	}

public:
	bool selected() { return _flags & shpSelected ? true : false; }
	void selected(bool value) { setVisibilityFlag(shpSelected, value); }

	bool isVisible() { return _flags & shpVisible ? true : false ; }
	void isVisible(bool value) { setVisibilityFlag(shpVisible, value); }

	bool hidden() { return _flags & shpHidden ? true : false; }
	void hidden(bool value) { setVisibilityFlag(shpHidden, value); }

	bool modified() { return _flags & shpModified ? true : false; }
	void modified(bool value) { setVisibilityFlag(shpModified, value); }

	bool wasRendered() { return _flags & shpWasRendered? true : false; }
	void wasRendered(bool value) { setVisibilityFlag(shpWasRendered, value); }
};

