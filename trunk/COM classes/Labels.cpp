/**************************************************************************************
 * File name: Labels.cpp 
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CLabels
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

#include "stdafx.h"
#include "GeometryHelper.h"
#include "LabelCategory.h"
#include "LabelClass.h"
#include "Labels.h"
#include "Shapefile.h"
#include "TableClass.h"
#include "Templates.h"

// ////////////////////////////////////////////////////////////// //
//	PROPERTIES COMMON FOR CLABELS AND CLABELCATEGORY
//	CAN BE COPIED TO THE OTHER CLASS; DON'T BREAK THIS BLOCK
// ////////////////////////////////////////////////////////////// //

// *****************************************************************
//		FontName()
// *****************************************************************
STDMETHODIMP CLabels::get_FontName(BSTR* retval)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retval = OLE2BSTR(m_options.fontName);		
	return S_OK;
};	
STDMETHODIMP CLabels::put_FontName(BSTR newVal)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	::SysFreeString(m_options.fontName);
	m_options.fontName = OLE2BSTR(newVal);
	return S_OK;
};	

// *****************************************************************
//		Font/FrameTransparency()
// *****************************************************************
STDMETHODIMP CLabels::get_FontTransparency(long* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.fontTransparency;		
	return S_OK;
};		
STDMETHODIMP CLabels::put_FontTransparency(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal < 0) newVal = 0;
	if (newVal > 255) newVal = 255;
	m_options.fontTransparency = newVal;
	return S_OK;
};
STDMETHODIMP CLabels::get_FrameTransparency(long* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_options.frameTransparency;		
	return S_OK;
};		
STDMETHODIMP CLabels::put_FrameTransparency(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal < 0) newVal = 0;
	if (newVal > 255) newVal = 255;
	m_options.frameTransparency = newVal;
	return S_OK;
};

// *****************************************************************
//			Font style options
// *****************************************************************
STDMETHODIMP CLabels::get_FontItalic(VARIANT_BOOL* retval)
{
	*retval = ((m_options.fontStyle & fstItalic)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontItalic(VARIANT_BOOL newVal)
{
	if (newVal)		m_options.fontStyle |= fstItalic;
	else			m_options.fontStyle &= (0xFFFFFFFF ^ fstItalic);
	return S_OK;
};
STDMETHODIMP CLabels::get_FontBold(VARIANT_BOOL* retval)
{
	*retval = ((m_options.fontStyle & fstBold)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontBold(VARIANT_BOOL newVal)
{
	if (newVal)		m_options.fontStyle |= fstBold;
	else			m_options.fontStyle &= (0xFFFFFFFF ^ fstBold);
	return S_OK;
};
STDMETHODIMP CLabels::get_FontUnderline(VARIANT_BOOL* retval)
{
	*retval = ((m_options.fontStyle & fstUnderline)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontUnderline(VARIANT_BOOL newVal)
{
	if (newVal)		m_options.fontStyle |= fstUnderline;
	else			m_options.fontStyle &= (0xFFFFFFFF ^ fstUnderline);
	return S_OK;
};
STDMETHODIMP CLabels::get_FontStrikeOut(VARIANT_BOOL* retval)
{
	*retval = ((m_options.fontStyle & fstStrikeout)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontStrikeOut(VARIANT_BOOL newVal)
{
	if (newVal)		m_options.fontStyle |= fstStrikeout;
	else			m_options.fontStyle &= (0xFFFFFFFF ^ fstStrikeout);
	return S_OK;
};
////////////////////////////////////////////////////////////////
//      END OF COMMON OPTIONS
////////////////////////////////////////////////////////////////

//***********************************************************************/
//*		get_Key()
//***********************************************************************/
STDMETHODIMP CLabels::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(m_key);
	return S_OK;
}
STDMETHODIMP CLabels::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(m_key);
	USES_CONVERSION;
	m_key = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
inline void CLabels::ErrorMessage(long ErrorCode)
{
	m_lastErrorCode = ErrorCode;
	Utility::DisplayErrorMsg(m_globalCallback, m_key, ErrorMsg(m_lastErrorCode));
}
STDMETHODIMP CLabels::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_lastErrorCode;
	m_lastErrorCode = tkNO_ERROR;
	return S_OK;
}
STDMETHODIMP CLabels::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

STDMETHODIMP CLabels::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_globalCallback;
	if( m_globalCallback != NULL ) 
	{
		m_globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CLabels::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&m_globalCallback);
	return S_OK;
}

// ************************************************************
//		get_Label
// ************************************************************
STDMETHODIMP CLabels::get_Label(long Index, long Part, ILabel** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Index < 0 || Index >= (long)m_labels.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = NULL;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = m_labels[Index];
		if (Part < 0 || Part >= (long)parts->size())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			*pVal = NULL;
		}		
		else
		{
			ILabel* lbl = NULL;
			CoCreateInstance(CLSID_Label,NULL,CLSCTX_INPROC_SERVER,IID_ILabel,(void**)&lbl);
			CLabelClass* coLabel = static_cast<CLabelClass*>(lbl);
			coLabel->put_LabelData(reinterpret_cast<char*>(parts->at(Part)));
			*pVal = lbl;
		}
	}
	return S_OK;
};

//***********************************************************************/
//*			get_NumCategories()
//***********************************************************************/
STDMETHODIMP CLabels::get_NumCategories(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_categories.size();
	return S_OK;
};

//***********************************************************************/
//*		get_numParts()
//***********************************************************************/
STDMETHODIMP CLabels::get_NumParts(long Index, long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index >= (long)m_labels.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = -1;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = m_labels[Index];
		*pVal = parts->size();
	}
	return S_OK;
};
 
//***********************************************************************/
//*			get_Category()
//***********************************************************************/
STDMETHODIMP CLabels::get_Category(long Index, ILabelCategory** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = NULL;
	
	//if (Index == -1)
	//{
		// TODO (important): label category should be stored in the label class directly
		// in case we return reference to underlying structure, we'll have crash
		// when user release the parent label class
		
		// we shall return labels settings in this case
		//ILabelCategory* cat = NULL;
		//this->get_Options(&cat);
		//*retval = cat;
	//}
	//else
	{
		if( Index < 0 || Index >= (long)m_categories.size() )
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else
		{
			*retval	= m_categories[Index];
			if (*retval !=NULL)
				(*retval)->AddRef();
		}
	}
	return S_OK;
}
// ************************************************************
//		put_Category
// ************************************************************
STDMETHODIMP CLabels::put_Category(long Index, ILabelCategory* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( Index < 0 || Index >= (long)m_categories.size() )
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		if (newVal == NULL)
		{
			ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		}
		else
		{
			Utility::put_ComReference(newVal, (IDispatch**)&m_categories[Index], false);
		}
	}
	return S_OK;
};

//***********************************************************************/
//*		AddLabel()
//***********************************************************************/
STDMETHODIMP CLabels::AddLabel(BSTR Text, double x, double y, double Rotation, long Category)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL vbretval;
	this->InsertLabel(m_labels.size(), Text, x, y, Rotation, Category, &vbretval);
	return S_OK;
}

//***********************************************************************/
//*		InsertLabel()
//***********************************************************************/
STDMETHODIMP CLabels::InsertLabel(long Index, BSTR Text, double x, double y, double Rotation, long Category, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index > (long)m_labels.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
		
	CLabelInfo* lbl = new CLabelInfo();
	
	lbl->text = Text;
	lbl->x = x;
	lbl->y = y;
	lbl->rotation = Rotation;
	lbl->category = Category;

	std::vector<CLabelInfo*>* parts = new std::vector<CLabelInfo*>;
	parts->push_back(lbl);
	
	if (Index == m_labels.size())
	{
		m_labels.push_back(parts);
		*retVal = VARIANT_TRUE;
	}
	else
	{
		m_labels.insert( m_labels.begin() + m_labels.size(), parts);
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}
//***********************************************************************/
/*		RemoveLabel()
//***********************************************************************/
STDMETHODIMP CLabels::RemoveLabel(long Index, VARIANT_BOOL* vbretval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index >= (long)m_labels.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*vbretval = VARIANT_FALSE;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = m_labels[Index];
		for(unsigned int i=0; i < parts->size(); i++)
		{
			delete parts->at(i);
		}
		parts->clear();
		m_labels.erase(m_labels.begin() + Index);
		*vbretval = VARIANT_TRUE;
	}
	return S_OK;
};

