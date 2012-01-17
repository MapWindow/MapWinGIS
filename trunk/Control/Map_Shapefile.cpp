// This file stores properties of shapefile layers and separate shapes
// The properties are obsolete, new ShapeDrawingOptions and ShapeCategory classes should be used instead
// Where possible the call where redirected to the new classes

// TODO: old and new implementations should be merged
// m_drawingMethod = dmNewSymbology will serve as a switch

#pragma once
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"
#include <vector>
#include "Enumerations.h"
#include "ShapeDrawingOptions.h"

// TODO: the following properties for the new symbology must be implemented
// ShapeLayerLineStipple
// ShapeLayerFillStipple
// ShapeLayerPointType
// SetUDPointType

#pragma region General
// *************************************************************
//	  ShapefileDrawingOptions
// *************************************************************
// Returns drawing options for a given shapefile
CDrawingOptionsEx* CMapView::get_ShapefileDrawingOptions(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)m_allLayers.size())
	{
		Layer * layer = m_allLayers[layerHandle];
		if( layer->type == ShapefileLayer )
		{
			IShapefile* sf = NULL;
			layer->object->QueryInterface(IID_IShapefile, (void**)&sf);
			if (sf)
			{
				IShapeDrawingOptions* options = NULL;
				sf->get_DefaultDrawingOptions(&options);
				sf->Release();
				if (options)
				{
					CDrawingOptionsEx* retVal = ((CShapeDrawingOptions*)options)->get_UnderlyingOptions();
					options->Release();
					return retVal;
				}
			}
		}
		else
		{
			this->ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
		}
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	return NULL;
}

IShapeDrawingOptions* CMapView::get_ShapeDrawingOptions(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)m_allLayers.size())
	{
		Layer * layer = m_allLayers[layerHandle];
		if( layer->type == ShapefileLayer )
		{
			IShapefile* sf = NULL;
			layer->object->QueryInterface(IID_IShapefile, (void**)&sf);
			if (sf)
			{
				IShapeDrawingOptions* options = NULL;
				sf->get_DefaultDrawingOptions(&options);
				return options;
			}
		}
		else
		{
			this->ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
		}
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	return NULL;
}

// *************************************************************
//	  GetShapefileType
// *************************************************************
// Returns type of the shapefile with a given handle
ShpfileType CMapView::get_ShapefileType(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)m_allLayers.size())
	{
		Layer * layer = m_allLayers[layerHandle];
		if( layer->type == ShapefileLayer )
		{
			IShapefile* sf = NULL;
			layer->object->QueryInterface(IID_IShapefile, (void**)&sf);
			if (sf)
			{
				ShpfileType type;
				sf->get_ShapefileType(&type);
				return type;
			}
		}
		else
		{
			this->ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
		}
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	return SHP_NULLSHAPE;
}

// ***************************************************************
//		get_ShapefileLayer()
// ***************************************************************
// Returns pointer to the shapefile layer with the given index, checks it's type
Layer* CMapView::get_ShapefileLayer(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)m_allLayers.size())
	{
		Layer * layer = m_allLayers[layerHandle];
		if( layer->type == ShapefileLayer )
		{
			return layer;
		}
		else
		{
			this->ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return NULL;
		}
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return NULL;
	}
}

// ***************************************************************
//		IsValidLayer()
// ***************************************************************
bool CMapView::IsValidLayer( long layerHandle )
{
	if (layerHandle >= 0 && layerHandle < (long)m_allLayers.size())
	{
		return (m_allLayers[layerHandle]!=NULL)?true:false;
	}
	else
	{
		return false;
	}
}

// ***************************************************************
//		IsValidShape()
// ***************************************************************
bool CMapView::IsValidShape( long layerHandle, long shape )
{	
	if( IsValidLayer(layerHandle) )
	{
		Layer * l = m_allLayers[layerHandle];
		if( l->type == ShapefileLayer )
		{	
			this->AlignShapeLayerAndShapes(l);
			
			IDispatch * object = l->object;
			IShapefile * ishp = NULL;

			object->QueryInterface(IID_IShapefile,(void**)&ishp);

			long numShapes = 0;
			if( ishp == NULL )
			{
				numShapes = 0;
			}
			else
			{	
				ishp->get_NumShapes(&numShapes);
				ishp->Release();
				ishp = NULL;
			}
			
			if( shape >= 0 && shape < (long)numShapes )
			{
				return TRUE;
			}
			else
			{	
				ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
				return FALSE;
			}
		}
		else
		{	
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return FALSE;
		}
	}
	else
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return FALSE;
	}
	
}
#pragma endregion

#pragma region LayerProperties

#pragma region Points

// *****************************************************************
//		GetShapeLayerDrawPoint()
// *****************************************************************
BOOL CMapView::GetShapeLayerDrawPoint(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			ShpfileType type = this->get_ShapefileType(LayerHandle);
			if (type == SHP_POINT || type == SHP_POINTM || type == SHP_POINTZ ||
				type == SHP_MULTIPOINT || type == SHP_MULTIPOINTZ || type == SHP_MULTIPOINTM)
			{
				return options->fillVisible?TRUE:FALSE;
			}
			else
			{
				return options->verticesVisible?TRUE:FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->flags & slfDrawPoint;
		}
		else
			return FALSE;
	}
}

