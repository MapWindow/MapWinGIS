//********************************************************************************************************
//File name: Macros.h
//Description:  Defines common macro functions.
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
//3-28-2005 dpa - Added point selection.
//5-2-2005 dpa - Modified bitblitting behavior.
//3-16-2006 Chris Michaelis (cdm) -- Added the ability to use multiple (differing) icons 
//			in a single layer through the new functions set_ShapePointImageListID and 
//			set_UDPointImageListAdd; also the new tkPointType enum value "ptImageList".
//			Ramifications in macros.h are observed in the DRAW_POINT macro.
// 11-7-2008 Andrew Purkis (ajp) -- Added the ability to use Font Characters as display points for
//			Point Shape files, functionality is very similar to ptImageList. 
//          Added the new tkPointType enum value "ptFontChar"
//			Ramifications in macros.h are observed in the DRAW_POINT macro.
// 27-aug-2009	lsu - Added PS_DASHDOTDOT and HS_CROSS styles. Some macros for CMapView::DrawShapefileAlt function
//********************************************************************************************************
# ifndef MAP_MACROS
# define MAP_MACROS

//MACROS
	
	# define SWAP(c,d,ctmp)(ctmp=c,\
							c=d,\
							d=ctmp)	
	# define SWAP_ENDIAN_INT(c)\
	{\
		char ctmp;\
		SWAP(c[0],c[3],ctmp);\
		SWAP(c[1],c[2],ctmp);\
	}
	
	//Shade1974 - to help eliminate 1 pixel deviation in y direction for smo
	//code Shade1974 replaced 
	//	piY = (prY - extents.bottom)*m_pixelPerProjectionY;\
	//	piY = m_viewHeight-piY-1;\ //
	# define PROJECTION_TO_PIXEL(prX,prY,piX,piY)\
	{\
		piX = (prX - extents.left)*m_pixelPerProjectionX;\
		piY = (extents.top - prY) * m_pixelPerProjectionY;\
	}

	# define DRAW_POINT(dc,sli,si,pixX,pixY,hsize)\
	{\
		if( si->pointType == ptSquare )\
		{	if( si->pointSize <= 1 )\
				dc->SetPixelV(pixX,pixY,si->pointClr);\
			else\
				dc->FillSolidRect(pixX-hsize,pixY-hsize,si->pointSize,si->pointSize,si->pointClr);\
		}\
		else if( si->pointType == ptCircle )\
		{\
			if( si->pointSize <= 1 )\
				dc->SetPixel(pixX,pixY,si->pointClr);\
			else\
				dc->Ellipse(pixX-hsize,pixY-hsize,pixX+hsize,pixY+hsize);\
		}\
		else if( si->pointType == ptDiamond )\
		{	CPoint pnts[4];\
			pnts[0]=CPoint(pixX,pixY-hsize);\
			pnts[1]=CPoint(pixX-hsize,pixY);\
			pnts[2]=CPoint(pixX,pixY+hsize);\
			pnts[3]=CPoint(pixX+hsize,pixY);\
			dc->Polygon(pnts,4);\
		}\
		else if( si->pointType == ptTriangleUp )\
		{	CPoint pnts[3];\
			pnts[0]=CPoint(pixX,pixY-hsize);\
			pnts[1]=CPoint(pixX-hsize,pixY+hsize);\
			pnts[2]=CPoint(pixX+hsize,pixY+hsize);\
			dc->Polygon(pnts,3);\
		}\
		else if( si->pointType == ptTriangleDown )\
		{	CPoint pnts[3];\
			pnts[0]=CPoint(pixX,pixY+hsize);\
			pnts[1]=CPoint(pixX+hsize,pixY-hsize);\
			pnts[2]=CPoint(pixX-hsize,pixY-hsize);\
			dc->Polygon(pnts,3);\
		}\
		else if( si->pointType == ptTriangleLeft )\
		{	CPoint pnts[3];\
			pnts[0]=CPoint(pixX-hsize,pixY);\
			pnts[1]=CPoint(pixX+hsize,pixY+hsize);\
			pnts[2]=CPoint(pixX+hsize,pixY-hsize);\
			dc->Polygon(pnts,3);\
		}\
		else if( si->pointType == ptTriangleRight )\
		{	CPoint pnts[3];\
			pnts[0]=CPoint(pixX+hsize,pixY);\
			pnts[1]=CPoint(pixX-hsize,pixY-hsize);\
			pnts[2]=CPoint(pixX-hsize,pixY+hsize);\
			dc->Polygon(pnts,3);\
		}\
		else if( si->pointType == ptUserDefined )\
		{\
			long width = sli->udPointTypeWidth;\
			long height = sli->udPointTypeHeight;\
			if( width<=0 || height<=0 || sli->udPointType==NULL)\
				dc->SetPixelV(pixX,pixY,si->pointClr);\
			else\
			{	double left=pixX-width*.5;\
				double right=pixX+width*.5;\
				double top=pixY+height*.5;\
				double bottom=pixY-height*.5;\
				if( left> m_viewWidth || right < 0 || bottom > m_viewHeight || top < 0 )\
					continue;\
				if( si->pointSize != 1.0 )\
				{\
					if(si->pointClr == sli->pointClr){\
						CDC * stretchDC = new CDC();\
						stretchDC->CreateCompatibleDC(dc);\
						CBitmap * bmp = new CBitmap();\
						CBitmap * oldbmp = NULL;\
						long stretchWidth=sli->udPointTypeWidth*si->pointSize;\
						long stretchHeight=sli->udPointTypeHeight*si->pointSize;\
						bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);\
						oldbmp = stretchDC->SelectObject(bmp);\
						stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,sli->udDC,0,0,width,height,SRCCOPY);\
						TransparentBlt(dc->m_hDC,pixX-(stretchWidth*.5),pixY-(stretchHeight*.5),stretchWidth,stretchHeight,stretchDC->m_hDC,0,0,stretchWidth,stretchHeight,sli->udTransColor);\
						stretchDC->SelectObject(oldbmp);\
						bmp->DeleteObject();\
						stretchDC->DeleteDC();\
						delete stretchDC;\
						delete bmp;\
					}else{\
						long r, g, b;\
						CDC * stretchDC = new CDC();\
						stretchDC->CreateCompatibleDC(dc);\
						CBitmap * bmp = new CBitmap();\
						CBitmap * oldbmp = NULL;\
						long stretchWidth=sli->udPointTypeWidth*si->pointSize;\
						long stretchHeight=sli->udPointTypeHeight*si->pointSize;\
						COLORREF color;\
						COLORREF transparent_color = (COLORREF)sli->udTransColor;\
						bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);\
						oldbmp = stretchDC->SelectObject(bmp);\
						for(int i = 0; i < height; i++){\
							for(int j = 0; j < width; j++){\
								color = sli->udDC->GetPixel(j,i);\
								color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));\
								stretchDC->SetPixel(j,i,color);\
							}\
						}\
						r = 255-GetRValue(transparent_color);\
						g = 255-GetGValue(transparent_color);\
						b = 255-GetBValue(transparent_color);\
						stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,stretchDC,0,0,width,height,SRCCOPY);\
						TransparentBlt(dc->m_hDC,pixX-(stretchWidth*.5),pixY-(stretchHeight*.5),stretchWidth,stretchHeight,stretchDC->m_hDC,0,0,stretchWidth,stretchHeight,RGB(r,g,b));\
						stretchDC->SelectObject(oldbmp);\
						bmp->DeleteObject();\
						stretchDC->DeleteDC();\
						delete stretchDC;\
						delete bmp;\
					}\
				}\
				else\
				{\
					if(si->pointClr == sli->pointClr){\
						TransparentBlt(dc->m_hDC,pixX-(width*.5),pixY-(height*.5),width,height,sli->udDC->m_hDC,0,0,width,height,sli->udTransColor);\
					}else{\
						long r, g, b;\
						CDC * selectedDC = new CDC();\
						selectedDC->CreateCompatibleDC(dc);\
						CBitmap * bmp = new CBitmap();\
						CBitmap * oldbmp = NULL;\
						COLORREF color;\
						COLORREF transparent_color = (COLORREF)sli->udTransColor;\
						bmp->CreateDiscardableBitmap(dc,width,height);\
						oldbmp = selectedDC->SelectObject(bmp);\
						for(int i = 0; i < height; i++){\
							for(int j = 0; j < width; j++){\
								color = sli->udDC->GetPixel(j,i);\
								color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));\
								selectedDC->SetPixel(j,i,color);\
							}\
						}\
						r = 255-GetRValue(transparent_color);\
						g = 255-GetGValue(transparent_color);\
						b = 255-GetBValue(transparent_color);\
						TransparentBlt(dc->m_hDC,pixX-(width*.5),pixY-(height*.5),width,height,selectedDC->m_hDC,0,0,width,height,RGB(r,g,b));\
						selectedDC->SelectObject(oldbmp);\
						bmp->DeleteObject();\
						selectedDC->DeleteDC();\
						delete selectedDC;\
						delete bmp;\
					}\
				}\
			}\
		}\
		else if( si->pointType == ptImageList )\
		{\
			if( si->ImageListIndex == -1)\
				dc->SetPixelV(pixX,pixY,si->pointClr);\
			else\
			{	udPointListItem * iconItem = sli->PointImageList[si->ImageListIndex];\
				long width = iconItem->udPointTypeWidth;\
				long height = iconItem->udPointTypeHeight;\
				if( width<=0 || height<=0 || iconItem->udPointType==NULL)\
					dc->SetPixelV(pixX,pixY,si->pointClr);\
				else\
				{	double left=pixX-width*.5;\
					double right=pixX+width*.5;\
					double top=pixY+height*.5;\
					double bottom=pixY-height*.5;\
					if( left> m_viewWidth || right < 0 || bottom > m_viewHeight || top < 0 )\
						continue;\
					if( si->pointSize != 1.0 )\
					{\
						if(si->pointClr == sli->pointClr){\
							CDC * stretchDC = new CDC();\
							stretchDC->CreateCompatibleDC(dc);\
							CBitmap * bmp = new CBitmap();\
							CBitmap * oldbmp = NULL;\
							long stretchWidth=iconItem->udPointTypeWidth*si->pointSize;\
							long stretchHeight=iconItem->udPointTypeHeight*si->pointSize;\
							bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);\
							oldbmp = stretchDC->SelectObject(bmp);\
							stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,iconItem->udDC,0,0,width,height,SRCCOPY);\
							TransparentBlt(dc->m_hDC,pixX-(stretchWidth*.5),pixY-(stretchHeight*.5),stretchWidth,stretchHeight,stretchDC->m_hDC,0,0,stretchWidth,stretchHeight,iconItem->udTransColor);\
							stretchDC->SelectObject(oldbmp);\
							bmp->DeleteObject();\
							stretchDC->DeleteDC();\
							delete stretchDC;\
							delete bmp;\
						}else{\
							long r, g, b;\
							CDC * stretchDC = new CDC();\
							stretchDC->CreateCompatibleDC(dc);\
							CBitmap * bmp = new CBitmap();\
							CBitmap * oldbmp = NULL;\
							long stretchWidth=iconItem->udPointTypeWidth*si->pointSize;\
							long stretchHeight=iconItem->udPointTypeHeight*si->pointSize;\
							COLORREF color;\
							COLORREF transparent_color = (COLORREF)iconItem->udTransColor;\
							bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);\
							oldbmp = stretchDC->SelectObject(bmp);\
							for(int i = 0; i < height; i++){\
								for(int j = 0; j < width; j++){\
									color = iconItem->udDC->GetPixel(j,i);\
									color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));\
									stretchDC->SetPixel(j,i,color);\
								}\
							}\
							r = 255-GetRValue(transparent_color);\
							g = 255-GetGValue(transparent_color);\
							b = 255-GetBValue(transparent_color);\
							stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,stretchDC,0,0,width,height,SRCCOPY);\
							TransparentBlt(dc->m_hDC,pixX-(stretchWidth*.5),pixY-(stretchHeight*.5),stretchWidth,stretchHeight,stretchDC->m_hDC,0,0,stretchWidth,stretchHeight,RGB(r,g,b));\
							stretchDC->SelectObject(oldbmp);\
							bmp->DeleteObject();\
							stretchDC->DeleteDC();\
							delete stretchDC;\
							delete bmp;\
						}\
					}\
					else\
					{\
						if(si->pointClr == sli->pointClr){\
							TransparentBlt(dc->m_hDC,pixX-(width*.5),pixY-(height*.5),width,height,iconItem->udDC->m_hDC,0,0,width,height,iconItem->udTransColor);\
						}else{\
							long r, g, b;\
							CDC * selectedDC = new CDC();\
							selectedDC->CreateCompatibleDC(dc);\
							CBitmap * bmp = new CBitmap();\
							CBitmap * oldbmp = NULL;\
							COLORREF color;\
							COLORREF transparent_color = (COLORREF)iconItem->udTransColor;\
							bmp->CreateDiscardableBitmap(dc,width,height);\
							oldbmp = selectedDC->SelectObject(bmp);\
							for(int i = 0; i < height; i++){\
								for(int j = 0; j < width; j++){\
									color = iconItem->udDC->GetPixel(j,i);\
									color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));\
									selectedDC->SetPixel(j,i,color);\
								}\
							}\
							r = 255-GetRValue(transparent_color);\
							g = 255-GetGValue(transparent_color);\
							b = 255-GetBValue(transparent_color);\
							TransparentBlt(dc->m_hDC,pixX-(width*.5),pixY-(height*.5),width,height,selectedDC->m_hDC,0,0,width,height,RGB(r,g,b));\
							selectedDC->SelectObject(oldbmp);\
							bmp->DeleteObject();\
							selectedDC->DeleteDC();\
							delete selectedDC;\
							delete bmp;\
						}\
					}\
				}\
			}\
		}\
		else if( si->pointType == ptFontChar )\
		{\
			if( si->FontCharListIndex == -1)\
				dc->SetPixelV(pixX,pixY,si->pointClr);\
			else\
			{\
				CFont * charFont = new CFont();\
				CFont * oldFont = NULL;\
				COLORREF oldColor;\
				char cVal[2];\
				LOGFONT lf;\
				RECT myRect;\
				CSize mySize;\
				udPointListFontCharItem *fontCharItem = sli->PointFontCharList[si->FontCharListIndex];\
				cVal[0] = fontCharItem->udPointFontCharIdx;\
				cVal[1] = '\0';\
				sli->GetFont()->GetLogFont(&lf);\
				lf.lfHeight = sli->fontSize * 10;\
				charFont->CreatePointFontIndirect(&lf, dc);\
				oldFont = dc->SelectObject(charFont);\
				oldColor = dc->SetTextColor(fontCharItem->udPointFontCharColor);\
				mySize = dc->GetTextExtent(cVal, 1);\
				myRect.top = pixY - (mySize.cy / 2);\
				myRect.bottom =myRect.top + mySize.cy;\
				myRect.left = pixX - (mySize.cx / 2);\
				myRect.right = myRect.left  + mySize.cx;\
				dc->DrawTextEx(cVal, 1, &myRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE , NULL);\
				dc->SelectObject(oldFont);\
				dc->SetTextColor(oldColor);\
				charFont->DeleteObject();\
			}\
		}\
	}\

	# define LINE_STIPPLE(sli,si,newPen)\
	{\
		if( si->lineStipple != lsNone )\
		{	switch( si->lineStipple )\
			{\
				case( lsDotted ):{\
					newPen->CreatePen(PS_DOT,si->lineWidth,si->lineClr);\
					break;}\
				case( lsDashed ):{\
					newPen->CreatePen(PS_DASH,1,si->lineClr);\
					break;}\
				case( lsDashDotDash ):{\
					newPen->CreatePen(PS_DASHDOT,1,si->lineClr);\
					break;}\
				case ( lsCustom ):{\
					/* Create a custom geometric pen.*/\
					createCustomPen(sli, si, newPen);\
					break;}\
				case ( lsDoubleSolid ):{\
					newPen->CreatePen(PS_SOLID,si->lineWidth,si->fillClr);\
					break;}\
				case ( lsDashDotDot ):{\
					newPen->CreatePen(PS_DASHDOTDOT,si->lineWidth,si->fillClr);\
					break;}\
				default:{\
					newPen->CreatePen(PS_SOLID,si->lineWidth,si->fillClr);\
					break;}\
			}\
		}\
		else\
			{ newPen->CreatePen(PS_SOLID,si->lineWidth,si->lineClr);}\
	}\

	# define FILL_STIPPLE(dc,si,newBrush)\
	{\
		CBitmap * bmp=NULL;\
		if( si->fillStipple != fsNone )\
		{	switch( si->fillStipple )\
			{\
				case( fsVerticalBars ):\
					newBrush->CreateHatchBrush(HS_VERTICAL, si->stippleLineClr);\
					break;\
				case( fsHorizontalBars ):\
					newBrush->CreateHatchBrush(HS_HORIZONTAL, si->stippleLineClr);\
					break;\
				case( fsDiagonalDownRight ):\
					newBrush->CreateHatchBrush(HS_BDIAGONAL, si->stippleLineClr);\
					break;\
				case( fsDiagonalDownLeft ):\
					newBrush->CreateHatchBrush(HS_FDIAGONAL, si->stippleLineClr);\
					break;\
				case( fsPolkaDot ):\
					newBrush->CreateHatchBrush(HS_DIAGCROSS, si->stippleLineClr);\
					break;\
				case( fsCross ):\
					newBrush->CreateHatchBrush(HS_CROSS, si->stippleLineClr);\
					break;\
				default:\
					newBrush->CreateSolidBrush(BGR_TO_RGB(si->fillClr));\
					break;\
			}\
		}\
		else\
			newBrush->CreateSolidBrush(si->fillClr);\
		if( bmp )\
			delete bmp;\
	}\

	# define SET_SIGN(val,SH)(val<0?SH=-1:SH=1)
	# define OUT_OF_VIEW(xm,ym,xM,yM,extents)(xm>extents.right||xM<extents.left||ym>extents.top||yM<extents.bottom?TRUE:FALSE)
	# define IS_VALID_PTR(ptr)(ptr==NULL?FALSE:TRUE)
	# define IS_VALID_INDEX(index,size)(index>=0&&index<size?TRUE:FALSE)
	# define MAXIMUM(n1,n2)((n1)>(n2)?(n1):(n2))
	# define MINIMUM(n1,n2)((n1)<(n2)?(n1):(n2))

	# define IS_VALID_LAYER(layerHandle,allLayers)(layerHandle >= 0 && layerHandle < allLayers.size()?(allLayers[layerHandle]!=NULL?TRUE:FALSE):FALSE)
	# define IS_VALID_LAYER_POSITION(position,activeLayers)(position >= 0 && position < activeLayers.size()?TRUE:FALSE)
	# define IS_VALID_DRAWLIST(listHandle,allDrawLists)(listHandle >= 0 && listHandle < allDrawLists.size()?(allDrawLists[listHandle]!=NULL?TRUE:FALSE):FALSE)
	
	# define PROJX_TO_PIXELX(prX,piX,extents,ppx)(piX=(prX - extents.left)*ppx)
	# define PROJY_TO_PIXELY(prY,piY)(piY=(prY - extents.bottom)*ppy)
	#define BGR_TO_RGB(color)((color & 0x00FF0000)>>16) | ((color & 0x0000FF00)) | ((color & 0x000000FF)<<16)
	
	
	// lsu 27 aug 2009 for DrawShapefileAlt function
	# define PROJECTION_TO_PIXEL_INT(prX,prY,pxX,pxY)\
	{\
		double dX = (prX - extents.left)* m_pixelPerProjectionX;\
		double dY = (extents.top - prY) * m_pixelPerProjectionY;\
		pxX = Utility::Rint(dX);\
		pxY = Utility::Rint(dY);\
	}

	# define PROJECTION_TO_PIXEL_INT1(prX,prY,pxX,pxY, xMin, yMin, dx, dy)\
	{\
		double dX = (prX - xMin)* dx;\
		double dY = (yMin - prY) * dy;\
		pxX = (int)dX;\
		pxY = (int)dY;\
	}

	#define DECODE_FILL_STIPPLE(fillStipple, hatchStyle)\
	{\
		switch( fillStipple )\
		{\
			case( fsVerticalBars ):\
				hatchStyle = HS_VERTICAL;\
				break;\
			case( fsHorizontalBars ):\
				hatchStyle = HS_HORIZONTAL;\
				break;\
			case( fsDiagonalDownRight ):\
				hatchStyle = HS_BDIAGONAL;\
				break;\
			case( fsDiagonalDownLeft ):\
				hatchStyle = HS_FDIAGONAL;\
				break;\
			case( fsPolkaDot ):\
				hatchStyle = HS_DIAGCROSS;\
				break;\
			case( fsCross ):\
				hatchStyle = HS_CROSS;\
				break;\
		}\
	}\
	
	# define DECODE_LINE_STIPPLE(lineStipple, penStyle)\
	{\
		switch( lineStipple )\
		{\
			case( lsDoubleSolid ):\
				penStyle = PS_SOLID;\
				break;\
			case( lsDotted ):\
				penStyle = PS_DOT;\
				break;\
			case( lsDashed ):\
				penStyle = PS_DASH;\
				break;\
			case( lsDashDotDash ):\
				penStyle = PS_DASHDOT;\
				break;\
			case( lsDashDotDot ):\
				penStyle = PS_DASHDOTDOT;\
				break;\
		}\
	}\

	# define RELEASE_BRUSH(dc, newBrush, oldBrush)\
	{\
		dc->SelectObject(oldBrush);\
		newBrush->DeleteObject();\
		delete newBrush;\
		newBrush = NULL;\
	}\

	# define RELEASE_PEN(dc, newPen, oldPen)\
	{\
		dc->SelectObject(oldPen);\
		newPen->DeleteObject();\
		delete newPen;\
		newPen = NULL;\
	}\
//MACROS

# endif