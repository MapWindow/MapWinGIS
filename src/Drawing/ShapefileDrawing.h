/**************************************************************************************
 * File name: ShapefileDrawing.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CShapefileDrawer
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
#include "BaseDrawer.h"
#include "Shapefile.h"
#include "LinePattern.h"
#include "ShapefileReader.h"
#include "DrawingOptions.h"
#include "CollisionList.h"
#include "Shape.h"

class CShapefileDrawer
{
public:
	CShapefileDrawer(Gdiplus::Graphics* graphics, Extent* extents, double dx, double dy, CCollisionList* collisionList, double scale, int currentZoom, bool forceGdiplus = false)
	{
		m_hdc = NULL;
		_dc = NULL;			// should be obtained from Graphics and released after the usage
		_shapefile = NULL;
		_extents = extents;
		_dx = dx;
		_dy = dy;
		_sfReader = NULL;
		_xMin = 0.0;
		_yMin = 0.0;
		_xMax = 0.0;
		_yMax = 0.0;
		_shptype = SHP_NULLSHAPE;
		
		_avoidCollisions = true;

		_isEditing = VARIANT_FALSE;
		_fastMode = VARIANT_FALSE;

		_shapeData = NULL;

		_collisionList = collisionList;
		_forceGdiplus = forceGdiplus;

		m_selectionColor = RGB(255, 255, 0);
		m_selectionTransparency = 180;

		_graphics = graphics;
		_graphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

		_bmpPixel = new Gdiplus::Bitmap(1, 1);

		_scale = scale;
        _currentZoom = currentZoom;
		_shapeCount = 0;
	};

	~CShapefileDrawer()
	{
		for (unsigned long i = 0; i < _pointRectangles.size(); i++)
			delete _pointRectangles[i];
		_pointRectangles.clear();
		delete _bmpPixel;
	};

protected:
	double _scale;
    int _currentZoom;
	Gdiplus::Graphics* _graphics;
	Gdiplus::Bitmap* _bmpPixel;
	
	CDC* _dc;	// should be released after each usage
	HDC m_hdc;


	OLE_COLOR m_selectionColor;
	unsigned short m_selectionTransparency;
	std::vector<ShapeRecord*>* _shapeData;		// reference for shapefile's data for faster access
	std::vector<CRect*> _pointRectangles;
	CShapefile* _shapefile;
	Extent* _extents; 
	double _dx;
	double _dy;
	CShapefileReader* _sfReader;
	int _shapeCount;
	
	double _xMin;
	double _yMin;
	double _xMax;
	double _yMax;
	
	ShpfileType _shptype;
	bool _avoidCollisions;
	VARIANT_BOOL _isEditing;
	VARIANT_BOOL _fastMode;
	CCollisionList* _collisionList;
	CCollisionList _localCollisionList;
	bool _forceGdiplus;
	
	struct VertexPath
	{
		Gdiplus::GraphicsPath* path;
		CDrawingOptionsEx* options;
	};

	std::vector<VertexPath> _vertexPathes;
public:
	bool Draw(const CRect & rcBounds, IShapefile* sf);
	int GetShapeCount() { return _shapeCount; }
private:	
	std::vector<long>* SelectShapesFromSpatialIndex(char* sFilename, Extent* extents);
	
	// GDI drawing
	void DrawLineCategoryGDI( CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	void DrawPolyGDI(PolygonData* shapeData, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded );
	void DrawPolyGDI(IShapeData* shp, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded );
	void DrawPolyGDI(IShapeWrapper* shp, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded );
	inline void DrawPolygonPoint(double &xMin, double& xMax, double& yMin, double& yMax, OLE_COLOR& pointColor);

	// GDI+ drawing
	void DrawPolyCategory(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	bool DrawPolygonGDIPlus(int shapeIndex, Gdiplus::GraphicsPath& path, double minSize, OLE_COLOR pointColor, tkVectorDrawingMode drawingMode,
										 double& xMin, double& xMax, double& yMin, double& yMax);

	void AddPolygonToPath( Gdiplus::GraphicsPath* pathFill, IShapeData* shp, tkVectorDrawingMode drawingMode);
	void AddPolygonToPath( Gdiplus::GraphicsPath* pathFill, PolygonData* shapeData, tkVectorDrawingMode drawingMode);
	void AddPolygonToPath( Gdiplus::GraphicsPath* pathFill, IShapeWrapper* shp, tkVectorDrawingMode drawingMode);
	
	// drawing of point layer
	void DrawPointCategory( CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	void DrawVertices(Gdiplus::GraphicsPath* path, CDrawingOptionsEx* options);

	// line pattern
	void DrawLinePatternCategory(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	void DrawPolylinePath(Gdiplus::GraphicsPath* path, CDrawingOptionsEx* options, bool drawSelection);

	void DrawCategory(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);

	void GetVisibilityMask(std::vector<int>& indices, vector<bool>& visibilityMask);

	bool WithinVisibleExtents(int shapeIndex, double xMin, double xMax, double yMin, double yMax)
	{
		bool result = !(xMin > _extents->right || xMax < _extents->left || yMin > _extents->top || yMax < _extents->bottom);

		if (result) 
		{
			(*_shapeData)[shapeIndex]->size = (int)(((xMax - xMin) + (yMax - yMin)) / 2.0*_dx);
			(*_shapeData)[shapeIndex]->wasRendered(true);

			_shapeCount++;
		}

		return result;
	};

	void InitDC()
	{
		if (!_dc)
		{
			m_hdc = _graphics->GetHDC();
			_dc = CDC::FromHandle(m_hdc);
		}
	}
	void ReleaseDC()
	{
		if (_dc)
		{
			_graphics->ReleaseHDC(m_hdc);
			_dc = NULL;
		}
	}

	IShapeData* ReadAndCacheShapeData(int shapeIndex);
};