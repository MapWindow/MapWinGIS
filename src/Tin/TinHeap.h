# ifndef TINHEAP_H
# define TINHEAP_H

# include "heap_node.h"
# include "tintypes.h"
# include "vertex.h"

class TinHeap
{
	public:
		TinHeap();
		~TinHeap();
		TinHeap( double minDeviation );
		void insert( long index, double value, vertex v );
		void pop();
		heapNode top();
		void clear();

	private:

		//Sorted by Index
		inline void trickleUp();
		inline void trickleDown();
		inline long maxValIndex( long kid1, long kid2 );		
		std::deque<heapNode> all_nodes;
		double minDeviation;
};

# endif