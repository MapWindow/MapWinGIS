/**************************************************************************************
 * File name: ShapefileCategories.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of ShapeDrawingOptions - a class used to generate and store
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
 // Paul Meems August 2018: Modernized the code as suggested by CLang and ReSharper

#include "StdAfx.h"
#include "ShapefileCategories.h"
#include "ShapefileCategory.h"
#include "ShapeDrawingOptions.h"
#include "Shapefile.h"
#include "TableClass.h"
#include "TableHelper.h"

//***********************************************************************/
//*			get_Count()
//***********************************************************************/
STDMETHODIMP CShapefileCategories::get_Count(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _categories.size();
	return S_OK;
}

// ***************************************************************
//		Add()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Add(BSTR Name, IShapefileCategory** retVal)
{
	this->Insert(_categories.size(), Name, retVal);
	
	return S_OK;
}

// ***************************************************************
//		Insert()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Insert(long Index, BSTR Name, IShapefileCategory** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		
	if(Index < 0 || Index > (long)_categories.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = nullptr;
		return NULL;
	}

	*retVal = nullptr;
	IShapefileCategory* cat = nullptr;
	CoCreateInstance( CLSID_ShapefileCategory, nullptr, CLSCTX_INPROC_SERVER, IID_IShapefileCategory, (void**)&cat);
	if (cat == nullptr) return S_OK;
	
	// initialization with default options if shapefile is present
	if (_shapefile != nullptr)
	{
		IShapeDrawingOptions* defaultOpt = nullptr;
		_shapefile->get_DefaultDrawingOptions(&defaultOpt);
		CDrawingOptionsEx* newOpt =((CShapeDrawingOptions*)defaultOpt)->get_UnderlyingOptions();
		defaultOpt->Release();
		
		IShapeDrawingOptions* opt = nullptr;
		cat->get_DrawingOptions(&opt);
		((CShapeDrawingOptions*)opt)->put_underlyingOptions(newOpt);
		opt->Release();
	}
	cat->put_Name(Name);
	
	if (Index == _categories.size())
	{
		_categories.push_back(cat);
	}
	else
	{
		_categories.insert( _categories.begin() + Index, cat);
	}
	
	*retVal = cat;
	((CShapefileCategory*) *retVal)->put_parentCollection(this);

	cat->AddRef();	// we'll return the category, therefore one more reference
	return S_OK;
}

// ***************************************************************
//		Remove()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Remove(long Index, VARIANT_BOOL* vbRetval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*vbRetval = VARIANT_FALSE;

	if( Index < 0 || Index >= (long)_categories.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*vbRetval = VARIANT_FALSE;
	}
	else
	{
		_categories[Index]->Release();
		_categories[Index] = nullptr;
		_categories.erase(_categories.begin() + Index);
		*vbRetval = VARIANT_TRUE;
	}
	return S_OK;
}

// ***************************************************************
//		Clear()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	for (auto& _categorie : _categories)
	{
	    _categorie->Release();
	}
	_categories.clear();

	if (_shapefile)
	{
		std::vector<ShapeRecord*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
		for (auto& i : *data)
		{
		    i->category = -1;
		}
	}
	return S_OK;
}

