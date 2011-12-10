# ifndef HEAP_H
# define HEAP_H

# include "heap_node.h"
# include <deque>

class heap
{
	public:
		heap();
		~heap();
		void insert(long index, double value);
		void pop();
		heapnode top();
		void clear();
		long size();

	private:

		//Sorted by Index
		inline void trickleUp();
		inline void trickleDown();
		inline long minValIndex( long kid1, long kid2 );		
		std::deque<heapnode> all_nodes;		
};

# endif