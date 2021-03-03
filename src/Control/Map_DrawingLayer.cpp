// Implementation of CMapView class (see other files as well)
// Drawing layer functionality is stored here
// TODO: this code should be wrapped in a separate class

#pragma once
#include "stdafx.h"
#include "Map.h"
#include "LabelDrawing.h"

// ***************************************************************
//		IsValidDrawList()
// ***************************************************************
bool CMapView::IsValidDrawList(long listHandle)
{
	if (listHandle >= 0 && listHandle < (long)_allDrawLists.size())
	{
		return _allDrawLists[listHandle]!=NULL;
	}
	return false;
}

#pragma region REGION DrawingLabels

// ***************************************************************
//		GetDrawingLabelsOffset()
// ***************************************************************
long CMapView::GetDrawingLabelsOffset(long drawHandle)
{
	if( IsValidDrawList(drawHandle) )
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0;
	}
	ErrorMessage(tkINVALID_DRAW_HANDLE);
	return 0;
}

// ***************************************************************
//		SetDrawingLabelsOffset()
// ***************************************************************
void CMapView::SetDrawingLabelsOffset(long drawHandle, long Offset)
{
	if( IsValidDrawList(drawHandle) )
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return;
	}
	ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		GetDrawingLabelsShadowColor()
// ***************************************************************
OLE_COLOR CMapView::GetDrawingLabelsShadowColor(long drawHandle)
{
	if( IsValidDrawList(drawHandle) )
	{
		OLE_COLOR clr;
		_allDrawLists[drawHandle]->m_labels->get_HaloColor(&clr);
		return clr;
	}
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		return RGB(255,255,255);
	}
}

