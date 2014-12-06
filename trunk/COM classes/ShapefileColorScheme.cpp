/**************************************************************************************
 * File name: ShapefileColorScheme.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CShapefileColorScheme
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
#include "ShapefileColorScheme.h"

/////////////////////////////////////////////////////////////////////////////
// CShapefileColorScheme

STDMETHODIMP CShapefileColorScheme::NumBreaks(long *result)
{
	
	*result = _allBreaks.size();	
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::Remove(long Index)
{
	
	if( Index >= 0 && Index < (int)_allBreaks.size() )
	{	if( _allBreaks[Index] != NULL )
			_allBreaks[Index]->Release();
		_allBreaks.erase( _allBreaks.begin() + Index );
	}
	else
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::Add(IShapefileColorBreak *Break, long *result)
{
	if( Break == NULL )
		return S_OK;

	
	*result = _allBreaks.size();
	Break->AddRef();
	_allBreaks.push_back( Break );
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::InsertAt(int Position, IShapefileColorBreak *Break, long *result)
{
	if( Break == NULL )
		return S_OK;

	
	*result = _allBreaks.size();
	Break->AddRef();
	_allBreaks.insert(_allBreaks.begin() + Position, Break);
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::get_ColorBreak(long Index, IShapefileColorBreak **pVal)
{
	
	if( Index >= 0 && Index < (int)_allBreaks.size() )
	{	
		if( _allBreaks[Index] != NULL )
			_allBreaks[Index]->AddRef();
		*pVal = _allBreaks[Index];
	}
	else
	{	*pVal = NULL;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);	
	}
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::put_ColorBreak(long Index, IShapefileColorBreak *newVal)
{
	if( Index >= 0 && Index < (int)_allBreaks.size() )
	{	
		if (!newVal)
		{
			ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		}
		else
		{
			ComHelper::SetRef(newVal, (IDispatch**)&_allBreaks[Index], false);
		}
	}
	else
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CShapefileColorScheme::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("ShapefileColorScheme", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

STDMETHODIMP CShapefileColorScheme::get_LayerHandle(long *pVal)
{

	*pVal = _layerHandle;
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::put_LayerHandle(long newVal)
{
	_layerHandle = newVal;
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::get_FieldIndex(long *pVal)
{
	*pVal = _fieldIndex;
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::put_FieldIndex(long newVal)
{
	_fieldIndex = newVal;
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _globalCallback;
	if( _globalCallback )
		_globalCallback->AddRef();

	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;

	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);

	return S_OK;
}

STDMETHODIMP CShapefileColorScheme::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);

	return S_OK;
}

