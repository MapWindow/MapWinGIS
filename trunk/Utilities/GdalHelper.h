#pragma once
//#include "gdal.h"
#include "gdal_priv.h"
#include "gdal_frmts.h"

int CPL_STDCALL GDALProgressFunction( double dfComplete, const char* pszMessage, void *pData);

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
	static bool GdalHelper::CanOpenWithGdal(CStringW filename);
	static bool GdalHelper::SupportsOverviews(CStringW filename, ICallback* callback = NULL);

	static GdalSupport GdalHelper::TryOpenWithGdal(CStringW filename);

	static bool GdalHelper::NeedsOverviews(GDALDataset* dt);
	static bool GdalHelper::HasOverviews(GDALDataset* dt);
	static bool GdalHelper::HasOverviews(CStringW filename);
	static bool GdalHelper::RemoveOverviews(CStringW filename);
	static void GdalHelper::BuildOverviewsIfNeeded(GDALDataset* dt, ICallback* callback);
	static bool GdalHelper::BuildOverviewsIfNeeded(CStringW filename, bool external = true, ICallback* callback = NULL);
	static bool GdalHelper::BuildOverviewsCore(GDALDataset* dt, tkGDALResamplingMethod resamlingMethod, int* overviewList, int numOverviews, ICallback* callback);
};