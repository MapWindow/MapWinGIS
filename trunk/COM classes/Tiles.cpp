/**************************************************************************************
 * File name: Tiles.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CTiles
 *
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
 // lsu 21 aug 2011 - created the file

#include "stdafx.h"
#include "Tiles.h"
#include "SqliteCache.h"
#include "RamCache.h"
#include "map.h"
#include "DiskCache.h"
#include "TileHelper.h"
#include "LoadingTask.h"

::CCriticalSection m_tilesBufferSection;

// ************************************************************
//		Stop()
// ************************************************************
void CTiles::Stop() 
{
	_tileLoader.Stop();
	this->_visible = false;	// will prevent reloading tiles after remove all layers in map destructor
}

// ************************************************************
//		MarkUndrawn()
// ************************************************************
void CTiles::MarkUndrawn()
{
	m_tilesBufferLock.Lock();
	for (unsigned int i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i]->m_drawn = false;
	}
	m_tilesBufferLock.Unlock();
}

// ************************************************************
//		UndrawnTilesExist()
// ************************************************************
// Returns true if at least one undrawn tile exists
bool CTiles::UndrawnTilesExist()
{
	m_tilesBufferLock.Lock();
	bool exists = false;
	for (unsigned int i = 0; i < m_tiles.size(); i++)
	{
		if (!m_tiles[i]->m_drawn) {
			exists = true;
			break;
		}
	}
	m_tilesBufferLock.Unlock();

	return exists;
}

// ************************************************************
//		DrawnTilesExist()
// ************************************************************
// Returns true if at least one drawn tile exists
bool CTiles::DrawnTilesExist()
{
	m_tilesBufferLock.Lock();
	bool exists = false;
	for (unsigned int i = 0; i < m_tiles.size(); i++)
	{
		if (m_tiles[i]->m_drawn) {
			exists = true;
			break;
		}
	}
	m_tilesBufferLock.Unlock();

	return exists;
}


#pragma region "ErrorHandling"
// ************************************************************
//		ClearPrefetchErrors()
// ************************************************************
STDMETHODIMP CTiles::ClearPrefetchErrors()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_prefetchLoader.m_errorCount = 0;
	_prefetchLoader.m_sumCount = 0;
	return S_OK;
}

// ************************************************************
//		get_PrefetchErrorCount()
// ************************************************************
STDMETHODIMP CTiles::get_PrefetchTotalCount(int *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _prefetchLoader.m_sumCount;
	return S_OK;
}

// ************************************************************
//		get_PrefetchErrorCount()
// ************************************************************
STDMETHODIMP CTiles::get_PrefetchErrorCount(int *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _prefetchLoader.m_errorCount;
	return S_OK;
}

// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CTiles::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();
	return S_OK;
}

// ************************************************************
//		put_GlobalCallback()
// ************************************************************
STDMETHODIMP CTiles::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CTiles::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CTiles::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CTiles::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	Utility::DisplayErrorMsg("Tiles", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CTiles::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CTiles::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

#pragma endregion

// *********************************************************
//	     SleepBeforeRequestTimeout()
// *********************************************************
STDMETHODIMP CTiles::get_SleepBeforeRequestTimeout(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _prefetchLoader.m_sleepBeforeRequestTimeout;
	return S_OK;
}
STDMETHODIMP CTiles::put_SleepBeforeRequestTimeout(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal > 10000) newVal = 10000;
	if (newVal < 0) newVal = 0;
	_prefetchLoader.m_sleepBeforeRequestTimeout = newVal;
	return S_OK;
}

// *********************************************************
//	     ScalingRatio()
// *********************************************************
STDMETHODIMP CTiles::get_ScalingRatio(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _scalingRatio;
	return S_OK;
}
STDMETHODIMP CTiles::put_ScalingRatio(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal < 0.5 || newVal > 4.0)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_FALSE;
	}
	_scalingRatio = newVal;
	return S_OK;
}

#pragma region Proxy
// *********************************************************
//	     AutodetectProxy()
// *********************************************************
STDMETHODIMP CTiles::AutodetectProxy(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	*retVal = m_provider->AutodetectProxy();
	return S_OK;
}

// *********************************************************
//	     SetProxy()
// *********************************************************
STDMETHODIMP CTiles::SetProxy(BSTR address, int port, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = m_provider->SetProxy(OLE2A(address), port);
	return S_OK;
}

// *********************************************************
//	     SetProxyAuthorization()
// *********************************************************
STDMETHODIMP CTiles::SetProxyAuthorization(BSTR username, BSTR password, BSTR domain, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = m_provider->SetProxyAuthorization(username, password, domain) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *********************************************************
//	     ClearProxyAuthorization()
// *********************************************************
STDMETHODIMP CTiles::ClearProxyAuthorization()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_provider->ClearProxyAuthorization();
	return S_OK;
}

// *********************************************************
//	     get_Proxy()
// *********************************************************
STDMETHODIMP CTiles::get_Proxy(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	CString s;
	s = m_provider->get_ProxyAddress();
	if (s.GetLength() == 0)
	{
		*retVal = A2BSTR("");
	}
	else
	{
		CString format = s + ":%d";
		short num = m_provider->get_ProxyPort();
		s.Format(format, num);
		*retVal = A2BSTR(s);
	}
	return S_OK;
}
#pragma endregion

#pragma region Choose zoom

// ************************************************************
//		ChooseZoom()
// ************************************************************
int CTiles::ChooseZoom(IExtents* ext, double pixelPerDegree, bool limitByProvider, BaseProvider* provider)
{
	double xMaxD, xMinD, yMaxD, yMinD, zMaxD, zMinD;
	ext->GetBounds(&xMinD, &yMinD, &zMinD, &xMaxD, &yMaxD, &zMaxD);
	return this->ChooseZoom(xMinD, xMaxD, yMinD, yMaxD, pixelPerDegree, limitByProvider, provider);
}

// ************************************************************
//		ChooseZoom()
// ************************************************************
int CTiles::ChooseZoom(double xMin, double xMax, double yMin, double yMax, 
					   double pixelPerDegree, bool limitByProvider, BaseProvider* provider)
{
	if (!provider)
		return 1;

	double lon = (xMax + xMin) / 2.0;
	double lat = (yMax + yMin) / 2.0;
	PointLatLng location(lat, lon);
	CMapView* map = (CMapView*)_mapView;
	if (!map)
		return 1;

	bool precise = map->_tileProjectionState == ProjectionMatch;
	double ratio = precise ? 0.99: 0.75;		// 0.99 = set some error margin for rounding issues

	int bestZoom = provider->minZoom;
	for (int i = provider->minZoom; i <= (limitByProvider ? provider->maxZoom : 20); i++)
	{
		VARIANT_BOOL isSame = precise ? VARIANT_TRUE : VARIANT_FALSE;
		double tileSize = TileHelper::GetTileSizeProj(isSame, provider, map->GetWgs84ToMapTransform(), location, i);
		if (tileSize == -1)
			continue;

		double pixelsPerMapUnit = map->PixelsPerMapUnit();
		tileSize *= pixelsPerMapUnit;
		//double tileSize = GetTileSizeByWidth(location, i, pixelPerDegree);

		int minSize = (int)(256 * _scalingRatio  * ratio);	
		if (tileSize < minSize)
		{
			//Debug::WriteLine("Choose zoom; Tile size: %f", tileSize * 2.0);
			//Debug::WriteLine("Choose zoom; Zoom chosen: %d", bestZoom);
			break;
		}

		bestZoom = i;
	}

	CSize s1, s2;
	provider->Projection->GetTileMatrixMinXY(bestZoom, s1);
	provider->Projection->GetTileMatrixMaxXY(bestZoom, s2);
	provider->minOfTiles.cx = s1.cx;
	provider->minOfTiles.cy = s1.cy;
	provider->maxOfTiles.cx = s2.cx;
	provider->maxOfTiles.cy = s2.cy;
	provider->zoom = bestZoom;

	return bestZoom;
}

// ************************************************************
//		get_CurrentZoom()
// ************************************************************
STDMETHODIMP CTiles::get_CurrentZoom(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = -1;
	CMapView* map = (CMapView*)this->_mapView;
	if (map) {
		IExtents* ext = map->GetGeographicExtents();
		if (ext) {
			*retVal = this->ChooseZoom(ext, map->GetPixelsPerDegree(), false, m_provider);
			ext->Release();
		}
	}
	return S_OK;
}
#pragma endregion

#pragma region Seek tiles
// ************************************************************
//		getRectangleXY()
// ************************************************************
void CTiles::getRectangleXY(double xMinD, double xMaxD, double yMinD, double yMaxD, int zoom, CRect &rect, BaseProvider* provider)
{
	// the number of tiles around
    if (provider)
	{
		CPoint p1, p2;

		provider->Projection->FromLatLngToXY(PointLatLng(yMaxD, xMinD), zoom, p1);
		provider->Projection->FromLatLngToXY(PointLatLng(yMinD, xMaxD), zoom, p2);

		rect.left = p1.x;
		rect.right = p2.x;
		rect.top = p1.y;
		rect.bottom = p2.y;
	}
}

// ************************************************************
//		TilesAreInScreenBuffer()
// ************************************************************
bool CTiles::TilesAreInScreenBuffer(void* mapView)
{
	if (!_visible || !m_provider) {
		return true;		// true because no tiles are actually needed for drawing, hence we need no tiles and yes we "have" them
	}

	int xMin, xMax, yMin, yMax, zoom;
	if (!GetTilesForMap(mapView, xMin, xMax, yMin, yMax, zoom))
		return true;
	
	for (int x = xMin; x <= xMax; x++)
	{
		for (int y = yMin; y <= yMax; y++)
		{
			bool found = false;
			
			m_tilesBufferLock.Lock();
			for (size_t i = 0; i < m_tiles.size(); i++ )
			{
				if (m_tiles[i]->m_tileX == x && m_tiles[i]->m_tileY == y && m_tiles[i]->m_providerId == m_provider->Id)
				{
					found = true;
					break;
				}
			}
			m_tilesBufferLock.Unlock();

			if (!found)
				return false;
		}
	}
	return true;
}

// *********************************************************
//	     TilesAreInCache()
// *********************************************************
// checks whether all the tiles are present in cache
bool CTiles::TilesAreInCache(void* mapView, tkTileProvider providerId)
{
	BaseProvider* provider = providerId == ProviderNone ? m_provider : ((CTileProviders*)_providers)->get_Provider(providerId);

	if (!_visible || !provider) {
		return true;		// true because no tiles are actually needed for drawing, hence we need no tiles and yes we "have" them
	}

	int xMin, xMax, yMin, yMax, zoom;
	if (!GetTilesForMap(mapView, providerId, xMin, xMax, yMin, yMax, zoom))
		return true;
	
	for (int x = xMin; x <= xMax; x++)
	{
		for (int y = yMin; y <= yMax; y++)
		{
			bool found = false;
			
			m_tilesBufferLock.Lock();
			for (size_t i = 0; i < m_tiles.size(); i++ )
			{
				if (m_tiles[i]->m_tileX == x && 
					m_tiles[i]->m_tileY == y && 
					m_tiles[i]->m_scale == zoom && 
					m_tiles[i]->m_providerId == provider->Id)
				{
					found = true;
					break;
				}
			}
			m_tilesBufferLock.Unlock();

			if (!found && _useRamCache)
			{
				TileCore* tile = RamCache::get_Tile(provider->Id, zoom, x, y);
				if (tile)	found = true;
			}

			if (!found && _useDiskCache)
			{
				TileCore* tile = SQLiteCache::get_Tile(provider, zoom, x, y);
				if (tile)	found = true;
			}

			if (!found)
				return false;
		}
	}
	return true;
}

bool CTiles::GetTilesForMap(void* mapView, int& xMin, int& xMax, int& yMin, int& yMax, int& zoom)
{
	return GetTilesForMap(mapView, -1, xMin, xMax, yMin, yMax, zoom);
}

// *********************************************************
//	     GetTilesForMap()
// *********************************************************
// returns list of tiles for current map extents
bool CTiles::GetTilesForMap(void* mapView, int providerId, int& xMin, int& xMax, int& yMin, int& yMax, int& zoom)
{
	BaseProvider* provider = providerId == -1 ? m_provider : ((CTileProviders*)_providers)->get_Provider(providerId);
	if (!provider)
		return false;

	CMapView* map = (CMapView*)mapView;
	tkTransformationMode transformMode = map->_transformationMode;
	if (transformMode == tmNotDefined)
		return false;		// no need to go any further there is no projection

	Extent clipExtents(map->_extents.left, map->_extents.right, map->_extents.bottom, map->_extents.top);
	bool clipForTiles = this->ProjectionBounds(provider, map->GetMapProjection(), map->GetWgs84Projection(), transformMode, clipExtents);
	
	if (!provider->mapView)
		provider->mapView = mapView;

	// we don't want to have coordinates outside world bounds, as it breaks tiles loading
	IExtents* ext = map->GetGeographicExtentsCore(clipForTiles, &clipExtents);
	if (!ext) {
		return false;
	}

	double xMaxD, xMinD, yMaxD, yMinD, zMaxD, zMinD;
	ext->GetBounds(&xMinD, &yMinD, &zMinD, &xMaxD, &yMaxD, &zMaxD);
	zoom = ChooseZoom(ext, map->GetPixelsPerDegree(), true, provider);
	ext->Release();

	// what tiles are needed?
	CRect rect;
	this->getRectangleXY(xMinD, xMaxD, yMinD, yMaxD, zoom, rect, provider);
	yMin = MIN(rect.top, rect.bottom);
	yMax = MAX(rect.top, rect.bottom);
	xMin = MIN(rect.left, rect.right);
	xMax = MAX(rect.left, rect.right);
	return true;
}
#pragma endregion

#pragma region Load tiles
// *********************************************************
//	     LoadTiles()
// *********************************************************
// current provider will be used
void CTiles::LoadTiles(void* mapView, bool isSnapshot, CString key)
{
	LoadTiles(mapView, isSnapshot, m_provider->Id, key);
}

// *********************************************************
//	     LoadTiles()
// *********************************************************
// any provider can be passed (for caching or snapshot)
void CTiles::LoadTiles(void* mapView, bool isSnapshot, int providerId, CString key)
{
	CMapView* map = (CMapView*)mapView;
	if (_lastMapExtents.left == map->_extents.left && _lastMapExtents.right == map->_extents.right &&
		_lastMapExtents.top == map->_extents.top && _lastMapExtents.bottom == map->_extents.bottom &&
		_lastProvider == providerId)
	{
		tilesLogger.WriteLine("Duplicate request dropped.");
		return;	
	}
	
	BaseProvider* provider = ((CTileProviders*)_providers)->get_Provider(providerId);
	if (!_visible || !provider) {
		this->Clear();
		return;
	}

	int xMin, xMax, yMin, yMax, zoom;
	if (!GetTilesForMap(mapView, provider->Id, xMin, xMax, yMin, yMax, zoom))
	{
		this->Clear();
		return;
	}

	if (xMin == _lastTileExtents.left && xMax == _lastTileExtents.right &&
		yMin == _lastTileExtents.bottom && yMax == _lastTileExtents.top &&
		_lastProvider == provider->Id && _lastZoom == zoom)
	{
		// map extents has changed but the list of tiles to be displayed is the same
		tilesLogger.WriteLine("The same list of tiles can be used.");
		
		((CMapView*)mapView)->_tileBuffer.Initialized = false;
		return;	
	}

	_lastMapExtents.left = map->_extents.left;
	_lastMapExtents.right = map->_extents.right;
	_lastMapExtents.top = map->_extents.top;
	_lastMapExtents.bottom = map->_extents.bottom;
	_lastTileExtents.left = xMin;
	_lastTileExtents.right = xMax;
	_lastTileExtents.top = yMax;
	_lastTileExtents.bottom = yMin;
	_lastProvider = providerId;
	_lastZoom = zoom;

	tilesLogger.WriteLine("");
	tilesLogger.WriteLine("LOAD TILES: xMin=%d; xMax=%d; yMin=%d; yMax=%d; zoom =%d", xMin, xMax, yMin, yMax, zoom);

	m_tilesBufferLock.Lock();
	for (size_t i = 0; i < m_tiles.size(); i++ )
	{
		m_tiles[i]->m_drawn = false;
		m_tiles[i]->m_inBuffer = false;
		m_tiles[i]->m_toDelete = true;
	}
	m_tilesBufferLock.Unlock();
	((CMapView*)mapView)->_tileBuffer.Initialized = false;

	if (!provider->mapView)
		provider->mapView = mapView;
	
	int centX = (xMin + xMax) /2;
	int centY = (yMin + yMax) /2;
	
	if (_doDiskCaching)  {
		SQLiteCache::Initialize(SqliteOpenMode::OpenOrCreate);
	}
	if (_useDiskCache)	{
		SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists);
		SQLiteCache::m_locked = true;	// caching will be stopped while loading tiles to avoid locking the database
	}
	
	int generation = 0;
	_tileLoader.tileGeneration++;		// all incoming tasks will be discarded
	generation = _tileLoader.tileGeneration;

	// ------------------------------------------------------------------
	//  check which ones we already have, and which ones are to be loaded
	// ------------------------------------------------------------------
	// first check active tasks
	std::vector<CTilePoint*> activeTasks;
	if (!isSnapshot) {
		// lock it, so active task can't be removed while we analyze it here
		_tileLoader.LockActiveTasks(true);
		
		std::list<void*> list = _tileLoader.GetActiveTasks();
		std::list<void*>::iterator it = list.begin();
		while (it != list.end())
		{
			LoadingTask* task = (LoadingTask*)*it;
			if (task->Provider->Id == providerId && task->zoom == zoom &&
				task->x >= xMin && task->x <= xMax && task->y >= yMin && task->y <= yMax)
			{
				tilesLogger.WriteLine("Tile reassigned to current generation: %d\\%d\\%d", zoom, task->x, task->y);
				task->generation = generation;								// reassign it to current generation
				activeTasks.push_back(new CTilePoint(task->x, task->y));    // don't include in current list of requests
			}
			++it;
		}
		_tileLoader.m_count = 0;
		_tileLoader.m_totalCount = 1000;		// set it to a big number until we count them all
		_tileLoader.LockActiveTasks(false);
	}
	
	std::vector<CTilePoint*> points;
	for (int x = xMin; x <= xMax; x++)
	{
		for (int y = yMin; y <= yMax; y++)
		{
			// was it reassigned already is reassigned?
			bool found = false;
			for (size_t i = 0; i < activeTasks.size(); i++ )
			{
				if (activeTasks[i]->x == x && activeTasks[i]->y == y)
				{
					found = true;
					break;
				}
			}
			
			if (found)
				continue;
			
			// check maybe the tile is already in the buffer
			m_tilesBufferLock.Lock();
			for (size_t i = 0; i < m_tiles.size(); i++ )
			{
				TileCore* tile = m_tiles[i];
				if (tile->m_tileX == x && tile->m_tileY == y  && tile->m_scale == zoom && tile->m_providerId == provider->Id)
				{
					tile->m_toDelete = false;
					tile->m_inBuffer = true;
					found = true;
					break;
				}
			}
			m_tilesBufferLock.Unlock();
			
			if (found)
				continue;
			
			if (_useRamCache)
			{
				TileCore* tile = RamCache::get_Tile(provider->Id, zoom, x, y);
				if (tile)
				{
					this->AddTileNoCaching(tile);
					continue;
				}
			}

			if (_useDiskCache)
			{
				TileCore* tile = SQLiteCache::get_Tile(provider, zoom, x, y);
				if (tile)
				{
					this->AddTileNoCaching(tile);
					continue;
				}
			}

			// if the tile isn't present in both caches, request it from the server
			if (_useServer)
			{
				CTilePoint* pnt = new CTilePoint(x, y);
				pnt->dist = sqrt(pow((pnt->x - centX), 2.0) + pow((pnt->y - centY), 2.0));
				points.push_back(pnt);
			}
		}
	}

	if(!isSnapshot) {
		_tileLoader.m_totalCount = points.size() + activeTasks.size();
	}

	for (size_t i = 0; i < activeTasks.size(); i++)
		delete activeTasks[i];

	// -------------------------------------------------
	// delete unused tiles from the screen buffer
	// -------------------------------------------------
	m_tilesBufferLock.Lock();			
	std::vector<TileCore*>::iterator it = m_tiles.begin();
	while (it < m_tiles.end())
	{
		TileCore* tile = (*it);
		if (tile->m_toDelete)
		{
			tile->Release();
			it = m_tiles.erase(it);
		}
		else
		{
			++it;
		}
	}
	m_tilesBufferLock.Unlock();

	if (_useDiskCache)
	{
		SQLiteCache::m_locked = false;	// caching will be stopped while loading tiles to avoid locking the database and speed up things
	}
	
	// -------------------------------------------------
	// passing list of tiles too loader
	// -------------------------------------------------
	// let's try not to mess up snapshot if main tile loader is still working
	TileLoader* loader = isSnapshot ? &_prefetchLoader : &_tileLoader;

	loader->RunCaching();
	if (points.size() > 0)
	{
		tilesLogger.WriteLine("Queued to load from server: %d", points.size());
		
		// zoom can change in the process, so we use the calculated version
		// and not the one current for provider
		loader->Load(points, zoom, provider, (void*)this, isSnapshot, key, generation);	
														
		// releasing points
		for (size_t i = 0; i < points.size(); i++)
			delete points[i];
	}
	else
	{
		HandleOnTilesLoaded(false, "", true);
	}
}

// *********************************************************
//	     HandleOnTilesLoaded()
// *********************************************************
void CTiles::HandleOnTilesLoaded(bool isSnapshot, CString key, bool nothingToLoad)
{
	if ((CMapView*)m_provider->mapView != NULL)
	{
		LPCTSTR newStr = (LPCTSTR)key;
		((CMapView*)m_provider->mapView)->FireTilesLoaded(this, isSnapshot, newStr);
		tilesLogger.WriteLine("Tiles loaded event; Were loaded from server (y/n): %d", !nothingToLoad);
	}
}

#pragma endregion

#pragma region Functions
// *********************************************************
//	     AddTileWithCaching()
// *********************************************************
void CTiles::AddTileWithCaching(TileCore* tile)
{
	this->AddTileNoCaching(tile);
	this->AddTileOnlyCaching(tile);
}

void CTiles::AddTileNoCaching(TileCore* tile)
{
	tile->m_inBuffer = true;
	tile->m_toDelete = false;
	tile->AddRef();
	
	m_tilesBufferLock.Lock();
	m_tiles.push_back(tile);
	m_tilesBufferLock.Unlock();
}

void CTiles::AddTileOnlyCaching(TileCore* tile)
{
	/*if (m_doRamCaching) {
		RamCache::AddToCache(tile);
	}*/
	if (_doDiskCaching) {
		_tileLoader.ScheduleForCaching(tile);
	}
}

