#pragma once
#include "baseprovider.h"
#include "tileloader.h"

// *******************************************************
//		LoadingTask
// *******************************************************
// Represents a single loading task (a single tile to load)
class LoadingTask : ITask
{
public:
	LoadingTask(int x, int y, int zoom, BaseProvider* provider, int generation, bool cacheOnly)
		: _x(x), _y(y), _zoom(zoom), _cacheOnly(cacheOnly), Loader(NULL)
	{
		_busy = false;
		_completed = false;
		_generation = generation;
		Provider = provider;
	}

private:
	int _x;
	int _y;
	int _zoom;
	int _generation;
	bool _cacheOnly;
	bool _completed;
	bool _busy;

public:
	// TODO: wrap
	BaseProvider* Provider;
	TileLoader* Loader;

public:
	// properties
	int x() { return _x; }
	int y() { return _y; }
	int zoom() { return _zoom; }
	int generation() { return _generation; }
	void generation(int value) { _generation = value; }
	int cacheOnly() { return _cacheOnly; }
	int completed() { return _completed; }
	int busy() { return _busy; }

	bool Compare(LoadingTask* other) {
		return this->_x == other->_x && this->_y == other->_y && this->_zoom == other->_zoom;
	}

	void DoTask();
};





