#include "stdafx.h"
#include "ReferenceCounter.h"

// ********************************************************
//     WriteReport()
// ********************************************************
void ReferenceCounter::WriteReport(bool unreleasedOnly)
{
	CString s = GetReport(unreleasedOnly);
	Debug::WriteLine(s);
	Debug::WriteLine("-------------------------");
}

// ********************************************************
//     GetReport()
// ********************************************************
CString ReferenceCounter::GetReport(bool unreleasedOnly)
{
	CString s, temp;
	s += unreleasedOnly ? "UNRELEASED COM REFERENCES:\n": "COM REFERENCES (unreleased/allocated):\n";
	for(int i = 0; i < INTERFACES_COUNT; i++)
	{
		if (unreleasedOnly)
		{
			if (referenceCounts[i] != 0)	
			{
				temp.Format("Class %s: %d\n", ComHelper::GetInterfaceName((tkInterface)i), referenceCounts[i]);
				s += temp;
			}
		}
		else
		{
			if (totalCounts[i] != 0)	
			{
				temp.Format("Class %s: %d/%d\n", ComHelper::GetInterfaceName((tkInterface)i), referenceCounts[i], totalCounts[i]);
				s += temp;
			}
		}
	}
	if (temp.GetLength() == 0) {
		s += "<none>";
	}

	return s;
}