#pragma once
#include "gdal_frmts.h"

int CPL_STDCALL GDALProgressFunction( double dfComplete, const char* pszMessage, void *pData);

class GdalHelper
{
public:
	GdalHelper(void) {};
	~GdalHelper(void) {};

	static GDALDataset* OpenOgrDatasetW(CStringW filenameW, bool forUpdate);
	static GDALDataset* OpenOgrDatasetA(char* filenameUtf8, bool forUpdate);

	static GDALDataset* OpenDatasetA( char* filenameUtf8 );
	static GDALDataset* OpenDatasetA( CStringA& filenameUtf8 );
	static GDALDataset* OpenDatasetW( CStringW filenameW );
	static GDALDataset* OpenDatasetA(char* filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenDatasetA(CStringA& filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenDatasetW(CStringW filenameW, GDALAccess accessType );

	static bool IsRgb(GDALDataset* dt);

	static CPLXMLNode* ParseXMLFile(CStringW filename);
	static int SerializeXMLTreeToFile(CPLXMLNode* psTree, CStringW filename);

	static void CloseDataset(GDALDataset* dt);
	static bool CanOpenWithGdal(CStringW filename);
	static bool SupportsOverviews(CStringW filename, ICallback* callback = NULL);

	static GdalSupport TryOpenWithGdal(CStringW filename);

	static bool NeedsOverviews(GDALDataset* dt);
	static bool HasOverviews(GDALDataset* dt);
	static bool HasOverviews(CStringW filename);
	static bool RemoveOverviews(CStringW filename);
	static void BuildOverviewsIfNeeded(GDALDataset* dt, ICallback* callback);
	static bool BuildOverviewsIfNeeded(CStringW filename, bool external = true, ICallback* callback = NULL);
	static bool BuildOverviewsCore(GDALDataset* dt, tkGDALResamplingMethod resamlingMethod, int* overviewList, int numOverviews, ICallback* callback);

	static CStringA GetMetadataNameString(tkGdalDriverMetadata metadata);
	void GetMetaData(GDALDriver* driver);
	void DumpDriverInfo();
};