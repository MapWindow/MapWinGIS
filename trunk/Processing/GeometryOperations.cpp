#include "stdafx.h"
#include "GeometryOperations.h"

// *******************************************************************
//     LineIntersection1()
// *******************************************************************
bool LineIntersection1(const POINT& a1, const POINT& a2, const POINT& b1, const POINT& b2)
{
	// calcultion of roots for system of 2 linear equations by the Kramer algorythms
	double d, da, db;	// determinants
	d  =(a1.x-a2.x)*(b2.y-b1.y) - (a1.y-a2.y)*(b2.x-b1.x);
	da =(a1.x-b1.x)*(b2.y-b1.y) - (a1.y-b1.y)*(b2.x-b1.x);
	db =(a1.x-a2.x)*(a1.y-b1.y) - (a1.y-a2.y)*(a1.x-b1.x);

	if (fabs(d) < eps)
		return false;		// segments are parallel but may lie on the same line
	else
	{
		double ta = da/d;
		double tb = db/d;
		if ((ta >= 0) && (ta <= 1) && (tb >= 0) && (tb<=1))
		{
			return true;	// there is intersection
		}
		else
			return false;	// there is no intersection of segments
	}
};

// *******************************************************************
//     LineIntersection()
// *******************************************************************
bool LineIntersection(const POINT& a1, const POINT& a2, const POINT& b1, const POINT& b2, POINT& pntCross)
{
	// calcultion of roots for system of 2 linear equations by the Kramer algorythms
	double d, da, db;	// determinants
	d  =(a1.x-a2.x)*(b2.y-b1.y) - (a1.y-a2.y)*(b2.x-b1.x);
	da =(a1.x-b1.x)*(b2.y-b1.y) - (a1.y-b1.y)*(b2.x-b1.x);
	db =(a1.x-a2.x)*(a1.y-b1.y) - (a1.y-a2.y)*(a1.x-b1.x);

	if (fabs(d) < eps)
		return false;		// segments are parallel but may lie on the same line
	else
	{
		double ta = da/d;
		double tb = db/d;
		if ((ta >= 0) && (ta <= 1) && (tb >= 0) && (tb<=1))
		{
			pntCross.x = a1.x + (LONG)ta*(a2.x-a1.x);
            pntCross.y = a1.y + (LONG)tb*(a2.y-a1.y);
			return true;	// there is intersection
		}
		else
			return false;	// there is no intersection of segments
	}
};


bool LineIntersection(const POINT& p1, const POINT& p2, const POINT& p3, const POINT& p4)
{
	int detP1P2P3, detP1P2P4, detP3P4P1, detP3P4P2;
	detP1P2P3 = (p2.x - p1.x)*(p3.y - p1.y) - (p3.x - p1.x)*(p2.y - p1.y);
	detP1P2P4 = (p2.x - p1.x)*(p4.y - p1.y) - (p4.x - p1.x)*(p2.y - p1.y);
	detP3P4P1 = (p3.x - p1.x)*(p4.y - p1.y) - (p4.x - p1.x)*(p3.y - p1.y);
	detP3P4P2 = detP1P2P3-detP1P2P4+detP3P4P1;
	if(detP3P4P1*detP3P4P2 < 0)
	{
		if(ExtentsIntersection(p1,p2,p3,p4))
		{
			return true;
		}
	}
	return false;
}

bool ExtentsIntersection(const POINT& p1, const POINT& p2, const POINT& p3, const POINT& p4)
{
	int x_max1 = 0, x_min1 = 0, y_max1 = 0, y_min1 = 0, x_max2 = 0, x_min2 = 0, y_max2 = 0, y_min2 = 0;
	if(p1.x > p2.x)
	{
		x_max1 = p1.x;
		x_min1 = p2.x;
	}
	else
	{
		x_max1 = p2.x;
		x_min1 = p1.x;
	}

	if(p1.y > p2.y)
	{
		y_max1 = p1.y;
		y_min1 = p2.y;
	}
	else
	{
		y_max1 = p2.y;
		y_min1 = p1.y;
	}

	if(p3.x > p4.x)
	{
		x_max2 = p3.x;
		x_min2 = p4.x;
	}
	else
	{
		x_max2 = p4.x;
		x_min2 = p3.x;
	}

	if(p3.y > p4.y)
	{
		y_max2 = p3.y;
		y_min2 = p4.y;
	}
	else
	{
		y_max2 = p4.y;
		y_min2 = p3.y;
	}

	if(y_max1 < y_min2 || y_max2 < y_min1)
	{
		return false;
	}
	if(y_min1 > y_max2 || y_min2 > y_max1)
	{
		return false;
	}
	if(x_max1 < x_min2 || x_max2 < x_min1)
	{
		return false;
	}
	if(x_min1 > x_max2 || x_min2 > x_max1)
	{
		return false;
	}

	return true;
};

/***********************************************************************/
/*						RelateShapeExtents()				           */
/***********************************************************************/

