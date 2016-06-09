#pragma once
#include "ILoadingTask.h"

// *******************************************************
//		LoadingTask
// *******************************************************
// Represents a single loading task (a single tile to load)
class MapLoadingTask : public ILoadingTask
{
public:
	MapLoadingTask(int x, int y, int zoom, BaseProvider* provider, int generation)
		: ILoadingTask(x, y, zoom, provider, generation)
	{
	}

	virtual ~MapLoadingTask() 
	{
	}

public:
	void BeforeRequest();
	void AfterRequest(TileCore* tile);
};





