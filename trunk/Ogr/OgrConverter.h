//********************************************************************************************************
//File name: OGR2MWShape.h
//Description: functions for conversion between OGRGeomerty Class and MapWinGis Shape Representation
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The initial version of this file was written by Sergei Leschinski (31 jul 2009)
//Contributor(s): (Open source contributors should list themselves and their modifications here). 

#pragma once
#include "ogrsf_frmts.h"
using namespace std;

class OgrConverter
{
	public:
		static IShape * GeometryToShape(OGRGeometry* oGeom, bool isM, OGRwkbGeometryType oBaseType = wkbNone, OGRwkbGeometryType oForceType = wkbNone);
		static bool GeometryToShapes(OGRGeometry* oGeom, vector<IShape *>* vShapes, bool isM, OGRwkbGeometryType oForceType = wkbNone);
		static OGRGeometry* ShapeToGeometry(IShape* shp, OGRwkbGeometryType forceGeometryType = wkbNone);
		static IShapefile* ReadOgrLayer(BSTR Filename, ShpfileType shpType = SHP_NULLSHAPE);
		static bool WriteOgrLayer(IShapefile* sf, BSTR Filename);
		static bool MultiPolygon2Polygons(OGRGeometry* geom, vector<OGRGeometry*>* results);
		static ShpfileType GeometryType2ShapeType(OGRwkbGeometryType oType);
		static OGRwkbGeometryType ShapeType2GeometryType(ShpfileType shpType, bool forceMulti = false);
		
	private:
};



