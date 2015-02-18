#pragma once
#include "baseprovider.h"
#include "tileloader.h"

// Represents a single loading task (a single tile to load)
class LoadingTask : ITask
{
public:
	int x;
	int y;
	int zoom;
	int generation;
	BaseProvider* Provider;
	bool cacheOnly;
	bool completed;
	bool busy;
	TileLoader* Loader;

	LoadingTask(int x, int y, int zoom, BaseProvider* provider, int generation, bool cacheOnly)
		: x(x), y(y), zoom(zoom), cacheOnly(cacheOnly)
	{
		Loader = NULL;
		this->busy = false;
		this->completed = false;
		this->Provider = provider;
		this->generation = generation;
	}

	bool Compare(LoadingTask* other) {
		return this->x == other->x && this->y == other->y && this->zoom == other->zoom;
	}

	void DoTask();
};





