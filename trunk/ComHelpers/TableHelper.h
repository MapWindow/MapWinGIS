#pragma once
#include "TableClass.h"

class TableHelper
{
public:
	static CTableClass* Cast(CComPtr<ITable>& table)
	{
		return (CTableClass*)&(*table);
	}
};

