# include "stdafx.h"
# include "graph.h"

graph::graph()
{	
}

graph::~graph()
{	for( int i = 0; i < (int)edges.size(); i++ )
	{	if( edges[i] != NULL )
			delete edges[i];
		edges[i] = NULL;
	}
	for( int j = 0; j < (int)graphnodes.size(); j++ )
	{	delete graphnodes[j];
		graphnodes[j] = NULL;
	}
}

long graph::Insert( edge * e, void * exParam, bool (* CALLBACK_EQUALS)( void * dataOne, void * dataTwo, void * exParam ) )
{	
	e->one->edges.clear();
	e->two->edges.clear();

	bool one_equal = false;
	bool two_equal = false;

	long oneIndex = 0, twoIndex = 0;

	for( int i = 0; i < (int)graphnodes.size(); i++ )
	{	
		if( !one_equal ) 
		{	if( CALLBACK_EQUALS(e->one->data, graphnodes[i]->data, exParam ) == true )
			{	one_equal = true;
				delete e->one;
				e->one = graphnodes[i];
				oneIndex = i;
			}
		}

		if( !two_equal )
		{	if( CALLBACK_EQUALS(e->two->data, graphnodes[i]->data, exParam ) == true )
			{	two_equal = true;
				delete e->two;
				e->two = graphnodes[i];
				twoIndex = i;
			}
		}
		
		//short circuit the loop
		if( one_equal && two_equal )
			break;
	}

	if( one_equal == false )
	{	oneIndex = graphnodes.size();
		graphnodes.push_back(e->one);
	}
	if( two_equal == false )
	{	twoIndex = graphnodes.size();	
		graphnodes.push_back(e->two);
	}

	e->oneIndex = oneIndex;
	e->twoIndex = twoIndex;
	long position = edges.size();
	e->one->edges.push_back(position);
	e->two->edges.push_back(position);
	edges.push_back( e );
	
	return position;
}

void graph::InsertBlank()
{	edges.push_back( NULL );
}

void graph::Save(const char * filename, void (*PRINT_DATA)( ofstream & out, void * data ) )
{	
	ofstream outf("graph.txt");

	for( int j = 0; j < (int)graphnodes.size(); j++ )
	{	PRINT_DATA( outf, graphnodes[j]->data );
		outf<<"\t:";
		for( int i = 0; i < (int)graphnodes[j]->edges.size(); i++ )
			outf<<graphnodes[j]->edges[i]<<" ";
		outf<<endl;
	}

	outf.close();
}