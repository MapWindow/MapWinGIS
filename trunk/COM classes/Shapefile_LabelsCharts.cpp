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
// lsu 3-02-2011: split the initial Shapefile.cpp file to make entities of the reasonble size

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
	
	m_labels->Clear();
	BSTR text;
	double x = 0.0,y = 0.0;
	long percent = 0;
	
	ShpfileType shpType;
	this->get_ShapefileType(&shpType);
	shpType = Utility::ShapeTypeConvert2D(shpType);
	
	long _numShapes;
	this->get_NumShapes(&_numShapes);
	
	tkLineLabelOrientation orientation;
	m_labels->get_LineOrientation(&orientation);

	for(int i = 0; i < _numShapes; i++)
	{
		long newpercent = (long)(((double)i/_numShapes)*100);
		if( newpercent > percent )
		{	
			percent = newpercent;
			if( globalCallback != NULL ) 
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Calculating label positions..."));
		}
		
		/* extracting field value */
		if (FieldIndex != -1)
		{
			VARIANT val;
			VariantInit(&val);
			this->get_CellValue(FieldIndex,i, &val);
			
			text = Utility::Variant2BSTR(&val);
			VariantClear(&val);
		}
		else
		{
			text = A2BSTR("");
		}
		
		// fictitious label should be added even if an error occured while calculating position
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
			m_labels->AddLabel(text, x, y, rotation);
		}
		else if (numParts == 0)
		{
			if (shpType == SHP_POINT || shpType == SHP_MULTIPOINT)
			{
				// points
				((CShape*)shp)->get_LabelPosition(Method, x, y, rotation, orientation);
				m_labels->AddLabel(text, x, y, rotation);
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
						m_labels->AddLabel(text, x, y, rotation);
						partCount++;
					}
					else		
					{
						m_labels->AddPart(i, text, x, y, rotation);
					}
					shpPart->Release(); 
					shpPart = NULL;
				}

				if (partCount == 0)
				{
					// if no parts valid parts were found, add ficitious label then
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
					
					// Seeking the larges part of shape
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
						m_labels->AddLabel(text, x, y, rotation);
						shpPart->Release();
					}
					else
						goto add_empty_label;
				}
				else
				{
add_empty_label:
					m_labels->AddLabel(text, 0.0, 0.0, 0.0);
					ILabel* lbl = NULL;
					m_labels->get_Label((long)i, 0, &lbl);
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
	m_labels->get_Count(&numLabels);
	*Count = numLabels;
	m_labels->put_Synchronized(VARIANT_TRUE);
	m_labels->put_Positioning(Method);

	if (FieldIndex == -1)
	{
		// in case there is label expression, reapply it
		BSTR expr;
		m_labels->get_Expression(&expr);
		m_labels->put_Expression(A2BSTR(""));
		m_labels->put_Expression(expr);
	}
	else
	{
		// save it for deserialization
		((CLabels*)m_labels)->SaveSourceField(FieldIndex);
	}

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));
	return S_OK;
}

// ******************************************************************
//		get_Labels/putLabels()
// ******************************************************************
//  Returns reference to Labels class
STDMETHODIMP CShapefile::get_Labels(ILabels** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_labels;
	if (m_labels)
		m_labels->AddRef();
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
		Utility::put_ComReference(newVal, (IDispatch**)&m_labels, false);
		this->put_ReferenceToLabels(false);
	}
	return S_OK;
}

/***********************************************************************/
/*		put_ReferenceToLabels
/***********************************************************************/
void CShapefile::put_ReferenceToLabels(bool bNullReference)
{
	if (m_labels == NULL) return;
	CLabels* coLabels = static_cast<CLabels*>(m_labels);
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
	*pVal = m_charts;
	if ( m_charts != NULL)
		m_charts->AddRef();
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
		if (newVal != m_charts)
		{
			if (m_charts != NULL) 
			{
				m_charts->Release();
				m_charts = NULL;
			}
			m_charts = newVal;
			m_charts->AddRef();
		}
	}
	return S_OK;
}
/***********************************************************************/
/*		put_ReferenceToCharts
/***********************************************************************/
void CShapefile::put_ReferenceToCharts(bool bNullReference)
{
	if (m_charts == NULL) return;
	CCharts* coCharts = static_cast<CCharts*>(m_charts);
	//CShapefile* coShapefile = static_cast<CShapefile*>(this);
	if (!bNullReference)
		coCharts->put_ParentShapefile(this);
	else
		coCharts->put_ParentShapefile(NULL);
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
		((CCharts*)m_charts)->_chartsExist = true;
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
			long newpercent = (long)(((double)i/_numShapes)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				if( globalCallback != NULL ) 
					globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Calculating charts positions..."));
			}
			
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
		
		if( globalCallback != NULL )
			globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));
	}
	return;
}

// *******************************************************************
//		ReadChartFields()
// *******************************************************************
// Fills array with data values from the selected fields, returns number
bool CShapefile::ReadChartFields(std::vector<double*>* values)
{
	struct FieldIndex
	{
		FieldType type;
		int index;
	};
	
	long numShapes;
	this->get_NumShapes(&numShapes);
	
	long numBars;
	m_charts->get_NumFields(&numBars);

	if ( numBars == 0 ) return false;

	// reading types of fields
	std::vector<FieldIndex> fields;
	for (int j = 0; j < numBars; j++)
	{
		long fieldIndex;
		IChartField* chartField = NULL;
		m_charts->get_Field(j, &chartField);
		if (chartField)
		{
			chartField->get_Index(&fieldIndex);
			chartField->Release();
		}

		//m_charts->get_FieldIndex(j, &fieldIndex);
		IField* fld = NULL;
		this->get_Field(fieldIndex, &fld);
		if ( fld )
		{
			FieldIndex ind;
			fld->get_Type(&ind.type);
			ind.index = fieldIndex;
			fields.push_back(ind);
			fld->Release(); fld = NULL;
		}
		else
		{
			FieldIndex ind;
			ind.index = -1;
			ind.type = STRING_FIELD;
			fields.push_back(ind);
		}
	}
	
	// reading data
	VARIANT val;
	VariantInit(&val);
	
	values->resize(numShapes);
	for (int i = 0; i < numShapes; i++)
	{	
		(*values)[i] = new double[numBars];
		double* arr = (*values)[i];
		for (int j = 0; j < numBars; j++)
		{
			if (fields[j].type == INTEGER_FIELD || fields[j].type == DOUBLE_FIELD)
			{
				this->get_CellValue(fields[j].index, i, &val );
				dVal(val, arr[j]);
			}
			else
			{
				arr[j] = 0.0;
			}
		}
	}
	VariantClear(&val);
	return true;
}

// *******************************************************************
//		ReadChartField()
// *******************************************************************
// ReadChartFields
bool CShapefile::ReadChartField(std::vector<double>* values, int fieldIndex)
{
	IField* fld = NULL;
	this->get_Field(fieldIndex, &fld);
	if ( !fld )
		return false;
	FieldType type;
	fld->get_Type(&type);
	if (type != INTEGER_FIELD && type != DOUBLE_FIELD)
		return false;
	fld->Release(); fld = NULL;

	// reading data
	VARIANT val;
	VariantInit(&val);
	
	long numShapes;
	this->get_NumShapes(&numShapes);
	values->resize(numShapes);
	for (int i = 0; i < numShapes; i++)
	{	
		this->get_CellValue(fieldIndex, i, &val );
		dVal(val, (*values)[i]);
	}
	VariantClear(&val);
	return true;
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