void CTiles::AddTileToRamCache(TileCore* tile) {
	if (_doRamCaching) {
		RamCache::AddToCache(tile);
	}
}

// *********************************************************
//	     Clear()
// *********************************************************
void CTiles::Clear()
{
	m_tilesBufferLock.Lock();
	for (size_t i = 0; i < m_tiles.size(); i++)
	{
		m_tiles[i]->m_drawn = false;
		m_tiles[i]->m_inBuffer = false;
		m_tiles[i]->Release();
	}
	m_tiles.clear();
	m_tilesBufferLock.Unlock();

	_lastMapExtents.left = 0;
	_lastMapExtents.right = 0;
	_lastMapExtents.top = 0;
	_lastMapExtents.bottom = 0;
	_lastTileExtents.left = 0;
	_lastTileExtents.right = 0;
	_lastTileExtents.top = 0;
	_lastTileExtents.bottom = 0;
}
#pragma endregion

#pragma region Properties
// *********************************************************
//	     Provider
// *********************************************************
STDMETHODIMP CTiles::get_Provider(tkTileProvider* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CustomProvider* p = dynamic_cast<CustomProvider*>(m_provider);
	*pVal = p ? tkTileProvider::ProviderCustom : (tkTileProvider)m_provider->Id;
	return S_OK;
}
STDMETHODIMP CTiles::put_Provider(tkTileProvider newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal < 0 || newVal >= tkTileProvider::ProviderCustom)
		return S_FALSE;

	if (m_provider->Id != newVal && newVal != tkTileProvider::ProviderCustom) 
	{
		m_provider = ((CTileProviders*)_providers)->get_Provider((int)newVal);
		this->UpdateProjection();
	}
	return S_OK;
}

