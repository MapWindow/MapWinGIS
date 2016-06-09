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
#include <afxmt.h>
#include <atlutil.h>
#include <queue>
#include "Threading.h"
#include "ITileCache.h"

// ***********************************************************
//		TileCacher
// ***********************************************************
// Handles the queue of requests to add tiles to specific cache provider
class TileCacher
{
public:
	TileCacher(ITileCache* cache)
		: _running(false), _stopped(false), _cache(cache)
	{
		_pool = new CThreadPool<ThreadWorker>();

		int poolSize = 1;
		HRESULT hr = _pool->Initialize((void*)322, poolSize);
		if (!SUCCEEDED(hr)) {
			CallbackHelper::ErrorMsg("Failed to initialize thread pool for caching tiles.");
		}
	}

	virtual ~TileCacher(void)
	{
		if (_pool != NULL) {
			_pool->SetTimeout(50);
			_pool->Shutdown();
			delete _pool;
		}
	}

private:
	ITileCache* _cache;
	std::queue<TileCore*> _queue;
	CThreadPool<ThreadWorker>* _pool;
	bool _running;
	bool _stopped;
	::CCriticalSection _queueLock;

public:
	// properties	
	bool isStopped() { return _stopped; }
	ITileCache* get_Cache() { return _cache; }

public:
	// methods
	void Enqueue(TileCore* tile);
	void Run();
	void Stop() { _stopped = true; }
};

// ***********************************************************
//		CachingTask
// ***********************************************************
// Represents a single caching task (single tile to write)
class CachingTask: public ITask
{
public:
	CachingTask(TileCore* tile, TileCacher* cacher)
	{
		_cacher = cacher;
		_tile = tile;
	}

private:
	TileCore* _tile;
	TileCacher* _cacher;

public:	
	void DoTask();
};