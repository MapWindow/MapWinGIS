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
#include "InMemoryBitmap.h"
#include "GeoPoint.h"
#include "BaseProjection.h"

// ******************************************************
//    TileIdentity
// ******************************************************
// Info about tile request (map display, snapshot or caching) to be returned 
// to the caller via callback
struct TileRequestInfo
{
	TileRequestInfo(CString key, bool isSnapshot)
		: count(0), totalCount(10000), key(key), isSnapshot(isSnapshot), generation(-1)
	{
	}

	CString key;
	bool isSnapshot;	
	int totalCount;
	int count;
	int generation;

	bool Completed() { return totalCount == count; }
};



// ******************************************************
//    TilePoint
// ******************************************************
// A point with X, Y coordinates and distance from center of screen
class TilePoint : public CPoint
{
public:
	TilePoint(int x, int y)
		: CPoint(x, y), dist(0.0)
	{
	}

public:
	double dist;

public:
	static void ReleaseMemory(vector<TilePoint*>& points)
	{
		for (size_t i = 0; i < points.size(); i++) {
			delete points[i];
		}
	}
};

// ******************************************************
//    TileCore
// ******************************************************
// Represents a single map tile - image or several overlayed images
// with specified zoom and X, Y coordinates, usually with size 256 by 256 pixels
class TileCore
{
public:
	TileCore(int providerId, int zoom, CPoint& pnt, BaseProjection* projection)
		: _scale(zoom), _tileX(pnt.x), _tileY(pnt.y), _providerId(providerId), _projection(projection)
	{
		_refCount = 0;
		_projectionChangeCount = -1;
		_hasErrors = false;
		_drawn = false;
		_toDelete = false;
		_geogBounds = projection->CalculateGeogBounds(pnt, zoom);
	}

	virtual ~TileCore()
	{
		ClearOverlays();
	}

private:
	// a tile can be deleted while:
	// 1) clearing drawing buffer: Tiles::Clear()
	// 2) clearing RAM cache: RAMCache::Clear()
	// 3) after disk caching: SQLiteCache::DoCaching()
	long _refCount;		// number of references (it can be used in drawing, RAM cache or be scheduled for disk caching)
	bool _hasErrors;	// there were errors during download, one of several layers weren't loaded
	long _scale;
	long _tileX;
	long _tileY;
	int _projectionChangeCount;	// position of tile was recalculated using the current projection; number of projection changes
	BaseProjection* _projection;
	RectLatLng _projectedBounds;
	RectLatLng _geogBounds;
	bool _drawn;			// it's drawn on screen
	bool _toDelete;		// for inner working of RAM cache
	bool _inBuffer;		// it's currently displayed or scheduled to be displayed; it must not be destroyed while cleaning the cache
	int _providerId;

public:
	// a tile may be comprised of several semi-transparent bitmaps (e.g. satellite image and labels above it)
	vector<CMemoryBitmap*> Overlays;

public:
	// properties
	int get_ProjectionChangeCount() { return _projectionChangeCount; }
	RectLatLng* get_ProjectedBounds() { return &_projectedBounds; }
	RectLatLng* get_GeographicBounds() { return &_geogBounds; }
	BaseProjection* get_Projection() { return _projection; }
	void set_Projection(BaseProjection* projection) { _projection = projection; }
	CStringW get_Path(CStringW root, CStringW ext);
	CMemoryBitmap* get_Bitmap(int overlayIndex);
	int get_ByteSize();
	bool IsEmpty() 	{ return Overlays.size() == 0; }
	bool hasErrors() { return _hasErrors; }
	void hasErrors(bool value) { _hasErrors = true; }
	int tileX() { return _tileX; }
	int tileY() { return _tileY; }
	int zoom() { return _scale; }
	int get_ProviderId() { return _providerId; }
	void set_ProviderId(int value) { _providerId = value; }
	bool inBuffer() { return _inBuffer; }
	void inBuffer(bool value) { _inBuffer = value; }
	bool isDrawn() { return _drawn; }
	void isDrawn(bool value) { _drawn = value; }
	bool toDelete() { return _toDelete; }
	void toDelete(bool value) { _toDelete = value; }

public:
	//methods
	void AddOverlay(CMemoryBitmap* bitmap) { Overlays.push_back(bitmap);	}
	void ClearOverlays();
	long AddRef();
	long Release();

	void UpdateProjectedBounds(RectLatLng bounds, int projectionChangeCount) {
		_projectedBounds = bounds;
		_projectionChangeCount = projectionChangeCount;
	}

public:
	// operators
	bool TileCore::operator==(const TileCore &t2);
};

