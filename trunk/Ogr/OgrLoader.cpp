#include "stdafx.h"
#include "OgrLoader.h"

// **********************************************
//		Restart()
// **********************************************
void OgrDynamicLoader::Restart()
{
	_lockCounter = 0L;
	_stop = false;
}

// **********************************************
//		AddWaitingTask()
// **********************************************
bool OgrDynamicLoader::AddWaitingTask(bool terminate)
{
	if (_stop) return false;
	if (terminate) {
		_stop = true;          // any newly arrived threads won't reach the lock
		_lockCounter += (ULONG)1e4;   // any threads awaiting the lock will be aborted immediately after acquiring it
	}
	InterlockedIncrement(&_lockCounter);
	return true;
};

// **********************************************
//		Clear()
// **********************************************
void OgrDynamicLoader::Clear()
{
	for (size_t i = 0; i < Data.size(); i++) {
		delete Data[i];
	}
	Data.clear();
}

// **********************************************
//		LockLoading()
// **********************************************
void OgrDynamicLoader::LockLoading(bool lock)
{
	if (lock)
		LoadingLock.Lock();
	else
		LoadingLock.Unlock();
}

// **********************************************
//		LockData()
// **********************************************
void OgrDynamicLoader::LockData(bool lock)
{
	if (lock)
		DataLock.Lock();
	else
		DataLock.Unlock();
}

// **********************************************
//		LockProvider()
// **********************************************
void OgrDynamicLoader::LockProvider(bool lock)
{
	if (lock)
		ProviderLock.Lock();
	else
		ProviderLock.Unlock();
}

// **********************************************
//		LockShapefile()
// **********************************************
void OgrDynamicLoader::LockShapefile(bool lock)
{
	if (lock)
		ShapefileLock.Lock();
	else
		ShapefileLock.Unlock();
}

// **********************************************
//		GetLabelPosition()
// **********************************************
tkLabelPositioning OgrDynamicLoader::GetLabelPosition(ShpfileType type)
{
	if (LabelPosition == lpNone)
		return Utility::LabelPositionForShapeType(type);
	else
		return LabelPosition;
}

