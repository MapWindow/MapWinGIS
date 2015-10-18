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
#include "ITileLoader.h"
#include "ILoadingTask.h"
#include "TileBulkLoader.h"
#include "TileMapLoader.h"

::CCriticalSection TileLoaderFactory::_lock;
vector<ITileLoader*> TileLoaderFactory::_loaders;

// *******************************************************
//		Create()
// *******************************************************
ITileLoader* TileLoaderFactory::Create(CacheType type, TileLoaderType loaderType)
{
	ITileLoader* loader = NULL;

	if (loaderType == tltBulkLoader)
	{
		ITileCache* cache = TileCacheManager::get_Cache(type);
		loader = new TileBulkLoader(cache);
	}
	else if (loaderType == tltMapLoader)
	{
		loader = new TileMapLoader(type);
	}

	_lock.Lock();
	_loaders.push_back(loader);
	_lock.Unlock();

	return loader;
}

// *******************************************************
//		Clear()
// *******************************************************
void TileLoaderFactory::Clear()
{
	_lock.Lock();

	for (size_t i = 0; i < _loaders.size(); i++)
	{
		delete _loaders[i];
	}

	_loaders.clear();

	_lock.Unlock();
}

// *******************************************************
//		InitPools()
// *******************************************************
bool ITileLoader::InitPools()
{
	_stopped = false;

	int size = m_globalSettings.GetTilesThreadPoolSize();

	if (!_pool)
	{
		_pool = new CThreadPool<ThreadWorker>();
		HRESULT hr = _pool->Initialize(NULL, size);
		if (!SUCCEEDED( hr ))
			CallbackHelper::ErrorMsg("Failed to initialize thread pool (1) for tile loading.");
	}

	if (!_pool2)
	{
		_pool2 = new CThreadPool<ThreadWorker>();
		HRESULT hr = _pool2->Initialize(NULL, size);
		if(!SUCCEEDED( hr ))
			CallbackHelper::ErrorMsg("Failed to initialize thread pool (2) for tile loading.");
	}

	return _pool && _pool2;
}

// *******************************************************
//		PreparePool()
// *******************************************************
CThreadPool<ThreadWorker>* ITileLoader::PreparePool()
{
	if (!InitPools())
		return NULL;

	CThreadPool<ThreadWorker>* pool = (_lastGeneration % 2 == 0) ? _pool : _pool2;

	pool->SetSize(m_globalSettings.GetTilesThreadPoolSize());

	pool->SetTimeout(100000);		// 100 seconds (lower rate limit may be set)

	tilesLogger.WriteLine("Tiles requested; generation = %d", _lastGeneration);

	CleanTasks();

	CleanRequests();

	return pool;
}

// *******************************************************
//		Load()
// *******************************************************
//	Creates tasks for tile points and enqueues them in thread pool
void ITileLoader::Load(std::vector<TilePoint*> &points, BaseProvider* provider, int zoom, TileRequestInfo* requestInfo)
{
	CThreadPool<ThreadWorker>* pool = PreparePool();
	if (!pool) return;

	sort(points.begin(), points.end(), [](TilePoint* p1, TilePoint* p2) { return p1->dist < p2->dist; } );

	for ( size_t i = 0; i < points.size(); i++ ) 
	{
		ILoadingTask* task = CreateTask(points[i]->x, points[i]->y, zoom, provider, _lastGeneration);
		task->set_Loader(this);
		_tasks.push_back(task);
		pool->QueueRequest( (ThreadWorker::RequestType) task );
	}
}

// *******************************************************
//	   Stop()
// *******************************************************
void ITileLoader::Stop()
{
	// This one is called from map destructor. Don't wait for tasks to finish 
	// as it deters the closing of form or application. 
	// Don't bother with releasing memory for unfinished tasks as 
	// they use around 30 bytes of memory, so just let it leak.

	_stopped = true;

	CleanTasks();
}

// *******************************************************
//		CleanTasks()
// *******************************************************
// Cleans completed tasks
void ITileLoader::CleanTasks()
{
	list<void*>::iterator it = _tasks.begin();
	while (it != _tasks.end())
	{
		ILoadingTask* task = (ILoadingTask*)*it;
		if (task->completed())
		{
			delete task;
			it = _tasks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// *******************************************************
//		ClearRequests()
// *******************************************************
void ITileLoader::CleanRequests()
{
	_requestLock.Lock();

	int size = static_cast<int>(_requests.size());

	// remove all requests but the last one
	list<TileRequestInfo*>::iterator it = _requests.begin();
	while (it != _requests.end())
	{
		TileRequestInfo* info = *it;
		if (info->generation < _lastGeneration) 
		{
			delete info;
			it = _requests.erase(it);
		}
		else 
		{
			++it;
		}
	}
}

// *******************************************************
//		RegisterTile()
// *******************************************************
void ITileLoader::RegisterTile(int generation)
{
	TileRequestInfo* info = FindRequest(generation);
	if (info)
	{
		info->count++;
	}
}

// *******************************************************
//		FindRequest()
// *******************************************************
TileRequestInfo* ITileLoader::FindRequest(int generation)
{
	CSingleLock lock(&_requestLock, TRUE);

	list<TileRequestInfo*>::iterator it = find_if(_requests.begin(), _requests.end(),
						[&](TileRequestInfo* info){ return info->generation == generation; });

	if (it != _requests.end())
	{
		return *it;
	}

	return NULL;
}

// *******************************************************
//		CreateNextRequest()
// *******************************************************
TileRequestInfo* ITileLoader::CreateNextRequest(CString key, bool isSnapshot)
{
	TileRequestInfo* info = new TileRequestInfo(key, isSnapshot);
	info->generation = ++_lastGeneration;
	
	_requestLock.Lock();
	_requests.push_back(info);
	_requestLock.Unlock();

	return info;
}
