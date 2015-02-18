# ifndef HEAP_NODE_H
# define HEAP_NODE_H


class heapnode
{
	public:
		heapnode()
		{	index = -1;
			value = -1;			
		}
		~heapnode()
		{
		}
		heapnode( const heapnode & hn )
		{	index = hn.index;
			value = hn.value;			
		}
		heapnode( int triIndex, double val )
		{	index = triIndex;
			value = val;			
		}
		heapnode operator=( const heapnode & hn )
		{	index = hn.index;
			value = hn.value;
			return *this;
		}
		bool operator > ( const heapnode & hn )
		{	if( value > hn.value )
				return true;
			return false;
		}
		bool operator < ( const heapnode & hn )
		{	if( value < hn.value )
				return true;
			return false;
		}
		
		long index;
		double value;
};

# endif