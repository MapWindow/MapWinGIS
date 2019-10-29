/**************************************************************************************
 * File name: LabelOptions.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Definition of CLabelOptions
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
 // Sergei Leschinski (lsu) 10 april 2011 - created the file
#include "stdafx.h"
#include "LabelOptions.h"

// ********************************************************
//		Deserialize()
// ********************************************************
void CLabelOptions::DeserializeFromNode(CPLXMLNode* node)
{
	if (node)	// it's assumed here that a correct node is passed
	{
		CString s;
		
		// setting default options first
		CLabelOptions opt;
		*this = opt;
		
		// set all the saved options
		s = CPLGetXMLValue(node, "Alignment", NULL);
		if (s != "") this->alignment = (tkLabelAlignment)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontColor", NULL);
		if (s != "") this->fontColor = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontColor2", NULL);
		if (s != "") this->fontColor2 = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontGradientMode", NULL);
		if (s != "") this->fontGradientMode = (tkLinearGradientMode)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontName", NULL);
		if (s != "")
		{
			SysFreeString(this->fontName);
			this->fontName = A2BSTR(s);
		}
	
		s = CPLGetXMLValue(node, "FontOutlineColor", NULL);
		if (s != "") this->fontOutlineColor = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontOutlineVisible", NULL);
		if (s != "") this->fontOutlineVisible = atoi(s.GetString()) == 0? false : true;

		s = CPLGetXMLValue(node, "FontOutlineWidth", NULL);
		if (s != "") this->fontOutlineWidth = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontSize", NULL);
		if (s != "") this->fontSize = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontSize2", NULL);
		if (s != "") this->fontSize2 = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontStyle", NULL);
		if (s != "") this->fontStyle = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FontTransparency", NULL);
		if (s != "") this->fontTransparency = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameBackColor", NULL);
		if (s != "") this->frameBackColor = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameBackColor2", NULL);
		if (s != "") this->frameBackColor2 = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameGradientMode", NULL);
		if (s != "") this->frameGradientMode = (tkLinearGradientMode)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameOutlineColor", NULL);
		if (s != "") this->frameOutlineColor = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameOutlineStyle", NULL);
		if (s != "") this->frameOutlineStyle = (tkDashStyle)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameOutlineWidth", NULL);
		if (s != "") this->frameOutlineWidth = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FramePaddingX", NULL);
		if (s != "") this->framePaddingX = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FramePaddingY", NULL);
		if (s != "") this->framePaddingY = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameTransparency", NULL);
		if (s != "") this->frameTransparency = atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameType", NULL);
		if (s != "") this->frameType = (tkLabelFrameType)atoi(s.GetString());

		s = CPLGetXMLValue(node, "FrameVisible", NULL);
		if (s != "") this->frameVisible = atoi(s.GetString());

		s = CPLGetXMLValue(node, "HaloColor", NULL);
		if (s != "") this->haloColor = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "HaloSize", NULL);
		if (s != "") this->haloSize = atoi(s.GetString());

		s = CPLGetXMLValue(node, "HaloVisible", NULL);
		if (s != "") this->haloVisible = atoi(s.GetString()) == 0? false : true;

		s = CPLGetXMLValue(node, "InboxAlignment", NULL);
		if (s != "") this->inboxAlignment = (tkLabelAlignment)atoi(s.GetString());

		s = CPLGetXMLValue(node, "LineOrientation", NULL);
		if (s != "") this->lineOrientation = (tkLineLabelOrientation)atoi(s.GetString());
		
		s = CPLGetXMLValue(node, "OffsetX", NULL);
		if (s != "") this->offsetX = atof(s.GetString());

		s = CPLGetXMLValue(node, "OffsetY", NULL);
		if (s != "") this->offsetY = atof(s.GetString());

        s = CPLGetXMLValue(node, "OffsetXField", NULL);
        if (s != "") this->offsetXField = atol(s.GetString());

        s = CPLGetXMLValue(node, "OffsetYField", NULL);
        if (s != "") this->offsetYField = atol(s.GetString());

		s = CPLGetXMLValue(node, "ShadowColor", NULL);
		if (s != "") this->shadowColor = (OLE_COLOR)atoi(s.GetString());

		s = CPLGetXMLValue(node, "ShadowOffsetX", NULL);
		if (s != "") this->shadowOffsetX = atoi(s.GetString());

		s = CPLGetXMLValue(node, "ShadowOffsetY", NULL);
		if (s != "") this->shadowOffsetY = atoi(s.GetString());

		s = CPLGetXMLValue(node, "ShadowVisible", NULL);
		if (s != "") this->shadowVisible = atoi(s.GetString()) == 0? false : true;
		
		s = CPLGetXMLValue(node, "Visible", NULL);
		if (s != "") this->visible = atoi(s.GetString()) == 0? false : true;

        s = CPLGetXMLValue(node, "OffsetXField", NULL);
        if (s != "") this->offsetXField = atoi(s.GetString());

        s = CPLGetXMLValue(node, "OffsetYField", NULL);
        if (s != "") this->offsetYField = atoi(s.GetString());
	}
}

// ********************************************************
//		Deserialize()
// ********************************************************
void CLabelOptions::Deserialize(CString s)
{
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	DeserializeFromNode(node);
	CPLDestroyXMLNode(node);
}

// ********************************************************
//		SerializeToTree()
// ********************************************************
CPLXMLNode* CLabelOptions::SerializeToTree(CString elementName)
{
	USES_CONVERSION;
    CPLXMLNode *psTree = CPLCreateXMLNode(NULL, CXT_Element, elementName.GetString());
	CLabelOptions* opt = new CLabelOptions();

	if (opt->alignment != this->alignment)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Alignment", CPLString().Printf("%d", (int)this->alignment));

	if (opt->fontColor != this->fontColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontColor", CPLString().Printf("%d", this->fontColor));
	
	if (opt->fontColor2 != this->fontColor2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontColor2", CPLString().Printf("%d", this->fontColor2));
	
	if (opt->fontGradientMode != this->fontGradientMode)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontGradientMode", CPLString().Printf("%d", this->fontGradientMode));
	
	CString str = OLE2CA(this->fontName);
	CString str2 = OLE2CA(opt->fontName);
	if (str != str2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontName", str);
	
	if (opt->fontOutlineColor != this->fontOutlineColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontOutlineColor", CPLString().Printf("%d", this->fontOutlineColor));
	
	if (opt->fontOutlineVisible != this->fontOutlineVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontOutlineVisible", CPLString().Printf("%d", (int)this->fontOutlineVisible));
	
	if (opt->fontOutlineWidth != this->fontOutlineWidth)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontOutlineWidth", CPLString().Printf("%d", this->fontOutlineWidth));
	
	if (opt->fontSize != this->fontSize)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontSize", CPLString().Printf("%d", this->fontSize));

	if (opt->fontSize2 != this->fontSize2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontSize2", CPLString().Printf("%d", this->fontSize2));
	
	if (opt->fontStyle != this->fontStyle)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontStyle", CPLString().Printf("%d", this->fontStyle));
	
	if (opt->fontTransparency != this->fontTransparency)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FontTransparency", CPLString().Printf("%d", this->fontTransparency));
	
	if (opt->frameBackColor != this->frameBackColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameBackColor", CPLString().Printf("%d", this->frameBackColor));
	
	if (opt->frameBackColor2 != this->frameBackColor2)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameBackColor2", CPLString().Printf("%d", this->frameBackColor2));
	
	if (opt->frameGradientMode != this->frameGradientMode)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameGradientMode", CPLString().Printf("%d", (int)this->frameGradientMode));
	
	if (opt->frameOutlineColor != this->frameOutlineColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameOutlineColor", CPLString().Printf("%d", this->frameOutlineColor));
	
	if (opt->frameOutlineStyle != this->frameOutlineStyle)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameOutlineStyle", CPLString().Printf("%d", (int)this->frameOutlineStyle));
	
	if (opt->frameOutlineWidth != this->frameOutlineWidth)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameOutlineWidth", CPLString().Printf("%d", this->frameOutlineWidth));
	
	if (opt->framePaddingX != this->framePaddingX)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FramePaddingX", CPLString().Printf("%d", this->framePaddingX));
	
	if (opt->framePaddingY != this->framePaddingY)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FramePaddingY", CPLString().Printf("%d", this->framePaddingY));
	
	if (opt->frameTransparency != this->frameTransparency)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameTransparency", CPLString().Printf("%d", this->frameTransparency));
	
	if (opt->frameType != this->frameType)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameType", CPLString().Printf("%d", (int)this->frameType));
	
	if (opt->frameVisible != this->frameVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FrameVisible", CPLString().Printf("%d", (int)this->frameVisible));

	if (opt->haloColor != this->haloColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "HaloColor", CPLString().Printf("%d", (int)this->haloColor));
	
	if (opt->haloSize != this->haloSize)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "HaloSize", CPLString().Printf("%d", this->haloSize));
	
	if (opt->haloVisible != this->haloVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "HaloVisible", CPLString().Printf("%d", (int)this->haloVisible));
	
	if (opt->inboxAlignment != this->inboxAlignment)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "InboxAlignment", CPLString().Printf("%d", (int)this->inboxAlignment));
	
	if (opt->lineOrientation != this->lineOrientation)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LineOrientation", CPLString().Printf("%d", (int)this->lineOrientation));
	
	if (opt->offsetX != this->offsetX)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "OffsetX", CPLString().Printf("%G", this->offsetX));
	
	if (opt->offsetY != this->offsetY)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "OffsetY", CPLString().Printf("%G", this->offsetY));

    if (opt->offsetXField != this->offsetXField)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "OffsetXField", CPLString().Printf("%d", this->offsetXField));

    if (opt->offsetYField != this->offsetYField)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "OffsetYField", CPLString().Printf("%d", this->offsetYField));

	if (opt->shadowColor != this->shadowColor)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShadowColor", CPLString().Printf("%d", (int)this->shadowColor));
	
	if (opt->shadowOffsetX != this->shadowOffsetX)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShadowOffsetX", CPLString().Printf("%d", this->shadowOffsetX));
	
	if (opt->shadowOffsetY != this->shadowOffsetY)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShadowOffsetY", CPLString().Printf("%d", this->shadowOffsetY));
	
	if (opt->shadowVisible != this->shadowVisible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ShadowVisible", CPLString().Printf("%d", this->shadowVisible));
	
	if (opt->visible != this->visible)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Visible", CPLString().Printf("%d", this->visible));

	delete opt;
	return psTree;
}