// *********************************************************
//	     get_ProviderName
// *********************************************************
STDMETHODIMP CTiles::get_ProviderName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = m_provider ? A2BSTR(m_provider->Name) : A2BSTR("");
	return S_OK;
}

// *********************************************************
//	     GridLinesVisible
// *********************************************************
STDMETHODIMP CTiles::get_GridLinesVisible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _gridLinesVisible;
	return S_OK;
}
STDMETHODIMP CTiles::put_GridLinesVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_gridLinesVisible = newVal ? true: false;
	return S_OK;
}

// *********************************************************
//	     MinScaleToCache
// *********************************************************
STDMETHODIMP CTiles::get_MinScaleToCache(int* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _minScaleToCache;		// TODO: use in caching process
	return S_OK;
}
STDMETHODIMP CTiles::put_MinScaleToCache(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_minScaleToCache = newVal;
	return S_OK;
}

// *********************************************************
//	     MaxScaleToCache
// *********************************************************
STDMETHODIMP CTiles::get_MaxScaleToCache(int* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _maxScaleToCache;		// TODO: use in caching process
	return S_OK;
}
STDMETHODIMP CTiles::put_MaxScaleToCache(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_maxScaleToCache = newVal;
	return S_OK;
}

// *********************************************************
//	     Visible
// *********************************************************
STDMETHODIMP CTiles::get_Visible(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _visible;
	return S_OK;
}

