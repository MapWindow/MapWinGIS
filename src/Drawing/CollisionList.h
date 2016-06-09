/**************************************************************************************
 * File name: CollisionList.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CCollisionList
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

#pragma once
#include "RotatedRectangle.h"

class CCollisionList
{
public:
	CCollisionList(void){}
	~CCollisionList(void)
	{
		this->Clear();
	}

	// members
	std::vector<CRect*> _extentsHorizontal;			 // extents for labels w/o rotation
	std::vector<CRotatedRectangle*> _extentsRotated; // extents for labels with rotation

	// functions
public:
	bool CCollisionList::HaveCollision(CRect& rect);
	bool CCollisionList::HaveCollision(CRotatedRectangle& rect);
	void Clear();
	void AddRectangle(CRect* rect, int bufferX = 0, int bufferY = 0);
	void AddRotatedRectangle(CRotatedRectangle* rect, int bufferX = 0, int bufferY = 0);
};

