#pragma once
#include <algorithm>
#include "WinBase.h"
#include "afxmt.h"
#include <queue>

struct OgrLoadingTask
{
	static long SeedId;
	long LayerHandle;
	long Id;
	long FeatureCount;
	long LoadedCount;
	bool Finished;
	bool Cancelled;
	
	OgrLoadingTask(long layerHandle) : Id(SeedId++), FeatureCount(0), LoadedCount(0), 
					Finished(false), Cancelled(false), LayerHandle(layerHandle)
	{}

	OgrLoadingTask* Clone() {
		OgrLoadingTask* task = new OgrLoadingTask(LayerHandle);
		task->FeatureCount = FeatureCount;
		task->SeedId = SeedId;
		task->Id = Id;
		task->LoadedCount = LoadedCount;
		task->Finished = Finished;
		task->Cancelled = Cancelled;
		return task;
	}
};

class OgrDynamicLoader
{
public:
	OgrDynamicLoader()
	{
		_stop = false;
		_maxCacheCount = m_globalSettings.ogrLayerMaxFeatureCount;
		_lockCounter = 0;
		IsMShapefile = false;
	}
	~OgrDynamicLoader() {
		CancelAllTasks();
		ClearFinishedTasks();
	}

private:
	::CCriticalSection DataLock;
	::CCriticalSection QueueLock;
	bool _stop;
	int _maxCacheCount;
	unsigned long _lockCounter;
	std::queue<OgrLoadingTask*> Queue;
	vector<ShapeRecordData*> Data;
	bool hasData;

public:
	::CCriticalSection ShapefileLock;
	::CCriticalSection LoadingLock;
	::CCriticalSection ProviderLock;	

	bool IsMShapefile;
	Extent LastExtents;
	Extent LastSuccessExtents;
	
	void EnqueueTask(OgrLoadingTask* task);
	bool SignalWaitingTask();
	void ReleaseWaitingTask() { InterlockedDecrement(&_lockCounter);}
	bool HaveWaitingTasks() { return _lockCounter > 0; }
	void CancelAllTasks();
	void Restart();

	int GetMaxCacheCount() { return _maxCacheCount; }
	void SetMaxCacheCount(int value) { _maxCacheCount = value; }
	bool CanLoad(int featureCount) { return featureCount < GetMaxCacheCount(); }

	void ClearFinishedTasks();
	void AwaitTasks();
	
	vector<ShapeRecordData*> FetchData();
	void PutData(vector<ShapeRecordData*> shapeData);
	bool HasData();
};

