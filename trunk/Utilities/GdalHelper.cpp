#include "stdafx.h"
#include "GdalHelper.h"
#include "ogrsf_frmts.h"
#include "CallbackHelper.h"
#include "windows.h"

// **************************************************************
//		OpenOgrDatasetA
// **************************************************************
GDALDataset* GdalHelper::OpenOgrDatasetA(char* filenameUtf8, bool forUpdate)
{
	m_globalSettings.SetGdalUtf8(true);

	GDALAllRegister();
	int method = GDAL_OF_VECTOR;

	bool forceUpdate = m_globalSettings.ogrLayerForceUpdateMode;
	if (forUpdate || forceUpdate) 
		method |= GDAL_OF_UPDATE;
	
	GDALDataset* dt = (GDALDataset *)GDALOpenEx(filenameUtf8, method, NULL, NULL, NULL);
	
	if (!dt && forceUpdate)  {	// let's try again without the update flag
		method = GDAL_OF_VECTOR;
		dt = (GDALDataset *)GDALOpenEx(filenameUtf8, method, NULL, NULL, NULL);
	}
	
	m_globalSettings.SetGdalUtf8(false);
	return dt;
}

// **************************************************************
//		OpenOgrDatasetW
// **************************************************************
GDALDataset* GdalHelper::OpenOgrDatasetW(CStringW filenameW, bool forUpdate)
{
	CStringA filenameA = Utility::ConvertToUtf8(filenameW);
	return OpenOgrDatasetA(filenameA.GetBuffer(), forUpdate);
}

// **************************************************************
//		CanOpenAsOgrDataset
// **************************************************************
bool GdalHelper::CanOpenAsOgrDataset(CStringW filename)
{
	bool forceUpdate = m_globalSettings.ogrLayerForceUpdateMode;
	m_globalSettings.ogrLayerForceUpdateMode = false;

	GDALDataset* dt = GdalHelper::OpenOgrDatasetW(filename, false);

	m_globalSettings.ogrLayerForceUpdateMode = forceUpdate;

	bool success = dt != NULL;
	if (dt)
	{
		GdalHelper::CloseDataset(dt);
	}
	return success;
}

// **************************************************************
//		OpenRasterDatasetA
// **************************************************************
GDALDataset* GdalHelper::OpenRasterDatasetA( char* filenameUtf8 )
{
	m_globalSettings.SetGdalUtf8(true);

	GDALAllRegister();
	
	// let's report the failure on the second attempt only
	m_globalSettings.suppressGdalErrors = true;

	GDALDataset* dt = NULL;
	
	if (!m_globalSettings.forceReadOnlyModeForGdalRasters)
		dt = (GDALDataset *) GDALOpen(filenameUtf8, GA_Update );
	
	m_globalSettings.suppressGdalErrors = false;

	if (dt == NULL) 
		dt = (GDALDataset *) GDALOpen(filenameUtf8, GA_ReadOnly );

	m_globalSettings.SetGdalUtf8(false);

	return dt;
}

GDALDataset* GdalHelper::OpenRasterDatasetA(char* filenameUtf8, GDALAccess accessType)
{
	m_globalSettings.SetGdalUtf8(true);

	GDALAllRegister();
	GDALDataset* dt = (GDALDataset *)GDALOpen(filenameUtf8, accessType);

	m_globalSettings.SetGdalUtf8(false);

	return dt;
}

GDALDataset* GdalHelper::OpenRasterDatasetA( CStringA& filenameUtf8 )
{
	return OpenRasterDatasetA(filenameUtf8.GetBuffer());
}

GDALDataset* GdalHelper::OpenRasterDatasetA(CStringA& filenameUtf8, GDALAccess accessType )
{
	return OpenRasterDatasetA(filenameUtf8.GetBuffer(), accessType);
}

// **************************************************************
//		OpenRasterDatasetW
// **************************************************************
GDALDataset* GdalHelper::OpenRasterDatasetW(CStringW filenameW, GDALAccess accessType )
{
	CStringA filenameA = Utility::ConvertToUtf8(filenameW);
	return OpenRasterDatasetA(filenameA.GetBuffer(), accessType);
}