///***********************************************************************/
///*		AddPart()
///***********************************************************************/
STDMETHODIMP CLabels::AddPart(long Index, BSTR Text, double x, double y, double Rotation, long Category)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index > (int)m_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	} 
	std::vector<CLabelInfo*>* parts = m_labels[Index];
	
	VARIANT_BOOL vbretval;
	InsertPart(Index, parts->size(), Text, x, y, Rotation, Category, &vbretval);
	return S_OK;
};

///***********************************************************************/
///*		AddPart()
///***********************************************************************/
STDMETHODIMP CLabels::InsertPart(long Index, long Part, BSTR Text, double x, double y, double Rotation, long Category, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Index < 0 || Index >=(int)m_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	} 
	std::vector<CLabelInfo*>* parts = m_labels[Index];

	CLabelInfo* lbl = new CLabelInfo();
	
	lbl->text = Text;
	lbl->x = x;
	lbl->y = y;
	lbl->rotation = Rotation;
	lbl->category = Category;
	
	if (Part == parts->size())
	{
		parts->push_back(lbl);
		*retVal = VARIANT_TRUE;
	}
	else
	{
		parts->insert( parts->begin() + parts->size(), lbl);
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
};
///***********************************************************************/
///*		RemovePart()
///***********************************************************************/
STDMETHODIMP CLabels::RemovePart(long Index, long Part, VARIANT_BOOL* vbretval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( Index < 0 || Index >= (long)m_labels.size() )
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*vbretval = VARIANT_FALSE;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = m_labels[Index];
		if(Part < 0 || Part >= (long)parts->size())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
			*vbretval = VARIANT_FALSE;
		}
		else
		{
			delete parts->at(Part);
			parts->erase(parts->begin() + Part);
			*vbretval = VARIANT_TRUE;
		}
	}
	return S_OK;
};


// *****************************************************************
//			AddCategory()
// *****************************************************************
STDMETHODIMP CLabels::AddCategory(BSTR Name, ILabelCategory** retVal)
{
	this->InsertCategory(m_categories.size(), Name, retVal);
	return S_OK;
}

// *****************************************************************
//			InsertCategory()
// *****************************************************************
STDMETHODIMP CLabels::InsertCategory(long Index, BSTR Name, ILabelCategory** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if(Index < 0 || Index > (long)m_categories.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = NULL;
		return NULL;
	}
	
	*retVal = NULL;
	ILabelCategory* cat = NULL;
	CoCreateInstance( CLSID_LabelCategory, NULL, CLSCTX_INPROC_SERVER, IID_ILabelCategory, (void**)&cat);
	if (cat == NULL) return S_OK;

	CLabelOptions* options = this->get_LabelOptions(); 
	((CLabelCategory*)cat)->put_LabelOptions(options);
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
	cat->AddRef();	// we'll return the category, therefore one more reference
	return S_OK;
}

// *****************************************************************
//			RemoveCategory()
// *****************************************************************
STDMETHODIMP CLabels::RemoveCategory(long Index, VARIANT_BOOL* vbretval)
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

//***********************************************************************/
//*		ClearAllLabels()
//***********************************************************************/
STDMETHODIMP CLabels::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	for( size_t i = 0; i < m_labels.size(); i++ )
	{	
		for(size_t j = 0; j < m_labels[i]->size(); j++)
		{
			delete m_labels[i]->at(j);
		}
		delete m_labels[i];
	}
	m_labels.clear();
	return S_OK;
};

// *****************************************************************
//			ClearAllCategories()
// *****************************************************************
STDMETHODIMP CLabels::ClearCategories()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	for (unsigned int i=0; i < m_categories.size(); i++ )
	{
		m_categories[i]->Release();
	}
	m_categories.clear();
	
	for (unsigned  int i = 0; i < m_labels.size(); i++)
	{
		(*m_labels[i])[0]->category = -1;
	}
	return S_OK;
}

// ********************************************************
//	  ApplyColorScheme()
// ********************************************************
STDMETHODIMP CLabels::ApplyColorScheme (tkColorSchemeType Type, IColorScheme* ColorScheme)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ApplyColorScheme2(Type, ColorScheme, leDefault);
	return S_OK;
}

// ********************************************************
//	  ApplyColorScheme2()
// ********************************************************
STDMETHODIMP CLabels::ApplyColorScheme2 (tkColorSchemeType Type, IColorScheme* ColorScheme, tkLabelElements Element)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ApplyColorScheme3(Type, ColorScheme, Element, 0, m_categories.size() - 1);
	return S_OK;
}

// ********************************************************
//	  ApplyColorScheme3()
// ********************************************************
STDMETHODIMP CLabels::ApplyColorScheme3 (tkColorSchemeType Type, IColorScheme* ColorScheme, 
										 tkLabelElements Element, long CategoryStartIndex, long CategoryEndIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	long numBreaks;
	ColorScheme->get_NumBreaks(&numBreaks);

	if (Element == leDefault)
	{
		if (m_options.frameVisible)
			Element = leFrameBackground;
		else
			Element = leFont;
	}

	if (numBreaks <= 1)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	// we'll correct inproper indices
	if (CategoryEndIndex >= (long)m_categories.size())
		CategoryEndIndex = (long)(m_categories.size() - 1);

	if (CategoryStartIndex < 0)
		CategoryStartIndex = 0;

	if ( CategoryEndIndex == CategoryStartIndex )
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	double maxValue;
	ColorScheme->get_BreakValue(numBreaks - 1, &maxValue);
	
	for (int i = CategoryStartIndex; i <= CategoryEndIndex; i++)
	{
		double value = double(i - CategoryStartIndex)/double(CategoryEndIndex - CategoryStartIndex) * maxValue;
		
		OLE_COLOR color;
		if ( Type == ctSchemeRandom )			ColorScheme->get_RandomColor(value, &color);
		else if ( Type == ctSchemeGraduated )	ColorScheme->get_GraduatedColor(value, &color);
		
		switch (Element)
		{
			case leFrameBackground:		
				m_categories[i]->put_FrameBackColor(color);
				break;
			case leFrameOutline:		
				m_categories[i]->put_FrameOutlineColor(color);
				break;
			case leFont:		
				m_categories[i]->put_FontColor(color);
				break;
			case leFontOutline :		
				m_categories[i]->put_FontOutlineColor(color);
				break;
			case leShadow:		
				m_categories[i]->put_ShadowColor(color);
				break;
			case leHalo:		
				m_categories[i]->put_HaloColor(color);
				break;
		}
	}
	return S_OK;
}

// *****************************************************************
//		GetMinMaxCategoryValue()
// *****************************************************************
bool CLabels::GetMinMaxCategoryValue(double& globalMax, double& globalMin)
{
	CComVariant vMin, vMax;
	for(unsigned int i = 0; i < m_categories.size(); i++)
	{
		m_categories[i]->get_MinValue(&vMin);
		m_categories[i]->get_MaxValue(&vMax);

		// TODO: it can be string value, we must not process it
		double dMin, dMax;
		dVal(vMin, dMin); dVal(vMin, dMax);
		vMin.Clear(); vMax.Clear();
		
		double mean = (dMin + dMax)/2.0;
		if (i == 0)
		{
			globalMax = mean;
			globalMin = mean;
		}
		else
		{
			if (mean > globalMax) globalMax = mean;
			if (mean < globalMin) globalMin = mean;
		}
	}
	return true;
}

