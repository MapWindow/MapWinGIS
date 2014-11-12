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
#include <afxmt.h>				// for CMutex
#include "Shapefile.h"
#include "Templates.h"
#include "ShapeHelper.h"
#include "SelectionHelper.h"
#include "ExtentsHelper.h"
#include "ShapefileHelper.h"

#pragma region SelectShapes
// ******************************************************************
//		SelectShapes()
// ******************************************************************
//# include "Timer.h"
CMutex selectShapesMutex(FALSE);
STDMETHODIMP CShapefile::SelectShapes(IExtents *BoundBox, double Tolerance, SelectMode SelectMode, VARIANT *Result, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *retval = VARIANT_FALSE;

	selectShapesMutex.Lock();
	
	double b_minX, b_maxX, b_minY, b_maxY, b_minZ, b_maxZ;
	BoundBox->GetBounds(&b_minX,&b_minY,&b_minZ,&b_maxX,&b_maxY,&b_maxZ);
	
	// the core routine
	std::vector<long> selectResult;
	Extent box(b_minX, b_maxX, b_minY, b_maxY);
	this->SelectShapesCore(box, Tolerance, SelectMode, selectResult);

	*retval  = Templates::Vector2SafeArray(&selectResult, VT_I4, Result) ? VARIANT_TRUE : VARIANT_FALSE;

	selectShapesMutex.Unlock();
	return S_OK;
}