// *****************************************************************
//		SetShapeLayerDrawPoint()
// *****************************************************************
void CMapView::SetShapeLayerDrawPoint(long LayerHandle, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		
		if (options)
		{
			ShpfileType type = this->get_ShapefileType(LayerHandle);
			if (type == SHP_POINT || type == SHP_POINTM || type == SHP_POINTZ ||
				type == SHP_MULTIPOINT || type == SHP_MULTIPOINTZ || type == SHP_MULTIPOINTM)
			{
				options->fillVisible = (bNewValue == TRUE);
				options->linesVisible = (bNewValue == TRUE);

			}
			else
			{
				options->verticesVisible = bNewValue;
			}
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( bNewValue != FALSE )
				sli->flags |= slfDrawPoint;
			else
				sli->flags = sli->flags & (0xFFFFFFFF ^ slfDrawPoint);

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
			{	if( bNewValue != FALSE )
					sli->shapeInfo[i]->flags |= slfDrawPoint;
				else
					sli->shapeInfo[i]->flags = sli->shapeInfo[i]->flags  & (0xFFFFFFFF ^ slfDrawPoint);
			}

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeLayerPointSize()
// *****************************************************************
float CMapView::GetShapeLayerPointSize(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return options->pointSize;
		else
			return 0.0f;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->pointSize;
		}
		else
			return 0.0f;
	}
}

// *****************************************************************
//		SetShapeLayerPointSize()
// *****************************************************************
void CMapView::SetShapeLayerPointSize(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )
		newValue = 0.0;

	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{		
			options->pointSize = newValue;
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{		
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->pointSize = newValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->pointSize = newValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		ShapeLayerPointType()
// *****************************************************************
short CMapView::GetShapeLayerPointType(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);	// TODO: write conversions between point types
			return ptSquare;
		}
		else
			return 0;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->pointType;
		}
		else
			return 0;
	}
}

// *****************************************************************
//		ShapeLayerPointType()
// *****************************************************************
void CMapView::SetShapeLayerPointType(long LayerHandle, short nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		IShapeDrawingOptions* options = get_ShapeDrawingOptions(LayerHandle);
		if (options)
		{
			switch (nNewValue)
			{
				case ptSquare:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsSquare);
					break;
				case ptCircle:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsCircle);
					break;
				case ptDiamond:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsDiamond);
					break;
				case ptTriangleUp:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsTriangleUp);
					break;
				case ptTriangleDown:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsTriangleDown);
					break;
				case ptTriangleLeft:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsTriangleLeft);
					break;
				case ptTriangleRight:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsTriangleRight);
					break;
				case ptUserDefined:
					options->put_PointType(tkPointSymbolType::ptSymbolPicture);
					break;
				case ptImageList:
					options->put_PointType(tkPointSymbolType::ptSymbolPicture);
					break;
				case ptFontChar:
					options->put_PointType(tkPointSymbolType::ptSymbolFontCharacter);
					break;
				default:
					options->SetDefaultPointSymbol(tkDefaultPointSymbol::dpsDiamond);
					break;
			}

			//ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);	// TODO: write conversions between point types	
			options->Release();
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->pointType = (tkPointType)nNewValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->pointType = (tkPointType)nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}	

// *****************************************************************
//		GetShapeLayerPointColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeLayerPointColor(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			ShpfileType type = this->get_ShapefileType(LayerHandle);
			if (type == SHP_POINT || type == SHP_POINTM || type == SHP_POINTZ ||
				type == SHP_MULTIPOINT || type == SHP_MULTIPOINTZ || type == SHP_MULTIPOINTM)
			{
				return options->fillColor;
			}
			else
			{
				return options->verticesColor;
			}
		}
		else
		{
			return RGB(0,0,0);
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->pointClr;
		}
		else
			return RGB(0,0,0);
	}
}

// *****************************************************************
//		ShapeLayerPointColor()
// *****************************************************************
void CMapView::SetShapeLayerPointColor(long LayerHandle, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			ShpfileType type = this->get_ShapefileType(LayerHandle);
			if (type == SHP_POINT || type == SHP_POINTM || type == SHP_POINTZ ||
				type == SHP_MULTIPOINT || type == SHP_MULTIPOINTZ || type == SHP_MULTIPOINTM)
			{
				options->fillColor = nNewValue;
			}
			else
			{
				options->verticesColor = nNewValue;
			}
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->pointClr = nNewValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->pointClr = nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetUDPointType()
// *****************************************************************
LPDISPATCH CMapView::GetUDPointType(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			if (options->picture)
			{
				options->picture->AddRef();
				return options->picture;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( sli->udPointType )
				sli->udPointType->AddRef();
			return sli->udPointType;
		}
		else
			return NULL;
	}
}

