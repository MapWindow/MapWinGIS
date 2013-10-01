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
	}
	else
	{
		TileCore* tile = Provider->GetTileImage(CPoint(x, y), zoom);		// http call

		if (this->Loader->stopped)
		{
			delete tile;	// requesting from a server takes time; probably the task was already aborted
		}
		else 
		{
			this->Loader->m_count++;

			// prefetching without display
			if (this->cacheOnly)
			{
				this->Loader->TileLoaded(tile);
			}
			else
			{
				this->busy = true;	// notifies that related classes like CTiles can't be deleted until exiting this section
				CMapView* mapView = (CMapView*)Provider->mapView;
				CTiles* tiles = (CTiles*)mapView->GetTilesNoRef();
				
				if (this->generation < this->Loader->tileGeneration)
				{
					tiles->AddTileOnlyCaching(tile);
					Debug::WriteLine("Outdated tile loading: tile cached");
				}
				else
				{
					tiles->AddTileWithCaching(tile);
					mapView->Invalidate();			// schedule map updated
					this->Loader->RunCaching();		// if there is no pending tasks, the caching will be started		
				}
				this->busy = false;
			}

			this->Loader->CheckComplete();
		}
	}
	this->completed = true;
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
void TileLoader::Load(std::vector<CTilePoint*> &points, int zoom, BaseProvider* provider, void* tiles, bool isSnapshot, CString key, bool cacheOnly)
{
	this->key = key;
	this->isSnapshot = isSnapshot;
	this->tiles = tiles;
	this->tileGeneration++;
	int generation = this->tileGeneration;
	
	if (!InitPools())
		return;

	CThreadPool<ThreadWorker>* pool = (this->tileGeneration % 2 == 0)  ? m_pool : m_pool2;
	
	Debug::WriteLine("Tiles requested; generation = %d", generation);	

	m_count = 0;
	m_totalCount = points.size();

	this->CleanTasks();

	sort(points.begin(), points.end(), &compPoints);
	for (size_t i = 0; i < points.size(); i++ ) 
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
		((CTiles*)this->tiles)->HandleOnTilesLoaded(this->isSnapshot, this->key);
	}
}

// *******************************************************
//		TileLoaded()
// *******************************************************
//	Reports progress to clients, aborts the operation
void TileLoader::TileLoaded(TileCore* tile)
{	
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
	this->ScheduleForCaching(tile);
	this->RunCaching();
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
			it++;
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
			it++;
		}
	}
}







