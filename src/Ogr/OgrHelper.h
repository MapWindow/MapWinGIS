#pragma once
#include "ogr_feature.h"
#include "OgrLayer.h"
#include "OgrDatasource.h"

class OgrHelper
{
public:
	static CString GetLayerCapabilityString(tkOgrLayerCapability capability);
	static CString GetDsCapabilityString(tkOgrDSCapability capability);
	static CStringA Bstr2OgrString(BSTR& inputString);
	static CStringA String2OgrString(CStringW inputString);
	static CStringW OgrString2Unicode(const char* outputString);
	static bool IsPostGisDatasource(GDALDataset* ds);
	static OGRFieldType GetFieldType(IField* fld);
	static FieldType GetFieldType(OGRFieldType ogrType);
	static bool GetFieldList(OGRLayer* layer, vector<CString>& fields);
	static void GetFieldValues(OGRLayer* layer, int featureCount, OGRFieldType fieldType, vector<VARIANT*>& values, ICallback* cback);
	static COgrLayer* Cast(CComPtr<IOgrLayer>& layer);
	static ShpfileType ShapeType2D(IOgrLayer* layer);
	static tkOgrSourceType GetSourceType(IOgrLayer* layer);

	static COgrDatasource* CastDatasource(CComPtr<IOgrDatasource>& ds);
	static IOgrLayer* ChooseLayerByShapeType(IOgrDatasource* ds, ShpfileType shpType, VARIANT_BOOL forUpdate = VARIANT_FALSE);
	static int GetLayerCount(IOgrDatasource* ds);

private:	
	static CStringA Bstr2OgrString(BSTR& inputString, tkOgrEncoding encoding);
	static CStringW OgrString2Unicode(const char* outputString, tkOgrEncoding encoding);
	
};

