// This file stores properties of shapefile layers and separate shapes
// The properties are obsolete, new ShapeDrawingOptions and ShapeCategory classes should be used instead
// Where possible the call where redirected to the new classes
#pragma once
#include "stdafx.h"
#include "Map.h"
#include "ShapeDrawingOptions.h"
#include "Shapefile.h"
#include "Labels.h"

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
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		Layer * layer = _allLayers[layerHandle];
		if( layer->IsShapefile() )
		{
			IShapefile* sf = NULL;
			if (layer->QueryShapefile(&sf))
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

// *************************************************************
//	  GetShapefileType
// *************************************************************
// Returns type of the shapefile with a given handle
ShpfileType CMapView::get_ShapefileType(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		Layer * layer = _allLayers[layerHandle];
		if( layer->IsShapefile() )
		{
			IShapefile* sf = NULL;
			layer->QueryShapefile(&sf);
			if (sf)
			{
				ShpfileType type;
				sf->get_ShapefileType(&type);
				sf->Release();
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
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		Layer * layer = _allLayers[layerHandle];
		if( layer->IsShapefile() )
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
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		return (_allLayers[layerHandle]!=NULL)?true:false;
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
		Layer * l = _allLayers[layerHandle];
		if( l->IsShapefile() )
		{	
			this->AlignShapeLayerAndShapes(l);
			
			IShapefile * ishp = NULL;
			l->QueryShapefile(&ishp);

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

// *****************************************************************
//		SetShapeLayerDrawPoint()
// *****************************************************************
void CMapView::SetShapeLayerDrawPoint(long LayerHandle, BOOL bNewValue)
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
		_canUseLayerBuffer = FALSE;
	}
}

// *****************************************************************
//		GetShapeLayerPointSize()
// *****************************************************************
float CMapView::GetShapeLayerPointSize(long LayerHandle)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return options->pointSize;
	else
		return 0.0f;
}

// *****************************************************************
//		SetShapeLayerPointSize()
// *****************************************************************
void CMapView::SetShapeLayerPointSize(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )
		newValue = 0.0;

	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{		
		options->pointSize = newValue;
		_canUseLayerBuffer = FALSE;
	}
}

// *****************************************************************
//		ShapeLayerPointType()
// *****************************************************************
short CMapView::GetShapeLayerPointType(long LayerHandle)
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

// *****************************************************************
//		ShapeLayerPointType()
// *****************************************************************
void CMapView::SetShapeLayerPointType(long LayerHandle, short nNewValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);	// TODO: write conversions between point types	
	}
}	

// *****************************************************************
//		GetShapeLayerPointColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeLayerPointColor(long LayerHandle)
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

// *****************************************************************
//		ShapeLayerPointColor()
// *****************************************************************
void CMapView::SetShapeLayerPointColor(long LayerHandle, OLE_COLOR nNewValue)
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
		_canUseLayerBuffer = FALSE;
	}
}

// *****************************************************************
//		GetUDPointType()
// *****************************************************************
LPDISPATCH CMapView::GetUDPointType(long LayerHandle)
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

// *****************************************************************
//		SetUDPointType()
// *****************************************************************
void CMapView::SetUDPointType(long LayerHandle, LPDISPATCH newValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		IImage * iimg = NULL;
		newValue->QueryInterface( IID_IImage, (void**)&iimg );
		Utility::put_ComReference((IDispatch*)iimg, (IDispatch**)&options->picture, true);
		if (iimg)
		{
			iimg->Release();
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
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return (BOOL)options->linesVisible;
	else
		return FALSE;
}

// *****************************************************************
//		SetShapeLayerDrawLine()
// *****************************************************************
void CMapView::SetShapeLayerDrawLine(long LayerHandle, BOOL bNewValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->linesVisible = (bNewValue == TRUE);
		_canUseLayerBuffer = FALSE;
	}
}	

// ***********************************************************
//		GetShapeLayerLineColor()
// ***********************************************************
OLE_COLOR CMapView::GetShapeLayerLineColor(long LayerHandle)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return options->lineColor;
	else
		return RGB(0,0,0);
}

