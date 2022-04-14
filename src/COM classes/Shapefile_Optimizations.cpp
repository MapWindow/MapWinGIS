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

#pragma once 
#include "stdafx.h"
#include "Shapefile.h"
#include "ShapefileReader.h"
#include "Shape.h"

#pragma region FastMode
// ************************************************************
//		get_fastMode()
// ************************************************************
STDMETHODIMP CShapefile::get_FastMode (VARIANT_BOOL* retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = _fastMode ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ************************************************************
//	  put_FastMode()
// ************************************************************
STDMETHODIMP CShapefile::put_FastMode (VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (!_isEditingShapes) {
		return S_OK;
	}

	if (newVal != _fastMode) 
	{
		for (unsigned int i = 0; i < _shapeData.size(); i++)
		{
			((CShape*)_shapeData[i]->shape)->put_FastMode(newVal ? true : false);
		}

		_fastMode = newVal;
	}

	return S_OK;
}

// *****************************************************************
//	   ReleaseRenderingData()
// *****************************************************************
void CShapefile::ReleaseRenderingCache()
{
	for (unsigned int i = 0; i < _shapeData.size(); i++) {
		_shapeData[i]->ReleaseRenderingData();
	}
}

#pragma endregion

#pragma region PointOptimizations

// *****************************************************************
//	   get_NumPoints()
// *****************************************************************
STDMETHODIMP CShapefile::get_NumPoints(long ShapeIndex, long *pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = 0;

	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size()) 
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return FALSE;
	}
	
	// we already have the shape data in a cache
	if( _isEditingShapes == TRUE)
	{
		_shapeData[ShapeIndex]->shape->get_NumPoints(pVal);
		return S_OK;
	}

	CSingleLock lock(&_readLock, TRUE);

	// get the Info from the disk
	fseek(_shpfile, _shpOffsets[ShapeIndex], SEEK_SET);
	
	int intbuf;
	fread(&intbuf, sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int));
	
	// shape records are 1 based
	if( intbuf != ShapeIndex + 1 && intbuf != ShapeIndex )
    {
		ErrorMessage(tkINVALID_SHP_FILE);
		return FALSE;
	}
	
	fread(&intbuf,sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int));

	int contentLength = intbuf*2;	//(16 to 32 bit words)
	if (contentLength <= 0) {
		return FALSE;
	}

	long numParts=0, numPoints=0;
	
	char * cdata = new char[contentLength];
	fread(cdata, sizeof(char), contentLength, _shpfile);
	
	int * intdata = (int*)cdata;						
	ShpfileType shapetype = (ShpfileType)intdata[0];
	
	shapetype = ShapeUtility::Convert2D(shapetype);
  	if( shapetype == SHP_NULLSHAPE ) 
	{
		*pVal = NULL;
	}
	else if( shapetype == SHP_POINT)
	{
		*pVal = 1;
	}
	else
	{
		*pVal = intdata[10];
	}

	delete [] cdata;
	cdata = NULL;

	return S_OK;
}

