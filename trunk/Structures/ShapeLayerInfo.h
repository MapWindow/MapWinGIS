//********************************************************************************************************
//File name: ShapeLayerInfo.h
//Description:  Defines ShapeLayerInfo class.
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
//3-28-2005 dpa - Identical to public domain version except I changed udLineStipple=0
//3-16-2006 Chris Michaelis (cdm) -- Added the ability to use multiple (differing) icons 
//			in a single layer through the new functions set_ShapePointImageListID and 
//			set_UDPointImageListAdd; also the new tkPointType enum value "ptImageList".
// 11-7-2008 Andrew Purkis (ajp) -- Added the ability to use multiple (differing) font characters
//           in a single layer.  Added the new tkPointType enum value "ptFontChar".
//********************************************************************************************************

# ifndef SHAPE_LAYER_INFO_H
# define SHAPE_LAYER_INFO_H

# include <vector>
# include "ShapeInfo.h"
# include "BaseLayerInfo.h"




class udPointListItem
{
public:
	IImage * udPointType;
	long udPointTypeWidth;
	long udPointTypeHeight;
	unsigned char * udPointTypeImage;
	OLE_COLOR udTransColor;
	CDC * udDC;

	udPointListItem()
	{
		udPointType = NULL;
		udPointTypeWidth = 0;
		udPointTypeHeight = 0;
		udPointTypeImage = NULL;
		udDC=NULL;
	}

	~udPointListItem()
	{
		if( udDC )
		{	CBitmap * newbmp = new CBitmap();
			CBitmap * bmp = udDC->SelectObject(newbmp);
			if( bmp )
				bmp->DeleteObject();
			udDC->DeleteDC();
		}
	}
};

class udPointListFontCharItem
{
public:
	long udPointFontCharIdx;
	OLE_COLOR udPointFontCharColor;

	udPointListFontCharItem()
	{
		udPointFontCharIdx = 0;
	}

	~udPointListFontCharItem()
	{
	}
};

struct ShapeLayerInfo : public BaseLayerInfo
{
	ShapeLayerInfo()
	{	flags = 0;
		trans = 0;
		legend = NULL;
		pointType = ptSquare;
		pointSize = 1.0;
		udLineStipple = 0;
		fillStipple = fsNone;
		lineStipple = lsNone;
		transparentStipple = false;

		//Stipples
		long byteLoc = 0;
		char lbyte = (char)0x80;
		char rbyte = (char)0x01;

		for( int i = 0; i < 32; i++ )
		{	
			if( i%8 == 0 )
			{	lbyte = (char)0x80;
				rbyte = (char)0x01;
			}
			else
			{	lbyte = ( lbyte >> 1 ) & 0x7F;
				rbyte = ( rbyte << 1 );
			}

			//BYTE 1		
			udFillStipple[byteLoc] = 0xFF;			
			byteLoc++;

			//BYTE 2
			udFillStipple[byteLoc] = 0xFF;		
			byteLoc++;

			//BYTE 3
			udFillStipple[byteLoc] = 0xFF;		
			byteLoc++;

			//BYTE 4
			udFillStipple[byteLoc] = 0xFF;		
			byteLoc++;
		}

		udPointType = NULL;
		udPointTypeWidth = 0;
		udPointTypeHeight = 0;
		udPointTypeImage = NULL;
		udDC=NULL;
		font=NULL;
	}

	virtual ~ShapeLayerInfo()
	{	
		int endcondition = shapeInfo.size();
		for( int i = 0; i < endcondition; i++ )
			delete shapeInfo[i];
		shapeInfo.clear();

		endcondition = PointImageList.size();
		for( int i = 0; i < endcondition; i++ )
			delete PointImageList[i];
		PointImageList.clear();

		if( legend != NULL )
			legend->Release();
		legend = NULL;

		if( udPointType != NULL )
			udPointType->Release();
		udPointType = NULL;

		if( udPointTypeImage != NULL )
			delete [] udPointTypeImage;
		udPointTypeImage = NULL;

		if( udDC )
		{	CBitmap * newbmp = new CBitmap();
			CBitmap * bmp = udDC->SelectObject(newbmp);
			if( bmp )
				bmp->DeleteObject();
			udDC->DeleteDC();
		}
	}

	void AlignShapeInfo( ShapeInfo & si )
	{	si.trans = trans;
		si.fillClr = fillClr;
		si.lineClr = lineClr;
		si.pointClr = pointClr;
		si.lineWidth = lineWidth;
		si.pointSize = pointSize;
		si.lineStipple = lineStipple;
		si.fillStipple = fillStipple;
		si.stippleLineClr = stippleLineClr;
		si.transparentStipple = transparentStipple;
		si.flags = flags;
		si.lenAHead = lenAHead;
		si.pointType = pointType;
	}

	//Applied Legend Object
	IShapefileColorScheme * legend;

	//Point type
	tkPointType pointType;
	IImage * udPointType;
	long udPointTypeWidth;
	long udPointTypeHeight;
	unsigned char * udPointTypeImage;
	OLE_COLOR udTransColor;

	std::vector<udPointListItem *> PointImageList;
	
	//Font
	float fontSize;
	CString fontName;
	BOOL isBold;				
	BOOL isItalic;				
	BOOL isUnderline;

	CFont* GetFont() { if ( font ) ASSERT_VALID(font); return font;}
	inline 
	void SetFont(CFont* newFont)
	{
		if (newFont) ASSERT_VALID(newFont);
		if (font) ASSERT_VALID(font);
		delete font;
		font = NULL;
		font = newFont;
	}

	std::vector<udPointListFontCharItem *> PointFontCharList;
	
	//Transparency
	float trans;
	//Colors
	OLE_COLOR fillClr;
	OLE_COLOR lineClr;
	OLE_COLOR pointClr;
	OLE_COLOR stippleLineClr;
	bool transparentStipple;
	//Size
	float lineWidth;
	float pointSize;		
	//Stipples
	tkLineStipple lineStipple;
	tkFillStipple fillStipple;	

	/*unsigned char udLineStipple;*/
	long udLineStipple;
	unsigned char udFillStipple[128];
		
	//Additional
	long flags;
	
	//Net ShapeLayer Info
	float lenAHead;
		
	std::vector<ShapeInfo *> shapeInfo;

	//UD_DC
	CDC * udDC;
	private:
	  CFont *font;

};

# endif