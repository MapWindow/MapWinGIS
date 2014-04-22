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
#include <afxmt.h>				// for CMutex
#include "Shapefile.h"
#include "Templates.h"

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

	bool bPtSelection = false;
    if( b_minX == b_maxX && b_minY == b_maxY )  // Point selection
	{
		bPtSelection  = true;
	}

	if( Tolerance > 0.0 )
	{	
		double halfTolerance = Tolerance*.5;
		b_minX -= halfTolerance;
		b_minY -= halfTolerance;
		b_maxX += halfTolerance;
		b_maxY += halfTolerance;				
	}

	int local_numShapes = _shapeData.size();

	IndexSearching::CIndexSearching *res;
	vector<int> qtreeResult;
	bool useSpatialIndexResults = false;
	bool useQTreeResults = false;

	bool useSpatialIndex = this->get_CanUseSpatialIndexCore(extents); // BoundBox, &useSpatialIndex);
	
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

		int ret = selectShapesFromIndex(spatialIndexID, lowVals, highVals, qType, res);
		if (ret != 0)
		{
			hasSpatialIndex = false;
			spatialIndexLoaded = false;
			IndexSearching::unloadSpatialIndex(spatialIndexID);
			delete res;		//TODO throw error
		}
		else
		{
			local_numShapes = res->getLength();
			useSpatialIndexResults = true;
		}
	}
	else if(_isEditingShapes && useQTree)
	{
		//07/23/2009 ,Neio fix the bug that select shapes in edit mode and add QTree
		if(bPtSelection )
		{
			qtreeResult = m_qtree->GetNodes(QTreeExtent(b_minX,b_minX +1,b_minY + 1,b_minY));
		}
		else
		{
			qtreeResult = m_qtree->GetNodes(QTreeExtent(b_minX,b_maxX,b_maxY,b_minY));
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

				if( this->pointInPolygon( shapeVal, b_minX, b_minY ) )
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
				if( defineShapePoints( shapeVal, ShapeType, parts, xPts, yPts ) != FALSE )
				{
					if (PolylineIntersection(xPts, yPts, parts, b_minX, b_maxX, b_minY, b_maxY, Tolerance))
						selectResult.push_back( shapeVal );
				}		
			}
			else if( shpType2D == SHP_POLYGON && SelectMode == INTERSECTION)
			{		

				if( defineShapePoints( shapeVal, ShapeType, parts, xPts, yPts) )
				{
					if( PolygonIntersection(xPts, yPts, parts, b_minX, b_maxX, b_minY, b_maxY, Tolerance, shapeVal))
						selectResult.push_back( shapeVal );
				}	
			}
			else if( shpType2D == SHP_MULTIPOINT && SelectMode == INTERSECTION)
			{	
				if( this->defineShapePoints( shapeVal, ShapeType, parts, xPts, yPts ) != FALSE )
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
	else if( useQTree == VARIANT_TRUE && _isEditingShapes != FALSE)
	{
		qtreeResult.clear();
	}
	return true;
}
#pragma endregion

// *****************************************************
//		PolygonIntersection
// *****************************************************
bool CShapefile::PolygonIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts,
						 double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance, int& shapeVal)
{
	double cy = ( b_minY + b_maxY ) * .5;
	double cx = ( b_minX + b_maxX ) * .5;

	int beg_part = 0;
	int end_part = 0;						
	
	bool selected = false;
	long numparts=parts.size();
	size_t numpoints=xPts.size();						
	
	for(long j=0; j < numparts && !selected;j++)
	{	
		beg_part = parts[j];
		if( beg_part < 0 )
			beg_part = 0;

		if( (int)(parts.size() - 1) > j )
			end_part = parts[j+1];
		else
			end_part = numpoints;																						
		
		for(long k = beg_part; k < end_part - 1; k++)
		{
			double p1x = xPts[k];
			double p1y = yPts[k];
			double p2x = xPts[k+1];
			double p2y = yPts[k+1];

			// test for inclusion p1
			if( p1y <= b_maxY &&
				p1y >= b_minY &&
				p1x <= b_maxX &&
				p1x >= b_minX )
				{
					return true;
				}								

			// test the Y line
			if( ( p1y > b_maxY && p2y > b_maxY ) ||
				( p1y < b_minY && p2y < b_minY ) )
			{	
				continue;
			}
			
			// test the X line
			if( ( p1x > b_maxX && p2x > b_maxX ) ||
				( p1x < b_minX && p2x < b_minX ) )
			{
				continue;
			}

			double dx = p2x - p1x;
			double dy = p2y - p1y;
			
			// check for vertical lines
			if( fabs( dy ) <= Tolerance )
			{
				return true;
			}
			
			// check for horizontal lines
			if( fabs( dx ) <= Tolerance )
			{
				return true;
			}

			// generate the equation of the line							
			double m = dy/dx;
			double b = p1y - m*p1x;

			double pm = -1*(dx/dy);
			double pb = cy - pm*cx;
			double mx = (pb - b)/(m-pm);
			double my = m*mx + b;							

			// test for inclusion mx/my
			if( my <= b_maxY &&
				my >= b_minY &&
				mx <= b_maxX &&
				mx >= b_minX )
				{
					return true;
				}								
		}	
	}

	// test for Reverse inclusion
	if( this->pointInPolygon( shapeVal, b_minX, b_minY ) )
	{	
		return true;
	}
	else if( this->pointInPolygon( shapeVal, b_maxX, b_maxY ) )
	{	
		return true;
	}
	else if( this->pointInPolygon( shapeVal, b_minX, b_maxY ) )
	{	
		return true;
	}
	else if( this->pointInPolygon( shapeVal, b_maxX, b_minY ) )
	{	
		return true;
	}
	return false;
}

