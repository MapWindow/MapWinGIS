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
#include "LabelOptions.h"
#include "LabelCategory.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CLabels
class ATL_NO_VTABLE CLabels :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CLabels, &CLSID_Labels>,
	public IDispatchImpl<ILabels, &IID_ILabels, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CLabels()
	{
		_pUnkMarshaler = NULL;
		
		_useVariableSize = VARIANT_TRUE;
		_logScaleForSize = VARIANT_FALSE;
		_fontSizeChanged = true;
		_floatNumberFormat = m_globalSettings.floatNumberFormat;
		_shapefile = NULL;
		_synchronized = VARIANT_FALSE;
		USES_CONVERSION;
		_key = SysAllocString(L"");
		_expression = SysAllocString(L"");
		
		_globalCallback = NULL;
		_lastErrorCode = tkNO_ERROR;
		_scale = false;
		_verticalPosition = vpAboveAllLayers;
		_basicScale = 0.0;
		_maxVisibleScale = 100000000.0;
		_minVisibleScale = 0.0;
		_minVisibleZoom = 0;
		_maxVisibleZoom = 25;
		_collisionBuffer = 0;

		_dynamicVisibility = VARIANT_FALSE;
		_avoidCollisions = m_globalSettings.labelsCollisionMode != tkCollisionMode::AllowCollisions;
		_useWidthLimits = VARIANT_FALSE;
		_removeDuplicates = VARIANT_FALSE;
		_gdiPlus = VARIANT_TRUE;

		_classificationField = -1;
		_sourceField = -1;

		_minDrawingSize = 1;	// turn off
		_autoOffset = VARIANT_TRUE;

		_savingMode = modeXML;
		_positioning = lpNone;

		_textRenderingHint = AntiAliasGridFit;

		ComHelper::CreateInstance(idLabelCategory, (IDispatch**)&_category);
		_options = ((CLabelCategory*)_category)->get_LabelOptions();

		gReferenceCounter.AddRef(tkInterface::idLabels);
	}
	~CLabels()
	{
		Clear();
		ClearCategories();

		::SysFreeString(_key);
		::SysFreeString(_expression);

		if (_category) {
			_category->Release();
		}

		_shapefile = NULL;

		gReferenceCounter.Release(tkInterface::idLabels);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_LABELS)

	BEGIN_COM_MAP(CLabels)
		COM_INTERFACE_ENTRY(ILabels)
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
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	
	STDMETHOD(AddLabel)(BSTR Text, double x, double y, double Rotation, long Category = -1, double offsetX = 0, double offsetY = 0);
	STDMETHOD(InsertLabel)(long Index, BSTR Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY, VARIANT_BOOL* retVal);
	STDMETHOD(RemoveLabel)(long Index, VARIANT_BOOL* vbretval);
	
	STDMETHOD(AddPart)(long Index, BSTR Text, double x, double y, double Rotation, long Category = -1, double offsetX = 0, double offsetY = 0);
	STDMETHOD(InsertPart)(long Index, long Part, BSTR Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY, VARIANT_BOOL* retVal);
	STDMETHOD(RemovePart)(long Index, long Part, VARIANT_BOOL* vbretval);

	STDMETHOD(AddCategory)(BSTR Name, ILabelCategory** retVal);
	STDMETHOD(InsertCategory)(long Index, BSTR Name, ILabelCategory** retVal);
	STDMETHOD(RemoveCategory)(long Index, VARIANT_BOOL* vbretval);

	STDMETHOD(Clear)();
	STDMETHOD(ClearCategories)();

	// managing labels
	STDMETHOD(get_Count)(/*[out, retval]*/long* pVal)				{*pVal = _labels.size();	return S_OK;};
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
	STDMETHOD(get_UseGdiPlus)(VARIANT_BOOL* retval)						{*retval = _gdiPlus;				return S_OK;};		
	STDMETHOD(put_UseGdiPlus)(VARIANT_BOOL newVal)						{_gdiPlus = newVal;				return S_OK;};
	
	STDMETHOD(get_Synchronized)(VARIANT_BOOL* retval);
	STDMETHOD(put_Synchronized)(VARIANT_BOOL newVal);
	
	STDMETHOD(get_ScaleLabels)(VARIANT_BOOL* retval)					{*retval = _scale;					return S_OK;};
	STDMETHOD(put_ScaleLabels)(VARIANT_BOOL newVal)						{_scale = newVal?true:false;		return S_OK;};

	STDMETHOD(get_VerticalPosition)(tkVerticalPosition* retval)	{*retval = _verticalPosition;		return S_OK;};	
	STDMETHOD(put_VerticalPosition)(tkVerticalPosition newVal)		{_verticalPosition = newVal;		return S_OK;};

	STDMETHOD(get_BasicScale)(double* retval)							{*retval = _basicScale;			return S_OK;};	
	STDMETHOD(put_BasicScale)(double newVal)							{_basicScale = newVal;				return S_OK;};

	STDMETHOD(get_MaxVisibleScale)(double* retval)						{*retval = _maxVisibleScale;		return S_OK;};		
	STDMETHOD(put_MaxVisibleScale)(double newVal)						{_maxVisibleScale = newVal;		return S_OK;};
	STDMETHOD(get_MinVisibleScale)(double* retval)						{*retval = _minVisibleScale;		return S_OK;};		
	STDMETHOD(put_MinVisibleScale)(double newVal)						{_minVisibleScale = newVal;		return S_OK;};

	STDMETHOD(get_MinVisibleZoom)(LONG* pVal) 							{ *pVal = _minVisibleZoom;		return S_OK; };
	STDMETHOD(put_MinVisibleZoom)(LONG newVal)							{ _minVisibleZoom = newVal;		return S_OK; };
	STDMETHOD(get_MaxVisibleZoom)(LONG* pVal)							{ *pVal = _maxVisibleZoom;		return S_OK; };
	STDMETHOD(put_MaxVisibleZoom)(LONG newVal)							{ _maxVisibleZoom = newVal;		return S_OK; };

	STDMETHOD(get_DynamicVisibility)(VARIANT_BOOL* retval)				{*retval = _dynamicVisibility;		return S_OK;};		
	STDMETHOD(put_DynamicVisibility)(VARIANT_BOOL newVal)				{_dynamicVisibility = newVal;		return S_OK;};

	STDMETHOD(get_AvoidCollisions)(VARIANT_BOOL* retval)				{*retval = _avoidCollisions;		return S_OK;};		
	STDMETHOD(put_AvoidCollisions)(VARIANT_BOOL newVal)					{_avoidCollisions = newVal;		return S_OK;};

	STDMETHOD(get_CollisionBuffer)(long* retval)						{*retval = _collisionBuffer;		return S_OK;};		
	STDMETHOD(put_CollisionBuffer)(long newVal)							{_collisionBuffer = newVal;		return S_OK;};
	
	// not implemented
	STDMETHOD(get_UseWidthLimits)(VARIANT_BOOL* retval)					{*retval = _useWidthLimits;		return S_OK;};		
	STDMETHOD(put_UseWidthLimits)(VARIANT_BOOL newVal)					{_useWidthLimits = newVal;			return S_OK;};
	
	// not implemented
	STDMETHOD(get_RemoveDuplicates)(VARIANT_BOOL* retval)				{*retval = _removeDuplicates;		return S_OK;};		
	STDMETHOD(put_RemoveDuplicates)(VARIANT_BOOL newVal)				{_removeDuplicates = newVal;		return S_OK;};
	
	// ---------------------------------------------------------------------------------
	// Properties common for CLabels and CLabelCategory
	// it's easier to port the options in separate class to avoid duplication;
	// but such disposition upon my opinion ensure more readable code in the client part
	// Also it's possible to treat CLabels basic options like Category with index 0
	// but again the code outside will be easier if we use just Labels.FontColor rather than 
	// Labels.Category[0].FontColor or Labels.Options.FontColor, therefore such disposition
	// CLabelOptions structure is used to hold the options, so declarations can be just copied
	// between CLabels and CLabelCategory, the names of properties should be the same in both classes
	// ---------------------------------------------------------------------------------
	STDMETHOD(get_Visible)(VARIANT_BOOL* retval)					{*retval = _options->visible;			return S_OK;};
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal)						{_options->visible = newVal?true:false;			return S_OK;};		
	
	// position
	STDMETHOD(get_OffsetX)(DOUBLE* retval)							{*retval = _options->offsetX;			return S_OK;};
	STDMETHOD(put_OffsetX)(DOUBLE newVal)							{_options->offsetX = newVal;			return S_OK;};
	STDMETHOD(get_OffsetY)(DOUBLE* retval)							{*retval = _options->offsetY;			return S_OK;};
	STDMETHOD(put_OffsetY)(DOUBLE newVal)							{_options->offsetY = newVal;			return S_OK;};
    STDMETHOD(get_OffsetXField)(long* retval)                       {*retval = _options->offsetXField;		return S_OK;};
    STDMETHOD(put_OffsetXField)(long newVal)                        {_options->offsetXField = newVal;		return S_OK;};
    STDMETHOD(get_OffsetYField)(long* retval)                       {*retval = _options->offsetYField;		return S_OK;};
    STDMETHOD(put_OffsetYField)(long newVal)                        {_options->offsetYField = newVal;		return S_OK;};
	STDMETHOD(get_Alignment)(tkLabelAlignment* retval)				{*retval = _options->alignment;			return S_OK;};
	STDMETHOD(put_Alignment)(tkLabelAlignment newVal)				{_options->alignment = newVal;			return S_OK;};
	STDMETHOD(get_LineOrientation)(tkLineLabelOrientation* retval)	{*retval = _options->lineOrientation;	return S_OK;};		
	STDMETHOD(put_LineOrientation)(tkLineLabelOrientation newVal)	{_options->lineOrientation = newVal;	return S_OK;};
	
	// font
	STDMETHOD(get_FontName)(BSTR* retval);
	STDMETHOD(put_FontName)(BSTR newVal);
	STDMETHOD(get_FontSize)(LONG* retval);
	STDMETHOD(put_FontSize)(LONG newVal);
	
	STDMETHOD(get_FontItalic)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontItalic)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontBold)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontBold)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontUnderline)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontUnderline)(VARIANT_BOOL newVal);
	STDMETHOD(get_FontStrikeOut)(VARIANT_BOOL* retval);
	STDMETHOD(put_FontStrikeOut)(VARIANT_BOOL newVal);
	
	STDMETHOD(get_FontColor)(OLE_COLOR* retval)						{*retval = _options->fontColor;			return S_OK;};
	STDMETHOD(put_FontColor)(OLE_COLOR newVal)						{_options->fontColor = newVal;			return S_OK;};
	STDMETHOD(get_FontColor2)(OLE_COLOR* retval)					{*retval = _options->fontColor2;		return S_OK;};
	STDMETHOD(put_FontColor2)(OLE_COLOR newVal)						{_options->fontColor2 = newVal;			return S_OK;};
	STDMETHOD(get_FontGradientMode)(tkLinearGradientMode* retval)	{*retval = _options->fontGradientMode;	return S_OK;};
	STDMETHOD(put_FontGradientMode)(tkLinearGradientMode newVal)	{_options->fontGradientMode = newVal;	return S_OK;};

	STDMETHOD(get_FontTransparency)(LONG* retval);
	STDMETHOD(put_FontTransparency)(LONG newVal);
	
	// outlines
	STDMETHOD(get_FontOutlineVisible)(VARIANT_BOOL* retval)			{*retval = _options->fontOutlineVisible;				return S_OK;};
	STDMETHOD(put_FontOutlineVisible)(VARIANT_BOOL newVal)			{_options->fontOutlineVisible = newVal?true:false;		return S_OK;};		
	STDMETHOD(get_ShadowVisible)(VARIANT_BOOL* retval)				{*retval = _options->shadowVisible;						return S_OK;};
	STDMETHOD(put_ShadowVisible)(VARIANT_BOOL newVal)				{_options->shadowVisible = newVal?true:false;			return S_OK;};		
	STDMETHOD(get_HaloVisible)(VARIANT_BOOL* retval)				{*retval = _options->haloVisible;						return S_OK;};
	STDMETHOD(put_HaloVisible)(VARIANT_BOOL newVal)					{_options->haloVisible = newVal?true:false;				return S_OK;};		
	
	STDMETHOD(get_FontOutlineColor)(OLE_COLOR* retval)				{*retval = _options->fontOutlineColor;	return S_OK;};
	STDMETHOD(put_FontOutlineColor)(OLE_COLOR newVal)				{_options->fontOutlineColor = newVal;	return S_OK;};
	STDMETHOD(get_ShadowColor)(OLE_COLOR* retval)					{*retval = _options->shadowColor;		return S_OK;};
	STDMETHOD(put_ShadowColor)(OLE_COLOR newVal)					{_options->shadowColor = newVal;		return S_OK;};
	STDMETHOD(get_HaloColor)(OLE_COLOR* retval)						{*retval = _options->haloColor;			return S_OK;};
	STDMETHOD(put_HaloColor)(OLE_COLOR newVal)						{_options->haloColor = newVal;			return S_OK;};

	STDMETHOD(get_FontOutlineWidth)(LONG* retval)					{*retval = _options->fontOutlineWidth;	return S_OK;};
	STDMETHOD(put_FontOutlineWidth)(LONG newVal)					{_options->fontOutlineWidth = newVal;	return S_OK;};
	STDMETHOD(get_ShadowOffsetX)(LONG* retval)						{*retval = _options->shadowOffsetX;		return S_OK;};
	STDMETHOD(put_ShadowOffsetX)(LONG newVal)						{_options->shadowOffsetX = newVal;		return S_OK;};
	STDMETHOD(get_ShadowOffsetY)(LONG* retval)						{*retval = _options->shadowOffsetY;		return S_OK;};
	STDMETHOD(put_ShadowOffsetY)(LONG newVal)						{_options->shadowOffsetY = newVal;		return S_OK;};
	
	// haloSize - in 1/16 of font height
	STDMETHOD(get_HaloSize)(LONG* retval)							{*retval = _options->haloSize;			return S_OK;};
	STDMETHOD(put_HaloSize)(LONG newVal)							{_options->haloSize = newVal;			return S_OK;};
	
	// frame
	STDMETHOD(get_FrameType)(tkLabelFrameType* retval)				{*retval = _options->frameType;			return S_OK;};		
	STDMETHOD(put_FrameType)(tkLabelFrameType newVal)				{_options->frameType = newVal;			return S_OK;};
	STDMETHOD(get_FrameOutlineColor)(OLE_COLOR* retval)				{*retval = _options->frameOutlineColor;	return S_OK;};		
	STDMETHOD(put_FrameOutlineColor)(OLE_COLOR newVal)				{_options->frameOutlineColor = newVal;	return S_OK;};
	STDMETHOD(get_FrameBackColor)(OLE_COLOR* retval)				{*retval = _options->frameBackColor;	return S_OK;};		
	STDMETHOD(put_FrameBackColor)(OLE_COLOR newVal)					{_options->frameBackColor = newVal;		return S_OK;};
	STDMETHOD(get_FrameBackColor2)(OLE_COLOR* retval)				{*retval = _options->frameBackColor2;	return S_OK;};		
	STDMETHOD(put_FrameBackColor2)(OLE_COLOR newVal)				{_options->frameBackColor2 = newVal;	return S_OK;};
	STDMETHOD(get_FrameGradientMode)(tkLinearGradientMode* retval)	{*retval = _options->frameGradientMode;	return S_OK;};
	STDMETHOD(put_FrameGradientMode)(tkLinearGradientMode newVal)	{_options->frameGradientMode = newVal;	return S_OK;};

	STDMETHOD(get_FrameOutlineStyle)(tkDashStyle* retval)			{*retval = _options->frameOutlineStyle;	return S_OK;};		
	STDMETHOD(put_FrameOutlineStyle)(tkDashStyle newVal)			{_options->frameOutlineStyle = newVal;	return S_OK;};
	STDMETHOD(get_FrameOutlineWidth)(LONG* retval)					{*retval = _options->frameOutlineWidth;	return S_OK;};		
	STDMETHOD(put_FrameOutlineWidth)(LONG newVal)					{if (newVal >= 1)_options->frameOutlineWidth = newVal;	return S_OK;};
	
	STDMETHOD(get_FramePaddingX)(LONG* retval)						{*retval = _options->framePaddingX;		return S_OK;};		
	STDMETHOD(put_FramePaddingX)(LONG newVal)						{_options->framePaddingX = newVal;		return S_OK;};
	STDMETHOD(get_FramePaddingY)(LONG* retval)						{*retval = _options->framePaddingY;		return S_OK;};		
	STDMETHOD(put_FramePaddingY)(LONG newVal)						{_options->framePaddingY = newVal;		return S_OK;};
	
	STDMETHOD(get_FrameTransparency)(long* retval);	
	STDMETHOD(put_FrameTransparency)(long newVal);

	STDMETHOD(get_InboxAlignment)(tkLabelAlignment* retval)			{*retval = _options->inboxAlignment;	return S_OK;};		
	STDMETHOD(put_InboxAlignment)(tkLabelAlignment newVal)			{_options->inboxAlignment = newVal;		return S_OK;};
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

	STDMETHOD(get_FrameVisible)(VARIANT_BOOL* retVal)				{*retVal = _options->frameVisible;	return S_OK;};
	STDMETHOD(put_FrameVisible)(VARIANT_BOOL newVal)				{_options->frameVisible = newVal;	return S_OK;};
	
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
						 VARIANT_BOOL saveText, VARIANT_BOOL saveCategory, VARIANT_BOOL createFields, VARIANT_BOOL* retVal);

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

	STDMETHOD(get_FloatNumberFormat)(BSTR* pVal);
	STDMETHOD(put_FloatNumberFormat)(BSTR newVal);

	STDMETHOD(ForceRecalculateExpression)();
    STDMETHOD(ApplyLabelExpression)();

	STDMETHOD(get_FontSize2)(LONG* pVal);
	STDMETHOD(put_FontSize2)(LONG newVal);
	STDMETHOD(get_UseVariableSize)(VARIANT_BOOL* pVal);
	STDMETHOD(put_UseVariableSize)(VARIANT_BOOL newVal);
	STDMETHOD(get_LogScaleForSize)(VARIANT_BOOL* pVal);
	STDMETHOD(put_LogScaleForSize)(VARIANT_BOOL newVal);
	STDMETHOD(UpdateSizeField)();
	
