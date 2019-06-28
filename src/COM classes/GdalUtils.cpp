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

#include "StdAfx.h"
#include "GdalUtils.h"
#include <atlsafe.h>
// #include "GdalDataset.h"


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
STDMETHODIMP CGdalUtils::GdalRasterWarp(const BSTR bstrSrcFilename, const BSTR bstrDstFilename, SAFEARRAY* options, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";

	USES_CONVERSION;
	const CStringW srcFilename = OLE2W(bstrSrcFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		ErrorMessage(tkINVALID_FILENAME);
		CallbackHelper::ErrorMsg(Debug::Format("Source file %s does not exists.", srcFilename));
		_detailedError = "Source file " + srcFilename + " does not exists.";
		return S_OK;
	}

	// Open file as GdalDataset:
	CallbackHelper::Progress(_globalCallback, 0, "Open source file as raster", _key);
	GDALDatasetH dt = GdalHelper::OpenRasterDatasetW(srcFilename, GA_ReadOnly);
	if (!dt)
	{
		CallbackHelper::ErrorMsg(Debug::Format("Can't open %s as a raster file.", srcFilename));
		_detailedError = "Can't open " + srcFilename + " as a raster file.";
		ErrorMsg(tkINVALID_FILENAME);
		goto cleaning;
	}

	// Make options:		
	if (SafeArrayGetDim(options) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format("The warp options are invalid."));
		_detailedError = "The option array doesn't have 1 dimension";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	const auto warpOptions = ConvertSafeArray(options);
	const auto gdalWarpOptions = GDALWarpAppOptionsNew(warpOptions, nullptr);
	if (!gdalWarpOptions)
	{
		CallbackHelper::ErrorMsg(Debug::Format("The warp options are invalid."));
		_detailedError = "Can't convert the option array to GDALWarpAppOptions";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	// Call the gdalWarp function:
	CallbackHelper::Progress(_globalCallback, 50, "Start warping", _key);
	GDALWarpAppOptionsSetProgress(gdalWarpOptions, GDALProgressCallback, nullptr);
	const auto dtNew = GDALWarp(OLE2A(bstrDstFilename), nullptr, 1, &dt, gdalWarpOptions, nullptr);
	CallbackHelper::Progress(_globalCallback, 75, "Finished warping", _key);
	if (dtNew)
	{
		*retVal = VARIANT_TRUE;
		GDALClose(dtNew);
	}
	else
	{
		CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, "Warping failed");
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
STDMETHODIMP CGdalUtils::GdalRasterTranslate(const BSTR bstrSrcFilename, const BSTR bstrDstFilename, SAFEARRAY* options, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";

	USES_CONVERSION;
	const CStringW srcFilename = OLE2W(bstrSrcFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		ErrorMessage(tkINVALID_FILENAME);
		CallbackHelper::ErrorMsg(Debug::Format("Source file %s does not exists.", srcFilename));
		_detailedError = "Source file " + srcFilename + " does not exists.";
		return S_OK;
	}

	// Open file as GdalDataset:
	CallbackHelper::Progress(_globalCallback, 0, "Open source file as raster", _key);
	const GDALDatasetH dt = GdalHelper::OpenRasterDatasetW(srcFilename, GA_ReadOnly);
	if (!dt)
	{
		CallbackHelper::ErrorMsg(Debug::Format("Can't open %s as a raster file.", srcFilename));
		_detailedError = "Can't open " + srcFilename + " as a raster file.";
		ErrorMsg(tkINVALID_FILENAME);
		goto cleaning;
	}

	// Make options:		
	if (SafeArrayGetDim(options) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format("The translate options are invalid."));
		_detailedError = "The option array doesn't have 1 dimension";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	const auto translateOptions = ConvertSafeArray(options);
	const auto gdalTranslateOptions = GDALTranslateOptionsNew(translateOptions, nullptr);
	if (!gdalTranslateOptions)
	{
		CallbackHelper::ErrorMsg(Debug::Format("The translate options are invalid."));
		_detailedError = "Can't convert the option array to GDALTranslateOptions";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	// Call the gdalWarp function:
	CallbackHelper::Progress(_globalCallback, 50, "Start translating", _key);
	GDALTranslateOptionsSetProgress(gdalTranslateOptions, GDALProgressCallback, nullptr);
	const auto dtNew = GDALTranslate(OLE2A(bstrDstFilename), dt, gdalTranslateOptions, nullptr);
	CallbackHelper::Progress(_globalCallback, 75, "Finished translating", _key);
	if (dtNew)
	{
		*retVal = VARIANT_TRUE;
		GDALClose(dtNew);
	}
	else
	{
		CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, "Raster translate failed");
		// TODO: Put GDAL error in Detailed ErrorMsg. Not sure how to get the GDAL error.
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
//	     GdalVectorTranslate()
// *********************************************************
STDMETHODIMP CGdalUtils::GdalVectorTranslate(const BSTR bstrSrcFilename, const BSTR bstrDstFilename, SAFEARRAY* options, const VARIANT_BOOL useSharedConnection, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";

	USES_CONVERSION;
	const CStringW srcFilename = OLE2W(bstrSrcFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		CallbackHelper::ErrorMsg(Debug::Format("Source file %s does not exists.", srcFilename));
		_detailedError = "Subject file " + srcFilename + " does not exists.";
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	// Open file as GdalDataset:
	CallbackHelper::Progress(_globalCallback, 0, "Open source file as vector", _key);

	GDALDatasetH dt = GdalHelper::OpenOgrDatasetW(srcFilename, GA_ReadOnly, useSharedConnection == VARIANT_TRUE);
	if (!dt)
	{
		CallbackHelper::ErrorMsg(Debug::Format("Can't open %s as a vector file.", srcFilename));
		_detailedError = "Can't open " + srcFilename + " as a vector file.";
		ErrorMsg(tkINVALID_FILENAME);
		goto cleaning;
	}

	// Make options:		
	if (SafeArrayGetDim(options) != 1)
	{
		CallbackHelper::ErrorMsg(Debug::Format("The vector translate options are invalid."));
		_detailedError = "The option array doesn't have 1 dimension";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	const auto translateOptions = ConvertSafeArray(options);
	const auto gdalVectorTranslateOptions = GDALVectorTranslateOptionsNew(translateOptions, nullptr);
	if (!gdalVectorTranslateOptions)
	{
		CallbackHelper::ErrorMsg(Debug::Format("The vector translate options are invalid."));
		_detailedError = "Can't convert the option array to GDALVectorTranslateOptions";
		ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
		goto cleaning;
	}

	// Call the gdalWarp function:
	CallbackHelper::Progress(_globalCallback, 50, "Start translating", _key);
	GDALVectorTranslateOptionsSetProgress(gdalVectorTranslateOptions, GDALProgressCallback, nullptr);
	const auto dtNew = GDALVectorTranslate(OLE2A(bstrDstFilename), nullptr, 1, &dt, gdalVectorTranslateOptions, nullptr);
	CallbackHelper::Progress(_globalCallback, 75, "Finished translating", _key);
	if (dtNew)
	{
		*retVal = VARIANT_TRUE;
		GDALClose(dtNew);
	}
	else
	{
		CallbackHelper::ErrorMsg("GdalUtils", _globalCallback, _key, "Translating failed");
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
STDMETHODIMP CGdalUtils::ClipVectorWithVector(const BSTR bstrSubjectFilename, const BSTR bstrOverlayFilename, const BSTR bstrDstFilename, const VARIANT_BOOL useSharedConnection, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	_detailedError = "No error";

	USES_CONVERSION;
	const CStringW subjectFilename = OLE2W(bstrSubjectFilename);
	if (!Utility::FileExistsW(subjectFilename))
	{
		CallbackHelper::ErrorMsg(Debug::Format("Subject file %s does not exists.", subjectFilename));
		_detailedError = "Subject file " + subjectFilename + " does not exists.";
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	const CStringW overlayFilename = OLE2W(bstrOverlayFilename);
	if (!Utility::FileExistsW(overlayFilename))
	{
		CallbackHelper::ErrorMsg(Debug::Format("Overlay file %s does not exists.", overlayFilename));
		_detailedError = "Overlay file " + overlayFilename + " does not exists.";
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	// Call VectorTranslate:
	CComSafeArray<BSTR> translateOptions(10);
	translateOptions[0] = "-f";
	translateOptions[1] = "ESRI Shapefile";
	translateOptions[2] = "-overwrite";
	translateOptions[3] = "-clipsrc";
	translateOptions[4] = bstrOverlayFilename;
	this->GdalVectorTranslate(bstrSubjectFilename, bstrDstFilename, translateOptions, useSharedConnection, retVal);

	return S_OK;
}

// *********************************************************************
//		get_LastErrorCode
// *********************************************************************
STDMETHODIMP CGdalUtils::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// *********************************************************************
//		get_ErrorMsg
// *********************************************************************
STDMETHODIMP CGdalUtils::get_ErrorMsg(const long errorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(errorCode));
	return S_OK;
}

// *********************************************************************
//		get_DetailedErrorMsg
// *********************************************************************
STDMETHODIMP CGdalUtils::get_DetailedErrorMsg(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	*pVal = A2BSTR(static_cast<LPCSTR>(_detailedError));
	return S_OK;
}

// *********************************************************************
//		GlobalCallback
// *********************************************************************
STDMETHODIMP CGdalUtils::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		*pVal = _globalCallback;
	if (_globalCallback != nullptr)
	{
		_globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CGdalUtils::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		ComHelper::SetRef(newVal, reinterpret_cast<IDispatch**>(&_globalCallback));
	return S_OK;
}

STDMETHODIMP CGdalUtils::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

	*pVal = OLE2BSTR(_key);

	return S_OK;
}

STDMETHODIMP CGdalUtils::put_Key(const BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;

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
char** CGdalUtils::ConvertSafeArray(SAFEARRAY* safeArray) const
{
	USES_CONVERSION;
	char** papszStrList = nullptr;
	LONG lLBound, lUBound;
	BSTR HUGEP *pbstr;
	const auto hr1 = SafeArrayGetLBound(safeArray, 1, &lLBound);
	const auto  hr2 = SafeArrayGetUBound(safeArray, 1, &lUBound);
	const auto  hr3 = SafeArrayAccessData(safeArray, reinterpret_cast<void HUGEP* FAR*>(&pbstr));
	if (!FAILED(hr1) && !FAILED(hr2) && !FAILED(hr3))
	{
		const auto  count = lUBound - lLBound + 1;
		for (auto i = 0; i < count; i++){
			// Create array:
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
