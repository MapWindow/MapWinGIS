/**************************************************************************************
 * File name: ChartDrawing.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CChartDrawer
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

#pragma once
#include "BaseDrawer.h"
#include "CollisionList.h"
#include "Charts.h"

class CChartDrawer: public CBaseDrawer
{
public:
	// constructor
	CChartDrawer(Gdiplus::Graphics* graphics, Extent* extents, double dx, double dy, double currentScale, CCollisionList* collisionList,
				bool printing)
	{
		_dc = NULL;
		_extents = extents;
		_pixelPerProjectionX = dx;
		_pixelPerProjectionY = dy;
		_collisionList = collisionList;
		_currentScale = currentScale;
		_graphics = graphics;
		
		// TODO: restore!!!
		_printing = true; //printing;	
	}
	
	~CChartDrawer(void)
	{
	}

private:
	bool _printing;

protected:	
	double _currentScale;
	CCollisionList* _collisionList;

private:
	bool WithinVisibleExtents(double xMin, double xMax, double yMin, double yMax)
	{
		return 	
			!(xMin > _extents->right || xMax < _extents->left || yMin > _extents->top || yMax < _extents->bottom);
	};
	
	void DrawLabels(Gdiplus::Font* font, ChartOptions* options, std::vector<ValueRectangle>& labels, bool addToCollisionList, bool vertical);
	Gdiplus::Font* CreateGdiPlusFont(ChartOptions* options);
	bool CheckVisibility(ICharts* charts);
	bool PrepareValues(IShapefile* sf, ICharts* charts, ChartOptions* options, std::vector<long>& arr, std::vector<double*>& values);
	void PrepareBrushes(long numBars, ICharts* charts, ChartOptions* options, std::vector<Gdiplus::Brush*>& brushes, std::vector<Gdiplus::Brush*>& brushesDimmed);
	void DrawPieCharts(IShapefile* sf, ICharts* charts, ChartOptions* options, std::vector<double*>& values, std::vector<long>& arr, std::vector<Gdiplus::Brush*>& brushes, std::vector<Gdiplus::Brush*>& brushesDimmed, long numBars, Gdiplus::Pen& pen, CString sFormat, CBrush& brushFrame, CPen& penFrame, Gdiplus::Font* gdiPlusFont);
	void DrawBarCharts(IShapefile* sf, ICharts* charts, ChartOptions* options, std::vector<double*>& values, std::vector<long>& arr, std::vector<Gdiplus::Brush*>& brushes, std::vector<Gdiplus::Brush*>& brushesDimmed, long numBars, Gdiplus::Pen& pen, CString sFormat, bool vertical, CBrush& brushFrame, CPen& penFrame, Gdiplus::Font* gdiPlusFont);
	bool NormalizeValues(ICharts* charts, IShapefile* sf, ChartOptions* options, vector<double*> values);
public:
	void DrawCharts(IShapefile* sf);
};


