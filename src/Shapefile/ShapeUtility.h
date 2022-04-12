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
	static ShpfileType Get25DShapeType(ShpfileType shpTypeBase, bool isZ, bool isM);
	static IShapeWrapper* CreateWrapper(char* data, int recordLength, bool forceCom);
	static IShapeWrapper* CreateWrapper(ShpfileType shpType, bool forceCom);
	static IShapeWrapper* CreateEmptyWrapper(bool forceCom);
	static ShapeWrapperType GetShapeWrapperType(ShpfileType shpType, bool forceCom);
	static void WriteBigEndian(FILE* file, int value);
	static long ReadIntBigEndian(FILE* file);

	static void WritePointXY(IShapeWrapper* shape, int pointIndex, FILE* file);
	static void WritePointXYM(IShapeWrapper* shape, int pointIndex, FILE* file);
	static void WritePointXYZ(IShapeWrapper* shape, int pointIndex, FILE* file);
	static void WritePointZ(IShapeWrapper* shape, int pointIndex, FILE* file);
	static void WritePointM(IShapeWrapper* shape, int pointIndex, FILE* file);
	static void WriteExtentsXY(IShapeWrapper* shape, FILE* file);
	static void WriteExtentsM(IShapeWrapper* shape, FILE* file);
	static void WriteExtentsZ(IShapeWrapper* shape, FILE* file);
};