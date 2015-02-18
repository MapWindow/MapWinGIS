# ifndef GRAPH_H
# define GRAPH_H

# include "edge.h"
# include "graphnode.h"
//# include <fstream>

using namespace std;

class graph
{
public:

	graph();
	~graph();
	long Insert( edge * e, void * exParam, bool (* CALLBACK_EQUALS)( void * dataOne, void * dataTwo, void * exParam ) );
	void InsertBlank();
	void Save(const char * filename,void (*PRINT_DATA)( ofstream & out, void * data ));

	std::deque<edge *> edges;
	std::deque<graphnode *> graphnodes;
};

# endif