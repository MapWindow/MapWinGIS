/**************************************************************************************
 * File name: ShapefileCategories.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of ShapeDrawingOptions - a class used to generate and store
 * categories for shapefile visualization
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
 // lsu 9 may 2010 - created the file

#pragma once
#include "ShapefileCategory.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CShapefileCategories
class ATL_NO_VTABLE CShapefileCategories :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CShapefileCategories, &CLSID_ShapefileCategories>,
	public IDispatchImpl<IShapefileCategories, &IID_IShapefileCategories, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShapefileCategories()
	{
		_pUnkMarshaler = NULL;
		_shapefile = NULL;
		_key = SysAllocString(L"");
		_caption = SysAllocString(L"");
        _visExpression = SysAllocString(L"");
		_globalCallback = NULL;
		_lastErrorCode = tkNO_ERROR;
		_classificationField = -1;
		gReferenceCounter.AddRef(tkInterface::idShapefileCategories);
	}

	~CShapefileCategories()
	{
		::SysFreeString(_key);
		::SysFreeString(_caption);
		_shapefile = NULL;
		for(int i = 0; i< (int)_categories.size(); i++)
			_categories[i]->Release();
		_categories.clear();

		if (_globalCallback)
		{
			_globalCallback->Release();
			_globalCallback = NULL;
		}
		gReferenceCounter.Release(tkInterface::idShapefileCategories);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SHAPEFILECATEGORIES)

	BEGIN_COM_MAP(CShapefileCategories)
		COM_INTERFACE_ENTRY(IShapefileCategories)
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
	STDMETHOD(get_Count)(long* pVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	
	STDMETHOD(get_Shapefile)(IShapefile** retVal);
	STDMETHOD(Add)(BSTR Name, IShapefileCategory** retVal);
	STDMETHOD(Insert)(long Index, BSTR Name, IShapefileCategory** retVal);
	STDMETHOD(Remove)(long Index, VARIANT_BOOL* vbretval);
	STDMETHOD(Clear)();
	STDMETHOD(get_Item)(long Index, IShapefileCategory** retval);
	STDMETHOD(put_Item)(long Index, IShapefileCategory* newVal);
	STDMETHOD(Generate)(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT_BOOL* retVal);
	STDMETHOD(ApplyExpressions)();
	STDMETHOD(ApplyExpression)(long CategoryIndex, long startRowIndex = -1, long endRowIndex = -1);

	STDMETHOD(ApplyColorScheme)(tkColorSchemeType Type, IColorScheme* ColorScheme);
	STDMETHOD(ApplyColorScheme2)(tkColorSchemeType Type, IColorScheme* ColorScheme, tkShapeElements ShapeElement);
	STDMETHOD(ApplyColorScheme3)(tkColorSchemeType Type, IColorScheme* ColorScheme, tkShapeElements ShapeElement, long CategoryStartIndex, long CategoryEndIndex);

	STDMETHOD(get_Caption)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Caption)(/*[in]*/ BSTR newVal);

    STDMETHOD(get_VisibilityExpression)(BSTR* retval);
    STDMETHOD(put_VisibilityExpression)(BSTR newVal);

	STDMETHOD(MoveUp)(long Index, VARIANT_BOOL* retval);
	STDMETHOD(MoveDown)(long Index, VARIANT_BOOL* retval);
	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);
	STDMETHOD(AddRange)(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT minValue, VARIANT maxValue, VARIANT_BOOL* retVal);
	STDMETHOD(get_CategoryIndexByName)(BSTR categoryName, int* categoryIndex);
	STDMETHOD(get_CategoryIndex)(IShapefileCategory* category, int* categoryIndex);
	STDMETHOD(GeneratePolygonColors)(IColorScheme* scheme, VARIANT_BOOL* retval);
	STDMETHOD(Sort)(LONG FieldIndex, VARIANT_BOOL Ascending, VARIANT_BOOL* retVal);
	STDMETHOD(get_ClassificationField)(LONG* pVal);
	STDMETHOD(put_ClassificationField)(LONG newVal);
	STDMETHOD(Generate2)(BSTR fieldName, tkClassificationType ClassificationType, LONG numClasses, VARIANT_BOOL* retVal);
	STDMETHOD(Add2)(IShapefileCategory* category);
	STDMETHOD(Insert2)(LONG index, IShapefileCategory* category, VARIANT_BOOL* retVal);
	
private:
	BSTR _key;
	BSTR _caption;

    // visibility expression
    BSTR _visExpression;

	long _lastErrorCode;
	ICallback * _globalCallback;
	IShapefile* _shapefile;		// parent shapefile
	long _classificationField;		// used for fast processing of unique values classification; 
									// m_value property of each category (with vt different from VT_EMPTY) will be used in this case
									// should be set to -1 to use the common expression parsing

	std::vector<IShapefileCategory*> _categories;

private:
	void ErrorMessage(long ErrorCode);
    void CalculateRotations(CComPtr<IShapeDrawingOptions>& options, CComPtr<ITable>& tbl,
		std::vector<double>& rotations, int startIndex = -1, int endIndex = -1);
	bool get_AreaValues(std::vector<double>* values);
	bool get_LengthValues(std::vector<double>* values);
	
public:	
	bool DeserializeCore(CPLXMLNode* node, bool applyExpressions);
	CPLXMLNode* SerializeCore(CString ElementName);

	void put_ParentShapefile(IShapefile* newVal);
	IShapefile* get_ParentShapefile() const;
	CDrawingOptionsEx* get_UnderlyingOptions(int Index);
	void GenerateCore(std::vector<CategoriesData>* categories, long FieldIndex, tkClassificationType ClassificationType, VARIANT_BOOL* retVal);
	void GetCategoryData(vector<CategoriesData*>& data);
	void ApplyExpressionCore(long CategoryIndex, long startShapeIndex = 0, long endShapeIndex = -1);
};

OBJECT_ENTRY_AUTO(__uuidof(ShapefileCategories), CShapefileCategories)
