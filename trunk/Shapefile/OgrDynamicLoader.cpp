#include "stdafx.h"
#include "OgrDynamicLoader.h"

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