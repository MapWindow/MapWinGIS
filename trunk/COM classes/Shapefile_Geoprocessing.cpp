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

#include "stdafx.h"
#include "Shapefile.h"
#include "GeometryConverter.h"
#include <map>
#include "Varh.h"

#include "Extents.h"
#include "clipper.h"

#include "ogr_spatialref.h"

#ifdef SERIALIZE_POLYGONS
#include <fstream>
#include <iostream>
using namespace std;
#endif

//#define AREA_TOLERANCE 0.001

#pragma region SelectByShapefile

// ******************************************************************
//		SelectByShapefile()
// ******************************************************************
//  Returns numbers(ids) of shapes of this shapefile which are situated 
//	in certain spatial relation to the shapes of the input shapefile. 

STDMETHODIMP CShapefile::SelectByShapefile(IShapefile* sf, tkSpatialRelation Relation, 
											VARIANT_BOOL SelectedOnly,  VARIANT* arr, ICallback* cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	*retval = VARIANT_FALSE;
	
	if(globalCallback == NULL && cBack != NULL)
	{
		globalCallback = cBack;	
		globalCallback->AddRef();
	}

	if( sf == NULL)
	{	
		ErrorMessage( tkUNEXPECTED_NULL_PARAMETER );
		return S_OK;
	} 

	long _numShapes1, _numShapes2;		
	_numShapes1 = _shapeData.size();
	sf->get_NumShapes(&_numShapes2);		
	if (_numShapes1 == 0)return NULL;
	if (_numShapes2 == 0) return NULL;
	
	QTree* qTree = GenerateLocalQTree(this, false);
	if (qTree == NULL) return NULL;

	// ids of selected shapes
	set<long> result;				

	// to avoid converting same shapes to ogr geometry multiple times
	vector<OGRGeometry *> vGeometries;		  
	vGeometries.assign(_numShapes1, NULL);

	vector<ShapeData*>* data = ((CShapefile*)sf)->get_ShapeVector();

	long percent = 0;
	for(long shapeid2 = 0; shapeid2 < _numShapes2; shapeid2++)		
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)shapeid2/_numShapes2)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Calculating..."));
			}
		}
		
		if (SelectedOnly && !(*data)[shapeid2]->selected)
			continue;

		vector<int> shapeIds;
		IExtents* box;
		double xMin, xMax, yMin, yMax, zMin, zMax;
		sf->QuickExtents(shapeid2, &box);
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		box->Release();
		shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));
		
		if (shapeIds.size() > 0)
		{
			IShape* shp2 = NULL;
			OGRGeometry* oGeom2 = NULL;
			sf->get_Shape(shapeid2, &shp2);
			oGeom2 = GeometryConverter::ShapeToGeometry(shp2);
			shp2->Release();

			if (oGeom2 != NULL)
			{
				for (int j=0; j < (int)shapeIds.size(); j++)
				{
					long shapeId1 = shapeIds[j];
					
					// shape wasn't selected so far
					if (result.find(shapeId1) == result.end())	
					{
						OGRGeometry* oGeom1 = NULL;
						
						if (vGeometries[shapeId1] == NULL)
						{
							IShape* shp1 = NULL;
							this->get_Shape(shapeId1, &shp1);
							oGeom1 = GeometryConverter::ShapeToGeometry(shp1);
							shp1->Release();
							vGeometries[shapeId1] = oGeom1;
						}
						else
						{
							oGeom1 = vGeometries[shapeId1];
						}
						
						OGRBoolean res;
						if		(Relation == srContains)	res = oGeom1->Contains(oGeom2);
						else if (Relation == srCrosses)		res = oGeom1->Crosses(oGeom2);
						else if (Relation == srEquals)		res = oGeom1->Equal(oGeom2);
						else if (Relation == srIntersects)	res = oGeom1->Intersect(oGeom2);
						else if (Relation == srDisjoint)	res = oGeom1->Intersect(oGeom2);
						else if (Relation == srOverlaps)	res = oGeom1->Overlaps(oGeom2);
						else if (Relation == srTouches)		res = oGeom1->Touches(oGeom2);
						else if (Relation == srWithin)		res = oGeom1->Within(oGeom2);
						
						if (res)
						{
							result.insert(shapeId1);
						}
					}
				}
				delete oGeom2;
			}
		}
	}
	
	// disjoint is opposite to intersects; so to get it we need to find shapes that do intersect with input
	// and then invert the selection
	if (Relation == srDisjoint)		
	{
		vector<long> v;
		set<long>::iterator p = result.begin();
		bool include;		

		for (int i = 0; i < _numShapes1; i++)
		{
			include = true;	
			while(p != result.end())
			{
				if (*p == i)
					include = false;	// this shape actually intersects, so it can't be disjointed

				if (*p > i)
					break;

				p++;
			}
			if (include)
			{
				v.push_back(i);
			}
		}
		*retval = Utility::Vector2SafeArray(&v, VT_I4, arr);
	}
	else
	{
		*retval = Utility::Set2SafeArray(&result, arr);
	}
	
	//  cleaning
	if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key),100,A2BSTR(""));
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));
	}

	for(int i = 0; i < (int)vGeometries.size(); i++)
	{	
		if (vGeometries[i] !=NULL) 
			delete vGeometries[i];		
	}

	if (qTree !=NULL) 
		delete qTree;

	return S_OK;
}

// ***********************************************************
// 		SelectShapesAlt()
// ***********************************************************
//  Alternative function for selection of shapes which fall in the given 
//	rectangular. Uses quad tree on permanent basis. May become 
//	a substitute for SelectShapes function after some improvements.
//  returns VARIANT_TRUE when shapes were selected, and VARIANT_FALSE if
//	no shapes fell into selection
VARIANT_BOOL CShapefile::SelectShapesAlt(IExtents *BoundBox, double Tolerance, SelectMode SelectMode, VARIANT* arr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VARIANT_BOOL retval = VARIANT_FALSE;
	
	double xMin, xMax, yMin, yMax, zMin, zMax;
	BoundBox->GetBounds(&xMin,&yMin,&zMin,&xMax,&yMax,&zMax);
	
    if( xMin == xMax && yMin == yMax )
	{
		xMax +=1;
		yMax +=1;
	}
	
	QTree* qTree = GenerateLocalQTree(this, false);
	if (qTree == NULL) return NULL;

	if( Tolerance > 0.0 )
	{	xMin = xMin - Tolerance/2;
		yMin = yMin - Tolerance/2;
		xMax = xMax + Tolerance/2;
		yMax = yMax + Tolerance/2;				
	}
	
	BoundBox->SetBounds(xMin, yMin, zMin, xMax, yMax, zMax);

	set<long> results;
	vector<int> shapeIds;
	shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));

	IShape* temp = NULL;
	OGRGeometry* oBox;
	BoundBox->ToShape(&temp);
	oBox = GeometryConverter::ShapeToGeometry(temp);
	temp->Release();

	for(int i=0; i < (int)shapeIds.size(); i++)
	{
		IShape* shp = NULL;
		OGRGeometry* oGeom;
		OGRBoolean res = 0;
		this->get_Shape((long)shapeIds[i], &shp);
		oGeom = GeometryConverter::ShapeToGeometry(shp);
		shp->Release();
		
		res = oBox->Contains(oGeom);
		
		if (!res)
			res = oGeom->Contains(oBox);	

		if( SelectMode == INTERSECTION && !res)
			res = oGeom->Intersect (oBox);

		if (res) results.insert(shapeIds[i]);
		delete oGeom;
	}
	
	if (qTree!=NULL) delete qTree;
	if (oBox!=NULL)	delete oBox;
	retval = Utility::Set2SafeArray(&results, arr);
	return retval;
}
#pragma endregion

#pragma region Dissolve
// *************************************************************
//     Dissolve()
// *************************************************************
//  Merges shapes of the shapefile based on the attribute of the given
// 	field. Shapes with the same attribute are merged into one.
STDMETHODIMP CShapefile::Dissolve(long FieldIndex, VARIANT_BOOL SelectedOnly, IShapefile** sf)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// validating parameters
	long numFields;
	this->get_NumFields(&numFields);

	if( FieldIndex < 0 || FieldIndex >= numFields)
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	} 
	
	LONG numSelected;
	this->get_NumSelected(&numSelected);
	if (numSelected == 0 && SelectedOnly)
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return S_OK;
	}
	
	// creating output shapefile
	USES_CONVERSION;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)sf);
	ShpfileType shpType;
	VARIANT_BOOL vbretval;
	this->get_ShapefileType(&shpType);
	(*sf)->CreateNew(A2BSTR(""), shpType, &vbretval);
	
	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
	{
		(*sf)->put_Projection(pVal);
		SysFreeString(pVal);
	}
	
	// copying dissolve field
	long ind = 0;
	IField * fld = NULL;
	this->get_Field(FieldIndex,&fld);
	(*sf)->EditInsertField(fld, &ind, NULL, &vbretval);
	fld->Release();

	if (_geometryEngine == engineGeos)
	{
		DissolveGEOS(FieldIndex, SelectedOnly, *sf);
	}
	else
	{
		DissolveClipper(FieldIndex, SelectedOnly, *sf);
	}

	long numShapes;
	(*sf)->get_NumShapes(&numShapes);
	if (numShapes <= 0)
	{
		(*sf)->Close(&vbretval);
		(*sf)->Release();
		(*sf) = NULL;
	}
	return S_OK;
}

// *************************************************************
//     DissolveGEOS()
// *************************************************************
void CShapefile::DissolveGEOS(long FieldIndex, VARIANT_BOOL SelectedOnly, IShapefile* sf)
{
	map <CComVariant, vector<GEOSGeometry*>*> shapeMap;
	map <CComVariant, vector<GEOSGeometry*>*>::iterator p;
	CComVariant val;	// VARIANT hasn't got comparison operators and therefore
						// can't be used with assosiative containers
	long percent = 0;
	int size = (int)_shapeData.size();
	for(long i = 0; i < size; i++)
	{
		long newpercent = (long)(((double)i/size)*100);
		if( newpercent > percent )
		{	
			percent = newpercent;
			if( globalCallback != NULL ) 
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Grouping shapes..."));
		}

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		GEOSGeometry* gsGeom = GeometryConverter::Shape2GEOSGeom(shp);
		shp->Release();
		if (gsGeom == NULL) continue;
		
		/*if (!GEOSisValid(gsGeom))
		{
			GEOSGeom_destroy(gsGeom);
			continue;
		}*/
		
		if (gsGeom != NULL)
		{
			this->get_CellValue(FieldIndex, i, &val);
		
			if(shapeMap.find(val) != shapeMap.end())
			{
				shapeMap[val]->push_back(gsGeom);
			}
			else
			{
				vector<GEOSGeometry*>* v = new vector<GEOSGeometry*>;
				v->push_back(gsGeom);
				shapeMap[val] = v;
			}
		}
	}
	
	// saving results							
	long count = 0;	// number of shapes inserted
	int i = 0;		// for progress bar
	percent = 0;
	size = shapeMap.size();

	VARIANT_BOOL vbretval;
	p = shapeMap.begin();

	while(p != shapeMap.end())
	{
		long newpercent = (long)(((double)i/size)*100);
		if( newpercent > percent )
		{	
			percent = newpercent;
			if( globalCallback != NULL ) 
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Merging shapes..."));
		}
		
		GEOSGeometry* gsGeom = GeometryConverter::MergeGeosGeometries(*(p->second), NULL);
		delete p->second;	// deleting the vector

		if (gsGeom != NULL)
		{
			std::vector<IShape*> vShapes;
			if (GeometryConverter::GEOSGeomToShapes(gsGeom, &vShapes))
			{
				for (unsigned int i = 0; i < vShapes.size(); i++)
				{
					IShape* shp = vShapes[i];
					if (shp != NULL)
					{
						sf->EditInsertShape(shp, &count, &vbretval);
						sf->EditCellValue(0, count, (VARIANT)p->first, &vbretval);
						shp->Release();
						count++;
					}
				}
			}
			GEOSGeom_destroy(gsGeom);
		}
		p++;
		i++;
	}

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));
}

