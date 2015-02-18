# include "stdafx.h"
# include "TinHeap.h"

TinHeap::TinHeap()
{	minDeviation = 1.0;
}
TinHeap::TinHeap( double p_minDeviation )
{	minDeviation = p_minDeviation;
}
TinHeap::~TinHeap()
{
}
void TinHeap::clear()
{	
	all_nodes.clear();
}

void TinHeap::insert( long triangleIndex, double dev, vertex v )
{
	if( dev < minDeviation )
		return;

	heapNode newNode( triangleIndex, dev, v );
	all_nodes.push_back(newNode);

	trickleUp();
}

inline void TinHeap::trickleUp()
{	
	long index = all_nodes.size() - 1 ;
	long parent;

	bool trickle = true;
	heapNode tempNode;	

	while( trickle )
	{		
		//Even Kid
		if( index % 2 == 0 )
			parent = ( index - 2 ) / 2; //*.5;
		//Odd Kid
		else
			parent = ( index - 1 ) / 2; //*.5;

		if( parent >= 0 )
		{	if( all_nodes[index] > all_nodes[parent] )
			{
				tempNode = all_nodes[index];
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

void TinHeap::pop()
{	all_nodes.pop_front();
	if( all_nodes.size() > 1 )
	{	all_nodes.push_front( all_nodes[ all_nodes.size() - 1 ] );
		all_nodes.pop_back();
		trickleDown();
	}	
}

heapNode TinHeap::top()
{	if( all_nodes.size() > 0 )
		return all_nodes[0];
	else
		return heapNode();
}

inline void TinHeap::trickleDown()
{	
	long index = 0;
	bool trickle = true;
	long swap_index;
	heapNode tempNode;
	while( trickle )
	{
		long kid1 = index*2 + 1;
		long kid2 = kid1 + 1;

		swap_index = maxValIndex( kid1, kid2 );
		if( swap_index > 0 && swap_index < (int)all_nodes.size() )
		{
			if( all_nodes[index] < all_nodes[swap_index] )
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

inline long TinHeap::maxValIndex( long kid1, long kid2 )
{
	long heap_size = all_nodes.size();
	if( kid2 >= heap_size || kid1 >= heap_size )
		return kid1;	
	else
	{	if( all_nodes[kid1] > all_nodes[kid2] )
			return kid1;
		else
			return kid2;
	}
}