// *****************************************************************
//		SetUDPointType()
// *****************************************************************
void CMapView::SetUDPointType(long LayerHandle, LPDISPATCH newValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			IImage * iimg = NULL;
			newValue->QueryInterface( IID_IImage, (void**)&iimg );
			put_ComReference((IDispatch*)iimg, (IDispatch**)&options->picture, true);
			if (iimg)
			{
				iimg->Release();
			}
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			IImage * iimg = NULL;
			newValue->QueryInterface( IID_IImage, (void**)&iimg );

			if( iimg != NULL )
			{	ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
				if( sli->udPointType != NULL )
					sli->udPointType->Release();
				sli->udPointType = iimg;

				if( sli->udDC != NULL )
					sli->udDC->DeleteDC();
				sli->udDC=NULL;

				iimg->get_OriginalWidth(&(sli->udPointTypeWidth));
				iimg->get_OriginalHeight(&(sli->udPointTypeHeight));

				if( sli->udPointTypeWidth > 0 && sli->udPointTypeHeight > 0 )
				{
					CBitmap * bmp = new CBitmap();

					bmp->CreateDiscardableBitmap(GetDC(),sli->udPointTypeWidth,sli->udPointTypeHeight);
					sli->udDC = new CDC();
					sli->udDC->CreateCompatibleDC(GetDC());
					
					sli->udDC->SelectObject(bmp);

					long pad=sli->udPointTypeWidth*24;
					pad%=32;
					if(pad!=0)
					{	pad=32-pad;
						pad/=8;
					}

					if( sli->udPointTypeImage != NULL )
						delete [] sli->udPointTypeImage;
					sli->udPointTypeImage = new unsigned char[(sli->udPointTypeWidth*3+pad)*sli->udPointTypeHeight];

					long rowLength = sli->udPointTypeWidth*3 + pad;
					VARIANT_BOOL useImageTransparencyColor = FALSE;
					iimg->get_UseTransparencyColor(&useImageTransparencyColor);
					OLE_COLOR imageTransparencyColor = RGB(0,0,0);
					iimg->get_TransparencyColor(&imageTransparencyColor);
					sli->udTransColor=imageTransparencyColor;

					register int j, i;
					for( j = 0; j < sli->udPointTypeHeight; j++ )
					{	
						long * row = new long[sli->udPointTypeWidth+1];
						VARIANT_BOOL retval;
						iimg->GetRow( j, row, &retval );
						
						for( i = 0; i < sli->udPointTypeWidth; i++ )
						{	
							sli->udPointTypeImage[(sli->udPointTypeHeight-j-1)*rowLength+i*3] = GetBValue( row[i] );
							sli->udPointTypeImage[(sli->udPointTypeHeight-j-1)*rowLength+i*3+1] = GetGValue( row[i] );
							sli->udPointTypeImage[(sli->udPointTypeHeight-j-1)*rowLength+i*3+2] = GetRValue( row[i] );
						}
						if( row != NULL )
							delete [] row;
					}

					BITMAPINFO bif;
					BITMAPINFOHEADER bih;
					bih.biBitCount=24;
					bih.biWidth=sli->udPointTypeWidth;
					bih.biHeight=sli->udPointTypeHeight;
					bih.biPlanes=1;
					bih.biSize=sizeof(BITMAPINFOHEADER);
					bih.biCompression=0;
					bih.biXPelsPerMeter=0; 
					bih.biYPelsPerMeter=0;
					bih.biClrUsed=0;
					bih.biClrImportant=0;
					bih.biSizeImage=(sli->udPointTypeWidth*3+pad)*sli->udPointTypeHeight;
					bif.bmiHeader = bih;

					SetDIBitsToDevice( sli->udDC->m_hDC,0,0,sli->udPointTypeWidth,sli->udPointTypeHeight,
									   0,0,0,sli->udPointTypeHeight,sli->udPointTypeImage, &bif, DIB_RGB_COLORS );
				}
				else
				{	if( sli->udPointTypeImage != NULL )
						delete [] sli->udPointTypeImage;
					sli->udPointTypeImage = NULL;
					sli->udPointTypeWidth = 0;
					sli->udPointTypeHeight = 0;
				}
			}
			else
				ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		}
	}
}
#pragma endregion

#pragma region Lines
// *****************************************************************
//		GetShapeLayerDrawLine()
// *****************************************************************
BOOL CMapView::GetShapeLayerDrawLine(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return (BOOL)options->linesVisible;
		else
			return FALSE;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->flags & slfDrawLine;
		}
		else
			return FALSE;
	}
}

// *****************************************************************
//		SetShapeLayerDrawLine()
// *****************************************************************
void CMapView::SetShapeLayerDrawLine(long LayerHandle, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->linesVisible = (bNewValue == TRUE);
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( bNewValue != FALSE )
				sli->flags |= slfDrawLine;
			else
				sli->flags = sli->flags & (0xFFFFFFFF ^ slfDrawLine);

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
			{	
				if( bNewValue != FALSE )
					sli->shapeInfo[i]->flags |= slfDrawLine;
				else
					sli->shapeInfo[i]->flags = sli->shapeInfo[i]->flags  & (0xFFFFFFFF ^ slfDrawLine);
			}

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}	

// ***********************************************************
//		GetShapeLayerLineColor()
// ***********************************************************
OLE_COLOR CMapView::GetShapeLayerLineColor(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return options->lineColor;
		else
			return RGB(0,0,0);
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->lineClr;
		}
		else
		{
			return RGB(0,0,0);
		}
	}
}