private:
	int _sourceField;

	tkLabelPositioning _positioning;
	
	tkSavingMode _savingMode;
	
	// expression to generate label text
	CString _labelExpression;
	CString _floatNumberFormat;

	// the minimal size of shape to label
	long _minDrawingSize;		
	
	// visibility expression
	BSTR _expression;

	bool _fontSizeChanged;

	long _classificationField;

	IShapefile* _shapefile;

	// inner vector for a given index should exist even if there are no actual labels (parts) in it
	std::vector<std::vector<CLabelInfo*>*> _labels;
	std::vector<ILabelCategory*> _categories;
	
	ILabelCategory* _category;
	CLabelOptions* _options;
	
	BSTR _key;
	long _lastErrorCode;
	ICallback * _globalCallback;
	
	tkVerticalPosition _verticalPosition;

	double		_basicScale;
	double		_maxVisibleScale;
	double		_minVisibleScale;
	int _minVisibleZoom;
	int _maxVisibleZoom;
	long		_collisionBuffer;
	bool		_scale;
	
	VARIANT_BOOL _autoOffset;
	VARIANT_BOOL _dynamicVisibility;
	VARIANT_BOOL _avoidCollisions;
	VARIANT_BOOL _useWidthLimits;
	VARIANT_BOOL _removeDuplicates;
	VARIANT_BOOL _gdiPlus;
	VARIANT_BOOL _useVariableSize;
	VARIANT_BOOL _logScaleForSize;

    // Field names & flags for Dbf serialization configuration
    BSTR _xField;
    BSTR _yField;
    BSTR _angleField;
    BSTR _textField;
    BSTR _categoryField;
    VARIANT_BOOL _saveText;
    VARIANT_BOOL _saveCategory;
    VARIANT_BOOL _createFields;

	tkTextRenderingHint _textRenderingHint;
	VARIANT_BOOL _synchronized;

