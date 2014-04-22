/**************************************************************************************
 * File name: ShapefileColorBreak.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CShapefileColorBreak
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
 
#ifndef __SHAPEFILELEGENDBREAK_H_
#define __SHAPEFILELEGENDBREAK_H_

/////////////////////////////////////////////////////////////////////////////
// CShapefileColorBreak
class ATL_NO_VTABLE CShapefileColorBreak : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShapefileColorBreak, &CLSID_ShapefileColorBreak>,
	public IDispatchImpl<IShapefileColorBreak, &IID_IShapefileColorBreak, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShapefileColorBreak()
	{
		VariantInit(&startValue);
		VariantInit(&endValue);
		m_Visible = VARIANT_TRUE;
	}
	~CShapefileColorBreak()
	{	
		VariantClear(&startValue);
		VariantClear(&endValue);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SHAPEFILELEGENDBREAK)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CShapefileColorBreak)
	COM_INTERFACE_ENTRY(IShapefileColorBreak)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IShapefileColorBreak
public:
	STDMETHOD(get_EndColor)(/*[out, retval]*/ OLE_COLOR *pVal);
	STDMETHOD(put_EndColor)(/*[in]*/ OLE_COLOR newVal);
	STDMETHOD(get_StartColor)(/*[out, retval]*/ OLE_COLOR *pVal);
	STDMETHOD(put_StartColor)(/*[in]*/ OLE_COLOR newVal);
	STDMETHOD(get_EndValue)(/*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(put_EndValue)(/*[in]*/ VARIANT newVal);
	STDMETHOD(get_StartValue)(/*[out, retval]*/ VARIANT *pVal);
	STDMETHOD(put_StartValue)(/*[in]*/ VARIANT newVal);
	STDMETHOD(get_Visible)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_Visible)(/*[in]*/ VARIANT_BOOL newVal);

private:
	VARIANT startValue;
	VARIANT endValue;
	CString m_Caption;
	OLE_COLOR startColor;
	OLE_COLOR endColor;
	VARIANT_BOOL m_Visible;
public:
	STDMETHOD(get_Caption)(BSTR* pVal);
	STDMETHOD(put_Caption)(BSTR newVal);
};

#endif //__SHAPEFILELEGENDBREAK_H_

