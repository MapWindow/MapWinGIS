#pragma once
#include "ShapeInterfaces.h"

class ShapeUtility
{
public:
	static int get_ContentLength(ShpfileType shpType, int numPoints, int numParts);
	static void SwapEndian(char* a, int size);
	static bool IsM(ShpfileType shpType);
	static bool IsZ(ShpfileType shpType);
	static ShpfileType Convert2D(ShpfileType shpType);
	static IShapeWrapper* CreateFastWrapper(char* data);
	static IShapeWrapper* CreateWrapper(ShpfileType shpType, bool forceCOM);
	static IShapeWrapper* CreateEmptyWrapper(bool forceCOM);
	static ShapeWrapperType GetShapeWrapperType(ShpfileType shpType, bool forceCom);
};