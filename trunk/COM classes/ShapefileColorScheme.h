/**************************************************************************************
 * File name: ShapefileColorScheme.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CShapefileColorScheme
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

#ifndef __SHAPEFILELEGEND_H_
#define __SHAPEFILELEGEND_H_

#include "MapWinGis.h"       // main symbols
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CShapefileColorScheme
class ATL_NO_VTABLE CShapefileColorScheme : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShapefileColorScheme, &CLSID_ShapefileColorScheme>,
	public IDispatchImpl<IShapefileColorScheme, &IID_IShapefileColorScheme, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShapefileColorScheme()
	{	globalCallback = NULL;
		lastErrorCode = tkNO_ERROR;
		layerHandle = -1;
		key = A2BSTR("");
	}
	~CShapefileColorScheme()
	{	for( int i = 0; i < (int)allBreaks.size(); i++ )
		{	if( allBreaks[i] != NULL )
				allBreaks[i]->Release();
			allBreaks[i] = NULL;
		}
		SysFreeString(key);
		if( globalCallback != NULL )
			globalCallback->Release();
		globalCallback = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SHAPEFILELEGEND)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CShapefileColorScheme)
	COM_INTERFACE_ENTRY(IShapefileColorScheme)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IShapefileColorScheme
public:
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);	
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);	
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);	
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_FieldIndex)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_FieldIndex)(/*[in]*/ long newVal);
	STDMETHOD(get_LayerHandle)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_LayerHandle)(/*[in]*/ long newVal);
	STDMETHOD(get_ColorBreak)(/*[in]*/ long Index, /*[out, retval]*/ IShapefileColorBreak * *pVal);
	STDMETHOD(put_ColorBreak)(/*[in]*/ long Index, /*[in]*/ IShapefileColorBreak * newVal);
	STDMETHOD(Add)(/*[in]*/ IShapefileColorBreak * Break, /*[out,retval]*/ long * result);
	STDMETHOD(Remove)(/*[in]*/ long Index);
	STDMETHOD(NumBreaks)(/*[out, retval]*/ long * result);
	STDMETHOD(InsertAt)(/*[in]*/ int Position, /*[in]*/ IShapefileColorBreak *Break, /*[out,retval]*/ long *result);

private:
	long fieldIndex;
	long layerHandle;
	std::vector<IShapefileColorBreak *> allBreaks;
	long lastErrorCode;
	ICallback * globalCallback;
	BSTR key;
};

#endif //__SHAPEFILELEGEND_H_



