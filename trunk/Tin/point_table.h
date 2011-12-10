# ifndef POINT_TABLE_H
# define POINT_TABLE_H

# include <deque>
# include "point.h"
class point_table
{
	public:
		point_table();
		long add( Point p );
		Point getPoint( long index );
		void clear();
		int size()
		{	return point_list.size(); }
	private:		
		std::deque<Point> point_list;
};

# endif