/**************************************************************************************
 * File name: ColorScheme.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CColorScheme 
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
 // lsu 2 jul 2010 - created the file

#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CColorScheme
class ATL_NO_VTABLE CColorScheme :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColorScheme, &CLSID_ColorScheme>,
	public IDispatchImpl<IColorScheme, &IID_IColorScheme, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CColorScheme()
	{
		_key = SysAllocString(L"");
		_globalCallback = NULL;
		_lastErrorCode = tkNO_ERROR;
		_type = ctSchemeGraduated;
	}

	~CColorScheme()
	{
		::SysFreeString(_key);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_COLORSCHEME)

	BEGIN_COM_MAP(CColorScheme)
		COM_INTERFACE_ENTRY(IColorScheme)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(SetColors)(OLE_COLOR Color1, OLE_COLOR Color2);
	STDMETHOD(SetColors2)(tkMapColor Color1, tkMapColor Color2);
	STDMETHOD(SetColors3)(short MinRed, short MinGreen, short MinBlue, short MaxRed, short MaxGreen, short MaxBlue);
	STDMETHOD(SetColors4)(PredefinedColorScheme Scheme);
	STDMETHOD(get_NumBreaks)(long * retVal);
	STDMETHOD(AddBreak)(double Value, OLE_COLOR Color);//, VARIANT_BOOL* retVal);
	STDMETHOD(Remove)(long Index, VARIANT_BOOL* retVal);
	STDMETHOD(Clear)();
	STDMETHOD(get_RandomColor)(double Value, OLE_COLOR* retVal);
	STDMETHOD(get_GraduatedColor)(double Value, OLE_COLOR* retVal);
	STDMETHOD(get_BreakColor)(long Index, OLE_COLOR* retVal);
	STDMETHOD(put_BreakColor)(long Index, OLE_COLOR newVal);
	STDMETHOD(get_BreakValue)(long Index, double* retVal);

	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	
private:
	struct ColorBreak
	{
		double value;
		OLE_COLOR color;
	};

private:	
	std::vector<ColorBreak> _breaks;
	BSTR _key;
	long _lastErrorCode;
	ICallback* _globalCallback;
	tkColorSchemeType _type;

public:
	void ErrorMessage(long ErrorCode);
};

OBJECT_ENTRY_AUTO(__uuidof(ColorScheme), CColorScheme)
