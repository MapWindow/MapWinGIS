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
	*pVal = m_categories.size();
	return S_OK;
};


// ***************************************************************
//		Add()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Add(BSTR Name, IShapefileCategory** retVal)
{
	this->Insert(m_categories.size(), Name, retVal);
	
	return S_OK;
}

// ***************************************************************
//		Insert()
// ***************************************************************
STDMETHODIMP CShapefileCategories::Insert(long Index, BSTR Name, IShapefileCategory** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		
	if(Index < 0 || Index > (long)m_categories.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
		return NULL;
	}

	*retVal = NULL;
	IShapefileCategory* cat = NULL;
	CoCreateInstance( CLSID_ShapefileCategory, NULL, CLSCTX_INPROC_SERVER, IID_IShapefileCategory, (void**)&cat);
	if (cat == NULL) return S_OK;
	
	// initializtion with default options if shapefile is present
	if (m_shapefile != NULL)
	{
		IShapeDrawingOptions* defaultOpt = NULL;
		m_shapefile->get_DefaultDrawingOptions(&defaultOpt);
		CDrawingOptionsEx* newOpt =((CShapeDrawingOptions*)defaultOpt)->get_UnderlyingOptions();
		defaultOpt->Release();
		
		IShapeDrawingOptions* opt = NULL;
		cat->get_DrawingOptions(&opt);
		((CShapeDrawingOptions*)opt)->put_underlyingOptions(newOpt);
		opt->Release();
	}
	cat->put_Name(Name);
	
	if (Index == m_categories.size())
	{
		m_categories.push_back(cat);
	}
	else
	{
		m_categories.insert( m_categories.begin() + Index, cat);
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

	if( Index < 0 || Index >= (long)m_categories.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*vbretval = VARIANT_FALSE;
	}
	else
	{
		m_categories[Index]->Release();
		m_categories[Index] = NULL;
		m_categories.erase(m_categories.begin() + Index);
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
	for (unsigned int i=0; i < m_categories.size(); i++ )
	{
		m_categories[i]->Release();
	}
	m_categories.clear();

	if (m_shapefile)
	{
		std::vector<ShapeData*>* data = ((CShapefile*)m_shapefile)->get_ShapeVector();
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
	if( Index < 0 || Index >= (long)m_categories.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retval = NULL;
	}
	else
	{
		*retval = m_categories[Index];
		m_categories[Index]->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CShapefileCategories::put_Item(long Index, IShapefileCategory* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index >= (long)m_categories.size() )
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
			if (m_categories[Index] != newVal)
			{
				m_categories[Index]->Release();
				m_categories[Index] = newVal;
				m_categories[Index]->AddRef();
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
	
	if(m_shapefile == NULL) 
		return S_FALSE;
	
	ITable* tbl = NULL;
	m_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_FALSE;
		
	std::vector<CategoriesData>* values = ((CTableClass*)tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses, minValue, maxValue);
	tbl->Release();
	tbl = NULL;
	
	if (!values)
		return S_FALSE;
	

	//this->Clear();
	m_classificationField = -1;		// fast processing is off

	IShapefileCategory* cat = NULL;
	
	for (int i = 0; i < (int)values->size(); i++ )
	{
		CString strValue;

		this->Add((*values)[i].name.AllocSysString(), &cat);
		cat->put_Expression((*values)[i].expression.AllocSysString());
		((CShapefileCategory*)cat)->m_value = (*values)[i].minValue;	// must be used after put_Expression, otherwise will be lost
																		// does "=" operator work for variants?
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
	
	if(m_shapefile == NULL) 
		return S_FALSE;
	
	ITable* tbl = NULL;
	m_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_FALSE;
		
	std::vector<CategoriesData>* values = ((CTableClass*)tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses);
	if (!values)
		return S_OK;

	this->Clear();
	m_classificationField = -1;		// fast processing is off

	IShapefileCategory* cat = NULL;
	
	for (int i = 0; i < (int)values->size(); i++ )
	{
		CString strValue;

		this->Add((*values)[i].name.AllocSysString(), &cat);
		cat->put_Expression((*values)[i].expression.AllocSysString());
		((CShapefileCategory*)cat)->m_value = (*values)[i].minValue;	// must be used after put_Expression, otherwise will be lost
																		// does "=" operator work for variants?
		cat->Release();
	}	

	if (ClassificationType == ctUniqueValues)
	{	
		// fast processing of categories will be available (m_value will be used without parsing of expressions)
		m_classificationField = FieldIndex;		
	}
	
	delete values;
	
	this->ApplyExpressions();

	*retVal = VARIANT_TRUE;
	return S_OK;
}

//***********************************************************************
//*		get/put_Key()
//***********************************************************************
STDMETHODIMP CShapefileCategories::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(m_key);
	return S_OK;
}
STDMETHODIMP CShapefileCategories::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(m_key);
	USES_CONVERSION;
	m_key = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************
//*		get/put_Caption()
//***********************************************************************
STDMETHODIMP CShapefileCategories::get_Caption(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(m_caption);
	return S_OK;
}
STDMETHODIMP CShapefileCategories::put_Caption(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(m_caption);
	USES_CONVERSION;
	m_caption = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
void CShapefileCategories::ErrorMessage(long ErrorCode)
{
	m_lastErrorCode = ErrorCode;
	if( m_globalCallback != NULL) m_globalCallback->Error(OLE2BSTR(m_key),A2BSTR(ErrorMsg(m_lastErrorCode)));
	return;
}

STDMETHODIMP CShapefileCategories::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_lastErrorCode;
	m_lastErrorCode = tkNO_ERROR;
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
	*pVal = m_globalCallback;
	if( m_globalCallback != NULL ) m_globalCallback->AddRef();
	return S_OK;
}

STDMETHODIMP CShapefileCategories::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&m_globalCallback);
	return S_OK;
}

// *******************************************************************
//		get_Table()
// *******************************************************************
STDMETHODIMP CShapefileCategories::get_Shapefile (IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = m_shapefile;
	if (m_shapefile)
		m_shapefile->AddRef();
	return S_OK;
}

// *******************************************************************
//		get/put_ParentShapefile()
// *******************************************************************
// For inner use only
void CShapefileCategories::put_ParentShapefile(IShapefile* newVal)
{
	m_shapefile = newVal;
}
IShapefile* CShapefileCategories::get_ParentShapefile()
{
	return m_shapefile;
}

// *******************************************************************
//		get_UnderlyingCategory()
// *******************************************************************
CDrawingOptionsEx* CShapefileCategories::get_UnderlyingOptions(int Index)
{
	// TODO: add check for input parameter
	return ((CShapefileCategory*)m_categories[Index])->get_UnderlyingOptions();
}

// *******************************************************************
//		ApplyExpressions()
// *******************************************************************
STDMETHODIMP CShapefileCategories::ApplyExpressions()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ApplyExpression_(-1);
	return S_OK;
}

// *******************************************************************
//		ApplyExpression()
// *******************************************************************
STDMETHODIMP CShapefileCategories::ApplyExpression(long CategoryIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	// switching off shapes that are currently included in the category
	std::vector<ShapeData*>* data = ((CShapefile*)m_shapefile)->get_ShapeVector();
	for (unsigned int i = 0; i < data->size(); i++)
	{
		if ((*data)[i]->category == CategoryIndex)
		{
			(*data)[i]->category = -1;
		}
	}

	ApplyExpression_(CategoryIndex);
	return S_OK;
}

// *******************************************************************
//		ApplyExpression_
// *******************************************************************
void CShapefileCategories::ApplyExpression_(long CategoryIndex)
{
	if (!m_shapefile)
		return;
	
	ITable* tbl = NULL;
	m_shapefile->get_Table(&tbl);
	if ( tbl )
	{
		long numShapes;
		m_shapefile->get_NumShapes(&numShapes);
		
		// vector of numShapes size with category index for each shape
		std::vector<int> results;
		results.resize(numShapes, -1);

		// ----------------------------------------------------------------
		// we got unique values classification and want to process it fast
		// ----------------------------------------------------------------
		bool parsingIsNeeded = true;	
		if (m_classificationField != -1)
		{
			parsingIsNeeded = false;	// in case there are unique values only we don't need any parsing
			
			std::map<CComVariant, long> myMap;				// variant value as key and number of category as result
			for (unsigned int i = 0; i < m_categories.size(); i++)
			{
				if (i == CategoryIndex || CategoryIndex == -1 )
				{
					CComVariant* val = &(((CShapefileCategory*)m_categories[i])->m_value);
					if (val->vt != VT_EMPTY)
					{
						CComVariant val2;
						VariantCopy(&val2, val);
						myMap[val2] = i;
					}
				}
			}
		
			// applying categories to shapes
			VARIANT val;
			VariantInit(&val);
			for (long i = 0; i < numShapes; i++)
			{
				tbl->get_CellValue(m_classificationField, i, &val);
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
			BSTR expr;
			std::vector<CString> expressions;
			for (unsigned int i = 0; i < m_categories.size(); i++)
			{
				if (i == CategoryIndex || CategoryIndex == -1 )
				{
					CComVariant* val = &(((CShapefileCategory*)m_categories[i])->m_value);
					if (val->vt != VT_EMPTY && m_classificationField != -1)
					{
						// we analyzed this one before, so just a dummy string here
						CString str = "";
						expressions.push_back(str);
					}
					else
					{
						m_categories[i]->get_Expression(&expr);
						USES_CONVERSION;
						CString str = OLE2CA(expr);	
						::SysFreeString(expr);
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
				m_shapefile->put_ShapeCategory(i, results[i]);
			}
		}
		else
		{
			for (unsigned long i = 0; i < results.size(); i++)
			{
				if (results[i] == CategoryIndex)	// wasn't tested !!!
					m_shapefile->put_ShapeCategory(i, CategoryIndex);
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
	ApplyColorScheme3(Type, ColorScheme, ShapeElement, 0, m_categories.size() - 1);
	return S_OK;
}

// ********************************************************
//	  ApplyColorScheme3()
// ********************************************************
STDMETHODIMP CShapefileCategories::ApplyColorScheme3 (tkColorSchemeType Type, IColorScheme* ColorScheme, 
													  tkShapeElements ShapeElement, long CategoryStartIndex, long CategoryEndIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (!m_shapefile)
	{
		// TODO: report error
		return S_OK;
	}

	long numBreaks;
	ColorScheme->get_NumBreaks(&numBreaks);

	if (numBreaks <= 1)
	{
		// TODO: report error
		return S_OK;
	}

	// we'll correct inproper indices
	if (CategoryEndIndex >= (long)m_categories.size())
	{
		CategoryEndIndex = (long)(m_categories.size() - 1);
	}

	if (CategoryStartIndex < 0)
	{
		CategoryStartIndex = 0;
	}

	if ( CategoryEndIndex == CategoryStartIndex )
	{
		// TODO: report error
		return S_OK;
	}

	double maxValue;
	ColorScheme->get_BreakValue(numBreaks - 1, &maxValue);
	
	// choosing the element to apply colors to
	if ( ShapeElement == shElementDefault)
	{
		ShpfileType shpType;
		m_shapefile->get_ShapefileType(&shpType);
		
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
		
		m_categories[i]->get_DrawingOptions(&options);
		
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
	if (Index < (long)m_categories.size() && Index > 0)
	{
		IShapefileCategory* catBefore = m_categories[Index - 1];
		m_categories[Index - 1] = m_categories[Index];
		m_categories[Index] = catBefore;
		
		std::vector<ShapeData*>* data = ((CShapefile*)m_shapefile)->get_ShapeVector();
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
	if (Index < (long)m_categories.size() - 1 && Index >= 0)
	{
		IShapefileCategory* catAfter = m_categories[Index + 1];
		m_categories[Index + 1] = m_categories[Index];
		m_categories[Index] = catAfter;

		std::vector<ShapeData*>* data = ((CShapefile*)m_shapefile)->get_ShapeVector();
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
	USES_CONVERSION;

	CPLXMLNode* node = this->SerializeCore("ShapefileCategoriesClass");
	if (node)
	{
		CString str = CPLSerializeXMLTree(node);	
		CPLDestroyXMLNode(node);
		*retVal = A2BSTR(str);
	}
	else
	{
		*retVal = A2BSTR("");
	}
	return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
CPLXMLNode* CShapefileCategories::SerializeCore(CString ElementName)
{
	USES_CONVERSION;
	
	// we need to return empty node even if there are no categories (for serialize/Deserialize routines)
	/*if (m_categories.size() == 0)
	{
		return NULL;
	}*/

	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, ElementName);
	CString str;

	// classification field
	bool serializeField = (m_classificationField != -1) && m_shapefile;
	FieldType type;
	
	if (serializeField)
	{
		serializeField  = false;

		// field type
		ITable* table = NULL;
		m_shapefile->get_Table(&table);
		if (table)
		{
			IField* fld = NULL;
			table->get_Field(m_classificationField, &fld);

			if (fld)
			{
				fld->get_Type(&type);

				Utility::CPLCreateXMLAttributeAndValue( psTree, "ClassificationField", CPLString().Printf("%d", m_classificationField));
				Utility::CPLCreateXMLAttributeAndValue( psTree, "FieldType", CPLString().Printf("%d", (int)type));
				serializeField = true;

				fld->Release();
			}
			table->Release();
		}
	}
	

	for (unsigned int i = 0; i < m_categories.size(); i++)
	{
		CPLXMLNode* psNode = CPLCreateXMLNode(psTree, CXT_Element, "ShapefileCategoryClass");
		
		// name
		BSTR vbstr;
		m_categories[i]->get_Name(&vbstr);
		Utility::CPLCreateXMLAttributeAndValue( psNode, "Name", OLE2CA(vbstr));
		SysFreeString(vbstr);

		// expression
		m_categories[i]->get_Expression(&vbstr);
		str = OLE2CA(vbstr);
		//str.Replace("%", "%%");

		Utility::CPLCreateXMLAttributeAndValue( psNode, "Expression", str);			//CPLString().Printf(str)
		SysFreeString(vbstr);
		
		if (serializeField)
		{
			CComVariant* val = &(((CShapefileCategory*)m_categories[i])->m_value);
			
			if (type == STRING_FIELD)
			{
				Utility::CPLCreateXMLAttributeAndValue( psNode, "Value", OLE2CA(val->bstrVal));
			}
			else if (type == DOUBLE_FIELD)
			{
				Utility::CPLCreateXMLAttributeAndValue( psNode, "Value", CPLString().Printf("%f",val->dblVal));
			}
			else if (type == INTEGER_FIELD)
			{
				Utility::CPLCreateXMLAttributeAndValue( psNode, "Value", CPLString().Printf("%d", val->lVal));
			}
		}
		
		// options
		IShapeDrawingOptions* options;
		m_categories[i]->get_DrawingOptions(&options);
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

	m_classificationField = -1;
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
			m_shapefile->get_Table(&table);
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
						m_classificationField = fieldIndex;
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
				BSTR vbstr = A2BSTR( str );
				IShapefileCategory* cat = NULL;
				this->Add( vbstr, &cat );

				str = CPLGetXMLValue( node, "Expression", NULL );
				vbstr = A2BSTR( str );
				cat->put_Expression( vbstr );
				
				if (m_classificationField != -1)
				{
					str = CPLGetXMLValue( node, "Value", NULL );
					switch (type)
					{
						case STRING_FIELD:
							((CShapefileCategory*)cat)->m_value.vt = VT_BSTR;
							((CShapefileCategory*)cat)->m_value.bstrVal = A2BSTR( str );
							break;
						case INTEGER_FIELD:
							((CShapefileCategory*)cat)->m_value.vt = VT_I4;
							((CShapefileCategory*)cat)->m_value.lVal = atoi(str);
							break;
						case DOUBLE_FIELD:
							((CShapefileCategory*)cat)->m_value.vt = VT_R8;
							((CShapefileCategory*)cat)->m_value.dblVal = Utility::atof_custom(str);
							break;
					}
				}

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
	for(size_t i = 0; i < m_categories.size(); i++)
	{
		CComBSTR bstr;
		m_categories[i]->get_Name(&bstr);
		
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
	for(size_t i = 0; i < m_categories.size(); i++)
	{
		if (m_categories[i] == category)
		{
			*categoryIndex = i;
			break;
		}
	}
	return S_OK;
}

// ********************************************************
//     Sort()
// ********************************************************
//STDMETHODIMP CShapefileCategories::Sort(LONG FieldIndex, VARIANT_BOOL Ascending, tkGroupOperation Operation, VARIANT_BOOL* retVal)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	*retVal = VARIANT_FALSE;
//
//	if (!m_shapefile)
//		return S_OK;
//
//	LONG numFields;
//	m_shapefile->get_NumFields(&numFields);
//	if (FieldIndex < 0 || FieldIndex >= numFields)
//	{
//		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
//		return S_OK;
//	}
//	
//	if (m_categories.size() < 2)
//	{
//		// nothing to sort
//		return S_OK;
//	}
//
//	ITable* table = NULL;
//	m_shapefile->get_Table(&table);
//	if (!table)
//	{
//		return S_OK;
//	}
//
//	IField* fld = NULL;
//	table->get_Field(FieldIndex, &fld);
//	FieldType type;
//	fld->get_Type(&type);
//	fld->Release();
//	CComVariant valDefault;
//	if (type == DOUBLE_FIELD || INTEGER_FIELD)
//	{
//		valDefault = 0;
//	}
//	else
//	{
//		valDefault = "";
//	}
//		
//	multimap <CComVariant, IShapefileCategory*> map;
//
//	for (unsigned int i = 0; i < m_categories.size(); i++)
//	{
//		VARIANT_BOOL vbretval;
//		CComVariant var = NULL;
//		BSTR expr;
//		m_categories[i]->get_Expression(&expr);
//		table->CalculateStat(FieldIndex, Operation, expr, &var, &vbretval);
//
//		if (vbretval)
//		{
//			pair<CComVariant, IShapefileCategory*> myPair(var, m_categories[i]);	
//			map.insert(myPair);	
//		}
//		else
//		{
//			pair<CComVariant, IShapefileCategory*> myPair(valDefault, m_categories[i]);	
//			map.insert(myPair);	
//		}
//	}
//	
//	if (table)
//	{
//		table->Release();
//		table = NULL;
//	}
//	
//	// returning result
//	multimap <CComVariant, IShapefileCategory*>::iterator p;
//	p = map.begin();
//		
//	int i = 0;
//	ASSERT(map.size() == m_categories.size());
//	
//	while(p != map.end())
//	{
//		IShapefileCategory* cat = p->second;
//		m_categories[i] = p->second;
//		i++; p++;
//	}
//
//	*retVal = VARIANT_TRUE;
//	return S_OK;
//}