// *************************************************************
//     DissolveClipper()
// *************************************************************
void CShapefile::DissolveClipper(long FieldIndex, VARIANT_BOOL SelectedOnly, IShapefile* sf)
{
	map <CComVariant, clipper::Clipper*> shapeMap;
	map <CComVariant, clipper::Clipper*>::iterator p;
	CComVariant val;	// VARIANT hasn't got comparison operators and therefore
						// can't be used with assosiative containers
	long percent = 0;
	int size = (int)_shapeData.size();
	std::vector<TPolyPolygon*> polygons;
	polygons.resize(size, NULL);

	GeometryConverter ogr;

	OGRSpatialReference* reference = this->get_OGRSpatialReference();
	if (reference)
	{
		if (reference->IsGeographic() != 0)
		{
			ogr.conversionFactor = m_globalSettings.clipperGcsMultiplicationFactor;	
		}
		delete reference;
	}

	for(long i = 0; i < size; i++)
	{
		long newpercent = (long)(((double)i/size)*100); 
		if( newpercent > percent )
		{	
			percent = newpercent;
			if( globalCallback != NULL ) 
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Merging shapes..."));
		}

		if (SelectedOnly && !_shapeData[i]->selected)
			continue;

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		
		TPolyPolygon* poly = ogr.Shape2ClipperPolygon(shp);
		shp->Release();

		if (poly == NULL) continue;

		this->get_CellValue(FieldIndex, i, &val);
		
		if(shapeMap.find(val) != shapeMap.end())
		{
			shapeMap[val]->AddPolyPolygon(*poly, ptClip);
			polygons[i] = poly;
		}
		else
		{
			shapeMap[val] = new clipper::Clipper();
			shapeMap[val]->AddPolyPolygon(*poly, ptClip);
			polygons[i] = poly;
		}
	}

	// perform clipping and saving the results
	VARIANT_BOOL vbretval;
	long count = 0;
	p = shapeMap.begin();
	while(p != shapeMap.end())
	{
		IShape* shp = NULL; 
		TPolyPolygon result;
		clipper::Clipper* clip = p->second;
		if (clip)
		{
			clip->Execute(ctUnion, result);
			shp = ogr.ClipperPolygon2Shape(&result);

			if (shp)
			{
				long numPoints;
				shp->get_NumPoints(&numPoints);
				if (numPoints > 0)
				{
					sf->EditInsertShape(shp, &count, &vbretval);
					sf->EditCellValue(0, count, (VARIANT)p->first, &vbretval);
					count++;
				}
				shp->Release();
			}
			delete p->second;
		}
		p++;
	}

	// deleting the polygons
	for (int i = 0; i < size; i++)
	{
		if (polygons[i])
		{
			delete polygons[i];
		}
	}

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	shapeMap.clear();
}
#pragma endregion

#pragma region Buffer
// ********************************************************************
//		BufferByDistance()
// ********************************************************************
STDMETHODIMP CShapefile::BufferByDistance(double Distance, LONG nSegments, VARIANT_BOOL SelectedOnly, VARIANT_BOOL MergeResults, IShapefile** sf)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	LONG numSelected;
	this->get_NumSelected(&numSelected);
	if (numSelected == 0 && SelectedOnly)
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return S_OK;
	}
	
	// creating output shapefile
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)sf);
	VARIANT_BOOL vbretval;
	USES_CONVERSION;
	(*sf)->CreateNewWithShapeID(A2BSTR(""), SHP_POLYGON, &vbretval);
	
	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
		(*sf)->put_Projection(pVal);
	
	// perform buffering
	int size = _shapeData.size();
	long count = 0;
	long percent = 0;
	
	std::vector<GEOSGeometry*> results;
	results.reserve(size);

	for (long i = 0; i < size; i++)
	{
		if( globalCallback) 
		{
			long newpercent = (long)(((double)i/size)*100); 
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Buffering shapes..."));
			}
		}

		if (SelectedOnly && !_shapeData[i]->selected)
			continue;
		
		IShape* shp = NULL;
		GEOSGeometry* oGeom1 = NULL;
		GEOSGeometry* oGeom2 = NULL;

		this->get_Shape(i, &shp);
		if (shp)
		{
			oGeom1 = GeometryConverter::Shape2GEOSGeom(shp);

			shp->Release();
			if (oGeom1 == NULL) continue;
			
			oGeom2 = GEOSBuffer( oGeom1, Distance, (int)nSegments);
			GEOSGeom_destroy(oGeom1);

			if (oGeom2 == NULL)	continue;
			
			if (MergeResults)
			{
				results.push_back(oGeom2);
			}
			else
			{
				vector<IShape*> vShapes;
				if (GeometryConverter::GEOSGeomToShapes(oGeom2, &vShapes))
				{
					for (unsigned int j = 0; j < vShapes.size(); j++ )
					{
						IShape* shp = vShapes[j];
						(*sf)->EditInsertShape(shp, &count, &vbretval);
						shp->Release();
						// shapeid will be set automatically
						count++;
					}
				}
				GEOSGeom_destroy(oGeom2);
			}
		}
	}
	
	// merging the results
	if (MergeResults)
	{
		GEOSGeometry* gsGeom = GeometryConverter::MergeGeosGeometries(results, globalCallback);	// geometries will be released in the process
		
		if (gsGeom != NULL)		// the result should always be in g1
		{
			OGRGeometry* oGeom = OGRGeometryFactory::createFromGEOS(gsGeom);
			GEOSGeom_destroy(gsGeom);
			
			if (oGeom)
			{
				OGRwkbGeometryType type = oGeom->getGeometryType();
				if (type == wkbMultiPolygon || type == wkbMultiPolygon25D)
				{
					std::vector<OGRGeometry*> polygons;
					if (GeometryConverter::MultiPolygon2Polygons(oGeom, &polygons))
					{
						for (unsigned int i = 0; i < polygons.size(); i++)
						{
							IShape* shp = GeometryConverter::GeometryToShape(polygons[i]);
							if (shp)
							{
								(*sf)->EditInsertShape(shp, &count, &vbretval);
								shp->Release();
								count++;
							}
						}
					}
				}
				else
				{
					IShape* shp = GeometryConverter::GeometryToShape(oGeom);
					if (shp)
					{
						(*sf)->EditInsertShape(shp, &count, &vbretval);	
						shp->Release();
						count++;
					}
				}
				delete oGeom;
			}
		}
	}
		

	long numShapes;
	(*sf)->get_NumShapes(&numShapes);
	if (numShapes <= 0)
	{
		(*sf)->Close(&vbretval);
		(*sf)->Release();
		(*sf) = NULL;
	}

	if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key),100,A2BSTR(""));
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));
	}
	return S_OK;
}
#pragma endregion

#pragma region Clipping
// ********************************************************************
//		GetDifference()
// ********************************************************************
STDMETHODIMP CShapefile::Difference(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clDifference);
	return S_OK;
}

// ********************************************************************
//		Clip()
// ********************************************************************
STDMETHODIMP CShapefile::Clip(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clClip);	// enumeration should be repaired
	return S_OK;
}

// **********************************************************************
// *	GetIntersection()				             
// **********************************************************************
STDMETHODIMP CShapefile::GetIntersection(VARIANT_BOOL SelectedOnlyOfThis, IShapefile* sf,
										 VARIANT_BOOL SelectedOnly, ShpfileType fileType, ICallback* cBack, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DoClipOperation(SelectedOnlyOfThis, sf, SelectedOnly, retval, clIntersection, fileType);
	return S_OK;
}

// ********************************************************************
//		GetSymmDifference()
// ********************************************************************
STDMETHODIMP CShapefile::SymmDifference(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clSymDifference);	// enumeration should be repaired
	return S_OK;
}

// ********************************************************************
//		GetUnion()
// ********************************************************************
STDMETHODIMP CShapefile::Union(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DoClipOperation(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, retval, clUnion);	// enumeration should be repaired
	return S_OK;
}

// ****************************************************************
//		FieldsAreEqual()
// ****************************************************************
bool CShapefile::FieldsAreEqual(IField* field1, IField* field2)
{
	if (!field1 || !field2)
	{
		return false;
	}
	else
	{
		BSTR name1, name2;
		field1->get_Name(&name1);
		field2->get_Name(&name2);
		
		USES_CONVERSION;
		CString s1 = OLE2CA(name1);
		CString s2 = OLE2CA(name2);

		bool equal = s1 == s2;
		SysFreeString(name1);
		SysFreeString(name2);

		if (!equal)
		{
			return false;
		}
		else
		{
			FieldType type1, type2;
			field1->get_Type(&type1);
			field2->get_Type(&type2);
			
			if (type1 != type2)
			{
				return false;
			}
			else
			{
				if (type1 == INTEGER_FIELD)
				{
					return true;
				}
				else if (type1 == STRING_FIELD)
				{
					long width1, width2;
					field1->get_Width(&width1);
					field2->get_Width(&width2);
					return width1 == width2;
				}
				else // DOUBLE_FIELD
				{
					/*long precision1, precision2;
					field1->get_Precision(&precision1);
					field2->get_Precision(&precision2);
					return precision1 == precision2;*/
					return true;
				}
			}
		}
	}
}

// ****************************************************************
//		CopyFields()
// ****************************************************************
void CShapefile::CopyFields(IShapefile* source, IShapefile* target)
{
	LONG numFields, position;
	VARIANT_BOOL vbretval;
	source->get_NumFields(&numFields);

	for (long i = 0; i < numFields; i++)
	{
		IField * field = NULL;
		IField * fieldNew = NULL;
		source->get_Field(i,&field);
		field->Clone(&fieldNew);
		
		target->get_NumFields(&position);
		target->EditInsertField(fieldNew, &position, NULL, &vbretval);

		field->Release();
		fieldNew->Release();
	}
}

// ****************************************************************
//		CopyFields()
// ****************************************************************
// Exposed publicly
void CShapefile::CopyFields(IShapefile* target)
{
	if (!target)
		return;
	
	LONG numFields, position;
	VARIANT_BOOL vbretval;
	this->get_NumFields(&numFields);

	for (long i = 0; i < numFields; i++)
	{
		IField * field = NULL;
		IField * fieldNew = NULL;
		this->get_Field(i,&field);
		field->Clone(&fieldNew);
		
		target->get_NumFields(&position);
		target->EditInsertField(fieldNew, &position, NULL, &vbretval);

		field->Release();
		fieldNew->Release();
	}
}

// ****************************************************************
//		CopyFields()
// ****************************************************************
void CShapefile::CopyFields(IShapefile* sfSubject, IShapefile* sfOverlay, IShapefile* sfResult, map<long, long>& fieldMap, bool mergeFields)
{
	// fields of the subject shapefile
	LONG numFields, position;
	VARIANT_BOOL vbretval;
	sfSubject->get_NumFields(&numFields);

	CopyFields(sfSubject, sfResult);
	
	// passing the fields of overlay shapefile
	if ( sfOverlay )
	{
		LONG numFields2;
		sfOverlay->get_NumFields(&numFields2);
		for (long i = 0; i < numFields2; i++)
		{
			IField * field1 = NULL;
			sfOverlay->get_Field(i,&field1);
			
			// checking whether we have such field already
			bool found = false;
			if (mergeFields)
			{
				for (int j = 0; j < numFields; j++)
				{
					IField * field2 = NULL;
					sfResult->get_Field(j,&field2);
					if (FieldsAreEqual(field1, field2))
					{
						fieldMap[i] = j;
						found = true;
					}
					field2->Release();
				}
			}
			
			if (!found)
			{
				IField* fieldNew = NULL;
				field1->Clone(&fieldNew);
				sfResult->get_NumFields(&position);
				sfResult->EditInsertField(fieldNew, &position, NULL, &vbretval);
				fieldNew->Release();

				fieldMap[i] = position;
			}
			field1->Release();
		}
		UniqueFieldNames(sfResult);
	}
}

