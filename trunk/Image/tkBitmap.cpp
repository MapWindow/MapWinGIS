//********************************************************************************************************
//File name: tkBitmap.cpp
//Description: Declaration of the CMap ActiveX Control class.
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
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//3-28-2005 dpa - Updated for custom line stipples and label collision.
//9-12-2005 Lain Chen - Solved the memory cruption problem.
//********************************************************************************************************

#include "stdafx.h"

#include "tkBitmap.h"
#include "colour.h" 

/////////////////////////////////////////////////////////////////////////////
//


//WriteBitmap takes the name of the file to write to as a parameter
// and then opens the file, writes the header information and then 
// writes the bitmap info...
bool tkBitmap::WriteBitmap(const char * bmp_file, const colour * ImageData)
{	
	USES_CONVERSION;

	FILE* fout = fopen(bmp_file,"wb");
	
	if( !fout )
		return false; //if the output file can't be opened, stop...

	
	//constant values
	bmfh.bfType = 19778;		//always BM   
	bmfh.bfReserved1 = 0;		//always 0
	bmfh.bfReserved2 = 0;		//always 0
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  //54

	bmif.biSize = sizeof(BITMAPINFOHEADER);  //40
	bmif.biPlanes = 1;
	bmif.biBitCount = 24;
	bmif.biCompression = 0;
	bmif.biXPelsPerMeter = 0;
	bmif.biYPelsPerMeter = 0;
	bmif.biClrUsed = 0;
	bmif.biClrImportant = 0;

	//given or calculated values
	bmif.biWidth = cols;  
	bmif.biHeight = rows; 

	pad = cols*24;
	pad %= 32;
	if(pad != 0)
	{
		pad = 32 - pad;
		pad /= 8;
	}
	sizeMap = cols*3 + pad;			//3 bytes per pixel plus pad
	sizeMap *= rows;				//times number rows
	
	bmif.biSizeImage = sizeMap;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmif.biSizeImage;

	
	
	//if there is data in the BitmapBits variable, then delete it and start with new 
	// storage space.
	//CGOH if (BitmapBits)
	//CGOH 	delete [] BitmapBits;
	//CGOH BitmapBits = new BYTE[sizeMap];

	//create BitmapBits with padding inserted because it the padding isn't used
	// in the generic data structure(only in Bitmaps).
	long curElement = 0;
	//long numCols = cols*3;
	long curImageDataElement = 0;
	int padCount = 0;
	unsigned char white = 255;
	unsigned char black = 0;
	
	//write in BITMAPFILEHEADER
	fwrite(&bmfh.bfType,sizeof(bmfh.bfType),1,fout);
	fwrite(&bmfh.bfSize,sizeof(bmfh.bfSize),1,fout);
	fwrite(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,fout);
	fwrite(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,fout);
	fwrite(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,fout);

	//write in BITMAPINFOHEADER
	fwrite(&bmif.biSize,sizeof(bmif.biSize),1,fout);
	fwrite(&bmif.biWidth,sizeof(bmif.biWidth),1,fout);
	fwrite(&bmif.biHeight,sizeof(bmif.biHeight),1,fout);
	fwrite(&bmif.biPlanes,sizeof(bmif.biPlanes),1,fout);
	fwrite(&bmif.biBitCount,sizeof(bmif.biBitCount),1,fout);
	fwrite(&bmif.biCompression,sizeof(bmif.biCompression),1,fout);
	fwrite(&bmif.biSizeImage,sizeof(bmif.biSizeImage),1,fout);
	fwrite(&bmif.biXPelsPerMeter,sizeof(bmif.biXPelsPerMeter),1,fout);
	fwrite(&bmif.biYPelsPerMeter,sizeof(bmif.biYPelsPerMeter),1,fout);
	fwrite(&bmif.biClrUsed,sizeof(bmif.biClrUsed),1,fout);
	fwrite(&bmif.biClrImportant,sizeof(bmif.biClrImportant),1,fout);


	//don't try to write to file if the image data hasn't been created successfully
//CGOH 	if (!BitmapBits)
		//CGOH return false;

	//write the bitmap data to the file
	//this part writes from the top to the bottom

	long ecnt = 0, percent = 0, newpercent = 0;
	double total = rows;

	char * padding = NULL;
	if( pad > 0 )
	{	padding = new char[pad];
		memset(padding,0,sizeof(char)*pad);
	}

	long byterowsize = bmif.biWidth*3;
	for(long curRow = 0; curRow < rows; curRow++)
	{
		//Write the data
		fwrite(&(ImageData[curRow*bmif.biWidth]),sizeof(BYTE),byterowsize,fout);
		
		//*****WRITE THE PAD******
		if( pad > 0 )
			fwrite(padding,sizeof(BYTE),pad,fout);				

		if( globalCallback != NULL )
		{
			newpercent = (long)(((ecnt++)/total)*100);
			if( newpercent > percent )
			{	percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Saving Image"));
			}
		}			
		
	}//end outside for loop	

	fclose(fout);

	//CGOH if (BitmapBits)
	//CGOH 	delete [] BitmapBits;
	//CGOH BitmapBits = NULL;

	return true;
}

