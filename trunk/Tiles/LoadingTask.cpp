#include "stdafx.h"
#include "LoadingTask.h"
#include "map.h"
#include "tiles.h"

// *******************************************************
//		LoadingTask::DoTask()
// *******************************************************
void LoadingTask::DoTask()
{
	ASSERT(this->Loader);

	if (this->Loader->stopped)
	{
		// do nothing; the task was aborted
	}
	else if (this->generation < this->Loader->tileGeneration)
	{
		// this tile is no longer needed; another set of tiles was requested		
		//Debug::WriteLine("Outdated tile loading: canceled");
		tilesLogger.WriteLine("Outdated tile loading; cancelled: %d\\%d\\%d", this->zoom, this->x, this->y);
	}
	else
	{
		long timeout = this->Loader->m_sleepBeforeRequestTimeout;
		if (timeout > 0 && timeout < 10000) Sleep(timeout);

		if (!this->cacheOnly)
			this->Loader->AddActiveTask(this);

		TileCore* tile = Provider->GetTileImage(CPoint(x, y), zoom);		// HTTP call

		if (this->Loader->stopped)
		{
			delete tile;	// requesting from a server takes time; probably the task was already aborted
			if (!this->cacheOnly)
				this->Loader->RemoveActiveTask(this);
		}
		else
		{
			if (this->generation == this->Loader->tileGeneration)
				this->Loader->m_count++;

			// pre-fetching without display
			if (this->cacheOnly)
			{
				this->Loader->TileLoaded(tile);
			}
			else
			{
				if (!tile->IsEmpty())
				{
					this->busy = true;	// notifies that related classes like CTiles can't be deleted until exiting this section
					CMapView* mapView = (CMapView*)Provider->mapView;
					CTiles* tiles = (CTiles*)mapView->GetTilesNoRef();

					// quickly pass it from active list to cache; so it's always avaiable while building the next list of requests
					this->Loader->LockActiveTasks(true);
					tiles->AddTileToRamCache(tile);
					this->Loader->RemoveActiveTask(this);
					this->Loader->LockActiveTasks(false);

					if (this->generation < this->Loader->tileGeneration)
					{
						tiles->AddTileOnlyCaching(tile);
						tilesLogger.WriteLine("Outdated tile; cached: %d\\%d\\%d", this->zoom, this->x, this->y);
					}
					else
					{
						tiles->AddTileWithCaching(tile);
						mapView->RedrawCore(RedrawSkipDataLayers, false, false);
						this->Loader->RunCaching();		// if there is no pending tasks, the caching will be started		
					}
					this->busy = false;
				}
				else {
					this->Loader->RemoveActiveTask(this);
				}
			}

			// check generation to avoid firing the event several times when outdated times are loaded
			if (this->generation == this->Loader->tileGeneration) {
				this->Loader->CheckComplete();
			}
		}
	}

	// let's try to duplicate this, as sometimes tiles still remain in the list when completed flag is set
	this->Loader->RemoveActiveTask(this);
	this->completed = true;
}