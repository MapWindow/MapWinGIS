#include "stdafx.h"
#include "ILoadingTask.h"
#include "TileManager.h"

// *******************************************************
//		Compare()
// *******************************************************
bool ILoadingTask::Compare(ILoadingTask* other) 
{
	return _x == other->_x && _y == other->_y && _zoom == other->_zoom;
}

// *******************************************************
//		DoTask()
// *******************************************************
void ILoadingTask::DoTask()
{
	if (_loader->IsOutdated(_generation))
	{
		// this tile is no longer needed; another set of tiles was requested		
		tilesLogger.WriteLine("Outdated tile loading; cancelled: %d\\%d\\%d", _zoom, _x, _y);
		return;
	}
	
	long timeout = _loader->get_DelayRequestTimeout();
	if (timeout > 0 && timeout < 10000)  {
		Sleep(timeout);
	}

	// let descendants register the request
	BeforeRequest();

	// HTTP call
	TileCore* tile = _provider->GetTileImage(CPoint(_x, _y), _zoom);		

	// let descendants do something useful with the tile
	AfterRequest(tile);

	if (_loader->isStopped())
	{
		delete tile;	// requesting from a server takes time; probably the task was already aborted
		tile = NULL;
	}

	unsigned int count = _loader->RegisterTile(_generation);

	// check generation to avoid firing the event several times when outdated tiles are loaded
	TileRequestInfo* info = _loader->FindRequest(_generation);
	if (info)
	{
		if (count == info->totalCount)
		{
			TileManager* manager = (TileManager*)_provider->get_Manager();
			manager->FireTilesLoaded(info->isSnapshot, info->key, false);			
		}
	}

	_completed = true;
}