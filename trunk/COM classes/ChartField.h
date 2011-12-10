/**************************************************************************************
 * File name: ChartField.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CChartsField
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
 // Sergei Leschinski (lsu) 19 june 2010 - created the file.

#pragma once
#include "MapWinGis.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CChartField
class ATL_NO_VTABLE CChartField :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CChartField, &CLSID_ChartField>,
	public IDispatchImpl<IChartField, &IID_IChartField, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CChartField()
	{
		USES_CONVERSION;
		_name =  A2BSTR("");
		_index = -1;
		_color = RGB(255, 255, 255);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_CHARTFIELD)
	BEGIN_COM_MAP(CChartField)
		COM_INTERFACE_ENTRY(IChartField)
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
	STDMETHOD(get_Index)( long* retVal);
	STDMETHOD(put_Index)( long newVal);
	STDMETHOD(get_Color)( OLE_COLOR* retVal);
	STDMETHOD(put_Color)( OLE_COLOR newVal);
	STDMETHOD(get_Name)( BSTR* retVal);
	STDMETHOD(put_Name)( BSTR newVal);

private:	
	BSTR _name;
	int _index;
	OLE_COLOR _color;
	
	// TODO: add ShapeDrawingOptions later
};

OBJECT_ENTRY_AUTO(__uuidof(ChartField), CChartField)
