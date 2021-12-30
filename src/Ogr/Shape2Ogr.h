#pragma once
#include "ogr_feature.h"
#include <set>

class Shape2Ogr
{
public:
	static bool Shapefile2OgrLayer(IShapefile* sf, OGRLayer* poLayer, bool saveLabels, ICallback* callback = NULL);
	static int SaveShapefileChanges(OGRLayer* poLayer, IShapefile* shapefile, 
		long shapeCmnIndex, tkOgrSaveType saveType, bool validateShapes, bool safeToDelete, vector<OgrUpdateError>& errors);
private:
	static int RemoveDeletedFeatures(OGRLayer* layer, IShapefile* sf, long shapeCmnIndex);
	static bool ShapeRecord2Feature(IShapefile* sf, long shapeIndex, OGRFeature* feature, OGRFeatureDefn* fields, tkOgrSaveType saveType, bool editing, CString& validationError, bool validateShape, vector<int>* fieldMap);
	static bool ShapefileFieldsToOgr(IShapefile* sf, OGRLayer* poLayer);
	static void ShapesToOgr(IShapefile* sf, OGRLayer* layer, ICallback* callback);
	static void RecreateFieldsFromShapefile(OGRLayer* layer, IShapefile* sf);
	static OGRFieldType ShapeFieldType2OgrFieldType(FieldType fieldType);
	static void CopyRecordAttributes(IShapefile* sf, long shapeIndex, OGRFeature* feature, bool editing, OGRFeatureDefn* fields, vector<int>* fieldMap);
	static bool SaveShape(OGRLayer* poLayer, OGRFeature* ft, OGRFeatureDefn* fields, IShapefile* shapefile, int shapeIndex, long shapeCmnIndex, tkOgrSaveType saveType, bool validateShapes, vector<OgrUpdateError>& errors, vector<int>& fieldMap);
	static OGRFeature* GetFeature(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, long shapeIndex, long& featId);
	static void CreateField(IField* field, OGRLayer* layer);
	static void GetOldFields(IShapefile* sf, std::set<int>& indices);
	static void RemovedStaleFields(OGRLayer* layer, IShapefile* sf, int fieldCount);
	static void CreateNewFields(OGRLayer* layer, IShapefile* sf);
	static void GetNewFields(IShapefile* sf, std::set<int>& indices);
	static void CopyShapeData(IShapefile* shapefile, OGRLayer* poLayer, long shapeCmnIndex, tkOgrSaveType saveType, bool validateShapes, vector<OgrUpdateError>& errors, int& shapeCount, int& rowCount);
	static void BuildFieldMap(IShapefile* sf, vector<int>& indices);
	static void UpdateModifiedFields(IShapefile* sf, OGRLayer* layer);
};


