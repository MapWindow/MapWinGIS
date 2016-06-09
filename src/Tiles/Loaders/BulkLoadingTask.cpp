#include "stdafx.h"
#include "BulkLoadingTask.h"
#include "TileBulkLoader.h"

// *******************************************************
//		BeforeRequest()
// *******************************************************
void BulkLoadingTask::BeforeRequest()
{
	// do nothing; log request if needed
}

// *******************************************************
//		AfterRequest()
// *******************************************************
void BulkLoadingTask::AfterRequest(TileCore* tile)
{
	TileBulkLoader* loader = dynamic_cast<TileBulkLoader*>(_loader);
	if (loader)
	{
		loader->TileLoaded(tile, _generation);
	}
}

