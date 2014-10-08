#include "stdafx.h"
#include "OgrHelper.h"
#include "ogrsf_frmts.h"
#include "GeometryConverter.h"
#include "GeoProjection.h"
#include <set>
#include "Shapefile.h"

// *************************************************************
//		GetFieldType()
// *************************************************************
OGRFieldType GetFieldType(IField* fld)
{
	FieldType type;
	fld->get_Type(&type);
	switch (type)
	{
		case DOUBLE_FIELD: return OFTReal;
		case INTEGER_FIELD: return OFTInteger;
		case STRING_FIELD: return OFTString;
	}
	return OFTString;
}

// *************************************************************
//		GetLayerCapabilityString()
// *************************************************************
CString OgrHelper::GetLayerCapabilityString(tkOgrLayerCapability capability)
{
	switch (capability)
	{
	case olcRandomRead:          return "RandomRead";
	case olcSequentialWrite:     return  "SequentialWrite";
	case olcRandomWrite:         return  "RandomWrite";
	case olcFastSpatialFilter:   return  "FastSpatialFilter";
	case olcFastFeatureCount:    return  "FastFeatureCount";
	case olcFastGetExtent:       return  "FastGetExtent";
	case olcCreateField:         return  "CreateField";
	case olcDeleteField:         return  "DeleteField";
	case olcReorderFields:       return  "ReorderFields";
	case olcAlterFieldDefn:      return  "AlterFieldDefn";
	case olcTransactions:        return  "Transactions";
	case olcDeleteFeature:       return  "DeleteFeature";
	case olcFastSetNextByIndex:  return  "FastSetNextByIndex";
	case olcStringsAsUTF8:       return  "StringsAsUTF8";
	case olcIgnoreFields:        return  "IgnoreFields";
	case olcCreateGeomField:     return  "CreateGeomField";
	
	}
	return "";
}

// *************************************************************
//		GetDsCapabilityString()
// *************************************************************
CString OgrHelper::GetDsCapabilityString(tkOgrDSCapability capability)
{
	switch (capability)
	{
		case odcCreateLayer:        return  "CreateLayer";
		case odcDeleteLayer:        return  "DeleteLayer";
		case odcCreateGeomFieldAfterCreateLayer:   return  "CreateGeomFieldAfterCreateLayer";
		case odcCreateDataSource:   return  "CreateDataSource";
		case odcDeleteDataSource:   return "DeleteDataSource";
	}
	return "";
}

