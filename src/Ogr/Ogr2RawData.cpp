#include "stdafx.h"
#include "Ogr2RawData.h"
#include "OgrHelper.h"
#include "OgrConverter.h"
#include "Shape.h"
#include "CustomExpression.h"

// *************************************************************
//		Layer2RawData()
// *************************************************************
bool Ogr2RawData::Layer2RawData(OGRLayer* layer, Extent* extents, OgrDynamicLoader* loader, 
			vector<CategoriesData*>& categories, OgrLoadingTask* callback)
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
	bool generateLabels = false;

	// ! Don't forget to delete the shape records if data is not used !
	vector<ShapeRecordData*> shapeData;
	vector<CStringW> fieldNames;

	{ // Locking the provider in this section
		CSingleLock lock(&loader->ProviderLock, TRUE);

		CStringA fidColumn = layer->GetFIDColumn();
		bool hasFID = fidColumn.GetLength() > 0;

		map<long, long> fids;
		OGRFeatureDefn *poFields = layer->GetLayerDefn();

		OgrLabelsHelper::LabelFields labelFields;
		bool hasLabels = false;
		if (m_globalSettings.saveOgrLabels)
			hasLabels = OgrLabelsHelper::GetLabelFields(layer, labelFields);

		generateLabels = !hasLabels && numFeatures <= loader->GetMaxCacheCount();

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

			if (generateLabels)
			{
				((CShape*)shp)->get_LabelPositionAuto(loader->LabelPosition,
					data->LabelX, data->LabelY, data->LabelRotation, loader->LabelOrientation);
			}

			shp->Release();

			FieldsToShapeRecord(poFields, poFeature, data, hasFID, hasLabels, labelFields);

			shapeData.push_back(data);
			OGRFeature::DestroyFeature(poFeature);

			// Check for a cancel request:
			if (loader->HaveWaitingTasks()) {
				Debug::WriteWithThreadId("Task cancelling.", DebugOgrLoading);
				DeleteAndClearShapeData(shapeData);
				return false;
			}
		}
	
		// If we need to generate labels or apply catagories, ensure we have field names:
		if ((generateLabels || categories.size() > 0) && !OgrHelper::GetFieldList(layer, fieldNames)) {
			Debug::WriteWithThreadId("Task cancelling.", DebugOgrLoading);
			DeleteAndClearShapeData(shapeData);
			return false;
		}
	}

	// Check for a cancel request:
	if (loader->HaveWaitingTasks()) {
		Debug::WriteWithThreadId("Task cancelling.", DebugOgrLoading);
		DeleteAndClearShapeData(shapeData);
		return false;
	}
		
	// Process loaded data:
	if (shapeData.size() > 0)
	{
		// Generate labels
		if (generateLabels) {
			CStringW error;
			GenerateLabels(shapeData, fieldNames, loader->LabelExpression, error, loader);
		}

		// Apply categories
		if (categories.size() > 0) {
			ApplyCategories(shapeData, fieldNames, categories, loader);
		}

		// copy to the location accessible by map rendering
		loader->PutData(shapeData);
		callback->LoadedCount = shapeData.size();
		loader->LastSuccessExtents = *extents;
		Debug::WriteWithThreadId("Task succeeded.", DebugOgrLoading);
		return true;
	}
	else 
	{
		Debug::WriteWithThreadId("Task succeeded but no data loaded.", DebugOgrLoading);
		DeleteAndClearShapeData(shapeData);
		return false;
	}
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
//		ApplyCategories()
// *************************************************************
void Ogr2RawData::ApplyCategories(vector<ShapeRecordData*>& data, vector<CStringW>& fields, vector<CategoriesData*>& categories, OgrDynamicLoader* loader)
{
	for (size_t i = 0; i < categories.size(); i++)
	{
		if (loader->HaveWaitingTasks()) {
			return;
		}

		categories[i]->skip = false;
		CComVariant* minVal = &categories[i]->minValue;
		CComVariant* maxVal = &categories[i]->maxValue;

		if (categories[i]->classificationField == -1)
		categories[i]->valueType = cvExpression;

		switch (categories[i]->valueType)
		{
			case cvSingleValue:
				if (minVal->vt == VT_EMPTY)
					categories[i]->skip = true;
				break;
			case cvRange:
				if (minVal->vt == VT_EMPTY || maxVal->vt == VT_EMPTY)
					categories[i]->skip = true;
				break;
		}
	}

	CString format = "%g";
	USES_CONVERSION;
	for (size_t i = 0; i < categories.size(); i++)
	{
		if (categories[i]->skip) continue;

		int fieldIndex = categories[i]->classificationField;

		CComVariant* minVal = &categories[i]->minValue;
		CComVariant* maxVal = &categories[i]->maxValue;

		tkCategoryValue valueType = categories[i]->valueType;

		if (loader->HaveWaitingTasks()) {
			return;
		}

		CustomExpression expr;
		CStringW error;
		if (valueType == cvExpression)
		{
			expr.SetFields(fields);
			if (!expr.Parse(categories[i]->expression, true, error))
				continue;
		}

		for (size_t j = 0; j < data.size(); j++)
		{
			if (data[j]->CategoryIndex != -1) continue;

			switch (valueType)
			{
				case cvSingleValue:
					{
						// CComVar does have oveloaded == operator for VARIANT type
						if (*minVal == *data[j]->Row->values[fieldIndex]) {
							data[j]->CategoryIndex = i;
							break;
						}
					}
					break;
				case cvRange:
						// CComVar does have oveloaded == operator for VARIANT type
						if (*minVal < *data[j]->Row->values[fieldIndex] &&
							*maxVal > *data[j]->Row->values[fieldIndex]) 
						{
							data[j]->CategoryIndex = i;
							break;
						}
					break;
				case cvExpression:
				{
					if (!PopulateExpressionFields(data, i, expr))
						continue;

					// no need to delete the result
					CExpressionValue* val = expr.Calculate(error);
					if (val)
					{
						CExpressionValue* result = expr.Calculate(error);
						if (result)
						{
							if (result->isBoolean() && result->bln()) {
								data[j]->CategoryIndex = i;
							}							
						}
					}
					break;
				}
			}
		}
	}
}

