// Extent.h: interface for the Extent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTENT_H__CCC7B001_1ECF_11D5_A566_00104BCC583E__INCLUDED_)
#define AFX_EXTENT_H__CCC7B001_1ECF_11D5_A566_00104BCC583E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "DebugHelper.h"

class Extent  
{
public:

	Extent()
	{	left = 0;
		right = 0;
		bottom = 0;
		top = 0;
	}

	~Extent() {}

	Extent(double x, double y, double tol)
	{
		left = x - tol / 2;
		right = x + tol / 2;
		bottom = y - tol / 2;
		top = y + tol / 2;
	}

	Extent( double Left, double Right, double Bottom, double Top )
	{	
		left = Left;
		right = Right;
		bottom = Bottom;
		top = Top;
	}

	Extent(Point2D center, double width, double height)
	{
		left = center.x - width / 2.0;
		right = center.x + width / 2.0;
		bottom = center.y - height / 2.0;
		top = center.y + height / 2.0;
	}

	Extent(IExtents* box) 
	{
		if (!box) return;
		double xMin, yMin, zMin, xMax, yMax, zMax;
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		left = xMin;
		right = xMax;
		bottom = yMin;
		top = yMax;
	}

	double left;
	double right;
	double bottom;
	double top;

	Extent& operator=(const Extent& ext)
	{
		if (&ext == this)
			return *this;

		this->bottom = ext.bottom;
		this->left = ext.left;
		this->right = ext.right;
		this->top = ext.top;

		return *this;
	}

	bool operator==(const Extent& ext)
	{
		if (this->bottom != ext.bottom) return false;
		if (this->left != ext.left) return false;
		if (this->right != ext.right) return false;
		if (this->top != ext.top) return false;
		return true;
	}

	bool getIntersection(const Extent& ext, Extent& retVal)
	{
		retVal.left = MAX(ext.left, this->left);
		retVal.right = MIN(ext.right, this->right);
		retVal.top = MIN(ext.top, this->top);
		retVal.bottom = MAX(ext.bottom, this->bottom);
		return (retVal.left <= retVal.right) && (retVal.top >= retVal.bottom);
	}

	bool Intersects(const Extent& ext)
	{
		return !(this->right < ext.left || this->left  > ext.right || this->bottom > ext.top || this->top < ext.bottom);
	}

	bool Within(const Extent& ext)
	{
		return this->left >= ext.left && this->right <= ext.right && this->top <= ext.top && this->bottom >= ext.bottom;
	}

	double getArea()
	{
		return ((this->right - this->left) * (this->top - this->bottom));
	}

	double Width()
	{
		return right - left;
	}

	double Height()
	{
		return top - bottom;
	}

	void MoveTo( double xCent, double yCent ) 
	{
		double dx = (this->right - this->left) / 2.0;
		double dy = (this->top - this->bottom) / 2.0;
		left = xCent - dx;
		right = xCent + dx;
		bottom = yCent - dy;
		top = yCent + dy;
	}

	Point2D GetCenter () 
	{
		return Point2D((left + right) / 2.0, (top + bottom) / 2.0);
	}

	bool OutsideWorldBounds()
	{
		return left < -180.0 || right > 180.0 || top > 90.0 || bottom < -90.0;
	}

	CString ToString()
	{
		return Debug::Format("x: %f; y: %f; w: %f; h: %f", left, top, Width(), Height());
	}
	
};

#endif // !defined(AFX_EXTENT_H__CCC7B001_1ECF_11D5_A566_00104BCC583E__INCLUDED_)
