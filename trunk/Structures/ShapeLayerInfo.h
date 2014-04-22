//********************************************************************************************************
//File name: ShapeLayerInfo.h
//Description:  Defines ShapeLayerInfo class.
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

//class udPointListItem
//{
//public:
//	IImage * udPointType;
//	long udPointTypeWidth;
//	long udPointTypeHeight;
//	unsigned char * udPointTypeImage;
//	OLE_COLOR udTransColor;
//	CDC * udDC;
//
//	udPointListItem()
//	{
//		udPointType = NULL;
//		udPointTypeWidth = 0;
//		udPointTypeHeight = 0;
//		udPointTypeImage = NULL;
//		udDC=NULL;
//	}
//
//	~udPointListItem()
//	{
//		if( udDC )
//		{	CBitmap * newbmp = new CBitmap();
//			CBitmap * bmp = udDC->SelectObject(newbmp);
//			if( bmp )
//				bmp->DeleteObject();
//			udDC->DeleteDC();
//		}
//	}
//};
//
//class udPointListFontCharItem
//{
//public:
//	long udPointFontCharIdx;
//	OLE_COLOR udPointFontCharColor;
//
//	udPointListFontCharItem()
//	{
//		udPointFontCharIdx = 0;
//	}
//
//	~udPointListFontCharItem()
//	{
//	}
//};

# endif