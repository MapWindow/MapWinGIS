//********************************************************************************************************
//File name: OGR2MWShape.h
//Description: functions for conversion between OGRGeomerty Class and MapWinGis Shape Representation
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
//limitations under the License. 
//
//The initial version of this file was written by Sergei Leschinski (31 jul 2009)
//Contributor(s): (Open source contributors should list themselves and their modifications here). 

#pragma once
#include "MapWinGis.h"
#include "ogrsf_frmts.h"
#include <vector>
#include "clipper.h"
#include "geos_c.h"

using namespace std;
using namespace clipper;

class GeometryConverter
{
	public:
		static IShape * GeometryToShape(OGRGeometry* oGeom, OGRwkbGeometryType oBaseType = wkbNone);
		static bool GeometryToShapes(OGRGeometry* oGeom, vector<IShape *>* vShapes);
		static OGRGeometry* ShapeToGeometry(IShape* shp);
		static IShapefile* Read_OGR_Layer(BSTR Filename, ShpfileType shpType = SHP_NULLSHAPE);
		static bool Write_OGR_Layer(IShapefile* sf, BSTR Filename);
		static bool GEOSGeomToShapes(GEOSGeom gsGeom, vector<IShape*>* vShapes);
		static GEOSGeom Shape2GEOSGeom(IShape* shp);
		static TPolyPolygon* ClipPolygon(TPolyPolygon* polyClip, TPolyPolygon* polySubject, TClipType operation);
		static IShape* ClipPolygon(IShape* shapeClip, IShape* shapeSubject, PolygonOperation operation);
		static bool MultiPolygon2Polygons(OGRGeometry* geom, vector<OGRGeometry*>* results);
		static GEOSGeometry* MergeGeosGeometries( std::vector<GEOSGeometry*>& data, ICallback* callback, bool deleteInput = true );
		
		// coordinates for clipper are multiplied in case of geographic coordinate systems
		double conversionFactor;
		GeometryConverter::GeometryConverter()
		{
			conversionFactor = 1.0;
		}
		TPolyPolygon* Shape2ClipperPolygon(IShape* shp);
		IShape* ClipperPolygon2Shape(TPolyPolygon* polygon);
	private:
		static ShpfileType GeometryType2ShapeType(OGRwkbGeometryType oType);
		static OGRwkbGeometryType ShapeType2GeometryType(ShpfileType shpType);
};