// *****************************************************************
//		SetShapeLayerLineColor()
// *****************************************************************
void CMapView::SetShapeLayerLineColor(long LayerHandle, OLE_COLOR nNewValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->lineColor = nNewValue;
		_canUseLayerBuffer = FALSE;
	}
}

// *****************************************************************
//		GetShapeLayerLineWidth()
// *****************************************************************
float CMapView::GetShapeLayerLineWidth(long LayerHandle)
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

// *****************************************************************
//		ShapeLayerLineWidth()
// *****************************************************************
void CMapView::SetShapeLayerLineWidth(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )
			newValue = 0.0;

	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->lineWidth = newValue;
		_canUseLayerBuffer = FALSE;
		if( !_lockCount )
			InvalidateControl();
	}
}

// *****************************************************************
//		ShapeLayerLineStipple()
// *****************************************************************
short CMapView::GetShapeLayerLineStipple(long LayerHandle)
{
	ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return (short)options->lineStipple;				// TODO: convert between enumerations
	else
		return -1;
}

// *****************************************************************
//		SetShapeLayerLineStipple()
// *****************************************************************
void CMapView::SetShapeLayerLineStipple(long LayerHandle, short nNewValue)
{
	ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{	
		options->lineStipple = (tkDashStyle)nNewValue;	// TODO: convert between enumerations
		_canUseLayerBuffer = FALSE;
	}		
}

// *****************************************************************
//		GetUDLineStipple()
// *****************************************************************
long CMapView::GetUDLineStipple(long LayerHandle)
{
	ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
	return 0;
}

// *****************************************************************
//		UDLineStipple()
// *****************************************************************
void CMapView::SetUDLineStipple(long LayerHandle, long nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}
#pragma endregion

#pragma region Fill

// *****************************************************************
//		GetShapeLayerDrawFill()
// *****************************************************************
BOOL CMapView::GetShapeLayerDrawFill(long LayerHandle)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return (BOOL)options->fillVisible;
	else
		return FALSE;
}

// *****************************************************************
//		SetShapeLayerDrawFill()
// *****************************************************************
void CMapView::SetShapeLayerDrawFill(long LayerHandle, BOOL bNewValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->fillVisible = (bNewValue == TRUE);
		_canUseLayerBuffer = FALSE;
	}
}

// *********************************************************
//		GetShapeLayerFillColor()
// *********************************************************
OLE_COLOR CMapView::GetShapeLayerFillColor(long LayerHandle)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return options->fillColor;
	else
		return RGB(0,0,0);
}

// **********************************************************
//		SetShapeLayerFillColor()
// **********************************************************
void CMapView::SetShapeLayerFillColor(long LayerHandle, OLE_COLOR nNewValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->fillColor = nNewValue;
		_canUseLayerBuffer = FALSE;
	}
}

// *****************************************************************
//		GetShapeLayerFillTransparency
// *****************************************************************
float CMapView::GetShapeLayerFillTransparency(long LayerHandle)
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

// *****************************************************************
//		SetShapeLayerFillTransparency
// *****************************************************************
void CMapView::SetShapeLayerFillTransparency(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )		newValue = 0.0;
	else if( newValue > 1.0 )	newValue = 1.0;

	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->fillTransparency = newValue * 255.0f;
		_canUseLayerBuffer = FALSE;
	}
}
#pragma endregion

#pragma region FillStipple
// *****************************************************************
//		GetShapeLayerFillStipple()
// *****************************************************************
short CMapView::GetShapeLayerFillStipple(long LayerHandle)
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

// *****************************************************************
//		SetShapeLayerFillStipple()
// *****************************************************************
void CMapView::SetShapeLayerFillStipple(long LayerHandle, short nNewValue)
{
	ErrorMessage(tkPROPERTY_NOT_IMPLEMENTED);
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)	
	{
		// TODO: write conversion between tkFillStipple and tkGDIPlusHatchStyle enumerations
		_canUseLayerBuffer = FALSE;		
	}
}	

// *****************************************************************
//		GetShapeLayerStippleTransparent()
// *****************************************************************
BOOL CMapView::GetShapeLayerStippleTransparent(long LayerHandle)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return (BOOL)options->fillBgTransparent;
	else
		return TRUE;
}

