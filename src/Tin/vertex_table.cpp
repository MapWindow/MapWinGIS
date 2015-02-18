# include "stdafx.h"
# include "vertex_table.h"

vertexTable::vertexTable()
{
}

vertexTable::~vertexTable()
{
}

long vertexTable::add( vertex p )
{	vertex_list.push_back( p );
	return vertex_list.size() - 1;
}

void vertexTable::clear()
{	vertex_list.clear();
}

vertex vertexTable::getVertex( long index )
{	return vertex_list[ index ];
}

void vertexTable::setVertex( vertex v, long index )
{	vertex_list[ index ] = v;
}

long vertexTable::size()
{	return vertex_list.size();
}