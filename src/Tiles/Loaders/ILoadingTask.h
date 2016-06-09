#pragma once
#include "baseprovider.h"
#include "ITileLoader.h"

class ITileLoader;

// *******************************************************
//		LoadingTask
// *******************************************************
// Represents a single loading task (a single tile to load)
class ILoadingTask : ITask
{
public:
	ILoadingTask(int x, int y, int zoom, BaseProvider* provider, int generation)
		: _x(x), _y(y), _zoom(zoom), _loader(NULL)
	{
		_completed = false;
		_generation = generation;
		_provider = provider;
	}

protected:
	int _x;
	int _y;
	int _zoom;
	int _generation;
	bool _completed;
	ITileLoader* _loader;
	BaseProvider* _provider;

public:
	// properties
	int x() { return _x; }
	int y() { return _y; }
	int zoom() { return _zoom; }
	int generation() { return _generation; }
	void generation(int value) { _generation = value; }
	int completed() { return _completed; }
	void set_Loader(ITileLoader* loader) { _loader = loader; }
	BaseProvider* get_Provider() { return _provider; }
	bool Compare(ILoadingTask* other);
	void DoTask();

public:
	virtual void BeforeRequest() = 0;
	virtual void AfterRequest(TileCore* tile) = 0;
};





