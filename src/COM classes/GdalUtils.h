/**************************************************************************************
* File name: GdalUtils.h
*
* Project: MapWindow Open Source (MapWinGis ActiveX control)
* Description: Declaration of the CGdalUtils
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

#pragma once
#include "gdal_utils.h" // Is used, don't remove


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CGdalUtils :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CGdalUtils, &CLSID_GdalUtils>,
	public IDispatchImpl<IGdalUtils, &IID_IGdalUtils, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGdalUtils()
	{
		m_pUnkMarshaler = nullptr;
		_lastErrorCode = tkNO_ERROR;
		_globalCallback = nullptr;
		_key = SysAllocString(L"");
		gReferenceCounter.AddRef(idGdalUtils);
		CPLSetConfigOption("GDAL_NUM_THREADS", "ALL_CPUS");
	}

	virtual ~CGdalUtils();

	DECLARE_REGISTRY_RESOURCEID(IDR_GdalUtils)

	BEGIN_COM_MAP(CGdalUtils)
		COM_INTERFACE_ENTRY(IGdalUtils)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(
			GetControllingUnknown(), &m_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		m_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> m_pUnkMarshaler;

public:
	HRESULT __stdcall get_ErrorMsg(/*[in]*/ long errorCode, /*[out, retval]*/ BSTR *pVal) override;
	HRESULT __stdcall get_LastErrorCode(/*[out, retval]*/ long *pVal) override;
	HRESULT __stdcall get_DetailedErrorMsg(/*[out, retval]*/ BSTR *pVal) override;
	HRESULT __stdcall get_Key(/*[out, retval]*/ BSTR *pVal) override;
	HRESULT __stdcall put_Key(/*[in]*/ wchar_t* newVal) override;
	HRESULT __stdcall get_GlobalCallback(/*[out, retval]*/ ICallback * *pVal) override;
	HRESULT __stdcall put_GlobalCallback(/*[in]*/ ICallback * newVal) override;
	HRESULT __stdcall GdalRasterWarp(/*[in]*/ wchar_t* sourceFilename, /*[in]*/ wchar_t* destinationFilename, /*[in]*/ SAFEARRAY* options, /*[out, retval]*/ VARIANT_BOOL* retVal) override;
	HRESULT __stdcall GdalRasterTranslate(/*[in]*/ wchar_t* sourceFilename, /*[in]*/ wchar_t* destinationFilename, /*[in]*/ SAFEARRAY* options, /*[out, retval]*/ VARIANT_BOOL* retVal) override;
	HRESULT __stdcall GdalVectorTranslate(/*[in]*/ wchar_t* sourceFilename, /*[in]*/ wchar_t* destinationFilename, /*[in]*/ SAFEARRAY* options, /*[in, optional, defaultvalue(FALSE)]*/ VARIANT_BOOL useSharedConnection, /*[out, retval]*/ VARIANT_BOOL* retVal) override;
	HRESULT __stdcall ClipVectorWithVector(/*[in]*/ wchar_t* subjectFilename, /*[in]*/ wchar_t* overlayFilename, /*[in]*/ wchar_t* destinationFilename, /*[in, optional, defaultvalue(TRUE)]*/ VARIANT_BOOL useSharedConnection, /*[out, retval]*/ VARIANT_BOOL* retVal) override;
	HRESULT __stdcall GdalBuildOverviews(wchar_t* sourceFilename, tkGDALResamplingMethod resamplingAlgorithm, SAFEARRAY* overviewList, SAFEARRAY* bandList, SAFEARRAY* configOptions, VARIANT_BOOL* retVal) override;
	
private:
	long _lastErrorCode;
	ICallback * _globalCallback;
	BSTR _key;
	CString _detailedError;

	char** ConvertSafeArrayToChar(SAFEARRAY* safeArray) const;
	static int* ConvertSafeArrayToInt(SAFEARRAY* safeArray, int &size);
	void SetConfigOptionFromSafeArray(SAFEARRAY* configOptions, bool reset) const;
	void HandleException(int exception);

//public:
	// properties

public:
	// methods
	inline void ErrorMessage(long errorCode);
};

OBJECT_ENTRY_AUTO(__uuidof(GdalUtils), CGdalUtils)
