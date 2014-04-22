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
#include "tiles.h"
#include "map.h"

::CCriticalSection TileLoader::section;

// *******************************************************
//		LoadingTask::DoTask()
// *******************************************************
void LoadingTask::DoTask()
{
	ASSERT(this->Loader);

	if (this->Loader->stopped)
	{
		// do nothing; the task was aborted
	}
	else if (this->generation < this->Loader->tileGeneration)
	{
		// this tile is no longer needed; another set of tiles was requested		
		//Debug::WriteLine("Outdated tile loading: canceled");
		tilesLogger.WriteLine("Outdated tile loading; cancelled: %d\\%d\\%d", this->zoom, this->x, this->y);
	}
	else
	{
		long timeout = this->Loader->m_sleepBeforeRequestTimeout;
		if (timeout > 0 && timeout < 10000) Sleep(timeout);

		if (!this->cacheOnly)
			this->Loader->AddActiveTask(this);
		
		TileCore* tile = Provider->GetTileImage(CPoint(x, y), zoom);		// http call

		if (this->Loader->stopped)
		{
			delete tile;	// requesting from a server takes time; probably the task was already aborted
			if (!this->cacheOnly)
				this->Loader->RemoveActiveTask(this);
		}
		else 
		{
			if (this->generation == this->Loader->tileGeneration)
				this->Loader->m_count++;
			
			// prefetching without display
			if (this->cacheOnly)
			{
				this->Loader->TileLoaded(tile);
			}
			else
			{
				if (!tile->IsEmpty())
				{
					this->busy = true;	// notifies that related classes like CTiles can't be deleted until exiting this section
					CMapView* mapView = (CMapView*)Provider->mapView;
					CTiles* tiles = (CTiles*)mapView->GetTilesNoRef();
					
					// quickly pass it from active list to cache; so it's always avaiable while building the next list of requests
					this->Loader->LockActiveTasks(true);
					tiles->AddTileToRamCache(tile);
					this->Loader->RemoveActiveTask(this);
					this->Loader->LockActiveTasks(false);

					if (this->generation < this->Loader->tileGeneration)
					{
						tiles->AddTileOnlyCaching(tile);      
						tilesLogger.WriteLine("Outdated tile; cached: %d\\%d\\%d", this->zoom, this->x, this->y);
					}
					else
					{
						tiles->AddTileWithCaching(tile);
						mapView->RedrawCore(RedrawSkipDataLayers, false, false);
						this->Loader->RunCaching();		// if there is no pending tasks, the caching will be started		
					}
					this->busy = false;
				}
				else {
					this->Loader->RemoveActiveTask(this);
				}
			}

			// check generation to avoid firing the event several times when outdated times are loaded
			if(this->generation == this->Loader->tileGeneration) {
				this->Loader->CheckComplete();		
			}
		}
	}
	
	// let's try to duplicate this, as sometimes tiles still remain in the list when completed flag is set
	this->Loader->RemoveActiveTask(this);	
	this->completed = true;
}

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

	if (!m_pool)
	{
		m_pool = new CThreadPool<ThreadWorker>();
		HRESULT hr = m_pool->Initialize(NULL, THREADPOOL_SIZE);
		if (!SUCCEEDED( hr ))
			Debug::WriteLine("Failed to initialize pool 1");
	}

	if (!m_pool2)
	{
		m_pool2 = new CThreadPool<ThreadWorker>();
		HRESULT hr = m_pool2->Initialize(NULL, THREADPOOL_SIZE);
		if(!SUCCEEDED( hr ))
			Debug::WriteLine("Failed to initialize pool 2");
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

	pool->SetTimeout(100000);		// 100 seconds (low rate limit may be set)

	//Debug::WriteLine();	
	tilesLogger.WriteLine("Tiles requested; generation = %d", generation);

	if (isSnapshot) {
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
		m_callback->Progress(A2BSTR(""), this->m_count >= this->m_totalCount ? -1 : this->m_count, A2BSTR("Caching..."));
		section.Unlock();
	}

	if (m_stopCallback)
	{
		VARIANT_BOOL stop;
		m_stopCallback->StopFunction(&stop);
		if (stop && !this->stopped)
		{
			this->stopped = true;
			m_callback->Progress(A2BSTR(""),-2,A2BSTR("Caching..."));
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
	this->stopped = true;
	
	this->StopCaching();

	bool busy;
	do		// WARNING: potentially endless loop
	{
		busy = false;
		list<void*>::iterator it = m_tasks.begin();
		while (it != m_tasks.end())
		{
			LoadingTask* task = (LoadingTask*)*it;
			if (task->busy)
			{
				busy = true;
				break;
			}
			++it;
		}
	} while (busy);

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







