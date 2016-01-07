#pragma once
#include "ogr_feature.h"
#include "OgrHelper.h"
#include <set>
#include "OgrLabels.h"

class Ogr2Shape
{
public:
	static IShapefile* CreateShapefile(OGRLayer* layer, ShpfileType activeShapeType);
	static IShapefile* Layer2Shapefile(OGRLayer* layer, ShpfileType activeShapeType, int maxFeatureCount, bool& isTrimmed, OgrDynamicLoader* loader, ICallback* callback = NULL);
	static bool FillShapefile(OGRLayer* layer, IShapefile* sf, int maxFeatureCount, bool loadLabels, ICallback* callback, bool& isTrimmed);
	static void ReadGeometryTypes(OGRLayer* layer, set<OGRwkbGeometryType>& types);
	static void GeometryTypesToShapeTypes(set<OGRwkbGeometryType>& types, vector<ShpfileType>& result);
private:
	static void CopyValues(OGRFeatureDefn* poFields, OGRFeature* poFeature, IShapefile* sf, bool hasFID, long numShapes, bool loadLabels, OgrLabelsHelper::LabelFields labelFields);
	static void CopyFields(OGRLayer* layer, IShapefile* sf);
	static void ReadShapeTypes(OGRLayer* layer, set<ShpfileType>& types);
	
};

