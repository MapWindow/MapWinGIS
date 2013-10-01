/**************************************************************************************
 * File name: RotatedRectange.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CRotatedRectangle
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

#pragma once

// ***********************************************************
//		Structure CRotatedRect
// ***********************************************************
// a class to manipulate data of label bounds
class CRotatedRectangle
{
public:	
	// constructors
	CRotatedRectangle(long& x1, long& y1, long& x2, long& y2, long& x3, long& y3, long& x4, long& y4);
	CRotatedRectangle(CRect& rect);
	CRotatedRectangle(){boxCalculated = false;};

	// destructor
	virtual ~CRotatedRectangle()
	{
	}
	
	// functions
	bool Intersects(CRotatedRectangle& rect);
	bool Intersects(CRect& rect);
	bool BoundsIntersect(CRotatedRectangle& rect);
	bool BoundsIntersect(CRect& rect);
	CRect* BoundingBox();
	POINT points[4];		// data
private:
	CRect box;
	bool boxCalculated;	// if we calculted rectangle, it will be cached in rect variable
};