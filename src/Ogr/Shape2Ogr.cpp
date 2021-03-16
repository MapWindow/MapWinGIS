#include "stdafx.h"
#include "Shape2Ogr.h"
#include "OgrLabels.h"
#include "OgrConverter.h"
#include "OgrHelper.h"
#include "OgrLabels.h"
#include "Shapefile.h"
#include "ShapefileHelper.h"
#include "TableHelper.h"

// *************************************************************
//		Shapefile2OgrLayer()
// *************************************************************
bool Shape2Ogr::Shapefile2OgrLayer(IShapefile* sf, OGRLayer* poLayer, bool saveLabels, ICallback* callback /*= NULL*/)
{
	ShapefileFieldsToOgr(sf, poLayer);

	if (m_globalSettings.saveOgrLabels) {
		OgrLabelsHelper::AddFieldsForLabels(poLayer);
	}

	ShapesToOgr(sf, poLayer, callback);

	return true;
}

// *************************************************************
//		ShapefileFieldsToOgr()
// *************************************************************
bool Shape2Ogr::ShapefileFieldsToOgr(IShapefile* sf, OGRLayer* poLayer)
{
	long numFields;
	sf->get_NumFields(&numFields);

	for (int i = 0; i < numFields; i++)
	{
		long precision, width;
		CComBSTR name;
		CComPtr<IField> fld = NULL;
		sf->get_Field(i, &fld);
		fld->get_Name(&name);
		fld->get_Precision(&precision);
		fld->get_Width(&width);

		OGRFieldType fieldType = OgrHelper::GetFieldType(fld);

		OGRFieldDefn oField(OgrHelper::Bstr2OgrString(name.m_str), fieldType);

		// see https://mapwindow.atlassian.net/browse/CORE-81 for details
		if (fieldType == OFTReal && width - precision < 10)
		{
			width = 10 + precision;
		}

		oField.SetWidth(width);
		oField.SetPrecision(precision);

		// insert it
		if (poLayer->CreateField(&oField) != OGRERR_NONE)
			return false;
	}

	return true;
}

// *************************************************************
//		ShapesToOgr()
// *************************************************************
void Shape2Ogr::ShapesToOgr(IShapefile* sf, OGRLayer* poLayer, ICallback* callback)
{
	OGRFeatureDefn* fields = poLayer->GetLayerDefn();
	int fieldCount = fields->GetFieldCount();

	USES_CONVERSION;
	CComVariant var;
	long numShapes, percent = 0;

	sf->get_NumShapes(&numShapes);
	CString validationError;

	CComPtr<ILabels> labels = NULL;
	sf->get_Labels(&labels);

	bool saveLabels = true;
	OgrLabelsHelper::LabelFields labelFields;
	if (!OgrLabelsHelper::GetLabelFields(poLayer, labelFields)) {
		saveLabels = false;
	}

	for (long i = 0; i < numShapes; i++)
	{
		CallbackHelper::Progress(callback, i, numShapes, "Converting shapes...", percent);

		if (((CShapefile*)sf)->ShapeAvailable(i, VARIANT_FALSE))
		{
			OGRFeature* poFeature = OGRFeature::CreateFeature(fields);

			if (ShapeRecord2Feature(sf, i, poFeature, fields, ostSaveAll, false, validationError, true, NULL))
			{
				if (saveLabels) {
					OgrLabelsHelper::AddLabel2Feature(labels, i, poFeature, labelFields);
				}
				
				OGRErr err = poLayer->CreateFeature(poFeature);
				if (err != OGRERR_NONE)
				{
					CallbackHelper::ErrorMsg("Failed to add feature to OGR layer.");
				}
			}
			else {
				CString s = Debug::Format("Geometry import: %s", validationError);
				CallbackHelper::ErrorMsg(s);
			}

			OGRFeature::DestroyFeature(poFeature);
		}
	}

	CallbackHelper::ProgressCompleted(callback);
}

