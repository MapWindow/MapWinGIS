#include "stdafx.h"
#include "OgrLoader.h"
#include "LabelsHelper.h"
#include "windows.h" 

long OgrLoadingTask::SeedId = 0;

// **********************************************
//		Restart()
// **********************************************
void OgrDynamicLoader::Restart()
{
	if (!_stop) CancelAllTasks();

	_lockCounter = 0L;
	_stop = false;
}

// **********************************************
//		PutData()
// **********************************************
void OgrDynamicLoader::PutData(vector<ShapeRecordData*> shapeData)
{ // Locking data in this function
	CSingleLock lock(&DataLock, TRUE);
	Data.insert(Data.end(), shapeData.begin(), shapeData.end());
}

// **********************************************
//		FetchData()
// **********************************************
vector<ShapeRecordData*> OgrDynamicLoader::FetchData()
{ // Locking data in this function
	vector<ShapeRecordData*> data;
	CSingleLock lock(&DataLock, TRUE);
	if (Data.size() > 0) {
		data.insert(data.end(), Data.begin(), Data.end());
		Data.clear();
	}
	return data;
}

// **********************************************
//		EnqueueTask()
// **********************************************
void OgrDynamicLoader::EnqueueTask(OgrLoadingTask * task)
{
	CSingleLock (&QueueLock, TRUE);
	Queue.push(task);
}

// **********************************************
//		SignalWaitingTask()
// **********************************************
bool OgrDynamicLoader::SignalWaitingTask()
{
	if (_stop) return false;
	InterlockedIncrement(&_lockCounter);
	return true;
};

// **********************************************
//		CancelAllTasks()
// **********************************************
void OgrDynamicLoader::CancelAllTasks()
{
	if (_stop) return;
	_stop = true;          // any newly arrived threads won't reach the lock
	_lockCounter += (ULONG)1e4;   // any threads awaiting the lock will be aborted immediately after acquiring it
	InterlockedIncrement(&_lockCounter);
	return;
}

// **********************************************
//		ClearFinishedTasks()
// **********************************************
void OgrDynamicLoader::ClearFinishedTasks()
{
	CSingleLock queueLock(&QueueLock, TRUE);

	std::queue<OgrLoadingTask*> unfqueue;

	while (!Queue.empty())
	{
		OgrLoadingTask* task = Queue.front();
		Queue.pop();
		if (!task->Finished) {
			unfqueue.push(task);
			continue;
		}
			
		if (!task->Cancelled) { // If succesful return a clone:
			OgrLoadingTask* infoClone = task->Clone();
		}

		delete task;
	}

	// Replace queue with the unfinished items queue
	Queue.swap(unfqueue);
}

// **********************************************
//		AwaitTasks()
// **********************************************
void OgrDynamicLoader::AwaitTasks()
{
	CSingleLock queueLock(&QueueLock, TRUE);

	while (!Queue.empty())
	{
		OgrLoadingTask* task = Queue.front();
		Queue.pop();
		if (!task->Finished) {
			queueLock.Unlock(); // Unlock briefly so other threads can fetch
			Sleep(10);
			queueLock.Lock();
			continue; // Try next
		}
        else 
        {
            delete task;
        }
    }
}

// **********************************************
//		GetLabelPosition()
// **********************************************
tkLabelPositioning OgrDynamicLoader::GetLabelPosition(ShpfileType type)
{
	if (LabelPosition == lpNone)
		return LabelsHelper::LabelPositionForShapeType(type);
	else
		return LabelPosition;
}

