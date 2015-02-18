#pragma once
#include "ogr_feature.h"
#include "OgrHelper.h"

class Ogr2Shape
{
public:
	static IShapefile* CreateShapefile(OGRLayer* layer);
	static IShapefile* Layer2Shapefile(OGRLayer* layer, int maxFeatureCount, bool& isTrimmed, OgrDynamicLoader* loader, ICallback* callback = NULL);
	static bool FillShapefile(OGRLayer* layer, IShapefile* sf, int maxFeatureCount, bool loadLabels, ICallback* callback, bool& isTrimmed);
};