// *************************************************************
//		CopyRecordAttributes()
// *************************************************************
void Shape2Ogr::CopyRecordAttributes(IShapefile* sf, long shapeIndex, OGRFeature* feature, bool editing, 
									OGRFeatureDefn* fields, vector<int>* fieldMap)
{
	CComVariant var;
	double dval;
	long lval, numFields;
	sf->get_NumFields(&numFields);

	USES_CONVERSION;
	int startIndex = editing ? 1 : 0;

	int sourceFieldCount = fields->GetFieldCount();

	for (int j = startIndex; j < numFields; j++)
	{
		sf->get_CellValue(j, shapeIndex, &var);

		int sourceIndex = j - startIndex;
		if (fieldMap) {
			sourceIndex = (*fieldMap)[j];
		}

		if (sourceIndex >= sourceFieldCount) {
			CallbackHelper::ErrorMsg("Invalid source field index for OGR datasource.");
			continue;
		}

		if (var.vt != VT_EMPTY)
		{
			OGRFieldType type = fields->GetFieldDefn(sourceIndex)->GetType();
			switch (type)
			{
				case OGRFieldType::OFTReal:
					dVal(var, dval);
					feature->SetField(sourceIndex, dval);
					break;
				case OGRFieldType::OFTInteger:
					lVal(var, lval);
					feature->SetField(sourceIndex, lval);
					break;
				case OGRFieldType::OFTString:
					feature->SetField(sourceIndex, OLE2A(var.bstrVal));
					break;
			}
		}
	}
}

// *************************************************************
//		ShapeRecord2Feature()
// *************************************************************
bool Shape2Ogr::ShapeRecord2Feature(IShapefile* sf, long shapeIndex, OGRFeature* feature,
	OGRFeatureDefn* fields, tkOgrSaveType saveType, bool editing, CString& validationError, bool validateShape, 
	vector<int>* fieldMap)
{
	if (!sf || !feature) return false;

	if (saveType != tkOgrSaveType::ostGeometryOnly)
	{
		CopyRecordAttributes(sf, shapeIndex, feature, editing, fields, fieldMap);
	}

	if (saveType != tkOgrSaveType::ostAttributesOnly)
	{
		CComPtr<IShape> shp = NULL;
		((CShapefile*)sf)->GetValidatedShape(shapeIndex, &shp);
		if (shp)
		{
			OGRwkbGeometryType type = fields->GetGeomType();

			if (validateShape)
			{
				VARIANT_BOOL isValid;
				shp->get_IsValid(&isValid);
				if (!isValid)
				{
					CComBSTR reason;
					shp->get_IsValidReason(&reason);
					USES_CONVERSION;
					validationError = OLE2A(reason);
					return false;
				}
			}

			OGRGeometry* geom = OgrConverter::ShapeToGeometry(shp, type);
			feature->SetGeometry(geom);
			OGRGeometryFactory::destroyGeometry(geom);
		}
	}
	return true;
}

// *************************************************************
//		ShapeFieldType2OgrFieldType()
// *************************************************************
OGRFieldType Shape2Ogr::ShapeFieldType2OgrFieldType(FieldType fieldType)
{
	switch (fieldType)
	{
		case DOUBLE_FIELD:
			return OGRFieldType::OFTReal;
		case INTEGER_FIELD:
			return OGRFieldType::OFTInteger;
		case STRING_FIELD:
		default:
			return OGRFieldType::OFTString;
	}
}

// *************************************************************
//		CreateNewFields()
// *************************************************************
void Shape2Ogr::CreateNewFields(OGRLayer* layer, IShapefile* sf)
{
	set<int> indices;
	GetNewFields(sf, indices);
	if (indices.size() == 0) return;

	CComPtr<ITable> table = NULL;
	sf->get_Table(&table);
	CTableClass* tableInternal = TableHelper::Cast(table);

	OGRFeatureDefn* fields = layer->GetLayerDefn();

	long numFields;
	sf->get_NumFields(&numFields);

	// the first one is FID
	for (long i = 1; i < numFields; i++)
	{
		if (indices.find(i) != indices.end())
		{
			CComPtr<IField> field = NULL;
			sf->get_Field(i, &field);
			CreateField(field, layer);
			tableInternal->SetFieldSourceIndex(i, fields->GetFieldCount() - 1);
		}
	}
}

// *************************************************************
//		RemoveFields()
// *************************************************************
void Shape2Ogr::RemovedStaleFields(OGRLayer* layer, IShapefile* sf, int maxFieldCount)
{
	set<int> indices;
	GetOldFields(sf, indices);

	OGRFeatureDefn* fields = layer->GetLayerDefn();
	int fieldCount = fields->GetFieldCount();

	// we must not remove fields that were created during this session
	fieldCount = MIN(fieldCount, maxFieldCount);

	// we shall remove both those that are no longer present in the local table
	// and those that were marked as changed
	for (int i = fieldCount - 1; i >= 0; i--)
	{
		if (indices.find(i) == indices.end())
		{
			OGRErr err = layer->DeleteField(i);
			if (err != OGRERR_NONE)
			{
				Debug::WriteError("Failed to remove field: %d", err);
			}
		}
	}
}

