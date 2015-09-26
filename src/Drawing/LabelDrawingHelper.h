#pragma once
#include "LabelOptions.h"

class LabelDrawingHelper
{
public:
	static void CalcRotation(CLabelInfo* lbl, double mapRotation, double& angle);
	static void AlignRectangle(CRect& rect, tkLabelAlignment alignment);
	static void UpdateAutoOffset(CRect& rect, tkLabelAlignment align, int offset);
};

