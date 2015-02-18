# include "stdafx.h"
# include "heap.h"

heap::heap()
{	
}

heap::~heap()
{
}
void heap::clear()
{	
	all_nodes.clear();
}

void heap::insert( long triangleIndex, double value )
{
	heapnode newNode( triangleIndex, value );
	all_nodes.push_back(newNode);

	trickleUp();
}

long heap::size()
{	return all_nodes.size();
}

inline void heap::trickleUp()
{	
	long index = all_nodes.size() - 1 ;
	long parent;

	bool trickle = true;
	heapnode tempNode;	

	while( trickle )
	{		
		//Even Kid
		if( index % 2 == 0 )
			parent = ( index - 2 ) / 2;//*.5;
		//Odd Kid
		else
			parent = ( index - 1 ) / 2;//*.5;

		if( parent >= 0 )
		{	if( all_nodes[index] < all_nodes[parent] )
			{	tempNode = all_nodes[index];
				all_nodes[index] = all_nodes[parent];
				all_nodes[parent] = tempNode;
				index = parent;
			}
			else
				trickle = false;
		}
		else
			trickle = false;
	}
}

void heap::pop()
{	all_nodes.pop_front();
	if( all_nodes.size() > 1 )
	{	all_nodes.push_front( all_nodes[ all_nodes.size() - 1 ] );
		all_nodes.pop_back();
		trickleDown();
	}	
}

heapnode heap::top()
{	if( all_nodes.size() > 0 )
		return all_nodes[0];
	else
		return heapnode();
}

inline void heap::trickleDown()
{	
	long index = 0;
	bool trickle = true;
	long swap_index;
	heapnode tempNode;
	while( trickle )
	{
		long kid1 = index*2 + 1;
		long kid2 = kid1 + 1;

		swap_index = minValIndex( kid1, kid2 );
		if( swap_index > 0 && swap_index < (int)all_nodes.size() )
		{
			if( all_nodes[index] > all_nodes[swap_index] )
			{	tempNode = all_nodes[index];
				all_nodes[index] = all_nodes[swap_index];
				all_nodes[swap_index] = tempNode;			
				index = swap_index;
			}
			else
				trickle = false;
		}
		else
			trickle = false;
	}
}

inline long heap::minValIndex( long kid1, long kid2 )
{
	long heap_size = all_nodes.size();
	if( kid2 >= heap_size || kid1 >= heap_size )
		return kid1;	
	else
	{	if( all_nodes[kid1] < all_nodes[kid2] )
			return kid1;
		else
			return kid2;
	}
}
