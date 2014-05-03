/**************************************************************************************
 * File name: ShapeDrawingOptions.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of the CShapeDrawingOptions. Currently
 *				used by SelectionDrawOptions
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
 // lsu 22 aug 2009 - Created the file. Portions of code from MapWindow 6 project were used.

#include "stdafx.h"
#include "ShapeDrawingOptions.h"
#include "ImageAttributes.h"
#include "macros.h"
#include "PointSymbols.h"
#include "Image.h"
#include "LinePattern.h"
#include "Shape.h"

// *************************************************************
//		get_UseLinePattern
// *************************************************************
STDMETHODIMP CShapeDrawingOptions::get_UseLinePattern(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!m_options.CanUseLinePattern() && m_options.useLinePattern)
	{
		m_options.useLinePattern = false;
	}
	*retVal = m_options.useLinePattern ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *************************************************************
//		put_UseLinePattern
// *************************************************************
STDMETHODIMP CShapeDrawingOptions::put_UseLinePattern(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.useLinePattern = newVal && m_options.CanUseLinePattern();
	return S_OK;
}

// *************************************************************
//		get_LastErrorCode
// *************************************************************
STDMETHODIMP CShapeDrawingOptions::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;

	return S_OK;
}

// *************************************************************
//		get_ErrorMsg
// *************************************************************
STDMETHODIMP CShapeDrawingOptions::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

// *******************************************************
//	   get_Picture()
// *******************************************************
STDMETHODIMP CShapeDrawingOptions::get_Picture(IImage** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_options.picture;
	if ( m_options.picture )
		m_options.picture->AddRef();
	return S_OK;
};

// *******************************************************
//	   put_Picture()
// *******************************************************
STDMETHODIMP CShapeDrawingOptions::put_Picture(IImage* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	tkImageSourceType type;
	newVal->get_SourceType(&type);

	long width, height;
	newVal->get_Width(&width);
	newVal->get_Height(&height);
	if (width * height > 3000000)
	{
		ErrorMessage(tkICON_OR_TEXTURE_TOO_BIG);
	}
	else
	{
		if (type == istGDALBased)
		{
			if (width == 0 || height == 0)
			{
				VARIANT_BOOL vbretval;
				newVal->LoadBuffer(10, &vbretval); 
			}
		}
		
		newVal->get_Width(&width);
		newVal->get_Height(&height);
		
		if (width > 0 && height > 0)
		{
			Utility::put_ComReference(newVal, (IDispatch**)&m_options.picture);
		}
	}
	return S_OK;
};

// There are 2 overloads for each function: 
// hdc passed as int** - for new Graphics handle
// hdc passed as int - for VB6/VBA 

#pragma region DrawPoint
// *******************************************************
//	   DrawPoint()
// *******************************************************
STDMETHODIMP CShapeDrawingOptions::DrawPoint(int** hdc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CDC* dc = CDC::FromHandle((HDC)hdc);
	
	if (clipWidth == 0)
		clipWidth = (int)m_options.pointSize + 1;
	
	if (clipHeight == 0)
		clipHeight =  (int)m_options.pointSize + 1;

	*retVal = DrawPointCore(dc, x, y, clipWidth, clipHeight, backColor);
	return S_OK;
}

// *******************************************************
//	   DrawPointVB()
// *******************************************************
STDMETHODIMP CShapeDrawingOptions::DrawPointVB(int hdc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CDC* dc = CDC::FromHandle((HDC)hdc);
	
	if (clipWidth == 0)
		clipWidth = (int)m_options.pointSize + 1;
	
	if (clipHeight == 0)
		clipHeight =  (int)m_options.pointSize + 1;

	*retVal = DrawPointCore(dc, x, y, clipWidth, clipHeight, backColor);
	return S_OK;
}

// *******************************************************
//	   DrawPointCore()
// *******************************************************
VARIANT_BOOL CShapeDrawingOptions::DrawPointCore(CDC* dc, float x, float y, int clipWidth, int clipHeight, OLE_COLOR backColor )
{
	if (!dc)
	{
		ErrorMessage(tkFAILED_TO_OBTAIN_DC);
		return VARIANT_FALSE;
	}
	
	IShape* shp = NULL;
	CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
	if (!shp) 
		return VARIANT_FALSE;

	VARIANT_BOOL vbretval;
	shp->Create(SHP_POINT, &vbretval);
	if (!vbretval)
	{
		shp->Release();
		return VARIANT_FALSE;
	}

	IPoint* pnt = NULL;
	m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
	pnt->put_X(clipWidth/2.0); 
	pnt->put_Y(clipHeight/2.0);
	long position = 0;
	shp->InsertPoint(pnt, &position, &vbretval); 
	pnt->Release();
	
	VARIANT_BOOL result = this->DrawShapeCore(dc, x, y, shp, VARIANT_FALSE, clipWidth, clipHeight, backColor);
	shp->Release();

	return result;
}
#pragma endregion

#pragma region DrawLine

// *******************************************************
//	   DrawLine()
// *******************************************************
STDMETHODIMP CShapeDrawingOptions::DrawLine(int** hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CDC* dc = CDC::FromHandle((HDC)hdc);

	if (clipWidth == 0)
		clipWidth = width + 1;

	if (clipHeight == 0)
		clipHeight = height + 1;

	*retVal = DrawLineCore(dc, x, y, width, height, drawVertices, clipWidth, clipHeight, backColor);
	return S_OK;
}

// *******************************************************
//	   DrawLineVB()
// *******************************************************
STDMETHODIMP CShapeDrawingOptions::DrawLineVB(int hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CDC* dc = CDC::FromHandle((HDC)hdc);
	
	if (clipWidth == 0)
		clipWidth = width + 1;

	if (clipHeight == 0)
		clipHeight = height + 1;

	*retVal = DrawLineCore(dc, x, y, width, height, drawVertices, clipWidth, clipHeight, backColor);
	return S_OK;
}

// *******************************************************
//	   DrawLineCore()
// *******************************************************
VARIANT_BOOL CShapeDrawingOptions::DrawLineCore(CDC* dc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, int clipWidth, int clipHeight, OLE_COLOR backColor)
{
	if (!dc)
	{
		ErrorMessage(tkFAILED_TO_OBTAIN_DC);
		return VARIANT_FALSE;
	}
	
	IShape* shp = NULL;
	CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
	if (!shp) return VARIANT_FALSE;

	VARIANT_BOOL vbretval;
	shp->Create(SHP_POLYLINE, &vbretval);
	
    long position = 0;
    shp->InsertPart(0, &position, &vbretval);
    
	IPoint* pnt = NULL;
	
	for (int i = 0; i < 2; i++)
	{
		m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
		//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
		
		if ( i == 0 )
		{
			if (m_options.verticesVisible && drawVertices)
			{
				pnt->put_X(m_options.verticesSize/2); 
				pnt->put_Y(height/2);
			}
			else
			{
				pnt->put_X(0.0); 
				pnt->put_Y(height/2);
			}
		}
		else if ( i == 1 )
		{
			if (m_options.verticesVisible && drawVertices)
			{
				pnt->put_X(width); 
				pnt->put_Y(height/2);
			}
			else
			{
				pnt->put_X(width - m_options.verticesSize/2); 
				pnt->put_Y(height/2);
			}
		}
		shp->InsertPoint(pnt, &position, &vbretval); position++;
		pnt->Release();
	}
	
	VARIANT_BOOL retVal = this->DrawShapeCore(dc, x, y, shp, drawVertices, clipWidth, clipHeight, backColor);
	shp->Release();
	return retVal;
}
#pragma endregion

#pragma region DrawRectangle

// *****************************************************************
//	   DrawRectangle()
// *****************************************************************
// We shall create the rectangle shape here
STDMETHODIMP CShapeDrawingOptions::DrawRectangle(int** hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, 
												 int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CDC* dc = CDC::FromHandle((HDC)hdc);

	if (clipWidth == 0)
		clipWidth = width + 1;

	if (clipHeight == 0)
		clipHeight = height + 1;

	*retVal = this->DrawRectangleCore(dc, x, y, width, height, drawVertices, clipWidth, clipHeight, backColor);
	return S_OK;
}

// *****************************************************************
//	   DrawRectangleVB()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::DrawRectangleVB(int hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, 
												   int clipWidth, int clipHeight, OLE_COLOR backColor, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDC* dc = CDC::FromHandle((HDC)hdc);
	
	if (clipWidth == 0)
		clipWidth = width + 1;

	if (clipHeight == 0)
		clipHeight = height + 1;

	*retVal = DrawRectangleCore(dc, x, y, width, height, drawVertices, clipWidth, clipHeight, backColor);
	return S_OK;
}

// ****************************************************************
//		DrawRectangleCore()
// ****************************************************************
VARIANT_BOOL CShapeDrawingOptions::DrawRectangleCore(CDC* dc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, int clipWidth, int clipHeight, OLE_COLOR backColor )
{
	if (!dc)
	{
		ErrorMessage(tkFAILED_TO_OBTAIN_DC);
		return VARIANT_FALSE;
	}
	
	IShape* shp = NULL;
	CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
	if (!shp) return VARIANT_FALSE;

	VARIANT_BOOL vbretval;
	shp->Create(SHP_POLYGON, &vbretval);
	
    long position = 0;
    shp->InsertPart(0, &position, &vbretval);
    
	IPoint* pnt = NULL;
	
	for (int i = 0; i <= 4; i++)	// <=4
	{
		m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
		//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
		
		if (i == 0 || i == 4)
		{
			if (m_options.verticesVisible && drawVertices)
			{
				pnt->put_X(m_options.verticesSize/2); 
				pnt->put_Y(m_options.verticesSize/2);
			}
			else
			{
				pnt->put_X(0.0); 
				pnt->put_Y(0.0);
			}
		}
		else if (i == 1)
		{
			if (m_options.verticesVisible && drawVertices)
			{
				pnt->put_X(width - m_options.verticesSize/2); 
				pnt->put_Y(m_options.verticesSize/2);
			}
			else
			{
				pnt->put_X(width); 
				pnt->put_Y(0.0);
			}
		}
		else if (i == 2)
		{
			if (m_options.verticesVisible && drawVertices)
			{
				pnt->put_X(width - m_options.verticesSize/2); 
				pnt->put_Y(height - m_options.verticesSize/2); 
			}
			else
			{
				pnt->put_X(width); 
				pnt->put_Y(height);
			}
		}
		else if (i == 3)
		{
			if (m_options.verticesVisible && drawVertices)
			{
				pnt->put_X(m_options.verticesSize/2); 
				pnt->put_Y(height - m_options.verticesSize/2); 
			}
			else
			{
				pnt->put_X(0.0); 
				pnt->put_Y(height);
			}
		}

		shp->InsertPoint(pnt, &position, &vbretval); position++;
		pnt->Release();
	}
	
	VARIANT_BOOL retVal = this->DrawShapeCore(dc, x, y, shp, drawVertices, clipWidth, clipHeight, backColor);
	shp->Release();
	return retVal;

	return VARIANT_TRUE;
}
#pragma endregion

#pragma region DrawShape

// *****************************************************************
//	   DrawShape()
// *****************************************************************
// We shall create the rectangle shape here
STDMETHODIMP CShapeDrawingOptions::DrawShape(int** hdc, float x, float y, IShape* shape, VARIANT_BOOL drawVertices, 
											 int clipWidth, int clipHeight, OLE_COLOR backColor, 
											 VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CDC* dc = CDC::FromHandle((HDC)hdc);

	*retVal = this->DrawShapeCore(dc, x, y, shape, drawVertices, clipWidth, clipHeight, backColor);
	return S_OK;
}

// *****************************************************************
//	   DrawShapeVB()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::DrawShapeVB(int hdc, float x, float y, IShape* shape, VARIANT_BOOL drawVertices, 
											   int clipWidth, int clipHeight, OLE_COLOR backColor, 
											   VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDC* dc = CDC::FromHandle((HDC)hdc);

	*retVal = this->DrawShapeCore(dc, x, y, shape, drawVertices, clipWidth, clipHeight, backColor);
	return S_OK;
}

// *****************************************************************
//	   DrawShapeCore()
// *****************************************************************
VARIANT_BOOL CShapeDrawingOptions::DrawShapeCore(CDC* dc, float x, float y, IShape* shape, VARIANT_BOOL drawVertices,
												 int clipWidth, int clipHeight, OLE_COLOR backColor)
{
	if (!dc)
	{
		_lastErrorCode = tkFAILED_TO_OBTAIN_DC;
		return VARIANT_FALSE;
	}
	if (!shape)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return VARIANT_FALSE;
	}
	
	long numPoints;
	shape->get_NumPoints(&numPoints);
	if (numPoints == 0)	return S_OK;

	Gdiplus::Bitmap bmp(clipWidth, clipHeight, PixelFormat32bppARGB);
	Gdiplus::Graphics g(&bmp);

	// reading the points
	Gdiplus::Point* points = new Gdiplus::Point[numPoints];
	for (long i = 0; i < numPoints; i++)
	{
		double xVal, yVal;
		VARIANT_BOOL vbretval;
		shape->get_XY(i, &xVal, &yVal, &vbretval);
		points[i].X = (int)xVal;
		points[i].Y = (int)yVal;
	}
	
	Gdiplus::GraphicsPath path;
	path.AddLines(points, numPoints);

	// getting shape type
	ShpfileType shpType;
	shape->get_ShapeType(&shpType);
	ShpfileType simpleType =Utility::ShapeTypeConvert2D(shpType);
	
	// do we have line pattern here?
	bool usePattern = false;
	if (simpleType == SHP_POLYLINE)
	{
		usePattern = m_options.useLinePattern && m_options.CanUseLinePattern();
	}

	if (usePattern)
	{
		VARIANT_BOOL vbretval;
		HDC hdcTemp = g.GetHDC();
		m_options.linePattern->Draw((int**)hdcTemp, 0.0f, 0.0f, clipWidth, clipHeight, backColor, &vbretval);
		g.ReleaseHDC(hdcTemp);
	}
	else
	{
		Gdiplus::Color clr(255 << 24 | BGR_TO_RGB(backColor));
		Gdiplus::SolidBrush brushBackground(clr);
		
		g.Clear(clr);
		
		// calculating bounding rectangle (for gradient)
		double xMin, xMax, yMin, yMax;
		((CShape*)shape)->get_ExtentsXY(xMin, yMin, xMax, yMax);

		Gdiplus::RectF rect((Gdiplus::REAL)(xMin), (Gdiplus::REAL)(yMin), (Gdiplus::REAL)(xMax - xMin), (Gdiplus::REAL)(yMax - yMin));
		
		// initializing
		if (simpleType != SHP_POINT)
		{
			m_options.InitGdiPlusBrushAndPen(&rect);
			if (m_options.penPlus)
				m_options.penPlus->SetAlignment(Gdiplus::PenAlignmentInset);
		}
		
		// drawing
		Gdiplus::SmoothingMode mode;
		mode = g.GetSmoothingMode();

		if (simpleType == SHP_POLYGON)
		{
			g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);

			if (numPoints == 2 )
			{
				Gdiplus::Rect r(points[0].X, points[0].Y, points[1].X, points[1].Y);
				
				if (m_options.fillVisible)
				{
					g.FillRectangle(&brushBackground, r);
					g.FillRectangle(m_options.brushPlus, r);
				}
				
				if (m_options.penPlus)
					g.DrawRectangle(m_options.penPlus, r);
			}
			else
			{
				if (m_options.fillVisible)
				{
					g.FillPolygon(&brushBackground, points, numPoints);
					g.FillPolygon(m_options.brushPlus, points, numPoints);
				}

				if (m_options.penPlus)
					g.DrawPolygon(m_options.penPlus, points, numPoints);
			}
		}
		else if (simpleType == SHP_POLYLINE)
		{
			g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);

			if (m_options.penPlus)
				g.DrawLines(m_options.penPlus, points, numPoints);
		}
		else if (simpleType == SHP_POINT)
		{
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			
			m_options.DrawPointSymbol(g, dc, points, NULL, 1);
		}

		// clearing
		g.SetSmoothingMode(mode);
		m_options.ReleaseGdiPlusBrush();
		m_options.ReleaseGdiPlusPen();
	}

	// -----------------------------------------
	// drawing of vertices
	// -----------------------------------------
	if ( m_options.verticesVisible && drawVertices)
	{
		dc->EndPath();
		m_options.InitGdiVerticesPen(dc);
		if (simpleType == SHP_POLYLINE || simpleType == SHP_POLYGON )
		{
			CPoint* square = m_options.GetVertex();
			int size = m_options.verticesSize/2;
			
			Gdiplus::SolidBrush brush(255<<24 | BGR_TO_RGB(m_options.verticesColor));
			Gdiplus::Pen pen(255<<24 | BGR_TO_RGB(m_options.verticesColor));

			for (int i = 0; i < numPoints; i++)
			{
				Gdiplus::Matrix mtx;
				mtx.Translate((Gdiplus::REAL)points[i].X, (Gdiplus::REAL)points[i].Y);
				g.SetTransform(&mtx);
			
				if ( m_options.verticesType == vtSquare )
				{
					if (m_options.verticesFillVisible )
					{
						g.FillPolygon(&brush, reinterpret_cast<Gdiplus::Point*>( square), 4);
					}
					else
					{
						g.DrawPolygon(&pen, reinterpret_cast<Gdiplus::Point*> (square), 4);
					}

				}
				else if ( m_options.verticesType == vtCircle )
				{
					if (m_options.verticesFillVisible)
					{
						g.FillEllipse(&brush, -size, -size, m_options.verticesSize, m_options.verticesSize);
					}
					else
					{
						g.DrawEllipse(&pen, -size, -size, m_options.verticesSize, m_options.verticesSize);
					}
				}

				mtx.Reset();
				g.SetTransform(&mtx);
			}
			delete[] square;
		}
		m_options.ReleaseGdiBrushAndPen(dc);
	}
	
	Gdiplus::Graphics gResult(dc->GetSafeHdc());
	gResult.DrawImage(&bmp, x, y);
	
	delete[] points;

	return VARIANT_TRUE;
}
#pragma endregion

// ****************************************************
//	 GetPointShape()
// ****************************************************
STDMETHODIMP CShapeDrawingOptions::GetPointShape(IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	return S_OK;
}

// *********************************************************
//		get_PointCharacter
// *********************************************************
STDMETHODIMP CShapeDrawingOptions::get_PointCharacter (short* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = (short)m_options.pointCharcter;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PointCharacter (short newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_options.pointCharcter = (unsigned char)newVal;
	return S_OK;
}

// *****************************************************************
//		FontName()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FontName(BSTR* retval)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retval = A2BSTR(m_options.fontName);
	return S_OK;
};	
STDMETHODIMP CShapeDrawingOptions::put_FontName(BSTR newVal)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	m_options.fontName = OLE2CA(newVal);
	return S_OK;
};	

// *****************************************************************
//		FontName()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_Tag(BSTR* retVal)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = A2BSTR(m_options.tag);
	return S_OK;
};	
STDMETHODIMP CShapeDrawingOptions::put_Tag(BSTR newVal)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	m_options.tag = OLE2CA(newVal);
	return S_OK;
};	

// *****************************************************************
//		Clone()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::Clone(IShapeDrawingOptions** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	IShapeDrawingOptions* sdo = NULL;
	CoCreateInstance(CLSID_ShapeDrawingOptions,NULL,CLSCTX_INPROC_SERVER,IID_IShapeDrawingOptions,(void**)&sdo);
	if (sdo)
	{
		((CShapeDrawingOptions*)sdo)->put_underlyingOptions(&m_options);
	}
	*retval = sdo;
	return S_OK;
}

// *****************************************************************
//		VerticesVisible()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_VerticesVisible(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.verticesVisible;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_VerticesVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.verticesVisible = newVal;
	return S_OK;
}

// *****************************************************************
//		VerticesSize()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_VerticesSize(LONG* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.verticesSize;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_VerticesSize(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.verticesSize = newVal;
	return S_OK;
}

// *****************************************************************
//		VerticesColor()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_VerticesColor(OLE_COLOR* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.verticesColor;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_VerticesColor(OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.verticesColor = newVal;
	return S_OK;
}

// *****************************************************************
//		VerticesFillVisible()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_VerticesFillVisible(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.verticesFillVisible;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_VerticesFillVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.verticesFillVisible = newVal;
	return S_OK;
}

// *****************************************************************
//		VerticesType()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_VerticesType(tkVertexType* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.verticesType;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_VerticesType(tkVertexType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.verticesType = newVal;
	return S_OK;
}

// *****************************************************************
//		get_FrameVisible()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FrameVisible(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.drawFrame;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FrameVisible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		m_options.drawFrame = newVal ? true: false;
	return S_OK;
}

// *****************************************************************
//		get_FrameType()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FrameType(tkLabelFrameType* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.frameType;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FrameType(tkLabelFrameType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.frameType = newVal;
	return S_OK;
}

// *****************************************************************
//		LinePattern ()
// *****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_LinePattern(ILinePattern** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = m_options.linePattern;
	if (m_options.linePattern)
	{
		m_options.linePattern->AddRef();
	}

	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_LinePattern(ILinePattern* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference((IDispatch*)newVal, (IDispatch**)&(m_options.linePattern), false);
	return S_OK;
}

// ****************************************************************
//		SetGradientFill
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::SetGradientFill(OLE_COLOR color, short range)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (range < 0)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	unsigned char r = GetRValue(color);
	unsigned char g = GetGValue(color);
	unsigned char b = GetBValue(color);

	m_options.fillColor = RGB(MAX(r - range/2, 0) , MAX(g - range/2, 0), MAX(b- range/2,0));
	m_options.fillColor2 = RGB(MIN(r + range/2, 255) , MIN(g + range/2,255), MIN(b+ range/2,255));
	m_options.fillType = ftGradient;
	return S_OK;
}

// ****************************************************************
//		SetDefaultPointSymbol
// ****************************************************************
// Gives fast access to the most common shapes
STDMETHODIMP CShapeDrawingOptions::SetDefaultPointSymbol (tkDefaultPointSymbol symbol)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_options.pointSymbolType = ptSymbolStandard;
	m_options.rotation = 0.0;

	tkPointShapeType type;
	double rotation;
	float sidesRatio;
	int numSides; 

	get_DefaultSymbolOptions(symbol, type, rotation, numSides, sidesRatio);
	
	m_options.pointShapeType = type;
	m_options.rotation = rotation;
	m_options.pointNumSides = numSides;
	m_options.pointShapeRatio = sidesRatio;

	return S_OK;
}

#pragma region Properties

// ****************************************************************
//		get_Visible
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_Visible(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = (VARIANT_BOOL)m_options.visible; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_Visible(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.visible = newVal?true:false;	
	return S_OK;
}

// ****************************************************************
//		get_FillVisible
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillVisible (VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.fillVisible;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillVisible (VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.fillVisible = newVal?true:false;	
	return S_OK;
}

// ****************************************************************
//		get_LineVisible
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_LineVisible (VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.linesVisible; 				
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_LineVisible (VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_options.linesVisible = newVal?true:false;	
	return S_OK;
}

// ****************************************************************
//		get_FillColor
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillColor (OLE_COLOR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillColor; 		
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillColor (OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_options.fillColor = newVal;
	return S_OK;
}

// ****************************************************************
//		get_LineColor
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_LineColor (OLE_COLOR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.lineColor;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_LineColor (OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.lineColor = newVal;		
	return S_OK;
}

// ****************************************************************
//		get_DrawingMode
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_DrawingMode (tkVectorDrawingMode *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.drawingMode;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_DrawingMode (tkVectorDrawingMode newVal)	
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	// It's no longer allowed to change drawing mode
	//m_options.drawingMode = newVal;
	return S_OK;
}

// ****************************************************************
//		get_FillHatchStyle
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillHatchStyle (tkGDIPlusHatchStyle *pVal)	
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillHatchStyle; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillHatchStyle (tkGDIPlusHatchStyle newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal >= -1 && newVal <= 52)
	{
		m_options.fillHatchStyle = newVal;	
	}
	else
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// ****************************************************************
//		get_LineStipple
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_LineStipple (tkDashStyle *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.lineStipple; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_LineStipple (tkDashStyle newVal)			
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal >= 0 && newVal <= 5)
	{
		m_options.lineStipple = newVal;	
	}
	else
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// ****************************************************************
//		get_PointShape
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PointShape (tkPointShapeType *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.pointShapeType;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PointShape (tkPointShapeType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0 && newVal <= 5)
	{
		m_options.pointShapeType = newVal;	
	}
	else
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// ****************************************************************
//		get_FillTransparency
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillTransparency (float *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.fillTransparency;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillTransparency (float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal < 0) newVal = 0;
	if (newVal > 255) newVal = 255;
	m_options.fillTransparency = newVal;	
	return S_OK;
}

// ****************************************************************
//		get_LineWidth
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_LineWidth (float *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.lineWidth; 			
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_LineWidth (float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal < 1) newVal = 1;
	if (newVal > 20) newVal = 20;
	m_options.lineWidth = newVal;		
	return S_OK;
}

// ****************************************************************
//		get_PointSize
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PointSize (float *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.pointSize; 							
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PointSize (float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal < 1) newVal = 1;
	if (newVal > 100) newVal = 100;
	m_options.pointSize = newVal;							
	return S_OK;
}


// ****************************************************************
//		get_FillBgTransparent
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillBgTransparent (VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillBgTransparent; 				
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillBgTransparent (VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.fillBgTransparent = newVal?true:false;	
	return S_OK;
}

// ****************************************************************
//		get_FillBgColor
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillBgColor (OLE_COLOR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillBgColor; 					
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillBgColor (OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.fillBgColor = newVal;					
	return S_OK;
}

// ****************************************************************
//		get_FillType
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillType (tkFillType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillType; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillType (tkFillType newVal)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal >= 0 && newVal <= 3)
	{
		m_options.fillType = newVal;		
	}
	else
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// ****************************************************************
//		get_FillGradientType
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillGradientType (tkGradientType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillGradientType; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillGradientType (tkGradientType newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal >= 0 && newVal <= 2)
	{
		m_options.fillGradientType = newVal;	
	}
	else
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// ****************************************************************
//		get_PointType
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PointType(tkPointSymbolType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.pointSymbolType;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PointType (tkPointSymbolType newVal)			
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal >= 0 && newVal <= 2)
	{
		m_options.pointSymbolType = newVal;
	}
	else
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// ****************************************************************
//		get_FillColor2
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillColor2 (OLE_COLOR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillColor2; 		
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillColor2 (OLE_COLOR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.fillColor2 = newVal;		
	return S_OK;
}

// ****************************************************************
//		get_PointRotation
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PointRotation (double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.rotation; 		
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PointRotation (double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal > 360.0 || newVal < -360.0)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	else
	{
		m_options.rotation = newVal;		
	}
	return S_OK;
}

// ****************************************************************
//		get_PointSidesCount
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PointSidesCount (long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.pointNumSides;	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PointSidesCount (long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal > 20) newVal = 20;
	if (newVal < 2) newVal = 2;
	m_options.pointNumSides = newVal;	
	return S_OK;
}

// ****************************************************************
//		get_PointSidesRatio
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PointSidesRatio (float *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.pointShapeRatio;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PointSidesRatio (float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal < 0.1f) newVal = 0.1f;
	if (newVal > 1.0f) newVal = 1.0f;
	m_options.pointShapeRatio = newVal;
	return S_OK;
}

// ****************************************************************
//		get_PointSidesRatio
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillRotation(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillGradientRotation;	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillRotation (double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal > 360.0 || newVal < -360.0)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	else
	{
		m_options.fillGradientRotation = newVal;
	}
	return S_OK;
}

// ****************************************************************
//		get_FillGradientBounds
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_FillGradientBounds (tkGradientBounds *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.fillGradientBounds;	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_FillGradientBounds (tkGradientBounds newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal >= 0 && newVal <= 1)
	{
		m_options.fillGradientBounds = newVal;	
	}
	else
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
	}
	return S_OK;
}

// ****************************************************************
//		get_LineTransparency
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_LineTransparency(float *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_options.lineTransparency;
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_LineTransparency(float newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal < 0) newVal = 0;
	if (newVal > 255) newVal = 255;
	m_options.lineTransparency = newVal;	
	return S_OK;
}

// ****************************************************************
//		get_PictureScaleX
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PictureScaleX(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.scaleX; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PictureScaleX(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal < 0.1) newVal = 0.1;
	if (newVal > 5.0) newVal = 5.0;
	m_options.scaleX = newVal;	
	return S_OK;
}

// ****************************************************************
//		get_PictureScaleY
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_PictureScaleY(double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.scaleY; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_PictureScaleY(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	if (newVal < 0.1) newVal = 0.1;
	if (newVal > 5.0) newVal = 5.0;
	m_options.scaleY = newVal;	
	return S_OK;
}
#pragma endregion

// ****************************************************************
//		get_AlignPictureBottom
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_AlignPictureByBottom(VARIANT_BOOL *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.alignIconByBottom; 	
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_AlignPictureByBottom(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.alignIconByBottom = newVal ? true: false;	
	return S_OK;
}

// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CShapeDrawingOptions::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CPLXMLNode* node = SerializeCore("ShapeDrawingOptionsClass");
	if (node)
	{
		CString str;
		char* buffer = CPLSerializeXMLTree(node);	
		str = buffer;
		CPLFree(buffer);
		*retVal = A2BSTR(str);
		CPLDestroyXMLNode(node);
	}
	else
	{
		*retVal = A2BSTR("");
	}
	return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
CPLXMLNode* CShapeDrawingOptions::SerializeCore(CString ElementName)
{
	USES_CONVERSION;
	
	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, ElementName);
	CString str;
	
	CDrawingOptionsEx* opt = new CDrawingOptionsEx();

	if (opt->fillBgColor != m_options.fillBgColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillBgColor", CPLString().Printf("%d", m_options.fillBgColor));

	if (opt->fillBgTransparent != m_options.fillBgTransparent)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillBgTransparent", CPLString().Printf("%d", (int)m_options.fillBgTransparent));

	if (opt->fillColor != m_options.fillColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillColor", CPLString().Printf("%d", m_options.fillColor));

	if (opt->fillColor2 != m_options.fillColor2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillColor2", CPLString().Printf("%d", m_options.fillColor2));
	
	if (opt->fillGradientBounds != m_options.fillGradientBounds)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillGradientBounds", CPLString().Printf("%d", (int)m_options.fillGradientBounds));
	
	if (opt->fillGradientRotation != m_options.fillGradientRotation)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillGradientRotation", CPLString().Printf("%f", m_options.fillGradientRotation));
	
	if (opt->fillGradientType != m_options.fillGradientType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillGradientType", CPLString().Printf("%d", (int)m_options.fillGradientType));
	
	if (opt->fillHatchStyle != m_options.fillHatchStyle)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillHatchStyle", CPLString().Printf("%d", (int)m_options.fillHatchStyle));
	
	if (opt->fillTransparency != m_options.fillTransparency)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillTransparency", CPLString().Printf("%f", m_options.fillTransparency));
	
	if (opt->fillType != m_options.fillType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillType", CPLString().Printf("%d", (int)m_options.fillType));
	
	if (opt->fillVisible != m_options.fillVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FillVisible", CPLString().Printf("%d", (int)m_options.fillVisible));
	
	if (opt->fontName != m_options.fontName)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontName", m_options.fontName);
	
	if (opt->lineColor != m_options.lineColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineColor", CPLString().Printf("%d", m_options.lineColor));

	if (opt->lineTransparency != m_options.lineTransparency)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineTransparency", CPLString().Printf("%f", m_options.lineTransparency));
	
	if (opt->lineStipple != m_options.lineStipple)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineStipple", CPLString().Printf("%d", (int)m_options.lineStipple));
	
	if (opt->linesVisible != m_options.linesVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LinesVisible", CPLString().Printf("%d", (int)m_options.linesVisible));
	
	if (opt->lineWidth != m_options.lineWidth)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineWidth", CPLString().Printf("%f", m_options.lineWidth));
	
	if (opt->pointCharcter != m_options.pointCharcter)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointCharcter", CPLString().Printf("%d", (int)m_options.pointCharcter));
	
	if (opt->pointColor != m_options.pointColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointColor", CPLString().Printf("%d", m_options.pointColor));
	
	if (opt->pointNumSides != m_options.pointNumSides)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointNumSides", CPLString().Printf("%d", m_options.pointNumSides));
	
	if (opt->pointShapeRatio != m_options.pointShapeRatio)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointShapeRatio", CPLString().Printf("%f", m_options.pointShapeRatio));
	
	if (opt->pointShapeType != m_options.pointShapeType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointShapeType", CPLString().Printf("%d", (int)m_options.pointShapeType));
	
	if (opt->pointSize != m_options.pointSize)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointSize", CPLString().Printf("%f", m_options.pointSize));
	
	if (opt->pointSymbolType != m_options.pointSymbolType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "PointSymbolType", CPLString().Printf("%d", (int)m_options.pointSymbolType));
	
	if (opt->rotation != m_options.rotation)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Rotation", CPLString().Printf("%f", m_options.rotation));
	
	if (opt->scaleX != m_options.scaleX)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ScaleX", CPLString().Printf("%f", m_options.scaleX));
	
	if (opt->scaleY != m_options.scaleY)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ScaleY", CPLString().Printf("%f", m_options.scaleY));
	
	if (opt->verticesColor != m_options.verticesColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VerticesColor", CPLString().Printf("%d", m_options.verticesColor));
	
	if (opt->verticesFillVisible != m_options.verticesFillVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VerticesFillVisible", CPLString().Printf("%d", (int)m_options.verticesFillVisible));
	
	if (opt->verticesSize != m_options.verticesSize)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VerticesSize", CPLString().Printf("%d", m_options.verticesSize));
	
	if (opt->verticesType != m_options.verticesType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VerticesType", CPLString().Printf("%d", (int)m_options.verticesType));
	
	if (opt->verticesVisible != m_options.verticesVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VerticesVisible", CPLString().Printf("%d", (int)m_options.verticesVisible));
		
	if (opt->visible != m_options.visible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Visible", CPLString().Printf("%d", (int)m_options.visible));

	if (opt->useLinePattern != m_options.useLinePattern)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseLinePattern", CPLString().Printf("%d", (int)m_options.useLinePattern));

	if (opt->alignIconByBottom != m_options.alignIconByBottom)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AlignPictureByBottom", CPLString().Printf("%d", (int)m_options.alignIconByBottom));

	if (opt->drawFrame != m_options.drawFrame)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameVisible", CPLString().Printf("%d", (int)m_options.drawFrame));

	if (opt->frameType != m_options.frameType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameType", CPLString().Printf("%d", (int)m_options.frameType));

	delete opt;

	if (m_options.linePattern)
	{
		CPLXMLNode* psNode = ((CLinePattern*)m_options.linePattern)->SerializeCore("LinePatternClass");
		if (psNode)
		{
			CPLAddXMLChild(psTree, psNode);
		}
	}
	
	if (m_options.picture)
	{
		CPLXMLNode* psNode = ((CImageClass*)m_options.picture)->SerializeCore(VARIANT_TRUE, "Picture");
		if (psNode)
		{
			CPLAddXMLChild(psTree, psNode);
		}
	}
	
	return psTree;
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CShapeDrawingOptions::DeserializeCore(CPLXMLNode* node)
{
	if (!node)
		return false;
	
	// to set default options to the values that were not serialized
	CDrawingOptionsEx* opt = new CDrawingOptionsEx();

	CString s;
	
	s = CPLGetXMLValue( node, "FillBgColor", NULL );
	m_options.fillBgColor = (s == "") ? opt->fillBgColor : (OLE_COLOR)atoi(s.GetString());

	s = CPLGetXMLValue( node, "FillBgTransparent", NULL );
	m_options.fillBgTransparent = (s == "") ? opt->fillBgTransparent : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue( node, "FillColor", NULL );
	m_options.fillColor = (s == "") ? opt->fillColor : (OLE_COLOR)atoi(s.GetString());

	s = CPLGetXMLValue( node, "FillColor2", NULL );
	m_options.fillColor2 = (s == "") ? opt->fillColor2 : (OLE_COLOR)atoi(s.GetString());

	s = CPLGetXMLValue( node, "FillGradientBounds", NULL );
	m_options.fillGradientBounds = (s == "") ? opt->fillGradientBounds : (tkGradientBounds)atoi(s.GetString());

	s = CPLGetXMLValue( node, "FillGradientRotation", NULL );
	m_options.fillGradientRotation = (s == "") ? opt->fillGradientRotation : Utility::atof_custom(s);

	s = CPLGetXMLValue( node, "FillGradientType", NULL );
	m_options.fillGradientType = (s == "") ? opt->fillGradientType : (tkGradientType)atoi(s.GetString());
	
	s = CPLGetXMLValue( node, "FillHatchStyle", NULL );
	m_options.fillHatchStyle = (s == "") ? opt->fillHatchStyle : (tkGDIPlusHatchStyle)atoi(s.GetString());

	s = CPLGetXMLValue( node, "FillTransparency", NULL );
	m_options.fillTransparency = (s == "") ? opt->fillTransparency : (float)Utility::atof_custom(s);

	s = CPLGetXMLValue( node, "FillType", NULL );
	m_options.fillType = (s == "") ? opt->fillType : (tkFillType)atoi(s.GetString());
	
	s = CPLGetXMLValue( node, "FillVisible", NULL );
	m_options.fillVisible = (s == "") ? opt->fillVisible : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue( node, "FontName", NULL );
	m_options.fontName = (s == "") ? opt->fontName : s.GetString();

	s = CPLGetXMLValue( node, "LineColor", NULL );
	m_options.lineColor = (s == "") ? opt->lineColor : (OLE_COLOR)atoi(s.GetString());
	
	s = CPLGetXMLValue( node, "LineStipple", NULL );
	m_options.lineStipple = (s == "") ? opt->lineStipple : (tkDashStyle)atoi(s.GetString());

	s = CPLGetXMLValue( node, "LineTransparency", NULL );
	m_options.lineTransparency = (s == "") ? opt->lineTransparency : (float)Utility::atof_custom(s);

	s = CPLGetXMLValue( node, "LinesVisible", NULL );
	m_options.linesVisible = (s == "") ? opt->linesVisible : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue( node, "LineWidth", NULL );
	m_options.lineWidth = (s == "") ? opt->lineWidth : (float)Utility::atof_custom(s);

	s = CPLGetXMLValue( node, "PointCharcter", NULL );
	m_options.pointCharcter = (s == "") ? opt->pointCharcter : (unsigned char)atoi(s.GetString());

	s = CPLGetXMLValue( node, "PointColor", NULL );
	m_options.pointColor = (s == "") ? opt->pointColor : (OLE_COLOR)atoi(s.GetString());
	
	s = CPLGetXMLValue( node, "PointNumSides", NULL );
	m_options.pointNumSides = (s == "") ? opt->pointNumSides : atoi(s.GetString());

	s = CPLGetXMLValue( node, "PointShapeRatio", NULL );
	m_options.pointShapeRatio = (s == "") ? opt->pointShapeRatio : (float)Utility::atof_custom(s);

	s = CPLGetXMLValue( node, "PointShapeType", NULL );
	m_options.pointShapeType = (s == "") ? opt->pointShapeType : (tkPointShapeType)atoi(s.GetString());
	
	s = CPLGetXMLValue( node, "PointSize", NULL );
	m_options.pointSize = (s == "") ? opt->pointSize : (float)Utility::atof_custom(s);

	s = CPLGetXMLValue( node, "PointSymbolType", NULL );
	m_options.pointSymbolType = (s == "") ? opt->pointSymbolType : (tkPointSymbolType)atoi(s.GetString());
	
	s = CPLGetXMLValue( node, "Rotation", NULL );
	m_options.rotation = (s == "") ? opt->rotation : Utility::atof_custom(s);
	
	s = CPLGetXMLValue( node, "ScaleX", NULL );
	m_options.scaleX = (s == "") ? opt->scaleX : Utility::atof_custom(s);
	
	s = CPLGetXMLValue( node, "ScaleY", NULL );
	m_options.scaleY = (s == "") ? opt->scaleY : Utility::atof_custom(s);
	
	s = CPLGetXMLValue( node, "VerticesColor", NULL );
	m_options.verticesColor = (s == "") ? opt->verticesColor : atoi(s);

	s = CPLGetXMLValue( node, "VerticesFillVisible", NULL );
	m_options.verticesFillVisible = (s == "") ? opt->verticesFillVisible : (VARIANT_BOOL)atoi(s.GetString());

	s = CPLGetXMLValue( node, "VerticesSize", NULL );
	m_options.verticesSize = (s == "") ? opt->verticesSize : atoi(s.GetString());

	s = CPLGetXMLValue( node, "VerticesType", NULL );
	m_options.verticesSize = (s == "") ? opt->verticesSize : (tkVertexType)atoi(s.GetString());

	s = CPLGetXMLValue( node, "VerticesVisible", NULL );
	m_options.verticesVisible = (s == "") ? opt->verticesVisible : (VARIANT_BOOL)atoi(s.GetString());
	
	s = CPLGetXMLValue( node, "Visible", NULL );
	m_options.visible = (s == "") ? opt->visible : atoi(s.GetString()) == 0 ? false : true;
	
	s = CPLGetXMLValue( node, "AlignPictureByBottom", NULL );
	m_options.alignIconByBottom = (s == "") ? opt->alignIconByBottom : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue( node, "FrameVisible", NULL );
	m_options.drawFrame = (s == "") ? opt->drawFrame : atoi(s.GetString()) == 0 ? false : true;

	s = CPLGetXMLValue( node, "FrameType", NULL );
	m_options.frameType = (s == "") ? opt->frameType : (tkLabelFrameType)atoi(s.GetString());

	delete opt;

	// restoring picture
	CPLXMLNode* psChild = CPLGetXMLNode(node, "Picture");
	if (psChild)
	{
		IImage* img = NULL;
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&img);
		if (img)
		{
			((CImageClass*)img)->DeserializeCore(psChild);
			this->put_Picture(img);
			img->Release();
		}
	}

	// restoring line pattern
	psChild = CPLGetXMLNode(node, "LinePatternClass");
	if (psChild)
	{
		ILinePattern* pattern = NULL;
		CoCreateInstance(CLSID_LinePattern,NULL,CLSCTX_INPROC_SERVER,IID_ILinePattern,(void**)&pattern);
		if (pattern)
		{
			((CLinePattern*)pattern)->DeserializeCore(psChild);
			this->put_LinePattern(pattern);
			pattern->Release();
		}
	}
	else
	{
		if (m_options.linePattern)
		{
			m_options.linePattern->Clear();
			m_options.linePattern->Release();
			m_options.linePattern = NULL;
		}
	}

	s = CPLGetXMLValue( node, "UseLinePattern", NULL );
	m_options.useLinePattern = (s == "") ? opt->useLinePattern : atoi(s.GetString()) == 0 ? false : true;

	return true;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CShapeDrawingOptions::Deserialize(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeSdo = CPLGetXMLNode(node, "=ShapeDrawingOptionsClass");
		if (nodeSdo)
		{
			DeserializeCore(nodeSdo);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

// ****************************************************************
//		get_MinScale
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_MinScale (double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.minScale; 			
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_MinScale (double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.minScale = newVal;		
	return S_OK;
}

// ****************************************************************
//		get_MaxScale
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_MaxScale (double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.maxScale; 			
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_MaxScale (double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.maxScale = newVal;		
	return S_OK;
}

// ****************************************************************
//		get_MinScale
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_MinLineWidth (double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.minLineWidth; 			
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_MinLineWidth (double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.minLineWidth = newVal;		
	return S_OK;
}

// ****************************************************************
//		get_MaxScale
// ****************************************************************
STDMETHODIMP CShapeDrawingOptions::get_MaxLineWidth (double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	*pVal = m_options.maxLineWidth; 			
	return S_OK;
}
STDMETHODIMP CShapeDrawingOptions::put_MaxLineWidth (double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	m_options.maxLineWidth = newVal;		
	return S_OK;
}