bool tkBitmap::CreateBitmap(const char * bmp_file, const colour * ImageData) 
{
	//constant values
	bmfh.bfType = 19778;		//always BM   
	bmfh.bfReserved1 = 0;		//always 0
	bmfh.bfReserved2 = 0;		//always 0
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  //54

	bmif.biSize = sizeof(BITMAPINFOHEADER);  //40
	bmif.biPlanes = 1;
	bmif.biBitCount = 24;
	bmif.biCompression = 0;
	bmif.biXPelsPerMeter = 0;
	bmif.biYPelsPerMeter = 0;
	bmif.biClrUsed = 0;
	bmif.biClrImportant = 0;

	//given or calculated values
	bmif.biWidth = cols;  
	bmif.biHeight = rows; 

	pad = cols*24;
	pad %= 32;
	if(pad != 0)
	{
		pad = 32 - pad;
		pad /= 8;
	}
	sizeMap = cols*3 + pad;			//3 bytes per pixel plus pad
	sizeMap *= rows;				//times number rows
	
	bmif.biSizeImage = sizeMap;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmif.biSizeImage;
	
		
	//Write the bmp
	WriteBitmap( bmp_file , ImageData);
	return true;
}

bool tkBitmap::Open(const char *FileName, colour *& ImageData)
{
	USES_CONVERSION;
	
	/********************************
	Begin Code for actual Bitmap file reading
	********************************/

	FILE* fin = fopen(FileName,"rb");
	
	if( !fin )
		return false;
	
	
	//read in BITMAPFILEHEADER
	fread(&bmfh.bfType,sizeof(bmfh.bfType),1,fin);
	
	//if not a valid bitmap file, fail to load
	if (bmfh.bfType != 19778) 
		return false;


	fread(&bmfh.bfSize,sizeof(bmfh.bfSize),1,fin);
	fread(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,fin);
	fread(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,fin);
	fread(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,fin);


	//read in BITMAPINFOHEADER
	fread(&bmif.biSize,sizeof(bmif.biSize),1,fin);
	fread(&bmif.biWidth,sizeof(bmif.biWidth),1,fin);
	fread(&bmif.biHeight,sizeof(bmif.biHeight),1,fin);
	fread(&bmif.biPlanes,sizeof(bmif.biPlanes),1,fin);
	fread(&bmif.biBitCount,sizeof(bmif.biBitCount),1,fin);
	fread(&bmif.biCompression,sizeof(bmif.biCompression),1,fin);
	fread(&bmif.biSizeImage,sizeof(bmif.biSizeImage),1,fin);
	fread(&bmif.biXPelsPerMeter,sizeof(bmif.biXPelsPerMeter),1,fin);
	fread(&bmif.biYPelsPerMeter,sizeof(bmif.biYPelsPerMeter),1,fin);
	fread(&bmif.biClrUsed,sizeof(bmif.biClrUsed),1,fin);
	fread(&bmif.biClrImportant,sizeof(bmif.biClrImportant),1,fin);

	//Read the color table
	if( bmiColors != NULL )
		delete [] bmiColors;
	bmiColors = NULL;

	bmif.biClrUsed = ( bmfh.bfOffBits - 54 )/4;
	if( bmif.biClrUsed != 0 )
	{	bmiColors = new RGBQUAD[bmif.biClrUsed];
		fread( bmiColors, sizeof(RGBQUAD), bmif.biClrUsed, fin );
	}	

	//Read to the beginning of the data
	char no_value;
	int sizeof_header = 54;
	for( unsigned int n = 54 + bmif.biClrUsed*4; n < bmfh.bfOffBits; n++ )
		fread(&no_value,sizeof(char),1,fin);

	pad = bmif.biWidth*bmif.biBitCount;
	pad %= 32;
	if(pad != 0)
	{	pad = 32 - pad;
		pad /= 8;
	}

	int bitcount = 3;
	//sizeMap = cols*3 + pad;			//3 bytes per pixel plus pad
	//sizeMap *= rows;
	long size_image = ( bmif.biWidth*bitcount + pad )*bmif.biHeight;
//CGOH 	if (BitmapBits)
//CGOH 		delete [] BitmapBits;
//CGOH 	BitmapBits = new BYTE[size_image];
	BYTE * BitmapBitsNoPad = new BYTE[bmif.biWidth*bitcount*bmif.biHeight];

	long percent = 0, newpercent = 0, ecnt = 0;
	double total = bmif.biHeight*bmif.biWidth*2;

	if( bmif.biBitCount == 24 )
	{
		BYTE * ConvertBits = new BYTE[size_image];
		fread(ConvertBits,sizeof(BYTE),size_image,fin);
		
		//Get Rid of the pad
		long loc = 0, y = 0, x = 0, cnt = 0;
		for( y = 0; y < bmif.biHeight; y++ )
		{	for( x = 0; x < bmif.biWidth; x++ )
			{	BitmapBitsNoPad[cnt++] = ConvertBits[loc++];
				BitmapBitsNoPad[cnt++] = ConvertBits[loc++];
				BitmapBitsNoPad[cnt++] = ConvertBits[loc++];

				newpercent = (long)(((ecnt++)/total)*100);
				if( newpercent > percent )
				{	percent = newpercent;
					if( globalCallback != NULL )
						globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Opening Image"));
				}
			}
			for( int p = 0; p < pad; p++ )
				loc++;
		}

		delete [] ConvertBits;
		ConvertBits = NULL;
	}
	else if( bmif.biBitCount == 8 )
	{	
		if( bmiColors == NULL )
		{	fclose(fin);
			//clean up after yourself...
//CGOH 			if (BitmapBits)
//CGOH 				delete [] BitmapBits;
//CGOH 			BitmapBits = NULL; //not a local variable, set it to NULL after deleting it.

			if (BitmapBitsNoPad)
				delete [] BitmapBitsNoPad;	//local variable, will be removed automatically
			BitmapBitsNoPad = NULL;			//Cleaning it up anyway...just to be safe
			return false;
		}

		long y = 0, x = 0, cnt = 0;
		unsigned char color = 0;
		for(  y = 0; y < bmif.biHeight; y++ )
		{	for( x = 0; x < bmif.biWidth; x++ )
			{	fread( &color, sizeof( unsigned char ), 1, fin );
				short index = abs( color );
				BitmapBitsNoPad[cnt++] = bmiColors[index].rgbBlue;
				BitmapBitsNoPad[cnt++] = bmiColors[index].rgbGreen;
				BitmapBitsNoPad[cnt++] = bmiColors[index].rgbRed;

				newpercent = (long)(((ecnt++)/total)*100);
				if( newpercent > percent )
				{	percent = newpercent;
					if( globalCallback != NULL )
						globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Opening Image"));
				}
			}
			for( int p = 0; p < pad; p++ )
				fread( &color, sizeof(char), 1, fin );			
		}
	}
	else if( bmif.biBitCount == 4 )
	{	if( bmiColors == NULL )
		{	fclose(fin);
			//clean up after yourself...
//CGOH 			if (BitmapBits)
//CGOH 				delete [] BitmapBits;
//CGOH 			BitmapBits = NULL; //not a local variable, set it to NULL after deleting it.

			if (BitmapBitsNoPad)
				delete [] BitmapBitsNoPad;	//local variable, will be removed automatically
			BitmapBitsNoPad = NULL;			//Cleaning it up anyway...just to be safe
			return false;
		}

		long y = 0, x = 0, cnt = 0;
		unsigned char color = 0;
		for(  y = 0; y < bmif.biHeight; y++ )
		{	for( x = 0; x < bmif.biWidth; x=x+2 )
			{	fread( &color, sizeof( unsigned char ), 1, fin );
				short hiIndex = color >> 4;
				short lowIndex = color & 0x0F;
				
				BitmapBitsNoPad[cnt++] = bmiColors[hiIndex].rgbBlue;
				BitmapBitsNoPad[cnt++] = bmiColors[hiIndex].rgbGreen;
				BitmapBitsNoPad[cnt++] = bmiColors[hiIndex].rgbRed;

				if( x + 1 < bmif.biWidth )
				{	BitmapBitsNoPad[cnt++] = bmiColors[lowIndex].rgbBlue;
					BitmapBitsNoPad[cnt++] = bmiColors[lowIndex].rgbGreen;
					BitmapBitsNoPad[cnt++] = bmiColors[lowIndex].rgbRed;
				}
				
				ecnt += 2;
				newpercent = (long)((ecnt/total)*100);
				if( newpercent > percent )
				{	percent = newpercent;
					if( globalCallback != NULL )
						globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Opening Image"));
				}
			}
			for( int p = 0; p < pad; p++ )
				fread( &color, sizeof(char), 1, fin );			
		}
	}
	else if( bmif.biBitCount == 1 )
	{	if( bmiColors == NULL )
		{	fclose(fin);
			//clean up after yourself...
//CGOH 			if (BitmapBits)
//CGOH 				delete [] BitmapBits;
//CGOH 			BitmapBits = NULL; //not a local variable, set it to NULL after deleting it.

			if (BitmapBitsNoPad)
				delete [] BitmapBitsNoPad;	//local variable, will be removed automatically
			BitmapBitsNoPad = NULL;			//Cleaning it up anyway...just to be safe
			return false;
		}

		long y = 0, x = 0, cnt = 0;
		unsigned char color = 0;
		for(  y = 0; y < bmif.biHeight; y++ )
		{	
			for( x = 0; x < bmif.biWidth; x+=8 )
			{
				fread( &color, sizeof( unsigned char ), 1, fin );
				
				unsigned char bit = 0x80;
				
				for( int i = 0; i < 8 && x + i < bmif.biWidth; i++ )
				{	unsigned char thisBit = bit >> i;
					short index = (unsigned char)( (thisBit & color) >> ( 7 - i ) );
					
					BitmapBitsNoPad[cnt++] = bmiColors[index].rgbBlue;
					BitmapBitsNoPad[cnt++] = bmiColors[index].rgbGreen;
					BitmapBitsNoPad[cnt++] = bmiColors[index].rgbRed;
				}
				
				ecnt += 8;
				newpercent = (long)((ecnt/total)*100);
				if( newpercent > percent )
				{	percent = newpercent;
					if( globalCallback != NULL )
						globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Opening Image"));
				}
			}
			for( int p = 0; p < pad; p++ )
				fread( &color, sizeof(char), 1, fin );	
		}		
	}
	else
	{	fclose(fin);
		//clean up after yourself...
//CGOH 		if (BitmapBits)
//CGOH 			delete [] BitmapBits;
//CGOH 		BitmapBits = NULL; //not a local variable, set it to NULL after deleting it.

		if (BitmapBitsNoPad)
			delete [] BitmapBitsNoPad;	//local variable, will be removed automatically
		BitmapBitsNoPad = NULL;			//Cleaning it up anyway...just to be safe
		return false;
	}

	this->cols = bmif.biWidth;
	this->rows = bmif.biHeight;	
	
	fclose(fin);
	/********************************
	End Code for actual Bitmap file reading
	********************************/

	
	/*********************************
	Conversion from Bitmap to our standard format (colour class)
	*********************************/
	
	if (ImageData) // if ImageData contains information, delete it
		delete [] ImageData;
		
	long arraySize = bmif.biWidth*bmif.biHeight;
	ImageData = new colour[arraySize];

	long curByte;
	long curDataCell;
	long loc = 0;

	//loop throught the bitmap data and store the information into basic image storage
	for (curByte = 0, curDataCell = 0; curByte < arraySize; curByte++)
	{
		ImageData[curDataCell].blue = BitmapBitsNoPad[loc++];
		ImageData[curDataCell].green = BitmapBitsNoPad[loc++];
		ImageData[curDataCell].red = BitmapBitsNoPad[loc++];
		curDataCell++;

		newpercent = (long)(((ecnt++)/total)*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Opening Image"));
		}
	}
	
	if( globalCallback != NULL )
			globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	/*********************************
	End Conversion from Bitmap to the standard format (colour class)
	*********************************/

	//clean up after yourself...
//CGOH 	if (BitmapBits)
//CGOH 		delete [] BitmapBits;
//CGOH 	BitmapBits = NULL; //not a local variable, set it to NULL after deleting it.

	if (BitmapBitsNoPad)
		delete [] BitmapBitsNoPad;	//local variable, will be removed automatically
	BitmapBitsNoPad = NULL;			//Cleaning it up anyway...just to be safe

	if( bmiColors )
		delete [] bmiColors;
	bmiColors = NULL;
	
	//everthing went ok, return true
	return true;
}

