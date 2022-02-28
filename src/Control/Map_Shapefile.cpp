// This file stores properties of shapefile layers and separate shapes
// The properties are obsolete, new ShapeDrawingOptions and ShapeCategory classes should be used instead
// Where possible the call where redirected to the new classes
#include <stdafx.h>
#include "Map.h"
#include "ShapeDrawingOptions.h"
#include "Shapefile.h"

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
CDrawingOptionsEx* CMapView::get_ShapefileDrawingOptions(const long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[layerHandle];
		if (layer->IsShapefile())
		{
			IShapefile* sf;
			if (layer->QueryShapefile(&sf))
			{
				IShapeDrawingOptions* options = nullptr;
				sf->get_DefaultDrawingOptions(&options);
				sf->Release();
				if (options)
				{
					CDrawingOptionsEx* retVal = static_cast<CShapeDrawingOptions*>(options)->get_UnderlyingOptions();
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
	return nullptr;
}

// *************************************************************
//	  GetShapefileType
// *************************************************************
// Returns type of the shapefile with a given handle
ShpfileType CMapView::get_ShapefileType(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[layerHandle];
		if (layer->IsShapefile())
		{
			IShapefile* sf = nullptr;
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
Layer* CMapView::GetShapefileLayer(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		Layer* layer = _allLayers[layerHandle];
		if (layer->IsShapefile())
		{
			return layer;
		}
		else
		{
			this->ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return nullptr;
		}
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return nullptr;
	}
}

// ***************************************************************
//		get_Layer()
// ***************************************************************
Layer* CMapView::GetLayer(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		return _allLayers[layerHandle];
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
		return nullptr;
	}
}

// ***************************************************************
//		IsValidLayer()
// ***************************************************************
bool CMapView::IsValidLayer(long layerHandle)
{
	if (layerHandle >= 0 && layerHandle < (long)_allLayers.size())
	{
		return (_allLayers[layerHandle] != nullptr) ? true : false;
	}
	else
	{
		return false;
	}
}

// ***************************************************************
//		IsValidShape()
// ***************************************************************
bool CMapView::IsValidShape(long layerHandle, long shape)
{
	if (IsValidLayer(layerHandle))
	{
		Layer* l = _allLayers[layerHandle];
		if (l->IsShapefile())
		{
			this->AlignShapeLayerAndShapes(l);

			IShapefile* ishp = nullptr;
			l->QueryShapefile(&ishp);

			long numShapes = 0;
			if (ishp == nullptr)
			{
				numShapes = 0;
			}
			else
			{
				ishp->get_NumShapes(&numShapes);
				ishp->Release();
				ishp = nullptr;
			}

			if (shape >= 0 && shape < (long)numShapes)
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
			return options->fillVisible ? TRUE : FALSE;
		}
		else
		{
			return options->verticesVisible ? TRUE : FALSE;
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
		ScheduleLayerRedraw();
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
	if (newValue < 0.0)
		newValue = 0.0;

	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->pointSize = newValue;
		ScheduleLayerRedraw();
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
		return RGB(0, 0, 0);
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
		ScheduleLayerRedraw();
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
			return nullptr;
	}
	else
		return nullptr;
}

// *****************************************************************
//		SetUDPointType()
// *****************************************************************
void CMapView::SetUDPointType(long LayerHandle, LPDISPATCH newValue)
{
	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		IImage* iimg = nullptr;
		newValue->QueryInterface(IID_IImage, (void**)&iimg);
		ComHelper::SetRef((IDispatch*)iimg, (IDispatch**)&options->picture, true);
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
		ScheduleLayerRedraw();
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
		return RGB(0, 0, 0);
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
		ScheduleLayerRedraw();
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
	if (newValue < 0.0)
		newValue = 0.0;

	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->lineWidth = newValue;
		ScheduleLayerRedraw();
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
		ScheduleLayerRedraw();
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
		ScheduleLayerRedraw();
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
		return RGB(0, 0, 0);
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
		ScheduleLayerRedraw();
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
		return options->fillTransparency / 255.0f;
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
	if (newValue < 0.0)		newValue = 0.0;
	else if (newValue > 1.0)	newValue = 1.0;

	CDrawingOptionsEx* options = get_ShapefileDrawingOptions(LayerHandle);
	if (options)
	{
		options->fillTransparency = newValue * 255.0f;
		ScheduleLayerRedraw();
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
		ScheduleLayerRedraw();
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
		ScheduleLayerRedraw();
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
		return RGB(0, 0, 0);
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
		ScheduleLayerRedraw();
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
	return RGB(220, 220, 220);
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
	return RGB(220, 220, 220);
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
	return RGB(220, 220, 220);
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
	return RGB(220, 220, 220);
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
	return RGB(220, 220, 220);
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
	return nullptr;
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
void CMapView::set_UDPointFontCharFontSize(long LayerHandle, float FontSize)
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

// *************************************************************
//		SaveAsEx()
// *************************************************************
STDMETHODIMP CShapefile::SaveAsEx(const BSTR newFilename, const VARIANT_BOOL stopEditing, const VARIANT_BOOL unboundFile, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (unboundFile) {
		Save(nullptr, retVal);
		return S_OK;
	}

	SaveAs(newFilename, nullptr, retVal);

	if (retVal && stopEditing) {
		StopEditingShapes(VARIANT_FALSE, VARIANT_TRUE, nullptr, retVal);
	}

	return S_OK;
}
