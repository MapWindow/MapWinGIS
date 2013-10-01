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

#pragma once
#include <afxmt.h>
#include <queue>
#include <list>
#include "sqlite3.h"
#include "TileCore.h"
#include "TileCacher.h"
#include "BaseProvider.h"

#define DB_NAME "mwtiles.db3"

// Provides storage for map tiles in SQLite database
class SQLiteCache
{
	static CString m_dbName;
	static sqlite3 *m_conn;
public:
	static ::CCriticalSection section;  // finer locks to prevent simultaneous access of the same operation from several threads
	static bool m_initNeeded;	// to create a database
	static bool m_locked;		// coarse lock to block the adding tile to cache when extracting operations are made
	static double maxSizeDisk;	// max size of disk cache in megabytes
	
	static CString get_DbName();
	static bool set_DbName(CString name);
	static bool CreateDatabase();
	
	static TileCore* get_Tile(BaseProvider* provider, LONG scale, LONG x, LONG y);
	static void Clear(int providerId = -1, int fromScale = 0, int toScale = 100);
	static double get_FileSize();
	
	static double get_FileSize(tkTileProvider provider, int scale);
	static void AutoClear();
	static int get_TileCount(int provider, int zoom, int xMin, int xMax, int yMin, int yMax);
	static bool get_TilesXY(int provider, int zoom, int xMin, int xMax, int yMin, int yMax, std::list<CPoint*>& list);
	static bool get_Exists(BaseProvider* provider, LONG scale, LONG x, LONG y);

	static void ProcessQueue();
	static void DoCaching(TileCore* tile);
	
	// closes db connection
	static void Close()
	{
		if (m_conn)
		{
			sqlite3_close(m_conn);
			m_conn = NULL;
		}
	}

	// creates database opens connection
	static void Initialize()
	{
		section.Lock();
		if (m_initNeeded) {
			SQLiteCache::CreateDatabase();
			m_initNeeded = false;
			
		}
		section.Unlock();
	}
	
	// to monitor potential treed conflicts
	static void PrintThread(CString msg)
	{
		//Debug::WriteLine("Thread id: %d; %s", GetCurrentThreadId(), msg);
	}
};

	