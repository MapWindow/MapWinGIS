/**************************************************************************************
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
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

#include "StdAfx.h"
#include "DiskCache.h"

// ReSharper disable CppUseAuto

// *********************************************************
//	     InitEncoder()
// *********************************************************
void DiskCache::InitEncoder()
{
	if (_ext.GetLength() >= 4)
	{
	    const CStringW s = _ext.Mid(0, 4).MakeLower(); // try to guess it from input

		if (s == L".png")
		{
			_encoder = "image/png";
		}
		else if (s == ".jpg")
		{
			_encoder = "image/jpeg";
		}
	}
}

// *********************************************************
//	     get_TileExists()
// *********************************************************
bool DiskCache::get_TileExists(BaseProvider* provider, int zoom, int x, int y)
{
	return Utility::FileExistsW(get_TilePath(zoom, x, y)) == TRUE;
}

// *********************************************************
//	     get_TilePath()
// *********************************************************
CStringW DiskCache::get_TilePath(int zoom, int x, int y)
{
	CStringW path;
	path.Format(L"%s%d\\%d\\%d%s", _rootPath, zoom, x, y, _ext);
	return path;
}

// *********************************************************
//	     CreateFolders()
// *********************************************************
// creates folders to store provided list of points
void DiskCache::CreateFolders(int zoom, vector<TilePoint*>& points)
{
	for (size_t i = 0; i < points.size(); i++)
	{
		CreateFolder(zoom, points[i]);
	}
}

// *********************************************************
//	     CreateFolder()
// *********************************************************
// creates folder for a single point
void DiskCache::CreateFolder(int zoom, TilePoint* pnt)
{
	CStringW name;
	name.Format(L"\\%d\\", zoom);	//	_mkdir can't create folders recursively
	_wmkdir(_rootPath + name);
	name.Format(L"\\%d\\%d\\", zoom, pnt->x);
	_wmkdir(_rootPath + name);
}

// *********************************************************
//	     AddTile()
// *********************************************************
// writes to disk a single tile
void DiskCache::AddTile(TileCore* tile)
{
	if (!_init)
	{
		USES_CONVERSION;
		Utility::GetEncoderClsid(A2OLE(_encoder), &_pngClsid);
		_init = true;
	}

	// TODO: better to get it from provider (as tile size is not necessarily 256 by 256 pixels)
	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(256, 256);
	Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(bmp);

	for (size_t i = 0; i < tile->Overlays.size(); i++)
	{
		bmp = tile->get_Bitmap(i)->m_bitmap;
		if (bmp)
		{
			g->DrawImage(bmp, 0.0f, 0.0f);
		}
	}

	USES_CONVERSION;
    const CStringW path = tile->GetPath(_rootPath, _ext);
	bmp->Save(path, &_pngClsid, nullptr);

	delete g;
	delete bmp;

	tile->Release();
}

// ReSharper restore CppUseAuto