#include "stdafx.h"
#include "UndoListHelper.h"
#include "ShapefileHelper.h"

// ********************************************************************
//		AddShapes()
// ********************************************************************
void UndoListHelper::AddShapes(IShapefile* sf, vector<IShape*> shapes, long layerHandle, IUndoList* undoList, int sourceShapeIndex, int skipField)
{
	VARIANT_BOOL vb;
	for (size_t j = 0; j < shapes.size(); j++)
	{
		long shapeIndex;
		sf->EditAddShape(shapes[j], &shapeIndex);
		ShapefileHelper::CopyAttributes(sf, sourceShapeIndex, shapeIndex, skipField);
		undoList->Add(uoAddShape, layerHandle, shapeIndex, &vb);
		shapes[j]->Release();
	}
}

// ********************************************************************
//		DeleteShapes()
// ********************************************************************
void UndoListHelper::DeleteShapes(IShapefile* sf, vector<long>& deleteList, long layerHandle, IUndoList* undoList)
{
	VARIANT_BOOL vb;
	for (int i = deleteList.size() - 1; i >= 0; i--)
	{
		undoList->Add(uoRemoveShape, layerHandle, deleteList[i], &vb);
		if (vb) {
			sf->EditDeleteShape(deleteList[i], &vb);
		}
	}
}