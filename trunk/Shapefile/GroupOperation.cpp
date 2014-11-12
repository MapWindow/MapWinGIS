#include "stdafx.h"
#include "GroupOperation.h"
#include "ShapefileHelper.h"
#include "Shape.h"
#include "SelectionHelper.h"
#include "ShapeHelper.h"
#include "GeosConverter.h"
#include "GeosHelper.h"
#include "UndoListHelper.h"

// ***************************************************************
//	Run
// ***************************************************************
bool GroupOperation::Run(tkCursorMode cursorMode, long layerHandle, IShapefile* sf, IShape* shp, IUndoList* undoList, int& errorCode)
{
	if (!sf || !undoList || !shp)  {
		errorCode = tkUNEXPECTED_NULL_PARAMETER;
		return false;
	}

	vector<long> indices;
	if (!SelectionHelper::SelectWithShapeBounds(sf, shp, indices))
		return false;    // TODO: fire event

	errorCode = tkNO_ERROR;

	VARIANT_BOOL vb;
	undoList->BeginBatch(&vb);
	if (!vb) {
		errorCode = tkCANT_START_BATCH_OPERATION;
		return false;
	}

	bool result = false;
	switch (cursorMode)
	{
		case cmSplitByPolyline:
			result = SplitByPolyline(layerHandle, sf, indices, shp, undoList, errorCode);
			break;	
		case cmSplitByPolygon:
			result = SplitByPolygon(layerHandle, sf, indices, shp, undoList, errorCode);
			break;
		case cmEraseByPolygon:
		case cmClipByPolygon:
			result = ClipByPolygon(cursorMode, layerHandle, sf, indices, shp, undoList, errorCode);
	}

	long count;
	undoList->EndBatch(&count);
	return result;
}

// ***************************************************************
//	EraseByPolygon
// ***************************************************************
bool GroupOperation::ClipByPolygon(tkCursorMode cursorMode, long layerHandle, IShapefile* sf, vector<long>& indices, IShape*polygon, IUndoList* undoList, int& errorCode)
{
	GEOSGeometry* gPoly = GeosConverter::ShapeToGeom(polygon);
	if (!gPoly) {
		errorCode = tkCANT_CONVERT_SHAPE_GEOS;
		return false;
	}

	vector<long> deleteList;
	if (cursorMode == cmClipByPolygon)
	{
		// all the shapes in this mode are to be removed
		long numShapes = ShapefileHelper::GetNumShapes(sf);
		deleteList.reserve(numShapes);
		for (long i = 0; i < numShapes; i++) {
			deleteList.push_back(i);
		}
	}

	bool isM = ShapefileHelper::ShapeTypeIsM(sf);
	long mwShapeIdIndex = ShapefileHelper::GetMWShapeIdIndex(sf);

	bool success = false;
	for (long i = 0; i < (long)indices.size(); i++)
	{
		CComPtr<IShape> shp = NULL;
		sf->get_Shape(indices[i], &shp);
		if (shp) 
		{
			GEOSGeometry* g = GeosConverter::ShapeToGeom(shp);
			if (g) 
			{
				if (GeosHelper::Intersects(g, gPoly)) 
				{
					GEOSGeometry* r = NULL;
					switch (cursorMode)
					{
						case cmEraseByPolygon:
							r = GeosHelper::Difference(g, gPoly);
							break;
						case cmClipByPolygon:
							r = GeosHelper::Intersection(g, gPoly);
							break;
					}
					
					if (r) {
						vector<IShape*> shapes;
						if (GeosConverter::GeomToShapes(r, &shapes, isM))
						{
							UndoListHelper::AddShapes(sf, shapes, layerHandle, undoList, indices[i], mwShapeIdIndex);
							success = true;
							if (cursorMode == cmEraseByPolygon)
								deleteList.push_back(indices[i]);
						}
						GeosHelper::DestroyGeometry(r);
					}
				}
				GeosHelper::DestroyGeometry(g);
			}
		}
	}
	GeosHelper::DestroyGeometry(gPoly);

	UndoListHelper::DeleteShapes(sf, deleteList, layerHandle, undoList);

	return success;
}

// ***************************************************************
//	SplitByPolygon
// ***************************************************************
bool GroupOperation::SplitByPolygon(long layerHandle, IShapefile* sf, vector<long>& indices, IShape*polygon, IUndoList* undoList, int& errorCode)
{
	// TODO: implement
	return false;
}

// ***************************************************************
//	SplitByPolyline
// ***************************************************************
bool GroupOperation::SplitByPolyline(long layerHandle, IShapefile* sf, vector<long>& indices, 
		IShape* polyline, IUndoList* undoList, int& errorCode)
{
	vector<long> deleteList;

	long mwShapeIdIndex = ShapefileHelper::GetMWShapeIdIndex(sf);

	bool split = false;
	for (long i = 0; i < (long)indices.size(); i++)
	{
		CComPtr<IShape> shp = NULL;
		sf->get_Shape(indices[i], &shp);
		if (shp)
		{
			vector<IShape*> shapes;
			if (ShapeHelper::SplitByPolyline(shp, polyline, shapes))
			{
				UndoListHelper::AddShapes(sf, shapes, layerHandle, undoList, indices[i], mwShapeIdIndex);
				deleteList.push_back(indices[i]);
				split = true;
			}
		}
	}

	UndoListHelper::DeleteShapes(sf, deleteList, layerHandle, undoList);
	return split;
}