// *****************************************************************
//		SetShapeLayerLineColor()
// *****************************************************************
void CMapView::SetShapeLayerLineColor(long LayerHandle, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->lineColor = nNewValue;
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->lineClr = nNewValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->lineClr = nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeLayerLineWidth()
// *****************************************************************
float CMapView::GetShapeLayerLineWidth(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			return options->lineWidth;
		}
		else
		{
			return 0.0f;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->lineWidth;
		}
		else
			return 0.0f;
	}
}

// *****************************************************************
//		ShapeLayerLineWidth()
// *****************************************************************
void CMapView::SetShapeLayerLineWidth(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )
			newValue = 0.0;

	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->lineWidth = newValue;
			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
		
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->lineWidth = newValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->lineWidth = newValue;

			m_canbitblt = FALSE;
		}
	}
}

// *****************************************************************
//		ShapeLayerLineStipple()
// *****************************************************************
short CMapView::GetShapeLayerLineStipple(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return (short)options->lineStipple;				// TODO: convert between enumerations
		else
			return -1;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{		
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->lineStipple;
		}
		else
			return -1;
	}
}

// *****************************************************************
//		SetShapeLayerLineStipple()
// *****************************************************************
void CMapView::SetShapeLayerLineStipple(long LayerHandle, short nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{	
			options->lineStipple = (tkDashStyle)nNewValue;	// TODO: convert between enumerations
			m_canbitblt = FALSE;
		}		
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->lineStipple = (tkLineStipple)nNewValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->lineStipple = (tkLineStipple)nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}		
	}
}

// *****************************************************************
//		GetUDLineStipple()
// *****************************************************************
long CMapView::GetUDLineStipple(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->udLineStipple;
		}
		else
			return 0;
	}
}

// *****************************************************************
//		UDLineStipple()
// *****************************************************************
void CMapView::SetUDLineStipple(long LayerHandle, long nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->udLineStipple = nNewValue;
		}
	}
}
#pragma endregion

#pragma region Fill

// *****************************************************************
//		GetShapeLayerDrawFill()
// *****************************************************************
BOOL CMapView::GetShapeLayerDrawFill(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return (BOOL)options->fillVisible;
		else
			return FALSE;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->flags & slfDrawFill;
		}
		else
			return FALSE;
	}
}

// *****************************************************************
//		SetShapeLayerDrawFill()
// *****************************************************************
void CMapView::SetShapeLayerDrawFill(long LayerHandle, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->fillVisible = (bNewValue == TRUE);
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( bNewValue != FALSE )
				sli->flags |= slfDrawFill;
			else
				sli->flags = sli->flags & (0xFFFFFFFF ^ slfDrawFill);

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
			{	if( bNewValue != FALSE )
					sli->shapeInfo[i]->flags |= slfDrawFill;
				else
					sli->shapeInfo[i]->flags = sli->shapeInfo[i]->flags  & (0xFFFFFFFF ^ slfDrawFill);
			}

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *********************************************************
//		GetShapeLayerFillColor()
// *********************************************************
OLE_COLOR CMapView::GetShapeLayerFillColor(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return options->fillColor;
		else
			return RGB(0,0,0);
	}
	else
	{
		Layer * layer = get_ShapefileLayer(LayerHandle);
		if (layer)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)layer->addInfo;
			return sli->fillClr;
		}
		else
		{
			return RGB(0,0,0);
		}
	}
}

// **********************************************************
//		SetShapeLayerFillColor()
// **********************************************************
void CMapView::SetShapeLayerFillColor(long LayerHandle, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->fillColor = nNewValue;
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->fillClr = nNewValue;

			AlignShapeLayerAndShapes(l);
			long endcondition = sli->shapeInfo.size();
			for(int i = 0; i < endcondition; i++ )
			{
				sli->shapeInfo[i]->fillClr = nNewValue;
			}

			m_canbitblt = FALSE;
			if( !m_lockCount ) InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeLayerFillTransparency
// *****************************************************************
float CMapView::GetShapeLayerFillTransparency(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			return options->fillTransparency/255.0f;
		}
		else
		{
			return 0.0f;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{		
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->trans/255.0f;
		}
		else
			return 0.0f;
	}
}

// *****************************************************************
//		SetShapeLayerFillTransparency
// *****************************************************************
void CMapView::SetShapeLayerFillTransparency(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )		newValue = 0.0;
	else if( newValue > 1.0 )	newValue = 1.0;

	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->fillTransparency = newValue * 255.0f;
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{		
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->trans = newValue*255.0f;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->trans = newValue*255; //cdm 10/31/2006 added * 255 for consistency

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}
#pragma endregion

#pragma region FillStipple
// *****************************************************************
//		GetShapeLayerFillStipple()
// *****************************************************************
short CMapView::GetShapeLayerFillStipple(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			
			return fsNone;			// TODO: write conversion between tkFillStipple and tkGDIPlusHatchStyle enumerations
		}
		else
		{
			return -1;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->fillStipple;
		}
		else
			return -1;
	}
}

