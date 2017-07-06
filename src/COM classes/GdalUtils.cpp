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

#include "stdafx.h"
#include "GdalUtils.h"
#include "GdalDataset.h"

// *********************************************************************
//		~CGdalUtils
// *********************************************************************
CGdalUtils::~CGdalUtils()
{

	gReferenceCounter.Release(tkInterface::idGdalUtils);
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
STDMETHODIMP CGdalUtils::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// *********************************************************
//	     Deserialize()
// *********************************************************
STDMETHODIMP CGdalUtils::GdalWarp(BSTR bstrSrcFilename, BSTR bstrDstFilename, SAFEARRAY* options, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CStringW srcFilename = OLE2W(bstrSrcFilename);
	if (!Utility::FileExistsW(srcFilename))
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	// Open file as GdalDataset:
	GDALDatasetH dt = GdalHelper::OpenRasterDatasetW(srcFilename, GA_ReadOnly);
	if (dt)
	{
		// Make options:		
		if (SafeArrayGetDim(options) != 1)
		{
			ErrorMessage(tkINVALID_PARAMETERS_ARRAY);
			return S_OK;
		}
		
		// TODO: Move to seperate function:
		char** warpOptions = NULL;
		LONG lLBound, lUBound;
		BSTR HUGEP *pbstr;
		HRESULT hr1 = SafeArrayGetLBound(options, 1, &lLBound);
		HRESULT hr2 = SafeArrayGetUBound(options, 1, &lUBound);
		HRESULT hr3 = SafeArrayAccessData(options, (void HUGEP* FAR*)&pbstr);
		if (!FAILED(hr1) && !FAILED(hr2) && !FAILED(hr3))
		{
			LONG count = lUBound - lLBound + 1;
			for (int i = 0; i < count; i++){
				// Create array:
				warpOptions = CSLAddString(warpOptions, OLE2A(pbstr[i]));
			}
		}

		GDALWarpAppOptions* gdalWarpOptions = GDALWarpAppOptionsNew(warpOptions, NULL);

		// TODO: Callback and error handling

		// Call the gdalWarp function:
		auto dtNew = GDALWarp(OLE2A(bstrDstFilename), NULL, 1, &dt, gdalWarpOptions, NULL);
		if (dtNew)
		{
			*retVal = VARIANT_TRUE;
			GDALClose(dtNew);
		}

		// Free options:
		GDALWarpAppOptionsFree(gdalWarpOptions);
		CSLDestroy(warpOptions);

		// Close the dataset:
		GDALClose(dt);		
	}

	return S_OK;
}

// **********************************************************
//		ErrorMessage()
// **********************************************************
inline void CGdalUtils::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	// TODO: CallbackHelper::ErrorMsg("Charts", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}
