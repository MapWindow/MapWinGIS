#pragma once
//#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_frmts.h"

class GdalHelper
{
public:
	GdalHelper(void) {};
	~GdalHelper(void) {};

	static GDALDataset* OpenDatasetA( char* filenameUtf8 );
	static GDALDataset* OpenDatasetA( CStringA& filenameUtf8 );
	static GDALDataset* OpenDatasetW( CStringW filenameW );
	static GDALDataset* OpenDatasetA(char* filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenDatasetA(CStringA& filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenDatasetW(CStringW filenameW, GDALAccess accessType );

	static bool GdalHelper::IsRgb(GDALDataset* dt);

	static CPLXMLNode* ParseXMLFile(CStringW filename);
	static int SerializeXMLTreeToFile(CPLXMLNode* psTree, CStringW filename);

	static void CloseDataset(GDALDataset* dt);
};