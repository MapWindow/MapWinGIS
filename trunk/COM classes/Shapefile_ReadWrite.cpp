//********************************************************************************************************
//File name: Shapefile.cpp
//Description: Implementation of the CShapefile (see other cpp files as well)
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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

#pragma once 
#include "stdafx.h"
#include "Shapefile.h"
//#include "UtilityFunctions.h"

#pragma region GetShape
// ************************************************************
//		get_Shape()
// ************************************************************
STDMETHODIMP CShapefile::get_Shape(long ShapeIndex, IShape **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	VARIANT_BOOL vbretval;

	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*pVal = NULL;
	}
	else
	{
		if (_isEditingShapes)
		{
			*pVal = _shapeData[ShapeIndex]->shape;
			_shapeData[ShapeIndex]->shape->AddRef();
			return S_OK;
		}
		else
		{
			if (_fastMode)
			{
				fseek(_shpfile,shpOffsets[ShapeIndex],SEEK_SET);
					
				// read the shp from disk			
				int intbuf;
				fread(&intbuf,sizeof(int),1,_shpfile);
				Utility::swapEndian((char*)&intbuf,sizeof(int));

				if( intbuf != ShapeIndex + 1 )
				{	
					*pVal = NULL;
					ErrorMessage(tkINVALID_SHP_FILE);
					return S_OK;
				}
				else
				{	
					fread(&intbuf,sizeof(int),1,_shpfile);
					Utility::swapEndian((char*)&intbuf,sizeof(int));
					int contentLength = intbuf * 2;	//(32 bit words)

					// *2: for conversion from 16-bit words to 8-bit words
					// -2: skip first 2 int - it's record number and content length;
					int length = contentLength; //- 2 * sizeof(int);

					char* shapeData = new char[length];
					int count = (int)fread(shapeData, sizeof(char), length, _shpfile);
					
					if (count != length) 
					{
						*pVal = NULL;
						return S_OK;
					}
					
					CShapeWrapper* shp = new CShapeWrapper(shapeData);
					delete[] shapeData;

					if (shp != NULL)
					{
						IShape* shape = NULL;
						CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
						shape->put_GlobalCallback(globalCallback);
						//((CShape*)shape)->put_fastMode(true);
						//((CShape*)shape)->put_fastModeAdd(true);
						((CShape*)shape)->put_ShapeWrapper(shp);
						*pVal = shape;
					}
					return S_OK;
				}
			}
			else
			{
				fseek(_shpfile,shpOffsets[ShapeIndex],SEEK_SET);
				// read the shp from disk			
				int intbuf;
				fread(&intbuf,sizeof(int),1,_shpfile);
				Utility::swapEndian((char*)&intbuf,sizeof(int));

				// shape records are 1 based - Allow for a mistake
				if( intbuf != ShapeIndex + 1 && intbuf != ShapeIndex )
				{	
					*pVal = NULL;
					ErrorMessage(tkINVALID_SHP_FILE);
				}
				else
				{	
					fread(&intbuf,sizeof(int),1,_shpfile);
					Utility::swapEndian((char*)&intbuf,sizeof(int));
					int contentLength = intbuf * 2;//(32 bit words)

					fread(&intbuf,sizeof(int),1,_shpfile);
					ShpfileType shpType = (ShpfileType)intbuf;
					
					IShape * shape = NULL;
					IPoint * pnt = NULL;
#pragma region Nullshape
					// ------------------------------------------------------
					//	  Shape specific record contents
					// ------------------------------------------------------
					if( _shpfiletype == SHP_NULLSHAPE )
					{	
						if( shpType != SHP_NULLSHAPE )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							*pVal = shape;					
						}
					}
#pragma endregion
#pragma region Point						
					// ------------------------------------------------------
					//	  SHP_POINT
					// ------------------------------------------------------
					else if( _shpfiletype == SHP_POINT )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POINT )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_POINT )
							{	
								m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
								//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
								pnt->put_GlobalCallback(globalCallback);
								
								double x, y;							
								fread(&x,sizeof(double),1,_shpfile);
								fread(&y,sizeof(double),1,_shpfile);
								pnt->put_X(x);
								pnt->put_Y(y);
								
								long pointIndex = 0;
								shape->InsertPoint(pnt, &pointIndex, &vbretval);
								if( vbretval == VARIANT_FALSE )
								{	
									*pVal = NULL;
									shape->Release();
									return S_OK;
								}
								pnt->Release();
							}						
							*pVal = shape;
						}
					}

					// ------------------------------------------------------
					//	  SHP_POINTZ
					// ------------------------------------------------------
					else if( _shpfiletype == SHP_POINTZ )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POINTZ )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_POINTZ )
							{	
								m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
								//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
								pnt->put_GlobalCallback(globalCallback);
								
								double x, y, z, m;							
								fread(&x,sizeof(double),1,_shpfile);
								fread(&y,sizeof(double),1,_shpfile);
								fread(&z,sizeof(double),1,_shpfile);
								fread(&m,sizeof(double),1,_shpfile);
								pnt->put_X(x);
								pnt->put_Y(y);
								pnt->put_Z(z);
								pnt->put_M(m);
								
								long pointIndex = 0;
								shape->InsertPoint(pnt, &pointIndex, &vbretval);
								if( vbretval == VARIANT_FALSE )
								{	
									*pVal = NULL;
									shape->Release();
									return S_OK;
								}
								pnt->Release();
							}						
							*pVal = shape;
						}
					}

					// ------------------------------------------------------
					//	  SHP_POINTM
					// ------------------------------------------------------
					else if( _shpfiletype == SHP_POINTM )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POINTM )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_POINTM )
							{	
								m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
								//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
								pnt->put_GlobalCallback(globalCallback);
								
								double x, y, m;							
								fread(&x,sizeof(double),1,_shpfile);
								fread(&y,sizeof(double),1,_shpfile);
								fread(&m,sizeof(double),1,_shpfile);
								pnt->put_X(x);
								pnt->put_Y(y);
								pnt->put_M(m);
								
								long pointIndex = 0;
								shape->InsertPoint(pnt, &pointIndex, &vbretval);
								if( vbretval == VARIANT_FALSE )
								{	
									*pVal = NULL;
									shape->Release();
									return S_OK;
								}
								pnt->Release();
							}						
							*pVal = shape;
						}
					}
