// The old implementation of labels is stored here.
// The old properties are redirected to the new implementation where applicable.

#pragma once
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"

#include <fstream>
#include <vector>
#include <atlsafe.h>
#include "IndexSearching.h"

#include "MapPpg.h"
#include "Enumerations.h"

#include "LabelCategory.h"
#include "Labels.h"
//#include "UtilityFunctions.h"

// *************************************************************
//		GetShapefile()
// *************************************************************
IShapefile* CMapView::GetShapefile(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer * layer = m_allLayers[LayerHandle];
		IShapefile* sf = NULL;
		layer->object->QueryInterface(IID_IShapefile, (void**)&sf);
		if (sf)
		{
			return sf;
		}
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	return NULL;
}

void CMapView::SetShapefile(LONG LayerHandle, IShapefile* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetNotSupported();
}

// *************************************************************
//		GetImage()
// *************************************************************
IImage* CMapView::GetImage(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer * layer = m_allLayers[LayerHandle];
		IImage* img = NULL;
		layer->object->QueryInterface(IID_IImage, (void**)&img);
		if (img)
		{
			return img;
		}
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	return NULL;
}
void CMapView::SetImage(LONG LayerHandle, IImage* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetNotSupported();
}

// *************************************************************
//		GetLayerLabels()
// *************************************************************
ILabels* CMapView::GetLayerLabels(LONG LayerHandle)
{
	if (LayerHandle >= 0 && LayerHandle < (long)m_allLayers.size())
	{
		Layer * layer = m_allLayers[LayerHandle];
		return layer->get_Labels();
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	return NULL;
}

// *************************************************************
//		SetLayerLabels()
// *************************************************************
void CMapView::SetLayerLabels(LONG LayerHandle, ILabels* pVal)
{
	// TODO: Add your property handler code here
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetModifiedFlag();
}

#pragma region Properties

// *************************************************************
//		GetLayerLabelsVisible()
// *************************************************************
BOOL CMapView::GetLayerLabelsVisible(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL visible;
			labels->get_Visible(&visible);
			labels->Release();
			return visible == VARIANT_TRUE?TRUE:FALSE;
		}
		else
			return FALSE;
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{	
			return m_allLayers[LayerHandle]->visibleLabels;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return FALSE;
		}
	}
}

// *************************************************************
//		SetLayerLabelsVisible()
// *************************************************************
void CMapView::SetLayerLabelsVisible(long LayerHandle, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL visible = (bNewValue == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
			labels->put_Visible(visible);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			m_allLayers[LayerHandle]->visibleLabels = bNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// *************************************************************
//		GetLayerLabelsShadow()
// *************************************************************
BOOL CMapView::GetLayerLabelsShadow(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL visible;
			labels->get_HaloVisible(&visible);
			labels->Release();
			return visible == VARIANT_TRUE?TRUE:FALSE;
		}
		else
			return FALSE;
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{	
			return m_allLayers[LayerHandle]->shadowLabels;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return FALSE;
		}
	}
}

// *************************************************************
//		SetLayerLabelsShadow()
// *************************************************************
void CMapView::SetLayerLabelsShadow(long LayerHandle, BOOL newValue)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL visible = (newValue == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
			labels->put_HaloVisible(visible);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			m_allLayers[LayerHandle]->shadowLabels = newValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		GetLayerLabelsScale()
// **********************************************************
BOOL CMapView::GetLayerLabelsScale(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL scale;
			labels->get_ScaleLabels(&scale);
			labels->Release();
			return scale == VARIANT_TRUE?TRUE:FALSE;
		}
		else
			return FALSE;
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{	
			return m_allLayers[LayerHandle]->scaleLabels;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return FALSE;
		}
	}
}

// **********************************************************
//		SetLayerLabelsScale()
// **********************************************************
void CMapView::SetLayerLabelsScale(long LayerHandle, BOOL newValue)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL scale = (newValue == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
			labels->put_ScaleLabels(scale);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			m_allLayers[LayerHandle]->scaleLabels = newValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		GetLayerLabelsOffset()
// **********************************************************
long CMapView::GetLayerLabelsOffset(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			double offset;
			labels->get_OffsetX(&offset);
			labels->Release();
			return (long)offset;
		}
		else
			return 0;
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			return m_allLayers[LayerHandle]->labelsOffset;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return 0;
		}
	}
}

// **********************************************************
//		GetLayerLabelsOffset()
// **********************************************************
void CMapView::SetLayerLabelsOffset(long LayerHandle, long Offset)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			double offset = (double)Offset;
			labels->put_OffsetX(offset);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			m_allLayers[LayerHandle]->labelsOffset = Offset;

			m_canbitblt = FALSE;
			if( !m_lockCount )InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		GetLayerLabelsShadowColor()
// **********************************************************
OLE_COLOR CMapView::GetLayerLabelsShadowColor(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			OLE_COLOR color;
			labels->get_HaloColor(&color);
			labels->Release();
			return color;
		}
		else
			return 0;
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			return m_allLayers[LayerHandle]->shadowColor;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return RGB(255,255,255);
		}
	}
}

// **********************************************************
//		SetLayerLabelsShadowColor()
// **********************************************************
void CMapView::SetLayerLabelsShadowColor(long LayerHandle, OLE_COLOR color)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			labels->put_HaloColor(color);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			m_allLayers[LayerHandle]->shadowColor = color;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		GetMultilineLabeling()
// **********************************************************
bool CMapView::GetMultilineLabeling()
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	// we shall save the value all the same
	return MultilineLabeling?true:false;
}

// **********************************************************
//		SetMultilineLabeling()
// **********************************************************
void CMapView::SetMultilineLabeling(bool Value)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	// we shall save the value all the same
	MultilineLabeling = Value;
}

// **********************************************************
//		GetUseLabelCollision()
// **********************************************************
BOOL CMapView::GetUseLabelCollision(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL value;
			labels->get_AvoidCollisions(&value);
			labels->Release();
			return (value == VARIANT_TRUE)?TRUE:FALSE;
		}
		else
			return FALSE;
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			return m_allLayers[LayerHandle]->useLabelCollision;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return FALSE;
		}
	}
}

