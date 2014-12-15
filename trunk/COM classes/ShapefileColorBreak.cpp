/**************************************************************************************
 * File name: ShapefileColorBreak.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CShapefileColorBreak
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
 
#include "stdafx.h"
#include "ShapefileColorBreak.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef OLD_API
/////////////////////////////////////////////////////////////////////////////
// CShapefileColorBreak

STDMETHODIMP CShapefileColorBreak::get_StartValue(VARIANT *pVal)
{
	VariantClear(pVal);
	VariantInit(pVal);
	VariantCopy(pVal,&_startValue);
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::put_StartValue(VARIANT newVal)
{
	VariantClear(&_startValue);	
	VariantInit(&_startValue);	
	VariantCopy(&_startValue,&newVal);
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::get_EndValue(VARIANT *pVal)
{
	VariantClear(pVal);
	VariantInit(pVal);
	VariantCopy(pVal,&_endValue);
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::put_EndValue(VARIANT newVal)
{
	VariantClear(&_endValue);
	VariantInit(&_endValue);	
	VariantCopy(&_endValue,&newVal);
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::get_StartColor(OLE_COLOR *pVal)
{
	*pVal = _startColor;
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::put_StartColor(OLE_COLOR newVal)
{
	_startColor = newVal;
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::get_EndColor(OLE_COLOR *pVal)
{
	*pVal = _endColor;
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::put_EndColor(OLE_COLOR newVal)
{
	_endColor = newVal;
	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::get_Visible(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _visible;

	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_visible = newVal;

	return S_OK;
}


STDMETHODIMP CShapefileColorBreak::get_Caption(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _caption.AllocSysString();

	return S_OK;
}

STDMETHODIMP CShapefileColorBreak::put_Caption(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_caption = newVal;

	return S_OK;
}

#endif