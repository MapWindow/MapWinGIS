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
#include "stdafx.h"
#include "QTree.h"

QTree::QTree(QTreeExtent extent)
	:extent(extent)
{
	LT = NULL;
	RT = NULL;
	LB = NULL;
	RB = NULL;
	isFull = false;
	regenerating = false;
}

QTree::~QTree(void)
{
	for(unsigned int i = 0;i<nodes.size(); i++)
	{
		delete nodes[i];
	}
	nodes.clear();
	//clear four corners
	if(LT!=NULL)delete LT;
	if(RT!=NULL)delete RT;
	if(LB!=NULL)delete LB;
	if(RB!=NULL)delete RB;
}

void QTree::Regenerate()
{
	unsigned int nodeNumber = nodes.size();
	double middleX = (this->extent.left + this->extent.right) /2;
	double middleY = (this->extent.top + this->extent.bottom) /2;
	
	///vector<QTreeNode*> nodesCross(MAX_LEAF_NODES);//Crossed Shapes
	
	for(int i = nodeNumber-1 ; i >=0; i--)
	{
		//not exist in any two areas
		if( ! ( nodes[i]->Extent.left <= middleX 
			&& nodes[i]->Extent.right > middleX
			|| 
			nodes[i]->Extent.top > middleY 
			&& nodes[i]->Extent.bottom <= middleY))
		{
			//nodesCross.push_back(new QTreeNode(Node));
			QTreeNode* node = nodes[i];
			nodes.erase(nodes.begin() + i);
			this->AddNode(*node);
			//just move away one element
			break;
			
		}
	}
}

void QTree::AddNode(const QTreeNode& Node)
{
	//if the Node overflow the extent of QuadTree
	if( ! this->extent.Contain(Node.Extent))
	{
		//if the old qtree is empty
		if(this->extent.left == 0 &&
			this->extent.right == 0 && 
			this->extent.top == 0 &&
			this->extent.bottom == 0)
		{
			this->extent = Node.Extent;
		}
		else
		{
			QTree* oldQtree = new QTree(
				QTreeExtent(this->extent.left,
				this->extent.right,this->extent.top,this->extent.bottom));
			oldQtree->LT  = this->LT;
			oldQtree->RT = this->RT;
			oldQtree->LB = this->LB;
			oldQtree->RB = this->RB;
			oldQtree->nodes = this->nodes;
			oldQtree->isFull = this->isFull;
			oldQtree->regenerating = this->regenerating;

			this->LT = NULL;
			this->RT = NULL;
			this->LB = NULL;
			this->RB = NULL;

			if(Node.Extent.left < this->extent.left )
			{
				this->extent.left -= (this->extent.right - this->extent.left);
				if(Node.Extent.top > this->extent.top)
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
			else if(Node.Extent.right > this->extent.right || Node.Extent.top > this->extent.top || Node.Extent.bottom < this->extent.bottom)
			{

				this->extent.right += (this->extent.right - this->extent.left);
				if(Node.Extent.top > this->extent.top)
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
			AddNode(Node);
			return ;
		}
	}

	//Count the nodes of this leaf
	
	if(nodes.size() < MAX_LEAF_NODES && isFull == false)
	{
		
		//add node into this leaf
		nodes.push_back(new QTreeNode(Node));
		
	}
	else if(nodes.size() == MAX_LEAF_NODES && isFull == false)
	{
		//need to regenerate

		isFull = true;//set the flag
		this->Regenerate();//call for re-generate

		//after regenerated the tree
		//continue to add node
		this->AddNode(Node);
	}
	else
	{
		double middleX = (this->extent.left + this->extent.right) /2;
		double middleY = (this->extent.top + this->extent.bottom) /2;
		
		//Exist in any two areas
		if( Node.Extent.left <= middleX && Node.Extent.right > middleX
			|| 
			Node.Extent.top > middleY && Node.Extent.bottom <= middleY)
		{
			nodes.push_back(new QTreeNode(Node));
		}
		else if( Node.Extent.right <= middleX && Node.Extent.bottom > middleY)
		{
			//LT
			if(LT ==NULL)
			{
				LT = new QTree(QTreeExtent(this->extent.left
					,middleX, this->extent.top, middleY));
				
			}
			LT->AddNode(Node);
		}
		else if( Node.Extent.left > middleX && Node.Extent.bottom > middleY)
		{
			//RT
			if(RT == NULL)
			{
				RT = new QTree(QTreeExtent(middleX,this->extent.right,
					this->extent.top, middleY));
				
			}
			RT->AddNode(Node);
		}
		else if( Node.Extent.right <= middleX && Node.Extent.top < middleY)
		{
			//LB
			if(LB == NULL)
			{
				LB = new QTree(QTreeExtent(this->extent.left,middleX,
					middleY,this->extent.bottom));
				
			}
			LB->AddNode(Node);
		}
		else //if(Node.Extent.left > middleY && Node.Extent.top < middleY)
		{
			//RB
			if(RB == NULL)
			{
				RB = new QTree(QTreeExtent(middleX, this->extent.right,
					middleY, this->extent.bottom));
				
			}
			RB->AddNode(Node);
		}
	}
		
}

bool QTree::RemoveNode(int index)
{
	for(int i = nodes.size() -1 ; i >=0; i--)
	{
		if(nodes[i]->index == index)
		{
			//found in this leaf
			delete nodes[i];
			nodes.erase(nodes.begin() + i);
			return true;
		}
		
	}
	if(LT != NULL && LT->RemoveNode(index))
	{
		return true;
	}
	else if(RT != NULL && RT->RemoveNode(index))
	{
		return true;
	}
	else if(LB != NULL && LB->RemoveNode(index))
	{
		return true;
	}
	else if(RB != NULL && RB->RemoveNode(index))
	{
		return true;
	}
	else
		return false;
}

vector<int> QTree::GetNodes(QTreeExtent QueryExtent)
{
	vector<int> result;
	if(!this->extent.IntersectIn(QueryExtent))
	/*if(QueryExtent.right <= this->extent.left 
		|| QueryExtent.left >= this->extent.right
		|| QueryExtent.top <= this->extent.bottom
		|| QueryExtent.bottom >= this->extent.top)*/
	{
		return result;
	}
	
	unsigned int nodeNumber = nodes.size();
	double middleX = (this->extent.left + this->extent.right) /2;
	double middleY = (this->extent.top + this->extent.bottom) /2;
	
	if(LT != NULL)
	{
		vector<int> lt = LT->GetNodes(QueryExtent);
		if(lt.size()>0)
		{
			for(unsigned int i = 0; i< lt.size(); i++)
				result.push_back(lt[i]);
		}
	}
	if(RT != NULL)
	{
		vector<int> rt = RT->GetNodes(QueryExtent);
		if(rt.size()>0)
		{
			for(unsigned int i = 0; i< rt.size(); i++)
				result.push_back(rt[i]);
		}
	}
	if(LB != NULL)
	{
		vector<int> lb = LB->GetNodes(QueryExtent);
		if(lb.size()>0)
		{
			for(unsigned int i = 0; i< lb.size(); i++)
				result.push_back(lb[i]);
		}
	}

	if(RB != NULL)
	{
		vector<int> rb = RB->GetNodes(QueryExtent);
		if(rb.size()>0)
		{
			for(unsigned int i = 0; i< rb.size(); i++)
				result.push_back(rb[i]);
		}
	}
	for(int i = nodes.size()-1; i>=0; i--)
	{
		if(nodes[i]->Extent.IntersectIn(QueryExtent))
			result.push_back(nodes[i]->index);
	}
	return result;

}
