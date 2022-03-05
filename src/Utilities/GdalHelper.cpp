/**************************************************************************************
* File name: GdalHelper.cpp
*
* Project: MapWindow Open Source (MapWinGis ActiveX control)
* Description: Implementation of GdalHelper
*
**************************************************************************************
* The contents of this file are subject to the Mozilla Public License Version 1.1
* (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/
* See the License for the specific language governing rights and limitations
* under the License.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
**************************************************************************************
* Contributor(s):
* (Open source contributors should list themselves and their modifications here). */
// august 2019 PaulM - Clean up of code as suggested by CLang-tidy.

#include "StdAfx.h"
#include "GdalHelper.h"

#include <gsl/pointers>
#include <gsl/util>

#include "ogrsf_frmts.h"
#include "CallbackHelper.h"
#include "Windows.h"
#include "GdalDriverHelper.h"
#include "RasterBandHelper.h"

// ReSharper disable once CppInconsistentNaming
map<CStringA, GDALDataset*> GdalHelper::m_ogrDatasets;

// **************************************************************
//		OpenOgrDatasetA
// **************************************************************
GDALDataset* GdalHelper::OpenOgrDatasetA(const char* filenameUtf8, const bool forUpdate)
{
	m_globalSettings.SetGdalUtf8(true);

	GDALAllRegister();
	int method = GDAL_OF_VECTOR;

	const bool forceUpdate = m_globalSettings.ogrLayerForceUpdateMode;
	if (forUpdate || forceUpdate)
		method |= GDAL_OF_UPDATE;

	auto dt = static_cast<GDALDataset*>(GDALOpenEx(filenameUtf8, method, nullptr, nullptr, nullptr));

	if (!dt && forceUpdate)
	{
		// let's try again without the update flag
		method = GDAL_OF_VECTOR;
		dt = static_cast<GDALDataset*>(GDALOpenEx(filenameUtf8, method, nullptr, nullptr, nullptr));
	}

	m_globalSettings.SetGdalUtf8(false);
	return dt;
}

// **************************************************************
//		OpenOgrDatasetW
// **************************************************************
GDALDataset* GdalHelper::OpenOgrDatasetW(const CStringW& filenameW, const bool forUpdate, const bool allowShared)
{
	CStringA filenameA = Utility::ConvertToUtf8(filenameW);

	if (allowShared && m_globalSettings.ogrShareConnection)
	{
		CStringA key = filenameA;
		key += forUpdate ? "1" : "0";

		if (m_ogrDatasets.find(key) != m_ogrDatasets.end()) // TODO: Fix compile warning
		{
			// it is opened already, try to reuse
			const gsl::not_null<GDALDataset*> ds = m_ogrDatasets[key];

			ds->Reference();
			//Debug::WriteLine("Referencing shared datasource: %d", count);
			return ds;
		}

		GDALDataset* ds = OpenOgrDatasetA(filenameA.GetBuffer(), forUpdate);
		if (ds)
		{
			// let's cache it for further reuse of connection
			if (m_ogrDatasets.find(key) == m_ogrDatasets.end()) // TODO: Fix compile warning
			{
				m_ogrDatasets[key] = ds;
			}
		}

		return ds;
	}

	return OpenOgrDatasetA(filenameA.GetBuffer(), forUpdate);
}

// **************************************************************
//		CloseSharedOgrDataset
// **************************************************************
int GdalHelper::CloseSharedOgrDataset(GDALDataset* ds)
{
	if (m_globalSettings.ogrShareConnection)
	{
		const int count = ds->Dereference();
		if (count == 0)
		{
			//Debug::WriteLine("Shared datasource is closed.");
			RemoveCachedOgrDataset(ds);
			GDALClose(ds);
		}
		else
		{
			//Debug::WriteLine("Dereferencing shared datasource: %d", count);
		}

		return count;
	}

	GDALClose(ds);
	return 0;
}

// **************************************************************
//		RemoveCachedOgrDataset
// **************************************************************
void GdalHelper::RemoveCachedOgrDataset(GDALDataset* ds)
{
	auto it = m_ogrDatasets.begin();
	while (it != m_ogrDatasets.end())
	{
		if (it->second == ds)
		{
			m_ogrDatasets.erase(it->first);
			break;
		}

		++it;
	}
}

