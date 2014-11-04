#include "stdafx.h"
#include "Ogr2RawData.h"
#include "OgrHelper.h"
#include "GeometryConverter.h"
#include "Shape.h"
#include "Expression.h"

// *************************************************************
//		Layer2RawData()
// *************************************************************
bool Ogr2RawData::Layer2RawData(OGRLayer* layer, Extent* extents, OgrDynamicLoader* loader, 
			vector<CategoriesData*>& categories)
{
	if (!layer || !extents || !loader) return false;

	Debug::WriteWithThreadId("View extents: %f %f %f %f", extents->left, extents->right, extents->bottom, extents->top);
	layer->SetSpatialFilterRect(extents->left, extents->bottom, extents->right, extents->top);

	loader->LockProvider(true);
	int numFeatures = layer->GetFeatureCount();
	loader->LockProvider(false);

	if (!loader->CanLoad(numFeatures))
	{
		Debug::WriteWithThreadId("Too much features. Skip loading.");
		return false;
	}

	// if th loading is aborted by next thread before it's actually loaded, it doesn't matter
	// the new thread will update this variable
	loader->LastSuccessExtents = *extents;

	OGRFeature *poFeature;
	VARIANT_BOOL vb;

	CSingleLock lock(&loader->ProviderLock);
	lock.Lock();

	CStringA name = layer->GetFIDColumn();
	bool hasFID = name.GetLength() > 0;

	map<long, long> fids;
	OGRFeatureDefn *poFields = layer->GetLayerDefn();

	OgrLabelsHelper::LabelFields labelFields;
	bool hasLabels = false;
	if (m_globalSettings.saveOgrLabels) 
		hasLabels = OgrLabelsHelper::GetLabelFields(layer, labelFields);

	bool generateLabels = !hasLabels && numFeatures <= loader->GetMaxCacheCount();

	vector<ShapeRecordData*> list;
	
	layer->ResetReading();
	while ((poFeature = layer->GetNextFeature()) != NULL)
	{
		OGRGeometry *oGeom = poFeature->GetGeometryRef();

		IShape* shp = NULL;
		if (oGeom)
			shp = GeometryConverter::GeometryToShape(oGeom, loader->IsMShapefile);

		if (!shp)  // insert null shape so that client can still access it
			GetUtils()->CreateInstance(tkInterface::idShape, (IDispatch**)&shp);

		ShapeRecordData* data = new ShapeRecordData();
		shp->ExportToBinary(&(data->Shape), &vb);

		if (generateLabels)
		{
			((CShape*)shp)->get_LabelPositionAutoChooseMethod(loader->LabelPosition, 
				data->LabelX, data->LabelY, data->LabelRotation, loader->LabelOrientation);
		}
		
		shp->Release();

		FieldsToShapeRecord(poFields, poFeature, data, hasFID, hasLabels, labelFields);

		list.push_back(data);
		OGRFeature::DestroyFeature(poFeature);

		if (loader->HaveWaitingTasks())
			break;
	}

	vector<CString> fields;
	if ((generateLabels || categories.size() > 0) && (!loader->HaveWaitingTasks()))
	{
		if (!OgrHelper::GetFieldList(layer, fields)) {
			lock.Unlock();
			return false;
		}
	}
	lock.Unlock();

	if (generateLabels) {
		CString error;
		GenerateLabels(list, fields, loader->LabelExpression, error, loader);
	}

	if (categories.size() > 0) {
		ApplyCategories(list, fields, categories, loader);
	}

	bool success = false;
	if (list.size() > 0 && !loader->HaveWaitingTasks())
	{
		// copy to the location accessible by map rendering
		loader->LockData(true);
		loader->Data.insert(loader->Data.end(), list.begin(), list.end());
		loader->LockData(false);
		success = true;
	}
	else {
		for (size_t i = 0; i < list.size(); i++) {
			delete list[i];
		}
	}

	if (success) {
		Debug::WriteWithThreadId("Task succeeded.");
	}

	return success;
}

// *************************************************************
//		ApplyCategories()
// *************************************************************
void Ogr2RawData::ApplyCategories(vector<ShapeRecordData*>& data, vector<CString>& fields, vector<CategoriesData*>& categories, OgrDynamicLoader* loader)
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

		CExpression expr;
		CString error;
		if (valueType == cvExpression)
		{
			expr.SetFields(fields);
			if (!expr.ParseExpression(categories[i]->expression, true, error))
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
							if (result->type == vtBoolean && result->bln)
								data[j]->CategoryIndex = i;
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
int Ogr2RawData::GetLabelFieldIndex(CString expression, vector<CString>& fields)
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
bool Ogr2RawData::PopulateExpressionFields(vector<ShapeRecordData*>& data, int rowIndex, CExpression& expr)
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
bool Ogr2RawData::GenerateLabels(vector<ShapeRecordData*>& data, vector<CString>& fields, CStringW expression, CString& error, OgrDynamicLoader* loader )
{
	if (expression.GetLength() == 0) return false;

	CString format = "%g";

	// maybe it's single field expression; no need for parsing then
	USES_CONVERSION;
	int fieldIndex = GetLabelFieldIndex(W2A(expression), fields);
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

	CExpression expr;
	expr.SetFields(fields);

	CString err;
	
	if (expr.ParseExpression(W2A(expression), true, error))
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
				if (val->type == vtBoolean)
				{
					data[i]->LabelText = val->bln ? L"true" : L"false";
				}
				else if (val->type == vtDouble)
				{
					data[i]->LabelText = Utility::FormatNumber(val->dbl, format);
					
				}
				else if (val->type == vtString)
				{
					data[i]->LabelText = A2W(val->str);
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
		var->lVal = poFeature->GetFID();
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
