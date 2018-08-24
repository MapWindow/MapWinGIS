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
// Paul Meems August 2018: Modernized the code as suggested by CLang and ReSharper

#include "StdAfx.h"
#include "TileMapLoader.h"
#include "MapLoadingTask.h"

// *******************************************************
//		CreateTask()
// *******************************************************
ILoadingTask* TileMapLoader::CreateTask(int x, int y, int zoom, BaseProvider* provider, int generation)
{
    return new MapLoadingTask(x, y, zoom, provider, generation);
}

// *******************************************************
//		LockActiveTasks()
// *******************************************************
void TileMapLoader::LockActiveTasks(bool lock)
{
    if (lock)
        _activeTasksLock.Lock();
    else
        _activeTasksLock.Unlock();
}

// *******************************************************
//		AddActiveTask()
// *******************************************************
void TileMapLoader::AddActiveTask(void* task)
{
    _activeTasksLock.Lock();
    _activeTasks.push_back(task);
    _activeTasksLock.Unlock();
}

// *******************************************************
//		RemoveActiveTask()
// *******************************************************
void TileMapLoader::RemoveActiveTask(void* t)
{
    _activeTasksLock.Lock();

    auto* task = (ILoadingTask*)t;
    list<void*>::iterator it = _activeTasks.begin();

    while (it != _activeTasks.end())
    {
        ILoadingTask* item = (ILoadingTask*)*it;
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
bool TileMapLoader::HasActiveTask(void* t)
{
    _activeTasksLock.Lock();

    auto* task = (ILoadingTask*)t;
    list<void*>::iterator it = _activeTasks.begin();
    bool found = false;

    while (it != _activeTasks.end())
    {
        ILoadingTask* item = (ILoadingTask*)*it;
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
//		StopCaching()
// *******************************************************
void TileMapLoader::StopCaching()
{
    _cacher->Stop();
}

// *******************************************************
//		ScheduleForCaching()
// *******************************************************
void TileMapLoader::ScheduleForCaching(TileCore* tile)
{
    if (tile)
    {
        tile->AddRef();
        _cacher->Enqueue(tile);
    }
}

// *******************************************************
//		RunCaching()
// *******************************************************
void TileMapLoader::RunCaching()
{
    _cacher->Run();
}

// *******************************************************
//		Stop()
// *******************************************************
void TileMapLoader::Stop()
{
    ITileLoader::Stop();

    StopCaching();
}
