#pragma once
#include <list>

namespace Coloring
{
	class ColorNode;

	class ColorEdge 
	{
	public:
		ColorEdge(): angle(0.0) {}
		double angle;
		ColorNode* n1;
		ColorNode* n2;
		void SetNodes(ColorNode* node1, ColorNode* node2);
		ColorNode* GetNeighbor(ColorNode* n);
	};

	class ColorNode
	{
	public:
		int id;
		int color;
		int count;
		int step;
		int spent;
		bool active;
		std::deque<ColorEdge*> edges;

		ColorNode(): color(-1), id(-1), step(-1), count(-1),  spent(-1), active(false)	{ }
		
		void UpdateNeighbours();
		void AssignColor(int step);
		ColorNode* GetNeighbor(int i);
		void IncrementSpentCount();
	};

	class ColorGraph
	{
	public:
		std::deque<ColorNode*> nodes;
	private:
		std::list<ColorNode*> candidates;
		std::map<int, ColorNode*> nodesMap;
		std::deque<ColorEdge*> edges;

		struct compareNodes: binary_function<ColorNode*, ColorNode*, bool> 
		{	
			bool operator()( ColorNode* const &left,  ColorNode* const &right)
			{   
				return left->id == right->id;
			};
		};
	public:
		ColorGraph() {}
		~ColorGraph()
		{
			Clear();
		}

		void Clear()
		{
			for(size_t i = 0; i < edges.size(); i++)
				delete edges[i];
			for(size_t i = 0; i < nodes.size(); i++)
				delete nodes[i];
			edges.clear();
			nodes.clear();
		}
		int ColorGraph::GetColorCount();
		bool ColorGraph::HasNonColoredNodes();
		bool DoColoring();
		void InsertNode(int id);
		void InsertEdge(int id1, int id2, double angle);
		void AddCandidates(ColorNode* n);
	};
}