// *****************************************************************
//		Select()
// *****************************************************************
// Selection of labels which fall in the given bounding box
STDMETHODIMP CLabels::Select(IExtents* BoundingBox, long Tolerance, SelectMode SelectMode, VARIANT* LabelIndices, VARIANT* PartIndices, VARIANT_BOOL* retval)
{
	*retval = VARIANT_FALSE;
	if (!BoundingBox) return S_OK;
	double xMin, yMin, zMin, xMax, yMax, zMax;
	BoundingBox->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
	CRect box(int(xMin - Tolerance/2), int(yMin - Tolerance/2), int(xMax + Tolerance/2), int(yMax + Tolerance/2));
	
	vector<long> indices;
	vector<long> parts;
	
	IUtils* utils = NULL;
	CoCreateInstance(CLSID_Utils,NULL,CLSCTX_INPROC_SERVER,IID_IUtils,(void**)&utils);

	for (unsigned long i = 0; i < m_labels.size(); i++)
	{
		vector<CLabelInfo*>* labelParts = m_labels[i];
		for (unsigned long j =0; j < labelParts->size(); j++ )
		{
			CLabelInfo* lbl = labelParts->at(j);
			if (lbl->isDrawn)
			{
				if (lbl->horizontalFrame)
				{
					CRect* frame = lbl->horizontalFrame;
					tkExtentsRelation relation = GeometryHelper::RelateExtents(box, *frame);
					if ((SelectMode == INTERSECTION) && (relation != erNone))
					{
						indices.push_back(i);
						parts.push_back(j);
					}
					else if ((SelectMode == INCLUSION) && ((relation == erInclude) || (relation == erEqual)))
					{
						indices.push_back(i);
						parts.push_back(j);
					}
				}
				else if (lbl->rotatedFrame)
				{
					CRotatedRectangle* frame = lbl->rotatedFrame;
					//if (frame ==  NULL)
					//	continue;

					tkExtentsRelation relation = GeometryHelper::RelateExtents(*frame->BoundingBox(), box);
					if (relation == erNone) 
					{
						continue;
					}
					else if (relation == erEqual)
					{
						// this is good for both modes
						indices.push_back(i);
						parts.push_back(j);
					}
					else
					{
						if ((frame->Intersects(box)) && (SelectMode == INTERSECTION)) 
						{
							// this variant is appropriate for INTERSECTION only
							indices.push_back(i);
							parts.push_back(j);
						}
						else
						{
							if (box.PtInRect(frame->points[0]))
							{
								// frame is inside box (one point is in it, and we know that there is no intersection)
								// this is good for both modes
								indices.push_back(i);
								parts.push_back(j);
							}
							else
							{
								// TODO: Make function which take array POINTs as input to test point in polygon
								// we shall make a point in polygon test; 
								VARIANT_BOOL vbretval;
								IShape* shp = NULL;
								CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
								shp->put_ShapeType(SHP_POLYGON);
								long part = 0;
								shp->InsertPart(0, &part, &vbretval);
								IPoint* pnt = NULL;
								for (long n =0; n < 5; n++)
								{
									int k = n;
									if (k == 4) k = 0;

									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									pnt->put_X(double(frame->points[k].x));
									pnt->put_Y(double(frame->points[k].y));
									shp->InsertPoint(pnt,&n,&vbretval);
									pnt->Release();
								}

								m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
								pnt->put_X(double(box.left));
								pnt->put_Y(double(box.bottom));

								utils->PointInPolygon(shp, pnt, &vbretval);
								pnt->Release();
								if (vbretval)
								{
									// box is inside frame; this is good for both modes
									indices.push_back(i);
									parts.push_back(j);
								}
							}
						}
					}
				}
			}
		}
	}

	utils->Release();

	bool result = Templates::Vector2SafeArray(&indices, VT_I4, LabelIndices);
	(*retval) = Templates::Vector2SafeArray(&parts, VT_I4, PartIndices);
	(*retval) = (*retval) && result;

	return S_OK;
};

// *******************************************************************
//		put_ParentShapefile()
// *******************************************************************
// For inner use only
void CLabels::put_ParentShapefile(IShapefile* newVal)
{
	m_shapefile = newVal;
}
IShapefile* CLabels::get_ParentShapefile()
{
	return m_shapefile;
}

// *******************************************************************
//	Passing data inside ocx
// *******************************************************************
std::vector<std::vector<CLabelInfo*>*>* CLabels::get_LabelData()
{
	//return reinterpret_cast<char*>(&m_labels);
	return &m_labels;
}
CLabelOptions* CLabels::get_LabelOptions()
{
	return &m_options;
}

// *****************************************************************
//	    GenerateCategories()
// *****************************************************************
STDMETHODIMP CLabels::GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = VARIANT_FALSE;

	if(m_shapefile == NULL) 
	{
		return S_FALSE;
	}
	
	ITable* tbl = NULL;
	m_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_FALSE;
		
	std::vector<CategoriesData>* values = ((CTableClass*)tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses);
	if (!values)
		return S_OK;

	this->ClearCategories();
	m_classificationField = -1;		// fast processing is off

	ILabelCategory* cat = NULL;
	
	for (int i = 0; i < (int)values->size(); i++ )
	{
		CString strValue;

		this->AddCategory((*values)[i].name.AllocSysString(), &cat);
		cat->put_Expression((*values)[i].expression.AllocSysString());
		((CLabelCategory*)cat)->m_value = (*values)[i].minValue;	// must be used after put_Expression, otherwise will be lost
																		// does "=" operator work for variants?
		cat->Release();
	}	

	if (ClassificationType == ctUniqueValues)
	{	
		// fast processing of categories will be available (m_value will be used without parsing of expressions)
		m_classificationField = FieldIndex;		
	}
	
	delete values;
	
	*retVal = VARIANT_TRUE;
	return S_OK;
}

// *******************************************************************
//		ApplyExpression_
// *******************************************************************
void CLabels::ApplyExpression_(long CategoryIndex)
{
	if (!m_shapefile)
		return;
	
	VARIANT_BOOL synchronized;
	this->get_Synchronized(&synchronized);
	if (!synchronized)
	{
		ErrorMessage(tkLABELS_NOT_SYNCHRONIZE);
		return;
	}

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
					CComVariant* val = &(((CLabelCategory*)m_categories[i])->m_value);
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
					CComVariant* val = &(((CLabelCategory*)m_categories[i])->m_value);
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
				(*m_labels[i])[0]->category = results[i];
				//m_shapefile->put_ShapeCategory(i, results[i]);
			}
		}
		else
		{
			for (unsigned long i = 0; i < results.size(); i++)
			{
				if (results[i] == CategoryIndex)	// wasn't tested !!!
				{
					(*m_labels[i])[0]->category = results[i];
				}
					//m_shapefile->put_ShapeCategory(i, CategoryIndex);
			}
		}
		tbl->Release();
	}
}

// *****************************************************************
//			ApplyCategories()
// *****************************************************************
// Detemines to which category belong individual labels
STDMETHODIMP CLabels::ApplyCategories()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	this->ApplyExpression_(-1);
	return S_OK;
}

// *******************************************************************
//		RefreshExpressions()
// *******************************************************************
// Writes expressions according to MinValue, MaxValue properties of categories
void CLabels::RefreshExpressions()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (m_shapefile == NULL) return ;
	
	long numFields;
	m_shapefile->get_NumFields(&numFields);
	if (m_classificationField < 0 || m_classificationField >= numFields)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return;
	}
	
	IField* fld = NULL;
	BSTR name;
	m_shapefile->get_Field(m_classificationField, &fld);
	fld->get_Name(&name);
	
	USES_CONVERSION;
	CString fieldName = OLE2CA(name);
	
	SysFreeString(name);
	fieldName = "[" + fieldName + "]";

	for(int i = 0; i < (int)m_categories.size(); i++)
	{
		CComVariant vMin, vMax;
		m_categories[i]->get_MinValue(&vMin);
		m_categories[i]->get_MaxValue(&vMax);
		
		double dMax, dMin;
		dVal(vMin, dMin);
		dVal(vMax, dMax);

		CString sMax;
		CString sMin;
		CString s;

		sMin.Format("%g", dMin);
		sMax.Format("%g", dMax);
		
		s = fieldName + " >= " + sMin + " AND " + fieldName + " <= " + sMax;

		BSTR result;
		result = s.AllocSysString();
		m_categories[i]->put_Expression(result);
	}
	return;
}

