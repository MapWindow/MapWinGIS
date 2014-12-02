/**************************************************************************************
 * File name: LinePattern.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CLinePattern
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
 // Sergei Leschinski (lsu) 18 august 2010 - created the file.

#pragma once
#include "LineSegment.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// ------------------------------------------------------
//	   CLinePattern
// ------------------------------------------------------
class ATL_NO_VTABLE CLinePattern :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLinePattern, &CLSID_LinePattern>,
	public IDispatchImpl<ILinePattern, &IID_ILinePattern, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CLinePattern()
	{
		USES_CONVERSION;
		_key = A2BSTR("");
		_globalCallback = NULL;
		_lastErrorCode = tkNO_ERROR;
		_transparency = 255;
		gReferenceCounter.AddRef(idLinePattern);
	}
	~CLinePattern() 
	{
		::SysFreeString(_key);
		Clear();
		gReferenceCounter.Release(idLinePattern);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_LINEPATTERN)

	BEGIN_COM_MAP(CLinePattern)
		COM_INTERFACE_ENTRY(ILinePattern)
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
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	
	STDMETHOD(get_Count)(int* retVal);

	STDMETHOD(AddLine)(OLE_COLOR color, float width, tkDashStyle style);
	STDMETHOD(InsertLine)(int Index, OLE_COLOR color, float width, tkDashStyle style, VARIANT_BOOL* retVal);

	STDMETHOD(AddMarker)(tkDefaultPointSymbol marker, ILineSegment** retVal );
	STDMETHOD(InsertMarker)(int Index, tkDefaultPointSymbol marker, ILineSegment** retVal );

	STDMETHOD(RemoveItem)(int Index, VARIANT_BOOL* retVal);
	STDMETHOD(Clear)();

	STDMETHOD(get_Line)(int Index, ILineSegment** retVal);
	STDMETHOD(put_Line)(int Index, ILineSegment* retVal);
	
	STDMETHOD(Draw)(int** hdc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal);
	STDMETHOD(DrawVB)(int hdc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal);

	STDMETHOD(get_Transparency)(BYTE *retVal);
	STDMETHOD(put_Transparency)(BYTE newVal);

	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);
	
private:
	std::vector <ILineSegment*> _lines;
	BSTR _key;
	long _lastErrorCode;
	ICallback* _globalCallback;
	unsigned char _transparency;

private:
	void ErrorMessage(long ErrorCode);

public:
	bool DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);
	VARIANT_BOOL DrawCore(CDC* dc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor);
};

OBJECT_ENTRY_AUTO(__uuidof(LinePattern), CLinePattern)