// ********************************************************************
//		DoClipOperarion()
// ********************************************************************
void CShapefile::DoClipOperation(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, 
								 VARIANT_BOOL SelectedOnlyOverlay, IShapefile** retval, 
								 tkClipOperation operation, ShpfileType returnType)
{
	USES_CONVERSION;
	if( sfOverlay == NULL)
	{	
		ErrorMessage( tkUNEXPECTED_NULL_PARAMETER );
		return;
	} 

	// do we have enough shapes, are they selected?
	long numShapes1, numShapes2;		
	numShapes1 = _shapeData.size();
	sfOverlay->get_NumShapes(&numShapes2);		

	if (numShapes1 == 0 || numShapes2 == 0) 
		return;

	LONG numSelected1, numSelected2;
	this->get_NumSelected(&numSelected1);
	sfOverlay->get_NumSelected(&numSelected2);

	if (SelectedOnlySubject && numSelected1 == 0 ||
		SelectedOnlyOverlay && numSelected2 == 0 )
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return;
	}
	
	//#define _DEBUG
	#ifdef _DEBUG
	DWORD startTick = ::GetTickCount();
	#endif

	// we can use clipper for polygons only
	ShpfileType type1, type2;	
	type1 = Utility::ShapeTypeConvert2D(_shpfiletype);
	sfOverlay->get_ShapefileType(&type2);	
	type2 = Utility::ShapeTypeConvert2D(type2);
	bool canUseClipper = (type1 == SHP_POLYGON && type2 == SHP_POLYGON);
	
	// autochoosing the resulting type for intersection
	if (operation == clIntersection && 
		returnType == SHP_NULLSHAPE)
	{
		if (type1 == SHP_POINT || type2 == SHP_POINT ||
			type1 == SHP_MULTIPOINT || type2 == SHP_MULTIPOINT)
		{
			type1 = SHP_POINT;
		}
		else if (type1 == SHP_POLYLINE || type2 == SHP_POLYLINE)
		{
			type1 = SHP_POLYLINE;
		}
		else
		{
			type1 = SHP_POLYGON;
		}
	}

	// creation of resulting shapefile
	VARIANT_BOOL vbretval;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNew(A2BSTR(""), type1, &vbretval);

	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
	{
		(*retval)->put_Projection(pVal);
		SysFreeString(pVal);
	}
	
	// do field mapping
	std::map<long, long> fieldMap;

	// fields from the overlay shapefile will be copied for the limited number od operation only
	IShapefile* sfCopy = (operation == clIntersection || operation == clSymDifference || operation == clUnion)? sfOverlay : NULL;
	CopyFields(this, sfCopy, *retval, fieldMap);

	bool useClipper = (_geometryEngine == engineClipper && canUseClipper);
	
	if (globalCallback)
	{
		globalCallback->QueryInterface(IID_IStopExecution,(void**)&_stopExecution);
	}

	// do calculation by Clipper
	if (useClipper)
	{
		switch (operation)
		{
			case clDifference:
				this->DifferenceClipper(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
				break;
			case clIntersection:
				this->IntersectionClipper(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap);
				break;
			case clClip:
				this->ClipClipper(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
				break;
			case clSymDifference:
				this->DifferenceClipper(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
				this->DifferenceClipper(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap);
				break;
			case clUnion:
				std::set<int> shapesToSkipSubject;
				std::set<int> shapesToSkipClipping;
				this->IntersectionClipper(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap, &shapesToSkipSubject, &shapesToSkipClipping);
				this->DifferenceClipper(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, NULL, &shapesToSkipSubject);
				this->DifferenceClipper(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap, &shapesToSkipClipping);
		}
	}

	// do calculation by GEOS
	switch (operation)
	{	
		case clDifference:
			this->DifferenceGEOS(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
			break;
		case clIntersection:
			this->IntersectionGEOS(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap);
			break;
		case clClip:
			this->ClipGEOS(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
			break;
		case clSymDifference:
			// 2 differences
			this->DifferenceGEOS(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval);
			this->DifferenceGEOS(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap);	
			break;
		case clUnion:
			// intersection + symmetrical difference
			std::set<int> shapesToSkipSubject;
			std::set<int> shapesToSkipClipping;
			this->IntersectionGEOS(SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, &fieldMap, &shapesToSkipSubject, &shapesToSkipClipping );
			this->DifferenceGEOS(this, SelectedOnlySubject, sfOverlay, SelectedOnlyOverlay, *retval, NULL, &shapesToSkipSubject);
			this->DifferenceGEOS(sfOverlay, SelectedOnlyOverlay, this, SelectedOnlySubject, *retval, &fieldMap, &shapesToSkipClipping);	
			break;
	}

	long numShapes;
	(*retval)->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		(*retval)->Close(&vbretval);
		(*retval)->Release();
		(*retval) = NULL;
	}

	//  cleaning
	if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key),100,A2BSTR(""));
	}

	// showing time
	#ifdef _DEBUG
	DWORD endTick = ::GetTickCount();
	float time = (float)(endTick - startTick)/1000.0f;
	CString s;
	s.Format("Calculation time: %.3f sec", time);
	AfxMessageBox(s);
	#endif
}
#pragma endregion

#pragma region Clip
// ********************************************************************
//		ClipGEOS()
// ********************************************************************
void CShapefile::ClipGEOS(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile* sfResult)
{
	// spatial index for clip shapefile
	QTree* qTree = GenerateLocalQTree(sfOverlay, SelectedOnlyOverlay?true:false);
	if (!qTree) 
		return;
	
	long numShapesSubject, numShapesClip;		
	this->get_NumShapes(&numShapesSubject);		
	sfOverlay->get_NumShapes(&numShapesClip);		

	// a cache for clipping geometries
	vector<GEOSGeometry*> vGeometries;
	vGeometries.assign(numShapesClip, NULL);	
	
	std::vector<ShapeData*>* shapeDataClip = ((CShapefile*)sfOverlay)->get_ShapeVector();

	long percent = 0;
	for(long subjectId = 0; subjectId < numShapesSubject; subjectId++)		
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(subjectId + 1)/numShapesSubject)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Clipping shapes..."));
			}
		}

		if (SelectedOnlySubject && !_shapeData[subjectId]->selected)
			continue;
		
		IExtents* box;
		vector<int> shapeIds;
		double xMin, xMax, yMin, yMax, zMin, zMax;
		this->QuickExtents(subjectId, &box);			// extracting subject extents
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		box->Release();
		shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));
		
		if (shapeIds.size() > 0)
		{
			// extracting subject geometry
			IShape* shp1 = NULL;
			this->get_Shape(subjectId, &shp1);
			GEOSGeometry* gsGeom1 = GeometryConverter::Shape2GEOSGeom(shp1);
			shp1->Release();

			if (gsGeom1)
			{
				// iterating through clip geometries preparing their union
				vector<GEOSGeometry*> vUnion;
				GEOSGeometry* gsGeom2 = NULL;

				for (int j = 0; j < (int)shapeIds.size(); j++)
				{
					// user can abort the operation in any time
					if (_stopExecution)
					{
						VARIANT_BOOL stop;
						_stopExecution->StopFunction(&stop);
						if (stop)
						{
							sfResult->EditClear(&stop);
							GEOSGeom_destroy(gsGeom1);
							goto cleaning;
						}
					}
					
					// extracting clip geometry
					long clipId = shapeIds[j];

					if (SelectedOnlyOverlay && !(*shapeDataClip)[clipId]->selected)
						continue;
					
					if (vGeometries[clipId] == NULL)
					{
						// in case we meet this geometry for the first time, we shall convert it
						IShape* shp1 = NULL;
						sfOverlay->get_Shape(clipId, &shp1);
						gsGeom2 = GeometryConverter::Shape2GEOSGeom(shp1);
						vGeometries[clipId] = gsGeom2;
						shp1->Release();
					}
					else
					{					
						// otherwise, we just take it from the cache
						gsGeom2 = vGeometries[clipId];
					}
					
					if (gsGeom2 && GEOSIntersects(gsGeom1, gsGeom2))
					{
						vUnion.push_back(gsGeom2);
					}
				}
				
				// merging (input geometries won't be destroyed)
				gsGeom2 = NULL;
				bool deleteNeeded = false;
				
				if ((int)vUnion.size() > 1)
				{
					gsGeom2 = GeometryConverter::MergeGeosGeometries(vUnion, NULL, false);
					deleteNeeded = true;
				}
				else if ((int)vUnion.size() == 1)
				{
					gsGeom2 = vUnion[0];
				}

				if (gsGeom2)
				{
					GEOSGeometry* gsResult = GEOSIntersection(gsGeom1, gsGeom2);
					if (gsResult != NULL)
					{
						vector<IShape* > vShapes;
						bool result = GeometryConverter::GEOSGeomToShapes(gsResult, &vShapes);
						GEOSGeom_destroy(gsResult);
						
						this->InsertShapesVector(sfResult, vShapes, this, subjectId, NULL);
						//InsertShapesVector(sfResult, vShapes, this, NULL);	// shapes are released here
					}
					
					// clipping geometry should be deleted only in case it was build up from several parts
					if (deleteNeeded)
						GEOSGeom_destroy(gsGeom2);
				}

				// subject geometry should always be deleted
				GEOSGeom_destroy(gsGeom1);
			}
		}
	}
cleaning:	
	// destroying the cache
	for(int i = 0; i < (int)vGeometries.size(); i++)
	{	
		if (vGeometries[i] !=NULL) 
		{
			GEOSGeom_destroy(vGeometries[i]);
		}
	}

	if (qTree !=NULL) 
		delete qTree;
}

// ********************************************************************
//		ClipClipper()
// ********************************************************************
void CShapefile::ClipClipper(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, IShapefile* sfResult)
{
	// spatial index for clip shapefile
	QTree* qTree = GenerateLocalQTree(sfOverlay, SelectedOnlyOverlay?true:false);
	if (qTree == NULL) 
		return;
	
	long numShapesSubject, numShapesClip;		
	this->get_NumShapes(&numShapesSubject);		
	sfOverlay->get_NumShapes(&numShapesClip);	
	
	vector<TPolyPolygon*> vPolygons;		// we shall create vectors for both clipper and GEOS 
	vPolygons.assign(numShapesClip, NULL);	// this won't take much RAM or time
	
	std::vector<ShapeData*>* shapeDataClip = ((CShapefile*)sfOverlay)->get_ShapeVector();

	clipper::Clipper clp; 
	GeometryConverter ogr;

	OGRSpatialReference* reference = ((CShapefile*)this)->get_OGRSpatialReference();
	if (reference)
	{
		if (reference->IsGeographic() != 0)
		{
			ogr.conversionFactor = m_globalSettings.clipperGcsMultiplicationFactor;	// multiplication for geographic coordinate systems
		}
		delete reference;
	}
	
	long percent = 0;
	for(long subjectId =0; subjectId < numShapesSubject; subjectId++)		
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(subjectId + 1)/numShapesSubject)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Clipping shapes..."));
			}
		}

		if (SelectedOnlySubject && !_shapeData[subjectId]->selected)
			continue;
		
		IExtents* box;
		vector<int> shapeIds;
		double xMin, xMax, yMin, yMax, zMin, zMax;
		this->QuickExtents(subjectId, &box);
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		box->Release();
		shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));
		
		if (shapeIds.size() > 0)
		{
			// extracting subject polygon
			IShape* shp1 = NULL;
			this->get_Shape(subjectId, &shp1);
			TPolyPolygon* poly1 = ogr.Shape2ClipperPolygon(shp1);
			shp1->Release();

			if (poly1)
			{
				// extracting clipping polygons
				for (int j = 0; j < (int)shapeIds.size(); j++)
				{
					// user can abort the operation in any time
					if (_stopExecution)
					{
						VARIANT_BOOL stop;
						_stopExecution->StopFunction(&stop);
						if (stop)
						{
							sfResult->EditClear(&stop);
							delete poly1;
							goto cleaning;
						}
					}
					
					long clipId = shapeIds[j];

					if (SelectedOnlyOverlay && !(*shapeDataClip)[clipId]->selected)
						continue;

					vector<IShape* > vShapes;
					
					TPolyPolygon* poly2 = NULL;
					if (vPolygons[clipId] == NULL)
					{
						IShape* shp2 = NULL;
						sfOverlay->get_Shape(clipId, &shp2);
						poly2 = ogr.Shape2ClipperPolygon(shp2);
						vPolygons[clipId] = poly2;
						shp2->Release();
					}
					else
					{
						poly2 = vPolygons[clipId];
					}
					
					if (poly2)
						clp.AddPolyPolygon(*poly2, ptClip);
				}
				
				// in case there are several input polygons, they should be merged before clipping
				if (shapeIds.size() > 1)
				{
					TPolyPolygon polyUnion;
					clp.Execute(ctUnion, polyUnion);
					clp.Clear();
					clp.AddPolyPolygon(polyUnion, ptClip);
				}
				
				// adding subject polygon
				if (poly1)
					clp.AddPolyPolygon(*poly1, ptSubject);

				// do clipping
				TPolyPolygon polyResult;
				if (clp.Execute(ctIntersection, polyResult))
				{
					IShape* shp = ogr.ClipperPolygon2Shape(&polyResult);
					if (shp)
					{
						vector<IShape* > vShapes;
						vShapes.push_back(shp);
						this->InsertShapesVector(sfResult, vShapes, this, subjectId, NULL);
					}
				}
				
				delete poly1;
				clp.Clear();
			}
		}
	}

