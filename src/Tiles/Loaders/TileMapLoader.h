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
#include "ITileLoader.h"

// ******************************************************
//    TileMapLoader()
// ******************************************************
// Tile loader bound to map display, maintains list of active tasks to avoid 
// duplicate requests on fast zooming / panning, calls map update after each new tile is loaded.
class TileMapLoader : public ITileLoader
{
public:
	TileMapLoader(CacheType cacheType)
	{
		ITileCache* cache = TileCacheManager::get_Cache(cacheType);
		_cacher = new TileCacher(cache);
	}

	virtual ~TileMapLoader(void)
	{
		delete _cacher;
	}

protected:
	list<void*> _activeTasks;	// HTTP requests being currently performed
	::CCriticalSection _activeTasksLock;
	TileCacher* _cacher;

public:
	// properties
	list<void*>& get_ActiveTasks() { return _activeTasks; }
	bool IsOutdated(int generation) { return _stopped || generation < _lastGeneration; }

public:
	//methods
	ILoadingTask* CreateTask(int x, int y, int zoom, BaseProvider* provider, int generation);
	void LockActiveTasks(bool lock);
	void AddActiveTask(void* task);
	void RemoveActiveTask(void* task);
	bool HasActiveTask(void* task);
	void StopCaching();
	void ScheduleForCaching(TileCore* tile);
	void RunCaching();
	void Stop();
};

