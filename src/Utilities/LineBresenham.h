#ifndef _LINEBREZENHAM_H_
#define _LINEBREZENHAM_H_

#include <math.h>

//Point struct to make sure that any platform 
//has access to this simple object
struct lbPoint
{
	long x;
	long y;
	//"constructors" for struct
	lbPoint(long newX, long newY)
	{
		x = newX;
		y = newY;
	}
	lbPoint()
	{
		x = 0;
		y = 0;
	}
	
};

typedef std::vector<lbPoint> PointVector;

/////////////////////////////////////////////////////////////////////
//
// This class implements the Bresenham line drawing technique.
//
class LineBresenham
{
public:
	LineBresenham();
	virtual ~LineBresenham();
	std::vector<lbPoint> ComputeLinePoints(struct lbPoint StartPoint, struct lbPoint EndPoint);
	

private:
	void SwapPoints(lbPoint &p1, lbPoint &p2);
	inline void SetupPoints(lbPoint &StartPoint, lbPoint &EndPoint, int &nDx, int &nDy, double &slope, int &nFactorX, int &nFactorY);
};

#endif // _LINEBREZENHAM_H_
