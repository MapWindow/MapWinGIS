# include "stdafx.h"
# include "point_table.h"

point_table::point_table()
{
}

long point_table::add( Point p )
{	point_list.push_back( p );
	return point_list.size() - 1;
}

void point_table::clear()
{	point_list.clear();
}

Point point_table::getPoint( long index )
{	return point_list[ index ];
}