bool tkBitmap::Open(const char *FileName)
{
	
	/********************************
	Begin Code for actual Bitmap file reading
	********************************/	

	// Chris Michaelis -- "bmpfile" is used to write to the file as well,
	// but most of the time writing is not necessarily needed. Reopen the
	// file with write access when (if?) needed.
	// bmpfile = fopen(FileName,"r+b");
	writable = false;
	bmpFileName = new char[_tcslen(FileName) + 1];
	strcpy(bmpFileName, FileName);
	bmpfile = fopen(FileName,"rb");

	if( !bmpfile )
		return false;
	
	
	//read in BITMAPFILEHEADER
	fread(&bmfh.bfType,sizeof(bmfh.bfType),1,bmpfile);
	
	//if not a valid bitmap file, fail to load
	if (bmfh.bfType != 19778) 
		return false;


	fread(&bmfh.bfSize,sizeof(bmfh.bfSize),1,bmpfile);
	fread(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,bmpfile);
	fread(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,bmpfile);
	fread(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,bmpfile);


	//read in BITMAPINFOHEADER
	fread(&bmif.biSize,sizeof(bmif.biSize),1,bmpfile);
	fread(&bmif.biWidth,sizeof(bmif.biWidth),1,bmpfile);
	fread(&bmif.biHeight,sizeof(bmif.biHeight),1,bmpfile);
	fread(&bmif.biPlanes,sizeof(bmif.biPlanes),1,bmpfile);
	fread(&bmif.biBitCount,sizeof(bmif.biBitCount),1,bmpfile);
	fread(&bmif.biCompression,sizeof(bmif.biCompression),1,bmpfile);
	fread(&bmif.biSizeImage,sizeof(bmif.biSizeImage),1,bmpfile);
	fread(&bmif.biXPelsPerMeter,sizeof(bmif.biXPelsPerMeter),1,bmpfile);
	fread(&bmif.biYPelsPerMeter,sizeof(bmif.biYPelsPerMeter),1,bmpfile);
	fread(&bmif.biClrUsed,sizeof(bmif.biClrUsed),1,bmpfile);
	fread(&bmif.biClrImportant,sizeof(bmif.biClrImportant),1,bmpfile);

	//Read the color table
	if( bmiColors != NULL )
		delete [] bmiColors;
	bmiColors = NULL;

	bmif.biClrUsed = ( bmfh.bfOffBits - 54 )/4;
	if( bmif.biClrUsed != 0 )
	{	bmiColors = new RGBQUAD[bmif.biClrUsed];
		fread( bmiColors, sizeof(RGBQUAD), bmif.biClrUsed, bmpfile );
	}	

	//Read to the beginning of the data
	char no_value;
	int sizeof_header = 54;
	for( unsigned int n = 54 + bmif.biClrUsed*4; n < bmfh.bfOffBits; n++ )
		fread(&no_value,sizeof(char),1,bmpfile);

	begOfData = ftell( bmpfile );

	pad = bmif.biWidth*bmif.biBitCount;
	pad %= 32;
	if(pad != 0)
	{
		pad = 32 - pad;
		pad /= 8;
	}	

	this->cols = bmif.biWidth;
	this->rows = bmif.biHeight;	

	rowOne = new colour[cols];
	rowTwo = new colour[cols];
	rowThree = new colour[cols];	
	bufferRows( 1 );		

	/********************************
	End Code for actual Bitmap file reading
	********************************/

	//everthing went ok, return true
	//fclose(bmpfile);
	return true;
}

