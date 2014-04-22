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
#include "afxmt.h"
#include "RamCache.h"

// init static members
TilesCache RamCache::m_tilesMap;
vector<TileCore*> RamCache::m_list;
int RamCache::m_size = 0;
double RamCache::m_maxSize = 100.0;

::CCriticalSection m_section;

#define SIZE_TO_CLEAR (20 * 0x1 << 20)	// 20 MB; when overall size of tiles exceeds maximum, tiles will be removed 
									    // starting from the oldest until this amount of memory is freed

// *********************************************************
//	     Close()
// *********************************************************
void RamCache::Close()
{
	// release tiles first
	RamCache::ClearByProvider();
	
	// then release dynamically allocated containers
	for( TilesCache::iterator it = m_tilesMap.begin(); it != m_tilesMap.end(); ++it ) 
	{
		vector<TilePoints*>* list = it->second;
		if (list)
		{
			for (size_t i = 0; i< (int)list->size(); i++)
			{
				TilePoints* points = (*list)[i];
				if (points)
				{
					TilePoints::iterator it2 = points->begin();
					while (it2 != points->end())
					{
						TilePositions* tiles = it2->second;
						if (tiles) {
							delete tiles;
						}
						++it2;
					}
					delete points;
				}
			}
			delete list;
		}
    }
}

// *********************************************************
//	     AddToCache()
// *********************************************************
void RamCache::AddToCache(TileCore* tile)
{
	if (tile->m_scale < 0 || tile->m_scale >= 25) {
		return;
	}

	m_section.Lock();

	// seeking provider
	std::vector<TilePoints*>* list = NULL;
	if (m_tilesMap.find(tile->m_providerId) != m_tilesMap.end())
	{
		list = m_tilesMap[tile->m_providerId];
	}
	else
	{
		list = new std::vector<TilePoints*>();
		list->resize(25, NULL);
		m_tilesMap[tile->m_providerId] = list;
	}

	// seeking scale
	TilePoints* points = (*list)[tile->m_scale];
	if (!points)
	{
		points = new TilePoints();
		(*list)[tile->m_scale] = points;
	}
	
	// seeking X
	std::map<int, TileCore*>* map = NULL;
	if (points->find(tile->m_tileX) != points->end())
	{
		map = (*points)[tile->m_tileX];
	}
	else
	{
		map = new std::map<int, TileCore*>();
		(*points)[tile->m_tileX] = map;
	}

	// seeking Y
	if (map->find(tile->m_tileY) == map->end())
	{
		tile->AddRef();
		(*map)[tile->m_tileY] = tile;
		m_size += tile->getSize();
		m_list.push_back(tile);
	}
	m_section.Unlock();
	
	// automatically clear the cache if it exceeds the maximum size
	if ((double)m_size/(double)(0x1 << 20) > m_maxSize)
	{
		ClearOldest(SIZE_TO_CLEAR);
	}
}

// *********************************************************
//	     ClearOldest()
// *********************************************************
void RamCache::ClearOldest(int sizeToClearBytes)
{
	int size = 0;
	for(size_t i = 0; i < m_list.size(); i++)
	{
		if (!m_list[i]->m_inBuffer) {
			m_list[i]->m_toDelete = true;
			size += m_list[i]->getSize();
			if (size > SIZE_TO_CLEAR)
				break;
		}
	}
	DeleteMarkedTiles();
}

// **********************************************************
//		ClearAll()
// **********************************************************
// Clears tiles of all providers
void RamCache::ClearAll(int fromScale, int toScale)
{
	ClearByProvider(-1, fromScale, toScale);
}

