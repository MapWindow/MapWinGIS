#pragma once
#include "ShapeEditor.h"
class Digitizer
{
public:
    static void StartNewBoundShape(CShapeEditor* editor, ShpfileType shpType, IShapeDrawingOptions* options, long layerHandle);
	static bool OnMouseDown(CShapeEditor* editor, double projX, double projY, bool ctrl);
	
};

