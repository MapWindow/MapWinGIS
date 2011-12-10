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
#include "MapWinGis.h"
#include <gdiplus.h>
#include <vector>

#include "Shapefile.h"
#include "LinePattern.h"
#include "ShapefileReader.h"
#include "DrawingOptions.h"
#include "CollisionList.h"


class CShapefileDrawer
{

public:
	CShapefileDrawer(CDC* dc, Extent* extents, double dx, double dy, CCollisionList* collisionList, bool forceGdiplus = false)
	{
		_shapefile = NULL;
		_extents = extents;
		_dx = dx;
		_dy = dy;
		_dc = dc;
		_sfReader = NULL;
		_xMin = 0.0;
		_yMin = 0.0;
		_xMax = 0.0;
		_yMax = 0.0;
		_shptype = SHP_NULLSHAPE;
		_graphics = new Gdiplus::Graphics(dc->m_hDC);
		_avoidCollisions = true;

		_isEditing = VARIANT_FALSE;
		_fastMode = VARIANT_FALSE;

		_shapeData = NULL;

		_collisionList = collisionList;
		_forceGdiplus = forceGdiplus;

		m_selectionColor = RGB(255, 255, 0);
		m_selectionTransparency = 180;
	};

	~CShapefileDrawer()
	{
		for (unsigned long i = 0; i < _pointRectangles.size(); i++)
			delete _pointRectangles[i];
		_pointRectangles.clear();
		delete _graphics;
	};

protected:
	
	OLE_COLOR m_selectionColor;
	unsigned short m_selectionTransparency;
	std::vector<ShapeData*>* _shapeData;		// reference for shapefile's data for faster access
	std::vector<CRect*> _pointRectangles;
	CShapefile* _shapefile;
	Extent* _extents; 
	double _dx;
	double _dy;
	CShapefileReader* _sfReader;
	CDC* _dc;
	Gdiplus::Graphics* _graphics;
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
	void Draw(const CRect & rcBounds, IShapefile* sf, FILE* file);
private:	
	std::vector<long>* SelectShapesFromSpatialIndex(char* sFilename, Extent* extents);
	
	// GDI drawing
	void DrawPolyCategoryGDI( CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	void DrawPolyGDI(PolygonData* shapeData, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded );
	void DrawPolyGDI(IShapeData* shp, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded );
	void DrawPolyGDI(CShapeWrapperCOM* shp, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded );
	inline void DrawPolygonPointGDI(double &xMin, double& xMax, double& yMin, double& yMax, OLE_COLOR& pointColor);

	// GDI+ drawing
	void DrawPolyCategoryGDIPlus(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	bool DrawPolygonGDIPlus(int shapeIndex, Gdiplus::GraphicsPath& path, double minSize, OLE_COLOR pointColor, tkVectorDrawingMode drawingMode,
										 double& xMin, double& xMax, double& yMin, double& yMax);

	void AddPolygonToPath( Gdiplus::GraphicsPath* pathFill, IShapeData* shp, tkVectorDrawingMode drawingMode);
	void AddPolygonToPath( Gdiplus::GraphicsPath* pathFill, PolygonData* shapeData, tkVectorDrawingMode drawingMode);
	void AddPolygonToPath( Gdiplus::GraphicsPath* pathFill, CShapeWrapperCOM* shp, tkVectorDrawingMode drawingMode);
	
	// drawing of point layer
	void DrawPointCategory( CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	void DrawVertices(Gdiplus::GraphicsPath* path, CDrawingOptionsEx* options);

	// line pattern
	void DrawPolylinePatternCategory(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);
	void DrawPolylinePath(Gdiplus::GraphicsPath* path, CDrawingOptionsEx* options, bool drawSelection);

	void CShapefileDrawer::DrawCategory(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection);

	bool WithinVisibleExtents(double xMin, double xMax, double yMin, double yMax)
	{
		return 	!(xMin > _extents->right || xMax < _extents->left || yMin > _extents->top || yMax < _extents->bottom);
	};
	bool HavePointCollision(CRect* rect);
};