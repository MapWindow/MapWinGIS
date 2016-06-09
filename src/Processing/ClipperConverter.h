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
	ClipperLib::Polygons* Shape2ClipperPolygon(IShape* shp);
	IShape* ClipperPolygon2Shape(ClipperLib::Polygons* polygon);
	static IShape* ClipPolygon(IShape* shapeClip, IShape* shapeSubject, PolygonOperation operation);
	static void AddPolygons(IShapefile* sf, ClipperLib::Clipper& clp, ClipperLib::PolyType clipType, bool selectedOnly);
	static ClipperLib::Polygons* ClipPolygon(ClipperLib::Polygons* polyClip, ClipperLib::Polygons* polySubject, ClipperLib::ClipType operation);
};