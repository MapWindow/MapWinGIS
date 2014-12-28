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

// **************************************************
//		SetFieldValue()
// **************************************************
void TableHelper::SetFieldValue(ITable* table, int rowIndex, CExpression& expr )
{
	if (!table) return;
	
	for (long j = 0; j< expr.get_NumFields(); j++) 
	{
		CComVariant var;
		int fieldIndex = expr.get_FieldIndex(j);
		table->get_CellValue(fieldIndex, rowIndex, &var);
		switch (var.vt)
		{
			case VT_BSTR: expr.put_FieldValue(j, var.bstrVal); break;
			case VT_I4:	  expr.put_FieldValue(j, (double)var.lVal); break;
			case VT_R8:	  expr.put_FieldValue(j, (double)var.dblVal); break;
		}
	}
}