#include "stdafx.h"
#include "OgrHelper.h"
#include "ogrsf_frmts.h"
#include "OgrConverter.h"
#include "GeoProjection.h"
#include <set>
#include "Shapefile.h"
#include "Labels.h"

// *************************************************************
//		GetFieldValues()
// *************************************************************
void OgrHelper::GetFieldValues(OGRLayer* layer, int featureCount, OGRFieldType fieldType, vector<VARIANT*>& values, ICallback* cback)
{
	values.clear();
	
	int count = 0;
	long percent = 0;

	layer->ResetReading();
	CComBSTR key = L"";

	OGRFeature* ft = NULL;
	while ((ft = layer->GetNextFeature()) != NULL)
	{
		if (count % 50 == 0)
			CallbackHelper::Progress(cback, count, featureCount, "Reading field values...", key.m_str, percent);
		count++;

		VARIANT* var = new VARIANT();
		VariantInit(var);

		switch (fieldType)
		{
			case OFTReal:
				var->vt = VT_R8;
				var->dblVal = ft->GetFieldAsDouble(0);
				break;
			case OFTInteger:
				var->vt = VT_I4;
				var->dblVal = ft->GetFieldAsInteger(0);
				break;
			case OFTString:
			default:
				var->vt = VT_BSTR;
				CStringW ws = OgrHelper::OgrString2Unicode(ft->GetFieldAsString(0));
				var->bstrVal = W2BSTR(ws);
				break;
		}
		values.push_back(var);
		OGRFeature::DestroyFeature(ft);
	}

	CallbackHelper::ProgressCompleted(cback);
}

// *************************************************************
//		GetFieldType()
// *************************************************************
FieldType OgrHelper::GetFieldType(OGRFieldType ogrType)
{
	switch (ogrType)
	{
		case OFTReal: return DOUBLE_FIELD;
		case OFTInteger: return INTEGER_FIELD;
		case OFTString: 
		default:
			return STRING_FIELD;
	}
}

// *************************************************************
//		GetFieldType()
// *************************************************************
OGRFieldType OgrHelper::GetFieldType(IField* fld)
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

CStringA OgrHelper::String2OgrString(CStringW inputString)
{
	USES_CONVERSION;
	switch (m_globalSettings.ogrEncoding)
	{
		case oseUtf8:
			return Utility::ConvertToUtf8(inputString);
		case oseAnsi:
		default:
			return W2A(inputString);
	}
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

// *************************************************************
//		IsPostGisDatasource()
// *************************************************************
bool OgrHelper::IsPostGisDatasource(GDALDataset* ds)
{
	const char* driver = ds->GetDriverName();
	return _stricmp(driver, "PostgreSQL") == 0;
}

// *************************************************************
//		GetFieldList()
// *************************************************************
bool OgrHelper::GetFieldList(OGRLayer* layer, vector<CString>& fields)
{
	if (!layer) return false;
	fields.clear();

	OGRFeatureDefn *poFields = layer->GetLayerDefn();

	CString s = layer->GetFIDColumn();
	if (s.GetLength() > 0) {
		fields.push_back(s);
	}

	if (!poFields) return false;
	int count = poFields->GetFieldCount();
	for (int i = 0; i < count; i++) 
	{
		OGRFieldDefn* fld = poFields->GetFieldDefn(i);
		s = fld->GetNameRef();
		fields.push_back(s);
	}
	return true;
}

// *************************************************************
//		Cast()
// *************************************************************
COgrLayer* OgrHelper::Cast(CComPtr<IOgrLayer>& layer)
{
	return (COgrLayer*)&(*layer);
}

// *************************************************************
//		ShapeType2D()
// *************************************************************
ShpfileType OgrHelper::ShapeType2D(IOgrLayer* layer)
{
	if (!layer) return SHP_NULLSHAPE;
	ShpfileType shpType;
	layer->get_ShapeType2D(&shpType);
	return shpType;
}

// *************************************************************
//		ChooseLayerByShapeType()
// *************************************************************
IOgrLayer* OgrHelper::ChooseLayerByShapeType(IOgrDatasource* ds, ShpfileType shpType, VARIANT_BOOL forUpdate)
{
	int layerCount = GetLayerCount(ds);
	if (layerCount == 0) return NULL;

	for (int i = 0; i < layerCount; i++)
	{
		IOgrLayer* tempLayer = NULL;
		ds->GetLayer(i, forUpdate, &tempLayer);
		if (tempLayer)
		{
			if (OgrHelper::ShapeType2D(tempLayer) == shpType) {
				return tempLayer;
			}
			tempLayer->Close();
			tempLayer->Release();
		}
	}
	return NULL;
}

// *************************************************************
//		GetLayerCount()
// *************************************************************
int OgrHelper::GetLayerCount(IOgrDatasource* ds)
{
	if (!ds) return 0;
	int layerCount;
	ds->get_LayerCount(&layerCount);
	return layerCount;
}

// *************************************************************
//		CastDatasource()
// *************************************************************
static COgrDatasource* CastDatasource(CComPtr<IOgrDatasource>& ds)
{
	return (COgrDatasource*)&(*ds);
}