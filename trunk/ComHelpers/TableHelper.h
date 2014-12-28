#pragma once
#include "TableClass.h"

class TableHelper
{
public:
	static CTableClass* Cast(CComPtr<ITable>& table);
	static long GetNumRows(ITable* table);
	static void SetFieldValue(ITable* table, int rowIndex, CExpression& expr);
};

