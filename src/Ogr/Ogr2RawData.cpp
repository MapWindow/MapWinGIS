#include "stdafx.h"
#include "Ogr2RawData.h"
#include "OgrHelper.h"
#include "OgrConverter.h"
#include "Shape.h"
#include "CustomExpression.h"

// *************************************************************
//		Layer2RawData()
// *************************************************************
bool Ogr2RawData::Layer2RawData(OGRLayer* layer, Extent* extents, OgrDynamicLoader* loader, OgrLoadingTask* callback)
{
	if (!layer || !extents || !loader) return false;

	Debug::WriteWithThreadId(Debug::Format("View extents: %f %f %f %f", extents->left, extents->right, extents->bottom, extents->top), DebugOgrLoading);

	layer->SetSpatialFilterRect(extents->left, extents->bottom, extents->right, extents->top);

	// Get number of loaded features:
	int numFeatures;
	{ // Locking the provider in this section
		CSingleLock lock(&loader->ProviderLock, TRUE);
		numFeatures = static_cast<int>(layer->GetFeatureCount());
	}
	callback->FeatureCount = numFeatures;

	if (!loader->CanLoad(numFeatures))
	{
		Debug::WriteWithThreadId("Too much features. Skip loading.", DebugOgrLoading);
		return false;
	}

	OGRFeature *poFeature;
	VARIANT_BOOL vb;

	// ! Don't forget to delete the shape records if data is not used !
	vector<ShapeRecordData*> shapeData;

	{ // Locking the provider in this section
		CSingleLock lock(&loader->ProviderLock, TRUE);

		CStringA fidColumn = layer->GetFIDColumn();
		bool hasFID = fidColumn.GetLength() > 0;

		OGRFeatureDefn *poFields = layer->GetLayerDefn();

		layer->ResetReading();
		while ((poFeature = layer->GetNextFeature()) != NULL)
		{
			// Get shape or create empty one:
			IShape* shp = NULL;
			OGRGeometry *oGeom = poFeature->GetGeometryRef();
			if (oGeom)
				shp = OgrConverter::GeometryToShape(oGeom, loader->IsMShapefile);
			if (!shp)  // insert null shape so that client can still access it
				ComHelper::CreateShape(&shp);

			// Get shape record:
			ShapeRecordData* data = new ShapeRecordData();
			shp->ExportToBinary(&(data->Shape), &vb);
			shp->Release();

			FieldsToShapeRecord(poFields, poFeature, data, hasFID);

			shapeData.push_back(data);
			OGRFeature::DestroyFeature(poFeature);

			// Check for a cancel request:
			if (loader->HaveWaitingTasks()) {
				Debug::WriteWithThreadId("Task cancelling.", DebugOgrLoading);
				DeleteAndClearShapeData(shapeData);
				return false;
			}
		}
	}

	// Check for a cancel request:
	if (loader->HaveWaitingTasks()) {
		Debug::WriteWithThreadId("Task cancelling.", DebugOgrLoading);
		DeleteAndClearShapeData(shapeData);
		return false;
	}

	loader->PutData(shapeData);
	callback->LoadedCount = shapeData.size();
	loader->LastSuccessExtents = *extents;

	if (callback->LoadedCount == 0)
		Debug::WriteWithThreadId("Task succeeded but no data loaded.", DebugOgrLoading);
	else
		Debug::WriteWithThreadId("Task succeeded.", DebugOgrLoading);

	return true;
}

// *************************************************************
//		DeleteAndClearShapeData()
// *************************************************************
void Ogr2RawData::DeleteAndClearShapeData(vector<ShapeRecordData*>& shapeData) {
	// Delete referenced objects in the vector
	for (size_t i = 0; i < shapeData.size(); i++) {
		delete shapeData[i];
	}
	shapeData.clear();
}

// *************************************************************
//		FieldsToShapeRecord()
// *************************************************************
void Ogr2RawData::FieldsToShapeRecord(OGRFeatureDefn* poFields, OGRFeature* poFeature, ShapeRecordData* data, bool hasFid)
{
	if (hasFid)
	{
		VARIANT* var = new VARIANT;
		VariantInit(var);
		var->vt = VT_I4;
		var->lVal = static_cast<long>(poFeature->GetFID());
		data->Row->values.push_back(var);
	}

	for (int iFld = 0; iFld < poFields->GetFieldCount(); iFld++)
	{
		OGRFieldDefn* oField = poFields->GetFieldDefn(iFld);
		OGRFieldType type = oField->GetType();

		VARIANT* var = new VARIANT;
		VariantInit(var);
		if (type == OFTInteger)
		{
			var->vt = VT_I4;
			var->lVal = poFeature->GetFieldAsInteger(iFld);
		}
		else if (type == OFTReal)
		{
			var->vt = VT_R8;
			var->dblVal = poFeature->GetFieldAsDouble(iFld);
		}
		else //if (type == OFTString )
		{
			var->vt = VT_BSTR;
			var->bstrVal = A2BSTR(poFeature->GetFieldAsString(iFld));		// BSTR will be cleared by CComVariant destructor
		}
		data->Row->values.push_back(var);
	}
}
