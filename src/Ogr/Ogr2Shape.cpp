#include "stdafx.h"
#include "Ogr2Shape.h"
#include "OgrLabels.h"
#include "OgrConverter.h"
#include "GeoProjection.h"
#include "ShapefileHelper.h"
#include "TableHelper.h"
#include "Templates.h"

// *************************************************************
//		Layer2Shapefile()
// *************************************************************
IShapefile* Ogr2Shape::Layer2Shapefile(OGRLayer* layer, ShpfileType activeShapeType, int maxFeatureCount, bool& isTrimmed, 
	OgrDynamicLoader* loader, ICallback* callback /*= NULL*/)
{
	if (!layer)	return NULL;

	IShapefile* sf = CreateShapefile(layer, activeShapeType);

	if (sf) 
	{
		FillShapefile(layer, sf, maxFeatureCount, m_globalSettings.saveOgrLabels, callback, isTrimmed);
		
		// let's generate labels to unify API with dynamic mode
		if (loader->LabelExpression.GetLength() != 0)
		{
			CComPtr<ILabels> labels = NULL;
			sf->get_Labels(&labels);
			if (labels) 
			{
				labels->put_LineOrientation(loader->LabelOrientation);
				long count;
				ShpfileType type;
				sf->get_ShapefileType(&type);
				CComBSTR bstr(loader->LabelExpression);
				labels->Generate(bstr, loader->GetLabelPosition(type), VARIANT_TRUE, &count);
			}
		}
	}
	return sf;
}

// *************************************************************
//		CreateShapefile()
// *************************************************************
IShapefile* Ogr2Shape::CreateShapefile(OGRLayer* layer, ShpfileType activeShapeType)
{
	layer->ResetReading();

	IShapefile* sf;
	VARIANT_BOOL vbretval;

	OGRFeature *poFeature;

	ShpfileType shpType = OgrConverter::GeometryType2ShapeType(layer->GetGeomType());

	// in case it's not strongly typed, let's check if use specified the type
	if (shpType == SHP_NULLSHAPE)
	{
		shpType = activeShapeType;
	}

	// if not grab the type from the first feature
	if (shpType == SHP_NULLSHAPE)
	{
		OGRFeatureDefn* defn = layer->GetLayerDefn();
		while ((poFeature = layer->GetNextFeature()) != NULL)
		{
			OGRGeometry* geom = poFeature->GetGeometryRef();
			if (geom)
			{
				shpType = OgrConverter::GeometryType2ShapeType(geom->getGeometryType());
			}
			OGRFeature::DestroyFeature(poFeature);
			break;
		}
		layer->ResetReading();
	}

	ComHelper::CreateInstance(idShapefile, (IDispatch**)&sf);
	CComBSTR bstr = L"";
	sf->CreateNew(bstr, shpType, &vbretval);

	// setting projection for shapefile
	OGRSpatialReference* sr = layer->GetSpatialRef();
	if (sr)
	{
		// only if SRID is specified for geometry field
		IGeoProjection* gp = NULL;
		sf->get_GeoProjection(&gp);
		if (gp)
		{
			((CGeoProjection*)gp)->InjectSpatialReference(sr);
			gp->Release();
		}
	}
	
	CopyFields(layer, sf);

	long numFields = ShapefileHelper::GetNumFields(sf);

	if (numFields == 0)
		ShapefileHelper::InsertMwShapeIdField(sf);

	return sf;
}