// *****************************************************************
//		SetShapeLayerFillStipple()
// *****************************************************************
void CMapView::SetShapeLayerFillStipple(long LayerHandle, short nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)	
		{
			m_canbitblt = FALSE;		// TODO: write conversion between tkFillStipple and tkGDIPlusHatchStyle enumerations
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{		
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->fillStipple = (tkFillStipple)nNewValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->fillStipple = (tkFillStipple)nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}	

// *****************************************************************
//		GetShapeLayerStippleTransparent()
// *****************************************************************
BOOL CMapView::GetShapeLayerStippleTransparent(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return (BOOL)options->fillBgTransparent;
		else
			return TRUE;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return (sli->transparentStipple ? TRUE : FALSE);
		}
		else
			return TRUE;
	}
}

// *****************************************************************
//		SetShapeLayerStippleTransparent()
// *****************************************************************
void CMapView::SetShapeLayerStippleTransparent(long LayerHandle, BOOL nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->fillBgTransparent = (nNewValue == TRUE ? true : false);
			m_canbitblt = FALSE;
		}
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->transparentStipple = (nNewValue == TRUE ? true : false);

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
			{
				sli->shapeInfo[i]->transparentStipple = (nNewValue == TRUE);
			}

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeLayerStippleColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeLayerStippleColor(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
			return options->fillBgColor;
		else
			return RGB(0,0,0);
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->stippleLineClr;
		}
		else
			return RGB(0,0,0);
	}
}

// *****************************************************************
//		SetShapeLayerStippleColor()
// *****************************************************************
void CMapView::SetShapeLayerStippleColor(long LayerHandle, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
		if (options)
		{
			options->fillBgColor = nNewValue;
			m_canbitblt = FALSE;
		}
	}
	else
	{	
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->stippleLineClr = nNewValue;

			AlignShapeLayerAndShapes(l);
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
				sli->shapeInfo[i]->stippleLineClr = nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetUDFillStipple()
// *****************************************************************
long CMapView::GetUDFillStipple(long LayerHandle, long StippleRow)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			if( StippleRow >= 0 && StippleRow < 31 )
			{	ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
				char one = 0, two = 0, three = 0, four = 0;
				long byteCount = StippleRow*4;
				one = sli->udFillStipple[byteCount++];
				two = sli->udFillStipple[byteCount++];
				three = sli->udFillStipple[byteCount++];
				four = sli->udFillStipple[byteCount++];
				long value = one;
				value = value << 8;
				value = value & two;
				value = value << 8;
				value = value & three;
				value = value << 8;
				value = value & four;
				return value;
			}
			else
			{	
				ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
				return 0;
			}
		}
		else
			return 0;
	}
}

// *****************************************************************
//		SetUDFillStipple()
// *****************************************************************
void CMapView::SetUDFillStipple(long LayerHandle, long StippleRow, long nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			if( StippleRow >= 0 && StippleRow < 31 )
			{	ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
				char one = 0, two = 0, three = 0, four = 0;
				long byteCount = StippleRow*4;
				one = (char)nNewValue;
				nNewValue = nNewValue >> 8;
				two = (char)nNewValue;
				nNewValue = nNewValue >> 8;
				three = (char)nNewValue;
				nNewValue = nNewValue >> 8;
				four = (char)nNewValue;
				sli->udFillStipple[byteCount++] = one;
				sli->udFillStipple[byteCount++] = two;
				sli->udFillStipple[byteCount++] = three;
				sli->udFillStipple[byteCount++] = four;
			}
			else
				ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
	}
}
#pragma endregion

#pragma endregion
	
#pragma region REGION PerShapeDrawingOptions

#pragma region ShapeVisible
// *****************************************************************
//		GetShapeVisible()
// *****************************************************************
BOOL CMapView::GetShapeVisible(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return FALSE;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->flags & sfVisible;
		}
		else
			return FALSE;
	}
}

// *****************************************************************
//		SetShapeVisible()
// *****************************************************************
void CMapView::SetShapeVisible(long LayerHandle, long Shape, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		long siFlags;

		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( bNewValue != FALSE )
				sli->shapeInfo[Shape]->flags |= sfVisible;
			else
			{
				siFlags = sli->shapeInfo[Shape]->flags;
				sli->shapeInfo[Shape]->flags = sli->shapeInfo[Shape]->flags  & (0xFFFFFFFF ^ sfVisible);
				siFlags = sli->shapeInfo[Shape]->flags;
			}

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}
#pragma endregion

#pragma region Points

// *****************************************************************
//		GetShapeDrawPoint()
// *****************************************************************
BOOL CMapView::GetShapeDrawPoint(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return FALSE;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->flags & slfDrawPoint;
		}
		else
			return FALSE;
	}
}

// *****************************************************************
//		SetShapeDrawPoint()
// *****************************************************************
void CMapView::SetShapeDrawPoint(long LayerHandle, long Shape, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( bNewValue != FALSE )
				sli->shapeInfo[Shape]->flags |= slfDrawPoint;
			else
				sli->shapeInfo[Shape]->flags = sli->shapeInfo[Shape]->flags  & (0xFFFFFFFF ^ slfDrawPoint);

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapePointColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapePointColor(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return RGB(220,220,220);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->pointClr;
		}
		else
			return RGB(0,0,0);
	}
}

