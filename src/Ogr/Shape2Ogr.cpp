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
		// copy field
		long precision, width;
		CComBSTR name;
		CComPtr<IField> fld = NULL;
		sf->get_Field(i, &fld);
		fld->get_Name(&name);
		fld->get_Precision(&precision);
		fld->get_Width(&width);
		OGRFieldDefn oField(OgrHelper::Bstr2OgrString(name.m_str), OgrHelper::GetFieldType(fld));
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
	if (!OgrLabelsHelper::GetLabelFields(poLayer, labelFields))
		saveLabels = false;

	for (long i = 0; i < numShapes; i++)
	{
		CallbackHelper::Progress(callback, i, numShapes, "Converting shapes...", percent);

		if (((CShapefile*)sf)->ShapeAvailable(i, VARIANT_FALSE))
		{
			OGRFeature* poFeature = OGRFeature::CreateFeature(fields);
			if (ShapeRecord2Feature(sf, i, poFeature, fields, ostSaveAll, false, validationError))
			{
				if (saveLabels) {
					OgrLabelsHelper::AddLabel2Feature(labels, i, poFeature, labelFields);
				}
				OGRErr result = poLayer->CreateFeature(poFeature);
			}
			OGRFeature::DestroyFeature(poFeature);
		}
	}
	CallbackHelper::ProgressCompleted(callback);
}

// *************************************************************
//		CopyRecordAttributes()
// *************************************************************
void Shape2Ogr::CopyRecordAttributes(IShapefile* sf, long shapeIndex, OGRFeature* feature, bool editing, OGRFeatureDefn* fields)
{
	CComVariant var;
	double dval;
	long lval, numFields;
	sf->get_NumFields(&numFields);

	USES_CONVERSION;
	int startIndex = editing ? 1 : 0;
	for (int j = 0; j < numFields - startIndex; j++)
	{
		sf->get_CellValue(j + startIndex, shapeIndex, &var);
		if (var.vt != VT_EMPTY)
		{
			OGRFieldType type = fields->GetFieldDefn(j)->GetType();
			switch (type)
			{
				case OGRFieldType::OFTReal:
					dVal(var, dval);
					feature->SetField(j, dval);
					break;
				case OGRFieldType::OFTInteger:
					lVal(var, lval);
					feature->SetField(j, lval);
					break;
				case OGRFieldType::OFTString:
					feature->SetField(j, OLE2A(var.bstrVal));
					break;
			}
		}
	}
}