STDMETHODIMP CTiles::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_visible = newVal != 0;
	return S_OK;
}

// *********************************************************
//	     CustomProviders
// *********************************************************
STDMETHODIMP CTiles::get_Providers(ITileProviders** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_providers)
		_providers->AddRef();
	*retVal = _providers;
	return S_OK;
}

// *********************************************************
//	     DoCaching
// *********************************************************
STDMETHODIMP CTiles::get_DoCaching(tkCacheType type, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			*pVal =  _doRamCaching;
			break;
		case tkCacheType::Disk:
			*pVal = _doDiskCaching;
			break;
		case tkCacheType::Both:
			*pVal = _doRamCaching || _doDiskCaching;
			break;
		default:
			*pVal = VARIANT_FALSE;
			break;
	}
	return S_OK;
}
STDMETHODIMP CTiles::put_DoCaching(tkCacheType type, VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			_doRamCaching = newVal != 0;
			break;
		case tkCacheType::Disk:
			_doDiskCaching = newVal != 0;
			break;
		case tkCacheType::Both:
			_doDiskCaching = _doRamCaching = newVal != 0;
			break;
	}
	return S_OK;
}

// *********************************************************
//	     UseCache
// *********************************************************
STDMETHODIMP CTiles::get_UseCache(tkCacheType type, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			*pVal =  _useRamCache;
			break;
		case tkCacheType::Disk:
			*pVal = _useDiskCache;
			break;
		case tkCacheType::Both:
			*pVal = _useRamCache || _useDiskCache;
			break;
		default:
			*pVal = VARIANT_FALSE;
			break;
	}
	return S_OK;
}
STDMETHODIMP CTiles::put_UseCache(tkCacheType type, VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			_useRamCache = newVal != 0;
			break;
		case tkCacheType::Disk:
			_useDiskCache = newVal != 0;
			break;
		case tkCacheType::Both:
			_useRamCache = _useDiskCache = newVal != 0;
			break;
	}
	return S_OK;
}