// ***************************************************************
//		SetDrawingLabelsShadowColor()
// ***************************************************************
void CMapView::SetDrawingLabelsShadowColor(long drawHandle, OLE_COLOR color)
{
	if( IsValidDrawList(drawHandle) )
	{
		_allDrawLists[drawHandle]->m_labels->put_HaloColor(color);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		SetDrawingLabelsShadow()
// ***************************************************************
void CMapView::SetDrawingLabelsShadow(long drawHandle, BOOL newValue)
{
	if( IsValidDrawList(drawHandle) )
	{
		_allDrawLists[drawHandle]->m_labels->put_HaloVisible(newValue? VARIANT_TRUE : VARIANT_FALSE);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		GetDrawingLabelsShadow()
// ***************************************************************
BOOL CMapView::GetDrawingLabelsShadow(long drawHandle)
{
	if( IsValidDrawList(drawHandle) )
	{	
		VARIANT_BOOL visible;
		_allDrawLists[drawHandle]->m_labels->get_HaloVisible(&visible);
		return visible ? TRUE: FALSE;
	}
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		return FALSE;
	}
}

// ***************************************************************
//		SetDrawingLabelsScale()
// ***************************************************************
void CMapView::SetDrawingLabelsScale(long drawHandle, BOOL newValue)
{
	if( IsValidDrawList(drawHandle) )
	{
		_allDrawLists[drawHandle]->m_labels->put_ScaleLabels(newValue? VARIANT_TRUE : VARIANT_FALSE);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		GetDrawingLabelsScale()
// ***************************************************************
BOOL CMapView::GetDrawingLabelsScale(long drawHandle)
{
	if( IsValidDrawList(drawHandle) )
	{	
		VARIANT_BOOL scale;
		_allDrawLists[drawHandle]->m_labels->get_ScaleLabels(&scale);
		return scale ? TRUE: FALSE;
	}
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		return FALSE;
	}
}

// ***************************************************************
//		GetUseDrawingLabelCollision()
// ***************************************************************
BOOL CMapView::GetUseDrawingLabelCollision(long drawHandle)
{
	if( IsValidDrawList(drawHandle) )
	{	
		VARIANT_BOOL val;
		_allDrawLists[drawHandle]->m_labels->get_AvoidCollisions(&val);
		return val ? TRUE: FALSE;
	}
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		return FALSE;
	}
}

// ***************************************************************
//		SetUseDrawingLabelCollision()
// ***************************************************************
void CMapView::SetUseDrawingLabelCollision(long drawHandle, BOOL bNewValue)
{
	if( IsValidDrawList(drawHandle) )
	{
		_allDrawLists[drawHandle]->m_labels->put_AvoidCollisions(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		GetDrawingLabelsVisible()
// ***************************************************************
BOOL CMapView::GetDrawingLabelsVisible(long drawHandle)
{
	if( IsValidDrawList(drawHandle) )
	{	
		VARIANT_BOOL visible;
		_allDrawLists[drawHandle]->m_labels->get_Visible(&visible);
		return visible ? TRUE: FALSE;
	}
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		return FALSE;
	}
}

// ***************************************************************
//		SetDrawingLabelsVisible()
// ***************************************************************
void CMapView::SetDrawingLabelsVisible(long drawHandle, BOOL bNewValue)
{
	if( IsValidDrawList(drawHandle) )
	{
		_allDrawLists[drawHandle]->m_labels->put_Visible(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		GetDrawingStandardViewWidth()
// ***************************************************************
void CMapView::GetDrawingStandardViewWidth(long drawHandle, double * Width)
{
	*Width = 0.0;
	if( IsValidDrawList(drawHandle) )
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return;
	}
	ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		SetDrawingStandardViewWidth()
// ***************************************************************
void CMapView::SetDrawingStandardViewWidth(long drawHandle, double Width)
{
	if( IsValidDrawList(drawHandle) )
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return;
	}
	ErrorMessage(tkINVALID_DRAW_HANDLE);
}
#pragma endregion

#pragma region DrawingLayer

// ******************************************************************
//		HasDrawLists
// ******************************************************************
bool CMapView::HasDrawLists()
{
	return _activeDrawLists.size() > 0;
}

// ******************************************************************
//		Draw the DrawLayers
// ******************************************************************
void CMapView::DrawLists(const CRect & rcBounds, Gdiplus::Graphics* graphics, tkDrawReferenceList listType)
{
	// TODO: implement proper collision list for drawing labels; 
	// frames for drawing labels should be cleared from list while frames from the rest layers should remain 
	// in case Map.Refresh() was called
	CCollisionList collisionList;

	// label drawer for spatially referenced list
	CLabelDrawer lblDrawer(graphics, &_extents, _pixelPerProjectionX, _pixelPerProjectionY, 
		this->GetCurrentScale(), _currentZoom, &collisionList, _rotateAngle, _isSnapshot );
	
	// label drawer for screen referenced list
	Extent ext(rcBounds.left, rcBounds.right, rcBounds.top, rcBounds.bottom);
	CLabelDrawer lblDrawerScreen(graphics, &ext, &collisionList, _rotateAngle );

	std::vector<ILabels*> topLabels;	// labels that should be drawn above all layers
	std::vector<tkDrawReferenceList> labelsType;
	
	for(unsigned int j = 0; j < _activeDrawLists.size(); j++ )
	{
		bool isSkip = false;
		for (unsigned int i = 0; i < _drawingLayerInvisilbe.size(); i++)
		{
			if (_drawingLayerInvisilbe[i] == j)
			{
				isSkip = true;	// skip if this layer is set invisible
				break;  
			}
		}
		if(isSkip) 
		{
			continue;
		}

		DrawList * dlist = _allDrawLists[_activeDrawLists[j]];
		if( IS_VALID_PTR(dlist) )
		{
			if (dlist->listType == listType)
			{
                // the main drawing
				this->DrawDrawing( graphics, dlist);
				
				// labels
				ILabels* labels = dlist->m_labels;
				
				if(labels )
				{
					tkVerticalPosition vertPos;
					labels->get_VerticalPosition(&vertPos);
					if (vertPos == vpAboveParentLayer)		
					{
						if (dlist->listType == dlSpatiallyReferencedList)
						{
							lblDrawer.DrawLabels(labels);
						}
						else
						{
							lblDrawerScreen.DrawLabels(labels);
						}
					}
					else
					{
						topLabels.push_back(labels);
						labelsType.push_back(dlist->listType);
					}
				}
			}
		}
	}

	// draw labels above the other layers
	for (unsigned int i = 0; i < topLabels.size(); i++)
	{
		if (labelsType[i] == dlSpatiallyReferencedList)
		{
			lblDrawer.DrawLabels(topLabels[i]);
		}
		else
		{
			lblDrawerScreen.DrawLabels(topLabels[i]);
		}
	}
}

// *****************************************************************
//		DrawDrawing()
// *****************************************************************
void CMapView::DrawDrawing(Gdiplus::Graphics* graphics, DrawList * dlist)
{	
	bool project = (dlist->listType == dlSpatiallyReferencedList);

	for (auto polygon : dlist->m_dpolygons)
		DrawPolygonOnGraphics(graphics, polygon, project);

	for (auto circle : dlist->m_dcircles)
		DrawCircleOnGraphics(graphics, circle, project);

	for (auto line : dlist->m_dlines)
		DrawLineOnGraphics(graphics, line, project);

	for (auto point : dlist->m_dpoints)
		DrawPointOnGraphics(graphics, point, project);
}

void CMapView::DrawCircleOnGraphics(Gdiplus::Graphics* graphics, _DrawCircle* circle, bool project)
{

	auto color = Utility::OleColor2GdiPlus(circle->color, circle->alpha);
	auto radius = (float)circle->radius;
	if (project)
		radius *= _pixelPerProjectionX;
	auto width = (float)circle->width;

	Gdiplus::REAL pixX = (float)circle->x; 
	Gdiplus::REAL pixY = (float)circle->y;
	if (project)
		PROJECTION_TO_PIXEL(pixX, pixY, pixX, pixY);
	pixX -= radius;
	pixY -= radius;
	

	if (circle->fill)
	{
		Gdiplus::SolidBrush brush(color);
		graphics->FillEllipse(&brush, pixX, pixY, radius*2, radius*2);
	}
	else
	{
		Gdiplus::Pen pen(color, width);
		graphics->DrawEllipse(&pen, pixX, pixY, radius*2, radius*2);
	}
}

void CMapView::DrawPolygonOnGraphics(Gdiplus::Graphics* graphics, _DrawPolygon* polygon, bool project)
{
	if (polygon->numPoints <= 0)
		return;

	Gdiplus::Point* pnts = new Gdiplus::Point[polygon->numPoints];
	long pointCount = polygon->numPoints;
	auto width = (float)polygon->width;
	auto color = Utility::OleColor2GdiPlus(polygon->color, polygon->alpha);

	for (int j = 0; j < pointCount; j++)
	{
		auto pixX = polygon->xpnts[j];
		auto pixY = polygon->ypnts[j];
		if (project)
			PROJECTION_TO_PIXEL(pixX, pixY, pixX, pixY);

		int xpt = std::lround(pixX);
		int ypt = std::lround(pixY);
		pnts[j] = Gdiplus::Point(xpt, ypt);
	}

	if (polygon->fill)
	{	
		Gdiplus::SolidBrush brush(color);
		graphics->FillPolygon(&brush, pnts, (INT)pointCount);
	}
	else
	{
		Gdiplus::Pen pen(color, width);
		graphics->DrawPolygon(&pen, pnts, (INT)pointCount);
	}

	delete[] pnts;
}

void CMapView::DrawLineOnGraphics(Gdiplus::Graphics* graphics, _DrawLine* line, bool project)
{
	auto width = (float)line->width;
	auto color = Utility::OleColor2GdiPlus(line->color, line->alpha);

	Gdiplus::Point* pnts = new Gdiplus::Point[2];
	auto pixX1 = std::lround(line->x1);
	auto pixY1 = std::lround(line->y1);
	if (project)
		PROJECTION_TO_PIXEL_INT(line->x1, line->y1, pixX1, pixY1);

	auto pixX2 = std::lround(line->x2);
	auto pixY2 = std::lround(line->y2);
	if (project)
		PROJECTION_TO_PIXEL_INT(line->x2, line->y2, pixX2, pixY2);
	
	Gdiplus::Pen pen(color, width);
	graphics->DrawLine(&pen, Gdiplus::Point(pixX1, pixY1), Gdiplus::Point(pixX2, pixY2));

	delete[]pnts;
}

void CMapView::DrawPointOnGraphics(Gdiplus::Graphics* graphics, _DrawPoint* point, bool project)
{
	auto color = Utility::OleColor2GdiPlus(point->color, point->alpha);
	auto size = point->size;

	double pixX = point->x;
	double pixY = point->y;
	if (project)
		PROJECTION_TO_PIXEL(pixX, pixY, pixX, pixY);
	auto x = std::lround(pixX - size * 0.5);
	auto y = std::lround(pixY - size * 0.5);

	Gdiplus::SolidBrush brush(color);
	graphics->FillRectangle(&brush, x, y, size, size);
}

#pragma endregion

#pragma region REGION DrawingLayers

// *****************************************************************
//		OnDrawingLayersChanged()
// *****************************************************************
void CMapView::OnDrawingLayersChanged()
{
	ScheduleVolatileRedraw();
	if (!_lockCount)
		InvalidateControl();
}

// *****************************************************************
//		ClearDrawing()
// *****************************************************************
void CMapView::ClearDrawing(long Drawing)
{
	if( Drawing == _currentDrawing )
		_currentDrawing = -1;

	USES_CONVERSION;

	if( IsValidDrawList (Drawing) )
	{
		long endcondition = _activeDrawLists.size();
		for(int i = 0; i < endcondition; i++ )
		{	
			if( _activeDrawLists[i] == Drawing )
			{	
				_activeDrawLists.erase( _activeDrawLists.begin() + i );
				break;
			}
		}
		if( _allDrawLists[Drawing] )
		{
			// it's cleared in destructor I think, but I add it here as well t be sure
			_allDrawLists[Drawing]->m_labels->Clear();
		
			delete _allDrawLists[Drawing];
		}
		_allDrawLists[Drawing] = NULL;

		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

//STAN 30.09.2006
//Label handing for Drawings

// ***************************************************************
//		DrawLabel()
// ***************************************************************
LONG CMapView::DrawLabel(LPCTSTR text, DOUBLE x, DOUBLE y, DOUBLE rotation)
{
	return DrawLabelEx(_currentDrawing, text, x, y, rotation);
}

// ***************************************************************
//		DrawLabelEx()
// ***************************************************************
LONG CMapView::DrawLabelEx(LONG drawHandle, LPCTSTR text, DOUBLE x, DOUBLE y, DOUBLE rotation)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (IsValidDrawList(drawHandle))
	{
		if (_allDrawLists[drawHandle]->m_labels) 
		{
			CComBSTR bstr(text);
			_allDrawLists[drawHandle]->m_labels->AddLabel(bstr, x, y, rotation);
			OnDrawingLayersChanged();
			return drawHandle;
		}
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
	return -1;
}

// *****************************************************************
//		AddDrawingLabel()
// *****************************************************************
void CMapView::AddDrawingLabel(long drawHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification)
{
	if( IsValidDrawList (drawHandle) )
	{	
		if (_allDrawLists[drawHandle]->m_labels) {
			CComBSTR bstr(Text);
			_allDrawLists[drawHandle]->m_labels->AddLabel(bstr, x, y);
		}
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		AddDrawingLabelEx()
// *****************************************************************
void CMapView::AddDrawingLabelEx(long drawHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification, double Rotation)
{
	if( IsValidDrawList(drawHandle) )
	{	
		if (_allDrawLists[drawHandle]->m_labels) {
			CComBSTR bstr(Text);
			_allDrawLists[drawHandle]->m_labels->AddLabel(bstr, x, y, Rotation);
		}
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		ClearDrawingLabels()
// *****************************************************************
void CMapView::ClearDrawingLabels(long drawHandle)
{
	if( IsValidDrawList(drawHandle) )
	{	
		_allDrawLists[drawHandle]->m_labels->Clear();
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
	
}

// *****************************************************************
//		DrawingFont()
// *****************************************************************
void CMapView::DrawingFont(long drawHandle, LPCTSTR FontName, long FontSize)
{
	if( IsValidDrawList(drawHandle) )
	{
		DrawList * dlist = _allDrawLists[drawHandle];
		CComBSTR bstr(FontName);
		dlist->m_labels->put_FontName(bstr);
		dlist->m_labels->put_FontSize(FontSize);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		ClearDrawings()
// *****************************************************************
void CMapView::ClearDrawings()
{	
	_currentDrawing = -1;

	long endcondition = _allDrawLists.size();
	for(int i = 0; i < endcondition; i++ )
	{	
		if( IsValidDrawList(i) )
			ClearDrawing(i);
	}
	_allDrawLists.clear();

	OnDrawingLayersChanged();
}

// *****************************************************************
//		NewDrawing()
// *****************************************************************
long CMapView::NewDrawing(short Projection)
{
	DrawList * dlist = new DrawList();
	dlist->listType = (tkDrawReferenceList)Projection;
	bool inserted = false;
	long drawHandle =  - 1;
	register int i;

	long endcondition = _allDrawLists.size();
	for( i = 0; i < endcondition && !inserted; i++ )
	{	
		if( _allDrawLists[i] == NULL )
		{	
			drawHandle = i;
			_allDrawLists[i] = dlist;
			inserted = true;
		}
	}
	if( inserted == false )
	{	
		drawHandle = _allDrawLists.size();
		_allDrawLists.push_back(dlist);
	}

	_activeDrawLists.push_back( drawHandle );
	_currentDrawing = drawHandle;
	return drawHandle;
}

// *****************************************************************
//		DrawPoint()
// *****************************************************************
void CMapView::DrawPoint(double x, double y, long size, OLE_COLOR color, BYTE alpha)
{
	if( IsValidDrawList(_currentDrawing) )
	{	
		DrawList * dlist = _allDrawLists[_currentDrawing];
		_DrawPoint * dp = new _DrawPoint;
		dp->x = x;
		dp->y = y;
		dp->size = size;
		dp->color = color;
		dp->alpha = alpha;
		dlist->m_dpoints.push_back(dp);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawLine()
// *****************************************************************
void CMapView::DrawLine(double x1, double y1, double x2, double y2, long width, OLE_COLOR color, BYTE alpha)
{
	if( IsValidDrawList(_currentDrawing) )
	{	
		DrawList * dlist = _allDrawLists[_currentDrawing];
		_DrawLine * dl = new _DrawLine;
		dl->x1 = x1;
		dl->x2 = x2;
		dl->y1 = y1;
		dl->y2 = y2;
		dl->width = width;
		dl->color = color;
		dl->alpha = alpha;
		dlist->m_dlines.push_back(dl);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawCircle()
// *****************************************************************
void CMapView::DrawCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill, BYTE alpha)
{
	if( IsValidDrawList(_currentDrawing) )
	{	
		DrawList * dlist = _allDrawLists[_currentDrawing];
		_DrawCircle * dc = new _DrawCircle;
		dc->x = x;
		dc->y = y;
		dc->radius = radius;
		dc->color = color;
		dc->alpha = alpha;
		dc->fill = (fill == TRUE);
		dc->width = 1;
		dlist->m_dcircles.push_back(dc);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***********************************************************
//	  DrawWideCircle()
// ***********************************************************
void CMapView::DrawWideCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill, int width, BYTE alpha)
{
	USES_CONVERSION;

	if( IsValidDrawList(_currentDrawing) )
	{	
		DrawList * dlist = _allDrawLists[_currentDrawing];
		_DrawCircle * dc = new _DrawCircle;
		dc->x = x;
		dc->y = y;
		dc->radius = radius;
		dc->color = color;
		dc->alpha = alpha;
		dc->fill = (fill == TRUE);
		dc->width = width;
		dlist->m_dcircles.push_back(dc);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawPolygon()
// *****************************************************************
void CMapView::DrawPolygon(VARIANT *xPoints,VARIANT *yPoints, long numPoints, OLE_COLOR color, BOOL fill, BYTE alpha)
{
	USES_CONVERSION;
	SAFEARRAY *sax = *xPoints->pparray;
	SAFEARRAY *say = *yPoints->pparray;
	double *xPts = (double *)sax->pvData;
	double *yPts =(double*)say->pvData;

	if( IsValidDrawList( _currentDrawing) )
	{	
		DrawList * dlist = _allDrawLists[_currentDrawing];
		_DrawPolygon * dp = new _DrawPolygon();
		if( numPoints )
		{	dp->xpnts = new double[numPoints];
			dp->ypnts = new double[numPoints];
			register int i;
			for( i = 0; i < numPoints; i++ )
			{	dp->xpnts[i] = xPts[i];
				dp->ypnts[i] = yPts[i];
			}
		}
		dp->numPoints = numPoints;
		dp->color = color;
		dp->alpha = alpha;
		dp->fill = (fill == TRUE);
		dp->width = 1;
		dlist->m_dpolygons.push_back(dp);
    
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***********************************************************
//	  DrawWidePolygon()
// ***********************************************************
void CMapView::DrawWidePolygon(VARIANT *xPoints, VARIANT *yPoints, long numPoints, OLE_COLOR color, BOOL fill, short Width, BYTE alpha)
{
	USES_CONVERSION;
	SAFEARRAY *sax = *xPoints->pparray;
	SAFEARRAY *say = *yPoints->pparray;
	double *xPts = (double *)sax->pvData;
	double *yPts =(double*)say->pvData;

	if( IsValidDrawList(_currentDrawing) )
	{	
		DrawList * dlist = _allDrawLists[_currentDrawing];
		_DrawPolygon * dp = new _DrawPolygon();
		if( numPoints )
		{	dp->xpnts = new double[numPoints];
			dp->ypnts = new double[numPoints];
			register int i;
			for( i = 0; i < numPoints; i++ )
			{	dp->xpnts[i] = xPts[i];
				dp->ypnts[i] = yPts[i];
			}
		}
		dp->numPoints = numPoints;
		dp->color = color;
		dp->alpha = alpha;
		dp->fill = (fill == TRUE);
		dp->width = Width;
		dlist->m_dpolygons.push_back(dp);
		OnDrawingLayersChanged();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawWideCircleEx()
// *****************************************************************
void CMapView::DrawWideCircleEx(LONG LayerHandle, double x, double y, double radius, OLE_COLOR color, VARIANT_BOOL fill, short OutlineWidth, BYTE alpha)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long oldCurrentLayer = this->_currentDrawing;	//Save the current layer for restore
	this->_currentDrawing = LayerHandle;
	this->DrawWideCircle(x, y, radius,color,fill, OutlineWidth, alpha);
	this->_currentDrawing = oldCurrentLayer;		// restore current layer
}

// ***********************************************************
//	  DrawWidePolygonEx()
// ***********************************************************
void CMapView::DrawWidePolygonEx(LONG LayerHandle, VARIANT *xPoints, VARIANT *yPoints, long numPoints, OLE_COLOR color, VARIANT_BOOL fill, short OutlineWidth, BYTE alpha)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long oldCurrentLayer = this->_currentDrawing;	//Save the current layer for restore
	this->_currentDrawing = LayerHandle;
	this->DrawWidePolygon(xPoints,yPoints,numPoints,color,fill, OutlineWidth, alpha);
	this->_currentDrawing = oldCurrentLayer;		// restore current layer
}

// *****************************************************************
//		DrawLineEx()
// *****************************************************************
void CMapView::DrawLineEx(LONG LayerHandle, DOUBLE x1, DOUBLE y1, DOUBLE x2, DOUBLE y2, LONG pixelWidth, OLE_COLOR color, BYTE alpha)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->_currentDrawing;//Save the current layer for restore
	this->_currentDrawing = LayerHandle;
	this->DrawLine(x1, y1, x2, y2, pixelWidth, color, alpha);
	this->_currentDrawing = oldCurrentLayer; // restore current layer

}

// *****************************************************************
//		DrawPointEx()
// *****************************************************************
void CMapView::DrawPointEx(LONG LayerHandle, DOUBLE x, DOUBLE y, LONG pixelSize, OLE_COLOR color, BYTE alpha)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->_currentDrawing;//Save the current layer for restore
	this->_currentDrawing = LayerHandle;
	this->DrawPoint(x,y,pixelSize,color, alpha);
	this->_currentDrawing = oldCurrentLayer; // restore current layer

}

// *****************************************************************
//		DrawCircleEx()
// *****************************************************************
void CMapView::DrawCircleEx(LONG LayerHandle, DOUBLE x, DOUBLE y, DOUBLE pixelRadius, OLE_COLOR color, VARIANT_BOOL fill, BYTE alpha)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->_currentDrawing;//Save the current layer for restore
	this->_currentDrawing = LayerHandle;
	this->DrawCircle(x,y,pixelRadius,color,fill, alpha);
	this->_currentDrawing = oldCurrentLayer; // restore current layer
}

// *****************************************************************
//		DrawPolygonEx()
// *****************************************************************
void CMapView::DrawPolygonEx(LONG LayerHandle, VARIANT* xPoints, VARIANT* yPoints, LONG numPoints, OLE_COLOR color, VARIANT_BOOL fill, BYTE alpha)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->_currentDrawing;//Save the current layer for restore
	this->_currentDrawing = LayerHandle;
	this->DrawPolygon(xPoints,yPoints,numPoints,color,fill, alpha);
	this->_currentDrawing = oldCurrentLayer; // restore current layer
}

// *****************************************************************
//		SetDrawingLayerVisible()
// *****************************************************************
void CMapView::SetDrawingLayerVisible(LONG LayerHandle, VARIANT_BOOL Visible)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!Visible)
	{
		for (unsigned int i = 0; i < _drawingLayerInvisilbe.size(); i++)
		{
			if (_drawingLayerInvisilbe[i] == LayerHandle)
				break;
		}
		//Put the layer into invisible list
		_drawingLayerInvisilbe.push_back(LayerHandle); 

		OnDrawingLayersChanged();
	}
	else
	{
		for (unsigned int i = 0; i < _drawingLayerInvisilbe.size(); i++)
		{
			if (_drawingLayerInvisilbe[i] == LayerHandle)
			{
				//Release the layer from invisible list
				_drawingLayerInvisilbe.erase(_drawingLayerInvisilbe.begin() + i); 
				break;
			}
		}
		OnDrawingLayersChanged();
	}
}

// *****************************************************************
//		GetDrawingKey()
// *****************************************************************
BSTR CMapView::GetDrawingKey(long DrawHandle)
{
	if( IsValidDrawList(DrawHandle) )
	{	
		return OLE2BSTR( _allDrawLists[DrawHandle]->key );
	}
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		CString result;
		return result.AllocSysString();
	}
}

// *****************************************************************
//		SetDrawingKey()
// *****************************************************************
void CMapView::SetDrawingKey(long DrawHandle, LPCTSTR lpszNewValue)
{
	if( IsValidDrawList (DrawHandle) )
	{	
		::SysFreeString(_allDrawLists[DrawHandle]->key);
		_allDrawLists[DrawHandle]->key = A2BSTR(lpszNewValue);
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// **********************************************************
//			GetDrawingLabels()
// **********************************************************
// Access for CLabels class
ILabels* CMapView::GetDrawingLabels(long DrawingLayerIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( IsValidDrawList(DrawingLayerIndex))
	{	
		_allDrawLists[DrawingLayerIndex]->m_labels->AddRef();
		return _allDrawLists[DrawingLayerIndex]->m_labels;
	}
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		return FALSE;
	}
	return  NULL;
}

// **********************************************************
//			GetDrawingLabels()
// **********************************************************
// Setting new CLabels class
void CMapView::SetDrawingLabels(long DrawingLayerIndex, ILabels* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( newVal == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return;
	}
	
	if( IsValidDrawList(DrawingLayerIndex))
	{	
		ComHelper::SetRef((IDispatch*)newVal, (IDispatch**)&_allDrawLists[DrawingLayerIndex]->m_labels, false);
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}
#pragma endregion


