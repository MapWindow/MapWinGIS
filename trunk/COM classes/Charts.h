/**************************************************************************************
 * File name: Charts.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CCharts
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
 // Sergei Leschinski (lsu) 19 june 2010 - created the file.

#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// underlying structure
struct ChartOptions
{
	bool useVariableRadius;
	bool use3Dmode;

	double rotation;
	double thickness;
	double tilt;
	
	long transparency;
	long barWidth;
	long barHeight;
	long radius;
	long normalizationField;
	long sizeField;
	long radius2;

	OLE_COLOR lineColor;
	VARIANT_BOOL visible;
	VARIANT_BOOL avoidCollisions;
	tkChartType chartType;
	tkVerticalPosition verticalPosition;
	
	BSTR valuesFontName;
	VARIANT_BOOL valuesVisible;
	VARIANT_BOOL valuesFrameVisible;
	VARIANT_BOOL valuesFontItalic;
	VARIANT_BOOL valuesFontBold;
	OLE_COLOR valuesFontColor;
	OLE_COLOR valuesFrameColor;
	long valuesFontSize;
	tkChartValuesStyle valuesStyle;

	ChartOptions()
	{
		avoidCollisions = VARIANT_TRUE;
		barHeight = 50;
		barWidth = 10;
		chartType = chtPieChart;
		lineColor = RGB(128, 128, 128);
		normalizationField = -1;
		radius = 15;
		radius2 = radius * 2;
		rotation = 0.0;
		sizeField = -1;
		thickness = 10.0;
		tilt = 30.0;
		transparency = 255;
		use3Dmode = true;		
		useVariableRadius = false;
		verticalPosition = vpAboveAllLayers;
		visible = true;
		
		USES_CONVERSION;
		valuesFontName = A2BSTR("Arial");
		valuesFrameVisible = VARIANT_TRUE;
		valuesVisible = VARIANT_TRUE;
		valuesFontItalic = VARIANT_FALSE;
		valuesFontBold = VARIANT_FALSE;
		valuesFontColor = RGB(0,0,0);
		valuesFrameColor = RGB(255, 200, 200);
		valuesFontSize = 8;
		valuesStyle = vsHorizontal;
	}

	ChartOptions& operator=(const ChartOptions& options)
	{
		if (this == &options)
			return *this;

		this->avoidCollisions = options.avoidCollisions;
		this->barHeight = options.barHeight;
		this->barWidth = options.barWidth;
		this->chartType = options.chartType;
		this->lineColor = options.lineColor;
		this->normalizationField = options.normalizationField;
		this->radius = options.radius;
		this->radius2 = options.radius2;
		this->rotation = options.rotation;
		this->sizeField = options.sizeField;
		this->thickness = options.thickness;
		this->tilt = options.tilt;
		this->transparency = options.transparency;
		this->use3Dmode = options.use3Dmode;
		this->useVariableRadius = options.useVariableRadius;
		this->verticalPosition = options.verticalPosition;
		this->visible = options.visible;
		
		// values
		this->valuesFontBold = options.valuesFontBold;
		this->valuesFontItalic = options.valuesFontItalic;
		this->valuesFontColor = options.valuesFontColor;
		this->valuesFrameColor = options.valuesFrameColor;
		this->valuesVisible = options.valuesVisible;
		this->valuesFrameVisible = options.valuesFrameVisible;
		this->valuesFontBold = options.valuesFontBold;
		this->valuesFontSize = options.valuesFontSize;
		SysFreeString(this->valuesFontName);
		this->valuesFontName = SysAllocString(options.valuesFontName);
		this->valuesStyle = options.valuesStyle;
	}

	~ChartOptions()
	{
		if (valuesFontName)
			SysFreeString(valuesFontName);
	}
};

// necessary for drawing values
struct ValueRectangle
{
	CString string;
	CRect rect;
};

// -----------------------------------------
//	  CCharts
// -----------------------------------------
class ATL_NO_VTABLE CCharts :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCharts, &CLSID_Charts>,
	public IDispatchImpl<ICharts, &IID_ICharts, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	// constructor
	CCharts()
	{
		USES_CONVERSION;
		_key = A2BSTR("");
		_caption = A2BSTR("");
		_expression = A2BSTR("");
		_shapefile = NULL;
		_lastErrorCode = tkNO_ERROR;
		_chartsExist = false;
		_globalCallback = NULL;

		_minVisibleScale = 0.0;
		_maxVisibleScale = 100000000.0;
		_dynamicVisibility = VARIANT_FALSE;

		_collisionBuffer = 0;
		_offsetX = 0;
		_offsetY = 0;

		_savingMode = modeXML;

		gReferenceCounter.AddRef(tkInterface::idCharts);
	}
	
	// destructor
	~CCharts()
	{
		::SysFreeString(_key);
		::SysFreeString(_caption);
		::SysFreeString(_expression);

		gReferenceCounter.Release(tkInterface::idCharts);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_CHARTS)

	BEGIN_COM_MAP(CCharts)
		COM_INTERFACE_ENTRY(ICharts)
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
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *retVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Visible)(/*[out, retval]*/ VARIANT_BOOL *retVal);
	STDMETHOD(put_Visible)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_AvoidCollisions)(/*[out, retval]*/ VARIANT_BOOL *retVal);
	STDMETHOD(put_AvoidCollisions)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_ChartType)(tkChartType* retVal);
	STDMETHOD(put_ChartType)(tkChartType newVal);
	STDMETHOD(get_BarWidth)(long* retVal);
	STDMETHOD(put_BarWidth)(long newVal);
	STDMETHOD(get_BarHeight)(long* retVal);
	STDMETHOD(put_BarHeight)(long newVal);
	STDMETHOD(get_PieRadius)(long* retVal);
	STDMETHOD(put_PieRadius)(long newVal);
	STDMETHOD(get_PieRotation)(double* retVal);
	STDMETHOD(put_PieRotation)(double newVal);
	STDMETHOD(get_NumFields)(long* retVal);
	STDMETHOD(AddField)(IChartField* Field, VARIANT_BOOL* retVal);
	STDMETHOD(AddField2)(long FieldIndex, OLE_COLOR color);
	STDMETHOD(InsertField)(long Index, IChartField* Field, VARIANT_BOOL* retVal);
	STDMETHOD(InsertField2)(long Index, long FieldIndex, OLE_COLOR color, VARIANT_BOOL* retVal);
	STDMETHOD(RemoveField)(long Index, VARIANT_BOOL* vbretval);
	STDMETHOD(ClearFields)();
	STDMETHOD(MoveField)(long OldIndex, long NewIndex, VARIANT_BOOL* vbretval);
	STDMETHOD(get_Tilt)(double* retVal);
	STDMETHOD(put_Tilt)(double newVal);
	STDMETHOD(get_Thickness)(double* retVal);
	STDMETHOD(put_Thickness)(double newVal);
	STDMETHOD(get_PieRadius2)(LONG* retVal);
	STDMETHOD(put_PieRadius2)(LONG newVal);
	STDMETHOD(get_SizeField)(LONG* retVal);
	STDMETHOD(put_SizeField)(LONG newVal);
	STDMETHOD(get_NormalizationField)(LONG* retVal);
	STDMETHOD(put_NormalizationField)(LONG newVal);
	STDMETHOD(get_UseVariableRadius)(VARIANT_BOOL* retVal);
	STDMETHOD(put_UseVariableRadius)(VARIANT_BOOL newVal);
	STDMETHOD(get_Transparency)(SHORT* retVal);
	STDMETHOD(put_Transparency)(SHORT newVal);
	STDMETHOD(get_LineColor)(OLE_COLOR* retVal);
	STDMETHOD(put_LineColor)(OLE_COLOR newVal);
	STDMETHOD(get_Use3DMode)(VARIANT_BOOL* retVal);
	STDMETHOD(put_Use3DMode)(VARIANT_BOOL newVal);
	STDMETHOD(get_VerticalPosition)(tkVerticalPosition* retVal);
	STDMETHOD(put_VerticalPosition)(tkVerticalPosition newVal);
	STDMETHOD(get_Chart)(long ShapeIndex, IChart** retVal);
	//STDMETHOD(put_Chart)(long ShapeIndex, IChart* newVal);
	STDMETHOD(get_Field)(long FieldIndex, IChartField** retVal);

	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
	STDMETHOD(Generate)(tkLabelPositioning Position, VARIANT_BOOL* retVal);
	STDMETHOD(Clear)();
	STDMETHOD(DrawChart)(int** hdc, float xOrigin, float yOrigin, VARIANT_BOOL hideLabels, OLE_COLOR backColor, VARIANT_BOOL* retVal);
	STDMETHOD(get_MaxVisibleScale)(double* retval)			{*retval = _maxVisibleScale;		return S_OK;};		
	STDMETHOD(put_MaxVisibleScale)(double newVal)			{_maxVisibleScale = newVal;		return S_OK;};
	STDMETHOD(get_MinVisibleScale)(double* retval)			{*retval = _minVisibleScale;		return S_OK;};		
	STDMETHOD(put_MinVisibleScale)(double newVal)			{_minVisibleScale = newVal;		return S_OK;};
	STDMETHOD(get_DynamicVisibility)(VARIANT_BOOL* retval)	{*retval = _dynamicVisibility;		return S_OK;};		
	STDMETHOD(put_DynamicVisibility)(VARIANT_BOOL newVal)	{_dynamicVisibility = newVal;		return S_OK;};

	STDMETHOD(get_IconWidth)(long* retVal);
	STDMETHOD(get_IconHeight)(long* retVal);

	STDMETHOD(get_Caption)(BSTR* retVal);
	STDMETHOD(put_Caption)(BSTR newVal);
	
	// displaying chart values
	STDMETHOD(get_ValuesVisible)(VARIANT_BOOL* retval)				{*retval = _options.valuesVisible;		return S_OK;};
	STDMETHOD(put_ValuesVisible)(VARIANT_BOOL newVal)				{_options.valuesVisible = newVal;		return S_OK;};
	STDMETHOD(get_ValuesFrameVisible)(VARIANT_BOOL* retval)			{*retval = _options.valuesFrameVisible;	return S_OK;};
	STDMETHOD(put_ValuesFrameVisible)(VARIANT_BOOL newVal)			{_options.valuesFrameVisible = newVal;	return S_OK;};
	STDMETHOD(get_ValuesFontSize)(LONG* retval)						{*retval = _options.valuesFontSize;		return S_OK;};
	STDMETHOD(put_ValuesFontSize)(LONG newVal)						{_options.valuesFontSize = newVal;		return S_OK;};
	STDMETHOD(get_ValuesFontItalic)(VARIANT_BOOL* retval)			{*retval = _options.valuesFontItalic;	return S_OK;};
	STDMETHOD(put_ValuesFontItalic)(VARIANT_BOOL newVal)			{_options.valuesFontItalic = newVal;	return S_OK;};		
	STDMETHOD(get_ValuesFontBold)(VARIANT_BOOL* retval)				{*retval = _options.valuesFontBold;		return S_OK;};
	STDMETHOD(put_ValuesFontBold)(VARIANT_BOOL newVal)				{_options.valuesFontBold = newVal;		return S_OK;};		
	STDMETHOD(get_ValuesFontColor)(OLE_COLOR* retval)				{*retval = _options.valuesFontColor;	return S_OK;};
	STDMETHOD(put_ValuesFontColor)(OLE_COLOR newVal)				{_options.valuesFontColor = newVal;		return S_OK;};
	STDMETHOD(get_ValuesFrameColor)(OLE_COLOR* retval)				{*retval = _options.valuesFrameColor;	return S_OK;};
	STDMETHOD(put_ValuesFrameColor)(OLE_COLOR newVal)				{_options.valuesFrameColor = newVal;	return S_OK;};
	STDMETHOD(get_ValuesStyle)(tkChartValuesStyle* retval)			{*retval = _options.valuesStyle;		return S_OK;};
	STDMETHOD(put_ValuesStyle)(tkChartValuesStyle newVal)			{_options.valuesStyle = newVal;			return S_OK;};
	STDMETHOD(get_ValuesFontName)(BSTR* retval);
	STDMETHOD(put_ValuesFontName)(BSTR newVal);
	STDMETHOD(Select)(IExtents* BoundingBox, long Tolerance, SelectMode SelectMode, VARIANT* Indices, VARIANT_BOOL* retval);
	STDMETHOD(get_VisibilityExpression)(BSTR* retval);
	STDMETHOD(put_VisibilityExpression)(BSTR newVal);

	STDMETHOD(get_CollisionBuffer)(long* retval)					{*retval = _collisionBuffer;		return S_OK;};		
	STDMETHOD(put_CollisionBuffer)(long newVal)						{_collisionBuffer = newVal;		return S_OK;};

	STDMETHOD(get_OffsetX)(LONG* retval)							{*retval = _offsetX;			return S_OK;};
	STDMETHOD(put_OffsetX)(LONG newVal)								{_offsetX = newVal;			return S_OK;};
	STDMETHOD(get_OffsetY)(LONG* retval)							{*retval = _offsetY;			return S_OK;};
	STDMETHOD(put_OffsetY)(LONG newVal)								{_offsetY = newVal;			return S_OK;};

	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);

	STDMETHOD(SaveToXML)(BSTR Filename, VARIANT_BOOL* retVal);
	STDMETHOD(LoadFromXML)(BSTR Filename, VARIANT_BOOL* retVal);

	STDMETHOD(get_SavingMode)(tkSavingMode* retVal);
	STDMETHOD(put_SavingMode)(tkSavingMode newVal);
	
	STDMETHOD(DrawChartVB)(int hdc, float x, float y, VARIANT_BOOL hideLabels, OLE_COLOR backColor, VARIANT_BOOL* retVal);

