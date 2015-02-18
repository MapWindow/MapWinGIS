# ifndef GRAPH_NODE
# define GRAPH_NODE

class graphnode
{
public:

	graphnode()
	{	data = NULL;
	}
	~graphnode()
	{	if( data != NULL )
			delete data;
		data = NULL;
	}

	void * data;
	std::deque<long> edges;
};

# endif