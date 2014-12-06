//********************************************************************************************************
//File name: Shapefile.cpp
//Description: Implementation of the CShapefile (see other cpp files as well)
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
// -------------------------------------------------------------------------------------------------------
// lsu 3-02-2011: split the initial Shapefile.cpp file to make entities of the reasonable size

#include "stdafx.h"
#include "Shapefile.h"
#include "Labels.h"
#include "Charts.h"
#include "TableClass.h"
#include "Shape.h"

#pragma region Labels

// ************************************************************
//			GenerateLabels
// ************************************************************
// Routine for generation of labels
// FieldIndex == -1: labelswithout textwill be generated; 
// Method == lpNone: labels with (0.0,0.0) coordinates will be generated
STDMETHODIMP CShapefile::GenerateLabels(long FieldIndex, tkLabelPositioning Method, VARIANT_BOOL LargestPartOnly, long* Count)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*Count = 0;
	
	long numFields;
	this->get_NumFields(&numFields);
	
	if( FieldIndex < -1 || FieldIndex >= numFields)
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS); 
		return S_OK;
	} 
	
	_labels->Clear();
	
	double x = 0.0,y = 0.0;
	long percent = 0;
	
	ShpfileType shpType;
	this->get_ShapefileType(&shpType);
	shpType = Utility::ShapeTypeConvert2D(shpType);
	
	long _numShapes;
	this->get_NumShapes(&_numShapes);
	
	tkLineLabelOrientation orientation;
	_labels->get_LineOrientation(&orientation);

	for(int i = 0; i < _numShapes; i++)
	{
		CallbackHelper::Progress(_globalCallback, i, _numShapes, "Calculating label positions...", _key, percent);
		
		/* extracting field value */
		CComBSTR text;
		if (FieldIndex != -1)
		{
			VARIANT val;
			VariantInit(&val);
			this->get_CellValue(FieldIndex,i, &val);
			
			text.Attach(Utility::Variant2BSTR(&val));
			VariantClear(&val);
		}
		else
		{
			text.Attach(A2BSTR(""));
		}
		
		// fictitious label should be added even if an error occurred while calculating position
		// otherwise labels won't be synchronized and not a single label will be displayed at all

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		if (shp == NULL)
			goto add_empty_label;

		long numParts;
		shp->get_NumParts(&numParts);
		VARIANT_BOOL vbretval;
		double rotation = 0.0;

		if( numParts == 1)
		{
			// labeling the only part
			((CShape*)shp)->get_LabelPosition(Method, x, y, rotation, orientation);
			_labels->AddLabel(text, x, y, rotation);
		}
		else if (numParts == 0)
		{
			if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
			{
				// points
				((CShape*)shp)->get_LabelPosition(Method, x, y, rotation, orientation);
				_labels->AddLabel(text, x, y, rotation);
			}	
			else
			{
				// it's an invalid shape, we'll add a label but it is fictitious
				goto add_empty_label;
				
			}
		}
		else
		{
			//	labeling every part
			if (!LargestPartOnly) 
			{
				int partCount = 0;
				for (int j = 0; j < numParts; j++)
				{
					IShape* shpPart = NULL;
					shp->get_PartAsShape(j, &shpPart);
					if (shpPart == NULL) 
						continue;

					if (shpType == SHP_POLYGON)
					{
						shpPart->get_PartIsClockWise(0, &vbretval);		// Holes of polygons must not be labeled
						if(!vbretval) 
						{
							shpPart->Release();
							continue;
						}
					}
					
					((CShape*)shpPart)->get_LabelPosition(Method, x, y, rotation, orientation);
					
					if (partCount == 0) 
					{
						_labels->AddLabel(text, x, y, rotation);
						partCount++;
					}
					else		
					{
						_labels->AddPart(i, text, x, y, rotation);
					}
					shpPart->Release(); 
					shpPart = NULL;
				}

				if (partCount == 0)
				{
					// if no parts valid parts were found, add fictitious label then
					goto add_empty_label;
				}
			}
			else
			{
				// labeling only the largest/longest part
				long maxPart = -1; 
				double maxValue = 0;

				for (int j = 0; j < numParts; j++)
				{
					IShape* shpPart = NULL;
					shp->get_PartAsShape(j, &shpPart);
					if (!shpPart) 
						continue;
					
					if (shpType == SHP_POLYGON)
					{
						// Holes of polygons must not be labeled
						VARIANT_BOOL vbretval;
						shpPart->get_PartIsClockWise(0, &vbretval);
						if(!vbretval)
						{
							shpPart->Release();
							continue;
						}
					}	
					
					// Seeking the largest part of shape
					double value = 0.0;
					if (shpType == SHP_POLYGON)
					{
						shpPart->get_Area(&value);
					}
					else if (shpType == SHP_POLYLINE)
					{
						shpPart->get_Length(&value);
					}
					if(value > maxValue)
					{
						maxValue = value;
						maxPart = j;
					}
					shpPart->Release();
				}

				if (maxPart >= 0)
				{
					IShape* shpPart = NULL;
					shp->get_PartAsShape(maxPart, &shpPart);
					if (shpPart)
					{
						((CShape*)shpPart)->get_LabelPosition(Method, x, y, rotation, orientation);
						_labels->AddLabel(text, x, y, rotation);
						shpPart->Release();
					}
					else
						goto add_empty_label;
				}
				else
				{
add_empty_label:
					_labels->AddLabel(text, 0.0, 0.0, 0.0);
					ILabel* lbl = NULL;
					_labels->get_Label((long)i, 0, &lbl);
					if(lbl != NULL)
					{
						lbl->put_Visible(VARIANT_FALSE);
						lbl->Release(); 
					}
				}
			
			} // LargestPartOnly
		} // numParts > 1
		
		shp->Release(); 
		shp = NULL;
	}
	
	long numLabels;
	_labels->get_Count(&numLabels);
	*Count = numLabels;
	_labels->put_Synchronized(VARIANT_TRUE);
	_labels->put_Positioning(Method);

	if (FieldIndex == -1)
	{
		// in case there is label expression, reapply it
		CComBSTR expr;
		_labels->get_Expression(&expr);
		CComBSTR bstrEmpty = L"";
		_labels->put_Expression(bstrEmpty);
		_labels->put_Expression(expr);
	}
	else
	{
		// save it for deserialization
		((CLabels*)_labels)->SaveSourceField(FieldIndex);
	}

	CallbackHelper::ProgressCompleted(_globalCallback, _key);
	return S_OK;
}

