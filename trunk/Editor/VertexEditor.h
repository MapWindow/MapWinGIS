#pragma once
#include "ShapeEditor.h"

// Managing of cmEditShape cursor
class VertexEditor
{
public:
	static bool OnMouseDown(IMapViewCallback* map, CShapeEditor* editor, double projX, double projY, bool ctrl);
	static void StartEdit(CShapeEditor* editor, long layerHandle, long shapeIndex);
private:
	static bool OnMouseDownEditing(IMapViewCallback* map, CShapeEditor* editor, double projX, double projY, bool ctrl);
};