// *************************************************************
//		CopyFields()
// *************************************************************
void Ogr2Shape::CopyFields(OGRLayer* layer, IShapefile* sf)
{
	IField * fld = NULL;
	VARIANT_BOOL vb;

	// creating FID field to be able to write back to database
	bool hasFID = false;
	long fieldIndex = 0;
	CStringA name = layer->GetFIDColumn();
	if (name.GetLength() > 0)
	{
		ComHelper::CreateInstance(idField, (IDispatch**)&fld);
		fld->put_Type(INTEGER_FIELD);
		CComBSTR bstrName(name);
		fld->put_Name(bstrName);
		sf->EditInsertField(fld, &fieldIndex, NULL, &vb);
		fld->Release();
		hasFID = true;
	}

	OGRFeatureDefn *poFields = layer->GetLayerDefn();

	for (long iFld = 0; iFld < poFields->GetFieldCount(); iFld++)
	{
		ComHelper::CreateInstance(idField, (IDispatch**)&fld);

		OGRFieldDefn* oField = poFields->GetFieldDefn(iFld);
		OGRFieldType type = oField->GetType();

		if (type == OFTInteger)	        fld->put_Type(INTEGER_FIELD);
		else if (type == OFTDate 
              || type == OFTDateTime)   fld->put_Type(DATE_FIELD);
		else if (type == OFTReal)   	fld->put_Type(DOUBLE_FIELD);
		else if (type == OFTString)	    fld->put_Type(STRING_FIELD);

		CComBSTR bstrName(oField->GetNameRef());
		fld->put_Name(bstrName);
		fld->put_Width((long)oField->GetWidth());
		fld->put_Precision((long)oField->GetPrecision());

		fieldIndex++;
		sf->EditInsertField(fld, &fieldIndex, NULL, &vb);

		fld->Release();
	}

	// we are saving the initial indices to know later on which fields has changed
	CComPtr<ITable> tbl = NULL;
	sf->get_Table(&tbl);
	CTableClass* tableInternal = TableHelper::Cast(tbl);

	long numFields = ShapefileHelper::GetNumFields(sf);
	for (long i = 1; i < numFields; i++) 
	{
		// the first one is FID
		tableInternal->SetFieldSourceIndex(i, i - 1);
	}
}
// *************************************************************
//		ExtendShapefile()
// *************************************************************
bool Ogr2Shape::ExtendShapefile(OGRLayer* layer, IShapefile* sf, bool loadLabels, ICallback* callback)
{
    layer->ResetReading();

    int numFeatures = static_cast<int>(layer->GetFeatureCount());

    int count = 0;
    long percent = 0;
    USES_CONVERSION;
    CComBSTR key = L"";

    OGRFeature *poFeature;
    VARIANT_BOOL vbretval;

    CStringA name = layer->GetFIDColumn();
    bool hasFID = name.GetLength() > 0;
    ShpfileType shpType;
    sf->get_ShapefileType(&shpType);

    map<long, long> fids;

    OGRFeatureDefn *poFields = layer->GetLayerDefn();

    CComPtr<ILabels> labels = NULL;
    sf->get_Labels(&labels);

    OgrLabelsHelper::LabelFields labelFields;
    if (loadLabels) {
        if (!OgrLabelsHelper::GetLabelFields(layer, labelFields))
            loadLabels = false;
    }

    ShpfileType targetType = ShapefileHelper::GetShapeType(sf);

    while ((poFeature = layer->GetNextFeature()) != NULL)
    {
        CallbackHelper::Progress(callback, count, numFeatures, "Converting geometries...", key.m_str, percent);
        count++;

        OGRGeometry *oGeom = poFeature->GetGeometryRef();

        IShape* shp = NULL;
        if (oGeom)
        {
            shpType = OgrConverter::GeometryType2ShapeType(oGeom->getGeometryType());
            if (shpType != targetType)
            {
                goto next_feature;
            }

            shp = OgrConverter::GeometryToShape(oGeom, ShapeUtility::IsM(shpType));
        }

        if (!shp)
        {
            // insert null shape so that client can still access it
            ComHelper::CreateShape(&shp);
        }

        // Get number of shapes already loaded
        long numShapes;
        sf->get_NumShapes(&numShapes);

        // Check if this OGR_FID is already in the featureset
        long replaceIndex = -1;
        long index = numShapes;
        if (hasFID) {
            CComVariant fid_var;
            fid_var.vt = VT_I4;
            fid_var.lVal = static_cast<long>(poFeature->GetFID());

            for (int i = 0; i < numShapes; i++) {
                VARIANT pVal;
                sf->get_CellValue(0, i, &pVal);
                long fid = 0;
                lVal(pVal, fid);
                if (pVal.lVal == fid_var.lVal) {
                    replaceIndex = i;
                    break;
                }
            }
        }

        // Update index in case we are replacing:
        index = replaceIndex > 0 ? replaceIndex : index;

        // Insert or replace the shape:
        if (replaceIndex > 0)
            sf->EditUpdateShape(index, shp, &vbretval);
        else
            sf->EditInsertShape(shp, &index, &vbretval);

        // No longer need this:
        shp->Release();

        // Set feature id
        if (hasFID) {
            CComVariant fid_var;
            fid_var.vt = VT_I4;
            fid_var.lVal = static_cast<long>(poFeature->GetFID());
            sf->EditCellValue(0, index, fid_var, &vbretval);
        }

        CopyValues(poFields, poFeature, sf, hasFID, index, loadLabels, labelFields);

    next_feature:
        OGRFeature::DestroyFeature(poFeature);
    }
    CallbackHelper::ProgressCompleted(callback);

    sf->RefreshExtents(&vbretval);
    ShapefileHelper::ClearShapefileModifiedFlag(sf);		// inserted shapes were marked as modified, correct this
    return true;
}

