#include "stdafx.h"
#include "EditorHelper.h"
#include "ShapeEditor.h"
#include "ShapefileHelper.h"

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
	ShpfileType shpType = ShapefileHelper::GetShapeType2D(sf);
	CopyOptionsFrom(editor, options, shpType);
}
void EditorHelper::CopyOptionsFrom(IShapeEditor* editor, IShapeDrawingOptions* options, ShpfileType shpType)
{
	if (!editor || !options) return;
	float lineWidth;
	OLE_COLOR fillColor, lineColor;
	options->get_FillColor(&fillColor);
	options->get_LineColor(&lineColor);
	options->get_LineWidth(&lineWidth);
	editor->put_FillColor(fillColor);
	
	if (Utility::ShapeTypeConvert2D(shpType) == SHP_POLYGON) {
		lineColor = Utility::ChangeBrightness(fillColor, -50);
		lineWidth = 1.0f;
	}
	editor->put_LineColor(lineColor);
	editor->put_LineWidth(lineWidth);
}

// *******************************************************
//		OnCursorChanged()
// *******************************************************
bool EditorHelper::OnCursorChanged(CShapeEditor* editor, bool clearEditor, tkCursorMode newCursor, bool& redrawNeeded)
{
	if (!editor) return true;

	if (clearEditor)
	{
		CComPtr<IShape> shp = NULL;
		editor->get_RawData(&shp);

		if (shp)
		{
			VARIANT_BOOL saved;
			editor->SaveChanges(&saved);
			if (!saved) return false;  // don't change cursor as user will loose unsaved changes

			redrawNeeded = true;
		}
		else {
			bool empty = EditorHelper::IsEmpty(editor);
			editor->Clear();
			if (!empty) {
				redrawNeeded = true;
			}
		}

		if (newCursor == cmAddShape) {
			editor->put_EditorState(esDigitize);
		}
	}

	if (EditorHelper::IsGroupOverlayCursor(newCursor))
	{
		editor->StartUnboundShape(newCursor);
	}
	return true;
}