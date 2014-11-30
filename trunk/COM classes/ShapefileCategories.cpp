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

#include "stdafx.h"
#include "ShapefileCategories.h"
#include "ShapefileCategory.h"
#include "ShapeDrawingOptions.h"
#include "Shapefile.h"
#include "TableClass.h"

//***********************************************************************/
//*			get_Count()
//***********************************************************************/
STDMETHODIMP CShapefileCategories::get_Count(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _categories.size();
	return S_OK;
};

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
		*retVal = VARIANT_FALSE;
		return NULL;
	}

	*retVal = NULL;
	IShapefileCategory* cat = NULL;
	CoCreateInstance( CLSID_ShapefileCategory, NULL, CLSCTX_INPROC_SERVER, IID_IShapefileCategory, (void**)&cat);
	if (cat == NULL) return S_OK;
	
	// initialization with default options if shapefile is present
	if (_shapefile != NULL)
	{
		IShapeDrawingOptions* defaultOpt = NULL;
		_shapefile->get_DefaultDrawingOptions(&defaultOpt);
		CDrawingOptionsEx* newOpt =((CShapeDrawingOptions*)defaultOpt)->get_UnderlyingOptions();
		defaultOpt->Release();
		
		IShapeDrawingOptions* opt = NULL;
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
STDMETHODIMP CShapefileCategories::Remove(long Index, VARIANT_BOOL* vbretval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*vbretval = VARIANT_FALSE;

	if( Index < 0 || Index >= (long)_categories.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*vbretval = VARIANT_FALSE;
	}
	else
	{
		_categories[Index]->Release();
		_categories[Index] = NULL;
		_categories.erase(_categories.begin() + Index);
		*vbretval = VARIANT_TRUE;
	}
	return S_OK;
}

// ***************************************************************
//		Clear()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	for (unsigned int i=0; i < _categories.size(); i++ )
	{
		_categories[i]->Release();
	}
	_categories.clear();

	if (_shapefile)
	{
		std::vector<ShapeData*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
		for (unsigned  int i = 0; i < data->size(); i++)
		{
			(*data)[i]->category = -1;
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
		*retval = NULL;
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
		return S_FALSE;
	}
	else
	{
		if (!newVal)
		{
			ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
			return S_FALSE;
		}
		else
		{
			if (_categories[Index] != newVal)
			{
				_categories[Index]->Release();
				_categories[Index] = newVal;
				_categories[Index]->AddRef();
			}
			return S_OK;
		}
	}
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
	
	if(_shapefile == NULL) 
		return S_FALSE;
	
	ITable* tbl = NULL;
	_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_FALSE;
		
	std::vector<CategoriesData>* values = ((CTableClass*)tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses, minValue, maxValue);
	tbl->Release();
	tbl = NULL;
	
	if (!values)
		return S_FALSE;
	
	_classificationField = -1;		// fast processing is off

	IShapefileCategory* cat = NULL;
	
	for (int i = 0; i < (int)values->size(); i++ )
	{
		CString strValue;

		CComBSTR bstrName((*values)[i].name);
		CComBSTR bstrExpression((*values)[i].expression);
		this->Add(bstrName, &cat);
		cat->put_Expression(bstrExpression);
		cat->put_MinValue((*values)[i].minValue);
		cat->put_MaxValue((*values)[i].maxValue);
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

// ***************************************************************
//		Generate()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Generate(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = VARIANT_FALSE;
	
	if(_shapefile == NULL) 
		return S_FALSE;
	
	ITable* tbl = NULL;
	_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_FALSE;
		
	std::vector<CategoriesData>* values = ((CTableClass*)tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses);
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

	IShapefileCategory* icat = NULL;
	CShapefileCategory* ct = NULL;

	for (int i = 0; i < (int)categories->size(); i++)
	{
		CString strValue;
		CComBSTR bstrName((*categories)[i].name);
		CComBSTR bstrExpression((*categories)[i].expression);
		
		this->Add(bstrName, &icat);
		icat->put_Expression(bstrExpression);

		icat->put_ValueType(ClassificationType == tkClassificationType::ctUniqueValues ? cvSingleValue : cvRange);
		icat->put_MinValue((*categories)[i].minValue);
		icat->put_MaxValue((*categories)[i].maxValue);
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
	::SysFreeString(_key);
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
	::SysFreeString(_caption);
	USES_CONVERSION;
	_caption = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CShapefileCategories::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	Utility::DisplayErrorMsg(_globalCallback, _key, ErrorMsg(_lastErrorCode));
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
	if( _globalCallback != NULL ) _globalCallback->AddRef();
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
IShapefile* CShapefileCategories::get_ParentShapefile()
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
	else
		return NULL;
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
STDMETHODIMP CShapefileCategories::ApplyExpression(long CategoryIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	// switching off shapes that are currently included in the category
	std::vector<ShapeData*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
	for (unsigned int i = 0; i < data->size(); i++)
	{
		if ((*data)[i]->category == CategoryIndex)
		{
			(*data)[i]->category = -1;
		}
	}

	ApplyExpressionCore(CategoryIndex);
	return S_OK;
}

// *******************************************************************
//		ApplyExpressionCore
// *******************************************************************
void CShapefileCategories::ApplyExpressionCore(long CategoryIndex)
{
	if (!_shapefile)
		return;
	
	ITable* tbl = NULL;
	_shapefile->get_Table(&tbl);
	if ( tbl )
	{
		long numShapes;
		_shapefile->get_NumShapes(&numShapes);
		
		// vector of numShapes size with category index for each shape
		std::vector<int> results;
		results.resize(numShapes, -1);

		bool uniqueValues = true;
		for (unsigned int i = 0; i < _categories.size(); i++) {
			tkCategoryValue value;
			_categories[i]->get_ValueType(&value);
			if (value != cvSingleValue) {
				uniqueValues = false;
				break;
			}
		}

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
				if (i == CategoryIndex || CategoryIndex == -1 )
				{
					CComVariant val;
					_categories[i]->get_MinValue(&val);
					if (val.vt != VT_EMPTY)
					{
						CComVariant val2;
						VariantCopy(&val2, &val);
						myMap[val2] = i;
					}
				}
			}
		
			// applying categories to shapes
			VARIANT val;
			VariantInit(&val);
			for (long i = 0; i < numShapes; i++)
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
		if (parsingIsNeeded)
		{
			// building list of expressions
			
			std::vector<CString> expressions;
			for (unsigned int i = 0; i < _categories.size(); i++)
			{
				if (i == CategoryIndex || CategoryIndex == -1 )
				{
					CComVariant val;
					_categories[i]->get_MinValue(&val);
					//if (val.vt != VT_EMPTY && _classificationField != -1)
					//{
					//	// we analyzed this one before, so just a dummy string here
					//	CString str = "";
					//	expressions.push_back(str);
					//}
					//else
					{
						CComBSTR expr;
						_categories[i]->get_Expression(&expr);
						USES_CONVERSION;
						CString str = OLE2CA(expr);	
						expressions.push_back(str);
					}
				}
				else
				{
					// we don't need this categories, so dummy strings for them
					CString str = "";
					expressions.push_back(str);
				}
			}

			// adding category indices for shapes in the results vector
			((CTableClass*)tbl)->AnalyzeExpressions(expressions, results);
		}
		
		// saving results
		if (CategoryIndex == -1 )
		{
			for (unsigned long i = 0; i < results.size(); i++)
			{
				_shapefile->put_ShapeCategory(i, results[i]);
			}
		}
		else
		{
			for (unsigned long i = 0; i < results.size(); i++)
			{
				if (results[i] == CategoryIndex)
					_shapefile->put_ShapeCategory(i, CategoryIndex);
			}
		}
		tbl->Release();
	}
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

	IShapeDrawingOptions* options = NULL;
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
		
		std::vector<ShapeData*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
		for (unsigned int i = 0; i < data->size(); i++)
		{
			if ((*data)[i]->category == Index)
			{
				(*data)[i]->category = Index - 1;
			}
			else if ((*data)[i]->category == Index - 1)
			{
				(*data)[i]->category = Index;
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

		std::vector<ShapeData*>* data = ((CShapefile*)_shapefile)->get_ShapeVector();
		for (unsigned int i = 0; i < data->size(); i++)
		{
			if ((*data)[i]->category == Index)
			{
				(*data)[i]->category = Index + 1;
			}
			else if ((*data)[i]->category == Index + 1)
			{
				(*data)[i]->category = Index;
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
	
	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, ElementName);
	CString str;

	// classification field
	Utility::CPLCreateXMLAttributeAndValue(psTree, "ClassificationField", CPLString().Printf("%d", _classificationField));

	// field type
	FieldType type;
	CComPtr<ITable> table = NULL;
	_shapefile->get_Table(&table);
	if (table && _classificationField != -1)
	{
		CComPtr<IField> fld = NULL;
		table->get_Field(_classificationField, &fld);
		if (fld)
		{
			fld->get_Type(&type);
			Utility::CPLCreateXMLAttributeAndValue( psTree, "FieldType", CPLString().Printf("%d", (int)type));
		}
	}
	
	for (unsigned int i = 0; i < _categories.size(); i++)
	{
		CPLXMLNode* psNode = CPLCreateXMLNode(psTree, CXT_Element, "ShapefileCategoryClass");
		
		// name
		BSTR vbstr;
		_categories[i]->get_Name(&vbstr);
		Utility::CPLCreateXMLAttributeAndValue( psNode, "Name", OLE2CA(vbstr));
		SysFreeString(vbstr);

		// expression
		_categories[i]->get_Expression(&vbstr);
		str = OLE2CA(vbstr);

		Utility::CPLCreateXMLAttributeAndValue( psNode, "Expression", str);
		SysFreeString(vbstr);

		CShapefileCategory* ct = (CShapefileCategory*)_categories[i];
		Utility::CPLCreateXMLAttributeAndValue(psNode, "ValueType", CPLString().Printf("%d", (int)ct->GetCategoryValue()));

		// values
		Utility::SerializeVariant(psNode, "Value", ct->GetMinValue());
		Utility::SerializeVariant(psNode, "MaxValue", ct->GetMaxValue());
		
		// options
		IShapeDrawingOptions* options;
		_categories[i]->get_DrawingOptions(&options);
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
	CString s = CPLGetXMLValue( node, "FieldType", NULL );
	if (s != "")
	{
		type = (FieldType)atoi(s);
		loadField = true;
	}

	if (loadField)
	{
		s = CPLGetXMLValue( node, "ClassificationField", NULL );
		if (s != "")
		{
			int fieldIndex = atoi(s);

			ITable* table = NULL;
			_shapefile->get_Table(&table);
			if (table)
			{
				IField* fld = NULL;
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
				table->Release();
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
				str = CPLGetXMLValue( node, "Name", NULL );
				CComBSTR bstrName( str );
				IShapefileCategory* cat = NULL;
				this->Add(bstrName, &cat);

				str = CPLGetXMLValue( node, "Expression", NULL );
				CComBSTR bstrExpression(str);
				cat->put_Expression(bstrExpression);

				str = CPLGetXMLValue(node, "ValueType", NULL);
				tkCategoryValue ctVal = (str != "") ? (tkCategoryValue)atoi(str.GetString()) : cvExpression;
				cat->put_ValueType(ctVal);

				str = CPLGetXMLValue(node, "Value", NULL);
				Utility::DeserializeVariant(str, type, ((CShapefileCategory*)cat)->GetMinValue());

				str = CPLGetXMLValue(node, "MaxValue", NULL);
				Utility::DeserializeVariant(str, type, ((CShapefileCategory*)cat)->GetMaxValue());

				// drawing options
				CPLXMLNode* nodeOptions = CPLGetXMLNode( node, "ShapeDrawingOptionsClass" );
				if (nodeOptions)
				{
					IShapeDrawingOptions* options = NULL;
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
		return S_FALSE;
	}

	ShpfileType shpType;
	_shapefile->get_ShapefileType(&shpType);
	if (Utility::ShapeTypeConvert2D(shpType) != SHP_POLYGON)
	{
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return S_FALSE;
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
		int colorCount = graph->GetColorCount();
		Debug::WriteLine("Has non-colored nodes: %d", graph->HasNonColoredNodes());
		Debug::WriteLine("Number of colors used: %d", colorCount);

		// -------------------------------------------------
		// create categories
		// -------------------------------------------------
		int firstCategory = _categories.size();
		long numBreaks;
		scheme->get_NumBreaks(&numBreaks);
		for(int i = 0; i < colorCount; i++)
		{
			CString s;
			s.Format("Color %d", i + 1);
			IShapefileCategory* ct = NULL;

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
		for(size_t i = 0; i < graph->nodes.size(); i++)
		{
			int shapeId = graph->nodes[i]->id;
			_shapefile->put_ShapeCategory(shapeId, firstCategory + graph->nodes[i]->color);
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

	ITable* table = NULL;
	_shapefile->get_Table(&table);
	if (!table)
	{
		return S_OK;
	}

	IField* fld = NULL;
	table->get_Field(FieldIndex, &fld);
	FieldType type;
	fld->get_Type(&type);
	fld->Release();
	CComVariant valDefault;
	if (type == DOUBLE_FIELD || INTEGER_FIELD)
	{
		valDefault = 0;
	}
	else
	{
		valDefault = "";
	}
		
	multimap <CComVariant, IShapefileCategory*> map;

	for (unsigned int i = 0; i < _categories.size(); i++)
	{
		VARIANT_BOOL vbretval = VARIANT_FALSE;
		CComVariant var = NULL;
		BSTR expr;
		_categories[i]->get_Expression(&expr);

		// TODO: implement
		//table->CalculateStat(FieldIndex, Operation, expr, &var, &vbretval);

		if (vbretval)
		{
			pair<CComVariant, IShapefileCategory*> myPair(var, _categories[i]);	
			map.insert(myPair);	
		}
		else
		{
			pair<CComVariant, IShapefileCategory*> myPair(valDefault, _categories[i]);	
			map.insert(myPair);	
		}
	}
	
	if (table)
	{
		table->Release();
		table = NULL;
	}
	
	// returning result
	multimap <CComVariant, IShapefileCategory*>::iterator p;
	p = map.begin();
		
	int i = 0;
	ASSERT(map.size() == _categories.size());
	
	while(p != map.end())
	{
		IShapefileCategory* cat = p->second;
		_categories[i] = p->second;
		i++; p++;
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
	for (size_t i = 0; i < _categories.size(); i++) 
	{
		CategoriesData* ct = new CategoriesData();
		_categories[i]->get_MinValue(&ct->minValue);
		_categories[i]->get_MaxValue(&ct->maxValue);
		_categories[i]->get_ValueType(&ct->valueType);
		CComBSTR expr;
		_categories[i]->get_Expression(&expr);
		ct->expression = OLE2A(expr);
		ct->classificationField = _classificationField;
		data.push_back(ct);
	}
}