// *************************************************************
//		GetNewFields()
// *************************************************************
// We are using shapefile indices here
void Shape2Ogr::GetNewFields(IShapefile* sf, set<int>& indices)
{
	indices.clear();

	CComPtr<ITable> table = NULL;
	sf->get_Table(&table);
	CTableClass* tableInternal = TableHelper::Cast(table);

	long numFields = ShapefileHelper::GetNumFields(sf);
	for (long i = 1; i < numFields; i++)
	{
		CComPtr<IField> field = NULL;
		sf->get_Field(i, &field);

		if (tableInternal->GetFieldSourceIndex(i) == -1)
		{
			indices.insert(i);
		}
	}
}

// *************************************************************
//		GetOldFields()
// *************************************************************
// We are using OGR indices here
void Shape2Ogr::GetOldFields(IShapefile* sf, set<int>& indices)
{
	indices.clear();

	CComPtr<ITable> table = NULL;
	sf->get_Table(&table);
	CTableClass* tableInternal = TableHelper::Cast(table);

	long numFields = ShapefileHelper::GetNumFields(sf);
	for (long i = 0; i < numFields; i++)
	{
		CComPtr<IField> field = NULL;
		sf->get_Field(i, &field);

		int originalIndex = tableInternal->GetFieldSourceIndex(i);

		if (originalIndex != -1)
		{
			indices.insert(originalIndex);
		}
	}
}

// *************************************************************
//		CreateField()
// *************************************************************
void Shape2Ogr::CreateField(IField* field, OGRLayer* layer)
{
	CComBSTR bstr;
	field->get_Name(&bstr);
	CStringA name = Utility::ConvertToUtf8(OLE2W(bstr));

	FieldType fieldType;
	field->get_Type(&fieldType);

	long precision;
	field->get_Precision(&precision);

	long width;
	field->get_Width(&width);

	OGRFieldType ogrType = ShapeFieldType2OgrFieldType(fieldType);
	OGRFieldDefn* ogrField = new OGRFieldDefn(name, ogrType);

	ogrField->SetPrecision(precision);
	ogrField->SetWidth(width);
	OGRErr err = layer->CreateField(ogrField);

	if (err != OGRERR_NONE)
	{
		Debug::WriteError("Failed to create field: %d", err);
	}
}

// *************************************************************
//		GetFeature()
// *************************************************************
OGRFeature* Shape2Ogr::GetFeature(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, long shapeIndex, long& featId)
{
	featId = -1;	

	CComVariant var;
	shapefile->get_CellValue(shapeCmnIndex, shapeIndex, &var);

	lVal(var, featId);
	
	return featId != -1 ? poLayer->GetFeature(featId): NULL;
}

// *************************************************************
//		SaveShapefileChanges()
// *************************************************************
int Shape2Ogr::SaveShapefileChanges(OGRLayer* layer, IShapefile* sf, long shapeCmnIndex, 
	tkOgrSaveType saveType, bool safeToDelete,
	bool validateShapes, vector<OgrUpdateError>& errors)
{
	errors.clear();

	if (!sf || !layer || shapeCmnIndex == -1) return 0;

	OGRFeatureDefn* fields = layer->GetLayerDefn();
	int fieldCount = fields->GetFieldCount();

	UpdateModifiedFields(sf, layer);

	// create new fields, don't remove yet the stale ones, 
	// since it will break the indices to which the local fields are mapped
	CreateNewFields(layer, sf);

	int rowCount = 0, shapeCount = 0;

	CopyShapeData(sf, layer, shapeCmnIndex, saveType, validateShapes, errors, shapeCount, rowCount);

	// remove fields that were either removed or changed locally
	RemovedStaleFields(layer, sf, fieldCount);

	if (safeToDelete)
		shapeCount += RemoveDeletedFeatures(layer, sf, shapeCmnIndex);

	// the source indices of the fields will still be incorrect
	// if certain fields were removed, but at least this will help
	// to pass OgrLayer::HasChanges test
	ShapefileHelper::MarkFieldsAreUnmodified(sf);

	if (shapeCount > 0 || rowCount > 0) 
	{
		layer->SyncToDisk();
	}

	return shapeCount + rowCount;
}

