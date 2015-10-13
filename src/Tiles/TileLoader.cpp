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
#include "TileLoader.h"
#include "SqliteCache.h"
#include "LoadingTask.h"
#include "Tiles.h"

::CCriticalSection TileLoader::_callbackLock;

// *******************************************************
//		LockActiveTasks()
// *******************************************************
void TileLoader::LockActiveTasks(bool lock)
{
	if (lock)
		_activeTasksLock.Lock();
	else
		_activeTasksLock.Unlock();
}

// *******************************************************
//		AddActiveTask()
// *******************************************************
void TileLoader::AddActiveTask(void* task)
{
	_activeTasksLock.Lock();
	_activeTasks.push_back(task);
	_activeTasksLock.Unlock();
}

// *******************************************************
//		RemoveActiveTask()
// *******************************************************
void TileLoader::RemoveActiveTask(void* t)
{
	_activeTasksLock.Lock();
	LoadingTask* task = (LoadingTask*)t;
	list<void*>::iterator it = _activeTasks.begin();
	while (it != _activeTasks.end())
	{
		LoadingTask* item = (LoadingTask*)*it;
		if (item->Compare(task))
		{
			_activeTasks.remove(item);
			break;
		}
		++it;
	}
	_activeTasksLock.Unlock();
}

// *******************************************************
//		HasActiveTask()
// *******************************************************
bool TileLoader::HasActiveTask(void* t)
{
	_activeTasksLock.Lock();
	LoadingTask* task = (LoadingTask*)t;
	list<void*>::iterator it = _activeTasks.begin();
	bool found = false;
	while (it != _activeTasks.end())
	{
		LoadingTask* item = (LoadingTask*)*it;
		if (item->Compare(task))
		{
			found = true;
			break;
		}
		++it;
	}
	_activeTasksLock.Unlock();
	return found;
}

// *******************************************************
//		InitPools()
// *******************************************************
bool TileLoader::InitPools()
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
//		Load()
// *******************************************************
//	Creates tasks for tile points and enqueues them in thread pool
void TileLoader::Load(std::vector<CTilePoint*> &points, BaseProvider* provider, int zoom,
					  bool isSnapshot, CString key, int generation, bool cacheOnly)
{
	_key = key;
	_isSnapshot = isSnapshot;
	if (isSnapshot) {
		_tileGeneration++;
		generation = _tileGeneration;
	}
	
	if (!InitPools())
		return;

	CThreadPool<ThreadWorker>* pool = (generation % 2 == 0)  ? _pool : _pool2;

	pool->SetSize(m_globalSettings.GetTilesThreadPoolSize());

	pool->SetTimeout(100000);		// 100 seconds (low rate limit may be set)

	tilesLogger.WriteLine("Tiles requested; generation = %d", generation);

	if (isSnapshot || cacheOnly) {
		_count = 0;
		_totalCount = points.size();
	}

	CleanTasks();

	sort(points.begin(), points.end(), &compPoints);
	for ( size_t i = 0; i < points.size(); i++ ) 
	{
		LoadingTask* task = new LoadingTask(points[i]->x, points[i]->y, zoom, provider, generation, cacheOnly);
		task->Loader = this;
		_tasks.push_back(task);
		pool->QueueRequest( (ThreadWorker::RequestType) task );
	}
}
bool compPoints(CTilePoint* p1, CTilePoint* p2)
{
	return p1->dist < p2->dist;
}

// *******************************************************
//		TileLoaded()
// *******************************************************
//	Reports progress to clients, aborts the operation
void TileLoader::TileLoaded(TileCore* tile)
{	
	_sumCount++;

	if (tile->m_hasErrors)
	{
		_errorCount++;
	}

	if (_callback != NULL)
	{
		_callbackLock.Lock();
		CallbackHelper::Progress(_callback, _count >= _totalCount ? -1 : _count, "Caching...");
		_callbackLock.Unlock();
	}

	if (_stopCallback)
	{
		VARIANT_BOOL stop;
		_stopCallback->StopFunction(&stop);
		if (stop && !_stopped)
		{
			_stopped = true;
			CallbackHelper::Progress(_callback, -2, "Caching...");
		}
	}
	
	if (!tile->IsEmpty())
	{
		ScheduleForCaching(tile);
		RunCaching();
	}
}

// *******************************************************
//	   Stop()
// *******************************************************
void TileLoader::Stop()
{
	// This one is called from map destructor. Don't wait for tasks to finish 
	// as it deters the closing of form or application. 
	// Don't bother with releasing memory for unfinished tasks as 
	// they use around 30 bytes of memory, so just let it leak.

	_stopped = true;

	/// notify the provider
	/*list<void*>::iterator it = m_tasks.begin();
	while (it != m_tasks.end())
	{
		LoadingTask* task = (LoadingTask*)*it;
		if (task) {
			task->Provider->IsStopped = true;
		}
		++it;
	}*/

	StopCaching();

	CleanTasks();
}

// *******************************************************
//		CleanTasks()
// *******************************************************
// Cleans completed tasks
void TileLoader::CleanTasks()
{
	list<void*>::iterator it = _tasks.begin();
	while (it != _tasks.end())
	{
		LoadingTask* task = (LoadingTask*)*it;
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
//		StopCaching()
// *******************************************************
void TileLoader::StopCaching()
{
	_diskCacher.stopped = true;
	_sqliteCacher.stopped = true;
}

// *******************************************************
//		ScheduleForCaching()
// *******************************************************
void TileLoader::ScheduleForCaching(TileCore* tile)
{
	if (tile)
	{
		if (_doCacheSqlite)
		{
			tile->AddRef();
			_sqliteCacher.Enqueue(tile);
		}
		if (_doCacheDisk)
		{
			tile->AddRef();
			_diskCacher.Enqueue(tile);
		}
	}
}

// *******************************************************
//		ScheduleForCaching()
// *******************************************************
void TileLoader::RunCaching()
{
	if (_doCacheSqlite)
	{
		_sqliteCacher.Run();
	}
	if (_doCacheDisk)
	{
		_diskCacher.Run();
	}
}