tkBitmap::tkBitmap()
{	USES_CONVERSION;
//CGOH 	BitmapBits = NULL;
	bmpfile = NULL;
	rowOne = NULL;
	rowTwo = NULL;
	rowThree = NULL;
	bmiColors = NULL;
	globalCallback = NULL;
	key = A2BSTR("");
	bmpFileName = NULL;
}

tkBitmap::~tkBitmap()
{	Close();

	if (bmpFileName != NULL)
		delete [] bmpFileName;

	::SysFreeString(key);
}

long tkBitmap::FileHandle()
{	if( bmpfile != NULL )
		return _fileno(bmpfile);
	else
		return -1;
}

void tkBitmap::Close()
{	
	if( bmpfile != NULL )
	{
		fclose( bmpfile );
		bmpfile = NULL;
	}
	if( rowOne != NULL )
		delete [] rowOne;
	rowOne = NULL;
	if( rowTwo != NULL )
		delete [] rowTwo;
	rowTwo = NULL;
	if( rowThree != NULL )
		delete [] rowThree;
	rowThree = NULL;
	rows = 0;
	cols = 0;
	 
	if( bmiColors != NULL )
	{
		delete [] bmiColors;
		//11-Nov-2009 Rob Cairns Bug 1483 MapWinGIS crashes when removing certain types of Bitmaps (RGB?)
		//Set to NULL after deleting
		bmiColors = NULL;
	}
	
	globalCallback = NULL;
}