// ***************************************************************
//		get/put_Item()
// ***************************************************************
STDMETHODIMP CShapefileCategories::get_Item (long Index, IShapefileCategory** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index >= (long)_categories.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retval = nullptr;
	}
	else
	{
		*retval = _categories[Index];
		_categories[Index]->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CShapefileCategories::put_Item(long Index, IShapefileCategory* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index >= (long)_categories.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
    if (!newVal)
    {
        ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
        return S_OK;
    }
    if (_categories[Index] != newVal)
    {
        _categories[Index]->Release();
        _categories[Index] = newVal;
        _categories[Index]->AddRef();
    }
    return S_OK;
}

// ***************************************************************
//		AddRange()
// ***************************************************************
// Add categories for a given range
STDMETHODIMP CShapefileCategories::AddRange(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT minValue, VARIANT maxValue, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = VARIANT_FALSE;
	
	if(_shapefile == nullptr) 
		return S_OK;
	
	CComPtr<ITable> tbl = nullptr;
	_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_OK;
		
	std::vector<CategoriesData>* values = TableHelper::Cast(tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses, minValue, maxValue);
	
	if (!values)
		return S_OK;
	
	_classificationField = -1;		// fast processing is off

	IShapefileCategory* cat = nullptr;
	
	for (auto& value : *values)
	{
		CString strValue;

		CComBSTR bstrName(value.name);
		CComBSTR bstrExpression(value.expression);
		this->Add(bstrName, &cat);
		cat->put_Expression(bstrExpression);
		cat->put_MinValue(value.minValue);
		cat->put_MaxValue(value.maxValue);
		cat->Release();
	}	

	if (ClassificationType == ctUniqueValues)
	{	
		// no fast processing in this mode as user could generate categories by several fields
		// m_classificationField = FieldIndex;		
	}
	
	delete values;
	
	//this->ApplyExpressions();

	*retVal = VARIANT_TRUE;
	return S_OK;
}

// ********************************************************
//     Generate2()
// ********************************************************
STDMETHODIMP CShapefileCategories::Generate2(BSTR fieldName, tkClassificationType ClassificationType, LONG numClasses, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (!_shapefile) return S_OK;

	CComPtr<ITable> tbl = nullptr;
	_shapefile->get_Table(&tbl);
	if (!tbl) return S_OK;

	long fieldIndex;
	tbl->get_FieldIndexByName(fieldName, &fieldIndex);

	Generate(fieldIndex, ClassificationType, numClasses, retVal);

	return S_OK;
}

// ***************************************************************
//		Generate()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Generate(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = VARIANT_FALSE;
	
	if(_shapefile == nullptr) 
		return S_OK;
	
	CComPtr<ITable> tbl = nullptr;
	_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_OK;
		
	std::vector<CategoriesData>* values = TableHelper::Cast(tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses);
	if (!values)
		return S_OK;

	GenerateCore(values, FieldIndex, ClassificationType, retVal);

	delete values;

	this->ApplyExpressions();

	*retVal = VARIANT_TRUE;
	return S_OK;
}

// ***************************************************************
//		GenerateCore()
// ***************************************************************
void CShapefileCategories::GenerateCore(std::vector<CategoriesData>* categories, long FieldIndex,
	tkClassificationType ClassificationType, VARIANT_BOOL* retVal)
{
	this->Clear();
	_classificationField = -1;		// fast processing is off

	IShapefileCategory* icat = nullptr;
	// CShapefileCategory* ct = nullptr;

	for (auto& categorie : *categories)
	{
		CComBSTR bstrName(categorie.name);
		CComBSTR bstrExpression(categorie.expression);
		
		this->Add(bstrName, &icat);
		icat->put_Expression(bstrExpression);

		icat->put_ValueType(ClassificationType == tkClassificationType::ctUniqueValues ? cvSingleValue : cvRange);
		icat->put_MinValue(categorie.minValue);
		icat->put_MaxValue(categorie.maxValue);
		icat->Release();
	}

	// fast processing of categories will be available (m_value will be used without parsing of expressions)
	_classificationField = FieldIndex;
}

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP CShapefileCategories::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CShapefileCategories::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	SysFreeString(_key);
	USES_CONVERSION;
	_key = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************