#pragma endregion
#pragma region Polyline
					// ------------------------------------------------------
					//	  SHP_POLYLINE
					// ------------------------------------------------------
					else if( _shpfiletype == SHP_POLYLINE )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POLYLINE )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_POLYLINE )
							{	
								VARIANT_BOOL retval;
								double bx, by;
								int numParts;
								int numPoints;														
								int part;
								double x, y;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numParts,sizeof(int),1,_shpfile);
								fread(&numPoints,sizeof(int),1,_shpfile);
								
								long partIndex = 0;
								for( int i = 0; i < numParts; i++ )
								{	
									fread(&part,sizeof(int),1,_shpfile);
									partIndex = i;
									shape->InsertPart(part,&partIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	
										*pVal = NULL;
										shape->Release();
										return S_OK;
									}
								}
								
								long pointIndex = 0;
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	
										*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}			

					// ------------------------------------------------------
					//	  SHP_POLYLINEZ
					// ------------------------------------------------------
					else if( _shpfiletype == SHP_POLYLINEZ )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POLYLINEZ )
						{	
							ErrorMessage (tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							if( shpType == SHP_POLYLINEZ )
							{	
								VARIANT_BOOL retval;
								double bx, by, bz, bm;
								int numParts;
								int numPoints;														
								int part;
								double x, y, z, m;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numParts,sizeof(int),1,_shpfile);
								fread(&numPoints,sizeof(int),1,_shpfile);
								
								long partIndex = 0;
								for( int i = 0; i < numParts; i++ )
								{	fread(&part,sizeof(int),1,_shpfile);
									partIndex = i;
									shape->InsertPart(part,&partIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
								}
								
								long pointIndex = 0;
								//Read the x, y part of the point
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}

								fread(&bz,sizeof(double),1,_shpfile);
								fread(&bz,sizeof(double),1,_shpfile);

								for( int k = 0; k < numPoints; k++ )
								{	fread(&z,sizeof(double),1,_shpfile);
									pointIndex = k;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_Z(z);
									//Rob Cairns 20-Dec-05
									pnt->Release();
								}

								fread(&bm,sizeof(double),1,_shpfile);
								fread(&bm,sizeof(double),1,_shpfile);

								for( int mc = 0; mc < numPoints; mc++ )
								{	fread(&m,sizeof(double),1,_shpfile);
									pointIndex = mc;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_M(m);
									//Rob Cairns 20-Dec-05
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}
					
					// ------------------------------------------------------
					//	  SHP_POLYLINEM
					// ------------------------------------------------------				
					else if( _shpfiletype == SHP_POLYLINEM )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POLYLINEM )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_POLYLINEM )
							{	
								VARIANT_BOOL retval;
								double bx, by, bm;
								int numParts;
								int numPoints;														
								int part;
								double x, y, m;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numParts,sizeof(int),1,_shpfile);
								fread(&numPoints,sizeof(int),1,_shpfile);
								
								long partIndex = 0;
								for( int i = 0; i < numParts; i++ )
								{	fread(&part,sizeof(int),1,_shpfile);
									partIndex = i;
									shape->InsertPart(part,&partIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
								}
								
								long pointIndex = 0;
								//Read the x, y part of the point
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}						

								fread(&bm,sizeof(double),1,_shpfile);
								fread(&bm,sizeof(double),1,_shpfile);

								for( int mc = 0; mc < numPoints; mc++ )
								{	fread(&m,sizeof(double),1,_shpfile);
									pointIndex = mc;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_M(m);
									//Rob Cairns 20-Dec-05
									pnt->Release();

								}
							}						
							*pVal = shape;						
						}
					}