cleaning:	
	for(int i = 0; i < (int)vPolygons.size(); i++)
	{	
		if (vPolygons[i] !=NULL) 
		{
			delete vPolygons[i];
		}
	}
	delete qTree;
}
#pragma endregion

#pragma region Intersection

// ******************************************************************
//		IntersectionGEOS()
// ******************************************************************
// shapesToExclude - vector to return shapes sum of intersected results for those is equasl to the initial area
// such shapes can be excluded from further calculation in case of union; 
void CShapefile::IntersectionGEOS(VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip, 
								  IShapefile* sfResult, map<long, long>* fieldMap, 
								  std::set<int>* subjectShapesToSkip, 
								  std::set<int>* clippingShapesToSkip)
{
	// spatial index for clip shapefile
	QTree* qTree = GenerateLocalQTree(sfClip, SelectedOnlyClip?true:false);
	if (!qTree) 
		return;
	
	long numShapesSubject, numShapesClip;		
	this->get_NumShapes(&numShapesSubject);		
	sfClip->get_NumShapes(&numShapesClip);		
	
	vector<GEOSGeometry*> vGeometries;
	vGeometries.assign(numShapesClip, NULL);
	
	std::vector<ShapeData*>* shapeDataClip = ((CShapefile*)sfClip)->get_ShapeVector();
	
	// initial areas areas of the clipping shapes
	std::vector<double> initClipAreas;
	// areas of the clipping shapes that were passed to the result:  (int)index of shapes -> (double)area
	std::vector<double> resultClipAreas;

	bool buildSkipLists = (subjectShapesToSkip != NULL && clippingShapesToSkip != NULL && m_globalSettings.shapefileFastUnion);
	
	if (buildSkipLists)
	{
		initClipAreas.resize(numShapesClip, 0.0);
		resultClipAreas.resize(numShapesClip, 0.0);
	}
	
	IGeoProjection* projection = NULL;
	sfClip->get_GeoProjection(&projection);
	VARIANT_BOOL isGeographic = VARIANT_FALSE;
	if (projection)
	{
		projection->get_IsGeographic(&isGeographic);
		projection->Release();
		projection = NULL;
	}
	double AREA_TOLERANCE = m_globalSettings.GetMinPolygonArea(isGeographic) * 0.001;

	long percent = 0;
	for(long subjectId = 0; subjectId < numShapesSubject; subjectId++)		
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(subjectId + 1)/numShapesSubject)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Clipping shapes..."));
			}
		}

		if (SelectedOnlySubject && !_shapeData[subjectId]->selected)
			continue;
		
		IExtents* box = NULL;
		vector<int> shapeIds;
		double xMin, xMax, yMin, yMax, zMin, zMax;
		this->QuickExtents(subjectId, &box);			// extracting subject extents
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		box->Release();
		shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));
		
		if (shapeIds.size() > 0)
		{
			// extracting subject geometry
			IShape* shp1 = NULL;
			this->get_Shape(subjectId, &shp1);
			GEOSGeometry* geom1 = GeometryConverter::Shape2GEOSGeom(shp1);
			
			double initArea = 0.0;
			shp1->get_Area(&initArea);
			shp1->Release();

			if (geom1)
			{
				double sumArea = 0.0;

				// iterating through clip geometries
				for (int j = 0; j < (int)shapeIds.size(); j++)
				{
					// user can abort the operation in any time
					if (_stopExecution)
					{
						VARIANT_BOOL stop;
						_stopExecution->StopFunction(&stop);
						if (stop)
						{
							sfResult->EditClear(&stop);
							GEOSGeom_destroy(geom1);
							goto cleaning;
						}
					}
					
					// extracting clip geometry
					long clipId = shapeIds[j];

					if (SelectedOnlyClip && !(*shapeDataClip)[clipId]->selected)
						continue;

					GEOSGeometry* geom2 = NULL;
					
					if (vGeometries[clipId] == NULL)
					{
						// in case we meet this geometry for the first time, we shall convert it
						IShape* shp2 = NULL;
						sfClip->get_Shape(clipId, &shp2);
						geom2 = GeometryConverter::Shape2GEOSGeom(shp2);
						vGeometries[clipId] = geom2;
						
						if (buildSkipLists)
						{
							double tempArea = 0.0;
							shp2->get_Area(&tempArea);
							initClipAreas[clipId] = tempArea;
						}

						shp2->Release();
					}
					else
					{					
						// otherwise, we just take it from the cache
						geom2 = vGeometries[clipId];
					}

					// calculating intersection
					GEOSGeometry* geom = GEOSIntersection(geom1, geom2);		// don't delete oGeom1 as it will be used on the next loops
					if (geom == NULL) continue;
					
					// saving the results
					vector<IShape* > vShapes;
					bool result = GeometryConverter::GEOSGeomToShapes(geom, &vShapes);
					GEOSGeom_destroy(geom);

					// sum of area to exclude shapes from difference
					if (buildSkipLists)
					{
						for (unsigned int n = 0; n < vShapes.size(); n++)
						{
							double areaTemp;
							vShapes[n]->get_Area(&areaTemp);
							
							// subject shape
							sumArea += areaTemp;

							resultClipAreas[clipId] += areaTemp;
						}
					}

					this->InsertShapesVector(sfResult, vShapes, this, subjectId, NULL, sfClip, clipId, fieldMap);	// shapes are released here
				}
				GEOSGeom_destroy(geom1);
				
				if (buildSkipLists)
				{
					if (fabs(sumArea - initArea) < AREA_TOLERANCE )
					{
						subjectShapesToSkip->insert(subjectId);
					}
				}
			}
		}
	}
	
	// building the list of clipping shapes to skip
	if (buildSkipLists)
	{
		for (int i = 0; i < numShapesClip; i++)
		{
			if ( fabs(initClipAreas[i] - resultClipAreas[i]) < AREA_TOLERANCE && initClipAreas[i] != 0.0)
			{
				clippingShapesToSkip->insert(i);
			}
		}
	}

cleaning:
	for(int i = 0; i < (int)vGeometries.size(); i++)
	{	
		if (vGeometries[i] !=NULL) 
		{
			GEOSGeom_destroy(vGeometries[i]);
		}
	}

	if (qTree !=NULL) 
		delete qTree;
}

// ******************************************************************
//		IntersectionClipper()
// ******************************************************************
void CShapefile::IntersectionClipper( VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip, 
									  IShapefile* sfResult, map<long, long>* fieldMap,
									  std::set<int>* subjectShapesToSkip, 
								      std::set<int>* clippingShapesToSkip)
{
	// spatial index for clip shapefile
	QTree* qTree = GenerateLocalQTree(sfClip, SelectedOnlyClip?true:false);
	if (qTree == NULL) 
		return;
	
	long numShapesSubject, numShapesClip;		
	this->get_NumShapes(&numShapesSubject);		
	sfClip->get_NumShapes(&numShapesClip);	
	
	vector<TPolyPolygon*> vPolygons;		// we shall create vectors for both clipper and GEOS 
	vPolygons.assign(numShapesClip, NULL);	// this won't take much RAM or time
	
	std::vector<ShapeData*>* shapeDataClip = ((CShapefile*)sfClip)->get_ShapeVector();

	clipper::Clipper clp; 
	GeometryConverter ogr;
	
	OGRSpatialReference* reference = this->get_OGRSpatialReference();
	if (reference)
	{
		if (reference->IsGeographic() != 0)
		{
			ogr.conversionFactor = m_globalSettings.clipperGcsMultiplicationFactor;	// multiplication for geographic coordinate systems
		}
		delete reference;
	}

	IGeoProjection* projection = NULL;
	sfClip->get_GeoProjection(&projection);
	VARIANT_BOOL isGeographic = VARIANT_FALSE;
	if (projection)
	{
		projection->get_IsGeographic(&isGeographic);
		projection->Release();
		projection = NULL;
	}
	double AREA_TOLERANCE = m_globalSettings.GetMinPolygonArea(isGeographic) * 0.001;

	// initial areas areas of the clipping shapes
	std::vector<double> initClipAreas;
	// areas of the clipping shapes that were passed to the result:  (int)index of shapes -> (double)area
	std::vector<double> resultClipAreas;

	ShpfileType shpType;
	sfClip->get_ShapefileType(&shpType);

	bool buildSkipLists = (subjectShapesToSkip != NULL && clippingShapesToSkip != NULL && m_globalSettings.shapefileFastUnion &&
						   Utility::ShapeTypeConvert2D(_shpfiletype) == SHP_POLYGON && Utility::ShapeTypeConvert2D(shpType) == SHP_POLYGON);
	
	if (buildSkipLists)
	{
		initClipAreas.resize(numShapesClip, 0.0);
		resultClipAreas.resize(numShapesClip, 0.0);
	}
	
	long percent = 0;
	for(long subjectId =0; subjectId < numShapesSubject; subjectId++)		
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(subjectId + 1)/numShapesSubject)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Clipping shapes..."));
			}
		}

		if (SelectedOnlySubject && !_shapeData[subjectId]->selected)
			continue;
		
		IExtents* box;
		vector<int> shapeIds;
		double xMin, xMax, yMin, yMax, zMin, zMax;
		this->QuickExtents(subjectId, &box);
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		box->Release();
		shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));
		
		if (shapeIds.size() > 0)
		{
			// extracting subject polygon
			IShape* shp1 = NULL;
			this->get_Shape(subjectId, &shp1);
			TPolyPolygon* poly1 = ogr.Shape2ClipperPolygon(shp1);

			double initArea = 0.0;
			shp1->get_Area(&initArea);

			shp1->Release();

			if (poly1)
			{
				double sumArea = 0.0;

				// extracting clipping polygons
				for (int j = 0; j < (int)shapeIds.size(); j++)
				{
					// user can abort the operation in any time
					if (_stopExecution)
					{
						VARIANT_BOOL stop;
						_stopExecution->StopFunction(&stop);
						if (stop)
						{
							sfResult->EditClear(&stop);
							delete poly1;
							goto cleaning;
						}
					}
					
					long clipId = shapeIds[j];

					if (SelectedOnlyClip && !(*shapeDataClip)[clipId]->selected)
						continue;

					vector<IShape* > vShapes;
					
					// processng with Clipper
					TPolyPolygon* poly2 = NULL;
					if (vPolygons[clipId] == NULL)
					{
						IShape* shp2 = NULL;
						sfClip->get_Shape(clipId, &shp2);
						poly2 = ogr.Shape2ClipperPolygon(shp2);
						vPolygons[clipId] = poly2;
						
						if (buildSkipLists)
						{
							double tempArea = 0.0;
							shp2->get_Area(&tempArea);
							initClipAreas[clipId] = tempArea;
						}

						shp2->Release();
					}
					else
					{
						poly2 = vPolygons[clipId];
					}
				
					if (poly2)
					{
						if (poly1)
							clp.AddPolyPolygon(*poly1, ptSubject);
						if (poly2)
							clp.AddPolyPolygon(*poly2, ptClip);

						// do clipping
						TPolyPolygon polyResult;
						if (clp.Execute(ctIntersection, polyResult))
						{
							IShape* shp = ogr.ClipperPolygon2Shape(&polyResult);
							if (shp)
							{
								// sum of area to exclude shapes from difference
								if (buildSkipLists)
								{
									double areaTemp;
									shp->get_Area(&areaTemp);
									
									// subject shape
									sumArea += areaTemp;

									resultClipAreas[clipId] += areaTemp;
								}
								
								vector<IShape* > vShapes;
								vShapes.push_back(shp);
								this->InsertShapesVector(sfResult, vShapes, this, subjectId, NULL, sfClip, clipId, fieldMap);
							}
						}
						clp.Clear();
					}
				}
				delete poly1;

				if (buildSkipLists)
				{
					if (fabs(sumArea - initArea) < AREA_TOLERANCE)
					{
						subjectShapesToSkip->insert(subjectId);
					}
				}
			}
		}
	}

	// building the list of clipping shapes to skip
	if (buildSkipLists)
	{
		for (int i = 0; i < numShapesClip; i++)
		{
			if ( fabs(initClipAreas[i] - resultClipAreas[i]) < AREA_TOLERANCE && initClipAreas[i] != 0.0)
			{
				clippingShapesToSkip->insert(i);
			}
		}
	}