// *****************************************************************
//			get/put_ClassificationField()
// *****************************************************************
STDMETHODIMP CLabels::get_ClassificationField(long* FieldIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*FieldIndex = m_classificationField;
	return S_OK;
}
STDMETHODIMP CLabels::put_ClassificationField(long FieldIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_classificationField = FieldIndex;
	return S_OK;
}

// *****************************************************************
//			get/put_Synchronized()
// *****************************************************************
STDMETHODIMP CLabels::put_Synchronized(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal)
	{
		if (LabelsSynchronized())
			m_synchronized = VARIANT_TRUE;
		else
			ErrorMessage(tkLABELS_CANT_SYNCHRONIZE);
	}
	else
		m_synchronized = VARIANT_FALSE;

	return S_OK;
}
STDMETHODIMP CLabels::get_Synchronized(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// the propety must be set and the number of labels/parts must correspond
	if (LabelsSynchronized() && m_synchronized)
		*retval = VARIANT_TRUE;
	else
		*retval = VARIANT_FALSE;

	return S_OK;
}

// ****************************************************************
//	 Checking the synchronization
// ****************************************************************
bool CLabels::LabelsSynchronized()
{
	if (m_shapefile == NULL) return false;
	
	long numShapes;
	m_shapefile->get_NumShapes(&numShapes);
	if (numShapes != (long)m_labels.size())	return false;
	return true;
}
// *****************************************************************
//			OptionsAsCategory()
// *****************************************************************
STDMETHODIMP CLabels::get_Options(ILabelCategory** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*retVal = NULL;
	ILabelCategory* cat = NULL;
	CoCreateInstance( CLSID_LabelCategory, NULL, CLSCTX_INPROC_SERVER, IID_ILabelCategory, (void**)&cat);
	if (cat == NULL) return S_OK;

	((CLabelCategory*)cat)->put_LabelOptions(&m_options);

	cat->put_Name(A2BSTR("Default"));
	*retVal = cat;
	return S_OK;
}
STDMETHODIMP CLabels::put_Options(ILabelCategory* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CLabelCategory* coCat =	static_cast<CLabelCategory*>(newVal);
	CLabelOptions* options =  coCat->get_LabelOptions();
	m_options = *options;
	return S_OK;
}

// **********************************************************
//	  ClearLabelFrames()
// **********************************************************
// Clears bounds of labels that were drawn on screen (screen coordinates)
// Must be called before the redraw (before the call of MapView::DrawLabelsAlt).
void CLabels::ClearLabelFrames()
{
    for (int iLabel = 0; iLabel < (int)m_labels.size(); iLabel++)
	{
		vector<CLabelInfo*>* parts = m_labels[iLabel];
		for (int j =0; j < (int)parts->size(); j++ )
		{
			CLabelInfo* lbl = parts->at(j);
			if (lbl->horizontalFrame) 
			{
				delete lbl->horizontalFrame;	
				lbl->horizontalFrame = NULL;
			}
			if (lbl->rotatedFrame) 
			{
				delete lbl->rotatedFrame;
				lbl->rotatedFrame = NULL;
			}
			lbl->isDrawn = VARIANT_FALSE;
		}
	}
}

// **********************************************************
//		get/put_VisibilityExpression()
// **********************************************************
STDMETHODIMP CLabels::get_VisibilityExpression(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(m_expression);
	return S_OK;
}
STDMETHODIMP CLabels::put_VisibilityExpression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(m_expression);
	USES_CONVERSION;
	m_expression = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//		get/put_MinDrawingSize()
// **********************************************************
STDMETHODIMP CLabels::get_MinDrawingSize(LONG* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = m_minDrawingSize;
	return S_OK;
}
STDMETHODIMP CLabels::put_MinDrawingSize(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_minDrawingSize = newVal;
	return S_OK;
}

// ***************************************************************
//		MoveUp
// ***************************************************************
STDMETHODIMP CLabels::MoveCategoryUp (long Index, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < (long)m_categories.size() && Index > 0)
	{
		ILabelCategory* catBefore = m_categories[Index - 1];
		m_categories[Index - 1] = m_categories[Index];
		m_categories[Index] = catBefore;
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
STDMETHODIMP CLabels::MoveCategoryDown (long Index, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < (long)m_categories.size() - 1 && Index >= 0)
	{
		ILabelCategory* catAfter = m_categories[Index + 1];
		m_categories[Index + 1] = m_categories[Index];
		m_categories[Index] = catAfter;
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
//		get_AutoOffset
// ***************************************************************
STDMETHODIMP CLabels::get_AutoOffset(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = m_autoOffset;
	return S_OK;
}
STDMETHODIMP CLabels::put_AutoOffset(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_autoOffset = newVal;
	return S_OK;
}

// ********************************************************
//     Serialize()
// ********************************************************
 CPLXMLNode* CLabels::SerializeCore(CString ElementName)
{
	USES_CONVERSION;
	CString str;
	
	// drawing options
	CPLXMLNode* psTree = m_options.SerializeToTree(ElementName);
	if (!psTree)
	{
		return NULL;
	}
	
	// adding categories if any
	if (m_categories.size() > 0)
	{
		CPLXMLNode* psCategories = CPLCreateXMLNode(psTree, CXT_Element, "LabelCategories");
		for (unsigned int i = 0; i < m_categories.size(); i++)
		{
			CPLXMLNode* node = ((CLabelCategory*)m_categories[i])->SerializeCore("LabelCategoryClass");
			if (node)
			{
				CPLAddXMLChild(psCategories, node);
			}
		}
	}

	// labels
	if (m_labels.size() > 0)  		// positions should be saved only if there are labels
	{
		if (m_savingMode == modeStandard)
		{
			bool saveRotation, saveText;
			CPLXMLNode* nodeLabels = this->SerializeLabelData("Labels", saveRotation, saveText );
			
			Utility::CPLCreateXMLAttributeAndValue( psTree, "RotationSaved", CPLString().Printf("%d", (int)saveRotation));
			Utility::CPLCreateXMLAttributeAndValue( psTree, "TextSaved", CPLString().Printf("%d", (int)(saveText)));
			
			if (nodeLabels)
			{
				CPLAddXMLChild(psTree, nodeLabels);
			}
		}
		else if (m_savingMode == modeXML || m_savingMode == modeXMLOverwrite)
		{
			if (m_shapefile)
			{
				tkShapefileSourceType sourceType;
				m_shapefile->get_SourceType(&sourceType);
				if (sourceType == sstDiskBased)
				{
					// constructing the name of .lbl file
					CComBSTR name;
					m_shapefile->get_Filename(&name);
					CString path = Utility::GetPathWOExtension(OLE2CA(name));
					path += ".lbl";
					
					if (m_savingMode == modeXMLOverwrite && Utility::FileExists(path))
					{
						if( remove( path ) != 0 )
						{
							ErrorMessage(tkCANT_DELETE_FILE);
						}
					}

					// serialize only if there is no file, it's a responsibility 
					// of user to keep this file updated
					if (!Utility::FileExists(path))
					{
						VARIANT_BOOL retVal;
						SaveToXML(A2BSTR(path), &retVal);

						// user will need to save modeXMLOverwrite once more to overwrite the file
						if (m_savingMode == modeXMLOverwrite)
							m_savingMode = modeXML;
					}
				}
			}
		}
		else if (m_savingMode == modeDBF)
		{
			ErrorMessage(tkMETHOD_NOT_IMPLEMENTED);
		}
	}

	Utility::CPLCreateXMLAttributeAndValue( psTree, "Generated", CPLString().Printf("%d", m_labels.size() > 0 ));
	
	// start labels specific options
	if (m_minDrawingSize != 1)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "MinDrawingSize", CPLString().Printf("%d", m_minDrawingSize ));

	str = OLE2CA(m_expression);
	if (str != "")
		Utility::CPLCreateXMLAttributeAndValue( psTree, "VisibilityExpression", str);
	
	str = OLE2CA(m_key);
	if (str != "")
		Utility::CPLCreateXMLAttributeAndValue( psTree, "Key", str);

	if (m_verticalPosition != vpAboveAllLayers)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "VerticalPosition", CPLString().Printf("%d", (int)m_verticalPosition));

	if (m_basicScale != 0.0)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "BasicScale", CPLString().Printf("%f", m_basicScale));

	if (m_maxVisibleScale != 100000000.0)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "MaxVisibleScale", CPLString().Printf("%f", m_maxVisibleScale));
	
	if (m_minVisibleScale != 0.0)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "MinVisibleScale", CPLString().Printf("%f", m_minVisibleScale));

	if (m_collisionBuffer != 0)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "CollisionBuffer", CPLString().Printf("%d", m_collisionBuffer));

	if (m_scale != false)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "ScaleLabels", CPLString().Printf("%d", (int)m_scale));

	if (m_autoOffset != VARIANT_TRUE)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "AutoOffset", CPLString().Printf("%d", (int)m_autoOffset));

	if (m_dynamicVisibility != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "DynamicVisibility", CPLString().Printf("%d", (int)m_dynamicVisibility));

	if (m_globalSettings.labelsCollisionMode == tkCollisionMode::AllowCollisions)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "AvoidCollisions", CPLString().Printf("%d", (int)m_avoidCollisions));

	if (m_useWidthLimits != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "UseWidthLimits", CPLString().Printf("%d", (int)m_useWidthLimits));

	if (m_removeDuplicates != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "RemoveDuplicates", CPLString().Printf("%d", (int)m_removeDuplicates));

	if (m_gdiPlus != VARIANT_TRUE)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "UseGdiPlus", CPLString().Printf("%d", (int)m_gdiPlus));

	if (m_savingMode != modeXML)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "SavingMode", CPLString().Printf("%d", (int)m_savingMode));

	if (m_positioning != lpNone)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "Positioning", CPLString().Printf("%d", (int)m_positioning));

	if (m_textRenderingHint != HintAntiAlias)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "TextRenderingHint", CPLString().Printf("%d", (int)m_textRenderingHint));

	if (m_labelExpression != "")
		Utility::CPLCreateXMLAttributeAndValue( psTree, "Expression", m_labelExpression);

	if (m_sourceField != -1)
		Utility::CPLCreateXMLAttributeAndValue( psTree, "SourceField",CPLString().Printf("%d", m_sourceField));

	return psTree;
}

// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CLabels::Serialize(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CPLXMLNode* psTree =this->SerializeCore("LabelsClass");
	Utility::SerializeAndDestroyXmlTree(psTree, retVal);
	return S_OK;
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CLabels::DeserializeCore(CPLXMLNode* node)
{
	if (!node)
		return false;
		
	CString s;
	s = CPLGetXMLValue( node, "SavingMode", NULL );
	m_savingMode = (s != "") ? (tkSavingMode)atoi(s.GetString()) : modeXML;

	// restoring categories
	this->ClearCategories();
	
	if (m_savingMode != modeNone)
	{
		this->Clear();
	}

	CPLXMLNode* nodeCategories = CPLGetXMLNode( node, "LabelCategories" );
	if (nodeCategories)
	{
		nodeCategories = nodeCategories->psChild;
		while (nodeCategories)
		{
			if (strcmp(nodeCategories->pszValue, "LabelCategoryClass") == 0)
			{
				ILabelCategory* cat = NULL;
				this->AddCategory(A2BSTR(""), &cat);
				((CLabelCategory*)cat)->DeserializeFromNode(nodeCategories);
				cat->Release();
			}
			nodeCategories = nodeCategories->psNext;
		} 
	}

	s = CPLGetXMLValue( node, "Positioning", NULL );
	if (s != "") m_positioning =  (tkLabelPositioning)atoi(s.GetString());

	bool inMemory = false;
	if (m_shapefile)
	{
		tkShapefileSourceType sourceType;
		m_shapefile->get_SourceType(&sourceType);
		inMemory = sourceType != sstDiskBased;
	}

	CString expression = CPLGetXMLValue(node, "Expression", "");
	expression.Replace("&#xA;", "\r\n");

	// restoring labels
	s = CPLGetXMLValue( node, "Generated", NULL );
	bool loadLabels = atoi(s.GetString()) == 0 ? false : true;
	if (loadLabels)
	{
		bool xmlMode = m_savingMode == modeXML || m_savingMode == modeXMLOverwrite;

		if ((xmlMode && inMemory) ||  m_savingMode == modeNone)
		{
			long count;
			s = CPLGetXMLValue( node, "SourceField", NULL );
			if (s != "") 
			{
				m_sourceField = atoi(s.GetString()) ? true : false;
				if (m_sourceField != -1)
				{
					m_shapefile->GenerateLabels(m_sourceField, m_positioning, true, &count);
				}
			}
			else {
				if (expression.GetLength() != 0)
				{
					m_shapefile->GenerateLabels(-1, m_positioning, true, &count);
				}
			}
		}
		else if (m_savingMode == modeStandard)
		{
			bool loadText, loadRotation;
			s = CPLGetXMLValue( node, "RotationSaved", NULL );
			if (s != "") loadRotation = atoi(s.GetString()) ? true : false;

			s = CPLGetXMLValue( node, "TextSaved", NULL );
			if (s != "") loadText = atoi(s.GetString()) ? true : false;
			
			CPLXMLNode* nodeData = CPLGetXMLNode( node, "Labels" );
			if (nodeData)
			{
				DeserializeLabelData(nodeData, loadRotation, loadText);
			}
		}
		else if (m_savingMode == modeXML || m_savingMode == modeXMLOverwrite)
		{
			// constructing the name of .lbl file
			CComBSTR name;
			m_shapefile->get_Filename(&name);
			USES_CONVERSION;
			CStringW path = Utility::GetPathWOExtension(OLE2W(name));
			path += L".lbl";
					
			// restoring labels
			if (Utility::FileExistsW(path))
			{
				VARIANT_BOOL retVal;
				CComBSTR bstrPath(path);
				this->LoadFromXML(bstrPath, &retVal);
			}
		}
	}
	
	// properties should be re read after labels were restored,
	// as old .lbl file have their own properties, and we don't want to overwrite project information
	m_options.DeserializeFromNode(node);
	
	// start labels specific options
	s = CPLGetXMLValue( node, "MinDrawingSize", NULL );
	m_minDrawingSize = (s != "") ? atoi(s.GetString()) : 1;

	s = CPLGetXMLValue( node, "VisibilityExpression", NULL );

	USES_CONVERSION;
	CComBSTR bstrExpression = A2W(s);
	this->put_VisibilityExpression(bstrExpression);

	/*s = CPLGetXMLValue( node, "ClassificationField", NULL );
	m_classificationField = (s != "") ? (long)atoi(s.GetString()) : -1;*/

	s = CPLGetXMLValue( node, "Key", NULL );
	CComBSTR bstrKey = A2W(s);
	this->put_Key(bstrKey);
	
	s = CPLGetXMLValue( node, "VerticalPosition", NULL );
	m_verticalPosition = (s != "") ? (tkVerticalPosition)atoi(s.GetString()) : vpAboveAllLayers;

	s = CPLGetXMLValue( node, "BasicScale", NULL );
	m_basicScale = (s != "") ? Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue( node, "MaxVisibleScale", NULL );
	m_maxVisibleScale = (s != "") ? Utility::atof_custom(s) : 100000000.0;

	s = CPLGetXMLValue( node, "MinVisibleScale", NULL );
	m_minVisibleScale = (s != "") ? Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue( node, "CollisionBuffer", NULL );
	m_collisionBuffer = (s != "") ? atoi(s.GetString()) : 0;

	s = CPLGetXMLValue( node, "ScaleLabels", NULL );
	m_scale = (s != "") ? (atoi(s.GetString()) == 0? false : true) : false;

	s = CPLGetXMLValue( node, "AutoOffset", NULL );
	m_autoOffset = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_TRUE;

	s = CPLGetXMLValue( node, "DynamicVisibility", NULL );
	m_dynamicVisibility = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;

	s = CPLGetXMLValue( node, "AvoidCollisions", NULL );
	m_avoidCollisions = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_TRUE;
	
	s = CPLGetXMLValue( node, "UseWidthLimits", NULL );
	m_useWidthLimits = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;
	
	s = CPLGetXMLValue( node, "RemoveDuplicates", NULL );
	m_removeDuplicates = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;

	s = CPLGetXMLValue( node, "UseGdiPlus", NULL );
	m_gdiPlus = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_TRUE;
	
	s = CPLGetXMLValue( node, "TextRenderingHint", NULL );
	m_textRenderingHint = (s != "") ? (tkTextRenderingHint)atoi(s.GetString()) : HintAntiAlias;
	
	// applying the expressions
	if (m_categories.size() > 0)
	{
		this->ApplyCategories();
	}
	
	// applying expression; should be done after label generation
	CComBSTR bstr = A2W(expression);
	this->put_Expression(bstr);

	return true;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CLabels::Deserialize(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	CString s = OLE2CA(newVal);
	CPLXMLNode* node = CPLParseXMLString(s.GetString());
	if (node)
	{
		CPLXMLNode* nodeLabels = CPLGetXMLNode(node, "=LabelsClass");
		if (nodeLabels)
		{
			DeserializeCore(nodeLabels);
		}
		CPLDestroyXMLNode(node);
	}
	return S_OK;
}

// ********************************************************
//     get_Expression()
// ********************************************************
STDMETHODIMP CLabels::get_Expression(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = A2BSTR(m_labelExpression.GetString());
	return S_OK;
}

STDMETHODIMP CLabels::put_Expression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if (!m_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return S_OK;
	}
	
	CString str = OLE2CA(newVal);
	m_labelExpression = str;

	if (!m_synchronized)
	{
		ErrorMessage(tkLABELS_NOT_SYNCHRONIZE);
	}
	else
	{
		ITable* table = NULL;
		m_shapefile->get_Table(&table);
		if (table)
		{
			// analyzes expression
			CString strError;
			std::vector<CString> results;
			if (((CTableClass*)table)->Calculate_(str, results, strError))
			{
				// updating labels
				if (results.size() == m_labels.size())
				{
					for (unsigned int i = 0; i < m_labels.size(); i++)
					{
						(*m_labels[i])[0]->text = results[i];
					}
				}
			}
			else
			{
				if (str == "")
				{
					for (unsigned int i = 0; i < m_labels.size(); i++)
					{
						(*m_labels[i])[0]->text = "";
					}
				}
				else
				{
					ErrorMessage(tkINVALID_EXPRESSION);
				}
			}
			table->Release();
		}
	}
	return S_OK;
}