GDALDataset* GdalHelper::OpenRasterDatasetW(CStringW filenameW)
{
	CStringA filenameA = Utility::ConvertToUtf8(filenameW);
	return OpenRasterDatasetA(filenameA.GetBuffer());
}

// **************************************************************
//		CPLParseXMLFile
// **************************************************************
bool GdalHelper::IsRgb(GDALDataset* dt)
{
	bool isRgb = false;
	if (dt)
	{
		bool r, g, b;
		r = g = b = false;
		int numBands = dt->GetRasterCount();
		for(int i = 1; i <= numBands; i++)
		{
			GDALRasterBand* band = dt->GetRasterBand(i);
			if (band)
			{
				GDALColorInterp ci = band->GetColorInterpretation();
				if (ci == GCI_RedBand) r = true;
				if (ci == GCI_GreenBand) g = true;
				if (ci == GCI_BlueBand) b = true;
			}
		}
		isRgb = r && g && b;
	}
	return isRgb;
}

// **************************************************************
//		CPLParseXMLFile
// **************************************************************
CPLXMLNode* GdalHelper::ParseXMLFile(CStringW filename)
{
	CStringA nameA = Utility::ConvertToUtf8(filename);
	m_globalSettings.SetGdalUtf8(true);
	CPLXMLNode* node = CPLParseXMLFile(nameA);
	m_globalSettings.SetGdalUtf8(false);
	return node;
}

// **************************************************************
//		SerializeXMLTreeToFile
// **************************************************************
int GdalHelper::SerializeXMLTreeToFile(CPLXMLNode* psTree, CStringW filename)
{
	CStringA nameA = Utility::ConvertToUtf8(filename);	
	m_globalSettings.SetGdalUtf8(true);
	int val = CPLSerializeXMLTreeToFile(psTree, nameA);
	m_globalSettings.SetGdalUtf8(false);
	return val;
}

// **************************************************************
//		CloseDataset
// **************************************************************
void GdalHelper::CloseDataset(GDALDataset* dt)
{
	if (dt)
	{
		int count = dt->Dereference();
		dt->Reference();
		if (count > 0)
			Debug::WriteLine("References remain on closing dataset: %d", count);
		GDALClose(dt);
	}
}

// **************************************************************
//		CanOpenAsGdalRaster
// **************************************************************
bool GdalHelper::CanOpenAsGdalRaster(CStringW filename)
{
	GDALDataset* dt = GdalHelper::OpenRasterDatasetW(filename, GDALAccess::GA_ReadOnly);
	bool gdalFormat = dt != NULL;
	if (dt)
	{
		GdalHelper::CloseDataset(dt);
	}
	return gdalFormat;
}

// **************************************************************
//		ClearOverviews
// **************************************************************
bool ClearOverviews(GDALDataset* dt, ICallback* cb, bool clear)
{
	CallbackParams params;
	CallbackHelper::FillGdalCallbackParams(params, cb, "Clearing overviews");

	int overviewList = clear ? 0 : 2;
	bool success = dt->BuildOverviews("NONE", (clear ? 0 : 1), &overviewList, 0, NULL, 
			(GDALProgressFunc)GDALProgressCallback, &params) == CPLErr::CE_None;
	return success;
}

// **************************************************************
//		SupportsOverviews
// **************************************************************
bool GdalHelper::SupportsOverviews(CStringW filename, ICallback* callback)
{
	GDALAllRegister();
	bool supports = false;
	GDALDataset* dt = OpenRasterDatasetW(filename, GDALAccess::GA_ReadOnly);
	if (dt) {
		supports = HasOverviews(dt);
		if (!supports) {
			// TODO: can we check just by running clear?
			supports = ClearOverviews(dt, callback, true);
			// otherwise it's seems that the only way to check is try to create them
			// TODO: check driver to weed out formats which don't support overviews
		}
		CloseDataset(dt);
	}
	return supports;
}

// **************************************************************
//		TryOpenWithGdal
// **************************************************************
GdalSupport GdalHelper::TryOpenWithGdal(CStringW filename)
{
	GDALDataset* dt = GdalHelper::OpenRasterDatasetW(filename, GDALAccess::GA_ReadOnly);
	if (!dt) {
		return GdalSupport::GdalSupportNone;
	}
	else
	{
		bool isRgb = IsRgb(dt);
		GdalHelper::CloseDataset(dt);
		return isRgb ? GdalSupportRgb : GdalSupportGrid;
	}
}