// *****************************************************************
//		SetShapeLayerStippleTransparent()
// *****************************************************************
void CMapView::SetShapeLayerStippleTransparent(long LayerHandle, BOOL nNewValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->fillBgTransparent = (nNewValue == TRUE ? true : false);
		_canUseLayerBuffer = FALSE;
	}
}

// *****************************************************************
//		GetShapeLayerStippleColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeLayerStippleColor(long LayerHandle)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
		return options->fillBgColor;
	else
		return RGB(0,0,0);
}

// *****************************************************************
//		SetShapeLayerStippleColor()
// *****************************************************************
void CMapView::SetShapeLayerStippleColor(long LayerHandle, OLE_COLOR nNewValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->fillBgColor = nNewValue;
		_canUseLayerBuffer = FALSE;
	}
}

// *****************************************************************
//		GetUDFillStipple()
// *****************************************************************
long CMapView::GetUDFillStipple(long LayerHandle, long StippleRow)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return 0;
}

// *****************************************************************
//		SetUDFillStipple()
// *****************************************************************
void CMapView::SetUDFillStipple(long LayerHandle, long StippleRow, long nNewValue)
{
		ErrorMessage(tkPROPERTY_DEPRECATED);
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
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return FALSE;
}

// *****************************************************************
//		SetShapeVisible()
// *****************************************************************
void CMapView::SetShapeVisible(long LayerHandle, long Shape, BOOL bNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}
#pragma endregion

#pragma region Points

// *****************************************************************
//		GetShapeDrawPoint()
// *****************************************************************
BOOL CMapView::GetShapeDrawPoint(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return FALSE;
}

