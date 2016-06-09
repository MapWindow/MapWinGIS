#include "stdafx.h"
#include "MapLoadingTask.h"
#include "TileManager.h"
#include "TileMapLoader.h"

// *******************************************************
//		BeforeRequest()
// *******************************************************
void MapLoadingTask::BeforeRequest()
{
	TileMapLoader* loader = dynamic_cast<TileMapLoader*>(_loader);
	if (loader) {
		loader->AddActiveTask(this);
	}
}

// *******************************************************
//		AfterRequest()
// *******************************************************
void MapLoadingTask::AfterRequest(TileCore* tile)
{
	TileMapLoader* loader = dynamic_cast<TileMapLoader*>(_loader);
	if (!loader) return;

	if (loader->isStopped() || tile->IsEmpty())
	{
		loader->RemoveActiveTask(this);
		return;
	}
	
	TileManager* manager = (TileManager*)_provider->get_Manager();

	// quickly pass it from active list to cache; so it's always avaiable while building the next list of requests
	loader->LockActiveTasks(true);

	manager->AddTileToRamCache(tile);
	loader->RemoveActiveTask(this);

	loader->LockActiveTasks(false);

	// now decide whether it's fresh enough to be displayed on the map
	if (_generation < _loader->get_LastGeneration())
	{
		manager->AddTileOnlyCaching(tile);
		tilesLogger.WriteLine("Outdated tile; cached: %d\\%d\\%d", _zoom, _x, _y);
	}
	else
	{
		manager->AddTileWithCaching(tile);
		manager->TriggerMapRedraw();
	}

	//and try to cache it
	loader->RunCaching();		// if there is no pending tasks, the caching will be started		
}
