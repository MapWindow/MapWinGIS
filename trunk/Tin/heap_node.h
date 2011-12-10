# ifndef HEAP_NODE_H
# define HEAP_NODE_H

# include <stdlib.h>
# include "vertex.h"
# include "tinTypes.h"

class heapNode
{
	public:
		heapNode()
		{	index = Undefined;
			value = Undefined;
			devVertex = vertex( 0, 0, 0 );
		}
		~heapNode()
		{
		}
		heapNode( const heapNode & hn )
		{	index = hn.index;
			value = hn.value;
			devVertex = hn.devVertex;
		}
		heapNode( int triIndex, double val, vertex v )
		{	index = triIndex;
			value = val;
			devVertex = v;
		}
		heapNode operator=( const heapNode & hn )
		{	index = hn.index;
			value = hn.value;
			devVertex = hn.devVertex;
			return *this;
		}
		bool operator > ( const heapNode & hn )
		{	if( value > hn.value )
				return true;
			return false;
		}
		bool operator < ( const heapNode & hn )
		{	if( value < hn.value )
				return true;
			return false;
		}
		int index;
		double value;
		vertex devVertex;
};

# endif