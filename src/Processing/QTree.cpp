/////////////////////////////////////////////
// QTree.cpp
//Description: This class is a general component that can make quadtree support for other components
////////////////////////////////////////////
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
// The original Author is Neio(neio.zhou@gmail.com) and released as public domain in July 2009 to be a part of MapWinGIS.
// The contributors should list themselves and their modifications here.
//////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "QTree.h"

QTree::QTree(QTreeExtent extent)
	:extent(extent)
{
	LT = nullptr;
	RT = nullptr;
	LB = nullptr;
	RB = nullptr;
	isFull = false;
	regenerating = false;
}

QTree::~QTree(void)
{
	for (const auto& node : nodes)
	{
		delete node;
	}
	nodes.clear();

	//clear four corners
	delete LT;
	delete RT;
	delete LB;
	delete RB;
}

void QTree::Regenerate()
{
	const unsigned int nodeNumber = nodes.size();
	const double middleX = (this->extent.left + this->extent.right) / 2;
	const double middleY = (this->extent.top + this->extent.bottom) / 2;

	///vector<QTreeNode*> nodesCross(MAX_LEAF_NODES);//Crossed Shapes

	for (int i = nodeNumber - 1; i >= 0; i--)
	{
		//not exist in any two areas
		if (!(nodes[i]->Extent.left <= middleX
			&& nodes[i]->Extent.right > middleX
			||
			nodes[i]->Extent.top > middleY
			&& nodes[i]->Extent.bottom <= middleY))
		{
			//nodesCross.push_back(new QTreeNode(Node));
			const QTreeNode* node = nodes[i];
			nodes.erase(nodes.begin() + i);
			this->AddNode(*node);
			//just move away one element
			break;
		}
	}
}

void QTree::AddNode(const QTreeNode& node)
{
	//if the Node overflow the extent of QuadTree
	if (!this->extent.Contain(node.Extent))
	{
		//if the old qtree is empty
		if (this->extent.left == 0 &&
			this->extent.right == 0 &&
			this->extent.top == 0 &&
			this->extent.bottom == 0)
		{
			this->extent = node.Extent;
		}
		else
		{
			const auto oldQtree = new QTree(QTreeExtent(this->extent.left, this->extent.right, this->extent.top, this->extent.bottom));
			oldQtree->LT = this->LT;
			oldQtree->RT = this->RT;
			oldQtree->LB = this->LB;
			oldQtree->RB = this->RB;
			oldQtree->nodes = this->nodes;
			oldQtree->isFull = this->isFull;
			oldQtree->regenerating = this->regenerating;

			this->LT = nullptr;
			this->RT = nullptr;
			this->LB = nullptr;
			this->RB = nullptr;

			if (node.Extent.left < this->extent.left)
			{
				this->extent.left -= (this->extent.right - this->extent.left);
				if (node.Extent.top > this->extent.top)
				{
					//put this qtre as new qtree's RB
					this->RB = oldQtree;
					this->extent.top += (this->extent.top - this->extent.bottom);
				}
				else
				{
					//put this qtree as new qtree's RT
					this->RT = oldQtree;
					this->extent.bottom -= (this->extent.top - this->extent.bottom);
				}
			}
			else if (node.Extent.right > this->extent.right || node.Extent.top > this->extent.top || node.Extent.bottom < this->extent.bottom)
			{

				this->extent.right += (this->extent.right - this->extent.left);
				if (node.Extent.top > this->extent.top)
				{
					this->LB = oldQtree;

					this->extent.top += (this->extent.top - this->extent.bottom);
				}
				else
				{
					this->LT = oldQtree;

					this->extent.bottom -= (this->extent.top - this->extent.bottom);
				}
			}

			this->nodes.clear();
			this->isFull = false;
			this->regenerating = false;
			AddNode(node);
			return;
		}
	}

	//Count the nodes of this leaf

	if (nodes.size() < MAX_LEAF_NODES && isFull == false)
	{

		//add node into this leaf
		nodes.push_back(new QTreeNode(node));

	}
	else if (nodes.size() == MAX_LEAF_NODES && isFull == false)
	{
		//need to regenerate

		isFull = true;//set the flag
		this->Regenerate();//call for re-generate

		//after regenerated the tree
		//continue to add node
		this->AddNode(node);
	}
	else
	{
		const double middleX = (this->extent.left + this->extent.right) / 2;
		const double middleY = (this->extent.top + this->extent.bottom) / 2;

		//Exist in any two areas
		if (node.Extent.left <= middleX && node.Extent.right > middleX
			||
			node.Extent.top > middleY && node.Extent.bottom <= middleY)
		{
			nodes.push_back(new QTreeNode(node));
		}
		else if (node.Extent.right <= middleX && node.Extent.bottom > middleY)
		{
			//LT
			if (LT == nullptr)
			{
				LT = new QTree(QTreeExtent(this->extent.left
					, middleX, this->extent.top, middleY));

			}
			LT->AddNode(node);
		}
		else if (node.Extent.left > middleX && node.Extent.bottom > middleY)
		{
			//RT
			if (RT == nullptr)
			{
				RT = new QTree(QTreeExtent(middleX, this->extent.right,
					this->extent.top, middleY));

			}
			RT->AddNode(node);
		}
		else if (node.Extent.right <= middleX && node.Extent.top < middleY)
		{
			//LB
			if (LB == nullptr)
			{
				LB = new QTree(QTreeExtent(this->extent.left, middleX,
					middleY, this->extent.bottom));

			}
			LB->AddNode(node);
		}
		else //if(Node.Extent.left > middleY && Node.Extent.top < middleY)
		{
			//RB
			if (RB == nullptr)
			{
				RB = new QTree(QTreeExtent(middleX, this->extent.right,
					middleY, this->extent.bottom));

			}
			RB->AddNode(node);
		}
	}

}