cleaning:	
	for(int i = 0; i < (int)vPolygons.size(); i++)
	{	
		if (vPolygons[i] !=NULL) 
		{
			delete vPolygons[i];
		}
	}
	delete qTree;
}
#pragma endregion

#pragma region Difference
// ********************************************************************
//		DifferenceGEOS()
// ********************************************************************
void CShapefile::DifferenceGEOS(IShapefile* sfSubject, VARIANT_BOOL SelectedOnlySubject, IShapefile* sfOverlay, VARIANT_BOOL SelectedOnlyOverlay, 
								IShapefile* sfResult, map<long, long>* fieldMap, set<int>* shapesToSkip)
{
		// spatial index for clip shapefile
	QTree* qTree = GenerateLocalQTree(sfOverlay, SelectedOnlyOverlay?true:false);
	if (!qTree) 
		return;
	
	long numShapesSubject, numShapesClip;		
	sfSubject->get_NumShapes(&numShapesSubject);		
	sfOverlay->get_NumShapes(&numShapesClip);		

	// a cache for clipping geometries
	vector<GEOSGeometry*> vGeometries;
	vGeometries.assign(numShapesClip, NULL);	
	
	std::vector<ShapeData*>* shapeDataClip = ((CShapefile*)sfOverlay)->get_ShapeVector();
	std::vector<ShapeData*>* shapeDataSubject = ((CShapefile*)sfSubject)->get_ShapeVector();

	long percent = 0;
	for(long subjectId = 0; subjectId < numShapesSubject; subjectId++)		
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(subjectId + 1)/numShapesSubject)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Clipping shapes..."));
			}
		}

		if (SelectedOnlySubject && !(*shapeDataSubject)[subjectId]->selected)
			continue;
		
		// those shapes are marked to skip in the course of intersection
		if (shapesToSkip != NULL)
		{
			if (shapesToSkip->find(subjectId) != shapesToSkip->end())
				continue;
		}

		IExtents* box;
		vector<int> shapeIds;
		double xMin, xMax, yMin, yMax, zMin, zMax;
		sfSubject->QuickExtents(subjectId, &box);			// extracting subject extents
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		box->Release();
		shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));
		
		if (shapeIds.size() > 0)
		{
			// extracting subject geometry
			IShape* shp1 = NULL;
			sfSubject->get_Shape(subjectId, &shp1);
			GEOSGeometry* gsGeom1 = GeometryConverter::Shape2GEOSGeom(shp1);
			shp1->Release();

			if (gsGeom1)
			{
				vector<GEOSGeometry*> vClip;
				
				// iterating through clip geometries, if the subject will stand this, we add it to the result
				for (int j = 0; j < (int)shapeIds.size(); j++)
				{
					// user can abort the operation in any time
					if (_stopExecution)
					{
						VARIANT_BOOL stop;
						_stopExecution->StopFunction(&stop);
						if (stop)
						{
							sfResult->EditClear(&stop);
							GEOSGeom_destroy(gsGeom1);
							goto cleaning;
						}
					}

					// extracting clip geometry
					long clipId = shapeIds[j];

					if (SelectedOnlyOverlay && !(*shapeDataClip)[clipId]->selected)
						continue;

					GEOSGeometry* gsGeom2 = NULL;
					
					if (vGeometries[clipId] == NULL)
					{
						// in case we meet this geometry for the first time, we shall convert it
						IShape* shp1 = NULL;
						sfOverlay->get_Shape(clipId, &shp1);
						gsGeom2 = GeometryConverter::Shape2GEOSGeom(shp1);
						vGeometries[clipId] = gsGeom2;
						shp1->Release();
					}
					else
					{					
						// otherwise, we just take it from the cache
						gsGeom2 = vGeometries[clipId];
					}
					
					if (gsGeom2 && GEOSIntersects(gsGeom1, gsGeom2))
					{
						vClip.push_back(gsGeom2);
					}
				}
				
				GEOSGeometry* gsClip = NULL;
				if (vClip.size() == 1)
				{
					gsClip = vClip[0];
				}
				else if (vClip.size() > 1)
				{
					// union of the clipping shapes
					gsClip = GeometryConverter::MergeGeosGeometries(vClip, NULL, false);
				}
				
				if (gsClip)
				{
					GEOSGeometry* gsTemp = gsGeom1;
					gsGeom1 = GEOSDifference(gsGeom1, gsClip);
					GEOSGeom_destroy(gsTemp);	// initial subject geometry isn't needed any more
					
					// if clip geometry was merged, we should delete it
					if (vClip.size() > 1)
					{
						GEOSGeom_destroy(gsClip);
					}
				}
				
				// saving what was left from the subject
				if (gsGeom1 != NULL)
				{
					vector<IShape* > vShapes;
					bool result = GeometryConverter::GEOSGeomToShapes(gsGeom1, &vShapes);
					GEOSGeom_destroy(gsGeom1);

					this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap);	// shapes are released here
				}
			}
		}
		else
		{
			// insert the shape directly no other shapes intersects it
			// TODO: it makes sense to rewrite it in more efficient way
			IShape* shp1 = NULL;
			vector<IShape* > vShapes;
			sfSubject->get_Shape(subjectId, &shp1);
			vShapes.push_back(shp1);
			this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap);	// shapes are released here
		}
	}

cleaning:	
	for(int i = 0; i < (int)vGeometries.size(); i++)
	{	
		if (vGeometries[i] !=NULL) 
		{
			GEOSGeom_destroy(vGeometries[i]);
		}
	}

	if (qTree !=NULL) 
		delete qTree;
}

#ifdef SERIALIZE_POLYGONS
void SerializePolygon(ofstream& out, TPolyPolygon* poly)
{
	if (poly && out.good())
	{
		out.precision(14);
		out << "Polygon" << endl;
		int part = 0;
		for (unsigned int i = 0; i < poly->size(); i++)
		{
			TPolygon* p = &((*poly)[i]);
			if (p)
			{
				part++;
				for (unsigned int j = 0; j < p->size(); j++)
				{
					out << (*p)[j].X << ", " << (*p)[j].Y;
					if (j != p->size() - 1)
					{
						out << ",";
					}
					out << endl;
				}
			}
		}
		out << endl;
	}
}
#endif

// ******************************************************************
//		DifferenceClipper()
// ******************************************************************
void CShapefile::DifferenceClipper(IShapefile* sfSubject, VARIANT_BOOL SelectedOnlySubject, IShapefile* sfClip, VARIANT_BOOL SelectedOnlyClip, 
								   IShapefile* sfResult, map<long, long>* fieldMap, set<int>* shapesToSkip)
{
	#ifdef SERIALIZE_POLYGONS
	ofstream out;
	out.open("c:\\polygons.txt");
	#endif
	
	// spatial index for clip shapefile
	QTree* qTree = GenerateLocalQTree(sfClip, SelectedOnlyClip?true:false);
	if (qTree == NULL) 
		return;
	
	long numShapesSubject, numShapesClip;		
	sfSubject->get_NumShapes(&numShapesSubject);		
	sfClip->get_NumShapes(&numShapesClip);	
	
	vector<TPolyPolygon*> vPolygons;		// we shall create vectors for both clipper and GEOS 
	vPolygons.assign(numShapesClip, NULL);	// this won't take much RAM or time
	
	std::vector<ShapeData*>* shapeDataClip = ((CShapefile*)sfClip)->get_ShapeVector();
	std::vector<ShapeData*>* shapeDataSubject = ((CShapefile*)sfSubject)->get_ShapeVector();

	
	GeometryConverter ogr;

	OGRSpatialReference* reference = ((CShapefile*)sfSubject)->get_OGRSpatialReference();
	if (reference)
	{
		if (reference->IsGeographic() != 0)
		{
			ogr.conversionFactor = m_globalSettings.clipperGcsMultiplicationFactor;	// multiplication for geographic coordinate systems
		}
		delete reference;
	}
	
	long percent = 0;
	for(long subjectId =0; subjectId < numShapesSubject; subjectId++)		
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(subjectId + 1)/numShapesSubject)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Clipping shapes..."));
			}
		}

		if (SelectedOnlySubject && !(*shapeDataSubject)[subjectId]->selected)
			continue;
		
		if (shapesToSkip != NULL)
		{
			if (shapesToSkip->find(subjectId) != shapesToSkip->end())
				continue;
		}

		IExtents* box;
		vector<int> shapeIds;
		double xMin, xMax, yMin, yMax, zMin, zMax;
		sfSubject->QuickExtents(subjectId, &box);
		box->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		box->Release();
		shapeIds = qTree->GetNodes(QTreeExtent(xMin,xMax,yMax,yMin));
		
		if (shapeIds.size() > 0)
		{
			// extracting subject polygon
			IShape* shp1 = NULL;
			sfSubject->get_Shape(subjectId, &shp1);
			TPolyPolygon* poly1 = ogr.Shape2ClipperPolygon(shp1);
			shp1->Release();

			if (poly1)
			{
				clipper::Clipper* clp = new clipper::Clipper; 

				// extracting clipping polygons
				TPolyPolygon* poly2 = NULL;
				for (int j = 0; j < (int)shapeIds.size(); j++)
				{
					// user can abort the operation in any time
					if (_stopExecution)
					{
						VARIANT_BOOL stop;
						_stopExecution->StopFunction(&stop);
						if (stop)
						{
							sfResult->EditClear(&stop);
							delete clp;
							goto cleaning;
						}
					}
					
					long clipId = shapeIds[j];

					if (SelectedOnlyClip && !(*shapeDataClip)[clipId]->selected)
						continue;

					vector<IShape* > vShapes;
					
					// processng with Clipper
					if (vPolygons[clipId] == NULL)
					{
						IShape* shp2 = NULL;
						sfClip->get_Shape(clipId, &shp2);
						poly2 = ogr.Shape2ClipperPolygon(shp2);
						vPolygons[clipId] = poly2;
						shp2->Release();
					}
					else
					{
						poly2 = vPolygons[clipId];
					}
					
					if (poly2)
						clp->AddPolyPolygon(*poly2, ptClip);
					
					#ifdef SERIALIZE_POLYGONS
					SerializePolygon(out, poly2);
					#endif
				}
				
				if (poly1)
					clp->AddPolyPolygon(*poly1, ptSubject);

				#ifdef SERIALIZE_POLYGONS
				SerializePolygon(out, poly1);
				#endif

				// do clipping
				TPolyPolygon polyResult;
				if (clp->Execute(ctDifference, polyResult))
				{
					IShape* shp = ogr.ClipperPolygon2Shape(&polyResult);
					if (shp)
					{
						vector<IShape* > vShapes;
						vShapes.push_back(shp);
						this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap);
					}
				}
				
				delete poly1;
				clp->Clear();
				delete clp;
			}
		}
		else
		{
			// insert the shape directly no other shapes intersects it
			// TODO: it makes sense to rewrite it in more efficient way
			IShape* shp1 = NULL;
			vector<IShape* > vShapes;
			sfSubject->get_Shape(subjectId, &shp1);
			
			VARIANT_BOOL editingShapes;
			sfSubject->get_EditingShapes(&editingShapes);
			IShape* shpCopy = NULL;
			if (editingShapes)
			{
				shp1->Clone(&shpCopy);
				shp1->Release();
			}
			else
			{
				shpCopy = shp1;
			}
			vShapes.push_back(shpCopy);
			this->InsertShapesVector(sfResult, vShapes, sfSubject, subjectId, fieldMap);	// shapes are released here
		}
	}

