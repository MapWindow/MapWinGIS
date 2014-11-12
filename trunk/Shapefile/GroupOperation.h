#pragma once
class GroupOperation
{
public:
	static bool Run(tkCursorMode cursorMode, long layerHandle, IShapefile* sf, IShape* polyline, IUndoList* undoList, int& errorCode);
	static bool SplitByPolyline(long layerHandle, IShapefile* sf, vector<long>& indices, IShape* polyline, IUndoList* undoList, int& errorCode);
	static bool ClipByPolygon(tkCursorMode cursorMode, long layerHandle, IShapefile* sf, vector<long>& indices, IShape*polygon, IUndoList* undoList, int& errorCode);
	static bool SplitByPolygon(long layerHandle, IShapefile* sf, vector<long>& indices, IShape*polygon, IUndoList* undoList, int& errorCode);
};

