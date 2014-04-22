/**************************************************************************************
 * File name: LabelDrawing.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CLabelDrawer
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
#include "LabelOptions.h"
#include "CollisionList.h"
#include "BaseDrawer.h"

class CLabelDrawer: public CBaseDrawer
{
public:
	// constructor for spatially referenced labels
	CLabelDrawer(Gdiplus::Graphics* graphics, Extent* extents, double pixelPerProjectionX, double pixelPerProjectionY, 
										double currentScale, CCollisionList* collisionList, double mapRotation, bool printing);

	// constructor for screen referenced labels
	CLabelDrawer(Gdiplus::Graphics* graphics, Extent* extents, CCollisionList* collisionList, double mapRotation);
	~CLabelDrawer(void){};

private:	
	HDC m_hdc;
	double _currentScale;
	double _mapRotation;
	CCollisionList* _collisionList;
	bool _spatiallyReferenced;
	bool _printing;
public:
	void DrawLabels( ILabels* LabelsClass);
	
private:
	inline void AlignRectangle(CRect& rect, tkLabelAlignment alignment);
	bool HaveCollision(CRotatedRectangle& rect);
	bool HaveCollision(CRect& rect);
	void DrawLabelFrameGdiPlus(Gdiplus::Graphics& graphics, Gdiplus::Brush* brush, Gdiplus::Pen& pen, Gdiplus::RectF& rect, CLabelOptions* m_options);
	void DrawLabelFrameGdi(CDC* dc,  CRect* rect, CLabelOptions* m_options );
	inline UINT AlignmentToGDI(tkLabelAlignment alignment);
	inline void AlignmentToGDIPlus(tkLabelAlignment alignment, Gdiplus::StringFormat& format);
};
