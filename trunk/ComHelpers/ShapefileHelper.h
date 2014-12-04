#pragma once
#include "Shapefile.h"
class ShapefileHelper
{
public:
	static long GetMWShapeIdIndex(IShapefile* sf);
	static void CopyAttributes(IShapefile* sf, long sourceIndex, long targetIndex, long skipFieldIndex);
	static void CloneNoFields(IShapefile* sfSource, IShapefile** retVal, ShpfileType shpType, bool addShapeId = false);
	static void CloneNoFields(IShapefile* sf, IShapefile** retVal, bool addShapeId = false);
	static void CloneCore(IShapefile* sfSource, IShapefile** retVal, ShpfileType shpType, bool addShapeId = false);
	static bool GetSelectedExtents(IShapefile* sf, double& xMinRef, double& yMinRef, double& xMaxRef, double& yMaxRef);
	static void CopyFields(IShapefile* source, IShapefile* target);
	static vector<int>* GetSelectedIndices(IShapefile* sf);
	static long GetNumShapes(IShapefile* sf);
	static long GetNumFields(IShapefile* sf);
	static long GetNumSelected(IShapefile* sf);
	static void Rotate(IShapefile* sf, double originX, double originY, double angleDegree);
	static ShpfileType GetShapeType2D(IShapefile* sf);
	static ShpfileType GetShapeType(IShapefile* sf);
	static IShapefile* CloneSelection(IShapefile* sf);
	static bool ShapeSelected(IShapefile* sf, int shapeIndex);
	static bool GetClosestPoint(IShapefile* sf, double x, double y, double maxDistance, std::vector<long>& ids, long* shapeIndex, long* pointIndex, double& dist);
	static bool PointInPolygon(IShapefile* sf, long ShapeIndex, double x, double y);
	static bool BoundsWithinPolygon(IShapefile* sf, int shapeIndex, double b_minX, double b_minY, double b_maxX, double b_maxY);
	static bool ShapeTypeIsM(IShapefile* sf);
	static CStringW GetSymbologyFilename(IShapefile* sf);
	static CShapefile* Cast(CComPtr<IShapefile>& sf);
	static bool InteractiveEditing(IShapefile* sf);
};

