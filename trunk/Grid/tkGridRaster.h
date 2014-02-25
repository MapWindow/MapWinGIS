//********************************************************************************************************
//File name: tkGridRaster.h
//Description:  Generic grid wrapper class to allow MapWinGIs to utilize formats supported by GDAL
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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

#include "MapWinGis.h"
#include "grdTypes.h"

#include "HashTable.h"
#include "Extent.h"
#include "ImageStructs.h"

#include "gdal_priv.h"
#include "colour.h"


class tkGridRaster
{
public:
	double noDataValue;
	void SaveHeaderInfo();

	void setDX(double dx) {dX = dx;}				
	void setDY(double dy) {dY = dy;}				
	void setYllCenter(double yll) {YllCenter = yll;}	
	void setXllCenter(double xll) {XllCenter = xll;}	

	bool PutFloatWindow(float *Vals, long StartRow, long EndRow, long StartCol, long EndCol);
	bool GetFloatWindow(float *Vals, long StartRow, long EndRow, long StartCol, long EndCol);

	void ProjToCell( double x, double y, long & column, long & row );
	void CellToProj( long column, long row, double & x, double & y );
	
	inline int round( double d );

	double GetMinimum();
	double GetMaximum();

	bool CanCreate();
	bool CanCreate(GridFileType newFileType);
	bool CreateNew(char * filename, GridFileType newFileType, double dx, double dy, 
			   double xllcenter, double yllcenter, double nodataval, char * projection,
			   long ncols, long nrows, GridDataType DataType, bool CreateInRam, 
			   double initialValue, bool applyInitialValue);
	bool Save(char * saveToFilename, GridFileType newFileFormat);
	bool SaveToBGD(CString filename, void(*callback)(int number, const char * message));
	bool ReadFromBGD(CString filename, void (*callback)(int number, const char * message));
	bool ReadBGDHeader(CString filename, DATA_TYPE &bgdDataType);
	bool GetIntValueGridColorTable(IGridColorScheme ** newscheme);

	bool isInRam() { return inRam; }

	BSTR key;
	bool hasTransparency;
	colort transColor;
	ICallback * cBack;
	void FlipBuffer();

	double getDX() {return dX;} 
	double getDY() {return dY;}
	double getYllCenter() {return YllCenter;}
	double getXllCenter() {return XllCenter;}

	tkGridRaster():plotloc(0),blocksize(0),numread(0),bitshift(0),
		transColor(0), hasTransparency(false),buffSize(0),cBack(NULL),dX(-1),dY(-1),
		XllCenter(-1),YllCenter(-1)
	{		//Rob Cairns
			mFilename = "";
			rasterDataset=NULL;
			floatbuffer = NULL;
			_int32buffer = NULL;
			_int32ScanlineBufferA = NULL;
			floatScanlineBufferA = NULL;
			_int32ScanlineBufferB = NULL;
			floatScanlineBufferB = NULL;
			noDataValue = static_cast<double>(-3.40282346638529E+38);
			CanScanlineBuffer = false;
			scanlineBufferNumberA = -1;
			scanlineBufferNumberB = -1;
			cachedMax = -9999;
			cachedMin = 9999;
			genericType = GDT_Unknown;
			hasColorTable = false;
	} //Constructor

	virtual ~tkGridRaster()
	{
		Close(); // Free all memory used in Close.
	} // Deconstructor
	
	bool LoadRaster(const char * filename, bool InRam, GridFileType fileType);

	unsigned char * toDib();
	long getWidth(){return width;}
	long getHeight(){return height;}
	double getValue( long Row, long Column );
	void putValue( long Row, long Column, double Value );
	void clear(double value);
	GridDataType GetDataType();
	CString Projection;
	bool Close();
	bool ColorTable2BSTR(BSTR * pVal);
	bool BSTR2ColorTable(BSTR cTbl);
	bool OpenBand(int bandIndex);
	void ReadProjection();
	int getNumBands();

private:
	const static long MAX_INRAM_SIZE = 536870912;
	CString mFilename;
			//world coordinate related variables

	double dY;			//change in Y (for size of cell or pixel)
	double dX;			//change in X (for size of cell or pixel)
	double YllCenter;	//Y coordinate of lower left corner of image (world coordinate)
	double XllCenter;	//X coordinate of lower left corner of image (world coordinate)

	_int32 * _int32buffer;
	float * floatbuffer;

	_int32 * _int32ScanlineBufferA;
	float * floatScanlineBufferA;
	long scanlineBufferNumberA;
	_int32 * _int32ScanlineBufferB;
	float * floatScanlineBufferB;
	long scanlineBufferNumberB;
	bool CanScanlineBuffer;
	bool scanlineADataChanged;
	bool scanlineBDataChanged;
	char scanlineLastAccessed;

	bool inRam;
	void LoadFullBuffer();
	bool SaveFullBuffer();
	void WriteBGDHeader( CString filename, FILE * out );
	void ReadBGDHeader( CString filename, FILE * in, DATA_TYPE &bgdDataType );
	bool MemoryAvailable(double bytes);
	bool startsWith(const char* compare, const char* starts) const;
	bool contains(char * haystack, char needle) const;

	GDALDataset * rasterDataset;
	GDALColorTable * poColorT;
	bool hasColorTable;
	int buffSize;
	inline bool inColorMap(colort c);
	int ColorMapSize;
	ColorEntry ColorMap[4096];
	long width, height;
	int plotloc;
	int maxcode;
	int max_code;
	
	int blocksize;
	int numread;
	int bitshift;
	FILE * fp;
	int codesize;
	int isInterlaced;

	GDALColorInterp cIntp;
	GDALDataType dataType;
	GDALDataType genericType;
	int nBands;
	GDALRasterBand  *poBand;
	GridFileType currentFileType;

	unsigned char block[256];

	HashTable * ht;

	double cachedMax;
	double cachedMin;

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

};


#endif // !defined(tkJpg_H)