// *************************************************************
//		UpdateModifiedFields()
// *************************************************************
void Shape2Ogr::UpdateModifiedFields(IShapefile* sf, OGRLayer* layer)
{
	long numFields = ShapefileHelper::GetNumFields(sf);

	CComPtr<ITable> table = NULL;
	sf->get_Table(&table);
	CTableClass* tableInternal = TableHelper::Cast(table);

	OGRFeatureDefn* fields = layer->GetLayerDefn();
	int fieldCount = fields->GetFieldCount();
	
	for (long i = 0; i < numFields; i++)
	{
		CComPtr<IField> fld = NULL;
		sf->get_Field(i, &fld);

		VARIANT_BOOL modified;
		fld->get_Modified(&modified);
		int sourceIndex = tableInternal->GetFieldSourceIndex(i);

		if (modified && sourceIndex != -1 && sourceIndex < fieldCount)
		{
			CComBSTR name;
			fld->get_Name(&name);

			FieldType fieldType;
			fld->get_Type(&fieldType);

			USES_CONVERSION;
			CStringA nameA = Utility::ConvertToUtf8(OLE2W(name));

			OGRFieldType ogrType = ShapeFieldType2OgrFieldType(fieldType);

			OGRFieldDefn* fd = new OGRFieldDefn(nameA, ogrType);

			OGRErr err = layer->AlterFieldDefn(sourceIndex, fd, ALTER_NAME_FLAG);

			if (err != OGRERR_NONE)
			{
				CallbackHelper::ErrorMsg("Failed to update OGR field name.");
			}
		}
	}
}

// *************************************************************
//		BuildFieldMap()
// *************************************************************
// Let's map indices of fields in the local table to those on the server
void Shape2Ogr::BuildFieldMap(IShapefile* sf, vector<int>& indices)
{
	CComPtr<ITable> table = NULL;
	sf->get_Table(&table);
	CTableClass* tableInternal = TableHelper::Cast(table);

	long fieldCount = ShapefileHelper::GetNumFields(sf);
	if (fieldCount > 0)
	{
		indices.resize(fieldCount);

		for (long i = 1; i < fieldCount; i++) 
		{
			indices[i] = tableInternal->GetFieldSourceIndex(i);
		}
	}
}

// *************************************************************
//		SaveShape()
// *************************************************************
void Shape2Ogr::CopyShapeData(IShapefile* sf, OGRLayer* layer, long shapeCmnIndex, tkOgrSaveType saveType,
	bool validateShapes, vector<OgrUpdateError>& errors, int& shapeCount, int& rowCount)
{
	long lastPercent = 0;
	long numShapes = ShapefileHelper::GetNumShapes(sf);

	CComPtr<ITable> table = NULL;
	sf->get_Table(&table);
	CTableClass* tableInternal = TableHelper::Cast(table);

	OGRFeatureDefn* fields = layer->GetLayerDefn();

	vector<int> fieldMap;
	BuildFieldMap(sf, fieldMap);

	for (long i = 0; i < numShapes; i++)
	{
		CallbackHelper::Progress(NULL, i, numShapes + 1, "Saving changes...", lastPercent);

		VARIANT_BOOL shapeModified, rowModified;
		sf->get_ShapeModified(i, &shapeModified);
		table->get_RowIsModified(i, &rowModified);

		if (!shapeModified && !rowModified) continue;

		if ((!shapeModified && saveType == ostGeometryOnly) ||
			(!rowModified && saveType == ostAttributesOnly)) {
			continue;
		}

		long featId;
		OGRFeature* ft = GetFeature(layer, sf, shapeCmnIndex, i, featId);

		if (shapeModified)
		{
			if (SaveShape(layer, ft, fields, sf, i, shapeCmnIndex, saveType, validateShapes, errors, fieldMap))
			{
				shapeCount++;
			}
		}
		else
		{
			// it's attributes that are modified
			if (!ft)
			{
				CStringW s;
				s.Format(L"Failed to find feature with id %d to save attributes.", featId);
				errors.push_back(OgrUpdateError(i, s));
				continue;
			}

			CopyRecordAttributes(sf, i, ft, true, fields, &fieldMap);

			OGRErr err = layer->SetFeature(ft);
			if (err == OGRERR_NONE)
			{
				rowCount++;
				tableInternal->MarkRowIsClean(i);
			}
			else
			{
				CStringW s = OgrHelper::OgrString2Unicode(CPLGetLastErrorMsg());
				errors.push_back(OgrUpdateError(i, s));
			}
		}

		if (ft) {
			OGRFeature::DestroyFeature(ft);
		}
	}

	CallbackHelper::ProgressCompleted();
}