// *************************************************************
//		FillShapefile()
// *************************************************************
bool Ogr2Shape::FillShapefile(OGRLayer* layer, IShapefile* sf, int maxFeatureCount, bool loadLabels, ICallback* callback, bool& isTrimmed)
{
	if (!sf || !layer) return false;

	layer->ResetReading();

	int numFeatures = static_cast<int>(layer->GetFeatureCount());

	int count = 0;
	long percent = 0;
	USES_CONVERSION;
	CComBSTR key = L"";

	OGRFeature *poFeature;
	VARIANT_BOOL vbretval;

	CStringA name = layer->GetFIDColumn();
	bool hasFID = name.GetLength() > 0;
	ShpfileType shpType;
	sf->get_ShapefileType(&shpType);

	map<long, long> fids;

	OGRFeatureDefn *poFields = layer->GetLayerDefn();

	CComPtr<ILabels> labels = NULL;
	sf->get_Labels(&labels);

	OgrLabelsHelper::LabelFields labelFields;
	if (loadLabels) {
		if (!OgrLabelsHelper::GetLabelFields(layer, labelFields))
			loadLabels = false;
	}

	ShpfileType targetType = ShapefileHelper::GetShapeType(sf);

	while ((poFeature = layer->GetNextFeature()) != NULL)
	{
		CallbackHelper::Progress(callback, count, numFeatures, "Converting geometries...", key.m_str, percent);
		count++;

		if (count > maxFeatureCount) {
			OGRFeature::DestroyFeature(poFeature);
			isTrimmed = true;
			break;
		}

		OGRGeometry *oGeom = poFeature->GetGeometryRef();

		IShape* shp = NULL;
		if (oGeom)
		{
			shpType = OgrConverter::GeometryType2ShapeType(oGeom->getGeometryType());
			if (shpType != targetType)
			{
				goto next_feature;
			}

			shp = OgrConverter::GeometryToShape(oGeom, ShapeUtility::IsM(shpType));
		}

		if (!shp)
		{
			// insert null shape so that client can still access it
			ComHelper::CreateShape(&shp);
		}

		long numShapes;
		sf->get_NumShapes(&numShapes);
		sf->EditInsertShape(shp, &numShapes, &vbretval);
		shp->Release();

		if (hasFID)
		{
			CComVariant var;
			var.vt = VT_I4;
			var.lVal = static_cast<long>(poFeature->GetFID());
			sf->EditCellValue(0, numShapes, var, &vbretval);
		}

		CopyValues(poFields, poFeature, sf, hasFID, numShapes, loadLabels, labelFields);

next_feature:
		OGRFeature::DestroyFeature(poFeature);
	}
	CallbackHelper::ProgressCompleted(callback);

	sf->RefreshExtents(&vbretval);
	ShapefileHelper::ClearShapefileModifiedFlag(sf);		// inserted shapes were marked as modified, correct this
	return true;
}

// is the specified character one of the valid XBase Logical characters
bool isXBaseLogicalChar(wchar_t c)
{
	return (c == L'Y' || c == L'N' || c == L'T' || c == L'F' || c == L'?'); // || c == 'y' || c == 'n' || c == 't' || c == 'f');
}

// is the specified character one of the valid XBase Logical characters indicating TRUE
bool isXBaseLogicalTrue(wchar_t c)
{
	return (c == L'Y' || c == L'T'); // || c == 'y' || c == 't');
}

