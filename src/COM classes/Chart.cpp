/**************************************************************************************
 * File name: Chart.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CChart
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
 // Sergei Leschinski (lsu) 25 june 2010 - created the file.

#include "stdafx.h"
#include "Chart.h"

// **********************************************************
//		PositionX
// **********************************************************
STDMETHODIMP CChart::get_PositionX(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _chartData->x;
	return S_OK;
}
STDMETHODIMP CChart::put_PositionX(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_chartData->x = newVal;
	return S_OK;
}

// **********************************************************
//		PositionY
// **********************************************************
STDMETHODIMP CChart::get_PositionY(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _chartData->y;
	return S_OK;
}
STDMETHODIMP CChart::put_PositionY(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_chartData->y = newVal;
	return S_OK;
}

// **********************************************************
//		ChartVisible
// **********************************************************
STDMETHODIMP CChart::get_Visible(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _chartData->visible;
	return S_OK;
}
STDMETHODIMP CChart::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_chartData->visible = newVal?true:false;
	return S_OK;
}

// **********************************************************
//		ChartVisible
// **********************************************************
STDMETHODIMP CChart::get_IsDrawn(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _chartData->isDrawn;
	return S_OK;
}
STDMETHODIMP CChart::put_IsDrawn(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_chartData->isDrawn = newVal?true:false;
	return S_OK;
}

// ***********************************************************
//		ScreenExtents
// ***********************************************************
STDMETHODIMP CChart::get_ScreenExtents(IExtents** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IExtents* ext = NULL;

	if (_chartData->frame)
	{
		ComHelper::CreateExtents(&ext);
		ext->SetBounds( _chartData->frame->left,_chartData->frame->top, 0.0,
						_chartData->frame->right, _chartData->frame->bottom, 0.0);
		*retval = ext;
	}
	else
	{
		*retval = NULL;
	}
	return S_OK;
}


// ***********************************************************
//		LabelData
// ***********************************************************
char* CChart::get_ChartData()
{
	return reinterpret_cast<char*>(_chartData);
}
void CChart::put_ChartData(char* newVal)
{
	if (newVal == NULL) return;
	
	// if the memory was allocated in this class we should free it;
	if (_canDelete)
	{
		delete _chartData;
		_chartData = NULL;
	}

	_chartData = reinterpret_cast<CChartInfo*>(newVal);
	_canDelete = false; // we can't delete the object hereafter as we didn't allocate the memory;
}