//*		get/put_Caption()
//***********************************************************************
STDMETHODIMP CShapefileCategories::get_Caption(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_caption);
	return S_OK;
}
STDMETHODIMP CShapefileCategories::put_Caption(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	SysFreeString(_caption);
	USES_CONVERSION;
	_caption = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//		get/put_VisibilityExpression()
// **********************************************************
STDMETHODIMP CShapefileCategories::get_VisibilityExpression(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        USES_CONVERSION;
    *pVal = OLE2BSTR(_visExpression);
    return S_OK;
}
STDMETHODIMP CShapefileCategories::put_VisibilityExpression(BSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
        ::SysFreeString(_visExpression);
    USES_CONVERSION;
    _visExpression = OLE2BSTR(newVal);
    return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CShapefileCategories::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("ShapefileCategories", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

STDMETHODIMP CShapefileCategories::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

STDMETHODIMP CShapefileCategories::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

//***********************************************************************/
//*		get/put_GlobalCallback()
//***********************************************************************/
STDMETHODIMP CShapefileCategories::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _globalCallback;
	if( _globalCallback != nullptr ) _globalCallback->AddRef();
	return S_OK;
}

STDMETHODIMP CShapefileCategories::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *******************************************************************
//		get_Table()
// *******************************************************************
STDMETHODIMP CShapefileCategories::get_Shapefile (IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = _shapefile;
	if (_shapefile)
		_shapefile->AddRef();
	return S_OK;
}

// *******************************************************************
//		get/put_ParentShapefile()
// *******************************************************************
// For inner use only
void CShapefileCategories::put_ParentShapefile(IShapefile* newVal)
{
	_shapefile = newVal;
}
IShapefile* CShapefileCategories::get_ParentShapefile() const
{
	return _shapefile;
}

// *******************************************************************
//		get_UnderlyingCategory()
// *******************************************************************
CDrawingOptionsEx* CShapefileCategories::get_UnderlyingOptions(int Index)
{
	if (Index >=0 && Index < (int)_categories.size())
		return ((CShapefileCategory*)_categories[Index])->get_UnderlyingOptions();
    return nullptr;
}

// *******************************************************************
//		ApplyExpressions()
// *******************************************************************
STDMETHODIMP CShapefileCategories::ApplyExpressions()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ApplyExpressionCore(-1);
	return S_OK;
}

// *******************************************************************
//		ApplyExpression()
// *******************************************************************
STDMETHODIMP CShapefileCategories::ApplyExpression(long CategoryIndex, long startRowIndex, long endRowIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	// switching off shapes that are currently included in the category
	std::vector<ShapeRecord*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
	for (auto& i : *data)
	{
		if (i->category == CategoryIndex)
		{
		    i->category = -1;
		}
	}

	ApplyExpressionCore(CategoryIndex, startRowIndex, endRowIndex);
	return S_OK;
}

// *******************************************************************
//		ApplyExpressionCore
// *******************************************************************
void CShapefileCategories::ApplyExpressionCore(long CategoryIndex, long startRowIndex, long endRowIndex)
{
	if (!_shapefile)
		return;
	
	CComPtr<ITable> tbl = nullptr;
	_shapefile->get_Table(&tbl);
	if ( !tbl )	return;
	
	// Process shape index range:
	long numShapes;
	_shapefile->get_NumShapes(&numShapes);
	endRowIndex = endRowIndex < 0 ? numShapes - 1 : endRowIndex;
	startRowIndex = startRowIndex < 0 ? 0 : startRowIndex;
	numShapes = endRowIndex - startRowIndex + 1;
		
	// vector of numShapes size with category index for each shape
	std::vector<int> results;
	results.resize(numShapes, -1);

    std::vector<std::vector<double>> rotations;
    rotations.resize(_categories.size() + 1);

	bool uniqueValues = true;
	for (auto& _categorie : _categories)
	{
		tkCategoryValue value;
	    _categorie->get_ValueType(&value);
		if (value != cvSingleValue) {
			uniqueValues = false;
			break;
		}
	}

    bool allCategories = CategoryIndex == -1;

	// ----------------------------------------------------------------
	// we got unique values classification and want to process it fast
	// ----------------------------------------------------------------
	bool parsingIsNeeded = true;	
	if (_classificationField != -1 && uniqueValues)
	{
		parsingIsNeeded = false;	// in case there are unique values only we don't need any parsing
			
		std::map<CComVariant, long> myMap;				// variant value as key and number of category as result
		for (unsigned int i = 0; i < _categories.size(); i++)
		{
            if (!allCategories && i != CategoryIndex)
                continue;

			CComVariant val;
			_categories[i]->get_MinValue(&val);
			if (val.vt != VT_EMPTY)
			{
				CComVariant val2;
				VariantCopy(&val2, &val);
				myMap[val2] = i;
			}
		}
		
		// applying categories to shapes
		VARIANT val;
		VariantInit(&val);
		for (long i = startRowIndex; i <= endRowIndex; i++)
		{
			tbl->get_CellValue(_classificationField, i, &val);
			if (myMap.find(val) != myMap.end())
			{
				results[i] = myMap[val];	// writing the index of category
			}
		}
		VariantClear(&val);
	}
		
	// -------------------------------------------------------------
	//		Analyzing expressions
	// -------------------------------------------------------------

    // Get default point rotation:
    CComPtr<IShapeDrawingOptions> options = nullptr;
    _shapefile->get_DefaultDrawingOptions(&options);
    rotations[0].resize(numShapes);
    CalculateRotations(options, tbl, rotations[0], startRowIndex, endRowIndex);

    std::vector<CStringW> expressions;
    for (unsigned int i = 0; i < _categories.size(); i++)
    {
        // Resize & set rotations to 0
        rotations[i + 1].resize(numShapes);

        // Is this a category we need?
        if (!allCategories && i != CategoryIndex)
            continue;

        // Do we even need to parse category expressions?
        if (parsingIsNeeded)
        {
            // Get the expression (will be parsed after this for-loop)
            CComBSTR expr;
            _categories[i]->get_Expression(&expr);
            USES_CONVERSION;
            CStringW str = OLE2CW(expr);
            expressions.push_back(str);
        }

        // Can we proceed to analyse rotations expressions?
        _categories[i]->get_DrawingOptions(&options);
        if (options) 
            CalculateRotations(options, tbl, rotations[i + 1],
				startRowIndex, endRowIndex);
	}

    // adding category indices for shapes in the results vector
    if (parsingIsNeeded)
	    TableHelper::Cast(tbl)->AnalyzeExpressions(
			expressions, results, startRowIndex, endRowIndex);

    // -------------------------------------------------------------
    //		Saving results
    // -------------------------------------------------------------
    for (unsigned long i = 0; i < results.size(); i++) {
        // Is this a category we need?
        if (!allCategories && i != CategoryIndex)
            continue;

		const int categoryIndex = results[i];
		const int rowIndex = startRowIndex + i;
        _shapefile->put_ShapeCategory(rowIndex, categoryIndex);
        _shapefile->put_ShapeRotation(rowIndex, rotations[categoryIndex + 1][i]);
    }
}

void CShapefileCategories::CalculateRotations(
	CComPtr<IShapeDrawingOptions>& options, CComPtr<ITable>& tbl, 
	std::vector<double>& rotations, int startIndex, int endIndex)
{
    // Fill it with defaults:
    double rotation;
    options->get_PointRotation(&rotation);

    std::fill(rotations.begin(), rotations.end(), rotation);

    // Get optional rotation expression
    CComBSTR rotExpr;
    options->get_PointRotationExpression(&rotExpr);
    if (!rotExpr || rotExpr.Length() == 0)  // no expr so we're done here
        return;

    // Calculate angles:
    CStringW ErrorString;
    TableHelper::Cast(tbl)->CalculateCoreRaw(
        OLE2CW(rotExpr),
        [&](CExpressionValue* result, int rowIndex, CStringW& ErrorString) -> int {
            USES_CONVERSION;
            if (result->isString())
                rotations[rowIndex-startIndex] = Utility::wtof_custom(result->str());
            else if (result->IsDouble())
                rotations[rowIndex-startIndex] = result->dbl();
            else
                rotations[rowIndex-startIndex] = 0.0;
            return true;
        },
        ErrorString, m_globalSettings.floatNumberFormat, 
		startIndex, endIndex, true
    );
}

// ********************************************************
//	  ApplyColorScheme()
// ********************************************************
STDMETHODIMP CShapefileCategories::ApplyColorScheme (tkColorSchemeType Type, IColorScheme* ColorScheme)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ApplyColorScheme2(Type, ColorScheme, shElementDefault);
	return S_OK;
}

