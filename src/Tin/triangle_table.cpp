# include "stdafx.h"
# include "triangle_table.h"

triangleTable::triangleTable()
{
}

triangleTable::~triangleTable()
{
}

void triangleTable::clear()
{	for( int i = 0; i < static_cast<int>(rows.size()); i++ )
	{	delete rows[i];
		rows[i] = nullptr;
	}
	rows.clear();
}

/*
enum COLUMN { VTX_ONE, VTX_TWO, VTX_THREE,	//long
			  BDR_ONE, BDR_TWO, BDR_THREE,	//long
			  DEV_VERTEX,					//vertex
			  MAX_DEV };					//double
*/

void * triangleTable::getValue( COLUMN column, long row )
{	if( column == VTX_ONE )
		return &(rows[row]->row.vertexOne);
	else if( column == VTX_TWO )
		return &(rows[row]->row.vertexTwo);
	else if( column == VTX_THREE )
		return &(rows[row]->row.vertexThree);
	else if( column == BDR_ONE )
		return &(rows[row]->row.borderOne);
	else if( column == BDR_TWO )
		return &(rows[row]->row.borderTwo);
	else if( column == BDR_THREE )
		return &(rows[row]->row.borderThree);
	else if( column == DEV_VERTEX )
		return &(rows[row]->row.devVertex);
	else if( column == MAX_DEV )
		return &(rows[row]->row.maxDev);
	return NULL;
}

void triangleTable::setValue( COLUMN column, long row, void * value )
{	if( column == VTX_ONE )
		rows[row]->row.vertexOne = *static_cast<long*>(value);
	else if( column == VTX_TWO )
		rows[row]->row.vertexTwo = *static_cast<long*>(value);
	else if( column == VTX_THREE )
		rows[row]->row.vertexThree = *static_cast<long*>(value);
	else if( column == BDR_ONE )
		rows[row]->row.borderOne = *static_cast<long*>(value);
	else if( column == BDR_TWO )
		rows[row]->row.borderTwo = *static_cast<long*>(value);
	else if( column == BDR_THREE )
		rows[row]->row.borderThree = *static_cast<long*>(value);
	else if( column == DEV_VERTEX )
		rows[row]->row.devVertex = *static_cast<vertex*>(value);
	else if( column == MAX_DEV )
		rows[row]->row.maxDev = *static_cast<double*>(value);
}

long triangleTable::addRow( tinTableRow & r )
{	tableRowNode * new_row = new tableRowNode();
	new_row->row = r;
	rows.push_back( new_row );
	return static_cast<long>(rows.size()) - 1;
}

void triangleTable::setRow( tinTableRow & r, long row )
{	rows[row]->row = r;
}

long triangleTable::size()
{	return static_cast<long>(rows.size());
}