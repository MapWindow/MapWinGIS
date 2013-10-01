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
#include <queue>
#include "Threading.h"
#include "TileCore.h"

#define POOL_SIZE	1

// Handles the queue requests to add tiles to specific cache provider
class TileCacher
{
private:
	std::queue<TileCore*> queue;
	CThreadPool<ThreadWorker>* myPool;
	bool running;
public:
	bool stopped;
	CacheType cacheType;

	TileCacher()
	{
		this->cacheType = CacheType::SqliteCache;
		myPool = new CThreadPool<ThreadWorker>();
		HRESULT hr = myPool->Initialize((void*)322, POOL_SIZE);
		if (!SUCCEEDED(hr))
			Debug::WriteLine("Failed to initialize caching pool");
		running = false;
		stopped = false;
	}

	~TileCacher(void)
	{
		if (myPool != NULL) {
			myPool->SetTimeout(50);
			myPool->Shutdown();
			delete myPool;
		}
	}

	void Enqueue(TileCore* tile)
	{
		queue.push(tile);
	}

	void Run();
};

// Represents a single caching task (single tile to write)
class CachingTask: public ITask
{
public:	
	TileCore* tile;
	TileCacher* cacher;
	CacheType cacheType;

	CachingTask(TileCore* tile, TileCacher* cacher, CacheType cacheType)
	{
		this->cacher = cacher;
		this->tile = tile;
		this->cacheType = cacheType;
	}

	void DoTask();
};