#include "StdAfx.h"
#include "ShapeUtility.h"
#include "ShapeWrapperCOM.h"
#include "ShapeWrapperEmpty.h"
#include "ShapeWrapperPoint.h"
#include "ShapeWrapper.h"
#include "Shape.h"

// **************************************************************
//		IsM()
// **************************************************************
bool ShapeUtility::IsM(const ShpfileType shpType)
{
	return shpType == SHP_POINTM
		|| shpType == SHP_MULTIPOINTM
		|| shpType == SHP_POLYLINEM
		|| shpType == SHP_POLYGONM;
}

// **************************************************************
//		ShapeTypeIsZ()
// **************************************************************
bool ShapeUtility::IsZ(const ShpfileType shpType)
{
	return shpType == SHP_POINTZ
		|| shpType == SHP_MULTIPOINTZ
		|| shpType == SHP_POLYLINEZ
		|| shpType == SHP_POLYGONZ;
}

// **************************************************************
//		Convert2D()
// **************************************************************
ShpfileType ShapeUtility::Convert2D(const ShpfileType shpType)
{
	if (shpType == SHP_NULLSHAPE)																return SHP_NULLSHAPE;
	if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ)				    return SHP_POINT;
	if (shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ)	return SHP_MULTIPOINT;
	if (shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)			return SHP_POLYGON;
	if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)		return SHP_POLYLINE;
	return shpType;
}

// **************************************************************
//		SwapEndian()
// **************************************************************
void ShapeUtility::SwapEndian(char* a, const int size)
{
	for (int i = 0; i < size * .5; i++)
	{
		const char hold = a[i];
		a[i] = a[size - i - 1];
		a[size - i - 1] = hold;
	}
}

// **************************************************************
//		get_ContentLength 
// **************************************************************
int ShapeUtility::get_ContentLength(const ShpfileType shptype, const int numPoints, const int numParts)
{
	int contentLength;
	if (shptype == SHP_NULLSHAPE)
		contentLength = sizeof(int);		// type is stored
	else if (shptype == SHP_POINT)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 2;
	}
	else if (shptype == SHP_POINTZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4;
	}
	else if (shptype == SHP_POINTM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 3;
	}
	else if (shptype == SHP_POLYLINE)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int) * numParts +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_POLYLINEZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int) * numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints;
	}
	else if (shptype == SHP_POLYLINEM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int) * numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints;
	}
	else if (shptype == SHP_POLYGON)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int) * numParts +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_POLYGONZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int) * numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints;
	}
	else if (shptype == SHP_POLYGONM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int) * numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints;
	}
	else if (shptype == SHP_MULTIPOINT)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_MULTIPOINTZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints;
	}
	else if (shptype == SHP_MULTIPOINTM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double) * numPoints;
	}
	else
	{
		return 0;
	}
	return contentLength;
}

// **************************************************************
//		CreateFastWrapper 
// **************************************************************
IShapeWrapper* ShapeUtility::CreateWrapper(char* data, const int recordLength, const bool forceCom)
{
	if (!data) {
		Debug::WriteError("CreateWrapper: shape data was expected.");
		return nullptr;
	}

	ShpfileType shpType = (ShpfileType) * (int*)data;  // TODO: Fix compile warning
	//ShpfileType shpType2D = ShapeUtility::Convert2D(shpType);

	switch (GetShapeWrapperType(shpType, forceCom))
	{
	case ShapeWrapperType::swtPoint:
		return new CShapeWrapperPoint(data, recordLength);
	case ShapeWrapperType::swtFast:
		return new CShapeWrapper(data, recordLength);
	case ShapeWrapperType::swtCom:
		return new CShapeWrapperCOM(data, recordLength);
	case ShapeWrapperType::swtEmpty:
		return new CShapeWrapperEmpty();
	}

	// let's return a stub for any unforeseen situations, it stands a better chance of avoiding a crash
	return new CShapeWrapperEmpty();
}

// **************************************************************
//		CreateWrapper 
// **************************************************************
IShapeWrapper* ShapeUtility::CreateWrapper(const ShpfileType shpType, const bool forceCom)
{
	switch (GetShapeWrapperType(shpType, forceCom))
	{
	case ShapeWrapperType::swtEmpty:
		return new CShapeWrapperEmpty();
	case ShapeWrapperType::swtPoint:
		return new CShapeWrapperPoint(shpType);
	case ShapeWrapperType::swtFast:
		return new CShapeWrapper(shpType);
	case ShapeWrapperType::swtCom:
		return new CShapeWrapperCOM(shpType);
	}

	// let's return a stub for any unforeseen situations, it stands a better chance of avoiding a crash
	return new CShapeWrapperEmpty();
}

// **************************************************************
//		GetShapeWrapperType 
// **************************************************************
ShapeWrapperType ShapeUtility::GetShapeWrapperType(const ShpfileType shpType, const bool forceCom)
{
	if (forceCom) {
		return ShapeWrapperType::swtCom;
	}

	const ShpfileType shpType2D = ShapeUtility::Convert2D(shpType);

	if (shpType == SHP_NULLSHAPE)
	{
		return ShapeWrapperType::swtEmpty;
	}
	if (shpType2D == SHP_POINT)
	{
		return ShapeWrapperType::swtPoint;
	}
	if (IsM(shpType) || IsZ(shpType)) {
		return ShapeWrapperType::swtCom;
	}
	return ShapeWrapperType::swtFast;
}

