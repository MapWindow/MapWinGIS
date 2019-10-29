/**************************************************************************************
 * File name: LabelOptions.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CLabelOptions
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 // Sergei Leschinski (lsu) 25 june 2010 - created the file

#pragma once
#include "RotatedRectangle.h"
#include "cpl_minixml.h"

class CLabelOptions
{
public:
	CLabelOptions(void)
	{
		visible = true;

		// position
		offsetX = offsetY = 0.0;
        offsetXField = offsetYField = -1;
		alignment = laCenter;
		lineOrientation = lorParallel;
		inboxAlignment = laCenter;
		
		// font
		USES_CONVERSION;
		fontName = A2BSTR("Arial");
		fontSize = 10;
		fontSize2 = 14;
		fontStyle = fstRegular;
		fontColor = 0;
		fontColor2 = 0;
		fontGradientMode = gmNone;
		fontTransparency = 255;
		
		// outlines
		fontOutlineVisible = false;
		shadowVisible = false;
		haloVisible = false;
		fontOutlineColor = RGB(255,0,0);
		shadowColor = 12237498;
		haloColor = RGB(255,255,255);
		fontOutlineWidth = 1;
		shadowOffsetX = 2;
		shadowOffsetY = 2;
		haloSize = 2;

		// frame
		frameType = lfRectangle;
		frameOutlineColor = RGB(150, 150, 150);
		frameBackColor = RGB(255, 255, 255); // 13952764; //RGB(255, 165, 0);		// orange
		frameBackColor2 = RGB(176,224,230);
		frameGradientMode = gmNone;
		frameOutlineStyle = dsSolid;			// PS_SOLID
		frameOutlineWidth = 1;
		framePaddingX = 10;
		framePaddingY = 0;
		frameTransparency = 255;
		frameVisible = true;

	}
	~CLabelOptions()
	{
		::SysFreeString(fontName);
	}
	
	// redefinition of = operator
	CLabelOptions& operator=(const CLabelOptions& lbl)
	{
		if (this == &lbl)
			return *this;
		
		visible = lbl.visible;

		// position
		offsetX = lbl.offsetX;
		offsetY = lbl.offsetY;
        offsetXField = lbl.offsetXField;
        offsetYField = lbl.offsetYField;
		alignment = lbl.alignment;
		lineOrientation = lbl.lineOrientation;
		inboxAlignment = lbl.inboxAlignment;
		
		// font
		SysFreeString(fontName);
		fontName = SysAllocString(lbl.fontName);

		fontSize = lbl.fontSize;
		fontSize2 = lbl.fontSize2;
		fontStyle = lbl.fontStyle;
		fontColor = lbl.fontColor;
		fontColor2 = lbl.fontColor2;
		fontGradientMode = lbl.fontGradientMode;
		fontTransparency = lbl.fontTransparency;
		
		// outlines
		fontOutlineVisible = lbl.fontOutlineVisible;
		shadowVisible = lbl.shadowVisible;
		haloVisible = lbl.haloVisible;
		fontOutlineColor = lbl.fontOutlineColor;
		shadowColor = lbl.shadowColor;
		haloColor = lbl.haloColor;
		fontOutlineWidth = lbl.fontOutlineWidth;
		shadowOffsetX = lbl.shadowOffsetX;
		shadowOffsetY = lbl.shadowOffsetY;
		haloSize = lbl.haloSize;
		
		// frame
		frameType = lbl.frameType;
		frameOutlineColor = lbl.frameOutlineColor ;
		frameBackColor = lbl.frameBackColor;
		frameBackColor2 = lbl.frameBackColor2;
		frameGradientMode = lbl.frameGradientMode;
		frameOutlineStyle = lbl.frameOutlineStyle;			// PS_SOLID
		frameOutlineWidth = lbl.frameOutlineWidth;
		framePaddingX = lbl.framePaddingX;
		framePaddingY = lbl.framePaddingY;
		frameTransparency = lbl.frameTransparency;
		frameVisible = lbl.frameVisible;

		return *this;
	}

	bool visible;

	// position
	double offsetX;
	double offsetY;
    long offsetXField;
    long offsetYField;
	tkLabelAlignment alignment;
	tkLineLabelOrientation lineOrientation;
	tkLabelAlignment inboxAlignment;

	// font
	BSTR fontName;
	int fontSize;
	int fontSize2;
	long fontStyle;
	OLE_COLOR fontColor;
	OLE_COLOR fontColor2;
	tkLinearGradientMode fontGradientMode;
	int fontTransparency;

	// outlines
	bool fontOutlineVisible;
	bool shadowVisible;
	bool haloVisible;
	OLE_COLOR fontOutlineColor;
	OLE_COLOR shadowColor;
	OLE_COLOR haloColor;
	int fontOutlineWidth;
	int shadowOffsetX;
	int shadowOffsetY;
	long haloSize;	// from 0 to 1 relative to height

	// frame
	tkLabelFrameType frameType;
	OLE_COLOR frameOutlineColor;
	OLE_COLOR frameBackColor;
	OLE_COLOR frameBackColor2;
	tkLinearGradientMode frameGradientMode;
	tkDashStyle frameOutlineStyle;
	int frameOutlineWidth;
	int framePaddingX;
	int framePaddingY;
	int frameTransparency;
	int frameVisible;

	void Deserialize(CString s);
	CPLXMLNode* SerializeToTree(CString elementName );
	void DeserializeFromNode(CPLXMLNode* node);
	
};

// ***********************************************************
//		Structure CLabelInfo
// ***********************************************************
// structure used internally by CLabels and CLabelClass
struct CLabelInfo
{	
public:
	CLabelInfo()
	{
		fontSize = 0;
		visible	= true;
		x = y = 0.0;
        offsetX = offsetY = 0.0;
		rotation = 0.0;
		text = "";
		category = -1;
		rotatedFrame = NULL;
		horizontalFrame = NULL;
		isDrawn = VARIANT_FALSE;
	}
	~CLabelInfo()
	{
		// don't forget to set pointer to NULL after the use of this 2 members
		// or it will be the problems here
		if (rotatedFrame) delete rotatedFrame;
		if (horizontalFrame) delete horizontalFrame;
	}
	
	CRotatedRectangle* rotatedFrame;	// this members are allocated dynamically only if the label was drawn
	CRect* horizontalFrame;		// and are deleted on the next redraw
	bool visible;
	double x;
	double y;
    double offsetX;
    double offsetY;
	double rotation;
	CStringW text;
	long category;
	VARIANT_BOOL isDrawn;
	short fontSize;
};