// ********************************************************
//	  ApplyColorScheme2()
// ********************************************************
STDMETHODIMP CShapefileCategories::ApplyColorScheme2 (tkColorSchemeType Type, IColorScheme* ColorScheme, tkShapeElements ShapeElement)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ApplyColorScheme3(Type, ColorScheme, ShapeElement, 0, _categories.size() - 1);
	return S_OK;
}

// ********************************************************
//	  ApplyColorScheme3()
// ********************************************************
STDMETHODIMP CShapefileCategories::ApplyColorScheme3 (tkColorSchemeType Type, IColorScheme* ColorScheme, 
													  tkShapeElements ShapeElement, long CategoryStartIndex, long CategoryEndIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (!_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return S_OK;
	}

	long numBreaks;
	ColorScheme->get_NumBreaks(&numBreaks);

	if (numBreaks <= 1)
	{
		ErrorMessage(tkCOLOR_SCHEME_IS_EMPTY);
		return S_OK;
	}

	// we'll correct invalid indices
	if (CategoryEndIndex >= (long)_categories.size())
	{
		CategoryEndIndex = (long)(_categories.size() - 1);
	}

	if (CategoryStartIndex < 0)
	{
		CategoryStartIndex = 0;
	}

	if ( CategoryEndIndex == CategoryStartIndex )
	{
		CallbackHelper::ErrorMsg("Can't apply color scheme to a single a single category (start index and end index are the same.)");
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	double maxValue;
	ColorScheme->get_BreakValue(numBreaks - 1, &maxValue);
	
	// choosing the element to apply colors to
	if ( ShapeElement == shElementDefault)
	{
		ShpfileType shpType;
		_shapefile->get_ShapefileType(&shpType);
		
		if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ ||
			shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ ||
			shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)
		{
			ShapeElement = shElementFill;
		}
		else if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)
		{
			ShapeElement = shElementLines;
		}
	}

	IShapeDrawingOptions* options = nullptr;
	for (int i = CategoryStartIndex; i <= CategoryEndIndex; i++)
	{
		OLE_COLOR color;
		double value;
		if ( Type == ctSchemeRandom )			
		{
			value = double(rand()/double(RAND_MAX));
			ColorScheme->get_RandomColor(value, &color);
		}
		else if ( Type == ctSchemeGraduated )	
		{
			value = double(i - CategoryStartIndex)/double(CategoryEndIndex - CategoryStartIndex) * maxValue;
			ColorScheme->get_GraduatedColor(value, &color);
		}
		
		_categories[i]->get_DrawingOptions(&options);
		
		if ( options )
		{
			switch (ShapeElement)
			{
				case shElementFill:				options->put_FillColor(color);		break;
				case shElementFill2:			options->put_FillColor2(color);		break;
				case shElementLines:			options->put_LineColor(color);		break;
				case shElementFillBackground:	options->put_FillBgColor(color);	break;
			case shElementDefault: break;
			default: ;
			}
			options->Release();
		}
	}
	return S_OK;
}