#pragma endregion		
#pragma region Polygon
					// ------------------------------------------------------
					//	  SHP_POLYGON
					// ------------------------------------------------------				
					else if( _shpfiletype == SHP_POLYGON )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POLYGON )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_POLYGON )
							{	
								VARIANT_BOOL retval;
								double bx, by;
								int numParts;
								int numPoints;														
								int part;
								double x, y;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numParts,sizeof(int),1,_shpfile);
								fread(&numPoints,sizeof(int),1,_shpfile);
															
								long partIndex = 0;
								for( int i = 0; i < numParts; i++ )
								{	fread(&part,sizeof(int),1,_shpfile);
									partIndex = i;
									shape->InsertPart(part,&partIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
								}
								
								long pointIndex = 0;
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}
					
					// ------------------------------------------------------
					//	  SHP_POLYGONZ
					// ------------------------------------------------------				
					else if( _shpfiletype == SHP_POLYGONZ )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POLYGONZ )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							if( shpType == SHP_POLYGONZ )
							{	
								VARIANT_BOOL retval;
								double bx, by, bz, bm;
								int numParts;
								int numPoints;														
								int part;
								double x, y, z, m;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numParts,sizeof(int),1,_shpfile);
								fread(&numPoints,sizeof(int),1,_shpfile);
															
								long partIndex = 0;
								for( int i = 0; i < numParts; i++ )
								{	fread(&part,sizeof(int),1,_shpfile);
									partIndex = i;
									shape->InsertPart(part,&partIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
								}
								
								//Read the x, y part of the point
								long pointIndex = 0;
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}

								fread(&bz,sizeof(double),1,_shpfile);
								fread(&bz,sizeof(double),1,_shpfile);

								for( int k = 0; k < numPoints - 1; k++ )
								{	fread(&z,sizeof(double),1,_shpfile);
									pointIndex = k;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_Z(z);
									pnt->Release();
								}

								fread(&bm,sizeof(double),1,_shpfile);
								fread(&bm,sizeof(double),1,_shpfile);

								for( int mc = 0; mc < numPoints; mc++ )
								{	fread(&m,sizeof(double),1,_shpfile);
									pointIndex = mc;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_M(m);
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}

					// ------------------------------------------------------
					//	  SHP_POLYGONM
					// ------------------------------------------------------				
					else if( _shpfiletype == SHP_POLYGONM )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_POLYGONM )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							if( shpType == SHP_POLYGONM )
							{	
								VARIANT_BOOL retval;
								double bx, by, bm;
								int numParts;
								int numPoints;														
								int part;
								double x, y, m;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numParts,sizeof(int),1,_shpfile);
								fread(&numPoints,sizeof(int),1,_shpfile);
															
								long partIndex = 0;
								for( int i = 0; i < numParts; i++ )
								{	
									fread(&part,sizeof(int),1,_shpfile);
									partIndex = i;
									shape->InsertPart(part,&partIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	
										*pVal = NULL;
										shape->Release();
										return S_OK;
									}
								}
								
								//Read the x, y part of the point
								long pointIndex = 0;
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}

								fread(&bm,sizeof(double),1,_shpfile);
								fread(&bm,sizeof(double),1,_shpfile);

								for( int mc = 0; mc < numPoints; mc++ )
								{	
									fread(&m,sizeof(double),1,_shpfile);
									pointIndex = mc;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_M(m);
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}

					// ------------------------------------------------------
					//	  SHP_POLYGONM
					// ------------------------------------------------------				
					else if( _shpfiletype == SHP_MULTIPOINT )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_MULTIPOINT )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_MULTIPOINT )
							{	
								VARIANT_BOOL retval;
								double bx, by;
								int numPoints;
								double x, y;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numPoints,sizeof(int),1,_shpfile);							
						
								long pointIndex = 0;
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}
#pragma endregion						
#pragma region Multipoint
					// ------------------------------------------------------
					//	  SHP_MULTIPOINTZ
					// ------------------------------------------------------				
					else if( _shpfiletype == SHP_MULTIPOINTZ )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_MULTIPOINTZ )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_MULTIPOINTZ )
							{	
								VARIANT_BOOL retval;
								double bx, by, bz, bm;
								int numPoints;
								double x, y, z, m;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numPoints,sizeof(int),1,_shpfile);							
						
								long pointIndex = 0;
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);															
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}

								fread(&bz,sizeof(double),1,_shpfile);
								fread(&bz,sizeof(double),1,_shpfile);

								for( int k = 0; k < numPoints; k++ )
								{	
									fread(&z,sizeof(double),1,_shpfile);
									pointIndex = k;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_Z(z);
									pnt->Release();
								}

								fread(&bm,sizeof(double),1,_shpfile);
								fread(&bm,sizeof(double),1,_shpfile);

								for( int mc = 0; mc < numPoints; mc++ )
								{	
									fread(&m,sizeof(double),1,_shpfile);
									pointIndex = mc;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_M(m);
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}
					
					// ------------------------------------------------------
					//	  SHP_MULTIPOINTM
					// ------------------------------------------------------				
					else if( _shpfiletype == SHP_MULTIPOINTM )
					{	
						if( shpType != SHP_NULLSHAPE && shpType != SHP_MULTIPOINTM )
						{	
							ErrorMessage(tkINVALID_SHP_FILE);
							*pVal = NULL;
						}
						else
						{	
							CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
							shape->Create(shpType,&vbretval);
							shape->put_GlobalCallback(globalCallback);
							
							if( shpType == SHP_MULTIPOINTM )
							{	
								VARIANT_BOOL retval;
								double bx, by, bm;
								int numPoints;
								double x, y, m;
								
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);
								fread(&bx,sizeof(double),1,_shpfile);
								fread(&by,sizeof(double),1,_shpfile);

								fread(&numPoints,sizeof(int),1,_shpfile);							
						
								long pointIndex = 0;
								for( int j = 0; j < numPoints; j++ )
								{	
									m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
									//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
									pnt->put_GlobalCallback(globalCallback);
									fread(&x,sizeof(double),1,_shpfile);
									fread(&y,sizeof(double),1,_shpfile);														
									pnt->put_X(x);
									pnt->put_Y(y);
									pointIndex = j;
									shape->InsertPoint(pnt,&pointIndex,&retval);
									if( retval == VARIANT_FALSE )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->Release();
								}						

								fread(&bm,sizeof(double),1,_shpfile);
								fread(&bm,sizeof(double),1,_shpfile);

								for( int mc = 0; mc < numPoints; mc++ )
								{	
									fread(&m,sizeof(double),1,_shpfile);
									pointIndex = mc;
									IPoint * pnt = NULL;
									shape->get_Point(pointIndex, &pnt);
									if( pnt == NULL )
									{	*pVal = NULL;
										shape->Release();
										return S_OK;
									}
									pnt->put_M(m);
									pnt->Release();
								}
							}						
							*pVal = shape;						
						}
					}