// *****************************************************************
//	   QuickPoint()
// *****************************************************************
//This function does not extract Z or M values!!!!!!!!
STDMETHODIMP CShapefile::QuickPoint(long ShapeIndex, long PointIndex, IPoint **retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		*retval = NULL;
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
	}
	else
	{
		if( _isEditingShapes )
		{
			_shapeData[ShapeIndex]->shape->get_Point(PointIndex, retval);
		}
		else
		{	
			CSingleLock lock(&_readLock, TRUE);

			//Get the Info from the disk
			fseek(_shpfile,_shpOffsets[ShapeIndex],SEEK_SET);

			int intbuf;
			fread(&intbuf,sizeof(int),1,_shpfile);
			ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));

			//Shape records are 1 based
			if( intbuf != ShapeIndex + 1 && intbuf != ShapeIndex )
			{	
				ErrorMessage(tkINVALID_SHP_FILE);
				*retval = NULL;
			}
			else
			{
				bool validPoint = true;
				double x=0.0, y=0.0, z=0.0, m=0.0;
				
				fread(&intbuf,sizeof(int),1,_shpfile);
				ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));
				int contentLength = intbuf*2;	//(16 to 32 bit words)
				
				if( contentLength <= 0 )
					return FALSE;

				long numParts=0, numPoints=0;
				char * cdata = new char[contentLength];
				fread(cdata,sizeof(char),contentLength,_shpfile);
				int * intdata = (int*)cdata;						
				ShpfileType shapetype = (ShpfileType)intdata[0];
				double * pntdata;

				lock.Unlock();

				if( shapetype == SHP_NULLSHAPE )
				{
					*retval = NULL;				
				}
				else if( shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM )
				{	
					if( PointIndex == 0 )
					{
						int * begOfPts = &(intdata[1]);
						pntdata=(double*)begOfPts;				
						x=pntdata[0];
						y=pntdata[1];						
					}
					else
					{	
						*retval = NULL;
						ErrorMessage( tkINDEX_OUT_OF_BOUNDS );
						validPoint = false;					
					}
				}
				else if( shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM )
				{					
					//NumParts,NumPoints
					numParts=intdata[9];
					numPoints=intdata[10];
					
					if( numPoints < 2 )
					{	
						*retval = NULL;
						ErrorMessage( tkINVALID_SHP_FILE );
						validPoint = false;
					}
					else
					{
						if( PointIndex < 0 || PointIndex >= numPoints )
						{	
							*retval = NULL;
							ErrorMessage( tkINDEX_OUT_OF_BOUNDS );
							validPoint = false;		
						}
						else
						{
							int * begOfPts = &(intdata[11+numParts]);
							double * pntdata = (double*)begOfPts;
							int idx=0;
							idx=2*PointIndex;
							x=pntdata[idx];
							y=pntdata[idx+1];
						}
					}
				}
				else if( shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM )
				{
					//NumParts,NumPoints
					numParts=intdata[9];
					numPoints=intdata[10];

					if( numPoints < 2 )
					{	
						*retval = NULL;
						ErrorMessage( tkINVALID_SHP_FILE );
						validPoint = false;
					}
					else
					{
						if( PointIndex < 0 || PointIndex >= numPoints )
						{	
							*retval = NULL;
							ErrorMessage( tkINDEX_OUT_OF_BOUNDS );
							validPoint = false;		
						}
						else
						{
							int * begOfPts = &(intdata[11+numParts]);
							double * pntdata = (double*)begOfPts;
							int idx=0;
							idx=PointIndex*2;
							x=pntdata[idx];
							y=pntdata[idx+1];							
						}
					}
				}
				else if( shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM )
				{	
					//NumPoints
					numPoints=intdata[9];

					if( numPoints < 1 )
					{	
						*retval = NULL;
						ErrorMessage( tkINVALID_SHP_FILE );
						validPoint = false;
					}
					else
					{		
						if( PointIndex < 0 || PointIndex >= numPoints )
						{	
							*retval = NULL;
							ErrorMessage( tkINDEX_OUT_OF_BOUNDS );
							validPoint = false;		
						}
						else
						{
							int * begOfPts = &(intdata[10]);
							double * pntdata = (double*)begOfPts;
							int idx=0;
							idx=PointIndex*2;
							x=pntdata[idx];
							y=pntdata[idx+1];
						}
					}
					
				}
				else
				{	
					ErrorMessage( tkUNSUPPORTED_SHAPEFILE_TYPE );
					*retval = NULL;
				}

				if( validPoint == true )
				{
					ComHelper::CreatePoint(retval);
					(*retval)->put_X(x);
					(*retval)->put_Y(y);
					(*retval)->put_Z(z);
					(*retval)->put_M(m);
				}

				delete [] cdata;
				cdata = NULL;
			}
		}
	}

	return S_OK;
}
#pragma endregion

