/**************************************************************************************
 * File name: RotatedRectange.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CRotatedRectangle
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 // Sergei Leschinski (lsu) 25 june 2010 - created the file

#include "stdafx.h"
#include "RotatedRectangle.h"
#include "GeometryOperations.h"

// *****************************************************************
//	Constructor
// *****************************************************************
CRotatedRectangle::CRotatedRectangle(long& x1, long& y1, long& x2, long& y2, long& x3, long& y3, long& x4, long& y4)
{
	points[0].x = x1; points[0].y = y1;
	points[1].x = x2; points[1].y = y2;
	points[2].x = x3; points[2].y = y3;
	points[3].x = x4; points[3].y = y4;
	boxCalculated = false;
}

CRotatedRectangle::CRotatedRectangle(CRect& r)
{
	points[0].x = r.left; points[0].y = r.bottom;
	points[1].x = r.left; points[1].y = r.top;
	points[2].x = r.right; points[2].y = r.top;
	points[3].x = r.right; points[3].y = r.bottom;
	boxCalculated = false;
}

// *****************************************************************
//	Intersects
// *****************************************************************
// test for intersection of 2 rectangles
bool CRotatedRectangle::Intersects(CRotatedRectangle& r)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j =0; j<4; j++)
		{
			// if i or j == 3, we must consider segment from 3 to 0
			if (i != 3 && j !=3 )
			{
				if (LineIntersection1(points[i], points[i+1], r.points[j], r.points[j+1])) return true;
			}
			else if (i == 3 && j !=3 )	
			{
				if (LineIntersection1(points[i], points[0], r.points[j], r.points[j+1])) return true;
			}
			else if (i != 3 && j ==3 )	
			{
				if (LineIntersection1(points[i], points[i+1], r.points[j], r.points[0])) return true;
			}
			else if (i == 3 && j ==3 )	
			{
				if (LineIntersection1(points[i], points[0], r.points[j], r.points[0])) return true;
			}
		}
	}
	return false;
}

// *****************************************************************
//	Intersects
// *****************************************************************
bool CRotatedRectangle::Intersects(CRect& r)
{
	for(int i = 0; i < 4; i++)
	{
		int i2 = (i != 3)? i+1:0;
		if (LineIntersection1(points[i], points[i2], CPoint(r.left, r.bottom), CPoint(r.left, r.top))) return true;
		if (LineIntersection1(points[i], points[i2], CPoint(r.left, r.top), CPoint(r.right, r.top))) return true;
		if (LineIntersection1(points[i], points[i2], CPoint(r.right, r.top), CPoint(r.right, r.bottom))) return true;
		if (LineIntersection1(points[i], points[i2], CPoint(r.right, r.bottom), CPoint(r.left, r.bottom))) return true;
		
		// currently if this rect is compeltely included by the r, we'll consider they intersect
		CRect* box1 = this->BoundingBox();
		if (r.left < box1->left && r.right > box1->right && r.top < box1->top && r.bottom > box1->bottom)
		{
			return true;
		}
	}
	return false;
}

// *****************************************************************
//	  Bounds intersect
// *****************************************************************
bool CRotatedRectangle::BoundsIntersect(CRotatedRectangle& r)
{
	CRect* box1, *box2;
	box1 = this->BoundingBox();
	box2 = r.BoundingBox();
	
	if((box1->right < box2->left) ||  (box1->bottom < box2->top)  ||  (box2->right  < box1->left) ||  (box2->bottom < box1->top))
	{
		return false;
	}
	else 
		return true;
}

// *****************************************************************
//	  Bounds intersect
// *****************************************************************
bool CRotatedRectangle::BoundsIntersect(CRect& r)
{
	CRect* box1;
	box1 = this->BoundingBox();
	
	if ((r.right < box1->left) || (r.bottom < box1->top)  || (box1->right < r.left) || (box1->bottom < r.top))
		return false;
	else 
		return true;
}

// *****************************************************************
//	Calculation of bounding box
// *****************************************************************
CRect* CRotatedRectangle::BoundingBox()
{
	if (boxCalculated)
	{
		return &box;
	}
	else
	{
		long minX, maxX, minY, maxY;
		for(int i = 0; i< 4; i++)
		{
			if (i ==0)
			{
				minX = points[0].x;
				maxX = points[0].x;
				minY = points[0].y;
				maxY = points[0].y;
			}
			else
			{
				if (minX > points[i].x)	minX = points[i].x;
				if (maxX < points[i].x)	maxX = points[i].x;
				if (minY > points[i].y)	minY = points[i].y;
				if (maxY < points[i].y)	maxY = points[i].y;
			}
		}
		box.left = minX;
		box.top = minY;
		box.right = maxX;
		box.bottom = maxY;
		boxCalculated = true;
		return &box;
	}
}