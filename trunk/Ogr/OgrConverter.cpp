//********************************************************************************************************
//File name: OGR2MWShape.cpp
//Description: functions for conversion between OGRGeomerty Class and MapWinGis Shape Representation
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The initial version of this code was adapted from GDAL/OGR library by Sergei Leschinski on 31 jul 2009
//See OGRGeometry inheritance diagram is here: http://gdal.org/ogr/classOGRGeometry.html
//See discussion of here: http://www.mapwindow.org/phorum/read.php?5,13428
//Contributor(s): (Open source contributors should list themselves and their modifications here). 

#include "stdafx.h"
#include "OgrConverter.h"
#include "Shape.h"
#include "Shapefile.h"
#include "ogr_spatialref.h"
#include <GeosHelper.h>
#include "Ogr2Shape.h"

// *********************************************************************
//			SetConversionFactor()
// *********************************************************************
void ClipperConverter::SetConversionFactor(IShapefile* sf)
{
	if (!sf) return;
	CComPtr<IGeoProjection> proj = NULL;
	sf->get_GeoProjection(&proj);
	if (proj)
	{
		VARIANT_BOOL vb;
		proj->get_IsGeographic(&vb);
		this->conversionFactor = vb ? m_globalSettings.clipperGcsMultiplicationFactor : 1.0;;
		
	}
}

//**********************************************************************
//							ExplodePolygon()			               
//**********************************************************************
bool OgrConverter::MultiPolygon2Polygons(OGRGeometry* geom, vector<OGRGeometry*>* results)
{
	if (!geom || !results)
		return false;

	OGRwkbGeometryType oType = geom->getGeometryType();
	if (oType == wkbMultiPolygon || oType == wkbMultiPolygon25D)
	{
		OGRMultiPolygon* oMPoly = (OGRMultiPolygon *) geom;

		for(int iGeom=0; iGeom < oMPoly->getNumGeometries(); iGeom++ )
		{
			OGRGeometry* oPoly = oMPoly->getGeometryRef(iGeom);
			results->push_back(oPoly);
		}
		return (results->size() > 0);
	}
	else
		return false;
}

//**********************************************************************
//							AddPoints()			               
//**********************************************************************
void AddPoints(CShape* shp, OGRLineString* geom, int startPointIndex, int endPointIndex)
{
	ShpfileType shpType;
	shp->get_ShapeType(&shpType);
	bool isM = Utility::ShapeTypeIsM(shpType);

	VARIANT_BOOL vb;
	double x, y, z;
	for( int i = startPointIndex; i < endPointIndex; i++ )
	{
		if (isM)
		{
			shp->get_XY(i, &x, &y, &vb);
			shp->get_M(i, &z);
		}
		else
		{
			shp->get_XYZ(i, &x, &y, &z);
		}
		geom->addPoint(x, y, z);
	}
}

