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
#include "Templates.h"
#include "TableHelper.h"
#include "LabelsHelper.h"

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
	*retval = OLE2BSTR(_options->fontName);		
	return S_OK;
};	
STDMETHODIMP CLabels::put_FontName(BSTR newVal)					
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	::SysFreeString(_options->fontName);
	_options->fontName = OLE2BSTR(newVal);
	return S_OK;
};	

// *****************************************************************
//		Font/FrameTransparency()
// *****************************************************************
STDMETHODIMP CLabels::get_FontTransparency(long* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = _options->fontTransparency;		
	return S_OK;
};		
STDMETHODIMP CLabels::put_FontTransparency(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal < 0) newVal = 0;
	if (newVal > 255) newVal = 255;
	_options->fontTransparency = newVal;
	return S_OK;
};
STDMETHODIMP CLabels::get_FrameTransparency(long* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = _options->frameTransparency;		
	return S_OK;
};		
STDMETHODIMP CLabels::put_FrameTransparency(long newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (newVal < 0) newVal = 0;
	if (newVal > 255) newVal = 255;
	_options->frameTransparency = newVal;
	return S_OK;
};

// *****************************************************************
//			Font style options
// *****************************************************************
STDMETHODIMP CLabels::get_FontItalic(VARIANT_BOOL* retval)
{
	*retval = ((_options->fontStyle & fstItalic)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontItalic(VARIANT_BOOL newVal)
{
	if (newVal)		_options->fontStyle |= fstItalic;
	else			_options->fontStyle &= (0xFFFFFFFF ^ fstItalic);
	return S_OK;
};
STDMETHODIMP CLabels::get_FontBold(VARIANT_BOOL* retval)
{
	*retval = ((_options->fontStyle & fstBold)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontBold(VARIANT_BOOL newVal)
{
	if (newVal)		_options->fontStyle |= fstBold;
	else			_options->fontStyle &= (0xFFFFFFFF ^ fstBold);
	return S_OK;
};
STDMETHODIMP CLabels::get_FontUnderline(VARIANT_BOOL* retval)
{
	*retval = ((_options->fontStyle & fstUnderline)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontUnderline(VARIANT_BOOL newVal)
{
	if (newVal)		_options->fontStyle |= fstUnderline;
	else			_options->fontStyle &= (0xFFFFFFFF ^ fstUnderline);
	return S_OK;
};
STDMETHODIMP CLabels::get_FontStrikeOut(VARIANT_BOOL* retval)
{
	*retval = ((_options->fontStyle & fstStrikeout)?VARIANT_TRUE:VARIANT_FALSE);
	return S_OK;
};
STDMETHODIMP CLabels::put_FontStrikeOut(VARIANT_BOOL newVal)
{
	if (newVal)		_options->fontStyle |= fstStrikeout;
	else			_options->fontStyle &= (0xFFFFFFFF ^ fstStrikeout);
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
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CLabels::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	USES_CONVERSION;
	_key = OLE2BSTR(newVal);
	return S_OK;
}

//***********************************************************************/
//*			ErrorMessage()
//***********************************************************************/
inline void CLabels::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("Labels", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}
STDMETHODIMP CLabels::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
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
	*pVal = _globalCallback;
	if(_globalCallback != NULL ) 
	{
		_globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CLabels::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// ************************************************************
//		get_Label
// ************************************************************
STDMETHODIMP CLabels::get_Label(long Index, long Part, ILabel** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if(Index < 0 || Index >= (long)_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = NULL;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = _labels[Index];
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
	*pVal = _categories.size();
	return S_OK;
};

//***********************************************************************/
//*		get_numParts()
//***********************************************************************/
STDMETHODIMP CLabels::get_NumParts(long Index, long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if(Index < 0 || Index >= (long)_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = -1;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = _labels[Index];
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
		if(Index < 0 || Index >= (long)_categories.size())
		{
			ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		}
		else
		{
			*retval	= _categories[Index];
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
	
	if(Index < 0 || Index >= (long)_categories.size())
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
			ComHelper::SetRef(newVal, (IDispatch**)&_categories[Index], false);
		}
	}
	return S_OK;
};

//***********************************************************************/
//*		AddLabel()
//***********************************************************************/
STDMETHODIMP CLabels::AddLabel(BSTR Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL vbretval;
	this->InsertLabel(_labels.size(), Text, x, y, Rotation, Category, offsetX, offsetY, &vbretval);
	return S_OK;
}

//***********************************************************************/
//*		InsertLabel()
//***********************************************************************/
STDMETHODIMP CLabels::InsertLabel(long Index, BSTR Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if(Index < 0 || Index > (long)_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = VARIANT_FALSE;
	}
		
    CLabelInfo* lbl = CreateNewLabel(Text, x, y, Rotation, Category, offsetX, offsetY);

	std::vector<CLabelInfo*>* parts = new std::vector<CLabelInfo*>;
	parts->push_back(lbl);
	
	if (Index == _labels.size())
	{
		_labels.push_back(parts);
	}
	else
	{
		_labels.insert(_labels.begin() + _labels.size(), parts);
	}
	*retVal = VARIANT_TRUE;
	return S_OK;
}

//***********************************************************************/
/*		CreateNewLabel()
//***********************************************************************/
CLabelInfo* CLabels::CreateNewLabel(const BSTR &Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY)
{
    // TODO this really is just a constructor for label info => need to move this here ...
    CLabelInfo *lbl = new CLabelInfo();

    lbl->text = Text;
    lbl->x = x;
    lbl->y = y;
    lbl->offsetX = offsetX;
    lbl->offsetY = offsetY;
    lbl->rotation = Rotation;
    lbl->category = Category;

    return lbl;
}
//***********************************************************************/
/*		RemoveLabel()
//***********************************************************************/
STDMETHODIMP CLabels::RemoveLabel(long Index, VARIANT_BOOL* vbretval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if(Index < 0 || Index >= (long)_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*vbretval = VARIANT_FALSE;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = _labels[Index];
		for(unsigned int i=0; i < parts->size(); i++)
		{
			delete parts->at(i);
		}
		parts->clear();
		_labels.erase(_labels.begin() + Index);
		*vbretval = VARIANT_TRUE;
	}
	return S_OK;
};

///***********************************************************************/
///*		AddPart()
///***********************************************************************/
STDMETHODIMP CLabels::AddPart(long Index, BSTR Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if(Index < 0 || Index > (int)_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	} 
	std::vector<CLabelInfo*>* parts = _labels[Index];
	
	VARIANT_BOOL vbretval;
	InsertPart(Index, parts->size(), Text, x, y, Rotation, Category, offsetX, offsetY, &vbretval);
	return S_OK;
};

///***********************************************************************/
///*		AddPart()
///***********************************************************************/
STDMETHODIMP CLabels::InsertPart(long Index, long Part, BSTR Text, double x, double y, double Rotation, long Category, double offsetX, double offsetY, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if(Index < 0 || Index >=(int)_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	} 
	std::vector<CLabelInfo*>* parts = _labels[Index];

    CLabelInfo* lbl = CreateNewLabel(Text, x, y, Rotation, Category, offsetX, offsetY);
	
	if (Part == parts->size())
	{
		parts->push_back(lbl);
		*retVal = VARIANT_TRUE;
	}
	else
	{
		parts->insert(parts->begin() + parts->size(), lbl);
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
	
	if(Index < 0 || Index >= (long)_labels.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*vbretval = VARIANT_FALSE;
	}
	else
	{
		std::vector<CLabelInfo*>* parts = _labels[Index];
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
	this->InsertCategory(_categories.size(), Name, retVal);
	return S_OK;
}

// *****************************************************************
//			InsertCategory()
// *****************************************************************
STDMETHODIMP CLabels::InsertCategory(long Index, BSTR Name, ILabelCategory** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if(Index < 0 || Index > (long)_categories.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = NULL;
		return NULL;
	}
	
	*retVal = NULL;
	ILabelCategory* cat = NULL;
	CoCreateInstance(CLSID_LabelCategory, NULL, CLSCTX_INPROC_SERVER, IID_ILabelCategory, (void**)&cat);
	if (cat == NULL) return S_OK;

	CLabelOptions* options = this->get_LabelOptions(); 
	((CLabelCategory*)cat)->put_LabelOptions(options);
	cat->put_Name(Name);
	
	if (Index == _categories.size())
	{
		_categories.push_back(cat);
	}
	else
	{
		_categories.insert(_categories.begin() + Index, cat);
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

	if(Index < 0 || Index >= (long)_categories.size())
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

//***********************************************************************/
//*		ClearAllLabels()
//***********************************************************************/
STDMETHODIMP CLabels::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	for(size_t i = 0; i < _labels.size(); i++)
	{	
		for(size_t j = 0; j < _labels[i]->size(); j++)
		{
			delete _labels[i]->at(j);
		}
		delete _labels[i];
	}
	_labels.clear();
	return S_OK;
};

// *****************************************************************
//			ClearAllCategories()
// *****************************************************************
STDMETHODIMP CLabels::ClearCategories()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	for (unsigned int i=0; i < _categories.size(); i++)
	{
		_categories[i]->Release();
	}
	_categories.clear();
	
	for (unsigned  int i = 0; i < _labels.size(); i++)
	{
		(*_labels[i])[0]->category = -1;
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
	ApplyColorScheme3(Type, ColorScheme, Element, 0, _categories.size() - 1);
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
		if (_options->frameVisible)
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
	if (CategoryEndIndex >= (long)_categories.size())
		CategoryEndIndex = (long)(_categories.size() - 1);

	if (CategoryStartIndex < 0)
		CategoryStartIndex = 0;

	if (CategoryEndIndex == CategoryStartIndex)
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
		if (Type == ctSchemeRandom)			ColorScheme->get_RandomColor(value, &color);
		else if (Type == ctSchemeGraduated)	ColorScheme->get_GraduatedColor(value, &color);
		
		switch (Element)
		{
			case leFrameBackground:		
				_categories[i]->put_FrameBackColor(color);
				break;
			case leFrameOutline:		
				_categories[i]->put_FrameOutlineColor(color);
				break;
			case leFont:		
				_categories[i]->put_FontColor(color);
				break;
			case leFontOutline :		
				_categories[i]->put_FontOutlineColor(color);
				break;
			case leShadow:		
				_categories[i]->put_ShadowColor(color);
				break;
			case leHalo:		
				_categories[i]->put_HaloColor(color);
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
	for(unsigned int i = 0; i < _categories.size(); i++)
	{
		_categories[i]->get_MinValue(&vMin);
		_categories[i]->get_MaxValue(&vMax);

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

	for (unsigned long i = 0; i < _labels.size(); i++)
	{
		vector<CLabelInfo*>* labelParts = _labels[i];
		for (unsigned long j =0; j < labelParts->size(); j++)
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
								ComHelper::CreateShape(&shp);
								shp->put_ShapeType(SHP_POLYGON);
								long part = 0;
								shp->InsertPart(0, &part, &vbretval);
								IPoint* pnt = NULL;
								for (long n =0; n < 5; n++)
								{
									int k = n;
									if (k == 4) k = 0;

									ComHelper::CreatePoint(&pnt);			
									pnt->put_X(double(frame->points[k].x));
									pnt->put_Y(double(frame->points[k].y));
									shp->InsertPoint(pnt,&n,&vbretval);
									pnt->Release();
								}

								ComHelper::CreatePoint(&pnt);
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
	_shapefile = newVal;
}
IShapefile* CLabels::get_ParentShapefile()
{
	return _shapefile;
}

// *******************************************************************
//	Passing data inside ocx
// *******************************************************************
std::vector<std::vector<CLabelInfo*>*>* CLabels::get_LabelData()
{
	//return reinterpret_cast<char*>(&m_labels);
	return &_labels;
}
CLabelOptions* CLabels::get_LabelOptions()
{
	return _options;
}

// *****************************************************************
//	    GenerateCategories()
// *****************************************************************
STDMETHODIMP CLabels::GenerateCategories(long FieldIndex, tkClassificationType ClassificationType, long numClasses, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retVal = VARIANT_FALSE;

	if(_shapefile == NULL) 
	{
		return S_OK;
	}
	
	CComPtr<ITable> tbl = NULL;
	_shapefile->get_Table(&tbl);
	if (!tbl) 
		return S_OK;
		
	std::vector<CategoriesData>* values =TableHelper::Cast(tbl)->GenerateCategories(FieldIndex, ClassificationType, numClasses);
	if (!values)
		return S_OK;

	this->ClearCategories();
	_classificationField = -1;		// fast processing is off

	ILabelCategory* cat = NULL;
	
	for (int i = 0; i < (int)values->size(); i++)
	{
		CString strValue;

		CComBSTR bstrName((*values)[i].name);
		this->AddCategory(bstrName, &cat);
		CComBSTR bstrExpression((*values)[i].expression);
		cat->put_Expression(bstrExpression);
		((CLabelCategory*)cat)->m_value = (*values)[i].minValue;	// must be used after put_Expression, otherwise will be lost
																		// does "=" operator work for variants?
		cat->Release();
	}	

	if (ClassificationType == ctUniqueValues)
	{	
		// fast processing of categories will be available (m_value will be used without parsing of expressions)
		_classificationField = FieldIndex;		
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
	if (!_shapefile)
		return;
	
    if (CategoryIndex == -1) { // reload all labels when refreshing everything
        const CLabelOptions* options = get_LabelOptions();

        tkLabelPositioning Method;
        get_Positioning(&Method);
        long count;

        // First generate without text
        _shapefile->GenerateLabels(-1, Method, true, options->offsetXField, options->offsetYField, &count);
        if (count > 0) // then apply the label text expression
            this->ApplyLabelExpression();
    }

    if (!_synchronized) { // this will now be set to true, unless something really bad happened
        ErrorMessage(tkLABELS_NOT_SYNCHRONIZE);
        return;
    }

	CComPtr<ITable> tbl = nullptr;
	_shapefile->get_Table(&tbl);
    if (!tbl)
        return;
	
	long numShapes;
	_shapefile->get_NumShapes(&numShapes);
		
	// vector of numShapes size with category index for each shape
	std::vector<int> results;
	results.resize(numShapes, -1);

	// ----------------------------------------------------------------
	// we got unique values classification and want to process it fast
	// ----------------------------------------------------------------
	bool parsingIsNeeded = true;	
	if (_classificationField != -1)
	{
		parsingIsNeeded = false;	// in case there are unique values only we don't need any parsing
			
		std::map<CComVariant, long> myMap;				// variant value as key and number of category as result
		for (unsigned int i = 0; i < _categories.size(); i++)
		{
			if (i == CategoryIndex || CategoryIndex == -1)
			{
				CComVariant* val = &(((CLabelCategory*)_categories[i])->m_value);
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
		std::vector<CStringW> expressions;
		for (unsigned int i = 0; i < _categories.size(); i++)
		{
			if (i == CategoryIndex || CategoryIndex == -1)
			{
				CComVariant* val = &(((CLabelCategory*)_categories[i])->m_value);
				if (val->vt != VT_EMPTY && _classificationField != -1)
				{
					// we analyzed this one before, so just a dummy string here
					CStringW str = L"";
					expressions.push_back(str);
				}
				else
				{
					CComBSTR expr;
					_categories[i]->get_Expression(&expr);
					USES_CONVERSION;
					CStringW str = OLE2CW(expr);
					expressions.push_back(str);
				}
			}
			else
			{
				// we don't need this categories, so dummy strings for them
				CStringW str = L"";
				expressions.push_back(str);
			}
		}

		// adding category indices for shapes in the results vector
		TableHelper::Cast(tbl)->AnalyzeExpressions(expressions, results);
	}
		
	// Saving results
	if (CategoryIndex == -1)
	{
        for (unsigned long i = 0; i < results.size(); i++)
            SetCategoryForLabel(i, results[i]);
	}
	else
	{
		for (unsigned long i = 0; i < results.size(); i++)
		{
			if (results[i] == CategoryIndex)
                SetCategoryForLabel(i, results[i]);
		}
	}
}

// *****************************************************************
//			SetCategoryForLabel()
// *****************************************************************
void CLabels::SetCategoryForLabel(long labelIndex, long categoryIndex) 
{
    CLabelInfo* lbl = (*_labels[labelIndex])[0];
    lbl->category = categoryIndex;
    UpdateLabelOffsetsFromShapefile(labelIndex, categoryIndex);
}


// *****************************************************************
//			UpdateLabelOffsetsFromShapefile()
// *****************************************************************
void CLabels::UpdateLabelOffsetsFromShapefile(long labelIndex, long categoryIndex)
{
    VARIANT_BOOL retVal;
    get_Synchronized(&retVal);
    if (retVal == VARIANT_TRUE) {
        CLabelInfo* lbl = (*_labels[labelIndex])[0];

        CShapefile* sfClass = (CShapefile*) this->get_ParentShapefile();

        CLabelOptions* options;
        if (categoryIndex == -1)
            options = this->_options;
        else
            options = ((CLabelCategory*)_categories[categoryIndex])->get_LabelOptions();

        if (options->offsetXField >= 0) // don't overwrite existing values if field is not set by the user
            sfClass->GetLabelOffset(options->offsetXField, labelIndex, &lbl->offsetX);

        if (options->offsetYField >= 0) // don't overwrite existing values if field is not set by the user
            sfClass->GetLabelOffset(options->offsetYField, labelIndex, &lbl->offsetY);
    }
}

// *****************************************************************
//			ApplyCategories()
// *****************************************************************
// Determines to which category belong individual labels
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
	
	if (_shapefile == NULL) return ;
	
	long numFields;
	_shapefile->get_NumFields(&numFields);
	if (_classificationField < 0 || _classificationField >= numFields)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return;
	}
	
	IField* fld = NULL;
	CComBSTR name;
	_shapefile->get_Field(_classificationField, &fld);
	fld->get_Name(&name);
	
	USES_CONVERSION;
	CString fieldName = OLE2CA(name);
	
	fieldName = "[" + fieldName + "]";

	CString numberFormat = LabelsHelper::GetFloatNumberFormat(this);

	for(int i = 0; i < (int)_categories.size(); i++)
	{
		CComVariant vMin, vMax;
		_categories[i]->get_MinValue(&vMin);
		_categories[i]->get_MaxValue(&vMax);
		
		double dMax, dMin;
		dVal(vMin, dMin);
		dVal(vMax, dMax);

		CString sMax;
		CString sMin;
		CString s;

		sMin.Format(numberFormat, dMin);
		sMax.Format(numberFormat, dMax);
		
		s = fieldName + " >= " + sMin + " AND " + fieldName + " <= " + sMax;

		CComBSTR result(s);
		_categories[i]->put_Expression(result);
	}
	return;
}

// *****************************************************************
//			get/put_ClassificationField()
// *****************************************************************
STDMETHODIMP CLabels::get_ClassificationField(long* FieldIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*FieldIndex = _classificationField;
	return S_OK;
}
STDMETHODIMP CLabels::put_ClassificationField(long FieldIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_classificationField = FieldIndex;
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
        _synchronized = newVal;
        if (!LabelsSynchronized())
        {
            // Check label count matches with feature count
            _synchronized = VARIANT_FALSE;
            ErrorMessage(tkLABELS_CANT_SYNCHRONIZE);
        }
	}
	else
		_synchronized = VARIANT_FALSE;

	return S_OK;
}
STDMETHODIMP CLabels::get_Synchronized(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// the property must be set and the number of labels/parts must correspond
	if (LabelsSynchronized() && _synchronized)
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
	if (_shapefile == NULL) return false;
	
	long numShapes;
	_shapefile->get_NumShapes(&numShapes);
	return numShapes == (long)_labels.size();
}

// *****************************************************************
//			OptionsAsCategory()
// *****************************************************************
STDMETHODIMP CLabels::get_Options(ILabelCategory** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	*retVal = _category;

	if (_category)
		_category->AddRef();

	return S_OK;
}
STDMETHODIMP CLabels::put_Options(ILabelCategory* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		/*CLabelCategory* coCat =	static_cast<CLabelCategory*>(newVal);
		CLabelOptions* options =  coCat->get_LabelOptions();
		_options = *options;*/

	ComHelper::SetRef(newVal, (IDispatch**)&_category, false);
	_options = ((CLabelCategory*)_category)->get_LabelOptions();

	return S_OK;
}

// **********************************************************
//	  ClearLabelFrames()
// **********************************************************
// Clears bounds of labels that were drawn on screen (screen coordinates)
// Must be called before the redraw (before the call of MapView::DrawLabelsAlt).
void CLabels::ClearLabelFrames()
{
    for (int iLabel = 0; iLabel < (int)_labels.size(); iLabel++)
	{
		vector<CLabelInfo*>* parts = _labels[iLabel];
		for (int j =0; j < (int)parts->size(); j++)
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
	*pVal = OLE2BSTR(_expression);
	return S_OK;
}
STDMETHODIMP CLabels::put_VisibilityExpression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_expression);
	USES_CONVERSION;
	_expression = OLE2BSTR(newVal);
	return S_OK;
}

// **********************************************************
//		get/put_MinDrawingSize()
// **********************************************************
STDMETHODIMP CLabels::get_MinDrawingSize(LONG* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = _minDrawingSize;
	return S_OK;
}
STDMETHODIMP CLabels::put_MinDrawingSize(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_minDrawingSize = newVal;
	return S_OK;
}

// ***************************************************************
//		MoveUp
// ***************************************************************
STDMETHODIMP CLabels::MoveCategoryUp (long Index, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < (long)_categories.size() && Index > 0)
	{
		ILabelCategory* catBefore = _categories[Index - 1];
		_categories[Index - 1] = _categories[Index];
		_categories[Index] = catBefore;
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
	if (Index < (long)_categories.size() - 1 && Index >= 0)
	{
		ILabelCategory* catAfter = _categories[Index + 1];
		_categories[Index + 1] = _categories[Index];
		_categories[Index] = catAfter;
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
	*retval = _autoOffset;
	return S_OK;
}
STDMETHODIMP CLabels::put_AutoOffset(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_autoOffset = newVal;
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
	CPLXMLNode* psTree = _options->SerializeToTree(ElementName);
	if (!psTree)
	{
		return NULL;
	}
	
	// adding categories if any
	if (_categories.size() > 0)
	{
		CPLXMLNode* psCategories = CPLCreateXMLNode(psTree, CXT_Element, "LabelCategories");
		for (unsigned int i = 0; i < _categories.size(); i++)
		{
			CPLXMLNode* node = ((CLabelCategory*)_categories[i])->SerializeCore("LabelCategoryClass");
			if (node)
			{
				CPLAddXMLChild(psCategories, node);
			}
		}
	}

	// labels
	if (_labels.size() > 0)  		// positions should be saved only if there are labels
	{
		if (_savingMode == modeStandard)
		{
            bool saveRotation, saveText, saveOffsetX, saveOffsetY;
			CPLXMLNode* nodeLabels = this->SerializeLabelData("Labels", saveRotation, saveText, saveOffsetX, saveOffsetY);
			
			Utility::CPLCreateXMLAttributeAndValue(psTree, "RotationSaved", CPLString().Printf("%d", (int)saveRotation));
			Utility::CPLCreateXMLAttributeAndValue(psTree, "TextSaved", CPLString().Printf("%d", (int)(saveText)));
            Utility::CPLCreateXMLAttributeAndValue(psTree, "OffsetXSaved", CPLString().Printf("%d", (int)(saveOffsetX)));
            Utility::CPLCreateXMLAttributeAndValue(psTree, "OffsetYSaved", CPLString().Printf("%d", (int)(saveOffsetY)));
			
			if (nodeLabels)
			{
				CPLAddXMLChild(psTree, nodeLabels);
			}
		}
		else if (_savingMode == modeXML || _savingMode == modeXMLOverwrite)
		{
			if (_shapefile)
			{
				tkShapefileSourceType sourceType;
				_shapefile->get_SourceType(&sourceType);
				if (sourceType == sstDiskBased)
				{
					// constructing the name of .lbl file
					CComBSTR name;
					_shapefile->get_Filename(&name);
					CString path = Utility::GetPathWOExtension(OLE2CA(name));
					path += ".lbl";
					
					if (_savingMode == modeXMLOverwrite && Utility::FileExists(path))
					{
						if(remove(path) != 0)
						{
							ErrorMessage(tkCANT_DELETE_FILE);
						}
					}

					// serialize only if there is no file, it's a responsibility 
					// of user to keep this file updated
					if (!Utility::FileExists(path))
					{
						VARIANT_BOOL retVal;
						CComBSTR bstrPath(path);
						SaveToXML(bstrPath, &retVal);

						// user will need to save modeXMLOverwrite once more to overwrite the file
						if (_savingMode == modeXMLOverwrite)
							_savingMode = modeXML;
					}
				}
			}
		}
		else if (_savingMode == modeDBF)
		{
            /*if (_shapefile) {
                tkShapefileSourceType sourceType;
                _shapefile->get_SourceType(&sourceType);
                if (sourceType == sstDiskBased)
                {
                    VARIANT_BOOL retVal;
                    SaveToDbf2(_xField, _yField, _angleField, _textField, _categoryField, _saveText, _saveCategory, _createFields, &retVal);
                }
            }*/
			ErrorMessage(tkMETHOD_NOT_IMPLEMENTED);
		}
	}

	Utility::CPLCreateXMLAttributeAndValue(psTree, "Generated", CPLString().Printf("%d", _labels.size() > 0));
	
	// start labels specific options
	if (_minDrawingSize != 1)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MinDrawingSize", CPLString().Printf("%d", _minDrawingSize));

	str = OLE2CA(_expression);
	if (str != "")
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VisibilityExpression", str);
	
	str = OLE2CA(_key);
	if (str != "")
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Key", str);

	if (_verticalPosition != vpAboveAllLayers)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "VerticalPosition", CPLString().Printf("%d", (int)_verticalPosition));

	if (_basicScale != 0.0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "BasicScale", CPLString().Printf("%f", _basicScale));

	if (_maxVisibleScale != 100000000.0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxVisibleScale", CPLString().Printf("%f", _maxVisibleScale));
	
	if (_minVisibleScale != 0.0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MinVisibleScale", CPLString().Printf("%f", _minVisibleScale));

	if (_minVisibleZoom != 0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MinVisibleZoom", CPLString().Printf("%d", _minVisibleZoom));

	if (_maxVisibleZoom != 25)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxVisibleZoom", CPLString().Printf("%d", _maxVisibleZoom));

	if (_collisionBuffer != 0)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "CollisionBuffer", CPLString().Printf("%d", _collisionBuffer));

	if (_scale != false)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "ScaleLabels", CPLString().Printf("%d", (int)_scale));

	if (_autoOffset != VARIANT_TRUE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AutoOffset", CPLString().Printf("%d", (int)_autoOffset));

	if (_dynamicVisibility != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "DynamicVisibility", CPLString().Printf("%d", (int)_dynamicVisibility));

	if (m_globalSettings.labelsCollisionMode == tkCollisionMode::AllowCollisions)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "AvoidCollisions", CPLString().Printf("%d", (int)_avoidCollisions));

	if (_useWidthLimits != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseWidthLimits", CPLString().Printf("%d", (int)_useWidthLimits));

	if (_removeDuplicates != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "RemoveDuplicates", CPLString().Printf("%d", (int)_removeDuplicates));

	if (_gdiPlus != VARIANT_TRUE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseGdiPlus", CPLString().Printf("%d", (int)_gdiPlus));

	if (_savingMode != modeXML)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "SavingMode", CPLString().Printf("%d", (int)_savingMode));

	if (_positioning != lpNone)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Positioning", CPLString().Printf("%d", (int)_positioning));

	if (_textRenderingHint != HintAntiAlias)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "TextRenderingHint", CPLString().Printf("%d", (int)_textRenderingHint));

	if (_labelExpression != "")
		Utility::CPLCreateXMLAttributeAndValue(psTree, "Expression", _labelExpression);

	if (_sourceField != -1)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "SourceField",CPLString().Printf("%d", _sourceField));

	if (_floatNumberFormat != m_globalSettings.floatNumberFormat)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "FloatNumberFormat", _floatNumberFormat);

	if (_useVariableSize != VARIANT_TRUE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "UseVariableSize", CPLString().Printf("%d", (int)_useVariableSize));

	if (_logScaleForSize != VARIANT_FALSE)
		Utility::CPLCreateXMLAttributeAndValue(psTree, "LogScaleForSize", CPLString().Printf("%d", (int)_logScaleForSize));
	

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
	s = CPLGetXMLValue(node, "SavingMode", NULL);
	_savingMode = (s != "") ? (tkSavingMode)atoi(s.GetString()) : modeXML;

	// restoring categories
	this->ClearCategories();
	
	if (_savingMode != modeNone)
	{
		this->Clear();
	}

	CPLXMLNode* nodeCategories = CPLGetXMLNode(node, "LabelCategories");
	if (nodeCategories)
	{
		nodeCategories = nodeCategories->psChild;
		while (nodeCategories)
		{
			if (strcmp(nodeCategories->pszValue, "LabelCategoryClass") == 0)
			{
				ILabelCategory* cat = NULL;
				this->AddCategory(m_globalSettings.emptyBstr, &cat);
				((CLabelCategory*)cat)->DeserializeFromNode(nodeCategories);
				cat->Release();
			}
			nodeCategories = nodeCategories->psNext;
		} 
	}

	s = CPLGetXMLValue(node, "Positioning", NULL);
	if (s != "") _positioning =  (tkLabelPositioning)atoi(s.GetString());

	bool inMemory = false;
	if (_shapefile)
	{
		tkShapefileSourceType sourceType;
		_shapefile->get_SourceType(&sourceType);
		inMemory = sourceType != sstDiskBased;
	}

	CString expression = CPLGetXMLValue(node, "Expression", "");
	expression.Replace("&#xA;", "\r\n");

	// restoring labels
	s = CPLGetXMLValue(node, "Generated", NULL);
	bool loadLabels = atoi(s.GetString()) == 0 ? false : true;
	if (loadLabels)
	{
		bool xmlMode = _savingMode == modeXML || _savingMode == modeXMLOverwrite;

        s = CPLGetXMLValue(node, "OffsetXField", NULL);
        if (s != "")
            _options->offsetXField = atoi(s.GetString()) ? true : false;

        s = CPLGetXMLValue(node, "OffsetYField", NULL);
        if (s != "")
            _options->offsetYField = atoi(s.GetString()) ? true : false;

		if ((xmlMode && inMemory) ||  _savingMode == modeNone)
		{
			long count;
			s = CPLGetXMLValue(node, "SourceField", NULL);
            if (s != "")
                _sourceField = atoi(s.GetString()) ? true : false;

			if (_sourceField != -1) // old style
				_shapefile->GenerateLabels(_sourceField, _positioning, true, _options->offsetXField, _options->offsetYField, &count);
			else if (expression.GetLength() != 0) // new expression style - the expression gets parsed after initial generation of labels & positions
				_shapefile->GenerateLabels(-1, _positioning, true, _options->offsetXField, _options->offsetYField, &count);
		}
		else if (_savingMode == modeStandard)
		{
			bool loadText, loadRotation, loadOffsetX, loadOffsetY;
			s = CPLGetXMLValue(node, "RotationSaved", NULL);
			if (s != "") loadRotation = atoi(s.GetString()) ? true : false;

			s = CPLGetXMLValue(node, "TextSaved", NULL);
			if (s != "") loadText = atoi(s.GetString()) ? true : false;
			
            s = CPLGetXMLValue(node, "OffsetXSaved", NULL);
            if (s != "") loadOffsetX = atoi(s.GetString()) ? true : false;

            s = CPLGetXMLValue(node, "OffsetXSaved", NULL);
            if (s != "") loadOffsetY = atoi(s.GetString()) ? true : false;

			CPLXMLNode* nodeData = CPLGetXMLNode(node, "Labels");
			if (nodeData)
			{
				DeserializeLabelData(nodeData, loadRotation, loadText, loadOffsetX, loadOffsetY);
			}
		}
		else if (_savingMode == modeXML || _savingMode == modeXMLOverwrite)
		{
			// constructing the name of .lbl file
			CComBSTR name;
			_shapefile->get_Filename(&name);
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
	_options->DeserializeFromNode(node);
	
	// start labels specific options
	s = CPLGetXMLValue(node, "MinDrawingSize", NULL);
	_minDrawingSize = (s != "") ? atoi(s.GetString()) : 1;

	s = CPLGetXMLValue(node, "VisibilityExpression", NULL);

	USES_CONVERSION;
	CComBSTR bstrExpression(s);
	this->put_VisibilityExpression(bstrExpression);

	/*s = CPLGetXMLValue(node, "ClassificationField", NULL);
	m_classificationField = (s != "") ? (long)atoi(s.GetString()) : -1;*/

	s = CPLGetXMLValue(node, "Key", NULL);
	CComBSTR bstrKey = A2W(s);
	this->put_Key(bstrKey);
	
	s = CPLGetXMLValue(node, "VerticalPosition", NULL);
	_verticalPosition = (s != "") ? (tkVerticalPosition)atoi(s.GetString()) : vpAboveAllLayers;

	s = CPLGetXMLValue(node, "BasicScale", NULL);
	_basicScale = (s != "") ? Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue(node, "MaxVisibleScale", NULL);
	_maxVisibleScale = (s != "") ? Utility::atof_custom(s) : 100000000.0;

	s = CPLGetXMLValue(node, "MinVisibleScale", NULL);
	_minVisibleScale = (s != "") ? Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue(node, "MaxVisibleZoom", NULL);
	_maxVisibleZoom = (s != "") ? atoi(s) : 25;

	s = CPLGetXMLValue(node, "MinVisibleZoom", NULL);
	_minVisibleZoom = (s != "") ? atoi(s) : 0;

	s = CPLGetXMLValue(node, "CollisionBuffer", NULL);
	_collisionBuffer = (s != "") ? atoi(s.GetString()) : 0;

	s = CPLGetXMLValue(node, "ScaleLabels", NULL);
	_scale = (s != "") ? (atoi(s.GetString()) == 0? false : true) : false;

	s = CPLGetXMLValue(node, "AutoOffset", NULL);
	_autoOffset = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_TRUE;

	s = CPLGetXMLValue(node, "DynamicVisibility", NULL);
	_dynamicVisibility = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;

	s = CPLGetXMLValue(node, "AvoidCollisions", NULL);
	_avoidCollisions = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_TRUE;
	
	s = CPLGetXMLValue(node, "UseWidthLimits", NULL);
	_useWidthLimits = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;
	
	s = CPLGetXMLValue(node, "RemoveDuplicates", NULL);
	_removeDuplicates = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;

	s = CPLGetXMLValue(node, "UseGdiPlus", NULL);
	_gdiPlus = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_TRUE;
	
	s = CPLGetXMLValue(node, "TextRenderingHint", NULL);
	_textRenderingHint = (s != "") ? (tkTextRenderingHint)atoi(s.GetString()) : HintAntiAlias;

	s = CPLGetXMLValue(node, "FloatNumberFormat", NULL);
	CComBSTR bstrFormat(s);
	put_FloatNumberFormat(bstrFormat);

	s = CPLGetXMLValue(node, "UseVariableSize", NULL);
	_useVariableSize = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_TRUE;

	s = CPLGetXMLValue(node, "LogScaleForSize", NULL);
	_logScaleForSize = (s != "") ? (VARIANT_BOOL)atoi(s.GetString()) : VARIANT_FALSE;

	// applying the expressions
	if (_categories.size() > 0)
	{
		this->ApplyCategories();
	}
	
	// applying expression; should be done after label generation
	CComBSTR bstr(expression);
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
	*retVal = A2BSTR(_labelExpression.GetString());
	return S_OK;
}

STDMETHODIMP CLabels::put_Expression(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if (!_shapefile)
	{
		if (SysStringLen(newVal) > 0)
			ErrorMessage(tkEXPRESSION_NO_SHAPEFILE);
		return S_OK;
	}
	
	CStringW str = OLE2CW(newVal);
	_labelExpression = str;
    ApplyLabelExpression();

	return S_OK;
}

STDMETHODIMP CLabels::ApplyLabelExpression()
{
    // If we're not in sync with the featureset we can't do this
    VARIANT_BOOL inSync;
    get_Synchronized(&inSync);
    if (inSync == VARIANT_FALSE && _labels.size() > 0)
    {
        ErrorMessage(tkLABELS_NOT_SYNCHRONIZE);
        return S_OK;
    }
    
    // If expression is empty, simply clear label text
    if (_labelExpression.GetLength() == 0)
    {
        for (unsigned int i = 0; i < _labels.size(); i++)
        {
            (*_labels[i])[0]->text = L"";
        }
        return S_OK;
    }

    // Get the properties table
    CComPtr<ITable> table = NULL;
    _shapefile->get_Table(&table);
    if (!table)
        return S_OK;

    // Analyze expression
    CStringW strError;
    std::vector<CStringW> results;
    if (!TableHelper::Cast(table)->CalculateCore(_labelExpression.GetString(), results, strError, LabelsHelper::GetFloatNumberFormat(this)))
    {
        // Invalid expression:
        return S_OK;
        ErrorMessage(tkINVALID_EXPRESSION);
    }

    // Apply if sizes match
    if (results.size() != _labels.size())
        return S_OK;

    // Update label text
    for (unsigned int i = 0; i < _labels.size(); i++)
        (*_labels[i])[0]->text = results[i];

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
		return S_OK;
	}

	if (s.Right(4).MakeLower() != L".lbl")
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_OK;
	}
	
	CPLXMLNode *psTree = CPLCreateXMLNode(NULL, CXT_Element, "MapWindow");
	if (psTree)
	{
		Utility::WriteXmlHeaderAttributes(psTree, "Labels");

        bool saveText, saveRotation, saveOffsetX, saveOffsetY;
		CPLXMLNode* node = SerializeLabelData("Labels", saveRotation, saveText, saveOffsetX, saveOffsetY);

		if (node)
		{
			Utility::CPLCreateXMLAttributeAndValue(node, "Count", CPLString().Printf("%d", (int)_labels.size()));
			Utility::CPLCreateXMLAttributeAndValue(node, "RotationSaved", CPLString().Printf("%d", (int)saveRotation));
			Utility::CPLCreateXMLAttributeAndValue(node, "TextSaved", CPLString().Printf("%d", (int)(saveText)));
            Utility::CPLCreateXMLAttributeAndValue(node, "OffsetXSaved", CPLString().Printf("%d", (int)(saveOffsetX)));
            Utility::CPLCreateXMLAttributeAndValue(node, "OffsetYSaved", CPLString().Printf("%d", (int)(saveOffsetY)));

			CPLAddXMLChild(psTree, node);
			*retVal = GdalHelper::SerializeXMLTreeToFile(psTree, s);
			CPLDestroyXMLNode(psTree);
		}
	}
	return S_OK;
}

// ********************************************************
//		SerializeLabelData()
// ********************************************************
CPLXMLNode* CLabels::SerializeLabelData(CString ElementName, bool& saveRotation, bool& saveText, bool& saveOffsetX, bool& saveOffsetY)
{
	saveRotation = false;
	saveText = _labelExpression == "";
	
	CPLXMLNode* psLabels = CPLCreateXMLNode(NULL, CXT_Element, ElementName);
	if (psLabels)
	{
		for (unsigned int i = 0; i < _labels.size(); i++)
		{
			CLabelInfo* info = (*_labels[i])[0];
			if (info->rotation != 0.0)
			{
				saveRotation = true;
				break;
			}
		}
		
		CPLXMLNode* nodeOld = NULL;
		CPLXMLNode* nodeNew = NULL;

		for (unsigned int i = 0; i < _labels.size(); i++)
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
			
			CLabelInfo* info = (*_labels[i])[0];
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
            if (saveOffsetX)
            {
                Utility::CPLCreateXMLAttributeAndValue(nodeOld, "Name", info->offsetX);
            }
            if (saveOffsetY)
            {
                Utility::CPLCreateXMLAttributeAndValue(nodeOld, "Name", info->offsetY);
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
		CString s = CPLGetXMLValue(node, "Font", NULL);
		if (s != "") {
			SysFreeString(_options->fontName);
			_options->fontName = A2BSTR(s);
		}

		s = CPLGetXMLValue(node, "Size", NULL);
		if (s != "") _options->fontSize = (int)Utility::atof_custom(s);

		s = CPLGetXMLValue(node, "Size2", NULL);
		if (s != "") _options->fontSize2 = (int)Utility::atof_custom(s);

		s = CPLGetXMLValue(node, "Color", NULL);
		if (s != "") _options->fontColor = (OLE_COLOR)(atoi(s) + pow(2.0, 24.0));

		s = CPLGetXMLValue(node, "Scaled", NULL);
		if (s != "") _scale = atoi(s) == 0? false : true;

		s = CPLGetXMLValue(node, "Scale", NULL);
		if (s != "") _basicScale = Utility::atof_custom(s);

		s = CPLGetXMLValue(node, "UseShadows", NULL);
		if (s != "") _options->shadowVisible = atoi(s) == 0 ? false : true;

		s = CPLGetXMLValue(node, "ShadowColor", NULL);
		if (s != "") _options->shadowColor = (OLE_COLOR)(atoi(s) + pow(2.0, 24.0));

		s = CPLGetXMLValue(node, "UseLabelCollision", NULL);
		if (s != "") _avoidCollisions = atoi(s) == 0 ? false : true;

		s = CPLGetXMLValue(node, "Bold", NULL);
		if (_stricmp(s, "true") == 0)
			_options->fontStyle |= fstBold;

		s = CPLGetXMLValue(node, "Italic", NULL);
		if (_stricmp(s, "true") == 0)
			_options->fontStyle |= fstItalic;

		s = CPLGetXMLValue(node, "Underline", NULL);
		if (_stricmp(s, "true") == 0)
			_options->fontStyle |= fstUnderline;
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
			CString s = CPLGetXMLValue(node, "FileVersion", "0");
			int version = atoi(s);

			node = CPLGetXMLNode(node, "Labels");
			if (node)
			{
                bool loadText = true, loadRotation = true, loadOffsetX = true, loadOffsetY = true;
				
				// loading options from the old format
				if (version < 1)
				{
					// old format
					CLabelOptions opt;
					*_options = opt;
					this->LoadLblOptions(node);
				}
				else
				{
					// new format
					s = CPLGetXMLValue(node, "RotationSaved", NULL);
					if (s != "") loadRotation = atoi(s.GetString()) ? true : false;

					s = CPLGetXMLValue(node, "TextSaved", NULL);
					if (s != "") loadText = atoi(s.GetString()) ? true : false;

                    s = CPLGetXMLValue(node, "OffsetXSaved", NULL);
                    if (s != "") loadOffsetX = atoi(s.GetString()) ? true : false;

                    s = CPLGetXMLValue(node, "OffsetYSaved", NULL);
                    if (s != "") loadOffsetY = atoi(s.GetString()) ? true : false;
				}
				DeserializeLabelData(node, loadRotation, loadText, loadOffsetY, loadOffsetX);
			}
		}
		CPLDestroyXMLNode(root);
	}
	return S_OK;
}

// ********************************************************
//     DeserializeLabelData()
// ********************************************************
bool CLabels::DeserializeLabelData(CPLXMLNode* node, bool loadRotation, bool loadText, bool loadOffsetX, bool loadOffsetY)
{
	if (!node) return false;
	
	this->Clear();
	CComBSTR bstrDefault("");
	CString s;
	CString text;
    double x, y, angle, offsetX, offsetY;
	node = CPLGetXMLNode(node, "Label");
		
	while (node)
	{
		s = CPLGetXMLValue(node, "X", "0.0");
		x = Utility::atof_custom(s);

		s = CPLGetXMLValue(node, "Y", "0.0");
		y = Utility::atof_custom(s);

        if (loadOffsetX) {
            s = CPLGetXMLValue(node, "OffsetX", "0.0");
            offsetX = Utility::atof_custom(s);
        }
        else 
        {
            offsetX = 0.0;
        }

        if (loadOffsetY) {
            s = CPLGetXMLValue(node, "OffsetY", "0.0");
            offsetY = Utility::atof_custom(s);
        }
        else
        {
            offsetY = 0.0;
        }

		CComBSTR bstr;
		if (loadText)
		{
			text = CPLGetXMLValue(node, "Name", "");
			bstr.Attach(A2BSTR(text));
        }
        else 
        {
            bstr = bstrDefault;
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

		this->AddLabel(bstr, x, y, angle, -1, offsetX, offsetY);
		node = node->psNext;
	}
		
	// let's try to sync
	this->put_Synchronized(VARIANT_TRUE);

	_fontSizeChanged = true;		

	return true;
}

#pragma region "DbfSerialization"

// ********************************************************
//     SaveLabels()
// ********************************************************
STDMETHODIMP CLabels::SaveToDbf(VARIANT_BOOL saveText, VARIANT_BOOL saveCategory, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CComBSTR labelX("_LabelX");
	CComBSTR labelY("_LabelY");
	CComBSTR labelAngle("_LabelAngle");
	CComBSTR labelText("_LabelText");
	CComBSTR labelCategory("_LabelCategory");
	SaveToDbf2(labelX, labelY, labelAngle, labelText, labelCategory, saveText, saveCategory, VARIANT_TRUE, retVal);
	return S_OK;
}
STDMETHODIMP CLabels::SaveToDbf2(BSTR xField, BSTR yField, BSTR angleField, BSTR textField, BSTR categoryField,  
								 VARIANT_BOOL saveText, VARIANT_BOOL saveCategory, VARIANT_BOOL createFields, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	if (!_shapefile)
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
	_shapefile->get_SourceType(&sourceType);
	if (sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_OK;
	}
		
	// extracting table
	CComPtr<ITable> dbf = NULL;
	_shapefile->get_Table(&dbf);
	
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
		ErrorMessage(errorCode);
		return S_OK;
	}
	
	// angles are saved only in case there is at least one label with non-zero angle
	bool saveAngle = false;
	for (unsigned int i = 0; i < _labels.size(); i++)
	{
		CLabelInfo* info = (*(_labels)[i])[0];
		if (info->rotation != 0.0)
		{
			saveAngle = true;
		}
		if (saveAngle)
			break;
	}

	// checking fields, creating if necessary
	CComBSTR fields[5];
	fields[0] = xField;
	fields[1] = yField;
	fields[2] = angleField;
	fields[3] = textField;
	fields[4] = categoryField;
	
    

    for (int i = 0; i < fields->Length(); i++)
	{
		if (i == 2 && !saveAngle)
			continue;

		if (i == 3 && !saveText)
			continue;

		if (i == 4 && !saveCategory)
			continue;
		
		IField* field = NULL;
		_shapefile->get_FieldByName(fields[i], &field);
		if (!field && createFields == VARIANT_TRUE) {
            // creating fields
            CoCreateInstance(CLSID_Field, NULL, CLSCTX_INPROC_SERVER, IID_IField, (void**)&field);
            if (field)
            {
                field->put_Name(fields[i]);
                FieldType type = i < 3 ? DOUBLE_FIELD : (i == 3 ? STRING_FIELD : INTEGER_FIELD);
                field->put_Type(type);

                long numFields;
                _shapefile->get_NumFields(&numFields);
                _shapefile->EditInsertField(field, &numFields, NULL, retVal);		// not checking the result here; nothing can occur inside 
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
		CLabelInfo* info = (*(_labels)[i])[0];

		// X
        if (indices[0] > 0) {
            var.vt = VT_R8;
            var.dblVal = info->x;
            dbf->EditCellValue(indices[0], i, var, &vbretval);
        }

		// Y
        if (indices[1] > 0) {
            var.vt = VT_R8;
            var.dblVal = info->y;
            dbf->EditCellValue(indices[1], i, var, &vbretval);
        }

		// Rotation
		if (saveAngle && indices[2] > 0)
		{
			var.vt = VT_R8;
			var.dblVal = info->rotation;
			dbf->EditCellValue(indices[2], i, var, &vbretval);
		}

		// Text
		if (saveText && indices[3] > 0)
		{
			var.vt = VT_BSTR;
			var.bstrVal = W2BSTR(info->text);
			dbf->EditCellValue(indices[3], i, var, &vbretval);
		}

		// Category
		if (saveCategory && indices[4] > 0)
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

	return S_OK;
}

// ********************************************************
//     LoadLabels()
// ********************************************************
STDMETHODIMP CLabels::LoadFromDbf(VARIANT_BOOL loadText, VARIANT_BOOL loadCategory, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CComBSTR labelX("_LabelX");
	CComBSTR labelY("_LabelY");
	CComBSTR labelAngle("_LabelAngle");
	CComBSTR labelText("_LabelText");
	CComBSTR labelCategory("_LabelCategory");
	LoadFromDbf2(labelX, labelY, labelAngle, labelText, labelCategory, loadText, loadCategory, retVal);
	return S_OK;
}

STDMETHODIMP CLabels::LoadFromDbf2(BSTR xField, BSTR yField, BSTR angleField, BSTR textField, BSTR categoryField, 
									 VARIANT_BOOL loadText, VARIANT_BOOL loadCategory, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if (!_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return S_OK;
	}
    
	tkShapefileSourceType sourceType;
	_shapefile->get_SourceType(&sourceType);
	if (sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_OK;
	}

	CComPtr<ITable> dbf = NULL;
	_shapefile->get_Table(&dbf);
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
				(i == 4 && type != INTEGER_FIELD))
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
		return S_OK;
	}

	// fast reading of the values from table
	std::vector<double> xValues;
    TableHelper::Cast(dbf)->get_FieldValuesDouble((int)indices[0], xValues);

    std::vector<double> yValues;
    TableHelper::Cast(dbf)->get_FieldValuesDouble((int)indices[1], yValues);
	
	//angles
	std::vector<double> vAngles;
	if (loadAngle)
		TableHelper::Cast(dbf)->get_FieldValuesDouble((int)indices[2], vAngles);

	// text
	std::vector<CString> vText;
	if (loadText)
		TableHelper::Cast(dbf)->get_FieldValuesString((int)indices[3], vText);

	// category
	std::vector<int> vCategory;
	if (loadCategory)
		TableHelper::Cast(dbf)->get_FieldValuesInteger((int)indices[4], vCategory);

	long numShapes;
	_shapefile->get_NumShapes(&numShapes);

	if (xValues.size() == yValues.size() && xValues.size() == numShapes)
	{
		for (int i = 0; i < numShapes; i++)
		{
			CLabelInfo* info =  (*(_labels)[i])[0];
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
	if (!_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return false;
	}

	this->Clear();
	
	long numShapes;
	_shapefile->get_NumShapes(&numShapes);

	for(int i = 0; i < numShapes; i++)
	{
		CLabelInfo* info = new CLabelInfo();
		std::vector<CLabelInfo*>* parts = new std::vector<CLabelInfo*>;
		parts->push_back(info);
		_labels.push_back(parts);
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
	if (!_shapefile)
	{
		ErrorMessage(tkPARENT_SHAPEFILE_NOT_EXISTS);
		return false;
	}

	tkShapefileSourceType sourceType;
	_shapefile->get_SourceType(&sourceType);
	if (sourceType == sstUninitialized)
	{
		ErrorMessage(tkSHAPEFILE_UNINITIALIZED);
		return S_OK;
	}

	CComPtr<ITable> table = NULL;
	_shapefile->get_Table(&table);
	
	CStringW error;
	VARIANT_BOOL vbretval;
	
	TableHelper::Cast(table)->ParseExpressionCore(Expression, vtString, error, &vbretval);

	if (!vbretval)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return S_OK;
	}

	// generating without text
	_shapefile->GenerateLabels(-1, Method, LargestPartOnly, -1, -1, Count);
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
	*retVal = _savingMode;
	return S_OK;
}

// *************************************************************
//		put_SavingMode()
// *************************************************************
STDMETHODIMP CLabels::put_SavingMode(tkSavingMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	_savingMode = newVal;
	return S_OK;
}

// *************************************************************
//		get_Positioning()
// *************************************************************
STDMETHODIMP CLabels::get_Positioning(tkLabelPositioning* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _positioning;
	return S_OK;
}

// *************************************************************
//		put_Positioning()
// *************************************************************
STDMETHODIMP CLabels::put_Positioning(tkLabelPositioning newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_shapefile)
	{
		ShpfileType type;
		_shapefile->get_ShapefileType(&type);
		type = ShapeUtility::Convert2D(type);

		bool polyline = false, polygon = false;
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
            case lpNone:
                polygon = polyline = true;
                break;
		}

		if ((type == SHP_POLYLINE && !polyline) ||
			(type == SHP_POLYGON && !polygon))
		{
			ErrorMessage(tkLABEL_POSITIONING_INVALID);
			return S_OK;
		}
	}
	_positioning = newVal;
	return S_OK;
}

// *************************************************************
//		TextRenderingHint()
// *************************************************************
STDMETHODIMP CLabels::get_TextRenderingHint(tkTextRenderingHint* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _textRenderingHint;
	return S_OK;
}

STDMETHODIMP CLabels::put_TextRenderingHint(tkTextRenderingHint newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0 && newVal <= ClearTypeGridFit)
		_textRenderingHint = newVal;
	return S_OK;
}

// *************************************************************
//		HasRotation()
// *************************************************************
bool CLabels::HasRotation()
{
	for (unsigned int i = 0; i < _labels.size(); i++)
	{
		vector<CLabelInfo*>* parts = _labels[i];
		for (int j = 0; j < (int)parts->size(); j++)
		{
			CLabelInfo* lbl = (*parts)[j];
			if (lbl->rotation != 0.0)
				return true;
		}
	}
	return false;
}

// *************************************************************
//		AddEmptyLabel()
// *************************************************************
void CLabels::AddEmptyLabel()
{
	AddLabel(m_globalSettings.emptyBstr, 0.0, 0.0, 0.0, 0, 0.0, 0.0);
	if (_labels.size() == 0) return;
	CLabelInfo* lbl = (*_labels[_labels.size() - 1])[0];
	if (lbl) {
		lbl->visible = false;
	}
}

// *************************************************************
//		get_FloatNumberFormat()
// *************************************************************
STDMETHODIMP CLabels::get_FloatNumberFormat(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = A2BSTR(_floatNumberFormat);
	return S_OK;
}

// *************************************************************
//		put_FloatNumberFormat()
// *************************************************************
STDMETHODIMP CLabels::put_FloatNumberFormat(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	CString format = OLE2A(newVal);
	CString s;
	s.Format(format, 1.1);
	if (SysStringLen(newVal) == 0) return S_OK;
	if (s != format) {
		_floatNumberFormat = newVal;
	}
	else {
		ErrorMessage(tkINVALID_FLOAT_NUMBER_FORMAT);
	}
	return S_OK;
}

// *************************************************************
//		ForceRecalculateExpression()
// *************************************************************
STDMETHODIMP CLabels::ForceRecalculateExpression()
{
    ApplyLabelExpression();
	return S_OK;
}

// *************************************************************
//		FontSize()
// *************************************************************
STDMETHODIMP CLabels::get_FontSize(LONG* retval)							
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = _options->fontSize;			
	return S_OK; 
}
STDMETHODIMP CLabels::put_FontSize(LONG newVal)							
{ 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_options->fontSize = newVal;			

	if (_options->fontSize > MAX_LABEL_SIZE) {
		_options->fontSize = MAX_LABEL_SIZE;
	}

	_fontSizeChanged = true;

	return S_OK; 
}

// *************************************************************
//		FontSize2()
// *************************************************************
STDMETHODIMP CLabels::get_FontSize2(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _options->fontSize2;

	return S_OK;
}
STDMETHODIMP CLabels::put_FontSize2(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_options->fontSize2 = newVal;

	if (_options->fontSize2 > MAX_LABEL_SIZE) {
		_options->fontSize2 = MAX_LABEL_SIZE;
	}

	_fontSizeChanged = true;

	return S_OK;
}



// *************************************************************
//		UseVariableSize()
// *************************************************************
STDMETHODIMP CLabels::get_UseVariableSize(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _useVariableSize;

	return S_OK;
}
STDMETHODIMP CLabels::put_UseVariableSize(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_useVariableSize = newVal;
	_fontSizeChanged = true;

	return S_OK;
}

// *************************************************************
//		LogScaleForSize()
// *************************************************************
STDMETHODIMP CLabels::get_LogScaleForSize(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _logScaleForSize;

	return S_OK;
}
STDMETHODIMP CLabels::put_LogScaleForSize(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_logScaleForSize = newVal;
	_fontSizeChanged = true;

	return S_OK;
}

// *************************************************************
//		UpdateFontSize()
// *************************************************************
bool CLabels::RecalculateFontSize()
{
	if (!_fontSizeChanged && LabelsSynchronized()) {
		return true;
	}

	if (!_shapefile) {
		return false;
	}

	if (!_useVariableSize || _options->fontSize == _options->fontSize2) {
		return false;
	}	

	CComBSTR sortField;
	_shapefile->get_SortField(&sortField);

	long fieldIndex;
	_shapefile->get_FieldIndexByName(sortField, &fieldIndex);

	if (fieldIndex == -1) {
		return false;
	}

	if (!LabelsSynchronized()) {
		return false;
	}

	CComPtr<ITable> tbl = NULL;
	_shapefile->get_Table(&tbl);

	bool logScale = _logScaleForSize ? true : false;

	vector<double> values;
	if (!TableHelper::Cast(tbl)->GetRelativeValues(fieldIndex, logScale, values)) {
		return false;
	}

	if (values.size() != _labels.size()) {
		return false;
	}

	double delta = _options->fontSize2 - _options->fontSize;
	
	if (_options->fontSize > MAX_LABEL_SIZE) _options->fontSize = MAX_LABEL_SIZE;
	if (_options->fontSize2 > MAX_LABEL_SIZE) _options->fontSize2 = MAX_LABEL_SIZE;

	for (size_t i = 0; i < _labels.size(); i++)
	{
		for (size_t j = 0; j < _labels[i]->size(); j++) 
		{
			CLabelInfo* info = (*_labels[i])[j];
			info->fontSize = Utility::Rint(_options->fontSize + values[i] * delta);
		}
	}

	_fontSizeChanged = false;	

	return true;
}

// *************************************************************
//		UpdateSizeField()
// *************************************************************
STDMETHODIMP CLabels::UpdateSizeField()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_fontSizeChanged = true;

	return S_OK;
}

