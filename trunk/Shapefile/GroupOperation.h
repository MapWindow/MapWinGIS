#pragma once
class GroupOperation
{
public:
	static bool SplitByPolyline(long layerHandle, IShapefile* sf, vector<long>& indices, IShape* polyline, IUndoList* undoList, int& errorCode);
};