cleaning:
	for(int i = 0; i < (int)vPolygons.size(); i++)
	{	
		if (vPolygons[i] !=NULL) 
		{
			delete vPolygons[i];
		}
	}
	delete qTree;
	#ifdef SERIALIZE_POLYGONS
	out.close();	
	#endif
}
#pragma endregion

#pragma region Utilities

// ******************************************************************
//		InsertShapesVector()
// ******************************************************************
// Inserts shapes, resulting from intersection of one shape of subject shapefile, 
// and one shape of clip shapefile, attributes a copied from both shapefiles
void CShapefile::InsertShapesVector(IShapefile* sf, vector<IShape* >& vShapes, 
									IShapefile* sfSubject, long subjectId, std::map<long, long>* fieldMapSubject,
									IShapefile* sfClip, long clipId, std::map<long, long>* fieldMapClip)
{
	long numFieldSubject, numFieldsClip;
	sfSubject->get_NumFields(&numFieldSubject);
	if (sfClip)
		sfClip->get_NumFields(&numFieldsClip);

	VARIANT_BOOL vbretval;
	
	ShpfileType fileType;
	sf->get_ShapefileType(&fileType);

	VARIANT var;
	VariantInit(&var);
	
	IGeoProjection* proj = NULL;
	sf->get_GeoProjection(&proj);
	VARIANT_BOOL isGeographic;
	proj->get_IsGeographic(&isGeographic);
	proj->Release();

	for (int i = 0; i < (int)vShapes.size(); i++) 
	{	
		IShape* shp = NULL;
		shp = vShapes[i];
		
		ShpfileType shpType;
		shp->get_ShapeType(&shpType);
		
		if (shpType == fileType)
		{
			// area checking
			shpType = Utility::ShapeTypeConvert2D(shpType);
			if (shpType == SHP_POLYGON)
			{
				double area;
				shp->get_Area(&area);
				if (area < m_globalSettings.GetMinPolygonArea(isGeographic))
				{
					shp->Release();
					continue;
				}
				
				double perimeter;
				shp->get_Perimeter(&perimeter);
				if (isGeographic)
					area *= 110899.999942;	// comparison is perfromed in meters, area will grow as squar of linear size
											// we multilpy area only; in reality:((area * c^2)/ (perimeter * c))
				
				if (area/perimeter < m_globalSettings.minAreaToPerimeterRatio)
				{
					shp->Release();
					continue;
				}
			}

			// checking for validity
			shp->get_IsValid(&vbretval);
			if (!vbretval)
			{
				// let's try to buffer it
				IShape* shpNew = NULL;
				double distance = m_globalSettings.invalidShapesBufferDistance;
				shp->Buffer(distance, 30, &shpNew);
				
				if (shpNew )
				{
					// swapping shapes
					shpNew->get_IsValid(&vbretval);
					if (vbretval)
					{
						shp->Release();
						shp = shpNew;
					}
				}
			}

			long newId;
			sf->get_NumShapes(&newId);
			sf->EditInsertShape(shp, &newId, &vbretval);
			
			// passing the values from subject shapefile
			// can be optimized for not extracting the same values many times
			if (fieldMapSubject)
			{
				
				std::map<long, long>::iterator p = fieldMapSubject->begin();
				while (p != fieldMapSubject->end())
				{
					sfSubject->get_CellValue(p->first, subjectId, &var);
					sf->EditCellValue(p->second, newId, var, &vbretval);
					p++;
				}
			}
			else
			{
				for (int iFld = 0; iFld < numFieldSubject; iFld++)
				{	
					sfSubject->get_CellValue(iFld, subjectId, &var);
					sf->EditCellValue(iFld, newId, var, &vbretval);
				}
			}
			
			// passing the values from clip shapefile
			if ( sfClip && fieldMapClip)
			{
				std::map<long, long>::iterator p = fieldMapClip->begin();
				while (p != fieldMapClip->end())
				{
					sfClip->get_CellValue(p->first, clipId, &var);
					sf->EditCellValue(p->second, newId, var, &vbretval);
					p++;
				}
			}
		}
		shp->Release();
	}

	VariantClear(&var);
}
#pragma endregion

#pragma region Properties
// ********************************************************************
//		GeometryEngine()
// ********************************************************************
STDMETHODIMP CShapefile::get_GeometryEngine(tkGeometryEngine* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _geometryEngine;
	return S_OK;
}

STDMETHODIMP CShapefile::put_GeometryEngine(tkGeometryEngine newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_geometryEngine = newVal;
	return S_OK;
}

#pragma endregion

#pragma region PointInPolygon

// ********************************************************************
//		pointInPolygon()
// ********************************************************************
BOOL CShapefile::pointInPolygon( long ShapeIndex, double x, double y )
{
	VARIANT_BOOL InPolygon;
	this->PointInShape(ShapeIndex, x, y, &InPolygon);
	return -InPolygon;
}

// ********************************************************************
//		PointInShape()
// ********************************************************************
STDMETHODIMP CShapefile::PointInShape(LONG ShapeIndex, DOUBLE x, DOUBLE y, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if( ShapeIndex < 0 || ShapeIndex >= (long)_shapeData.size())//_numShapes )
	{	
		*retval = VARIANT_FALSE;
		ErrorMessage( tkINDEX_OUT_OF_BOUNDS );
		return S_OK;
	}
	
	long numParts, numPoints;
	long * Parts;
	Point2D * Points;
	
	if (_isEditingShapes)
	{	
		if(_shpfiletype != SHP_POLYGON && _shpfiletype != SHP_POLYGONM && _shpfiletype != SHP_POLYGONZ)
		{
			*retval = VARIANT_FALSE;
			return S_OK;
		}
		
		IExtents * ext;
		QuickExtents(ShapeIndex, &ext);
		double xMin, yMin, xMax, yMax,zMin, zMax;
		ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		ext->Release();
		if(x < xMin || y < yMin || x > xMax || y > yMax)
		{	
			*retval = VARIANT_FALSE;
			return S_OK;
		}
		
		// retrieving shapes from memory
		IShape * shp = _shapeData[ShapeIndex]->shape;
		shp->get_NumParts(&numParts);
		shp->get_NumPoints(&numPoints);
		Parts = new long[numParts + 1];
		Points = new Point2D[numPoints];
		
		for(int nPart = numParts - 1; nPart >= 0; nPart--)
			shp->get_Part(nPart, &Parts[nPart]);

		IPoint * pnt = NULL;
		for(int nPoint = numPoints - 1; nPoint >= 0; nPoint--)
		{
			shp->get_Point(nPoint, &pnt);
			pnt->get_X(&Points[nPoint].x);
			pnt->get_Y(&Points[nPoint].y);
			pnt->Release();
		}
		Parts[numParts] = numPoints;
	}	
	else
	{
		int shpType;
		fseek(_shpfile, shpOffsets[ShapeIndex] + sizeof(int)*2, SEEK_SET);
		fread(&shpType, sizeof(int), 1, _shpfile);

		shpType = Utility::ShapeTypeConvert2D((ShpfileType)shpType);
		if(shpType != SHP_POLYGON)
		{
			*retval = VARIANT_FALSE;
			return S_OK;
		}

		ShapeHeader shpHeader;
		fread(&shpHeader, sizeof(ShapeHeader), 1, _shpfile);

		// check the bounds
		if(x < shpHeader.MinX || y < shpHeader.MinY || x > shpHeader.MaxX || y > shpHeader.MaxY)
		{
			*retval = VARIANT_FALSE;
			return S_OK;
		}
		
		Parts = new long[shpHeader.NumParts + 1];
		Points = new Point2D[shpHeader.NumPoints];

		fread(Parts, sizeof(int), shpHeader.NumParts, _shpfile);
		fread(Points, sizeof(Point2D), shpHeader.NumPoints, _shpfile);
		Parts[shpHeader.NumParts] = shpHeader.NumPoints;
		numParts =  shpHeader.NumParts;
	}

	int CrossCount = 0;
	for(int nPart = numParts - 1; nPart >= 0; nPart--)
	{
		int nPointMax = Parts[nPart+1] - 1;
		for(int nPoint = Parts[nPart]; nPoint < nPointMax; nPoint++)
		{
			double x1 = Points[nPoint].x - x;
			double y1 = Points[nPoint].y - y;
			double x2 = Points[nPoint+1].x - x;
			double y2 = Points[nPoint+1].y - y;

			double y1y2 = y1*y2;
			if(y1y2 > 0.0) // If the signs are the same
			{
				// Then it does not cross
				continue;
			}
			else if(y1y2 == 0.0) // Then it has intesected a vertex
			{
				if(y1 == 0.0)
				{
					if( y2 > 0.0 )
						continue;
				}
				else if( y1 > 0.0 )
					continue;
			}

			if( x1 > 0.0 && x2 > 0.0 )
			{
				CrossCount++;
				continue;
			}

			// Calculate Intersection
			double dy = y2 - y1;
			double dx = x2 - x1;

			// CDM March 2008 - if dy is zero (horiz line), this will be a bad idea...
			if (dy != 0)
			{
				if (x1 - y1*(dx/dy) > 0.0)
					CrossCount++;
			}
		}
	}
	delete [] Points;
	delete [] Parts;

	*retval = CrossCount&1 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}
#pragma endregion

#pragma region PointInShapefile
// ********************************************************************
//		PointInShapefile()
// ********************************************************************
STDMETHODIMP CShapefile::PointInShapefile(DOUBLE x, DOUBLE y, LONG* ShapeIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	int nShapeCount = m_PolySF.size();
	for(int nShape = nShapeCount - 1; nShape >= 0; nShape--) 
	// for(int nShape = 0; nShape < ShapeCount; nShape++) see http://www.mapwindow.org/phorum/read.php?3,9745,9950#msg-9950
	{
		PolygonShapefile * sf = &m_PolySF[nShape];

		if(x < sf->shpHeader.MinX || y < sf->shpHeader.MinY || x > sf->shpHeader.MaxX || y > sf->shpHeader.MaxY)
		{
			continue;
		}

		int CrossCount = 0;

		for(int nPart = sf->shpHeader.NumParts - 1; nPart >= 0; nPart--)
		{
			int nPointMax = sf->Parts[nPart+1] - 1;
			for(int nPoint = sf->Parts[nPart]; nPoint < nPointMax; nPoint++)
			{
				double x1 = sf->Points[nPoint].x - x;
				double y1 = sf->Points[nPoint].y - y;
				double x2 = sf->Points[nPoint+1].x - x;
				double y2 = sf->Points[nPoint+1].y - y;

				double y1y2 = y1*y2;

				if(y1y2 > 0.0) // If the signs are the same
				{
					// Then it does not cross
					continue;
				}
				else if(y1y2 == 0.0) // Then it has intesected a vertex
				{
					if(y1 == 0.0)
					{
						if( y2 > 0.0 )
							continue;
					}
					else if( y1 > 0.0 )
						continue;
				}

				if( x1 > 0.0 && x2 > 0.0 )
				{
					CrossCount++;
					continue;
				}

				// Calculate Intersection
				double dy = y2 - y1;
				double dx = x2 - x1;
				double xint = x1 - y1*(dx/dy);

				if(xint > 0.0)
					CrossCount++;
			}
		}

		if(CrossCount&1)
		{
			*ShapeIndex = nShape;
			return S_OK;
		}
		
	}

	*ShapeIndex = -1;
	return S_OK;
}