// *********************************************************
//	     UseServer
// *********************************************************
STDMETHODIMP CTiles::get_UseServer(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _useServer;
	return S_OK;
}
STDMETHODIMP CTiles::put_UseServer(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_useServer = newVal != 0;
	return S_OK;
}

// *********************************************************
//	     get_DiskCacheFilename
// *********************************************************
STDMETHODIMP CTiles::get_DiskCacheFilename(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = W2BSTR(SQLiteCache::get_DbName());
	return S_OK;
}

// *********************************************************
//	     put_DiskCacheFilename
// *********************************************************
STDMETHODIMP CTiles::put_DiskCacheFilename(BSTR pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	SQLiteCache::set_DbName(OLE2W(pVal));
	return S_OK;
}

// *********************************************************
//	     MaxCacheSize
// *********************************************************
STDMETHODIMP CTiles::get_MaxCacheSize(tkCacheType type, double* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			*pVal =  RamCache::m_maxSize;
			break;
		case tkCacheType::Disk:
			*pVal = SQLiteCache::maxSizeDisk;
			break;
		default:
			*pVal = 0;
			break;
	}
	return S_OK;
}
STDMETHODIMP CTiles::put_MaxCacheSize(tkCacheType type, double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			RamCache::m_maxSize = newVal;
			break;
		case tkCacheType::Disk:
			SQLiteCache::maxSizeDisk = newVal;
			break;
		case tkCacheType::Both:
			RamCache::m_maxSize = SQLiteCache::maxSizeDisk = newVal;
			break;
	}
	return S_OK;
}

// *********************************************************
//	     ClearCache()
// *********************************************************
STDMETHODIMP CTiles::ClearCache(tkCacheType type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			RamCache::ClearByProvider(tkTileProvider::ProviderNone, 0, 100);
			break;
		case tkCacheType::Disk:
			SQLiteCache::Clear(tkTileProvider::ProviderNone, 0, 100);
			break;
		case tkCacheType::Both:
			RamCache::ClearAll(0, 100);
			SQLiteCache::Clear(tkTileProvider::ProviderNone, 0, 100);
			break;
	}
	return S_OK;
}

// *********************************************************
//	     ClearCache2()
// *********************************************************
STDMETHODIMP CTiles::ClearCache2(tkCacheType type, tkTileProvider provider, int fromScale, int toScale)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	switch(type)
	{
		case tkCacheType::RAM:
			RamCache::ClearByProvider(provider, fromScale, toScale);
			break;
		case tkCacheType::Disk:
			SQLiteCache::Clear(provider, fromScale, toScale);
			break;
		case tkCacheType::Both:
			SQLiteCache::Clear(provider, fromScale, toScale);
			RamCache::ClearByProvider(provider, fromScale, toScale);
			break;
	}
	return S_OK;
}

// *********************************************************
//	     get_CacheSize()
// *********************************************************
STDMETHODIMP CTiles::get_CacheSize(tkCacheType type, double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = 0.0;
	switch(type)
	{
		case tkCacheType::RAM:
			*retVal = RamCache::get_MemorySize();
			break;
		case tkCacheType::Disk:
			*retVal = SQLiteCache::get_FileSize();
			break;
		case tkCacheType::Both:
			*retVal = 0.0;		// it hardly makes sense to show sum of both values or any of it separately
			break;
	}
	return S_OK;
}

// *********************************************************
//	     get_CacheSize3()
// *********************************************************
// TODO: use provider id rather than provider enumeration.
STDMETHODIMP CTiles::get_CacheSize2(tkCacheType type, tkTileProvider provider, int scale, double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = 0.0;
	switch(type)
	{
		case tkCacheType::RAM:
			*retVal = RamCache::get_MemorySize(provider, scale);
			break;
		case tkCacheType::Disk:
			*retVal = SQLiteCache::get_FileSize(provider, scale);
			break;
		case tkCacheType::Both:
			*retVal = 0.0;	// it doesn't make sense to return any of the two
			break;
	}
	return S_OK;
}
#pragma endregion

#pragma region "Serialization"
// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CTiles::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CPLXMLNode* psTree = this->SerializeCore("TilesClass");
	Utility::SerializeAndDestroyXmlTree(psTree, retVal);
	return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
