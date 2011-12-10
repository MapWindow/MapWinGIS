/**************************************************************************************
 * File name: LabelClass.cpp 
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CLabelClass
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
 // lsu: jan 2010 - created the file.

#include "stdafx.h"
#include "LabelClass.h"

// ***********************************************************
//		LabelText
// ***********************************************************
// Passing pointer to LabelInfo structure through COM
STDMETHODIMP CLabelClass::get_Text(BSTR* retval)						
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retval = m_label->text.AllocSysString();
	return S_OK;
};
STDMETHODIMP CLabelClass::put_Text(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	m_label->text = OLE2CA(newVal);
	return S_OK;
};

// ***********************************************************
//		LabelData
// ***********************************************************
char* CLabelClass::get_LabelData()
{
	return reinterpret_cast<char*>(m_label);
}
void CLabelClass::put_LabelData(char* newVal)
{
	if (newVal == NULL) return;
	
	// if the memory was allocated in this class we should free it;
	if (m_canDelete)
		delete m_label;

	m_label = reinterpret_cast<CLabelInfo*>(newVal);
	
	m_canDelete = false; // we can't delete the object hereafter as we didn't allocate the memory;
}

// ***********************************************************
//		ScreenExtents
// ***********************************************************
STDMETHODIMP CLabelClass::get_ScreenExtents(IExtents** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IExtents* ext = NULL;
	if (m_label->horizontalFrame)
	{
		CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&ext);
		ext->SetBounds( m_label->horizontalFrame->left, m_label->horizontalFrame->top, 0.0,
						m_label->horizontalFrame->right, m_label->horizontalFrame->bottom, 0.0);
		*retval = ext;
	}
	else if (m_label->rotatedFrame)
	{
		CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&ext);
		CRect* rect = m_label->rotatedFrame->BoundingBox();
		ext->SetBounds( rect->left, rect->top, 0.0, rect->right, rect->bottom, 0.0);
		*retval = ext;
	}
	else
	{
		*retval = NULL;
	}
	return S_OK;
}
