#include "stdafx.h"
#include "ReferenceCounter.h"

void ReferenceCounter::WriteReport()
{
	//#ifdef _DEBUG
		Debug::WriteLine("UNRELEASED COM REFERENCES:");
		for(int i = 0; i < 33; i++)
		{
			if (referenceCounts[i] != 0)
				Debug::WriteLine("Class: %d; count: %d", i, referenceCounts[i]);
		}
		Debug::WriteLine("-------------------------");
	//#endif
}