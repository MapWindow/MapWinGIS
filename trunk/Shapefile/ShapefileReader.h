#pragma once
#include "ShapeWrapper.h"

//Shapefile File Info
#define HEADER_BYTES_16 50
#define HEADER_BYTES_32 100
#define FILE_CODE      9994
#define VERSION        1000
#define UNUSEDSIZE 5
#define UNUSEDVAL		0     	    
#define RECORD_HEADER_LENGTH_32 8
#define RECORD_SHAPE_TYPE_32 8

struct PolygonData
{
public:	
	double* points;
	int* parts;
	int pointCount;
	int partCount;
};

// ---------------------------------------------------------
//   class to encapsulate reading of the shapefile
// ---------------------------------------------------------
class CShapefileReader
{
public:
	CShapefileReader()
	{
		_indexData = NULL;
		_shpfile = NULL;
		_dataLoaded = false;
	}

	~CShapefileReader()
	{
		if (_indexData)
		{
			delete[] _indexData;
		}
	};

private:
	bool _dataLoaded;
	char * _indexData;			// content of shx file
	FILE * _shpfile;

public:
	// functions
	bool ReadShapefileIndex(const char* filename, FILE* shpFile);
	char* ReadShapeData(int& offset);
	PolygonData* ReadPolygonData(char* data);
	PolygonData* ReadMultiPointData(char* data);
};