// ********************************************************
//     SaveToXML()
// ********************************************************
STDMETHODIMP CLabels::SaveToXML(BSTR filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CStringW s = OLE2W(filename);
	if (s.GetLength() < 5)
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_FALSE;
	}

	if (s.Right(4).MakeLower() != L".lbl")
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_FALSE;
	}
	
	CPLXMLNode *psTree = CPLCreateXMLNode( NULL, CXT_Element, "MapWindow" );
	if (psTree)
	{
		Utility::WriteXmlHeaderAttributes( psTree, "Labels");

		bool saveText, saveRotation;
		CPLXMLNode* node = SerializeLabelData("Labels", saveRotation, saveText);

		if (node)
		{
			Utility::CPLCreateXMLAttributeAndValue( node, "Count", CPLString().Printf("%d", (int)m_labels.size()));
			Utility::CPLCreateXMLAttributeAndValue( node, "RotationSaved", CPLString().Printf("%d", (int)saveRotation));
			Utility::CPLCreateXMLAttributeAndValue( node, "TextSaved", CPLString().Printf("%d", (int)(saveText)));

			CPLAddXMLChild(psTree, node);
			*retVal = GdalHelper::SerializeXMLTreeToFile( psTree, s );
			CPLDestroyXMLNode(psTree);
		}
	}
	return S_OK;
}

// ********************************************************
//		SerializeLabelData()
// ********************************************************
CPLXMLNode* CLabels::SerializeLabelData(CString ElementName, bool& saveRotation, bool& saveText)
{
	saveRotation = false;
	saveText = m_labelExpression == "";
	
	CPLXMLNode* psLabels = CPLCreateXMLNode( NULL, CXT_Element, ElementName );
	if (psLabels)
	{
		for (unsigned int i = 0; i < m_labels.size(); i++)
		{
			CLabelInfo* info = (*m_labels[i])[0];
			if (info->rotation != 0.0)
			{
				saveRotation = true;
				break;
			}
		}
		
		CPLXMLNode* nodeOld = NULL;
		CPLXMLNode* nodeNew = NULL;

		for (unsigned int i = 0; i < m_labels.size(); i++)
		{
			if (!nodeOld)
			{
				nodeOld = CPLCreateXMLNode(psLabels, CXT_Element, "Label");
			}
			else
			{
				nodeNew = CPLCreateXMLNode(NULL, CXT_Element, "Label");
				CPLAddXMLSibling(nodeOld, nodeNew);
				nodeOld = nodeNew;
			}
			
			CLabelInfo* info = (*m_labels[i])[0];
			Utility::CPLCreateXMLAttributeAndValue(nodeOld, "X", CPLString().Printf("%f", info->x));
			Utility::CPLCreateXMLAttributeAndValue(nodeOld, "Y", CPLString().Printf("%f", info->y));
			if (saveRotation)
			{
				Utility::CPLCreateXMLAttributeAndValue(nodeOld, "Rotation", CPLString().Printf("%f", info->rotation));
			}
			if (saveText)
			{
				Utility::CPLCreateXMLAttributeAndValue(nodeOld, "Name", info->text);
			}
		}
	}
	return psLabels;
}

// ********************************************************
//     LoadLblOptions()
// ********************************************************
// Loads options from the old-style .lbl file
void CLabels::LoadLblOptions(CPLXMLNode* node)
{
   USES_CONVERSION;
	
	try
	{
		CString s = CPLGetXMLValue( node, "Font", NULL );
		if (s != "") m_options.fontName = A2BSTR(s);

		s = CPLGetXMLValue( node, "Size", NULL );
		if (s != "") m_options.fontSize = (int)Utility::atof_custom(s);

		s = CPLGetXMLValue( node, "Color", NULL );
		if (s != "") m_options.fontColor = (OLE_COLOR)(atoi(s) + pow(2.0, 24.0));

		s = CPLGetXMLValue( node, "Scaled", NULL );
		if (s != "") m_scale = atoi(s) == 0? false : true;

		s = CPLGetXMLValue( node, "Scale", NULL );
		if (s != "") m_basicScale = Utility::atof_custom(s);

		s = CPLGetXMLValue( node, "UseShadows", NULL );
		if (s != "") m_options.shadowVisible = atoi(s) == 0 ? false : true;

		s = CPLGetXMLValue( node, "ShadowColor", NULL );
		if (s != "") m_options.shadowColor = (OLE_COLOR)(atoi(s) + pow(2.0, 24.0));

		s = CPLGetXMLValue( node, "UseLabelCollision", NULL );
		if (s != "") m_avoidCollisions = atoi(s) == 0 ? false : true;

		s = CPLGetXMLValue( node, "Bold", NULL );
		if (_stricmp(s, "true") == 0)
			m_options.fontStyle |= fstBold;

		s = CPLGetXMLValue( node, "Italic", NULL );
		if (_stricmp(s, "true") == 0)
			m_options.fontStyle |= fstItalic;

		s = CPLGetXMLValue( node, "Underline", NULL );
		if (_stricmp(s, "true") == 0)
			m_options.fontStyle |= fstUnderline;
	}
	catch(...)
	{
		this->ErrorMessage(tkINVALID_FILE);
	}
	// can be mapped to the new properties
	//Justification="1" 
}