/*  Temporary function will be shifted to extents class
 *  Extents1 include/is included/insterect extents2
 */
tkExtentsRelation RelateExtents(IShape* shp1, IShape* shp2)
{
	IExtents* box1 = NULL;
	IExtents* box2 = NULL;
	shp1->get_Extents(&box1);
	shp2->get_Extents(&box2);
	
	tkExtentsRelation ret;
	ret = RelateExtents(box1, box2);
	
	box1->Release();
	box2->Release();

	return ret;
}

/*  Temporary function will be shifted to extents class
 *  Extents1 include/is included/insterect extents2
 */
tkExtentsRelation RelateExtents(IExtents* ext1, IExtents* ext2)
{
	double xMin1, yMin1, xMax1, yMax1, zMin1, zMax1;
	double xMin2, yMin2, xMax2, yMax2, zMin2, zMax2;
	ext1->GetBounds(&xMin1, &yMin1, &zMin1, &xMax1, &yMax1, &zMax1);
	ext2->GetBounds(&xMin2, &yMin2, &zMin2, &xMax2, &yMax2, &zMax2);

	if(xMax1 < xMin2 || 
	   yMax1 < yMin2 || 
	   xMax2 < xMin1 || 
	   yMax2 < yMin1)
	
		return erNone;

	else if (xMax1 == xMax2 && 
			 xMin1 == xMin2 && 
			 yMin1 == yMin2 && 
			 yMax1 == yMax2)
		
		return erEqual;

	else if (xMin1 <= xMin2 && 
			 xMax1 >= xMax2 && 
			 yMin1 <= yMin2 && 
			 yMax1 >= yMax2)
		
		return erInclude;			// 1 include 2

	else if (xMin1 >= xMin2 && 
			 xMax1 <= xMax2 && 
			 yMin1 >= yMin2 && 
			 yMax1 <= yMax2)
		
		return erIsIncluded;		// 2 include 1
    
    else
		return erIntersection;
}

tkExtentsRelation RelateExtents(CRect& r1, CRect& r2)
{
	if(r1.right  < r2.left || 
	   r1.bottom < r2.top  || 
	   r2.right  < r1.left || 
	   r2.bottom < r1.top)
	
		return erNone;
	
	else if (r1.EqualRect(r2))
		
		return erEqual;

	else if (r1.left <= r2.left && 
			 r1.right >= r2.right && 
			 r1.top <= r2.top  && 
			 r1.bottom >= r2.bottom)
		
		return erInclude;			// 1 include 2

	else if (r1.left >= r2.left && 
			 r1.right <= r2.right && 
			 r1.top >= r2.top && 
			 r1.bottom <= r2.bottom)
		
		return erIsIncluded;		// 2 include 1
    
    else
		return erIntersection;
}

/****************************************************************************/
/*		get_PointAngle()														*/
/****************************************************************************/
//	Returns directional angle to the point in radians
 double GetPointAngle(double &x, double &y)
{    
	if( y != 0)
	{
		double angle = atan(x / y);
		if (y < 0)			return pi + angle;
		else if (x >= 0)	return angle;
		else /*if (x <= 0)*/return 2.0 * pi + angle;
	}
	else
	{
		if (x > 0)		return pi / 2.0;
		else if(x < 0)	return 1.5 * pi;
		else			return 0.0;
	}
}

 /****************************************************************************/
/*		LinesIntersection()													*/
/****************************************************************************/

/*	Seeks intersection point of two line segments. First - first 4 values;
 *  second - from 5 to 8. x,y - intersectoin position. Returns true if lines
 *  have intersection and false otherwise (lines parallel)
 */
//inline bool LineDrawer::LinesIntersection(long &x11, long &y11, long &x12, long &y12, long &x21, long &y21, long &x22, long &y22, long &x, long &y)
//{
//	double a1, b1, a2, b2;	// lines equations
//	bool vert1, vert2;		// lines vertical
//
//	x = 0.0;
//	y = 0.0;
//
//	if ((x11 - x21) != 0)				// first line equation
//	{	
//		a1 = (y11 - y21) / (x11 - x21);
//		b1 = y11 - a1 * x11;
//	}	
//	else 
//		vert1 = true;
//
//	if ((x21 - x22)!= 0)				// second line equation
//	{	
//		a2 = (y12 - y22) / (x21 - x22);
//		b2 = y12 - a2 * x12;
//	}	
//	else
//		vert2 = true;
//
//	if ((a1 == a2) || (vert1 && vert2))
//	{	
//		return false;
//	}
//	if (vert1 && !vert2)
//	{ 	
//		x = x11;
//		y = (long)a2 * x11 + b2 + 0.5;
//	}
//	else if (!vert1 && vert2)
//	{	
//		x = x12;
//		y = (long)a1 * x12 + b1 + 0.5;
//	}
//	else
//	{   x = (long)(b2 - b1) / (a1 - a2) + 0.5;
//		y = (long) a1 * x + b1 + 0.5;
//	}
//	return true;
//}
