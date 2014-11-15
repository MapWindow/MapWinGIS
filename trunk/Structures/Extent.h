// Extent.h: interface for the Extent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTENT_H__CCC7B001_1ECF_11D5_A566_00104BCC583E__INCLUDED_)
#define AFX_EXTENT_H__CCC7B001_1ECF_11D5_A566_00104BCC583E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class Extent  
{
public:

	Extent::Extent()
	{	left = 0;
		right = 0;
		bottom = 0;
		top = 0;
	}

	Extent::~Extent() {}

	Extent::Extent(double x, double y, double tol)
	{
		left = x - tol / 2;
		right = x + tol / 2;
		bottom = y - tol / 2;
		top = y + tol / 2;
	}

	Extent::Extent( double Left, double Right, double Bottom, double Top )
	{	
		left = Left;
		right = Right;
		bottom = Bottom;
		top = Top;
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

	Extent& Extent::operator=(const Extent& ext)
	{
		if (&ext == this)
			return *this;

		this->bottom = ext.bottom;
		this->left = ext.left;
		this->right = ext.right;
		this->top = ext.top;

		return *this;
	}

	bool Extent::operator==(const Extent& ext)
	{
		if (this->bottom != ext.bottom) return false;
		if (this->left != ext.left) return false;
		if (this->right != ext.right) return false;
		if (this->top != ext.top) return false;
		return true;
	}

	bool Extent::getIntersection(Extent& ext, Extent& retVal)
	{
		retVal.left = MAX(ext.left, this->left);
		retVal.right = MIN(ext.right, this->right);
		retVal.top = MIN(ext.top, this->top);
		retVal.bottom = MAX(ext.bottom, this->bottom);
		return (retVal.left <= retVal.right) && (retVal.top >= retVal.bottom);
	}

	bool Extent::Intersects(Extent& ext)
	{
		return !(this->right < ext.left || this->left  > ext.right || this->bottom > ext.top || this->top < ext.bottom);
	}

	bool Extent::Within(Extent& ext)
	{
		return this->left >= ext.left && this->right <= ext.right && this->top <= ext.top && this->bottom >= ext.bottom;
	}

	double Extent::getArea()
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

	Point2D GetCenter () {
		return Point2D((left + bottom) / 2.0, (top + right)/2.0);
	}
};

#endif // !defined(AFX_EXTENT_H__CCC7B001_1ECF_11D5_A566_00104BCC583E__INCLUDED_)
