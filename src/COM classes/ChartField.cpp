/**************************************************************************************
 * File name: ChartField.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CChartField
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
 // Sergei Leschinski (lsu) 12 aug 2010 - created the file.
#include "stdafx.h"
#include "ChartField.h"

// **********************************************************
//		get/put_Index
// **********************************************************
STDMETHODIMP CChartField::get_Index(long* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _index;
	return S_OK;
}
STDMETHODIMP CChartField::put_Index(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_index = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_Color
// **********************************************************
STDMETHODIMP CChartField::get_Color(OLE_COLOR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _color;
	return S_OK;
}
STDMETHODIMP CChartField::put_Color(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_color = newVal;
	return S_OK;
}

// **********************************************************
//		get/put_Name
// **********************************************************
STDMETHODIMP CChartField::get_Name(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = OLE2BSTR(_name);
	return S_OK;
}
STDMETHODIMP CChartField::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_name);
	USES_CONVERSION;
	_name = OLE2BSTR(newVal);
	return S_OK;
}
