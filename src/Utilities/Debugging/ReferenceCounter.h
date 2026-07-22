#pragma once
#if DEBUG_ALLOCATED_OBJECTS
#include <unordered_set>
#endif

class ReferenceCounter
{
	static const int INTERFACES_COUNT = 100;
	int referenceCounts[INTERFACES_COUNT];
	int totalCounts[INTERFACES_COUNT];
#if DEBUG_ALLOCATED_OBJECTS
	unordered_set<void*> referencePtrs;
#endif
public:
	ReferenceCounter(void) 
	{
		for(int i = 0; i < INTERFACES_COUNT; i++)
		{
			referenceCounts[i] = 0;
			totalCounts[i] = 0;
		}
	};
	~ReferenceCounter(void) {};
	void AddRef(tkInterface type)
	{
		int* val = &referenceCounts[(int)type];
		(*val)++;
		val = &totalCounts[(int)type];
		(*val)++;
	}
	void Release(tkInterface type)
	{
		int* val = &referenceCounts[(int)type];
		(*val)--;
	}
	void WriteReport(bool unreleasedOnly);
	CString GetReport(bool unreleasedOnly);

#if DEBUG_ALLOCATED_OBJECTS
	void AddRef(void* pThis)
	{
		referencePtrs.insert(pThis);
	}

	void Release(void* pThis)
	{
		referencePtrs.erase(pThis);
	}

	long GetReferenceCount() const
	{
		return referencePtrs.size();
	}

	CString GetReferenceReport();
#endif 
};
