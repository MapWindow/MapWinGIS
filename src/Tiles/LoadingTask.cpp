#include "stdafx.h"
#include "LoadingTask.h"
#include "map.h"
#include "tiles.h"

// *******************************************************
//		LoadingTask::DoTask()
// *******************************************************
void LoadingTask::DoTask()
{
	ASSERT(Loader);

	if (Loader->stopped)
	{
		return;		// do nothing; the task was aborted
	}

	if (this->_generation < Loader->tileGeneration)
	{
		// this tile is no longer needed; another set of tiles was requested		
		tilesLogger.WriteLine("Outdated tile loading; cancelled: %d\\%d\\%d", this->_zoom, this->_x, this->_y);
		return;
	}
	
	long timeout = Loader->m_sleepBeforeRequestTimeout;
	if (timeout > 0 && timeout < 10000)  {
		Sleep(timeout);
	}

	if (!this->_cacheOnly) {
		Loader->AddActiveTask(this);
	}

	TileCore* tile = Provider->GetTileImage(CPoint(_x, _y), _zoom);		// HTTP call

	if (this->Loader->stopped)
	{
		delete tile;	// requesting from a server takes time; probably the task was already aborted

		if (!this->_cacheOnly) {
			this->Loader->RemoveActiveTask(this);
		}
	}
	else
	{
		if (this->_generation == this->Loader->tileGeneration) {
			this->Loader->m_count++;
		}

		// prefetching without display
		if (this->_cacheOnly)
		{
			this->Loader->TileLoaded(tile);
		}
		else
		{
			if (!tile->IsEmpty())
			{
				_busy = true;	// notifies that related classes like CTiles can't be deleted until exiting this section
				TileManager* manager = (TileManager*)Provider->get_Manager();
					
				// quickly pass it from active list to cache; so it's always avaiable while building the next list of requests
				Loader->LockActiveTasks(true);

				manager->AddTileToRamCache(tile);
				Loader->RemoveActiveTask(this);

				Loader->LockActiveTasks(false);

				if (this->_generation < this->Loader->tileGeneration)
				{
					manager->AddTileOnlyCaching(tile);
					tilesLogger.WriteLine("Outdated tile; cached: %d\\%d\\%d", this->_zoom, this->_x, this->_y);
				}
				else
				{
					manager->AddTileWithCaching(tile);
					manager->TriggerMapRedraw();
					Loader->RunCaching();		// if there is no pending tasks, the caching will be started		
				}
				this->_busy = false;
			}
			else {
				Loader->RemoveActiveTask(this);
			}
		}

		// check generation to avoid firing the event several times when outdated times are loaded
		if (_generation == Loader->tileGeneration && Loader->CheckComplete()) {
			TileManager* manager = (TileManager*)Provider->get_Manager();
			manager->FireTilesLoaded(Loader->get_isSnapShot(), Loader->get_Key());
		}
	}

	// let's try to duplicate this, as sometimes tiles still remain in the list when completed flag is set
	Loader->RemoveActiveTask(this);
	this->_completed = true;
}