CPLXMLNode* CTiles::SerializeCore(CString ElementName)
{
	USES_CONVERSION;
	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, ElementName);
	
	if (!_visible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Visible", CPLString().Printf("%d", (int)_visible));
	if (_gridLinesVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "GridLinesVisible", CPLString().Printf("%d", (int)_gridLinesVisible));
	if (m_provider->Id != 0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Provider", CPLString().Printf("%d", (int)m_provider->Id));
	if (!_doRamCaching)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "DoRamCaching", CPLString().Printf("%d", (int)_doRamCaching));
	if (_doDiskCaching)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "DoDiskCaching", CPLString().Printf("%d", (int)_doDiskCaching));
	if (!_useRamCache)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseRamCache", CPLString().Printf("%d", (int)_useRamCache));
	if (!_useDiskCache)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseDiskCache", CPLString().Printf("%d", (int)_useDiskCache));
	if (!_useServer)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseServer", CPLString().Printf("%d", (int)_useServer));
	if (RamCache::m_maxSize != 100.0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxRamCacheSize", CPLString().Printf("%f", RamCache::m_maxSize));
	if (SQLiteCache::maxSizeDisk != 100.0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxDiskCacheSize", CPLString().Printf("%f", SQLiteCache::maxSizeDisk));
	if (_minScaleToCache != 0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MinScaleToCache", CPLString().Printf("%d", _minScaleToCache));
	if (_maxScaleToCache != 100)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxScaleToCache", CPLString().Printf("%d", _maxScaleToCache));
	
	CStringW dbName = SQLiteCache::get_DbName();
	if (dbName.GetLength() != 0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "DiskCacheFilename", dbName);
	
	// serialization of custom providers
	CPLXMLNode* psProviders = CPLCreateXMLNode( NULL, CXT_Element, "TileProviders");
	if (psProviders)
	{
		vector<BaseProvider*>* providers = ((CTileProviders*)_providers)->GetList();
		for(size_t i = 0; i < providers->size(); i++)
		{
			CustomProvider* cp = dynamic_cast<CustomProvider*>(providers->at(i));
			if (cp)
			{
				CPLXMLNode* psCustom = CPLCreateXMLNode( NULL, CXT_Element, "TileProvider");
				Utility::CPLCreateXMLAttributeAndValue(psCustom, "Id", CPLString().Printf("%d", cp->Id));
				Utility::CPLCreateXMLAttributeAndValue(psCustom, "Name", cp->Name);
				Utility::CPLCreateXMLAttributeAndValue(psCustom, "Url", cp->UrlFormat);
				Utility::CPLCreateXMLAttributeAndValue(psCustom, "Projection", CPLString().Printf("%d", (int)cp->m_projectionId));
				Utility::CPLCreateXMLAttributeAndValue(psCustom, "MinZoom", CPLString().Printf("%d", cp->minZoom));
				Utility::CPLCreateXMLAttributeAndValue(psCustom, "MaxZoom", CPLString().Printf("%d", cp->maxZoom));
				CPLAddXMLChild(psProviders, psCustom);
			}
		}
		CPLAddXMLChild(psTree, psProviders);
	}
	return psTree;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CTiles::Deserialize(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeTiles = CPLGetXMLNode(node, "=TilesClass");
		if (nodeTiles)
		{
			DeserializeCore(nodeTiles);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

void setBoolean(CPLXMLNode *node, CString name, bool& value)
{
	CString s = CPLGetXMLValue( node, name, NULL );
	if (s != "") value = atoi( s ) == 0? false : true;
}

void setInteger(CPLXMLNode *node, CString name, int& value)
{
	CString s = CPLGetXMLValue( node, name, NULL );
	if (s != "") value = atoi( s );
}

void setDouble(CPLXMLNode *node, CString name, double& value)
{
	CString s = CPLGetXMLValue( node, name, NULL );
	if (s != "") value = Utility::atof_custom( s );
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CTiles::DeserializeCore(CPLXMLNode* node)
{
	if (!node)
		return false;

	this->SetDefaults();

	setBoolean(node, "Visible", _visible);
	setBoolean(node, "GridLinesVisible", _gridLinesVisible);
	setBoolean(node, "DoRamCaching", _doRamCaching);
	setBoolean(node, "DoDiskCaching", _doDiskCaching);
	setBoolean(node, "UseRamCache", _useRamCache);
	setBoolean(node, "UseDiskCache", _useDiskCache);
	setBoolean(node, "UseServer", _useServer);

	CString s = CPLGetXMLValue( node, "Provider", NULL );
	if (s != "") this->put_ProviderId(atoi( s ));

	setDouble(node, "MaxRamCacheSize", RamCache::m_maxSize);
	setDouble(node, "MaxDiskCacheSize", SQLiteCache::maxSizeDisk);
	setInteger(node, "MinScaleToCache", _minScaleToCache);
	setInteger(node, "MaxScaleToCache", _maxScaleToCache);
	
	USES_CONVERSION;
	s = CPLGetXMLValue( node, "DiskCacheFilename", NULL );
	if (s != "") SQLiteCache::set_DbName(Utility::ConvertFromUtf8(s));

	// custom providers
	CPLXMLNode* nodeProviders = CPLGetXMLNode(node, "TileProviders");
	if (nodeProviders)
	{
		// don't clear providers as it will clear the cache as well,
		// if provider with certain id exists, it simply won't be added twice
		vector<BaseProvider*>* providers = ((CTileProviders*)_providers)->GetList();

		CPLXMLNode* nodeProvider = nodeProviders->psChild;
		while (nodeProvider)
		{
			if (strcmp(nodeProvider->pszValue, "TileProvider") == 0)
			{
				int id, minZoom, maxZoom, projection;
				CString url, name;
				
				s = CPLGetXMLValue( nodeProvider, "Id", NULL );
				if (s != "") id = atoi(s);

				s = CPLGetXMLValue( nodeProvider, "MinZoom", NULL );
				if (s != "") minZoom = atoi(s);

				s = CPLGetXMLValue( nodeProvider, "MaxZoom", NULL );
				if (s != "") maxZoom = atoi(s);

				s = CPLGetXMLValue( nodeProvider, "Projection", NULL );
				if (s != "") projection = atoi(s);

				s = CPLGetXMLValue( nodeProvider, "Url", NULL );
				if (s != "") url = s;

				s = CPLGetXMLValue( nodeProvider, "Name", NULL );
				if (s != "") name = s;

				VARIANT_BOOL vb;
				CComBSTR bstrName(name);
				CComBSTR bstrUrl(url);

				_providers->Add(id, bstrName, bstrUrl, (tkTileProjection)projection, minZoom, maxZoom, &vb);
			}
			nodeProvider = nodeProvider->psNext;
		}
	}
	return true;
}
#pragma endregion

// *********************************************************
//	     CustomProviderId
// *********************************************************
STDMETHODIMP CTiles::get_ProviderId(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_provider->Id;
	return S_OK;
}

STDMETHODIMP CTiles::put_ProviderId(int providerId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BaseProvider* provider = ((CTileProviders*)_providers)->get_Provider(providerId);
	if (provider) {
		m_provider = provider;
		this->UpdateProjection();
	}
	return S_OK;
}

// *********************************************************
//	     GetTilesIndices
// *********************************************************
STDMETHODIMP CTiles::GetTilesIndices(IExtents* boundsDegrees, int zoom, int providerId, IExtents** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!boundsDegrees)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}
	
	double xMin, xMax, yMin, yMax, zMin, zMax;
	boundsDegrees->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
	
	BaseProvider* provider = ((CTileProviders*)_providers)->get_Provider(providerId);
	if (provider)
	{
		CPoint p1;
		provider->Projection->FromLatLngToXY(PointLatLng(yMax, xMin), zoom, p1);

		CPoint p2;
		provider->Projection->FromLatLngToXY(PointLatLng(yMin, xMax), zoom, p2);

		IExtents* ext = NULL;
		CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&ext);
		ext->SetBounds(p1.x, p1.y, 0.0, p2.x, p2.y, 0.0);
		*retVal = ext;
	}
	else {
		*retVal = NULL;
	}
	return S_OK;
}

// *********************************************************
//	     Prefetch
// *********************************************************
STDMETHODIMP CTiles::Prefetch(double minLat, double maxLat, double minLng, double maxLng, int zoom, int providerId, 
							  IStopExecution* stop, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	BaseProvider* p = ((CTileProviders*)_providers)->get_Provider(providerId);
	if (!p)
	{
		ErrorMessage(tkINVALID_PROVIDER_ID);
		return S_FALSE;
	}
	else
	{
		CPoint p1;
		p->Projection->FromLatLngToXY(PointLatLng(minLat, minLng), zoom, p1);

		CPoint p2;
		p->Projection->FromLatLngToXY(PointLatLng(maxLat, maxLng), zoom, p2);

		this->Prefetch2(p1.x, p2.x, MIN(p1.y, p2.y) , MAX(p1.y, p2.y), zoom, providerId, stop, retVal);
		return S_OK;
	}
}

// *********************************************************
//	     Prefetch2
// *********************************************************
STDMETHODIMP CTiles::Prefetch2(int minX, int maxX, int minY, int maxY, int zoom, int providerId, IStopExecution* stop, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = PrefetchCore(minX, maxX, minY, maxY, zoom, providerId, m_globalSettings.emptyBstr, m_globalSettings.emptyBstr, stop);
	return S_OK;
}

// *********************************************************
//	     StartLogRequests()
// *********************************************************
STDMETHODIMP CTiles::StartLogRequests(BSTR filename, VARIANT_BOOL errorsOnly, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	CStringW path = OLE2W(filename);
	tilesLogger.Open(path);
	tilesLogger.errorsOnly = errorsOnly ? true: false;
	*retVal = tilesLogger.IsOpened();
	return S_OK;
}

// *********************************************************
//	     StopLogRequests()
// *********************************************************
STDMETHODIMP CTiles::StopLogRequests()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	tilesLogger.Close();
	return S_OK;
}

// *********************************************************
//	     LogIsOpened()
// *********************************************************
STDMETHODIMP CTiles::get_LogIsOpened(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = tilesLogger.IsOpened();
	return S_OK;
}

// *********************************************************
//	     LogFilename()
// *********************************************************
STDMETHODIMP CTiles::get_LogFilename(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = W2BSTR(tilesLogger.GetFilename());
	return S_OK;
}

// *********************************************************
//	     LogErrorsOnly()
// *********************************************************
STDMETHODIMP CTiles::get_LogErrorsOnly(VARIANT_BOOL *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = tilesLogger.errorsOnly;
	return S_OK;
}
STDMETHODIMP CTiles::put_LogErrorsOnly(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	tilesLogger.errorsOnly = newVal ? true: false;
	return S_OK;
}

// *********************************************************
//	     PrefetchToFolder()
// *********************************************************
// Writes tiles to the specified folder
STDMETHODIMP CTiles::PrefetchToFolder(IExtents* ext, int zoom, int providerId, BSTR savePath, BSTR fileExt, IStopExecution* stop, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = 0;
	
	USES_CONVERSION;
	CStringW path = OLE2W(savePath);
	if (!Utility::DirExists(path))
	{
		ErrorMessage(tkFOLDER_NOT_EXISTS);
		*retVal = -1;
		return S_FALSE;
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
		return S_FALSE;
	}
	else
	{
		double xMin, xMax, yMin, yMax, zMin, zMax;
		ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

		CPoint p1;
		p->Projection->FromLatLngToXY(PointLatLng(yMin, xMin), zoom, p1);

		CPoint p2;
		p->Projection->FromLatLngToXY(PointLatLng(yMax, xMax), zoom, p2);

		*retVal = this->PrefetchCore(p1.x, p2.x, MIN(p1.y, p2.y) , MAX(p1.y, p2.y), zoom, providerId, savePath, fileExt, stop);
	}
	return S_OK;
}

// *********************************************************
//	     PrefetchToFolder()
// *********************************************************
long CTiles::PrefetchCore(int minX, int maxX, int minY, int maxY, int zoom, int providerId, 
								  BSTR savePath, BSTR fileExt, IStopExecution* stop)
{
	BaseProvider* provider = ((CTileProviders*)_providers)->get_Provider(providerId);
	if (!provider)
	{
		ErrorMessage(tkINVALID_PROVIDER_ID);
		return S_FALSE;
	}
	else if (provider->maxZoom < zoom) 
	{
		ErrorMessage(tkINVALID_ZOOM_LEVEL);
		return S_FALSE;
	}
	else
	{
		CSize size1, size2;
		provider->Projection->GetTileMatrixMinXY(zoom, size1);
		provider->Projection->GetTileMatrixMaxXY(zoom, size2);

		minX = (int)BaseProjection::Clip(minX, size1.cx, size2.cx);
		maxX = (int)BaseProjection::Clip(maxX, size1.cy, size2.cy);
		minY = (int)BaseProjection::Clip(minY, size1.cx, size2.cx);
		maxY = (int)BaseProjection::Clip(maxY, size1.cy, size2.cy);
		
		int centX = (maxX + minX)/2;
		int centY = (maxY + minY)/2;

		USES_CONVERSION;
		CStringW path = OLE2W(savePath);
		if (path.GetLength() > 0 && path.GetAt(path.GetLength() - 1) != L'\\')
		{
			path += L"\\";
		}
		CacheType type = path.GetLength() > 0 ? CacheType::DiskCache : CacheType::SqliteCache;

		if (type == CacheType::DiskCache)
		{
			CString ext = OLE2A(fileExt);
			DiskCache::ext = ext;
			DiskCache::rootPath = path;
			DiskCache::encoder = "image/png";	// default
			
			if (ext.GetLength() >= 4)
			{
				CStringW s = ext.Mid(0, 4).MakeLower(); // try to guess it from input
				if (s == ".png")
				{
					DiskCache::encoder = "image/png";
				}
				else if (s == ".jpg")
				{
					DiskCache::encoder = "image/jpeg";
				}
			}
		}

		std::vector<CTilePoint*> points;
		for (int x = minX; x <= maxX; x++)
		{
			for (int y = minY; y <= maxY; y++)
			{
				if ((type == CacheType::SqliteCache && !SQLiteCache::get_Exists(provider, zoom, x, y)) || 
					 type == CacheType::DiskCache && !DiskCache::get_TileExists(zoom, x, y))
				{
					CTilePoint* pnt = new CTilePoint(x, y);
					pnt->dist = sqrt(pow((pnt->x - centX), 2.0) + pow((pnt->y - centY), 2.0));
					points.push_back(pnt);
				}
			}
		}

		if (points.size() > 0)
		{
			_prefetchLoader.doCacheSqlite = type == CacheType::SqliteCache;
			_prefetchLoader.doCacheDisk = type == CacheType::DiskCache;
			
			if (type == CacheType::SqliteCache)
			{
				SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists);
			}
			else 
			{
				DiskCache::CreateFolders(zoom, points);
			}
			_prefetchLoader.m_stopCallback = stop;

			if (_globalCallback)
			{
				_prefetchLoader.m_callback = this->_globalCallback;
			}
			
			// actual call to do the job
			_prefetchLoader.Load(points, zoom, provider, (void*)this, false, "", 0, true);

			for (size_t i = 0; i < points.size(); i++)
			{
				delete points[i];
			}
			return points.size();
		}
	}
	return 0;
}

// *********************************************************
//	     get_DiskCacheCount
// *********************************************************
STDMETHODIMP CTiles::get_DiskCacheCount(int provider, int zoom, int xMin, int xMax, int yMin, int yMax, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = SQLiteCache::get_TileCount(provider, zoom, xMin, xMax, yMin, yMax);
	return S_OK;
}

// *********************************************************
//	     CheckConnection
// *********************************************************
STDMETHODIMP CTiles::CheckConnection(BSTR url, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_provider != NULL)
	{
		USES_CONVERSION;
		*retVal = m_provider->CheckConnection(OLE2A(url)) ? VARIANT_TRUE: VARIANT_FALSE;
	}
	else {
		*retVal = VARIANT_FALSE;
	}
	return S_OK;
}