// *****************************************************************
//		SetShapeDrawPoint()
// *****************************************************************
void CMapView::SetShapeDrawPoint(long LayerHandle, long Shape, BOOL bNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapePointColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapePointColor(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return RGB(220,220,220);
}

// *****************************************************************
//		SetShapePointColor()
// *****************************************************************
void CMapView::SetShapePointColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapePointSize()
// *****************************************************************
float CMapView::GetShapePointSize(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return 0.0f;
}

// *****************************************************************
//		SetShapePointSize()
// *****************************************************************
void CMapView::SetShapePointSize(long LayerHandle, long Shape, float newValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapePointType()
// *****************************************************************
short CMapView::GetShapePointType(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return 0;
}

// *****************************************************************
//		SetShapePointType()
// *****************************************************************
void CMapView::SetShapePointType(long LayerHandle, long Shape, short nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}
#pragma endregion

#pragma region Lines

// *****************************************************************
//		GetShapeDrawLine()
// *****************************************************************
BOOL CMapView::GetShapeDrawLine(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return FALSE;
}

// *****************************************************************
//		SetShapeDrawLine()
// *****************************************************************
void CMapView::SetShapeDrawLine(long LayerHandle, long Shape, BOOL bNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapeLineColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeLineColor(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return RGB(220,220,220);
}

// *****************************************************************
//		SetShapeLineColor()
// *****************************************************************
void CMapView::SetShapeLineColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapeLineWidth()
// *****************************************************************
float CMapView::GetShapeLineWidth(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return 0.0f;
}

// *****************************************************************
//		SetShapeLineWidth()
// *****************************************************************
void CMapView::SetShapeLineWidth(long LayerHandle, long Shape, float newValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapeLineStipple()
// *****************************************************************
short CMapView::GetShapeLineStipple(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return -1;
}

// *****************************************************************
//		SetShapeLineStipple()
// *****************************************************************
void CMapView::SetShapeLineStipple(long LayerHandle, long Shape, short nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}
#pragma endregion

#pragma region Fill

// *****************************************************************
//		GetShapeDrawFill()
// *****************************************************************
BOOL CMapView::GetShapeDrawFill(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return RGB(220,220,220);
}

// *****************************************************************
//		SetShapeDrawFill()
// *****************************************************************
void CMapView::SetShapeDrawFill(long LayerHandle, long Shape, BOOL bNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapeFillColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeFillColor(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return RGB(220,220,220);
}

// *****************************************************************
//		SetShapeFillColor()
// *****************************************************************
void CMapView::SetShapeFillColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapeFillTransparency()
// *****************************************************************
float CMapView::GetShapeFillTransparency(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return 0.0f;
}

// *****************************************************************
//		SetShapeFillTransparency()
// *****************************************************************
void CMapView::SetShapeFillTransparency(long LayerHandle, long Shape, float newValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}
#pragma endregion 

#pragma region FillStipple

// *****************************************************************
//		GetShapeFillStipple()
// *****************************************************************
short CMapView::GetShapeFillStipple(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return -1;
}

// *****************************************************************
//		SetShapeFillStipple()
// *****************************************************************
void CMapView::SetShapeFillStipple(long LayerHandle, long Shape, short nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapeStippleTransparent()
// *****************************************************************
BOOL CMapView::GetShapeStippleTransparent(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return TRUE;
}

// *****************************************************************
//		SetShapeStippleTransparent()
// *****************************************************************
void CMapView::SetShapeStippleTransparent(long LayerHandle, long Shape, BOOL nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapeStippleColor()
// *****************************************************************
OLE_COLOR CMapView::GetShapeStippleColor(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return RGB(220,220,220);
}

// *****************************************************************
//		SetShapeStippleColor()
// *****************************************************************
void CMapView::SetShapeStippleColor(long LayerHandle, long Shape, OLE_COLOR nNewValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}
#pragma endregion

#pragma endregion

#pragma region PointImageList
// *****************************************************************
//		UDPointImageListCount()
// *****************************************************************
long CMapView::get_UDPointImageListCount(long LayerHandle)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return 0;
}

// *****************************************************************
//		get_UDPointImageListItem()
// *****************************************************************
IDispatch* CMapView::get_UDPointImageListItem(long LayerHandle, long ImageIndex)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return NULL;
}

// *****************************************************************
//		ClearUDPointImageList()
// *****************************************************************
void CMapView::ClearUDPointImageList(long LayerHandle)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		set_UDPointImageListAdd()
// *****************************************************************
long CMapView::set_UDPointImageListAdd(long LayerHandle, LPDISPATCH newValue)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return -1;
}

	// *****************************************************************
//		SetShapePointImageListID()
// *****************************************************************
void CMapView::SetShapePointImageListID(long LayerHandle, long Shape, long ImageIndex)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapePointImageListID()
// *****************************************************************
long CMapView::GetShapePointImageListID(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return -1;
}
#pragma endregion

#pragma region PointFontCharList
// *****************************************************************
//		set_UDPointFontCharListAdd()
// *****************************************************************
long CMapView::set_UDPointFontCharListAdd(long LayerHandle, long newValue, OLE_COLOR color)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return -1;
}

// *****************************************************************
//		set_UDPointFontCharFont()
// *****************************************************************
void CMapView::set_UDPointFontCharFont(long LayerHandle, LPCTSTR FontName, float FontSize, BOOL isBold, BOOL isItalic, BOOL isUnderline)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		set_UDPointFontCharFontSize()
// *****************************************************************
void CMapView::set_UDPointFontCharFontSize(long LayerHandle,  float FontSize)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		SetShapePointFontCharListID()
// *****************************************************************
void CMapView::SetShapePointFontCharListID(long LayerHandle, long Shape, long FontCharIndex)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// *****************************************************************
//		GetShapePointFontCharListID()
// *****************************************************************
long CMapView::GetShapePointFontCharListID(long LayerHandle, long Shape)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	return -1;
}
#pragma endregion

#pragma region Loop through shapefiles
// ************************************************************
//		FindSnapPoint
// ************************************************************
VARIANT_BOOL CMapView::FindSnapPoint(double tolerance, double xScreen, double yScreen, double* xFound, double* yFound)
{
	double x, y, x2, y2;
	this->PixelToProjection( xScreen, yScreen, x, y );
	this->PixelToProjection( xScreen + tolerance, yScreen + tolerance, x2, y2 );
	double maxDist = sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0));

	long shapeIndex;
	long pointIndex;
	VARIANT_BOOL vb;
	double distance;
	
	double minDist = DBL_MAX;
	IShapefile* foundShapefile = NULL;
	long foundShapeIndex;
	long foundPointIndex;

	for(long i = 0; i < this->GetNumLayers(); i++)
	{
		CComPtr<IShapefile> sf = this->GetShapefile(this->GetLayerHandle(i));
		if (sf)
		{
			VARIANT_BOOL snappable;
			sf->get_Snappable(&snappable);
			if (snappable) {
				sf->GetClosestVertex(x, y, maxDist, &shapeIndex, &pointIndex, &distance, &vb);
				if (vb)
				{
					if (distance < minDist)
					{
						minDist = distance;
						foundShapefile = sf;
						foundPointIndex = pointIndex;
						foundShapeIndex = shapeIndex;
					}
				}
			}
		}
	}

	bool result = false;
	if (minDist != DBL_MAX && foundShapefile)
	{
		IShape* shape = NULL;
		foundShapefile->get_Shape(foundShapeIndex, &shape);
		if (shape)
		{
			shape->get_XY(foundPointIndex, xFound, yFound, &vb);
			shape->Release();
			result = true;
		}
	}
	return result;
}