private:
	tkSavingMode _savingMode;
	
	long _lastErrorCode;
	ICallback * _globalCallback;
	BSTR _key;
	BSTR _expression;
	BSTR _caption;

	IShapefile* _shapefile;
	std::vector<IChartField*> _bars;
	ChartOptions _options;
	
	double _maxVisibleScale;
	double _minVisibleScale;
	VARIANT_BOOL _dynamicVisibility;
	
	LONG _collisionBuffer;
	LONG _offsetX;
	LONG _offsetY;

public:
	bool _chartsExist;

public:
	inline void ErrorMessage(long ErrorCode);
	void SetDefaultColorScheme();
	Gdiplus::Color ChangeBrightness(OLE_COLOR color, int shiftValue, long alpha);
	void put_ParentShapefile(IShapefile* newVal);
	IShapefile* get_ParentShapefile();

	ChartOptions* get_UnderlyingOptions() { return &_options; }

	CPLXMLNode* SerializeCore(CString ElementName);
	CPLXMLNode* SerializeChartData(CString ElementName);
	bool DeserializeChartData(CPLXMLNode* node);
	bool DeserializeCore(CPLXMLNode* node);
	VARIANT_BOOL DrawChartCore(CDC* dc, float x, float y, VARIANT_BOOL hideLabels, OLE_COLOR backColor);
};

OBJECT_ENTRY_AUTO(__uuidof(Charts), CCharts)
