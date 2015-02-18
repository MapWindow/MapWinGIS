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
#include "SQLiteCache.h"
#include "BaseProvider.h"

// init static members
sqlite3 *SQLiteCache::m_conn = NULL;
bool SQLiteCache::m_locked = false;
bool SQLiteCache::_createNeeded = true;
bool SQLiteCache::_openNeeded = true;
CStringW SQLiteCache::m_dbName;
double SQLiteCache::maxSizeDisk = 100.0;

::CCriticalSection SQLiteCache::section;

#define SIZE_TO_CLEAR 2 * 0x1 << 20	 // 20 MB; when overall size of tiles exceeds maximum, tiles will be removed 
									 // until this amount of memory is freed


// ***********************************************************
//		Initialize()
// ***********************************************************
// creates database opens connection
bool SQLiteCache::Initialize(SqliteOpenMode openMode)
{
	section.Lock();
	switch(openMode)
	{
		case SqliteOpenMode::OpenIfExists:
			// need to open exiting one
			if (_openNeeded)
			{
				CStringW s = get_DbName();
				if (Utility::FileExistsW(s))
				{
					SQLiteCache::CreateDatabase();
				}
				_openNeeded = false;
			}
			break;
		case SqliteOpenMode::OpenOrCreate:
			// need to open or create
			if (_createNeeded) {
				SQLiteCache::CreateDatabase();
				_createNeeded = false;
			}
	}
	section.Unlock();
	return m_conn != NULL;
}

// ***********************************************************
//		get_DbName()
// ***********************************************************
CStringW SQLiteCache::get_DbName()
{
	return m_dbName;
}

// ***********************************************************
//		set_DbName()
// ***********************************************************
bool SQLiteCache::set_DbName(CStringW name)
{
	if (name.MakeLower() != m_dbName.MakeLower())
	{
		m_dbName = name;
		SQLiteCache::CreateDatabase();
	}
	return true;
}

// ***********************************************************
//		get_DefaultDbName()
// ***********************************************************
CStringW SQLiteCache::get_DefaultDbName()
{
	wchar_t* path = new wchar_t[MAX_PATH + 1];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	CStringW name = Utility::GetFolderFromPath(path);
	name += L"\\";
	name += DB_NAME;
	delete[] path;
	return name;
}

// ***********************************************************
//		CreateDatabase()
// ***********************************************************
bool SQLiteCache::CreateDatabase()
{
	if (m_dbName.GetLength() == 0) {
		m_dbName = get_DefaultDbName();
	}
	
	USES_CONVERSION;
	CString name = W2A(m_dbName);	// TODO: use Unicode
	
	if (m_conn)
	{
		int val = sqlite3_close(m_conn);
		m_conn = NULL;
	}

	bool ret = false;
	int val = sqlite3_open_v2(name, &m_conn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE| SQLITE_OPEN_FULLMUTEX, NULL);
	if (!val)
	{
		char* sql = "CREATE TABLE IF NOT EXISTS Tiles (id INTEGER NOT NULL PRIMARY KEY, X INTEGER NOT NULL, "
					"Y INTEGER NOT NULL, Zoom INTEGER NOT NULL, Type INTEGER NOT NULL, Size INTEGER NOT NULL, CacheTime DATETIME );";
		val = sqlite3_exec(m_conn, sql, NULL, NULL, NULL);
		
		if (!val)
		{
			CString s;
			s = "CREATE TRIGGER IF NOT EXISTS set_created AFTER INSERT ON Tiles "
				"BEGIN "
				"UPDATE Tiles SET cacheTime = DATETIME('NOW') WHERE rowid = new.rowid; "
				"END;";

			val = sqlite3_exec(m_conn, s, NULL, NULL, NULL);
			if (!val)
			{
				sql = "CREATE TABLE IF NOT EXISTS TilesData (id INTEGER NOT NULL PRIMARY KEY CONSTRAINT fk_Tiles_id "
					  "REFERENCES Tiles(id) ON DELETE CASCADE, Tile BLOB NULL);";
				val = sqlite3_exec(m_conn, sql, NULL, NULL, NULL);
				
				if (!val)
				{
					sql = "CREATE TRIGGER IF NOT EXISTS tiles_cascade_delete "
						  "BEFORE DELETE ON Tiles "
						  "FOR EACH ROW BEGIN "
						  "DELETE from TilesData WHERE TilesData.Id = OLD.id; "
						  "END;";

					return !sqlite3_exec(m_conn, sql, NULL, NULL, NULL);
				}
			}
		}
	}
	return ret;
}

