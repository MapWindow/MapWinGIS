#pragma once
#include "ogrsf_frmts.h"
#include "ogr_feature.h"
#include "OgrLoader.h"
#include "OgrLabels.h"
#include "CustomExpression.h"

class Ogr2RawData
{
 public:
	static bool Ogr2RawData::Layer2RawData(OGRLayer* layer, Extent* extents, OgrDynamicLoader* loader, OgrLoadingTask* callback);
 private:
	static void Ogr2RawData::FieldsToShapeRecord(OGRFeatureDefn* poFields, OGRFeature* poFeature, ShapeRecordData* data, bool hasFid);
	static void DeleteAndClearShapeData(vector<ShapeRecordData*>& data);
};

