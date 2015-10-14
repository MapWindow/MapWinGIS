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
#include "stdafx.h"
#include "TileCacher.h"
#include "SqliteCache.h"
#include "DiskCache.h"

// ***********************************************************
//		DoTask()
// ***********************************************************
// Runs caching of appropriate type
void CachingTask::DoTask()
{
	if (!_cacher->isStopped())
	{
		_cacher->get_Cache()->AddTile(_tile);
		_cacher->Run();	// cache the next tile
	}

	delete this;
}

// ***********************************************************
//		Run()
// ***********************************************************
// Queues caching request
void TileCacher::Run()
{
	if (_cache->IsLocked()) {
		return;
	}

	_queueLock.Lock();

	if (!_queue.empty())
	{
		TileCore* tile = _queue.front();
		_queue.pop();

		CachingTask* task = new CachingTask(tile, this);

		_pool->QueueRequest( (ThreadWorker::RequestType) task );
	}

	_queueLock.Unlock();
}