#pragma region QuickPoints
// *****************************************************************
//	   QuickPoints()
// *****************************************************************
//This function does not extract Z or M values!!!!!!!!
STDMETHODIMP CShapefile::QuickPoints(long ShapeIndex, long *NumPoints, SAFEARRAY ** retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = NULL;
	*NumPoints = 0;

	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		*retval = NULL;
		ErrorMessage( tkINDEX_OUT_OF_BOUNDS );
	}
	else
	{
		if( _isEditingShapes == TRUE )
		{	
			_shapeData[ShapeIndex]->shape->get_NumPoints(NumPoints);
			if( *NumPoints > 0 )
			{
				*retval = SafeArrayCreateVector(VT_R8,0,(*NumPoints)*2);
				SafeArrayLock((*retval));
				register int i;
				double x, y;
				double * pData = NULL;
				pData = (double*)(*retval)->pvData;

				VARIANT_BOOL vbretval;
				for( i = 0; i < *NumPoints; i++ )
				{
					_shapeData[ShapeIndex]->shape->get_XY(i, &x, &y, &vbretval);
					pData[i*2] = x;
					pData[i*2+1] = y;											
				}
				SafeArrayUnlock((*retval));					
			}
		}
		else
		{	
			// get the Info from the disk
			fseek(_shpfile,_shpOffsets[ShapeIndex],SEEK_SET);

			int intbuf;
			fread(&intbuf,sizeof(int),1,_shpfile);
			ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));

			//Shape records are 1 based
			if( intbuf != ShapeIndex + 1 && intbuf != ShapeIndex )
			{	
				ErrorMessage( tkINVALID_SHP_FILE );
				*retval = NULL;
			}
			else
			{
				fread(&intbuf,sizeof(int),1,_shpfile);
				ShapeUtility::SwapEndian((char*)&intbuf,sizeof(int));
				int contentLength = intbuf*2;//(16 to 32 bit words)
				
				if( contentLength <= 0 )
					return FALSE;

				long numParts=0, numPoints=0;
				char * cdata = new char[contentLength];
				fread(cdata,sizeof(char),contentLength,_shpfile);
				int * intdata = (int*)cdata;						
				ShpfileType shapetype = (ShpfileType)intdata[0];
				double * pntdata;

				if( shapetype == SHP_NULLSHAPE )
				{
					*retval = NULL;				
				}
				else if( shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM )
				{	
					int * begOfPts = &(intdata[1]);
					pntdata=(double*)begOfPts;				
					*NumPoints = 1;
					*retval = SafeArrayCreateVector(VT_R8,0,2);
					SafeArrayLock((*retval));
					double * pData = NULL;
					pData = (double*)(*retval)->pvData;
					pData[0] = pntdata[0];
					pData[1] = pntdata[1];
					SafeArrayUnlock((*retval));				
				}
				else if( shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM )
				{					
					//NumParts,NumPoints
					numParts=intdata[9];
					numPoints=intdata[10];
					*NumPoints = numPoints;
					if( numPoints < 2 )
					{	
						ErrorMessage( tkINVALID_SHP_FILE );
						*retval = NULL;
					}
					else
					{
						int * begOfPts = &(intdata[11+numParts]);
						double * pntdata = (double*)begOfPts;												
						*retval = SafeArrayCreateVector(VT_R8,0,(*NumPoints)*2);
						SafeArrayLock((*retval));
						register int i;
						int idx=0;
						double * pData = NULL;
						pData = (double*)(*retval)->pvData;
						for( i = 0; i < *NumPoints; i++ )
						{
							idx=2*i;
							pData[idx] = pntdata[idx];
							pData[idx+1] = pntdata[idx+1];	
						}
						SafeArrayUnlock((*retval));	
					}
				}
				else if( shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM )
				{
					//NumParts,NumPoints
					numParts=intdata[9];
					numPoints=intdata[10];
					*NumPoints = numPoints;
					
					if( numPoints < 3 ) // changed from 2 (don't discard last point dangit!)
					{	
						ErrorMessage( tkINVALID_SHP_FILE );
						*retval = NULL;
					}
					else
					{
						int * begOfPts = &(intdata[11+numParts]);
						double * pntdata = (double*)begOfPts;												
						*retval = SafeArrayCreateVector(VT_R8,0,(*NumPoints)*2);
						SafeArrayLock((*retval));
						register int i;
						int idx=0;
						double * pData = NULL;
						pData = (double*)(*retval)->pvData;
						for( i = 0; i < *NumPoints; i++ )
						{
							idx=2*i;
							pData[idx] = pntdata[idx];
							pData[idx+1] = pntdata[idx+1];	
						}
						SafeArrayUnlock((*retval));													
					}
				}
				else if( shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM )
				{	
					//NumPoints
					numPoints=intdata[9];
					*NumPoints = numPoints;
					
					if( numPoints < 1 )
					{	
						ErrorMessage( tkINVALID_SHP_FILE );
						*retval = NULL;
					}
					else
					{		
						int * begOfPts = &(intdata[10]);
						double * pntdata = (double*)begOfPts;												
						*retval = SafeArrayCreateVector(VT_R8,0,(*NumPoints)*2);
						SafeArrayLock((*retval));
						register int i;
						int idx=0;
						double * pData = NULL;
						pData = (double*)(*retval)->pvData;
						for( i = 0; i < *NumPoints; i++ )
						{
							idx=2*i;
							pData[idx] = pntdata[idx];
							pData[idx+1] = pntdata[idx+1];	
						}
						SafeArrayUnlock((*retval));						
					}
					
				}
				else
				{	
					ErrorMessage( tkUNSUPPORTED_SHAPEFILE_TYPE );
					*retval = NULL;
				}				

				delete [] cdata;
				cdata = NULL;
			}
		}
	}

	return S_OK;
}
#pragma endregion