// *****************************************************************
//		SetShapePointColor()
// *****************************************************************
void CMapView::SetShapePointColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->pointClr = nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapePointSize()
// *****************************************************************
float CMapView::GetShapePointSize(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0.0f;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->pointSize;
		}
		else
			return 0.0f;
	}
}

// *****************************************************************
//		SetShapePointSize()
// *****************************************************************
void CMapView::SetShapePointSize(long LayerHandle, long Shape, float newValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( newValue < 0.0 )
			newValue = 0.0;

		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->pointSize = newValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapePointType()
// *****************************************************************
short CMapView::GetShapePointType(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->pointType;
		}
		else
			return 0;
	}
}

// *****************************************************************
//		SetShapePointType()
// *****************************************************************
void CMapView::SetShapePointType(long LayerHandle, long Shape, short nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->pointType = (tkPointType)nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}
#pragma endregion

#pragma region Lines

// *****************************************************************
//		GetShapeDrawLine()
// *****************************************************************
BOOL CMapView::GetShapeDrawLine(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return FALSE;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->flags & slfDrawLine;
		}
		else
			return FALSE;
	}
}

// *****************************************************************
//		SetShapeDrawLine()
// *****************************************************************
void CMapView::SetShapeDrawLine(long LayerHandle, long Shape, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( bNewValue != FALSE )
				sli->shapeInfo[Shape]->flags |= slfDrawLine;
			else
				sli->shapeInfo[Shape]->flags = sli->shapeInfo[Shape]->flags  & (0xFFFFFFFF ^ slfDrawLine);

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeLineColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeLineColor(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return RGB(220,220,220);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->lineClr;
		}
		else
			return RGB(0,0,0);
	}
}

// *****************************************************************
//		SetShapeLineColor()
// *****************************************************************
void CMapView::SetShapeLineColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->lineClr = nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeLineWidth()
// *****************************************************************
float CMapView::GetShapeLineWidth(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0.0f;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->lineWidth;
		}
		else
			return 0.0f;
	}
}

// *****************************************************************
//		SetShapeLineWidth()
// *****************************************************************
void CMapView::SetShapeLineWidth(long LayerHandle, long Shape, float newValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( newValue < 0.0 )
			newValue = 0.0;

		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->lineWidth = newValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeLineStipple()
// *****************************************************************
short CMapView::GetShapeLineStipple(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return -1;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->lineStipple;
		}
		else
			return -1;
	}
}

// *****************************************************************
//		SetShapeLineStipple()
// *****************************************************************
void CMapView::SetShapeLineStipple(long LayerHandle, long Shape, short nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->lineStipple = (tkLineStipple)nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}
#pragma endregion

#pragma region Fill

// *****************************************************************
//		GetShapeDrawFill()
// *****************************************************************
BOOL CMapView::GetShapeDrawFill(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return RGB(220,220,220);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->flags & slfDrawFill;
		}
		else
			return FALSE;
	}
}

// *****************************************************************
//		SetShapeDrawFill()
// *****************************************************************
void CMapView::SetShapeDrawFill(long LayerHandle, long Shape, BOOL bNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( bNewValue != FALSE )
				sli->shapeInfo[Shape]->flags |= slfDrawFill;
			else
				sli->shapeInfo[Shape]->flags = sli->shapeInfo[Shape]->flags  & (0xFFFFFFFF ^ slfDrawFill);

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeFillColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeFillColor(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return RGB(220,220,220);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ))
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->fillClr;
		}
		else
			return RGB(0,0,0);
	}
}

// *****************************************************************
//		SetShapeFillColor()
// *****************************************************************
void CMapView::SetShapeFillColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ))
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->fillClr = nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeFillTransparency()
// *****************************************************************
float CMapView::GetShapeFillTransparency(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0.0f;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->trans/255.0f;
		}
		else
			return 0.0f;
	}
}

// *****************************************************************
//		SetShapeFillTransparency()
// *****************************************************************
void CMapView::SetShapeFillTransparency(long LayerHandle, long Shape, float newValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( newValue < 0.0 )		newValue = 0.0;
		else if( newValue > 1.0 )	newValue = 1.0;

		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->trans = newValue*255.0f;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}
#pragma endregion 

#pragma region FillStipple

// *****************************************************************
//		GetShapeFillStipple()
// *****************************************************************
short CMapView::GetShapeFillStipple(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return -1;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->fillStipple;
		}
		else
			return -1;
	}
}

// *****************************************************************
//		SetShapeFillStipple()
// *****************************************************************
void CMapView::SetShapeFillStipple(long LayerHandle, long Shape, short nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->fillStipple = (tkFillStipple)nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeStippleTransparent()
// *****************************************************************
BOOL CMapView::GetShapeStippleTransparent(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return TRUE;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return (sli->shapeInfo[Shape]->transparentStipple ? TRUE : FALSE);
		}
		else	
			return TRUE;
	}
}

