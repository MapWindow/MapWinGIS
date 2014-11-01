#pragma once
#include "ogr_feature.h"
#include "OgrLayer.h"

class OgrHelper
{
public:
	static bool OgrHelper::Shapefile2OgrLayer(IShapefile* sf, OGRLayer* poLayer, ICallback* callback = NULL);
	static IShapefile* Layer2Shapefile(OGRLayer* layer, bool& isTrimmed, ICallback* callback = NULL);
	static bool Layer2RawData(OGRLayer* layer, Extent* extents, OgrDynamicLoader* loader);
	static bool FillShapefile(OGRLayer* layer, IShapefile* sf, ICallback* callback, bool& isTrimmed);
	static CString GetLayerCapabilityString(tkOgrLayerCapability capability);
	static CString GetDsCapabilityString(tkOgrDSCapability capability);
	static int SaveShapefileChanges(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, tkOgrSaveType saveType, bool validateShapes, vector<OgrUpdateError>& errors);
	static bool ShapeRecord2Feature(IShapefile* sf, long shapeIndex, OGRFeature* feature, OGRFeatureDefn* fields, tkOgrSaveType saveType, bool editing, CString& validationError, bool validateShape = false);
	static int RemoveDeletedFeatures(OGRLayer* layer, IShapefile* sf, long shapeCmnIndex);
	static CStringA Bstr2OgrString(BSTR& inputString);
	static CStringW OgrString2Unicode(const char* outputString);
	static IShapefile* CreateShapefile(OGRLayer* layer);
private:	
	static CStringA Bstr2OgrString(BSTR& inputString, tkOgrEncoding encoding);
	static CStringW OgrString2Unicode(const char* outputString, tkOgrEncoding encoding);
};