//**********************************************************************
//							ShapeToGeometry()			               
//**********************************************************************
// Converts MapWinGis shape object to OGR geometry object.
OGRGeometry* OgrConverter::ShapeToGeometry(IShape* shape, OGRwkbGeometryType forceGeometryType)
{
	if( shape == NULL)
		return NULL;
	
	CShape* shp = (CShape*)shape;

	OGRGeometry * oGeom = NULL;
		
	long numPoints, numParts;
	long beg_part, end_part;
	ShpfileType shptype;
	double x,y,z;
	
	shp->get_ShapeType(&shptype);
	shp->get_NumParts(&numParts);
	shp->get_NumPoints(&numPoints);
	
	if (numPoints == 0)
		return NULL;

	VARIANT_BOOL vb;

	if (shptype == SHP_POINT || shptype == SHP_POINTM || shptype == SHP_POINTZ)
	{
		if (shptype == SHP_POINTM)
		{
			shp->get_XY(0, &x, &y, &vb);
			shp->get_M(0, &z);
		}
		else
		{
			shp->get_XYZ(0, &x, &y, &z);
		}
		OGRPoint *oPnt = (OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
		oPnt->setX(x);
		oPnt->setY(y);
		oPnt->setZ(z);
		oGeom = oPnt;
	}

	else if (shptype == SHP_MULTIPOINT || shptype == SHP_MULTIPOINTM || shptype == SHP_MULTIPOINTZ)
	{
		if (numPoints == 0)
			oGeom = NULL;
		else
		{
			OGRMultiPoint *oMPnt = (OGRMultiPoint*)OGRGeometryFactory::createGeometry(wkbMultiPoint);
			for( int i = 0; i < numPoints; i++ )
			{
				if (shptype == SHP_MULTIPOINTM)
				{
					shp->get_XY(i, &x, &y, &vb);
					shp->get_M(i, &z);
				}
				else
				{
					shp->get_XYZ(i, &x, &y, &z);
				}
				OGRPoint *oPnt = (OGRPoint*)OGRGeometryFactory::createGeometry(wkbPoint);
				oPnt->setX(x);
				oPnt->setY(y);
				oPnt->setZ(z);
				oMPnt->addGeometryDirectly(oPnt);		// no memory release is needed when "direct" methods are used
			}
			oGeom = oMPnt;
		}
	}
	else if (shptype == SHP_POLYLINE || shptype == SHP_POLYLINEM || shptype == SHP_POLYLINEZ)
	{	
		if (numParts == 0)
			oGeom = NULL;
		
		bool multiLineString = forceGeometryType == wkbMultiLineString || forceGeometryType == wkbMultiLineString;
		if (numParts == 1 && !multiLineString)
		{	
			OGRLineString *oLine = (OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
			AddPoints(shp, oLine, 0, numPoints);
			oGeom = oLine;
		}
		else
		{
			OGRMultiLineString* oMLine = (OGRMultiLineString*)OGRGeometryFactory::createGeometry(wkbMultiLineString);
			
			for( int j = 0; j < numParts; j++ )
			{
				shp->get_Part(j, &beg_part);
				shp->get_EndOfPart(j, &end_part);
			
				OGRLineString* oLine = (OGRLineString*)OGRGeometryFactory::createGeometry(wkbLineString);
				AddPoints(shp, oLine, beg_part, end_part + 1);
				oMLine->addGeometryDirectly(oLine);
			}
			oGeom = oMLine;
		}
	}
		
	else if (shptype == SHP_POLYGON || shptype == SHP_POLYGONM || shptype == SHP_POLYGONZ)
	{	
		if (numParts == 0)
			return NULL;

		bool multiPolygon = (forceGeometryType == wkbMultiPolygon || forceGeometryType == wkbMultiPolygon25D);
		if (numParts == 1 && !multiPolygon)
		{	
			OGRLinearRing* oRing = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
			AddPoints(shp, oRing, 0, numPoints);

			OGRPolygon* oPoly = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
			oPoly->addRingDirectly(oRing);
			oGeom = oPoly;
		}
		else
		{	
			OGRPolygon** tabPolygons = new OGRPolygon*[numParts];
			
			for( int j = 0; j < numParts; j++ )
			{			
				shp->get_Part(j, &beg_part);
				shp->get_EndOfPart(j, &end_part);
				
				OGRLinearRing* oRing = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
				AddPoints(shp, oRing, beg_part, end_part + 1);
				
				tabPolygons[j] = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
				tabPolygons[j]->addRingDirectly(oRing);
			}				
			int isValidGeometry;
			const char* papszOptions[] = {"METHOD=ONLY_CCW", NULL};
				
			oGeom = OGRGeometryFactory::organizePolygons
					((OGRGeometry**)tabPolygons, numParts, &isValidGeometry, papszOptions);
			delete[] tabPolygons;

			if (multiPolygon)
				oGeom = OGRGeometryFactory::forceToMultiPolygon(oGeom);		// TODO: should we destroy the initial one?

			//if (!isValidGeometry) all polygons will be contained in one multipolygon;
		}
	}
	else
		return NULL;	//	other types aren't supported
	
	if (oGeom != NULL)
	{		
		if (shptype == SHP_POINT || shptype == SHP_MULTIPOINT || shptype == SHP_POLYLINE || shptype == SHP_POLYGON)
			oGeom->setCoordinateDimension(2);
		else
			oGeom->setCoordinateDimension(3);
	}

	return oGeom;
}

/***********************************************************************/
/*							GeometryToShapes()			               */
/***********************************************************************/

/*  Converts OGR geometry object to MapWinGis shape objects. Designed
 *  to handle geometry collections which can store geometry objects of 
 *  different types.
 *  
 *	@param oGeom	input OGR geometry
 *	@param vShapes  vector to return resulting shapes
 *
 *  @return true when at least one shape was created, and false otherwise
 */
bool OgrConverter::GeometryToShapes(OGRGeometry* oGeom, vector<IShape *>* vShapes, bool isM, 
			OGRwkbGeometryType oForceType /*= wkbNone*/, bool force25D /*= false*/ )
{
	IShape* shp;
	vShapes->clear();

	if (oGeom == NULL) return false;
	
	bool bPoly =false;		bool bPoly25 =false;
	bool bLine = false;		bool bLine25 = false;
	bool bPoint = false;	bool bPoint25 = false;

	OGRwkbGeometryType oType;
	oType = oGeom->getGeometryType();

	if (oType == wkbGeometryCollection || oType == wkbGeometryCollection25D)
	{
		OGRGeometryCollection* oGCol = (OGRGeometryCollection *) oGeom;
		OGRGeometry* oTest;
		
		for (int i =0; i < oGCol->getNumGeometries(); i++)		
		{	
			oTest =  (OGRGeometry *) oGCol->getGeometryRef(i);
			oType = oTest->getGeometryType(); 
			
			if		(oType == wkbPoint)				bPoint = true;
			else if (oType == wkbPoint25D)			bPoint25 = true;
			else if (oType == wkbLineString)		bLine = true;
			else if (oType == wkbLineString25D)		bLine25  = true;
			else if (oType == wkbPolygon)			bPoly = true;
			else if (oType == wkbPolygon25D)		bPoly25 = true;
			else return false;		// we don't accept other types for now
		}
		if (bPoly25)
		{
			shp = OgrConverter::GeometryToShape(oGeom, isM, wkbMultiPolygon25D);
			if (shp != NULL) vShapes->push_back(shp);
		}
		else if (bPoly)
		{
			shp = OgrConverter::GeometryToShape(oGeom, isM, wkbMultiPolygon);
			if (shp != NULL) vShapes->push_back(shp);
		}

		if (bLine25)
		{
			shp = OgrConverter::GeometryToShape(oGeom, isM, wkbMultiLineString25D);
			if (shp != NULL) vShapes->push_back(shp);
		}
		else if (bLine)
		{
			shp = OgrConverter::GeometryToShape(oGeom, isM, wkbMultiLineString);
			if (shp != NULL) vShapes->push_back(shp);
		}

		if (bPoint25)
		{
			shp = OgrConverter::GeometryToShape(oGeom, isM, wkbMultiPoint25D);
			if (shp != NULL) vShapes->push_back(shp);
		}
		else if (bPoint)
		{
			shp = OgrConverter::GeometryToShape(oGeom, isM, wkbMultiPoint);
			if (shp != NULL) vShapes->push_back(shp);
		}
	}
	else
	{
		shp = OgrConverter::GeometryToShape(oGeom, isM, wkbNone, oForceType, force25D );
		if (shp != NULL) vShapes->push_back(shp);
	}
	return vShapes->size() > 0;
}

/***********************************************************************/
/*							GeometryToShape()			               */
/***********************************************************************/

/*  Converts OGR geometry object to a single MapWinGis shape object. In 
 *  case of geometry collections return only type of geometries specified
 *	by parameter.
 *  
 *	@param oGeom	 input OGR geometry
 *	@param oBaseType sub type of geometry collection. Geometry collection 
 *					 will be treated as object of this geometry type
 *
 *  @return pointer to shape on success, or NULL otherwise
 */
IShape * OgrConverter::GeometryToShape(OGRGeometry* oGeom, bool isM, 
		OGRwkbGeometryType oBaseType /*= wkbNone*/, OGRwkbGeometryType oForceType /*= wkbNone*/, 
		bool force25D /*= false*/)
{
	if (oGeom == NULL)
		return NULL;
	
	IShape* shp = NULL;

	IPoint* pnt;
	ShpfileType shptype;
	VARIANT_BOOL retval;
	long partIndex = 0;
	long pointIndex = 0;
	
	OGRwkbGeometryType oType;
	oType = oGeom->getGeometryType();

	if (oType == wkbGeometryCollection || oType == wkbGeometryCollection25D)
	{
		if ( oBaseType == wkbMultiPolygon || oBaseType == wkbMultiPolygon25D ||
			 oBaseType == wkbMultiLineString || oBaseType == wkbMultiLineString25D ||
			 oBaseType == wkbMultiPoint || oBaseType == wkbMultiPoint25D)
	
			oType = oBaseType;
		else
		{
			// to return objects of all types from geometry collection use GeometryToShapes
			return NULL;	
		}
	}

	if (oForceType != wkbNone)
		oType = oForceType;

	if (oType == wkbPoint || oType == wkbPoint25D) 
	{
		if		(oType == wkbPoint) 	shptype = SHP_POINT;
		else if (oType == wkbPoint25D)	shptype = isM ? SHP_POINTM : SHP_POINTZ;
		
		OGRPoint* oPnt = (OGRPoint *) oGeom;

		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
		shp->put_ShapeType(shptype);

		ComHelper::CreatePoint(&pnt);
		pnt->put_X(oPnt->getX());
		pnt->put_Y(oPnt->getY());
		if (isM)	pnt->put_M(oPnt->getZ());
		else		pnt->put_Z(oPnt->getZ());
		shp->InsertPoint(pnt,&pointIndex,&retval);
		pnt->Release();
	}
	
	else if (oType == wkbMultiPoint || oType == wkbMultiPoint25D)
	{
		if		(oType == wkbMultiPoint) 	shptype = SHP_MULTIPOINT;
		else if (oType == wkbMultiPoint25D)	shptype = isM ? SHP_MULTIPOINTM : SHP_MULTIPOINTZ;
		
		OGRMultiPoint* oMPnt = (OGRMultiPoint* ) oGeom;
		if (oMPnt->getNumGeometries() == 0)
			return NULL;
		
		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
		shp->put_ShapeType(shptype);

		for(long i = 0; i < oMPnt->getNumGeometries(); i++ )
		{
			OGRPoint* oPnt = (OGRPoint *) oMPnt->getGeometryRef(i);
			if (oPnt != NULL)
			{
				if (oPnt->getGeometryType() == wkbPoint || oPnt->getGeometryType() == wkbPoint25D)
				{
					ComHelper::CreatePoint(&pnt);
					pnt->put_X(oPnt->getX());
					pnt->put_Y(oPnt->getY());
					if (isM)	pnt->put_M(oPnt->getZ());
					else		pnt->put_Z(oPnt->getZ());
					shp->InsertPoint(pnt,&i,&retval);
					pnt->Release();
				}
			}
		}
	}
	
	else if (oType == wkbLineString || oType == wkbLineString25D)
	{
		if		(oType == wkbLineString) 	shptype = SHP_POLYLINE;
		else if (oType == wkbLineString25D)	shptype = isM ? SHP_POLYLINEM : SHP_POLYLINEZ;
		
		OGRLineString* oLine = (OGRLineString *) oGeom;
		if (oLine->getNumPoints() == 0)
			return NULL;

		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
		shp->put_ShapeType(shptype);
		shp->InsertPart(0,&partIndex,&retval);
	
		for(long i = 0; i < oLine->getNumPoints(); i++ )
		{
			ComHelper::CreatePoint(&pnt);
			pnt->put_X(oLine->getX(i));
			pnt->put_Y(oLine->getY(i));
			if (isM)	pnt->put_M(oLine->getZ(i));
			else		pnt->put_Z(oLine->getZ(i));
			shp->InsertPoint(pnt,&i,&retval);
			pnt->Release();
		}
	}
	
	else if (oType == wkbMultiLineString || oType == wkbMultiLineString25D)
	{
		if		(oType == wkbMultiLineString) 		shptype = SHP_POLYLINE;
		else if (oType == wkbMultiLineString25D)	shptype = isM ? SHP_POLYLINEM : SHP_POLYLINEZ;

		OGRMultiLineString * oMLine = (OGRMultiLineString *) oGeom;
		if (oMLine->getNumGeometries() == 0) 
			return NULL;

		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
		shp->put_ShapeType(shptype);
		
		long count = 0;
		for (long j=0; j < oMLine->getNumGeometries(); j++)
		{	
			OGRLineString *oLine;
			oLine = (OGRLineString *) oMLine->getGeometryRef(j);
			
			if (!oLine == NULL)
			{
				if (oLine->getGeometryType() == wkbLineString || oLine->getGeometryType() == wkbLineString25D)
				{
					shp->InsertPart(count,&j,&retval);

					for(long i = 0; i < oLine->getNumPoints(); i++ )
					{
						ComHelper::CreatePoint(&pnt);
						pnt->put_X(oLine->getX(i));
						pnt->put_Y(oLine->getY(i));
						if (isM)	pnt->put_M(oLine->getZ(i));
						else		pnt->put_Z(oLine->getZ(i));
						shp->InsertPoint(pnt,&count,&retval);
						pnt->Release();
						if (retval) count++;
					}
				}
			}
		}
	}
	
	// (meant to be part of poly; but we'll treat it alone also)
	else if (oType == wkbLinearRing)	
	{
		OGRLinearRing* oRing;
		
		shptype = SHP_POLYGON;
		if (oGeom->getCoordinateDimension() == 3)	shptype = isM ? SHP_POLYGONM : SHP_POLYGONZ;
		else										shptype = SHP_POLYGON;
		
		oRing = (OGRLinearRing*) oGeom;
		if (oRing->getNumPoints() == 0) 
			return NULL;
		
		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
		shp->put_ShapeType(shptype);
		shp->InsertPart(0,&partIndex,&retval);

		for(long i=0; i< oRing->getNumPoints(); i++)
		{
			ComHelper::CreatePoint(&pnt);
			pnt->put_X(oRing->getX(i));
			pnt->put_Y(oRing->getY(i));
			if (isM)	pnt->put_M(oRing->getZ(i));
			else		pnt->put_Z(oRing->getZ(i));
			shp->InsertPoint(pnt,&i,&retval);
			pnt->Release();
		}
	}

	else if (oType == wkbPolygon || oType == wkbPolygon25D || oType == wkbMultiPolygon || oType == wkbMultiPolygon25D)
	{
		OGRPolygon*      oPoly;
		OGRLinearRing**  papoRings=NULL;
		int nRings = 0;
		
		if ((oType == wkbPolygon25D || oType == wkbMultiPolygon25D) || force25D)	shptype = isM ? SHP_POLYGONM : SHP_POLYGONZ;
		else if (oType == wkbPolygon || oType == wkbMultiPolygon)		shptype = SHP_POLYGON;
		
		if (oType == wkbPolygon || oType == wkbPolygon25D)
		{
			oPoly = (OGRPolygon *) oGeom;
			
			// there is no use of holes if we have no poly
			if( oPoly->getExteriorRing() == NULL)		return NULL;	
			if (oPoly->getExteriorRing()->IsEmpty())	return NULL;
			
			int nSrcRings = oPoly->getNumInteriorRings()+1;
			papoRings = (OGRLinearRing **) malloc(sizeof(void*)* nSrcRings); //CPLMalloc(sizeof(void*)* nSrcRings);

			for(int iRing = 0; iRing < nSrcRings; iRing++ )
			{
				if( iRing == 0 )	papoRings[nRings] = oPoly->getExteriorRing();
				else				papoRings[nRings] = oPoly->getInteriorRing(iRing-1);
				
				if (!papoRings[nRings]->IsEmpty()) 
					nRings ++;
			}
		}
				
		else if (oType == wkbMultiPolygon || oType == wkbMultiPolygon25D)
		{
			OGRMultiPolygon* oMPoly = (OGRMultiPolygon *) oGeom;

			for(int iGeom=0; iGeom < oMPoly->getNumGeometries(); iGeom++ )
			{
				oPoly = (OGRPolygon *) oMPoly->getGeometryRef(iGeom);
				
				if (oPoly->getGeometryType() == wkbPolygon || oPoly->getGeometryType() == wkbPolygon25D)
				{					
					if( oPoly->getExteriorRing() == NULL)		continue;
					if (oPoly->getExteriorRing()->IsEmpty())	continue;
							
					papoRings = (OGRLinearRing **) realloc(papoRings, sizeof(void*)* (nRings + oPoly->getNumInteriorRings() + 1));
					for(int iRing = 0; iRing < oPoly->getNumInteriorRings()+1; iRing++ )
					{
						if(iRing == 0)	papoRings[nRings] = oPoly->getExteriorRing();
						else			papoRings[nRings] = oPoly->getInteriorRing( iRing-1 );
						
						if (!papoRings[nRings]->IsEmpty())
						nRings ++;
					}
				}
			}
		}
		
		if (nRings == 0)
		{	
			if (papoRings !=NULL) free(papoRings);
			return NULL;
		}

		CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
		shp->put_ShapeType(shptype);
		
		OGRLinearRing *oRing;
		long count = 0;

		for(long iRing = 0; iRing < nRings; iRing++ )	
		{
			oRing = papoRings[iRing];
			shp->InsertPart(count,&iRing,&retval);
			
			for(long i = 0; i < oRing->getNumPoints(); i++ )		
			{
				ComHelper::CreatePoint(&pnt);
				pnt->put_X(oRing->getX(i));
				pnt->put_Y(oRing->getY(i));
				if (isM)	pnt->put_M(oRing->getZ(i));
				else		pnt->put_Z(oRing->getZ(i));
				shp->InsertPoint(pnt,&count,&retval);
				pnt->Release();
				pnt = NULL;
				if (retval) 
					count++;
			}
		}
		free(papoRings);
	}
	else	
		return NULL;	//(oType == wkbUnknown || oType == wkbNone)

	if (shp)
	{	
		long numPoints = 0;
		shp->get_NumPoints(&numPoints);
		if (numPoints == 0) 
		{
			shp->Release();
			shp = NULL;
		}
	}
	return shp;
}
 
/***********************************************************************/
/*						Read_OGR_Layer()				               */
/***********************************************************************/

/*  Reads OGR layer from the disk into memory shapefile. Format of input 
 *	layer is determined automatically by OGR library. Type of resulting
 *  shapefile is read from layer or specified as a parameter for formats
 *  which can store features of different types simultaneously (MapInfo 
 *  *.tab format for example). 
 *  
 *	@param Filename	 name of the file with a layer info on the disk
 *	@param shpType	 type of shapefile which will be created. Will be
 *					 read from layer if none was specified.
 *  @return resulting shapefile pointer on success, or NULL otherwise
 */
IShapefile* OgrConverter::ReadOgrLayer(BSTR Filename, ICallback* callback)
{
	IShapefile* sf = NULL;
	GDALDataset* ds = GdalHelper::OpenOgrDatasetW(OLE2W(Filename), false);
	if (!ds)
	{
		return NULL;
	}
	
	OGRLayer* layer = ds->GetLayer(0);
	if (layer)
	{
		sf = Ogr2Shape::CreateShapefile(layer);
		if (sf) {
			bool isTrimmed = false;
			Ogr2Shape::FillShapefile(layer, sf, m_globalSettings.ogrLayerMaxFeatureCount, false, callback, isTrimmed);
		}
	}
	GDALClose(ds);

	return sf;
}

/***********************************************************************/
/*			GeometryType2ShapeType()/ShapeType2GeometryType			   */
/***********************************************************************/

/*  Establish correspondence between the types of MapWinGis shapefile  
 *	layer and the types of ogr layers.
 */
ShpfileType OgrConverter::GeometryType2ShapeType(OGRwkbGeometryType oType)
{
	switch ( oType )
	{
		case wkbPoint:				return SHP_POINT;
		case wkbPoint25D:			return SHP_POINTZ;
		case wkbMultiPoint:			return SHP_MULTIPOINT;
		case wkbMultiPoint25D:		return SHP_MULTIPOINTZ;
		case wkbLineString:			
		case wkbMultiLineString:
									return SHP_POLYLINE;
		case wkbLineString25D:		
		case wkbMultiLineString25D:
									return SHP_POLYLINEZ;
		case wkbPolygon:			
		case wkbMultiPolygon:		
									return SHP_POLYGON;
		case wkbPolygon25D:
		case wkbMultiPolygon25D:	
									return SHP_POLYGONZ;
		case wkbNone:				return SHP_NULLSHAPE;
	}
	return SHP_NULLSHAPE;
}
OGRwkbGeometryType OgrConverter::ShapeType2GeometryType(ShpfileType shpType, bool forceMulti /*= false*/)
{
	switch( shpType )
	{
		case SHP_POINT:			return wkbPoint;
		case SHP_POINTM:		return wkbPoint;
		case SHP_POINTZ:		return wkbPoint25D;
		case SHP_MULTIPOINT:	return wkbMultiPoint;
		case SHP_MULTIPOINTM:	return wkbMultiPoint;
		case SHP_MULTIPOINTZ:	return wkbMultiPoint25D;
		case SHP_POLYLINE:		return forceMulti ? wkbMultiLineString : wkbLineString;
		case SHP_POLYLINEM:		return forceMulti ? wkbMultiLineString : wkbLineString;
		case SHP_POLYLINEZ:		return forceMulti ? wkbMultiLineString25D : wkbLineString25D;
		case SHP_POLYGON:		return forceMulti ? wkbMultiPolygon : wkbPolygon;
		case SHP_POLYGONM:		return forceMulti ? wkbMultiPolygon : wkbPolygon;
		case SHP_POLYGONZ:		return forceMulti ? wkbMultiPolygon25D : wkbPolygon25D;
		case SHP_NULLSHAPE:		return wkbNone;
	}
	return wkbNone;
}

// **********************************************************************
// *						Write_OGR_Layer()				               
// **********************************************************************
//  Writes memory shapefile to the file of specified format.
//  Isn't implemented yet.
bool OgrConverter::WriteOgrLayer(IShapefile* sf, BSTR Filename)
{
	return false;
}





