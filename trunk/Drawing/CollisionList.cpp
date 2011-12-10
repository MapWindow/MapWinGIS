/**************************************************************************************
 * File name: CollisionList.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CCollisionList
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
 // Sergei Leschinski (lsu) 25 june 2010 - created the file.

#include "stdafx.h"
#include "CollisionList.h"

// ****************************************************************
//		Clear()
// ****************************************************************
void CCollisionList::Clear()
{
	for (unsigned int i = 0; i < _extentsHorizontal.size(); i++)
	{
		delete _extentsHorizontal[i];
	}

	for (unsigned int i = 0; i < _extentsRotated.size(); i++)
	{
		delete _extentsRotated[i];
	}

	_extentsHorizontal.clear();
	_extentsRotated.clear();
}

// ****************************************************************
//		HaveCollision
// ****************************************************************
// Checking overlapping with existing labels; input label isn't rotated
bool CCollisionList::HaveCollision(CRect& rect)
{
	for(int i=0; i < (int)_extentsHorizontal.size(); i++)
	{
		CRect* r = _extentsHorizontal[i];
		if ((rect.right < r->left) || (rect.bottom < r->top)  || (r->right < rect.left) || (r->bottom < rect.top))
			continue;
		else
			return true;
	}
	
	if (_extentsRotated.size() > 0) 
	{
		for(int i=0; i < (int)_extentsRotated.size(); i++)
		{
			CRotatedRectangle* r = _extentsRotated[i];
			if (r->BoundsIntersect(rect))
			{
				if (r->Intersects(rect)) return true;
			}
		}
	}
	return false;
}

// ****************************************************************
//		HaveCollision
// ****************************************************************
// Checking overlapping with existing labels; input label is rotated
bool CCollisionList::HaveCollision(CRotatedRectangle& rect)
{
	for(int i=0; i < (int)_extentsHorizontal.size(); i++)
	{
		CRect* r = _extentsHorizontal[i];
		if (rect.BoundsIntersect(*r))
		{
			if (rect.Intersects(*r)) return true;
		}
	}
	
	for(int i=0; i < (int)_extentsRotated.size(); i++)
	{
		CRotatedRectangle* r = _extentsRotated[i];
		if (rect.BoundsIntersect(*r))
		{
			if (rect.Intersects(*r)) return true;
		}
	}
	return false;
}

// ****************************************************************
//		AddRectangle
// ****************************************************************
void CCollisionList::AddRectangle(CRect* rect, int bufferX, int bufferY)
{
	CRect* rectNew = NULL;
	if (bufferX != 0 || bufferY != 0)
	{
		rectNew = new CRect(rect->left - bufferX/2, rect->top - bufferY/2, rect->right + bufferX/2, rect->bottom + bufferY/2);
	}
	else
	{
		rectNew = new CRect(rect->left, rect->top, rect->right, rect->bottom);
	}
	
	_extentsHorizontal.push_back(rectNew);
}

// ****************************************************************
//		AddRotatedRectangle
// ****************************************************************
void CCollisionList::AddRotatedRectangle(CRotatedRectangle* rect, int bufferX, int bufferY)
{
	if (bufferX != 0 || bufferY != 0)
	{
		// TODO: the calculations here aren't precise
		CRect* rectNew = NULL;
		CRect* r = rect->BoundingBox();
		r->InflateRect(bufferX, bufferY);
		rectNew = new CRect(r->left, r->top, r->right, r->bottom);
		_extentsHorizontal.push_back(rectNew);
	}
	else
	{
		CRotatedRectangle* rectNew = NULL;
		rectNew = new CRotatedRectangle();
		memcpy(rectNew->points, rect->points, sizeof(POINT) * 4);
		_extentsRotated.push_back(rectNew);
	}
}

