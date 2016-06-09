// TableRow.cpp: implementation of the TableRow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TableRow.h"
#include "Field.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TableRow::TableRow() : _status(DATA_CLEAN)
{		
}

TableRow::~TableRow()
{	
	for( int i = 0; i < (int)values.size(); i++ )
	{	
		if( values[i] != NULL )
		{	
			VariantClear(values[i]);
			delete values[i];
		}
		values[i] = NULL;
	}
	values.clear();
}	

// ************************************************
//   GetJoinId
// ************************************************
int FieldWrapper::GetJoinId()
{	
	return field != NULL ? ((CField*)field)->GetJoinId() : -1;
}

// ************************************************
//   SetJoinId
// ************************************************
void FieldWrapper::SetJoinId(int value)
{
	((CField*)field)->SetJoinId(value);
}