// *****************************************************
//		PolylineIntersection
// *****************************************************
bool CShapefile::PolylineIntersection(std::vector<double>& xPts, std::vector<double>& yPts, std::vector<long>& parts,
						  double& b_minX, double& b_maxX, double& b_minY, double& b_maxY, double& Tolerance)
{
	double cy = ( b_minY + b_maxY ) * .5;
	double cx = ( b_minX + b_maxX ) * .5;
	
	int beg_part = 0;
	int end_part = 0;						

	size_t numpoints = xPts.size();
	long numparts=parts.size();
	
	for(long i=0; i < numparts;i++)
	{	
		beg_part = parts[i];
		if( beg_part < 0 ) beg_part = 0;
		end_part = (int)(parts.size() - 1) > i ? parts[i+1] : numpoints;
		
		//for(size_t j = 0; j < numpoints - 1; j++)
		for(long j = beg_part; j < end_part - 1; j++)
		{
			double p1x = xPts[j];
			double p1y = yPts[j];
			double p2x = xPts[j+1];
			double p2y = yPts[j+1];

			//Test for inclusion p1
			if( p1y <= b_maxY && p1y >= b_minY && p1x <= b_maxX && p1x >= b_minX )
			{	
				return true;
			}							

			// test the Y line
			if( ( p1y > b_maxY && p2y > b_maxY ) || ( p1y < b_minY && p2y < b_minY ) )
			{	
				continue;
			}
			
			// test the X line
			if( ( p1x > b_maxX && p2x > b_maxX ) || ( p1x < b_minX && p2x < b_minX ) )
			{	
				continue;
			}

			double dx = p2x - p1x;
			double dy = p2y - p1y;
			
			// check for vertical lines
			if( fabs( dy ) <= Tolerance )
			{
				return true;
			}
			// check for horizontal lines
			if( fabs( dx ) <= Tolerance )
			{
				return true;
			}

			// generate the equation of the line							
			double m = dy/dx;
			double b = p1y - m*p1x;

			double pm = -1*(dx/dy);
			double pb = cy - pm*cx;
			double mx = (pb - b)/(m-pm);
			double my = m*mx + b;							

			// test for inclusion mx/my
			if( my <= b_maxY &&
				my >= b_minY &&
				mx <= b_maxX &&
				mx >= b_minX )
			{
				return true;
			}								
		}
	}
	return false;
}

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

