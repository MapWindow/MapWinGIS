// The old implementation of labels is stored here.
// The old properties are redirected to the new implementation where applicable.

#include "stdafx.h"
#include "IndexSearching.h"
#include "Labels.h"
#include "Map.h"
#include "Shapefile.h"

// *************************************************************
//		LineSeparationFactor()
// *************************************************************
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
//		GetWmsLayer()
// *************************************************************
IWmsLayer* CMapView::GetWmsLayer(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	Layer* layer = GetLayer(LayerHandle);
	if (!layer) return nullptr;

	IWmsLayer* wms = nullptr;
	if (layer->QueryWmsLayer(&wms)) {
		return wms;
	}

	return nullptr;
}

// *************************************************************
//		GetOgrLayer()
// *************************************************************
IOgrLayer* CMapView::GetOgrLayer(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	Layer* layer = GetLayer(LayerHandle);
	if (!layer) return nullptr;

	IOgrLayer* ogr = nullptr;
	if (layer->QueryOgrLayer(&ogr)) {
		return ogr;
	}

	return nullptr;
}

// *************************************************************
//		GetShapefile()
// *************************************************************
IShapefile* CMapView::GetShapefile(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	Layer* layer = GetLayer(LayerHandle);
	if (!layer) return nullptr;

	IShapefile* sf = nullptr;
	if (layer->QueryShapefile(&sf)) {
		return sf;
	}

	return nullptr;
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

	Layer* layer = GetLayer(LayerHandle);
	if (!layer) return nullptr;

	IImage* img = nullptr;
	if (layer->QueryImage(&img))
	{
		return img;
	}

	return nullptr;
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
	Layer* layer = GetLayer(LayerHandle);
	if (!layer) return nullptr;

	return layer->get_Labels();
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
		return visible == VARIANT_TRUE ? TRUE : FALSE;
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
		VARIANT_BOOL visible = (bNewValue == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
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
		return visible == VARIANT_TRUE ? TRUE : FALSE;
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
		VARIANT_BOOL visible = (newValue == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
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
		return scale == VARIANT_TRUE ? TRUE : FALSE;
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
		VARIANT_BOOL scale = (newValue == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
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
	return _multilineLabeling ? true : false;
}

// **********************************************************
//		SetMultilineLabeling()
// **********************************************************
void CMapView::SetMultilineLabeling(bool Value)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
	_multilineLabeling = Value;		// we shall save the value all the same
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
		return (value == VARIANT_TRUE) ? TRUE : FALSE;
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
		VARIANT_BOOL avoid = (value == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
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
		CComBSTR name(FontName);
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
		CComBSTR name(FontName);
		labels->put_FontName(name);
		labels->put_FontSize(FontSize);

		VARIANT_BOOL bold = (isBold == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
		VARIANT_BOOL italic = (isItalic == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
		VARIANT_BOOL underline = (isUnderline == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;

		labels->put_FontBold(bold);
		labels->put_FontItalic(italic);
		labels->put_FontUnderline(underline);

		labels->Release();
	}
}

// **********************************************************
//		LabelColor()
// **********************************************************
void CMapView::LabelColor(LONG layerHandle, OLE_COLOR labelFontColor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ILabels* labels = GetLayerLabels(layerHandle);
	if (labels)
	{
		USES_CONVERSION;
		labels->put_FontColor(labelFontColor);
		labels->Release();
	}
}

// **********************************************************
//		GetLayerStandardViewWidth()
// **********************************************************
void CMapView::GetLayerStandardViewWidth(long LayerHandle, double* Width)
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
	CComPtr<ILabels> labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		CComBSTR s(Text);
		labels->AddLabel(s, x, y);
	}
}

// **********************************************************
//		AddLabelEx()
// **********************************************************
void CMapView::AddLabelEx(long LayerHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification, double Rotation)
{
	CComPtr<ILabels> labels = GetLayerLabels(LayerHandle);
	if (labels)
	{
		CComBSTR s(Text);
		labels->AddLabel(s, x, y, Rotation);
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

// **********************************************************
//			ClearDrawingLabelFrames()
// **********************************************************
void CMapView::ClearDrawingLabelFrames()
{
	// clear frames for drawing labels
	for (size_t j = 0; j < _activeDrawLists.size(); j++)
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
		if (isSkip)
			continue;

		DrawList* dlist = _allDrawLists[_activeDrawLists[j]];
		if (IS_VALID_PTR(dlist))
		{
			CLabels* coLabels = static_cast<CLabels*>(dlist->m_labels);
			coLabels->ClearLabelFrames();
		}
	}
}

// **********************************************************
//			ClearLabelFrames()
// **********************************************************
void CMapView::ClearLabelFrames()
{
	// clear frames for regular labels
	for (int i = 0; i < (int)_activeLayers.size(); i++)
	{
		Layer* l = _allLayers[_activeLayers[i]];
		if (l != nullptr)
		{
			if (l->IsShapefile())
			{
				IShapefile* sf = nullptr;
				if (l->QueryShapefile(&sf))
				{
					((CShapefile*)sf)->ClearChartFrames();
					sf->Release();
				}
			}

			// labels
			ILabels* LabelsClass = l->get_Labels();
			if (LabelsClass == nullptr) continue;

			CLabels* coLabels = static_cast<CLabels*>(LabelsClass);
			coLabels->ClearLabelFrames();
			LabelsClass->Release(); LabelsClass = nullptr;
		}
	}
}