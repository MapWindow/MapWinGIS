//********************************************************************************************************
//File name: Macros.h
//Description:  Defines common macro functions.
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
	
	# define SWAP_LOCAL(c,d,ctmp)(ctmp=c,\
							c=d,\
							d=ctmp)	
	# define SWAP_ENDIAN_INT(c)\
	{\
		char ctmp;\
		SWAP_LOCAL(c[0],c[3],ctmp);\
		SWAP_LOCAL(c[1],c[2],ctmp);\
	}
	
	//Shade1974 - to help eliminate 1 pixel deviation in y direction for smo
	//code Shade1974 replaced 
	//	piY = (prY - extents.bottom)*m_pixelPerProjectionY;\
	//	piY = m_viewHeight-piY-1;\ //
	# define PROJECTION_TO_PIXEL(prX,prY,piX,piY)\
	{\
		(piX) = (float)(((prX) - _extents.left) * _pixelPerProjectionX);\
		(piY) = (float)((_extents.top - (prY)) * _pixelPerProjectionY);\
	}

	# define SET_SIGN(val,SH)(val<0?SH=-1:SH=1)
	# define OUT_OF_VIEW(xm,ym,xM,yM,_extents)(xm>_extents.right||xM<_extents.left||ym>_extents.top||yM<_extents.bottom?TRUE:FALSE)
	# define IS_VALID_PTR(ptr)(ptr==NULL?FALSE:TRUE)
	# define IS_VALID_INDEX(index,size)(index>=0&&index<size?TRUE:FALSE)
	# define MAXIMUM(n1,n2)((n1)>(n2)?(n1):(n2))
	# define MINIMUM(n1,n2)((n1)<(n2)?(n1):(n2))

	//# define IS_VALID_LAYER(layerHandle,allLayers)(layerHandle >= 0 && layerHandle < allLayers.size()?(allLayers[layerHandle]!=NULL?TRUE:FALSE):FALSE)
	# define IS_VALID_LAYER_POSITION(position,activeLayers)(position >= 0 && position < activeLayers.size()?TRUE:FALSE)
	# define IS_VALID_DRAWLIST(listHandle,allDrawLists)(listHandle >= 0 && listHandle < allDrawLists.size()?(allDrawLists[listHandle]!=NULL?TRUE:FALSE):FALSE)
	
	# define PROJX_TO_PIXELX(prX,piX,_extents,ppx)(piX=(prX - _extents.left)*ppx)
	# define PROJY_TO_PIXELY(prY,piY)(piY=(prY - _extents.bottom)*ppy)
	# define BGR_TO_RGB(color)((color & 0x00FF0000)>>16) | ((color & 0x0000FF00)) | ((color & 0x000000FF)<<16)
	# define GET_ALPHA(color)((color & 0xFF000000) >> 24)
	
	// lsu 27 aug 2009 for DrawShapefileAlt function
	# define PROJECTION_TO_PIXEL_INT(prX,prY,pxX,pxY)\
	{\
		double dX = (prX - _extents.left)* _pixelPerProjectionX;\
		double dY = (_extents.top - prY) * _pixelPerProjectionY;\
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