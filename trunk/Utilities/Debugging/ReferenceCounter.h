#pragma once

class ReferenceCounter
{
	static const int INTERFACES_COUNT = 38;
	int referenceCounts[INTERFACES_COUNT];
	int totalCounts[INTERFACES_COUNT];
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
};