long tkBitmap::getWidth()
{
	return cols;
}

long tkBitmap::getHeight()
{
	return rows;
}

void tkBitmap::setHeight(long h)
{
	rows = h;
}

void tkBitmap::setWidth(long w)
{
	cols = w;
}

colour tkBitmap::getValue( long Row, long Column )
{	
	if( bmpfile == NULL )
		return colour();
	else
	{	if( Row == currentRow - 1 )
			return rowOne[Column];
		else if( Row == currentRow )
			return rowTwo[Column];
		else if( Row == currentRow + 1 )
			return rowThree[Column];
		else
		{	bufferRows( Row );
			return rowTwo[Column];
		}
	}
}
bool tkBitmap::setValue( long Row, long Column, colour Value )
{	
	if (!writable)
	{
		bmpfile = freopen(bmpFileName, "r+b", bmpfile);
		writable = true;
	}

	if( bmpfile == NULL )
	{	return false;
	}
	else
	{	
		//long lval = RGB( Value.red, Value.green, Value.blue );
		if( Row == currentRow - 1 )
			rowOne[Column] = Value;
		else if( Row == currentRow )
			rowTwo[Column] = Value;
		else if( Row == currentRow + 1 )
			rowThree[Column] = Value;

		long offset = begOfData + ( cols*3 + pad )*Row + Column*3;
		fseek( bmpfile, offset, SEEK_SET );
		fwrite( &Value.blue, sizeof(BYTE), 1, bmpfile );
		fwrite( &Value.green, sizeof(BYTE), 1, bmpfile );
		fwrite( &Value.red, sizeof(BYTE), 1, bmpfile );

		return true;
	}
	
	return false;
}
void tkBitmap::getRow( long Row, long * result )
{
	if( bmpfile != NULL )
	{	if( Row == currentRow - 1 )
		{	for( int i = 0; i < cols; i++ )
			{					
				result[i] = RGB(rowOne[i].red,rowOne[i].green,rowOne[i].blue);
			}
		}
		else if( Row == currentRow )
		{	for( int i = 0; i < cols; i++ )
				result[i] = RGB(rowTwo[i].red,rowTwo[i].green,rowTwo[i].blue);		
		}
		else if( Row == currentRow + 1 )
		{	for( int i = 0; i < cols; i++ )
				result[i] = RGB(rowThree[i].red,rowThree[i].green,rowThree[i].blue);			
		}
		else
		{	
			bufferRows( Row );
			{	for( int i = 0; i < cols; i++ )
					result[i] = RGB(rowTwo[i].red,rowTwo[i].green,rowTwo[i].blue);			
			}
		}
	}
}