// **********************************************************
//		ClearByProvider()
// **********************************************************
// Removes tiles of the specified provider
void RamCache::ClearByProvider(int provider, int fromScale, int toScale)
{
	m_section.Lock();
	for( TilesCache::iterator it = m_tilesMap.begin(); it != m_tilesMap.end(); ++it ) 
	{
		// if provider is equal -1, then all tiles are to be removed
		if (provider != (int)tkTileProvider::ProviderNone && provider != it->first)
			continue;

		vector<TilePoints*>* list = it->second;
		if (list)
		{
			for (int i = 0; i< (int)list->size(); i++)
			{
				if (i < fromScale || i > toScale)
					continue;
				
				TilePoints* points = (*list)[i];
				if (points)
				{
					TilePoints::iterator it2 = points->begin();
					while (it2 != points->end())
					{
						TilePositions* tiles = it2->second;
						TilePositions::iterator it3 = tiles->begin();
						
						while(it3 != tiles->end())
						{
							TileCore* tile = it3->second;
							
							if (tile->m_inBuffer)
							{
								++it3;	// it doesn't make sense to delete tiles which are 
										// currently displayed; as they will be requested again on simple refresh of the map
							}
							else
							{
								tiles->erase(it3++);
								m_size -= tile->getSize();
								
								// marks it to be removed from chronological vector
								tile->m_toDelete = true;
							}
						}
						
						// if all the Y's for given X are removed - delete the container
						if (tiles->empty())
						{
							points->erase(it2++);
							delete tiles;
						}
						else
							++it2;
					}

					// if all the X's for a given scale are removed - delete the container,
					// but don't delete scale entry - 25 scales must preserved as they are accessed by index
					if (points->empty())
					{
						delete points;
						(*list)[i] = NULL;
					}
				}
			}
		}
    }
	
	// now remove tiles from chronological vector
	DeleteMarkedTiles();
	
	m_section.Unlock();
}

// *********************************************************
//		DeleteMarkedTiles()
// *********************************************************
// Removes tiles marked as "to delete" from chronological vector.
// The tiles should removed from fast access storage before this operation.
// It's done by either calling ClearAll(), ClearByProvider() or ClearOldest.
// It's resposibility of caller to lock the the cache to make it thread safe
void RamCache::DeleteMarkedTiles()
{
	vector<TileCore*>::iterator it = m_list.begin();
	while (it != m_list.end())
	{
		TileCore* tile = *it;
		if (tile->m_toDelete)
		{
			tile->Release();
			it = m_list.erase(it);
		}
		else
		{
			++it;
		}
	}
}

// ********************************************************
//		get_TileCore()
// ********************************************************
// Extracts a single tile from the cache
TileCore* RamCache::get_Tile(int providerId, int scale, int tileX, int tileY)
{
	m_section.Lock();
	TileCore* tile = NULL;
	if (m_tilesMap.find(providerId) != m_tilesMap.end())
	{
		std::vector<TilePoints*>* scales = m_tilesMap[providerId];
		if (scale >= 0 && scale < (int)scales->size())
		{
			TilePoints* points = (*scales)[scale];
			if (points != NULL)
			{
				if (points->find(tileX) != points->end())
				{
					std::map<int, TileCore*>* map = (*points)[tileX];
					if (map->find(tileY) != map->end())
					{
						tile = ((*map)[tileY]);
						
					}
				}
			}
		}
	}
	m_section.Unlock();
	return tile;
}

// ***********************************************************
//		get_Size()
// ***********************************************************
// Use -1 for provider and scale to retrieve all the values
double RamCache::get_MemorySize(int provider, int scale)
{
	m_section.Lock();
	int sum = 0;
	for( TilesCache::iterator it = m_tilesMap.begin(); it != m_tilesMap.end(); ++it ) 
	{
		if (provider != (int)tkTileProvider::ProviderNone && provider != it->first)
			continue;
		
		std::vector<TilePoints*>* scales = it->second;
		for (size_t i = 0; i < scales->size(); i++)
		{
			if (scale != -1 && i != scale)
				continue;

			TilePoints* points = (*scales)[i];
			if (points)
			{
				for( TilePoints::iterator it2 = points->begin(); it2 != points->end(); ++it2 ) 
				{
					TilePositions* tiles = it2->second;
					if (tiles)
					{
						for( TilePositions::iterator it3 = tiles->begin(); it3 != tiles->end(); ++it3 ) 
						{
							TileCore* tile = it3->second;
							sum += tile->getSize();
						}
					}
				}
			}
		}
	}
	m_section.Unlock();
	return (double)sum/(double)(0x1 << 20);
}