// *************************************************************
//		CopyValues()
// *************************************************************
void Ogr2Shape::CopyValues(OGRFeatureDefn* poFields, OGRFeature* poFeature, IShapefile* sf, bool hasFID, long numShapes, 
	bool loadLabels, OgrLabelsHelper::LabelFields labelFields)
{
	double x = 0.0, y = 0.0, rotation = 0;

	CStringW text;

	for (int iFld = 0; iFld < poFields->GetFieldCount(); iFld++)
	{
		OGRFieldDefn* oField = poFields->GetFieldDefn(iFld);
		OGRFieldType type = oField->GetType();

		CComVariant var;

		// https://mapwindow.atlassian.net/browse/MWGIS-57:		
		if (poFeature->IsFieldSetAndNotNull(iFld))
		{
			// TODO: Support date type
			if (type == OFTInteger)
			{
				var.vt = VT_I4;
				var.lVal = poFeature->GetFieldAsInteger(iFld);
			}
			else if (type == OFTInteger64)
			{
				var.vt = VT_I8;
				var.llVal = poFeature->GetFieldAsInteger64(iFld);
			}
			else if (type == OFTReal)
			{
				var.vt = VT_R8;
				var.dblVal = poFeature->GetFieldAsDouble(iFld);
			}
			else if (type == OFTDate || type == OFTDateTime)
			{
				int m, d, y, h, min, sec, flag;
				// should be able to read an an Integer
				int nFullDate = poFeature->GetFieldAsDateTime(iFld, &y, &m, &d, &h, &min, &sec, &flag);
				//y = (nFullDate / 10000);
				//m = ((nFullDate / 100) % 100);
				//d = (nFullDate % 100);
                // lop off time components
				COleDateTime dt(y, m, d, 0, 0, 0);
				var.vt = VT_DATE;
				var.date = dt.m_dt;
			}
			else if (type == OFTString)
			{
				// preview string
				// NOTE that it is presumed that ALL strings coming from OGR can be interpreted as UTF-8
				CStringW str = Utility::ConvertFromUtf8(poFeature->GetFieldAsString(iFld));
				// OGR does not currently support the Logical (boolean) field type.  It is possible that they will exist 
				// in the file, but OGR will interpret them as Strings.  Since we support boolean field types, we want 
				// to have a way of copying these particular string fields and interpreting them as booleans.  We will 
				// take the position that a single-character string field that contains any one of the valid XBase logical
				// characters is actually a Logical field.  This behavior can be turned off in the global settings.
				// NOTE: ESRI stores these values as Y and N, but the XBase spec indicates that it could also be a T or F,
				//       or lower case y, n, t, or f; so I have set up this test to accept all of the valid characters.
				// So, if the string is a single character, equal to one of the valid XBase Logical characters, 
				// AND our global settings are set to interpret a Yes/No character as a boolean, then do so.
				if (m_globalSettings.ogrInterpretYNStringAsBoolean && str.GetLength() == 1 && isXBaseLogicalChar(str.MakeUpper()[0]))
				{
					// interpret as a boolean
					var.vt = VT_BOOL;
					var.boolVal = isXBaseLogicalTrue(str.MakeUpper()[0]) ? VARIANT_TRUE : VARIANT_FALSE;
				}
				else
				{
					// else accept as a string
					var.vt = VT_BSTR;
					var.bstrVal = W2BSTR(str);		// BSTR will be cleared by CComVariant destructor
				}
			}
		}
		else
		{
			var.vt = VT_NULL;
		}

		VARIANT_BOOL vb;
		sf->EditCellValue(hasFID ? iFld + 1 : iFld, numShapes, var, &vb);
		
		if (loadLabels)
		{
			if (iFld == labelFields.X) x = var.dblVal;
			if (iFld == labelFields.Y) y = var.dblVal;
			if (iFld == labelFields.Text) text = OgrHelper::OgrString2Unicode(poFeature->GetFieldAsString(iFld));
			if (iFld == labelFields.Rotation) rotation = var.dblVal;
		}
	}

	if (loadLabels) 
	{
		CComBSTR bstr(text);

		CComPtr<ILabels> labels = NULL;
		sf->get_Labels(&labels);

		labels->AddLabel(bstr, x, y, rotation);
	}
}

// *************************************************************
//		ReadGeometryTypes()
// *************************************************************
void Ogr2Shape::ReadGeometryTypes(OGRLayer* layer, set<OGRwkbGeometryType>& types, bool readAll)
{
	types.clear();	

	layer->ResetReading();

	OGRFeature *poFeature;

	OGRFeatureDefn* defn = layer->GetLayerDefn();
	while ((poFeature = layer->GetNextFeature()) != NULL)
	{
		OGRGeometry* geom = poFeature->GetGeometryRef();
		if (geom)
		{
			OGRwkbGeometryType type = geom->getGeometryType();
			if (types.find(type) == types.end())
			{
				types.insert(type);
			}
		}
		OGRFeature::DestroyFeature(poFeature);

		if (!readAll && types.size() > 0) {
			break;
		}
	}

	layer->ResetReading();
}

// *************************************************************
//		GeometryTypesToShapeTypes()
// *************************************************************
void Ogr2Shape::GeometryTypesToShapeTypes(set<OGRwkbGeometryType>& types, vector<ShpfileType>& result)
{
	set<ShpfileType> shapeTypes;
	set<OGRwkbGeometryType>::iterator it = types.begin();
	while (it != types.end())
	{
		ShpfileType shpType = OgrConverter::GeometryType2ShapeType(*it);
		if (shapeTypes.find(shpType) == shapeTypes.end())
		{
			shapeTypes.insert(shpType);
		}
		it++;
	}

	Templates::SetToVector(shapeTypes, result);
}

