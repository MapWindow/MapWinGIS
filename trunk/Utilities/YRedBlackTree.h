# include <assert.h>
# include <iomanip>
# include "YRedBlackNode.h"

# ifndef YREDBLACKTREE_H
# define YREDBLACKTREE_H

class YRedBlack
{	
	public:
		YRedBlack()
			:Root(NULL){};
		~YRedBlack()
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
		  cout << "Y RED BLACK TREE \n";
		  PrintTree(Root,1);
		  cout << "Press any Key and Enter to continue";
		  cin >> c;
		}
		*/
		
	private:
		void Delete( YRedBlackNode *& T );
		YRedBlackNode * GetNode( POINT P, YRedBlackNode * T );
		bool Insert( POINT X, YRedBlackNode * & T );
		RBDirection TooRed( YRedBlackNode * T);
		void Srotateleft( YRedBlackNode * & k2);
		void Srotateright( YRedBlackNode * & k2 );
		void Drotateright( YRedBlackNode * & k3 );
		void Drotateleft( YRedBlackNode * & k3 );
		//void PrintTree( YRedBlackNode * T,int indent );

		YRedBlackNode * Root;
};

# endif