// ***********************************************************
//		DoCaching()
// ***********************************************************
// Pushes a single tile in the database. Called by the worker thread
void SQLiteCache::DoCaching(TileCore* tile)
{
	static int addedCount = 0;
	static int percent = 0;

	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return;
	
	section.Lock();

	try
	{
		const char   *tail;
		sqlite3_stmt *stmt;
		for (size_t i = 0; i < tile->Overlays.size(); i++)
		{
			CMemoryBitmap* bmp = tile->getBitmap(i);
			if (bmp)
			{
				int size = bmp->get_Size();
				if (size > 0)
				{
					CString s = "REPLACE INTO Tiles VALUES (?, ?, ?, ?, ?, ?, ?)";
					int val = sqlite3_prepare_v2(m_conn, s, s.GetLength()+1, &stmt, &tail);
					
					if (val != SQLITE_OK)
					{
						CallbackHelper::ErrorMsg("SQLiteCache::DoCaching: Failed to prepare statement.");
					}
					else
					{
						val = sqlite3_bind_int(stmt, 2, tile->m_tileX);
						val = sqlite3_bind_int(stmt, 3, tile->m_tileY);
						val = sqlite3_bind_int(stmt, 4, tile->m_scale);
						val = sqlite3_bind_int(stmt, 5, bmp->Provider);
						val = sqlite3_bind_int(stmt, 6, bmp->get_Size());
						
						val = sqlite3_step(stmt);
						sqlite3_finalize(stmt);
						
						s = "REPLACE INTO TilesData VALUES (?, ?)";
						val = sqlite3_prepare_v2(m_conn, s, s.GetLength()+1, &stmt, &tail);
						sqlite3_int64 id = sqlite3_last_insert_rowid(m_conn);
						val = sqlite3_bind_int64(stmt, 1, id);
						
						void* data = (void*)new char[size];
						void* hMem = ::GlobalLock(bmp->getData());
						memcpy(data, hMem, size);
						::GlobalUnlock(hMem);

						val = sqlite3_bind_blob(stmt, 2, data, size, NULL);
						val = sqlite3_step(stmt);
						
						if (val == SQLITE_OK || val == SQLITE_DONE)
						{
							addedCount++;
						}

						delete[] data;
						sqlite3_finalize(stmt);
					}
				}
			}
		}
	}
	catch(...)
	{
		CallbackHelper::ErrorMsg("SQLiteCache: exception on adding tiles.");
	}
	section.Unlock();
	
	tile->Release();

	// check if the database grew enough to run the cleaning routine
	if (addedCount % 25 == 0)
	{
		AutoClear();
	}
}

// ***********************************************************
//		AutoClear() 
// ***********************************************************
// Removes the oldest tiles if the size of the db is exceeding the limit
void SQLiteCache::AutoClear()
{
	if (SQLiteCache::get_FileSize() > SQLiteCache::maxSizeDisk)
	{
		try 
		{
			section.Lock();
			CString sql;
			sql = "SELECT Size, CacheTime FROM Tiles ORDER BY CacheTime ASC";
			const char   *tail;
			sqlite3_stmt *stmt;

			int val = sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt, &tail);
			
			CString time;
			int size = 0;
			while( sqlite3_step(stmt) == SQLITE_ROW) 
			{
				size += sqlite3_column_int(stmt, 0);
				if (size > SIZE_TO_CLEAR)
				{
					const unsigned char* s = sqlite3_column_text(stmt, 1);
					time.Format("%s", s);
					break;
				}
			}
			val = sqlite3_finalize(stmt);
			
			// now delete it
			if (time.GetLength() > 0)
			{
				sql.Format("DELETE FROM Tiles WHERE CacheTime > '%s'", time);
			}
			else
			{
				sql = "DELETE FROM Tiles";
			}
			
			val = sqlite3_exec(m_conn, sql, NULL, NULL, NULL);
			section.Unlock();
		}
		catch(...)
		{
			CallbackHelper::ErrorMsg("SQLiteCache: exception on clearing the tiles.");
		}
	}
}