// **************************************************************
//		CreateEmptyWrapper 
// **************************************************************
IShapeWrapper* ShapeUtility::CreateEmptyWrapper(const bool forceCom)
{
	if (forceCom) {
		return new CShapeWrapperCOM(SHP_NULLSHAPE);
	}
	return new CShapeWrapperEmpty();
}

// **************************************************************
//		WriteBigEndian 
// **************************************************************
void ShapeUtility::WriteBigEndian(FILE* file, int value)
{
	if (!file) return;

	void* intbuf = (char*)&value; // TODO: Fix compile warning
	ShapeUtility::SwapEndian((char*)intbuf, sizeof(int)); // TODO: Fix compile warning

	const size_t size = fwrite(intbuf, sizeof(int), 1, file);
	if (size != 1)
	{
		CallbackHelper::ErrorMsg("Failed to write int value");
	}
}

// **************************************************************
//		ReadIntBigEndian 
// **************************************************************
long ShapeUtility::ReadIntBigEndian(FILE* file)
{
	int buf;
	fread(&buf, sizeof(int), 1, file);
	ShapeUtility::SwapEndian((char*)&buf, sizeof(int)); // TODO: Fix compile warning
	return buf;
}

// **************************************************************
//		WritePointXY 
// **************************************************************
void ShapeUtility::WritePointXY(IShapeWrapper* shape, const int pointIndex, FILE* file)
{
	double x, y;
	shape->get_PointXY(pointIndex, x, y);
	fwrite(&x, sizeof(double), 1, file);
	fwrite(&y, sizeof(double), 1, file);
}

// **************************************************************
//		WritePointXYM 
// **************************************************************
void ShapeUtility::WritePointXYM(IShapeWrapper* shape, const int pointIndex, FILE* file)
{
	double x, y, m, z;
	shape->get_PointXYZM(pointIndex, x, y, z, m);
	fwrite(&x, sizeof(double), 1, file);
	fwrite(&y, sizeof(double), 1, file);
	fwrite(&m, sizeof(double), 1, file);
}

// **************************************************************
//		WritePointXYZ 
// **************************************************************
void ShapeUtility::WritePointXYZ(IShapeWrapper* shape, const int pointIndex, FILE* file)
{
	double x, y, m, z;
	shape->get_PointXYZM(pointIndex, x, y, z, m);
	fwrite(&x, sizeof(double), 1, file);
	fwrite(&y, sizeof(double), 1, file);
	fwrite(&z, sizeof(double), 1, file);
	fwrite(&m, sizeof(double), 1, file);
}

// **************************************************************
//		WritePointZ 
// **************************************************************
void ShapeUtility::WritePointZ(IShapeWrapper* shape, const int pointIndex, FILE* file)
{
	double z;
	shape->get_PointZ(pointIndex, z);
	fwrite(&z, sizeof(double), 1, file);
}

// **************************************************************
//		WritePointM 
// **************************************************************
void ShapeUtility::WritePointM(IShapeWrapper* shape, const int pointIndex, FILE* file)
{
	double m;
	shape->get_PointM(pointIndex, m);
	fwrite(&m, sizeof(double), 1, file);
}

// **************************************************************
//		WriteExtentsXY 
// **************************************************************
void ShapeUtility::WriteExtentsXY(IShapeWrapper* shape, FILE* file)
{
	double xMin, yMin, xMax, yMax;
	shape->get_BoundsXY(xMin, xMax, yMin, yMax);
	fwrite(&xMin, sizeof(double), 1, file);
	fwrite(&yMin, sizeof(double), 1, file);
	fwrite(&xMax, sizeof(double), 1, file);
	fwrite(&yMax, sizeof(double), 1, file);
}

// **************************************************************
//		WriteExtentsM 
// **************************************************************
void ShapeUtility::WriteExtentsM(IShapeWrapper* shape, FILE* file)
{
	double xMin, yMin, xMax, yMax, zMin, zMax, mMin, mMax;
	shape->get_Bounds(xMin, yMin, xMax, yMax, zMin, zMax, mMin, mMax);
	fwrite(&mMin, sizeof(double), 1, file);
	fwrite(&mMax, sizeof(double), 1, file);
}

// **************************************************************
//		WriteExtentsZ 
// **************************************************************
void ShapeUtility::WriteExtentsZ(IShapeWrapper* shape, FILE* file)
{
	double xMin, yMin, xMax, yMax, zMin, zMax, mMin, mMax;
	shape->get_Bounds(xMin, yMin, xMax, yMax, zMin, zMax, mMin, mMax);
	fwrite(&zMin, sizeof(double), 1, file);
	fwrite(&zMax, sizeof(double), 1, file);
}

// **************************************************************
//		Get25DShapeType 
// **************************************************************
ShpfileType ShapeUtility::Get25DShapeType(const ShpfileType shpTypeBase, const bool isZ, const bool isM)
{
	switch (shpTypeBase)
	{
	case SHP_POINT:
		if (isZ) return SHP_POINTZ;
		if (isM) return SHP_POINTM;
		return SHP_POINT;
	case SHP_MULTIPOINT:
		if (isZ) return SHP_MULTIPOINTZ;
		if (isM) return SHP_MULTIPOINTM;
		return SHP_MULTIPOINT;
	case SHP_POLYLINE:
		if (isZ) return SHP_POLYLINEZ;
		if (isM) return SHP_POLYLINEM;
		return SHP_POLYLINE;
	case SHP_POLYGON:
		if (isZ) return SHP_POLYGONZ;
		if (isM) return SHP_POLYGONM;
		return SHP_POLYGON;
	default: 
		return shpTypeBase;
	}
}

