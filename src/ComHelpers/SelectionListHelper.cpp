#include "stdafx.h"
#include "SelectionListHelper.h"
#include <set>
#include "ShapefileHelper.h"

/***********************************************************************/
/*		GetCount()
/***********************************************************************/
long SelectionListHelper::GetCount(ISelectionList* list)
{
	if (!list) return 0;
	long count = 0;
	list->get_Count(&count);
	return count;
}

/***********************************************************************/
/*		GetUniqueLayers()
/***********************************************************************/
bool SelectionListHelper::GetUniqueLayers(ISelectionList* list, vector<long>& result)
{
	long count = GetCount(list);
	if (count == 0) return false;

	long layerHandle = -1;
	set<long> handles;
	
	for (long i = 0; i < count; i++)
	{
		list->get_LayerHandle(i, &layerHandle);
		if (handles.find(layerHandle) == handles.end())
		{
			handles.insert(layerHandle);
		}
	}
	result.assign(handles.begin(), handles.end());
	return true;
}

/***********************************************************************/
/*		PopulateShapefile()
/***********************************************************************/
void SelectionListHelper::PopulateShapefile(ISelectionList* list, IShapefile* source, IShapefile* target, long layerHandle)
{
	if (!source || !target || !list) return;

	ShpfileType shpType = ShapefileHelper::GetShapeType2D(source);

	VARIANT_BOOL vb;
	target->EditClear(&vb);
	target->CreateNew(m_globalSettings.emptyBstr, shpType, &vb);

	long count = GetCount(list);
	long handle, shapeIndex;

	for (long j = 0; j < count; j++)
	{
		list->get_LayerHandle(j, &handle);

		if (handle == layerHandle)
		{
			IShape* shp = NULL;
			list->get_ShapeIndex(j, &shapeIndex);
			source->get_Shape(shapeIndex, &shp);
			if (shp)
			{
				target->EditAddShape(shp, &shapeIndex);
				shp->Release();
			}
		}
	}
}