// ********************************************************************
//		BeginPointInShapefile()
// ********************************************************************
STDMETHODIMP CShapefile::BeginPointInShapefile(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_writing)
	{
		//AfxMessageBox("Can't read");
		*retval = VARIANT_FALSE;
		return S_OK;
	}

	if( _shpfiletype != SHP_POLYGON )
	{
		*retval = VARIANT_FALSE;
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return S_OK;
	}
	
	int size = _shapeData.size();
	m_PolySF.resize(size);
	for(int nShape = 0; nShape < size; nShape++)
	{
		fseek(_shpfile, shpOffsets[nShape]+sizeof(int)*2, SEEK_SET);
		int shpType;
		fread(&shpType, sizeof(int), 1, _shpfile);
		if(shpType != SHP_POLYGON)
		{
			*retval = VARIANT_FALSE;
			ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
			return S_OK;
		}

		PolygonShapefile & sf = m_PolySF[nShape];
		fread(&sf.shpHeader, sizeof(ShapeHeader), 1, _shpfile);
		
		if (sf.shpHeader.NumPoints > 0 && sf.shpHeader.NumParts > 0) 
		{
			sf.Points.resize(sf.shpHeader.NumPoints);
			sf.Parts.resize(sf.shpHeader.NumParts + 1);		
			fread(&sf.Parts[0], sizeof(int), sf.shpHeader.NumParts, _shpfile);
			fread(&sf.Points[0], sizeof(Point2D), sf.shpHeader.NumPoints, _shpfile);		
			sf.Parts[sf.shpHeader.NumParts] = sf.shpHeader.NumPoints;
			*retval = VARIANT_TRUE;
		}
		else
		{
			// this isn't a valid shape, we simply won't process it
			ErrorMessage(tkINVALID_SHAPE);
			*retval = VARIANT_FALSE;
		}
	}	
	return S_OK;
}

// ********************************************************************
//		EndPointInShapefile()
// ********************************************************************
STDMETHODIMP CShapefile::EndPointInShapefile(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_PolySF.clear();

	return S_OK;
}
#pragma endregion

#pragma region NewMembers
// ********************************************************************
//		ExplodeShapes()
// ********************************************************************
STDMETHODIMP CShapefile::ExplodeShapes(VARIANT_BOOL SelectedOnly, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	// do we have enough shapes, are they selected?
	long numShapes = _shapeData.size();		
	if (numShapes == 0) 
		return S_OK;

	LONG numSelected;
	this->get_NumSelected(&numSelected);

	LONG numFields;
	this->get_NumFields(&numFields);

	if (SelectedOnly && numSelected == 0)
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return S_OK;
	}

	// creation of resulting shapefile
	VARIANT_BOOL vbretval;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNew(A2BSTR(""), _shpfiletype, &vbretval);
	
	CopyFields(this, *retval);

	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
	{
		(*retval)->put_Projection(pVal);
		SysFreeString(pVal);
	}

	// processing shapes
	long count;
	VARIANT var;
	VariantInit(&var);
	std::vector<IShape*> vShapes;
	long percent = 0;
	
	for (long i = 0; i < numShapes; i++)
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(i + 1)/numShapes)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Exploding..."));
			}
		}
		
		if (SelectedOnly && !_shapeData[i]->selected)
			continue;

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		if (shp)
		{
			if (((CShape*)shp)->ExplodeCore(vShapes))
			{
				for (long j = 0; j < (long)vShapes.size(); j++)
				{
					// all the shapes are copies of the initial ones, so no further cloning is needed					
					(*retval)->get_NumShapes(&count);
					(*retval)->EditInsertShape(vShapes[j], &count, &vbretval);
					
					if (vbretval)
					{
						// copy attributes
						for (int iFld = 0; iFld < numFields; iFld++)
						{	
							this->get_CellValue(iFld, i, &var);
							(*retval)->EditCellValue(iFld, count, var, &vbretval);
						}
					}
					vShapes[j]->Release();	// reference was added in EditInsertShape
				}
			}
		}
	}

	VariantClear(&var);

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	(*retval)->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		(*retval)->Close(&vbretval);
		(*retval)->Release();
		(*retval) = NULL;
	}
	return S_OK;
}

// ********************************************************************
//		AggregateShapes()
// ********************************************************************
STDMETHODIMP CShapefile::AggregateShapes(VARIANT_BOOL SelectedOnly, LONG FieldIndex, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (_shapeData.size() == 0)
		return S_OK;

	// validating parameters
	long numFields;
	this->get_NumFields(&numFields);

	if( FieldIndex != -1 && !(FieldIndex >= 0 && FieldIndex < numFields))
	{	
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_OK;
	} 
	
	LONG numSelected;
	this->get_NumSelected(&numSelected);
	if (numSelected == 0 && SelectedOnly)
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return S_OK;
	}

	// creation of resulting shapefile
	VARIANT_BOOL vbretval;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNewWithShapeID(A2BSTR(""), _shpfiletype, &vbretval);

	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
	{
		(*retval)->put_Projection(pVal);
		SysFreeString(pVal);
	}

	map <CComVariant, vector<IShape*>*> shapeMap;
	map <CComVariant, vector<IShape*>*>::iterator p;
	CComVariant val;	// VARIANT hasn't got comparison operators and therefore
						// can't be used with assosiative containers
	long percent = 0;
	int size = (int)_shapeData.size();

	for(long i = 0; i < size; i++)
	{
		if( globalCallback != NULL ) 
		{
			long newpercent = (long)(((double)i/size)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Grouping shapes..."));
			}
		}

		if (SelectedOnly && !_shapeData[i]->selected)
			continue;

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		
		if (FieldIndex != -1)
			this->get_CellValue(FieldIndex, i, &val);	

		if(shapeMap.find(val) != shapeMap.end())
		{
			shapeMap[val]->push_back(shp);
		}
		else
		{
			vector<IShape*>* v = new vector<IShape*>;
			v->push_back(shp);
			shapeMap[val] = v;
		}
	}
	
	// saving results							
	long count = 0;	// number of shapes inserted
	int i = 0;		// for progress bar
	percent = 0;
	size = shapeMap.size();
	p = shapeMap.begin();

	while(p != shapeMap.end())
	{
		if( globalCallback != NULL ) 
		{
			long newpercent = (long)(((double)i/size)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Merging shapes..."));
			}
		}
		
		// merging shapes
		vector<IShape*>* shapes = p->second;
		long pntIndex = 0;
		long partIndex = 0;

		IShape* shpBase = NULL;
		for (unsigned int j = 0; j < shapes->size(); j++)
		{
			if (j == 0)
			{
				if (_isEditingShapes)
				{
					// in editing mode we share the shape with parent shapefile
					// so a copy is needed to aviod conflicts
					(*shapes)[0]->Clone(&shpBase);
					(*shapes)[0]->Release();
				}
				else
				{
					// in the regular mode we are the sole owners of shape
					shpBase = (*shapes)[0];
				}
				shpBase->get_NumPoints(&pntIndex);
				shpBase->get_NumParts(&partIndex);
			}
			else
			{
				IShape* shp = (*shapes)[j];
				long numParts;
				shp->get_NumParts(&numParts);

				for (long part = 0; part < numParts; part++)
				{
					shpBase->InsertPart(pntIndex, &partIndex, &vbretval);
					
					long start, end;
					shp->get_Part(part, &start);
					shp->get_EndOfPart(part, &end);
					
					for (long point = start; point <= end; point++)
					{
						IPoint* pnt = NULL;
						shp->get_Point(point, &pnt);
						if (pnt)
						{
							IPoint* pntNew = NULL;
							pnt->Clone(&pntNew);
							shpBase->InsertPoint( pntNew, &pntIndex, &vbretval );
							pntIndex++;
							pntNew->Release();
							pnt->Release();
						}
					}

					partIndex++;
				}
				shp->Release();
			}
		}
		
		shpBase->get_NumPoints(&pntIndex);
		shpBase->get_NumParts(&partIndex);

		if (partIndex > 0 && pntIndex > 0)
		{
			(*retval)->EditInsertShape(shpBase, &count, &vbretval);
		}
		shpBase->Release();
			
		delete p->second;	// deleting the vector
		count++;
		p++;
		i++;
	}

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	long numShapes;
	(*retval)->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		(*retval)->Close(&vbretval);
		(*retval)->Release();
		(*retval) = NULL;
	}
	return S_OK;
}

// ********************************************************************
//		ExportSelection()
// ********************************************************************
STDMETHODIMP CShapefile::ExportSelection(IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	// do we have enough shapes, are they selected?
	long numShapes = _shapeData.size();		
	if (numShapes == 0) 
		return S_OK;

	LONG numSelected;
	this->get_NumSelected(&numSelected);
	if (numSelected == 0)
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return S_OK;
	}

	LONG numFields;
	this->get_NumFields(&numFields);

	// creation of resulting shapefile
	VARIANT_BOOL vbretval;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNew(A2BSTR(""), _shpfiletype, &vbretval);
	
	CopyFields(this, *retval);

	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
	{
		(*retval)->put_Projection(pVal);
		SysFreeString(pVal);
	}

	// processing shapes
	long count = 0;
	VARIANT var;
	VariantInit(&var);
	long percent = 0;
	
	for (long i = 0; i < numShapes; i++)
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(i + 1)/numShapes)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Exporting..."));
			}
		}
		
		if (!_shapeData[i]->selected)
			continue;

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		if (shp)
		{
			if (_isEditingShapes)
			{
				// in the editing mode we share a shape with the parent shapefile
				// a copy is needed to avoid conflicts
				IShape* shpCopy = NULL;
				shp->Clone(&shpCopy);
				(*retval)->EditInsertShape(shpCopy, &count, &vbretval);
				shpCopy->Release();
			}
			else
			{
				(*retval)->EditInsertShape(shp, &count, &vbretval);
			}
					
			if (vbretval)
			{
				// copy attributes
				for (int iFld = 0; iFld < numFields; iFld++)
				{	
					this->get_CellValue(iFld, i, &var);
					(*retval)->EditCellValue(iFld, count, var, &vbretval);
				}
				count++;
			}
			shp->Release();
		}
	}

	VariantClear(&var);

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	(*retval)->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		(*retval)->Close(&vbretval);
		(*retval)->Release();
		(*retval) = NULL;
	}
	return S_OK;
}

// ********************************************************************
//		CopyShape()
// ********************************************************************
// For the sort function
// The row index of attribute table are taken from the key property of the shape
void CopyShape(IShapefile* sfSource, IShape* shp, IShapefile* sfResult)
{
	if (shp)
	{
		USES_CONVERSION;
		BSTR key;
		shp->get_Key(&key);
		CString str = OLE2CA(key);
		SysFreeString(key);
		int initIndex = atoi(str);
		
		VARIANT_BOOL isEditingShapes, vbretval;
		sfSource->get_EditingShapes(&isEditingShapes);

		LONG numShapes;
		sfResult->get_NumShapes(&numShapes);

		if (isEditingShapes)
		{
			// in the editing mode we share a shape with the parent shapefile
			// a copy is needed to avoid conflicts
			IShape* shpCopy = NULL;
			shp->Clone(&shpCopy);
			sfResult->EditInsertShape(shpCopy, &numShapes, &vbretval);
			shpCopy->Release();
		}
		else
		{
			sfResult->EditInsertShape(shp, &numShapes, &vbretval);
		}
					
		if (vbretval)
		{
			LONG numFields;
			sfSource->get_NumFields(&numFields);

			// copy attributes
			CComVariant val;
			for (int iFld = 0; iFld < numFields; iFld++)
			{	
				sfSource->get_CellValue(iFld, initIndex, &val);
				sfResult->EditCellValue(iFld, numShapes, val, &vbretval);
			}
		}
		shp->Release();
	}
}

