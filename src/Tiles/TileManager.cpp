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
#include "stdafx.h"
#include "TileManager.h"

// ************************************************************
//		set_MapCallback()
// ************************************************************
void TileManager::set_MapCallback(IMapViewCallback* map)
{
	if (_map && map) 
	{
		// don't allow to use the same manager for 2 map controls,
		// sicne we have only one buffer for tiles;
		return;
	}

	_map = map;
}

// ************************************************************
//		InitCaches()
// ************************************************************
void TileManager::InitCaches()
{
	_ramCache.doCaching = true;
	_ramCache.useCache = true;
	_ramCache.cache = TileCacheManager::get_Cache(tctRamCache);
	_caches.push_back(&_ramCache);

	_diskCache.doCaching = false;
	_diskCache.useCache = true;
	_diskCache.cache = TileCacheManager::get_Cache(tctSqliteCache);
	_caches.push_back(&_diskCache);
}

// *********************************************************
//	     LoadTiles()
// *********************************************************
void TileManager::LoadTiles(BaseProvider* provider, bool isSnapshot, CString key)
{
	if (!provider) return;

	CRect indices;
	int zoom;

	ClearBuffer();

	if (!GetTileIndices(provider, indices, zoom, isSnapshot)) {
		return;
	}

	tilesLogger.WriteLine("\nLOAD TILES: xMin=%d; xMax=%d; yMin=%d; yMax=%d; zoom =%d", indices.left, indices.right, indices.bottom, indices.top, zoom);

	provider->put_Manager(this);

	InitializeDiskCache();

	// all incoming tasks will be discarded
	TileRequestInfo* requestInfo = _loader.CreateNextRequest(key, isSnapshot);

	//  check which ones we already have, and which ones are to be loaded
	std::vector<TilePoint*> activeTasks;
	GetActiveTasks(activeTasks, provider->Id, zoom, requestInfo->generation, indices);


	// loads tiles available in the cache to the buffer
	// builds list of tiles to be loaded from server
	std::vector<TilePoint*> points;
	BuildLoadingList(provider, indices, zoom, activeTasks, points);

	// it will be considered completed when this amount of tiles is loaded
	requestInfo->totalCount = activeTasks.size() + points.size();

	// delete unused tiles from the screen buffer
	DeleteMarkedTilesFromBuffer();

	UnlockDiskCache();

	_loader.RunCaching();

	_provider = provider;

	if (points.size() > 0)
	{
		tilesLogger.WriteLine("Queued to load from server: %d", points.size());

		// zoom can change in the process, so we use the calculated version and not the one current for provider
		_loader.Load(points, provider, zoom, requestInfo);
	}
	else
	{
		// tilesLogger.WriteLine("Tiles loaded event; Were loaded from server (y/n): %d", !nothingToLoad);
		FireTilesLoaded(isSnapshot, key);
	}

	TilePoint::ReleaseMemory(activeTasks);
	TilePoint::ReleaseMemory(points);
}

// *********************************************************
//	     GetTileIndices()
// *********************************************************
bool TileManager::GetTileIndices(BaseProvider* provider, CRect& indices, int& zoom, bool isSnapshot)
{
	Extent* mapExtents = _map->_GetExtents();

	if (!isSnapshot && _lastMapExtents == *mapExtents && _lastProvider == provider->Id)
	{
		tilesLogger.WriteLine("Duplicate request is dropped.");
		return false;
	}

	if (!_map->_GetTilesForMap(provider, _scalingRatio, indices, zoom))
	{
		return false;
	}

	if (!IsNewRequest(*mapExtents, indices, provider->Id, zoom)){
		return false;
	}

	// to apply API key for example
	if (!provider->Initialize())
	{
		Clear();
		UpdateScreenBuffer();
		return false;
	}

	return true;
}

// *********************************************************
//	     BuildLoadingList()
// *********************************************************
void TileManager::BuildLoadingList(BaseProvider* provider, CRect indices, int zoom, vector<TilePoint*>& activeTasks, vector<TilePoint*>& points)
{
	CPoint center = indices.CenterPoint();

	for (int x = indices.left; x <= indices.right; x++)
	{
		for (int y = indices.bottom; y <= indices.top; y++)
		{
			// was it already reassigned?
			bool found = false;
			for (size_t i = 0; i < activeTasks.size(); i++)
			{
				if (activeTasks[i]->x == x && activeTasks[i]->y == y)
				{
					found = true;
					break;
				}
			}

			if (found) {
				continue;
			}

			// check maybe the tile is already in the buffer
			_tilesBufferLock.Lock();

			for (size_t i = 0; i < _tiles.size(); i++)
			{
				TileCore* tile = _tiles[i];
				if (tile->tileX() == x && tile->tileY() == y  && tile->zoom() == zoom && tile->get_ProviderId() == provider->Id)
				{
					tile->toDelete(false);
					tile->inBuffer(true);
					found = true;
					break;
				}
			}

			_tilesBufferLock.Unlock();

			if (found) {
				continue;
			}

			// seeking through available caches
			for (size_t i = 0; i < _caches.size(); i++)
			{
				if (_caches[i]->useCache) 
				{
					TileCore* tile = _caches[i]->cache->get_Tile(provider, zoom, x, y);
					if (tile)
					{
						AddTileNoCaching(tile);
						found = true;
						break;
					}
				}
			}

			if (found) {
				continue;
			}

			// if the tile isn't present in both caches, request it from the server
			if (_useServer)
			{
				TilePoint* pnt = new TilePoint(x, y);
				pnt->dist = sqrt(pow((pnt->x - center.x), 2.0) + pow((pnt->y - center.y), 2.0));
				points.push_back(pnt);
			}
		}
	}
}

