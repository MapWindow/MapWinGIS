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
#include "BaseProvider.h"
#include "TileCacheManager.h"
#include "ITileLoader.h"

// *********************************************************
//	     PrefetchToFolder()
// *********************************************************
// Writes tiles to the specified folder
void PrefetchManager::PrefetchToFolder(IExtents* ext, int zoom, BaseProvider* provider, BSTR savePath, BSTR fileExt, 
									  IStopExecution* stop, LONG* retVal)
{
	/*AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = 0;

	USES_CONVERSION;
	CStringW path = OLE2W(savePath);
	if (!Utility::DirExists(path))
	{
	ErrorMessage(tkFOLDER_NOT_EXISTS);
	*retVal = -1;
	return S_OK;
	}

	if (tilesLogger.IsOpened())
	{
	tilesLogger.out() << "\n";
	tilesLogger.out() << "PREFETCHING TILES:\n";
	tilesLogger.out() << "ZOOM " << zoom << endl;
	tilesLogger.out() << "---------------------" << endl;
	}

	BaseProvider* p = ((CTileProviders*)_providers)->get_Provider(providerId);
	if (!p)
	{
	ErrorMessage(tkINVALID_PROVIDER_ID);
	*retVal = -1;
	return S_OK;
	}
	else
	{
	double xMin, xMax, yMin, yMax, zMin, zMax;
	ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

	CPoint p1;
	p->get_Projection()->FromLatLngToXY(PointLatLng(yMin, xMin), zoom, p1);

	CPoint p2;
	p->get_Projection()->FromLatLngToXY(PointLatLng(yMax, xMax), zoom, p2);

	*retVal = this->PrefetchCore(p1.x, p2.x, MIN(p1.y, p2.y), MAX(p1.y, p2.y), zoom, providerId, savePath, fileExt, stop);
	}
	return S_OK;*/
}

// *********************************************************
//	     PrefetchCore
// *********************************************************
long PrefetchManager::PrefetchCore(int minX, int maxX, int minY, int maxY, int zoom, BaseProvider* provider,
	BSTR savePath, BSTR fileExt, IStopExecution* stop)
{
	//BaseProvider* provider = ((CTileProviders*)_providers)->get_Provider(providerId);
	//if (!provider)
	//{
	//	ErrorMessage(tkINVALID_PROVIDER_ID);
	//	return 0;
	//}

	//if (provider->get_MaxZoom() < zoom)
	//{
	//	ErrorMessage(tkINVALID_ZOOM_LEVEL);
	//	return 0;
	//}

	//CSize size1, size2;
	//provider->get_Projection()->GetTileMatrixMinXY(zoom, size1);
	//provider->get_Projection()->GetTileMatrixMaxXY(zoom, size2);

	//minX = (int)BaseProjection::Clip(minX, size1.cx, size2.cx);
	//maxX = (int)BaseProjection::Clip(maxX, size1.cy, size2.cy);
	//minY = (int)BaseProjection::Clip(minY, size1.cx, size2.cx);
	//maxY = (int)BaseProjection::Clip(maxY, size1.cy, size2.cy);

	//int centX = (maxX + minX) / 2;
	//int centY = (maxY + minY) / 2;

	//USES_CONVERSION;
	//CStringW path = OLE2W(savePath);
	//if (path.GetLength() > 0 && path.GetAt(path.GetLength() - 1) != L'\\')
	//{
	//	path += L"\\";
	//}

	//CacheType type = path.GetLength() > 0 ? CacheType::tctDiskCache : CacheType::tctSqliteCache;

	//DiskCache* diskCache = NULL;
	//SQLiteCache* sqliteCache = NULL;

	//if (type == CacheType::tctDiskCache)
	//{
	//	diskCache = dynamic_cast<DiskCache*>(TileCacheManager::get_Cache(tctDiskCache));
	//	diskCache->InitializePath(path, OLE2W(fileExt));
	//}

	//if (type == CacheType::tctSqliteCache)
	//{
	//	sqliteCache = dynamic_cast<SQLiteCache*>(TileCacheManager::get_Cache(tctSqliteCache));
	//}

	//std::vector<TilePoint*> points;
	//for (int x = minX; x <= maxX; x++)
	//{
	//	for (int y = minY; y <= maxY; y++)
	//	{
	//		if ((type == tctSqliteCache && !sqliteCache->get_TileExists(provider, zoom, x, y)) ||
	//			type == tctDiskCache && !diskCache->get_TileExists(provider, zoom, x, y))
	//		{
	//			TilePoint* pnt = new TilePoint(x, y);
	//			pnt->dist = sqrt(pow((pnt->x - centX), 2.0) + pow((pnt->y - centY), 2.0));
	//			points.push_back(pnt);
	//		}
	//	}
	//}

	//if (points.size() > 0)
	//{
	//	TileLoader* loader = TileLoaderFactory::Create(type);

	//	if (type == CacheType::tctSqliteCache)
	//	{
	//		sqliteCache->Initialize(SqliteOpenMode::OpenIfExists);
	//	}
	//	else
	//	{
	//		diskCache->CreateFolders(zoom, points);
	//	}

	//	loader->set_StopCallback(stop);
	//	loader->set_Callback(_globalCallback);


	//	// actual call to do the job
	//	loader->Load(points, provider, zoom, false, "", 0, true);

	//	for (size_t i = 0; i < points.size(); i++)
	//	{
	//		delete points[i];
	//	}

	//	return points.size();
	//}

	return 0;
}