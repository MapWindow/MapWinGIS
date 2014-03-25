// tkBitmap.h: Definition of the tkBitmap class
//
//////////////////////////////////////////////////////////////////////

#ifndef tkBitmap_h
#define tkBitmap_h
#include "colour.h"
#include "MapWinGis.h"
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
// tkBitmap

class tkBitmap
{
public:
	tkBitmap();
	~tkBitmap();


// ItkBitmap
public:
	void setWidth(long w);
	void setHeight(long h);
	long getHeight();
	long getWidth();	
	colour getValue( long Row, long Column );
	bool setValue( long Row, long Column, colour Value );
	void getRow( long Row, long * result );
	bool Open(CStringW FileName, colour *& ImageData);
	bool Open(CStringW FileName);
	bool CreateBitmap(const char * bmp_file, const colour * ImageData);
	bool WriteBitmap(const char * bmp_file, const colour * ImageData);
	bool WriteDiskToDisk(const char * SourceFile, const char * DestinationFile);
	void Close();
	long FileHandle();

private:
	//Buffering Variables
	void bufferRows( long centerRow );
	long currentRow;
	colour * rowOne;
	colour * rowTwo;
	colour * rowThree;	

	//Don't keep this Very large chunk of memory around for the
	//rare case in which it's used briefly to remove the pad from a 24-bit
	//bitmap.
	//CGOH BYTE * BitmapBits;
	int	sizeMap;
	long cols;
	long rows;	

	// Chris M May 2006 -- Is this file currently opened in write mode?
	bool writable;
	CStringW bmpFileName;

public:
	FILE * bmpfile;	
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmif;
	RGBQUAD * bmiColors;
	int	pad;	
	long begOfData;	

	ICallback * globalCallback;
	BSTR key;
};

#endif // !defined tkBitmap_h