// *********************************************************
//	     Clear()
// *********************************************************
void TileManager::Clear()
{
	_tilesBufferLock.Lock();

	for (size_t i = 0; i < _tiles.size(); i++)
	{
		_tiles[i]->isDrawn(false);
		_tiles[i]->inBuffer(false);
		_tiles[i]->Release();
	}

	_tiles.clear();

	_tilesBufferLock.Unlock();

	_lastMapExtents.left = 0;
	_lastMapExtents.right = 0;
	_lastMapExtents.top = 0;
	_lastMapExtents.bottom = 0;

	_lastTileExtents.left = 0;
	_lastTileExtents.right = 0;
	_lastTileExtents.top = 0;
	_lastTileExtents.bottom = 0;
}

// *********************************************************
//	     UnlockDiskCache()
// *********************************************************
void TileManager::UnlockDiskCache()
{
	if (_diskCache.useCache)
	{
		// caching will be stopped while loading tiles to avoid locking the database and speed up things
		_diskCache.cache->Unlock();
	}
}

// *********************************************************
//	     InitializeDiskCache()
// *********************************************************
void TileManager::InitializeDiskCache()
{
	_diskCache.cache->Initialize(_diskCache.useCache, _diskCache.doCaching);
	if (_diskCache.useCache)
	{
		_diskCache.cache->Lock();	// caching will be stopped while loading tiles to avoid locking the database
	}
}

// *********************************************************
//	     DeleteMarkedTilesFromBuffer()
// *********************************************************
void TileManager::DeleteMarkedTilesFromBuffer()
{
	_tilesBufferLock.Lock();

	std::vector<TileCore*>::iterator it = _tiles.begin();

	while (it < _tiles.end())
	{
		TileCore* tile = (*it);
		if (tile->toDelete())
		{
			tile->Release();
			it = _tiles.erase(it);
		}
		else
		{
			++it;
		}
	}

	_tilesBufferLock.Unlock();
}

// *********************************************************
//	     IsNewRequest()
// *********************************************************
void TileManager::ClearBuffer()
{
	_tilesBufferLock.Lock();

	for (size_t i = 0; i < _tiles.size(); i++)
	{
		_tiles[i]->isDrawn(false);
		_tiles[i]->inBuffer(false);
		_tiles[i]->toDelete(true);
	}

	_tilesBufferLock.Unlock();

	UpdateScreenBuffer();
}

// *********************************************************
//	     IsNewRequest()
// *********************************************************
bool TileManager::IsNewRequest(Extent& mapExtents, CRect indices, int providerId, int zoom)
{
	if (indices == _lastTileExtents &&
		_lastProvider == providerId &&
		_lastZoom == zoom)
	{
		// map extents has changed but the list of tiles to be displayed is the same
		tilesLogger.WriteLine("The same list of tiles can be used.");

		UpdateScreenBuffer();

		return false;
	}

	_lastMapExtents = mapExtents;
	_lastTileExtents = indices;
	_lastProvider = providerId;
	_lastZoom = zoom;

	return true;
}

// *********************************************************
//	     GetActiveTasks()
// *********************************************************
void TileManager::GetActiveTasks(vector<TilePoint*>& activeTasks, int providerId, int zoom, int newGeneration, CRect indices)
{
	// lock it, so active task can't be removed while we analyze it here
	_loader.LockActiveTasks(true);

	std::list<void*> list = _loader.get_ActiveTasks();
	std::list<void*>::iterator it = list.begin();

	while (it != list.end())
	{
		ILoadingTask* task = (ILoadingTask*)*it;
		if (task->get_Provider()->Id == providerId &&
			task->zoom() == zoom &&
			task->x() >= indices.left &&
			task->x() <= indices.right &&
			task->y() >= indices.bottom &&
			task->y() <= indices.top)
		{
			tilesLogger.WriteLine("Tile reassigned to current generation: %d\\%d\\%d", zoom, task->x(), task->y());

			task->generation(newGeneration);								   // reassign it to current generation
			activeTasks.push_back(new TilePoint(task->x(), task->y()));    // don't include in current list of requests
		}

		++it;
	}

	_loader.LockActiveTasks(false);
}