// ********************************************************
//     LoadFromXML()
// ********************************************************
STDMETHODIMP CLabels::LoadFromXML(BSTR filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	
	USES_CONVERSION;
	CStringW name = OLE2W(filename);
	if (!Utility::FileExistsW(name))
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}

	
	CPLXMLNode* root = GdalHelper::ParseXMLFile(name); // CPLParseXMLFile(name.GetString());
	if (root)
	{
		CPLXMLNode* node = CPLGetXMLNode(root, "=MapWindow");
		if (node)
		{
			CString s = CPLGetXMLValue( node, "FileVersion", "0" );
			int version = atoi(s);

			node = CPLGetXMLNode(node, "Labels");
			if (node)
			{
				bool loadText = true, loadRotation = true;
				
				// loading options from the old format
				if (version < 1)
				{
					// old format
					CLabelOptions opt;
					m_options = opt;
					this->LoadLblOptions(node);
				}
				else
				{
					// new format
					s = CPLGetXMLValue( node, "RotationSaved", NULL );
					if (s != "") loadRotation = atoi(s.GetString()) ? true : false;

					s = CPLGetXMLValue( node, "TextSaved", NULL );
					if (s != "") loadText = atoi(s.GetString()) ? true : false;
				}
				DeserializeLabelData(node, loadRotation, loadText);
			}
		}
		CPLDestroyXMLNode(root);
	}
	return S_OK;
}

// ********************************************************
//     DeserializeLabelData()
// ********************************************************
bool CLabels::DeserializeLabelData(CPLXMLNode* node, bool loadRotation, bool loadText)
{
	if (!node) return false;
	
	this->Clear();
	CComBSTR bstrDefault("");
	CString s;
	CString text;
	double x,y, angle;
	node = CPLGetXMLNode(node, "Label");
		
	while (node)
	{
		s = CPLGetXMLValue(node, "X", "0.0");
		x = Utility::atof_custom(s);

		s = CPLGetXMLValue(node, "Y", "0.0");
		y = Utility::atof_custom(s);

		CComBSTR bstr;
		if (loadText)
		{
			text = CPLGetXMLValue(node, "Name", "");
			bstr.Attach(A2BSTR(text));
		}
			
		if (loadRotation)
		{
			s = CPLGetXMLValue(node, "Rotation", "0.0");
			angle = Utility::atof_custom(s);
		}
		else
		{
			angle = 0.0;
		}
		
		this->AddLabel(loadText ?  bstr : bstrDefault, x, y, angle);
		node = node->psNext;
	}
		
	// let's try to sync
	this->put_Synchronized(VARIANT_TRUE);
		
	return true;
}

#pragma region "DbfSerialization"

// ********************************************************
//     SaveLabels()
// ********************************************************
STDMETHODIMP CLabels::SaveToDbf(VARIANT_BOOL saveText, VARIANT_BOOL saveCategory, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	SaveToDbf2(A2BSTR("_LabelX"), A2BSTR("_LabelY"), A2BSTR("_LabelAngle"), A2BSTR("_LabelText"), A2BSTR("_LabelCategory"), saveText, saveCategory, retVal);
	return S_OK;
}
STDMETHODIMP CLabels::SaveToDbf2(BSTR xField, BSTR yField, BSTR angleField, BSTR textField, BSTR categoryField,  
								 VARIANT_BOOL saveText, VARIANT_BOOL saveCategory, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	if (!m_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return S_OK;
	}

	this->get_Synchronized(retVal);
	if (!(*retVal))
	{
		ErrorMessage(tkLABELS_NOT_SYNCHRONIZE);
		return S_OK;
	}

	tkShapefileSourceType sourceType;
	m_shapefile->get_SourceType(&sourceType);
	if (sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_OK;
	}
		
	// extracting table
	ITable* dbf = NULL;
	m_shapefile->get_Table(&dbf);
	
	// opening editing session
	VARIANT_BOOL editing;
	dbf->get_EditingTable(&editing);
	if (!editing)
	{
		dbf->StartEditingTable(NULL, retVal);
	}

	dbf->get_EditingTable(retVal);
	if (!(*retVal))
	{
		long errorCode;
		dbf->get_LastErrorCode(&errorCode);
		dbf->Release();
		ErrorMessage(errorCode);
		return S_OK;
	}
	
	// angles are saved only in case there is at least one label with non-zero angle
	bool saveAngle = false;
	for (unsigned int i = 0; i < m_labels.size(); i++)
	{
		CLabelInfo* info = (*(m_labels)[i])[0];
		if (info->rotation != 0.0)
		{
			saveAngle = true;
		}
		if (saveAngle)
			break;
	}

	// checking fields, creating if necessary
	BSTR fields[5];
	fields[0] = xField;
	fields[1] = yField;
	fields[2] = angleField;
	fields[3] = textField;
	fields[4] = categoryField;
	
	for (int i = 0; i < 5; i++)
	{
		if (i == 2 && !saveAngle)
			continue;

		if (i == 3 && !saveText)
			continue;

		if (i == 4 && !saveCategory)
			continue;
		
		IField* field = NULL;
		m_shapefile->get_FieldByName(fields[i], &field);
		if (!field)
		{
			// creating fields
			CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&field);
			if (field)
			{
				field->put_Name(fields[i]);
				FieldType type = i < 3 ? DOUBLE_FIELD: (i == 3 ? STRING_FIELD : INTEGER_FIELD);
				field->put_Type(type);
				
				long numFields;
				m_shapefile->get_NumFields(&numFields);
				m_shapefile->EditInsertField(field, &numFields, NULL, retVal);		// not checking the result here; nothing can occur inside 
																			//and we check the editing state previously
			}
		}
		if (field)
		{
			field->Release();
		}
	}
	
	// finding fields
	long indices[5];
	for (int i = 0; i < 5; i++)
	{
		dbf->get_FieldIndexByName(fields[i], &(indices[i]));
	}
	
	// saving properties
	long count;
	this->get_Count(&count);
	CComVariant var;
	VARIANT_BOOL vbretval;
	USES_CONVERSION;

	for (int i = 0; i < count; i++)
	{
		CLabelInfo* info = (*(m_labels)[i])[0];

		// X
		var.vt = VT_R8;
		var.dblVal = info->x;
		dbf->EditCellValue(indices[0], i, var, &vbretval);

		// Y
		var.vt = VT_R8;
		var.dblVal = info->y;
		dbf->EditCellValue(indices[1], i, var, &vbretval);

		// Rotation
		if (saveAngle)
		{
			var.vt = VT_R8;
			var.dblVal = info->rotation;
			dbf->EditCellValue(indices[2], i, var, &vbretval);
		}

		// Text
		if (saveText)
		{
			var.vt = VT_BSTR;
			var.bstrVal = A2BSTR(info->text);
			dbf->EditCellValue(indices[3], i, var, &vbretval);
		}

		// Category
		if (saveCategory)
		{
			var.vt = VT_I4;
			var.dblVal = info->category;
			dbf->EditCellValue(indices[4], i, var, &vbretval);
		}
	}

	// in case the table wasn't in the editing state initially, it should be saved
	if (!editing)
	{
		dbf->StopEditingTable(VARIANT_TRUE, NULL, retVal);
		if (!(*retVal))
		{
			// exit without saving
			long errorCode;
			dbf->get_LastErrorCode(&errorCode);
			ErrorMessage(errorCode);
			dbf->StopEditingTable(VARIANT_FALSE, NULL, retVal);
			*retVal = VARIANT_FALSE;
		}
	}

	dbf->Release();
	return S_OK;
}