// *****************************************************************
//		SetShapeStippleTransparent()
// *****************************************************************
void CMapView::SetShapeStippleTransparent(long LayerHandle, long Shape, BOOL nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->transparentStipple = sli->transparentStipple = (nNewValue == TRUE ? true : false);

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		GetShapeStippleColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeStippleColor(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return RGB(220,220,220);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->stippleLineClr;
		}
		else
			return RGB(0,0,0);
	}
}

// *****************************************************************
//		SetShapeStippleColor()
// *****************************************************************
void CMapView::SetShapeStippleColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			sli->shapeInfo[Shape]->stippleLineClr = nNewValue;

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}
#pragma endregion

#pragma endregion

#pragma region PointImageList
// *****************************************************************
//		UDPointImageListCount()
// *****************************************************************
long CMapView::get_UDPointImageListCount(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return 0;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->PointImageList.size();
		}
		else
			return 0;
	}
}

// *****************************************************************
//		get_UDPointImageListItem()
// *****************************************************************
IDispatch* CMapView::get_UDPointImageListItem(long LayerHandle, long ImageIndex)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return NULL;
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if (ImageIndex < 0 || ImageIndex > (long)sli->PointImageList.size())
			{
				return NULL;
			}

			if( sli->PointImageList[ImageIndex]->udPointType )
			{
				sli->PointImageList[ImageIndex]->udPointType->AddRef();
			}
			return sli->PointImageList[ImageIndex]->udPointType;
		}
		else
			return NULL;
	}
}

// *****************************************************************
//		ClearUDPointImageList()
// *****************************************************************
void CMapView::ClearUDPointImageList(long LayerHandle)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;

			sli->PointImageList.clear();
			
			// Reset all shapes
			register int i;
			long endcondition = sli->shapeInfo.size();
			for( i = 0; i < endcondition; i++ )
			{
				if (sli->shapeInfo[i]->ImageListIndex != -1)
				{
					sli->shapeInfo[i]->pointType = ptCircle;
					sli->shapeInfo[i]->pointSize = 1;
					sli->shapeInfo[i]->ImageListIndex = -1;
				}
			}

			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();
		}
	}
}

// *****************************************************************
//		set_UDPointImageListAdd()
// *****************************************************************
long CMapView::set_UDPointImageListAdd(long LayerHandle, LPDISPATCH newValue)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return -1;
	}
	else
	{
		int retVal = -1;
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			IImage * iimg = NULL;
			newValue->QueryInterface( IID_IImage, (void**)&iimg );

			if( iimg != NULL )
			{	
				ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;

				udPointListItem * newItem = new udPointListItem();
				newItem->udPointType = iimg;
		
				iimg->get_Width(&(newItem->udPointTypeWidth));
				iimg->get_Height(&(newItem->udPointTypeHeight));

				if( newItem->udPointTypeWidth > 0 && newItem->udPointTypeHeight > 0 )
				{
					CBitmap * bmp = new CBitmap();
					bmp->CreateDiscardableBitmap(GetDC(),newItem->udPointTypeWidth,newItem->udPointTypeHeight);
					newItem->udDC = new CDC();
					newItem->udDC->CreateCompatibleDC(GetDC());
					newItem->udDC->SelectObject(bmp);

					long pad=newItem->udPointTypeWidth*24;
					pad%=32;
					if(pad!=0)
					{	pad=32-pad;
						pad/=8;
					}

					if( newItem->udPointTypeImage != NULL )
						delete [] newItem->udPointTypeImage;
					newItem->udPointTypeImage = new unsigned char[(newItem->udPointTypeWidth*3+pad)*newItem->udPointTypeHeight];

					long rowLength = newItem->udPointTypeWidth*3 + pad;
					VARIANT_BOOL useImageTransparencyColor = FALSE;
					iimg->get_UseTransparencyColor(&useImageTransparencyColor);
					OLE_COLOR imageTransparencyColor = RGB(0,0,0);
					iimg->get_TransparencyColor(&imageTransparencyColor);
					newItem->udTransColor=imageTransparencyColor;

					register int j, i;
					for( j = 0; j < newItem->udPointTypeHeight; j++ )
					{	
						long * row = new long[newItem->udPointTypeWidth+1];
						VARIANT_BOOL retval;
						iimg->GetRow( j, row, &retval );
						
						for( i = 0; i < newItem->udPointTypeWidth; i++ )
						{	
							newItem->udPointTypeImage[(newItem->udPointTypeHeight-j-1)*rowLength+i*3] = GetBValue( row[i] );
							newItem->udPointTypeImage[(newItem->udPointTypeHeight-j-1)*rowLength+i*3+1] = GetGValue( row[i] );
							newItem->udPointTypeImage[(newItem->udPointTypeHeight-j-1)*rowLength+i*3+2] = GetRValue( row[i] );
						}
						if( row != NULL )
							delete [] row;
					}

					BITMAPINFO bif;
					BITMAPINFOHEADER bih;
					bih.biBitCount=24;
					bih.biWidth=newItem->udPointTypeWidth;
					bih.biHeight=newItem->udPointTypeHeight;
					bih.biPlanes=1;
					bih.biSize=sizeof(BITMAPINFOHEADER);
					bih.biCompression=0;
					bih.biXPelsPerMeter=0;
					bih.biYPelsPerMeter=0;
					bih.biClrUsed=0;
					bih.biClrImportant=0;
					bih.biSizeImage=(newItem->udPointTypeWidth*3+pad)*newItem->udPointTypeHeight;
					bif.bmiHeader = bih;

					SetDIBitsToDevice( newItem->udDC->m_hDC,0,0,newItem->udPointTypeWidth,newItem->udPointTypeHeight,
									   0,0,0,newItem->udPointTypeHeight,newItem->udPointTypeImage, &bif, DIB_RGB_COLORS );

				}
				else
				{	
					if( newItem->udPointTypeImage != NULL )
						delete [] newItem->udPointTypeImage;
					newItem->udPointTypeImage = NULL;
					newItem->udPointTypeWidth = 0;
					newItem->udPointTypeHeight = 0;

				}

				sli->PointImageList.push_back(newItem);
				retVal = sli->PointImageList.size() - 1;
			}
			else
				ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		}
		return retVal;
	}
}

	// *****************************************************************
