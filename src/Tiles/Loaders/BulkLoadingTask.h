#pragma once
#include "ILoadingTask.h"

// *******************************************************
//		LoadingTask
// *******************************************************
// A loading task that only runs caching after the completion.
class BulkLoadingTask : public ILoadingTask
{
public:
	BulkLoadingTask(int x, int y, int zoom, BaseProvider* provider, int generation)
		: ILoadingTask(x, y, zoom, provider, generation)
	{
	}

	virtual ~BulkLoadingTask()
	{
	}

public:
	void BeforeRequest();
	void AfterRequest(TileCore* tile);
};





