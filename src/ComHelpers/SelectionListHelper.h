#pragma once
class SelectionListHelper
{
public:
	static long GetCount(ISelectionList* list);
	static bool GetUniqueLayers(ISelectionList* list, vector<long>& layerHandles);
	static void PopulateShapefile(ISelectionList* list, IShapefile* source, IShapefile* target, long layerHandle);
};

