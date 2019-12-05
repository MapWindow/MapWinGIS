/**************************************************************************************
 * File name: ShapeDrawingOptions.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CShapeDrawingOptions and underlying structure
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
 // lsu 22 aug 2009 - Created the file; adapted the code from ShapeInfo.h
 // Paul Meems sept. 2019 - MWGIS-183: Merge .NET and VB drawing functions

#pragma once
#include "DrawingOptions.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// **************************************************************
//    CShapeDrawingOptions
// **************************************************************
class ATL_NO_VTABLE CShapeDrawingOptions :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CShapeDrawingOptions, &CLSID_ShapeDrawingOptions>,
	public IDispatchImpl<IShapeDrawingOptions, &IID_IShapeDrawingOptions, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShapeDrawingOptions()
	{	
		_pUnkMarshaler = NULL;
		_lastErrorCode = tkNO_ERROR;
		_isLineDecoration = false;
		gReferenceCounter.AddRef(tkInterface::idShapeDrawingOptions);
	}
	~CShapeDrawingOptions()
	{			
		gReferenceCounter.Release(tkInterface::idShapeDrawingOptions);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SHAPEDRAWINGOPTIONS)

	DECLARE_NOT_AGGREGATABLE(CShapeDrawingOptions)

	BEGIN_COM_MAP(CShapeDrawingOptions)
		COM_INTERFACE_ENTRY(IShapeDrawingOptions)
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
	STDMETHOD(get_Visible)(VARIANT_BOOL *pVal);
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal);
	// drawing flags
	STDMETHOD(get_FillVisible)(VARIANT_BOOL *pVal);
	STDMETHOD(put_FillVisible)(VARIANT_BOOL newVal);
	STDMETHOD(get_LineVisible)(VARIANT_BOOL *pVal);
	STDMETHOD(put_LineVisible)(VARIANT_BOOL newVal);
	// colors	
	STDMETHOD(get_FillColor)(OLE_COLOR *pVal);
	STDMETHOD(put_FillColor)(OLE_COLOR newVal);
	STDMETHOD(get_LineColor)(OLE_COLOR *pVal);
	STDMETHOD(put_LineColor)(OLE_COLOR newVal);
	
	STDMETHOD(get_DrawingMode)(tkVectorDrawingMode *pVal);
	STDMETHOD(put_DrawingMode)(tkVectorDrawingMode newVal);

	// stipples
	STDMETHOD(get_FillHatchStyle)(tkGDIPlusHatchStyle *pVal);
	STDMETHOD(put_FillHatchStyle)(tkGDIPlusHatchStyle newVal);
	STDMETHOD(get_LineStipple)(tkDashStyle *pVal);
	STDMETHOD(put_LineStipple)(tkDashStyle newVal);
	STDMETHOD(get_PointShape)(tkPointShapeType *pVal);
	STDMETHOD(put_PointShape)(tkPointShapeType newVal);

	// size, width, transparency
	STDMETHOD(get_FillTransparency)(float *pVal);
	STDMETHOD(put_FillTransparency)(float newVal);
	STDMETHOD(get_LineWidth)(float *pVal);
	STDMETHOD(put_LineWidth)(float newVal);
	STDMETHOD(get_PointSize)(float *pVal);
	STDMETHOD(put_PointSize)(float newVal);

	// stipple transparency
	STDMETHOD(get_FillBgTransparent)(VARIANT_BOOL* pVal);
	STDMETHOD(put_FillBgTransparent)(VARIANT_BOOL newVal);
	STDMETHOD(get_FillBgColor)(OLE_COLOR* pVal);
	STDMETHOD(put_FillBgColor)(OLE_COLOR newVal);
	
	// raster fill
	STDMETHOD(get_Picture)(IImage** pVal);
	STDMETHOD(put_Picture)(IImage* newVal);

	STDMETHOD(get_FillType)(tkFillType* pVal);
	STDMETHOD(put_FillType)(tkFillType newVal);					
	STDMETHOD(get_FillGradientType)(tkGradientType* pVal);
	STDMETHOD(put_FillGradientType)(tkGradientType newVal);
	STDMETHOD(get_PointType)(tkPointSymbolType* pVal);
	STDMETHOD(put_PointType)(tkPointSymbolType newVal);
	STDMETHOD(get_FillColor2)(OLE_COLOR *pVal);
	STDMETHOD(put_FillColor2)(OLE_COLOR newVal);
	STDMETHOD(get_PointRotation)(double *pVal);
	STDMETHOD(put_PointRotation)(double newVal);
	STDMETHOD(get_PointReflection)(tkPointReflectionType* pVal);
	STDMETHOD(put_PointReflection)(tkPointReflectionType newVal);

	STDMETHOD(get_PointSidesCount)(long *pVal);
	STDMETHOD(put_PointSidesCount)(long newVal);
	
	STDMETHOD(get_PointSidesRatio)(float *pVal);
	STDMETHOD(put_PointSidesRatio)(float newVal);

	STDMETHOD(get_FillRotation)(double *pVal);
	STDMETHOD(put_FillRotation)(double newVal);
	
	STDMETHOD(get_FillGradientBounds)(tkGradientBounds *pVal);
	STDMETHOD(put_FillGradientBounds)(tkGradientBounds newVal);

	STDMETHOD(get_LineTransparency)(float *pVal);
	STDMETHOD(put_LineTransparency)(float newVal);

	STDMETHOD(get_PictureScaleX)(double *pVal);
	STDMETHOD(put_PictureScaleX)(double newVal);
	STDMETHOD(get_PictureScaleY)(double *pVal);
	STDMETHOD(put_PictureScaleY)(double newVal);

	STDMETHOD(get_AlignPictureByBottom)(VARIANT_BOOL *pVal);
	STDMETHOD(put_AlignPictureByBottom)(VARIANT_BOOL newVal);
	
	// .NET overloads
	STDMETHOD(DrawPoint)(int hdc, float x, float y, 
						 int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha,
						 VARIANT_BOOL* retVal);

	STDMETHOD(DrawLine)(int hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, 
						int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, 
						VARIANT_BOOL* retVal);

	STDMETHOD(DrawRectangle)(int hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, 
						int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, 
						VARIANT_BOOL* retVal);

	STDMETHOD(DrawShape)(int hdc, float x, float y, IShape* shape,  VARIANT_BOOL drawVertices,
						int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, 
						VARIANT_BOOL* retVal);

	// VB6 overloads
	//STDMETHOD(DrawPointVB)(int hdc, float x, float y, 
	//					 int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, 
	//					 VARIANT_BOOL* retVal);

	//STDMETHOD(DrawLineVB)(int hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, 
	//					int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, 
	//					VARIANT_BOOL* retVal);

	//STDMETHOD(DrawRectangleVB)(int hdc, float x, float y, int width, int height, VARIANT_BOOL drawVertices, 
	//					int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, 
	//					VARIANT_BOOL* retVal);

	//STDMETHOD(DrawShapeVB)(int hdc, float x, float y, IShape* shape,  VARIANT_BOOL drawVertices,
	//					int clipWidth, int clipHeight, OLE_COLOR backColor, BYTE backAlpha, 
	//					VARIANT_BOOL* retVal);

	STDMETHOD(get_PointCharacter)(short* retVal);
	STDMETHOD(put_PointCharacter)(short newVal);

	STDMETHOD(get_FontName)(BSTR* retval);		
	STDMETHOD(put_FontName)(BSTR newVal);					

    STDMETHOD(get_RotationField)(BSTR* retval);
    STDMETHOD(put_RotationField)(BSTR newVal);

	STDMETHOD(Clone)(IShapeDrawingOptions** retval);

	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);

	STDMETHOD(get_VerticesVisible)(VARIANT_BOOL* retval);
	STDMETHOD(put_VerticesVisible)(VARIANT_BOOL newVal);
	STDMETHOD(get_VerticesSize)(LONG* retval);
	STDMETHOD(put_VerticesSize)(LONG newVal);
	STDMETHOD(get_VerticesColor)(OLE_COLOR* retval);
	STDMETHOD(put_VerticesColor)(OLE_COLOR newVal);
	STDMETHOD(get_VerticesFillVisible)(VARIANT_BOOL* retval);
	STDMETHOD(put_VerticesFillVisible)(VARIANT_BOOL newVal);
	STDMETHOD(get_VerticesType)(tkVertexType* retval);
	STDMETHOD(put_VerticesType)(tkVertexType newVal);

	STDMETHOD(get_FrameVisible)(VARIANT_BOOL* retval);
	STDMETHOD(put_FrameVisible)(VARIANT_BOOL newVal);

	STDMETHOD(get_FrameType)(tkLabelFrameType* retval);
	STDMETHOD(put_FrameType)(tkLabelFrameType newVal);

	STDMETHOD(get_LinePattern)(ILinePattern** retVal);
	STDMETHOD(put_LinePattern)(ILinePattern* newVal);
	
	STDMETHOD(get_Tag)(BSTR* retVal);		
	STDMETHOD(put_Tag)(BSTR newVal);					

	STDMETHOD(SetGradientFill)(OLE_COLOR color, short range);
	STDMETHOD(SetDefaultPointSymbol)(tkDefaultPointSymbol symbol);

	STDMETHOD(get_UseLinePattern)(VARIANT_BOOL* retVal);		
	STDMETHOD(put_UseLinePattern)(VARIANT_BOOL newVal);		

	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);

	STDMETHOD(get_MinScale)(double *pVal);
	STDMETHOD(put_MinScale)(double newVal);

	STDMETHOD(get_MinLineWidth)(double *pVal);
	STDMETHOD(put_MinLineWidth)(double newVal);

	STDMETHOD(get_MaxScale)(double *pVal);
	STDMETHOD(put_MaxScale)(double newVal);

	STDMETHOD(get_MaxLineWidth)(double *pVal);
	STDMETHOD(put_MaxLineWidth)(double newVal);

	STDMETHOD(get_DynamicVisibility)(VARIANT_BOOL* pVal);
	STDMETHOD(put_DynamicVisibility)(VARIANT_BOOL newVal);
	STDMETHOD(get_MinVisibleScale)(DOUBLE* pVal);
	STDMETHOD(put_MinVisibleScale)(DOUBLE newVal);
	STDMETHOD(get_MaxVisibleScale)(DOUBLE* pVal);
	STDMETHOD(put_MaxVisibleScale)(DOUBLE newVal);
	
private:
	CDrawingOptionsEx _options;
	long _lastErrorCode;
	bool _isLineDecoration;  // prevents the creation of child instances of CDrawingOptions class in case it's line decoration symbol, 
							 // which could become endless loop
private:
	VARIANT_BOOL DrawPointCore(CDC* dc, float x, float y, 
							   int clipWidth = 0, int clipHeight = 0, OLE_COLOR backColor = 16777215, BYTE backAlpha = 255);

	VARIANT_BOOL DrawLineCore(CDC* dc, float x, float y, int width, int height, VARIANT_BOOL drawVertices,
							  int clipWidth = 0, int clipHeight = 0, OLE_COLOR backColor = 16777215, BYTE backAlpha = 255);

	VARIANT_BOOL DrawRectangleCore(CDC* dc, float x, float y, int width, int height, VARIANT_BOOL drawVertices,
								   int clipWidth = 0, int clipHeight = 0, OLE_COLOR backColor = 16777215, BYTE backAlpha = 255);

	VARIANT_BOOL DrawShapeCore(CDC* dc, float x, float y, IShape* shape, VARIANT_BOOL drawVertices, 
							   int clipWidth = 0, int clipHeight = 0, OLE_COLOR backColor = 16777215, BYTE backAlpha = 255);

public:	
	bool DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);

	void ErrorMessage(long ErrorCode) {	_lastErrorCode = ErrorCode;	}

	CDrawingOptionsEx* get_UnderlyingOptions() {return &_options; }
	void put_underlyingOptions(CDrawingOptionsEx* newVal) {	_options = *newVal; }
	void put_IsLineDecoration(bool newVal){	_isLineDecoration = newVal;	}
	void get_IsLineDecoration(bool* retVal)	{*retVal = _isLineDecoration;}
};

OBJECT_ENTRY_AUTO(__uuidof(ShapeDrawingOptions), CShapeDrawingOptions)