// ************************************************************
//		FindShapeAtScreenPoint
// ************************************************************
HotTrackingInfo* CMapView::FindShapeAtScreenPoint(CPoint point, LayerSelector selector)
{
	double pixX = point.x;
	double pixY = point.y;
	double prjX, prjY;
	this->PixelToProj(pixX, pixY, &prjX, &prjY);
	
	std::vector<bool> layers;
	if (SelectLayers(selector, layers))
	{
		return FindShapeCore(prjX, prjY, layers);
	}
	return new HotTrackingInfo();
}


// ************************************************************
//		SelectLayerHandles
// ************************************************************
bool CMapView::SelectLayerHandles(LayerSelector selector, std::vector<int>& layers)
{
	IShapefile * sf = NULL;
	for (int i = 0; i < (int)_activeLayers.size(); i++)
	{
		int handle = GetLayerHandle(i);
		if (selector == slctAll)
		{
			layers.push_back(handle);
			continue;
		}

		bool result = CheckLayer(selector, handle);
		if (result)
			layers.push_back(handle);
	}
	return layers.size() > 0;
}

// ************************************************************
//		SelectLayers
// ************************************************************
bool CMapView::SelectLayers(LayerSelector selector, std::vector<bool>& layers)
{
	for (int i = 0; i < (int)_activeLayers.size(); i++)
	{
		if (selector == slctAll)
		{
			layers.push_back(true);
			continue;
		}
		bool result = CheckLayer(selector, _activeLayers[i]);
		layers.push_back(result);
	}
	for (size_t i = 0; i < layers.size(); i++)
		if (layers[i]) return true;
	return false;
}

// ************************************************************
//		CheckLayer
// ************************************************************
bool CMapView::CheckLayer(LayerSelector selector, int layerHandle)
{
	 CComPtr<IShapefile> sf = NULL;
	Layer* layer = _allLayers[layerHandle];
	if (layer->IsShapefile())
	{
		if (layer->QueryShapefile(&sf))
		{
			VARIANT_BOOL result = VARIANT_FALSE;
			switch (selector)
			{
			case slctHotTracking:
				if (!layer->wasRendered) return false;
				VARIANT_BOOL vb;
				sf->get_InteractiveEditing(&vb);
				if (vb) 
				{
					// only editor based highlighting in this mode
					VARIANT_BOOL highlight;
					_shapeEditor->get_HighlightShapes(&highlight);
					if (IsEditorCursor() && highlight) {
						result = VARIANT_TRUE;
					}
				}
				else {
					if (m_cursorMode == cmIdentify) {
						sf->get_HotTracking(&result);
					}
				}
				break;
			case slctInMemorySf:
				sf->get_EditingShapes(&result);
				break;
			case slctInteractiveEditing:
				sf->get_InteractiveEditing(&result);
				break;
			}
			return result ? true : false;
		}
	}
	return false;
}

