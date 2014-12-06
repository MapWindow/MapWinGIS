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
		return false;

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
					if (GeosHelper::Contains(gPoly, g))
					{
						if (cursorMode == cmEraseByPolygon)
							deleteList.push_back(indices[i]);
						// for other modes simply leave the untouched
					}
					else 
					{
						vector<GEOSGeometry*> results;

						GEOSGeometry* r = NULL;
						switch (cursorMode)
						{
							case cmEraseByPolygon:
								r = GeosHelper::Difference(g, gPoly);
								if (r) results.push_back(r);
								break;
							case cmClipByPolygon:
								r = GeosHelper::Intersection(g, gPoly);
								if (r) results.push_back(r);
								break;	
							case cmSplitByPolygon:
								r = GeosHelper::Intersection(g, gPoly);
								if (r) results.push_back(r);
								r = GeosHelper::Difference(g, gPoly);
								if (r) results.push_back(r);
								break;
						}
					
						int count = 0;
						for (size_t j = 0; j < results.size(); j++)
						{
							vector<IShape*> shapes;
							if (GeosConverter::GeomToShapes(results[j], &shapes, isM))
							{
								UndoListHelper::AddShapes(sf, shapes, layerHandle, undoList, indices[i], mwShapeIdIndex);
								success = true;
								count++;
							}
							GeosHelper::DestroyGeometry(results[j]);
						}

						if (count > 0 && cursorMode != cmClipByPolygon)
							deleteList.push_back(indices[i]);
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



