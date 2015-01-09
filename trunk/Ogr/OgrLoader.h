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
					Finished(false), Cancelled(false)
	{
		LayerHandle = layerHandle;
	}
};

class OgrDynamicLoader
{
public:
	OgrDynamicLoader() 
	{
		_stop = false;
		_maxCacheCount = m_globalSettings.ogrLayerMaxFeatureCount;
		LabelOrientation = lorParallel;
		LabelPosition = lpNone;
		_lockCounter = 0;
		IsMShapefile = false;
	}
	~OgrDynamicLoader() {
		Clear();
	}

private:
	bool _stop;
	int _maxCacheCount;
	unsigned long _lockCounter;

public:
	::CCriticalSection ShapefileLock;
	::CCriticalSection LoadingLock;
	::CCriticalSection DataLock;
	::CCriticalSection ProviderLock;

	std::queue<OgrLoadingTask*> Queue;
	bool IsMShapefile;
	CStringW LabelExpression;
	vector<ShapeRecordData*> Data;
	Extent LastExtents;
	Extent LastSuccessExtents;
	tkLabelPositioning LabelPosition;
	tkLineLabelOrientation LabelOrientation;
	tkLabelPositioning GetLabelPosition(ShpfileType type);
	
	bool AddWaitingTask(bool terminate = false);
	void ReleaseWaitingTask() { InterlockedDecrement(&_lockCounter);}
	bool HaveWaitingTasks() { return _lockCounter > 0; }
	int GetMaxCacheCount() { return _maxCacheCount; }
	void SetMaxCacheCount(int value) { _maxCacheCount = value; }
	bool CanLoad(int featureCount) { return featureCount < GetMaxCacheCount(); }
	void LockLoading(bool lock);
	void LockData(bool lock);
	void LockShapefile(bool lock);
	void LockProvider(bool lock);
	void Clear();
	void Restart();
};

