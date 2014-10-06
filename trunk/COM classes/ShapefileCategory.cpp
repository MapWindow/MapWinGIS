/**************************************************************************************
 * File name: ShapefileCategory.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CShapefileCategory - a class representing a particular
 * of shapefile visualization scheme
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
 // lsu 9 may 2010 - created the file

#include "stdafx.h"
#include "ShapefileCategory.h"
#include "Expression.h"
#include "Shapefile.h"
#include "TableClass.h"

// *****************************************************************
//		get/put_Name
// *****************************************************************
STDMETHODIMP CShapefileCategory::get_Name(BSTR* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retval = OLE2BSTR(m_name);
	return S_OK;
}
STDMETHODIMP CShapefileCategory::put_Name(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	::SysFreeString(m_name);
	m_name = OLE2BSTR(newVal);
	return S_OK;
}

// *****************************************************************
//		get/put_Expression
// *****************************************************************
STDMETHODIMP CShapefileCategory::get_Expression(BSTR* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retval = OLE2BSTR(m_expression);
	return S_OK;
}
STDMETHODIMP CShapefileCategory::put_Expression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	::SysFreeString(m_expression);
	m_expression = OLE2BSTR(newVal);
	m_value.vt = VT_EMPTY;
	return S_OK;
}

// ***************************************************************
//		get/put_DrawingOptions()
// ***************************************************************
STDMETHODIMP CShapefileCategory::get_DrawingOptions(IShapeDrawingOptions** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_drawingOptions;
	m_drawingOptions->AddRef();
	return S_OK;
}
STDMETHODIMP CShapefileCategory::put_DrawingOptions(IShapeDrawingOptions* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!newVal)
	{
		// TODO: report error
	}
	else
	{
		Utility::put_ComReference(newVal, (IDispatch**)&m_drawingOptions, false);
	}
	return S_OK;
}

// ***************************************************************
//		get/put_underlyingOptions()
// ***************************************************************
CDrawingOptionsEx* CShapefileCategory::get_UnderlyingOptions()
{
	if(m_drawingOptions != NULL)
	{
		return ((CShapeDrawingOptions*)m_drawingOptions)->get_UnderlyingOptions();
	}
	else
		return NULL;
}
void CShapefileCategory::put_underlyingOptions(CDrawingOptionsEx* newVal)
{
	if (m_drawingOptions == NULL)
	{
		// this should not happen, but we still add it to be sure
		CoCreateInstance(CLSID_ShapeDrawingOptions,NULL,CLSCTX_INPROC_SERVER,IID_IShapeDrawingOptions,(void**)&m_drawingOptions);
	}
	((CShapeDrawingOptions*)m_drawingOptions)->put_underlyingOptions(newVal);
}

// ***************************************************************
//		get_Count()
// ***************************************************************
// Returns number of shapes that fall into current category
//STDMETHODIMP CShapefileCategory::get_Count(LONG* retval)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState())
//	if (!_categories) return;
//	
//	IShapefile* sf = NULL;
//	_categories->get_Shapefile(&sf);
//	
//	if (sf)
//	{
//		long numShapes;
//		sf->get_NumShapes(&numShapes);
//
//		for (int i = 0; i < numShapes; i++ )
//		{
//			long val;
//			sf->get_ShapeCategory(i, &val);
//			if 
//		}
//	}
//
//	return S_OK;
//}