private:
	inline void ErrorMessage(long ErrorCode);
	bool GetMinMaxCategoryValue(double& globalMax, double& globalMin);
    void SetCategoryForLabel(long labelIndex, long categoryIndex);
    void UpdateLabelOffsetsFromShapefile(long labelIndex, long categoryIndex);
    CLabelInfo* CreateNewLabel(const BSTR &Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY);

public:
	void ClearLabelFrames();
	void SaveSourceField(int fieldIndex) {	_sourceField = fieldIndex;	}
	bool HasRotation();

	bool DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);

	std::vector<std::vector<CLabelInfo*>*>* get_LabelData();
	CLabelOptions* get_LabelOptions();

	void put_ParentShapefile(IShapefile* newVal);
	IShapefile* get_ParentShapefile();
	void RefreshExpressions();
	bool LabelsSynchronized();

	void ApplyExpression_(long CategoryIndex);

	CPLXMLNode* SerializeLabelData(CString ElementName, bool& saveRotation, bool& saveText, bool& saveOffsetX, bool& saveOffsetY);
	bool DeserializeLabelData(CPLXMLNode* node, bool loadRotation, bool loadText, bool loadOffsetX, bool loadOffsetY);

	bool GenerateEmptyLabels();
	void AddEmptyLabel();

	void LoadLblOptions(CPLXMLNode* node);

	bool RecalculateFontSize();
    
	
};

OBJECT_ENTRY_AUTO(__uuidof(Labels), CLabels)