#pragma endregion
				}
			}
		}
	}
	return S_OK;
}
#pragma endregion

#pragma region ShxReadingWriting
// **************************************************************
//		readShx()
// **************************************************************
BOOL CShapefile::readShx()
{	
	// guaranteed that .shx file is open		
	rewind(_shxfile);
	shpOffsets.clear();

	// file code
	int intbuf;
	fread(&intbuf,sizeof(int),1,_shxfile);
	Utility::swapEndian((char*)&intbuf,sizeof(int));
	if( intbuf != FILE_CODE )
		return FALSE;	

	// unused
	int unused = UNUSEDVAL;
	for(int i=0; i < UNUSEDSIZE; i++)
	{	
		fread(&intbuf,sizeof(int),1,_shxfile);		
		Utility::swapEndian((char*)&intbuf,sizeof(int));
		if( intbuf != unused )
			return FALSE;
	}

	// file length (16 bit words)
	fread(&intbuf,sizeof(int),1,_shxfile);
	Utility::swapEndian((char*)&intbuf,sizeof(int));
	int filelength = intbuf;
	
	// version
	fread(&intbuf,sizeof(int),1,_shxfile);
	if( intbuf != VERSION )
		return FALSE;

	// shpaefile type
	fread(&intbuf,sizeof(int),1,_shxfile);
	_shpfiletype = (ShpfileType)intbuf;
	
	// bounds
	fread(&_minX,sizeof(double),1,_shxfile);
	fread(&_minY,sizeof(double),1,_shxfile);
	fread(&_maxX,sizeof(double),1,_shxfile);
	fread(&_maxY,sizeof(double),1,_shxfile);
	fread(&_minZ,sizeof(double),1,_shxfile);
	fread(&_maxZ,sizeof(double),1,_shxfile);
	fread(&_minM,sizeof(double),1,_shxfile);
	fread(&_maxM,sizeof(double),1,_shxfile);
	
	int readLength = HEADER_BYTES_16;
	while( readLength < filelength )
	{	
		// offset
		fread(&intbuf,sizeof(int),1,_shxfile);
		Utility::swapEndian((char*)&intbuf,sizeof(int));
		shpOffsets.push_back(intbuf*2);			// convert to (32 bit words)
		
		// content length
		fread(&intbuf,sizeof(int),1,_shxfile);
		Utility::swapEndian((char*)&intbuf,sizeof(int));
		readLength += 4;
	}
	//_numShapes = shpOffsets.size();
	rewind(_shxfile);
	return TRUE;
}

