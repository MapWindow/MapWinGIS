#pragma once
#include <set>

class SelectionListHelper
{
public:
	static long GetCount(ISelectionList* list);
	static bool GetUniqueLayers(ISelectionList* list, vector<long>& layerHandles);
	static void PopulateShapefile(ISelectionList* list, IShapefile* source, IShapefile* target, long layerHandle);
	static bool HasLayers(ISelectionList* list, tkLayerType layerTypeToSearch);
	static void AddSelectedRasterPixels(ISelectionList* list, IShapefile* target, set<long>& layerHandles);
};

