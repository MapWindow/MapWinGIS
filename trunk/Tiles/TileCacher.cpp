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
#include "TileCacher.h"
#include "SqliteCache.h"
#include "DiskCache.h"

// Runs caching of appropriate type
void CachingTask::DoTask()
{
	if (!this->cacher->stopped)
	{
		if (cacheType == CacheType::SqliteCache) {
			SQLiteCache::DoCaching(this->tile);
		}
		else {
			DiskCache::DoCaching(tile);
		}
		this->cacher->Run();	// cache the next tile
	}
	delete this;
}

// Queues caching request
void TileCacher::Run()
{
	if (!queue.empty() && !SQLiteCache::m_locked)
	{
		TileCore* tile = queue.front();
		queue.pop();
		CachingTask* task = new CachingTask(tile, this, this->cacheType);
		myPool->QueueRequest( (ThreadWorker::RequestType) task );
	}
}
