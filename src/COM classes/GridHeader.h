/**************************************************************************************
 * File name: GridHeader.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CGridHeader
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
#include "fheader.h"
#include "sheader.h"
#include "dheader.h"
#include "lheader.h"
#include "tkGridRaster.h"

// CGridHeader
class ATL_NO_VTABLE CGridHeader : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CGridHeader, &CLSID_GridHeader>,
	public IDispatchImpl<IGridHeader, &IID_IGridHeader, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGridHeader()
	{	USES_CONVERSION;
		_dx = 1.0;
		_dy = 1.0;	
		_key = SysAllocString(L"");
		_notes = SysAllocString(L"");
		_colorTable = SysAllocString(L"");
		_globalCallback = NULL;
		_numbercols = 0;
		_numberrows = 0;
		_nodatavalue = -1;
		_lastErrorCode = tkNO_ERROR;

		_myowner_d = NULL;
		_myowner_f = NULL;
		_myowner_l = NULL;
		_myowner_t = NULL;
		_myowner_s = NULL;

		CoCreateInstance(CLSID_GeoProjection,NULL,CLSCTX_INPROC_SERVER,IID_IGeoProjection,(void**)&_projection);
	}
	~CGridHeader()
	{	
		::SysFreeString(_key);
		::SysFreeString(_notes);
		::SysFreeString(_colorTable);

		// Note -- don't delete myowner_*, these are just references
		// to things managed elsewhere
		_myowner_d = NULL;
		_myowner_f = NULL;
		_myowner_l = NULL;
		_myowner_t = NULL;
		_myowner_s = NULL;

		_projection->Release();
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GRIDHEADER)

	DECLARE_NOT_AGGREGATABLE(CGridHeader)

	BEGIN_COM_MAP(CGridHeader)
		COM_INTERFACE_ENTRY(IGridHeader)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()


// IGridHeader
public:
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Notes)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Notes)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Projection)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Projection)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_YllCenter)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_YllCenter)(/*[in]*/ double newVal);
	STDMETHOD(get_XllCenter)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_XllCenter)(/*[in]*/ double newVal);
	STDMETHOD(get_dY)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_dY)(/*[in]*/ double newVal);
	STDMETHOD(get_dX)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_dX)(/*[in]*/ double newVal);
	STDMETHOD(get_NodataValue)(/*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(put_NodataValue)(/*[in]*/ VARIANT newVal);
	STDMETHOD(get_NumberRows)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_NumberRows)(/*[in]*/ long newVal);
	STDMETHOD(get_NumberCols)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_NumberCols)(/*[in]*/ long newVal);
	STDMETHOD(put_Owner)(/*[in]*/ int * t, /*[in]*/ int * d, /*[in]*/ int * s, /*[in]*/ int * l, /*[in]*/ int * f);
	STDMETHOD(CopyFrom)(/*[in]*/ IGridHeader * pVal);
	STDMETHOD(get_ColorTable)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ColorTable)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GeoProjection)(IGeoProjection** pVal);
	STDMETHOD(put_GeoProjection)(IGeoProjection* newVal);

private:
	ICallback * _globalCallback;
	long _lastErrorCode;
	BSTR _key;	
	BSTR _notes;
	BSTR _colorTable;
	double _yllcenter;
	double _xllcenter;
	double _dx;
	double _dy;
	double _nodatavalue;
	long _numberrows;
	long _numbercols;
	IGeoProjection* _projection;

	dHeader * _myowner_d;
	fHeader * _myowner_f;
	sHeader * _myowner_s;
	lHeader * _myowner_l;
	tkGridRaster * _myowner_t;

private:
	void AttemptSave();
	void ErrorMessage(long ErrorCode);
};

OBJECT_ENTRY_AUTO(__uuidof(GridHeader), CGridHeader)
