#include "stdafx.h"
#include "ClipperConverter.h"
#include "ShapefileHelper.h"

// *********************************************************************
//			Shape2ClipperPolygon()
// *********************************************************************
// Converts shape clipper polygon 
ClipperLib::Polygons* ClipperConverter::Shape2ClipperPolygon(IShape* shp)
{
	if (!shp) 
		return NULL;

	ShpfileType shpType;
	shp->get_ShapeType(&shpType);
	if (shpType != SHP_POLYGON && shpType != SHP_POLYGONM && shpType != SHP_POLYGONZ)
		return NULL;
	
	long numParts, numPoints;
	shp->get_NumParts(&numParts);
	shp->get_NumPoints(&numPoints);
	
	if (numPoints == 0 || numParts == 0)
		return NULL;
	
	double x, y;

	ClipperLib::Polygons* retval = new ClipperLib::Polygons();
	for (int i = 0; i < numParts; i++)
	{
		ClipperLib::Polygon polygon;
		
		long begin, end;
		shp->get_Part(i, &begin);
		shp->get_EndOfPart(i, &end);

		VARIANT_BOOL vbretval;
		for (int j = begin; j <= end - 1; j++ )			// -1 added
		{
			ClipperLib::IntPoint pnt;
			shp->get_XY(j, &x, &y, &vbretval);
			
			if (conversionFactor != 1.0)
			{
				x *= conversionFactor;
				y *= conversionFactor;
			}

			pnt.X = (ClipperLib::long64)x;
			pnt.Y = (ClipperLib::long64)y;
			polygon.push_back(pnt);
		}
		retval->push_back(polygon);
	}

	if (retval->size() > 0)
	{
		return retval;
	}
	else
	{
		delete retval;
		return NULL;
	}
}

// ******************************************************************
//		ClipperPolygon2Shape()
// ******************************************************************
// Converts clipper polygon to shape
// TODO: perhaps include polygon type as a parameter
IShape* ClipperConverter::ClipperPolygon2Shape(ClipperLib::Polygons* polygon)
{
	bool pointsExist = false;
	for (long i = 0; i < (long)polygon->size(); i++)
	{
		ClipperLib::Polygon* poly = &((*polygon)[i]);
		if (poly->size() > 0)
		{
			pointsExist = true;
			break;
		}
	}

	if (!pointsExist)
		return NULL;
	
	IShape* shp = NULL;
	CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shp);
	if (!shp) 
		return NULL;
	
	VARIANT_BOOL vbretval;
	shp->Create(SHP_POLYGON, &vbretval);

	double x, y;

	long cnt = 0;
	long part = 0;
	for (long i = 0; i < (long)polygon->size(); i++)
	{
		ClipperLib::Polygon* poly = &((*polygon)[i]);
		if (poly->size() > 0)
		{
			shp->InsertPart(cnt, &part, &vbretval);
			part++;
			
			int j = poly->size() - 1;
			for (; j >= 0; j--)
			{
				IPoint* pnt = NULL;
				m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
				
				x = (double)(*poly)[j].X;
				y = (double)(*poly)[j].Y;

				if (this->conversionFactor != 1.0)
				{
					x  /= conversionFactor;
					y  /= conversionFactor;
				}
				
				pnt->put_X(x);
				pnt->put_Y(y);
				
				shp->InsertPoint(pnt, &cnt, &vbretval);
				pnt->Release();
				cnt++;
			}
			
			// the first and the last point of the part must be the same
			int size = poly->size() - 1;
			if (size > 0)
			{
				if (((*poly)[0]).X != ((*poly)[size]).X ||
					((*poly)[0]).Y != ((*poly)[size]).Y)
				{
					IPoint* pnt = NULL;
					m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
					
					x = (double)(*poly)[size].X;	// slightly inoptimal, this point was calculated already
					y = (double)(*poly)[size].Y;

					if (this->conversionFactor != 1.0)
					{
						x  /= conversionFactor;
						y  /= conversionFactor;
					}
					
					pnt->put_X(x);
					pnt->put_Y(y);

					shp->InsertPoint(pnt, &cnt, &vbretval);
					pnt->Release();
					cnt++;
				}
			}
		}
	}
	return shp;
}



// ******************************************************************
//		ClipPolygon()
// ******************************************************************
ClipperLib::Polygons* ClipperConverter::ClipPolygon(ClipperLib::Polygons* polyClip, ClipperLib::Polygons* polySubject, ClipperLib::ClipType operation)
{
	if (polyClip && polySubject)
	{
		if (polyClip->size() > 0 && polySubject->size() > 0)
		{
			ClipperLib::Clipper* c = new ClipperLib::Clipper();
			ClipperLib::Polygons* solution = new ClipperLib::Polygons();
			
			for (unsigned int i = 0; i < polySubject->size(); i++)
			{
				c->AddPolygon((*polySubject)[i], ClipperLib::ptSubject);
			}

			for (unsigned int i = 0; i < polyClip->size(); i++)
			{
				c->AddPolygon((*polyClip)[i], ClipperLib::ptClip);
			}
			
			c->Execute(operation, *solution);		// ctIntersection
			c->Clear();
			delete c;
			return solution;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

// ******************************************************************
//		ClipPolygon()
// ******************************************************************
IShape* ClipperConverter::ClipPolygon(IShape* shapeClip, IShape* shapeSubject, PolygonOperation operation)
{
	ClipperLib::ClipType operNew;
	switch(operation)
	{
		case DIFFERENCE_OPERATION:
			operNew = ClipperLib::ctDifference;
			break;
		case INTERSECTION_OPERATION:
			operNew = ClipperLib::ctIntersection;
			break;
		case EXCLUSIVEOR_OPERATION:
			operNew = ClipperLib::ctXor;
			break;
		case UNION_OPERATION:
			operNew = ClipperLib::ctUnion;
			break;
		default: 
			return NULL;
	}
	
	VARIANT_BOOL vbretval;
	CString s;
	
	shapeSubject->get_PartIsClockWise(0, &vbretval);
	s = "Subject: ";
	s += vbretval ? "CW" : "CCW";
	//AfxMessageBox(s);
	
	shapeClip->get_PartIsClockWise(0, &vbretval);
	s = "Clip: ";
	s += vbretval ? "CW" : "CCW";
	//AfxMessageBox(s);

	//shapeSubject
	ClipperConverter ogr;
	ClipperLib::Polygons* poly1 = ogr.Shape2ClipperPolygon(shapeClip);
	ClipperLib::Polygons* poly2 = ogr.Shape2ClipperPolygon(shapeSubject);
	if (poly1 && poly2)
	{
		ClipperLib::Polygons* result = ClipPolygon(poly1, poly2, operNew);
		if (result)
		{
			IShape* shp = ogr.ClipperPolygon2Shape(result);
			delete result;
			return shp;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

// ***************************************************
//	   AddPolygonsToClipper
// ***************************************************
void ClipperConverter::AddPolygons(IShapefile* sf, ClipperLib::Clipper& clp, ClipperLib::PolyType clipType, bool selectedOnly)
{
	if (!sf) return;

	long numShapes;
	sf->get_NumShapes(&numShapes);

	ClipperConverter converter(sf);

	IShape* shp = NULL;
	for (long i = 0; i < numShapes; i++)
	{
		if (selectedOnly && !ShapefileHelper::ShapeSelected(sf, i))
			continue;

		sf->get_Shape(i, &shp);
		if (shp) {
			ClipperLib::Polygons* polys = converter.Shape2ClipperPolygon(shp);
			clp.AddPolygons(*polys, clipType);
			shp->Release();
		}
	}
}