// *************************************************************
//		ShapeRecord2Feature()
// *************************************************************
bool Shape2Ogr::ShapeRecord2Feature(IShapefile* sf, long shapeIndex, OGRFeature* feature,
	OGRFeatureDefn* fields, tkOgrSaveType saveType, bool editing, CString& validationError, bool validateShape)
{
	if (!sf || !feature) return false;

	if (saveType != tkOgrSaveType::ostGeometryOnly)
	{
		CopyRecordAttributes(sf, shapeIndex, feature, editing, fields);
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
//		RecreateFieldsFromShapefile()
// *************************************************************
void Shape2Ogr::RecreateFieldsFromShapefile(OGRLayer* layer, OGRFeatureDefn* fields, IShapefile* sf)
{
	int fieldCount = fields->GetFieldCount();

	// geometry field is not included in this count
	for (int i = fieldCount - 1; i >= 0; i--)
	{
		OGRErr err = layer->DeleteField(i);
		if (err != OGRERR_NONE)
		{
			Debug::WriteError("Failed to remove field: %d", err);
		}
	}

	fieldCount = fields->GetFieldCount();
	Debug::WriteLine("Field count after: %d", fieldCount);

	long numFields;
	sf->get_NumFields(&numFields);

	// TODO: recreate only those fields that are actually changed and map the others
	// the first field is geometry one
	for (long i = 1; i < numFields; i++)
	{
		CComPtr<IField> field = NULL;
		sf->get_Field(i, &field);
	
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

	fieldCount = fields->GetFieldCount();
	Debug::WriteLine("Field count after: %d", fieldCount);
}

// *************************************************************
//		GetFeature()
// *************************************************************
OGRFeature* Shape2Ogr::GetFeature(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, long shapeIndex, long& featId)
{
	CComVariant var;
	shapefile->get_CellValue(shapeCmnIndex, shapeIndex, &var);

	lVal(var, featId);
	return poLayer->GetFeature(featId);
}

// *************************************************************
//		SaveShapefileChanges()
// *************************************************************
int Shape2Ogr::SaveShapefileChanges(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, tkOgrSaveType saveType,
	bool validateShapes, vector<OgrUpdateError>& errors)
{
	errors.clear();

	if (!shapefile || !poLayer || shapeCmnIndex == -1) return 0;

	// TODO: call only if there changes of fields
	//RecreateFieldsFromShapefile(poLayer, fields, shapefile);

	int rowCount = 0, shapeCount = 0; long lastPercent = 0;
	long numShapes = ShapefileHelper::GetNumShapes(shapefile);

	CComPtr<ITable> table = NULL;
	shapefile->get_Table(&table);
	CTableClass* tableInternal = TableHelper::Cast(table);
	OGRFeatureDefn* fields = poLayer->GetLayerDefn();
	
	for (long i = 0; i < numShapes; i++)
	{
		CallbackHelper::Progress(NULL, i, numShapes + 1, "Saving changes...", lastPercent);

		VARIANT_BOOL shapeModified, rowModified;
		shapefile->get_ShapeModified(i, &shapeModified);
		table->get_RowIsModified(i, &rowModified);
		
		if (!shapeModified && !rowModified) continue;

		if ((!shapeModified && saveType == ostGeometryOnly) || 
			(!rowModified && saveType == ostAttributesOnly)) {
			continue;
		}

		long featId;
		OGRFeature* ft = GetFeature(poLayer, shapefile, shapeCmnIndex, i, featId);

		if (shapeModified)
		{
			if (SaveShape(poLayer, ft, fields, shapefile, i, shapeCmnIndex, saveType, validateShapes, errors))
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

			CopyRecordAttributes(shapefile, i, ft, true, fields);
			
			OGRErr err = poLayer->SetFeature(ft);
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

	shapeCount += RemoveDeletedFeatures(poLayer, shapefile, shapeCmnIndex);

	if (shapeCount > 0 || rowCount > 0) 
	{
		poLayer->SyncToDisk();
	}

	return shapeCount + rowCount;
}

// *************************************************************
//		SaveShape()
// *************************************************************
bool Shape2Ogr::SaveShape(OGRLayer* poLayer, OGRFeature* ft, OGRFeatureDefn* fields, IShapefile* shapefile,
	int shapeIndex, long shapeCmnIndex, tkOgrSaveType saveType, bool validateShapes, vector<OgrUpdateError>& errors)
{
	OGRErr result;
	CString validationError;   // no need to store it in Unicode, it's almost certain uses ASCII only

	if (ft)
	{
		// update an existing feature
		if (ShapeRecord2Feature(shapefile, shapeIndex, ft, fields, saveType, true, validationError, validateShapes))
		{
			result = poLayer->SetFeature(ft);
		}

		// we don't own the feature object, it should be destroyed by caller
	}
	else
	{
		// we assume that it's a new feature
		ft = OGRFeature::CreateFeature(fields);
		if (ShapeRecord2Feature(shapefile, shapeIndex, ft, fields, saveType, true, validationError, validateShapes))
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
			}

			// we've created feature object, let's destroy it
			OGRFeature::DestroyFeature(ft);
		}
	}


	bool validation = validationError.GetLength() > 0;
	if (result == OGRERR_NONE && !validation)
	{
		shapefile->put_ShapeModified(shapeIndex, VARIANT_FALSE);
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
	long numShapes;
	CComVariant var;
	std::multiset<long> fids;

	// building list of ids
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
	return count;
}
