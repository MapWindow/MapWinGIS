#pragma once

int CPL_STDCALL GDALProgressFunction( double dfComplete, const char* pszMessage, void *pData);

class GdalHelper
{
public:
	static GDALDataset* OpenOgrDatasetW(const CStringW& filenameW, bool forUpdate, bool allowShared);
	static GDALDataset* OpenOgrDatasetA(const char* filenameUtf8, bool forUpdate);
	static bool CanOpenAsOgrDataset(const CStringW& filename);
	static int CloseSharedOgrDataset(GDALDataset* ds);

	static GDALDataset* OpenRasterDatasetA(const char* filenameUtf8 );
	static GDALDataset* OpenRasterDatasetA( CStringA& filenameUtf8 );
	static GDALDataset* OpenRasterDatasetW(const CStringW& filenameW );
	static GDALDataset* OpenRasterDatasetA(const char* filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenRasterDatasetA(CStringA& filenameUtf8, GDALAccess accessType );
	static GDALDataset* OpenRasterDatasetW(const CStringW& filenameW, GDALAccess accessType );

	static bool CopyDataset(GDALDataset* dataset, const CStringW& newName, ICallback* localCallback, bool createWorldFile);

	static void GetProjection(const CStringW& filename, CString& projection);

	static bool IsRgb(GDALDataset* dt);

	static CPLXMLNode* ParseXMLFile(const CStringW& filename);
	static int SerializeXMLTreeToFile(const CPLXMLNode* psTree, const CStringW& filename);

	static void CloseDataset(GDALDataset* dt);
	static bool CanOpenAsGdalRaster(const CStringW& filename);
	static bool SupportsOverviews(const CStringW& filename, ICallback* callback = nullptr);

	static GdalSupport TryOpenWithGdal(const CStringW& filename);

	static bool NeedsOverviews(GDALDataset* dt);
	static bool HasOverviews(GDALDataset* dt);
	static bool HasOverviews(const CStringW& filename);
	static bool RemoveOverviews(const CStringW& filename);
	static bool BuildOverviewsIfNeeded(GDALDataset* dt, ICallback* callback);
	static bool BuildOverviewsIfNeeded(const CStringW& filename, bool external = true, ICallback* callback = nullptr);
	static bool BuildOverviewsCore(GDALDataset* dt, tkGDALResamplingMethod resamplingMethod, int* overviewList, int numOverviews, ICallback* callback);
	static bool ClearOverviews(GDALDataset* dt, ICallback* cb = nullptr);

	static CStringA GetMetadataNameString(tkGdalDriverMetadata metadata);
	static void DumpMetadata(GDALDriver* driver);
	static void DumpDriverInfo();

	static char** ReadFile(const CStringW& filename);
	static char** SetCompressionRasterOptions(GDALDataset* dataset, char** options);

	static CString GetConfigPathString(GdalPath option);
	static CStringW GetDefaultConfigPath(GdalPath option);
	static void SetConfigPath(GdalPath option, const CStringW& newPath);
	static CStringW GetConfigPath(GdalPath option);
	static void SetDefaultConfigPaths();
	static void SetDirectory(CStringW path); // TODO: Fix compile warning

	// TODO: Rename to match other Metadata function??:
	static int get_DriverMetadataCount(GDALDataset* ds);
	static CString get_DriverMetadataItem(GDALDataset* ds, int metadataIndex);
	static CString GetDriverMetadata(GDALDataset* ds, tkGdalDriverMetadata metadata);
	static tkGdalDriverMetadata GetMetadataType(CStringA tag);

	static CString TiffCompressionToString(tkTiffCompression compression);
	static tkTiffCompression ParseTiffCompression(const CString& option);
	static const char* GetResamplingAlgorithm(tkGDALResamplingMethod resamplingAlgorithm);

private:
	static void RemoveCachedOgrDataset(GDALDataset* ds);
	// ReSharper disable once CppInconsistentNaming
	static map<CStringA, GDALDataset*> m_ogrDatasets;
};