// ***********************************************************
//		get_Exists() 
// ***********************************************************
bool SQLiteCache::get_Exists(BaseProvider* provider, LONG scale, LONG x, LONG y)
{
	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return false;

	const char   *tail;
	sqlite3_stmt *stmt;
	bool exists = true;

	try
	{
		for(size_t i = 0; i < provider->subProviders.size(); i++)
		{
			int providerId = provider->subProviders[i]->Id;

			CString sql;
			sql.Format("SELECT id FROM Tiles WHERE X = %d AND Y = %d AND Zoom = %d AND Type = %d", x, y, scale, provider->Id);
			int val = sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt, &tail);
			
			section.Lock();
			if( sqlite3_step(stmt) != SQLITE_ROW)
			{
				exists = false;
			}
			val = sqlite3_finalize(stmt);
			section.Unlock();

			if (!exists)
				break;
		}
		
	}
	catch (...)
	{
		exists = false;
	}

	return exists;
}


// ***********************************************************
//		getTile() 
// ***********************************************************
// Extracts a tile from the database
TileCore* SQLiteCache::get_Tile(BaseProvider* provider, LONG scale, LONG x, LONG y)
{
	TileCore* tile = NULL;
	
	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return NULL;

	section.Lock();

	const char   *tail;
	sqlite3_stmt *stmt;

	try
	{
		for(size_t i = 0; i < provider->subProviders.size(); i++)
		{
			int providerId = provider->subProviders[i]->Id;

			CString sql;
			sql.Format("SELECT id FROM Tiles WHERE X = %d AND Y = %d AND Zoom = %d AND Type = %d", x, y, scale, providerId);
			if (sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt, &tail) == SQLITE_OK)
			{
				if( sqlite3_step(stmt) == SQLITE_ROW) {
					
					const char   *tail2;
					sqlite3_stmt *stmt2;
					sqlite3_int64 id = sqlite3_column_int64(stmt, 0);
					sql.Format("SELECT tile FROM TilesData WHERE Id = %d", id);
					
					if (sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt2, &tail2) == SQLITE_OK)
					{
						if (sqlite3_step(stmt2) == SQLITE_ROW)
						{
							const void* data = sqlite3_column_blob(stmt2, 0);
							int size = sqlite3_column_bytes(stmt2, 0);
							
							if (size > 0)
							{
								CMemoryBitmap* bmp = new CMemoryBitmap();
								if (bmp->LoadFromRawData((const char*)data, size))
								{
									if (i == 0)
										tile = new TileCore(providerId, scale, CPoint(x, y), provider->Projection);

									if (tile)
										tile->AddBitmap(bmp);
									else
										delete bmp;
								}
								else
								{
									delete bmp;
								}
							}
						}
						sqlite3_finalize(stmt2);
					}
				}
				sqlite3_finalize(stmt);
			}
		}

		// have we found all the overlays? If not - request from server ones more
		if (tile && tile->Overlays.size() != provider->subProviders.size())
		{
			delete tile;
			tile = NULL;
		}

		// for composite providers
		if (tile)
			tile->m_providerId = provider->Id;
	}
	catch(...)
	{
		CallbackHelper::ErrorMsg("SQLiteCache: exception on getting tile.");
	}
	section.Unlock();
	return tile;
}

// ****************************************************************
//		Clear()
// ****************************************************************
void SQLiteCache::Clear(int providerId, int fromScale, int toScale)
{
	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return;
	
	// there is no need to delete from tilesdata table as there is ON CASCADE DELETE rule specified in foreign key constraint
	// updated: in fact there is a trigger which does the job
	CString temp;
	CString sql = "DELETE FROM TILES";
	
	if (providerId != (int)tkTileProvider::ProviderNone || fromScale != 0 || toScale != 100)
		sql += " WHERE ";
	
	if (providerId != (int)tkTileProvider::ProviderNone)
	{
		temp.Format(" Type = %d", providerId);
		sql += temp;
	}
	
	if (fromScale != 0 || toScale != 100)
	{
		if (temp.GetLength() > 0)
			sql += " AND ";

		temp.Format(" Zoom >= %d AND Zoom <= %d ", fromScale, toScale);
		sql += temp;
	}
	
	section.Lock();
	int val = sqlite3_exec(m_conn, sql, NULL, NULL, NULL);
	section.Unlock();
}

