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
#include "map.h"
#include "SQLiteCache.h"
#include "TileLoader.h"
#include "LoadingTask.h"
#include "RamCache.h"

// *********************************************************
//	     LoadTiles()
// *********************************************************
void TileManager::LoadTiles(BaseProvider* provider, bool isSnapshot, CString key)
{
	if (!provider) return;

	CRect indices;
	int zoom;

	if (!GetTileIndices(provider, indices, zoom)) {
		return;
	}

	tilesLogger.WriteLine("\nLOAD TILES: xMin=%d; xMax=%d; yMin=%d; yMax=%d; zoom =%d", indices.left, indices.right, indices.bottom, indices.top, zoom);

	ClearBuffer();

	provider->put_Manager(this);

	InitializeDiskCache();

	// all incoming tasks will be discarded
	int generation = ++_tileLoader.tileGeneration;

	//  check which ones we already have, and which ones are to be loaded
	std::vector<CTilePoint*> activeTasks;
	if (!isSnapshot)
	{
		GetActiveTasks(activeTasks, provider->Id, zoom, generation, indices);
	}

	// loads tiles available in the cache to the buffer
	// builds list of tiles to be loaded from server
	std::vector<CTilePoint*> points;
	BuildLoadingList(provider, indices, zoom, activeTasks, points);

	if (!isSnapshot) {
		// TODO review this
		_tileLoader.m_totalCount = points.size() + activeTasks.size();
	}

	// delete unused tiles from the screen buffer
	DeleteMarkedTilesFromBuffer();

	UnlockDiskCache();

	TileLoader* loader = isSnapshot ? &_prefetchLoader : &_tileLoader;

	loader->RunCaching();

	if (points.size() > 0)
	{
		tilesLogger.WriteLine("Queued to load from server: %d", points.size());

		// zoom can change in the process, so we use the calculated version and not the one current for provider
		loader->Load(points, zoom, provider, (void*)this, isSnapshot, key, generation);
	}
	else
	{
		// tilesLogger.WriteLine("Tiles loaded event; Were loaded from server (y/n): %d", !nothingToLoad);
		FireTilesLoaded(isSnapshot, key);
	}

	ReleaseMemory(activeTasks);
	ReleaseMemory(points);
}

// *********************************************************
//	     GetTileIndices()
// *********************************************************
bool TileManager::GetTileIndices(BaseProvider* provider, CRect& indices, int& zoom)
{
	Extent* mapExtents = _map->_GetExtents();

	if (_lastMapExtents == *mapExtents && _lastProvider == provider->Id)
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
void TileManager::BuildLoadingList(BaseProvider* provider, CRect indices, int zoom, vector<CTilePoint*>& activeTasks, vector<CTilePoint*>& points)
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
				if (tile->m_tileX == x && tile->m_tileY == y  && tile->m_scale == zoom && tile->m_providerId == provider->Id)
				{
					tile->m_toDelete = false;
					tile->m_inBuffer = true;
					found = true;
					break;
				}
			}

			_tilesBufferLock.Unlock();

			if (found)
				continue;

			if (_useRamCache)
			{
				TileCore* tile = RamCache::get_Tile(provider->Id, zoom, x, y);
				if (tile)
				{
					AddTileNoCaching(tile);
					continue;
				}
			}

			if (_useDiskCache)
			{
				TileCore* tile = SQLiteCache::get_Tile(provider, zoom, x, y);
				if (tile)
				{
					AddTileNoCaching(tile);
					continue;
				}
			}

			// if the tile isn't present in both caches, request it from the server
			if (_useServer)
			{
				CTilePoint* pnt = new CTilePoint(x, y);
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
		_tiles[i]->m_drawn = false;
		_tiles[i]->m_inBuffer = false;
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
//	     ReleaseMemory()
// *********************************************************
void TileManager::ReleaseMemory(vector<CTilePoint*> points)
{
	for (size_t i = 0; i < points.size(); i++) {
		delete points[i];
	}
}

// *********************************************************
//	     UnlockDiskCache()
// *********************************************************
void TileManager::UnlockDiskCache()
{
	if (_useDiskCache)
	{
		// caching will be stopped while loading tiles to avoid locking the database and speed up things
		SQLiteCache::m_locked = false;
	}
}

// *********************************************************
//	     InitializeDiskCache()
// *********************************************************
void TileManager::InitializeDiskCache()
{
	if (_doDiskCaching)  {
		SQLiteCache::Initialize(SqliteOpenMode::OpenOrCreate);
	}

	if (_useDiskCache)	{
		SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists);
		SQLiteCache::m_locked = true;	// caching will be stopped while loading tiles to avoid locking the database
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
		if (tile->m_toDelete)
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
		_tiles[i]->m_drawn = false;
		_tiles[i]->m_inBuffer = false;
		_tiles[i]->m_toDelete = true;
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
void TileManager::GetActiveTasks(vector<CTilePoint*>& activeTasks, int providerId, int zoom, int generation, CRect indices)
{
	// lock it, so active task can't be removed while we analyze it here
	_tileLoader.LockActiveTasks(true);

	std::list<void*> list = _tileLoader.GetActiveTasks();
	std::list<void*>::iterator it = list.begin();

	while (it != list.end())
	{
		LoadingTask* task = (LoadingTask*)*it;
		if (task->Provider->Id == providerId &&
			task->zoom() == zoom &&
			task->x() >= indices.left &&
			task->x() <= indices.right &&
			task->y() >= indices.bottom &&
			task->y() <= indices.top)
		{
			tilesLogger.WriteLine("Tile reassigned to current generation: %d\\%d\\%d", zoom, task->x(), task->y());

			task->generation(generation);								// reassign it to current generation
			activeTasks.push_back(new CTilePoint(task->x(), task->y()));    // don't include in current list of requests
		}

		++it;
	}

	_tileLoader.m_count = 0;
	_tileLoader.m_totalCount = 1000;		// set it to a big number until we count them all
	_tileLoader.LockActiveTasks(false);
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
	tile->m_inBuffer = true;
	tile->m_toDelete = false;
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
	if (_doDiskCaching) {
		_tileLoader.ScheduleForCaching(tile);
	}
}

// *********************************************************
//	     AddTileToRamCache()
// *********************************************************
void TileManager::AddTileToRamCache(TileCore* tile) 
{
	if (_doRamCaching) {
		RamCache::AddToCache(tile);
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
		if (_tiles[i]->m_tileX == x &&
			_tiles[i]->m_tileY == y &&
			_tiles[i]->m_scale == zoom &&
			_tiles[i]->m_providerId == providerId)
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
		_tiles[i]->m_drawn = false;
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
		if (!_tiles[i]->m_drawn) {
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
		if (_tiles[i]->m_drawn) {
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
}