// *********************************************************
//	     get_TileBounds
// *********************************************************
STDMETHODIMP CTiles::GetTileBounds(int provider, int zoom, int tileX, int tileY, IExtents** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	BaseProvider* prov = ((CTileProviders*)_providers)->get_Provider(provider);
	if (!prov)
	{
		ErrorMessage(tkINVALID_PROVIDER_ID);
		return S_FALSE;
	}
	else
	{
		CSize size;
		prov->Projection->GetTileMatrixSizeXY(zoom, size);
		if (tileX < 0 || tileX > size.cx - 1 || tileY < 0 || tileY > size.cy - 1)
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else 
		{
			CPoint pnt1(tileX, tileY);
			CPoint pnt2(tileX + 1, tileY + 1);
			PointLatLng p1, p2;
			prov->Projection->FromXYToLatLng(pnt1, zoom, p1);
			prov->Projection->FromXYToLatLng(pnt2, zoom, p2);
			IExtents* ext;
			CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&ext);
			ext->SetBounds(p1.Lng, p1.Lat, 0.0, p2.Lng, p2.Lat, 0.0);
			*retVal = ext;
		}
	}
	return S_OK;
}

// ************************************************************
//		ProjectionSupportsWorldWideTransform
// ************************************************************
bool CTiles::ProjectionSupportsWorldWideTransform( IGeoProjection* mapProjection, IGeoProjection* wgsProjection )
{
	// let's check if map projection supports world wide transformation
	VARIANT_BOOL isGeograpic;		
	mapProjection->get_IsGeographic(&isGeograpic);
	if (isGeograpic)
	{
		return true;
	}
	else
	{
		double TOLERANCE = 0.00001;
		VARIANT_BOOL vb1, vb2;
		double minLng = -180.0, maxLng = 180.0, minLat = -85.05112878, maxLat = 85.05112878;
		double x1 = minLng, x2 = maxLng, y1 = minLat, y2 = maxLat;
		wgsProjection->Transform(&x1, &y1, &vb1);
		wgsProjection->Transform(&x2, &y2, &vb2);
		if (vb1 && vb2)
		{
			mapProjection->Transform(&x1, &y1, &vb1);
			mapProjection->Transform(&x2, &y2, &vb1);
			if (abs(x1 - minLng) <  TOLERANCE &&
				abs(x2 - maxLng) <  TOLERANCE &&
				abs(y1 - minLat) <  TOLERANCE &&
				abs(y2 - maxLat) <  TOLERANCE)
			{
				return true;
			}
		}
	}
	return false;
}