// **********************************************************
//		SetUseLabelCollision()
// **********************************************************
void CMapView::SetUseLabelCollision(long LayerHandle, BOOL value)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			VARIANT_BOOL avoid = (value == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
			labels->put_AvoidCollisions(avoid);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			m_allLayers[LayerHandle]->useLabelCollision = value;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			return;
		}

		if(value == TRUE)
		{
			m_useLabelCollision = true;
		}
		else
		{
			bool result = false;
			long endcondition = m_allLayers.size();

			for(unsigned int i = 0; i < m_allLayers.size() && !result; i++)
			{
				if(m_allLayers[i] != NULL)
				{
					if(m_allLayers[i]->useLabelCollision == TRUE)
					{
						result = true;
					}
				}
			}
			endcondition = m_allDrawLists.size();
			for(long i = 0; i < endcondition && !result; i++ )
			{	
				if( m_allDrawLists[i] != NULL ) {
					if( m_allDrawLists[i]->useLabelCollision == TRUE )
					{
						result = true;
					}
				}
			}
			m_useLabelCollision = result;
		}
	}
}

// **********************************************************
//		LayerFont()
// **********************************************************
void CMapView::LayerFont(long LayerHandle, LPCTSTR FontName, long FontSize)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			USES_CONVERSION;
			BSTR name = A2BSTR(FontName);
			labels->put_FontName(name);
			labels->put_FontSize(FontSize);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			Layer * l = m_allLayers[LayerHandle];
			l->fontSize = FontSize;
			l->fontName = FontName;
			
			// We had l->font->DeleteObject() here. This deletes GDI font object alright,
			// but leaves a memory leak. I put this "font replace" function into Layer class instead.
			l->SetFont(new CFont);

			VERIFY(l->GetFont()->CreatePointFont(l->fontSize*10,l->fontName));
			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		LayerFontEx()
// **********************************************************
void CMapView::LayerFontEx(long LayerHandle, LPCTSTR FontName, long FontSize, BOOL isBold, BOOL isItalic, BOOL isUnderline)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			USES_CONVERSION;
			BSTR name = A2BSTR(FontName);
			labels->put_FontName(name);
			labels->put_FontSize(FontSize);
			
			VARIANT_BOOL bold = (isBold == TRUE)? VARIANT_TRUE: VARIANT_FALSE;
			VARIANT_BOOL italic = (isItalic == TRUE)? VARIANT_TRUE: VARIANT_FALSE;
			VARIANT_BOOL underline = (isUnderline == TRUE)? VARIANT_TRUE: VARIANT_FALSE;

			labels->put_FontBold(bold);
			labels->put_FontItalic(italic);
			labels->put_FontUnderline(underline);
			
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			Layer * l = m_allLayers[LayerHandle];
			l->fontSize = FontSize;
			l->fontName = FontName;
			l->isBold = isBold;
			l->isItalic = isItalic;
			l->isUnderline = isUnderline;
			
			// We had l->font->DeleteObject() here. This deletes GDI font object alright,
			// but leaves a memory leak. I put this "font replace" function into Layer class instead.
			//CFont tmpFont;

			l->SetFont(new CFont);

			VERIFY(l->GetFont()->CreatePointFont(l->fontSize*10,l->fontName));
			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		LabelColor()
// **********************************************************
void CMapView::LabelColor(LONG LayerHandle, OLE_COLOR LabelFontColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			USES_CONVERSION;
			labels->put_FontColor(LabelFontColor);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			Layer * l = m_allLayers[LayerHandle];

			int i = 0;
			int endCondition = l->allLabels.size();
			for (i=0; i < endCondition; i++)
			{
				l->allLabels[i].color = LabelFontColor;
			}

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		GetLayerStandardViewWidth()
// **********************************************************
void CMapView::GetLayerStandardViewWidth(long LayerHandle, double * Width)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		*Width = 0.0;
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			*Width = m_allLayers[LayerHandle]->standardViewWidth;
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
			*Width = 0.0;
		}
	}
}

// **********************************************************
//		SetLayerStandardViewWidth()
// **********************************************************
void CMapView::SetLayerStandardViewWidth(long LayerHandle, double Width)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{
			m_allLayers[LayerHandle]->standardViewWidth = Width;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}
#pragma endregion

