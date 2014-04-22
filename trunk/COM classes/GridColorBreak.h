/**************************************************************************************
 * File name: GridColorBreak.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CGridColorBreak 
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

// CGridColorBreak
class ATL_NO_VTABLE CGridColorBreak : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGridColorBreak, &CLSID_GridColorBreak>,
	public IDispatchImpl<IGridColorBreak, &IID_IGridColorBreak, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGridColorBreak()
	{
		ColorType = Hillshade;
		GradModel = Linear;

		USES_CONVERSION;

		lastErrorCode = tkNO_ERROR;
		globalCallback = NULL;
		key = A2BSTR("");
	}
	~CGridColorBreak()
	{
		::SysFreeString(key);
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_GRIDCOLORBREAK)

DECLARE_NOT_AGGREGATABLE(CGridColorBreak)

BEGIN_COM_MAP(CGridColorBreak)
	COM_INTERFACE_ENTRY(IGridColorBreak)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


// IGridColorBreak
public:
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_GradientModel)(/*[out, retval]*/ GradientModel *pVal);
	STDMETHOD(put_GradientModel)(/*[in]*/ GradientModel newVal);
	STDMETHOD(get_ColoringType)(/*[out, retval]*/ ColoringType *pVal);
	STDMETHOD(put_ColoringType)(/*[in]*/ ColoringType newVal);
	STDMETHOD(get_LowValue)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_LowValue)(/*[in]*/ double newVal);
	STDMETHOD(get_HighValue)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_HighValue)(/*[in]*/ double newVal);
	STDMETHOD(get_LowColor)(/*[out, retval]*/ OLE_COLOR *pVal);
	STDMETHOD(put_LowColor)(/*[in]*/ OLE_COLOR newVal);
	STDMETHOD(get_HighColor)(/*[out, retval]*/ OLE_COLOR *pVal);
	STDMETHOD(put_HighColor)(/*[in]*/ OLE_COLOR newVal);

	
private:
	OLE_COLOR HighColor;
	OLE_COLOR LowColor;
	double LowValue;
	double HighValue;
	CString m_Caption;
	ColoringType ColorType;
	GradientModel GradModel;

private:
	long lastErrorCode;
	ICallback * globalCallback;
	BSTR key;
public:
	STDMETHOD(get_Caption)(BSTR* pVal);
	STDMETHOD(put_Caption)(BSTR newVal);
};


OBJECT_ENTRY_AUTO(__uuidof(GridColorBreak), CGridColorBreak)