// ****************************************************************
//		SelectShapesCore()
// ****************************************************************
bool CShapefile::SelectShapesCore(Extent& extents, double Tolerance, SelectMode SelectMode, std::vector<long>& selectResult)
{
	double b_minX = extents.left;
	double b_maxX = extents.right;
	double b_minY = extents.bottom;
	double b_maxY = extents.top;
	
	double s_minX, s_maxX, s_minY, s_maxY;
	std::vector<double> xPts;
	std::vector<double> yPts;
	std::vector<long> parts;
	ShpfileType ShapeType;

	if( Tolerance > 0.0 )
	{	
		double halfTolerance = Tolerance*.5;
		b_minX -= halfTolerance;
		b_minY -= halfTolerance;
		b_maxX += halfTolerance;
		b_maxY += halfTolerance;				
	}

    bool bPtSelection = b_minX == b_maxX && b_minY == b_maxY;
	int local_numShapes = _shapeData.size();

	IndexSearching::CIndexSearching *res;
	vector<int> qtreeResult;
	bool useSpatialIndexResults = false;
	bool useQTreeResults = false;

	VARIANT_BOOL useSpatialIndex;
	CComPtr<IExtents> box = ExtentsHelper::Populate(extents);
	this->get_CanUseSpatialIndex(box, &useSpatialIndex);
	
	if (useSpatialIndex)   //ajp Jun 2008 select records from spatial index
	{
		double lowVals[2], highVals[2];
		IndexSearching::QueryTypeFlags qType;

		lowVals[0] = b_minX;
		lowVals[1] = b_minY;
		highVals[0] = b_maxX;
		highVals[1] = b_maxY;

		qType = (SelectMode == INTERSECTION) ? IndexSearching::intersection : IndexSearching::contained;
		res = new IndexSearching::CIndexSearching();

		int ret = selectShapesFromIndex(_spatialIndexID, lowVals, highVals, qType, res);
		if (ret != 0)
		{
			_hasSpatialIndex = false;
			_spatialIndexLoaded = false;
			IndexSearching::unloadSpatialIndex(_spatialIndexID);
			delete res;		//TODO throw error
		}
		else
		{
			local_numShapes = res->getLength();
			useSpatialIndexResults = true;
		}
	}
	else if(_isEditingShapes && _useQTree)
	{
		//07/23/2009 ,Neio fix the bug that select shapes in edit mode and add QTree
		if(bPtSelection )
		{
			qtreeResult = _qtree->GetNodes(QTreeExtent(b_minX,b_minX +1,b_minY + 1,b_minY));
		}
		else
		{
			qtreeResult = _qtree->GetNodes(QTreeExtent(b_minX,b_maxX,b_maxY,b_minY));
		}

		local_numShapes = qtreeResult.size();	
		useQTreeResults = true;
	}

	int shapeVal;
	int i, j;
	long numpoints;
	
	ShpfileType shpType2D = Utility::ShapeTypeConvert2D(_shpfiletype);;

	// --------------------------------------------------
	//	Point Selection
	// --------------------------------------------------
	if( b_minX == b_maxX && b_minY == b_maxY )
	{
		if( shpType2D == SHP_POLYGON )
		{	
			for(i=0;i<local_numShapes;i++)
			{
				if (useSpatialIndexResults) 
				{
					shapeVal = (res->getValue(i)) - 1;
				}
				else if (useQTreeResults)
				{
					shapeVal = qtreeResult[i];
				}
				else
				{
					shapeVal = i;
				}

				if( ShapefileHelper::PointInPolygon( this, shapeVal, b_minX, b_minY ) )
				{	
					selectResult.push_back( shapeVal );
					continue;														
				}
			}
		}		
	}
	//	Rectangle selection
	else
	{	
		for(i=0;i<local_numShapes;i++)
		{	
			if (useSpatialIndexResults) 
			{
				shapeVal = (res->getValue(i)) - 1;
			}
			else if (useQTreeResults)
			{
				shapeVal = qtreeResult[i];
			}
			else
			{
				shapeVal = i;
			}

			// bounds
			if (this->QuickExtentsCore(shapeVal, &s_minX, &s_minY, &s_maxX, &s_maxY))
			{
				// check bounds (maybe they don't touch at all)
				if( s_maxX < b_minX || 
					s_minX > b_maxX ||							
					s_maxY < b_minY || 
					s_minY > b_maxY )
				{
						continue;
				}
				
				// inclusion (works for every shape type)
				if( s_minX >= b_minX && s_minX <= b_maxX &&
					s_maxX >= b_minX && s_maxX <= b_maxX &&
					s_minY >= b_minY && s_minY <= b_maxY &&
					s_maxY >= b_minY && s_maxY <= b_maxY )
					{	
						selectResult.push_back( shapeVal );
						continue;
					}
			}

			if( shpType2D == SHP_POLYLINE && SelectMode == INTERSECTION)
			{
				if( DefineShapePoints( shapeVal, ShapeType, parts, xPts, yPts ) != FALSE )
				{
					if (SelectionHelper::PolylineIntersection(xPts, yPts, parts, b_minX, b_maxX, b_minY, b_maxY, Tolerance))
						selectResult.push_back( shapeVal );
				}		
			}
			else if( shpType2D == SHP_POLYGON && SelectMode == INTERSECTION)
			{		

				if( DefineShapePoints( shapeVal, ShapeType, parts, xPts, yPts) )
				{
					if (SelectionHelper::PolygonIntersection(xPts, yPts, parts, b_minX, b_maxX, b_minY, b_maxY, Tolerance))
						selectResult.push_back( shapeVal );

					if (ShapefileHelper::BoundsWithinPolygon(this, shapeVal, b_minX, b_maxX, b_minY, b_maxY))
						selectResult.push_back(shapeVal);
				}	
			}
			else if( shpType2D == SHP_MULTIPOINT && SelectMode == INTERSECTION)
			{	
				if( this->DefineShapePoints( shapeVal, ShapeType, parts, xPts, yPts ) != FALSE )
				{	
					numpoints=xPts.size();
					bool addShape = false;
					for(j=0;j<numpoints;j++ )
					{
						double px = xPts[j];
						double py = yPts[j];

						if( px >= b_minX && px <= b_maxX )
						{	
							if( py >= b_minY && py <= b_maxY )
							{	
								addShape = true;
								break;
							}
						}
					}
					if (addShape)
					{
						selectResult.push_back( shapeVal );
						continue;
					}
				}
			}
		}
	}

	if (useSpatialIndexResults)
	{
		delete res;
	}
	else if( _useQTree == VARIANT_TRUE && _isEditingShapes != FALSE)
	{
		qtreeResult.clear();
	}
	return true;
}
#pragma endregion

#pragma region Selection
/***********************************************************************/
/*		ShapeSelected()
/***********************************************************************/
//  Returns and sets the selection state for a shape.
STDMETHODIMP CShapefile::get_ShapeSelected(long ShapeIndex, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size()) //_numShapes)
	{	
		*pVal = VARIANT_FALSE;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
		*pVal = _shapeData[ShapeIndex]->selected; 
	return S_OK;
}
STDMETHODIMP CShapefile::put_ShapeSelected(long ShapeIndex, VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size()) //_numShapes)
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		_shapeData[ShapeIndex]->selected = newVal == VARIANT_TRUE?true:false;
	}
	return S_OK;
}


// *************************************************************
//		get_NumSelected
// *************************************************************
STDMETHODIMP CShapefile::get_NumSelected(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	long count = 0;
	for(int i =0; i < (int)_shapeData.size(); i++)
	{
		if ( _shapeData[i]->selected )
			count++;
	}
	*pVal = count;	
	return S_OK;
}

// *************************************************************
//     SelectAll()
// *************************************************************
STDMETHODIMP CShapefile::SelectAll()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	for(int i =0; i < (int)_shapeData.size(); i++)
		_shapeData[i]->selected = true;

	return S_OK;
}