//		SetShapePointImageListID()
// *****************************************************************
void CMapView::SetShapePointImageListID(long LayerHandle, long Shape, long ImageIndex)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( ImageIndex < 0 )
			ImageIndex = -1;

		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;

			if (ImageIndex > (long)sli->PointImageList.size())
				ImageIndex = -1;

			sli->shapeInfo[Shape]->ImageListIndex = ImageIndex;

			m_canbitblt = FALSE;
			if( !m_lockCount )
			{
				InvalidateControl();
			}
		}
	}
}

// *****************************************************************
//		GetShapePointImageListID()
// *****************************************************************
long CMapView::GetShapePointImageListID(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return -1;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			return sli->shapeInfo[Shape]->ImageListIndex ;
		}
		else
			return -1;
	}
}
#pragma endregion

#pragma region PointFontCharList
// *****************************************************************
//		set_UDPointFontCharListAdd()
// *****************************************************************
long CMapView::set_UDPointFontCharListAdd(long LayerHandle, long newValue, OLE_COLOR color)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return -1;
	}
	else
	{
		int retVal = -1;
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			udPointListFontCharItem * newItem = new udPointListFontCharItem();
			newItem->udPointFontCharIdx = newValue;
			newItem->udPointFontCharColor = color;
					
			sli->PointFontCharList.push_back(newItem);
			retVal = sli->PointFontCharList.size() - 1;
		}
		return retVal;	
	}
}

// *****************************************************************
//		set_UDPointFontCharFont()
// *****************************************************************
void CMapView::set_UDPointFontCharFont(long LayerHandle, LPCTSTR FontName, float FontSize, BOOL isBold, BOOL isItalic, BOOL isUnderline)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			LONG fontWeight = FW_REGULAR;
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if (sli->pointType ==  ptFontChar)
			{
				sli->SetFont(new CFont);
				sli->fontName = FontName;
				sli->fontSize = FontSize;
				sli->isBold = isBold;
				if (sli->isBold)
				   fontWeight = FW_BOLD;
				sli->isItalic = isItalic;
				sli->isUnderline = isUnderline;
				VERIFY(sli->GetFont()->CreateFont((int)sli->fontSize,0,0,0,fontWeight,sli->isItalic, sli->isUnderline, 
						 FALSE, DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY, 
							   DEFAULT_PITCH,sli->fontName)); 
			}
			else
				ErrorMessage(tkINVALID_PARAMETER_VALUE);
		}
	}
}

// *****************************************************************
//		set_UDPointFontCharFontSize()
// *****************************************************************
void CMapView::set_UDPointFontCharFontSize(long LayerHandle,  float FontSize)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		Layer * l = get_ShapefileLayer(LayerHandle);
		if (l)
		{
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if (sli->pointType ==  ptFontChar)
			{
				sli->fontSize = FontSize;
			}
			else
				ErrorMessage(tkINVALID_PARAMETER_VALUE);
		}
	}
}

// *****************************************************************
//		SetShapePointFontCharListID()
// *****************************************************************
void CMapView::SetShapePointFontCharListID(long LayerHandle, long Shape, long FontCharIndex)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	else
	{
		if( FontCharIndex < 0 )
		{
			FontCharIndex = -1;
		}

		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;

			if (FontCharIndex > (long)sli->PointFontCharList.size())
			{
				FontCharIndex = -1;
			}

			sli->shapeInfo[Shape]->FontCharListIndex = FontCharIndex;

			if( !m_lockCount )
			{
				InvalidateControl();
			}
		}
	}
}

// *****************************************************************
//		GetShapePointFontCharListID()
// *****************************************************************
long CMapView::GetShapePointFontCharListID(long LayerHandle, long Shape)
{
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
		return -1;
	}
	else
	{
		if( IsValidShape( LayerHandle, Shape ) )
		{	
			Layer * l = m_allLayers[LayerHandle];
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;

			return sli->shapeInfo[Shape]->FontCharListIndex ;
		}
		else
			return -1;
	}
}
#pragma endregion
	



