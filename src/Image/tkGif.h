// tkGif.h: interface for the tkGif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TKGIF_H__CE1F4C5D_F1FE_4749_AE18_61DE98857F34__INCLUDED_)
#define AFX_TKGIF_H__CE1F4C5D_F1FE_4749_AE18_61DE98857F34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#pragma pack(push, gif2, 1)

#include "HashTable.h"
#include "ImageStructs.h"

#define IMAGEBLOCK       ','
#define EXTENSIONBLOCK   '!'
#define PLAINTEXTBLOCK   0x0001
#define CONTROLBLOCK     0x00f9
#define APPLICATIONBLOCK 0x00ff


class tkGif  
{
public:
	BSTR key;
	bool hasTransparency;
	colort transColor;
	ICallback * cBack;
	void FlipBuffer();
	bool ReadPPM(const char * filename);
	bool WriteGif(const char * filename); // Write To Gif File,  data must already be loaded
	bool WritePPM(CStringW filename); // Write To PPM File,  data must already be loaded

	tkGif():plotloc(0),buffer(NULL),blocksize(0),numread(0),bitshift(0),
		transColor(0), hasTransparency(false),buffSize(0),cBack(NULL){} // Constructor
	virtual ~tkGif()
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
	bool ReadGif(const char * filename);	
	unsigned char * toDib();
	int getWidth(){return width;}
	int getHeight(){return height;}

	colort * buffer;
private:
	int buffSize;
	void getPalette();
	void Quantize();
	inline int AddEntry(int prefix, colort c);
	inline void PutCode(int code);
	void CompressGif();
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
	void DecompressGIF(int bits);
	inline int GetCode();
	void SkipExtension();
	void DeInterlace();
	colort basecolor(ColorEntry *ent)
	{
		while(ent->next) ent=ent->next;
		return ent->c;
	}
};

//#pragma pack(pop, gif2)

#endif // !defined(AFX_TKGIF_H__CE1F4C5D_F1FE_4749_AE18_61DE98857F34__INCLUDED_)
