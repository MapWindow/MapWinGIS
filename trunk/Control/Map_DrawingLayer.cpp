// Implementation of CMapView class (see other files as well)
// Drawing layer functionality is stored here
// TODO: this code should be wrapped in a separate class

#pragma once
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"
#include <vector>
#include "Enumerations.h"
#include "LabelDrawing.h"

// ***************************************************************
//		IsValidDrawList()
// ***************************************************************
bool CMapView::IsValidDrawList(long listHandle)
{
	if (listHandle >= 0 && listHandle < (long)m_allDrawLists.size())
	{
		return m_allDrawLists[listHandle]!=NULL;
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
	else
	{	
		ErrorMessage(tkINVALID_DRAW_HANDLE);
		return 0;
	}
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
	else
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
		m_allDrawLists[drawHandle]->m_labels->get_HaloColor(&clr);
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
		m_allDrawLists[drawHandle]->m_labels->put_HaloColor(color);
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
		m_allDrawLists[drawHandle]->m_labels->put_HaloVisible(newValue? VARIANT_TRUE : VARIANT_FALSE);
		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
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
		m_allDrawLists[drawHandle]->m_labels->get_HaloVisible(&visible);
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
		m_allDrawLists[drawHandle]->m_labels->put_ScaleLabels(newValue? VARIANT_TRUE : VARIANT_FALSE);
		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
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
		m_allDrawLists[drawHandle]->m_labels->get_ScaleLabels(&scale);
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
		m_allDrawLists[drawHandle]->m_labels->get_AvoidCollisions(&val);
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
		m_allDrawLists[drawHandle]->m_labels->put_AvoidCollisions(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
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
		m_allDrawLists[drawHandle]->m_labels->get_Visible(&visible);
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
		m_allDrawLists[drawHandle]->m_labels->put_Visible(bNewValue ? VARIANT_TRUE : VARIANT_FALSE);
		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***************************************************************
//		GetDrawingStandardViewWidth()
// ***************************************************************
void CMapView::GetDrawingStandardViewWidth(long drawHandle, double * Width)
{
	if( IsValidDrawList(drawHandle) )
	{
		*Width = 0.0;
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
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
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}
#pragma endregion

#pragma region DrawingLayer

// ******************************************************************
//		Draw the DrawLayers
// ******************************************************************
void CMapView::DrawLists(const CRect & rcBounds, Gdiplus::Graphics* graphics, tkDrawReferenceList listType)
{
	// TODO: implement proper collision list for drawing labels; frames for drawing labels should be cleared from list while frames from the rest layers should remain 
	// in case Map.Refresh() was called
	CCollisionList collisionList;

	// label drawer for spatially referenced list
	CLabelDrawer lblDrawer(graphics, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, 
		this->GetCurrentScale(), &collisionList, m_RotateAngle, m_isSnapshot );
	
	// label drawer for screen referenced list
	Extent ext(rcBounds.left, rcBounds.right, rcBounds.top, rcBounds.bottom);
	CLabelDrawer lblDrawerScreen(graphics, &ext, &collisionList, m_RotateAngle );

	std::vector<ILabels*> topLabels;	// labels that should be drawn above all layers
	std::vector<tkDrawReferenceList> labelsType;
	
	for(unsigned int j = 0; j < m_activeDrawLists.size(); j++ )
	{
		bool isSkip = false;
		for (unsigned int i = 0; i < DrawingLayerInVisilbe.size(); i++)
		{
			if (DrawingLayerInVisilbe[i] == j)
			{
				isSkip = true;	// skip if this layer is set invisiable
				break;  
			}
		}
		if(isSkip) 
		{
			continue;
		}

		DrawList * dlist = m_allDrawLists[m_activeDrawLists[j]];
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
	HDC hdc = graphics->GetHDC();
	CDC* dc = CDC::FromHandle(hdc);
	
	CBrush * brush = new CBrush();
	CPen * pen = new CPen();

	CBrush * oldBrush = NULL;
	CPen * oldPen = NULL;

	CRect rect;

	double pixX, pixY;

	register int j;
	
	if( dlist->listType == dlScreenReferencedList )
	{
		for(unsigned i = 0; i < dlist->m_dpolygons.size(); i++)
		{	
			if( dlist->m_dpolygons[i]->numPoints > 0 )
			{	
				CPoint * pnts = new CPoint[dlist->m_dpolygons[i]->numPoints];
				long endcondition2 = dlist->m_dpolygons[i]->numPoints;
				for(j=0;j<endcondition2;j++)
				{
					//Rob Cairns 10 Jan 06
					int xpt = Utility::Rint(dlist->m_dpolygons[i]->xpnts[j]);
					int ypt = Utility::Rint(dlist->m_dpolygons[i]->ypnts[j]);
					pnts[j] = CPoint(xpt,ypt);
				}
				if( dlist->m_dpolygons[i]->fill )
				{	
					brush->CreateSolidBrush(dlist->m_dpolygons[i]->color);
					pen->CreatePen(PS_SOLID,dlist->m_dpolygons[i]->width,dlist->m_dpolygons[i]->color);
					oldBrush = dc->SelectObject(brush);
					oldPen = dc->SelectObject(pen);
					dc->Polygon(pnts,dlist->m_dpolygons[i]->numPoints);
					dc->SelectObject(oldBrush);
					dc->SelectObject(oldPen);
					pen->DeleteObject();
					brush->DeleteObject();
				}
				else
				{	
					pen->CreatePen(PS_SOLID,dlist->m_dpolygons[i]->width,dlist->m_dpolygons[i]->color);
					oldPen = dc->SelectObject(pen);
					dc->Polyline(pnts,dlist->m_dpolygons[i]->numPoints);
					dc->SelectObject(oldPen);
					pen->DeleteObject();
				}
				delete [] pnts;
			}
		}
		
		for(unsigned int i = 0; i < dlist->m_dcircles.size(); i++)
		{	
			if( dlist->m_dcircles[i]->fill )
			{	
				brush->CreateSolidBrush(dlist->m_dcircles[i]->color);
				pen->CreatePen(PS_SOLID,dlist->m_dcircles[i]->width,dlist->m_dcircles[i]->color);
				oldBrush = dc->SelectObject(brush);
				oldPen = dc->SelectObject(pen);
				dc->Ellipse((int)(dlist->m_dcircles[i]->x - dlist->m_dcircles[i]->radius),
							(int)(dlist->m_dcircles[i]->y - dlist->m_dcircles[i]->radius), 
							(int)(dlist->m_dcircles[i]->x + dlist->m_dcircles[i]->radius), 
							(int)(dlist->m_dcircles[i]->y + dlist->m_dcircles[i]->radius) );
				dc->SelectObject(oldBrush);
				dc->SelectObject(oldPen);
				pen->DeleteObject();
				brush->DeleteObject();
			}
			else
			{	
				pen->CreatePen(PS_SOLID,dlist->m_dcircles[i]->width,dlist->m_dcircles[i]->color);
				oldPen = dc->SelectObject(pen);
				dc->MoveTo((int)(dlist->m_dcircles[i]->x+dlist->m_dcircles[i]->radius), (int)dlist->m_dcircles[i]->y);
				dc->AngleArc((int)(dlist->m_dcircles[i]->x), (int)(dlist->m_dcircles[i]->y), (int)(dlist->m_dcircles[i]->radius),0,360);
				dc->SelectObject(oldPen);
				pen->DeleteObject();
			}
		}
		
		for(unsigned int i = 0 ; i < dlist->m_dlines.size(); i++)
		{	
			CPoint *pnts = new CPoint[2];
			pen->CreatePen(PS_SOLID,dlist->m_dlines[i]->width,dlist->m_dlines[i]->color);
			oldPen = dc->SelectObject(pen);
			//Rob Cairns 10 Jan 06
			int ptX1 = Utility::Rint(dlist->m_dlines[i]->x1);
			int ptY1 = Utility::Rint(dlist->m_dlines[i]->y1);	
			int ptX2 = Utility::Rint(dlist->m_dlines[i]->x2);
			int ptY2 = Utility::Rint(dlist->m_dlines[i]->y2);
			pnts[0]=CPoint(ptX1,ptY1);
			pnts[1]=CPoint(ptX2,ptY2);
			dc->Polyline(pnts,2);
			dc->SelectObject(oldPen);
			pen->DeleteObject();
			delete []pnts;
		}
		
		
		for(unsigned int i = 0;i < dlist->m_dpoints.size(); i++) 
		{	if( dlist->m_dpoints[i]->size == 1 )
			{	//Rob Cairns 10 Jan 06
				int pxX = Utility::Rint(dlist->m_dpoints[i]->x);
				int pxY = Utility::Rint(dlist->m_dpoints[i]->y);
				dc->SetPixelV(CPoint(pxX,pxY),dlist->m_dpoints[i]->color);
			}	
			else
			{	
				rect = CRect((int)dlist->m_dpoints[i]->x,(int)dlist->m_dpoints[i]->y, (int)dlist->m_dpoints[i]->x, (int)dlist->m_dpoints[i]->y);
				rect.left -= (long)(dlist->m_dpoints[i]->size * 0.5);
				rect.top -= (long)(dlist->m_dpoints[i]->size * 0.5);
				dc->FillSolidRect(rect.left,rect.top,dlist->m_dpoints[i]->size,dlist->m_dpoints[i]->size,dlist->m_dpoints[i]->color);
			}
		}
	}
	else	// if( dlist->listType == dlSpatiallyReferencedList )
	{
		for(unsigned int i = 0; i < dlist->m_dpolygons.size(); i++)
		{	
			if( dlist->m_dpolygons[i]->numPoints > 0 )
			{	
				CPoint * pnts = new CPoint[dlist->m_dpolygons[i]->numPoints];
				long endcondition2 = dlist->m_dpolygons[i]->numPoints;
				for(j=0;j<endcondition2;j++)
				{	
					PROJECTION_TO_PIXEL(dlist->m_dpolygons[i]->xpnts[j],dlist->m_dpolygons[i]->ypnts[j],pixX,pixY);
					//Rob Cairns 10 Jan 06
					int pxX = Utility::Rint(pixX);
					int pxY = Utility::Rint(pixY);	
					pnts[j] = CPoint(pxX,pxY);
				}
				
				if( dlist->m_dpolygons[i]->fill )
				{	
					brush->CreateSolidBrush(dlist->m_dpolygons[i]->color);
					pen->CreatePen(PS_SOLID,dlist->m_dpolygons[i]->width,dlist->m_dpolygons[i]->color);
					oldBrush = dc->SelectObject(brush);
					oldPen = dc->SelectObject(pen);
					dc->Polygon(pnts,dlist->m_dpolygons[i]->numPoints);
					dc->SelectObject(oldBrush);
					dc->SelectObject(oldPen);
					pen->DeleteObject();
					brush->DeleteObject();
				}
				else
				{	
					pen->CreatePen(PS_SOLID,dlist->m_dpolygons[i]->width,dlist->m_dpolygons[i]->color);
					oldPen = dc->SelectObject(pen);
					dc->Polyline(pnts,dlist->m_dpolygons[i]->numPoints);
					dc->SelectObject(oldPen);
					pen->DeleteObject();
				}
				delete [] pnts;
			}
		}
		
		for(unsigned int i = 0; i < dlist->m_dcircles.size(); i++)
		{	
			if( dlist->m_dcircles[i]->fill )
			{	
				brush->CreateSolidBrush(dlist->m_dcircles[i]->color);
				pen->CreatePen(PS_SOLID,dlist->m_dcircles[i]->width,dlist->m_dcircles[i]->color);
				oldBrush = dc->SelectObject(brush);
				oldPen = dc->SelectObject(pen);
				PROJECTION_TO_PIXEL(dlist->m_dcircles[i]->x,dlist->m_dcircles[i]->y,pixX,pixY);
				
				// lsu 25-apr-2010: we need to draw it in screen coordinates
				int pxX = Utility::Rint(pixX - dlist->m_dcircles[i]->radius * m_pixelPerProjectionX);
				int pxY = Utility::Rint(pixY - dlist->m_dcircles[i]->radius * m_pixelPerProjectionY);
				int pxX2 = Utility::Rint(pixX + dlist->m_dcircles[i]->radius * m_pixelPerProjectionX);
				int pxY2 = Utility::Rint(pixY + dlist->m_dcircles[i]->radius * m_pixelPerProjectionY);
				
				dc->Ellipse( pxX, pxY, pxX2, pxY2);
				dc->SelectObject(oldBrush);
				dc->SelectObject(oldPen);
				brush->DeleteObject();
			}
			else
			{	pen->CreatePen(PS_SOLID, dlist->m_dcircles[i]->width,dlist->m_dcircles[i]->color);
				oldPen = dc->SelectObject(pen);
				PROJECTION_TO_PIXEL(dlist->m_dcircles[i]->x,dlist->m_dcircles[i]->y,pixX,pixY);
				//Rob Cairns 10 Jan 06
				int pxX = Utility::Rint(pixX);
				int pxY = Utility::Rint(pixY);
				dc->MoveTo((int)(pxX + dlist->m_dcircles[i]->radius * m_pixelPerProjectionX),pxY);
				dc->AngleArc(pxX, pxY, (int)(dlist->m_dcircles[i]->radius * m_pixelPerProjectionX), 0, 360);
				dc->SelectObject(oldPen);
			}
			pen->DeleteObject();
		}
		
		for(unsigned int i=0; i < dlist->m_dlines.size(); i++)
		{	
			CPoint *pnts = new CPoint[2];
			pen->CreatePen(PS_SOLID,dlist->m_dlines[i]->width,dlist->m_dlines[i]->color);
			oldPen = dc->SelectObject(pen);
			PROJECTION_TO_PIXEL(dlist->m_dlines[i]->x1,dlist->m_dlines[i]->y1,pixX,pixY);
			//Rob Cairns 10 Jan 06
			int pxX = Utility::Rint(pixX);
			int pxY = Utility::Rint(pixY);
			pnts[0]=CPoint(pxX,pxY);
			PROJECTION_TO_PIXEL(dlist->m_dlines[i]->x2,dlist->m_dlines[i]->y2,pixX,pixY);
			//Rob Cairns 10 Jan 06
			pxX = Utility::Rint(pixX);
			pxY = Utility::Rint(pixY);
			pnts[1]=CPoint(pxX,pxY);
			dc->Polyline(pnts,2);
			dc->SelectObject(oldPen);
			pen->DeleteObject();
			delete []pnts;
		}
		
		for(unsigned int i=0; i < dlist->m_dpoints.size(); i++)
		{	
			if( dlist->m_dpoints[i]->size == 1 )
			{
				PROJECTION_TO_PIXEL(dlist->m_dpoints[i]->x,dlist->m_dpoints[i]->y,pixX,pixY);
				//Rob Cairns 10 Jan 06
				int pxX = Utility::Rint(pixX);
				int pxY = Utility::Rint(pixY);
				dc->SetPixelV(CPoint(pxX,pxY),dlist->m_dpoints[i]->color);
			}
			else
			{
				PROJECTION_TO_PIXEL(dlist->m_dpoints[i]->x,dlist->m_dpoints[i]->y,pixX,pixY);
				pixX -= dlist->m_dpoints[i]->size*.5;
				pixY -= dlist->m_dpoints[i]->size*.5;
				//Rob Cairns 10 Jan 06
				int pxX = Utility::Rint(pixX);
				int pxY = Utility::Rint(pixY);
				dc->FillSolidRect(pxX,pxY,dlist->m_dpoints[i]->size,dlist->m_dpoints[i]->size,dlist->m_dpoints[i]->color);
			}
		}
	}

	delete brush;
	delete pen;

	graphics->ReleaseHDC(hdc);
	dc = NULL;
}

#pragma endregion

#pragma region REGION DrawingLayers

// *****************************************************************
//		ClearDrawing()
// *****************************************************************
void CMapView::ClearDrawing(long Drawing)
{
	if( Drawing == m_currentDrawing )
		m_currentDrawing = -1;

	USES_CONVERSION;

	if( IsValidDrawList (Drawing) )
	{
		long endcondition = m_activeDrawLists.size();
		for(int i = 0; i < endcondition; i++ )
		{	
			if( m_activeDrawLists[i] == Drawing )
			{	
				m_activeDrawLists.erase( m_activeDrawLists.begin() + i );
				break;
			}
		}
		if( m_allDrawLists[Drawing] )
		{
			if (m_allDrawLists[Drawing]->listType == dlSpatiallyReferencedList)
				m_canbitblt = FALSE;

			// lsu: it's cleared in destructor I think, but I add it here as well t be sure
			m_allDrawLists[Drawing]->m_labels->Clear();
		
			delete m_allDrawLists[Drawing];
		}
		m_allDrawLists[Drawing] = NULL;

		if( !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

//STAN 30.09.2006
//Label handing for Drawings

// *****************************************************************
//		AddDrawingLabel()
// *****************************************************************
void CMapView::AddDrawingLabel(long drawHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification)
{
	if( IsValidDrawList (drawHandle) )
	{	
		if (m_allDrawLists[drawHandle]->m_labels)
			m_allDrawLists[drawHandle]->m_labels->AddLabel(A2BSTR(Text), x, y);
		
		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
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
		if (m_allDrawLists[drawHandle]->m_labels)
			m_allDrawLists[drawHandle]->m_labels->AddLabel(A2BSTR(Text), x, y, Rotation );

		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
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
		m_allDrawLists[drawHandle]->m_labels->Clear();

		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
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
		DrawList * dlist = m_allDrawLists[drawHandle];
		dlist->m_labels->put_FontName(A2BSTR(FontName));
		dlist->m_labels->put_FontSize(FontSize);
		if (dlist->listType == dlSpatiallyReferencedList)
		{
			m_canbitblt = FALSE;
		}
		if( !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		ClearDrawings()
// *****************************************************************
void CMapView::ClearDrawings()
{	
	m_currentDrawing = -1;

	long endcondition = m_allDrawLists.size();
	for(int i = 0; i < endcondition; i++ )
	{	
		if( IsValidDrawList(i) )
			ClearDrawing(i);
	}
	m_allDrawLists.clear();

	m_canbitblt = FALSE;
	if( !m_lockCount )
			InvalidateControl();
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

	long endcondition = m_allDrawLists.size();
	for( i = 0; i < endcondition && !inserted; i++ )
	{	
		if( m_allDrawLists[i] == NULL )
		{	
			drawHandle = i;
			m_allDrawLists[i] = dlist;
			inserted = true;
		}
	}
	if( inserted == false )
	{	
		drawHandle = m_allDrawLists.size();
		m_allDrawLists.push_back(dlist);
	}

	m_activeDrawLists.push_back( drawHandle );
	m_currentDrawing = drawHandle;
	return drawHandle;
}

// *****************************************************************
//		DrawPoint()
// *****************************************************************
void CMapView::DrawPoint(double x, double y, long size, OLE_COLOR color)
{
	if( IsValidDrawList(m_currentDrawing) )
	{	
		DrawList * dlist = m_allDrawLists[m_currentDrawing];
		_DrawPoint * dp = new _DrawPoint;
		dp->x = x;
		dp->y = y;
		dp->size = size;
		dp->color = color;
		dlist->m_dpoints.push_back(dp);

		if (dlist->listType == dlSpatiallyReferencedList)
			m_canbitblt = FALSE;

		if( !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawLine()
// *****************************************************************
void CMapView::DrawLine(double x1, double y1, double x2, double y2, long width, OLE_COLOR color)
{
	if( IsValidDrawList(m_currentDrawing) )
	{	
		DrawList * dlist = m_allDrawLists[m_currentDrawing];
		_DrawLine * dl = new _DrawLine;
		dl->x1 = x1;
		dl->x2 = x2;
		dl->y1 = y1;
		dl->y2 = y2;
		dl->width = width;
		dl->color = color;
		dlist->m_dlines.push_back(dl);

		if (dlist->listType == dlSpatiallyReferencedList)
			m_canbitblt = FALSE;

		if( !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawCircle()
// *****************************************************************
void CMapView::DrawCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill)
{
	if( IsValidDrawList(m_currentDrawing) )
	{	
		DrawList * dlist = m_allDrawLists[m_currentDrawing];
		_DrawCircle * dc = new _DrawCircle;
		dc->x = x;
		dc->y = y;
		dc->radius = radius;
		dc->color = color;
		dc->fill = (fill == TRUE);
		dc->width = 1;
		dlist->m_dcircles.push_back(dc);

		if (dlist->listType == dlSpatiallyReferencedList)
			m_canbitblt = FALSE;

		if( !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***********************************************************
//	  DrawWideCircle()
// ***********************************************************
void CMapView::DrawWideCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill, int width)
{
	USES_CONVERSION;

	if( IsValidDrawList(m_currentDrawing) )
	{	
		DrawList * dlist = m_allDrawLists[m_currentDrawing];
		_DrawCircle * dc = new _DrawCircle;
		dc->x = x;
		dc->y = y;
		dc->radius = radius;
		dc->color = color;
		dc->fill = (fill == TRUE);
		dc->width = width;
		dlist->m_dcircles.push_back(dc);

		if (dlist->listType == dlSpatiallyReferencedList)
			m_canbitblt = FALSE;

		if( !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawPolygon()
// *****************************************************************
void CMapView::DrawPolygon(VARIANT *xPoints,VARIANT *yPoints, long numPoints, OLE_COLOR color, BOOL fill)
{
	USES_CONVERSION;
	SAFEARRAY *sax = *xPoints->pparray;
	SAFEARRAY *say = *yPoints->pparray;
	double *xPts = (double *)sax->pvData;
	double *yPts =(double*)say->pvData;

	if( IsValidDrawList( m_currentDrawing) )
	{	
		DrawList * dlist = m_allDrawLists[m_currentDrawing];
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
		dp->fill = (fill == TRUE);
		dp->width = 1;
		dlist->m_dpolygons.push_back(dp);
    
		//if (dlist->listType == dlSpatiallyReferencedList)
		//	m_canbitblt = FALSE;
		
		if(  !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// ***********************************************************
//	  DrawWidePolygon()
// ***********************************************************
void CMapView::DrawWidePolygon(VARIANT *xPoints, VARIANT *yPoints, long numPoints, OLE_COLOR color, BOOL fill, short Width)
{
	USES_CONVERSION;
	SAFEARRAY *sax = *xPoints->pparray;
	SAFEARRAY *say = *yPoints->pparray;
	double *xPts = (double *)sax->pvData;
	double *yPts =(double*)say->pvData;

	if( IsValidDrawList(m_currentDrawing) )
	{	
		DrawList * dlist = m_allDrawLists[m_currentDrawing];
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
		dp->fill = (fill == TRUE);
		dp->width = Width;
		dlist->m_dpolygons.push_back(dp);

		if (dlist->listType == dlSpatiallyReferencedList)
			m_canbitblt = FALSE;

		if(  !m_lockCount )
			InvalidateControl();
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// *****************************************************************
//		DrawWideCircleEx()
// *****************************************************************
void CMapView::DrawWideCircleEx(LONG LayerHandle, double x, double y, double radius, OLE_COLOR color, VARIANT_BOOL fill, short OutlineWidth)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long oldCurrentLayer = this->m_currentDrawing;	//Save the current layer for resotre
	this->m_currentDrawing = LayerHandle;
	this->DrawWideCircle(x, y, radius,color,fill, OutlineWidth);
	this->m_currentDrawing = oldCurrentLayer;		// restore current layer
}

// ***********************************************************
//	  DrawWidePolygonEx()
// ***********************************************************
void CMapView::DrawWidePolygonEx(LONG LayerHandle, VARIANT *xPoints, VARIANT *yPoints, long numPoints, OLE_COLOR color, VARIANT_BOOL fill, short OutlineWidth)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long oldCurrentLayer = this->m_currentDrawing;	//Save the current layer for resotre
	this->m_currentDrawing = LayerHandle;
	this->DrawWidePolygon(xPoints,yPoints,numPoints,color,fill, OutlineWidth);
	this->m_currentDrawing = oldCurrentLayer;		// restore current layer
}

// *****************************************************************
//		DrawLineEx()
// *****************************************************************
void CMapView::DrawLineEx(LONG LayerHandle, DOUBLE x1, DOUBLE y1, DOUBLE x2, DOUBLE y2, LONG pixelWidth, OLE_COLOR color)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->m_currentDrawing;//Save the current layer for resotre
	this->m_currentDrawing = LayerHandle;
	this->DrawLine(x1,y1,x2,y2,pixelWidth,color);
	this->m_currentDrawing = oldCurrentLayer; // restore current layer

}

// *****************************************************************
//		DrawPointEx()
// *****************************************************************
void CMapView::DrawPointEx(LONG LayerHandle, DOUBLE x, DOUBLE y, LONG pixelSize, OLE_COLOR color)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->m_currentDrawing;//Save the current layer for resotre
	this->m_currentDrawing = LayerHandle;
	this->DrawPoint(x,y,pixelSize,color);
	this->m_currentDrawing = oldCurrentLayer; // restore current layer

}

// *****************************************************************
//		DrawCircleEx()
// *****************************************************************
void CMapView::DrawCircleEx(LONG LayerHandle, DOUBLE x, DOUBLE y, DOUBLE pixelRadius, OLE_COLOR color, VARIANT_BOOL fill)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->m_currentDrawing;//Save the current layer for resotre
	this->m_currentDrawing = LayerHandle;
	this->DrawCircle(x,y,pixelRadius,color,fill);
	this->m_currentDrawing = oldCurrentLayer; // restore current layer
}

// *****************************************************************
//		DrawPolygonEx()
// *****************************************************************
void CMapView::DrawPolygonEx(LONG LayerHandle, VARIANT* xPoints, VARIANT* yPoints, LONG numPoints, OLE_COLOR color, VARIANT_BOOL fill)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long oldCurrentLayer = this->m_currentDrawing;//Save the current layer for resotre
	this->m_currentDrawing = LayerHandle;
	this->DrawPolygon(xPoints,yPoints,numPoints,color,fill);
	this->m_currentDrawing = oldCurrentLayer; // restore current layer
}

// *****************************************************************
//		SetDrawingLayerVisible()
// *****************************************************************
void CMapView::SetDrawingLayerVisible(LONG LayerHandle, VARIANT_BOOL Visiable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(!Visiable)
	{
		for (unsigned int i = 0; i < DrawingLayerInVisilbe.size(); i++)
		{
			if (DrawingLayerInVisilbe[i] == LayerHandle)
				break;
		}
		//Put the layer into invisiable list
		DrawingLayerInVisilbe.push_back(LayerHandle); 
	}
	else
	{
		for (unsigned int i = 0; i < DrawingLayerInVisilbe.size(); i++)
		{
			if (DrawingLayerInVisilbe[i] == LayerHandle)
			{
				//Release the layer from invisiable list
				DrawingLayerInVisilbe.erase(DrawingLayerInVisilbe.begin() + i); 
				break;
			}
		}
	}
}

// *****************************************************************
//		GetDrawingKey()
// *****************************************************************
BSTR CMapView::GetDrawingKey(long DrawHandle)
{
	if( IsValidDrawList(DrawHandle) )
	{	
		return OLE2BSTR( m_allDrawLists[DrawHandle]->key );
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
		::SysFreeString(m_allDrawLists[DrawHandle]->key);
		m_allDrawLists[DrawHandle]->key = A2BSTR(lpszNewValue);
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}

// **********************************************************
//			GetDrawingLabels()
// **********************************************************
// Access for CLabels class
//LPDISPATCH CMapView::GetDrawingLabels(long DrawingLayerIndex)
ILabels* CMapView::GetDrawingLabels(long DrawingLayerIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( IsValidDrawList(DrawingLayerIndex))
	{	
		m_allDrawLists[DrawingLayerIndex]->m_labels->AddRef();
		return m_allDrawLists[DrawingLayerIndex]->m_labels;
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
void CMapView::SetDrawingLabels(long DrawingLayerIndex, ILabels* newVal) //LPDISPATCH newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( newVal == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return;
	}
	
	if( IsValidDrawList(DrawingLayerIndex))
	{	
		Utility::put_ComReference((IDispatch*)newVal, (IDispatch**)&m_allDrawLists[DrawingLayerIndex]->m_labels, false);
	}
	else
		ErrorMessage(tkINVALID_DRAW_HANDLE);
}
#pragma endregion