// ************************************************************
//		GetMouseTolerance
// ************************************************************
// Mouse tolerance in projected units
double CMapView::GetMouseTolerance(MouseTolerance tolerance, bool proj)
{
	double tol = 0;
	switch(tolerance)
	{
		case ToleranceSnap:
			_shapeEditor->get_SnapTolerance(&tol);
			break;	
		case ToleranceSelect:
			tol = 20;    //TODO: make parameter
			break;
		case ToleranceInsert:
			tol = 10;
			break;
	}
	if (proj)
		tol /= this->PixelsPerMapUnit();
	return tol;
}

// ************************************************************
//		FindShapeCore
// ************************************************************
HotTrackingInfo* CMapView::FindShapeCore(double prjX, double prjY, std::vector<bool>& layers)
{
	HotTrackingInfo* info = NULL;
	IShapefile * sf = NULL;
	for (int i = (int)_activeLayers.size() - 1; i >= 0; i--)
	{
		if (!layers[i]) continue;

		Layer* layer = _allLayers[_activeLayers[i]];
		if (layer->IsShapefile())
		{
			if (layer->QueryShapefile(&sf))
			{
				std::vector<long> shapes;

				double tol = 0.0;
				ShpfileType type;
				sf->get_ShapefileType(&type);
				type = Utility::ShapeTypeConvert2D(type);
				if (type == SHP_MULTIPOINT || type == SHP_POINT || type == SHP_POLYLINE)
					tol = GetMouseTolerance(ToleranceSelect);

				SelectMode mode = type == SHP_POLYGON ? INCLUSION : INTERSECTION;

				((CShapefile*)sf)->SelectShapesCore(Extent(prjX , prjX , prjY , prjY ), tol, mode, shapes);
					
				VARIANT_BOOL visible;
				for (size_t j = 0; j < shapes.size(); j++)
				{
					sf->get_ShapeVisible(shapes[j], &visible);    
					if (visible) {
						IShape* shape = NULL;
						sf->get_Shape(shapes[j], &shape);
						info = new HotTrackingInfo();
						info->ShapeId = shapes[j];
						info->LayerHandle = _activeLayers[i];
						info->Shapefile = sf;
						info->Shape = shape;

						long numPoints;
						shape->get_NumPoints(&numPoints);
						return info;
					}
				}
				sf->Release();
				sf = NULL;
				if (info) break;
			}
		}
	}
	return NULL;
}

// ************************************************************
//		ClearHotTracking
// ************************************************************
void CMapView::ClearHotTracking()
{
	if (_hotTracking.ShapeId != -1) {
		_canUseMainBuffer = false;
	}
	VARIANT_BOOL vb;
	_hotTracking.ShapeId = -1;
	_hotTracking.LayerHandle = -1;
	if (_hotTracking.Shapefile)
		_hotTracking.Shapefile->Close(&vb);

}

// ************************************************************
//		ApplyHotTrackingDisplayOptions
// ************************************************************
void CMapView::ApplyHotTrackingInfo(HotTrackingInfo* info, IShape* shp)
{
	if (!_hotTracking.Shapefile) return;
	ShpfileType type;
	info->Shapefile->get_ShapefileType(&type);
	type = Utility::ShapeTypeConvert2D(type);

	VARIANT_BOOL vb;
	((CShapefile*)_hotTracking.Shapefile)->CreateNewCore(A2BSTR(""), type, false, &vb);
	long index = 0;
	_hotTracking.Shapefile->EditInsertShape(shp, &index, &vb);
	_hotTracking.Shapefile->RefreshExtents(&vb);
	_hotTracking.LayerHandle = info->LayerHandle;
	_hotTracking.ShapeId = info->ShapeId;

	CComPtr<IShapeDrawingOptions> options = NULL;

	info->Shapefile->get_DefaultDrawingOptions(&options);
	if (options)
	{
		CComPtr<IShapeDrawingOptions> newOptions = NULL;
		options->Clone(&newOptions);
		if (newOptions)
		{
			VARIANT_BOOL interactiveEditing;
			info->Shapefile->get_InteractiveEditing(&interactiveEditing);

			if (interactiveEditing)
			{
				if (type == SHP_POINT || type == SHP_MULTIPOINT)
				{
					newOptions->put_FillColor(RGB(0, 0, 255));   // blue
					newOptions->put_FillVisible(VARIANT_TRUE);
				}
				else {
					newOptions->put_LineVisible(VARIANT_FALSE);
					newOptions->put_FillVisible(VARIANT_FALSE);
					newOptions->put_VerticesVisible(VARIANT_TRUE);  // vertices only
				}
			}
			else
			{
				bool point = type == SHP_POINT || type == SHP_MULTIPOINT;
				newOptions->put_FillVisible(point ? VARIANT_TRUE : VARIANT_FALSE);
				newOptions->put_LineColor(RGB(30, 144, 255));
				newOptions->put_LineWidth(2.0f);
			}
			_hotTracking.Shapefile->put_DefaultDrawingOptions(newOptions);
		}
	}
}

