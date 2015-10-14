#include "stdafx.h"
#include "LoadingTask.h"
#include "map.h"
#include "tiles.h"

// *******************************************************
//		LoadingTask::DoTask()
// *******************************************************
void LoadingTask::DoTask()
{
	if (_loader->isStopped())
	{
		return;		// do nothing; the task was aborted
	}

	if (_generation < _loader->get_TileGeneration())
	{
		// this tile is no longer needed; another set of tiles was requested		
		tilesLogger.WriteLine("Outdated tile loading; cancelled: %d\\%d\\%d", _zoom, _x, _y);
		return;
	}
	
	long timeout = _loader->get_SleepBeforeRequestTimeout();
	if (timeout > 0 && timeout < 10000)  {
		Sleep(timeout);
	}

	if (!_cacheOnly) {
		_loader->AddActiveTask(this);
	}

	TileCore* tile = _provider->GetTileImage(CPoint(_x, _y), _zoom);		// HTTP call

	if (_loader->isStopped())
	{
		delete tile;	// requesting from a server takes time; probably the task was already aborted

		if (!_cacheOnly) {
			_loader->RemoveActiveTask(this);
		}
	}
	else
	{
		if (_generation == _loader->get_TileGeneration()) {
			_loader->IncrementCount();
		}

		if (_cacheOnly)
		{
			// loading without display
			_loader->TileLoaded(tile);
		}
		else
		{
			if (!tile->IsEmpty())
			{
				_busy = true;	// notifies that related classes like CTiles can't be deleted until exiting this section
				TileManager* manager = (TileManager*)_provider->get_Manager();
					
				// quickly pass it from active list to cache; so it's always avaiable while building the next list of requests
				_loader->LockActiveTasks(true);

				manager->AddTileToRamCache(tile);
				_loader->RemoveActiveTask(this);

				_loader->LockActiveTasks(false);

				if (_generation < _loader->get_TileGeneration())
				{
					manager->AddTileOnlyCaching(tile);
					tilesLogger.WriteLine("Outdated tile; cached: %d\\%d\\%d", _zoom, _x, _y);
				}
				else
				{
					manager->AddTileWithCaching(tile);
					manager->TriggerMapRedraw();
					_loader->RunCaching();		// if there is no pending tasks, the caching will be started		
				}
				_busy = false;
			}
			else {
				_loader->RemoveActiveTask(this);
			}
		}

		// check generation to avoid firing the event several times when outdated times are loaded
		if (_generation == _loader->get_TileGeneration() && _loader->IsCompleted()) {
			TileManager* manager = (TileManager*)_provider->get_Manager();
			manager->FireTilesLoaded(_loader->get_isSnapShot(), _loader->get_Key());
		}
	}

	// let's try to duplicate this, as sometimes tiles still remain in the list when completed flag is set
	_loader->RemoveActiveTask(this);
	_completed = true;
}