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
#include "BaseProvider.h"
#include "TileLoader.h"

class TileManager
{
public:
	TileManager()
	{
		_useDiskCache = false;
		_doDiskCaching = false;

		// TODO: initialize members
	}

private:
	// to avoid duplicate consecutive requests
	::CCriticalSection m_tilesBufferLock;
	vector<TileCore*> m_tiles;
	TileLoader _tileLoader;
	IMapViewCallback* _map;		// TODO: assign

	Extent _projExtents;			// extents of the world under current projection; in WGS84 it'll be (-180, 180, -90, 90)
	bool _projExtentsNeedUpdate;	// do we need to update bounds in m_projExtents on the next request?

	CRect _lastTileExtents;	// in tile coordinates
	Extent _lastMapExtents;
	int _lastZoom;
	int _lastProvider;

	bool _useDiskCache;
	bool _doDiskCaching;
	bool _useRamCache;
	bool _doRamCaching;
	bool _useServer;

private:
	void LoadTiles(BaseProvider* provider, bool isSnapshot, CString key);
	void Clear();
	void BuildLoadingList(BaseProvider* provider, CRect indices, int zoom, vector<CTilePoint*>& activeTasks, vector<CTilePoint*>& points);
	void GetActiveTasks(std::vector<CTilePoint*>& activeTasks, int providerId, int zoom, int generation, CRect indices);
	bool IsNewRequest(Extent& mapExtents, CRect indices, int providerId, int zoom);
	void ClearBuffer();
	void DeleteMarkedTilesFromBuffer();
	void InitializeDiskCache();
	void ReleaseMemory(vector<CTilePoint*> points);
	void AddTileWithCaching(TileCore* tile);
	void AddTileNoCaching(TileCore* tile);
	void AddTileOnlyCaching(TileCore* tile);
	void AddTileToRamCache(TileCore* tile);
	void UnlockDiskCache();
	bool GetTileIndices(BaseProvider* provider, CRect& indices, int& zoom);
};