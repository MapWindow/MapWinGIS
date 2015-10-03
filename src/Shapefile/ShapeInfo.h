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
		
		modified = false;
		selected = false;
		isVisible = false;
		hidden = false;
		wasRendered = false;
		rotation = 0.0;

		status = ShapeValidationStatus::Original;
	}

	~ShapeData()
	{
		if (shape)
		{
			shape->Release();
			shape = NULL;
		}

		if (chart)
		{
			delete chart;
			chart = NULL;
		}

		if (fastData)
		{
			delete fastData;
			fastData = NULL;
		}

		if (geosGeom)
		{
			GEOSGeom_destroy(geosGeom);
			geosGeom = NULL;
		}

		if (fixedShape)
		{
			fixedShape->Release();
			fixedShape = NULL;
		}
	}
	
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

	// TODO: use bit mask
	bool selected;
	bool isVisible;			// because of the visibility expression 		
	bool hidden;			// set per shape explicitly	
	bool modified;			// for saving of OGR layers
	bool wasRendered;
};

