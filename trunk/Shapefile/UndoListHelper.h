#pragma once
class UndoListHelper
{
public:
	static void AddShapes(IShapefile* sf, vector<IShape*> shapes, long layerHandle, IUndoList* undoList, int sourceShapeIndex, int skipField);
	static void DeleteShapes(IShapefile* sf, vector<long>& deleteList, long layerHandle, IUndoList* undoList);
};

