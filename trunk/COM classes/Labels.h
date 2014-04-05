/**************************************************************************************
 * File name: Labels.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CLabels
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
#include <vector>
#include "LabelOptions.h"
#include "cpl_minixml.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CLabels
class ATL_NO_VTABLE CLabels :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLabels, &CLSID_Labels>,
	public IDispatchImpl<ILabels, &IID_ILabels, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CLabels()
	{
		m_shapefile = NULL;
		m_synchronized = VARIANT_FALSE;
		USES_CONVERSION;
		m_key = A2BSTR("");
		m_expression = A2BSTR("");
		m_globalCallback = NULL;
		m_lastErrorCode = tkNO_ERROR;
		m_scale = false;
		m_verticalPosition = vpAboveAllLayers;
		m_basicScale = 0.0;
		m_maxVisibleScale = 100000000.0;
		m_minVisibleScale = 0.0;
		m_collisionBuffer = 0;

		m_dynamicVisibility = VARIANT_FALSE;
		m_avoidCollisions = m_globalSettings.labelsCollisionMode != tkCollisionMode::AllowCollisions;
		m_useWidthLimits = VARIANT_FALSE;
		m_removeDuplicates = VARIANT_FALSE;
		m_gdiPlus = VARIANT_TRUE;

		m_classificationField = -1;
		m_sourceField = -1;

		m_minDrawingSize = 1;	// turn off
		m_autoOffset = VARIANT_TRUE;

		m_savingMode = modeXML;
		m_positioning = lpNone;

		m_textRenderingHint = HintAntiAlias;
	}
	~CLabels()
	{
		this->Clear();
		this->ClearCategories();
		::SysFreeString(m_key);
		::SysFreeString(m_expression);
		
		m_shapefile = NULL;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_LABELS)

	BEGIN_COM_MAP(CLabels)
		COM_INTERFACE_ENTRY(ILabels)
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
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	
	
	STDMETHOD(AddLabel)(BSTR Text, double x, double y, double Rotation, long Category = -1);
	STDMETHOD(InsertLabel)(long Index, BSTR Text, double x, double y, double Rotation, long Category, VARIANT_BOOL* retVal);
	STDMETHOD(RemoveLabel)(long Index, VARIANT_BOOL* vbretval);
	
	STDMETHOD(AddPart)(long Index, BSTR Text, double x, double y, double Rotation, long Category = -1);
	STDMETHOD(InsertPart)(long Index, long Part, BSTR Text, double x, double y, double Rotation, long Category, VARIANT_BOOL* retVal);
	STDMETHOD(RemovePart)(long Index, long Part, VARIANT_BOOL* vbretval);

	STDMETHOD(AddCategory)(BSTR Name, ILabelCategory** retVal);
	STDMETHOD(InsertCategory)(long Index, BSTR Name, ILabelCategory** retVal);
	STDMETHOD(RemoveCategory)(long Index, VARIANT_BOOL* vbretval);

	STDMETHOD(Clear)();
	STDMETHOD(ClearCategories)();

	// managing labels
	STDMETHOD(get_Count)(/*[out, retval]*/long* pVal)				{*pVal = m_labels.size();	return S_OK;};
	STDMETHOD(get_NumParts)(/*[in]*/long Index, /*[out, retval]*/long* pVal);
	STDMETHOD(get_NumCategories)(/*[out, retval]*/long* pVal);
	
	STDMETHOD(get_Label)(long Index, long Part, ILabel** pVal);
	//STDMETHOD(put_Label)(long Index, long Part, ILabel* newVal);
	
	STDMETHOD(get_Category)(long Index, ILabelCategory** retval);
	STDMETHOD(put_Category)(long Index, ILabelCategory* newVal);

	// selection
	STDMETHOD(Select)(IExtents* BoundingBox, long Tolerance, SelectMode SelectMode, VARIANT* LabelIndices, VARIANT* PartIndices, VARIANT_BOOL* retval);

	// ------------------------------------------------------
	// Class-specific properties
	// ------------------------------------------------------
	STDMETHOD(get_UseGdiPlus)(VARIANT_BOOL* retval)						{*retval = m_gdiPlus;				return S_OK;};		
	STDMETHOD(put_UseGdiPlus)(VARIANT_BOOL newVal)						{m_gdiPlus = newVal;				return S_OK;};
	
	STDMETHOD(get_Synchronized)(VARIANT_BOOL* retval);
	STDMETHOD(put_Synchronized)(VARIANT_BOOL newVal);
	
	STDMETHOD(get_ScaleLabels)(VARIANT_BOOL* retval)					{*retval = m_scale;					return S_OK;};
	STDMETHOD(put_ScaleLabels)(VARIANT_BOOL newVal)						{m_scale = newVal?true:false;		return S_OK;};

	STDMETHOD(get_VerticalPosition)(tkVerticalPosition* retval)	{*retval = m_verticalPosition;		return S_OK;};	
	STDMETHOD(put_VerticalPosition)(tkVerticalPosition newVal)		{m_verticalPosition = newVal;		return S_OK;};

	STDMETHOD(get_BasicScale)(double* retval)							{*retval = m_basicScale;			return S_OK;};	
	STDMETHOD(put_BasicScale)(double newVal)							{m_basicScale = newVal;				return S_OK;};

	STDMETHOD(get_MaxVisibleScale)(double* retval)						{*retval = m_maxVisibleScale;		return S_OK;};		
	STDMETHOD(put_MaxVisibleScale)(double newVal)						{m_maxVisibleScale = newVal;		return S_OK;};
	
	STDMETHOD(get_MinVisibleScale)(double* retval)						{*retval = m_minVisibleScale;		return S_OK;};		
	STDMETHOD(put_MinVisibleScale)(double newVal)						{m_minVisibleScale = newVal;		return S_OK;};

	STDMETHOD(get_DynamicVisibility)(VARIANT_BOOL* retval)				{*retval = m_dynamicVisibility;		return S_OK;};		
	STDMETHOD(put_DynamicVisibility)(VARIANT_BOOL newVal)				{m_dynamicVisibility = newVal;		return S_OK;};

	STDMETHOD(get_AvoidCollisions)(VARIANT_BOOL* retval)				{*retval = m_avoidCollisions;		return S_OK;};		
	STDMETHOD(put_AvoidCollisions)(VARIANT_BOOL newVal)					{m_avoidCollisions = newVal;		return S_OK;};

	STDMETHOD(get_CollisionBuffer)(long* retval)						{*retval = m_collisionBuffer;		return S_OK;};		
	STDMETHOD(put_CollisionBuffer)(long newVal)							{m_collisionBuffer = newVal;		return S_OK;};
	
	// not implemented
	STDMETHOD(get_UseWidthLimits)(VARIANT_BOOL* retval)					{*retval = m_useWidthLimits;		return S_OK;};		
	STDMETHOD(put_UseWidthLimits)(VARIANT_BOOL newVal)					{m_useWidthLimits = newVal;			return S_OK;};
	
	// not implemented
	STDMETHOD(get_RemoveDuplicates)(VARIANT_BOOL* retval)				{*retval = m_removeDuplicates;		return S_OK;};		
	STDMETHOD(put_RemoveDuplicates)(VARIANT_BOOL newVal)				{m_removeDuplicates = newVal;		return S_OK;};
	
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
	STDMETHOD(get_OffsetX)(DOUBLE* retval)							{*retval = m_options.offsetX;			return S_OK;};
	STDMETHOD(put_OffsetX)(DOUBLE newVal)							{m_options.offsetX = newVal;			return S_OK;};
	STDMETHOD(get_OffsetY)(DOUBLE* retval)							{*retval = m_options.offsetY;			return S_OK;};
	STDMETHOD(put_OffsetY)(DOUBLE newVal)							{m_options.offsetY = newVal;			return S_OK;};
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
	STDMETHOD(get_FontOutlineVisible)(VARIANT_BOOL* retval)			{*retval = m_options.fontOutlineVisible;				return S_OK;};
	STDMETHOD(put_FontOutlineVisible)(VARIANT_BOOL newVal)			{m_options.fontOutlineVisible = newVal?true:false;		return S_OK;};		
	STDMETHOD(get_ShadowVisible)(VARIANT_BOOL* retval)				{*retval = m_options.shadowVisible;						return S_OK;};
	STDMETHOD(put_ShadowVisible)(VARIANT_BOOL newVal)				{m_options.shadowVisible = newVal?true:false;			return S_OK;};		
	STDMETHOD(get_HaloVisible)(VARIANT_BOOL* retval)				{*retval = m_options.haloVisible;						return S_OK;};
	STDMETHOD(put_HaloVisible)(VARIANT_BOOL newVal)					{m_options.haloVisible = newVal?true:false;				return S_OK;};		
	
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
	STDMETHOD(put_FrameOutlineWidth)(LONG newVal)					{if (newVal >= 1)m_options.frameOutlineWidth = newVal;	return S_OK;};
	
	STDMETHOD(get_FramePaddingX)(LONG* retval)						{*retval = m_options.framePaddingX;		return S_OK;};		
	STDMETHOD(put_FramePaddingX)(LONG newVal)						{m_options.framePaddingX = newVal;		return S_OK;};
	STDMETHOD(get_FramePaddingY)(LONG* retval)						{*retval = m_options.framePaddingY;		return S_OK;};		
	STDMETHOD(put_FramePaddingY)(LONG newVal)						{m_options.framePaddingY = newVal;		return S_OK;};
	
	STDMETHOD(get_FrameTransparency)(long* retval);	
	STDMETHOD(put_FrameTransparency)(long newVal);

	STDMETHOD(get_InboxAlignment)(tkLabelAlignment* retval)			{*retval = m_options.inboxAlignment;	return S_OK;};		
	STDMETHOD(put_InboxAlignment)(tkLabelAlignment newVal)			{m_options.inboxAlignment = newVal;		return S_OK;};
	// ------------------------------------------------------------------
	//	end of properties common for CLabels and CLabelCategory classes
	// ------------------------------------------------------------------
	STDMETHOD(GenerateCategories)(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT_BOOL* retVal);
	
	STDMETHOD(ApplyCategories)();
	
	STDMETHOD(get_ClassificationField)(long* FieldIndex);
	STDMETHOD(put_ClassificationField)(long FieldIndex);

	STDMETHOD(get_Options)(ILabelCategory** retVal);
	STDMETHOD(put_Options)(ILabelCategory* newVal);

	STDMETHOD(ApplyColorScheme) (tkColorSchemeType Type, IColorScheme* ColorScheme);
	STDMETHOD(ApplyColorScheme2) (tkColorSchemeType Type, IColorScheme* ColorScheme, tkLabelElements Element);
	STDMETHOD(ApplyColorScheme3) (tkColorSchemeType Type, IColorScheme* ColorScheme, 
											 tkLabelElements Element, long CategoryStartIndex, long CategoryEndIndex);

	STDMETHOD(get_FrameVisible)(VARIANT_BOOL* retVal)				{*retVal = m_options.frameVisible;	return S_OK;};
	STDMETHOD(put_FrameVisible)(VARIANT_BOOL newVal)				{m_options.frameVisible = newVal;	return S_OK;};
	
	STDMETHOD(get_VisibilityExpression)(BSTR* retval);
	STDMETHOD(put_VisibilityExpression)(BSTR newVal);

	STDMETHOD(get_MinDrawingSize)(LONG* retval);
	STDMETHOD(put_MinDrawingSize)(LONG newVal);
	
	STDMETHOD(MoveCategoryUp)(long Index, VARIANT_BOOL* retval);
	STDMETHOD(MoveCategoryDown)(long Index, VARIANT_BOOL* retval);

	STDMETHOD(get_AutoOffset)(VARIANT_BOOL* retval);
	STDMETHOD(put_AutoOffset)(VARIANT_BOOL newVal);

	STDMETHOD(get_Expression)(BSTR* retVal);
	STDMETHOD(put_Expression)(BSTR newVal);
	
	// serialization
	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);

	STDMETHOD(SaveToDbf)(VARIANT_BOOL saveText, VARIANT_BOOL saveCategory, VARIANT_BOOL* retVal);
	STDMETHOD(SaveToDbf2)(BSTR xField, BSTR yField, BSTR angleField, BSTR textField, BSTR categoryField, 
						 VARIANT_BOOL saveText, VARIANT_BOOL saveCategory, VARIANT_BOOL* retVal);

	STDMETHOD(LoadFromDbf)(VARIANT_BOOL loadText, VARIANT_BOOL loadCategory, VARIANT_BOOL* retVal);
	STDMETHOD(LoadFromDbf2)(BSTR xField, BSTR yField, BSTR angleField, BSTR textField, BSTR categoryField, 
						   VARIANT_BOOL loadText, VARIANT_BOOL loadCategory, VARIANT_BOOL* retVal);

	STDMETHOD(SaveToXML)(BSTR filename, VARIANT_BOOL* retVal);
	STDMETHOD(LoadFromXML)(BSTR filename, VARIANT_BOOL* retVal);
	STDMETHOD(Generate)(BSTR Expression, tkLabelPositioning Method, VARIANT_BOOL LargestPartOnly, long* Count);

	STDMETHOD(get_SavingMode)(tkSavingMode* retVal);
	STDMETHOD(put_SavingMode)(tkSavingMode newVal);

	STDMETHOD(get_Positioning)(tkLabelPositioning* pVal);
	STDMETHOD(put_Positioning)(tkLabelPositioning newVal);

	STDMETHOD(get_TextRenderingHint)(tkTextRenderingHint* pVal);
	STDMETHOD(put_TextRenderingHint)(tkTextRenderingHint newVal);

	// -----------------------------------------------------------------
	//   Public methods and properties not included in COM interface
	//   are meant for inner use
	// -----------------------------------------------------------------
	bool CLabels::DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* CLabels::SerializeCore(CString ElementName);
	
	std::vector<std::vector<CLabelInfo*>*>* get_LabelData();	
	CLabelOptions* get_LabelOptions();
	
	void put_ParentShapefile(IShapefile* newVal);
	IShapefile* get_ParentShapefile();
	void RefreshExpressions();
	bool LabelsSynchronized();

	void CLabels::ApplyExpression_(long CategoryIndex);
	VARIANT_BOOL m_synchronized;

	CPLXMLNode* CLabels::SerializeLabelData(CString ElementName, bool& saveRotation, bool& saveText);
	bool CLabels::DeserializeLabelData(CPLXMLNode* node, bool loadRotation, bool loadText);

	bool CLabels::GenerateEmptyLabels();

	void CLabels::LoadLblOptions(CPLXMLNode* node);

