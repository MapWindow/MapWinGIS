/**************************************************************************************
 * File name: Tiles.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CTiles
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
 // lsu 21 aug 2011 - created the file

#include "stdafx.h"
#include "Tiles.h"

// *********************************************************
//	     get_Count
// *********************************************************
STDMETHODIMP CTiles::get_Count(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_tiles.size();
	return S_OK;
}

// *********************************************************
//	     Clear()
// *********************************************************
STDMETHODIMP CTiles::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for (unsigned int i = 0; i < m_tiles.size(); i++)
	{
		delete m_tiles[i];
	}
	m_tiles.clear();
	
	m_tilesLoaded = false;
	return S_OK;
}

// *********************************************************
//	     Visible
// *********************************************************
STDMETHODIMP CTiles::get_Visible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_visible;
	return S_OK;
}

STDMETHODIMP CTiles::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_visible = newVal;
	return S_OK;
}

// *********************************************************
//	     Add()
// *********************************************************
STDMETHODIMP CTiles::Add(VARIANT bytesArray, double xLon, double yLat, double widthLon, double heightLat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (bytesArray.vt != (VT_ARRAY|VT_UI1))
		return S_FALSE;
	
	unsigned char* p = NULL;
	SafeArrayAccessData(bytesArray.parray,(void HUGEP* FAR*)(&p));
	char* data = reinterpret_cast<char*>(p);
	
	CMemoryBitmap* bmp = new CMemoryBitmap();
	int size = (int)bytesArray.parray->rgsabound[0].cElements;
	bmp->LoadFromRawData(data, size);

	Tile* tile = new Tile(bmp, xLon, yLat, widthLon, heightLat);
	m_tiles.push_back(tile);
	
	SafeArrayUnaccessData(bytesArray.parray);

	m_tilesLoaded = true;

	return S_OK;
}