# ifndef VERTEX_TABLE_H
# define VERTEX_TABLE_H

# include "vertex.h"

class vertexTable
{
	public:
		vertexTable();
		~vertexTable();

		long add( vertex v );
		vertex getVertex( long index );
		void setVertex( vertex v, long index );
		void clear();
		long size();
		

	private:		
		std::deque<vertex> vertex_list;
};

# endif