// ******************************************************************
//		get_Labels/putLabels()
// ******************************************************************
//  Returns reference to Labels class
STDMETHODIMP CShapefile::get_Labels(ILabels** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _labels;
	if (_labels)
		_labels->AddRef();
	return S_OK;
}
STDMETHODIMP CShapefile::put_Labels(ILabels* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!newVal)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE); 
	}
	else
	{
		ComHelper::SetRef(newVal, (IDispatch**)&_labels, false);
		this->put_ReferenceToLabels(false);
	}
	return S_OK;
}

/***********************************************************************/
/*		put_ReferenceToLabels
/***********************************************************************/
void CShapefile::put_ReferenceToLabels(bool bNullReference)
{
	if (_labels == NULL) return;
	CLabels* coLabels = static_cast<CLabels*>(_labels);
	if (!bNullReference)
		coLabels->put_ParentShapefile(this);
	else
		coLabels->put_ParentShapefile(NULL);
};
#pragma endregion

#pragma region Charts
// *******************************************************************
//		get/put_Charts()
// *******************************************************************
STDMETHODIMP CShapefile::get_Charts (ICharts** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _charts;
	if ( _charts != NULL)
		_charts->AddRef();
	return S_OK;
}
STDMETHODIMP CShapefile::put_Charts (ICharts* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!newVal)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE); 
	}
	else
	{
		if (newVal != _charts)
		{
			if (_charts != NULL) 
			{
				_charts->Release();
				_charts = NULL;
			}
			_charts = newVal;
			_charts->AddRef();
		}
	}
	return S_OK;
}
/***********************************************************************/
/*		put_ReferenceToCharts
/***********************************************************************/
void CShapefile::put_ReferenceToCharts(bool bNullReference)
{
	if (!_charts) return;
	CCharts* coCharts = static_cast<CCharts*>(_charts);
	coCharts->put_ParentShapefile(!bNullReference ? this : NULL);
};

