#pragma once
#include "ShapeData.h"
#include "geos_c.h"
#include "ChartInfo.h"

// Stores all the information about a single shape from the shapefile
struct ShapeRecord
{
	ShapeRecord()
	{
		shape = NULL;
		_renderingData = NULL;
		chart = NULL;
		geosGeom = NULL;

		category = -1;
		originalIndex = -1;
		size = 0;

		rotation = 0.0;

		_flags = 0;
	}

	~ShapeRecord()
	{
		if (shape) {
			shape->Release();
		}

		if (chart) {
			delete chart;
		}

		ReleaseRenderingData();

		if (geosGeom) {
			GEOSGeom_destroy(geosGeom);
		}
	}

private:
	BYTE _flags;
	CShapeData* _renderingData;	// fast non-edit mode	

public:
	IShape* shape;
	CChartInfo* chart;
	GEOSGeom geosGeom;		// caches geometry

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

	bool isVisible() { return _flags & shpVisible ? true : false; }
	void isVisible(bool value) { setVisibilityFlag(shpVisible, value); }

	bool hidden() { return _flags & shpHidden ? true : false; }
	void hidden(bool value) { setVisibilityFlag(shpHidden, value); }

	bool modified() { return _flags & shpModified ? true : false; }
	void modified(bool value) { setVisibilityFlag(shpModified, value); }

	bool wasRendered() { return _flags & shpWasRendered ? true : false; }
	void wasRendered(bool value) { setVisibilityFlag(shpWasRendered, value); }

	IShapeData* get_RenderingData() { return _renderingData; }

	void ReleaseShape() 
	{
		if (shape) {
			shape->Release();
			shape = NULL;
		}
	}

	void put_RenderingData(CShapeData* data) 
	{ 
		ReleaseRenderingData();
		_renderingData = data;
	}

	void ReleaseRenderingData() 
	{
		if (_renderingData){
			delete _renderingData;
			_renderingData = NULL;
		}
	}
};

