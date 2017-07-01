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
#include "gdal_utils.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CGdalUtils :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGdalUtils, &CLSID_GdalUtils>,
	public IDispatchImpl<IGdalUtils, &IID_IGdalUtils, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGdalUtils(): _lastErrorCode(0)
	{
		m_pUnkMarshaler = NULL;

		gReferenceCounter.AddRef(tkInterface::idGdalUtils);
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
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(GdalWarp)(/*[in]*/ BSTR bstrSrcFilename, /*[in]*/ BSTR bstrDstFilename, /*[in]*/ SAFEARRAY* options, /*[out, retval]*/ VARIANT_BOOL* retVal);

private:	
	long _lastErrorCode;
	
public:
	// properties

public:
	// methods
	inline void ErrorMessage(long ErrorCode);
};

OBJECT_ENTRY_AUTO(__uuidof(GdalUtils), CGdalUtils)