// ****************************************************************
//		get_FileSize()
// ****************************************************************
double SQLiteCache::get_FileSize() 
{	
	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return 0.0;
	
	const char   *tail;
	sqlite3_stmt *stmt;

	int size = 0;

	CString sql = "SELECT SUM(size) FROM Tiles";
	int val = sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt, &tail);
	
	if (val != SQLITE_OK)
	{
		CallbackHelper::ErrorMsg(Debug::Format("SQLiteCache::get_FileSize: Failed to prepare statement; %d.", val));
	}
	else
	{
		section.Lock();

		if( sqlite3_step(stmt) == SQLITE_ROW)
			size = sqlite3_column_int(stmt, 0);
		section.Unlock();	
		val = sqlite3_finalize(stmt);
	}
	return (double)size/(double)(0x1 << 20);
}

// ****************************************************************
//		get_FileSize()
// ****************************************************************
double SQLiteCache::get_FileSize(tkTileProvider provider, int scale) 
{
	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return 0.0;
	
	int size = 0;
	double result = 0.0;
	const char   *tail;
	sqlite3_stmt *stmt;

	try
	{
		CString sql = "SELECT size FROM Tiles";
		if (provider != tkTileProvider::ProviderNone || scale != -1)
			sql += " WHERE ";
		
		CString temp;
		if (provider != tkTileProvider::ProviderNone)
		{
			temp.Format("Type = %d", provider);
			sql += temp;
		}

		if (scale != -1)
		{
			if (temp.GetLength() > 0)
				sql += " AND ";
			temp.Format("Zoom = %d", scale);
			sql += temp;
		}

		int val = sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt, &tail);
		
		while( sqlite3_step(stmt) == SQLITE_ROW) 
		{
			size += sqlite3_column_int(stmt, 0);
		}
		val = sqlite3_finalize(stmt);
	}
	catch(...)
	{
		CallbackHelper::ErrorMsg("SQLiteCache: exception on getting file size.");
	}
	return (double)size/(double)(0x1 << 20);
}

// ****************************************************************
//		get_Count()
// ****************************************************************
int SQLiteCache::get_TileCount(int provider, int zoom, int xMin, int xMax, int yMin, int yMax) 
{
	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return 0;

	const char   *tail;
	sqlite3_stmt *stmt;

	try
	{
		CString format = "SELECT count(*) FROM Tiles Where Type = %d and Zoom = %d and X >= %d and "
						 "X <= %d and Y >= %d and Y <= %d";
		
		CString sql;
		sql.Format(format, provider, zoom, xMin, xMax, yMin, yMax);
	
		int val = sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt, &tail);

		if (!val)
		{
			int size = 0;
			
			section.Lock();	
			if( sqlite3_step(stmt) == SQLITE_ROW)
				size = sqlite3_column_int(stmt, 0);
			val = sqlite3_finalize(stmt);
			section.Unlock();

			return size;
		}
	}
	catch(...)
	{
		CallbackHelper::ErrorMsg("Exception in SQLiteCache::get_TileCount.");
	}
	return 0;
}

// ****************************************************************
//		get_TilesXY()
// ****************************************************************
bool SQLiteCache::get_TilesXY(int provider, int zoom, int xMin, int xMax, int yMin, int yMax, std::list<CPoint*>& list) 
{
	if(!SQLiteCache::Initialize(SqliteOpenMode::OpenIfExists))
		return false;

	const char   *tail;
	sqlite3_stmt *stmt;

	try
	{
		CString format = "SELECT X, Y FROM Tiles Where Type = %d and Zoom = %d and X >= %d and "
						 "X <= %d and Y >= %d and Y <= %d";
		
		CString sql;
		sql.Format(format, provider, zoom, xMin, xMax, yMin, yMax);

		section.Lock();
		int val = sqlite3_prepare_v2(m_conn, sql, sql.GetLength() + 1, &stmt, &tail);
		
		while( sqlite3_step(stmt) == SQLITE_ROW)
		{
			int x = sqlite3_column_int(stmt, 0);
			int y = sqlite3_column_int(stmt, 1);
			list.push_back(new CPoint(x, y));
		}
		val = sqlite3_finalize(stmt);
		section.Unlock();

		return true;
	}
	catch(...)
	{
		CallbackHelper::ErrorMsg("Exception on SQLiteCache::get_TilesXY.");
	}
	return false;
}