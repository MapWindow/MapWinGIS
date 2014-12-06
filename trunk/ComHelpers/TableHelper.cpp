#include "stdafx.h"
#include "TableHelper.h"

// **************************************************
//		Cast()
// **************************************************
CTableClass* TableHelper::Cast(CComPtr<ITable>& table)
{
	return (CTableClass*)&(*table);
}

// **************************************************
//		GetNumRows()
// **************************************************
long TableHelper::GetNumRows(ITable* table)
{
	if (!table) return 0;
	long numRows;
	table->get_NumRows(&numRows);
	return numRows;
}