void tkBitmap::bufferRows( long centerRow )
{	
	currentRow = centerRow;

	long sizeRow = cols*3 + pad;
	//long sizeDataRow = ( cols* (((double)bmif.biBitCount)/8) ) + pad;	

	long sizeDataRow = bmif.biWidth;
	if( bmif.biBitCount == 24 )
		sizeDataRow*=3;
	else if( bmif.biBitCount == 8 )
	{}
	else if( bmif.biBitCount == 4 )
	{	sizeDataRow /= 2; //sizeDataRow*=.5;
		if( bmif.biWidth % 2 )
			sizeDataRow++;
	}
	else if( bmif.biBitCount == 1 )
		sizeDataRow = (long)ceil( ((double)bmif.biWidth)/8.0 );
	sizeDataRow += pad;

	BYTE * buffer = new BYTE[ sizeDataRow ];	

	bool fseekSet = false;

	if( currentRow > 0 )
	{	long oneOffset = begOfData + sizeDataRow*( centerRow - 1 );
		fseek( bmpfile, oneOffset, SEEK_SET );
		fread( buffer, sizeof(BYTE), sizeDataRow, bmpfile );

		if( bmif.biBitCount == 24 )
		{
			//long loc = 0;
			//for( int i = 0; loc < cols; i = i+3 )
			//	rowOne[loc++] = colour( buffer[i+2],
			//						 buffer[i+1],
			//						 buffer[i] );
			//memcpy(rowOne,buffer,sizeDataRow*sizeof(BYTE)); --by Lailin Chen To solve the memory cruption problem
			memcpy(rowOne,buffer,cols*3); //This will ensure the memory copy won't copy over the boundary of the buffer
		}
		else if( bmif.biBitCount == 8 )
		{	
			unsigned char color;
			long loc = 0;
			for( int x = 0; x < bmif.biWidth; x++ )
			{	color = buffer[x];
				short index = abs( color );
				rowOne[loc++] = colour( bmiColors[index].rgbRed,
									 bmiColors[index].rgbGreen,
									 bmiColors[index].rgbBlue );
			}
		}
		else if( bmif.biBitCount == 4 )
		{	unsigned char color;
			long loc = 0;
			int cnt = 0;
			for( int x = 0; x < bmif.biWidth; x=x+2 )
			{	color = buffer[cnt++];
				short hiIndex = color >> 4;
				short lowIndex = color & 0x0F;
				
				rowOne[loc++] = colour( bmiColors[hiIndex].rgbBlue,
									 bmiColors[hiIndex].rgbGreen,
									 bmiColors[hiIndex].rgbRed );
				
				if( x+1 < bmif.biWidth )
					rowOne[loc++] = colour( bmiColors[lowIndex].rgbBlue,
										 bmiColors[lowIndex].rgbGreen,
										 bmiColors[lowIndex].rgbRed );
			}
		}
		else if( bmif.biBitCount == 1 )
		{
			long loc = 0;
			int cnt = 0;
			unsigned char color;
			for( int x = 0; x < bmif.biWidth; x+=8 )
			{
				color = buffer[cnt++];
				unsigned char bit = 0x80;
				
				for( int i = 0; i < 8 && x + i < bmif.biWidth; i++ )
				{	unsigned char thisBit = bit >> i;
					short index = (unsigned char)( (thisBit & color) >> ( 7 - i ) );
					
					rowOne[loc++] = colour( bmiColors[index].rgbBlue,
										 bmiColors[index].rgbGreen,
										 bmiColors[index].rgbRed );
				}
			}
		}

		fseekSet = true;
	}		

	if( currentRow >= 0 && currentRow <= rows - 1 )
	{
		if( !fseekSet )
		{	long twoOffset = begOfData + sizeDataRow*( centerRow );
			fseek( bmpfile, twoOffset, SEEK_SET );					
		}

		fread( buffer, sizeof(BYTE), sizeDataRow, bmpfile );

		if( bmif.biBitCount == 24 )
		{
			//long loc = 0;
			//for( int i = 0; loc < cols; i = i+3 )
			//	rowTwo[loc++] = colour( buffer[i+2],
			//						 buffer[i+1],
			//						 buffer[i] );
			//memcpy(rowOne,buffer,sizeDataRow*sizeof(BYTE));--by Lailin Chen To solve the memory cruption problem
			memcpy(rowTwo,buffer,cols*3); //This will ensure the memory copy won't copy over the boundary of the buffer
		}
		else if( bmif.biBitCount == 8 )
		{
			unsigned char color;
			long loc = 0;
			for( int x = 0; x < bmif.biWidth; x++ )
			{	color = buffer[x];
				short index = abs( color );
				rowTwo[loc++] = colour( bmiColors[index].rgbRed,
									 bmiColors[index].rgbGreen,
									 bmiColors[index].rgbBlue );
			}
		}
		else if( bmif.biBitCount == 4 )
		{	
			unsigned char color;
			long loc = 0;
			int cnt = 0;
			for( int x = 0; x < bmif.biWidth; x=x+2 )
			{	color = buffer[cnt++];
				short hiIndex = color >> 4;
				short lowIndex = color & 0x0F;
				
				rowTwo[loc++] = colour( bmiColors[hiIndex].rgbBlue,
									 bmiColors[hiIndex].rgbGreen,
									 bmiColors[hiIndex].rgbRed );

				if( x+1 < bmif.biWidth )
					rowTwo[loc++] = colour( bmiColors[lowIndex].rgbBlue,
										 bmiColors[lowIndex].rgbGreen,
										 bmiColors[lowIndex].rgbRed );
			}
		}
		else if( bmif.biBitCount == 1 )
		{
			long loc = 0;
			int cnt = 0;
			unsigned char color;
			for( int x = 0; x < bmif.biWidth; x+=8 )
			{
				color = buffer[cnt++];
				unsigned char bit = 0x80;
				
				for( int i = 0; i < 8 && x + i < bmif.biWidth; i++ )
				{	unsigned char thisBit = bit >> i;
					short index = (unsigned char)( (thisBit & color) >> ( 7 - i ) );
					
					rowTwo[loc++] = colour( bmiColors[index].rgbBlue,
										 bmiColors[index].rgbGreen,
										 bmiColors[index].rgbRed );
				}
			}
		}
		fseekSet = true;
	}

	if( currentRow < rows - 1 )
	{	
		if( !fseekSet )
		{	long threeOffset = begOfData + sizeDataRow*( centerRow + 1 );
			fseek( bmpfile, threeOffset, SEEK_SET );			
		}

		fread( buffer, sizeof(BYTE), sizeDataRow, bmpfile );

		if( bmif.biBitCount == 24 )
		{
			//long loc = 0;
			//for( int i = 0; loc < cols; i = i+3 )
			//	rowThree[loc++] = colour( buffer[i+2],
			//						   buffer[i+1],
			//						   buffer[i] );
			memcpy(rowThree,buffer,cols*3);
		}
		else if( bmif.biBitCount == 8 )
		{	
			unsigned char color;
			long loc = 0;
			for( int x = 0; x < bmif.biWidth; x++ )
			{	color = buffer[x];
				short index = abs( color );
				rowThree[loc++] = colour( bmiColors[index].rgbRed,
									   bmiColors[index].rgbGreen,
									   bmiColors[index].rgbBlue );
			}
		}
		else if( bmif.biBitCount == 4 )
		{	
			unsigned char color;
			long loc = 0;
			int cnt = 0;
			for( int x = 0; x < bmif.biWidth; x=x+2 )
			{	color = buffer[cnt++];
				short hiIndex = color >> 4;
				short lowIndex = color & 0x0F;
				
				rowThree[loc++] = colour( bmiColors[hiIndex].rgbBlue,
									   bmiColors[hiIndex].rgbGreen,
									   bmiColors[hiIndex].rgbRed );

				if( x+1 < bmif.biWidth )
					rowThree[loc++] = colour( bmiColors[lowIndex].rgbBlue,
										   bmiColors[lowIndex].rgbGreen,
										   bmiColors[lowIndex].rgbRed );
			}
		}
		else if( bmif.biBitCount == 1 )
		{
			long loc = 0;
			int cnt = 0;
			unsigned char color;
			for( int x = 0; x < bmif.biWidth; x+=8 )
			{
				color = buffer[cnt++];
				unsigned char bit = 0x80;
				
				for( int i = 0; i < 8 && x + i < bmif.biWidth; i++ )
				{	unsigned char thisBit = bit >> i;
					short index = (unsigned char)( (thisBit & color) >> ( 7 - i ) );
					
					rowThree[loc++] = colour( bmiColors[index].rgbBlue,
										   bmiColors[index].rgbGreen,
										   bmiColors[index].rgbRed );
				}
			}
		}
	}
	delete [] buffer;
}

bool tkBitmap::WriteDiskToDisk(const char * SourceFile, const char * DestinationFile)
{
	return false; //until code is written
}