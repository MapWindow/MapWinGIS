/**************************************************************************************
 * File name: ShapefileDrawing.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: draws shapefile either from disk or memory, taking into count 
 * styles (aka categories of drawing options), selection, visibility expression for shapes, 
 * collision avoidance for points
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
#include "ShapefileDrawing.h"
#include "LinePattern.h"
#include "ShapefileReader.h"
#include "Shape.h"
#include "GeometryHelper.h"
#include "macros.h"
#include "PointSymbols.h"
#include "ShapefileCategories.h"
#include "TableHelper.h"
#include "ImageHelper.h"

// MEMO: there are several formats to hold shape data while drawing
// there are 2 switches: regular/edit mode; and fast/slow mode
// regular-disk: PolygonData  (pointers to the positions of the memory from fread)
// regular-fast: IShapeData   (CShapeData class)
// edit-COM: IShapeData		  (CShapeWrapperCOM class)
// edit-fast: IShapeData	  (CShapeWrapper class)

using namespace Gdiplus;

enum tkDrawingShape
{
	pshPixel = 0,
	pshEllipse = 1,
	pshPolygon = 2,
	pshPicture = 3,
	pshCharacter = 4,
};

#pragma region MainDrawing
//*******************************************************************
//	Draw()				          
//*******************************************************************
bool CShapefileDrawer::Draw(const CRect & rcBounds, IShapefile* sf)
{
	if (!sf) return false;

	_shapefile = reinterpret_cast<CShapefile*>(sf);

	FILE* file = ((CShapefile*)sf)->get_File();

	#ifdef USE_TIMER
	CTimer tmr;
	tmr.Init("c:\\mw_output.txt");
	tmr.Start();
	tmr.PrintTime("Before bounds");
	#endif
	// -------------------------------------------------------
	//	 check bounds
	// -------------------------------------------------------
	double zMin, zMax;
	IExtents * box=NULL;
	sf->get_Extents(&box);
	box->GetBounds(&_xMin,&_yMin,&zMin,&_xMax,&_yMax,&zMax);
	box->Release();
	box=NULL;

	if ( _xMin>_extents->right || _xMax<_extents->left || _yMin>_extents->top || _yMax<_extents->bottom )
		return false;

	#ifdef USE_TIMER
	tmr.PrintTime("After bounds");
	#endif

	// --------------------------------------------------------
	//	 reading shapefile properties
	// --------------------------------------------------------
	long numShapes;
	
	VARIANT_BOOL _useQTree;
	VARIANT_BOOL _useSpatialIndex;
	VARIANT_BOOL _hasSpatilaIndex;
	tkSelectionAppearance selectionAppearance;
	
	_shapefile->get_SelectionAppearance(&selectionAppearance);
	_shapefile->get_FastMode(&_fastMode);
	_shapefile->get_ShapefileType(&_shptype);
	_shapefile->get_NumShapes(&numShapes);
	_shapefile->get_EditingShapes(&_isEditing);
	_shapefile->get_UseQTree(&_useQTree);
	_shapefile->get_UseSpatialIndex(&_useSpatialIndex);
	_shapefile->get_HasSpatialIndex(&_hasSpatilaIndex);
	_useSpatialIndex = (_useSpatialIndex && _hasSpatilaIndex);
		
	// get 2D type for not checking it afterwards
	_shptype = ShapeUtility::Convert2D(_shptype);

	// clearing the paths	
	_vertexPathes.clear();

	#ifdef USE_TIMER
	tmr.PrintTime("Before reading drawing options");
	#endif
	// --------------------------------------------------------
	//	 acquiring drawing options
	// --------------------------------------------------------
	// default options
	IShapeDrawingOptions* iDefOpt = NULL;
	_shapefile->get_DefaultDrawingOptions(&iDefOpt);
	CDrawingOptionsEx* defaultOptions = ((CShapeDrawingOptions*)iDefOpt)->get_UnderlyingOptions();
	iDefOpt->Release(); iDefOpt= NULL;
	
	// selection options
	CDrawingOptionsEx* selectionOptions = NULL;
	if (selectionAppearance == saDrawingOptions)
	{
		IShapeDrawingOptions* iSelOpt = NULL;
		_shapefile->get_SelectionDrawingOptions(&iSelOpt);
		selectionOptions = ((CShapeDrawingOptions*)iSelOpt)->get_UnderlyingOptions();
		iSelOpt->Release(); iSelOpt = NULL;
	}
	else
	{
		// a default options will be used with drawing transparent selection on top of it
		OLE_COLOR color;
		unsigned char transp;
		selectionOptions = defaultOptions;
		_shapefile->get_SelectionColor(&color);
		_shapefile->get_SelectionTransparency(&transp);
		m_selectionColor = color;
		m_selectionTransparency = transp;
	}
	
	// categories
	IShapefileCategories* icategories = NULL;
	_shapefile->get_Categories(&icategories);
	CShapefileCategories* categories = (CShapefileCategories*)icategories;
	icategories->Release();
	
	#ifdef USE_TIMER
		tmr.PrintTime("Before reading drawing options");
	#endif

	// --------------------------------------------------------
	//	 Settings DC/graphics options
	// --------------------------------------------------------
    int* qtreeResult = nullptr;			// results of quad tree selection
	vector<long>* selectResult = NULL;	// results of spatial index selection
	int offset;							// position (number) of a shape in the shapefile		

	// --------------------------------------------------------
	//		Reading from disk
	// --------------------------------------------------------
	if( !_isEditing)
	{		
		if (file == NULL )
		{
			CallbackHelper::AssertionFailed("Shapefile rendering: file doesn't exist.");
			return false;
		}
		
		CComBSTR fname;
		sf->get_Filename(&fname);

		CCriticalSection* readLock = ((CShapefile*)sf)->get_ReadLock();
	
		// reading index
		USES_CONVERSION;
		_sfReader = new CShapefileReader();

		if (!_sfReader->ReadShapefileIndex(OLE2W(fname), file, readLock))
		{
			delete _sfReader; 
			_sfReader = NULL;

			return false;
		}

		// ---------------------------------------------------------
		//	extracting shapes from spatial index
		// ---------------------------------------------------------
		if (_useSpatialIndex)
		{
			selectResult = SelectShapesFromSpatialIndex(OLE2A(fname), _extents);		// TODO: use Unicode
			if (!selectResult)
			{
				_useSpatialIndex = VARIANT_FALSE;
			}
			else
			{
				numShapes = selectResult->size();
				sort(selectResult->begin(), selectResult->end());
			}
		}
	}
	#ifdef USE_TIMER
		tmr.PrintTime("After reading shape index");	
	#endif

	long numCategories;
	categories->get_Count(&numCategories);
	std::vector<vector<int>> categoryIndices;
	std::vector<vector<int>> categorySelIndices;  // used for selectionAppearance == saSelectionColor only
	categoryIndices.resize(numCategories + 2);	// +1 = default options; +2 = selection options
	categorySelIndices.resize(numCategories + 1); // +1 = default options; 
	
	// --------------------------------------------------------------
	//	 Analyzing visibility expression
	// --------------------------------------------------------------
	std::vector<long> arr;
	CStringW err;
	bool useAll = true;
	
	CComBSTR expr;
	_shapefile->get_VisibilityExpression(&expr);

	if (SysStringLen(expr) > 0)
	{
		CComPtr<ITable> tbl = NULL;
		_shapefile->get_Table(&tbl);

		USES_CONVERSION;
		if (TableHelper::Cast(tbl)->QueryCore(OLE2CW(expr), arr, err))
		{
			useAll = false;
		}
	}

	// --------------------------------------------------------------
	//		Extracting shapes using quad tree							
	// --------------------------------------------------------------
	if(_useQTree & _isEditing)
	{
		int shapesCount;
		IExtents * bBox = NULL;
		ComHelper::CreateExtents(&bBox);
		bBox->SetBounds(_extents->left,_extents->bottom,0,_extents->right, _extents->top,0);
		((CShapefile*)sf)->QuickQueryInEditMode(bBox,&qtreeResult,&shapesCount);
		if (qtreeResult == NULL) 
		{
			bBox->Release(); bBox=NULL;
			goto cleaning; 
		}

		numShapes = (long)shapesCount;
	}
	
	// --------------------------------------------------------------------
	//  nullify the screen size of all shapes
	//  the size is used to choose whether to draw labels and charts or not
	// --------------------------------------------------------------------
	_shapeData = ((CShapefile*)_shapefile)->get_ShapeVector();
	if (_shptype == SHP_POLYGON || _shptype == SHP_POLYLINE)
	{
		int size = _shapeData->size();
		for (int i = 0; i < size; i++)
		{
			(*_shapeData)[i]->size = 0;
			(*_shapeData)[i]->isVisible(false);
		}
	}
    else if (_shptype == SHP_POINT)
    {
        // Since there may be (tens of) thousands of shapes,
        // I don't want to check for rotation field specifications 
        // and field indices on each iteration; so instead, I am
        // checking once for field specification and index, and 
        // then iterating shapes to set rotation-specific values.

		// NOTE: the following is superceded by the rotationExpression
		// is a Rotation field specified ?
        //if (defaultOptions->rotationField.GetLength() > 0)
        //{
        //    long idx;
        //    CComBSTR bstrName(defaultOptions->rotationField);
        //    _shapefile->get_FieldIndexByName(bstrName, &idx);
        //    // iterate shapes, set rotation based on field value
        //    // NOTE that this uses the existing 'rotation' field,
        //    // and thus takes precedence over options-based rotation
        //    for (long i = 0; i < (long)_shapeData->size(); i++)
        //    {
        //        VARIANT rotation;
        //        _shapefile->get_CellValue(idx, i, &rotation);
        //        (*_shapeData)[i]->rotation = rotation.dblVal;
        //    }
        //}
    }

	// --------------------------------------------------------------
	//	 Building lists of shape indices for each category
	// --------------------------------------------------------------
	unsigned int k = 0; // position in arr of visible shapes
	for(int i = 0; i < (int)numShapes; i++ )
	{
		if(_useQTree && _isEditing)
		{
			offset = qtreeResult[i];
		}
		else
		{
			if (!_isEditing && _useSpatialIndex)	
			{
				offset = (*selectResult)[i] - 1;	
			}
			else
			{
				offset = i;
			}
		}

		// searching the index
		if (!useAll)
		{
			bool stop = false;

			//empty list?
			if (arr.size() <= 0) {
				stop = true;
			}
			else
			{
				//determine if the shape is visible
     			while(arr[k] < offset)
     			{
     				k++;
     				if (k >= arr.size())
     				{
     					stop = true;
     					break;
     				}
     			}
			}
			
			// there can't be any visible shapes
			if (stop)
			{
				break;
			}

			// missing this shape, because it complies with the expression
			if (arr[k] > offset)
			{
				continue;
			}
		}

		if (offset >= (int)_shapeData->size())
		{
			// TODO: is is possible to check that the index has the same number of shapes as shapefile?
			if (!_isEditing && _useSpatialIndex) {
				CallbackHelper::ErrorMsg("Invalid spatial index. Index of shape is outside bounds.");
			}
			else {
				CallbackHelper::ErrorMsg("Shapefile drawing: index of shape is outside bounds.");
			}
			break;
		}

		// whether it was hidden explicitly by user
		if ((*_shapeData)[offset]->hidden()) {
			continue;
		}

		// marking shape as visible; it may still fall out of extents but it is inefficient to test it here
		(*_shapeData)[offset]->isVisible(true);
		
		bool selected = (*_shapeData)[offset]->selected();
		if (selected)
		{
			if (selectionAppearance == saDrawingOptions)
			{
				categoryIndices[numCategories + 1].push_back(offset);	// selection options
			}
			else
			{
				long catIndex = (*_shapeData)[offset]->category;
				if (catIndex < 0 || catIndex >= numCategories)
				{
					categorySelIndices[numCategories].push_back(offset);	// default options
				}
				else
				{
					categorySelIndices[catIndex].push_back(offset);		// category
				}
			}
		}
		else
		{
			long catIndex = (*_shapeData)[offset]->category;
			
			if (catIndex < 0 || catIndex >= numCategories)
			{
				categoryIndices[numCategories].push_back(offset);	// default options
			}
			else
			{
				categoryIndices[catIndex].push_back(offset);		// category
				// TODO: check whether the drawing options are different from default
			}
		}
	}

	// -----------------------------------------------------
	//	Drawing
	// -----------------------------------------------------
	CDrawingOptionsEx* options = NULL;
	
	#ifdef USE_TIMER
		tmr.PrintTime("Before reading data");
	#endif
	
	// in some cases selected objects should be drawn first
	bool selectionFirst = false;
	if (_shptype == SHP_POINT || _shptype == SHP_MULTIPOINT)
	{
		tkCollisionMode mode;
		sf->get_CollisionMode(&mode);
		if (mode != AllowCollisions)
		{
			selectionFirst = true;
		}
	}
	
	// drawing selection at the bottom
	if (selectionFirst)
	{
		if (selectionAppearance == saSelectionColor)
		{
			for(int i = categorySelIndices.size() - 1; i >= 0 ; i--)
			{
				if (i == numCategories)				
				{
					options = defaultOptions;
				}
				else if (i < numCategories)
				{
					options = ((CShapefileCategories*)categories)->get_UnderlyingOptions(i);
				}
				
				std::vector<int>* indices = &categorySelIndices[i];
				this->DrawCategory(options, indices, true);
			}
		}
		else		// selection drawing options
		{
			options = selectionOptions;
			std::vector<int>* indices = &categoryIndices[numCategories + 1];	
			this->DrawCategory(options, indices, false);	// false: options set in the selection itself so no need to draw it on top
		}
	}
	
	// drawing unselected shapes
	for(int i = categoryIndices.size() - 2; i >= 0 ; i--)
	{
		if (i == numCategories)				
		{
			options = defaultOptions;
		}
		else if (i < numCategories)
		{
			options = ((CShapefileCategories*)categories)->get_UnderlyingOptions(i);
		}
		else
		{
			CallbackHelper::AssertionFailed("Drawing options for category aren't found.");
		}
		
		std::vector<int>* indices = &categoryIndices[i];
		this->DrawCategory(options, indices, false);
	}

	// drawing selection at the top
	if (!selectionFirst)
	{
		if (selectionAppearance == saSelectionColor)
		{
			for(int i = categorySelIndices.size() - 1; i >= 0 ; i--)
			{
				if (i == numCategories)				
				{
					options = defaultOptions;
				}
				else if (i < numCategories)
				{
					options = ((CShapefileCategories*)categories)->get_UnderlyingOptions(i);
				}
				
				std::vector<int>* indices = &categorySelIndices[i];
				this->DrawCategory(options, indices, true);
			}
		}
		else
		{
			options = selectionOptions;
			std::vector<int>* indices = &categoryIndices[numCategories + 1];	
			this->DrawCategory(options, indices, false);	// false: options set in the selection itself so no need to draw it on top
		}
	}

	// drawing the vertices
	for (unsigned int i = 0; i <_vertexPathes.size(); i++)
	{
		DrawVertices(_vertexPathes[i].path, _vertexPathes[i].options);
	}
	
	// clearing paths
	for (unsigned int i = 0; i <_vertexPathes.size(); i++)
	{
		delete _vertexPathes[i].path;
	}

	#ifdef USE_TIMER
		tmr.PrintTime("After drawing");	
		tmr.Stop();
	#endif
	
// ------------------------------------------
//  final cleaning
// ------------------------------------------
cleaning:
	if (!_isEditing)
	{
		delete _sfReader;
		_sfReader = NULL;
	}
	
	if(_useQTree) 
	{
        // make sure it was allocated
        if (qtreeResult)
        {
	    	delete[] qtreeResult;
    	}
    }
	else
	{
		if (_useSpatialIndex && selectResult)
		{
			selectResult->clear();
			delete selectResult;
			selectResult = NULL;
		}
	}
	return true;
}

// ********************************************************
//		DrawCategory()
// ********************************************************
void CShapefileDrawer::DrawCategory(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection)
{
	if (indices->size() == 0)
		return;
	
	if ((!options->visible) || (!options->fillVisible && !options->linesVisible && !options->verticesVisible))
	{
		return;
	}

	if (!options->IsVisible(this->_scale, this->_currentZoom))
		return;

	if (_shptype == SHP_POINT || _shptype == SHP_MULTIPOINT)
		options->_shpType = tkSimpleShapeType::shpPoint;
	else if (_shptype == SHP_POLYLINE)
		options->_shpType = tkSimpleShapeType::shpPolyline;
	else if (_shptype == SHP_POLYGON)
		options->_shpType = tkSimpleShapeType::shpPolygon;
	else 
		options->_shpType = tkSimpleShapeType::shpNone;
	
	options->scale = this->_scale;

	// ----------------------------------------------------
	// auto selecting the fastest drawing mode 
	// for the current set of options
	// ----------------------------------------------------
	if (options->lineWidth == 1.0f && options->lineTransparency == 255 && options->linesVisible)
	{
		options->drawingMode = vdmGDIMixed;
	}
	else
	{
		options->drawingMode = vdmGDIPlus;
	}

	if (options->pointSymbolType == ptSymbolFontCharacter || options->pointSymbolType == ptSymbolPicture)
	{
		options->drawingMode = vdmGDIPlus;
	}
	
	// circles look more neat in GDI+
	if (options->pointSymbolType == ptSymbolStandard && options->pointShapeType == ptShapeCircle)
	{
		options->drawingMode = vdmGDIPlus;
	}

	if (_forceGdiplus)
	{
		options->drawingMode = vdmGDIPlus;
	}
	
	if (_dc)
	{
		_dc->SetBkColor(options->fillBgColor);
		if ( options->fillBgTransparent )
		{
			_dc->SetBkMode(TRANSPARENT);
		}
		else
		{
			_dc->SetBkMode(OPAQUE);
		}
	}
	
	// ------------------------------------------------
	// perform drawing
	// ------------------------------------------------
	if ( _shptype == SHP_POINT || _shptype == SHP_MULTIPOINT )
	{
		this->DrawPointCategory(options, indices, drawSelection);
	}
	else if ( _shptype == SHP_POLYLINE || _shptype == SHP_POLYGON )
	{
		if (_shptype == SHP_POLYLINE && options->useLinePattern && options->CanUseLinePattern())
		{
			this->DrawLinePatternCategory(options, indices, drawSelection);
		}
		else
		{
			if ( options->drawingMode == vdmGDIMixed && _shptype == SHP_POLYLINE )
			{
				this->DrawLineCategoryGDI( options, indices, drawSelection );	// only lines are drawn here
			}
			else
			{
				this->DrawPolyCategory( options, indices, drawSelection );
			}
		}
	}
}
#pragma endregion

#pragma region DrawPointCategory

// *************************************************************
//		GetVisibilityMask()
// *************************************************************
void CShapefileDrawer::GetVisibilityMask(std::vector<int>& indices, vector<bool>& visibilityMask) 
{
	long numShapes;
	_shapefile->get_NumShapes(&numShapes);

	visibilityMask.clear();
	visibilityMask.resize(numShapes, false);

	for (size_t i = 0; i < indices.size(); i++)
	{
		visibilityMask[indices[i]] = true;
	}
}

// *************************************************************
//		DrawPointsCategory()
// *************************************************************
void CShapefileDrawer::DrawPointCategory( CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection)
{
	IShapeWrapper* shp = NULL;
	tkDrawingShape pntShape;
	
	GraphicsPath* path = NULL;
	GraphicsPath* path2 = NULL;		// for frame around character
	float* data = NULL;
	Bitmap* bmPixel = NULL;
	int numPoints = 0;
	int size = int(options->pointSize/2.0);
	OLE_COLOR pixelColor;

	bool missingIcon = false;
	if (options->pointSymbolType == ptSymbolPicture)
	{
		if (ImageHelper::IsEmpty(options->picture))
		{
			CallbackHelper::ErrorMsg("ShapeDrawingOptions.Picture is empty when icon for point is expected.");
			missingIcon = true;
		}
	}

	// creating a symbol to draw
	if (options->pointSymbolType == ptSymbolStandard || missingIcon)
	{
		// receiving coordinates to define shape of symbol
		if ( options->pointSize <= 1.0 )
		{
			pntShape = pshPixel;
			pixelColor = options->linesVisible? options->lineColor: options->fillColor;
		}
		else if ( options->pointShapeType == ptShapeCircle )
		{
			pntShape = pshEllipse;
		}
		else
		{
			options->drawingMode = vdmGDIPlus;	// GDI drawing will lead to rounding coordinates to integers, and distortions as a result
			pntShape = pshPolygon;
			
			// regular point rotation is set here
			data = get_SimplePointShape(options->pointShapeType, options->pointSize, options->rotation, options->pointNumSides, options->pointShapeRatio, &numPoints);
			if (!data) 
				return;
		}
		
		// GDI+ is used at least partially
		if (options->drawingMode == vdmGDIPlus || options->drawingMode == vdmGDIMixed)
		{
			if ( pntShape == pshEllipse )
			{
				options->InitGdiPlusBrush(&RectF(Gdiplus::REAL(-size), Gdiplus::REAL(-size), Gdiplus::REAL(options->pointSize), Gdiplus::REAL(options->pointSize))) ;
				pntShape = pshEllipse;
				path = new GraphicsPath();
				path->StartFigure();
				path->AddEllipse((Gdiplus::REAL)(-options->pointSize/2.0), (Gdiplus::REAL)(-options->pointSize/2.0), options->pointSize, options->pointSize);
				path->CloseFigure();
			}
			else if ( pntShape == pshPolygon )
			{
				options->InitGdiPlusBrush(&RectF(Gdiplus::REAL(-size), Gdiplus::REAL(-size), Gdiplus::REAL(options->pointSize), Gdiplus::REAL(options->pointSize))) ;
				path = new GraphicsPath();
				path->StartFigure();
				path->AddLines(reinterpret_cast<Gdiplus::PointF*>(data), numPoints);
				path->CloseFigure();
			}
			else if ( pntShape == pshPixel && options->drawingMode == vdmGDIPlus)
			{
				bmPixel = new Bitmap( 1,1, _graphics);
				long alpha = ((long)options->fillTransparency)<<24;
				bmPixel->SetPixel( 0, 0, Color(alpha | BGR_TO_RGB(pixelColor)));
			}
		}

		// drawing with GDI
		if (options->drawingMode == vdmGDI || options->drawingMode == vdmGDIMixed)
		{
			m_hdc = _graphics->GetHDC();
			_dc = CDC::FromHandle(m_hdc);
			if (pntShape != pshPixel)
				options->InitGdiBrushAndPen(_dc);
		}
	}
	else if ( options->pointSymbolType == ptSymbolPicture )
	{
		options->InitGdiPlusPicture();
		if (!options->bitmapPlus)
			return;
		pntShape = pshPicture;
	}
	else if ( options->pointSymbolType == ptSymbolFontCharacter )
	{
		options->InitGdiPlusBrush(&RectF((Gdiplus::REAL)-size, (Gdiplus::REAL)-size, (Gdiplus::REAL)options->pointSize, (Gdiplus::REAL)options->pointSize)) ;
		
		m_hdc = _graphics->GetHDC();
		_dc = CDC::FromHandle(m_hdc);
		path = options->get_FontCharacterPath(_dc, false);
		_graphics->ReleaseHDC(m_hdc);
		_dc = NULL;

		Gdiplus::Matrix mtx;
		mtx.Reset();
		
		pntShape = pshPolygon;
	}

	// frame for a symbol
	if (path && options->drawFrame)
	{
		path2 = options->GetFrameForPath(*path);
	}

	VARIANT_BOOL fastMode;
	_shapefile->get_FastMode(&fastMode);

	double x = 0, y = 0;
	int shapeIndex;

	tkCollisionMode collisionMode;
	_shapefile->get_CollisionMode(&collisionMode);

	// sorting
	vector<long>* sorting;
	((CShapefile*)_shapefile)->GetSorting(&sorting);
	bool hasSorting = sorting != NULL && sorting->size() > 0;

	vector<bool> visibilityMask;
	if (hasSorting)  {
		GetVisibilityMask(*indices, visibilityMask);
	}

	size_t numShapes = hasSorting ? visibilityMask.size() : indices->size();

	for (int j = 0; j < (int)numShapes; j++)
	{
		if (hasSorting){

			shapeIndex = (*sorting)[j];

			if (!visibilityMask[shapeIndex]) {
				continue;
			}
		}
		else {
			shapeIndex = (*indices)[j];
		}
		
		// ------------------------------------------------------
		//	 Reading point data
		// ------------------------------------------------------
		std::vector<PointWithId> points;
		if (! _isEditing)
		{
			int recordLength;
			char* data = _sfReader->ReadShapeData(shapeIndex, recordLength);
			if ( data )
			{
				if (_shptype == SHP_POINT)
				{
					x = *(double*)(data + 4);	// 4 bytes on shape type
					y = *(double*)(data + 12); 
					delete[] data;
					points.push_back(PointWithId(x, y, shapeIndex));
				}
				else
				{
					PolygonData* pdata = _sfReader->ReadMultiPointData(data);
					for(int i = 0; i < pdata->pointCount; i++)
					{
						x = pdata->points[i * 2];
						y = pdata->points[i * 2 + 1];
						points.push_back(PointWithId(x, y, shapeIndex));
					}
					delete pdata;
					delete[] data;
				}
			}
			else
				continue;
		}
		else
		{
			shp = _shapefile->get_ShapeWrapper(shapeIndex);

			if (!shp) continue;
			
			for (int i = 0; i < shp->get_PointCount(); i++)
			{
				shp->get_PointXY(i, x, y);
				points.push_back(PointWithId(x, y, shapeIndex));
			}
		}
		
		for(size_t i = 0; i < points.size(); i++)
		{
			x = points[i].x;
			y = points[i].y;

			if (x > _extents->right || x < _extents->left || y > _extents->top || y < _extents->bottom) continue;

			// ------------------------------------------------------
			//	 Collision avoidance
			// ------------------------------------------------------
			int xInt = static_cast<int>((x - _extents->left) * _dx);
			int yInt = static_cast<int>((_extents->top - y) * _dy);
				
			// preventing point collision
			if (!collisionMode == AllowCollisions)
			{
				CCollisionList* list = collisionMode == LocalList ? &_localCollisionList : _collisionList;
					
				CRect* rect = NULL;
					
				if (options->pointSymbolType == ptSymbolPicture && options->picture != NULL)	
				{
					long width, height;
					options->picture->get_Width(&width);
					options->picture->get_Height(&height);
					int wd = static_cast<int>((double)width * options->scaleX/2.0);
					int ht = static_cast<int>((double)height * options->scaleY/2.0);
						
					if (!options->alignIconByBottom)
					{
						rect = new CRect(xInt - wd, yInt - ht, xInt + wd, yInt + ht);
					}
					else
					{
						rect = new CRect(xInt - wd, yInt - ht * 2, xInt + wd, yInt);
					}
				}
				else
				{
					rect = new CRect(xInt - int(options->pointSize/2.0), 
											yInt - int(options->pointSize/2.0),
											xInt + int(options->pointSize/2.0), 
											yInt + int(options->pointSize/2.0));
				}
					
				if (list->HaveCollision(*rect) && _avoidCollisions)
				{
					
					delete rect; 
					continue;
				}
				else
				{
					(*_shapeData)[shapeIndex]->wasRendered(true);
					list->AddRectangle(rect, 0, 0);
					delete rect;
				}
			}
			else
				(*_shapeData)[shapeIndex]->wasRendered(true);

			_shapeCount++;

			// ------------------------------------------------------
			//	 Drawing
			// ------------------------------------------------------
			if ( pntShape == pshPixel )
			{
				if (drawSelection)
				{
					_dc->SetPixelV(xInt, yInt, m_selectionColor);
				}
				else
				{
					if ( options->drawingMode == vdmGDIPlus )
					{
						_graphics->DrawImage(bmPixel, xInt, yInt);
					}
					else
					{
						_dc->SetPixelV(xInt, yInt, pixelColor);
					}
				}
				(*_shapeData)[shapeIndex]->size = 1;
			}
			else if (pntShape == pshPicture)
			{
				Gdiplus::Matrix mtxInit;
				_graphics->GetTransform(&mtxInit);
					
				long width, height;
				options->picture->get_Width(&width);
				options->picture->get_Height(&height);
				int wd = static_cast<int>((double)width * options->scaleX/2.0);
				int ht = static_cast<int>((double)height * options->scaleY/2.0);
					
				_graphics->TranslateTransform((float)(xInt), (float)(yInt));
					
                // see if individual shape has a specified rotation
                // (set either through shape rotation property, or from rotationExpression)
				float angle = (float)((*_shapeData)[points[i].id])->rotation;
					
				// if not set explicitly, try to grab it from category
				if (angle == 0)
					angle = (float)options->rotation;
				// if any angle is specified, apply it
                if (angle != 0)
					_graphics->RotateTransform(angle);
					
				// if reflecting
				if (options->pointReflectionType != prtNone)
				{
					Gdiplus::Matrix flipMatrix;
					// set up appropriate transformation
					if (options->pointReflectionType == prtLeftToRight)
						flipMatrix.SetElements(-1, 0, 0, 1, 0, 0);
					else if (options->pointReflectionType == prtTopToBottom)
						flipMatrix.SetElements(1, 0, 0, -1, 0, 0);
					// flip the matrix
					_graphics->MultiplyTransform(&flipMatrix);
				}

				if (!options->alignIconByBottom)
				{
					_graphics->TranslateTransform((float)-wd, (float)-ht);
				}
				else
				{
					_graphics->TranslateTransform((float)-wd, (float)-ht * 2);
				}
					
				Gdiplus::Rect rect(0, 0, INT(options->bitmapPlus->GetWidth() * options->scaleX), INT(options->bitmapPlus->GetHeight() * options->scaleY));

                if (!drawSelection || m_selectionTransparency < 255)
				{
					_graphics->DrawImage(options->bitmapPlus, rect, 0, 0, options->bitmapPlus->GetWidth(), options->bitmapPlus->GetHeight(), Gdiplus::UnitPixel, options->imgAttributes);
				}
					
				if (drawSelection)				
				{
					SolidBrush brush(Utility::OleColor2GdiPlus(m_selectionColor, (BYTE)m_selectionTransparency));
					_graphics->FillRectangle(&brush, rect);
				}

				//mtxInit.Reset();
				_graphics->SetTransform(&mtxInit);

				(*_shapeData)[shapeIndex]->size = MAX(wd, ht);
			}
			else
			{
				// GDI+ mode
				if ( options->drawingMode == vdmGDIPlus || options->drawingMode == vdmGDIMixed )
				{
					Gdiplus::Matrix mtxInit;
					_graphics->GetTransform(&mtxInit);
						
					_graphics->TranslateTransform(Gdiplus::REAL(xInt), Gdiplus::REAL(yInt));

					// does point have an individual rotation?
					float angle = (float)((*_shapeData)[points[i].id])->rotation;
					// if nothing specified, revert to options-level angle
					if (angle == 0)
						angle = (float)options->rotation;
					// if angle is non-zero, apply it
					if (angle != 0)
						_graphics->RotateTransform(angle);

					// if reflecting
					if (options->pointReflectionType != prtNone)
					{
						Gdiplus::Matrix flipMatrix;
						// set up appropriate transformation
						if (options->pointReflectionType == prtLeftToRight)
							flipMatrix.SetElements(-1, 0, 0, 1, 0, 0);
						else if (options->pointReflectionType == prtTopToBottom)
							flipMatrix.SetElements(1, 0, 0, -1, 0, 0);
						// flip the matrix
						_graphics->MultiplyTransform(&flipMatrix);
					}

					if (!drawSelection || m_selectionTransparency < 255)
					{
						if (options->pointSymbolType == ptSymbolFontCharacter && path2)
						{
							options->DrawGraphicPathWithFillColor(_graphics, path2, 2.0f);
						}
							
						// drawing fill
						if ( options->fillVisible )
						{
							_graphics->FillPath(options->brushPlus, path);
						}
							
						// we'll draw outline but it'll be slow
						if ( options->drawingMode == vdmGDIPlus && options->linesVisible)	
						{
							options->DrawGraphicPath(_graphics, path);
						}
					}
					_graphics->SetTransform(&mtxInit);
				}
					
				// GDI mode
				if ( options->drawingMode == vdmGDI || options->drawingMode == vdmGDIMixed )
				{
					if (!drawSelection || m_selectionTransparency < 255)
					{
						if ( pntShape == pshPolygon )
						{
							_dc->SetWindowOrg(-xInt , -yInt);
							_dc->Polygon(reinterpret_cast<LPPOINT>(data), numPoints);
							_dc->SetWindowOrg(0 , 0);
						}
						else if ( pntShape = pshEllipse )
						{
							_dc->SetWindowOrg(-xInt , -yInt);
							_dc->Ellipse( -size, -size, size, size);
							_dc->SetWindowOrg(0 , 0);
						}
					}
				}

				// drawing transparent selection
				if (drawSelection)
				{
					Gdiplus::Matrix mtxInit;
					_graphics->GetTransform(&mtxInit);
						
					_graphics->TranslateTransform(Gdiplus::REAL(xInt), Gdiplus::REAL(yInt));
					
					SolidBrush brush(Utility::OleColor2GdiPlus(m_selectionColor, (BYTE)m_selectionTransparency));
					_graphics->FillPath(&brush, path);

					_graphics->SetTransform(&mtxInit);	
				}

				(*_shapeData)[shapeIndex]->size = (int)options->pointSize;
			}
		}
	}
	
	if (_dc)
	{
		_dc->SetWindowOrg(0, 0);
		options->ReleaseGdiBrushAndPen(_dc);
		_graphics->ReleaseHDC(m_hdc);
		_dc = NULL;
	}
	
	// ----------------------------------------------------
	//	  Cleaning
	// ----------------------------------------------------
	if ( bmPixel ) delete bmPixel;
	if ( path )	delete path;
	if ( data )	delete[] data;
	if (path2) delete path2;
	
	options->ReleaseGdiPlusBrush();
	options->ReleaseGdiPlusBitmap();
}
#pragma endregion

#pragma region DrawPolyCategory
// *************************************************************
//		DrawPolygonCategory()
// *************************************************************
// Polygon or polyline category
void CShapefileDrawer::DrawPolyCategory( CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection)
{
	if (_shptype == SHP_POLYLINE && !options->linesVisible && !options->verticesVisible)
		return;
	
	double xMin, xMax, yMin, yMax;

	// ------------------------------------------------------------------------------------------
	// in GDIMixed, GDI will be used for drawing outlines, but only if width is equal to 1.  
	// ------------------------------------------------------------------------------------------
	tkVectorDrawingMode drawingMode = options->drawingMode;
	if ( options->drawingMode == vdmGDIMixed && (options->lineWidth != 1  || !options->linesVisible) ) 	 	
	{
		drawingMode = vdmGDIPlus;
	}
	
	// ------------------------------------------------------------------------------------------------------
	//  GDI+ mode for per-shape gradient is used, as it's time consuming to switch between GDI/GDI+ for each polygon
	//  Obtain dc -> add shapes to the path -> releases dc -> draw GDI+ fill -> Obtain dc -> Draw GDI+ path
	// ------------------------------------------------------------------------------------------------------
	bool perShapeDrawing = options->fillVisible && options->fillType == ftGradient && 
						   options->fillGradientBounds == gbPerShape && _shptype == SHP_POLYGON;
	
	Gdiplus::REAL dpi = _graphics->GetDpiX();
	if (dpi > 100.0f) 
	{
		perShapeDrawing = true;
	}

	if (perShapeDrawing)
	{
		drawingMode = vdmGDIPlus;
	}

	GraphicsPath* path = new Gdiplus::GraphicsPath(Gdiplus::FillModeWinding);
	
	LONG minDrawingSize;
	_shapefile->get_MinDrawingSize(&minDrawingSize);
	double delta = MIN(minDrawingSize/_dx, minDrawingSize/_dy);

	OLE_COLOR pointColor = options->linesVisible?options->lineColor:options->fillColor;
	_bmpPixel->SetPixel(0, 0, Utility::OleColor2GdiPlus(pointColor));
	
	VARIANT_BOOL fastMode;
	_shapefile->get_FastMode(&fastMode);
	
	// -----------------------------------------------------
	//  Per-shape gradient (GDI+ only)
	// -----------------------------------------------------
	if ( perShapeDrawing )
	{
		for (int j = 0; j < (int)indices->size(); j++)
		{
			GraphicsPath pathFill(Gdiplus::FillModeWinding);
		
			if (this->DrawPolygonGDIPlus((*indices)[j], pathFill , delta, pointColor, drawingMode, xMin, xMax, yMin, yMax))
			{
				// drawing fill
				int xmin = int((xMin - _extents->left)* _dx);
				int ymin = int((_extents->top - yMin) * _dy);
				int xmax = int((xMax - _extents->left)* _dx);
				int ymax = int((_extents->top - yMax) * _dy);
			
				RectF rect( (Gdiplus::REAL) xmin, (Gdiplus::REAL)ymin, (Gdiplus::REAL)xmax - xmin, (Gdiplus::REAL)ymax - ymin);
				if (!drawSelection || m_selectionTransparency < 255)
				{
					if ( options->fillVisible && _shptype == SHP_POLYGON )
					{
						options->FillGraphicsPath(_graphics, &pathFill, rect);
					}
				
					// drawing lines
					if ( options->linesVisible && drawingMode == vdmGDIPlus)
					{
						options->DrawGraphicPath(_graphics, &pathFill);
					}
					
					if (options->verticesVisible)
					{
						path->AddPath(&pathFill, FALSE);
					}
				}

				if (drawSelection && m_selectionTransparency > 0)
				{
					SolidBrush brush(Utility::OleColor2GdiPlus(m_selectionColor, (BYTE)m_selectionTransparency));
					_graphics->FillPath(&brush, &pathFill);
				}
			}
		}
	}
	
	// ----------------------------------------------------------
	//	  Draw as a single path for all shapes
	// ----------------------------------------------------------
	else
	{
		// -------------------------------------------------------------------------
		// opening GDI mode; no calls to the _graphics until handle is released !!!
		// -------------------------------------------------------------------------
		if ( drawingMode == vdmGDIMixed )  
		{
			m_hdc = _graphics->GetHDC();
			_dc = CDC::FromHandle(m_hdc);
			_dc->BeginPath();				// strange, but it affects drawing in GDIPlus
			options->InitGdiBrushAndPen(_dc);
		}
		
		// constructing a path
		for (int j = 0; j < (int)indices->size(); j++)
		{
			this->DrawPolygonGDIPlus((*indices)[j], *path, delta, pointColor, drawingMode, xMin, xMax, yMin, yMax);
		}
		
		if ( drawingMode == vdmGDIMixed )  
			_graphics->ReleaseHDC(m_hdc);
		
		if (!drawSelection || m_selectionTransparency < 255)
		{
			// drawing fill (calculating measures of gradient: whole layer)
			if ( options->fillVisible && _shptype == SHP_POLYGON )
			{
				int xmin = int((_xMin - _extents->left)* _dx);
				int ymin = int((_extents->top - _yMin) * _dy);
				int xmax = int((_xMax - _extents->left)* _dx);
				int ymax = int((_extents->top - _yMax) * _dy);
				
				RectF rect( (Gdiplus::REAL)xmin, (Gdiplus::REAL)ymin, (Gdiplus::REAL)xmax - xmin, (Gdiplus::REAL)ymax - ymin);
				options->FillGraphicsPath(_graphics, path, rect);
			}
			
			// drawing lines
			if ( options->linesVisible )
			{
				if ( drawingMode == vdmGDIPlus )
				{
					options->DrawGraphicPath(_graphics, path);
				}
				else if ( options->drawingMode == vdmGDIMixed )
				{
					m_hdc = _graphics->GetHDC();
					_dc->EndPath();
					_dc->StrokePath();
					_graphics->ReleaseHDC(m_hdc);
				}
			}
		}
		
		if (drawingMode == vdmGDIMixed)
		{
			m_hdc = _graphics->GetHDC();
			options->ReleaseGdiBrushAndPen(_dc);
			_graphics->ReleaseHDC(m_hdc);
			_dc = NULL;
		}

		// selection drawing: GDI+
		if (drawSelection && m_selectionTransparency > 0)
		{
			if (_shptype == SHP_POLYGON)
			{
				SolidBrush brush(Utility::OleColor2GdiPlus(m_selectionColor, (BYTE)m_selectionTransparency));
				_graphics->FillPath(&brush, path);
			}
			else
			{
				Pen pen(Utility::OleColor2GdiPlus(m_selectionColor, (BYTE)m_selectionTransparency));
				pen.SetLineJoin(Gdiplus::LineJoinRound);
				_graphics->DrawPath(&pen, path);
			}
		}
	}
	
	options->ReleaseGdiPlusBrush();
	
	if (options->verticesVisible)
	{
		VertexPath vertexPath;
		vertexPath.options = options;
		vertexPath.path = path;
		_vertexPathes.push_back(vertexPath);
	}
	else
	{
		delete path;
	}
}

// *************************************************************
//		ReadAndCacheShapeData()
// *************************************************************
IShapeData* CShapefileDrawer::ReadAndCacheShapeData(int shapeIndex)
{
	IShapeData* shapeData = _shapefile->get_ShapeRenderingData(shapeIndex);
	if (!shapeData)
	{
		int recordLength;

		char* data = _sfReader->ReadShapeData(shapeIndex, recordLength);
		if (data)
		{
			CShapeData* newData = new CShapeData(data, recordLength);
			_shapefile->put_ShapeRenderingData(shapeIndex, newData);
			delete[] data;

			shapeData = newData;
		}
	}

	return shapeData;
}

// *************************************************************
//		DrawPolygonGDIPlus()
// *************************************************************
bool CShapefileDrawer::DrawPolygonGDIPlus(int shapeIndex, Gdiplus::GraphicsPath& path, double minSize, OLE_COLOR pointColor, tkVectorDrawingMode drawingMode,
										 double& xMin, double& xMax, double& yMin, double& yMax)
{
	if (! _isEditing)
	{
		if (m_globalSettings.cacheShapeRenderingData)
		{
			IShapeData* shpData = ReadAndCacheShapeData(shapeIndex);
			if (shpData)
			{
				shpData->get_BoundsXY(xMin, xMax, yMin, yMax);
				if (WithinVisibleExtents(shapeIndex, xMin, xMax, yMin, yMax))
				{
					if ((xMax - xMin >= minSize) || (yMax - yMin >= minSize))	// the poly must be larger than a pixel at a current to be drawn
					{
						this->AddPolygonToPath(&path, shpData, drawingMode);
						return true;
					}
					else
					{
						this->DrawPolygonPoint(xMin, xMax, yMin, yMax, pointColor);
						return false;
					}
				}
			}
		}
		else
		{
			int recordLength;
			char* data = _sfReader->ReadShapeData(shapeIndex, recordLength);
			if (data)
			{
				double* bounds = (double*)(data + 4);
				xMin = bounds[0]; yMin = bounds[1];	xMax = bounds[2]; yMax = bounds[3];

				bool result = false;
				if (WithinVisibleExtents(shapeIndex, xMin, xMax, yMin, yMax))
				{
					if ((xMax - xMin >= minSize) || (yMax - yMin >= minSize))	// the poly must be larger than a pixel at a current to be drawn
					{
						PolygonData* shapeData = _sfReader->ReadPolygonData(data);
						this->AddPolygonToPath(&path, shapeData, drawingMode);
						delete shapeData;
						result = true;
					}
					else
					{
						this->DrawPolygonPoint(xMin, xMax, yMin, yMax, pointColor);
						result = false;
					}
				}
				delete[] data;
				return result;
			}
		}
	}
	else
	{
		IShapeWrapper* shp = ((CShapefile*)_shapefile)->get_ShapeWrapper(shapeIndex);
		if (shp)
		{
			shp->get_BoundsXY(xMin, xMax, yMin, yMax);

			if (this->WithinVisibleExtents(shapeIndex, xMin, xMax, yMin, yMax))
			{
				if ((xMax - xMin >= minSize) || (yMax - yMin >= minSize))	// the poly must be larger than a pixel at a current to be drawn
				{
					IShapeData* data = dynamic_cast<IShapeData*>(shp);
					if (data) {
						AddPolygonToPath(&path, data, drawingMode);
					}
					else {
						AddPolygonToPath(&path, shp, drawingMode);
					}
					return true;
				}
				else
				{
					this->DrawPolygonPoint(xMin, xMax, yMin, yMax, pointColor);
					return false;
				}
			}
		}
	}

	return false;
}
#pragma endregion

#pragma region DrawPolyCategoryGDI
// ****************************************************************
//		DrawPolygonCategoryGDI()
// ****************************************************************
// GDI drawing
void CShapefileDrawer::DrawLineCategoryGDI(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection)
{
	double xMin, xMax, yMin, yMax;
	
	// Entering GDI mode; no calls to _graphics until releasing HDC!!!
	m_hdc = _graphics->GetHDC();
	_dc = CDC::FromHandle(m_hdc);
	_dc->EndPath();
	_dc->SetPolyFillMode(WINDING);
	if (options->_shpType == tkSimpleShapeType::shpPolyline)
	{
		_dc->SetBkMode(TRANSPARENT);
	}
	options->InitGdiBrushAndPen(_dc, drawSelection, m_selectionColor);
	
	Gdiplus::GraphicsPath* path = new Gdiplus::GraphicsPath();
	
	VARIANT_BOOL fastMode;
	_shapefile->get_FastMode(&fastMode);
	
	LONG minDrawingSize;
	_shapefile->get_MinDrawingSize(&minDrawingSize);
	
	double delta = MIN(minDrawingSize/_dx, minDrawingSize/_dy);
	
	OLE_COLOR pointColor = options->linesVisible?options->lineColor:options->fillColor;
	_bmpPixel->SetPixel(0, 0, Utility::OleColor2GdiPlus(pointColor));

	for (int j = 0; j < (int)indices->size(); j++)
	{
		int shapeIndex = (*indices)[j];
		if (! _isEditing)
		{
			if (m_globalSettings.cacheShapeRenderingData)
			{
				IShapeData* shpData = ReadAndCacheShapeData(shapeIndex);
				if (shpData)
				{
					shpData->get_BoundsXY(xMin, xMax, yMin, yMax);
					
					if (WithinVisibleExtents(shapeIndex, xMin, xMax, yMin, yMax))
					{
						if ((xMax - xMin >= delta) || (yMax - yMin >= delta))	// the poly must be larger than a pixel at a current to be drawn
						{
							this->DrawPolyGDI( shpData, options, *path, options->verticesVisible?true:false);
						}
						else
						{
							this->DrawPolygonPoint(xMin, xMax, yMin, yMax, pointColor);
						}
					}
				}
			}
			else
			{
				int recordLength;
				char* data = _sfReader->ReadShapeData(shapeIndex, recordLength);
				if (data)
				{
					double* bounds = (double*)(data + 4);
					xMin = bounds[0]; yMin = bounds[1];	xMax = bounds[2]; yMax = bounds[3];

					if (WithinVisibleExtents(shapeIndex, xMin, xMax, yMin, yMax))
					{
						if ((xMax - xMin >= delta) || (yMax - yMin >= delta))	// the poly must be larger than a pixel at a current to be drawn
						{
							PolygonData* shapeData = _sfReader->ReadPolygonData(data);
							this->DrawPolyGDI( shapeData, options, *path, options->verticesVisible?true:false);
							delete shapeData;
						}
						else
						{
							this->DrawPolygonPoint(xMin, xMax, yMin, yMax, pointColor);
						}
					}
					delete[] data;
				}
			}
		}
		else
		{
			IShapeWrapper* shp = ((CShapefile*)_shapefile)->get_ShapeWrapper(shapeIndex);
			if (shp)
			{
				shp->get_BoundsXY(xMin, xMax, yMin, yMax);

				if (WithinVisibleExtents(shapeIndex, xMin, xMax, yMin, yMax))
				{
					if ((xMax - xMin >= delta) || (yMax - yMin >= delta))	// the poly must be larger than a pixel at a current to be drawn
					{
						IShapeData* data = dynamic_cast<IShapeData*>(shp);
						if (data) {
							DrawPolyGDI(data, options, *path, options->verticesVisible ? true : false);
						}
						else {
							DrawPolyGDI(shp, options, *path, options->verticesVisible?true:false);
						}
					}
					else
					{
						this->DrawPolygonPoint(xMin, xMax, yMin, yMax, pointColor);
					}
				}
			}
		}
	}

	options->ReleaseGdiBrushAndPen(_dc);
	_graphics->ReleaseHDC(m_hdc);
	_dc = NULL;

	// drawing of vertices
	if (options->verticesVisible)
	{
		VertexPath vertexPath;
		vertexPath.options = options;
		vertexPath.path = path;
		_vertexPathes.push_back(vertexPath);
	}
	else
	{
		delete path;
	}
}
#pragma endregion

#pragma region DrawVertices
// ******************************************************************
//		DrawVertices()
// ******************************************************************
void CShapefileDrawer::DrawVertices(Gdiplus::GraphicsPath* path, CDrawingOptionsEx* options)
{
	if (options->verticesVisible)
	{
		//_dc->EndPath();	   // in GDIPlus mode no drawing occurs otherwise
		HDC hdc = _graphics->GetHDC();
		_dc = CDC::FromHandle(hdc);

		int count = path->GetPointCount();
		if (count > 0)
		{
			Gdiplus::PointF* points = new Gdiplus::PointF[count];
			path->GetPathPoints(points, count);
			
			// drawing of vertices
			if (options->verticesVisible)
			{
				options->InitGdiVerticesPen(_dc);

				CPoint* square = options->GetVertex();
				int size = options->verticesSize/2;

				for (int i = 0; i < count; i++)
				{
					int x = (int)-points[i].X;
					int y = (int)-points[i].Y;

					_dc->SetWindowOrg(x, y);

					if (options->verticesType == vtSquare)
					{
						_dc->Polygon(square, 4);
					}
					else
					{
						_dc->Ellipse(-size, -size, size, size);
					}
				}
				_dc->SetWindowOrg(0 , 0);
				delete[] square;
				options->ReleaseGdiBrushAndPen(_dc);
			}
			delete[] points;
		}
		_graphics->ReleaseHDC(hdc);
		_dc = NULL;
	}
}
#pragma endregion

#pragma region MarkerLine

// ******************************************************************
//		DrawLinePatternCategory()
// ******************************************************************
// GDI+ only; no HDC is required
void CShapefileDrawer::DrawLinePatternCategory(CDrawingOptionsEx* options, std::vector<int>* indices, bool drawSelection)
{
	if (_shptype != SHP_POLYLINE )
		return;
	
	double xMin, xMax, yMin, yMax;

	GraphicsPath* path = new Gdiplus::GraphicsPath(Gdiplus::FillModeWinding);
	
	LONG minDrawingSize;
	_shapefile->get_MinDrawingSize(&minDrawingSize);
	double delta = MIN(minDrawingSize/_dx, minDrawingSize/_dy);

	OLE_COLOR pointColor = options->linesVisible?options->lineColor:options->fillColor;
	_bmpPixel->SetPixel(0, 0, Utility::OleColor2GdiPlus(pointColor));

	// constructing a path
	for (int j = 0; j < (int)indices->size(); j++)
	{
		this->DrawPolygonGDIPlus((*indices)[j], *path, delta, pointColor, vdmGDIPlus, xMin, xMax, yMin, yMax);
	}
	
	// the drawing
	this->DrawPolylinePath(path, options, drawSelection);

	if (options->linePattern && drawSelection)
	{
		int count;
		options->linePattern->get_Count(&count);
		float maxWidth = 0.0f;

		for (int i = 0; i < count; i++)
		{
			CComPtr<ILineSegment> line = NULL;
			options->linePattern->get_Line(i, &line);
			tkLineType style;
			line->get_LineType(&style);
			if (style == lltSimple)
			{
				float width;
				line->get_LineWidth(&width);
				if (width > maxWidth)
				{
					maxWidth = width;
				}
			}
		}
		
		if (maxWidth > 0.0f)
		{
			Gdiplus::Pen penSelection(Utility::OleColor2GdiPlus(m_selectionColor, (BYTE)m_selectionTransparency), maxWidth);
			penSelection.SetLineJoin(Gdiplus::LineJoinRound);
			_graphics->DrawPath(&penSelection, path);
		}
	}

	if (options->verticesVisible)
	{
		VertexPath vertexPath;
		vertexPath.options = options;
		vertexPath.path = path;
		_vertexPathes.push_back(vertexPath);
	}
	else
	{
		delete path;
	}
}

// ******************************************************************
//		DrawPolyline()
// ******************************************************************
void CShapefileDrawer::DrawPolylinePath(Gdiplus::GraphicsPath* path, CDrawingOptionsEx* options, bool drawSelection)
{
	if (!options->linePattern)
		return;

	ILinePattern* pattern = options->linePattern;

	int numLines;
    pattern->get_Count(&numLines);
	if (numLines == 0)
		return;
	
	// path related variables
	bool dataRead = false;
	int pointCount = 0;				// number of points in path
	
	double ratio;
	
	Gdiplus::PathData* data = new PathData();
	
	BYTE transparency;
    pattern->get_Transparency(&transparency);

	Gdiplus::Pen* penSelection = NULL;
	Gdiplus::SolidBrush* brushSelection = NULL;

	if (drawSelection)
	{
		brushSelection = new Gdiplus::SolidBrush(Utility::OleColor2GdiPlus(m_selectionColor, (BYTE)m_selectionTransparency));
	}

	Gdiplus::SmoothingMode mode = _graphics->GetSmoothingMode();
	_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	for ( int i = 0; i < numLines; i++)
	{
		CComPtr<ILineSegment> line = NULL;
		pattern->get_Line(i, &line);
		tkLineType type;
		OLE_COLOR color;
		line->get_LineType(&type);
		line->get_Color(&color);

		if (type == lltSimple)
		{
			float width;
			tkDashStyle style;
			line->get_LineWidth(&width);
			line->get_LineStyle(&style);
			
			Gdiplus::Pen* pen = new Gdiplus::Pen(Utility::OleColor2GdiPlus(color, transparency), width);
			pen->SetLineJoin(Gdiplus::LineJoinRound);
			switch (style)
			{
				case dsSolid:		pen->SetDashStyle(Gdiplus::DashStyleSolid);		break;
				case dsDash:		pen->SetDashStyle(Gdiplus::DashStyleDash);		break;
				case dsDot:			pen->SetDashStyle(Gdiplus::DashStyleDot);		break;
				case dsDashDotDot:	pen->SetDashStyle(Gdiplus::DashStyleDashDotDot);break;
				case dsDashDot:		pen->SetDashStyle(Gdiplus::DashStyleDashDot);	break;
				default:			pen->SetDashStyle(Gdiplus::DashStyleSolid);
			}
			
			_graphics->DrawPath(pen, path);
			delete pen;
		}
		else if (type == lltMarker)
		{

			if (!dataRead)
			{
				pointCount = path->GetPointCount();
				if (pointCount > 0)
				{
					path->GetPathData(data);
					pointCount = data->Count;
					//(path.GetPathData(&data) == Gdiplus::Ok)
				}
				dataRead = true;
			}

			if (pointCount > 0)
			{
				// extracting properties
				tkDefaultPointSymbol symbol;
				tkLineLabelOrientation orientation;
				float markerSize, intervalBase, markerOffsetBase;
                float interval, markerOffset;
				OLE_COLOR lineColor;
                VARIANT_BOOL flipFirst;
                VARIANT_BOOL intervalIsRelative;
                VARIANT_BOOL offsetIsRelative;
                VARIANT_BOOL overflow;
				
				line->get_Marker(&symbol);
				line->get_MarkerSize(&markerSize);
				line->get_MarkerInterval(&intervalBase);
                line->get_MarkerIntervalIsRelative(&intervalIsRelative);
				line->get_MarkerOffset(&markerOffsetBase);
                line->get_MarkerOffsetIsRelative(&offsetIsRelative);
				line->get_MarkerOutlineColor(&lineColor);
				line->get_MarkerOrientation(&orientation);
                line->get_MarkerFlipFirst(&flipFirst);
                line->get_MarkerAllowOverflow(&overflow);
				
				//	preparing marker
				int numPoints = 0;
				float* points = get_SimplePointShape(symbol, markerSize, &numPoints);
				
				if (numPoints > 0)
				{
					Gdiplus::SolidBrush* brush = new Gdiplus::SolidBrush(Utility::OleColor2GdiPlus(color, transparency));
					Gdiplus::Pen* pen = new Gdiplus::Pen(Utility::OleColor2GdiPlus(lineColor, transparency));
					pen->SetAlignment(Gdiplus::PenAlignmentInset);
				
                    std::vector<double> lengths(pointCount - 1);
                    std::vector<double> totalLengths(pointCount - 1);
                    double totalLength = 0.0;
                    double positionScale = 1.0; // default is = pixel scale

                    // Calculate lengths and total lengths
                    int currentStartN = 0;
                    for (int n = 0; n < pointCount; n++)
                    {
                        if (data->Types[n] == PathPointTypeStart) {
                            totalLength = 0;
                            currentStartN = n;
                            continue;
                        }

                        double dx = data->Points[n].X - data->Points[n - 1].X;
                        double dy = data->Points[n].Y - data->Points[n - 1].Y;

                        double length = sqrt(pow(dx, 2.0) + pow(dy, 2.0));

                        lengths[n - 1] = length;
                        totalLength += length;

                        // last iteration:
                        if (n+1 == pointCount || data->Types[n+1] == PathPointTypeStart)
                            for (int m = currentStartN; m < n; m++)
                                totalLengths[m] = totalLength;
                    }

                    // Equals the marker offset for the start point,
                    // and the undershot part of interval for all others
                    double offset = 0;
                    bool firstMarkerDrawn = false;
                    for (int n = 0; n < pointCount; n++)
					{
                        // Start of a new line:
						if (data->Types[n] == PathPointTypeStart)
						{
							double length = 0;
							double correctedLength = length;

                            totalLength = totalLengths[n] - (overflow ? 0 : markerSize);
                            // Apply scale factor for offset & interval values if requested:
                            markerOffset = markerOffsetBase * ((offsetIsRelative) ? (float)totalLength : 1.0f);
                            interval = intervalBase * ((intervalIsRelative) ? (float)totalLength : 1.0f);
                            firstMarkerDrawn = false;
                            // Set starting offset:
                            offset = markerOffset + (overflow ? 0 : markerSize * 0.5);
                            correctedLength -= (overflow ? 0 : markerSize * 0.5);
                            continue;
						}

						double length = lengths[n - 1];
						double correctedLength = length;

                        // Last segment:
                        if (n + 1 == pointCount || data->Types[n + 1] == PathPointTypeStart) 
                        {
                            correctedLength -= (overflow ? 0 : markerSize * 0.5);
                        }

						CPoint pnt;
						pnt.x = (LONG)data->Points[n - 1].X;
						pnt.y = (LONG)data->Points[n - 1].Y;

						double dx = data->Points[n].X - data->Points[n - 1].X;
						double dy = data->Points[n].Y - data->Points[n - 1].Y;

                        // If this segment is shorter than the offset,
                        // just subtract from offset & go the next segment
                        const int count = (interval == 0) ? 1 : (int)((correctedLength - offset) / interval + 1.0 + FLT_EPSILON);
						if (count <= 0 || offset > correctedLength + FLT_EPSILON)
						{
							offset -= correctedLength;
                            continue;
						}

						ratio = offset/length;
						Gdiplus::PointF pntStart(Gdiplus::REAL(pnt.x + dx * ratio), Gdiplus::REAL(pnt.y + dy * ratio));
								
                        double angle = 0.0f;
						if (orientation == lorParallel )
						{
							angle = GeometryHelper::GetPointAngle(dx, dy) / pi_ * 180.0 - 90.0;
						}
						else if (orientation == lorPerpindicular)
						{
							angle = GeometryHelper::GetPointAngle(dx, dy) / pi_ * 180.0;
						}
								
						SIZE size;
						size.cx = (LONG)markerSize;
						size.cy = (LONG)markerSize;

						for (int j = 0; j < count; j++)
						{
							double ratio = interval/length * (double)j;
							Gdiplus::REAL xPos =(Gdiplus::REAL)(pntStart.X + dx * ratio);
							Gdiplus::REAL yPos =(Gdiplus::REAL)(pntStart.Y + dy * ratio);
										
							_graphics->TranslateTransform(xPos , yPos);
							_graphics->RotateTransform((float)(-angle + ((flipFirst && !firstMarkerDrawn) ? 180 : 0)));

							_graphics->FillPolygon(brush, (Gdiplus::PointF*)points, numPoints);
							_graphics->DrawPolygon(pen, (Gdiplus::PointF*)points, numPoints);

							if (drawSelection)
								_graphics->FillPolygon(brushSelection, (Gdiplus::PointF*)points, numPoints);
								
							_graphics->ResetTransform();
                            firstMarkerDrawn = true;
						}

                        // if interval is zero, no point in going to the next segment of this polyline
                        while (interval == 0 && !(n+1==pointCount || data->Types[n + 1] == PathPointTypeStart))
                            n++;

                        // the part of interval left to draw:
                        offset += count * interval - length;
					}
					delete[] points;
					delete brush;
					delete pen;
				}
			}
		}
	}

	_graphics->SetSmoothingMode(mode);

	if (penSelection)	delete penSelection;
	if (brushSelection) delete brushSelection;

	delete data;

}
#pragma endregion

#pragma region PolygonToPathGDI+
// ******************************************************************
//		AddPolygonToPath()
// ******************************************************************
// disk version in GDI+
void CShapefileDrawer::AddPolygonToPath(Gdiplus::GraphicsPath* pathFill, PolygonData* shapeData, tkVectorDrawingMode drawingMode)
{
	double* srcPoints = shapeData->points;
	for (int i = 0; i < shapeData->partCount; i++)
	{
		int end;
		if (i == shapeData->partCount - 1)	
		{
			end = shapeData->pointCount - 1;
		}
		else
		{
			end = shapeData->parts[i+1] - 1;
		}
		
		int start = *(shapeData->parts + i);
		int count = end - start + 1;
		
		if ( count > 1)		// if (count > 2)
		{
			int* points = new int[count * 2];
			int k = 0;

			for (int j = start; j <= end; j++)
			{
				int k2 = k * 2;
				points[k2] = (int)((srcPoints[j * 2] - _extents->left) * _dx);
				points[k2 + 1] = (int)((_extents->top - srcPoints[j * 2 + 1]) * _dy);
				
				// we won't write duplicate points
				if (k != 0) 
				{
					if ((points[k2] != points[k2 - 2]) || 
						(points[k2 + 1] != points[k2 - 1]))
						k++;
				}
				else
					k++;
			}
			
			if (k > 1)
			{
				if ( drawingMode == vdmGDIMixed )
				{
					if ( _shptype == SHP_POLYGON )
					{
						_dc->Polygon(reinterpret_cast<POINT*>(points), k);
					}
					else if ( _shptype == SHP_POLYLINE ) 
					{
						_dc->Polyline(reinterpret_cast<POINT*>(points), k);
					}
				}
				
				pathFill->StartFigure();
				pathFill->AddLines(reinterpret_cast<Gdiplus::Point*>(points), k);
			}
			delete[] points; points = NULL;
		}
	}
}

// ******************************************************************
//		AddPolygonToPath()
// ******************************************************************
void CShapefileDrawer::AddPolygonToPath( Gdiplus::GraphicsPath* pathFill, IShapeWrapper* shp, tkVectorDrawingMode drawingMode)
{
	int partCount = shp->get_PartCount();

	for(int i = 0; i < partCount; i++)
	{
		long start, end;
		start = shp->get_PartStartPoint(i);
		end = shp->get_PartEndPoint(i);
		int count = end - start + 1;
		
		if (count > 1)			// if (count > 2)
		{
			int* points = new int[count * 2];
			
			int k = 0;
			double x,y;
			for(long j = start; j <= end; j++)
			{
				shp->get_PointXY(j, x, y);
				
				int k2 = k * 2;
				points[k2] = (int)((x - _extents->left) * _dx);
				points[k2 + 1] = (int)((_extents->top - y) * _dy);
				
				// we won't write duplicate points
				if (k != 0) 
				{
					if ((points[k2] != points[k2 - 2]) || (points[k2 + 1] != points[k2 - 1]))
						k++;
				}
				else
					k++;
			}

			if (k > 1)
			{
				if ( drawingMode == vdmGDIMixed )
				{
					if ( _shptype == SHP_POLYGON )		 
					{
						_dc->Polygon(reinterpret_cast<POINT*>(points), k);
					}
					else if ( _shptype == SHP_POLYLINE ) 
					{
						_dc->Polyline(reinterpret_cast<POINT*>(points), k);
					}
				}
				
				pathFill->StartFigure();
				pathFill->AddLines(reinterpret_cast<Gdiplus::Point*>(points), k);
			}
			delete[] points; points = NULL;
		}
	}
}

// ******************************************************************
//		AddPolygonToPath()
// ******************************************************************
// Fast in-memory version for GDI+
void CShapefileDrawer::AddPolygonToPath(GraphicsPath* pathFill, IShapeData* shp, tkVectorDrawingMode drawingMode)
{
	int partCount =  shp->get_PartCount();
	for(int i = 0; i < partCount; i++)
	{
		int start = shp->get_PartStartPoint(i);
		int end = shp->get_PartEndPoint(i);
		int count = end - start + 1;
		
		if (count > 1)		// if (count > 2)
		{
			int* points = new int[count * 2];
			double* srcPoints = shp->get_PointsXY();
			
			int k = 0;
			for(int j = start; j <= end; j++)
			{
				int k2 = k * 2;
				points[k2] = (int)((srcPoints[j * 2] - _extents->left) * _dx);
				points[k2 + 1] = (int)((_extents->top - srcPoints[j * 2 + 1]) * _dy);
				
				// we won't write duplicate points
				if (k != 0) 
				{
					if ((points[k2] != points[k2 - 2]) || (points[k2 + 1] != points[k2 - 1]))
						k++;
				}
				else
					k++;
			}

			if (k > 1)
			{
				if ( drawingMode == vdmGDIMixed )
				{
					if ( _shptype == SHP_POLYGON )		 
					{
						_dc->Polygon(reinterpret_cast<POINT*>(points), k);
					}
					else if ( _shptype == SHP_POLYLINE ) 
					{
						_dc->Polyline(reinterpret_cast<POINT*>(points), k);
					}
				}
				
				pathFill->StartFigure();
				pathFill->AddLines(reinterpret_cast<Gdiplus::Point*>(points), k);
			}
			delete[] points; points = NULL;
		}
	}
}
#pragma endregion

#pragma region DrawPolyGDI
// ***************************************************************
//	  DrawPolyGDI()
// ***************************************************************
// Disk version
void CShapefileDrawer::DrawPolyGDI( PolygonData* shapeData, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded )
{
	int* points = new int[shapeData->pointCount * 2];
	int* parts = new int[shapeData->partCount];
	double* srcPoints = shapeData->points;

	int count = 0;
	for (int part = 0; part < shapeData->partCount; part++)
	{
		int end, start;
		if (part == shapeData->partCount - 1)	
		{
			end = shapeData->pointCount;
		}
		else
		{
			end = shapeData->parts[part+1];
		}
		
		start = *(shapeData->parts + part);
		
		int partCount = 0;
		for (int j = start; j < end; j++)
		{
			int k2 = count * 2;
			points[k2] = (int)((srcPoints[j * 2] - _extents->left) * _dx);
			points[k2 + 1] = (int)((_extents->top - srcPoints[j * 2 + 1]) * _dy);
			
			count++;
			partCount++;
		}
			
		parts[part] = partCount;
	}		
	
	if ( _shptype == SHP_POLYLINE )
	{
		_dc->PolyPolyline(reinterpret_cast<POINT*>(points), (DWORD*)parts, shapeData->partCount);
	}
	else if ( _shptype == SHP_POLYGON )
	{
		_dc->PolyPolygon(reinterpret_cast<POINT*>(points), parts, shapeData->partCount);
	}
	
	if (pathIsNeeded)
	{
		path.AddLines(reinterpret_cast<Gdiplus::Point*>(points), count);
	}

	delete[] points;
	delete[] parts; 
}

// ******************************************************************
//		DrawPolyGDI()
// ******************************************************************
// Regular in-memory version
void CShapefileDrawer::DrawPolyGDI( IShapeWrapper* shp, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded )
{
	int numPoints = shp->get_PointCount();
	int numParts = shp->get_PartCount();

	if (numPoints == 0 || numParts == 0)
		return;
	
	int* points = new int[numPoints * 2];
	int* parts = new int[numParts];

	int count = 0;
	for(int part = 0; part < numParts; part++)
	{
		long start, end;
		start = shp->get_PartStartPoint(part);
		end = shp->get_PartEndPoint(part);
			
		int partCount = 0;
		double x, y;
		for(int j = start; j <= end; j++)
		{
			shp->get_PointXY(j, x, y);

			int k2 = count * 2;
			points[k2] = (int)((x - _extents->left) * _dx);
			points[k2 + 1] = (int)((_extents->top - y) * _dy);

			count++;
			partCount++;
		}
		parts[part] = partCount;
	}

	if ( _shptype == SHP_POLYLINE )
	{
		_dc->PolyPolyline(reinterpret_cast<POINT*>(points), (DWORD*)parts, numParts);
	}
	else if ( _shptype == SHP_POLYGON )
	{
		_dc->PolyPolygon(reinterpret_cast<POINT*>(points), parts, numParts);
	}
	
	if (pathIsNeeded)
	{
		path.AddLines(reinterpret_cast<Gdiplus::Point*>(points), count);
	}

	delete[] points;
	delete[] parts; 
}

// ******************************************************************
//		DrawPolyGDI()
// ******************************************************************
// Fast in-memory version
void CShapefileDrawer::DrawPolyGDI( IShapeData* shp, CDrawingOptionsEx* options, Gdiplus::GraphicsPath& path, bool pathIsNeeded )
{
	if (shp->get_PointCount() == 0 || shp->get_PartCount() == 0)
		return;
	
	int numParts = shp->get_PartCount();
	int* points = new int[shp->get_PointCount() * 2];
	int* parts = new int[numParts];
	double* srcPoints = shp->get_PointsXY();

	int count = 0;
	for(int part = 0; part < numParts; part++)
	{
		int start = shp->get_PartStartPoint(part);
		int end = shp->get_PartEndPoint(part);
			
		int partCount = 0;
		for(int j = start; j <= end; j++)
		{
			int k2 = count * 2;
			points[k2] = (int)((srcPoints[j * 2] - _extents->left) * _dx);
			points[k2 + 1] = (int)((_extents->top - srcPoints[j * 2 + 1]) * _dy);

			count++;
			partCount++;
		}
		parts[part] = partCount;
	}

	if ( _shptype == SHP_POLYLINE )
	{
		_dc->PolyPolyline(reinterpret_cast<POINT*>(points), (DWORD*)parts, numParts);
	}
	else if ( _shptype == SHP_POLYGON )
	{
		_dc->PolyPolygon(reinterpret_cast<POINT*>(points), parts, numParts);
	}
	
	if (pathIsNeeded)
	{
		path.AddLines(reinterpret_cast<Gdiplus::Point*>(points), count);
	}

	delete[] points;
	delete[] parts; 
}

// ******************************************************************
//		DrawPolygonPoint()
// ******************************************************************
// Draws point in place of small polygons
inline void CShapefileDrawer::DrawPolygonPoint(double &xMin, double& xMax, double& yMin, double& yMax, OLE_COLOR& pointColor)
{
	int x = (int)(((xMax + xMin)/2 - _extents->left) * _dx);
	int y = (int)((_extents->top - (yMax + yMin)/2) * _dy);

	if (!_dc)
	{
		_graphics->DrawImage(_bmpPixel, x, y);
	}
	else
	{
		_dc->SetPixelV( x, y, pointColor);
	}
}
#pragma endregion

#pragma region Utilities
//********************************************************************
//*		SelectShapesFromSpatialIndex()
//********************************************************************
std::vector<long>* CShapefileDrawer::SelectShapesFromSpatialIndex(char* sFilename, Extent* extents)
{
	string baseName;
	baseName = sFilename;
	baseName = baseName.substr(0,baseName.find_last_of("."));
	
	double lowVals[2], highVals[2];
	lowVals[0] = extents->left;
	lowVals[1] = extents->bottom;
	highVals[0] = extents->right;
	highVals[1] = extents->top;
	
	IndexSearching::CIndexSearching *res = new IndexSearching::CIndexSearching();	

	if (IndexSearching::selectShapesFromIndex((char *)baseName.c_str(), lowVals, highVals, IndexSearching::intersection, 100, res) == 0)	
	{
		std::vector<long>* selectResult = new std::vector<long>;	
		selectResult->reserve(res->getLength());
		for (int i = 0 ;i < res->getLength(); i++)
		{
			selectResult->push_back((long)res->getValue(i));
		}
		
		delete res;
		return selectResult;
	}
	else
	{
		delete res;
		return NULL;
	}
}

#pragma endregion
