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
#include <vector>
#include <map>
#include "TileCore.h"
using namespace std;

// provider id -> zoom -> x -> y
typedef std::map<int, TileCore*>					TilePositions;   // indexed by y
typedef std::map<int, TilePositions*>				TilePoints;		 // indexed by x
typedef std::map<int, std::vector<TilePoints*>*>	TilesCache;		 // indexed by provider; indices in vector - by zoom 

// Provides storage for map tiles in RAM
class RamCache
{
private:	
	static int m_size;		// size of cache in bytes
	static void DeleteMarkedTiles();	// deletes tiles from chronological vector
public:
	static double m_maxSize;			// max size in megabytes
	static TilesCache m_tilesMap;		// the main storage
	static vector<TileCore*> m_list;	// the list of tiles in the chronological order

	static void AddToCache(TileCore* tile);
	static TileCore* get_Tile(int provider, int scale, int tileX, int tileY);
	static double get_MemorySize(int provider = -1, int scale = -1);
	static void ClearAll(int fromScale, int toScale);
	static void ClearByProvider(int provider = -1, int fromScale = 0, int toScale = 100);
	static void ClearOldest(int sizeToClearBytes);
	static void Close();
};
