#pragma once

class ReferenceCounter
{
	static const int INTERFACES_COUNT = 33;
	int referenceCounts[INTERFACES_COUNT];
public:
	ReferenceCounter(void) 
	{
		for(int i = 0; i < INTERFACES_COUNT; i++)
		{
			referenceCounts[i] = 0;
		}
	};
	~ReferenceCounter(void) {};
	void AddRef(tkInterface type)
	{
#ifdef _DEBUG
		int* val = &referenceCounts[(int)type];
		(*val)++;
#endif
	}
	void Release(tkInterface type)
	{
#ifdef _DEBUG		
		int* val = &referenceCounts[(int)type];
		(*val)--;
#endif
	}
	void WriteReport();
};