// ***************************************************************
//		MoveUp
// ***************************************************************
STDMETHODIMP CShapefileCategories::MoveUp (long Index, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < (long)_categories.size() && Index > 0)
	{
		IShapefileCategory* catBefore = _categories[Index - 1];
		_categories[Index - 1] = _categories[Index];
		_categories[Index] = catBefore;
		
		std::vector<ShapeRecord*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
		for (auto& i : *data)
		{
			if (i->category == Index)
			{
			    i->category = Index - 1;
			}
			else if (i->category == Index - 1)
			{
			    i->category = Index;
			}
		}

		*retval = VARIANT_TRUE;
	}
	else
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retval = VARIANT_FALSE;
	}
	return S_OK;
}

// ***************************************************************
//		MoveDown
// ***************************************************************
STDMETHODIMP CShapefileCategories::MoveDown (long Index, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < (long)_categories.size() - 1 && Index >= 0)
	{
		IShapefileCategory* catAfter = _categories[Index + 1];
		_categories[Index + 1] = _categories[Index];
		_categories[Index] = catAfter;

		std::vector<ShapeRecord*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
		for (auto& i : *data)
		{
			if (i->category == Index)
			{
			    i->category = Index + 1;
			}
			else if (i->category == Index + 1)
			{
			    i->category = Index;
			}
		}
		*retval = VARIANT_TRUE;
	}
	else
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retval = VARIANT_FALSE;
	}
	return S_OK;
}


// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CShapefileCategories::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CPLXMLNode* psTree = this->SerializeCore("ShapefileCategoriesClass");
	Utility::SerializeAndDestroyXmlTree(psTree, retVal);
	return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
