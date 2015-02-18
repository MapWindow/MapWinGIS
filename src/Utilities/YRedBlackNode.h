# ifndef YREDBLACKNODE_H
# define YREDBLACKNODE_H

enum RBDirection{Right,RightLeft,Left,LeftRight,NoDirection};
enum TreeNodeColor{ red, black};

class YRedBlackNode
{
	public:
		
		YRedBlackNode( POINT E, TreeNodeColor C = red )
			:Element(E), branchColor(C), Left(NULL), Right(NULL), useCount(0), isColinear(false), canSetColinear(true){};
		~YRedBlackNode()
		{
		}

	public:
		YRedBlackNode * Left;
		YRedBlackNode * Right;
		POINT Element;
		TreeNodeColor branchColor;
		short useCount;
		bool isColinear;
		bool canSetColinear;
};

# endif


