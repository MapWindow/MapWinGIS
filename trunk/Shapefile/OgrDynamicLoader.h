#pragma once
#include <algorithm>
#include "TableClass.h"
#include "WinBase.h"
#include "afxmt.h"

class OgrDynamicLoader
{
public:
	OgrDynamicLoader() 
	{
		_maxCacheCount = 50000;
		_lockCounter = 0;
		LoadingLock.Unlock();
		DataLock.Unlock();
		ProviderLock.Unlock();
	}
	~OgrDynamicLoader() {
		Clear();
	}

private:
	int _maxCacheCount;
	unsigned long _lockCounter;
	::CCriticalSection LoadingLock;
	::CCriticalSection DataLock;
	::CCriticalSection ProviderLock;

public:

	vector<ShapeRecordData*> Data;
	Extent LastExtents;
	Extent LastSuccessExtents;
	
	void AddWaitingTask() { InterlockedIncrement(&_lockCounter); };
	void ReleaseWaitingTask() { InterlockedDecrement(&_lockCounter);}
	bool HaveWaitingTasks() { return _lockCounter > 0; }
	int GetMaxCacheCount() { return _maxCacheCount; }
	bool CanLoad(int featureCount) { return featureCount < GetMaxCacheCount(); }
	void LockLoading(bool lock);
	void LockData(bool lock);
	void LockProvider(bool lock);
	void Clear();
};