// *******************************************************
//		HasOverviews()
// *******************************************************
bool GdalHelper::HasOverviews(GDALDataset* dt) 
{
	if (dt) {
		GDALRasterBand* band = dt->GetRasterBand(1);
		if (band)
		{
			int numOverviews = band->GetOverviewCount();
			return numOverviews > 0;
		}
	}
	return false;
}

// *******************************************************
//		RemoveOverviews()
// *******************************************************
bool GdalHelper::RemoveOverviews(CStringW filename) 
{
	GDALDataset* dt = GdalHelper::OpenRasterDatasetW(filename, GDALAccess::GA_ReadOnly);
	if (dt ) {
		bool result = ClearOverviews(dt, NULL, true);
		CloseDataset(dt);
		return result;
	}
	return false;
}

// *******************************************************
//		HasOverviews()
// *******************************************************
bool GdalHelper::HasOverviews(CStringW filename) 
{
	GDALDataset* dt = GdalHelper::OpenRasterDatasetW(filename, GA_ReadOnly);
	if (dt ) {
		bool hasOverviews = GdalHelper::HasOverviews(dt);
		GdalHelper::CloseDataset(dt);
		return hasOverviews;
	}
	return false;
}

// *******************************************************
//		NeedsOverviews()
// *******************************************************
bool GdalHelper::NeedsOverviews(GDALDataset* dt)
{
	if (dt) {
		int w = dt->GetRasterXSize();
		int h = dt->GetRasterYSize();
		return w > m_globalSettings.minOverviewWidth || h > m_globalSettings.minOverviewWidth;
	}
	return false;
}

// *******************************************************
//		BuildOverviewsIfNeeded()
// *******************************************************
bool GdalHelper::BuildOverviewsIfNeeded(CStringW filename, bool external, ICallback* callback) 
{
	//if (!external)
		//CPLSetConfigOption( "COMPRESS_OVERVIEW", m_globalSettings.GetTiffCompression());

	// dataset must be opened in read-only mode, so that overviews are written in external ovr file
	GDALAccess accessMode = external ? GDALAccess::GA_ReadOnly : GDALAccess::GA_Update;
	GDALDataset* dt = GdalHelper::OpenRasterDatasetW(filename, accessMode);
	if (dt) {
		GdalHelper::BuildOverviewsIfNeeded(dt, callback);
		GdalHelper::CloseDataset(dt);
		return true;
	}
	return false;
}

// *******************************************************
//		BuildOverviewsIfNeeded()
// *******************************************************
void GdalHelper::BuildOverviewsIfNeeded(GDALDataset* dt, ICallback* callback) 
{
	if (m_globalSettings.rasterOverviewCreation == rocAuto || 
		m_globalSettings.rasterOverviewCreation == rocYes)
	{
		if (dt && !GdalHelper::HasOverviews(dt) && NeedsOverviews(dt))
		{
			int w = dt->GetRasterXSize() / 2;
			int h = dt->GetRasterYSize() / 2;
			
			int ratio = 2;
			std::vector<int> overviews;
			while(w > m_globalSettings.minOverviewWidth || h > m_globalSettings.minOverviewWidth)
			{
				overviews.push_back(ratio);
				w /= 2;
				h /= 2;
				ratio *= 2;
			}

			bool result = BuildOverviewsCore(dt, m_globalSettings.rasterOverviewResampling, &(overviews[0]), overviews.size(), callback);
		}
	}
	return;
}

// *******************************************************
//		BuildOverviewsCore()
// *******************************************************
bool GdalHelper::BuildOverviewsCore(GDALDataset* dt, tkGDALResamplingMethod resamlingMethod, int* overviewList, int numOverviews, ICallback* callback) 
{
	if (!dt) return false;
	
	if (numOverviews == 0) {
		CallbackHelper::ErrorMsg("No overviews list is passed to BuildOverviews routine.");
		return false;
	}

	const char* pszResampling;
	switch(resamlingMethod)			//"MODE", "AVERAGE_MAGPHASE"
	{
		case grmAverage: 
			pszResampling = "AVERAGE"; break;
		case grmBicubic: 
			pszResampling = "CUBIC"; break;
		case grmGauss:	
			pszResampling = "GAUSS"; break;
		case grmNearest: 
			pszResampling = "NEAREST"; break;
		default: 
			pszResampling = "NONE";
	}

	CallbackParams params;
	CallbackHelper::FillGdalCallbackParams(params, callback, "Building overviews");

	bool result = dt->BuildOverviews(pszResampling, numOverviews, overviewList, 0, NULL, 
		(GDALProgressFunc)GDALProgressCallback, &params) == CE_None;
		
	CallbackHelper::ProgressCompleted(callback);

	return result;
}