#pragma region Utilities
// **************************************************************
//		defineShapePoints
// **************************************************************
// A fast method to retrieve bounds of the given shape
// Used by SelectShapes procedure;
// TODO: embed in CShapeWrapper class
BOOL CShapefile::defineShapePoints(long ShapeIndex, ShpfileType & ShapeType, std::vector<long> & parts, std::vector<double> & xPts, std::vector<double> & yPts )
{	
	double x, y;
	long part;

	long numPoints;
	long numParts;

	parts.clear();
	xPts.clear();
	yPts.clear();
	VARIANT_BOOL vbretval;

	if( _isEditingShapes != FALSE )
	{	
		// -------------------------------------------------------
		// get the Info from the memShapes
		// -------------------------------------------------------
		IShape * shape = NULL;
		ShpfileType shapetype;
		//shape = memShapes[ShapeIndex];
		shape = _shapeData[ShapeIndex]->shape;
		shape->get_ShapeType(&shapetype);
		
		if( shapetype == SHP_NULLSHAPE )
		{	
			ShapeType = shapetype;
			return FALSE;
		}
		else if( shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM )
		{	
			shape->get_NumPoints(&numPoints);
			if( numPoints != 1) return FALSE;
			
			shape->get_XY(0, &x, &y, &vbretval);
			xPts.push_back(x);
			yPts.push_back(y);
			ShapeType = shapetype;
		}
		else if( shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM )
		{	
			shape->get_NumParts(&numParts);
			shape->get_NumPoints(&numPoints);
			if(numPoints < 2)	return FALSE;

			// fill up parts: polyline must have at least 1 part	
			if( numParts > 0 )
			{	
				for( int p = 0; p < numParts; p++ )
				{	
					shape->get_Part(p,&part);				
					parts.push_back(p);
				}
			}
			else
				parts.push_back(0);				
			
			// fill up xPts and yPts
			VARIANT_BOOL vbretval;
			for( int i = 0; i < numPoints; i++ )
			{	
				shape->get_XY(i, &x, &y, &vbretval);
				xPts.push_back(x);
				yPts.push_back(y);
			}
			ShapeType = shapetype;
		}		
		else if( shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM )
		{	
			shape->get_NumParts(&numParts);
			shape->get_NumPoints(&numPoints);
			if(numPoints < 2) return FALSE;

			// fill up parts: polygon must have at least 1 part	
			if( numParts > 0 )
			{	
				for( int p = 0; p < numParts; p++ )
				{	
					shape->get_Part(p,&part);				
					parts.push_back(part);
				}
			}
			else
				parts.push_back(0);
			
			// fill up xPts and yPts
			for( int i = 0; i < numPoints; i++ )
			{	
				shape->get_XY(i, &x, &y, &vbretval);
				xPts.push_back(x);
				yPts.push_back(y);
			}
			
			ShapeType = shapetype;
		}
		else if( shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM )
		{	
			shape->get_NumPoints(&numPoints);
			if( numPoints < 1)	return FALSE;	
			
			// fill up xPts and yPts
			for( int i = 0; i < numPoints; i++ )
			{	
				shape->get_XY(i, &x, &y, &vbretval);
				xPts.push_back(x);
				yPts.push_back(y);
			}
			ShapeType = shapetype;
		}
		else
			return FALSE;		
	}
	else
	{	
		//Get the Info from the disk
		fseek(_shpfile,shpOffsets[ShapeIndex],SEEK_SET);

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
#pragma endregion
