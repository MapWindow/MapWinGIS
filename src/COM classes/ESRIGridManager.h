/**************************************************************************************
 * File name: ESRIGridManager.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CESRIGridManager 
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
 
#pragma once
#include "GridManager.h"

// CESRIGridManager
class ATL_NO_VTABLE CESRIGridManager : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CESRIGridManager, &CLSID_ESRIGridManager>,
	public IDispatchImpl<IESRIGridManager, &IID_IESRIGridManager, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CESRIGridManager()
	{	_globalCallback = NULL;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ESRIGRIDMANAGER)

DECLARE_NOT_AGGREGATABLE(CESRIGridManager)

BEGIN_COM_MAP(CESRIGridManager)
	COM_INTERFACE_ENTRY(IESRIGridManager)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// IESRIGridManager
public:
	STDMETHOD(IsESRIGrid)(/*[in]*/ BSTR Filename, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(DeleteESRIGrids)(/*[in]*/ BSTR Filename, /*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(CanUseESRIGrids)(/*[out, retval]*/ VARIANT_BOOL * retval);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);

private:
	ICallback * _globalCallback;
	GridManager _gm;
};

OBJECT_ENTRY_AUTO(__uuidof(ESRIGridManager), CESRIGridManager)
