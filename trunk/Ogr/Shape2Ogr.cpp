#include "stdafx.h"
#include "Shape2Ogr.h"
#include "OgrLabels.h"
#include "GeometryConverter.h"
#include "OgrHelper.h"
#include "OgrLabels.h"
#include "Shapefile.h"

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
		BSTR name;
		CComPtr<IField> fld = NULL;
		sf->get_Field(i, &fld);
		fld->get_Name(&name);
		fld->get_Precision(&precision);
		fld->get_Width(&width);
		OGRFieldDefn oField(OgrHelper::Bstr2OgrString(name), OgrHelper::GetFieldType(fld));
		oField.SetWidth(width);
		oField.SetPrecision(precision);
		SysFreeString(name);

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
	BSTR key = A2BSTR("");
	CString validationError;

	CComPtr<ILabels> labels = NULL;
	sf->get_Labels(&labels);

	bool saveLabels = true;
	OgrLabelsHelper::LabelFields labelFields;
	if (!OgrLabelsHelper::GetLabelFields(poLayer, labelFields))
		saveLabels = false;

	for (long i = 0; i < numShapes; i++)
	{
		Utility::DisplayProgress(callback, i, numShapes, "Converting shapes...", key, percent);

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
	Utility::DisplayProgressCompleted(callback);
	SysFreeString(key);
}

// *************************************************************
//		ShapeRecord2Feature()
// *************************************************************
bool Shape2Ogr::ShapeRecord2Feature(IShapefile* sf, long shapeIndex, OGRFeature* feature,
	OGRFeatureDefn* fields, tkOgrSaveType saveType, bool editing, CString& validationError, bool validateShape)
{
	if (!sf || !feature) return false;

	CComVariant var;
	double dval;
	long lval, numFields;
	sf->get_NumFields(&numFields);

	if (saveType != tkOgrSaveType::ostGeometryOnly)
	{
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

			OGRGeometry* geom = GeometryConverter::ShapeToGeometry(shp, type);
			feature->SetGeometry(geom);
			OGRGeometryFactory::destroyGeometry(geom);
		}
	}
	return true;
}

// *************************************************************
//		SaveShapefileChanges()
// *************************************************************
int Shape2Ogr::SaveShapefileChanges(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, tkOgrSaveType saveType,
	bool validateShapes, vector<OgrUpdateError>& errors)
{
	errors.clear();

	if (!shapefile || !poLayer || shapeCmnIndex == -1) return 0;

	long numShapes, featId;
	shapefile->get_NumShapes(&numShapes);
	VARIANT_BOOL modified;
	int count = 0;

	OGRFeatureDefn* fields = poLayer->GetLayerDefn();

	for (long i = 0; i < numShapes; i++)
	{
		shapefile->get_ShapeModified(i, &modified);
		if (modified)
		{
			CComVariant var;
			shapefile->get_CellValue(shapeCmnIndex, i, &var);

			OGRErr result;
			CString validationError;   // no need to store it in Unicode, it's almost certain uses ASCII only

			lVal(var, featId);
			OGRFeature* ft = poLayer->GetFeature(featId);
			if (ft)
			{
				// update an existing feature
				if (ShapeRecord2Feature(shapefile, i, ft, fields, saveType, true, validationError, validateShapes))
				{
					result = poLayer->SetFeature(ft);
				}
			}
			else
			{
				// we assume that it's a new feature
				ft = OGRFeature::CreateFeature(fields);
				if (ShapeRecord2Feature(shapefile, i, ft, fields, saveType, true, validationError, validateShapes))
				{
					result = poLayer->CreateFeature(ft);

					if (result == OGRERR_NONE)
					{
						// let's write id generated by database to shapefile, so that the feature 
						// won't be later deleted as the one with id not present in the shapefile
						VARIANT_BOOL vb;
						CComVariant var;
						var.lVal = ft->GetFID();
						var.vt = VT_I4;
						shapefile->EditCellValue(shapeCmnIndex, i, var, &vb);
					}
				}
			}
			OGRFeature::DestroyFeature(ft);

			bool validation = validationError.GetLength() > 0;
			if (result == OGRERR_NONE && !validation)
			{
				count++;
				shapefile->put_ShapeModified(i, VARIANT_FALSE);
			}
			else
			{
				CStringW s = OgrHelper::OgrString2Unicode(CPLGetLastErrorMsg());
				USES_CONVERSION;
				errors.push_back(OgrUpdateError(i, validation ? A2W(validationError) : s));
			}
		}
	}

	count += RemoveDeletedFeatures(poLayer, shapefile, shapeCmnIndex);

	if (count > 0)
		poLayer->SyncToDisk();
	return count;
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
		long fid = ft->GetFID();
		std::multiset<long>::iterator it = fids.find(fid);
		if (it == fids.end())
		{
			Debug::WriteLine("About to delete feature: %d", fid);
			layer->DeleteFeature(fid);
			count++;
		}
		OGRFeature::DestroyFeature(ft);
	}
	return count;
}
