#pragma once

#include "ogrsf_frmts.h"

extern char* STYLES_TABLE_NAME;

class OgrStyleHelper
{
public:
	static bool OgrStyleHelper::HasStyleTable(GDALDataset* dataset, CStringW layerName);
	static bool SaveStyle(GDALDataset* dataset, IShapefile* shapefile, CStringW layerName, CStringW styleName);
	static CStringW GetStyleTableName(CStringW layerName = "");
	static CStringW GetDbSchemeName(CStringW layerName, bool withTrailingPoint);
	static int CreateStyleTable(GDALDataset* dataset, CStringW layerName);
	static bool SupportsStyles(GDALDataset* dataset, CStringW layerName);
};

