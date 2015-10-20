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
using namespace std;

// provider id -> zoom -> x -> y
typedef std::map<int, TileCore*>					TilePositions;   // indexed by y
typedef std::map<int, TilePositions*>				TilePoints;		 // indexed by x
typedef std::map<int, std::vector<TilePoints*>*>	TilesCache;		 // indexed by provider; indices in vector - by zoom 

// Provides storage for map tiles in RAM
class RamCache: public ITileCache
{
public:
	RamCache()
		: _size(0)
	{
		_maxSize = 100.0;
	}

private:	
	::CCriticalSection _section;
	vector<TileCore*> _list;		// the list of tiles in the chronological order
	TilesCache _tilesMap;		// the main storage
	int _size;					// size of cache in bytes

private:
	void DeleteMarkedTiles();	// deletes tiles from chronological vector	
	TileCore* get_TileCore(int providerId, int zoom, int tileX, int tileY);

public:
	// interface
	void Close();
	void AddTile(TileCore* tile);
	TileCore* get_Tile(BaseProvider* provider, int scale, int tileX, int tileY);
	CacheType get_CacheType() { return tctRamCache; }
	void Clear(int provider = -1, int fromScale = 0, int toScale = 100);
	double get_SizeMB() { return get_SizeMB(-1, -1); }
	double get_SizeMB(int provider, int scale);
	bool get_TileExists(BaseProvider* provider, int scale, int x, int y);
	bool IsLocked() { return false; }
	CStringW get_Filename() { return L""; }
	void set_Filename(CStringW name)  {  }
	virtual void Initialize(bool canUseCache, bool canDoCaching) { }
	void Lock() {  }
	void Unlock() {  }
	void InitBulkDownload(int zoom, vector<TilePoint*>& points) {  }
	long get_TileCount(int providerId, int zoom, CRect indices);

public:
	// methods
	void ClearAll(int fromScale, int toScale) { Clear(-1, fromScale, toScale); }
	void ClearOldest(int sizeToClearBytes);
	void OnProviderClosed(int providerId);

};
