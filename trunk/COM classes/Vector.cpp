/**************************************************************************************
 * File name: Vector.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CVector
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
#include "Vector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CVector
STDMETHODIMP CVector::get_i(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = v.geti();
	return S_OK;
}

STDMETHODIMP CVector::put_i(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	v.seti( newVal );
	return S_OK;
}

STDMETHODIMP CVector::get_j(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = v.getj();
	return S_OK;
}

STDMETHODIMP CVector::put_j(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	v.setj( newVal );
	return S_OK;
}

STDMETHODIMP CVector::get_k(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = v.getk();
	return S_OK;
}

STDMETHODIMP CVector::put_k(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	v.setk( newVal );
	return S_OK;
}

STDMETHODIMP CVector::Normalize()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	v.Normalize();
	return S_OK;
}

STDMETHODIMP CVector::Dot(IVector *V, double *result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	double i = 0, j = 0, k = 0;
	V->get_i( &i );
	V->get_j( &j );
	V->get_k( &k );
	*result = v.dot( cppVector( i, j, k ) );
	return S_OK;
}

STDMETHODIMP CVector::CrossProduct(IVector *V, IVector **result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	IVector * normal = new CComObject<CVector>;
	
	double i = 0, j = 0, k = 0;
	V->get_i( &i );
	V->get_j( &j );
	V->get_k( &k );
	cppVector cp = v.crossProduct( cppVector( i, j, k ) );

	normal->put_i( cp.geti() );
	normal->put_j( cp.getj() );
	normal->put_k( cp.getk() );

	*result = normal;

	return S_OK;
}



STDMETHODIMP CVector::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = lastErrorCode;
	lastErrorCode = tkNO_ERROR;

	return S_OK;
}

STDMETHODIMP CVector::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CVector::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = globalCallback;
	if( globalCallback != NULL )
	{	
		globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CVector::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	return S_OK;
}

STDMETHODIMP CVector::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	*pVal = OLE2BSTR(key);
	return S_OK;
}

STDMETHODIMP CVector::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	::SysFreeString(key);
	key = OLE2BSTR(newVal);

	return S_OK;
}