// ************************************************************
//		UpdateHotTracking
// ************************************************************
bool CMapView::UpdateHotTracking(CPoint point)
{
	bool found = false;

	if (HasHotTracking())
	{
		HotTrackingInfo* info = FindShapeAtScreenPoint(point, slctHotTracking);
		if (info)
		{
			bool sameShape = false;
	
			if (_shapeEditor->HasSubjectShape(info->LayerHandle, info->ShapeId)) {
				sameShape = true;
			}
			else {
				sameShape = !(info->LayerHandle != _hotTracking.LayerHandle || info->ShapeId != _hotTracking.ShapeId);
			}

			if (!sameShape)
			{
				IShape* shape = info->Shape;
				if (shape)
				{
					IShape* shpClone = NULL;
					shape->Clone(&shpClone);

					_hotTracking.UpdateShapefile();

					ApplyHotTrackingInfo(info, shpClone);
					UINT refCount = shpClone->Release();   // there is one reference in new shapefile
					found = true;
				}
			}
			delete info;
			
			if (sameShape) return false;
		}
	}

	bool refreshNeeded = false;
	if (found)
	{
		// passing event to the caller
		this->FireShapeHighlighted(_hotTracking.LayerHandle, _hotTracking.ShapeId, point.x, point.y);
		_canUseMainBuffer = false;
		refreshNeeded = true;
	}

	if (!found && _hotTracking.ShapeId != -1)
	{
		ClearHotTracking();

		// passing event to the caller
		this->FireShapeHighlighted(-1, -1, point.x, point.y);
		refreshNeeded = true;
	}
	return refreshNeeded;
}

// **********************************************************
//			GetDrawingLabels()
// **********************************************************
// Deletes dynamically alocated frames info for all layers; drops isDrawn flag
void CMapView::ClearLabelFrames()
{
	// clear frames for regular labels
	for (int i = 0; i < (int)_activeLayers.size(); i++)
	{
		Layer * l = _allLayers[_activeLayers[i]];
		if( l != NULL )
		{	
			// charts
			if (l->IsShapefile())
			{
				IShapefile * sf = NULL;
				if (l->QueryShapefile(&sf))
				{
					((CShapefile*)sf)->ClearChartFrames();
					sf->Release();
				}
			}
			
			// labels
			ILabels* LabelsClass = l->get_Labels();
			if (LabelsClass == NULL) continue;
			
			CLabels* coLabels = static_cast<CLabels*>(LabelsClass);
			coLabels->ClearLabelFrames();
			LabelsClass->Release(); LabelsClass = NULL;
		}
	}

	// clear frames for drawing labels
	for(size_t j = 0; j < _activeDrawLists.size(); j++ )
	{
		bool isSkip = false;
		for (size_t i = 0; i < _drawingLayerInvisilbe.size(); i++)
		{
			if (_drawingLayerInvisilbe[i] == j)
			{
				isSkip = true;	// skip if this layer is set invisible
				break;  
			}
		}
		if(isSkip) 
			continue;

		DrawList * dlist = _allDrawLists[_activeDrawLists[j]];
		if( IS_VALID_PTR(dlist) )
		{
			if (dlist->listType == dlSpatiallyReferencedList)
			{
				CLabels* coLabels = static_cast<CLabels*>(dlist->m_labels);
				coLabels->ClearLabelFrames();
			}
		}
	}
}

#pragma endregion

