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
#include "TileCore.h"
#include "BaseProvider.h"

#define TILE_CACHE_SIZE_TO_CLEAR (20 * 0x1 << 20)	// 20 MB; when overall size of tiles exceeds maximum, tiles will be removed 
													// starting from the oldest until this amount of memory is freed

// ********************************************************
//     ITileCache()
// ********************************************************
// Base class for all types of cache
class ITileCache
{
protected:
	double _maxSize;

public:
	virtual void AddTile(TileCore* tile) = 0;
	virtual void Close() = 0;
	virtual TileCore* get_Tile(BaseProvider* provider, int scale, int x, int y) = 0;
	virtual bool get_TileExists(BaseProvider* provider, int scale, int x, int y) = 0;
	virtual bool IsLocked() = 0;
	virtual CacheType get_CacheType() = 0;
	virtual void Clear(int providerId = -1, int fromScale = 0, int toScale = 100) = 0;
	virtual double get_SizeMB() = 0;
	virtual double get_SizeMB(int providerId, int scale) = 0;
	virtual CStringW get_Filename() = 0;
	virtual void set_Filename(CStringW name) = 0;
	virtual void Initialize(bool canUseCache, bool canDoCaching) = 0;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual void InitBulkDownload(int zoom, vector<TilePoint*>& points) = 0;
	virtual long get_TileCount(int providerId, int zoom, CRect indices) = 0;

public:
	double get_MaxSize() { return _maxSize; }
	void set_MaxSize(double value) { _maxSize = value; }
};

// ********************************************************
//     TileCacheInfo()
// ********************************************************
// The users of tile cache are interested what they are allowed to do with it.
// Since they all may be using the same instance of cache object, 
// we shall use this wrapper to store permissions.
struct TileCacheInfo
{
	TileCacheInfo()
	{
		useCache = true;
		doCaching = true;
	}

	ITileCache* cache;
	bool useCache;
	bool doCaching;
};