// **************************************************************
//		CanOpenAsOgrDataset
// **************************************************************
bool GdalHelper::CanOpenAsOgrDataset(const CStringW& filename)
{
	const bool forceUpdate = m_globalSettings.ogrLayerForceUpdateMode;
	m_globalSettings.ogrLayerForceUpdateMode = false;

	const auto dt = OpenOgrDatasetW(filename, false, true);

	m_globalSettings.ogrLayerForceUpdateMode = forceUpdate;

	const bool success = dt != nullptr;
	if (dt)
	{
		CloseDataset(dt);
	}
	return success;
}

// **************************************************************
//		OpenRasterDatasetA
// **************************************************************
GDALDataset* GdalHelper::OpenRasterDatasetA(const char* filenameUtf8)
{
	m_globalSettings.SetGdalUtf8(true);

	GDALAllRegister();

	// let's report the failure on the second attempt only
	m_globalSettings.suppressGdalErrors = true;

	GDALDataset* dt = nullptr;

	if (!m_globalSettings.forceReadOnlyModeForGdalRasters)
		dt = static_cast<GDALDataset*>(GDALOpen(filenameUtf8, GA_Update));

	m_globalSettings.suppressGdalErrors = false;

	if (dt == nullptr)
		dt = static_cast<GDALDataset*>(GDALOpen(filenameUtf8, GA_ReadOnly));

	m_globalSettings.SetGdalUtf8(false);

	return dt;
}

GDALDataset* GdalHelper::OpenRasterDatasetA(const char* filenameUtf8, const GDALAccess accessType)
{
	m_globalSettings.SetGdalUtf8(true);

	GDALAllRegister();
	const auto dt = static_cast<GDALDataset*>(GDALOpen(filenameUtf8, accessType));

	m_globalSettings.SetGdalUtf8(false);

	return dt;
}

GDALDataset* GdalHelper::OpenRasterDatasetA(CStringA& filenameUtf8)
{
	return OpenRasterDatasetA(filenameUtf8.GetBuffer());
}

GDALDataset* GdalHelper::OpenRasterDatasetA(CStringA& filenameUtf8, const GDALAccess accessType)
{
	return OpenRasterDatasetA(filenameUtf8.GetBuffer(), accessType);
}

// **************************************************************
//		OpenRasterDatasetW
// **************************************************************
GDALDataset* GdalHelper::OpenRasterDatasetW(const CStringW& filenameW, const GDALAccess accessType)
{
	CStringA filenameA = Utility::ConvertToUtf8(filenameW);
	return OpenRasterDatasetA(filenameA.GetBuffer(), accessType);
}

