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
#include "LabelOptions.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CLabelCategory
class ATL_NO_VTABLE CLabelCategory :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CLabelCategory, &CLSID_LabelCategory>,
	public IDispatchImpl<ILabelCategory, &IID_ILabelCategory, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CLabelCategory()
	{
		_pUnkMarshaler = NULL;
		_name = SysAllocString(L"");
		_expression = SysAllocString(L"");
		VariantInit(&_minValue);
		VariantInit(&_maxValue);
		_priority = 0;
		_enabled = VARIANT_TRUE;
		m_value.vt = VT_EMPTY;
		gReferenceCounter.AddRef(tkInterface::idLabelCategory);
	}
	
	~CLabelCategory()
	{
		::SysFreeString(_name);
		::SysFreeString(_expression);
		VariantClear(&_minValue);
		VariantClear(&_maxValue);
		gReferenceCounter.Release(tkInterface::idLabelCategory);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_LABELCATEGORY)

	BEGIN_COM_MAP(CLabelCategory)
		COM_INTERFACE_ENTRY(ILabelCategory)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
		return S_OK;
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;

public:
	STDMETHOD(get_Name)(BSTR* retval);
	STDMETHOD(put_Name)(BSTR newVal);

	STDMETHOD(get_Expression)(BSTR* retval);
	STDMETHOD(put_Expression)(BSTR newVal);

	STDMETHOD(get_MinValue)(VARIANT* pVal);
	STDMETHOD(put_MinValue)(VARIANT newVal);

	STDMETHOD(get_MaxValue)(VARIANT* pVal);
	STDMETHOD(put_MaxValue)(VARIANT newVal);

	STDMETHOD(get_Priority)(LONG* retval)							{*retval = _priority;			return S_OK;};
	STDMETHOD(put_Priority)(LONG newVal)							{_priority = newVal;			return S_OK;};
	
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
	STDMETHOD(get_Visible)(VARIANT_BOOL* retval)					{*retval = _options.visible;			return S_OK;};
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal)						{_options.visible = newVal?true:false;			return S_OK;};		
	
	// position
	STDMETHOD(get_OffsetX)(double* retval)							{*retval = _options.offsetX;			return S_OK;};
	STDMETHOD(put_OffsetX)(double newVal)							{_options.offsetX = newVal;			    return S_OK;};
	STDMETHOD(get_OffsetY)(double* retval)							{*retval = _options.offsetY;			return S_OK;};
	STDMETHOD(put_OffsetY)(double newVal)							{_options.offsetY = newVal;			    return S_OK;};
    STDMETHOD(get_OffsetXField)(long* retval)                       { *retval = _options.offsetXField;      return S_OK; };
    STDMETHOD(put_OffsetXField)(long newVal)                        { _options.offsetXField = newVal;	    return S_OK; };
    STDMETHOD(get_OffsetYField)(long* retval)                       { *retval = _options.offsetYField;	    return S_OK; };
    STDMETHOD(put_OffsetYField)(long newVal)                        { _options.offsetYField = newVal;	    return S_OK; };
	STDMETHOD(get_Alignment)(tkLabelAlignment* retval)				{*retval = _options.alignment;			return S_OK;};
	STDMETHOD(put_Alignment)(tkLabelAlignment newVal)				{_options.alignment = newVal;			return S_OK;};
	STDMETHOD(get_LineOrientation)(tkLineLabelOrientation* retval)	{*retval = _options.lineOrientation;	return S_OK;};		
	STDMETHOD(put_LineOrientation)(tkLineLabelOrientation newVal)	{_options.lineOrientation = newVal;	    return S_OK;};
	
	// font
	STDMETHOD(get_FontName)(BSTR* retval);
	STDMETHOD(put_FontName)(BSTR newVal);
	STDMETHOD(get_FontSize)(LONG* retval)							{*retval = _options.fontSize;			return S_OK;};
	STDMETHOD(put_FontSize)(LONG newVal)							{_options.fontSize = newVal;			return S_OK;};
	
	STDMETHOD(get_FontItalic)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontItalic)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontBold)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontBold)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontUnderline)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontUnderline)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontStrikeOut)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontStrikeOut)(VARIANT_BOOL newVal);
	
	STDMETHOD(get_FontColor)(OLE_COLOR* retval)						{*retval = _options.fontColor;			return S_OK;};
	STDMETHOD(put_FontColor)(OLE_COLOR newVal)						{_options.fontColor = newVal;			return S_OK;};
	STDMETHOD(get_FontColor2)(OLE_COLOR* retval)					{*retval = _options.fontColor2;		return S_OK;};
	STDMETHOD(put_FontColor2)(OLE_COLOR newVal)						{_options.fontColor2 = newVal;			return S_OK;};
	STDMETHOD(get_FontGradientMode)(tkLinearGradientMode* retval)	{*retval = _options.fontGradientMode;	return S_OK;};
	STDMETHOD(put_FontGradientMode)(tkLinearGradientMode newVal)	{_options.fontGradientMode = newVal;	return S_OK;};

	STDMETHOD(get_FontTransparency)(LONG* retval);
	STDMETHOD(put_FontTransparency)(LONG newVal);
	
	// outlines
	STDMETHOD(get_FontOutlineVisible)(VARIANT_BOOL* retval)			{*retval = _options.fontOutlineVisible;			return S_OK;};
	STDMETHOD(put_FontOutlineVisible)(VARIANT_BOOL newVal)			{_options.fontOutlineVisible = newVal?true:false;	return S_OK;};		
	STDMETHOD(get_ShadowVisible)(VARIANT_BOOL* retval)				{*retval = _options.shadowVisible;					return S_OK;};
	STDMETHOD(put_ShadowVisible)(VARIANT_BOOL newVal)				{_options.shadowVisible = newVal?true:false;		return S_OK;};		
	STDMETHOD(get_HaloVisible)(VARIANT_BOOL* retval)				{*retval = _options.haloVisible;					return S_OK;};
	STDMETHOD(put_HaloVisible)(VARIANT_BOOL newVal)					{_options.haloVisible = newVal?true:false;			return S_OK;};		
	
	STDMETHOD(get_FontOutlineColor)(OLE_COLOR* retval)				{*retval = _options.fontOutlineColor;	return S_OK;};
	STDMETHOD(put_FontOutlineColor)(OLE_COLOR newVal)				{_options.fontOutlineColor = newVal;	return S_OK;};
	STDMETHOD(get_ShadowColor)(OLE_COLOR* retval)					{*retval = _options.shadowColor;		return S_OK;};
	STDMETHOD(put_ShadowColor)(OLE_COLOR newVal)					{_options.shadowColor = newVal;		return S_OK;};
	STDMETHOD(get_HaloColor)(OLE_COLOR* retval)						{*retval = _options.haloColor;			return S_OK;};
	STDMETHOD(put_HaloColor)(OLE_COLOR newVal)						{_options.haloColor = newVal;			return S_OK;};

	STDMETHOD(get_FontOutlineWidth)(LONG* retval)					{*retval = _options.fontOutlineWidth;	return S_OK;};
	STDMETHOD(put_FontOutlineWidth)(LONG newVal)					{_options.fontOutlineWidth = newVal;	return S_OK;};
	STDMETHOD(get_ShadowOffsetX)(LONG* retval)						{*retval = _options.shadowOffsetX;		return S_OK;};
	STDMETHOD(put_ShadowOffsetX)(LONG newVal)						{_options.shadowOffsetX = newVal;		return S_OK;};
	STDMETHOD(get_ShadowOffsetY)(LONG* retval)						{*retval = _options.shadowOffsetY;		return S_OK;};
	STDMETHOD(put_ShadowOffsetY)(LONG newVal)						{_options.shadowOffsetY = newVal;		return S_OK;};
	
	// haloSize - in 1/16 of font height
	STDMETHOD(get_HaloSize)(LONG* retval)							{*retval = _options.haloSize;			return S_OK;};
	STDMETHOD(put_HaloSize)(LONG newVal)							{_options.haloSize = newVal;			return S_OK;};
	
	// frame
	STDMETHOD(get_FrameType)(tkLabelFrameType* retval)				{*retval = _options.frameType;			return S_OK;};		
	STDMETHOD(put_FrameType)(tkLabelFrameType newVal)				{_options.frameType = newVal;			return S_OK;};
	STDMETHOD(get_FrameOutlineColor)(OLE_COLOR* retval)				{*retval = _options.frameOutlineColor;	return S_OK;};		
	STDMETHOD(put_FrameOutlineColor)(OLE_COLOR newVal)				{_options.frameOutlineColor = newVal;	return S_OK;};
	STDMETHOD(get_FrameBackColor)(OLE_COLOR* retval)				{*retval = _options.frameBackColor;	return S_OK;};		
	STDMETHOD(put_FrameBackColor)(OLE_COLOR newVal)					{_options.frameBackColor = newVal;		return S_OK;};
	STDMETHOD(get_FrameBackColor2)(OLE_COLOR* retval)				{*retval = _options.frameBackColor2;	return S_OK;};		
	STDMETHOD(put_FrameBackColor2)(OLE_COLOR newVal)				{_options.frameBackColor2 = newVal;	return S_OK;};
	STDMETHOD(get_FrameGradientMode)(tkLinearGradientMode* retval)	{*retval = _options.frameGradientMode;	return S_OK;};
	STDMETHOD(put_FrameGradientMode)(tkLinearGradientMode newVal)	{_options.frameGradientMode = newVal;	return S_OK;};

	STDMETHOD(get_FrameOutlineStyle)(tkDashStyle* retval)			{*retval = _options.frameOutlineStyle;	return S_OK;};		
	STDMETHOD(put_FrameOutlineStyle)(tkDashStyle newVal)			{_options.frameOutlineStyle = newVal;	return S_OK;};
	STDMETHOD(get_FrameOutlineWidth)(LONG* retval)					{*retval = _options.frameOutlineWidth;	return S_OK;};		
	STDMETHOD(put_FrameOutlineWidth)(LONG newVal)					{if (newVal >= 1) _options.frameOutlineWidth = newVal;	return S_OK;};
	
	STDMETHOD(get_FramePaddingX)(LONG* retval)						{*retval = _options.framePaddingX;		return S_OK;};		
	STDMETHOD(put_FramePaddingX)(LONG newVal)						{_options.framePaddingX = newVal;		return S_OK;};
	STDMETHOD(get_FramePaddingY)(LONG* retval)						{*retval = _options.framePaddingY;		return S_OK;};		
	STDMETHOD(put_FramePaddingY)(LONG newVal)						{_options.framePaddingY = newVal;		return S_OK;};
	
	STDMETHOD(get_FrameTransparency)(long* retval);	
	STDMETHOD(put_FrameTransparency)(long newVal);

	STDMETHOD(get_InboxAlignment)(tkLabelAlignment* retval)			{*retval = _options.inboxAlignment;	return S_OK;};		
	STDMETHOD(put_InboxAlignment)(tkLabelAlignment newVal)			{_options.inboxAlignment = newVal;		return S_OK;};

	STDMETHOD(get_FrameVisible)(VARIANT_BOOL* retval)				{*retval = _options.frameVisible;	return S_OK;};		
	STDMETHOD(put_FrameVisible)(VARIANT_BOOL newVal)				{_options.frameVisible = newVal;	return S_OK;};

	// ------------------------------------------------------------------
	//	end of properties common for CLabels and CLabelCategory classes
	// ------------------------------------------------------------------
	
	STDMETHOD(get_Enabled)(VARIANT_BOOL* retval)			{*retval = _enabled;	return S_OK;};		
	STDMETHOD(put_Enabled)(VARIANT_BOOL newVal)				{_enabled = newVal;	return S_OK;};

	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);

	STDMETHOD(get_FontSize2)(LONG* pVal);
	STDMETHOD(put_FontSize2)(LONG newVal);
private:
	BSTR _name;
	BSTR _expression;
	VARIANT _minValue;
	VARIANT _maxValue;
	VARIANT_BOOL _enabled;		// doesn't influence the drawing; is needed for symbology plug-in
	long _priority;
	CLabelOptions _options;

public:
	CLabelOptions* get_LabelOptions();
	void put_LabelOptions(CLabelOptions* newVal);
	CComVariant m_value;
	void DeserializeFromNode(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);
	
};

OBJECT_ENTRY_AUTO(__uuidof(LabelCategory), CLabelCategory)

