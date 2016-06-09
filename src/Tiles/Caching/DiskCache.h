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
#pragma once
#include "ITileCache.h"

// Writes tile images in the following folder structure: Provider\Zoom\X\Y.png
class DiskCache: public ITileCache
{
public:
	DiskCache::DiskCache(CStringW path, CStringW extension)
		: _init(false)
	{
		_ext = _ext;
		_rootPath = path;
		_encoder = L"image/png";	// default

		InitEncoder();
	}

private:	
	bool _init;
	CLSID _pngClsid;
	CString _encoder;	// GDI+ encoder
	CStringW _ext;		// extension (with leading point)
	CStringW _rootPath;	// root folder for storage

private:
	// methods
	void InitEncoder();

public:	
	// interface
	void AddTile(TileCore* tile);
	bool get_TileExists(BaseProvider* provider, int zoom, int x, int y);
	CacheType get_CacheType() { return tctDiskCache; }
	TileCore* get_Tile(BaseProvider* provider, int scale, int x, int y) { return NULL; };
	void Close() { };
	bool IsLocked() { return false; }
	void Clear(int providerId = -1, int fromScale = 0, int toScale = 100) {}
	double get_SizeMB() { return 0.0; }
	double get_SizeMB(int providerId, int scale) { return 0.0; }
	CStringW get_Filename() { return _rootPath; }
	void set_Filename(CStringW name)  { _rootPath = name; }
	virtual void Initialize(bool canUseCache, bool canDoCaching) { }
	void Lock() {  }
	void Unlock() {  }
	void InitBulkDownload(int zoom, vector<TilePoint*>& points) { CreateFolders(zoom, points); }
	long get_TileCount(int providerId, int zoom, CRect indices) { return 0; }

public:
	// properties:
	CStringW get_TilePath(int zoom, int x, int y);

public:
	// methods:
	void CreateFolders(int zoom, vector<TilePoint*>& points);	// creates folders to store provided list of points
	void CreateFolder(int zoom, TilePoint* pnt);	// creates folder for a single point
};