#pragma region QuickExtents
// *****************************************************************
//	   QuickExtents()
// *****************************************************************
//This function does not extract Z and M values
STDMETHODIMP CShapefile::QuickExtents(long ShapeIndex, IExtents **retval)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = NULL;
	
	Extent ext;
	if (this->QuickExtentsCore(ShapeIndex, ext))
	{
		IExtents* bounds = NULL;
		ComHelper::CreateExtents(&bounds);

		bounds->SetBounds(ext.left, ext.bottom, 0, ext.right, ext.top, 0);
		*retval = bounds;
	}
	return S_OK;
}

// *****************************************************************
//	   QuickExtentsCore()
// *****************************************************************
bool CShapefile::QuickExtentsCore(long ShapeIndex, double* xMin, double* yMin, double* xMax, double* yMax)
{
	Extent ext;
	if (this->QuickExtentsCore(ShapeIndex, ext))
	{
		*xMin = ext.left;
		*xMax = ext.right;
		*yMin = ext.bottom;
		*yMax = ext.top;
		return true;
	}
	else
		return false;
}

// *****************************************************************
//	   QuickExtentsCore()
// *****************************************************************
//This function does not extract Z and M values
bool CShapefile::QuickExtentsCore(long ShapeIndex, Extent& result)
{
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		ErrorMessage( tkINDEX_OUT_OF_BOUNDS );
		return false;
	}
	
	if( _isEditingShapes)
	{		
		CShape* shp = (CShape*)_shapeData[ShapeIndex]->shape;
		shp->get_ExtentsXY(result.left, result.bottom, result.right, result.top);
		return true;
	}

	if (m_globalSettings.cacheShapeRenderingData)
	{
		// it's loaded during the rendering only, as there is instance of ShapefileReader available there
		IShapeData* data = _shapeData[ShapeIndex]->get_RenderingData();
		if (data)
		{
			data->get_BoundsXY(result.left, result.right, result.bottom, result.top);
			return true;
		}
	}
		
	return ReadShapeExtents(ShapeIndex, result);
}

// *****************************************************************
//	   ReadShapeExtents()
// *****************************************************************
bool CShapefile::ReadShapeExtents(long ShapeIndex, Extent& result)
{
	CSingleLock lock(&_readLock, TRUE);

	//Get the Info from the disk
	fseek(_shpfile, _shpOffsets[ShapeIndex], SEEK_SET);

	int intbuf;
	fread(&intbuf, sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int));

	//Shape records are 1 based
	if (intbuf != ShapeIndex + 1 && intbuf != ShapeIndex)
	{
		ErrorMessage(tkINVALID_SHP_FILE);
		return false;
	}
	
	fread(&intbuf, sizeof(int), 1, _shpfile);
	ShapeUtility::SwapEndian((char*)&intbuf, sizeof(int));
	int contentLength = intbuf * 2;//(16 to 32 bit words)

	if (contentLength <= 0)
		return FALSE;

	bool bSuccess = false;

	char * cdata = new char[contentLength];
	fread(cdata, sizeof(char), contentLength, _shpfile);
	int * intdata = (int*)cdata;
	ShpfileType shapetype = (ShpfileType)intdata[0];
	double * bnds;

	lock.Unlock();

	if (shapetype == SHP_NULLSHAPE)
	{
		bSuccess = false;
	}
	else if (shapetype == SHP_POINT || shapetype == SHP_POINTZ || shapetype == SHP_POINTM)
	{
		int * begOfPts = &(intdata[1]);
		bnds = (double*)begOfPts;
		result.left = bnds[0];
		result.bottom = bnds[1];
		result.right = bnds[0];
		result.top = bnds[1];
		bSuccess = true;
	}
	else if (shapetype == SHP_POLYLINE || shapetype == SHP_POLYLINEZ || shapetype == SHP_POLYLINEM ||
		shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM ||
		shapetype == SHP_MULTIPOINT || shapetype == SHP_MULTIPOINTZ || shapetype == SHP_MULTIPOINTM)
	{
		bnds = (double*)(&intdata[1]);
		result.left = bnds[0];
		result.bottom = bnds[1];
		result.right = bnds[2];
		result.top = bnds[3];
		bSuccess = true;
	}
	else
	{
		ErrorMessage(tkUNSUPPORTED_SHAPEFILE_TYPE);
		bSuccess = false;
	}

	// make sure to always delete allocation
	delete[] cdata;
	cdata = NULL;
	// return status
	return bSuccess;
}

#pragma endregion