// **************************************************************
//		writeShx()
// **************************************************************
BOOL CShapefile::writeShx(FILE * shx, ICallback * cBack)
{	
	//m_writing = true;
	// guaranteed that .shx file is open	
	rewind(shx);

	// FILE CODE
	void* intbuf;
	int fileCode = FILE_CODE;
	intbuf = (char*)&fileCode;
	Utility::swapEndian((char*)intbuf,sizeof(int));
	fwrite(intbuf,sizeof(int),1,shx);

	// unused
	for(int j = 0; j < UNUSEDSIZE; j++)
	{	
		int unused = UNUSEDVAL;
		intbuf = (char*)&unused;
		Utility::swapEndian((char*)intbuf,sizeof(int));
		fwrite(intbuf,sizeof(int),1,shx);
	}
	
	// FILELENGTH (16 bit words)
	int fileLength = HEADER_BYTES_16 + (int)_shapeData.size() * 4; //_numShapes*4;
	intbuf = (char*)&fileLength;
	Utility::swapEndian((char*)intbuf,sizeof(int));
	fwrite(intbuf, sizeof(int),1,shx);
	
	//VERSION
	int version = VERSION;
	fwrite(&version, sizeof(int),1,shx);
	
	//SHAPEFILE TYPE
	int tmpshapefiletype = (short)_shpfiletype;
	fwrite(&tmpshapefiletype, sizeof(int),1,shx);
	
	//BOUNDS
	double ShapefileBounds[8];
	ShapefileBounds[0] = _minX;
	ShapefileBounds[1] = _minY;
	ShapefileBounds[2] = _maxX;
	ShapefileBounds[3] = _maxY;
	ShapefileBounds[4] = _minZ;
	ShapefileBounds[5] = _maxZ;
	ShapefileBounds[6] = _minM;
	ShapefileBounds[7] = _maxM;	
	fwrite(ShapefileBounds,sizeof(double),8,shx);
	
	int offset = HEADER_BYTES_32;
	long numPoints = 0;
	long numParts = 0;
	ShpfileType shptype;
	IShape * shape = NULL;

	long percent = 0, newpercent = 0;

	shpOffsets.clear();
	int size  = (int)_shapeData.size();
	for( int i = 0; i < size; i++) //_numShapes; i++ )
	{	
		// convert to (32 bit words)
		shpOffsets.push_back(offset);

		void * intbuf;
		int sixteenBitOffset = offset/2;
		intbuf = (char*)&sixteenBitOffset;
		Utility::swapEndian((char*)intbuf,sizeof(int));
		fwrite(intbuf,sizeof(int),1,shx);
		
		get_Shape(i,&shape);
		shape->get_NumPoints(&numPoints);
		shape->get_NumParts(&numParts);
		shape->get_ShapeType(&shptype);
		
		int contentLength = CShapeWrapper::get_ContentLength(shptype, numPoints, numParts);

		offset = offset + RECORD_HEADER_LENGTH_32 + contentLength;		

		contentLength = contentLength/2;
		intbuf = (char*)&contentLength;		

		Utility::swapEndian((char*)intbuf,sizeof(int));
		fwrite(intbuf, sizeof(int),1,shx);																			
		
		shape->Release();

		newpercent = (long)(((i+1.0)/size) * 100); //_numShapes)*100);
		if( newpercent > percent )
		{	
			percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("Writing .shx file"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Writing .shx file"));
		}
	}

	if( cBack != NULL )
	{
		cBack->Progress(OLE2BSTR(key),100,A2BSTR("Complete"));
	}
	else if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key),100,A2BSTR("Complete"));
	}

	fflush(shx);

	return TRUE;
}
#pragma endregion

