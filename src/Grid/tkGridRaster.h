//********************************************************************************************************
//File name: tkGridRaster.h
//Description:  Generic grid wrapper class to allow MapWinGIs to utilize formats supported by GDAL
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Christopher Michaelis.
//
//Contributor(s): (Open source contributors should list themselves and their modifications here).
//1-10-2006 - 1-18-2006 - cdm -- Initial Revision
//********************************************************************************************************

#if !defined(tkGridRaster_H)
#define tkGridRaster_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma pack(push, gif2, 1)

#include "grdTypes.h"
#include "HashTable.h"
#include "ImageStructs.h"
#include "colour.h"

class tkGridRaster
{
public:
	tkGridRaster() :_plotloc(0), _blocksize(0), _numread(0), _bitshift(0),
		_buffSize(0), _dX(-1), _dY(-1),
		_xllCenter(-1), _yllCenter(-1)
	{		//Rob Cairns
		_filename = L"";
		_rasterDataset = NULL;
		_floatbuffer = NULL;
		_int32buffer = NULL;
		_int32ScanlineBufferA = NULL;
		_floatScanlineBufferA = NULL;
		_int32ScanlineBufferB = NULL;
		_floatScanlineBufferB = NULL;
		_noDataValue = static_cast<double>(-3.40282346638529E+38);
		_canScanlineBuffer = false;
		_scanlineBufferNumberA = -1;
		_scanlineBufferNumberB = -1;
		_cachedMax = -9999;
		_cachedMin = 9999;
		_genericType = GDT_Unknown;
		_hasColorTable = false;
		_activeBandIndex = 1;
	}

	virtual ~tkGridRaster()
	{
		Close(); // Free all memory used in Close.
	}

private:
	
	const static long MAX_INRAM_SIZE = 536870912;
	
	//world coordinate related variables
	double _dY;			//change in Y (for size of cell or pixel)
	double _dX;			//change in X (for size of cell or pixel)
	double _yllCenter;	//Y coordinate of lower left corner of image (world coordinate)
	double _xllCenter;	//X coordinate of lower left corner of image (world coordinate)

	int _activeBandIndex;
	_int32 * _int32buffer;
	float * _floatbuffer;

	_int32 * _int32ScanlineBufferA;
	float * _floatScanlineBufferA;
	long _scanlineBufferNumberA;
	_int32 * _int32ScanlineBufferB;
	float * _floatScanlineBufferB;
	long _scanlineBufferNumberB;
	bool _canScanlineBuffer;
	bool _scanlineADataChanged;
	bool _scanlineBDataChanged;
	char _scanlineLastAccessed;

	bool _inRam;

	GDALDataset * _rasterDataset;
	GDALColorTable * _poColorT;
	bool _hasColorTable;
	int _buffSize;
	
	int _colorMapSize;
	ColorEntry _colorMap[4096];
	long _width;
	long _height;
	int _plotloc;
	
	int _blocksize;
	int _numread;
	int _bitshift;
	FILE * _file;
	int _codesize;
	int _isInterlaced;

	GDALColorInterp _cIntp;
	GDALDataType _dataType;
	GDALDataType _genericType;
	int _nBands;
	GDALRasterBand  *_poBand;
	GridFileType _currentFileType;

	unsigned char _block[256];

	HashTable * _ht;

	double _cachedMax;
	double _cachedMin;
	CStringW _filename;

	double _noDataValue;
	CString _projection;

private:
	inline bool inColorMap(colort c);
	void LoadFullBuffer();
	bool SaveFullBuffer();
	void WriteBGDHeader(CString filename, FILE * out);
	void ReadBGDHeader(CString filename, FILE * in, DATA_TYPE &bgdDataType);
	bool MemoryAvailable(double bytes);
	bool startsWith(const char* compare, const char* starts) const;
	bool contains(char * haystack, char needle) const;
	bool LoadRasterCore(char* filenameUtf8, bool InRam, GridFileType fileType);

	void ReadPalette(int bits);
	void DecompressJpg(int bits);
	inline int GetCode();
	void SkipExtension();
	void DeInterlace();

	colort basecolor(ColorEntry *ent)
	{
		while(ent->next) ent=ent->next;
		return ent->c;
	}

	size_t getPtrSize( BYTE *ptr )
	{
		return sizeof( ptr );
	}

public: 	
	GDALRasterBand* GetBand(int index);
	
	void SaveHeaderInfo();

	double GetNoDataValue() { return _noDataValue; }
	void SetNoDataValue(double value) { _noDataValue = value; }

	CString GetProjection() { return _projection; }
	void SetProjection(CString value) { _projection = value; }

	void setDX(double dx) { _dX = dx; }
	void setDY(double dy) { _dY = dy; }
	void setYllCenter(double yll) { _yllCenter = yll; }
	void setXllCenter(double xll) { _xllCenter = xll; }

	bool PutFloatWindow(void *Vals, long StartRow, long EndRow, long StartCol, long EndCol, bool useDouble);
	bool GetFloatWindow(void *Vals, long StartRow, long EndRow, long StartCol, long EndCol, bool useDouble);

	void ProjToCell(double x, double y, long & column, long & row);
	void CellToProj(long column, long row, double & x, double & y);

	inline int round(double d);

	double GetMinimum();
	double GetMaximum();

	bool CanCreate();
	bool CanCreate(GridFileType newFileType);
	bool CreateNew(CStringW filename, GridFileType newFileType, double dx, double dy,
		double xllcenter, double yllcenter, double nodataval, char * projection,
		long ncols, long nrows, GridDataType DataType, bool CreateInRam,
		double initialValue, bool applyInitialValue);
	bool Save(CStringW saveToFilename, GridFileType newFileFormat);
	bool SaveToBGD(CString filename, void(*callback)(int number, const char * message));
	bool ReadFromBGD(CString filename, void(*callback)(int number, const char * message));
	bool ReadBGDHeader(CString filename, DATA_TYPE &bgdDataType);
	bool GetIntValueGridColorTable(IGridColorScheme ** newscheme);
	bool isInRam() { return _inRam; }
	bool IsRgb();

	void FlipBuffer();

	double getDX() { return _dX; }
	double getDY() { return _dY; }
	double getYllCenter() { return _yllCenter; }
	double getXllCenter() { return _xllCenter; }

	bool LoadRaster(CStringW filename, bool InRam, GridFileType fileType);
	unsigned char * toDib();
	long getWidth(){ return _width; }
	long getHeight(){ return _height; }
	double getValue(long Row, long Column);
	void putValue(long Row, long Column, double Value);
	void clear(double value);

	bool Close();
	bool ColorTable2BSTR(BSTR * pVal);
	bool BSTR2ColorTable(BSTR cTbl);
	bool OpenBand(int bandIndex);
	void ReadProjection();
	int getNumBands();
	int GetActiveBandIndex() { return _activeBandIndex; }
	GridDataType GetDataType();

};

#pragma pack(pop, gif2)

#endif // !defined(tkJpg_H)