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
#include "GeometryConverter.h"
#include "Shape.h"
#include "Shapefile.h"
#include "ogr_spatialref.h"
#include <GeosHelper.h>

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

// *********************************************************************
//			Shape2GEOSGeom()
// *********************************************************************
//  Converts MapWinGis shape to GEOS geometry
GEOSGeom GeometryConverter::Shape2GEOSGeom(IShape* shp)
{
	OGRGeometry* oGeom = ShapeToGeometry(shp);
	if (oGeom != NULL)
	{
		GEOSGeometry* result = GeosHelper::ExportToGeos(oGeom);
		OGRGeometryFactory::destroyGeometry(oGeom);
		return result;
	}
	else 
		return NULL;
}

GEOSGeometry* DoBuffer(DOUBLE distance, long nQuadSegments, GEOSGeometry* gsGeom)
{
	__try
	{
		return GeosHelper::Buffer(gsGeom, distance, nQuadSegments);
	}
	__except(1)
	{
		return NULL;
	}
}

// *********************************************************************
//		GEOSGeom2Shape()
// *********************************************************************
// Converts GEOSGeom to MapWinGIS shapes
bool GeometryConverter::GEOSGeomToShapes(GEOSGeom gsGeom, vector<IShape*>* vShapes, bool isM)
{
	bool substitute = false;
	if ( !GeosHelper::IsValid(gsGeom))
	{
		GEOSGeometry* gsNew = DoBuffer(m_globalSettings.invalidShapesBufferDistance, 30, gsGeom);
		if (GeosHelper::IsValid(gsNew))
		{
			//GEOSGeom_destroy(gsGeom);   // it should be deleted by caller as it can be a part of larger geometry
			gsGeom = gsNew;
			substitute = true;
		}
	}
	
	OGRGeometry* oGeom = GeosHelper::CreateFromGEOS(gsGeom);
	if (oGeom)
	{
		char* type = GeosHelper::GetGeometryType(gsGeom);
		CString s = type;
		GeosHelper::Free(type);
		
		OGRwkbGeometryType oForceType = wkbNone;
		if (s == "LinearRing" && oGeom->getGeometryType() != wkbLinearRing )
			oForceType = wkbLinearRing;

		bool result = GeometryToShapes(oGeom, vShapes, isM);
		OGRGeometryFactory::destroyGeometry(oGeom);

		if (substitute)
			GeosHelper::DestroyGeometry(gsGeom);
		return result;
	}
	else
	{
		if (substitute)
			GeosHelper::DestroyGeometry(gsGeom);

		return false;
	}
}

