/**************************************************************************************
 * File name: PointClass.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CPointClass
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

// CPointClass
class ATL_NO_VTABLE CPointClass : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPointClass, &CLSID_Point>,
	public IDispatchImpl<IPoint, &IID_IPoint, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CPointClass()
	{	USES_CONVERSION;
		x = 0.0;
		y = 0.0;
		z = 0.0;
		m = 0.0;
		key = A2BSTR("");
		globalCallback = NULL;
		lastErrorCode = tkNO_ERROR;
		gReferenceCounter.AddRef(tkInterface::idPoint);
	}
	~CPointClass()
	{	globalCallback = NULL;
		::SysFreeString(key);
		gReferenceCounter.Release(tkInterface::idPoint);
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_POINT)

DECLARE_NOT_AGGREGATABLE(CPointClass)

BEGIN_COM_MAP(CPointClass)
	COM_INTERFACE_ENTRY(IPoint)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// IPoint
public:
	STDMETHOD(get_M)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_M)(/*[in]*/ double newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Z)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_Z)(/*[in]*/ double newVal);
	STDMETHOD(get_Y)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_Y)(/*[in]*/ double newVal);
	STDMETHOD(get_X)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_X)(/*[in]*/ double newVal);
	STDMETHOD(Clone)(IPoint** retVal);
private:
	double x;
	double y;
	double z;
	double m;
	BSTR key;
	long lastErrorCode;
	ICallback * globalCallback;	
};

OBJECT_ENTRY_AUTO(__uuidof(Point), CPointClass)
