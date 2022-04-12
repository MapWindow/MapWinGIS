/**************************************************************************************
* File name: GdalUtils.cpp
*
* Project: MapWindow Open Source (MapWinGis ActiveX control)
* Description: Implementation of CGdalUtils
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
// june 2017 PaulM - Initial creation of this file
// november 2017 PaulM - Added GdalVectorTranslate
// june 2019 PaulM - Added GdalRasterTranslate, renamed GdalWarp to GdalRasterWarp due to IDL conflicts.
// august 2019 PaulM - Added GdalBuildOverviews and some improvement to the other methods.

#include "StdAfx.h"
#include "GdalUtils.h"
#include <atlsafe.h>

// *********************************************************************
//		~CGdalUtils
// *********************************************************************
CGdalUtils::~CGdalUtils()
{
	gReferenceCounter.Release(idGdalUtils);
}

// *********************************************************
//	     GdalRasterWarp()
// *********************************************************
STDMETHODIMP CGdalUtils::GdalRasterWarp(BSTR sourceFilename, BSTR destinationFilename, SAFEARRAY* options, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";
	CallbackParams params(_globalCallback, "Warping");

	//USES_CONVERSION;
	const CStringW srcFilename(sourceFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		ErrorMessage(tkINVALID_FILENAME);
		CallbackHelper::ErrorMsg(Debug::Format("Source file %s does not exists.", CString(srcFilename)));
		_detailedError = "Source file " + srcFilename + " does not exists.";
		return S_OK;
	}

	// Open file as GdalDataset:
	CallbackHelper::Progress(_globalCallback, 0, "Open source file as raster", _key);
	GDALDatasetH dt = GdalHelper::OpenRasterDatasetW(srcFilename, GA_ReadOnly);
	if (!dt)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Can't open %s as a raster file."), srcFilename));
		_detailedError = "Can't open " + srcFilename + " as a raster file.";
		ErrorMsg(tkINVALID_FILENAME);
		goto cleaning;
	}

	// Make options:		
	if (SafeArrayGetDim(options) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The warp options are invalid.")));
		_detailedError = "The option array doesn't have 1 dimension";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	// https://gdal.org/tutorials/warp_tut.html
	const auto warpOptions = ConvertSafeArrayToChar(options);
	GDALWarpAppOptions* const gdalWarpOptions = GDALWarpAppOptionsNew(warpOptions, nullptr);
	if (!gdalWarpOptions)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The warp options are invalid.")));
		_detailedError = "Can't convert the option array to GDALWarpAppOptions";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	// Call the gdalWarp function:		
	GDALWarpAppOptionsSetProgress(gdalWarpOptions, GDALProgressCallback, &params);
	m_globalSettings.SetGdalUtf8(true);
	const auto dtNew = GDALWarp(CString(destinationFilename), nullptr, 1, &dt, gdalWarpOptions, nullptr);
	m_globalSettings.SetGdalUtf8(false);
	if (dtNew)
	{
		*retVal = VARIANT_TRUE;
		GDALClose(dtNew);
	}
	else
	{
		CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, "Warping failed");
		ErrorMessage(tkGDAL_ERROR);
		// Put GDAL error in Detailed ErrorMsg:
		_detailedError = CPLGetLastErrorMsg();
	}

cleaning:
	// ------------------------------------------------------
	//	Cleaning
	// ------------------------------------------------------
	if (gdalWarpOptions)
		GDALWarpAppOptionsFree(gdalWarpOptions);

	if (warpOptions)
		CSLDestroy(warpOptions);

	if (dt)
		GDALClose(dt);

	CallbackHelper::ProgressCompleted(_globalCallback);

	return S_OK;
}

// *********************************************************
//	     GdalRasterTranslate()
// *********************************************************
STDMETHODIMP CGdalUtils::GdalRasterTranslate(BSTR sourceFilename, BSTR destinationFilename, SAFEARRAY* options, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";
	CallbackParams params(_globalCallback, "Translating");

	const CStringW srcFilename(sourceFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		ErrorMessage(tkINVALID_FILENAME);
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Source file %s does not exists."), srcFilename));
		_detailedError = "Source file " + srcFilename + " does not exists.";
		return S_OK;
	}

	// Open file as GdalDataset:
	CallbackHelper::Progress(_globalCallback, 0, "Open source file as raster", _key);
	const auto dt = GdalHelper::OpenRasterDatasetW(srcFilename, GA_ReadOnly);
	if (!dt)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Can't open %s as a raster file."), srcFilename));
		_detailedError = "Can't open " + srcFilename + " as a raster file.";
		ErrorMsg(tkINVALID_FILENAME);
		goto cleaning;
	}

	// Make options:	
	const auto translateOptions = ConvertSafeArrayToChar(options);
	const auto gdalTranslateOptions = GDALTranslateOptionsNew(translateOptions, nullptr);
	if (!gdalTranslateOptions)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The translate options are invalid.")));
		_detailedError = "Can't convert the option array to GDALTranslateOptions";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	// Call the gdalTranslate function:
	GDALTranslateOptionsSetProgress(gdalTranslateOptions, GDALProgressCallback, &params);
	m_globalSettings.SetGdalUtf8(true);
	const auto dtNew = GDALTranslate(CString(destinationFilename), dt, gdalTranslateOptions, nullptr);
	m_globalSettings.SetGdalUtf8(false);

	if (dtNew)
	{
		*retVal = VARIANT_TRUE;
		GDALClose(dtNew);
	}
	else
	{
		ErrorMessage(tkGDAL_ERROR);
		CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, "Raster translate failed");
		// Put GDAL error in Detailed ErrorMsg:
		_detailedError = CPLGetLastErrorMsg();
	}

cleaning:
	// ------------------------------------------------------
	//	Cleaning
	// ------------------------------------------------------
	if (gdalTranslateOptions)
		GDALTranslateOptionsFree(gdalTranslateOptions);

	if (translateOptions)
		CSLDestroy(translateOptions);

	if (dt)
		GDALClose(dt);

	CallbackHelper::ProgressCompleted(_globalCallback);

	return S_OK;
}


// *********************************************************
//	     GdalVectorReproject()
// *********************************************************
STDMETHODIMP CGdalUtils::GdalVectorReproject(BSTR sourceFilename, BSTR destinationFilename,
	const int sourceEpsgCode, const int destinationEpsgCode,
	const VARIANT_BOOL useSharedConnection, VARIANT_BOOL* retVal)
{
	CString sourceEpsgCodeBuilder;
	sourceEpsgCodeBuilder.Format("EPSG:%d", sourceEpsgCode);

	CString destEpsgCodeBuilder;
	destEpsgCodeBuilder.Format("EPSG:%d", destinationEpsgCode);

	CComSafeArray<BSTR> options(5);
	options[0] = "-s_srs";
	options[1] = sourceEpsgCodeBuilder;
	options[2] = "-t_srs";
	options[3] = destEpsgCodeBuilder;
	options[4] = "-overwrite";

	this->GdalVectorTranslate(sourceFilename, destinationFilename, options, useSharedConnection, retVal);

	return S_OK;
}

// *********************************************************
//	     GdalVectorTranslate()
// *********************************************************
STDMETHODIMP CGdalUtils::GdalVectorTranslate(BSTR sourceFilename, BSTR destinationFilename,
	SAFEARRAY* options, const VARIANT_BOOL useSharedConnection,
	VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		* retVal = VARIANT_FALSE;
	_detailedError = "No error";
	CallbackParams params(_globalCallback, "Vector translate");

	//USES_CONVERSION;
	const CStringW srcFilename(sourceFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Source file %s does not exists."), srcFilename));
		_detailedError = "Subject file " + srcFilename + " does not exists.";
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	// Open file as GdalDataset:
	CallbackHelper::Progress(_globalCallback, 0, "Open source file as vector", _key);

	GDALDatasetH dt = GdalHelper::OpenOgrDatasetW(srcFilename, GA_ReadOnly, useSharedConnection == VARIANT_TRUE);
	if (!dt)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Can't open %s as a vector file."), srcFilename));
		_detailedError = "Can't open " + srcFilename + " as a vector file.";
		ErrorMsg(tkINVALID_FILENAME);
		goto cleaning;
	}

	// Make options:		
	if (SafeArrayGetDim(options) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The vector translate options are invalid.")));
		_detailedError = "The option array doesn't have 1 dimension";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	const auto translateOptions = ConvertSafeArrayToChar(options);
	const auto gdalVectorTranslateOptions = GDALVectorTranslateOptionsNew(translateOptions, nullptr);
	if (!gdalVectorTranslateOptions)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The vector translate options are invalid.")));
		_detailedError = "Can't convert the option array to GDALVectorTranslateOptions";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	// Call the gdalWarp function:
	GDALVectorTranslateOptionsSetProgress(gdalVectorTranslateOptions, GDALProgressCallback, &params);
	m_globalSettings.SetGdalUtf8(true);
	USES_CONVERSION;
	const auto dtNew = GDALVectorTranslate(OLE2A(destinationFilename), nullptr, 1, &dt, gdalVectorTranslateOptions, nullptr);
	m_globalSettings.SetGdalUtf8(false);
	if (dtNew)
	{
		*retVal = VARIANT_TRUE;
		GDALClose(dtNew);
	}
	else
	{
		CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, "Vector translate failed");
		ErrorMessage(tkGDAL_ERROR);
		// Put GDAL error in Detailed ErrorMsg:
		_detailedError = CPLGetLastErrorMsg();
	}

cleaning:
	// ------------------------------------------------------
	//	Cleaning
	// ------------------------------------------------------
	if (gdalVectorTranslateOptions)
		GDALVectorTranslateOptionsFree(gdalVectorTranslateOptions);

	if (translateOptions)
		CSLDestroy(translateOptions);

	if (dt)
		GdalHelper::CloseSharedOgrDataset(static_cast<GDALDataset*>(dt));

	CallbackHelper::ProgressCompleted(_globalCallback);

	return S_OK;
}

// *********************************************************
//	     ClipVectorWithVector()
// *********************************************************
STDMETHODIMP CGdalUtils::ClipVectorWithVector(BSTR subjectFilename, BSTR overlayFilename, BSTR destinationFilename, const VARIANT_BOOL useSharedConnection, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";

	//USES_CONVERSION;
	const CStringW inputSubjectFilename(subjectFilename);
	if (!Utility::FileExistsW(inputSubjectFilename))
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Subject file %s does not exists."), inputSubjectFilename));
		_detailedError = "Subject file " + inputSubjectFilename + " does not exists.";
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	const CStringW inputOverlayFilename = OLE2W(overlayFilename);
	if (!Utility::FileExistsW(inputOverlayFilename))
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Overlay file %s does not exists."), inputOverlayFilename));
		_detailedError = "Overlay file " + inputOverlayFilename + " does not exists.";
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	// Call VectorTranslate:
	CComSafeArray<BSTR> translateOptions(10);
	translateOptions[0] = "-f";
	translateOptions[1] = "ESRI Shapefile";
	translateOptions[2] = "-overwrite";
	translateOptions[3] = "-clipsrc";
	translateOptions[4] = overlayFilename;
	this->GdalVectorTranslate(subjectFilename, destinationFilename, translateOptions, useSharedConnection, retVal);

	return S_OK;
}

// *********************************************************
//	     GdalBuildOverviews()
// *********************************************************
STDMETHODIMP CGdalUtils::GdalBuildOverviews(BSTR sourceFilename, const tkGDALResamplingMethod resamplingMethod,
	SAFEARRAY* overviewList, SAFEARRAY* bandList, SAFEARRAY* configOptions, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";
	CallbackParams params(_globalCallback, "Building overviews");

	//USES_CONVERSION;
	const CStringW srcFilename(sourceFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		ErrorMessage(tkINVALID_FILENAME);
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Source file %s does not exists."), srcFilename));
		_detailedError = "Source file " + srcFilename + " does not exists.";
		return S_OK;
	}

	// Open file as GdalDataset:
	CallbackHelper::Progress(_globalCallback, 0, "Open source file as raster", _key);
	const auto dt = GdalHelper::OpenRasterDatasetW(srcFilename, GA_ReadOnly);
	if (!dt)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("Can't open %s as a raster file."), srcFilename));
		_detailedError = "Can't open " + srcFilename + " as a raster file.";
		ErrorMsg(tkINVALID_FILENAME);
		goto cleaning;
	}

	// Set config options:	
	try
	{
		SetConfigOptionFromSafeArray(configOptions, false);
	}
	catch (const int e)
	{
		HandleException(e);
		goto cleaning;
	}

	auto nOverviews = 0;
	int* panOverviewList = nullptr;
	try
	{
		panOverviewList = ConvertSafeArrayToInt(overviewList, nOverviews);
	}
	catch (const int e)
	{
		HandleException(e);
		goto cleaning;
	}

	if (nOverviews == 0)
	{
		// Auto generate levels, see https://github.com/OSGeo/gdal/blob/master/gdal/apps/gdaladdo.cpp#L288
		const int nMinSize = 256;
		const int nXSize = GDALGetRasterXSize(dt);
		const int nYSize = GDALGetRasterYSize(dt);
		int nOvrFactor = 1;
		while (DIV_ROUND_UP(nXSize, nOvrFactor) > nMinSize ||
			DIV_ROUND_UP(nYSize, nOvrFactor) > nMinSize)
		{
			nOvrFactor *= 2;
			panOverviewList[nOverviews++] = nOvrFactor;
		}
	}

	auto nListBands = 0;
	int* panBandList = nullptr;
	try
	{
		panBandList = ConvertSafeArrayToInt(bandList, nListBands);
	}
	catch (const int e)
	{
		HandleException(e);
		goto cleaning;
	}

	const auto pszResampling = GdalHelper::GetResamplingAlgorithm(resamplingMethod);
	// It's a stack-based variable rather than an allocated variable.  So no delete is necessary.

	// Call the GDALBuildOverviews function:	
	m_globalSettings.SetGdalUtf8(true);
	// BuildOverviews(const char *pszResampling, int nOverviews, int *panOverviewList, int nListBands, int *panBandList, GDALProgressFuncp fnProgress, void *pProgressData)		
	const auto result = GDALBuildOverviews(dt, pszResampling, nOverviews, panOverviewList, nListBands, panBandList, GDALProgressCallback, &params);
	m_globalSettings.SetGdalUtf8(false);
	if (result == CE_None)
	{
		*retVal = VARIANT_TRUE;
	}
	else
	{
		*retVal = VARIANT_FALSE;
		ErrorMessage(tkGDAL_ERROR);
		CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, "Build overviews failed");
		// Put GDAL error in Detailed ErrorMsg:
		_detailedError = CPLGetLastErrorMsg();
	}

cleaning:
	// ------------------------------------------------------
	//	Cleaning
	// ------------------------------------------------------
	if (dt) GDALClose(dt);

	SetConfigOptionFromSafeArray(configOptions, true);

	// cleanup used arrays:
	delete[] panOverviewList;
	delete[] panBandList;
	// TODO: Doesn't work: delete[] pszResampling;

	CallbackHelper::ProgressCompleted(_globalCallback);

	return S_OK;
}

void CGdalUtils::HandleException(const int exception)
{
	if (exception == tkINVALID_PARAMETERS_ARRAY)
	{
		_detailedError = "The array doesn't have 1 dimension";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
	}
	else
	{
		_detailedError = "Unexpected exception in ConvertSafeArrayToInt";
		ErrorMessage(tkUNRECOVERABLE_ERROR);
	}

}

// *********************************************************************
//		get_LastErrorCode
// *********************************************************************
STDMETHODIMP CGdalUtils::get_LastErrorCode(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// *********************************************************************
//		get_ErrorMsg
// *********************************************************************
STDMETHODIMP CGdalUtils::get_ErrorMsg(const long errorCode, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = A2BSTR(ErrorMsg(errorCode));
	return S_OK;
}

// *********************************************************************
//		get_DetailedErrorMsg
// *********************************************************************
STDMETHODIMP CGdalUtils::get_DetailedErrorMsg(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = A2BSTR(_detailedError);
	return S_OK;
}

// *********************************************************************
//		GlobalCallback
// *********************************************************************
STDMETHODIMP CGdalUtils::get_GlobalCallback(ICallback** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _globalCallback;
	if (_globalCallback != nullptr)
	{
		_globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CGdalUtils::put_GlobalCallback(ICallback* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ComHelper::SetRef(newVal, reinterpret_cast<IDispatch**>(&_globalCallback));
	return S_OK;
}

STDMETHODIMP CGdalUtils::get_Key(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = OLE2BSTR(_key);
	return S_OK;
}

STDMETHODIMP CGdalUtils::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	SysFreeString(_key);
	_key = OLE2BSTR(newVal);

	return S_OK;
}

// **********************************************************
//		ErrorMessage()
// **********************************************************
inline void CGdalUtils::ErrorMessage(const long errorCode)
{
	_lastErrorCode = errorCode;
	CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// ***************************************************************************************
//		ConvertSafeArray()
//      Convert a safearray (coming outside the ocx) to char** (used internal) 
// ***************************************************************************************
char** CGdalUtils::ConvertSafeArrayToChar(SAFEARRAY* safeArray) const
{
	if (safeArray == nullptr) return nullptr;

	if (SafeArrayGetDim(safeArray) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The safe array list is invalid.")));
		throw tkINVALID_PARAMETERS_ARRAY;
	}

	USES_CONVERSION;
	char** papszStrList = nullptr;
	LONG lLBound, lUBound;
	BSTR HUGEP* pbstr;
	const auto hr1 = SafeArrayGetLBound(safeArray, 1, &lLBound);
	const auto  hr2 = SafeArrayGetUBound(safeArray, 1, &lUBound);
	const auto  hr3 = SafeArrayAccessData(safeArray, reinterpret_cast<void HUGEP * FAR*>(&pbstr));
	if (!FAILED(hr1) && !FAILED(hr2) && !FAILED(hr3))
	{
		const auto  count = lUBound - lLBound + 1;
		for (auto i = 0; i < count; i++) {
			// Add to array:
			papszStrList = CSLAddString(papszStrList, OLE2A(pbstr[i]));
		}

		// if safeArray was successfully locked, unlock it
		if (!FAILED(hr3))
		{
			SafeArrayUnaccessData(safeArray);
		}
	}

	return papszStrList;
}

// ***************************************************************************************
//		ConvertSafeArray()
//      Convert a safearray (coming outside the ocx) to int[] (used internal) 
// ***************************************************************************************
auto CGdalUtils::ConvertSafeArrayToInt(SAFEARRAY* safeArray, int& size) -> int*
{
	size = 0;
	if (safeArray == nullptr) return new int[1024];

	if (SafeArrayGetDim(safeArray) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The safe array list is invalid.")));
		throw tkINVALID_PARAMETERS_ARRAY;
	}

	//USES_CONVERSION;
	LONG lLBound, lUBound;
	int* pVals;
	const auto hr1 = SafeArrayGetLBound(safeArray, 1, &lLBound);
	const auto  hr2 = SafeArrayGetUBound(safeArray, 1, &lUBound);
	const auto  hr3 = SafeArrayAccessData(safeArray, reinterpret_cast<void**>(&pVals));
	if (!FAILED(hr1) && !FAILED(hr2) && !FAILED(hr3))
	{
		const auto count = lUBound - lLBound + 1;
		size = count;
		const auto arr = new int[count];
		for (auto i = 0; i < count; i++) {
			// Add to array:
			arr[i] = pVals[i];
		}

		// if safeArray was successfully locked, unlock it
		if (!FAILED(hr3))
		{
			SafeArrayUnaccessData(safeArray);
		}
		return arr;
	}

	return nullptr;
}


VOID CGdalUtils::SetConfigOptionFromSafeArray(SAFEARRAY* configOptions, const bool reset) const
{
	if (configOptions == nullptr) return;

	if (SafeArrayGetDim(configOptions) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format(static_cast<char*>("The config options list is invalid.")));
		throw tkINVALID_PARAMETERS_ARRAY;
	}

	USES_CONVERSION;
	LONG lLBound, lUBound;
	BSTR HUGEP* pbstr;
	const auto hr1 = SafeArrayGetLBound(configOptions, 1, &lLBound);
	const auto  hr2 = SafeArrayGetUBound(configOptions, 1, &lUBound);
	const auto  hr3 = SafeArrayAccessData(configOptions, reinterpret_cast<void HUGEP * FAR*>(&pbstr));
	if (!FAILED(hr1) && !FAILED(hr2) && !FAILED(hr3))
	{
		const auto  count = lUBound - lLBound + 1;
		for (auto i = 0; i < count; i++) {
			// Split value by space and set config:
			std::vector<std::string> v;
			const auto c = Utility::split(OLE2A(pbstr[i]), v, ' ');
			if (c == 2) {
				if (reset)
				{
					CPLSetConfigOption(v[0].c_str(), nullptr);
				}
				else {
					CPLSetConfigOption(v[0].c_str(), v[1].c_str());
				}

			}
		}

		// if safeArray was successfully locked, unlock it
		if (!FAILED(hr3))
		{
			SafeArrayUnaccessData(configOptions);
		}
	}
}
