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
#include <algorithm>
#include "baseprovider.h"
#include "tilecacher.h"
#include "TileCacheManager.h"
#include "ITileCache.h"
#include "ILoadingTask.h"

#define THREADPOOL_SIZE	5
class ITileLoader;
class ILoadingTask;

// ******************************************************
//    TileLoader
// ******************************************************
// Handles the loading queue of map tiles, schedules the loaded tiles for caching
// Base class for tile loaders.
class ITileLoader
{
public:
	ITileLoader()
		: _pool(NULL), _pool2(NULL), _stopped(false)
	{
		_delayRequestTimeout = 0;
		_lastGeneration = 0;
	}

	virtual ~ITileLoader(void)
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
	}

protected:
	// 2 pools will be used in alternate fashion,
	// as it's problematic to terminate threads correctly until their current task is finished
	// but it's quite desirable to start the processing of new requests ASAP
	CThreadPool<ThreadWorker>* _pool;
	CThreadPool<ThreadWorker>* _pool2;
	list<void*> _tasks;			// all the scheduled tasks
	list<TileRequestInfo*> _requests;
	::CCriticalSection _requestLock;
	long _delayRequestTimeout;
	bool _stopped;
	int _lastGeneration;   // the most recent generation

private:
	void CleanTasks();
	bool InitPools();
	CThreadPool<ThreadWorker>* PreparePool();
	void CleanRequests();

public:
	// properties
	virtual bool IsOutdated(int generation) { return _stopped; } 
	bool isStopped() { return _stopped; }
	int get_LastGeneration() { return _lastGeneration; }
	TileRequestInfo* CreateNextRequest(CString key, bool isSnapshot);
	long get_DelayRequestTimeout() { return _delayRequestTimeout; }
	void set_DelayRequestTimeout(long value) { _delayRequestTimeout = value; }

public:
	//methods
	virtual ILoadingTask* CreateTask(int x, int y, int zoom, BaseProvider* provider, int generation) = 0;
	virtual void Stop();
	TileRequestInfo* FindRequest(int generation);
	void Load(vector<TilePoint*> &points, BaseProvider* provider, int zoom, TileRequestInfo* info );
	unsigned int RegisterTile(int generation);
};