#pragma region Methods
// **********************************************************
//		AddLabel()
// **********************************************************
void CMapView::AddLabel(long LayerHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			USES_CONVERSION;
			BSTR s = A2BSTR(Text);
			labels->AddLabel(s, x, y);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{	
			LabelData l;
			l.text = Text;
			l.x = x;
			l.y = y;
			l.color = Color;
			l.justif = (tkHJustification)hJustification;

			m_allLayers[LayerHandle]->allLabels.push_back( l );

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		AddLabelEx()
// **********************************************************
void CMapView::AddLabelEx(long LayerHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification, double Rotation)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			USES_CONVERSION;
			BSTR s = A2BSTR(Text);
			labels->AddLabel(s, x, y, Rotation);
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{	
			LabelData l;
			l.text = Text;
			l.x = x;
			l.y = y;
			l.color = Color;
			l.justif = (tkHJustification)hJustification;
			l.rotation = (long)(Rotation*10);

			m_allLayers[LayerHandle]->allLabels.push_back( l );

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}

// **********************************************************
//		ClearLabels()
// **********************************************************
void CMapView::ClearLabels(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ILabels* labels = GetLayerLabels(LayerHandle);
		if (labels)
		{
			labels->Clear();
			labels->Release();
		}
	}
	else
	{
		if( IsValidLayer(LayerHandle) )
		{	
			m_allLayers[LayerHandle]->allLabels.clear();
			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		else
		{	
			ErrorMessage(tkINVALID_LAYER_HANDLE);
		}
	}
}
#pragma endregion

#pragma region Drawing

 /**************************************************************
	  DrawLabels()									  
 **************************************************************/
void CMapView::DrawLabels( CDC * dc, LabelLayer * layer )
{
	if( layer->visibleLabels == FALSE || layer->allLabels.size() <= 0 )
	{
		return;
	}

	if(layer->scaleLabels)
	{
		DrawScaledLabels(dc,layer);
	}
	else
	{
		DrawUnscaledLabels(dc,layer);
	}
}

// *************************************************************
//	  DrawScaledLabels()									  
// *************************************************************
void CMapView::DrawScaledLabels( CDC * dc, LabelLayer * layer )
{
	int dx = 0, dy = 0, outlineWidth = 6;
	double projX1, projY1, projX2, projY2, viewWidth, scaleFactor;
	CFont * labelFont = new CFont();
	CFont * oldFont = NULL;
	CRect rect;
	LOGFONT lf;

	layer->GetFont()->GetLogFont(&lf);

	//Added by LB 5/04 for scaling of labels	
	PixelToProj(0,0,&projX1,&projY1);
	PixelToProj(m_viewWidth,0,&projX2,&projY2);
	viewWidth = projX2 - projX1;

	if(viewWidth < 0)
	{
		viewWidth = viewWidth * -1;
	}
	if(layer->firstTimeLabelsDrawn)
	{
		if(layer->standardViewWidth == 0)
		{
			layer->standardViewWidth = viewWidth;
		}
		layer->firstTimeLabelsDrawn = false;
		//SJA mod to get scaleFactor right
		// http://www.mapwindow.org/phorum/read.php?3,1326,1330#msg-1330
		//scaleFactor = 1;
		scaleFactor = layer->standardViewWidth/viewWidth;
	}
	else
	{
		scaleFactor = layer->standardViewWidth/viewWidth;
	}

	dx = (int)(lf.lfWidth * scaleFactor) - lf.lfWidth; 
	dy = (int)(lf.lfHeight * scaleFactor) - lf.lfHeight; 
	if(dy > 0)
	{
		return;
	}

	lf.lfWidth = lf.lfWidth + dx;
	lf.lfHeight = lf.lfHeight + dy; 
	//ajp (30-11-07)
	lf.lfUnderline = layer->isUnderline;
	lf.lfItalic = layer->isItalic;
	if (layer->isBold)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = 0;

	if(abs(lf.lfHeight) < 10)
	{
		return;
	}

	outlineWidth = lf.lfHeight/20;

	if(abs(outlineWidth) > 4)
	{
		outlineWidth = 4;
	}

	dc->SetBkMode(TRANSPARENT);
	register int i;
	long endcondition = layer->allLabels.size();
	for( i = 0; i < endcondition; i++ )
	{
		LabelData l = layer->allLabels[i];

		//STAN 29.09.2006
		//No reason to do all the calculation if the label is out of the extent
		if( l.x < extents.left  )
			continue;
		else if( l.x > extents.right )
			continue;
		else if( l.y < extents.bottom )
			continue;
		else if( l.y > extents.top )
			continue;

		if( l.text.GetLength() > 0 )
		{
			lf.lfEscapement = l.rotation;
			labelFont->CreatePointFontIndirect(&lf,dc);
			oldFont = dc->SelectObject(labelFont);

			double piX = 0, piY = 0;
			PROJECTION_TO_PIXEL(l.x,l.y,piX,piY);

			piY = piY - layer->labelsOffset;

			// dc->SetPixel(piX,piY,RGB(0,255,0));

			piX = piX + dx/2;

			// dc->SetPixel(piX,piY,RGB(255,0,0));

			rect.left = 0;
			rect.top = 0;
			rect.right = 0;
			rect.bottom = 0;

			dc->SetTextColor(l.color);

			if (MultilineLabeling)
				rect.bottom = dc->DrawText(l.text,rect,DT_CALCRECT|DT_LEFT);
			else
				rect.bottom = dc->DrawText(l.text,rect,DT_CALCRECT|DT_LEFT|DT_BOTTOM|DT_SINGLELINE);

			int temp = rect.Height();
			if(rect.Width() < 4)
			{
				dc->SelectObject(oldFont);
				labelFont->DeleteObject();
				continue;
			}

			OutlineLabelText(dc,l,rect,outlineWidth,layer->shadowColor,false,layer->useLabelCollision == TRUE);

			long height = label_max_y.y - label_min_y.y;
			long width = label_max_x.x - label_min_x.x;
			double text_height = abs(lf.lfHeight/10);
			//Check - Adjust the extents
			UINT formats;			// ajp 5-15-2009
			if( l.justif == hjLeft )
			{
			// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
		formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP; 
				/****Scaled Left Justification Works 22 Feb 2005******/
				long x_offset = 0, y_offset = 0;
				
				if(l.rotation > 0)
				{
					y_offset = height;
				}
				else
				{
					x_offset = (long)text_height;
					y_offset = -1*height;
				}

				rect.left = (long)piX - width + x_offset;
				rect.right = (long)piX - width + x_offset;
				rect.top = (long)(piY + y_offset + text_height);
				rect.bottom = (long)(piY + y_offset + text_height);
				/****End Scaled Left Justification Works***************/

				if( rect.left > m_viewWidth || rect.right < 0 )
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
				else if( rect.bottom > m_viewHeight || rect.top < 0 )
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
			}
			else if( l.justif == hjCenter )
			{
			// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
		formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP; 
				/****Scaled Center Justification Works 22 Feb 2005***************/
				if(l.rotation < 0)
				{
					rect.left = (long)(piX - width * 0.5);
					rect.right = (long)(piX + width * 0.5);
					rect.top = (long)piY;
					rect.bottom = (long)(piY - height * 0.5);
				}
				else
				{
					rect.left = (long)(piX - width * 0.5);
					rect.right = (long)(piX + width * 0.5);
					rect.top = (long)piY;
					rect.bottom = (long)(piY + height * 0.5);
				}
				/****End Scaled Center Justification Works************************/

				if( rect.left > m_viewWidth || rect.right < 0 )
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
				else if( rect.bottom > m_viewHeight || rect.top < 0 )
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
			}
			else if( l.justif == hjRight )
			{
			// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
		formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP; 
				/*******Scaled Right Justification Works 22 Feb 05******/
				rect.left = (long)piX;
				rect.right = (long)(piX + width);
				rect.top = (long)(piY + text_height);
				rect.bottom = (long)(piY + text_height);
				/*******End Scaled Right Justification Works************/

				if( rect.left > m_viewWidth || rect.right < 0 )
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
				else if( rect.bottom > m_viewHeight || rect.top < 0 )
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
			}
			else if( l.justif == hjNone )
			{	
				// tws 6/24/07
				// NB for this option, DO NOT try to adjust the position based on the rotation
				// see http://bugs.mapwindow.org/show_bug.cgi?id=505
				// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
				formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP; 
				rect.left = (long)piX;
				rect.right = (long)(piX + width);
				rect.top = (long)(piY);
				rect.bottom = (long)(piY + height);

				if( rect.left > m_viewWidth)
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
				else if( rect.bottom > m_viewHeight)
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
			}
			else if( l.justif == hjRaw )
			{
				// ajp 5-15-2009
				// N.B rotating labels makes the bounding rectangle much larger, if you use DT_BOTTOM
				// the point gets offset more and more as you rotate towards 45deg, by using DT_TOP
				// the problem disappears.
				rect.left = (long)(piX);
				rect.right = (long)(piX + width);
				rect.top = (long)(piY);
				rect.bottom = (long)(piY + height);

				if( rect.left > m_viewWidth)
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
				else if( rect.bottom > m_viewHeight)
				{
					dc->SelectObject(oldFont);
					labelFont->DeleteObject();
					continue;
				}
			}

			if(layer->shadowLabels)
			{
				OutlineLabelText(dc,l,rect,outlineWidth,layer->shadowColor,true,layer->useLabelCollision == TRUE);
			}
			else
			{
				//ltest apr 2006
				if(m_useLabelCollision)
				{
					OutlineLabelText(dc,l,rect,outlineWidth,layer->shadowColor,false,layer->useLabelCollision == TRUE);
				}
			}

			if(label_DrawLabel)
			{
				if (MultilineLabeling)
					dc->DrawText(l.text,rect,DT_LEFT|DT_NOCLIP);
				else
					dc->DrawText(l.text,rect,formats);
				
				if(m_useLabelCollision)
				{
					Extent bounds;
					bounds.bottom = label_max_y.y;
					bounds.left = label_min_x.x;
					bounds.right = label_max_x.x;
					bounds.top = label_min_y.y;
					m_labelExtentsDrawn.push_back(bounds);
				}
			}

			dc->SelectObject(oldFont);
			labelFont->DeleteObject();
		}//if

	}//for
}

// *************************************************************
//	  DrawUnscaledLabels()									  
// *************************************************************
void CMapView::DrawUnscaledLabels( CDC * dc, LabelLayer * layer )
{
	int outlineWidth = 4;
	CFont labelFont; // We had CFont* here. It leaked memory. We can do what's needed with  alocal object.
	CRect rect;
	LOGFONT lf;

	VERIFY(layer->GetFont()->GetLogFont(&lf) != 0);

	//Unscaled fonts created below were approximately 1/8 of the size they needed to be to match
	//  the font size selected for the font when creating the labelFont from the logical font (lf).
	lf.lfWidth = lf.lfWidth*8;
	lf.lfHeight = lf.lfHeight*8;
	//ajp (30-11-07)
	lf.lfUnderline = layer->isUnderline;
	lf.lfItalic = layer->isItalic;
	if (layer->isBold)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = 0;

	dc->SetBkMode(TRANSPARENT);
	register int i;
	long endcondition = layer->allLabels.size();
	for( i = 0; i < endcondition; i++ )
	{
		LabelData l = layer->allLabels[i];

		//STAN 29.09.2006
		//No reason to do all the calculation if the label is out of the extent
		if( l.x < extents.left  )
			continue;
		else if( l.x > extents.right )
			continue;
		else if( l.y < extents.bottom )
			continue;
		else if( l.y > extents.top )
			continue;

		if( l.text.GetLength() > 0 )
		{
			lf.lfEscapement = l.rotation;
			VERIFY(labelFont.CreatePointFontIndirect(&lf,dc));
			
			/*
			This class will automate the process of DC font selection and font deletion
			(that is, the deletion of font GDI object, not CFont itself).

			Before, we had multiple lines of style: 
			if (SomeCondition)
			{
				dc->SelectObject(oldFont);
				logFont.DeleteObject();
				continue;
			}

			With this, we can leave out multiple explicit Select/DeleteObject calls
			by working C++ ctor/dtor magic with CTempDCFont. It's also exception-safe now.
			*/
			class CTempDCFont
			{
				CDC* m_dc;
				CFont* m_oldFont;
				CFont& m_newFont;
			public:
				CTempDCFont(CDC* dc, CFont& newFont) : m_dc(dc), m_oldFont(dc->SelectObject(&newFont)), m_newFont(newFont) {}
				~CTempDCFont()
				{
					m_dc->SelectObject(m_oldFont);
					m_newFont.DeleteObject();
				}
			};
			CTempDCFont UseThisFont(dc, labelFont);

			double piX = 0, piY = 0;
			PROJECTION_TO_PIXEL(l.x,l.y,piX,piY);

			piY = piY - layer->labelsOffset;

			//dc->SetPixel(piX,piY,RGB(0,255,0));

			rect.left = 0;
			rect.top = 0;
			rect.right = 0;
			rect.bottom = 0;

			dc->SetTextColor(l.color);

			if (MultilineLabeling)
				rect.bottom = dc->DrawText(l.text,(LPRECT)rect,DT_CALCRECT|DT_LEFT);
			else
				rect.bottom = dc->DrawText(l.text,(LPRECT)rect,DT_CALCRECT|DT_LEFT|DT_BOTTOM|DT_SINGLELINE);

			OutlineLabelText(dc,l,rect,outlineWidth,layer->shadowColor,false,layer->useLabelCollision == TRUE);
			
			long height = label_max_y.y - label_min_y.y;
			long width = label_max_x.x - label_min_x.x;
			double text_height = abs(lf.lfHeight/10);
			UINT formats;			// ajp 5-15-2009
			
			//Check - Adjust the extents
			if( l.justif == hjLeft )
			{
				// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
				formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP; 
				
				/****Unscaled Left Justification Works 22 Feb 2005******/
				long x_offset = 0, y_offset = 0;
				
				if(l.rotation > 0)
				{
					y_offset = height;
				}
				else
				{
					x_offset = (long)text_height;
					y_offset = -1*height;
				}

				rect.left = (long)(piX - width + x_offset);
				rect.right = (long)(piX - width + x_offset);
				rect.top = (long)(piY + y_offset + text_height);
				rect.bottom = (long)(piY + y_offset + text_height);
				/****End Unscaled Left Justification Works***************/

				if( rect.left > m_viewWidth || rect.right < 0 )
				{
					// NB: UseThisFont replaces Select/DeleteObject calls we had here before.
					continue;
				}
				else if( rect.bottom > m_viewHeight || rect.top < 0 )
				{
					// NB: UseThisFont replaces Select/DeleteObject calls we had here before.
					continue;
				}
			}
			else if( l.justif == hjCenter )
			{
				// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
				formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP;
				/****Unscaled Center Justification Works 22 Feb 2005***************/
				if(l.rotation < 0)
				{
					rect.left = (long)(piX - width * 0.5);
					rect.right = (long)(piX + width * 0.5);
					rect.top = (long)piY;
					rect.bottom = (long)(piY - height * 0.5);
				}
				else
				{
					rect.left = (long)(piX - width * 0.5);
					rect.right = (long)(piX + width * 0.5);
					rect.top = (long)(piY);
					rect.bottom = (long)(piY + height * 0.5);
				}
				/****End Unscaled Center Justification Works************************/

				if( rect.left > m_viewWidth || rect.right < 0 )
				{
					// NB: UseThisFont replaces Select/DeleteObject calls we had here before.
					continue;
				}
				else if( rect.bottom > m_viewHeight || rect.top < 0 )
				{
					// NB: UseThisFont replaces Select/DeleteObject calls we had here before.
					continue;
				}
			}
			else if( l.justif == hjRight )
			{
				// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
				formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP;
				/****Unscaled Right Justification Works 22 Feb 2005*****/
				rect.left = (long)(piX);
				rect.right = (long)(piX + width);
				rect.top = (long)(piY + text_height);
				rect.bottom = (long)(piY + text_height);
				/****End Unscaled Right Justification Works*************/

				if( rect.left > m_viewWidth || rect.right < 0 )
				{
					// NB: UseThisFont replaces Select/DeleteObject calls we had here before.
					continue;
				}
				else if( rect.bottom > m_viewHeight || rect.top < 0 )
				{
					// NB: UseThisFont replaces Select/DeleteObject calls we had here before.
					continue;
				}
			}
			else if( l.justif == hjNone )
			{	
				// tws 6/24/07
				// NB for this option, DO NOT try to adjust the position based on the rotation
				// see http://bugs.mapwindow.org/show_bug.cgi?id=505
				// ajp 5-15-2009 so as not to break existing code, this value is set to current settings
				formats = DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP;
				rect.left = (long)(piX);
				rect.right = (long)(piX + width);
				rect.top = (long)(piY);
				rect.bottom = (long)(piY + height);

				if( rect.left > m_viewWidth)
				{
					continue;
				}
				else if( rect.bottom > m_viewHeight)
				{
					continue;
				}
			}
			else if( l.justif == hjRaw )
			{
				// ajp 5-15-2009
				// N.B rotating labels makes the bounding rectangle much larger, if you use DT_BOTTOM
				// the point gets offset more and more as you rotate towards 45deg, by using DT_TOP
				// the problem disappears.
				formats = DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOCLIP;
				rect.left = (long)(piX);
				rect.right = (long)(piX + width);
				rect.top = (long)(piY);
				rect.bottom = (long)(piY + height);

				if( rect.left > m_viewWidth)
				{
					continue;
				}
				else if( rect.bottom > m_viewHeight)
				{
					continue;
				}
			}


			if(layer->shadowLabels)
			{
				OutlineLabelText(dc,l,rect,outlineWidth,layer->shadowColor,true,layer->useLabelCollision == TRUE);
			}
			else
			{
				if(m_useLabelCollision)
				{
					OutlineLabelText(dc,l,rect,outlineWidth,layer->shadowColor,false,layer->useLabelCollision == TRUE);
				}
			}

			if(label_DrawLabel)
			{
				if (MultilineLabeling)
					dc->DrawText(l.text,rect,DT_LEFT|DT_NOCLIP);
				else
					dc->DrawText(l.text,rect,formats); // ajp 5-15-2009

				if(m_useLabelCollision)
				{
					//Add bounding box to list of drawn label's bounds
					Extent bounds;
					bounds.bottom = label_max_y.y;
					bounds.left = label_min_x.x;
					bounds.right = label_max_x.x;
					bounds.top = label_min_y.y;
					m_labelExtentsDrawn.push_back(bounds);
				}
			}
			// NB: UseThisFont replaces Select/DeleteObject calls we had here before.
		}
	}
}
#pragma endregion 

#pragma region UtilityFunctions
// ************************************************************
//		LabelsCollisionDetected()							
// ************************************************************
// Testing if new label overlaps with existing labels
bool CMapView::LabelsCollisionDetected()
{
	if(m_useLabelCollision == false)
	{
		return false;
	}

	for(unsigned int i = 0; i < m_labelExtentsDrawn.size(); i++)
	{
		//Test for points lying inside another label's bounds first since it is quick
		if(label_max_y.y <= m_labelExtentsDrawn[i].bottom && label_max_y.y >= m_labelExtentsDrawn[i].top)
		{
			if(label_min_x.x >= m_labelExtentsDrawn[i].left && label_min_x.x <= m_labelExtentsDrawn[i].right)
			{
				return true;
			}
			if(label_max_x.x <= m_labelExtentsDrawn[i].right && label_max_x.x >= m_labelExtentsDrawn[i].left)
			{
				return true;
			}
		}
		//Second test for points lying inside another label's bounds
		if(label_min_y.y >= m_labelExtentsDrawn[i].top && label_min_y.y <= m_labelExtentsDrawn[i].bottom)
		{
			if(label_min_x.x >= m_labelExtentsDrawn[i].left && label_min_x.x <= m_labelExtentsDrawn[i].right)
			{
				return true;
			}
			if(label_max_x.x <= m_labelExtentsDrawn[i].right && label_max_x.x >= m_labelExtentsDrawn[i].left)
			{
				return true;
			}
		}
		//Test to see if drawn label corner points lie inside the proposed label's bounds
		if(m_labelExtentsDrawn[i].bottom <= label_max_y.y && m_labelExtentsDrawn[i].bottom >= label_min_y.y)
		{
			if(m_labelExtentsDrawn[i].left >= label_min_x.x && m_labelExtentsDrawn[i].left <= label_max_x.x)
			{
				return true;
			}
			if(m_labelExtentsDrawn[i].right <= label_max_x.x && m_labelExtentsDrawn[i].right >= label_min_x.x)
			{
				return true;
			}
		}
		//Second test for points of an existing label lying inside the bounds of the proposed label
		if(m_labelExtentsDrawn[i].top >= label_min_y.y && m_labelExtentsDrawn[i].top <= label_max_y.y)
		{
			if(m_labelExtentsDrawn[i].left >= label_min_x.x && m_labelExtentsDrawn[i].left <= label_max_x.x)
			{
				return true;
			}
			if(m_labelExtentsDrawn[i].right <= label_max_x.x && m_labelExtentsDrawn[i].right >= label_min_x.x)
			{
				return true;
			}
		}

		POINT upperLeft, upperRight, lowerRight, lowerLeft;
		upperLeft.x = (long)m_labelExtentsDrawn[i].left;
		upperLeft.y = (long)m_labelExtentsDrawn[i].top;
		upperRight.x =(long) m_labelExtentsDrawn[i].right;
		upperRight.y = (long)m_labelExtentsDrawn[i].top;
		lowerRight.x =(long) m_labelExtentsDrawn[i].right;
		lowerRight.y = (long)m_labelExtentsDrawn[i].bottom;
		lowerLeft.x = (long)m_labelExtentsDrawn[i].left;
		lowerLeft.y = (long)m_labelExtentsDrawn[i].bottom;

		POINT newUpperLeft, newUpperRight, newLowerRight, newLowerLeft;
		newUpperLeft.x = label_min_x.x;
		newUpperLeft.y = label_min_y.y;
		newUpperRight.x = label_max_x.x;
		newUpperRight.y = label_min_y.y;
		newLowerRight.x = label_max_x.x;
		newLowerRight.y = label_max_y.y;
		newLowerLeft.x = label_min_x.x;
		newLowerLeft.y = label_max_y.y;

		if(LineIntersection(newUpperLeft,newUpperRight,upperLeft,upperRight))
		{
			return true;
		}
		if(LineIntersection(newUpperLeft,newUpperRight,upperRight,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newUpperLeft,newUpperRight,lowerLeft,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newUpperLeft,newUpperRight,upperLeft,lowerLeft))
		{
			return true;
		}

		if(LineIntersection(newUpperRight,newLowerRight,upperLeft,upperRight))
		{
			return true;
		}
		if(LineIntersection(newUpperRight,newLowerRight,upperRight,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newUpperRight,newLowerRight,lowerLeft,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newUpperRight,newLowerRight,upperLeft,lowerLeft))
		{
			return true;
		}

		if(LineIntersection(newLowerLeft,newLowerRight,upperLeft,upperRight))
		{
			return true;
		}
		if(LineIntersection(newLowerLeft,newLowerRight,upperRight,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newLowerLeft,newLowerRight,lowerLeft,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newLowerLeft,newLowerRight,upperLeft,lowerLeft))
		{
			return true;
		}

		if(LineIntersection(newUpperLeft,newLowerLeft,upperLeft,upperRight))
		{
			return true;
		}
		if(LineIntersection(newUpperLeft,newLowerLeft,upperRight,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newUpperLeft,newLowerLeft,lowerLeft,lowerRight))
		{
			return true;
		}
		if(LineIntersection(newUpperLeft,newLowerLeft,upperLeft,lowerLeft))
		{
			return true;
		}
	}
	return false;
}

// **********************************************************
//			CMapView::OutlineLabelText()
// **********************************************************
// Drawing outline for the text
void CMapView::OutlineLabelText(CDC * dc, const LabelData& l,CRect& rect, const int& width, const OLE_COLOR& color, const bool& DrawOutline, const bool& layerUseLabelCollision)
{
	dc->BeginPath();
	//dc->DrawText(l.text,rect,DT_LEFT|DT_NOCLIP);
	if (MultilineLabeling)
		dc->DrawText(l.text,rect,DT_LEFT|DT_NOCLIP);
	else
		dc->DrawText(l.text,rect,DT_LEFT|DT_BOTTOM|DT_SINGLELINE|DT_NOCLIP);

	dc->EndPath();

	int numpts;
	numpts= dc->GetPath(NULL, NULL, 0);
	if(numpts > 0)
	{
		LPPOINT lpPoints = new POINT[numpts];
		if (lpPoints == NULL)
		{
			return;
		}
		LPBYTE lpTypes = new BYTE[numpts];
		if (lpTypes == NULL)
		{
			delete [] lpPoints;
			return;
		}

		numpts = dc->GetPath(lpPoints, lpTypes, numpts);

			label_max_x.x = lpPoints[0].x;
			label_max_x.y = lpPoints[0].y;
			label_max_y.x = lpPoints[0].x;
			label_max_y.y = lpPoints[0].y;
			label_min_x.x = lpPoints[0].x;
			label_min_x.y = lpPoints[0].y;
			label_min_y.x = lpPoints[0].x;
			label_min_y.y = lpPoints[0].y;
			for(int i = 1; i < numpts; i++)
			{
				if(lpPoints[i].x > label_max_x.x)
				{
					label_max_x.x = lpPoints[i].x;
				}
				if(lpPoints[i].y > label_max_y.y)
				{
					label_max_y.y = lpPoints[i].y;
				}
				if(lpPoints[i].x < label_min_x.x)
				{
					label_min_x.x = lpPoints[i].x;
				}
				if(lpPoints[i].y < label_min_y.y)
				{
					label_min_y.y = lpPoints[i].y;
				}
			}
			label_max_x.y = label_max_y.y;
			label_min_x.y = label_min_y.y;
			label_max_y.x = label_min_x.x;
			label_min_y.x = label_max_x.x;

		//draw outline
		if(numpts != -1 && (!LabelsCollisionDetected() || !layerUseLabelCollision))
		{
			if(DrawOutline)
			{
				CPen pen(PS_SOLID,width,color);
				CPen * oldPen = dc->SelectObject(&pen);
				myPolyDraw(dc,lpPoints,lpTypes,numpts);
				dc->SelectObject(oldPen);
			}
			label_DrawLabel = true;
		}
		else
		{
			label_DrawLabel = false;
		}

		delete [] lpPoints;
		delete [] lpTypes;

	}
}

// **********************************************************
//			CMapView::myPolyDraw()
// **********************************************************
//This function replaces the CDC::PolyDraw function which isn't supported in Windows 95 and 98
void CMapView::myPolyDraw(CDC* pDC, CONST LPPOINT& lppt, CONST LPBYTE& lpbTypes, const int& cCount)
{
	int nIndex;
	LPPOINT pptLastMoveTo = NULL;

	// for each of the points we have...
	for (nIndex = 0; nIndex < cCount; nIndex++)
	{
		switch(lpbTypes[nIndex])
		{
		case PT_MOVETO:
			if (pptLastMoveTo != NULL && nIndex > 0)
				pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
			pDC->MoveTo(lppt[nIndex].x, lppt[nIndex].y);
			pptLastMoveTo = &lppt[nIndex];
			break;

		case PT_LINETO | PT_CLOSEFIGURE:
			pDC->LineTo(lppt[nIndex].x, lppt[nIndex].y);
			if (pptLastMoveTo != NULL)
				pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
			pptLastMoveTo = NULL;
			break;

		case PT_LINETO:
			pDC->LineTo(lppt[nIndex].x, lppt[nIndex].y);
			break;

		case PT_BEZIERTO | PT_CLOSEFIGURE:
			ASSERT(nIndex + 2 <= cCount);
			pDC->PolyBezierTo(&lppt[nIndex], 3);
			nIndex += 2;
			if (pptLastMoveTo != NULL)
				pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
			pptLastMoveTo = NULL;
			break;

		case PT_BEZIERTO:
			ASSERT(nIndex + 2 <= cCount);
			pDC->PolyBezierTo(&lppt[nIndex], 3);
			nIndex += 2;
			break;
		}
	}

	// If the figure was never closed and should be,
	// close it now.
	if (pptLastMoveTo != NULL && nIndex > 1)
	{
		pDC->LineTo(pptLastMoveTo->x, pptLastMoveTo->y);
	}
}
// *********************************************************
//		createCustomPen()
// *********************************************************
void CMapView::createCustomPen(ShapeLayerInfo * sli, ShapeInfo * si, CPen * newPen, int penWidth, int lineOpt)
{
    /* Chris Michaelis and Michelle Hospodarsky 2-11-2004 */
    /* This function creates a custom Pen for a Custom Line Stipple : gets called from macro LINE_STIPPLE */

	int num;
	DWORD * dashp = Utility::cvtUCharToDword(sli->udLineStipple, num);
	LOGBRUSH logBrush;
	if (lineOpt == 0)
	{
		logBrush.lbStyle = BS_SOLID;
	}
	else
	{
		logBrush.lbHatch = HS_VERTICAL;
		logBrush.lbStyle = BS_HATCHED;
	}

	logBrush.lbStyle = BS_HATCHED;
	logBrush.lbColor = si->lineClr;

	newPen->CreatePen(PS_USERSTYLE|PS_GEOMETRIC, penWidth, &logBrush,num,dashp);
}

// *********************************************************
//		adjustLine()
// *********************************************************
void CMapView::adjustLine(char adjustmentType[], CPoint * pnts, int numPoints, int completedRepeats)
{
  /* Chris Michaelis and Michelle Hospodarsky */
  if (m_LineSeparationFactor == 0) m_LineSeparationFactor = 3;

  if (adjustmentType == "DOUBLESOLID")
  {

    CLine * candidateLines = new CLine[numPoints];
	candidateLines[0].start = pnts[0];
	for (int i = 1; i < numPoints - 1; i++)
	{
		candidateLines[i-1].end = pnts[i];
		candidateLines[i].start = pnts[i];
	}
	candidateLines[numPoints - 2].end = pnts[numPoints - 1];

	for (int i = 0; i < numPoints - 1; i++)
	{
			performAdjustment(m_LineSeparationFactor, completedRepeats, candidateLines[i].start.x, candidateLines[i].start.y, candidateLines[i].end.x, candidateLines[i].end.y);
	}

	if (numPoints > 2)
			for (int i = 0; i < numPoints - 2; i++)  // pnts + 1 as endpt
			{
				CPoint iSectPt;
				iSectPt = findISect(candidateLines[i].start.x, candidateLines[i].start.y, candidateLines[i].end.x, candidateLines[i].end.y, candidateLines[i+1].start.x, candidateLines[i+1].start.y, candidateLines[i+1].end.x, candidateLines[i+1].end.y);
				if (!(iSectPt.x == 0 && iSectPt.y == 0))
				{
					candidateLines[i+1].start = iSectPt;
					candidateLines[i].end = iSectPt;
				}
			}

	pnts[0] = candidateLines[0].start;
	for (int i = 1; i < numPoints - 1; i++)
	{
		pnts[i] = candidateLines[i-1].end;
	}
	pnts[numPoints - 1] = candidateLines[numPoints-2].end;

	delete [] candidateLines;
  }
}

// *********************************************************
//		performAdjustment()
// *********************************************************
void CMapView::performAdjustment(int adjustFactor, int adjUpDown, long &x1, long &y1, long &x2, long &y2)
{
	long distance;
	if (adjUpDown == 0)
		distance = adjustFactor;
	else
		distance = adjustFactor * (-1);

	long newX1, newY1, newX2, newY2;
	double theta;

	theta = CalcLineAngle(x1, y1, x2, y2);

	if (theta == asin(1.0) || theta == asin(-1.0) || theta == 3*asin(1.0))
	{
		if (y1 < y2)
		{
		 x1 += distance;
		 x2 += distance;
		}
		else
		{
		 x1 -= distance;
		 x2 -= distance;
		}
		return;
	}

	if (theta == 0 || (theta >= 3.14 && theta <= 3.15) || (theta <= -3.14 && theta >= -3.15) || (theta >= 3.14 * 2 && theta <= 3.15 * 2))
	{
		y2 -= distance;
		y1 -= distance;
		return;
	}

	newX1 = x1 + (long)(distance * sin(theta));
	newY1 = y1 - (long)(distance * cos(theta));
	newX2 = x2 + (long)(distance * sin(theta));
	newY2 = y2 - (long)(distance * cos(theta));


	x1 = newX1;
	x2 = newX2;
	y1 = newY1;
	y2 = newY2;
}

// *********************************************************
//		CalcLineAngle()
// *********************************************************
double CMapView::CalcLineAngle(long &x1, long &y1, long &x2, long &y2)
{
	double slope, angle, dX, dY;
	dX = x2 - x1;
	dY = y2 - y1;
	if (dX == 0)
	{
		angle = asin(1.0);
		if (dY < 0)
			angle = asin(-1.0);
		return angle;
	}

	slope = dY / dX;
	if (slope >= 0)
	{
		if (dY >= 0)
		{
			if (dY == 0)
			{
				if (dX >= 0)
				{
					angle = 0;
				}
				else
				{
					angle = pi;
				}
			}
			else
			{
				angle = atan(slope);
			}
		}
		else
		{
			angle = pi + atan(slope);
		}
	}
	else
	{
		if (dY < 0)
		{
			angle = (double)2 * pi + atan(slope);
		}
		else
		{
			angle = pi + atan(slope);
		}
	}
	return angle;
}

// *********************************************************
//		findISect()
// *********************************************************
CPoint CMapView::findISect(long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4)
{
	float r = (float)(((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3)));
	float divfactor = (float)(((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3)));

	if (divfactor == 0)
	{
		CPoint iSectPt;
		iSectPt.x = x1 + (x2 - x1);
		iSectPt.y = y1 + (y2 - y1);
		return iSectPt;
	}

	r = r / divfactor;

	float s = (float)(((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y2 - y1)));
	float divfactor2 = (float)(((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3)));
	if (divfactor2 == 0)
	{
		CPoint iSectPt;
		iSectPt.x = x1 + (x2 - x1);
		iSectPt.y = y1 + (y2 - y1);
		return iSectPt;
	}

	s = s / divfactor2;

	CPoint iSectPt;
	iSectPt.x = (long)(x1 + r * (x2 - x1));
	iSectPt.y = (long)(y1 + r * (y2 - y1));

	if (iSectPt.x == 0 && iSectPt.y == 0)
		x2 = x2;

	return iSectPt;
}

// *********************************************************
//		findPerpPoints()
// *********************************************************
void CMapView::findPerpPoints(int adjustFactor, long x1, long y1, long x2, long y2, long &newX, long &newY, long &mirX, long &mirY)
{
	float slope;
	float newB;

	if ((x2 - x1) == 0)
	{
		// Vertical line.
		slope = -1;
		newB = (((-1)/slope)*x1) - y1;
	}
	else
	{
		slope = ((float)(y2-y1))/(x2-x1); // Prevent integer divison
		if (slope == 0)
			slope = 1;
		newB = (((-1)/slope)*x1) - y1;
	}
	double perp_y1 = ((-1)/slope) * x1 - newB;
	double perp_x1 = x1;
	double perp_y2 = ((-1)/slope) * x2 - newB;
	double perp_x2 = x2;

	bool hasbeenLess = false;
	bool hasbeenMore = false;
	double shortenFactor = .3;

	if (sqrt(pow(perp_x2 - perp_x1, 2) + pow(perp_y2 - perp_y1, 2)) < adjustFactor)
	{
		while (sqrt(pow(perp_x2 - perp_x1, 2) + pow(perp_y2 - perp_y1, 2)) < adjustFactor)
		{ // While the length of the perpendicualr line is greater than the adjustment factor, shorten it.
			if (hasbeenLess && hasbeenMore) // Flipping!
			{
				// Reset to original vals
				perp_y1 = ((-1)/slope) * x1 - newB;
				perp_x1 = x1;
				perp_y2 = ((-1)/slope) * x2 - newB;
				perp_x2 = x2;
				// Adjust the factor
				shortenFactor /= 10;
				hasbeenLess = false;
				hasbeenMore = false;
			}
			if (perp_x2 > perp_x1)
			{
				perp_x2 += shortenFactor;
				hasbeenLess = true;
			}
			else
			{
				perp_x2 -= shortenFactor;
				hasbeenMore = true;
			}
			perp_y2 = ((-1)/slope) * perp_x2 - newB;
		}
	}
	else
	{
		while (sqrt(pow(perp_x2 - perp_x1, 2) + pow(perp_y2 - perp_y1, 2)) >= adjustFactor)
		{ // While the length of the perpendicualr line is greater than the adjustment factor, shorten it.
			if ((hasbeenLess == true) && (hasbeenMore == true)) // Flipping!
			{
				// Reset to original vals
				perp_x1 = x1;
				perp_x2 = x2;
				perp_y1 = ((-1)/slope) * x1 - newB;
				perp_y2 = ((-1)/slope) * x2 - newB;
				// Adjust the factor
				shortenFactor /= 10;
				hasbeenLess = false;
				hasbeenMore = false;
			}
			if (perp_x2 < perp_x1)
			{
				perp_x2 += shortenFactor;
				hasbeenLess = true;
			}
			else
			{
				perp_x2 -= shortenFactor;
				hasbeenMore = true;
			}
			perp_y2 = ((-1)/slope) * perp_x2 - newB;
		}
	}
	// Calculate inverse
	long Xdist = (long)abs(perp_x2 - perp_x1);
    double inverse_x2, inverse_y2;
	if (Xdist < 1) // Vertical adjustment only
	{
		long Ydist = (long)abs(perp_y2 - perp_y1);
		if (perp_y2 > perp_y1)
		{
			inverse_y2 = perp_y2 - Ydist * 2;
		}
		else
		{
			inverse_y2 = perp_y2 + Ydist * 2;
		}
		inverse_x2 = perp_x2; // (slope * (-1)) * (inverse_y2 + newB);
	}
	else
	{
		if (perp_x2 > perp_x1)
		{
			inverse_x2 = perp_x2 - Xdist * 2;
		}
		else
		{
			inverse_x2 = perp_x2 + Xdist * 2;
		}
		inverse_y2 = ((-1)/slope) * inverse_x2 - newB;
	}
	newX = (int)floor(perp_x2 + 0.5);
	newY = (int)floor(perp_y2 + 0.5);
	mirX = (int)floor(inverse_x2 + 0.5);
	mirY = (int)floor(inverse_y2 + 0.5);
}
#pragma endregion

