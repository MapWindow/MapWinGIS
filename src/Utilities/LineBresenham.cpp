
#include "stdafx.h"
#include "LineBresenham.h"


//contructor and destructor
LineBresenham::LineBresenham()
{

}

LineBresenham::~LineBresenham()
{
	
}

/////////////////////////////////////////////////////////////////////
//
// This is where it happens, the Bresenham line drawing algorithm.
//
std::vector<lbPoint> LineBresenham::ComputeLinePoints(struct lbPoint StartPoint, struct lbPoint EndPoint)
{
int nDy, nDx;  //overall change in x and y
double slope;
int nFactorX, nFactorY;
int nCurX, nCurY, nP, nTwoDy, nTwoDyDx;

//this is the deque of points to be returned...
PointVector LinePoints;


	//*******************************
	//check for a verticle line first
	//*******************************
	if ( StartPoint.x == EndPoint.x)
	{
		if (StartPoint.y > EndPoint.y)
		{
			//swap the start and ending locations
			int TempVal = StartPoint.y;
			StartPoint.y = EndPoint.y;
			EndPoint.y = TempVal;
		}
		
		int Range = EndPoint.y - StartPoint.y;
		

		//if no length, draw the one pixel and exit
		if (Range == 0)
		{
			LinePoints.push_back(lbPoint(StartPoint.x,StartPoint.y));
			return LinePoints;
		}
		
		//draw from both ends of the line to save calculation time
		for (int CurPixel = 0; CurPixel <= Range/2; CurPixel++)
		{
			LinePoints.push_back(lbPoint(StartPoint.x, StartPoint.y + CurPixel));
			LinePoints.push_back(lbPoint(StartPoint.x, EndPoint.y - CurPixel));
		}

		return LinePoints;
	}
	
	//*******************************
	//this function sets up the points
	//in the order that we are expecting
	//them to be in.
	//*******************************
	SetupPoints(StartPoint,EndPoint,nDx,nDy,slope,nFactorX,nFactorY);

	
	//*******************************
	//deal with any line with a slope
	//between -1 and 1
	//*******************************
	if ( (slope <= 1.0) && (slope >= -1.0) ) // -1 <= slope <= 1
	{
		nCurX = StartPoint.x;

		//nFactorY Translates the point into the first octant if necessary
		//then is used later to translate it back if necessary
		nCurY = StartPoint.y * nFactorY; 
		
		//we have now translated everything into the first octant,
		//and the points are in the correct order, so we work with
		//positive nDy and positive nDx
		nDy = abs(nDy);
		nDx = abs(nDx);

		nTwoDy = 2*nDy;
		nP = nTwoDy - nDx;
		nTwoDyDx = 2*(nDy - nDx);

		//add the first point		
		LinePoints.push_back( lbPoint(nCurX,nFactorY*nCurY) );

		while ( nCurX < EndPoint.x)
		{
			nCurX++;
			if (nP < 0)
				nP += nTwoDy;
			else
			{
				nCurY++;
				nP += nTwoDyDx;
			}
			LinePoints.push_back( lbPoint(nCurX,nFactorY*nCurY) );
			
		}
	}
	//*******************************
	//deal with slopes outside of the 
	// -1 to 1 area
	//*******************************
	else // slope > 1 and slope < -1
	{
		//nFactorY and nFactorX Translate the point into the first octant if necessary
		//then is used later to translate it back if necessary
		nCurX = StartPoint.x * nFactorX;		
		nCurY = StartPoint.y * nFactorY; 
		
		//we have now translated everything into the first octant,
		//and the points are in the correct order, so we work with
		//positive nDy and positive nDx
		
		nDy = abs(nDy);
		nDx = abs(nDx);

		//NOTE:  for this situation we have to swap dx and dy because we are 
		//working in the y direction
		nTwoDy = 2*nDy;
		nP = nTwoDy - nDx;
		nTwoDyDx = 2*(nDy - nDx);


		//add the starting point
		LinePoints.push_back( lbPoint(nFactorY*nCurY,nFactorX*nCurX) );
			
		
		while ( nCurX < nFactorX*EndPoint.x)
		{
			nCurX++;
			if (nP < 0)
				nP += nTwoDy;
			else
			{
				nCurY++;
				nP += nTwoDyDx;
			}
			LinePoints.push_back( lbPoint(nFactorY*nCurY,nFactorX*nCurX) );
		}
		
	}
	
return LinePoints;
}


//this function looks at the start and endpoints, swaps them if necessary
//and computes Multiplicity factors used in tranlation to the first octant and back
inline void LineBresenham::SetupPoints(lbPoint &StartPoint, lbPoint &EndPoint, int &nDx, int &nDy, double &slope, int &nFactorX, int &nFactorY)
{
int TempValue;
	nDx = EndPoint.x - StartPoint.x;
	nDy = EndPoint.y - StartPoint.y;
	
	if (abs(nDx) >= abs(nDy)) //we are going to move along the x-axis (slope between 1 and -1)
	{
		
		if ( nDx < 0) //points are in the wrong order, swap them
		{
			SwapPoints(StartPoint,EndPoint);
			//we have swapped the points, so nDx and nDy change signes
			nDx = -1*nDx;
			nDy = -1*nDy;
		}

		slope = (double)nDy / nDx;

		if ( slope < 0 ) //slope between 0 and -1
		{
			nFactorY = -1;
			nFactorX = 1;
		}
		else //slope between 0 and 1
		{
			nFactorY = 1;
			nFactorX = 1;
		}
	}
	else //(nDy > nDx) we are going to move along y-axis [ abs(slope) > 1 ]
	{
		if ( nDy < 0) //points are in the wrong order, swap them
		{
			SwapPoints(StartPoint,EndPoint);
			//we have swapped the points, so nDx and nDy change signes
			nDx = -1*nDx;
			nDy = -1*nDy;
		}

		slope = (double)nDy / nDx;

		//now we must swap the x's and y's so that we are working in the 
		//first octant
		
		//swap first point
		TempValue = StartPoint.x;
		StartPoint.x = StartPoint.y;
		StartPoint.y = TempValue;

		//swap second point
		TempValue = EndPoint.x;
		EndPoint.x = EndPoint.y;
		EndPoint.y = TempValue;

		//swap dx and dy
		TempValue = nDx;
		nDx = nDy;
		nDy = TempValue;

		if ( slope < 0 ) //slope less than -1
		{
			nFactorY = -1;
			nFactorX = 1;
		}
		else//slope greater than 1
		{
			nFactorY = 1;
			nFactorX = 1;
			
		}


	}

}


//...self explanatory...
void LineBresenham::SwapPoints(lbPoint &p1, lbPoint &p2)
{
	lbPoint TempPoint;

	TempPoint.x = p1.x;
	TempPoint.y = p1.y;

	p1.x = p2.x;
	p1.y = p2.y;
	
	
	p2.x = TempPoint.x;
	p2.y = TempPoint.y;
}
