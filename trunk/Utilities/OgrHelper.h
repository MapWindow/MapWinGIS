#pragma once
#include "ogr_feature.h"
#include "OgrLayer.h"

class OgrHelper
{
public:
	static bool OgrHelper::Shapefile2OgrLayer(IShapefile* sf, OGRLayer* poLayer, ICallback* callback = NULL);
	static IShapefile* Layer2Shapefile(OGRLayer* layer, ICallback* callback = NULL);
	static CString GetLayerCapabilityString(tkOgrLayerCapability capability);
	static CString GetDsCapabilityString(tkOgrDSCapability capability);
	static int SaveShapefileChanges(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, tkOgrSaveType saveType, bool validateShapes, vector<OgrUpdateError>& errors);
	static bool ShapeRecord2Feature(IShapefile* sf, long shapeIndex, OGRFeature* feature, OGRFeatureDefn* fields, tkOgrSaveType saveType, bool editing, CString& validationError, bool validateShape = false);
	static int RemoveDeletedFeatures(OGRLayer* layer, IShapefile* sf, long shapeCmnIndex);
	static CStringA Bstr2OgrString(BSTR& inputString);
	static CStringW OgrString2Unicode(const char* outputString);
private:	
	static CStringA Bstr2OgrString(BSTR& inputString, tkOgrEncoding encoding);
	static CStringW OgrString2Unicode(const char* outputString, tkOgrEncoding encoding);
};