// *************************************************************
//		GetLabelFieldIndex()
// *************************************************************
int Ogr2RawData::GetLabelFieldIndex(CStringW expression, vector<CStringW>& fields)
{
	expression = expression.Trim();
	if (expression.Mid(0, 1) == L"[" && expression.Mid(expression.GetLength() - 1, 1) == L"]")
		expression = expression.Mid(1, expression.GetLength() - 2).Trim();

	for (size_t i = 0; i < fields.size(); i++)
	{
		if (expression.CompareNoCase(fields[i]) == 0)
			return i;
	}
	return -1;
}

// *************************************************************
//		PopulateExpressionFields()
// *************************************************************
bool Ogr2RawData::PopulateExpressionFields(vector<ShapeRecordData*>& data, int rowIndex, CustomExpression& expr)
{
	bool success = false;

	for (int j = 0; j < expr.get_NumFields(); j++)
	{
		int fieldIndex = expr.get_FieldIndex(j);

		VARIANT* var = data[rowIndex]->Row->GetValue(fieldIndex);
		if (var)
		{
			switch (var->vt)
			{
				case VT_BSTR: expr.put_FieldValue(j, var->bstrVal); break;
				case VT_I4:	  expr.put_FieldValue(j, (double)var->lVal); break;
				case VT_R8:	  expr.put_FieldValue(j, (double)var->dblVal); break;
			}
			success = true;
		}
	}
	return success;
}

// *************************************************************
//		FieldsToShapeRecord()
// *************************************************************
bool Ogr2RawData::GenerateLabels(vector<ShapeRecordData*>& data, vector<CStringW>& fields, CStringW expression, CStringW& error, OgrDynamicLoader* loader )
{
	if (expression.GetLength() == 0) return false;

	CString format = "%g";

	// maybe it's single field expression; no need for parsing then
	USES_CONVERSION;
	int fieldIndex = GetLabelFieldIndex(expression, fields);
	if (fieldIndex != -1) 
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			if (loader->HaveWaitingTasks()) {
				return false;
			}

			VARIANT* var = data[i]->Row->GetValue(fieldIndex);
			if (var)
			{
				switch (var->vt)
				{
					case VT_BSTR: 
						data[i]->LabelText = OLE2W(var->bstrVal);
						break;
					case VT_I4:	  
						data[i]->LabelText.Format(L"%d", var->lVal);
						break;
					case VT_R8:	  
						data[i]->LabelText = Utility::FormatNumber(var->dblVal, format);
						break;
				}
			}
		}
		return true;
	}

	CustomExpression expr;
	expr.SetFields(fields);

	CStringW err;
	
	if (expr.Parse(W2A(expression), true, error))
	{
		for (size_t i = 0; i < data.size(); i++)
		{
			if (loader->HaveWaitingTasks()) return false;

			if (!PopulateExpressionFields(data, i, expr))
				continue;

			// no need to delete the result
			CExpressionValue* val = expr.Calculate(err);
			if (val)
			{
				if (val->isBoolean())
				{
					data[i]->LabelText = val->bln() ? L"true" : L"false";
				}
				else if (val->IsDouble())
				{
					data[i]->LabelText = Utility::FormatNumber(val->dbl(), format);
					
				}
				else if (val->isString())
				{
					data[i]->LabelText = val->str();
				}
			}
		}
		return true;
	}
	return false;
}

// *************************************************************
//		FieldsToShapeRecord()
// *************************************************************
void Ogr2RawData::FieldsToShapeRecord(OGRFeatureDefn* poFields, OGRFeature* poFeature, ShapeRecordData* data, bool hasFid,
	bool hasLabels, OgrLabelsHelper::LabelFields& fields)
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

		if (hasLabels)
		{
			if (iFld == fields.X) data->LabelX = var->dblVal;
			if (iFld == fields.Y) data->LabelY = var->dblVal;
			if (iFld == fields.Text) data->LabelText = OgrHelper::OgrString2Unicode(poFeature->GetFieldAsString(iFld));
			if (iFld == fields.Rotation) data->LabelRotation = var->dblVal;
		}
	}
}
