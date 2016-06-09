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
#include "TileCacheManager.h"
#include "DiskCache.h"
#include "RamCache.h"
#include "SQLiteCache.h"

ITileCache* TileCacheManager::_ramCache = NULL;
ITileCache* TileCacheManager::_sqlLiteCache = NULL;
ITileCache* TileCacheManager::_diskCache = NULL;
::CCriticalSection TileCacheManager::_lock;

// ***********************************************************
//		get_Cache()
// ***********************************************************
ITileCache* TileCacheManager::get_Cache(CacheType type)
{
	CSingleLock lock(&_lock, TRUE);

	switch (type) {
		case tctDiskCache:
			// we can't create an instance without path
			return NULL;
		case tctRamCache:
			if (!_ramCache) {
				_ramCache = new RamCache();
			}
			return _ramCache;
		case tctSqliteCache:
			if (!_sqlLiteCache) {
				_sqlLiteCache = new SQLiteCache();
			}
			return _sqlLiteCache;
	}

	return NULL;
}

// ***********************************************************
//		CloseAll()
// ***********************************************************
void TileCacheManager::CloseAll()
{
	if (_ramCache) {
		_ramCache->Close();
	}

	if (_ramCache) {
		_ramCache->Close();
	}

	if (_diskCache) {
		_diskCache->Close();
	}
}
