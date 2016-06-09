#pragma once
#include "Layer.h"
#include "ChartDrawing.h"
#include "ShapefileDrawing.h"
#include "ImageDrawing.h"
#include "LabelDrawing.h"
class LayerDrawer
{
public:
	static void DrawCharts(Layer* layer, CChartDrawer& drawer, tkVerticalPosition position);
	static void DrawLabels(Layer* layer, CLabelDrawer& drawer, tkVerticalPosition position);
};

