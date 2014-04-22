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
 // lsu 17 apr 2012 - created the file

#pragma once
#include "TileCore.h"
#include "TileLoader.h"
using namespace std;

// Writes tile images in the following folder structure: Provider\Zoom\X\Y.png
class DiskCache
{
private:	
	static bool init;
	static CLSID pngClsid;
public:
	static CString encoder;		// gdi+ encoder
	static CStringW ext;		// extension (with leading point)
	static CStringW rootPath;	// root folder for storage
	
	static bool get_TileExists( LONG zoom, LONG x, LONG y)
	{
		return (Utility::fileExistsW(get_TilePath(zoom, x, y)) == TRUE);
	}

	static CStringW get_TilePath(int zoom, int x, int y)
	{
		CStringW path;
		path.Format(L"%s%d\\%d\\%d%s", rootPath, zoom, x, y, ext);
		return path;
	}

	// creates folders to store provided list of points
	void static CreateFolders(int zoom, vector<CTilePoint*>& points)
	{
		for(size_t i = 0; i < points.size(); i++)	
		{
			CreateFolder(zoom, points[i]);
		}
	}
	// creates folder for a single point
	void static CreateFolder(int zoom, CTilePoint* pnt)
	{
		CStringW name;
		name.Format(L"\\%d\\", zoom);	//	_mkdir can't create folders recursively
		int val = _wmkdir(rootPath + name);
		name.Format(L"\\%d\\%d\\", zoom, pnt->x);
		_wmkdir(rootPath + name);
	}

	// writes to disk a single tile
	void static DoCaching(TileCore* tile)
	{
		if (!init)
		{
			USES_CONVERSION;
			Utility::GetEncoderClsid(A2OLE(DiskCache::encoder), &pngClsid);
			init = true;
		}
		
		// TODO: better to get it from provider (as tile size is not necessarily 256 by 256 pixels)
		Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(256, 256);		
		Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(bmp);
		
		for (size_t i = 0; i < tile->Overlays.size(); i++)
		{
			Gdiplus::Bitmap* bmp = tile->getBitmap(i)->m_bitmap;
			if (bmp)
			{
				g->DrawImage(bmp, 0.0f, 0.0f);
			}
		}

		USES_CONVERSION;
		CStringW path = tile->getPath(rootPath, ext);
		bmp->Save(path, &pngClsid, NULL);

		delete g;
		delete bmp;
		
		tile->Release();
	}
};
