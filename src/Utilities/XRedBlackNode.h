# ifndef XREDBLACKNODE_H
# define XREDBLACKNODE_H

# include "YRedBlackTree.h"

class XRedBlackNode
{
	public:
		
		XRedBlackNode( POINT E, YRedBlack * ET = NULL, TreeNodeColor C = red)
			:Element(E), ElementTree(ET), branchColor(C), Left(NULL), Right(NULL){};
		~XRedBlackNode()
		{
		}

	public:
		XRedBlackNode * Left;
		XRedBlackNode * Right;
		YRedBlack * ElementTree;
		POINT Element;
		TreeNodeColor branchColor;
};

# endif