//**********************************************************************
//							ExplodePolygon()			               
//**********************************************************************
bool GeometryConverter::MultiPolygon2Polygons(OGRGeometry* geom, vector<OGRGeometry*>* results)
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
OGRGeometry* GeometryConverter::ShapeToGeometry(IShape* shape, OGRwkbGeometryType forceGeometryType)
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
bool GeometryConverter::GeometryToShapes(OGRGeometry* oGeom, vector<IShape*>* vShapes, bool isM, OGRwkbGeometryType oForceType )
{
	IShape* shp;
	if (!vShapes->empty()) vShapes->clear();

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
		{	shp = GeometryConverter::GeometryToShape(oGeom, isM, wkbMultiPolygon25D);
			if (shp != NULL) vShapes->push_back(shp);
		}
		else if (bPoly)		
		{	shp = GeometryConverter::GeometryToShape(oGeom, isM, wkbMultiPolygon);
			if (shp != NULL) vShapes->push_back(shp);
		}
		
		if (bLine25)	
		{	shp = GeometryConverter::GeometryToShape(oGeom, isM, wkbMultiLineString25D);
			if (shp != NULL) vShapes->push_back(shp);
		}
		else if (bLine)		
		{	shp = GeometryConverter::GeometryToShape(oGeom, isM, wkbMultiLineString);
			if (shp != NULL) vShapes->push_back(shp);
		}
		
		if (bPoint25)	
		{	shp = GeometryConverter::GeometryToShape(oGeom, isM, wkbMultiPoint25D);
			if (shp != NULL) vShapes->push_back(shp);
		}
		else if (bPoint)	
		{	shp = GeometryConverter::GeometryToShape(oGeom, isM, wkbMultiPoint);
			if (shp != NULL) vShapes->push_back(shp);
		}
	}
	else
	{
		shp = GeometryConverter::GeometryToShape(oGeom, isM, wkbNone, oForceType );
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
IShape * GeometryConverter::GeometryToShape(OGRGeometry* oGeom, bool isM, OGRwkbGeometryType oBaseType, OGRwkbGeometryType oForceType)
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

		m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
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
					m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
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
			m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
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
						m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
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
			m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
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
		
		if		(oType == wkbPolygon || oType == wkbMultiPolygon)			shptype = SHP_POLYGON;
		else if (oType == wkbPolygon25D || oType == wkbMultiPolygon25D)		shptype = isM ? SHP_POLYGONM : SHP_POLYGONZ;
	
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
				m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
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
IShapefile* GeometryConverter::Read_OGR_Layer(BSTR Filename, ShpfileType shpType)
{
	return NULL;
	//	USES_CONVERSION;
//	
//	CString fname = W2CA(Filename);
//	if (fname.GetLength() == 0) return NULL;
//
//	OGRRegisterAll();
//    
//	OGRDataSource*	oData;
//    oData = OGRSFDriverRegistrar::Open(fname, FALSE);
//    if( oData == NULL ) return NULL;
//
//	OGRLayer*	oLayer;
//	OGRFeature* oFeature;
//    
//	oLayer = oData->GetLayer(0);
//	OGRFeatureDefn* oLDefn = oLayer->GetLayerDefn();
//	oLayer->ResetReading();
//
///* ----------------------------------------------------------------- */
///*		Creation of shapefile										 */
///* ----------------------------------------------------------------- */
//	IShapefile* sf;
//	VARIANT_BOOL vbretval;
//	
//	if (shpType == SHP_NULLSHAPE)
//		shpType = GeometryType2ShapeType(oLDefn->GetGeomType());
//	
//	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)&sf);
//	sf->CreateNew(A2BSTR(""), shpType, &vbretval);
//    
///* ----------------------------------------------------------------- */
///*		Converting of fields										 */
///* ----------------------------------------------------------------- */
//    for(long iFld = 0; iFld < oLDefn->GetFieldCount(); iFld++ )
//    {
//		IField * fld = NULL;
//		CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&fld);
//		
//		OGRFieldDefn* oField = oLDefn->GetFieldDefn(iFld);
//		OGRFieldType type = oField->GetType();
//		
//		if( type == OFTInteger )	fld->put_Type(INTEGER_FIELD);
//        else if(type == OFTReal )	fld->put_Type(DOUBLE_FIELD);
//        else if(type == OFTString )	fld->put_Type(STRING_FIELD);
//
//		fld->put_Name(A2BSTR(oField->GetNameRef()));
//		fld->put_Width((long)oField->GetWidth());
//		fld->put_Precision((long)oField->GetPrecision());
//
//		sf->EditInsertField(fld, &iFld, NULL, &vbretval);
//		fld->Release();
//    }
//	
///* ----------------------------------------------------------------- */
///*		Converting of the shapes and cellvalues						 */
///* ----------------------------------------------------------------- */
//	while( (oFeature = oLayer->GetNextFeature()) != NULL )
//    {
//		OGRGeometry *oGeom;
//        oGeom = oFeature->GetGeometryRef();
//		if(oGeom == NULL) continue;
//		
//		IShape* shp =NULL;
//		shp = GeometryConverter::GeometryToShape(oGeom, Utility::ShapeTypeIsM(shpType));
//		
//		long numShapes;
//		sf->get_NumShapes(&numShapes);
//		sf->EditInsertShape(shp, &numShapes, &vbretval);
//
//        for(int iFld = 0; iFld < oLDefn->GetFieldCount(); iFld++ )
//        {
//            OGRFieldDefn* oField = oLDefn->GetFieldDefn(iFld);
//			OGRFieldType type = oField->GetType();
//			VARIANT val;
//			VariantInit(&val);
//
//			if(type == OFTInteger)	
//			{
//				val.vt = VT_I4;
//				val.lVal = oFeature->GetFieldAsInteger(iFld);
//			}
//            else if(type == OFTReal)
//			{
//				val.vt = VT_R8;
//				val.dblVal = oFeature->GetFieldAsDouble(iFld);
//			}
//		    else //if (type == OFTString )
//			{	
//				val.vt = VT_BSTR;
//				val.bstrVal = A2BSTR(oFeature->GetFieldAsString(iFld));	
//			}
//            sf->EditCellValue(iFld, numShapes, val, &vbretval);
//			VariantClear(&val);
//        }
//        OGRFeature::DestroyFeature(oFeature);
//    }
//	
//    OGRDataSource::DestroyDataSource(oData);
//
//	sf->RefreshExtents(&vbretval);
//	return sf;
}

/***********************************************************************/
/*			GeometryType2ShapeType()/ShapeType2GeometryType			   */
/***********************************************************************/

/*  Establish correspondence between the types of MapWinGis shapefile  
 *	layer and the types of ogr layers.
 */
ShpfileType GeometryConverter::GeometryType2ShapeType(OGRwkbGeometryType oType)
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
OGRwkbGeometryType GeometryConverter::ShapeType2GeometryType(ShpfileType shpType, bool forceMulti /*= false*/)
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
bool GeometryConverter::Write_OGR_Layer(IShapefile* sf, BSTR Filename)
{
	return false;
}

// ********************************************************************
//		MergeGeosGeometries
// ********************************************************************
// Returns GEOS geometry which is result of the union operation for the geometries passed
GEOSGeometry* GeometryConverter::MergeGeosGeometries( std::vector<GEOSGeometry*>& data, ICallback* callback, bool deleteInput )
{
	if (data.size() == 0)
		return NULL;
	
	USES_CONVERSION;
	GEOSGeometry* g1 = NULL;
	GEOSGeometry* g2 = NULL;
	
	bool stop = false;
	int count = 0;	// number of union operation performed
	long percent = 0;
	
	int size = data.size();
	int depth = 0;

	if (size == 1)
	{
		// no need for calculation
		if (deleteInput)
			return data[0];	 // no need to clone; it will be exactly the same
		else
		{
			GEOSGeometry* geomTemp = GeosHelper::CloneGeometry(data[0]);
			return geomTemp;
		}
	}

	BSTR key = A2BSTR("");
	while (!stop)
	{
		stop = true;

		for (int i = 0; i < size; i++)
		{
			if (data[i] != NULL)
			{
				if (!g1)
				{
					g1 = data[i];
					data[i] = NULL;
				}
				else
				{
					g2 = data[i];
					data[i] = NULL;
				}

				if (g2 != NULL)
				{
					GEOSGeometry* geom = GeosHelper::Union(g1, g2);
					data[i] = geom;		// placing the resulting geometry back for further processing
					
					if (deleteInput || depth > 0)	// in clipping operation geometries are used several times
													// so the intial geometries should be intact (depth == 0)
													// in other cases (Buffer, Dissolve) the geometries can be deleted in place
					{
						GeosHelper::DestroyGeometry(g1);
						GeosHelper::DestroyGeometry(g2);
					}
					
					g1 = NULL;
					g2 = NULL;
					count++;
					stop = false;		// in case there is at least one union occured, we shall run once more

					Utility::DisplayProgress(callback, count, size, "Merging shapes...", key, percent);
				}
				
				// it is the last geometry, unpaired one, not the only one, it's the initial and must not be deleted
				if (i == size -1 && stop == false && g2 == NULL && g1 != NULL && depth == 0 && !deleteInput)
				{
					// we need to clone it, to be able to apply unified memory management afterwards
					// when depth > 0 all interim geometries are deleted, while this one should be preserved
					GEOSGeometry* geomTemp = GeosHelper::CloneGeometry(g1);
					g1 = geomTemp;
				}
			}
		}
		depth++;
	}
	return g1;
}

// *****************************************************
//		SimplifyPolygon()
// *****************************************************
// A polygon is expected as input 
// (multi-polygons should be split into parts before treating with this routine)
GEOSGeometry* GeometryConverter::SimplifyPolygon(const GEOSGeometry *gsGeom, double tolerance)
{
	const GEOSGeometry* gsRing =  GeosHelper::GetExteriorRing(gsGeom);	// no memory is allocated there
	GEOSGeom gsPoly = GeosHelper::TopologyPreserveSimplify(gsRing, tolerance);		// memory allocation

	if (!gsPoly)
		return NULL;
	
	std::vector<GEOSGeom> holes;
	for (int n = 0; n < GeosHelper::GetNumInteriorRings(gsGeom); n++)
	{
		gsRing = GeosHelper::GetInteriorRingN(gsGeom, n);				// no memory is allocated there
		if (gsRing)
		{
			GEOSGeom gsOut = GeosHelper::TopologyPreserveSimplify(gsRing, tolerance);	// memory allocation
			if (gsOut)
			{
				char* type = GeosHelper::GetGeometryType(gsOut);
				CString s = type;
				GeosHelper::Free(type);
				if (s == "LinearRing")
					holes.push_back(gsOut);
			}
		}
	}

	GEOSGeometry *gsNew = NULL;
	if (holes.size() > 0)
	{
		gsNew = GeosHelper::CreatePolygon(gsPoly, &(holes[0]), holes.size()); // memory allocation (should be released by caller)
	}
	else
	{
		gsNew = GeosHelper::CreatePolygon(gsPoly, NULL, 0);
	}
	return gsNew;

	/*GEOSGeometry* result = GeosHelper::Simplify(gsGeom, tolerance);
	char* type = GeosHelper::GetGeometryType(result);
	return result;*/
}
