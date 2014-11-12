#include "stdafx.h"
#include "GroupOperation.h"
#include "ShapefileHelper.h"
#include "Shape.h"

// ***************************************************************
//	SplitByPolyline
// ***************************************************************
bool GroupOperation::SplitByPolyline(long layerHandle, IShapefile* sf, vector<long>& indices, 
		IShape* polyline, IUndoList* undoList, int& errorCode)
{
	if (!sf || !undoList || !polyline)  {
		errorCode = tkUNEXPECTED_NULL_PARAMETER;
		return false;
	}

	errorCode = tkNO_ERROR;

	vector<long> deleteList;

	VARIANT_BOOL vb;
	undoList->BeginBatch(&vb);
	if (!vb) {
		errorCode = tkCANT_START_BATCH_OPERATION;
		return false;
	}

	long mwShapeIdIndex = ShapefileHelper::GetMWShapeIdIndex(sf);

	bool split = false;
	for (long i = 0; i < (long)indices.size(); i++)
	{
		IShape* shp = NULL;
		sf->get_Shape(indices[i], &shp);
		if (shp)
		{
			vector<IShape*> shapes;
			if (((CShape*)shp)->SplitByPolylineCore(polyline, shapes))
			{
				for (size_t j = 0; j < shapes.size(); j++)
				{
					long shapeIndex;
					sf->EditAddShape(shapes[j], &shapeIndex);
					ShapefileHelper::CopyAttributes(sf, indices[i], shapeIndex, mwShapeIdIndex);

					undoList->Add(uoAddShape, layerHandle, shapeIndex, &vb);
					shapes[j]->Release();
				}
				deleteList.push_back(indices[i]);
				split = true;
			}
			shp->Release();
		}
	}

	for (int i = deleteList.size() - 1; i >= 0; i--)
	{
		undoList->Add(uoRemoveShape, layerHandle, deleteList[i], &vb);
		if (vb) {
			sf->EditDeleteShape(deleteList[i], &vb);
		}
	}

	long count;
	undoList->EndBatch(&count);

	return split;
}