// *************************************************************
//		SaveShape()
// *************************************************************
bool Shape2Ogr::SaveShape(OGRLayer* poLayer, OGRFeature* ft, OGRFeatureDefn* fields, IShapefile* shapefile,
	int shapeIndex, long shapeCmnIndex, tkOgrSaveType saveType, bool validateShapes, 
	vector<OgrUpdateError>& errors, vector<int>& fieldMap)
{
	OGRErr result;
	CString validationError;   // no need to store it in Unicode, it's almost certain uses ASCII only

	if (ft)
	{
		// update an existing feature
		if (ShapeRecord2Feature(shapefile, shapeIndex, ft, fields, saveType, true, validationError, validateShapes, &fieldMap))
		{
			result = poLayer->SetFeature(ft);
		}

		// we don't own the feature object, it should be destroyed by caller
	}
	else
	{
		// we assume that it's a new feature
		ft = OGRFeature::CreateFeature(fields);
		if (ShapeRecord2Feature(shapefile, shapeIndex, ft, fields, saveType, true, validationError, validateShapes, &fieldMap))
		{
			result = poLayer->CreateFeature(ft);

			if (result == OGRERR_NONE)
			{
				// let's write id generated by database to shapefile, so that the feature 
				// won't be later deleted as the one with id not present in the database
				VARIANT_BOOL vb;
				CComVariant var;
				var.lVal = static_cast<long>(ft->GetFID());
				var.vt = VT_I4;
				shapefile->EditCellValue(shapeCmnIndex, shapeIndex, var, &vb);
				VARIANT_BOOL hasOgrFidMapping;
				shapefile->get_HasOgrFidMapping(&hasOgrFidMapping);
				if (hasOgrFidMapping)
					((CShapefile*)shapefile)->MapOgrFid2ShapeIndex(var.lVal, shapeIndex);
			}

			// we've created feature object, let's destroy it
			OGRFeature::DestroyFeature(ft);
		}
	}

	bool validation = validationError.GetLength() > 0;
	if (result == OGRERR_NONE && !validation)
	{
		ShapefileHelper::MarkShapeRecordIsUnmodified(shapefile, shapeIndex);
		return true;
	}
	else
	{
		CStringW s = OgrHelper::OgrString2Unicode(CPLGetLastErrorMsg());
		USES_CONVERSION;
		errors.push_back(OgrUpdateError(shapeIndex, validation ? A2W(validationError) : s));
		return false;
	}
}

// *************************************************************
//		RemoveDeletedFeatures()
// *************************************************************
int Shape2Ogr::RemoveDeletedFeatures(OGRLayer* layer, IShapefile* sf, long shapeCmnIndex)
{
	int count = 0;
	if (shapeCmnIndex == -1) return count;

	std::multiset<long> fids;

	VARIANT_BOOL hasFidMap;
	sf->get_HasOgrFidMapping(&hasFidMap);
	if (hasFidMap)
	{
		auto fids = ((CShapefile*)sf)->GetDeletedShapeFIDs();
		for (auto fid : fids)
			layer->DeleteFeature(fid);
		((CShapefile*)sf)->ClearDeleteShapeFIDs();
	}
	else
	{
		// building list of ids by difference of what's inside the source and what's in memory
		// this only works if we load the entire dataset
		long numShapes;
		CComVariant var;
		sf->get_NumShapes(&numShapes);
		for (int i = 0; i < numShapes; i++)
		{
			sf->get_CellValue(shapeCmnIndex, i, &var);
			fids.insert(var.lVal);
		}

		OGRFeature* ft = NULL;
		layer->ResetReading();
		while ((ft = layer->GetNextFeature()) != NULL)
		{
			long fid = static_cast<long>(ft->GetFID());
			std::multiset<long>::iterator it = fids.find(fid);
			if (it == fids.end())
			{
				layer->DeleteFeature(fid);
				count++;
			}
			OGRFeature::DestroyFeature(ft);
		}
	}

	return count;
}
