#pragma once
#include "gdal_frmts.h"

int CPL_STDCALL GDALProgressFunction( double dfComplete, const char* pszMessage, void *pData);

class GdalHelper
{
public:
	static GDALDataset* OpenOgrDatasetW(CStringW filenameW, bool forUpdate);
	static GDALDataset* OpenOgrDatasetA(char* filenameUtf8, bool forUpdate);
	static bool CanOpenAsOgrDataset(CStringW filename);

	static GDALDataset* OpenRasterDatasetA( char* filenameUtf8 );
	static GDALDataset* OpenRasterDatasetA( CStringA& filenameUtf8 );
	static GDALDataset* OpenRasterDatasetW( CStringW filenameW );
	static GDALDataset* OpenRasterDatasetA(char* filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenRasterDatasetA(CStringA& filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenRasterDatasetW(CStringW filenameW, GDALAccess accessType );

	static bool CopyDataset(GDALDataset* dataset, CStringW newName, ICallback* localCallback, bool createWorldFile);

	static void GetProjection(CStringW filename, CString& projection);

	static bool IsRgb(GDALDataset* dt);

	static CPLXMLNode* ParseXMLFile(CStringW filename);
	static int SerializeXMLTreeToFile(CPLXMLNode* psTree, CStringW filename);

	static void CloseDataset(GDALDataset* dt);
	static bool CanOpenAsGdalRaster(CStringW filename);
	static bool SupportsOverviews(CStringW filename, ICallback* callback = NULL);

	static GdalSupport TryOpenWithGdal(CStringW filename);

	static bool NeedsOverviews(GDALDataset* dt);
	static bool HasOverviews(GDALDataset* dt);
	static bool HasOverviews(CStringW filename);
	static bool RemoveOverviews(CStringW filename);
	static bool BuildOverviewsIfNeeded(GDALDataset* dt, ICallback* callback);
	static bool BuildOverviewsIfNeeded(CStringW filename, bool external = true, ICallback* callback = NULL);
	static bool BuildOverviewsCore(GDALDataset* dt, tkGDALResamplingMethod resamlingMethod, int* overviewList, int numOverviews, ICallback* callback);
	static bool ClearOverviews(GDALDataset* dt, ICallback* cb = NULL);

	static CStringA GetMetadataNameString(tkGdalDriverMetadata metadata);
	static void DumpMetadata(GDALDriver* driver);
	static void DumpDriverInfo();

	static char** ReadFile(CStringW filename);
	static char** SetCompressionRasterOptions(GDALDataset* dataset, char** options);

	static CString GetConfigPathString(GdalPath option);
	static CStringW GetDefaultConfigPath(GdalPath option);
	static void SetConfigPath(GdalPath option, CStringW newPath);
	static CStringW GetConfigPath(GdalPath option);
	static void SetDefaultConfigPaths();
	static void SetDirectory(CStringW path);

	static CString GetDriverMetadata(GDALDataset* ds, tkGdalDriverMetadata metadata);
	static int get_DriverMetadataCount(GDALDataset* ds);
	static CString get_DriverMetadataItem(GDALDataset* ds, int metadataIndex);

	static CString TiffCompressionToString(tkTiffCompression compression);
	static tkTiffCompression ParseTiffCompression(CString option);

};