// *************************************************************
//		Layer2Shapefile()
// *************************************************************
IShapefile* OgrHelper::Layer2Shapefile(OGRLayer* layer, ICallback* callback /*= NULL*/)
{
	if (!layer)	return NULL;
	layer->ResetReading();

	IShapefile* sf;
	VARIANT_BOOL vbretval;

	OGRFeature *poFeature;
	
	ShpfileType shpType = GeometryConverter::GeometryType2ShapeType(layer->GetGeomType());
		
	// in case of queries or generic (untyped) geometry columns, type isn't defined
	// as quick fix let's fetch it from the first shape
	if (shpType == SHP_NULLSHAPE)
	{
 		OGRFeatureDefn* defn = layer->GetLayerDefn();
		while ((poFeature = layer->GetNextFeature()) != NULL)
		{
			// TODO: perhaps loop through all features and find out the most frequent type
			OGRGeometry* geom = poFeature->GetGeometryRef();
			if (geom)
			{
				shpType = GeometryConverter::GeometryType2ShapeType(geom->getGeometryType());
			}
			OGRFeature::DestroyFeature(poFeature);
			break;
		}
		layer->ResetReading();
	}

	CoCreateInstance(CLSID_Shapefile, NULL, CLSCTX_INPROC_SERVER, IID_IShapefile, (void**)&sf);
	sf->CreateNew(A2BSTR(""), shpType, &vbretval);

	// setting projection for shapefile
	OGRSpatialReference* sr = layer->GetSpatialRef();
	if (sr)
	{
		// only if srid is specified for geometry field
		IGeoProjection* gp = NULL;
		sf->get_GeoProjection(&gp);
		if (gp)
		{
			((CGeoProjection*)gp)->InjectSpatialReference(sr);
			gp->Release();
		}
	}

	/* ----------------------------------------------------------------- */
	/*		Converting of fields										 */
	/* ----------------------------------------------------------------- */
	IField * fld = NULL;

	// creating FID field to be able to write back to database
	bool hasFID = false;
	long fieldIndex = 0;
	CStringA name = layer->GetFIDColumn();
	if (name.GetLength() > 0)
	{
		GetUtils()->CreateInstance(idField, (IDispatch**)&fld);
		fld->put_Type(INTEGER_FIELD);
		fld->put_Name(A2BSTR(name));
		sf->EditInsertField(fld, &fieldIndex, NULL, &vbretval);
		fld->Release();
		hasFID = true;
	}

	OGRFeatureDefn *poFields = layer->GetLayerDefn();
	for (long iFld = 0; iFld < poFields->GetFieldCount(); iFld++)
	{
		GetUtils()->CreateInstance(idField, (IDispatch**)&fld);

		OGRFieldDefn* oField = poFields->GetFieldDefn(iFld);
		OGRFieldType type = oField->GetType();

		if (type == OFTInteger)	fld->put_Type(INTEGER_FIELD);
		else if (type == OFTReal)	fld->put_Type(DOUBLE_FIELD);
		else if (type == OFTString)	fld->put_Type(STRING_FIELD);

		fld->put_Name(A2BSTR(oField->GetNameRef()));
		fld->put_Width((long)oField->GetWidth());
		fld->put_Precision((long)oField->GetPrecision());

		fieldIndex++;
		sf->EditInsertField(fld, &fieldIndex, NULL, &vbretval);
		fld->Release();
	}

	/* ----------------------------------------------------------------- */
	/*		Converting of the shapes and cell values						 */
	/* ----------------------------------------------------------------- */
	int numFeatures = layer->GetFeatureCount();
	int count = 0;
	long percent = 0;
	USES_CONVERSION;
	BSTR key = A2BSTR("");
	
	while ((poFeature = layer->GetNextFeature()) != NULL)
	{
		Utility::DisplayProgress(callback, count, numFeatures, "Converting geometries...", key, percent);
		count++;
		OGRGeometry *oGeom;
		oGeom = poFeature->GetGeometryRef();
		
		IShape* shp = NULL;
		if (oGeom)
		{
			shp = GeometryConverter::GeometryToShape(oGeom, Utility::ShapeTypeIsM(shpType));
		}
		
		if (!shp)
		{
			// insert null shape so that client can still access it
			GetUtils()->CreateInstance(tkInterface::idShape, (IDispatch**)&shp);
		}

		long numShapes;
		sf->get_NumShapes(&numShapes);
		sf->EditInsertShape(shp, &numShapes, &vbretval);
		shp->Release();

		if (hasFID)
		{
			CComVariant var;
			var.vt = VT_I4;
			var.lVal = poFeature->GetFID();
			sf->EditCellValue(0, numShapes, var, &vbretval);
		}

		for (int iFld = 0; iFld < poFields->GetFieldCount(); iFld++)
		{
			OGRFieldDefn* oField = poFields->GetFieldDefn(iFld);
			OGRFieldType type = oField->GetType();
			
			CComVariant var;
			if (type == OFTInteger)
			{
				var.vt = VT_I4;
				var.lVal = poFeature->GetFieldAsInteger(iFld);
			}
			else if (type == OFTReal)
			{
				var.vt = VT_R8;
				var.dblVal = poFeature->GetFieldAsDouble(iFld);
			}
			else //if (type == OFTString )
			{
				var.vt = VT_BSTR;
				var.bstrVal = A2BSTR(poFeature->GetFieldAsString(iFld));
			}
			sf->EditCellValue(hasFID ? iFld + 1 : iFld, numShapes, var, &vbretval);
		}
		OGRFeature::DestroyFeature(poFeature);
	}
	Utility::DisplayProgressCompleted(callback);
	SysFreeString(key);
	
	sf->RefreshExtents(&vbretval);
	Utility::ClearShapefileModifiedFlag(sf);		// inserted shapes were marked as modified, correct this
	return sf;
}