// ********************************************************************
//			SetChartsPositions
// ********************************************************************
// Routine for generation of labels
void CShapefile::SetChartsPositions(tkLabelPositioning Method)
{
	USES_CONVERSION;
	double x,y;
	
	ShpfileType shpType;
	this->get_ShapefileType(&shpType);
	shpType = Utility::ShapeTypeConvert2D(shpType);
	
	long _numShapes;
	this->get_NumShapes(&_numShapes);
	
	// creating structures
	for (unsigned int i = 0; i < _shapeData.size(); i++)
	{
		if (_shapeData[i]->chart == NULL)
		{
			_shapeData[i]->chart = new CChartInfo();
		}
	}

	if (Method == lpNone)
	{
		// simply set 0 positions, the actual positions will be set externally; is needed for loading of previously saved labels
		((CCharts*)_charts)->_chartsExist = true;
		for (unsigned int i = 0; i < _shapeData.size(); i++)
		{
			_shapeData[i]->chart->x = 0.0;
			_shapeData[i]->chart->y = 0.0;
		}
		return;
	}
	else
	{
		long percent = 0;
		for(int i = 0; i < _numShapes; i++)
		{
			CallbackHelper::Progress(_globalCallback, i, _numShapes, "Calculating charts positions...", _key, percent);
			
			IShape* shp = NULL;
			this->get_Shape(i, &shp);
			if (shp == NULL) continue;
			
			long numParts;
			shp->get_NumParts(&numParts);
			VARIANT_BOOL vbretval;
			double rotation = 0.0;

			// ----------------------------------------------------
			//	Labeling the only part								
			// ----------------------------------------------------
			if( numParts == 1)
			{
				((CShape*)shp)->get_LabelPosition(Method, x, y, rotation, lorHorizontal);			
			}
			else if (numParts == 0)
			{
				if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
				{
					shp->get_XY(0, &x, &y, &vbretval);
				}	
				else
				{
					// it's an invalid shape, we'll add a label but it is fictitious
					x = y = 0.0;
				}
			}
			else //if ( numParts > 1 )
			{
				long maxPart = -1; double maxValue = 0;
				for (int j = 0; j < numParts; j++)
				{
					IShape* shpPart = NULL;
					shp->get_PartAsShape(j, &shpPart);
					if (shpPart == NULL) continue;
					
					if (shpType == SHP_POLYGON)
					{
						// Holes of polygons must not be labeled
						VARIANT_BOOL vbretval;
						shpPart->get_PartIsClockWise(0, &vbretval);
						if(!vbretval) continue;
					}	
					
					// Seeking the larges part of shape
					double value = 0.0;
					if		(shpType == SHP_POLYGON)		shpPart->get_Area(&value);
					else if (shpType == SHP_POLYLINE)	shpPart->get_Length(&value);

					if ( value > maxValue )
					{
						maxValue = value;
						maxPart = j;
					}
					shpPart->Release(); shpPart = NULL;
				}

				IShape* shpPart = NULL;
				shp->get_PartAsShape(maxPart, &shpPart);
				if ( shpPart == NULL ) continue;
				
				((CShape*)shpPart)->get_LabelPosition(Method, x, y, rotation, lorHorizontal);
				shpPart->Release(); shpPart = NULL;
			} // numParts > 1
			
			_shapeData[i]->chart->x = x;
			_shapeData[i]->chart->y = y;
			
			shp->Release(); shp = NULL;
		}
		
		CallbackHelper::ProgressCompleted(_globalCallback, _key);
	}
	return;
}

// *************************************************************
//		ClearChartFrames()
// *************************************************************
void CShapefile::ClearChartFrames()
{
	for (unsigned int i = 0; i < _shapeData.size(); i++ ) 
	{
		CChartInfo* chart = _shapeData[i]->chart;
		if (chart)
		{
			if ( chart->frame != NULL )
			{
				delete chart->frame;
				chart->frame = NULL;
			}
		}
	}
}
#pragma endregion

