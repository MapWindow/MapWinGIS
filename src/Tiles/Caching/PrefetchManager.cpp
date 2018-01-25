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
#include "PrefetchManager.h"
#include "HttpProxyHelper.h"

::CCriticalSection PrefetchManagerFactory::_lock;
vector<PrefetchManager*> PrefetchManagerFactory::_managers;

// *******************************************************
//		Create()
// *******************************************************
PrefetchManager* PrefetchManagerFactory::Create(ITileCache* cache)
{
	if (!cache) return NULL;

	PrefetchManager* manager = new PrefetchManager(cache);

	_lock.Lock();
	_managers.push_back(manager);
	_lock.Unlock();

	return manager;
}

// *******************************************************
//		Clear()
// *******************************************************
void PrefetchManagerFactory::Clear()
{
	_lock.Lock();

	for (size_t i = 0; i < _managers.size(); i++)
	{
		delete _managers[i];
	}

	_managers.clear();

	_lock.Unlock();
}

// *********************************************************
//	     BuildDownloadList
// *********************************************************
void PrefetchManager::BuildDownloadList(BaseProvider* provider, int zoom, CRect indices, vector<TilePoint*>& points)
{
	CSize size1, size2;
	provider->get_Projection()->GetTileMatrixMinXY(zoom, size1);
	provider->get_Projection()->GetTileMatrixMaxXY(zoom, size2);

	int minX = (int)BaseProjection::Clip(indices.left, size1.cx, size2.cx);
	int maxX = (int)BaseProjection::Clip(indices.right, size1.cy, size2.cy);
	int minY = (int)BaseProjection::Clip(MIN(indices.top, indices.bottom), size1.cx, size2.cx);
	int maxY = (int)BaseProjection::Clip(MAX(indices.top, indices.bottom), size1.cy, size2.cy);

	int centX = (maxX + minX) / 2;
	int centY = (maxY + minY) / 2;

	USES_CONVERSION;

	ITileCache* cache = _loader.get_Cache();

	for (int x = minX; x <= maxX; x++)
	{
		for (int y = minY; y <= maxY; y++)
		{
			if (!cache->get_TileExists(provider, zoom, x, y))
			{
				TilePoint* pnt = new TilePoint(x, y);
				pnt->dist = sqrt(pow((pnt->x - centX), 2.0) + pow((pnt->y - centY), 2.0));
				points.push_back(pnt);
			}
		}
	}
}

// *********************************************************
//	     PrefetchCore
// *********************************************************
long PrefetchManager::Prefetch(BaseProvider* provider, CRect indices, int zoom, ICallback* callback, IStopExecution* stop)
{
	if (!provider)
	{
		CallbackHelper::ErrorMsg("PrefetchManager", NULL, "", "Invalid provider.");
		return -1;
	}

	LogBulkDownloadStarted(zoom);

	if (provider->get_MaxZoom() < zoom)
	{
		CallbackHelper::ErrorMsg("PrefetchManager", NULL, "", "Invalid zoom level for tile provider: %s, %d", provider->Name, zoom);
		return 0;
	}

	vector<TilePoint*> points;
	BuildDownloadList(provider, zoom, indices, points);

	if (points.size() == 0)
	{
		if (tilesLogger.IsOpened())
		{
			tilesLogger.out() << "\n";
			tilesLogger.out() << "Nothing to fetch\n";
			tilesLogger.out() << "---------------------" << endl;
		}
		return 0;
	}

	_loader.set_StopCallback(stop);
	_loader.set_Callback(callback);
	_loader.get_Cache()->InitBulkDownload(zoom, points);

	TileRequestInfo* info = _loader.CreateNextRequest("", false);

	// actual call to do the job
	_loader.Load(points, provider, zoom, info);

	long size = points.size();

	TilePoint::ReleaseMemory(points);

	return size;
}

// *********************************************************
//	     LogBulkDownloadStarted
// *********************************************************
void PrefetchManager::LogBulkDownloadStarted(int zoom)
{
	if (tilesLogger.IsOpened())
	{
		tilesLogger.out() << "\n";
		tilesLogger.out() << "PREFETCHING TILES:\n";
		tilesLogger.out() << "ZOOM " << zoom << endl;
		tilesLogger.out() << "---------------------" << endl;
	}
}
