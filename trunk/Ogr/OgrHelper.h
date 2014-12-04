#pragma once
#include "ogr_feature.h"
#include "OgrLayer.h"

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
private:	
	static CStringA Bstr2OgrString(BSTR& inputString, tkOgrEncoding encoding);
	static CStringW OgrString2Unicode(const char* outputString, tkOgrEncoding encoding);
	
};