CPLXMLNode* CShapefileCategories::SerializeCore(CString ElementName)
{
	USES_CONVERSION;
	
	CPLXMLNode* psTree = CPLCreateXMLNode( nullptr, CXT_Element, ElementName);

    // classification field
	Utility::CPLCreateXMLAttributeAndValue(psTree, "ClassificationField", CPLString().Printf("%d", _classificationField));

	// field type
	FieldType type;
	CComPtr<ITable> table = nullptr;
	_shapefile->get_Table(&table);
	if (table && _classificationField != -1)
	{
		CComPtr<IField> fld = nullptr;
		table->get_Field(_classificationField, &fld);
		if (fld)
		{
			fld->get_Type(&type);
			Utility::CPLCreateXMLAttributeAndValue( psTree, "FieldType", CPLString().Printf("%d", (int)type));
		}
	}
	
	for (auto& _categorie : _categories)
	{
		CPLXMLNode* psNode = CPLCreateXMLNode(psTree, CXT_Element, "ShapefileCategoryClass");
		
		// name
		BSTR vbstr;
	    _categorie->get_Name(&vbstr);
		Utility::CPLCreateXMLAttributeAndValue( psNode, "Name", OLE2CA(vbstr));
		SysFreeString(vbstr);

		// expression
	    _categorie->get_Expression(&vbstr);
		CString str = OLE2CA(vbstr);

		Utility::CPLCreateXMLAttributeAndValue( psNode, "Expression", str);
		SysFreeString(vbstr);

	    auto* ct = (CShapefileCategory*)_categorie;
		Utility::CPLCreateXMLAttributeAndValue(psNode, "ValueType", CPLString().Printf("%d", (int)ct->GetCategoryValue()));

		// values
		Utility::SerializeVariant(psNode, "Value", ct->GetMinValue());
		Utility::SerializeVariant(psNode, "MaxValue", ct->GetMaxValue());
		
		// options
		IShapeDrawingOptions* options;
	    _categorie->get_DrawingOptions(&options);
		CPLXMLNode* psChild = ((CShapeDrawingOptions*)options)->SerializeCore("ShapeDrawingOptionsClass");
		if (psChild)
		{
			CPLAddXMLChild(psNode, psChild);
		}
		options->Release();
	}
	return psTree;
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CShapefileCategories::DeserializeCore(CPLXMLNode* node, bool applyExpressions)
{
	if (!node)
		return false;
	
	// removing existing categories
	this->Clear();

	_classificationField = -1;
	bool loadField = false;

	FieldType type = INTEGER_FIELD;
	CString s = CPLGetXMLValue( node, "FieldType", nullptr );
	if (s != "")
	{
		type = (FieldType)atoi(s);
		loadField = true;
	}

	if (loadField)
	{
		s = CPLGetXMLValue( node, "ClassificationField", nullptr );
		if (s != "")
		{
		    const int fieldIndex = atoi(s);

			CComPtr<ITable> table = nullptr;
			_shapefile->get_Table(&table);
			if (table)
			{
				IField* fld = nullptr;
				table->get_Field(fieldIndex, &fld);
				if (fld)
				{
					FieldType realType;
					fld->get_Type(&realType);

					if (type == realType)		// a check by name can be added as well
					{
						_classificationField = fieldIndex;
					}
					fld->Release();
				}
			}
		}
	}
	

	node = node->psChild;
	if (node)
	{
		while (node)
		{
			if (strcmp(node->pszValue, "ShapefileCategoryClass") == 0)
			{
				CString str;
				str = CPLGetXMLValue( node, "Name", nullptr );
				CComBSTR bstrName( str );
				IShapefileCategory* cat = nullptr;
				this->Add(bstrName, &cat);

				str = CPLGetXMLValue( node, "Expression", nullptr );
				CComBSTR bstrExpression(str);
				cat->put_Expression(bstrExpression);

				str = CPLGetXMLValue(node, "ValueType", nullptr);
			    const tkCategoryValue ctVal = str != "" ? (tkCategoryValue)atoi(str.GetString()) : cvExpression;
				cat->put_ValueType(ctVal);

				str = CPLGetXMLValue(node, "Value", nullptr);
				Utility::DeserializeVariant(str, type, ((CShapefileCategory*)cat)->GetMinValue());

				str = CPLGetXMLValue(node, "MaxValue", nullptr);
				Utility::DeserializeVariant(str, type, ((CShapefileCategory*)cat)->GetMaxValue());

				// drawing options
				CPLXMLNode* nodeOptions = CPLGetXMLNode( node, "ShapeDrawingOptionsClass" );
				if (nodeOptions)
				{
					IShapeDrawingOptions* options = nullptr;
					cat->get_DrawingOptions(&options);
					((CShapeDrawingOptions*)options)->DeserializeCore(nodeOptions);
					options->Release();
				}
				cat->Release();
			}
			node = node->psNext;
		} 

		if (applyExpressions)
			this->ApplyExpressions();
	}
	return true;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CShapefileCategories::Deserialize(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CString str = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(str.GetString());
	if (node)
	{
		CPLXMLNode* nodeCat = CPLGetXMLNode(node, "=ShapefileCategoriesClass");
		if (nodeCat)
		{
			DeserializeCore(nodeCat, true);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

// ********************************************************
//     get_CategoryIndexByName()
// ********************************************************
STDMETHODIMP CShapefileCategories::get_CategoryIndexByName(BSTR categoryName, int* categoryIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*categoryIndex = -1;
	for(size_t i = 0; i < _categories.size(); i++)
	{
		CComBSTR bstr;
		_categories[i]->get_Name(&bstr);
		
		if (lstrcmpW(bstr, categoryName) == 0)
		{
			*categoryIndex = i;
			break;
		}
	}
	return S_OK;
}

// ********************************************************
//     get_CategoryIndex()
// ********************************************************
STDMETHODIMP CShapefileCategories::get_CategoryIndex(IShapefileCategory* category, int* categoryIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*categoryIndex = -1;
	for(size_t i = 0; i < _categories.size(); i++)
	{
		if (_categories[i] == category)
		{
			*categoryIndex = i;
			break;
		}
	}
	return S_OK;
}

// ********************************************************
//     GenerateColorForPolygons()
// ********************************************************
STDMETHODIMP CShapefileCategories::GeneratePolygonColors(IColorScheme* scheme, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = VARIANT_FALSE;
	
	// -------------------------------------------------
	// parameter validation
	// -------------------------------------------------
	if(!_shapefile) {
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return S_OK;
	}

	ShpfileType shpType;
	_shapefile->get_ShapefileType(&shpType);
	if (ShapeUtility::Convert2D(shpType) != SHP_POLYGON)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return S_OK;
	}

	bool tempScheme = false;
	if (!scheme)
	{
		// let's create a scheme if none is provided
		ComHelper::CreateInstance(idColorScheme, (IDispatch**)&scheme);
		scheme->AddBreak(0.0, RGB(255, 179, 71));
		scheme->AddBreak(0.1, RGB(253, 253, 150));
		scheme->AddBreak(0.2, RGB(119, 221, 119));
		scheme->AddBreak(0.3, RGB(174, 198, 207));
		scheme->AddBreak(0.4, RGB(222, 165, 164));
		scheme->AddBreak(0.5, RGB(255, 105, 97));
		scheme->AddBreak(0.6, RGB(130, 105, 83));
		scheme->AddBreak(0.7, RGB(100, 20, 100));
		scheme->AddBreak(1.0, RGB(207, 207, 196));
		//scheme->SetColors4(PredefinedColorScheme::SummerMountains);
		tempScheme= true;
	}
	
	// -------------------------------------------------
	//  do the processing
	// -------------------------------------------------
	Coloring::ColorGraph* graph = ((CShapefile*)_shapefile)->GeneratePolygonColors();
	if (graph)
	{
	    const int colorCount = graph->GetColorCount();

		// -------------------------------------------------
		// create categories
		// -------------------------------------------------
	    const int firstCategory = _categories.size();
		long numBreaks;
		scheme->get_NumBreaks(&numBreaks);
		for(int i = 0; i < colorCount; i++)
		{
			CString s;
			s.Format("Color %d", i + 1);
			IShapefileCategory* ct = nullptr;

			CComBSTR bstrName(s);
			this->Add(bstrName, &ct);
			CDrawingOptionsEx* opt = ((CShapefileCategory*)ct)->get_UnderlyingOptions();
			
			OLE_COLOR clr;
			if (colorCount <= numBreaks)
			{
				// we can use colors without interpolation
				scheme->get_BreakColor(i, &clr);
			}
			else
			{
				// must use interpolation
				scheme->get_GraduatedColor(i/(double)colorCount, &clr);
			}

			opt->fillColor = clr;
			ct->Release();
		}

		// -------------------------------------------------
		// apply indices to polygons
		// -------------------------------------------------
		for (auto& node : graph->nodes)
		{
		    const int shapeId = node->id;
			_shapefile->put_ShapeCategory(shapeId, firstCategory + node->color);
		}
		delete graph;
	}

	if (tempScheme)
		scheme->Release();
	return S_OK;
}

// ********************************************************
//     Sort()
// ********************************************************
// TODO: implement
STDMETHODIMP CShapefileCategories::Sort(LONG FieldIndex, VARIANT_BOOL Ascending, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (!_shapefile)
		return S_OK;

	LONG numFields;
	_shapefile->get_NumFields(&numFields);
	if (FieldIndex < 0 || FieldIndex >= numFields)
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
	
	if (_categories.size() < 2)
	{
		// nothing to sort
		return S_OK;
	}

	CComPtr<ITable> table = nullptr;
	_shapefile->get_Table(&table);
	if (!table) return S_OK;

	IField* fld = nullptr;
	table->get_Field(FieldIndex, &fld);
	FieldType type;
	fld->get_Type(&type);
	fld->Release();
	CComVariant valDefault;
	if (type == DOUBLE_FIELD || type == INTEGER_FIELD || type == DATE_FIELD)
	{
		// each of these can be treated as numeric
		valDefault = 0;
	}
	else
	{
		// string and boolean can be treated as string
		valDefault = "";
	}
		
	multimap <CComVariant, IShapefileCategory*> map;

	for (auto& _categorie : _categories)
	{
	    const VARIANT_BOOL vbretval = VARIANT_FALSE;
		CComVariant var = NULL;
		BSTR expr;
	    _categorie->get_Expression(&expr);

		// TODO: implement
		//table->CalculateStat(FieldIndex, Operation, expr, &var, &vbretval);

		if (vbretval)
		{
			pair<CComVariant, IShapefileCategory*> myPair(var, _categorie);	
			map.insert(myPair);	
		}
		else
		{
			pair<CComVariant, IShapefileCategory*> myPair(valDefault, _categorie);	
			map.insert(myPair);	
		}
	}

    multimap<CComVariant, IShapefileCategory*>::iterator p = map.begin();
		
	int i = 0;
	ASSERT(map.size() == _categories.size());
	
	while(p != map.end())
	{
		// IShapefileCategory* cat = p->second;
		_categories[i] = p->second;
		i++; 
	    ++p;
	}

	*retVal = VARIANT_TRUE;
	return S_OK;
}

// ********************************************************
//     ClassificationField()
// ********************************************************
STDMETHODIMP CShapefileCategories::get_ClassificationField(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _classificationField;
	return S_OK;
}
STDMETHODIMP CShapefileCategories::put_ClassificationField(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_classificationField = newVal;
	return S_OK;
}

// ********************************************************
//     GetCategoryData()
// ********************************************************
void CShapefileCategories::GetCategoryData(vector<CategoriesData*>& data)
{
	USES_CONVERSION;
	data.clear();
	for (auto& _categorie : _categories)
	{
	    auto* ct = new CategoriesData();
	    _categorie->get_MinValue(&ct->minValue);
	    _categorie->get_MaxValue(&ct->maxValue);
	    _categorie->get_ValueType(&ct->valueType);
		CComBSTR expr;
	    _categorie->get_Expression(&expr);
		ct->expression = OLE2A(expr);
		ct->classificationField = _classificationField;
		data.push_back(ct);
	}
}

// ********************************************************
//     Add2()
// ********************************************************
STDMETHODIMP CShapefileCategories::Add2(IShapefileCategory* category)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!category)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	_categories.push_back(category);
	category->AddRef();
	return S_OK;
}

// ********************************************************
//     Insert2()
// ********************************************************
STDMETHODIMP CShapefileCategories::Insert2(LONG index, IShapefileCategory* category, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!category)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	if (index < 0 || index >= (long)_categories.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	}
	_categories.insert(_categories.begin() + index, category);
	category->AddRef();
	return S_OK;
}
