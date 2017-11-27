#pragma once
#include "clipper.h"

class ClipperConverter
{
public:	
	// coordinates for clipper are multiplied in case of geographic coordinate systems
	double conversionFactor;
	ClipperConverter() : conversionFactor(1.0){	}
	ClipperConverter(IShapefile* sf) {
		this->SetConversionFactor(sf);
	}
	void SetConversionFactor(IShapefile* sf);
	// Old Clipper lib:
	// ClipperLib::Polygons* Shape2ClipperPolygon(IShape* shp);
	// IShape* ClipperPolygon2Shape(ClipperLib::Polygons* polygon);
	// static ClipperLib::Polygons* ClipPolygon(ClipperLib::Polygons* polyClip, ClipperLib::Polygons* polySubject, ClipperLib::ClipType operation);

	// Clipper lib v6+ uses paths instead of polygons:
	ClipperLib::Paths * Shape2ClipperPolygon(IShape* shp);	
	IShape* ClipperPolygon2Shape(ClipperLib::Paths* polygon);
	static ClipperLib::Paths* ClipPolygon(ClipperLib::Paths* polyClip, ClipperLib::Paths* polySubject, ClipperLib::ClipType operation);

	static IShape* ClipPolygon(IShape* shapeClip, IShape* shapeSubject, PolygonOperation operation);
	static void AddPolygons(IShapefile* sf, ClipperLib::Clipper& clp, ClipperLib::PolyType clipType, bool selectedOnly);
	
};