// ********************************************************************
//		Sort()
// ********************************************************************
STDMETHODIMP CShapefile::Sort(LONG FieldIndex, VARIANT_BOOL Ascending, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	// do we have enough shapes, are they selected?
	long numShapes = _shapeData.size();		
	if (numShapes == 0) 
		return S_OK;

	LONG numFields;
	this->get_NumFields(&numFields);

	// creation of resulting shapefile
	VARIANT_BOOL vbretval;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNew(A2BSTR(""), _shpfiletype, &vbretval);
	
	CopyFields(this, *retval);

	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
	{
		(*retval)->put_Projection(pVal);
		SysFreeString(pVal);
	}

	multimap <CComVariant, IShape*> shapeMap;
	
	CComVariant val;	// VARIANT hasn't got comparison operators and therefore
						// can't be used with assosiative containers

	// processing shapes
	long percent = 0;
	for (long i = 0; i < numShapes; i++)
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(i + 1)/numShapes)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Sorting..."));
			}
		}

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		if (shp)
		{
			// marking the index of shape
			CString str;
			str.Format("%d", i);
			BSTR index = A2BSTR(str);
			shp->put_Key(index);
			SysFreeString(index);
			
			this->get_CellValue(FieldIndex, i, &val);	
			pair<CComVariant, IShape*> myPair(val, shp);	
			shapeMap.insert(myPair);
		}
	}
	
	long count = 0;

	//writing the results
	if (Ascending)
	{
		multimap <CComVariant, IShape*>::iterator p;
		p = shapeMap.begin();
		
		while(p != shapeMap.end())
		{
			if( globalCallback != NULL ) 
			{
				long newpercent = (long)(((double)count/numShapes)*100);
				if( newpercent > percent )
				{	
					percent = newpercent;
					globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Writing..."));
				}
			}
			
			IShape* shp = p->second;
			CopyShape(this, shp, *retval);
			p++;
		}
	}
	else
	{
		std::multimap <CComVariant, IShape*>::reverse_iterator p = shapeMap.rbegin();
		while(p != shapeMap.rend())
		{
			if( globalCallback != NULL ) 
			{
				long newpercent = (long)(((double)count/numShapes)*100);
				if( newpercent > percent )
				{	
					percent = newpercent;
					globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Writing..."));
				}
			}

			IShape* shp = p->second;
			CopyShape(this, shp, *retval);

			p++;
		}
	}

	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	(*retval)->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		(*retval)->Close(&vbretval);
		(*retval)->Release();
		(*retval) = NULL;
	}
	return S_OK;
}

// ********************************************************************
//		Merge()
// ********************************************************************
STDMETHODIMP CShapefile::Merge(VARIANT_BOOL SelectedOnlyThis, IShapefile* sf, VARIANT_BOOL SelectedOnly, IShapefile** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	
	if( sf == NULL)
	{	
		ErrorMessage( tkUNEXPECTED_NULL_PARAMETER );
		return S_OK;
	} 

	// do we have enough shapes, are they selected?
	long numShapes1, numShapes2;		
	numShapes1 = _shapeData.size();
	sf->get_NumShapes(&numShapes2);		

	if (numShapes1 == 0 || numShapes2 == 0) 
		return S_OK;

	LONG numSelected1, numSelected2;
	this->get_NumSelected(&numSelected1);
	sf->get_NumSelected(&numSelected2);

	if (SelectedOnlyThis && numSelected1 == 0 ||
		SelectedOnly && numSelected2 == 0 )
	{
		ErrorMessage(tkSELECTION_EMPTY);
		return S_OK;
	}
	
	// we can use clipper for polygons only
	ShpfileType type1, type2;	
	type1 = Utility::ShapeTypeConvert2D(_shpfiletype);
	sf->get_ShapefileType(&type2);	
	type2 = Utility::ShapeTypeConvert2D(type2);
	if (type1  != type2 )
	{
		ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}
	
	long numFields;
	this->get_NumFields(&numFields);

	// creation of resulting shapefile
	VARIANT_BOOL vbretval;
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNew(A2BSTR(""), type1, &vbretval);
	
	// copying the projection string
	BSTR pVal;
	this->get_Projection(&pVal);
	if (pVal != NULL)
	{
		(*retval)->put_Projection(pVal);
		SysFreeString(pVal);
	}
	
	// do field mapping
	std::map<long, long> fieldMap;

	// copying fields from both shapefiles
	this->CopyFields(this, sf, *retval, fieldMap, true);
	
	long percent = 0;
	long count = 0;	// index of shape in new shapefile
	CComVariant val;

	for (int i = 0; i < numShapes1; i++)
	{
		if( globalCallback != NULL ) 
		{
			long newpercent = (long)(((double)count/numShapes1)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Writing..."));
			}
		}

		if (SelectedOnlyThis && !_shapeData[i]->selected)
			continue;

		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		if (shp)
		{
			vbretval = VARIANT_FALSE;
			if (_isEditingShapes)
			{
				IShape* shpCopy = NULL;
				shp->Clone(&shpCopy);
				if (shpCopy)
				{
					(*retval)->EditInsertShape(shpCopy, &count, &vbretval);
					shpCopy->Release();
				}
			}
			else
			{
				(*retval)->EditInsertShape(shp, &count, &vbretval);
			}
			
			// copying fields
			if (vbretval)
			{
				
				for (int iFld = 0; iFld < numFields; iFld++)
				{	
					this->get_CellValue(iFld, i, &val);
					(*retval)->EditCellValue(iFld, count, val, &vbretval);
				}
				count++;
			}
			shp->Release();
		}
	}
	
	// working with second shapefile
	percent = 0;
	std::vector<ShapeData*>* data = ((CShapefile*)sf)->get_ShapeVector();

	for (int i = 0; i < numShapes2; i++)
	{
		if( globalCallback != NULL ) 
		{
			long newpercent = (long)(((double)i/numShapes2)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Writing..."));
			}
		}
		
		if(SelectedOnly && !(*data)[i]->selected)
			continue;

		IShape* shp = NULL;
		sf->get_Shape(i, &shp);
		if (shp)
		{
			vbretval = VARIANT_FALSE;
			if (_isEditingShapes)
			{
				IShape* shpCopy = NULL;
				shp->Clone(&shpCopy);
				if (shpCopy)
				{
					(*retval)->EditInsertShape(shpCopy, &count, &vbretval);
					shpCopy->Release();
				}
			}
			else
			{
				(*retval)->EditInsertShape(shp, &count, &vbretval);
			}
			
			// copying fields
			if (vbretval)
			{
				std::map<long, long>::iterator p = fieldMap.begin();
				while (p != fieldMap.end())
				{
					sf->get_CellValue(p->first, i, &val);
					(*retval)->EditCellValue(p->second, count, val, &vbretval);
					p++;
				}
				count++;
			}
			shp->Release();
		}
	}

	long numShapes;
	(*retval)->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		VARIANT_BOOL vbretval;
		(*retval)->Close(&vbretval);
		(*retval)->Release();
		(*retval) = NULL;
	}

	//  cleaning
	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key),0,A2BSTR(""));

	return S_OK;
}

#pragma endregion

// **********************************************************************
//		InsertShapes()
// **********************************************************************
// A utility function to add geometry produced by simplification routine to the sfTarget shapefile,
// with copying of attributes from source shapefile.
// initShapeIndex - the index of shape to copy the attribute from
void InsertGeosGeometry(IShapefile* sfTarget, GEOSGeometry* gsNew, IShapefile* sfSouce, int initShapeIndex )
{
	if (gsNew)
	{
		std::vector<IShape*> shapes;
		if (GeometryConverter::GEOSGeomToShapes(gsNew, &shapes))
		{
			long index, numFields;
			VARIANT_BOOL vbretval;
			sfTarget->get_NumFields(&numFields);

			for (unsigned int j = 0; j < shapes.size(); j++)
			{
				sfTarget->get_NumShapes(&index);
				sfTarget->EditInsertShape(shapes[j], &index, &vbretval);
				
				if (vbretval)
				{
					CComVariant val;
					for (int f = 0; f < numFields; f++)
					{
						sfSouce->get_CellValue(f, initShapeIndex, &val);
						sfTarget->EditCellValue(f, index, val, &vbretval);
					}
				}
			}
		}
	}
}


// **********************************************************************
//		SimplifyLines()
// **********************************************************************
STDMETHODIMP CShapefile::SimplifyLines(DOUBLE Tolerance, VARIANT_BOOL SelectedOnly, IShapefile** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	
	(*retVal) = NULL;

	// do we have enough shapes, are they selected?
	long numShapes = _shapeData.size();		
	if (numShapes == 0) 
		return S_OK;

	LONG numFields;
	this->get_NumFields(&numFields);

	ShpfileType shpType = Utility::ShapeTypeConvert2D(_shpfiletype);
	if (shpType != SHP_POLYLINE && shpType != SHP_POLYGON)
	{
		ErrorMessage(tkINCOMPATIBLE_SHAPEFILE_TYPE);
		return S_OK;
	}

	IShapefile* sfNew = NULL;
	this->Clone(&sfNew);

	if (!sfNew)
		return S_OK;

	VARIANT_BOOL vbretval;
	long index = 0, percent = 0;
	
	for (int i = 0; i < numShapes; i++)
	{
		if( globalCallback != NULL )
		{
			long newpercent = (long)(((double)(i + 1)/numShapes)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Calculating..."));
			}
		}

		if(SelectedOnly && !(_shapeData)[i]->selected)
			continue;
		
		IShape* shp = NULL;
		this->get_Shape(i, &shp);
		GEOSGeometry* gsGeom = GeometryConverter::Shape2GEOSGeom(shp);
		shp->Release();
		if (gsGeom == NULL) continue;

		int numGeom = GEOSGetNumGeometries(gsGeom);
			
		/*	int numRings = 0;
			if (type == "Polygon")
				numRings = GEOSGetNumInteriorRings(gsGeom);*/

		if (shpType == SHP_POLYLINE)
		{
			GEOSGeom gsNew = GEOSSimplify(gsGeom, Tolerance);
			if (gsNew)
			{
				InsertGeosGeometry(sfNew, gsNew, this, i);
				GEOSGeom_destroy(gsNew);
			}
		}
		else
		{
			char* val = GEOSGeomType(gsGeom);
			CString type = val;
			CPLFree(val);

			if (type != "MultiPolygon")
			{
				GEOSGeom gsNew = GeometryConverter::SimplifyPolygon(gsGeom, Tolerance);
				if (gsNew)
				{
					InsertGeosGeometry(sfNew, gsNew, this, i);
					GEOSGeom_destroy(gsNew);
				}
			}
			else
			{
				for (int n = 0; n < GEOSGetNumGeometries(gsGeom); n++)
				{
					const GEOSGeometry* gsPart = GEOSGetGeometryN(gsGeom, n);
					GEOSGeom gsNew = GeometryConverter::SimplifyPolygon(gsPart, Tolerance);
					if (gsPart)
					{
						InsertGeosGeometry(sfNew, gsNew, this, i);
						GEOSGeom_destroy(gsNew);
					}
				}
			}
		}
		GEOSGeom_destroy(gsGeom);
	}

	globalCallback->Progress(OLE2BSTR(key),100,A2BSTR(""));

	sfNew->get_NumShapes(&numShapes);
	if (numShapes == 0)
	{
		sfNew->Close(&vbretval);
		sfNew->Release();
	}
	else
	{
		(*retVal) = sfNew;
	}
	return S_OK;
}

