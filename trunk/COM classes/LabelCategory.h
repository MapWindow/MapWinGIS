/**************************************************************************************
 * File name: LabelCategory.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CLabelCategory
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
 // lsu: jan 2010 - created the file.

#pragma once
#include "MapWinGis.h"
#include "LabelOptions.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CLabelCategory
class ATL_NO_VTABLE CLabelCategory :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLabelCategory, &CLSID_LabelCategory>,
	public IDispatchImpl<ILabelCategory, &IID_ILabelCategory, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CLabelCategory()
	{
		m_name = A2BSTR("");
		m_expression = A2BSTR("");
		VariantInit(&m_minValue);
		VariantInit(&m_maxValue);
		m_priority = 0;
		m_enabled = VARIANT_TRUE;
		m_value.vt = VT_EMPTY;
		gReferenceCounter.AddRef(tkInterface::idLabelCategory);
	}
	
	~CLabelCategory()
	{
		::SysFreeString(m_name);
		::SysFreeString(m_expression);
		VariantClear(&m_minValue);
		VariantClear(&m_maxValue);
		gReferenceCounter.Release(tkInterface::idLabelCategory);
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LABELCATEGORY)


BEGIN_COM_MAP(CLabelCategory)
	COM_INTERFACE_ENTRY(ILabelCategory)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(get_Name)(BSTR* retval);
	STDMETHOD(put_Name)(BSTR newVal);

	STDMETHOD(get_Expression)(BSTR* retval);
	STDMETHOD(put_Expression)(BSTR newVal);

	STDMETHOD(get_MinValue)(VARIANT* pVal);
	STDMETHOD(put_MinValue)(VARIANT newVal);

	STDMETHOD(get_MaxValue)(VARIANT* pVal);
	STDMETHOD(put_MaxValue)(VARIANT newVal);

	STDMETHOD(get_Priority)(LONG* retval)							{*retval = m_priority;			return S_OK;};
	STDMETHOD(put_Priority)(LONG newVal)							{m_priority = newVal;			return S_OK;};
	
	// ---------------------------------------------------------------------------------
	// Properties common for CLabels and CLabelCategory
	// it's easier to port the options in separate class to avoid duplication;
	// but such disposition upon my opinion ensure more readable code in the client part
	// Also it's possible to treat CLabels basic options like Category with index 0
	// but again the code outside will be easier if we use just Labels.FontColor rather than 
	// Labels.Category[0].FontColor or Labels.Options.FontColor, therefore such disposition
	// CLabelOptions structure is used to hold the options, so delcarations can be just copied
	// between CLabels and CLabelCategory, the names of properties should be the same in both classes
	// ---------------------------------------------------------------------------------
	STDMETHOD(get_Visible)(VARIANT_BOOL* retval)					{*retval = m_options.visible;			return S_OK;};
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal)						{m_options.visible = newVal?true:false;			return S_OK;};		
	
	// position
	STDMETHOD(get_OffsetX)(double* retval)							{*retval = m_options.offsetX;			return S_OK;};
	STDMETHOD(put_OffsetX)(double newVal)							{m_options.offsetX = newVal;			return S_OK;};
	STDMETHOD(get_OffsetY)(double* retval)							{*retval = m_options.offsetY;			return S_OK;};
	STDMETHOD(put_OffsetY)(double newVal)							{m_options.offsetY = newVal;			return S_OK;};
	STDMETHOD(get_Alignment)(tkLabelAlignment* retval)				{*retval = m_options.alignment;			return S_OK;};
	STDMETHOD(put_Alignment)(tkLabelAlignment newVal)				{m_options.alignment = newVal;			return S_OK;};
	STDMETHOD(get_LineOrientation)(tkLineLabelOrientation* retval)	{*retval = m_options.lineOrientation;	return S_OK;};		
	STDMETHOD(put_LineOrientation)(tkLineLabelOrientation newVal)	{m_options.lineOrientation = newVal;	return S_OK;};
	
	// font
	STDMETHOD(get_FontName)(BSTR* retval);
	STDMETHOD(put_FontName)(BSTR newVal);
	STDMETHOD(get_FontSize)(LONG* retval)							{*retval = m_options.fontSize;			return S_OK;};
	STDMETHOD(put_FontSize)(LONG newVal)							{m_options.fontSize = newVal;			return S_OK;};
	
	STDMETHOD(get_FontItalic)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontItalic)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontBold)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontBold)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontUnderline)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontUnderline)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontStrikeOut)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontStrikeOut)(VARIANT_BOOL newVal);
	
	STDMETHOD(get_FontColor)(OLE_COLOR* retval)						{*retval = m_options.fontColor;			return S_OK;};
	STDMETHOD(put_FontColor)(OLE_COLOR newVal)						{m_options.fontColor = newVal;			return S_OK;};
	STDMETHOD(get_FontColor2)(OLE_COLOR* retval)					{*retval = m_options.fontColor2;		return S_OK;};
	STDMETHOD(put_FontColor2)(OLE_COLOR newVal)						{m_options.fontColor2 = newVal;			return S_OK;};
	STDMETHOD(get_FontGradientMode)(tkLinearGradientMode* retval)	{*retval = m_options.fontGradientMode;	return S_OK;};
	STDMETHOD(put_FontGradientMode)(tkLinearGradientMode newVal)	{m_options.fontGradientMode = newVal;	return S_OK;};

	STDMETHOD(get_FontTransparency)(LONG* retval);
	STDMETHOD(put_FontTransparency)(LONG newVal);
	
	// outlines
	STDMETHOD(get_FontOutlineVisible)(VARIANT_BOOL* retval)			{*retval = m_options.fontOutlineVisible;			return S_OK;};
	STDMETHOD(put_FontOutlineVisible)(VARIANT_BOOL newVal)			{m_options.fontOutlineVisible = newVal?true:false;	return S_OK;};		
	STDMETHOD(get_ShadowVisible)(VARIANT_BOOL* retval)				{*retval = m_options.shadowVisible;					return S_OK;};
	STDMETHOD(put_ShadowVisible)(VARIANT_BOOL newVal)				{m_options.shadowVisible = newVal?true:false;		return S_OK;};		
	STDMETHOD(get_HaloVisible)(VARIANT_BOOL* retval)				{*retval = m_options.haloVisible;					return S_OK;};
	STDMETHOD(put_HaloVisible)(VARIANT_BOOL newVal)					{m_options.haloVisible = newVal?true:false;			return S_OK;};		
	
	STDMETHOD(get_FontOutlineColor)(OLE_COLOR* retval)				{*retval = m_options.fontOutlineColor;	return S_OK;};
	STDMETHOD(put_FontOutlineColor)(OLE_COLOR newVal)				{m_options.fontOutlineColor = newVal;	return S_OK;};
	STDMETHOD(get_ShadowColor)(OLE_COLOR* retval)					{*retval = m_options.shadowColor;		return S_OK;};
	STDMETHOD(put_ShadowColor)(OLE_COLOR newVal)					{m_options.shadowColor = newVal;		return S_OK;};
	STDMETHOD(get_HaloColor)(OLE_COLOR* retval)						{*retval = m_options.haloColor;			return S_OK;};
	STDMETHOD(put_HaloColor)(OLE_COLOR newVal)						{m_options.haloColor = newVal;			return S_OK;};

	STDMETHOD(get_FontOutlineWidth)(LONG* retval)					{*retval = m_options.fontOutlineWidth;	return S_OK;};
	STDMETHOD(put_FontOutlineWidth)(LONG newVal)					{m_options.fontOutlineWidth = newVal;	return S_OK;};
	STDMETHOD(get_ShadowOffsetX)(LONG* retval)						{*retval = m_options.shadowOffsetX;		return S_OK;};
	STDMETHOD(put_ShadowOffsetX)(LONG newVal)						{m_options.shadowOffsetX = newVal;		return S_OK;};
	STDMETHOD(get_ShadowOffsetY)(LONG* retval)						{*retval = m_options.shadowOffsetY;		return S_OK;};
	STDMETHOD(put_ShadowOffsetY)(LONG newVal)						{m_options.shadowOffsetY = newVal;		return S_OK;};
	
	// haloSize - in 1/16 of font height
	STDMETHOD(get_HaloSize)(LONG* retval)							{*retval = m_options.haloSize;			return S_OK;};
	STDMETHOD(put_HaloSize)(LONG newVal)							{m_options.haloSize = newVal;			return S_OK;};
	
	// frame
	STDMETHOD(get_FrameType)(tkLabelFrameType* retval)				{*retval = m_options.frameType;			return S_OK;};		
	STDMETHOD(put_FrameType)(tkLabelFrameType newVal)				{m_options.frameType = newVal;			return S_OK;};
	STDMETHOD(get_FrameOutlineColor)(OLE_COLOR* retval)				{*retval = m_options.frameOutlineColor;	return S_OK;};		
	STDMETHOD(put_FrameOutlineColor)(OLE_COLOR newVal)				{m_options.frameOutlineColor = newVal;	return S_OK;};
	STDMETHOD(get_FrameBackColor)(OLE_COLOR* retval)				{*retval = m_options.frameBackColor;	return S_OK;};		
	STDMETHOD(put_FrameBackColor)(OLE_COLOR newVal)					{m_options.frameBackColor = newVal;		return S_OK;};
	STDMETHOD(get_FrameBackColor2)(OLE_COLOR* retval)				{*retval = m_options.frameBackColor2;	return S_OK;};		
	STDMETHOD(put_FrameBackColor2)(OLE_COLOR newVal)				{m_options.frameBackColor2 = newVal;	return S_OK;};
	STDMETHOD(get_FrameGradientMode)(tkLinearGradientMode* retval)	{*retval = m_options.frameGradientMode;	return S_OK;};
	STDMETHOD(put_FrameGradientMode)(tkLinearGradientMode newVal)	{m_options.frameGradientMode = newVal;	return S_OK;};

	STDMETHOD(get_FrameOutlineStyle)(tkDashStyle* retval)			{*retval = m_options.frameOutlineStyle;	return S_OK;};		
	STDMETHOD(put_FrameOutlineStyle)(tkDashStyle newVal)			{m_options.frameOutlineStyle = newVal;	return S_OK;};
	STDMETHOD(get_FrameOutlineWidth)(LONG* retval)					{*retval = m_options.frameOutlineWidth;	return S_OK;};		
	STDMETHOD(put_FrameOutlineWidth)(LONG newVal)					{if (newVal >= 1) m_options.frameOutlineWidth = newVal;	return S_OK;};
	
	STDMETHOD(get_FramePaddingX)(LONG* retval)						{*retval = m_options.framePaddingX;		return S_OK;};		
	STDMETHOD(put_FramePaddingX)(LONG newVal)						{m_options.framePaddingX = newVal;		return S_OK;};
	STDMETHOD(get_FramePaddingY)(LONG* retval)						{*retval = m_options.framePaddingY;		return S_OK;};		
	STDMETHOD(put_FramePaddingY)(LONG newVal)						{m_options.framePaddingY = newVal;		return S_OK;};
	
	STDMETHOD(get_FrameTransparency)(long* retval);	
	STDMETHOD(put_FrameTransparency)(long newVal);

	STDMETHOD(get_InboxAlignment)(tkLabelAlignment* retval)			{*retval = m_options.inboxAlignment;	return S_OK;};		
	STDMETHOD(put_InboxAlignment)(tkLabelAlignment newVal)			{m_options.inboxAlignment = newVal;		return S_OK;};

	STDMETHOD(get_FrameVisible)(VARIANT_BOOL* retval)				{*retval = m_options.frameVisible;	return S_OK;};		
	STDMETHOD(put_FrameVisible)(VARIANT_BOOL newVal)				{m_options.frameVisible = newVal;	return S_OK;};

	// ------------------------------------------------------------------
	//	end of properties common for CLabels and CLabelCategory classes
	// ------------------------------------------------------------------
	
	STDMETHOD(get_Enabled)(VARIANT_BOOL* retval)			{*retval = m_enabled;	return S_OK;};		
	STDMETHOD(put_Enabled)(VARIANT_BOOL newVal)				{m_enabled = newVal;	return S_OK;};

	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);

	CLabelOptions* get_LabelOptions();	
	void put_LabelOptions(CLabelOptions* newVal);
	CComVariant m_value;

	void CLabelCategory::DeserializeFromNode(CPLXMLNode* node);
	CPLXMLNode* CLabelCategory::SerializeCore(CString ElementName);

private:
	BSTR m_name;
	BSTR m_expression;
	VARIANT m_minValue;
	VARIANT m_maxValue;
	VARIANT_BOOL m_enabled;		// doesn't influence the drawing; is needed for symbology plug-in
	long m_priority;
	CLabelOptions m_options;
	
};

OBJECT_ENTRY_AUTO(__uuidof(LabelCategory), CLabelCategory)

