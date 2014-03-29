#pragma once
#include "MapWinGis.h"
#include "ShapeData.h"
#include "ogr_spatialref.h"
#include "Chart.h"
#include "geos_c.h"

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
		handle = -1;
		size = 0;
		
		selected = false;
		isVisible = false;

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

		if (status == ShapeValidationStatus::Fixed && fixedShape)
		{
			fixedShape->Release();
			fixedShape = NULL;
		}
	}
	
	IShape* shape;
	CShapeData* fastData;	// fast non-edit mode
	CChartInfo* chart;
	GEOSGeom geosGeom;		// caches geometry
	
	IShape* fixedShape;				// will be used as substitute of original in input shapefil
	ShapeValidationStatus status;
	
	int handle;				// the unique handle of shape, assigned in the increasing order
	int category;			// the ShapefileCategory to which the shape belongs: < 0 - default options
	int size;				// size in pixels at current scale; the shape will be labeled or not considering this size
	
	bool selected;
	bool isVisible;			// because of the visibility expression 		
};