// ************************************************************
//		ProjectionBounds
// ************************************************************
bool CTiles::ProjectionBounds( BaseProvider* provider, IGeoProjection* mapProjection, IGeoProjection* wgsProjection, tkTransformationMode transformationMode, Extent& retVal )
{
	if (!wgsProjection || !provider || !provider->Projection)	return false;

	BaseProjection* proj = provider->Projection;
	
	if (proj && _projExtentsNeedUpdate)
	{
		double left =  proj->xMinLng;
		double right = proj->xMaxLng;
		double top = proj->yMaxLat;
		double bottom = proj->yMinLat;
		
		if (transformationMode == tmDoTransformation)	// i.e. map cs isn't in decimal degrees
		{
			// There is a problem if map projection isn't world wide (like Amersfoort for example).
			// Then values outside its bounds may not to be transformed correctly.
			// There is hardly any workaround here. Ideally we should know the bounds for map
			// projection and clip both by them and by bounds of server projection. Since bounds
			// of map projection aren't available partial solutions can be used:
			// - don't use clipping if map projection isn't world wide while server projection is
			// (which will obviously lead to server bounds outside transformation range).
			// Alternatives:
			// - doing some checks after transformation to make sure that calculations make sense;
			// - add a method to API to set bounds of map projection/tiles;
			// - store and update built-in database of bounds for different coordinate systems
			// and identify projection on setting it to map;
			bool supportsWorldWideTransform = ProjectionSupportsWorldWideTransform(mapProjection, wgsProjection);

			if (proj->worldWide && !supportsWorldWideTransform) // server projection is world wide, map projection - not
			{
				// so far just skip it;
				// optionally possible to transform to check if the results make sense
				return false;
			}
			else
			{
				VARIANT_BOOL vb;
				wgsProjection->Transform(&left, &top, &vb);
				if (!vb) {
					Debug::WriteLine("Failed to project: x = %f; y = %f", left, top);
					return false;
				}
				wgsProjection->Transform(&right, &bottom, &vb);
				if (!vb) {
					Debug::WriteLine("Failed to project: x = %f; y = %f", bottom, right);
					return false;
				}
			}
			//Debug::WriteLine("Projected world bounds: left = %f; right = %f; bottom = %f; top = %f", left, right, bottom, top);
		}
		
		_projExtents.left = left;
		_projExtents.right = right;
		_projExtents.top = top;
		_projExtents.bottom = bottom;
		_projExtentsNeedUpdate = false;
	}

	retVal.left = _projExtents.left;
	retVal.right = _projExtents.right;
	retVal.top = _projExtents.top;
	retVal.bottom = _projExtents.bottom;
	return true;
}

// ************************************************************
//		get_MaxZoom
// ************************************************************
STDMETHODIMP CTiles::get_MaxZoom(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_provider->maxZoom;
	return S_OK;
}

// ************************************************************
//		put_MinZoom
// ************************************************************
STDMETHODIMP CTiles::get_MinZoom(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_provider->minZoom;
	return S_OK;
}

// ************************************************************
//		ServerProjection
// ************************************************************
STDMETHODIMP CTiles::get_ServerProjection(tkTileProjection* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BaseProjection* p = m_provider->Projection;
	*retVal = p ? p->serverProjection : tkTileProjection::SphericalMercator;
	return S_OK;
}

// ************************************************************
//		ProjectionStatus
// ************************************************************
STDMETHODIMP CTiles::get_ProjectionStatus(tkTilesProjectionStatus* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = tkTilesProjectionStatus::tpsEmptyOrInvalid;

	CMapView* map = ((CMapView*)this->_mapView);
	if (map) {
		IGeoProjection* gp = map->GetMapProjection();
		IGeoProjection* gpServer = NULL;
		GetUtils()->TileProjectionToGeoProjection(m_provider->Projection->serverProjection, &gpServer);
		if (gp && gpServer)
		{
			VARIANT_BOOL vb;
			gp->get_IsSame(gpServer, &vb);
			if (vb) {
				*retVal = tkTilesProjectionStatus::tpsNative;
			}
			else
			{
				gpServer->StartTransform(gp, &vb);
				if (vb)
				{
					*retVal = tkTilesProjectionStatus::tpsCompatible;
					gpServer->StopTransform();
				}
			}
		}

		if (gpServer)
		{
			gpServer->Release();
			gpServer = NULL;
		}
	}
	return S_OK;
}

// ************************************************************
//		UpdateProjection
// ************************************************************
void CTiles::UpdateProjection()
{
	_projExtentsNeedUpdate = true;
	CMapView* map = (CMapView*)_mapView;
	if (map) {
		map->UpdateTileProjection();
	}
}