// ********************************************************
//     LoadLabels()
// ********************************************************
STDMETHODIMP CLabels::LoadFromDbf(VARIANT_BOOL loadText, VARIANT_BOOL loadCategory, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	LoadFromDbf2(A2BSTR("_LabelX"), A2BSTR("_LabelY"), A2BSTR("_LabelAngle"), A2BSTR("_LabelText"), A2BSTR("_LabelCategory"), loadText, loadCategory, retVal);
	return S_OK;
}

STDMETHODIMP CLabels::LoadFromDbf2(BSTR xField, BSTR yField, BSTR angleField, BSTR textField, BSTR categoryField, 
									 VARIANT_BOOL loadText, VARIANT_BOOL loadCategory, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (!m_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return S_OK;
	}

	tkShapefileSourceType sourceType;
	m_shapefile->get_SourceType(&sourceType);
	if (sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_OK;
	}

	ITable* dbf = NULL;
	m_shapefile->get_Table(&dbf);
	if (!dbf)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_OK;
	}

	// seeking fields
	BSTR fields[5];
	USES_CONVERSION;
	fields[0] = xField;
	fields[1] = yField;
	fields[2] = angleField;
	fields[3] = textField;
	fields[4] = categoryField;
	
	long indices[5];
	for (int i = 0; i < 4; i++)
	{
		dbf->get_FieldIndexByName(fields[i], &(indices[i]));

		// checking field type
		IField* field = NULL;
		dbf->get_Field(indices[i], &field);
		if (field)
		{
			FieldType type;
			field->get_Type(&type);
			if ((i < 3 && type != DOUBLE_FIELD) ||
				(i == 3 && type != STRING_FIELD) ||
				(i == 4 && type != INTEGER_FIELD) )
			{
				indices[i] = -1;
			}
			field->Release();
		}
	}

	// at least X, Y position should be saved
	if (indices[0] == -1 || indices[1] == -1)	
	{
		ErrorMessage(tkLABELS_NOT_SAVED);
		dbf->Release();
		return S_OK;
	}

	bool loadAngle = (indices[2] != -1);
	loadText &= (indices[3] != -1);
	loadCategory &= (indices[4] != -1);

	// creating labels
	this->GenerateEmptyLabels();
	
	this->get_Synchronized(retVal);
	if (!(*retVal))
	{
		this->Clear();
		ErrorMessage(tkLABELS_NOT_SYNCHRONIZE);
		dbf->Release();
		return S_OK;
	}
	
	// fast reading of the values from table
	std::vector<double> xValues;
	((CTableClass*)dbf)->get_FieldValuesDouble((int)indices[0], xValues);

	std::vector<double> yValues;
	((CTableClass*)dbf)->get_FieldValuesDouble((int)indices[1], yValues);
	
	//angles
	std::vector<double> vAngles;
	if (loadAngle)
		((CTableClass*)dbf)->get_FieldValuesDouble((int)indices[2], vAngles);

	// text
	std::vector<CString> vText;
	if (loadText)
		((CTableClass*)dbf)->get_FieldValuesString((int)indices[3], vText);

	// category
	std::vector<int> vCategory;
	if (loadCategory)
		((CTableClass*)dbf)->get_FieldValuesInteger((int)indices[4], vCategory);

	long numShapes;
	m_shapefile->get_NumShapes(&numShapes);

	if (xValues.size() == yValues.size() && xValues.size() == numShapes)
	{
		for (int i = 0; i < numShapes; i++)
		{
			CLabelInfo* info =  (*(m_labels)[i])[0];
			info->x = xValues[i];
			info->y = yValues[i];
			
			if (loadAngle)
				info->rotation = vAngles[i];

			if (loadText)
				info->text = vText[i];

			if (loadCategory)
				info->category = vCategory[i];
		}

		// let's try to sync it
		this->put_Synchronized(VARIANT_TRUE);
	}

	*retVal = VARIANT_TRUE;
	return S_OK;
}
#pragma endregion

// ************************************************************
//			GenerateEmptyLabels
// ************************************************************
bool CLabels::GenerateEmptyLabels()
{
	if (!m_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return false;
	}

	this->Clear();
	
	long numShapes;
	m_shapefile->get_NumShapes(&numShapes);

	for(int i = 0; i < numShapes; i++)
	{
		CLabelInfo* info = new CLabelInfo();
		std::vector<CLabelInfo*>* parts = new std::vector<CLabelInfo*>;
		parts->push_back(info);
		m_labels.push_back(parts);
	}
	
	this->put_Synchronized(VARIANT_TRUE);
 	return true;
}

// ************************************************************
//			Generate
// ************************************************************
STDMETHODIMP CLabels::Generate(BSTR Expression, tkLabelPositioning Method, VARIANT_BOOL LargestPartOnly, long* Count)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!m_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return false;
	}

	tkShapefileSourceType sourceType;
	m_shapefile->get_SourceType(&sourceType);
	if (sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_OK;
	}

	ITable* table = NULL;
	m_shapefile->get_Table(&table);
	
	USES_CONVERSION;
	BSTR errorString = A2BSTR("");
	VARIANT_BOOL vbretval;
	((CTableClass*)table)->ParseExpressionCore(Expression, vtString, &errorString, &vbretval);
	table->Release();

	if (!vbretval)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	// generating without text
	m_shapefile->GenerateLabels(-1, Method, LargestPartOnly, Count);
	if (*Count > 0)
	{
		this->put_Expression(Expression);
	}
	return S_OK;
}

// *************************************************************
//		get_SavingMode()
// *************************************************************
STDMETHODIMP CLabels::get_SavingMode(tkSavingMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = m_savingMode;
	return S_OK;
}

// *************************************************************
//		put_SavingMode()
// *************************************************************
STDMETHODIMP CLabels::put_SavingMode(tkSavingMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_savingMode = newVal;
	return S_OK;
}

// *************************************************************
//		get_Positioning()
// *************************************************************
STDMETHODIMP CLabels::get_Positioning(tkLabelPositioning* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_positioning;
	return S_OK;
}

// *************************************************************
//		put_Positioning()
// *************************************************************
STDMETHODIMP CLabels::put_Positioning(tkLabelPositioning newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_shapefile)
	{
		ShpfileType type;
		m_shapefile->get_ShapefileType(&type);
		type = Utility::ShapeTypeConvert2D(type);

		bool polyline =false, polygon = false;
		switch(newVal)
		{
			case lpFirstSegment:
			case lpLastSegment:
			case lpMiddleSegment:
			case lpLongestSegement:
				polyline = true;
				break;
			case lpCenter:
			case lpCentroid:
			case lpInteriorPoint:
				polygon = true;
				break;
		}

		if ((type == SHP_POLYLINE && !polyline) ||
			(type == SHP_POLYGON && !polygon))
		{
			ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
			return S_FALSE;
		}
	}
	m_positioning = newVal;
	return S_OK;
}

// *************************************************************
//		TextRenderingHint()
// *************************************************************
STDMETHODIMP CLabels::get_TextRenderingHint(tkTextRenderingHint* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_textRenderingHint;
	return S_OK;
}

STDMETHODIMP CLabels::put_TextRenderingHint(tkTextRenderingHint newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0 && newVal <= ClearTypeGridFit)
		m_textRenderingHint = newVal;
	return S_OK;
}

// *************************************************************
//		HasRotation()
// *************************************************************
bool CLabels::HasRotation()
{
	for (unsigned int i = 0; i < m_labels.size(); i++)
	{
		vector<CLabelInfo*>* parts = m_labels[i];
		for (int j = 0; j < (int)parts->size(); j++)
		{
			CLabelInfo* lbl = (*parts)[j];
			if (lbl->rotation != 0.0)
				return true;
		}
	}
	return false;
}