// *************************************************************
//		Shapefile2OgrLayer()
// *************************************************************
bool OgrHelper::Shapefile2OgrLayer(IShapefile* sf, OGRLayer* poLayer, ICallback* callback)
{
	OGRFeatureDefn* fields = poLayer->GetLayerDefn();
	int fieldCount = fields->GetFieldCount();

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
		OGRFieldDefn oField(OgrHelper::Bstr2OgrString(name), GetFieldType(fld));
		oField.SetWidth(width);
		oField.SetPrecision(precision);
		SysFreeString(name);

		// insert it
		if (poLayer->CreateField(&oField) != OGRERR_NONE)
			return false;
	}

	USES_CONVERSION;
	CComVariant var;
	long numShapes, percent = 0;
	
	sf->get_NumShapes(&numShapes);
	BSTR key = A2BSTR("");
	CString validationError;

	for (long i = 0; i < numShapes; i++)
	{
		Utility::DisplayProgress(callback, i, numShapes, "Converting shapes...", key, percent);

		if (((CShapefile*)sf)->ShapeAvailable(i, VARIANT_FALSE))
		{
			OGRFeature* poFeature = OGRFeature::CreateFeature(fields);
			if (ShapeRecord2Feature(sf, i, poFeature, fields, ostSaveAll, false, validationError))
			{
				OGRErr result = poLayer->CreateFeature(poFeature);
			}
			OGRFeature::DestroyFeature(poFeature);
		}
	}
	Utility::DisplayProgressCompleted(callback);
	SysFreeString(key);

	return true;
}

// *************************************************************
//		ShapeRecord2Feature()
// *************************************************************
bool OgrHelper::ShapeRecord2Feature(IShapefile* sf, long shapeIndex, OGRFeature* feature, OGRFeatureDefn* fields, tkOgrSaveType saveType, bool editing, CString& validationError, bool validateShape)
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
int OgrHelper::SaveShapefileChanges(OGRLayer* poLayer, IShapefile* shapefile, long shapeCmnIndex, tkOgrSaveType saveType, 
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
int OgrHelper::RemoveDeletedFeatures(OGRLayer* layer, IShapefile* sf, long shapeCmnIndex)
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

// *************************************************************
//		Bstr2OgrString()
// *************************************************************
CStringA OgrHelper::Bstr2OgrString(BSTR& inputString, tkOgrEncoding encoding)
{
	USES_CONVERSION;
	switch (encoding)
	{
	case oseUtf8:
		return Utility::ConvertToUtf8(OLE2W(inputString));
	case oseAnsi:
	default:
		return OLE2A(inputString);
	}
}

CStringA OgrHelper::Bstr2OgrString(BSTR& inputString)
{
	return Bstr2OgrString(inputString, m_globalSettings.ogrEncoding);
}

// *************************************************************
//		OgrString2Unicode()
// *************************************************************
CStringW OgrHelper::OgrString2Unicode(const char* outputString, tkOgrEncoding encoding)
{
	switch (encoding)
	{
		case oseUtf8:
			return Utility::ConvertFromUtf8(outputString);
		case oseAnsi:
		default:
			USES_CONVERSION;
			return A2W(outputString);
	}
}

CStringW OgrHelper::OgrString2Unicode(const char* outputString)
{
	return OgrString2Unicode(outputString, m_globalSettings.ogrEncoding);
}
