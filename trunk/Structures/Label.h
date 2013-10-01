//********************************************************************************************************
//File name: Label.h
//Description:  Header for Label.cpp.
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
//3-28-2005 dpa - Label rotation.
//9-30-2005 stan - New class - LabelLayer
//********************************************************************************************************
#pragma once
#include "MapWinGis.h"
#include <deque>

//dpa 3/28/2005 added property for rotation
//struct LabelData
//{	
//	LabelData()
//	{	x = 0.0;
//		y = 0.0;
//		color = 0;
//		justif = hjCenter;
//		rotation = 0;
//
//	}
//
//	LabelData( const LabelData & l )
//	{	text = l.text;
//		x = l.x;
//		y = l.y;
//		color = l.color;
//		justif = l.justif;
//		rotation = l.rotation;
//	}
//
//	~LabelData()
//	{
//	}
//	
//	tkHJustification justif;
//	CString text;
//	double x;
//	double y;
//	OLE_COLOR color;
//	long rotation;
//};
//
//class LabelLayer {
//public:
//	LabelLayer()
//	{	
//		visibleLabels = TRUE;
//		shadowLabels = FALSE;
//		scaleLabels = FALSE;
//		firstTimeLabelsDrawn = TRUE;
//		standardViewWidth = 0;
//		labelsOffset = 0;
//		shadowColor = RGB(255,255,255);
//		useLabelCollision = FALSE;
//		
//		//Font
//		fontSize = 10;
//		fontName = "Arial";
//		font = NULL;
//		isBold = FALSE;				  //ajp (30/11/07) 
//		isItalic = FALSE;			  //ajp (30/11/07)
//		isUnderline = FALSE;		//ajp (30/11/07)
//	}
//
//	~LabelLayer()
//	{
//		allLabels.clear();
//		delete font;
//	}	
//
//	BOOL visibleLabels;
//	BOOL shadowLabels;
//	BOOL scaleLabels;
//	BOOL firstTimeLabelsDrawn;
//	double standardViewWidth;
//	std::deque<LabelData> allLabels;
//	long labelsOffset;
//	OLE_COLOR shadowColor;
//	BOOL useLabelCollision;
//	BOOL isBold;				//ajp (30/11/07), set label to bold font
//	BOOL isItalic;				//ajp (30/11/07), set label to italic font
//	BOOL isUnderline;			//ajp (30/11/07), set label to underline
//
//	//Font
//	long fontSize;
//	CString fontName;
//	CFont* GetFont() { if ( font ) ASSERT_VALID(font); return font;}
//
//	inline void SetFont(CFont* newFont)
//	{
//		if (newFont) ASSERT_VALID(newFont);
//		if (font) ASSERT_VALID(font);
//		delete font;
//		font = NULL;
//		font = newFont;
//	}
//
//private:
//	CFont * font;
//};
