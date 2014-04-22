#include "stdafx.h"
#include "Macros.h"
#include "ShapefileReader.h"

// ****************************************************************
//		ReadShapefileIndex()
// ****************************************************************
// Fast reading for drawing procedure without bounds, file code, etc
bool CShapefileReader::ReadShapefileIndex(CStringW filename, FILE* shpFile)
{
	//if (strlen(filename) < 4)
	if (filename.GetLength() < 4)
	{
		return false;
	}
	
	// reading shape index (SHX)
	CStringW sFilename = filename;
	sFilename.SetAt(sFilename.GetLength() - 1, L'x');

	FILE* shxfile = _wfopen(sFilename, L"rb");
	//delete[] sFilename;

	if (!shxfile )
	{
		// TODO: report error
		_shpfile = NULL;
		_indexData = NULL;
		return false;
	}
	else
	{
		fseek (shxfile, 0, SEEK_END);
		int indexFileSize = ftell(shxfile);
		rewind(shxfile);
		
		// 100 is for header
		fseek(shxfile, 100, SEEK_SET);
		_indexData = new char[indexFileSize - 100];
		long result = fread(_indexData, sizeof(char), indexFileSize - 100, shxfile);
		fclose(shxfile);

		//_shpHeader.numShapes = (indexFileSize - 100)/8;	// 2 int on record

		_shpfile = shpFile;
		rewind(shpFile);
		return true;
	}
}

void SwapEndian(char* c)
{
	char ctmp;
	SWAP(c[0],c[3],ctmp);
	SWAP(c[1],c[2],ctmp);
}

// ****************************************************************
//		ReadShapeData()
// ****************************************************************
// Reads a single shape from the shapefile
char* CShapefileReader::ReadShapeData(int& offset)
{
	// index records are 8 bytes;
	char* data = _indexData + offset*8;	
	SwapEndian(data);
	int readOffset =  (*(int*)data) * 2;

	data = _indexData + offset * 8 + 4;
	SwapEndian(data);
	int contentLength = (*(int*)data) * 2;
	
	if (contentLength > 0)
	{
		int ret = fseek(_shpfile, (long)readOffset + 2 * sizeof(int), SEEK_SET);
		if (ret != 0) return NULL;
		
		// *2: for conversion from 16-bit words to 8-bit words
		// -2: skip first 2 int - it's record number and content length;
		int length = contentLength * 2 - 2 * sizeof(int);

		char* shapeData = new char[length];
		int count = (int)fread(shapeData, sizeof(char), length, _shpfile);
		return shapeData;
	}
	else
	{
		return NULL;
	}
}

// ****************************************************************
//		ReadPolyShape()
// ****************************************************************
PolygonData* CShapefileReader::ReadPolygonData(char* data)
{
	PolygonData* shapeData = new PolygonData();
	shapeData->partCount = *(int*)(data + 36);
	shapeData->pointCount = *(int*)(data + 40);
	shapeData->parts = (int*)(data + 44);
	shapeData->points = (double*)(data + 44 + sizeof(int) * shapeData->partCount);
	return shapeData;
}

PolygonData* CShapefileReader::ReadMultiPointData(char* data)
{
	PolygonData* shapeData = new PolygonData();
	shapeData->partCount = 0;
	shapeData->pointCount = *(int*)(data + 36);
	shapeData->parts = NULL;
	shapeData->points = (double*)(data + 40);
	return shapeData;
}

//// ****************************************************************
////		ReadShape()
//// ****************************************************************
//// Reads the given shape from the shapefile and returns CShapeWrapper object
//CShapeWrapper* CShapefileReader::ReadShape(int ShapeIndex)
//{
//	char* data = this->ReadShapeData(ShapeIndex);
//	if (data)
//	{
//		CShapeWrapper* shp = new CShapeWrapper(data);
//		delete[] data;
//		return shp;
//	}
//	else
//		return NULL;
//}

//// ****************************************************************
////		ReadShape()
//// ****************************************************************
//// Reads the given shape from the shapefile and eeturns the CShapeWrapper
//// object, but only in case it falls in the given extents.
//CShapeWrapper* CShapefileReader::ReadShapeWithinExtents(int ShapeIndex, Extent& extents)
//{
//	char* data = this->ReadShapeData(ShapeIndex);
//	if (data)
//	{
//		CShapeWrapper* shp = new CShapeWrapper(data, &extents);
//		delete[] data;
//		return shp;
//	}
//	else
//		return NULL;
//}

//// ****************************************************************
////		get_ShapefileHeader()
//// ****************************************************************
//ShapefileHeader* CShapefileReader::get_ShapefileHeader()
//{
//	return &_shpHeader;
//}