// *******************************************************
//		GetMetadataNameString()
// *******************************************************
CStringA GdalHelper::GetMetadataNameString(tkGdalDriverMetadata metadata)
{
	switch (metadata)
	{
		case dmdLONGNAME: return "DMD_LONGNAME";
		case dmdHELPTOPIC: return "DMD_HELPTOPIC";
		case dmdMIMETYPE: return "DMD_MIMETYPE";
		case dmdEXTENSION: return "DMD_EXTENSION";
		case dmdEXTENSIONS: return "DMD_EXTENSIONS";
		case dmdCREATIONOPTIONLIST: return "DMD_CREATIONOPTIONLIST";
		case dmdOPENOPTIONLIST: return "DMD_OPENOPTIONLIST";
		case dmdCREATIONDATATYPES: return "DMD_CREATIONDATATYPES";
		case dmdSUBDATASETS: return "DMD_SUBDATASETS";
		case dmdOPEN: return       "DCAP_OPEN";
		case dmdCREATE: return     "DCAP_CREATE";
		case dmdCREATECOPY: return "DCAP_CREATECOPY";
		case dmdVIRTUALIO: return  "DCAP_VIRTUALIO";
		case dmdLAYER_CREATIONOPTIONLIST: return  "DS_LAYER_CREATIONOPTIONLIST";
		case dmdOGR_DRIVER: return  "OGR_DRIVER";
	}
	return "";
}

// ******************************************************
//		DumpDriverInfo()
// ******************************************************
void GdalHelper::DumpDriverInfo()
{
	GDALAllRegister();
	GDALDriverManager* manager = GetGDALDriverManager();
	manager->AutoLoadDrivers();
	int count = manager->GetDriverCount();
	for (int i = 0; i < count; i++)
	{
		GDALDriver* driver = manager->GetDriver(i);
		OGRSFDriver* ogrDriver = dynamic_cast<OGRSFDriver*>(driver);
		CString description = driver->GetDescription();
		if (ogrDriver != NULL)
		{
			Debug::WriteLine("OGR Driver: %s", description);
		}
		else
		{
			Debug::WriteLine("GDAL Driver: %s", description);
		}
		//GetMetaData(driver);
	}
}

// **************************************************
//		GetMetaData()
// **************************************************
void GdalHelper::GetMetaData(GDALDriver* driver)
{
	if (driver)
	{
		Debug::WriteLine("Dumping metadata domain list:");
		char** list = driver->GetMetadataDomainList();
		int count = CSLCount(list);
		for (int i = 0; i < count; i++)
		{
			Debug::WriteLine(CSLGetField(list, i));
		}

		Debug::WriteLine("Dumping metadata:");
		list = driver->GetMetadata();
		count = CSLCount(list);
		for (int i = 0; i < count; i++)
		{
			Debug::WriteLine(CSLGetField(list, i));
		}
	}
}

// ****************************************************************
//		GetProjection
// ****************************************************************
void GdalHelper::GetProjection(CStringW filename, CString& projection)
{
	GDALDataset * rasterDataset = GdalHelper::OpenRasterDatasetW(filename);

	if (!rasterDataset)	return;

	const char * wkt = rasterDataset->GetProjectionRef();
	projection = wkt;
	GdalHelper::CloseDataset(rasterDataset);
}

// ****************************************************************
//		ReadFile
// ****************************************************************
char** GdalHelper::ReadFile(CStringW filename)
{
	CStringA utf8filename = Utility::ConvertToUtf8(filename);
	m_globalSettings.SetGdalUtf8(true);
	char** papszPrj = CSLLoad(utf8filename);
	m_globalSettings.SetGdalUtf8(false);
	return papszPrj;
}

