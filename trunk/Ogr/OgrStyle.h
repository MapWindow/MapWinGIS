#pragma once

#include "ogrsf_frmts.h"

extern char* STYLES_TABLE_NAME;

class OgrStyleHelper
{
public:
	static bool OgrStyleHelper::HasStyleTable(GDALDataset* dataset, CStringW layerName);
	static bool SaveStyle(GDALDataset* dataset, IShapefile* shapefile, CStringW layerName, CStringW styleName);
	static bool SaveStyle(GDALDataset* dataset, CStringW xml, CStringW layerName, CStringW styleName);
	static CStringW GetStyleTableName(CStringW layerName = "");
	static CStringW GetDbSchemeName(CStringW layerName, bool withTrailingPoint);
	static int CreateStyleTable(GDALDataset* dataset, CStringW layerName);
	static bool SupportsStyles(GDALDataset* dataset, CStringW layerName);
	static CStringW LoadStyle(GDALDataset* dataset, CStringW styleTableName, CStringW layerName, CStringW styleName);
	static bool RemoveStyle(GDALDataset* dataset, CStringW styleTableName, CStringW layerName, CStringW styleName);
};

