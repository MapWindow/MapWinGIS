// tkTiff.h: interface for the tkTiff class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TKTIFF_H)
#define TKTIFF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma pack(push, gif2, 1)

#include "../MapWinGIS.h"
#include <gdiplus.h>
#include <Gdipluspixelformats.h>

//#include "geotiff.h"

#include "HashTable.h"
#include "ImageStructs.h"

#define IMAGEBLOCK       ','
#define EXTENSIONBLOCK   '!'
#define PLAINTEXTBLOCK   0x0001
#define CONTROLBLOCK     0x00f9
#define APPLICATIONBLOCK 0x00ff


class tkTiff  
{
public:
	BSTR key;
	bool hasTransparency;
	colort transColor;
	ICallback * cBack;
	void FlipBuffer();

	int getDX() {return dX;}
	int getDY() {return dY;}
	int getYllCenter() {return YllCenter;}
	int getXllCenter() {return XllCenter;}


	bool ReadPPM(const char * filename);
	bool WriteTiff(const char * filename); // Write To Tiff File,  data must already be loaded
	//bool WriteGif(const char * filename); // Write To Gif File,  data must already be loaded
	bool WritePPM(const char * filename); // Write To PPM File,  data must already be loaded

	tkTiff():plotloc(0),buffer(NULL),blocksize(0),numread(0),bitshift(0),
		transColor(0), hasTransparency(false),buffSize(0),cBack(NULL),dX(-1),dY(-1),
		XllCenter(-1),YllCenter(-1){} // Constructor
	virtual ~tkTiff()
	{
		if(buffer)
			delete [] buffer;
	} // Deconstructor
	void InitSize(int nWidth, int nHeight)
	{
		width=nWidth; height=nHeight;
		buffSize = width*height;
		buffer = new colort[buffSize];
	}	

	bool ReadTiff(const char * filename, bool *foundGeoTags);	
	//unsigned char * toDib();
	int getWidth(){return width;}
	int getHeight(){return height;}

	colort * buffer;
private:
	int buffSize;
	void getPalette();
	void Quantize();
	inline int AddEntry(int prefix, colort c);
	inline void PutCode(int code);
	void CompressTiff();
	inline void WritePalette();
	inline bool inColorMap(colort c);
	int ColorMapSize;
	ColorEntry ColorMap[4096];
	
	int width, height;
	int plotloc;
	int maxcode;
	int max_code;
	
	int blocksize;
	int numread;
	int bitshift;
	FILE * fp;
	int codesize;
	int isInterlaced;

	unsigned char block[256];

	HashTable * ht;

		//world coordinate related variables
	double dY;			//change in Y (for size of cell or pixel)
	double dX;			//change in X (for size of cell or pixel)
	double YllCenter;	//Y coordinate of lower left corner of image (world coordinate)
	double XllCenter;	//X coordinate of lower left corner of image (world coordinate)

	void plot(ColorEntry * CE)
	{
		if(CE->next == NULL)
		{
			buffer[plotloc++] = CE->c;
			return;
		}
		plot(CE->next);
		buffer[plotloc++] = CE->c;
	}

	void ReadPalette(int bits);
	//void DecompressTIFF(int bits);
	//inline int GetCode();
	//void SkipExtension();
	//void DeInterlace();

	
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


#endif // !defined(TKTIFF_H)