// *********************************************************
//	     AddTileWithCaching()
// *********************************************************
void TileManager::AddTileWithCaching(TileCore* tile)
{
	AddTileNoCaching(tile);
	AddTileOnlyCaching(tile);
}

// *********************************************************
//	     AddTileNoCaching()
// *********************************************************
void TileManager::AddTileNoCaching(TileCore* tile)
{
	tile->inBuffer(true);
	tile->toDelete(false);
	tile->AddRef();

	_tilesBufferLock.Lock();
	_tiles.push_back(tile);
	_tilesBufferLock.Unlock();
}

// *********************************************************
//	     AddTileOnlyCaching()
// *********************************************************
void TileManager::AddTileOnlyCaching(TileCore* tile)
{
	if (_diskCache.doCaching) {
		_loader.ScheduleForCaching(tile);
	}
}

// *********************************************************
//	     AddTileToRamCache()
// *********************************************************
void TileManager::AddTileToRamCache(TileCore* tile) 
{
	if (_ramCache.doCaching)
	{
		_ramCache.cache->AddTile(tile);
	}
}

// *********************************************************
//	     TileIsInBufffer()
// *********************************************************
bool TileManager::TileIsInBuffer(int providerId, int zoom, int x, int y)
{
	CSingleLock lock(&_tilesBufferLock, TRUE);

	for (size_t i = 0; i < _tiles.size(); i++)
	{
		if (_tiles[i]->tileX() == x &&
			_tiles[i]->tileY() == y &&
			_tiles[i]->zoom() == zoom &&
			_tiles[i]->get_ProviderId() == providerId)
		{
			return true;
		}
	}

	return false;
}

// ************************************************************
//		MarkUndrawn()
// ************************************************************
void TileManager::MarkUndrawn()
{
	_tilesBufferLock.Lock();

	for (size_t i = 0; i < _tiles.size(); i++)
	{
		_tiles[i]->isDrawn(false);
	}

	_tilesBufferLock.Unlock();
}

// ************************************************************
//		UndrawnTilesExist()
// ************************************************************
// Returns true if at least one not drawn tile exists
bool TileManager::UndrawnTilesExist()
{
	CSingleLock lock(&_tilesBufferLock, TRUE);

	for (size_t i = 0; i < _tiles.size(); i++)
	{
		if (!_tiles[i]->isDrawn()) {
			return true;
		}
	}

	return false;
}

// ************************************************************
//		DrawnTilesExist()
// ************************************************************
// Returns true if at least one drawn tile exists
bool TileManager::DrawnTilesExist()
{
	CSingleLock lock(&_tilesBufferLock, TRUE);

	for (size_t i = 0; i < _tiles.size(); i++)
	{
		if (_tiles[i]->isDrawn()) {
			return true;
		}
	}

	return false;
}

// ************************************************************
//		CopyBuffer()
// ************************************************************
void TileManager::CopyBuffer(vector<TileCore*>& buffer)
{
	_tilesBufferLock.Lock();

	buffer.reserve(_tiles.size());
	copy(_tiles.begin(), _tiles.end(), inserter(buffer, buffer.end()));

	_tilesBufferLock.Unlock();
}

// ************************************************************
//		UpdateScreenBuffer()
// ************************************************************
void TileManager::UpdateScreenBuffer()
{
	if (_isBackground) {
		_map->_MarkTileBufferChanged();
	}
	else {
		_screenBufferChanged = true;
	}
}

// ************************************************************
//		get_ScreenBufferChanged()
// ************************************************************
bool TileManager::get_ScreenBufferChanged()
{
	bool val = _screenBufferChanged;
	_screenBufferChanged = false;
	return val;
}

// *********************************************************
//	     TilesAreInCache()
// *********************************************************
// checks whether all the tiles are present in cache
bool TileManager::TilesAreInCache(BaseProvider* provider)
{
	// if false is return the client code will try to load them
	// but there is no provider to do it
	if (!provider) return true;		

	CRect indices;
	int zoom;

	if (!_map->_GetTilesForMap(_provider, scalingRatio(), indices, zoom)) {
		return true;
	}

	for (int x = indices.left; x <= indices.right; x++)
	{
		for (int y = indices.bottom; y <= indices.top; y++)
		{
			if (TileIsInBuffer(provider->Id, zoom, x, y)) {
				continue;
			}

			bool found = false;

			vector<TileCacheInfo*>& caches = get_AllCaches();
			for (size_t i = 0; i < caches.size(); i++)
			{
				if (caches[i]->useCache)
				{
					if (caches[i]->cache->get_TileExists(provider, zoom, x, y))
					{
						found = true;
					}
				}
			}

			if (!found) {
				return false;
			}
		}
	}

	return true;
}
