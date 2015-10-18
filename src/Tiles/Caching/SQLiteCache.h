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
#include <afxmt.h>
#include <queue>
#include <list>
#include "sqlite3.h"
#include "ITileCache.h"

#define DB_NAME L"mwtiles.db3"

// Provides storage for map tiles in SQLite database
class SQLiteCache: public ITileCache
{
public:
	SQLiteCache()
		:_conn(NULL), _locked(false), _createNeeded(true), _openNeeded(true)
	{
		_maxSize = 100.0;
	}

private:
	CStringW _dbName;
	sqlite3 *_conn;
	::CCriticalSection _section;  // finer locks to prevent simultaneous access of the same operation from several threads
	bool _createNeeded;
	bool _openNeeded;
	bool _locked;		// coarse lock to block the adding tile to cache when extracting operations are made

private:
	CStringW get_DefaultDbName();
	bool CreateDatabase();
	void AutoClear();
	bool get_TilesXY(int provider, int zoom, int xMin, int xMax, int yMin, int yMax, std::list<CPoint*>& list);
	void ProcessQueue();

public:	
	// interface
	bool IsLocked() { return _locked; }
	void Close();
	void AddTile(TileCore* tile);
	TileCore* get_Tile(BaseProvider* provider, int scale, int x, int y);
	bool get_TileExists(BaseProvider* provider, int scale, int x, int y);
	CacheType get_CacheType() { return tctSqliteCache; }
	void Clear(int providerId = -1, int fromScale = 0, int toScale = 100);
	double get_SizeMB(int providerId, int scale);
	double get_SizeMB();
	CStringW get_Filename() { return get_DbName(); }
	void set_Filename(CStringW name)  { set_DbName(name); }
	virtual void Initialize(bool canUseCache, bool canDoCaching);
	void Lock() { _locked = true; }
	void Unlock() { _locked = false; }
	void InitBulkDownload(int zoom, vector<TilePoint*>& points) { Initialize(SqliteOpenMode::OpenIfExists); }

public:
	// properties:
	CStringW get_DbName();
	bool set_DbName(CStringW name);
	long get_TileCount(int provider, int zoom, CRect indices);
public:	
	// methods
	bool Initialize(SqliteOpenMode openMode);
};

	