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
#include "ShapeInfo.h"

class CLabelDrawer: public CBaseDrawer
{
public:
	// spatially referenced labels
	CLabelDrawer::CLabelDrawer(Gdiplus::Graphics* graphics, Extent* extents, double pixelPerProjectionX, double pixelPerProjectionY, double currentScale,
		int currentZoom, CCollisionList* collisionList, double mapRotation, bool printing)
	{
		_printing = false;
		_hdc = NULL;
		_extents = extents;
		_pixelPerProjectionX = pixelPerProjectionX;
		_pixelPerProjectionY = pixelPerProjectionY;
		_currentScale = currentScale;
		_currentZoom = currentZoom;
		_collisionList = collisionList;
		_mapRotation = mapRotation;
		_spatiallyReferenced = true;
		_dc = NULL;
		_graphics = graphics;
		_printing = printing;
	}

	// screen referenced labels
	CLabelDrawer::CLabelDrawer(Gdiplus::Graphics* graphics, Extent* extents, CCollisionList* collisionList, double mapRotation)
	{
		_currentScale = 0;
		_printing = false;
		_hdc = NULL;
		_extents = extents;
		_pixelPerProjectionX = 0.0;
		_pixelPerProjectionY = 0.0;
		_collisionList = collisionList;
		_mapRotation = mapRotation;
		_spatiallyReferenced = false;
		_dc = NULL;
		_graphics = graphics;
	}

	~CLabelDrawer(void){};

private:
	struct LabelSettings {
		VARIANT_BOOL avoidCollisions;
		VARIANT_BOOL removeDuplicates;
		double scaleFactor;
		long numCategories;
		long buffer;
		bool autoOffset;
		bool hasRotation;
		long numLabels;
		bool useVariableFontSize;
	};

private:	
	HDC _hdc;
	double _currentScale;
	int _currentZoom;
	double _mapRotation;
	CCollisionList* _collisionList;
	bool _spatiallyReferenced;
	bool _printing;
	
private:
	void InitSettings(LabelSettings& settings, ILabels* labels, IShapefile* sf);
	bool HaveCollision(CRotatedRectangle& rect);
	bool HaveCollision(CRect& rect);
	CLabelOptions* GetCategoryOptions(ILabels* labels, int categoryIndex);
	double GetScaleFactor(ILabels* labels);
	void GetVisibilityMask(ILabels* labels, IShapefile* sf, std::vector<ShapeData*>* shapeData, std::vector<bool>& visibilityMask);
	bool CheckDynamicVisibility(ILabels* labels);
	bool GetUseGdiPlus(ILabels* labels);
	bool GetAutoOffset(ILabels* labels, IShapefile* sf);
	bool GetExpressionFilter(ILabels* labels, IShapefile* sf, vector<long>& filter);
	void CalcScreenRectangle(CLabelOptions* options, CLabelInfo* lbl, bool autoOffset, int offset, CRect& rect, double& piX, double& piY);
	bool TryAvoidCollisions(CLabelInfo* lbl, VARIANT_BOOL avoidCollisions, CRect& rect, double piX, double piY, long buffer, double angleRad);
	bool CheckVisibility(ILabels* labels);

public:
	void DrawLabels(ILabels* LabelsClass);

};
