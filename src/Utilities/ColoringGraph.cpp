#include "stdafx.h"
#include "ColoringGraph.h"
#include <set>
#include <algorithm>
#define NO_COLOR -1

namespace Coloring
{
	// ********************************************************
	//     ColorEdge::SetNodes()
	// ********************************************************
	void ColorEdge::SetNodes(ColorNode* node1, ColorNode* node2)
	{
		n1 = node1;
		n2 = node2;
		n1->edges.push_back(this);
		n2->edges.push_back(this);
	}

	// ********************************************************
	//     ColorEdge::GetNeighbor()
	// ********************************************************
	ColorNode* ColorEdge::GetNeighbor(ColorNode* n)
	{
		return n1->id != n->id ? n1 : n2;
	}

	// ********************************************************
	//     ColorNode::IncrementSpentCount()
	// ********************************************************
	void ColorNode::IncrementSpentCount()
	{
		if (this->color == NO_COLOR)
			this->spent++;
	}

	// ********************************************************
	//     ColorNode::UpdateNeighbours()
	// ********************************************************
	void ColorNode::UpdateNeighbours()
	{
		for(size_t i = 0; i < this->edges.size(); i++)
		{
			this->GetNeighbor(i)->IncrementSpentCount();
		}
	}

	// ********************************************************
	//     ColorNode::GetNeighbor()
	// ********************************************************
	ColorNode* ColorNode::GetNeighbor(int i)
	{
		return this->edges[i]->GetNeighbor(this);
	}

	// ********************************************************
	//     ColorNode::AssignColor()
	// ********************************************************
	void ColorNode::AssignColor(int step)
	{
		int newColor = 0;

		bool found = false;
		do
		{
			found = false;
			// choose the smallest one not used by neighbors
			for(size_t i = 0; i < edges.size(); i++)
			{
				ColorNode* n = edges[i]->GetNeighbor(this);
				if (n->color == newColor)
				{
					newColor++;
					found = true;
					break;
				}
			}
		}
		while(found);

		this->spent = -1;
		this->color = newColor;
		this->step = step;

		UpdateNeighbours();
	}

	// ********************************************************
	//     ColorGraph::InsertNode()
	// ********************************************************
	void ColorGraph::InsertNode(int id)
	{
		if (nodesMap.find(id) == nodesMap.end())
		{
			ColorNode* node = new ColorNode();
			node->id = id;
			nodesMap[id] = node;
			nodes.push_back(node);
		}
	}

	// ********************************************************
	//     ColorGraph::InsertEdge()
	// ********************************************************
	void ColorGraph::InsertEdge(int id1, int id2, double angle)
	{
		if (nodesMap.find(id1) == nodesMap.end())
			InsertNode(id1);

		if (nodesMap.find(id2) == nodesMap.end())
			InsertNode(id2);

		ColorEdge* edge = new ColorEdge();
		edge->SetNodes(nodesMap[id1], nodesMap[id2]);
		edge->angle = angle;
		edges.push_back(edge);
	}

	// ********************************************************
	//     ColorGraph::GetColorCount()
	// ********************************************************
	int ColorGraph::GetColorCount()
	{
		std::set<int> colors;
		for(size_t i = 0; i < nodes.size(); i++)
		{
			if (nodes[i]->color == NO_COLOR)
				continue;

			if (colors.find(nodes[i]->color) == colors.end())
			{
				colors.insert(nodes[i]->color);
			}
		}
		return colors.size();
	}

	// ********************************************************
	//     ColorGraph::HasNonColoredNodes()
	// ********************************************************
	bool ColorGraph::HasNonColoredNodes()
	{
		for(size_t i = 0; i < nodes.size(); i++)
		{
			if (nodes[i]->color == NO_COLOR)
				return true;
		}
		return false;
	}

	// ********************************************************
	//     ColorGraph::AddCandidates()
	// ********************************************************
	void ColorGraph::AddCandidates(ColorNode* n)
	{
		for(size_t i = 0; i < n->edges.size(); i++)
		{
			ColorNode* nb = n->GetNeighbor(i);
			if (nb->spent == 0)
				candidates.push_back(nb);
		}
	}

	// ********************************************************
	//     ColorGraph::DoColoring()
	// ********************************************************
	bool ColorGraph::DoColoring()
	{
		if (nodes.size() == 0)
			return false;

		candidates.clear();
		int step = 0;
		int count = 0;
		
		do 
		{
			// setting a seed
			int seedId = 0;
			if (count == 0) {
				seedId = rand() % (nodes.size() - 1);
			}
			else {
				std::vector<int> nonColored;
				for(size_t i = 0; i < nodes.size(); i++)
				{
					if (nodes[i]->color == -1)
						nonColored.push_back(i);
				}
				seedId = nonColored.size() > 10 ? nonColored[rand() % (nonColored.size() - 1)] : nonColored[0];
			}

			ColorNode* n = nodes[seedId];
			n->AssignColor(step++);
			AddCandidates(n);
			
			bool proceed = true;
			do
			{
				proceed = false;
				ColorNode* maxNode;
				int max = -2;
				std::list<ColorNode*>::iterator it = candidates.begin();
				while(it != candidates.end())
				{
					if ((*it)->spent > max)
					{
						max = (*it)->spent;
						maxNode = *it;
						proceed = true;
					}
					it++;
				}
				
				maxNode->AssignColor(step++);
				maxNode->count = count++;
				candidates.remove(maxNode);
				AddCandidates(maxNode);
			} 
			while(proceed);
		} 
		while(HasNonColoredNodes());

		candidates.clear();
		return true;
	}
}