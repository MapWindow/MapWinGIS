#include "stdafx.h"
#include "EditorHelper.h"

// ************************************************************
//		GetShapeType2D
// ************************************************************
ShpfileType EditorHelper::GetShapeType2D(IShapeEditor* editor)
{
	if (!editor) return SHP_NULLSHAPE;
	ShpfileType shpType;
	editor->get_ShapeType(&shpType);
	return Utility::ShapeTypeConvert2D(shpType);
}

// ************************************************************
//		IsEmpty
// ************************************************************
bool EditorHelper::IsEmpty(IShapeEditor* editor)
{
	if (!editor) return true;
	VARIANT_BOOL isEmpty;
	editor->get_IsEmpty(&isEmpty);
	return isEmpty ? true : false;
}

// ************************************************************
//		IsGroupOverlayCursor
// ************************************************************
bool EditorHelper::IsGroupOverlayCursor(tkCursorMode cursorMode)
{
	return cursorMode == cmSplitByPolyline || cursorMode == cmSplitByPolygon ||
		cursorMode == cmEraseByPolygon || cursorMode == cmClipByPolygon ||
		cursorMode == cmSelectByPolygon;
}

// ************************************************************
//		IsDigitizingCursor
// ************************************************************
bool EditorHelper::IsDigitizingCursor(tkCursorMode mode)
{
	return mode == cmAddShape /*|| mode == cmEditShape*/ || IsGroupOverlayCursor(mode);
}

// *******************************************************
//		CopyOptionsFrom()
// *******************************************************
void EditorHelper::CopyOptionsFrom(IShapeEditor* editor, IShapefile* sf)
{
	if (!sf || !editor) return;
	CComPtr<IShapeDrawingOptions> options = NULL;
	sf->get_DefaultDrawingOptions(&options);
	CopyOptionsFrom(editor, options);
}
void EditorHelper::CopyOptionsFrom(IShapeEditor* editor, IShapeDrawingOptions* options)
{
	if (!editor || !options) return;
	float lineWidth;
	OLE_COLOR fillColor, lineColor;
	options->get_FillColor(&fillColor);
	options->get_LineColor(&lineColor);
	options->get_LineWidth(&lineWidth);
	editor->put_FillColor(fillColor);
	editor->put_LineColor(lineColor);
	editor->put_LineWidth(lineWidth);
}