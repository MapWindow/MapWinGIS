//# include <iostream>
# include <assert.h>
# include <iomanip>
# include "XRedBlackNode.h"
# include "YRedBlackTree.h"

//using namespace std;

# ifndef XREDBLACKTREE_H
# define XREDBLACKTREE_H

class XRedBlack
{	
	public:
		XRedBlack()
			:Root(NULL){};
		~XRedBlack()
		{	Delete( Root );
		}
		void Delete()
		{	Delete( Root );
		}
		bool Insert( POINT X )
	    { return Insert( X, Root );
		}
		YRedBlackNode * GetNode( POINT P )
		{ return GetNode( P, Root );
		}
		/*
		void PrintTree()
		{
		  char c;
		  cout << "X RED BLACK TREE \n";
		  PrintTree(Root,1);
		  cout << "Press any Key and Enter to continue";
		  cin >> c;
		}
		*/
		
	private:
		void Delete( XRedBlackNode *& T );
		YRedBlackNode * GetNode( POINT P, XRedBlackNode * T );
		bool Insert( POINT X, XRedBlackNode * & T );
		RBDirection TooRed( XRedBlackNode * T);
		void Srotateleft( XRedBlackNode * & k2);
		void Srotateright( XRedBlackNode * & k2 );
		void Drotateright( XRedBlackNode * & k3 );
		void Drotateleft( XRedBlackNode * & k3 );
		//void PrintTree( XRedBlackNode * T,int indent );

		XRedBlackNode * Root;
};

# endif