private:
	int m_sourceField;

	tkLabelPositioning m_positioning;
	
	tkSavingMode m_savingMode;
	
	// expression to generate label text
	CString m_labelExpression;

	// the minimal size of shape to label
	long m_minDrawingSize;		
	
	// visibility expression
	BSTR m_expression;

	long m_classificationField;

	IShapefile* m_shapefile;

	// inner vector for a given index should exist even if there are no actual labels (parts) in it
	std::vector<std::vector<CLabelInfo*>*> m_labels;
	std::vector<ILabelCategory*> m_categories;

	CLabelOptions m_options;
	
	BSTR m_key;
	long m_lastErrorCode;
	ICallback * m_globalCallback;
	
	tkVerticalPosition m_verticalPosition;

	double		m_basicScale;
	double		m_maxVisibleScale;
	double		m_minVisibleScale;
	long		m_collisionBuffer;
	bool		m_scale;
	
	VARIANT_BOOL m_autoOffset;
	VARIANT_BOOL m_dynamicVisibility;
	VARIANT_BOOL m_avoidCollisions;
	VARIANT_BOOL m_useWidthLimits;
	VARIANT_BOOL m_removeDuplicates;
	VARIANT_BOOL m_gdiPlus;
	
	inline void CLabels::ErrorMessage(long ErrorCode);
	bool CLabels::GetMinMaxCategoryValue(double& globalMax, double& globalMin);

	tkTextRenderingHint m_textRenderingHint;

public:
	void CLabels::ClearLabelFrames();
	void SaveSourceField(int fieldIndex)
	{
		m_sourceField = fieldIndex;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Labels), CLabels)
