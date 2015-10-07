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
bool ShapeUtility::IsM(ShpfileType shpType)
{
	return shpType == SHP_POINTM || shpType == SHP_MULTIPOINTM || shpType == SHP_POLYLINEM || shpType == SHP_POLYGONM;
}

// **************************************************************
//		ShapeTypeIsZ()
// **************************************************************
bool ShapeUtility::IsZ(ShpfileType shpType)
{
	return shpType == SHP_POINTZ || shpType == SHP_MULTIPOINTZ || shpType == SHP_POLYLINEZ || shpType == SHP_POLYGONZ;
}

// **************************************************************
//		Convert2D()
// **************************************************************
ShpfileType ShapeUtility::Convert2D(ShpfileType shpType)
{
	if (shpType == SHP_NULLSHAPE)																return SHP_NULLSHAPE;
	else if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ)				return SHP_POINT;
	else if (shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ)	return SHP_MULTIPOINT;
	else if (shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)			return SHP_POLYGON;
	else if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)		return SHP_POLYLINE;
	else																							return shpType;
}

// **************************************************************
//		SwapEndian()
// **************************************************************
void ShapeUtility::SwapEndian(char* a, int size)
{
	char hold;
	for (int i = 0; i < size*.5; i++)
	{
		hold = a[i];
		a[i] = a[size - i - 1];
		a[size - i - 1] = hold;
	}
}

// **************************************************************
//		get_ContentLength 
// **************************************************************
int ShapeUtility::get_ContentLength(ShpfileType shptype, int numPoints, int numParts)
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
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_POLYLINEZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_POLYLINEM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_POLYGON)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints;
	}
	else if (shptype == SHP_POLYGONZ)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_POLYGONM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(int) +
			sizeof(int)*numParts +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
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
			sizeof(double)*numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
	}
	else if (shptype == SHP_MULTIPOINTM)
	{
		contentLength = sizeof(int) +
			sizeof(double) * 4 +
			sizeof(int) +
			sizeof(double) * 2 * numPoints +
			sizeof(double) * 2 +
			sizeof(double)*numPoints;
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
IShapeWrapper* ShapeUtility::CreateFastWrapper(char* data)
{
	if (!data) {
		Debug::WriteError("CreateWrapper: shape data was expected.");
		return NULL;
	}

	ShpfileType shpType = (ShpfileType)*(int*)data;
	ShpfileType shpType2D = ShapeUtility::Convert2D(shpType);

	if (shpType == SHP_NULLSHAPE)
	{
		return new CShapeWrapperEmpty();
	}
	else if (shpType2D == SHP_POINT)
	{
		return new CShapeWrapperPoint(data);
	}
	else 
	{
		if (IsM(shpType) || IsZ(shpType)) {
			return new CShapeWrapperCOM(data);
		}
		
		return new CShapeWrapper(data);
	}
}

// **************************************************************
//		CreateWrapper 
// **************************************************************
IShapeWrapper* ShapeUtility::CreateWrapper(ShpfileType shpType, bool forceCom)
{
	ShapeWrapperType wrapperType = GetShapeWrapperType(shpType, forceCom);

	switch (wrapperType)
	{
		case swtEmpty:
			return new CShapeWrapperEmpty();
		case swtPoint:
			return new CShapeWrapperPoint(shpType);
		case swtFast:
			return new CShapeWrapper(shpType);
		case swtCom:
			return new CShapeWrapperCOM(shpType);
	}

	// let's return a stub for any unforeseen situations, it stands a better chance of avoiding a crash
	return new CShapeWrapperEmpty();
}

// **************************************************************
//		GetShapeWrapperType 
// **************************************************************
ShapeWrapperType ShapeUtility::GetShapeWrapperType(ShpfileType shpType, bool forceCom)
{
	if (forceCom) {
		return swtCom;
	}

	ShpfileType shpType2D = ShapeUtility::Convert2D(shpType);

	if (shpType == SHP_NULLSHAPE)
	{
		return swtEmpty;
	}
	else if (shpType2D == SHP_POINT)
	{
		return swtPoint;
	}
	else if (IsM(shpType) || IsZ(shpType)) {
		return swtCom;
	}
	else {
		return swtFast;
	}
}

// **************************************************************
//		CreateEmptyWrapper 
// **************************************************************
IShapeWrapper* ShapeUtility::CreateEmptyWrapper(bool forceCOM)
{
	if (forceCOM) {
		return new CShapeWrapperCOM(SHP_NULLSHAPE);
	}
	else {
		return new CShapeWrapperEmpty();
	}
}

// **************************************************************
//		WriteBigEndian 
// **************************************************************
void ShapeUtility::WriteBigEndian(FILE* file, int value)
{
	if (!file) return;

	void* intbuf = (char*)&value;
	ShapeUtility::SwapEndian((char*)intbuf, sizeof(int));

	size_t size = fwrite(intbuf, sizeof(int), 1, file);
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
	ShapeUtility::SwapEndian((char*)&buf, sizeof(int));
	return buf;
}

// **************************************************************
//		WritePointXY 
// **************************************************************
void ShapeUtility::WritePointXY(IShapeWrapper* shape, int pointIndex, FILE* file)
{
	double x, y;
	shape->get_PointXY(pointIndex, x, y);
	fwrite(&x, sizeof(double), 1, file);
	fwrite(&y, sizeof(double), 1, file);
}

// **************************************************************
//		WritePointXYM 
// **************************************************************
void ShapeUtility::WritePointXYM(IShapeWrapper* shape, int pointIndex, FILE* file)
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
void ShapeUtility::WritePointXYZ(IShapeWrapper* shape, int pointIndex, FILE* file)
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
void ShapeUtility::WritePointZ(IShapeWrapper* shape, int pointIndex, FILE* file)
{
	double z;
	shape->get_PointZ(pointIndex, z);
	fwrite(&z, sizeof(double), 1, file);
}

// **************************************************************
//		WritePointM 
// **************************************************************
void ShapeUtility::WritePointM(IShapeWrapper* shape, int pointIndex, FILE* file)
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
	shape->get_BoundsXY(xMin, yMin, xMax, yMax);
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

