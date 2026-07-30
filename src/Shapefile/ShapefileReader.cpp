#include "stdafx.h"
#include "Macros.h"
#include "ShapefileReader.h"
#include <afxmt.h>

// ****************************************************************
//		ReadShapefileIndex()
// ****************************************************************
// Fast reading for drawing procedure without bounds, file code, etc
bool CShapefileReader::ReadShapefileIndex(CStringW filename, FILE* shpFile, CCriticalSection* readLock)
{
	if (filename.GetLength() < 4)
	{
		return false;
	}

	// reading shape index (SHX)
	CStringW sFilename = filename;
	sFilename.SetAt(sFilename.GetLength() - 1, L'x');

	FILE* shxfile = _wfopen(sFilename, L"rb");

	if (!shxfile )
	{
		_shpfile = nullptr;
		_indexData = nullptr;

		USES_CONVERSION;
		CallbackHelper::ErrorMsg(Debug::Format("Failed to open SHX file: %s", OLE2A(sFilename)));

		return false;
	}

	fseek (shxfile, 0, SEEK_END);
	int indexFileSize = ftell(shxfile);
	rewind(shxfile);

	// 100 is for header
	fseek(shxfile, 100, SEEK_SET);
	_indexData = new char[indexFileSize - 100];
	auto result = fread(_indexData, sizeof(char), indexFileSize - 100, shxfile);
	fclose(shxfile);

	//_shpHeader.numShapes = (indexFileSize - 100)/8;	// 2 int on record

	_readLock = readLock;
	_shpfile = shpFile;
	rewind(shpFile);
	return true;
}

void SwapEndian(char* c)
{
	char ctmp;
	SWAP_LOCAL(c[0],c[3],ctmp);
	SWAP_LOCAL(c[1], c[2], ctmp);
}

// ****************************************************************
//		ReadShapeData()
// ****************************************************************
// Reads a single shape from the shapefile
char* CShapefileReader::ReadShapeData(int& offset, int& recordLength)
{
	recordLength = 0;

	// index records are 8 bytes;
	char* data = _indexData + offset*8;	
	SwapEndian(data);
	int readOffset =  (*reinterpret_cast<int*>(data)) * 2;

	data = _indexData + offset * 8 + 4;
	SwapEndian(data);
	int contentLength = (*reinterpret_cast<int*>(data));
	
	if (contentLength > 0)
	{
		CSingleLock lock(_readLock);
		lock.Lock();

		int ret = fseek(_shpfile, static_cast<long>(readOffset) + 2 * sizeof(int), SEEK_SET);
		if (ret != 0) return nullptr;
		
		// *2: for conversion from 16-bit words to 8-bit words
		int length = contentLength * 2;

		char* shapeData = new char[length];
		int count = static_cast<int>(fread(shapeData, sizeof(char), length, _shpfile));

		recordLength = length;
		return shapeData;
	}

	return nullptr;
}

// ****************************************************************
//		ReadPolyShape()
// ****************************************************************
PolygonData* CShapefileReader::ReadPolygonData(char* data)
{
	PolygonData* shapeData = new PolygonData();
	shapeData->partCount = *reinterpret_cast<int*>(data + 36);
	shapeData->pointCount = *reinterpret_cast<int*>(data + 40);
	shapeData->parts = reinterpret_cast<int*>(data + 44);
	shapeData->points = reinterpret_cast<double*>(data + 44 + sizeof(int) * shapeData->partCount);
	return shapeData;
}

PolygonData* CShapefileReader::ReadMultiPointData(char* data)
{
	PolygonData* shapeData = new PolygonData();
	shapeData->partCount = 0;
	shapeData->pointCount = *reinterpret_cast<int*>(data + 36);
	shapeData->parts = nullptr;
	shapeData->points = reinterpret_cast<double*>(data + 40);
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