// *************************************************************
//     SelectNone()
// *************************************************************
STDMETHODIMP CShapefile::SelectNone()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for(int i =0; i < (int)_shapeData.size(); i++)
		_shapeData[i]->selected = false;

	return S_OK;
}

// *************************************************************
//     InvertSelection()
// *************************************************************
STDMETHODIMP CShapefile::InvertSelection()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for(int i =0; i < (int)_shapeData.size(); i++)
		_shapeData[i]->selected = !_shapeData[i]->selected;

	return S_OK;
}
#pragma endregion

// **************************************************************
//		DefineShapePoints
// **************************************************************
// A fast method to retrieve bounds of the given shape
BOOL CShapefile::DefineShapePoints(long ShapeIndex, ShpfileType & ShapeType, std::vector<long> & parts, std::vector<double> & xPts, std::vector<double> & yPts )
{	
	long numPoints;
	long numParts;

	parts.clear();
	xPts.clear();
	yPts.clear();

	if( _isEditingShapes != FALSE )
	{	
		IShape * shape = NULL;
		shape = _shapeData[ShapeIndex]->shape;
		return ShapeHelper::get_MemShapePoints(shape, ShapeType, parts, xPts, yPts);
	}
	else		// not editing
	{	
		//Get the Info from the disk
		fseek(_shpfile,_shpOffsets[ShapeIndex],SEEK_SET);

		int intbuf;
		fread(&intbuf,sizeof(int),1,_shpfile);
		Utility::swapEndian((char*)&intbuf,sizeof(int));

		// shape records are 1 based
		if( intbuf != ShapeIndex + 1 && intbuf != ShapeIndex )
		{	
			ErrorMessage(tkINVALID_SHP_FILE);
			return FALSE;
		}
		else
		{	
			fread(&intbuf,sizeof(int),1,_shpfile);
			Utility::swapEndian((char*)&intbuf,sizeof(int));
			int contentLength = intbuf*2;			//(16 to 32 bit words)

			if( contentLength <= 0 )
				return FALSE;

			char * cdata = new char[contentLength];
			fread(cdata,sizeof(char),contentLength,_shpfile);
			int * intdata = (int*)cdata;						
			ShpfileType shapetype = (ShpfileType)intdata[0];
			double * pntdata;

			if( shapetype == SHP_NULLSHAPE )
			{	
				ShapeType = shapetype;
				return FALSE;
			}
			else if( shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM )
			{	
				int * begOfPts = &(intdata[1]);
				pntdata=(double*)begOfPts;				
				xPts.push_back(pntdata[0]);
				yPts.push_back(pntdata[1]);
				ShapeType = shapetype;
			}
			else if( shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM )
			{					
				numParts=intdata[9];
				numPoints=intdata[10];

				if( numPoints < 2 )
					return FALSE;

				// fill up parts: polyline must have at least 1 part			
				if( numParts > 0 )
				{	
					for( int p = 0; p < numParts; p++ )
						parts.push_back(intdata[11+p]);					
				}
				else
					parts.push_back(0);

				// fill up xPts and yPts
				int * begOfPts = &(intdata[11+numParts]);
				double * pntdata = (double*)begOfPts;
				int idx=0;
				for( int i = 0; i < numPoints; i++ )
				{	
					idx=2*i;
					xPts.push_back(pntdata[idx]);
					yPts.push_back(pntdata[idx+1]);
				}
				ShapeType = shapetype;
			}
			else if( shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM )
			{
				numParts=intdata[9];
				numPoints=intdata[10];

				if( numPoints < 2 )
					return FALSE;

				// fill up parts: polygon must have at least 1 part			
				if( numParts > 0 )
				{	
					for( int p = 0; p < numParts; p++ )
						parts.push_back(intdata[11+p]);								
				}
				else
					parts.push_back(0);

				// fill up xPts and yPts
				int * begOfPts = &(intdata[11+numParts]);
				double * pntdata = (double*)begOfPts;
				int idx=0;
				for( int i = 0; i < numPoints; i++ )
				{	idx=i*2;
				xPts.push_back(pntdata[idx]);
				yPts.push_back(pntdata[idx+1]);
				}

				ShapeType = shapetype;
			}
			else if( shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM )
			{	
				numPoints=intdata[9];

				if( numPoints < 1 )
					return FALSE;

				// fill up xPts and yPts
				int * begOfPts = &(intdata[10]);
				double * pntdata = (double*)begOfPts;
				int idx=0;
				for( int i = 0; i < numPoints; i++ )
				{	idx=i*2;
				xPts.push_back(pntdata[idx]);
				yPts.push_back(pntdata[idx+1]);
				}
				ShapeType = shapetype;
			}
			else
				return FALSE;

			delete [] cdata;
			cdata = NULL;
		}
	}

	return TRUE;
}

