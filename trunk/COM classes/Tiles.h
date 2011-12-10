/**************************************************************************************
 * File name: Tiles.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CTiles
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

#pragma once
#include "MapWinGIS.h"
#include "InMemoryBitmap.h"
#include <vector>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

struct Tile
{
	CMemoryBitmap* Bitmap;
	double xLon;
	double yLat;
	double widthLon;
	double heightLat;
	bool Drawn;

	Tile(CMemoryBitmap* bmp, double x, double y, double width, double height)
	{
		this->Bitmap = bmp;
		this->xLon = x;
		this->yLat = y;
		this->widthLon = width;
		this->heightLat = height;
		this->Drawn = false;
	}

	~Tile()
	{
		if (this->Bitmap)
			delete this->Bitmap;
	}
};

class ATL_NO_VTABLE CTiles :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTiles, &CLSID_Tiles>,
	public IDispatchImpl<ITiles, &IID_ITiles, &LIBID_MapWinGIS, /*wMajor =*/ 4, /*wMinor =*/ 8>
{
public:
	CTiles()
	{
		m_visible = false;
		m_tilesLoaded = false;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TILES)

	BEGIN_COM_MAP(CTiles)
		COM_INTERFACE_ENTRY(ITiles)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(Clear)(void);
	STDMETHOD(get_Count)(LONG* pVal);
	STDMETHOD(get_Visible)(VARIANT_BOOL* pVal);
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal);
	STDMETHOD(Add)(VARIANT bytesArray, double xLon, double yLat, double widthLon, double heightLat);
	
	bool UndrawnTilesExist()
	{
		for (unsigned int i = 0; i < m_tiles.size(); i++)
		{
			if (!m_tiles[i]->Drawn)
				return true;
		}
		return false;
	}
	bool DrawnTilesExist()
	{
		for (unsigned int i = 0; i < m_tiles.size(); i++)
		{
			if (m_tiles[i]->Drawn)
				return true;
		}
		return false;
	}

	std::vector<Tile*> m_tiles;
	bool m_tilesLoaded;	   // undrawn tiles do exist
private:	
	VARIANT_BOOL m_visible;
};

OBJECT_ENTRY_AUTO(__uuidof(Tiles), CTiles)
