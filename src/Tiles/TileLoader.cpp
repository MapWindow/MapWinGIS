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

#include "stdafx.h"
#include "TileLoader.h"
#include "SqliteCache.h"
#include "LoadingTask.h"
#include "Tiles.h"

::CCriticalSection TileLoader::section;
::CCriticalSection TileLoader::_requestLock;

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
	this->stopped = false;

	int size = m_globalSettings.GetTilesThreadPoolSize();

	if (!m_pool)
	{
		m_pool = new CThreadPool<ThreadWorker>();
		HRESULT hr = m_pool->Initialize(NULL, size);
		if (!SUCCEEDED( hr ))
			CallbackHelper::ErrorMsg("Failed to initialize thread pool (1) for tile loading.");
	}

	if (!m_pool2)
	{
		m_pool2 = new CThreadPool<ThreadWorker>();
		HRESULT hr = m_pool2->Initialize(NULL, size);
		if(!SUCCEEDED( hr ))
			CallbackHelper::ErrorMsg("Failed to initialize thread pool (2) for tile loading.");
	}
	return m_pool && m_pool2;
}

// *******************************************************
//		Load()
// *******************************************************
//	Creates tasks for tile points and enqueues them in thread pool
void TileLoader::Load(std::vector<CTilePoint*> &points, int zoom, BaseProvider* provider, void* tiles, 
					  bool isSnapshot, CString key, int generation, bool cacheOnly)
{
	this->key = key;
	this->isSnapshot = isSnapshot;
	this->tiles = tiles;
	if (isSnapshot) {
		this->tileGeneration++;
		generation = this->tileGeneration;
	}
	
	if (!InitPools())
		return;

	CThreadPool<ThreadWorker>* pool = (generation % 2 == 0)  ? m_pool : m_pool2;

	pool->SetSize(m_globalSettings.GetTilesThreadPoolSize());

	pool->SetTimeout(100000);		// 100 seconds (low rate limit may be set)

	tilesLogger.WriteLine("Tiles requested; generation = %d", generation);

	if (isSnapshot || cacheOnly) {
		m_count = 0;
		m_totalCount = points.size();
	}

	this->CleanTasks();

	sort(points.begin(), points.end(), &compPoints);
	for ( size_t i = 0; i < points.size(); i++ ) 
	{
		LoadingTask* task = new LoadingTask(points[i]->x, points[i]->y, zoom, provider, generation, cacheOnly);
		task->Loader = this;
		m_tasks.push_back(task);
		pool->QueueRequest( (ThreadWorker::RequestType) task );
	}
}
bool compPoints(CTilePoint* p1, CTilePoint* p2)
{
	return p1->dist < p2->dist;
}

// *******************************************************
//		CheckComplete()
// *******************************************************
void TileLoader::CheckComplete()
{
	if (this->tiles != NULL && m_count == m_totalCount)
	{
		((CTiles*)this->tiles)->HandleOnTilesLoaded(this->isSnapshot, this->key, false);
	}
}

// *******************************************************
//		TileLoaded()
// *******************************************************
//	Reports progress to clients, aborts the operation
void TileLoader::TileLoaded(TileCore* tile)
{	
	this->m_sumCount++;
	if (tile->m_hasErrors)
	{
		this->m_errorCount++;
	}

	if (m_callback != NULL)
	{
		section.Lock();
		USES_CONVERSION;
		CallbackHelper::Progress(m_callback, this->m_count >= this->m_totalCount ? -1 : this->m_count, "Caching...");
		section.Unlock();
	}

	if (m_stopCallback)
	{
		VARIANT_BOOL stop;
		m_stopCallback->StopFunction(&stop);
		if (stop && !this->stopped)
		{
			this->stopped = true;
			CallbackHelper::Progress(m_callback, -2, "Caching...");
		}
	}
	
	if (!tile->IsEmpty())
	{
		this->ScheduleForCaching(tile);
		this->RunCaching();
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

	this->stopped = true;

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

	this->StopCaching();
	
	this->CleanTasks();
}

// *******************************************************
//		CleanTasks()
// *******************************************************
// Cleans completed tasks
void TileLoader::CleanTasks()
{
	list<void*>::iterator it = m_tasks.begin();
	while (it != m_tasks.end())
	{
		LoadingTask* task = (LoadingTask*)*it;
		if (task->completed)
		{
			delete task;
			it = m_tasks.erase(it);
		}
		else
		{
			++it;
		}
	}
}







