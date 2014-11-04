#pragma once
#include "ogrsf_frmts.h"
#include "ogr_feature.h"
#include "OgrLoader.h"
#include "OgrLabels.h"

class Ogr2RawData
{
public:
	static bool Layer2RawData(OGRLayer* layer, Extent* extents, OgrDynamicLoader* loader, vector<CategoriesData*>& categories);
private:
	static void FieldsToShapeRecord(OGRFeatureDefn* poFields, OGRFeature* poFeature, ShapeRecordData* data, bool hasFid, bool hasLabels, OgrLabelsHelper::LabelFields& labelFields);
	static bool GenerateLabels(vector<ShapeRecordData*>& data, vector<CString>& fields, CStringW expression, CString& error, OgrDynamicLoader* loader);
	static void UpdateLabelsAndCategories(vector<ShapeRecordData*>&data, OgrDynamicLoader* loader, bool hasLabels);
	static int GetLabelFieldIndex(CString expression, vector<CString>& fields);
	static void ApplyCategories(vector<ShapeRecordData*>& data, vector<CString>& fields, vector<CategoriesData*>& categories, OgrDynamicLoader* loader);
	static bool PopulateExpressionFields(vector<ShapeRecordData*>& data, int rowIndex, CExpression& expr);
};