#pragma region ShpReadingWriting
// **************************************************************
//		writeShp()
// **************************************************************
BOOL CShapefile::writeShp(FILE * shp, ICallback * cBack)
{	
	// guaranteed that .shp file is open	
	rewind(shp);
	
	//FILE_CODE
	void* intbuf;
	int fileCode = FILE_CODE;
	intbuf = (char*)&fileCode;
	Utility::swapEndian((char*)intbuf,sizeof(int));
	fwrite(intbuf,sizeof(int),1,shp);

	//UNUSED
	for(int j = 0; j < UNUSEDSIZE; j++)
	{	
		int unused = UNUSEDVAL;
		intbuf = (char*)&unused;
		Utility::swapEndian((char*)intbuf,sizeof(int));
		fwrite(intbuf,sizeof(int),1,shp);
	}

	//FILELENGTH (32 bit words)	
	long numPoints = 0;
	long numParts = 0;
	long part = 0;
	ShpfileType shptype;
	int filelength = HEADER_BYTES_32;
	IPoint * pnt = NULL;
	IExtents * box = NULL;
	IShape * shape = NULL;
	
	int size  = (int)_shapeData.size();
	for( int i = 0; i < size; i++) //_numShapes; i++ )
	{	
		get_Shape(i,&shape);
		shape->get_NumPoints(&numPoints);
		shape->get_NumParts(&numParts);
		shape->get_ShapeType(&shptype);
		
		int contentLength = CShapeWrapper::get_ContentLength(shptype, numPoints, numParts);
		filelength = filelength + RECORD_HEADER_LENGTH_32 + contentLength;
	
		shape->Release();
	}

	filelength = filelength/2;
	intbuf = (char*)&filelength;
	Utility::swapEndian((char*)intbuf,sizeof(int));
	fwrite(intbuf, sizeof(int),1,shp);
	
	//VERSION
	int version = VERSION;
	fwrite(&version, sizeof(int),1,shp);
	
	//SHAPEFILE TYPE
	int tmpshapefiletype = (short)_shpfiletype;
	fwrite(&tmpshapefiletype, sizeof(int),1,shp);
	
	//BOUNDS
	double ShapefileBounds[8];
	ShapefileBounds[0] = _minX;
	ShapefileBounds[1] = _minY;
	ShapefileBounds[2] = _maxX;
	ShapefileBounds[3] = _maxY;
	ShapefileBounds[4] = _minZ;
	ShapefileBounds[5] = _maxZ;
	ShapefileBounds[6] = _minM;
	ShapefileBounds[7] = _maxM;
	fwrite(ShapefileBounds,sizeof(double),8,shp);

	long percent = 0, newpercent = 0;

	double x = 0.0, y = 0.0, z = 0.0, m = 0.0;
	//int size  = (int)_shapeData.size();
	
	for( int k = 0; k < size; k++) //_numShapes; k++ )
	{	
		get_Shape(k,&shape);
		shape->get_NumPoints(&numPoints);
		shape->get_NumParts(&numParts);
		shape->get_ShapeType(&shptype);
		
		int contentLength = CShapeWrapper::get_ContentLength(shptype, numPoints, numParts);
		int length = contentLength;
		contentLength /= 2;

		//Write the Record Header
		long recNum = k + 1;
		intbuf = (char*)&recNum;
		Utility::swapEndian((char*)intbuf,sizeof(int));
		fwrite(intbuf, sizeof(int),1,shp);
		intbuf = (char*)&contentLength;
		Utility::swapEndian((char*)intbuf,sizeof(int));
		fwrite(intbuf, sizeof(int),1,shp); 
		
		if ( _fastMode )
		{
			// new saving method
			CShapeWrapper* shpWrapper = (CShapeWrapper*)((CShape*)shape)->get_ShapeWrapper();
			shpWrapper->RefreshBounds();
			int* data = shpWrapper->get_ShapeData();
			fwrite(data, sizeof(char), length, shp);  //_shpfile	// 2 int - record number and content length - have been written already
		}
		else
		{
			//Write the Record
			if( shptype == SHP_NULLSHAPE )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
			}
			else if( shptype == SHP_POINT )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Point(0,&pnt);
				pnt->get_X(&x);
				pnt->get_Y(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				pnt->Release();
			}
			else if( shptype == SHP_POINTZ )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Point(0,&pnt);
				pnt->get_X(&x);
				pnt->get_Y(&y);
				pnt->get_Z(&z);
				pnt->get_M(&m);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&z,sizeof(double),1,shp);
				fwrite(&m,sizeof(double),1,shp);
				pnt->Release();
			}
			else if( shptype == SHP_POINTM )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Point(0,&pnt);
				pnt->get_X(&x);
				pnt->get_Y(&y);
				pnt->get_M(&m);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&m,sizeof(double),1,shp);
				pnt->Release();
			}
			else if( shptype == SHP_POLYLINE )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				box->Release();
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numParts,sizeof(int),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);
				for( int p = 0; p < numParts; p++ )
				{	shape->get_Part(p,&part);
					fwrite(&part,sizeof(int),1,shp);
				}
				for( int np = 0; np < numPoints; np++ )
				{	shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);
					pnt->Release();
				}
			}
			else if( shptype == SHP_POLYLINEZ )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numParts,sizeof(int),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);
				
				for( int p = 0; p < numParts; p++ )
				{	
					shape->get_Part(p,&part);
					fwrite(&part,sizeof(int),1,shp);
				}
				for( int np = 0; np < numPoints; np++ )
				{	
					shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);				
					pnt->Release();
				}
				box->get_zMin(&z);
				fwrite(&z,sizeof(double),1,shp);
				box->get_zMax(&z);								
				fwrite(&z,sizeof(double),1,shp);			
				for( int npz = 0; npz < numPoints; npz++ )
				{	
					shape->get_Point(npz,&pnt);
					pnt->get_Z(&z);
					fwrite(&z,sizeof(double),1,shp);
					pnt->Release();
				}
				box->get_mMin(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->get_mMax(&m);
				fwrite(&m,sizeof(double),1,shp);
				for( int npm = 0; npm < numPoints; npm++ )
				{	shape->get_Point(npm,&pnt);
					pnt->get_M(&m);
					fwrite(&m,sizeof(double),1,shp);
					pnt->Release();
				}
				box->Release();	
			}
			else if( shptype == SHP_POLYLINEM )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numParts,sizeof(int),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);
				for( int p = 0; p < numParts; p++ )
				{	shape->get_Part(p,&part);
					fwrite(&part,sizeof(int),1,shp);
				}
				for( int np = 0; np < numPoints; np++ )
				{	
					shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);				
					pnt->Release();
				}			
				box->get_mMin(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->get_mMax(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->Release();
				for( int npm = 0; npm < numPoints; npm++ )
				{	
					shape->get_Point(npm,&pnt);
					pnt->get_M(&m);
					fwrite(&m,sizeof(double),1,shp);
					pnt->Release();
				}
			}
			else if( shptype == SHP_POLYGON )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				box->Release();
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numParts,sizeof(int),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);
				
				for( int p = 0; p < numParts; p++ )
				{	
					shape->get_Part(p,&part);
					fwrite(&part,sizeof(int),1,shp);
				}
				
				for( int np = 0; np < numPoints; np++ )
				{	shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);
					pnt->Release();
				}
			}
			else if( shptype == SHP_POLYGONZ )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numParts,sizeof(int),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);
				
				for( int p = 0; p < numParts; p++ )
				{	
					shape->get_Part(p,&part);
					fwrite(&part,sizeof(int),1,shp);
				}
				
				for( int np = 0; np < numPoints; np++ )
				{	
					shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);
					pnt->Release();
				}
				
				box->get_zMin(&z);
				fwrite(&z,sizeof(double),1,shp);
				box->get_zMax(&z);		
				fwrite(&z,sizeof(double),1,shp);
				
				for( int npz = 0; npz < numPoints; npz++ )
				{	
					shape->get_Point(npz,&pnt);
					pnt->get_Z(&z);
					fwrite(&z,sizeof(double),1,shp);
					pnt->Release();
				}
				
				box->get_mMin(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->get_mMax(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->Release();
				for( int npm = 0; npm < numPoints; npm++ )
				{	
					fwrite(&m,sizeof(double),1,shp);
				}
			}
			else if( shptype == SHP_POLYGONM )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numParts,sizeof(int),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);
				
				for( int p = 0; p < numParts; p++ )
				{	
					shape->get_Part(p,&part);
					fwrite(&part,sizeof(int),1,shp);
				}
				
				for( int np = 0; np < numPoints; np++ )
				{	shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);
					pnt->Release();
				}			
				
				box->get_mMin(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->get_mMax(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->Release();
				
				for( int npm = 0; npm < numPoints; npm++ )
				{	
					shape->get_Point(npm,&pnt);
					pnt->get_M(&m);
					fwrite(&m,sizeof(double),1,shp);
					pnt->Release();
				}
			}
			else if( shptype == SHP_MULTIPOINT )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				box->Release();
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);			
				
				for( int np = 0; np < numPoints; np++ )
				{	
					shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);
					pnt->Release();
				}
			}
			else if( shptype == SHP_MULTIPOINTZ )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);
				
				for( int np = 0; np < numPoints; np++ )
				{	
					shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);				
					pnt->Release();
				}
				
				box->get_zMin(&z);
				fwrite(&z,sizeof(double),1,shp);
				box->get_zMax(&z);		
				fwrite(&z,sizeof(double),1,shp);
				
				for( int npz = 0; npz < numPoints; npz++ )
				{	
					shape->get_Point(npz,&pnt);
					pnt->get_Z(&z);
					fwrite(&z,sizeof(double),1,shp);
					pnt->Release();
				}
				box->get_mMin(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->get_mMax(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->Release();	
				
				for( int npm = 0; npm < numPoints; npm++ )
				{	
					shape->get_Point(npm,&pnt);
					pnt->get_M(&m);
					fwrite(&m,sizeof(double),1,shp);
					pnt->Release();
				}
			}
			else if( shptype == SHP_MULTIPOINTM )
			{	
				int ishptype = shptype;
				fwrite(&ishptype,sizeof(int),1,shp);
				shape->get_Extents(&box);
				box->get_xMin(&x);
				box->get_yMin(&y);
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				box->get_xMax(&x);
				box->get_yMax(&y);			
				fwrite(&x,sizeof(double),1,shp);
				fwrite(&y,sizeof(double),1,shp);
				fwrite(&numPoints,sizeof(int),1,shp);			
				
				for( int np = 0; np < numPoints; np++ )
				{	
					shape->get_Point(np,&pnt);
					pnt->get_X(&x);
					pnt->get_Y(&y);
					fwrite(&x,sizeof(double),1,shp);
					fwrite(&y,sizeof(double),1,shp);
					pnt->Release();
				}
				
				box->get_mMin(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->get_mMax(&m);
				fwrite(&m,sizeof(double),1,shp);
				box->Release();
				
				for( int npm = 0; npm < numPoints; npm++ )
				{	
					shape->get_Point(npm,&pnt);
					pnt->get_M(&m);
					fwrite(&m,sizeof(double),1,shp);
					pnt->Release();
				}
			}
		}

		shape->Release();

		newpercent = (long)(((k+1.0)/size) * 100); //_numShapes)*100);
		if( newpercent > percent )
		{	
			percent = newpercent;
			if( cBack != NULL )
			{
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("Writing .shp file"));
			}
			else if( globalCallback != NULL )
			{
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Writing .shp file"));
			}
		}
	}		

	if( cBack != NULL )
	{
		cBack->Progress(OLE2BSTR(key),100,A2BSTR("Complete"));
	}
	else if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key),100,A2BSTR("Complete"));
	}

	fflush(shp);

	return TRUE;
}
#pragma endregion

