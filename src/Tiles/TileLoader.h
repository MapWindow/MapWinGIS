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
#include <list>
#include "baseprovider.h"
#include "tilecacher.h"
#include "TileCacheManager.h"
#include "ITileCache.h"

#define THREADPOOL_SIZE	5
class TileLoader;

// ******************************************************
//    TilePoint
// ******************************************************
// A point with X, Y coordinates and distance from center of screen
class TilePoint: public CPoint
{
public:
	TilePoint(int x, int y)
		: CPoint(x, y), dist(0.0)
	{
	}

public:
	double dist;

public:
	static void ReleaseMemory(vector<TilePoint*>& points)
	{
		for (size_t i = 0; i < points.size(); i++) {
			delete points[i];
		}
	}
};

bool compPoints(TilePoint* p1, TilePoint* p2);

// ******************************************************
//    TileLoaderFactory()
// ******************************************************
class TileLoaderFactory
{
private:
	static vector<TileLoader*> _loaders;
	static ::CCriticalSection _lock;
public:
	static TileLoader* Create(CacheType type);
	static void Clear();
};

// ******************************************************
//    TileLoader()
// ******************************************************
// Handles the loading queue of map tiles, schedules the loaded tiles for caching
class TileLoader
{
public:
	// TODO: how to disable caching?
	TileLoader::TileLoader(CacheType cacheType)
		: _pool(NULL), _pool2(NULL), _callback(NULL), _stopCallback(NULL)
	{
		ITileCache* cache = TileCacheManager::get_Cache(cacheType);
		_cacher = new TileCacher(cache);

		_tileGeneration = 0;
	
		_stopped = false;
		_isSnapshot = false;
		_errorCount = 0;
		_sumCount = 0;
		_sleepBeforeRequestTimeout = 0;
		_totalCount = 0;
		_count = 0;
	}

	virtual ~TileLoader(void)
	{
		CleanTasks();

		if (_pool != NULL) {
			_pool->Shutdown(50);   // will result in TerminateThread call after 50ms delay
			delete _pool;
		}

		if (_pool2 != NULL) {
			_pool2->Shutdown(50);  // will result in TerminateThread call after 50ms delay
			delete _pool2;
		}

		delete _cacher;
	}

private:
	// 2 pools will be used in alternate fashion,
	// as it's problematic to terminate threads correctly until their current task is finished
	// but it's quite desirable to start the processing of new requests ASAP
	CThreadPool<ThreadWorker>* _pool;
	CThreadPool<ThreadWorker>* _pool2;
	list<void*> _tasks;			// all the scheduled tasks
	list<void*> _activeTasks;	// HTTP requests being currently performed
	::CCriticalSection _activeTasksLock;
	TileCacher* _cacher;
	bool _isSnapshot;
	CString _key;
	bool _stopped;
	bool _doCaching;
	int _tileGeneration;
	long _sleepBeforeRequestTimeout;
	int _totalCount;
	int _count;

	// caching only
	ICallback* _callback;			 // to report progress to clients via COM interface
	IStopExecution* _stopCallback;	 // to stop execution by clients via COM interface
	int _errorCount;
	int _sumCount;                   // sums all requests even if generation doesn't match
	static ::CCriticalSection _callbackLock;

private:
	void CleanTasks();

public:
	// properties
	bool get_isSnapShot() { return _isSnapshot; }
	CString get_Key() { return _key; }
	std::list<void*> GetActiveTasks() { return _activeTasks; }
	bool isStopped() { return _stopped; }
	void set_Callback(ICallback* callback) { _callback = callback; }
	void set_StopCallback(IStopExecution* callback) { _stopCallback = callback; }
	int get_TileGeneration() { return _tileGeneration; }
	int NextGeneration() { return ++_tileGeneration; }
	int get_ErrorCount() { return _errorCount; }
	void set_TotalCount(int value) { _totalCount = value; }
	int get_SumCount() { return _sumCount; }
	long get_SleepBeforeRequestTimeout() { return _sleepBeforeRequestTimeout; }
	void set_SleepBeforeRequestTimeout(long value) { _sleepBeforeRequestTimeout = value; }

public:
	//methods
	void Load(vector<TilePoint*> &points, BaseProvider* provider, int zoom, bool isSnaphot, CString key, int generation, bool cacheOnly = false);
	void LockActiveTasks(bool lock);
	void AddActiveTask(void* task);
	void RemoveActiveTask(void* task);
	bool HasActiveTask(void* task);
	void StopCaching();
	void ScheduleForCaching(TileCore* tile);
	void RunCaching();
	bool InitPools();
	void Stop();
	void TileLoaded(TileCore* tile);
	bool IsCompleted() { return _count == _totalCount; }
	void IncrementCount() { _count++; }
	void ResetCount() { _count = 0; };
	void ResetErrorCount() { _errorCount = 0; _sumCount = 0; }
};

