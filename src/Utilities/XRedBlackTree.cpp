# include "stdafx.h"
# include "XRedBlackTree.h"

bool XRedBlack::Insert( POINT X, XRedBlackNode * & T )
{
    if( T == NULL )	// Create a one node tree.
    {
		YRedBlack * Y = new YRedBlack();
		Y->Insert( X );
        T = new XRedBlackNode( X, Y );
        assert(T!=NULL);//  "Out of space" 
        return true;
    }

	//Going Into Recursion
	if(	T->Left!=NULL && T->Right!=NULL)
		if (T->Left->branchColor == red && T->Right->branchColor == red)
		{	T->branchColor = red;
			T->Left->branchColor = black;
			T->Right->branchColor = black;
		}

    if( X.x < T->Element.x )
    {
        if (Insert( X,  T->Left ))
		{ 
			if( TooRed( T ) == Left )
			{	Srotateright( T );
				//cout<<"Single Rotation Right"<<endl;
			}
			else if( TooRed( T ) == LeftRight )
			{	Drotateright( T );
				//cout<<"Double Rotation Right"<<endl;
			}			
			return true;
		}
		 return false;
    }
    if( X.x > T->Element.x )
    {
       if(Insert( X, T->Right ))
	   {
			if( TooRed( T ) == Right )
			{	Srotateleft( T );
				//cout<<"Single Rotation Left"<<endl;
			}
			else if( TooRed( T ) == RightLeft )
			{   Drotateleft( T );
				//cout<<"Double Rotation Left"<<endl;
			}
			return true;
	  }
    }
    // Else X is in the tree already, so we'll add it to the Y tree
	if( X.x == T->Element.x )
	{	return T->ElementTree->Insert( X );
	}
    return false;
}

//Returns the side that is heavy in Red
RBDirection XRedBlack::TooRed( XRedBlackNode * T)
{	
	if( T->Left !=NULL && T->Left->branchColor == red)
	{	
		if( T->Left->Left != NULL && T->Left->Left->branchColor == red)
			return Left;
		if( T->Left->Right != NULL && T->Left->Right->branchColor == red)
			return LeftRight;
	}

	if( T->Right != NULL && T->Right->branchColor == red)
	{	
		if( T->Right->Right != NULL && T->Right->Right->branchColor == red)
			return Right;
		if( T->Right->Left != NULL && T->Right->Left->branchColor == red)
			return RightLeft;
	}

	return NoDirection;
}

// Perform a left rotation at the tree rooted at k2
void XRedBlack::Srotateleft( XRedBlackNode * & k2 )
{ assert(k2!= NULL);
  //cout << "Srotateleft " << k2->Element << endl;
  XRedBlackNode *k1 =  k2->Right;
     assert(k1 != NULL);
  k2->Right = k1->Left;
  k1->Left = k2;
  TreeNodeColor temp=k2->branchColor;
  k2->branchColor=k1->branchColor;
  k1->branchColor=temp;					  
  k2 = k1;
}

void XRedBlack::Srotateright( XRedBlackNode * & k2 )
{   assert(k2!=NULL);
    //cout << "Srotateright " << k2->Element << endl;
    XRedBlackNode *k1 = k2->Left;
       assert(k1 != NULL);
    k2->Left = k1->Right;
    k1->Right = k2;
	TreeNodeColor temp=k2->branchColor;
	k2->branchColor=k1->branchColor;
	k1->branchColor=temp;

    k2 = k1;
}

void XRedBlack::Drotateleft( XRedBlackNode * & k3 )
{ 
  Srotateright ( ( XRedBlackNode * & ) k3->Right);
  Srotateleft ( k3 );

}

// Perform a double right rotation at the node rooted at k3
void XRedBlack::Drotateright( XRedBlackNode * & k3 )
{
    Srotateleft( ( XRedBlackNode * & ) k3->Left );
    Srotateright( k3 );
}

/*
void XRedBlack::PrintTree( XRedBlackNode * T,int indent ) 
{ 
  if (T==NULL) return;
  PrintTree (T->Right,indent+1);
  cout << setw(indent*3) << " ";
  cout << T->Element.x << "(";
	  if(T->branchColor == red)
		  cout<<"Red)"<< endl;
	  else
		  cout<<"Black)"<<endl;
  PrintTree (T->Left,indent+1);
}
*/

YRedBlackNode * XRedBlack::GetNode( POINT P, XRedBlackNode * T )
{	
	if( T == NULL )
		return NULL;
	else if( T->Element.x > P.x )
		return GetNode( P, T->Left );
	else if( T->Element.x < P.x )
		return GetNode( P, T->Right );
	else if( T->Element.x == P.x )
		return T->ElementTree->GetNode( P );	

	return NULL;
}

void XRedBlack::Delete( XRedBlackNode *& T )
{	
	if( T == NULL )
		return;
	
	Delete( T->Left );
	Delete( T->Right );

	if( T->ElementTree != NULL )
		T->ElementTree->Delete();
	delete T;
	T = NULL;	
}