// ****************************************************************
//		SetCompressionRasterOptions
// ****************************************************************
char** GdalHelper::SetCompressionRasterOptions(GDALDataset* dataset, char** options)
{
	if (!dataset) return options;

	CString driverName = dataset->GetDriverName();
	if (_strcmpi(driverName, "GTiff") == 0)
	{
		options = CSLSetNameValue(options, "COMPRESS", m_globalSettings.GetTiffCompression());
	}
	if (_strcmpi(driverName, "HFA") == 0)
	{
		if (m_globalSettings.tiffCompression != tkmNONE)
			options = CSLSetNameValue(options, "COMPRESSED", "YES");
	}
	return options;
}

// ****************************************************************
//		CopyDataset
// ****************************************************************
bool GdalHelper::CopyDataset(GDALDataset* dataset, CStringW newName, ICallback* localCallback, bool createWorldFile)
{
	if (!dataset) return false;

	GDALDriver* drv = dataset->GetDriver();
	if (!drv) return false;

	CallbackParams params;
	CallbackHelper::FillGdalCallbackParams(params, localCallback, "Copying dataset");
	
	char **papszOptions = NULL;
	papszOptions = SetCompressionRasterOptions(dataset, papszOptions);

	if (createWorldFile)
		papszOptions = CSLSetNameValue(papszOptions, "WORLDFILE", "YES");

	m_globalSettings.SetGdalUtf8(true);
	CStringA nameUtf8 = Utility::ConvertToUtf8(newName);
	GDALDataset* dst = drv->CreateCopy(nameUtf8, dataset, 0, papszOptions, (GDALProgressFunc)GDALProgressCallback, &params);
	m_globalSettings.SetGdalUtf8(false);

	CSLDestroy(papszOptions);

	if (dst)
	{
		GdalHelper::CloseDataset(dst);
		return true;
	}
	return false;
}

// ****************************************************************
//		GetDefaultConfigPath
// ****************************************************************
CStringW GdalHelper::GetDefaultConfigPath(GdalPath option)
{
	CStringW path = Utility::GetFolderFromPath(Utility::GetMapWinGISPath());
	switch (option)
	{
		case PathGdalData:
			path += L"\\gdal-data\\";
			break;
		case PathGdalPlugins:
			path += L"\\gdal\\plugins\\";
			break;
	}
	return path;
}

// ****************************************************************
//		GetConfigPathString
// ****************************************************************
CString GdalHelper::GetConfigPathString(GdalPath option)
{
	switch (option)
	{
		case PathGdalData:
			return "GDAL_DATA";
		case PathGdalPlugins:
			return "GDAL_DRIVER_PATH";
	}
	return "";
}

// ****************************************************************
//		SetConfigPath
// ****************************************************************
void GdalHelper::SetConfigPath(GdalPath option, CStringW newPath)
{
	CString optionName = GetConfigPathString(option);

	m_globalSettings.SetGdalUtf8(true);
	CStringA pathA = Utility::ConvertToUtf8(newPath);

	CPLSetConfigOption(optionName, pathA);

	m_globalSettings.SetGdalUtf8(false);

	if (option == PathGdalPlugins) {
		SetDllDirectoryW(L"");
		CStringW ocxPath = Utility::GetFolderFromPath(Utility::GetMapWinGISPath());
		SetDllDirectoryW(ocxPath);
	}

	USES_CONVERSION;
	Debug::WriteLine("GDAL Config is set: %s = %s", optionName, W2A(newPath));
}

// ****************************************************************
//		GetConfigPath
// ****************************************************************
CStringW GdalHelper::GetConfigPath(GdalPath option)
{
	CString optionName = GetConfigPathString(option);

	m_globalSettings.SetGdalUtf8(true);
	CStringA pathA = CPLGetConfigOption(optionName, "");
	m_globalSettings.SetGdalUtf8(false);
	
	return Utility::ConvertFromUtf8(pathA);
}

// ****************************************************************
//		SetDefaultConfigPaths
// ****************************************************************
void GdalHelper::SetDefaultConfigPaths()
{
	GdalPath options[] = { PathGdalData, PathGdalPlugins };

	for (int i = 0; i < 2; i++)
	{
		CStringW path = GetDefaultConfigPath(options[i]);
		SetConfigPath(options[i], path);
	}
}