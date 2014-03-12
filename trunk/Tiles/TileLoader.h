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

#pragma once
#include <atlutil.h>
#include <algorithm>
#include <list>
#include "baseprovider.h"
#include "Threading.h"
#include "SQLiteCache.h"

#define THREADPOOL_SIZE	5

// A point with X, Y coordinates and distance from center of screen
class CTilePoint: public CPoint
{
public:
	double dist;
	CTilePoint(int x, int y)
		: CPoint(x, y), dist(0.0) {}
};
bool compPoints(CTilePoint* p1, CTilePoint* p2);

// Handles the loading queue of map tiles, schedules the loaded tiles for caching
class TileLoader
{
private:
	// 2 pools will be used in alternate fashion,
	// as it's problematic to terminate threads correctly until their current task is finished
	// but it's quite desirable to start the processing of new requests ASAP
	CThreadPool<ThreadWorker>* m_pool;
	CThreadPool<ThreadWorker>* m_pool2;
	list<void*> m_tasks;
	TileCacher m_sqliteCacher;
	TileCacher m_diskCacher;
	void* tiles;
	bool isSnapshot;
	CString key;
	void CleanTasks();
public:
	long m_sleepBeforeRequestTimeout;
	int m_errorCount;
	int m_totalCount;
	int m_sumCount;
	int m_count;
	static ::CCriticalSection section;

	ICallback* m_callback;			 // to report progress to clients via COM interface
	IStopExecution* m_stopCallback;	 // to stop execution by clents via COM interface
	int tileGeneration;
	bool stopped;
	bool doCacheSqlite;
	bool doCacheDisk;

	TileLoader::TileLoader()
	{
		m_diskCacher.cacheType = CacheType::DiskCache;
		m_sqliteCacher.cacheType = CacheType::SqliteCache;
		tileGeneration = 0;
		m_pool = NULL;
		m_pool2 = NULL;
		m_callback = NULL;
		m_stopCallback = NULL;
		stopped = false;
		doCacheSqlite = true;
		doCacheDisk = false;
		tiles = NULL;
		m_errorCount = 0;
		m_sumCount = 0;
		//m_requestWindow = 20000;			// TODO: make parameters
		//m_maxRequestCount = 10;
		m_sleepBeforeRequestTimeout = 0;
	}

	TileLoader::~TileLoader(void)
	{
		Debug::WriteLine("Tile loader destructor");
		
		CleanTasks();

		if (m_pool != NULL) {
			m_pool->Shutdown();
			delete m_pool;
		}
		if (m_pool2 != NULL) {
			m_pool2->Shutdown();
			delete m_pool2;
		}
	}

#pragma region Request limit
	//std::deque<long> m_requestTimes;
	//int m_requestWindow;		// the length of period
	//int m_maxRequestCount;		// the max number of requests in window

	//long TryRequest()
	//{
	//	section.Lock();
	//	DWORD now = GetTickCount();
	//	int count = 0;
	//	long waitTime = 0;
	//	long time;
	//	for(size_t i = 0; i < m_requestTimes.size(); i++)		// probably should go in reverse order
	//	{
	//		time = m_requestTimes[i];
	//		Debug::WriteLine("Time: %d", time);
	//		if (time + m_requestWindow > now)
	//			count++;
	//		else
	//			break;	// outside the window
	//		if (count == m_maxRequestCount)
	//			waitTime = m_requestWindow - (now - time);
	//		if (count > m_maxRequestCount)
	//			break;
	//	}

	//	section.Lock();
	//	bool result = count < m_maxRequestCount;
	//	if (result) 
	//	{
	//		RegisterRequest();
	//		Debug::WriteLine("Can request", waitTime);
	//	}
	//	else
	//	{
	//		Debug::WriteLine("Request limit met. Sleeping: %d", waitTime);
	//	}

	//	section.Unlock();
	//	return result ? -1 : waitTime;
	//}

	//void RegisterRequest()
	//{
	//	DWORD now = GetTickCount();
	//	m_requestTimes.push_front(now);
	//	if (m_requestTimes.size() > 50)
	//		m_requestTimes.pop_back();
	//}
#pragma endregion

	void StopCaching()
	{
		m_diskCacher.stopped = true;
		m_sqliteCacher.stopped = true;
	}

	void ScheduleForCaching(TileCore* tile)
	{
		if (tile)
		{		
			if (doCacheSqlite) 
			{
				tile->AddRef();
				m_sqliteCacher.Enqueue(tile);
			}
			if (doCacheDisk)
			{
				tile->AddRef();
				m_diskCacher.Enqueue(tile);
			}
		}
	}
	void RunCaching()
	{
		if (doCacheSqlite) 
		{
			m_sqliteCacher.Run();
		}
		if (doCacheDisk)
		{
			m_diskCacher.Run();
		}
	}
	void Load(std::vector<CTilePoint*> &points, int zoom, BaseProvider* provider, void* tiles, bool isSnaphot, CString key, bool cacheOnly = false);
	bool InitPools();
	void Stop();
	void TileLoaded(TileCore* tile);
	void CheckComplete();
};

// Represents a single loading task (a single tile to load)
class LoadingTask: ITask
{
public:
	int x;
	int y;
	int zoom;
	int generation;
	BaseProvider* Provider;
	bool cacheOnly;
	bool completed;
	bool busy;
	TileLoader* Loader;

	LoadingTask(int x, int y, int zoom, BaseProvider* provider, int generation, bool cacheOnly)
		: x(x), y(y), zoom(zoom), cacheOnly(cacheOnly)
	{
		this->busy = false;
		this->completed = false;
		this->Provider = provider;
		this->generation = generation;
	}

	void DoTask();
};