GDALDataset* GdalHelper::OpenRasterDatasetW(const CStringW& filenameW)
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
		bool g, b;
		bool r = g = b = false;
		const int numBands = dt->GetRasterCount();
		// PM, march 2022: Do we start at 1?
		for (int i = 1; i <= numBands; i++)
		{
			if (GDALRasterBand* band = dt->GetRasterBand(i))
			{
				const GDALColorInterp ci = band->GetColorInterpretation();
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
CPLXMLNode* GdalHelper::ParseXMLFile(const CStringW& filename)
{
	const CStringA nameA = Utility::ConvertToUtf8(filename);
	m_globalSettings.SetGdalUtf8(true);
	CPLXMLNode* node = CPLParseXMLFile(nameA);
	m_globalSettings.SetGdalUtf8(false);
	return node;
}

// **************************************************************
//		SerializeXMLTreeToFile
// **************************************************************
int GdalHelper::SerializeXMLTreeToFile(const CPLXMLNode* psTree, const CStringW& filename)
{
	const CStringA nameA = Utility::ConvertToUtf8(filename);
	m_globalSettings.SetGdalUtf8(true);
	const int val = CPLSerializeXMLTreeToFile(psTree, nameA);
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
		// ReSharper disable once CppTooWideScopeInitStatement
		const int count = dt->GetRefCount();
		if (count > 0)
		{
			Debug::WriteLine("References remain on closing dataset: %d", count);
		}

		GDALClose(dt);
	}
}

// **************************************************************
//		CanOpenAsGdalRaster
// **************************************************************
bool GdalHelper::CanOpenAsGdalRaster(const CStringW& filename)
{
	GDALDataset* dt = OpenRasterDatasetW(filename, GA_ReadOnly);
	const bool gdalFormat = dt != nullptr;
	if (dt)
	{
		CloseDataset(dt);
	}
	return gdalFormat;
}

// **************************************************************
//		ClearOverviews
// **************************************************************
bool GdalHelper::ClearOverviews(GDALDataset* dt, ICallback* cb)
{
	CallbackParams params(cb, "Clearing overviews");

	m_globalSettings.SetGdalUtf8(true);

	const bool success = dt->BuildOverviews("NONE", 0, nullptr, 0, nullptr, GDALProgressCallback, &params) == CE_None;

	m_globalSettings.SetGdalUtf8(false);

	return success;
}

// **************************************************************
//		SupportsOverviews
// **************************************************************
bool GdalHelper::SupportsOverviews(const CStringW& filename, ICallback* callback)
{
	GDALAllRegister();
	bool supports = false;
	if (GDALDataset* dt = OpenRasterDatasetW(filename, GA_ReadOnly))
	{
		supports = HasOverviews(dt);
		if (!supports)
		{
			// TODO: can we check just by running clear?
			supports = ClearOverviews(dt, callback);
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
GdalSupport GdalHelper::TryOpenWithGdal(const CStringW& filename)
{
	GDALDataset* dt = OpenRasterDatasetW(filename, GA_ReadOnly);
	if (!dt)
	{
		return GdalSupportNone;
	}
	const bool isRgb = IsRgb(dt);
	CloseDataset(dt);
	return isRgb ? GdalSupportRgb : GdalSupportGrid;
}

// *******************************************************
//		HasOverviews()
// *******************************************************
bool GdalHelper::HasOverviews(GDALDataset* dt)
{
	if (dt)
	{
		if (GDALRasterBand* band = dt->GetRasterBand(1))
		{
			const int count = RasterBandHelper::GetOverviewCount(band);
			return count > 0;
		}
	}
	return false;
}

// *******************************************************
//		RemoveOverviews()
// *******************************************************
bool GdalHelper::RemoveOverviews(const CStringW& filename)
{
	if (GDALDataset* dt = OpenRasterDatasetW(filename, GA_ReadOnly))
	{
		const bool result = ClearOverviews(dt, nullptr);
		CloseDataset(dt);
		return result;
	}
	return false;
}

// *******************************************************
//		HasOverviews()
// *******************************************************
bool GdalHelper::HasOverviews(const CStringW& filename)
{
	if (GDALDataset* dt = OpenRasterDatasetW(filename, GA_ReadOnly))
	{
		const bool hasOverviews = HasOverviews(dt);
		CloseDataset(dt);
		return hasOverviews;
	}
	return false;
}

// *******************************************************
//		NeedsOverviews()
// *******************************************************
bool GdalHelper::NeedsOverviews(GDALDataset* dt)
{
	if (dt)
	{
		const int w = dt->GetRasterXSize();
		const int h = dt->GetRasterYSize();
		return w > m_globalSettings.minOverviewWidth || h > m_globalSettings.minOverviewWidth;
	}
	return false;
}

// *******************************************************
//		BuildOverviewsIfNeeded()
// *******************************************************
bool GdalHelper::BuildOverviewsIfNeeded(const CStringW& filename, const bool external, ICallback* callback)
{
	//if (!external)
	//CPLSetConfigOption( "COMPRESS_OVERVIEW", m_globalSettings.GetTiffCompression());

	// dataset must be opened in read-only mode, so that overviews are written in external ovr file
	const GDALAccess accessMode = external ? GA_ReadOnly : GA_Update;
	if (GDALDataset* dt = OpenRasterDatasetW(filename, accessMode))
	{
		const bool result = BuildOverviewsIfNeeded(dt, callback);
		CloseDataset(dt);
		return result;
	}
	return false;
}

// *******************************************************
//		BuildOverviewsIfNeeded()
// *******************************************************
bool GdalHelper::BuildOverviewsIfNeeded(GDALDataset* dt, ICallback* callback)
{
	bool result = false;
	if (m_globalSettings.rasterOverviewCreation == rocAuto ||
		m_globalSettings.rasterOverviewCreation == rocYes)
	{
		if (dt && !HasOverviews(dt) && NeedsOverviews(dt))
		{
			int w = dt->GetRasterXSize() / 2;
			int h = dt->GetRasterYSize() / 2;

			int ratio = 2;
			std::vector<int> overviews;
			while (w > m_globalSettings.minOverviewWidth || h > m_globalSettings.minOverviewWidth)
			{
				overviews.push_back(ratio);
				w /= 2;
				h /= 2;
				ratio *= 2;
			}

			result = BuildOverviewsCore(dt, m_globalSettings.rasterOverviewResampling, &overviews[0],
				gsl::narrow_cast<int>(overviews.size()), callback);
		}
	}
	return result;
}

// *******************************************************
//		BuildOverviewsCore()
// *******************************************************
bool GdalHelper::BuildOverviewsCore(GDALDataset* dt, const tkGDALResamplingMethod resamplingMethod, int* overviewList,
	const int numOverviews, ICallback* callback)
{
	// TODO: Similar function in GdalUtils::GdalBuildOverviews
	if (!dt) return false;

	if (numOverviews == 0)
	{
		CallbackHelper::ErrorMsg("No overviews list is passed to BuildOverviews routine.");
		return false;
	}

	const char* pszResampling = GetResamplingAlgorithm(resamplingMethod);

	CallbackParams params(callback, "Building overviews");

	m_globalSettings.SetGdalUtf8(true);

	const bool result = dt->BuildOverviews(pszResampling, numOverviews, overviewList, 0, nullptr, GDALProgressCallback,
		&params) == CE_None;

	m_globalSettings.SetGdalUtf8(false);

	CallbackHelper::ProgressCompleted(callback);

	return result;
}

// *******************************************************
//		GetResamplingAlgorithm()
// Also used by GdalUtils::GdalBuildOverviews
// *******************************************************
const char* GdalHelper::GetResamplingAlgorithm(const tkGDALResamplingMethod resamplingAlgorithm)
{
	switch (resamplingAlgorithm)
	{
	case grmAverage: return "AVERAGE";
	case grmAverageMagphase: return "AVERAGE_MAGPHASE";
	case grmBilinear: return "BILINEAR";
	case grmCubic: return "CUBIC";
	case grmCubicSpline: return "CUBICSPLINE";
	case grmGauss: return "GAUSS";
	case grmLanczos: return "LANCZOS";
	case grmMode: return "MODE";
	case grmNearest: return "NEAREST";
	case grmNone: return "NONE";
	default: return "NEAREST";
	}
}

// *******************************************************
//		GetMetadataNameString()
// *******************************************************
CStringA GdalHelper::GetMetadataNameString(const tkGdalDriverMetadata metadata)
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
	case dmdOPEN: return "DCAP_OPEN";
	case dmdCREATE: return "DCAP_CREATE";
	case dmdCREATECOPY: return "DCAP_CREATECOPY";
	case dmdVIRTUALIO: return "DCAP_VIRTUALIO";
	case dmdLAYER_CREATIONOPTIONLIST: return "DS_LAYER_CREATIONOPTIONLIST";
	case dmdOGR_DRIVER: return "OGR_DRIVER";
	case dmdRaster: return "DCAP_RASTER";
	case dmdVector: return "DCAP_VECTOR";
	case dmdNotNullFields: return "DCAP_NOTNULL_FIELDS";
	case dmdDefaultFields: return "DCAP_DEFAULT_FIELDS";
	case dmdNotNullGeometries: return "DCAP_NOTNULL_GEOMFIELDS";
	case dmCreationFieldDataTypes: return "DMD_CREATIONFIELDDATATYPES";
	default: return "";
	}

}

// *******************************************************
//		GetMetadataType()
// *******************************************************
tkGdalDriverMetadata GdalHelper::GetMetadataType(CStringA tag)
{
	tag = tag.MakeUpper();

	if (tag.Compare("DMD_LONGNAME") == 0) return dmdLONGNAME;
	if (tag.Compare("DMD_HELPTOPIC") == 0) return dmdHELPTOPIC;
	if (tag.Compare("DMD_MIMETYPE") == 0) return dmdMIMETYPE;
	if (tag.Compare("DMD_EXTENSION") == 0) return dmdEXTENSION;
	if (tag.Compare("DMD_EXTENSIONS") == 0) return dmdEXTENSIONS;
	if (tag.Compare("DMD_CREATIONOPTIONLIST") == 0) return dmdCREATIONOPTIONLIST;
	if (tag.Compare("DMD_OPENOPTIONLIST") == 0) return dmdOPENOPTIONLIST;
	if (tag.Compare("DMD_CREATIONDATATYPES") == 0) return dmdCREATIONDATATYPES;
	if (tag.Compare("DMD_SUBDATASETS") == 0) return dmdSUBDATASETS;
	if (tag.Compare("DCAP_OPEN") == 0) return dmdOPEN;
	if (tag.Compare("DCAP_CREATE") == 0) return dmdCREATE;
	if (tag.Compare("DCAP_CREATECOPY") == 0) return dmdCREATECOPY;
	if (tag.Compare("DCAP_VIRTUALIO") == 0) return dmdVIRTUALIO;
	if (tag.Compare("DS_LAYER_CREATIONOPTIONLIST") == 0) return dmdLAYER_CREATIONOPTIONLIST;
	if (tag.Compare("OGR_DRIVER") == 0) return dmdOGR_DRIVER;
	if (tag.Compare("DCAP_RASTER") == 0) return dmdRaster;
	if (tag.Compare("DCAP_VECTOR") == 0) return dmdVector;
	if (tag.Compare("DCAP_NOTNULL_FIELDS") == 0) return dmdNotNullFields;
	if (tag.Compare("DCAP_DEFAULT_FIELDS") == 0) return dmdDefaultFields;
	if (tag.Compare("DCAP_NOTNULL_GEOMFIELDS") == 0) return dmdNotNullGeometries;
	if (tag.Compare("DMD_CREATIONFIELDDATATYPES") == 0) return dmCreationFieldDataTypes;

	return dmdUnknown;
}

// ******************************************************
//		DumpDriverInfo()
// ******************************************************
void GdalHelper::DumpDriverInfo()
{
	GDALAllRegister();
	GDALDriverManager* manager = GetGDALDriverManager();
	GDALDriverManager::AutoLoadDrivers();
	const int count = manager->GetDriverCount();
	for (int i = 0; i < count; i++)
	{
		GDALDriver* driver = manager->GetDriver(i);
		const OGRSFDriver* ogrDriver = dynamic_cast<OGRSFDriver*>(driver);
		const CString description = driver->GetDescription();
		if (ogrDriver != nullptr)
		{
			Debug::WriteLine("OGR Driver: %s", description); // TODO: Fix compile warning
		}
		else
		{
			Debug::WriteLine("GDAL Driver: %s", description); // TODO: Fix compile warning
		}
		//GetMetaData(driver);
	}
}

// **************************************************
//		GetMetaData()
// **************************************************
void GdalHelper::DumpMetadata(GDALDriver* driver)
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
void GdalHelper::GetProjection(const CStringW& filename, CString& projection)
{
	GDALDataset* rasterDataset = OpenRasterDatasetW(filename);

	if (!rasterDataset) return;

	const char* wkt = rasterDataset->GetProjectionRef();
	projection = wkt;
	CloseDataset(rasterDataset);
}

// ****************************************************************
//		ReadFile
// ****************************************************************
char** GdalHelper::ReadFile(const CStringW& filename)
{
	const CStringA utf8Filename = Utility::ConvertToUtf8(filename);
	m_globalSettings.SetGdalUtf8(true);
	char** papszPrj = CSLLoad(utf8Filename);
	m_globalSettings.SetGdalUtf8(false);
	return papszPrj;
}

// ****************************************************************
//		SetCompressionRasterOptions
// ****************************************************************
char** GdalHelper::SetCompressionRasterOptions(GDALDataset* dataset, char** options)
{
	if (!dataset) return options;

	const CString driverName = dataset->GetDriverName();
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
bool GdalHelper::CopyDataset(GDALDataset* dataset, const CStringW& newName, ICallback* localCallback, const bool createWorldFile)
{
	if (!dataset) return false;

	GDALDriver* drv = dataset->GetDriver();
	if (!drv) return false;

	CallbackParams params(localCallback, "Copying dataset");

	char** papszOptions = nullptr;
	papszOptions = SetCompressionRasterOptions(dataset, papszOptions);

	if (createWorldFile)
		papszOptions = CSLSetNameValue(papszOptions, "WORLDFILE", "YES");

	m_globalSettings.SetGdalUtf8(true);
	const CStringA nameUtf8 = Utility::ConvertToUtf8(newName);
	GDALDataset* dst = drv->CreateCopy(nameUtf8, dataset, 0, papszOptions, GDALProgressCallback, &params);
	m_globalSettings.SetGdalUtf8(false);

	CSLDestroy(papszOptions);

	if (dst)
	{
		CloseDataset(dst);
		return true;
	}
	return false;
}

// ****************************************************************
//		GetDefaultConfigPath
// ****************************************************************
CStringW GdalHelper::GetDefaultConfigPath(const GdalPath option)
{
	auto path = Utility::GetFolderFromPath(Utility::GetMapWinGISPath());
	switch (option)
	{
	case PathGdalData:
		path += L"\\gdal-data\\";
		break;
	case PathGdalPlugins:
		path += L"\\gdalplugins\\";
		break;
	case PathProjLib:
		path += L"\\proj7\\share\\";
		break;
	}
	return path;
}

// ****************************************************************
//		GetConfigPathString
// ****************************************************************
CString GdalHelper::GetConfigPathString(const GdalPath option)
{
	switch (option)
	{
	case PathGdalData:
		return "GDAL_DATA";
	case PathGdalPlugins:
		return "GDAL_DRIVER_PATH";
	case PathProjLib:
		return "PROJ_LIB";
	}
	return "";
}

// ****************************************************************
//		SetConfigPath
// ****************************************************************
void GdalHelper::SetConfigPath(const GdalPath option, const CStringW& newPath)
{
	const CString optionName = GetConfigPathString(option);

	m_globalSettings.SetGdalUtf8(true);
	const CStringA pathA = Utility::ConvertToUtf8(newPath);

	CPLSetConfigOption(optionName, pathA);

	m_globalSettings.SetGdalUtf8(false);

	if (option == PathGdalPlugins)
	{
		SetDllDirectoryW(L"");
		const CStringW ocxPath = Utility::GetFolderFromPath(Utility::GetMapWinGISPath());
		SetDllDirectoryW(ocxPath);
	}

	if (!Utility::DirExists(newPath))
	{
		USES_CONVERSION;
		CallbackHelper::ErrorMsg(Debug::Format("The specified folder doesn't exist: %s", W2A(newPath))); // TODO: Fix compile warning
	}

	Debug::WriteLine("GDAL Config is set: %s = %s", CString(optionName), CString(newPath));
}

// ****************************************************************
//		GetConfigPath
// ****************************************************************
CStringW GdalHelper::GetConfigPath(const GdalPath option)
{
	const CString optionName = GetConfigPathString(option);

	m_globalSettings.SetGdalUtf8(true);
	const CStringA pathA = CPLGetConfigOption(optionName, "");
	m_globalSettings.SetGdalUtf8(false);

	return Utility::ConvertFromUtf8(pathA);
}

// ****************************************************************
//		SetDefaultConfigPaths
// ****************************************************************
void GdalHelper::SetDefaultConfigPaths()
{
	// ReSharper disable once CppTooWideScopeInitStatement
	constexpr GdalPath options[] = { PathGdalData, PathGdalPlugins, PathProjLib };

	for (auto& option : options)
	{
		const auto path = GetDefaultConfigPath(option);
		SetConfigPath(option, path);
	}
}

// ****************************************************************
//		GetDriverMetadata
// ****************************************************************
CString GdalHelper::GetDriverMetadata(GDALDataset* ds, const tkGdalDriverMetadata metadata)
{
	if (!ds) return "";
	return GdalDriverHelper::GetMetadata(ds->GetDriver(), metadata);
}

// ****************************************************************
//		get_DriverMetadataCount
// ****************************************************************
int GdalHelper::get_DriverMetadataCount(GDALDataset* ds)
{
	if (!ds) return 0;

	return GdalDriverHelper::get_MetadataCount(ds->GetDriver());
}

// *************************************************************
//		get_DriverMetadataItem()
// *************************************************************
CString GdalHelper::get_DriverMetadataItem(GDALDataset* ds, const int metadataIndex)
{
	if (!ds) return "";

	return GdalDriverHelper::get_MetadataItem(ds->GetDriver(), metadataIndex);
}

// *************************************************************
//		ParseTiffCompression()
// *************************************************************
tkTiffCompression GdalHelper::ParseTiffCompression(const CString& option)
{
	if (_stricmp(option, "AUTO") == 0)
		return tkmAUTO;

	if (_stricmp(option, "JPEG") == 0)
		return tkmJPEG;

	if (_stricmp(option, "LZW") == 0)
		return tkmLZW;

	if (_stricmp(option, "PACKBITS") == 0)
		return tkmPACKBITS;

	if (_stricmp(option, "DEFLATE") == 0)
		return tkmDEFLATE;

	if (_stricmp(option, "CCITTRLE") == 0)
		return tkmCCITTRLE;

	if (_stricmp(option, "CCITTFAX3") == 0)
		return tkmCCITTFAX3;

	if (_stricmp(option, "CCITTFAX4") == 0)
		return tkmCCITTFAX4;

	if (_stricmp(option, "NONE") == 0)
		return tkmNONE;

	if (_stricmp(option, "LZMA") == 0)
		return tkmLZMA;

	if (_stricmp(option, "ZSTD") == 0)
		return tkmZSTD;

	if (_stricmp(option, "LERC") == 0)
		return tkmLERC;

	if (_stricmp(option, "LERC_DEFLATE") == 0)
		return tkmLERC_DEFLATE;

	if (_stricmp(option, "LERC_ZSTD") == 0)
		return tkmLERC_ZSTD;

	if (_stricmp(option, "WEBP") == 0)
		return tkmWEBP;

	if (_stricmp(option, "JXL") == 0)
		return tkmJXL;

	return tkmAUTO;
}

// *************************************************************
//		TiffCompressionToString()
// *************************************************************
CString GdalHelper::TiffCompressionToString(const tkTiffCompression compression)
{
	// https://gdal.org/drivers/raster/gtiff.html#creation-options
	// JPEG/LZW/PACKBITS/DEFLATE/CCITTRLE/CCITTFAX3/CCITTFAX4/LZMA/ZSTD/LERC/LERC_DEFLATE/LERC_ZSTD/WEBP/JXL/NONE
	// In GDAL NONE is the default.
	switch (compression)
	{
	case tkmLZW:
		return "LZW";
	case tkmPACKBITS:
		return "PACKBITS";
	case tkmDEFLATE:
		return "DEFLATE";
	case tkmCCITTRLE:
		return "CCITTRLE";
	case tkmCCITTFAX3:
		return "CCITTFAX3";
	case tkmCCITTFAX4:
		return "CCITTFAX4";
	case tkmLZMA:
		return "LZMA";
	case tkmZSTD:
		return "ZSTD";
	case tkmLERC:
		return "LERC";
	case tkmLERC_DEFLATE:
		return "LERC_DEFLATE";
	case tkmLERC_ZSTD:
		return "LERC_ZSTD";
	case tkmWEBP:
		return "WEBP";
	case tkmJXL:
		return "JXL";
	case tkmNONE:
		return "NONE";
	case tkmAUTO:
	case tkmJPEG:
		return "JPEG";
	}
	return "NONE";
}