bool QTree::RemoveNode(int index)
{
	for (int i = nodes.size() - 1; i >= 0; i--)
	{
		if (nodes[i]->index == index)
		{
			//found in this leaf
			delete nodes[i];
			nodes.erase(nodes.begin() + i);
			return true;
		}

	}
	if (LT != nullptr && LT->RemoveNode(index))
	{
		return true;
	}
	if (RT != nullptr && RT->RemoveNode(index))
	{
		return true;
	}
	if (LB != nullptr && LB->RemoveNode(index))
	{
		return true;
	}
	if (RB != nullptr && RB->RemoveNode(index))
	{
		return true;
	}
	return false;
}

vector<int> QTree::GetNodes(QTreeExtent queryExtent)
{
	vector<int> result;
	if (!this->extent.IntersectIn(queryExtent))
		/*if(QueryExtent.right <= this->extent.left
			|| QueryExtent.left >= this->extent.right
			|| QueryExtent.top <= this->extent.bottom
			|| QueryExtent.bottom >= this->extent.top)*/
	{
		return result;
	}

	//unsigned int nodeNumber = nodes.size();
	//double middleX = (this->extent.left + this->extent.right) / 2;
	//double middleY = (this->extent.top + this->extent.bottom) / 2;

	if (LT != nullptr)
	{
		const vector<int> lt = LT->GetNodes(queryExtent);
		if (!lt.empty())
		{
			for (int i : lt)
				result.push_back(i);
		}
	}
	if (RT != nullptr)
	{
		const vector<int> rt = RT->GetNodes(queryExtent);
		if (!rt.empty())
		{
			for (int i : rt)
				result.push_back(i);
		}
	}
	if (LB != nullptr)
	{
		const vector<int> lb = LB->GetNodes(queryExtent);
		if (!lb.empty())
		{
			for (int i : lb)
				result.push_back(i);
		}
	}

	if (RB != nullptr)
	{
		const vector<int> rb = RB->GetNodes(queryExtent);
		if (!rb.empty())
		{
			for (int i : rb)
			{
				result.push_back(i);
			}
		}
	}
	for (int i = nodes.size() - 1; i >= 0; i--)
	{
		if (nodes[i]->Extent.IntersectIn(queryExtent))
			result.push_back(nodes[i]->index);
	}
	return result;
}
