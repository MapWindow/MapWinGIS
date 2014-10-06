// The old implementation of labels is stored here.
// The old properties are redirected to the new implementation where applicable.

#pragma once
#include "stdafx.h"
#include "Map.h"
#include "IndexSearching.h"
#include "LabelCategory.h"
#include "Labels.h"

#pragma region Properties
void CMapView::SetLineSeparationFactor(long sepFactor)
{
	_lineSeparationFactor = sepFactor;
}

long CMapView::GetLineSeparationFactor(void)
{
	//gReferenceCounter.WriteReport();
	// 0 is invalid, since it would result in no adjustments
    if (_lineSeparationFactor == 0) _lineSeparationFactor = 3;
	return _lineSeparationFactor;
}


// *************************************************************
//		GetOgrLayer()
// *************************************************************
IOgrLayer* CMapView::GetOgrLayer(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer * layer = _allLayers[LayerHandle];
		IOgrLayer* ogr = NULL;
		if (layer->QueryOgrLayer(&ogr))
			return ogr;
	}
	else
	{
		this->ErrorMessage(tkINVALID_LAYER_HANDLE);
	}
	return NULL;
}

// *************************************************************
//		GetShapefile()
// *************************************************************
IShapefile* CMapView::GetShapefile(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer * layer = _allLayers[LayerHandle];
		IShapefile* sf = NULL;
		if (layer->QueryShapefile(&sf))
			return sf;
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
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer * layer = _allLayers[LayerHandle];
		IImage* img = NULL;
		if (layer->QueryImage(&img))
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
	if (LayerHandle >= 0 && LayerHandle < (long)_allLayers.size())
	{
		Layer * layer = _allLayers[LayerHandle];
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
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetModifiedFlag();
}

// *************************************************************
//		GetLayerLabelsVisible()
// *************************************************************
BOOL CMapView::GetLayerLabelsVisible(long LayerHandle)
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

// *************************************************************
//		SetLayerLabelsVisible()
// *************************************************************
void CMapView::SetLayerLabelsVisible(long LayerHandle, BOOL bNewValue)
{
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		VARIANT_BOOL visible = (bNewValue == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
		labels->put_Visible(visible);
		labels->Release();
	}
}

// *************************************************************
//		GetLayerLabelsShadow()
// *************************************************************
BOOL CMapView::GetLayerLabelsShadow(long LayerHandle)
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

// *************************************************************
//		SetLayerLabelsShadow()
// *************************************************************
void CMapView::SetLayerLabelsShadow(long LayerHandle, BOOL newValue)
{
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		VARIANT_BOOL visible = (newValue == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
		labels->put_HaloVisible(visible);
		labels->Release();
	}
}

// **********************************************************
//		GetLayerLabelsScale()
// **********************************************************
BOOL CMapView::GetLayerLabelsScale(long LayerHandle)
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

// **********************************************************
//		SetLayerLabelsScale()
// **********************************************************
void CMapView::SetLayerLabelsScale(long LayerHandle, BOOL newValue)
{
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		VARIANT_BOOL scale = (newValue == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
		labels->put_ScaleLabels(scale);
		labels->Release();
	}
}

// **********************************************************
//		GetLayerLabelsOffset()
// **********************************************************
long CMapView::GetLayerLabelsOffset(long LayerHandle)
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

// **********************************************************
//		GetLayerLabelsOffset()
// **********************************************************
void CMapView::SetLayerLabelsOffset(long LayerHandle, long Offset)
{
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		double offset = (double)Offset;
		labels->put_OffsetX(offset);
		labels->Release();
	}
}

// **********************************************************
//		GetLayerLabelsShadowColor()
// **********************************************************
OLE_COLOR CMapView::GetLayerLabelsShadowColor(long LayerHandle)
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

// **********************************************************
//		SetLayerLabelsShadowColor()
// **********************************************************
void CMapView::SetLayerLabelsShadowColor(long LayerHandle, OLE_COLOR color)
{
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		labels->put_HaloColor(color);
		labels->Release();
	}
}

// **********************************************************
//		GetMultilineLabeling()
// **********************************************************
bool CMapView::GetMultilineLabeling()
{
	if (_shapeDrawingMethod == dmNewWithLabels || _shapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		//ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	// we shall save the value all the same
	return _multilineLabeling?true:false;
}

// **********************************************************
//		SetMultilineLabeling()
// **********************************************************
void CMapView::SetMultilineLabeling(bool Value)
{
	if (_shapeDrawingMethod == dmNewWithLabels || _shapeDrawingMethod == dmNewSymbology  || FORCE_NEW_LABELS)
	{
		ErrorMessage(tkPROPERTY_DEPRECATED);
	}
	// we shall save the value all the same
	_multilineLabeling = Value;
}

// **********************************************************
//		GetUseLabelCollision()
// **********************************************************
BOOL CMapView::GetUseLabelCollision(long LayerHandle)
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

// **********************************************************
//		SetUseLabelCollision()
// **********************************************************
void CMapView::SetUseLabelCollision(long LayerHandle, BOOL value)
{
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		VARIANT_BOOL avoid = (value == TRUE)?VARIANT_TRUE:VARIANT_FALSE;
		labels->put_AvoidCollisions(avoid);
		labels->Release();
	}
}

// **********************************************************
//		LayerFont()
// **********************************************************
void CMapView::LayerFont(long LayerHandle, LPCTSTR FontName, long FontSize)
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

// **********************************************************
//		LayerFontEx()
// **********************************************************
void CMapView::LayerFontEx(long LayerHandle, LPCTSTR FontName, long FontSize, BOOL isBold, BOOL isItalic, BOOL isUnderline)
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

// **********************************************************
//		LabelColor()
// **********************************************************
void CMapView::LabelColor(LONG LayerHandle, OLE_COLOR LabelFontColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		USES_CONVERSION;
		labels->put_FontColor(LabelFontColor);
		labels->Release();
	}
}

// **********************************************************
//		GetLayerStandardViewWidth()
// **********************************************************
void CMapView::GetLayerStandardViewWidth(long LayerHandle, double * Width)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	*Width = 0.0;
}

// **********************************************************
//		SetLayerStandardViewWidth()
// **********************************************************
void CMapView::SetLayerStandardViewWidth(long LayerHandle, double Width)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}
#pragma endregion

#pragma region Methods
// **********************************************************
//		AddLabel()
// **********************************************************
void CMapView::AddLabel(long LayerHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification)
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

// **********************************************************
//		AddLabelEx()
// **********************************************************
void CMapView::AddLabelEx(long LayerHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification, double Rotation)
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

// **********************************************************
//		ClearLabels()
// **********************************************************
void CMapView::ClearLabels(long LayerHandle)
{
	ILabels* labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		labels->Clear();
		labels->Release();
	}
}
#pragma endregion
