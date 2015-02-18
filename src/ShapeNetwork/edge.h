# ifndef EDGE_H
# define EDGE_H

# include "graphnode.h"

class edge
{
public:

	edge()
	{	one = NULL;
		two = NULL;
		length = 0;
	}

	~edge()
	{	
	}
	
	graphnode * one;
	graphnode * two;
	long oneIndex;
	long twoIndex;
	double length;
};

# endif