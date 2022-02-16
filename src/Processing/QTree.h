/////////////////////////////////////////////
//QTree.h the header of quadtree class
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
#pragma once
using namespace std;

#define MAX_LEAF_NODES 100

class QTreeExtent
{
public:
	double left;
	double right;
	double top;//max
	double bottom;//min
public:
	QTreeExtent() {
		this->left = this->right = this->top = this->bottom = 0.0;
	};
	QTreeExtent(double left, double right, double top, double bottom)
	{
		// Ensure the extent has a width > 0
		if (abs(left - right) < 0.0005) {
			left -= 0.00025;
			right += 0.00025;
		}
		// Ensure the extent has a height > 0
		if (abs(top - bottom) < 0.0005) {
			bottom -= 0.00025;
			top += 0.00025;
		}

		this->left = left;
		this->right = right;
		this->top = top;
		this->bottom = bottom;
	}
	bool Contain(const QTreeExtent& o)
	{
		return (left <= o.left
			&& right >= o.right
			&& top >= o.top
			&& bottom <= o.bottom);

	}

	bool ContainIn(const QTreeExtent& o)
	{
		return (left >= o.left
			&& right <= o.right
			&& top <= o.top
			&& bottom >= o.bottom);

	}

	bool IntersectIn(QTreeExtent& o)
	{
		return !(o.right < left
			|| o.left > right
			|| o.top < bottom
			|| o.bottom > top
			);
	}
	QTreeExtent& operator= (const QTreeExtent& o)
	{
		if (this == &o)
		{
			return *this;
		}

		this->left = o.left;
		this->right = o.right;
		this->top = o.top;
		this->bottom = o.bottom;
		return *this;
	}
};

struct QTreeNode
{
	QTreeExtent Extent;
	int index;//index for storaged element
};

class QTree
{
private:
	QTree* LT, * RT, * LB, * RB;	//Four corners
	vector<QTreeNode*> nodes;	//nodes

	QTreeExtent extent;
	void Regenerate();
	bool isFull;
	bool regenerating;
public:
	QTree() { LT = RT = LB = RB = nullptr; isFull = false; regenerating = false; }
	QTree(QTreeExtent);
	~QTree(void);

	void AddNode(const QTreeNode&);
	bool RemoveNode(int index);//return if success
	vector<int> GetNodes(QTreeExtent queryExtent);//Query Nodes
};
