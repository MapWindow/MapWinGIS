#include "stdafx.h"
#include "LayerDrawer.h"
#include "ShapefileHelper.h"

// ****************************************************************
//		DrawCharts()
// ****************************************************************
void LayerDrawer::DrawCharts(Layer* layer, CChartDrawer& drawer, tkVerticalPosition position)
{
	if (!layer) return;

	CComPtr<IShapefile> sf = NULL;
	if (!layer->QueryShapefile(&sf)) return;
	
	CComPtr<ICharts> charts = NULL;
	sf->get_Charts(&charts);
	if (charts)
	{
		tkVerticalPosition vertPosition;
		charts->get_VerticalPosition(&vertPosition);
		if (vertPosition == position)
		{
			drawer.DrawCharts(sf);
		}
	}
}

// ****************************************************************
//		DrawLabels()
// ****************************************************************
void LayerDrawer::DrawLabels(Layer* layer, CLabelDrawer& drawer, tkVerticalPosition position)
{
	if (!layer) return;
	ILabels* labels = layer->get_Labels();
	if (labels != NULL)
	{
		tkVerticalPosition vertPos;
		labels->get_VerticalPosition(&vertPos);
		if (vertPos == position)
		{
			drawer.DrawLabels(labels);
		}
		labels->Release();
		labels = NULL;
	}
}

