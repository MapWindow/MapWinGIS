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
#include "TileMapLoader.h"

class TileManager
{
public:
	TileManager(bool isBackground)
		: _map(NULL), _lastZoom(-1), _lastProvider(-1), _isBackground(isBackground), 
		_loader(tctSqliteCache), _gridLinesVisible(false), _provider(NULL)
	{
		_useServer = true;
		_lastZoom = -1;
		_lastProvider = tkTileProvider::ProviderNone;
		_scalingRatio = 1.0;

		InitCaches();
	}

private:
	BaseProvider* _provider;
	TileMapLoader _loader;
	IMapViewCallback* _map;
	double _scalingRatio;
	bool _isBackground;			    // this is background TMS layer associated with ITiles (screen buffer can be scaled on zooming)
	TileCacheInfo _diskCache;
	TileCacheInfo _ramCache;
	vector<TileCacheInfo*> _caches;

	// can be wrapped in a separate class
	::CCriticalSection _tilesBufferLock;
	vector<TileCore*> _tiles;

	Extent _projExtents;			// extents of the world under current projection; in WGS84 it'll be (-180, 180, -90, 90)
	bool _projExtentsNeedUpdate;	// do we need to update bounds in m_projExtents on the next request?

	CRect _lastTileExtents;	// in tile coordinates
	Extent _lastMapExtents;
	int _lastZoom;
	int _lastProvider;
	bool _useServer;
	bool _gridLinesVisible;

private:
	void InitCaches();
	void UpdateScreenBuffer();
	bool IsBackground() { return _isBackground; }
	void BuildLoadingList(BaseProvider* provider, CRect indices, int zoom, vector<TilePoint*>& activeTasks, vector<TilePoint*>& points);
	void GetActiveTasks(std::vector<TilePoint*>& activeTasks, int providerId, int zoom, int newGeneration, CRect indices);
	bool IsNewRequest(Extent& mapExtents, CRect indices, int providerId, int zoom);
	void ClearBuffer();
	void DeleteMarkedTilesFromBuffer();
	void InitializeDiskCache();
	void UnlockDiskCache();
	bool GetTileIndices(BaseProvider* provider, CRect& indices, int& zoom, bool isSnapshot);

public:
	// properties
	void set_MapCallback(IMapViewCallback* map) { _map = map; }
	IMapViewCallback* get_MapCallback() { return _map; }
	bool TileIsInBuffer(int providerId, int zoom, int x, int y);
	bool useServer() { return _useServer; }
	void useServer(bool value) { _useServer = value; }
	double scalingRatio() { return _scalingRatio; }
	void scalingRatio(double value) { _scalingRatio = value; }
	ITileLoader* get_Loader() { return &_loader; }
	void CopyBuffer(vector<TileCore*>& buffer);
	TileCacheInfo* get_DiskCache() { return &_diskCache; }
	TileCacheInfo* get_RamCache() { return &_ramCache; }
	TileCacheInfo* get_Cache(tkCacheType type) { return type == Disk ? &_diskCache : &_ramCache; }
	vector<TileCacheInfo*>& get_AllCaches() { return _caches; }
	bool get_GridLinesVisible() { return _gridLinesVisible; }
	void set_GridLinesVisible(bool value) { _gridLinesVisible = value; }
	BaseProvider* get_Provider() { return _provider; }

public:
	// methods
	void Clear();
	void LoadTiles(BaseProvider* provider, bool isSnapshot, CString key);
	void MarkUndrawn();
	bool UndrawnTilesExist();
	bool DrawnTilesExist();

	void AddTileToRamCache(TileCore* tile);
	void AddTileWithCaching(TileCore* tile);
	void AddTileNoCaching(TileCore* tile);
	void AddTileOnlyCaching(TileCore* tile);

	void TriggerMapRedraw() { _map->_Redraw(tkRedrawType::RedrawSkipDataLayers, false, true); };
	void FireTilesLoaded(bool isSnapshot, CString key) { _map->_FireTilesLoaded(isSnapshot, key); }
	bool TilesAreInCache(BaseProvider* provider);
};