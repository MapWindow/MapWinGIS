//********************************************************************************************************
//File name: Utils.cpp
//Description: Implementation of CUtils.
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
//********************************************************************************************************
//Contributor(s): dpa, angela, cdm, Rob Cairns, lsu
//********************************************************************************************************

#include "stdafx.h"
#include "Utils.h"
#include <stack>
#include "colour.h"
#include "Projections.h"
#include "GeometryConverter.h"
#include "LineBresenham.h"
#include "Image.h"
#include "Shapefile.h"
#include "GeoProjection.h"
#include "macros.h"
#include "XRedBlackTree.h"
#include "YRedBlackTree.h"
#include "GridInterpolate.h"
#include "PointInPolygon.h"
#include "Grid.h"
#include "Extents.h"
#include "TableClass.h"
#include "Shape.h"
#include "Vector.h"
#include "Expression.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

STDMETHODIMP CUtils::PointInPolygon(IShape *Shape, IPoint *TestPoint, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Shape != NULL )
	{	
		pip_cache_parts.clear();
		pip_cache_pointsX.clear();
		pip_cache_pointsY.clear();
	
		ShpfileType shptype;
		Shape->get_ShapeType(&shptype);

		if(shptype != SHP_POLYGON && shptype != SHP_POLYGONZ && shptype != SHP_POLYGONM )
		{	
			*retval = FALSE;
			this->ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
			return S_OK;
		}

		((CShape*)Shape)->get_ExtentsXY(pip_left,pip_bottom,pip_right,pip_top);

		long numParts = 0;
		long numPoints = 0;
		Shape->get_NumParts(&numParts);
		Shape->get_NumPoints(&numPoints);
		
		if(numParts == 0)//ah 11/22/05, for in-memory shapes that have no parts
		{
			pip_cache_parts.push_back(0);			
		}
		else
		{
			long part =0;
			for( int j = 0; j < numParts; j++ )
			{	
				Shape->get_Part(j,&part);
				pip_cache_parts.push_back(part);
			}
		}
		
		double pointX = 0.0;
		double pointY = 0.0;
		VARIANT_BOOL vbretval;
		for( int i = 0; i < numPoints; i++ )
		{	
			Shape->get_XY(i, &pointX, &pointY, &vbretval);
			pip_cache_pointsX.push_back(pointX);
			pip_cache_pointsY.push_back(pointY);
		}
	}
	
	if( TestPoint != NULL )
	{
		double test_pointX, test_pointY;	
		TestPoint->get_X(&test_pointX);
		TestPoint->get_Y(&test_pointY);
			
		//Initial Test on Bounds
		if( test_pointX < pip_left )
		{	
			*retval = FALSE;
			return S_OK;
		}
		else if( test_pointX > pip_right )
		{	
			*retval = FALSE;
			return S_OK;
		}
		if( test_pointY < pip_bottom )
		{	
			*retval = FALSE;
			return S_OK;
		}
		else if( test_pointY > pip_top )
		{	
			*retval = FALSE;
			return S_OK;
		}
			
		//X = U;
		//Y = V;
		//Always drop Z Coordinate and Project on XY Plane
		int beg_part = 0;
		int end_part = 0;

		int number_in_polygons = 0;
		int numCacheParts = (int)pip_cache_parts.size();//ah 6/6/05
		
		for( int j = 0; j < numCacheParts; j++ )
		{
			beg_part = pip_cache_parts[j];
			if( (numCacheParts - 1) > j )
				end_part = pip_cache_parts[j+1];
			else
				end_part = pip_cache_pointsX.size();

			int SH = 0;
			int NSH = 0;
			int NC = 0;		//number_crossings

			for( int i = beg_part; i < end_part - 1; i++ )
			{	
				long corner_two_index = i + 1;
				if( i == pip_cache_pointsX.size() - 1 )
					corner_two_index = beg_part;
				
				//Translate points to origin centered on test_point
				double corner_oneX = pip_cache_pointsX[i] - test_pointX;
				double corner_oneY = pip_cache_pointsY[i] - test_pointY;
				double corner_twoX = pip_cache_pointsX[corner_two_index] - test_pointX;
				double corner_twoY = pip_cache_pointsY[corner_two_index] - test_pointY;
				
				set_sign( corner_oneY, SH );
				set_sign( corner_twoY, NSH );
			
				if( does_cross( SH, NSH, corner_oneX, corner_oneY, corner_twoX, corner_twoY ) )
					NC++;	
				SH = NSH;		
			}
			//ODD Crossings = point_in_part
			if( NC % 2 != 0 )
				number_in_polygons++;		
		}

		//cout<<number_in_polygons<<endl;
		//ODD Part Crossings = point_in_polygon
		if( number_in_polygons% 2 != 0 )
			*retval = VARIANT_TRUE;
		else
			*retval = VARIANT_FALSE;
	}
	else
		*retval = VARIANT_FALSE;
	
	return S_OK;
}

inline bool CUtils::does_cross( int SH, int NSH, double corner_oneX, double corner_oneY, double corner_twoX, double corner_twoY )
{	
	if( SH != NSH )
	{	if( corner_oneX > 0 && corner_twoX > 0 )
			return true;
		else if( corner_oneX > 0 || corner_twoX > 0 )
		{	
			//b = v - u*m
			double m = ( corner_twoX - corner_oneX )/					
					   ( corner_twoY - corner_oneY );

			if( ( corner_oneX - corner_oneY*m ) > 0 )
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

inline void CUtils::set_sign( double val, int & SH )
{	if( val < 0 )
		SH = -1;
	else
		SH = 1;
}

STDMETHODIMP CUtils::GridReplace(IGrid *Grid, VARIANT OldValue, VARIANT NewValue, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if( Grid == NULL )
	{	
		*retval = NULL;
		lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	long ncols = 0, nrows = 0;
	IGridHeader * header = NULL;
	Grid->get_Header(&header);
	header->get_NumberCols(&ncols);
	header->get_NumberRows(&nrows);
	if( ncols <= 0 || nrows <= 0 )
	{	
		*retval = FALSE;
		lastErrorCode = tkZERO_ROWS_OR_COLS;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}
	header->Release();

	double oldValue = 0;
	dVal(OldValue,oldValue);
	
	VARIANT vval;
	VariantInit(&vval); //added by Rob Cairns 4-Jan-06
	double val;

	long percent = 0, newpercent = 0;
	double total = nrows * ncols;

	for( int j = 0; j < nrows; j++ )
	{	
		for( int i = 0; i < ncols; i++ )
		{	
			Grid->get_Value(i,j,&vval);
			dVal(vval,val);
			if( val == oldValue )
				Grid->put_Value(i,j,NewValue);

			newpercent = (long)((( j*ncols + i )/total)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				if( cBack != NULL )
					cBack->Progress(OLE2BSTR(key), percent, A2BSTR("GridReplace"));
				else if( globalCallback != NULL )
					globalCallback->Progress(OLE2BSTR(key), percent, A2BSTR("GridReplace"));
			}
		}
	}
	
	VariantClear(&vval); //added by Rob Cairns 4-Jan-06
	return S_OK;
}

STDMETHODIMP CUtils::GridInterpolateNoData(IGrid *Grid, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Grid == NULL )
	{
		*retval = NULL;
		lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( cBack != NULL )
		{
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		}
		else if( globalCallback != NULL )
		{
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		}//else if
		*retval = VARIANT_FALSE;
		return S_OK;
	}//if

	long ncols = 0, nrows = 0;
	double nodatavalue;
	IGridHeader * header = NULL;
	VARIANT nodataval_variant;
	VariantInit(&nodataval_variant); //added by Rob Cairns 4-Jan-06
	Grid->get_Header(&header);
	header->get_NumberCols(&ncols);
	header->get_NumberRows(&nrows);
	header->get_NodataValue(&nodataval_variant);
	dVal(nodataval_variant, nodatavalue);
	VariantClear(&nodataval_variant); //added by Rob Cairns 4-Jan-06

	if(ncols <= 0 || nrows <= 0)
	{
		*retval = FALSE;
		lastErrorCode = tkZERO_ROWS_OR_COLS;
		if(cBack != NULL)
		{
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		}
		else if(globalCallback != NULL)
		{
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		}//else
		return S_OK;
	}//if

	GridInterpolate gi(Grid,nodatavalue,nrows,ncols);
	
	if( cBack != NULL )
	{
		cBack->Progress(OLE2BSTR(key), 0, A2BSTR("GridInterpolateNoData"));
	}
	else if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key), 0, A2BSTR("GridInterpolateNoData"));
	}//else if

	gi.Interpolate(0,0);
	
	if( cBack != NULL )
	{
		cBack->Progress(OLE2BSTR(key), 25, A2BSTR("GridInterpolateNoData"));
	}
	else if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key), 25, A2BSTR("GridInterpolateNoData"));
	}//else if
	
	gi.Interpolate(0,ncols-1);
	
	if( cBack != NULL )
	{
		cBack->Progress(OLE2BSTR(key), 50, A2BSTR("GridInterpolateNoData"));
	}
	else if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key), 50, A2BSTR("GridInterpolateNoData"));
	}//else if
	
	gi.Interpolate(nrows-1,0);
	
	if( cBack != NULL )
	{
		cBack->Progress(OLE2BSTR(key), 75, A2BSTR("GridInterpolateNoData"));
	}
	else if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key), 75, A2BSTR("GridInterpolateNoData"));
	}//else if
	
	gi.Interpolate(nrows-1,ncols-1);
	
	if( cBack != NULL )
	{
		cBack->Progress(OLE2BSTR(key), 100, A2BSTR("GridInterpolateNoData"));
	}
	else if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key), 100, A2BSTR("GridInterpolateNoData"));
	}//else if

	*retval = VARIANT_TRUE;

	return S_OK;
}

// ************************************************************
//		RemoveColinearPoints
// ************************************************************
STDMETHODIMP CUtils::RemoveColinearPoints(IShapefile * Shapes, double LinearTolerance, ICallback *cBack, VARIANT_BOOL *retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Shapes == NULL )
	{
		*retval = NULL;
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}

	ShpfileType shptype;
	Shapes->get_ShapefileType(&shptype);

	if( shptype != SHP_POLYLINE && shptype != SHP_POLYGON )
	{
		*retval = NULL;
		if( shptype == SHP_POLYLINEZ || shptype == SHP_POLYGONZ ||
			shptype == SHP_POLYLINEM || shptype == SHP_POLYGONM )
			lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		else
			lastErrorCode = tkINCOMPATIBLE_SHAPEFILE_TYPE;

		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	if (!((CShapefile*)Shapes)->ValidateInput(Shapes, "RemoveColinearPoints", "Shapes", VARIANT_FALSE, "Utils"))
		return S_FALSE;

	VARIANT_BOOL vbretval;
	Shapes->StartEditingShapes(FALSE,cBack,&vbretval);
	long numShapes;
	Shapes->get_NumShapes(&numShapes);
	
	long percent = 0, cnt = 0;
	double total = 2*numShapes;
		
	XRedBlack rb;

	if( shptype == SHP_POLYLINE )
	{
	}
	else if( shptype == SHP_POLYGON )
	{	
		for( int currentShape = 0; currentShape < numShapes; currentShape++ )
		{	
			IShape * shape = NULL;
			((CShapefile*)Shapes)->GetValidatedShape(currentShape,&shape);
			if (!shape) continue;

			long numPoints = 0, forward_index = 0, backward_index = 0;
			shape->get_NumPoints(&numPoints);
				
			double x, y;
			VARIANT_BOOL vbretval;

			for( int point_index = 0; point_index < numPoints; point_index++ )
			{	
				shape->get_XY(point_index, &x, &y, &vbretval);

				POINT p;
				p.x = (LONG)x;
				p.y = (LONG)y;
				rb.Insert( p );

				forward_index = point_index + 1;
				backward_index = point_index - 1;
				if( forward_index >= numPoints )
					forward_index = 0;
				if( backward_index < 0 )
					backward_index = numPoints - 1;				

				VARIANT_BOOL vbretval;
				double onex, oney, twox, twoy, threex, threey;
				shape->get_XY(backward_index, &onex, &oney, &vbretval);
				shape->get_XY(forward_index, &twox, &twoy, &vbretval);
				shape->get_XY(point_index, &threex, &threey, &vbretval);

				POINT one;
				one.x = (LONG)onex;
				one.y = (LONG)oney;

				POINT two;
				two.x = (LONG)twox;
				two.y = (LONG)twoy;

				POINT three;
				three.x = (LONG)threex;
				three.y = (LONG)threey;

				YRedBlackNode * prbn = rb.GetNode( three );
				if( prbn != NULL )
				{
					if( prbn->canSetColinear )
					{
						if( isColinear( one, two, three, LinearTolerance ) )
							prbn->isColinear = true;									
						else
						{	prbn->isColinear = false;
							prbn->canSetColinear = false;
						}
					}
				}
			}			
				
			cnt++;
			int newpercent = (int)((cnt/total)*100);
			if( newpercent > percent )
			{	percent = newpercent;
				if( cBack != NULL )
					cBack->Progress(OLE2BSTR(key), percent, A2BSTR("RemoveColinearPoints"));
				else if( globalCallback != NULL )
					globalCallback->Progress(OLE2BSTR(key), percent, A2BSTR("RemoveColinearPoints"));
			}			
				
			std::deque< POINT > PointsToKeep;
			for( currentShape = 0; currentShape < numShapes; currentShape++ )
			{	
				double x, y;
				VARIANT_BOOL vbretval;

				for( int point_index = 0; point_index < numPoints; point_index++ )
				{	
					shape->get_XY(point_index, &x, &y, &vbretval);
					POINT p;
					p.x = (LONG)x; p.y = (LONG)y;
					rb.Insert( p );

					YRedBlackNode * prbn = rb.GetNode( p );
					if( prbn != NULL )
					{	
						if( prbn->isColinear == true && prbn->useCount < 2 )
						{	//Don't Keep the Point
						}
						else
							PointsToKeep.push_back( prbn->Element );
					}
				}
				
				for( int ns = 0; ns < numPoints; ns++ )
					shape->DeletePoint(0,&vbretval);
				
				PointsToKeep.push_back( PointsToKeep[0] );
				for( int i = 0; i < (int)PointsToKeep.size(); i++ )
				{	
					IPoint * pnt = NULL;
					m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
					
					pnt->put_X( PointsToKeep[i].x );
					pnt->put_Y( PointsToKeep[i].y );
					long pntpos = i;
					shape->InsertPoint( pnt, &pntpos, &vbretval );
				}			
				
				PointsToKeep.clear();

				cnt++;
				int newpercent = (int)((cnt/total)*100);
				if( newpercent > percent )
				{	
					percent = newpercent;
					if( cBack != NULL )
						cBack->Progress(OLE2BSTR(key), percent, A2BSTR("RemoveColinearPoints"));
					else if( globalCallback != NULL )
						globalCallback->Progress(OLE2BSTR(key), percent, A2BSTR("RemoveColinearPoints"));
				}				
			}
			
			shape->Release();
			shape = NULL;
		}		
	}	

	// ---------------------------------------------------
	//	 Validating output
	// ---------------------------------------------------
	Utility::DisplayProgressCompleted(globalCallback, key);
	((CShapefile*)Shapes)->ClearValidationList();
	((CShapefile*)Shapes)->ValidateOutput(&Shapes, "RemoveColinearPoints", "Utils");
	if (Shapes)
		Shapes->StopEditingShapes(TRUE,FALSE,cBack,&vbretval);
	return S_OK;
}

bool CUtils::isColinear( POINT one, POINT two, POINT test, double tolerance )
{
	double dx = two.x - one.x;
	double dy = two.y - one.y;
	if( dx != 0 )
	{	//Test to see if the slopes are equal
		double m1 = dy/dx;
		double dx2 = two.x - test.x;
		double dy2 = two.y - test.y;
		if( dx2 != 0 )
		{	
			double m2 = dy2/dx2;
			if( ( m1 > 0 && m2 > 0 ) || ( m1 < 0 && m2 < 0 ) )
			{
				if( fabs( m1 - m2 ) < tolerance )
					return true;
				else
					return false;
			}
			else if( m1 == 0 && m2 == 0 )
				return true;
			else
				return false;
		}
		else
			return false;
	}
	//Vertical Line
	//Check the X's
	else
	{	if( one.x == test.x )
			return true;
		else
			return false;
	}
}

STDMETHODIMP CUtils::get_Length(IShape *Shape, double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Shape == NULL )
	{	
		*pVal = 0.0;
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	Shape->get_Length(pVal);
	return S_OK;
}

STDMETHODIMP CUtils::get_Perimeter(IShape *Shape, double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if( Shape == NULL )
	{	
		*pVal = 0.0;
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	
	Shape->get_Perimeter(pVal);
	return S_OK;
}

STDMETHODIMP CUtils::get_Area(IShape *Shape, double *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if( Shape == NULL )	
	{	
		*pVal = 0.0;
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	
	Shape->get_Area(pVal);
	return S_OK;
}




//ah 2/24/06
//v2: Checks to see if a polygon is oriented clockwise about the normal.
//In multi-part polygons, clockwise parts are filled while counter-clockwise parts are holes.
//Returns true if clockwise, false if counter-clockwise.
bool CUtils::is_clockwise(Poly *polygon)
{
	int numPoints = polygon->polyX.size();
	double area = 0;
	for(int i = 0; i <= numPoints-2; i++)
	{
		double oneX = polygon->polyX[i];
		double oneY = polygon->polyY[i];
		double twoX = polygon->polyX[i+1];		
		double twoY = polygon->polyY[i+1];
			
		double trap_area = ((oneX * twoY) - (twoX * oneY));
		area += trap_area;
	}
	if(area > 0)
	{
		//if area is positive, polygon vertices are oriented
		//counter-clockwise about the normal.
		return false;
	}
	else
	{
		//polygon vertices are oriented clockwise about the normal.
		return true;
	}
}
//ah 2/24/06
//v3: Checks to see if a polygon is oriented clockwise about the normal.
//In multi-part polygons, clockwise parts are filled while counter-clockwise parts are holes.
//Returns true if clockwise, false if counter-clockwise.
bool CUtils::is_clockwise(IShape *Shape)
{
	long numPoints = 0;
	Shape->get_NumPoints(&numPoints);
	double area = 0;

	//calculate cross product for all vertices except last one (repeat of first)
	double oneX, oneY, twoX, twoY;
	VARIANT_BOOL vbretval;
	for(int i = 0; i <= numPoints-2; i++)
	{
		Shape->get_XY(i, &oneX, &oneY, &vbretval);
		Shape->get_XY(i+1, &twoX, &twoY, &vbretval);
		double trap_area = ((oneX * twoY) - (twoX * oneY));
		area += trap_area;
	}
	if(area > 0)
	{
		//if area is positive, polygon vertices are oriented
		//counter-clockwise about the normal.
		return false;
	}
	else
	{
		//polygon vertices are oriented clockwise about the normal.
		return true;
	}
}

bool CUtils::is_clockwise( double x0, double y0, double x1, double y1, double x2, double y2)
{
	//See http://astronomy.swin.edu.au/~pbourke/geometry/clockwise/, by Paul Bourke
	//for better explanation of using cross product to find clockwiseness.
	double crossProduct = (x1 - x0)*(y2 - y1) - (y1 - y0)*(x2 - x1);

	//a negative cross product means that we have a clockwise polygon
	if( crossProduct < 0 )
		return true;
	else
		return false;
}

STDMETHODIMP CUtils::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = lastErrorCode;
	lastErrorCode = tkNO_ERROR;

	return S_OK;
}

STDMETHODIMP CUtils::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	*pVal = A2BSTR(ErrorMsg(ErrorCode));

	return S_OK;
}

STDMETHODIMP CUtils::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = globalCallback;
	if( globalCallback != NULL )
	{	
		globalCallback->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CUtils::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&globalCallback);
	return S_OK;
}

STDMETHODIMP CUtils::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	*pVal = OLE2BSTR(key);

	return S_OK;
}

STDMETHODIMP CUtils::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	::SysFreeString(key);
	key = OLE2BSTR(newVal);

	return S_OK;
}

STDMETHODIMP CUtils::GridMerge(VARIANT Grids, BSTR MergeFilename, VARIANT_BOOL InRam, GridFileType GrdFileType, ICallback *cBack, IGrid **retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	BSTR final_projection = A2BSTR("");

	//Check the Array Type
	if( Grids.vt != (VT_ARRAY|VT_DISPATCH) )
	{	
		*retval = NULL;
		lastErrorCode = tkINVALID_VARIANT_TYPE;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	//Check the Array Size
	SAFEARRAY * arr = Grids.parray;
	SAFEARRAYBOUND arraybound = arr->rgsabound[0];
	if( arraybound.cElements <= 0 )
	{	
		*retval = NULL;
		return S_OK;
	}

	std::deque<IGrid*> allGrids;
	int gridSize = 0; //size of allGrids -- ah 6/6/03
	
	int elements = (int)arraybound.cElements; //ah 6/6/03
	
	for( int index = 0; index < elements; index++ )
	{	
		IUnknown * unknown = NULL;
		long ind = index;
		SafeArrayGetElement( arr, &ind, &unknown );
		
		if( unknown == NULL )
			continue;

		//Determine if the IUnknown supports the IGrid Interface
		IGrid * grid = NULL;		
		if( unknown->QueryInterface( IID_IGrid, (void**)&grid) != S_OK )
		{	
			*retval = NULL;			
			lastErrorCode = tkINTERFACE_NOT_SUPPORTED;
			if( cBack != NULL )
				cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
			else if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
			
			gridSize = (int)allGrids.size(); //ah 6/6/05
			for( int c = 0; c < gridSize; c++ )
			{	
				allGrids[c]->Release();
				allGrids[c] = NULL;
			}
			return S_OK;
		}
		else
			allGrids.push_back( grid );		
	}

	if( allGrids.size() <= 0 )
	{	
		*retval = NULL;
		return S_OK;
	}

	gridSize = (int)allGrids.size();
	ProjectionTools * pt = new ProjectionTools();
	for( int i = 0; i < gridSize; i++ )
	{	
		char * currentComparingProj = NULL;
		if( i == 0 )
		{	
			IGridHeader * header = NULL;
			allGrids[i]->get_Header(&header);
			header->get_Projection(&final_projection);
			currentComparingProj = OLE2A(final_projection);
			header->Release();
		}
		else
		{
			BSTR bstrProj;
			IGridHeader * header = NULL;
			allGrids[i]->get_Header(&header);
			header->get_Projection(&bstrProj);
			char * nextProj = OLE2A(bstrProj);
			if (!pt->IsSameProjection(currentComparingProj, nextProj))
			{
				if( cBack != NULL )
					cBack->Error(A2BSTR("GridMerge"), A2BSTR("Warning: Projection mismatch on merged grids"));
				else if( globalCallback != NULL )
					globalCallback->Error(A2BSTR("GridMerge"), A2BSTR("Warning: Projection mismatch on merged grids"));
			}
			header->Release();
		}
	}

	delete pt;

	double final_xllcenter;
	double final_yllcenter;
	double final_xurcenter;
	double final_yurcenter;
	double final_dx = 1;
	double final_dy = 1;
	GridDataType final_dType = UnknownDataType;
	long final_ncols, final_nrows;

	double ind_xllcenter;
	double ind_yllcenter;
	double ind_xurcenter;
	double ind_yurcenter;
	double ind_dx = 1;
	double ind_dy = 1;
	GridDataType ind_dType = UnknownDataType;
	long ind_ncols, ind_nrows;
	VARIANT vndv;
	VariantInit(&vndv); //added by Rob Cairns 4-Jan-06
	double ndv;

	long percent = 0, cnt = 0;
	double total = 0.0;
	
	//Get the bounds and DataType for the final grid
	gridSize = (int)allGrids.size();
	for( int i = 0; i < gridSize; i++ )
	{	if( i == 0 )
		{	
			IGridHeader * header = NULL;
			allGrids[i]->get_Header(&header);
			header->get_dX(&final_dx);
			header->get_dY(&final_dy);
			header->get_XllCenter(&final_xllcenter);
			header->get_YllCenter(&final_yllcenter);
			header->get_NodataValue(&vndv);
			dVal(vndv,ndv);
			
			header->get_NumberCols(&final_ncols);
			header->get_NumberRows(&final_nrows);

			final_xurcenter = final_xllcenter + (final_ncols-1)*final_dx;
			final_yurcenter = final_yllcenter + (final_nrows-1)*final_dy;
			total = final_ncols*final_nrows;
			header->Release();
			allGrids[i]->get_DataType(&final_dType);
		}
		else
		{	
			IGridHeader * header = NULL;
			allGrids[i]->get_Header(&header);
			header->get_dX(&ind_dx);
			header->get_dY(&ind_dy);
			header->get_XllCenter(&ind_xllcenter);
			header->get_YllCenter(&ind_yllcenter);
			
			header->get_NumberCols(&ind_ncols);
			header->get_NumberRows(&ind_nrows);

			ind_xurcenter = ind_xllcenter + (ind_ncols-1)*ind_dx;
			ind_yurcenter = ind_yllcenter + (ind_nrows-1)*ind_dy;
			header->Release();
			allGrids[i]->get_DataType(&ind_dType);

			if( ind_dType > final_dType && ind_dType != UnknownDataType )
				final_dType = ind_dType;

			if( ind_dx != final_dx )
			{	
				lastErrorCode = tkINCOMPATIBLE_DX;
				*retval = NULL;
				if( cBack != NULL )
					cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
				else if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
				
				gridSize = (int)allGrids.size();
				for( int c = 0; c < gridSize; c++ )
				{	allGrids[c]->Release();
					allGrids[c] = NULL;
				}
				VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
				return S_OK;				
			}
			else if( ind_dy != final_dy )
			{	lastErrorCode = tkINCOMPATIBLE_DY;
				*retval = NULL;
				if( cBack != NULL )
					cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
				else if( globalCallback != NULL )
					globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
				
				gridSize = (int)allGrids.size();
				for( int c = 0; c < gridSize; c++ )
				{	allGrids[c]->Release();
					allGrids[c] = NULL;
				}
				VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
				return S_OK;	
			}
	
			if( ind_xllcenter < final_xllcenter )
				final_xllcenter = ind_xllcenter;
			if( ind_yllcenter < final_yllcenter )
				final_yllcenter = ind_yllcenter;

			if( ind_xurcenter > final_xurcenter )
				final_xurcenter = ind_xurcenter;
			if( ind_yurcenter > final_yurcenter )
				final_yurcenter = ind_yurcenter;
			total += ind_ncols*ind_nrows;
		}
	}

	if( final_dType == UnknownDataType || final_dType == InvalidDataType )
	{	
		lastErrorCode = tkINVALID_DATA_TYPE;
		*retval = NULL;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));

		gridSize = (int)allGrids.size();
		
		for( int c = 0; c < gridSize; c++ )
		{	allGrids[c]->Release();
			allGrids[c] = NULL;
		}
		VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
		return S_OK;
	}

	if( total <= 0 )
	{	
		*retval = NULL;
		gridSize = (int)allGrids.size();
		
		for( int c = 0; c < gridSize; c++ )
		{
			allGrids[c]->Release();
			allGrids[c] = NULL;
		}
		VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
		return S_OK;
	}

	IGridHeader * final_header = NULL;
	CoCreateInstance( CLSID_GridHeader, NULL, CLSCTX_INPROC_SERVER, IID_IGridHeader, (void**)&final_header );
	final_header->put_NumberCols( (long)((final_xurcenter - final_xllcenter)/final_dx)+1 );
	final_header->put_NumberRows( (long)((final_yurcenter - final_yllcenter)/final_dy)+1 );
	final_header->put_dX( final_dx );
	final_header->put_dY( final_dy );
	final_header->put_NodataValue( vndv );
	final_header->put_XllCenter( final_xllcenter );
	final_header->put_YllCenter( final_yllcenter );
	final_header->put_Projection(final_projection);
		
	VARIANT_BOOL vbretval = FALSE;
	CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)retval);
	(*retval)->CreateNew( MergeFilename, final_header, final_dType, vndv, InRam, GrdFileType, cBack, &vbretval ); 
	final_header->Release();

	if( vbretval == FALSE )
	{
		*retval = NULL;
		gridSize = (int)allGrids.size();
		for( int c = 0; c < gridSize; c++ )
		{	allGrids[c]->Release();
			allGrids[c] = NULL;
		}
		VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
		return S_OK;
	}

	//Merge the grids
	long xoffset = 0, yoffset = 0;
	double val, nodata_value;
	VARIANT vval;
	VariantInit(&vval); //added by Rob Cairns 4-Jan-06

	gridSize = (int)allGrids.size();
	for( int n = gridSize - 1; n >= 0; n-- )
	{	
		IGridHeader * header = NULL;
		allGrids[n]->get_Header(&header);
		header->get_XllCenter(&ind_xllcenter);
		header->get_YllCenter(&ind_yllcenter);		
		header->get_NumberCols(&ind_ncols);
		header->get_NumberRows(&ind_nrows);		
		header->get_NodataValue(&vndv);
		dVal(vndv,nodata_value);
		header->Release();

		(*retval)->ProjToCell(ind_xllcenter,ind_yllcenter+(ind_nrows-1)*final_dy,&xoffset,&yoffset);
		
		for( int j = 0; j < ind_nrows; j++ )
		{	
			for( int i = 0; i < ind_ncols; i++ )
			{	
				allGrids[n]->get_Value( i, j, &vval );
				dVal( vval, val );
			
				if( val != nodata_value && !(val < -2147483640 && nodata_value < -2147483640))
					(*retval)->put_Value( i + xoffset, j + yoffset, vval );				

				int newpercent = (int)((++cnt/total)*100);
				if( newpercent > percent )
				{	percent = newpercent;
					if( cBack != NULL )
						cBack->Progress(OLE2BSTR(key),percent,A2BSTR("GridMerge"));
					else if( globalCallback != NULL )
						globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("GridMerge"));
				}
			}
		}
	}	

	gridSize = (int)allGrids.size();
	for( int c = 0; c < gridSize; c++ )
	{	allGrids[c]->Release();
		allGrids[c] = NULL;
	}

	VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
	VariantClear(&vval); //added by Rob Cairns 4-Jan-06
	
	return S_OK;
}

STDMETHODIMP CUtils::ShapeMerge(IShapefile *Shapes, long IndexOne, long IndexTwo, ICallback *cBack, IShape **retval)
{
	//This function is being replaced by the GPC clip function either in the UTILS object or in 
	//a separate dll... 
	//dpa 6/3/05
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	AfxMessageBox("Not Implemented");

	return S_OK;
}

STDMETHODIMP CUtils::GridToGrid(IGrid *Grid, GridDataType OutDataType, ICallback *cBack, IGrid **retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if( Grid == NULL )
	{	
		*retval = NULL;
		lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	long ncols = 0, nrows = 0;

	IGridHeader * header = NULL;
	Grid->get_Header(&header);
	header->get_NumberCols(&ncols);
	header->get_NumberRows(&nrows);

	if( ncols <= 0 || nrows <= 0 )
	{	
		*retval = NULL;
		lastErrorCode = tkZERO_ROWS_OR_COLS;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	VARIANT vndv;
	VariantInit(&vndv); //added by Rob Cairns 4-Jan-06
	header->get_NodataValue(&vndv);

	CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)retval);
	VARIANT_BOOL vbretval;
	(*retval)->CreateNew(A2BSTR(""),header,OutDataType,vndv,TRUE,UseExtension,cBack,&vbretval);
	header->Release();

	VARIANT val;
	VariantInit(&val); //added by Rob Cairns 4-Jan-06
	long percent = 0, newpercent = 0;
	
	for( int j = 0; j < nrows; j++ )
	{	for( int i = 0; i < ncols; i++ )
		{	
			Grid->get_Value(i,j,&val);
			(*retval)->put_Value(i,j,val);	
		}
		newpercent = (long)((j / nrows)*100);
		if( newpercent > percent )
		{	
			percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key), percent, A2BSTR("Grid2Grid"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key), percent, A2BSTR("Grid2Grid"));
		}
	}	

	VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
	VariantClear(&val); //added by Rob Cairns 4-Jan-06

	return S_OK;
}

STDMETHODIMP CUtils::ShapeToShapeZ(IShapefile * Shapefile, IGrid *Grid, ICallback *cBack, IShapefile **retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( Shapefile == NULL || Grid == NULL )
	{	
		*retval = NULL;
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	
	long ncols = 0, nrows = 0;
	IGridHeader * header = NULL;
	Grid->get_Header(&header);
	header->get_NumberCols(&ncols);
	header->get_NumberRows(&nrows);
	header->Release();

	double left=0, bottom=0, right=0, top=0;
	Grid->CellToProj(0,0,&left,&bottom);
	Grid->CellToProj(ncols-1,nrows-1,&right,&top);

	IExtents * box = NULL;
	Shapefile->get_Extents(&box);
	double s_left=0,s_bottom=0,s_right=0,s_top=0,nval;
	box->GetBounds(&s_left,&s_bottom,&nval,&s_right,&s_top,&nval);
	box->Release();

	//Check the bounds
	if( s_left < left || s_right > right || s_bottom < bottom || s_top < top )
	{	
		*retval = NULL;
		this->ErrorMessage(tkSHAPEFILE_LARGER_THAN_GRID);
		return S_OK;
	}

	VARIANT_BOOL vbretval;
	ShpfileType shapetype = SHP_NULLSHAPE;
	Shapefile->get_ShapefileType(&shapetype);
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNew(A2BSTR(""),shapetype,&vbretval);
	
	long numFields = 0;
	Shapefile->get_NumFields(&numFields);
	for( int i = 0; i < numFields; i++ )
	{	
		IField * field = NULL;
		Shapefile->get_Field(i,&field);
		long fpos = i;
		(*retval)->EditInsertField(field,&fpos,cBack,&vbretval);
		field->Release();
	}

	long numShapes = 0;
	Shapefile->get_NumShapes(&numShapes);
	for( int j = 0; j < numShapes; j++ )
	{	
		IShape * shape = NULL;
		Shapefile->get_Shape(j,&shape);
		
		long numPoints = 0;
		shape->get_NumPoints(&numPoints);
		int p = 0;
		for( p = 0; p < numPoints; p++ )
		{
			double x = 0, y = 0;
			double z = 0;

			IPoint * point = NULL;
			shape->get_Point(p,&point);
			point->get_X(&x);
			point->get_Y(&y);
			point->put_Z(z);
			point->Release();

			long col = 0, row = 0;
			Grid->ProjToCell(x,y,&col,&row);
			VARIANT val;
			VariantInit(&val); //added by Rob Cairns 4-Jan-06
			Grid->get_Value(col,row,&val);
			dVal(val,z);
			VariantClear(&val); //added by Rob Cairns 4-Jan-06
		}
		long spos = p;
		(*retval)->EditInsertShape(shape,&spos,&vbretval);
		shape->Release();

		VARIANT cval;
		VariantInit(&cval); //added by Rob Cairns 4-Jan-06
		for( int k = 0; k < numFields; k++ )
		{	
			Shapefile->get_CellValue(k,j,&cval);
			(*retval)->EditCellValue(k,j,cval,&vbretval);
		}
		VariantClear(&cval); //added by Rob Cairns 4-Jan-06
	}

	return S_OK;
}

STDMETHODIMP CUtils::TinToShapefile(ITin *Tin, ShpfileType Type, ICallback *cBack, IShapefile **retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;

	if( Tin == NULL )
	{	
		*retval = NULL;
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	long numTriangles = 0;
	Tin->get_NumTriangles(&numTriangles);
	long numVertices = 0;
	Tin->get_NumVertices(&numVertices);
	long percent = 0, newpercent = 0;
	double total = numTriangles;

	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);

	if( Type == SHP_POLYGON || Type == SHP_POLYGONZ )
	{	
		//Create shapefile
		VARIANT_BOOL vbretval;
		(*retval)->CreateNew(A2BSTR(""),Type,&vbretval);
		
		long pos = 0;
		IField * field = NULL;
		CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&field);
		field->put_Name(A2BSTR("Tin"));
		field->put_Type(INTEGER_FIELD);
		field->put_Width(5);
		(*retval)->EditInsertField(field,&pos,cBack,&vbretval);
		field->Release();
		field = NULL;		
		
		long vtx1, vtx2, vtx3;
		double x1, y1, z1;
		double x2, y2, z2;
		double x3, y3, z3;

		for( long i = 0; i < numTriangles; i++ )
		{			
			Tin->Triangle(i,&vtx1,&vtx2,&vtx3);
			Tin->Vertex(vtx1,&x1,&y1,&z1);
			Tin->Vertex(vtx2,&x3,&y3,&z3);
			Tin->Vertex(vtx3,&x2,&y2,&z2);
				
			IShape * shape = NULL;
			CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
			shape->Create(Type,&vbretval);

			pos = 0;
			shape->InsertPart(0,&pos,&vbretval);

			IPoint * point = NULL;
			CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&point);
			point->put_X(x1);
			point->put_Y(y1);
			point->put_Z(z1);
			pos = 0;
			shape->InsertPoint(point,&pos,&vbretval);
			point->Release();
			point = NULL;

			CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&point);
			point->put_X(x2);
			point->put_Y(y2);
			point->put_Z(z2);
			pos = 1;
			shape->InsertPoint(point,&pos,&vbretval);
			point->Release();
			point = NULL;
						
			CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&point);
			point->put_X(x3);
			point->put_Y(y3);
			point->put_Z(z3);
			pos = 2;
			shape->InsertPoint(point,&pos,&vbretval);
			point->Release();
			point = NULL;

			CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&point);
			point->put_X(x1);
			point->put_Y(y1);
			point->put_Z(z1);
			pos = 3;
			shape->InsertPoint(point,&pos,&vbretval);
			point->Release();
			point = NULL;
			
			pos = i;
			(*retval)->EditInsertShape(shape,&pos,&vbretval);
			shape->Release();
			shape = NULL;

			VARIANT val;
			VariantInit(&val); //added by Rob Cairns 4-Jan-06
			val.vt = VT_I4;
			val.lVal = i;
			(*retval)->EditCellValue(0,pos,val,&vbretval);
			
			newpercent = (long)((i/total)*100);
			if( newpercent > percent )
			{	percent = newpercent;
				if( cBack != NULL )
					cBack->Progress(OLE2BSTR(key), percent, A2BSTR("TinToShapefile"));
				else if( globalCallback != NULL )
					globalCallback->Progress(OLE2BSTR(key), percent, A2BSTR("TinToShapefile"));
			}

			VariantClear(&val); //added by Rob Cairns 4-Jan-06

		}		
	}
	else if( Type == SHP_POINT || Type == SHP_POINTZ )
	{	
		//Create shapefile
		VARIANT_BOOL vbretval;
		(*retval)->CreateNew(A2BSTR(""),Type,&vbretval);
		
		long pos = 0;
		IField * field = NULL;
		CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&field);
		field->put_Name(A2BSTR("Vertex"));
		field->put_Type(INTEGER_FIELD);
		field->put_Width(5);
		(*retval)->EditInsertField(field,&pos,cBack,&vbretval);
		field->Release();
		field = NULL;		
		
		double x1, y1, z1;
		
		for( long i = 0; i < numVertices; i++ )
		{	
			Tin->Vertex(i,&x1,&y1,&z1);
				
			IShape * shape = NULL;
			CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
			shape->Create(Type,&vbretval);

			IPoint * point = NULL;
			CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&point);
			point->put_X(x1);
			point->put_Y(y1);
			point->put_Z(z1);
			pos = 0;
			shape->InsertPoint(point,&pos,&vbretval);
			point->Release();
			point = NULL;

			(*retval)->EditInsertShape(shape,&i,&vbretval);
			shape->Release();
			shape = NULL;

			VARIANT val;
			VariantInit(&val); //added by Rob Cairns 4-Jan-06
			val.vt = VT_I4;
			val.lVal = i;
			(*retval)->EditCellValue(0,i,val,&vbretval);
			
			newpercent = (long)((i/total)*100);
			if( newpercent > percent )
			{	
				percent = newpercent;
				if( cBack != NULL )
					cBack->Progress(OLE2BSTR(key), percent, A2BSTR("TinToShapefile"));
				else if( globalCallback != NULL )
					globalCallback->Progress(OLE2BSTR(key), percent, A2BSTR("TinToShapefile"));
			}
	
			VariantClear(&val); //added by Rob Cairns 4-Jan-06

		}
	}

	if (*retval)
	{
		((CShapefile*)(*retval))->ValidateOutput(retval, "GridToShapefile", "Utils");
	}

	return S_OK;
}


///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			POLYGONAL ALGORITHM
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

# ifndef GRID_TO_SHAPES
# define GRID_TO_SHAPES

# define BORDER             -2225
# define DOUBLE_BORDER      -2226
# define DECISION           -2227
# define TRACE_BORDER       -2228
# define CURRENT_POLYGON	-2229

	enum UTILS_DIRECTION
	{ NONE,
	  RIGHT,
	  UPRIGHT,
	  UP,
	  UPLEFT,
	  LEFT,
	  DOWNLEFT,
	  DOWN,
	  DOWNRIGHT
	};

# endif

// ***************************************************
//    GridToShapefile()
// ***************************************************
STDMETHODIMP CUtils::GridToShapefile(IGrid *Grid, IGrid *ConnectionGrid, ICallback *cBack, IShapefile **retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	if( Grid == NULL )
	{	*retval = NULL;
		lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		return S_OK;
	}

	//Create a grid that is twice the size of the original
	IGridHeader * header = NULL;
	Grid->get_Header(&header);
	long cols = 0, rows = 0;
	header->get_NumberCols(&cols);
	header->get_NumberRows(&rows);
	
	GridDataType dType = UnknownDataType;
	Grid->get_DataType(&dType);

	double dx, dy, xllCenter, yllCenter;

	VARIANT vndv;
	VariantInit(&vndv); //added by Rob Cairns 4-Jan-06
	header->get_NodataValue(&vndv);
	header->get_dX(&dx);
	header->get_dY(&dy);
	header->get_XllCenter(&xllCenter);
	header->get_YllCenter(&yllCenter);
	header->Release();
	header = NULL;

	if( cols <= 0 || rows <= 0 )
	{	*retval = NULL;
		lastErrorCode = tkZERO_ROWS_OR_COLS;
		if( cBack != NULL )
			cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));	
		else if( globalCallback != NULL )
			globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
		VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
		return S_OK;
	}
	
	long exp_cols = 2*cols + 1;
	long exp_rows = 2*rows + 1;

	long percent = 0, newpercent = 0;	
	double polygon_id = 0;
	double nodata_value = -1;
	dVal(vndv,nodata_value);

	double expand_nodata_value = nodata_value;
	double total = rows;
	
	VARIANT_BOOL vbretval = FALSE;
	
	CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)&expand_grid);
	IGridHeader * expand_header = NULL;
	CoCreateInstance(CLSID_GridHeader,NULL,CLSCTX_INPROC_SERVER,IID_IGridHeader,(void**)&expand_header);
	expand_header->put_dX( dx*.5 );
	expand_header->put_dY( dy*.5 );
	expand_header->put_XllCenter( xllCenter - dx*.5 );
	expand_header->put_YllCenter( yllCenter - dy*.5 );
	expand_header->put_NodataValue( vndv );
	expand_header->put_NumberCols( exp_cols );
	expand_header->put_NumberRows( exp_rows );
	
	char * tmpgname = new char[512];
	char * tmpfname = new char[512];
	char * tmppath = new char[260 + 512 + 1];
	_getcwd(tmppath,260);
	tmpnam(tmpfname);
	sprintf(tmpgname, "%s.bgd", tmpfname);

	expand_grid->CreateNew(A2BSTR(tmpgname),expand_header,dType,vndv,VARIANT_TRUE,UseExtension,cBack,&vbretval);

	expand_header->Release();
	expand_header = NULL;

	connection_grid = ConnectionGrid;
	
	double expand_value = 0;
	for( int y = rows - 1; y>= 0; y-- )
	{	for( int x = 0; x < cols; x++ )
		{	
			polygon_id = getValue( Grid, x, y );

			//Expand as the center cell
			int expand_x = 2*x + 1;
			int expand_y = 2*y + 1;

			//Expanded Cell arrangement
			//4 3 2
			//5 0 1
			//6 7 8

			//Mark the cell as a border if it already has a value
			//Cell 0
			expand_value = getValue( expand_grid, expand_x, expand_y );
			//Set the polygon_id
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x,     expand_y,     polygon_id );
			
			//Cell 1
			expand_value = getValue( expand_grid, expand_x + 1, expand_y );
			//set the polygon id
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x + 1, expand_y,     polygon_id );
			//polygon_id already exists and is the same... reset it
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x + 1, expand_y,     polygon_id );										
			//set the cell as a border
			else if( expand_value >= 0 )
				setValue(expand_grid,  expand_x + 1, expand_y,     BORDER );
			
			//Cell 2
			expand_value = getValue( expand_grid, expand_x + 1, expand_y + 1 );
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x + 1, expand_y + 1, polygon_id );
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x + 1, expand_y + 1, polygon_id );									
			else if( expand_value >= 0 )
				setValue( expand_grid, expand_x + 1, expand_y + 1, BORDER );
			else if( expand_value == BORDER )
			{	expand_value -= 1;
				setValue( expand_grid, expand_x + 1, expand_y + 1,		expand_value );
			}
			else if( expand_value == DOUBLE_BORDER )
			{	if( is_decision( expand_grid, expand_x + 1, expand_y + 1 ) )
					setValue( expand_grid, expand_x + 1, expand_y + 1, DECISION );	
				else
					setValue( expand_grid, expand_x + 1, expand_y + 1, BORDER );	
			}

			//Cell 3
			expand_value = getValue( expand_grid, expand_x, expand_y + 1 );
			//set the polygon id
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x,     expand_y + 1, polygon_id );
			//polygon_id already exists and is the same... reset it
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x,     expand_y + 1, polygon_id );							
			//set the cell as a border
			else if( expand_value >= 0 )
				setValue( expand_grid, expand_x,     expand_y + 1, BORDER );
			
			//Cell 4
			expand_value = getValue( expand_grid, expand_x - 1, expand_y + 1 );
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x - 1, expand_y + 1, polygon_id );
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x - 1, expand_y + 1, polygon_id );				
			else if( expand_value >= 0 )
				setValue( expand_grid, expand_x - 1, expand_y + 1, BORDER );
			else if( expand_value == BORDER )
			{	expand_value -= 1;
				setValue( expand_grid, expand_x - 1, expand_y + 1,	expand_value );
			}
			else if( expand_value == DOUBLE_BORDER )
			{	if( is_decision( expand_grid, expand_x - 1, expand_y + 1 ) )
					setValue( expand_grid, expand_x - 1, expand_y + 1, DECISION );	
				else
					setValue( expand_grid, expand_x - 1, expand_y + 1, BORDER );	
			}

			//Cell 5
			expand_value = getValue( expand_grid, expand_x - 1, expand_y );
			//set the polygon id
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x - 1, expand_y, polygon_id );
			//polygon_id already exists and is the same... reset it
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x - 1, expand_y, polygon_id );						
			//set the cell as a border
			else if( expand_value >= 0 )
				setValue( expand_grid, expand_x - 1, expand_y, BORDER );
			
			//Cell 6
			expand_value = getValue( expand_grid, expand_x - 1, expand_y - 1 );
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x - 1,     expand_y - 1, polygon_id );
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x - 1,     expand_y - 1, polygon_id );								
			else if( expand_value >= 0 )
				setValue( expand_grid, expand_x - 1,     expand_y - 1, BORDER );
			else if( expand_value == BORDER )
			{	expand_value -= 1;
				setValue( expand_grid, expand_x - 1,	  expand_y - 1,	expand_value );	
			}
			else if( expand_value == DOUBLE_BORDER )
			{	if( is_decision( expand_grid, expand_x - 1, expand_y - 1 ) )
					setValue( expand_grid, expand_x - 1, expand_y - 1, DECISION );	
				else
					setValue( expand_grid, expand_x - 1, expand_y - 1, BORDER );	
			}

			//Cell 7
			expand_value = getValue( expand_grid, expand_x, expand_y - 1 );
			//set the polygon id
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x, expand_y - 1, polygon_id );
			//polygon_id already exists and is the same... reset it
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x, expand_y - 1, polygon_id );						
			//set the cell as a border
			else if( expand_value >= 0 )
				setValue( expand_grid, expand_x, expand_y - 1, BORDER );
			
			//Cell 8
			expand_value = getValue( expand_grid, expand_x + 1, expand_y - 1 );
			if( expand_value == expand_nodata_value )
				setValue( expand_grid, expand_x + 1, expand_y - 1, polygon_id );
			else if( expand_value == polygon_id )
				setValue( expand_grid, expand_x + 1, expand_y - 1, polygon_id );					
			else if( expand_value >= 0 )
				setValue( expand_grid, expand_x + 1, expand_y - 1, BORDER );		
			else if( expand_value == BORDER )
			{	expand_value -= 1;
				setValue( expand_grid, expand_x + 1, expand_y - 1,	expand_value );	
			}
			else if( expand_value == DOUBLE_BORDER )
			{	if( is_decision( expand_grid, expand_x + 1, expand_y - 1 ) )
					setValue( expand_grid, expand_x + 1, expand_y - 1, DECISION );	
				else
					setValue( expand_grid, expand_x + 1, expand_y - 1, BORDER );	
			}
		}			
		
		newpercent = (long)(( (double)( rows - y - 1 )/(total) )*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("Polygon Creation: Expanding Grid" ));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Polygon Creation: Expanding Grid" ));
		}			
	}

	//Mark edges of the expanded grid that have a polygon id as BORDER
	//	and reset DOUBLE_BORDER to BORDER
	//Mark edges of a polygon that have a nodata_value by them
	newpercent = 0;
	percent = 0;
	total = exp_rows;
	for( int j = 0; j < exp_rows; j++ )
	{
		for( int i = 0; i < exp_cols; i++ )
		{
			polygon_id = getValue( expand_grid, i, j );
			if( polygon_id != expand_nodata_value )
			{
				if( j == 0 )
					setValue( expand_grid, i, 0, BORDER );
				else if( j == exp_rows - 1 )
					setValue( expand_grid, i, exp_rows - 1, BORDER );
				else if( i == 0 )
					setValue( expand_grid, 0, j, BORDER );
				else if( i == exp_cols - 1 )
					setValue( expand_grid, exp_cols - 1, j, BORDER );
				else 
				{					
					if( polygon_id == DOUBLE_BORDER )
						setValue( expand_grid, i, j, BORDER );
					//Convex polygon test
					else if( polygon_id == DECISION )
					{	
						if (connection_grid != NULL)
						{
							long contract_x = ( i - 1 )/2;
							long contract_y = ( j - 1 )/2;

							IGridHeader * cHeader = NULL;
							connection_grid->get_Header(&cHeader);
							VARIANT cndv;
							VariantInit(&cndv); //added by Rob Cairns 4-Jan-06
							cHeader->get_NodataValue(&cndv);
							cHeader->Release();
							cHeader = NULL;

							double nodata = -1;
							dVal(cndv,nodata);
							
							VariantClear(&cndv); //added by Rob Cairns 4-Jan-06
							
							double decision = getValue( connection_grid, contract_x, contract_y );
							if( decision == nodata )
							{	expand_grid->Close(&vbretval);							
								*retval = NULL;
								lastErrorCode = tkCONCAVE_POLYGONS;
								if( cBack != NULL )
									cBack->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));	
								else if( globalCallback != NULL )
									globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
								return S_OK;					
							}
						}
					}
					//Test to see if it is on a nodata border
					else
					{	//4 3 2
						//5 0 1
						//6 7 8

						//Cell 1
						double cell1 = getValue( expand_grid, i+1, j );
						//Cell 2
						double cell2 = getValue( expand_grid, i+1, j-1 );
						//Cell 3
						double cell3 = getValue( expand_grid, i, j-1 );
						//Cell 4
						double cell4 = getValue( expand_grid, i-1, j-1 );
						//Cell 5
						double cell5 = getValue( expand_grid, i-1, j );
						//Cell 6
						double cell6 = getValue( expand_grid, i-1, j+1 );
						//Cell 7
						double cell7 = getValue( expand_grid, i, j+1 );
						//Cell 8
						double cell8 = getValue( expand_grid, i+1, j+1 );

						if( cell1 == expand_nodata_value || 
							cell2 == expand_nodata_value || 
							cell3 == expand_nodata_value || 
							cell4 == expand_nodata_value || 
							cell5 == expand_nodata_value || 
							cell6 == expand_nodata_value || 
							cell7 == expand_nodata_value || 
							cell8 == expand_nodata_value )
						{
							if( i%2 == 0 && j%2 == 0 )
							{	if( is_decision( expand_grid, i, j ) )
									setValue( expand_grid, i, j, DECISION );
								else
									setValue( expand_grid, i, j, BORDER );
							}
							else
								setValue( expand_grid, i, j, BORDER );
						}
					}					
				}
			}			
		}

		newpercent = (long)(( j/total )*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("Polygon Creation: Marking Borders" ));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Polygon Creation: Marking Borders" ));
		}	
	}

	//Create the shapefile
	CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)retval);
	(*retval)->CreateNew(A2BSTR(""),SHP_POLYGON,&vbretval);
	
	long fieldpos = 0;
	IField * field = NULL;
	CoCreateInstance(CLSID_Field,NULL,CLSCTX_INPROC_SERVER,IID_IField,(void**)&field);
	field->put_Name(A2BSTR("PolygonID"));
	field->put_Type(INTEGER_FIELD);
	field->put_Width(10);
	(*retval)->EditInsertField(field,&fieldpos,cBack,&vbretval);
	field->Release();
	field = NULL;

	//Create the polygons
	//Only look at the center value of the cells
	std::deque< RasterPoint > polygon;	
	int number_polygons = 0;
	double xllcenter = xllCenter - dx*.5;
	double yllcenter = yllCenter - dy*.5;
	double nodata = expand_nodata_value;
	
	total = rows;
	newpercent = 0;
	percent = 0;
	for( int py = 0; py < rows; py++) //rows - 1; py>= 0; py-- )
	{	for( int px = 0; px < cols; px++ )
		{
			//Expand as the center cell
			long expand_x = 2*px + 1;
			long expand_y = 2*py + 1;
			polygon_id = getValue( expand_grid, expand_x, expand_y );		

			//Check to see if polygon id is a specially marked cell or nodata
			if( polygon_id >= 0 )
			//if( polygon_id == 148 )
			{
				//Clear the polygon list
				polygon.clear();				

				//Mark the edge with
				// a special value.
				mark_edge( polygon_id, expand_x - 1, expand_y - 1 );

				//Always start Trace from Cell 3
				trace_polygon( expand_x - 1, expand_y - 1, polygon );
				
				//Flood fill the polygon with nodata
				scan_fill_to_edge( nodata, expand_x, expand_y + 1 );
				
				if( polygon.size() > 3 )
				{
					long endIndex = polygon.size()-1;

					double x1 = polygon[0].column,
						x2 = polygon[1].column,
						y1 = polygon[0].row, 
						y2 = polygon[1].row;

					cppVector v1(x2 - x1,y2 - y1,0.0);
					
					x1 = polygon[endIndex].column;
					x2 = polygon[0].column;
					y1 = polygon[endIndex].row;
					y2 = polygon[0].row;
					
					cppVector v2(x2 - x1,y2 - y1,0.0);
					
					cppVector cross = v1.crossProduct(v2);

					//Invert Polygon to Clockwise if necessary					
					//Counter ClockWise
					if( cross.getk() > 0.0 )
					{	
						std::deque<RasterPoint> reverse_polygon;
						for( int point = 0; point < (int)polygon.size(); point++ )
							reverse_polygon.push_front( polygon[point] );
						polygon = reverse_polygon;
					}
				
					
					//Write the final polygon
					IShape * shape = NULL;
					CoCreateInstance(CLSID_Shape,NULL,CLSCTX_INPROC_SERVER,IID_IShape,(void**)&shape);
					shape->Create(SHP_POLYGON,&vbretval);
					long ppos = 0;
					shape->InsertPart(0,&ppos,&vbretval);

					double poly_x = 0, poly_y = 0;
					long ras_x = 0, ras_y = 0;
					long last_point_index = -1;
					int point = 0;
					for( point = 0; point < (int)polygon.size(); point++) //(int)polygon.size()-1; point >= 0; point-- )
					{	
						ras_x = polygon[point].column;
						ras_y = polygon[point].row;

						//Check if the raster_point is a joint

						//Find the four cells in the flow grid
						//Constrict to center cell of connection_grid
						//( x, y ) == Cell 0
						//4 3 2
						//5 0 1
						//6 7 8

						//Cell 4
						double cell4_x = ( (ras_x-1) - 1 )/2;
						double cell4_y = ( (ras_y-1) - 1 )/2;
						//Cell 6
						double cell6_x = ( (ras_x-1) - 1 )/2;
						double cell6_y = ( (ras_y+1) - 1 )/2;
						//Cell 2
						double cell2_x = ( (ras_x+1) - 1 )/2;
						double cell2_y = ( (ras_y-1) - 1 )/2;
						//Cell 8			
						double cell8_x = ( (ras_x+1) - 1 )/2;
						double cell8_y = ( (ras_y+1) - 1 )/2;

						double cell2 = getValue( Grid, (long)cell2_x, (long)cell2_y );
						double cell8 = getValue( Grid, (long)cell8_x, (long)cell8_y );
						double cell4 = getValue( Grid, (long)cell4_x, (long)cell4_y );
						double cell6 = getValue( Grid, (long)cell6_x, (long)cell6_y );

						bool write_point = true;
						if( is_joint( cell2, cell8, cell4, cell6 ) )
							write_point = true;
						else if( ras_x == 0 || ras_x == exp_cols - 1 )
						{	write_point = true;
						}
						else if( ras_y == 0 || ras_y == exp_rows - 1 )
						{	write_point = true;
						}
						//Remove the Jagged Points if it isn't a joint
						else
						{	long ras_behind_x = 0, ras_behind_y = 0;
							long ras_front_x = 0, ras_front_y = 0;

							if( point == 0 )
							{	ras_behind_x = polygon[polygon.size()-1].column;
								ras_behind_y = polygon[polygon.size()-1].row;
							}
							else
							{	ras_behind_x = polygon[point-1].column;
								ras_behind_y = polygon[point-1].row;
							}

							if( point == polygon.size() -1 )
							{	ras_front_x = polygon[0].column;
								ras_front_y = polygon[0].row;
							}
							else
							{	ras_front_x = polygon[point+1].column;
								ras_front_y = polygon[point+1].row;
							}
							
							if( abs( ras_behind_x - ras_front_x ) == 1 &&
								abs( ras_behind_y - ras_front_y ) == 1 )
									write_point = false;
						}

						//Write the point if it's a joint or not a jaggy
						if( write_point == true )
						{	if( last_point_index < 0 )
								last_point_index = point;
							expand_grid->CellToProj( polygon[point].column, polygon[point].row, &poly_x, &poly_y );
							IPoint * ipoint = NULL;
							CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&ipoint);
							ipoint->put_X(poly_x);
							ipoint->put_Y(poly_y);
							long pindex = point;
							shape->InsertPoint( ipoint, &pindex, &vbretval ); 						
							ipoint->Release();
							ipoint = NULL;
						}
					}
					//Write the last point again
					expand_grid->CellToProj( polygon[last_point_index].column, polygon[last_point_index].row, &poly_x, &poly_y );
					IPoint * ipoint = NULL;
					CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&ipoint);
					ipoint->put_X(poly_x);
					ipoint->put_Y(poly_y);
					long pindex = point + 1;
					shape->InsertPoint( ipoint, &pindex, &vbretval );
					ipoint->Release();
					ipoint = NULL;

					long sindex = number_polygons;
					(*retval)->EditInsertShape(shape,&sindex,&vbretval);

					shape->Release();
					shape = NULL;

					VARIANT cval;
					VariantInit(&cval); //added by Rob Cairns 4-Jan-06
					cval.vt = VT_I4;
					cval.lVal = (LONG)polygon_id;
					(*retval)->EditCellValue(fieldpos,sindex,cval,&vbretval);				
					number_polygons++;
					VariantClear(&cval); //added by Rob Cairns 4-Jan-06
				}
			}					
		}

		newpercent = (long)((double)py/(double)rows*100);
		if( newpercent > percent )
		{	percent = newpercent;
			if( cBack != NULL )
				cBack->Progress(OLE2BSTR(key),percent,A2BSTR("Polygon Creation: Creating Polygons"));
			else if( globalCallback != NULL )
				globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Polygon Creation: Creating Polygons"));
		}	
	}

	expand_grid->Close(&vbretval);
	expand_grid->Release();
	expand_grid = NULL;
	
	VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
	
	if (*retval)
	{
		((CShapefile*)(*retval))->ValidateOutput(retval, "GridToShapefile", "Utils");
	}

	return S_OK;
}

//Determine if a given index of [] map is a DECISION SPOT . . .
//DECISION SPOT is a point where multiple path options are available
inline bool CUtils::is_decision( IGrid * g, int x, int y )
{	
	// ( x, y ) corresponds to 0

	//4 3 2
	//5 0 1
	//6 7 8

	//Check if cells (2 & 6) or (4 & 8) have the same polygon id.
	//If they do then this is a DECISION SPOT if there are three
	//unique values.
	
	double four = getValue( g, x - 1, y - 1 );
	double eight = getValue( g, x + 1, y + 1 );
	double two = getValue( g, x + 1, y - 1 );
	double six = getValue( g, x - 1, y + 1 );
	
	IGridHeader * header = NULL;
	g->get_Header(&header);
	VARIANT vval;
	VariantInit(&vval); //added by Rob Cairns 4-Jan-06
	header->get_NodataValue(&vval);
	double nodata = -1;
	dVal(vval,nodata);
	VariantClear(&vval); //added by Rob Cairns 4-Jan-06
	header->Release();
	header = NULL;

	if( two == six )
	{	if( two == four || two == eight )
			return false;
		else if( six == four || six == eight )
			return false;

		return true;
	}
	else if( four == eight )
	{	if( four == two || four == six )
			return false;
		else if( eight == two || eight == six )
			return false;
		return true;
	}

	return false;
}

//Mark the edges so the polygon can be traced
void CUtils::mark_edge( double & polygon_id, long x, long y )
{	
	std::stack< RasterPoint > stack;
	std::vector< RasterPoint > decisions;
	RasterPoint pt;
	//Change the BORDERS to TRACE_BORDERS
	stack.push( RasterPoint( x, y ) );
	int i = x, j = y;
	while( !stack.empty() )
	{		
		pt = stack.top();
		i = pt.column;
		j = pt.row;
		//i = stack[0].column;
		//j = stack[0].row;
		stack.pop(); //_front();
		//4 3 2
		//5 0 1
		//6 7 8
		//Cell 1
		double cell1 = getValue( expand_grid, i+1, j );		
		//Cell 2
		double cell2 = getValue( expand_grid, i+1, j-1 );		
		//Cell 3
		double cell3 = getValue( expand_grid, i, j-1 );		
		//Cell 4
		double cell4 = getValue( expand_grid, i-1, j-1 );		
		//Cell 5
		double cell5 = getValue( expand_grid, i-1, j );		
		//Cell 6
		double cell6 = getValue( expand_grid, i-1, j+1 );		
		//Cell 7
		double cell7 = getValue( expand_grid, i, j+1 );		
		//Cell 8
		double cell8 = getValue( expand_grid, i+1, j+1 );		

		if( cell1 == polygon_id || 
			cell2 == polygon_id || 
			cell3 == polygon_id || 
			cell4 == polygon_id || 
			cell5 == polygon_id || 
			cell6 == polygon_id || 
			cell7 == polygon_id || 
			cell8 == polygon_id )
		{	
			if( cell8 == BORDER )
				stack.push( RasterPoint( i+1, j+1 ));
			if( cell7 == BORDER )
				stack.push( RasterPoint( i, j+1 ));
			if( cell6 == BORDER )
				stack.push( RasterPoint( i-1, j+1 ));
			if( cell5 == BORDER )
				stack.push( RasterPoint( i-1, j ));
			if( cell4 == BORDER )
				stack.push( RasterPoint( i-1, j-1 ));
			if( cell3 == BORDER )
				stack.push( RasterPoint( i, j-1 ));
			if( cell2 == BORDER )
				stack.push( RasterPoint( i+1, j-1 ));
			if( cell1 == BORDER )
				stack.push( RasterPoint( i+1, j ));

			//Don't reset if DECISION
			if( getValue( expand_grid, i, j ) == BORDER )
				setValue( expand_grid, i, j, TRACE_BORDER );
			//Push diagonals if this is a DECISION
			else if( getValue( expand_grid, i, j ) == DECISION )
			{	stack.push( RasterPoint( i+1, j+1 ) );
				stack.push( RasterPoint( i-1, j+1 ) );
				stack.push( RasterPoint( i-1, j-1 ) );
				stack.push( RasterPoint( i+1, j-1 ) );
			}
		}
	}

	//Mark Polygon as the CURRENT_POLYGON
	stack.push( RasterPoint( x+1, y+1 ) );
	while( !stack.empty() )
	{	
		pt = stack.top();
		i = pt.column;
		j = pt.row;
		//i = stack[0].column;
		//j = stack[0].row;
		stack.pop();
		//4 3 2
		//5 0 1
		//6 7 8
		//Cell 1
		double cell1 = getValue( expand_grid, i+1, j );
		if( cell1 == polygon_id || cell1 == DECISION )
			stack.push( RasterPoint( i+1, j ));
		//Cell 2
		double cell2 = getValue( expand_grid, i+1, j-1 );
		if( cell2 == polygon_id || cell2 == DECISION  )
			stack.push( RasterPoint( i+1, j-1 ));
		//Cell 3
		double cell3 = getValue( expand_grid, i, j-1 );
		if( cell3 == polygon_id || cell3 == DECISION  )
			stack.push( RasterPoint( i, j-1 ));
		//Cell 4
		double cell4 = getValue( expand_grid, i-1, j-1 );
		if( cell4 == polygon_id || cell4 == DECISION  )
			stack.push( RasterPoint( i-1, j-1 ));
		//Cell 5
		double cell5 = getValue( expand_grid, i-1, j );
		if( cell5 == polygon_id || cell5 == DECISION  )
			stack.push( RasterPoint( i-1, j ));
		//Cell 6
		double cell6 = getValue( expand_grid, i-1, j+1 );
		if( cell6 == polygon_id || cell6 == DECISION  )
			stack.push( RasterPoint( i-1, j+1 ));
		//Cell 7
		double cell7 = getValue( expand_grid, i, j+1 );
		if( cell7 == polygon_id || cell7 == DECISION  )
			stack.push( RasterPoint( i, j+1 ));
		//Cell 8
		double cell8 = getValue( expand_grid, i+1, j+1 );
		if( cell8 == polygon_id || cell8 == DECISION  )
			stack.push( RasterPoint( i+1, j+1 ));

		//Don't reset if DECISION
		if( getValue( expand_grid, i, j ) == polygon_id )
			setValue( expand_grid, i, j, CURRENT_POLYGON );
		//Push diagonals if this is a DECISION
		else if( getValue( expand_grid, i, j ) == DECISION )
		{	setValue( expand_grid, i, j, CURRENT_POLYGON );
			decisions.push_back( RasterPoint( i, j ) );
			stack.push( RasterPoint( i+1, j+1 ) );
			stack.push( RasterPoint( i-1, j+1 ) );
			stack.push( RasterPoint( i-1, j-1 ) );
			stack.push( RasterPoint( i+1, j-1 ) );
		}			
	}
	
	for( int d1 = 0; d1 < (int)decisions.size(); d1++ )
		setValue( expand_grid, decisions[d1].column, decisions[d1].row, DECISION );
}	

//Erase the CurrenClipperLib::Polygon
void CUtils::scan_fill_to_edge( double & nodata, long x, long y )
{	
	std::stack< RasterPoint > stack;
	std::vector< RasterPoint > decisions;
	RasterPoint pt;

	//Erase the Polygon
	int i = x, j = y;
	stack.push( RasterPoint( x, y-1 ) );
	while( !stack.empty() )
	{	
		pt = stack.top();
		i = pt.column;
		j = pt.row;
		stack.pop();
		//4 3 2
		//5 0 1
		//6 7 8
		//Cell 1
		double cell1 = getValue( expand_grid, i+1, j );
		if( cell1 == CURRENT_POLYGON || cell1 == DECISION )
			stack.push( RasterPoint( i+1, j ));
		//Cell 2
		double cell2 = getValue( expand_grid, i+1, j-1 );
		if( cell2 == CURRENT_POLYGON || cell2 == DECISION  )
			stack.push( RasterPoint( i+1, j-1 ));
		//Cell 3
		double cell3 = getValue( expand_grid, i, j-1 );
		if( cell3 == CURRENT_POLYGON || cell3 == DECISION  )
			stack.push( RasterPoint( i, j-1 ));
		//Cell 4
		double cell4 = getValue( expand_grid, i-1, j-1 );
		if( cell4 == CURRENT_POLYGON || cell4 == DECISION  )
			stack.push( RasterPoint( i-1, j-1 ));
		//Cell 5
		double cell5 = getValue( expand_grid, i-1, j );
		if( cell5 == CURRENT_POLYGON || cell5 == DECISION  )
			stack.push( RasterPoint( i-1, j ));
		//Cell 6
		double cell6 = getValue( expand_grid, i-1, j+1 );
		if( cell6 == CURRENT_POLYGON || cell6 == DECISION  )
			stack.push( RasterPoint( i-1, j+1 ));
		//Cell 7
		double cell7 = getValue( expand_grid, i, j+1 );
		if( cell7 == CURRENT_POLYGON || cell7 == DECISION  )
			stack.push( RasterPoint( i, j+1 ));
		//Cell 8
		double cell8 = getValue( expand_grid, i+1, j+1 );
		if( cell8 == CURRENT_POLYGON || cell8 == DECISION  )
			stack.push( RasterPoint( i+1, j+1 ));

		//Don't reset if DECISION
		if( getValue( expand_grid, i, j ) == CURRENT_POLYGON )
			setValue( expand_grid, i, j, nodata );
		//Push diagonals if this is a DECISION
		else if( getValue( expand_grid, i, j ) == DECISION )
		{	setValue( expand_grid, i, j, nodata );
			decisions.push_back( RasterPoint( i, j ) );
			stack.push( RasterPoint( i+1, j+1 ) );
			stack.push( RasterPoint( i-1, j+1 ) );
			stack.push( RasterPoint( i-1, j-1 ) );
			stack.push( RasterPoint( i+1, j-1 ) );
		}			
	}
	
	for( int d1 = 0; d1 < (int)decisions.size(); d1++ )
		setValue( expand_grid, decisions[d1].column, decisions[d1].row, DECISION );
}	

inline double CUtils::getValue( IGrid * Grid, long column, long row )
{	VARIANT vval;
	VariantInit(&vval); //added by Rob Cairns 4-Jan-06
	Grid->get_Value(column,row,&vval);
	double val = 0;
	dVal(vval,val);
	VariantClear(&vval); //added by Rob Cairns 4-Jan-06
	return val;
}

inline void CUtils::setValue( IGrid * Grid, long column, long row, double val )
{	VARIANT vval;
	VariantInit(&vval); //added by Rob Cairns 4-Jan-06
	vval.vt = VT_R8;
	vval.dblVal = val;
	Grid->put_Value(column,row,vval);
	VariantClear(&vval); //added by Rob Cairns 4-Jan-06
}

void CUtils::trace_polygon( long x, long y, std::deque<RasterPoint> & polygon )
{	
	polygon.push_back( RasterPoint( x, y ) );

	std::stack<RasterPoint> stack;
	RasterPoint pt;

	stack.push(RasterPoint(x,y));

	while( !stack.empty() )
	{
		pt = stack.top();
		x = pt.column;
		y = pt.row;
		//x = stack[0].column;
		//y = stack[0].row;
		stack.pop();

		

		//Reset the current grid value back to BORDER
		if( getValue( expand_grid, x, y ) == TRACE_BORDER )
			setValue( expand_grid, x, y, BORDER );
		
		// ( x, y ) corresponds to 0
		//4 3 2
		//5 0 1
		//6 7 8
		
		bool moved_turtle = false;
		//Restrict to Cardinal Directions
		//Cell 1
		if( getValue( expand_grid, x + 1, y ) == TRACE_BORDER )
		{	
			moved_turtle = true;
			//trace_polygon( x + 1, y, polygon );
			stack.push(RasterPoint(x + 1, y));
			polygon.push_back(RasterPoint(x + 1, y));
		}
		//Cell 3
		else if( getValue( expand_grid, x, y - 1 ) == TRACE_BORDER )
		{	
			moved_turtle = true;
			//trace_polygon( x, y - 1, polygon );
			stack.push(RasterPoint(x, y - 1));
			polygon.push_back(RasterPoint(x, y - 1));
		}
		//Cell 5
		else if( getValue( expand_grid, x - 1, y ) == TRACE_BORDER )
		{	
			moved_turtle = true;
			//trace_polygon( x - 1, y, polygon );
			stack.push(RasterPoint(x - 1, y));
			polygon.push_back(RasterPoint(x - 1, y));
		}
		//Cell 7
		else if( getValue( expand_grid, x, y + 1 ) == TRACE_BORDER )
		{	
			moved_turtle = true;
			//trace_polygon( x, y + 1, polygon );
			stack.push(RasterPoint(x, y + 1));
			polygon.push_back(RasterPoint(x, y + 1));
		}
		
		//Look for a Decision Node ...
		//Cell 1
		if( !moved_turtle )
		{
			if( getValue( expand_grid, x + 1, y ) == DECISION )
			{
				if( connection_grid == NULL )
				{
					polygon.clear();
					return;
				}
				
				//Find the four cells in the flow grid
				//Constrict to center cell of connection_grid
				//( x, y ) now == Cell 5 
				//4 3 2
				//5 0 1
				//6 7 8
				
				//Cell 2
				cell2_x = ( (x+2) - 1 )/2;
				cell2_y = ( (y-1) - 1 )/2;
				//Cell 4
				cell4_x = ( (x) - 1 )/2;
				cell4_y = ( (y-1) - 1 )/2;
				//Cell 6
				cell6_x = ( (x) - 1 )/2;
				cell6_y = ( (y+1) - 1 )/2;			
				//Cell 8			
				cell8_x = ( (x+2) - 1 )/2;
				cell8_y = ( (y+1) - 1 )/2;
				
				flow2 = getValue( connection_grid, cell2_x, cell2_y );
				flow8 = getValue( connection_grid, cell8_x, cell8_y );
				flow4 = getValue( connection_grid, cell4_x, cell4_y );
				flow6 = getValue( connection_grid, cell6_x, cell6_y );
				
				//Move to Up-Right ... maybe
				if( flow2 == 6 || flow6 == 2 )
				{	
					if( getValue( expand_grid, x + 1, y - 1 ) == TRACE_BORDER )
					{
						moved_turtle = true;
						//trace_polygon( x + 1, y - 1, polygon );
						stack.push(RasterPoint( x + 1, y - 1));
						polygon.push_back(RasterPoint(x + 1, y - 1));
					}
				}
				//Move to Down-Right ... maybe
				else if( flow4 == 8 || flow8 == 4 )
				{	
					if( getValue( expand_grid, x + 1, y + 1 ) == TRACE_BORDER )
					{
						moved_turtle = true;
						//trace_polygon( x + 1, y + 1, polygon );
						stack.push(RasterPoint(x + 1, y + 1));
						polygon.push_back(RasterPoint(x + 1, y + 1));
					}
				}
				//Cannot be determined by flow directions
				else
				{	
					if( getValue( expand_grid, x, y - 1 ) == CURRENT_POLYGON )
					{
						moved_turtle = true;
						//trace_polygon( x + 1, y - 1, polygon );
						stack.push(RasterPoint(x + 1, y - 1));
						polygon.push_back(RasterPoint(x + 1, y - 1));
					}
					else
					{
						moved_turtle = true;
						//trace_polygon( x + 1, y + 1, polygon );
						stack.push(RasterPoint(x + 1, y + 1));
						polygon.push_back(RasterPoint(x + 1, y + 1));
					}
				}
			}
		}
		
		//Cell 3
		if( !moved_turtle )
		{
			if( getValue( expand_grid, x, y - 1 ) == DECISION )
			{
				if( connection_grid == NULL )
				{
					polygon.clear();
					return;
				}
				
				//Find the four cells in the flow grid
				//Constrict to center cell of connection_grid
				//( x, y ) now == Cell 7
				//4 3 2
				//5 0 1
				//6 7 8
				
				//Cell 2
				cell2_x = ( (x+1) - 1 )/2;
				cell2_y = ( (y-2) - 1 )/2;
				//Cell 4
				cell4_x = ( (x-1) - 1 )/2;
				cell4_y = ( (y-2) - 1 )/2;
				//Cell 6
				cell6_x = ( (x-1) - 1 )/2;
				cell6_y = ( (y) - 1 )/2;			
				//Cell 8			
				cell8_x = ( (x+1) - 1 )/2;
				cell8_y = ( (y) - 1 )/2;
				
				flow2 = getValue( connection_grid, cell2_x, cell2_y );
				flow8 = getValue( connection_grid, cell8_x, cell8_y );
				flow4 = getValue( connection_grid, cell4_x, cell4_y );
				flow6 = getValue( connection_grid, cell6_x, cell6_y );
				
				//Move to Up-Right ... maybe
				if( flow2 == 6 || flow6 == 2 )
				{	
					if( getValue( expand_grid, x + 1, y - 1 ) == TRACE_BORDER )
					{
						moved_turtle = true;
						//trace_polygon( x + 1, y - 1, polygon );
						stack.push(RasterPoint(x + 1, y - 1));
						polygon.push_back(RasterPoint(x + 1, y - 1));
					}
				}
				//Move to Up-Left ... maybe
				else if( flow4 == 8 || flow8 == 4 )
				{
					if( getValue( expand_grid, x - 1, y - 1 ) == TRACE_BORDER )
					{	
						moved_turtle = true;
						//trace_polygon( x - 1, y - 1, polygon );
						stack.push(RasterPoint(x - 1, y - 1));
						polygon.push_back(RasterPoint(x - 1, y - 1));
					}
				}
				//Cannot be determined by flow directions
				else
				{	
					if( getValue( expand_grid, x - 1, y ) == CURRENT_POLYGON )
					{
						moved_turtle = true;
						//trace_polygon( x - 1, y - 1, polygon );
						stack.push(RasterPoint(x - 1, y - 1));
						polygon.push_back(RasterPoint(x - 1, y - 1));
					}
					else
					{
						moved_turtle = true;
						//trace_polygon( x + 1, y - 1, polygon );
						stack.push(RasterPoint(x + 1, y - 1));
						polygon.push_back(RasterPoint(x + 1, y - 1));
					}
				}
			}
		}
		
		//Cell 5
		if( !moved_turtle )
		{
			if( getValue( expand_grid, x - 1, y ) == DECISION )
			{
				if( connection_grid == NULL )
				{
					polygon.clear();
					return;
				}
				
				//Find the four cells in the flow grid
				//Constrict to center cell of connection_grid
				//( x, y ) now == Cell 1
				//4 3 2
				//5 0 1
				//6 7 8
				
				//Cell 2
				cell2_x = ( (x) - 1 )/2;
				cell2_y = ( (y-1) - 1 )/2;
				//Cell 4
				cell4_x = ( (x-2) - 1 )/2;
				cell4_y = ( (y-1) - 1 )/2;
				//Cell 6
				cell6_x = ( (x-2) - 1 )/2;
				cell6_y = ( (y+1) - 1 )/2;			
				//Cell 8			
				cell8_x = ( (x) - 1 )/2;
				cell8_y = ( (y+1) - 1 )/2;
				
				flow2 = getValue( connection_grid, cell2_x, cell2_y );
				flow8 = getValue( connection_grid, cell8_x, cell8_y );
				flow4 = getValue( connection_grid, cell4_x, cell4_y );
				flow6 = getValue( connection_grid, cell6_x, cell6_y );
				
				//Move to Down-Left ... maybe
				if( flow2 == 6 || flow6 == 2 )
				{	
					if( getValue( expand_grid, x - 1, y + 1 ) == TRACE_BORDER )
					{
						moved_turtle = true;
						//trace_polygon( x - 1, y + 1, polygon );
						stack.push(RasterPoint(x - 1, y + 1));
						polygon.push_back(RasterPoint(x - 1, y + 1));
					}
				}
				//Move to Up-Left ... maybe
				else if( flow4 == 8 || flow8 == 4 )
				{	
					if( getValue( expand_grid, x - 1, y - 1 ) == TRACE_BORDER )
					{
						moved_turtle = true;
						//trace_polygon( x - 1, y - 1, polygon );
						stack.push(RasterPoint(x - 1, y - 1));
						polygon.push_back(RasterPoint(x - 1, y - 1));
					}
				}
				//Cannot be determined by flow directions
				else
				{	
					if( getValue( expand_grid, x, y - 1 ) == CURRENT_POLYGON )
					{	
						moved_turtle = true;
						//trace_polygon( x - 1, y - 1, polygon );
						stack.push(RasterPoint(x - 1, y - 1));
						polygon.push_back(RasterPoint(x - 1, y - 1));
					}
					else
					{
						moved_turtle = true;
						//trace_polygon( x - 1, y + 1, polygon );
						stack.push(RasterPoint(x - 1, y + 1));
						polygon.push_back(RasterPoint(x - 1, y + 1));
					}
				}
			}	
		}
		
		//Cell 7
		if( !moved_turtle )	{	
			if( getValue( expand_grid, x, y + 1 ) == DECISION )
			{
				if( connection_grid == NULL )
				{
					polygon.clear();
					return;
				}
				
				//Find the four cells in the flow grid
				//Constrict to center cell of connection_grid
				//( x, y ) now == Cell 3
				//4 3 2
				//5 0 1
				//6 7 8
				
				//Cell 2
				cell2_x = ( (x+1) - 1 )/2;
				cell2_y = ( (y) - 1 )/2;
				//Cell 4
				cell4_x = ( (x-1) - 1 )/2;
				cell4_y = ( (y) - 1 )/2;
				//Cell 6
				cell6_x = ( (x-1) - 1 )/2;
				cell6_y = ( (y+2) - 1 )/2;			
				//Cell 8			
				cell8_x = ( (x+1) - 1 )/2;
				cell8_y = ( (y+2) - 1 )/2;
				
				flow2 = getValue( connection_grid, cell2_x, cell2_y );
				flow8 = getValue( connection_grid, cell8_x, cell8_y );
				flow4 = getValue( connection_grid, cell4_x, cell4_y );
				flow6 = getValue( connection_grid, cell6_x, cell6_y );
				
				//Move to Down-Left ... maybe
				if( flow2 == 6 || flow6 == 2 )
				{	
					if( getValue( expand_grid, x - 1, y + 1 ) == TRACE_BORDER )
					{	
						moved_turtle = true;
						//trace_polygon( x - 1, y + 1, polygon );
						stack.push(RasterPoint(x - 1, y + 1));
						polygon.push_back(RasterPoint(x - 1, y + 1));
					}
				}
				//Move to Down-Right ... maybe
				else if( flow4 == 8 || flow8 == 4 )
				{	
					if( getValue( expand_grid, x + 1, y + 1 ) == TRACE_BORDER )
					{	
						moved_turtle = true;
						//trace_polygon( x + 1, y + 1, polygon );
						stack.push(RasterPoint( x + 1, y + 1));
						polygon.push_back(RasterPoint(x + 1, y + 1));
					}
				}
				//Cannot be determined by flow directions
				else
				{	
					if( getValue( expand_grid, x - 1, y ) == CURRENT_POLYGON )
					{
						moved_turtle = true;
						//trace_polygon( x - 1, y + 1, polygon );
						stack.push(RasterPoint(x - 1, y + 1));
						polygon.push_back(RasterPoint(x - 1, y + 1));
					}
					else
					{	
						moved_turtle = true;
						//trace_polygon( x + 1, y + 1, polygon );
						stack.push(RasterPoint(x + 1, y + 1));
						polygon.push_back(RasterPoint(x + 1, y + 1));
					}
				}
			}
		}
		
	}
}

inline bool CUtils::is_joint( double cell2, double cell8, double cell4, double cell6 )
{	
	int number_unique = 1;
	if( cell2 != cell8 && cell2 != cell4 && cell2 != cell6 )
		number_unique++;
	if( cell8 != cell2 && cell8 != cell4 && cell8 != cell6 )
		number_unique++;
	if( cell4 != cell2 && cell4 != cell8 && cell4 != cell6 )
		number_unique++;
	if( cell6 != cell2 && cell6 != cell8 && cell6 != cell4 )
		number_unique++;

	if( number_unique > 2 )
		return true;
	return false;
}


///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
//			POLYGONAL ALGORITHM
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
///\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

#ifndef ROUND
#define ROUND(X)  (long)(X + .5)
#endif

STDMETHODIMP CUtils::ShapefileToGrid(IShapefile * Shpfile, VARIANT_BOOL UseShapefileBounds, IGridHeader * GridHeader, double Cellsize, VARIANT_BOOL UseShapeNumber, short SingleValue,IGrid ** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	if( Shpfile == NULL )
	{	
		*retval = NULL;
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}
	
	if (!((CShapefile*)Shpfile)->ValidateInput(Shpfile, "ShapefileToGrid", "Shpfile", VARIANT_FALSE, "Utils"))
		return S_FALSE;

	CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)retval);

	VARIANT_BOOL result = VARIANT_FALSE;
	VARIANT vndv; //no data value
	VariantInit(&vndv); //added by Rob Cairns 4-Jan-06
	vndv.vt = VT_I2;
	short NoData;

	if (UseShapefileBounds == VARIANT_FALSE)
	{
		if ( GridHeader == NULL)
		{	//if the user doesn't want to use the shapefilebounds, then they MUST
			//give us a GridHeader...so, if this is null, fail
			(*retval)->Release();
			*retval = NULL;
			this->ErrorMessage(tkINVALID_PARAMETER_VALUE);
			goto cleaning;		
		}
		

		//attempt to create a new grid of the correct size
		GridHeader->get_NodataValue(&vndv);
		sVal(vndv,NoData);

		if (UseShapeNumber == VARIANT_FALSE && NoData == SingleValue)
		{
			//we must fail because we can't use the nodatavalue from the grid
			//as the singlevalue of the shapefile being converted to a grid;
			(*retval)->Release();
			*retval = NULL;
			this->ErrorMessage(tkINVALID_PARAMETER_VALUE);
			goto cleaning;
		}

		(*retval)->CreateNew(A2BSTR(""),GridHeader,ShortDataType ,vndv,VARIANT_TRUE,UseExtension,globalCallback,&result);

		if ( result == VARIANT_FALSE)
		{ //failed to create new grid
			(*retval)->get_LastErrorCode(&lastErrorCode);
			(*retval)->Release();
			*retval = NULL;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
			goto cleaning;
		}

		long NumShapes;

		Shpfile->get_NumShapes(&NumShapes);

		IShape * shape = NULL;
		
		short cellValue;

		//loop through each shape and examine all points within the shape
		for(short CurShape = 0; CurShape < NumShapes; CurShape++)
		{
			if (UseShapeNumber == VARIANT_FALSE)
				cellValue = SingleValue;
			else
				cellValue = CurShape;

			((CShapefile*)Shpfile)->GetValidatedShape(CurShape,&shape);
			if (!shape) continue;
			if ( PolygonToGrid(shape,retval,cellValue) == false)
				break;
			shape->Release();
		}
	}
	else
	{
		return NULL; //not complete

		IGridHeader * LocalGridHeader;
		(*retval)->get_Header(&LocalGridHeader);

		//attempt to create a new grid of the correct size
		LocalGridHeader->get_NodataValue(&vndv);
		sVal(vndv,NoData);

		//make sure that the singlevalue they want to use isn't the nodata value we are using
		if (UseShapeNumber == VARIANT_FALSE && NoData == SingleValue)
		{
			NoData = NoData - 1;
			vndv.vt = VT_I2;
			vndv.iVal = NoData;

			LocalGridHeader->put_NodataValue(vndv);
		}

		//Create a header that uses the minimum size allowed
		IExtents * bndbox;
		Shpfile->get_Extents(&bndbox);

		double xllcenter = 0, yllcenter = 0;
		double xurcenter = 0, yurcenter = 0;
		bndbox->get_xMin(&xllcenter);
		bndbox->get_yMin(&yllcenter);
		bndbox->get_xMax(&xurcenter);
		bndbox->get_yMax(&yurcenter);
		bndbox->Release();
		
		if( Cellsize <= 1 )
		{	*retval = NULL;
			this->ErrorMessage(	tkINVALID_PARAMETER_VALUE);
			goto cleaning;
		}

		long ncols = ROUND( ( xurcenter - xllcenter ) / Cellsize ) + 1;
		long nrows = ROUND( ( yurcenter - yllcenter )/ Cellsize ) + 1;

		LocalGridHeader->put_dX(Cellsize);
		LocalGridHeader->put_dY(Cellsize);
		VARIANT vndv;
		VariantInit(&vndv); //added by Rob Cairns 4-Jan-06 - is this code reachable?
		vndv.iVal = NoData;
		LocalGridHeader->put_NodataValue(vndv);
		LocalGridHeader->put_NumberCols(ncols);
		LocalGridHeader->put_NumberRows(nrows);
		LocalGridHeader->put_XllCenter(xllcenter );
		LocalGridHeader->put_YllCenter(yllcenter);
		
		(*retval)->CreateNew(A2BSTR(""),LocalGridHeader,ShortDataType ,vndv,TRUE,UseExtension,globalCallback,&result);

		bndbox->Release();
		LocalGridHeader->Release();

		if ( result == VARIANT_FALSE)
		{ //failed to create new grid
			(*retval)->get_LastErrorCode(&lastErrorCode);
			(*retval)->Release();
			*retval = NULL;
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
			return S_OK;
		}

		long NumShapes;

		Shpfile->get_NumShapes(&NumShapes);

		IShape * shape = NULL;
		
		short cellValue;

		//loop through each shape and examine all points within the shape
		for(short CurShape = 0; CurShape < NumShapes; CurShape++)
		{
			if (UseShapeNumber == FALSE)
				cellValue = SingleValue;
			else
				cellValue = CurShape;

			((CShapefile*)Shpfile)->GetValidatedShape(CurShape,&shape);
			if (!shape) continue;
			if ( PolygonToGrid(shape,retval,cellValue) == false)
				break;
			shape->Release();
		}
	}

cleaning:
	VariantClear(&vndv); //added by Rob Cairns 4-Jan-06
	Utility::DisplayProgressCompleted(globalCallback, key);
	((CShapefile*)Shpfile)->ClearValidationList();
	return S_OK;
}

// *******************************************************************
//		PolygonToGrid()
// *******************************************************************
bool CUtils::PolygonToGrid(IShape * shape, IGrid ** grid, short cellValue)
{
	LineBresenham Line;
	std::vector<lbPoint> ListOfPoints;
	lbPoint StartPoint, EndPoint;
	IPoint * shpPoint = NULL;
	long NumPoints = 0;
	double CurX;
	double CurY;

	ListOfPoints.clear();
	
	if (shape == NULL || grid == NULL)
	{
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return false;
	}
	
	CComVariant val;
	val.vt = VT_I2;			//the variant needs to be declared as a short
	val.iVal = cellValue;	//set it equal to the value specified in parameter list

	shape->get_NumPoints(&NumPoints);
	for(int CurPoint = 0; CurPoint <= NumPoints; CurPoint++)
	{
		if ( CurPoint < NumPoints)
		{
			shape->get_Point(CurPoint,&shpPoint);
		}
		else 
		{	
			//we need to make sure to connect the first point to the last point
			ShpfileType shapetype;
			shape->get_ShapeType(&shapetype);

			if (shapetype == SHP_POLYGON || shapetype == SHP_POLYGONZ || shapetype == SHP_POLYGONM )
			{
				shape->get_Point(0,&shpPoint);
			}
			else
				break;//jump out of loop, we are done
		}
		
		if ( !shpPoint )
		{
			shape->get_LastErrorCode(&lastErrorCode);
			if( globalCallback != NULL )
				globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
			return false;
		}
		else
		{
			//if we got to this point, then we have a valid shape and a valid shpPoint
			if ( CurPoint == 0  )
			{	
				//we are one the first point, so just store the point and continue
				shpPoint->get_X(&CurX);
				shpPoint->get_Y(&CurY);
				shpPoint->Release();
				
				//this next line uses the grid to calculate the location of 
				//the current point within the grid
				(*grid)->ProjToCell(CurX,CurY,&(EndPoint.x),&(EndPoint.y));
			}
			else
			{
				//set the previous point equal to the currentPoint
				//then get the new Current point
				StartPoint = EndPoint;
				
				shpPoint->get_X(&CurX);
				shpPoint->get_Y(&CurY);
				shpPoint->Release();
				
				//this next line uses the grid to calculate the location of 
				//the current point within the grid
				(*grid)->ProjToCell(CurX,CurY,&(EndPoint.x),&(EndPoint.y));
				
				//now run the bresenham on the two points so far
				ListOfPoints = Line.ComputeLinePoints(StartPoint,EndPoint);
				
				for (int i = 0; i < (int)ListOfPoints.size(); i++)
					(*grid)->put_Value(ListOfPoints[i].x,ListOfPoints[i].y, val);
			}
		}
	}//end for

	return true;
}

STDMETHODIMP CUtils::hBitmapToPicture(long hBitmap, IPictureDisp **retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	HBITMAP bitmap = (HBITMAP)hBitmap;
	PICTDESC pictdesc;
	pictdesc.cbSizeofstruct = sizeof(PICTDESC);
	pictdesc.picType = PICTYPE_BITMAP;
	pictdesc.bmp.hbitmap = bitmap;
	pictdesc.bmp.hpal = NULL;
	*retval = NULL;
	OleCreatePictureIndirect(&pictdesc,IID_IPictureDisp,TRUE,(void**)retval);

	return S_OK;
}

STDMETHODIMP CUtils::GenerateHillShade(BSTR  bstrGridFilename, BSTR  bstrShadeFilename, 
						 float z, float scale, float az, float alt, VARIANT_BOOL *retval) 
/*  Purpose:
		Hillshade generates a shaded relief map from any GDAL-supported elevation raster
    Credits:
		This code was swiped from Matt Perry
		perrygeo@gmail.com
		http://www.perrygeo.net
		Published in Gdal-dev Digest, Vol 19, Issue 20
	Usage:
		hillshade input_dem output_hillshade
		[-z ZFactor (default=1)] [-s scale* (default=1)]
		[-az Azimuth (default=315)] [-alt Altitude (default=45)]
	Notes : 
		Scale for Feet:Latlong use scale=370400, for Meters:LatLong use scale=111120
*/

{ 
	USES_CONVERSION;
	const char  *pszGridFilename = OLE2A(bstrGridFilename);
	const char  *pszShadeFilename = OLE2A(bstrShadeFilename);

    GDALDataset *poDataset;  
    const float radiansToDegrees = (float)(180.0 / 3.14159);  
    const float degreesToRadians = (float)(3.14159 / 180.0); 
    double      adfGeoTransf[6];
    float       *win;
    float       *shadeBuf;
	float  	    x;
	float		y;
    float       aspect;
	float		slope;
	float       cang;
    int         i;
    int         j;
    int         n;
    int         containsNull;
	const char *pszFormat = "GTiff";
	bool		retVal = false;
    
	try
	{	
		/* ----------------------------------- 
		* Default Values 
		*/

		if (z == NULL) z = 1.0; 
		if (scale == NULL) scale = 1.0;
		if (az == NULL) az = 315.0;
		if (alt == NULL) alt = 45.0;
		
		/*---------------------------------------
		* Open Dataset and get raster band (assuming it is band #1)
		*/
		
		GDALAllRegister(); 

		poDataset = GdalHelper::OpenDatasetW(OLE2W(bstrGridFilename), GA_ReadOnly);
		if( poDataset == NULL )
		{
			printf( "Couldn't open dataset %s\n", 
					pszGridFilename );
			return retVal;
		}
		GDALRasterBand  *poBnd;       
		poBnd = poDataset->GetRasterBand( 1 );
		poDataset->GetGeoTransform( adfGeoTransf );

		/* -------------------------------------
		* Get variables from input dataset
		*/
		const double   nsres = adfGeoTransf[5];
		const double   ewres = adfGeoTransf[1];	
		const float    nullValue = (float) poBnd->GetNoDataValue( );
		const int      nXSize = poBnd->GetXSize();
		const int      nYSize = poBnd->GetYSize();
		shadeBuf       = (float *) CPLMalloc(sizeof(float)*nXSize);
		win            = (float *) CPLMalloc(sizeof(float)*9);

		/* -----------------------------------------
		* Create the output dataset and copy over relevant metadata
		*/
		GDALDriver *poDriver;
		poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
		GDALDataset      *poShadeDS;    
		GDALRasterBand   *poShadeBand; 
		char **papszOptions = NULL;
		
		printf("XSize %d\n",nXSize);

		poShadeDS = poDriver->Create(pszShadeFilename,nXSize,nYSize,1,GDT_Byte, 
									papszOptions );

		if (poShadeDS == NULL)
			return retVal;
		poShadeDS->SetGeoTransform( adfGeoTransf );    
		poShadeDS->SetProjection( poDataset->GetProjectionRef() );
		poShadeBand = poShadeDS->GetRasterBand(1);
		poShadeBand->SetNoDataValue( poBnd->GetNoDataValue( ) );    
		/* ------------------------------------------
		* Move a 3x3 window over each cell 
		* (where the cell in question is #4)
		*
		*                 0 1 2
		*                 3 4 5
		*                 6 7 8
		*
		*/
		for ( i = 0; i < nYSize; i++) {
			for ( j = 0; j < nXSize; j++) {
				containsNull = 0;

				// Exclude the edges 
				if (i == 0 || j == 0 || i == nYSize-1 || j == nXSize-1 ) 
				{
					// We are at the edge so write nullValue and move on
					shadeBuf[j] = nullValue;
					continue;
				}

				// Read in 3x3 window
				poBnd->RasterIO( GF_Read, j-1, i-1, 3, 3,
							win, 3, 3, GDT_Float32, 
							0, 0 ); 

				// Check if window has null value
				for ( n = 0; n <= 8; n++) {
				if(win[n] == nullValue) {
					containsNull = 1;
					break;
				}
				}

				if (containsNull == 1) {
					// We have nulls so write nullValue and move on
					shadeBuf[j] = nullValue;
					continue;
				} 
				else 
				{
					// We have a valid 3x3 window. 
	         
					/* ---------------------------------------
					* Compute Hillshade
					*/

					// First Slope ...
					x = (float)(((z*win[0] + z*win[3] + z*win[3] + z*win[6]) - 
						(z*win[2] + z*win[5] + z*win[5] + z*win[8])) /
						(8.0 * ewres * scale));

					y = (float)(((z*win[6] + z*win[7] + z*win[7] + z*win[8]) - 
						(z*win[0] + z*win[1] + z*win[1] + z*win[2])) /
						(8.0 * nsres * scale));

					slope = (float)90.0 - atan(sqrt(x*x + y*y))*radiansToDegrees;
					
					// ... then aspect...
					aspect = atan2(x,y);			
					
					// ... then the shade value
					cang = (float)(sin(alt*degreesToRadians) * sin(slope*degreesToRadians) + 
						cos(alt*degreesToRadians) * cos(slope*degreesToRadians) * 
						cos((az-90.0)*degreesToRadians - aspect));

					if (cang <= 0.0) 
						cang = nullValue;
					else
						cang = static_cast<float>(255.0 * cang);

					shadeBuf[j] = cang;
					
				}
			}

			/* -----------------------------------------
			* Write Line to Raster
			*/
			poShadeBand->RasterIO( GF_Write, 0, i, nXSize, 1, 
						shadeBuf, nXSize, 1, GDT_Float32, 0, 0 );   
		}

		delete poShadeDS;
		retVal = true;
	}	
	
	catch(exception e)
	{
		retVal = false;
	}

	if (retVal)
		return S_OK;
	else
		return S_FALSE;

}

/************************************************************************/
/*                           Parse()                                    */
/************************************************************************/
void CUtils::Parse(CString sOrig, int * opts)
{
	if (sOrig.IsEmpty())
	{
		sArr.RemoveAll();
		*opts = 0;
		return;
	}

	CString sTemp, sTrans, sStore;
	int m, length;
	char chSeps[] = " ";

	//set an initial max array size
	sArr.RemoveAll();
	sOrig.TrimRight();
	sArr.Add( "Dummy value at 0" );
	while(1)
	{
		if (sOrig.GetLength() <= 0)
			break;

		sOrig.TrimLeft();

		if (sOrig[0] == '"')
		{
			sOrig.Delete(0);
			m = sOrig.Find("\"", 0);
		}
		else
		{
			m = sOrig.FindOneOf( (LPCTSTR)chSeps );
		}

		if (m == -1)
		{
			sArr.Add( sOrig );
			break;
		}

		sTemp = sOrig.Mid(0, m);
		sArr.Add( sTemp );
		sTrans = sOrig.Mid(m+1, sOrig.GetLength());
		sOrig = sTrans;
	}

	for (int i = 0; i < sArr.GetCount(); i++)
	{
		sTemp = sArr[i];
		length = sTemp.GetLength ();

		if (length < 2 || sTemp[0] != '"' || sTemp[length - 1] != '"')
			continue;

		sArr[i] = (sTemp.Left (length - 1)).Right (length - 2);
	}

	*opts = (int) sArr.GetCount();
}

// ***********************************************************
//	  OGRLayerToShapefile()
// ***********************************************************
STDMETHODIMP CUtils::OGRLayerToShapefile(BSTR Filename, ShpfileType shpType, ICallback *cBack, IShapefile** sf)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	(*sf) = GeometryConverter::Read_OGR_Layer(Filename, shpType);
	if (*sf)
	{
		((CShapefile*)(*sf))->ValidateOutput(sf, "OGRLayerToShapefile", "Utils");
	}
	return S_OK;
}

// ***********************************************************
//	  ClipPolygon
// ***********************************************************
// New implementation, based on GEOS
STDMETHODIMP CUtils::ClipPolygon(PolygonOperation op, IShape* SubjectPolygon, IShape* ClipPolygon, IShape** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = NULL;
	
	if( SubjectPolygon == NULL || ClipPolygon == NULL )
	{	
		this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	ShpfileType shptype;
	SubjectPolygon->get_ShapeType (&shptype);
	if( shptype != SHP_POLYGON && shptype != SHP_POLYGONM && shptype != SHP_POLYGONZ )
	{	
		this->ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}

	ClipPolygon->get_ShapeType(&shptype);
	if( shptype != SHP_POLYGON && shptype != SHP_POLYGONM && shptype != SHP_POLYGONZ )
	{	
		this->ErrorMessage(tkINCOMPATIBLE_SHAPE_TYPE);
		return S_OK;
	}
	
	bool geos = true;

	if (geos)
	{
		OGRGeometry* geomSubject = GeometryConverter::ShapeToGeometry(SubjectPolygon);
		if (geomSubject == NULL) return S_OK;

		OGRGeometry* geomClip = GeometryConverter::ShapeToGeometry(ClipPolygon);
		if (geomClip == NULL)
		{
			OGRGeometryFactory::destroyGeometry(geomSubject);
			return S_OK;
		}
		
		OGRGeometry* oGeom = NULL;
		switch(op)
		{
			case DIFFERENCE_OPERATION:
				oGeom = geomSubject->Difference(geomClip);
				break;
			case INTERSECTION_OPERATION:
				oGeom = geomSubject->Intersection(geomClip);
				break;
			case EXCLUSIVEOR_OPERATION:
				oGeom = geomSubject->SymmetricDifference(geomClip);
				break;
			case UNION_OPERATION:
				oGeom = geomSubject->Union(geomClip);
				break;
		}
		
		OGRGeometryFactory::destroyGeometry(geomClip);
		OGRGeometryFactory::destroyGeometry(geomSubject);

		if (oGeom != NULL)
		{
			SubjectPolygon->get_ShapeType(&shptype);
			
			vector<IShape* > vShapes;
			if (GeometryConverter::GeometryToShapes(oGeom, &vShapes, Utility::ShapeTypeIsM(shptype)))
			{
				*retval = vShapes[0];
				
				ASSERT(vShapes.size() < 2);

				// if there are more than one poly, we should release them
				for (int i = 1; i < (int)vShapes.size(); i++) 
				{
					if (vShapes[i] != NULL)
					{
						vShapes[i]->Release();
					}
				}
			}
			OGRGeometryFactory::destroyGeometry(oGeom);
		}
	}
	else
	{
		IShape* shp = ClipperConverter::ClipPolygon(ClipPolygon, SubjectPolygon, op);
		*retval = shp;
	}
	return S_OK;
}

#pragma region MergeImages
// ******************************************************************
//		MergeImages
// ******************************************************************
STDMETHODIMP CUtils::MergeImages(/*[in]*/SAFEARRAY* InputNames, /*[in]*/BSTR OutputName, /*out,retval*/VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	USES_CONVERSION;
	
	// Check dimensions of the array.
    if (SafeArrayGetDim(InputNames) != 1)
	{
        // most likely this error will be caught while marshalling the array
		// AfxThrowOleDispatchException(1002, "Type Mismatch in Parameter. Pass a one-dimensional array");
		ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return S_OK;
	}

	LONG cElements, lLBound, lUBound;
	HRESULT hr;
	hr = SafeArrayGetLBound(InputNames, 1, &lLBound);
    if (FAILED(hr))
	{
        ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return S_OK;
	}
    
	hr = SafeArrayGetUBound(InputNames, 1, &lUBound);
    if (FAILED(hr))
	{
		ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return S_OK;
	}
	
	// TODO: add check that we have an array of BSTR and not the other data type
	BSTR HUGEP *pbstr;
	hr = SafeArrayAccessData(InputNames, (void HUGEP* FAR*)&pbstr);
    if (FAILED(hr))
	{
		ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return S_OK;
	}
	
	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key), 0, A2BSTR("Loading input images..."));

	VARIANT_BOOL vbretval;
	std::vector<IImage*> images;
	cElements = lUBound-lLBound + 1;
    for (int i = 0; i < cElements; i++)
    {
		//AfxMessageBox(OLE2A(s));
		IImage* img = NULL;
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&img);
		img->Open(pbstr[i], USE_FILE_EXTENSION, VARIANT_FALSE, NULL, &vbretval);
		
		if (vbretval)
		{
			images.push_back(img);
		}
		else
		{
			img->Release();
			img = NULL;
		}
	}
	
	LONG width, height;
	if (images.size() <= 1)
	{
		ErrorMessage(tkAT_LEAST_TWO_DATASOURCES_EXPECTED);
		goto Cleaning;
	}
	else
	{
		// all the images must have the same size
		LONG w, h;
		for (unsigned int i = 0; i < images.size(); i++ )
		{
			images[i]->get_OriginalWidth(&w);
			images[i]->get_OriginalHeight(&h);

			if (i == 0)
			{
				width = w;
				height = h;
			}
			else if ( w != width || h != height)
			{
				ErrorMessage(tkIMAGES_MUST_HAVE_THE_SAME_SIZE);
				goto Cleaning;
			}
		}
	}

	// saving and copying the pixels
	colour* pixels = new colour[width * height];
	for (unsigned int i = 0; i < images.size(); i++ )
	{
		CString s; BSTR name;
		images[i]->get_Filename(&name);
		s.Format("Processing image %d: %s", i + 1, OLE2A(name));
		int percent = (int)((double)i/(double)(images.size() - 1)*100.0);
		if( globalCallback != NULL )
			globalCallback->Progress(OLE2BSTR(key), percent, A2BSTR(s));
		
		CImageClass* img  = (CImageClass*)images[i];
		img->SaveNotNullPixels(true);
		
		DataPixels* source = img->m_pixels;
		for (int j = 0; j < img->m_pixelsCount; j++)
		{
			pixels[(source + j)->position] = (source + j)->value;
		}

		img->ClearNotNullPixels();
	}
	
	// saving the results
	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key), 0, A2BSTR("Saving result..."));
	
	unsigned char* bits = reinterpret_cast<unsigned char*>(pixels);
	Utility::SaveBitmap(width, height, bits, OutputName);

Cleaning:
	// cleaning
	if( globalCallback != NULL )
		globalCallback->Progress(OLE2BSTR(key), 0, A2BSTR(""));

	for (unsigned int i = 0; i < images.size(); i++)
	{
		images[i]->Release();
	}
	
	if (pixels) 
	{
		delete[] pixels;
		pixels = NULL;
	}
	return S_OK;
}



// ***********************************************************
//		GetEncoderClsid()
// ***********************************************************
// Returns encoder for the specified image format
// The following call should be used for PNG fromat, for example: GetEncoderClsid(L"png", &pngClsid);
//int CUtils::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
//{
//   UINT  num = 0;          // number of image encoders
//   UINT  size = 0;         // size of the image encoder array in bytes
//	
//   
//   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
//
//   Gdiplus::GetImageEncodersSize(&num, &size);
//   if(size == 0)
//      return -1;  // Failure
//
//   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
//   if(pImageCodecInfo == NULL)
//      return -1;  // Failure
//
//   GetImageEncoders(num, size, pImageCodecInfo);
//
//   for(UINT j = 0; j < num; ++j)
//   {
//      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
//      {
//         *pClsid = pImageCodecInfo[j].Clsid;
//         free(pImageCodecInfo);
//         return j;  // Success
//      }    
//   }
//
//   free(pImageCodecInfo);
//   return -1;  // Failure
//}
#pragma endregion

// ***********************************************************
//		ReprojectShapefile()
// ***********************************************************
STDMETHODIMP CUtils::ReprojectShapefile(IShapefile* sf, IGeoProjection* source, IGeoProjection* target, IShapefile** result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// ------------------------------------------------
	//	Validation
	// ------------------------------------------------
	if (!sf || !source || !target)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		*result = NULL;
		return S_FALSE;
	}

	OGRSpatialReference* ref1 = ((CGeoProjection*)source)->get_SpatialReference();
	OGRSpatialReference* ref2 = ((CGeoProjection*)target)->get_SpatialReference();

	OGRCoordinateTransformation* transf = OGRCreateCoordinateTransformation( ref1, ref2 );
	if (!transf)
	{
		ErrorMessage(tkFAILED_TO_REPROJECT);
		*result = NULL;
		return S_FALSE;
	}

	if(!((CShapefile*)sf)->ValidateInput(sf, "ReprojectShapefile", "sf", VARIANT_FALSE, "Utils"))
		return S_FALSE;

	// ------------------------------------------------
	//	Creating output
	// ------------------------------------------------
	sf->Clone(result);

	// ------------------------------------------------
	//	Processing
	// ------------------------------------------------
	VARIANT_BOOL vbretval;
	CComVariant var;
	long numShapes, count = 0;
	sf->get_NumShapes(&numShapes);

	long numFields, percent = 0;
	sf->get_NumFields(&numFields);
	
	for (long i = 0; i < numShapes; i++)
	{
		Utility::DisplayProgress(globalCallback, i, numShapes, "Reprojecting...", key, percent);
		
		IShape* shp = NULL;
		sf->get_Shape(i, &shp);
		
		IShape* shpNew = NULL;
		shp->Clone(&shpNew);
		
		if (shpNew)
		{
			long numPoints;
			shpNew->get_NumPoints(&numPoints);
			
			double x, y;
			for (long j = 0; j < numPoints; j++)
			{
				shpNew->get_XY(j, &x, &y, &vbretval);

				// will work faster after embedding to the CShape class
				BOOL res = transf->Transform( 1, &x, &y);
				if (res)	
				{
					shpNew->put_XY(j, x, y, &vbretval);
				}
				else
				{
					// if there is at least one failed point, reprojection will be interrupted
					shpNew->Release();	
					(*result)->Release();
					(*result) = NULL;
					ErrorMessage(tkFAILED_TO_REPROJECT);
					return S_FALSE;
				}
			}

			(*result)->get_NumShapes(&count);
			(*result)->EditInsertShape(shpNew, &count, &vbretval);

			// copying attributes
			for (long j = 0; j < numFields; j++)
			{
				sf->get_CellValue(j, i, &var);
				(*result)->EditCellValue(j, i, var, &vbretval);
			}
		}
	}

	// --------------------------------------------------
	//    Validating output
	// --------------------------------------------------
	Utility::DisplayProgressCompleted(globalCallback, key);
	((CShapefile*)sf)->ClearValidationList();
	((CShapefile*)sf)->ValidateOutput(&sf, "ReprojectShapefile", "Utils");
	
	return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CUtils::ErrorMessage(long ErrorCode)
{
	lastErrorCode = ErrorCode;
	if( globalCallback != NULL) 
		globalCallback->Error(OLE2BSTR(key),A2BSTR(ErrorMsg(lastErrorCode)));
	return;
}

STDMETHODIMP CUtils::ColorByName(tkMapColor name, OLE_COLOR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = BGR_TO_RGB(name);
	return S_OK;
}

// **************************************************************
//		ConvertDistance()
// **************************************************************
STDMETHODIMP CUtils::ConvertDistance(tkUnitsOfMeasure sourceUnit, tkUnitsOfMeasure targetUnit, DOUBLE* value, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = Utility::ConvertDistance(sourceUnit, targetUnit, *value);
	return S_OK;
}

// **************************************************************
//		ClipGridWithPolygon()
// **************************************************************
STDMETHODIMP CUtils::ClipGridWithPolygon(BSTR inputGridfile, IShape* poly, BSTR resultGridfile, VARIANT_BOOL keepExtents, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	*retVal = VARIANT_FALSE;

	if (!Utility::fileExistsUnicode(inputGridfile))
	{
		ErrorMessage(tkINVALID_FILENAME);
		return S_FALSE;
	}

	IGrid* grid = NULL;
	CoCreateInstance(CLSID_Grid,NULL,CLSCTX_INPROC_SERVER,IID_IGrid,(void**)&grid);
	
	if (grid) 
	{
		bool inRam = false;
		VARIANT_BOOL vb;
		grid->Open(inputGridfile, GridDataType::UnknownDataType, inRam, GridFileType::UseExtension, NULL, &vb);
		if (vb) {
			this->ClipGridWithPolygon2(grid, poly, resultGridfile, keepExtents, retVal);
			grid->Close(&vb);
		}
		grid->Release();
	}
	return S_OK;
}

// ********************************************************
//		ClipGridWithPolygon2()
// ********************************************************
STDMETHODIMP CUtils::ClipGridWithPolygon2(IGrid* grid, IShape* poly, BSTR resultGridfile, VARIANT_BOOL keepExtents, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!poly || !grid) {
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}

	if (Utility::fileExistsUnicode(resultGridfile))
	{
		ErrorMessage(tkFILE_EXISTS);
		return S_FALSE;
	}
	
	VARIANT_BOOL vb;

	// find cell bounds
	IExtents* ext1 = NULL;
	poly->get_Extents(&ext1);
	
	IExtents* ext2 = NULL;
	((CGrid*)grid)->get_Extents(&ext2);
	
	IExtents* bounds = NULL;
	((CExtents*)ext1)->GetIntersection(ext2, &bounds);
	
	ext1->Release();
	ext2->Release();

	if (bounds) {
		double xMin, yMin, zMin, xMax, yMax, zMax;
		bounds->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
		bounds->Release();
		
		long firstCol, firstRow, lastCol, lastRow;
		grid->ProjToCell(xMin, yMin, &firstCol, &firstRow);
		grid->ProjToCell(xMax, yMax, &lastCol, &lastRow);
		
		IGrid* newGrid = NULL;

		if (!keepExtents) {
			newGrid = ((CGrid*)grid)->Clip(resultGridfile, firstCol, lastCol, firstRow, lastRow);
		}
		else {
			newGrid = ((CGrid*)grid)->Clone(resultGridfile);
		}

		// copy data
		if (newGrid)
		{
			double dx, dy, xll, yll;
			IGridHeader* header = NULL;
			newGrid->get_Header(&header);
			header->get_dX(&dx);
			header->get_dY(&dy);
			header->get_XllCenter(&xll);
			header->get_YllCenter(&yll);
			
			CComVariant var;
			header->get_NodataValue(&var);
			double noData;
			dVal(var, noData);

			long minRow = MIN(firstRow, lastRow);
			long maxRow = MAX(firstRow, lastRow);
			
			int cmnCount = lastCol - firstCol + 1;
			int rowCount = maxRow - minRow + 1;

			if (keepExtents)
			{
				long rowCount;
				header->get_NumberRows(&rowCount);
				
				xll += dx * firstCol;
				yll += dy * (rowCount - maxRow -1);
			}
			header->Release();

			if (cmnCount > 0 && rowCount > 0) 
			{
				double* vals = new double[cmnCount];
				long row = 0;

				CPointInPolygon pip;
				if (pip.SetPolygon(poly))
				{
					for (long i = minRow; i <= maxRow; i++ ) {
						grid->GetFloatWindow2(i, i, firstCol, lastCol, vals, &vb);
						
						if (vb) {
							
							//double y = yll + dy * (rowCount - row - 1.5);	// (rowCount - 1) - row;  -0.5 - center of cell
							double y = yll + dy * (rowCount - row - 1);   // a fix suggested here: http://bugs.mapwindow.org/view.php?id=2349

							pip.PrepareScanLine(y);
							
							// set values outside polygon to nodata
							for (long j = 0; j < cmnCount; j++)
							{
								// double x = xll + dx * (j + 0.5);
								double x = xll + dx * j;		// a fix suggested here: http://bugs.mapwindow.org/view.php?id=2349
								
								if (!pip.ScanPoint(x))
								{
									vals[j] = noData;
								}
							}
							
							if (keepExtents) {
								newGrid->PutFloatWindow2(i, i, firstCol, lastCol, vals, &vb);
								row++;
							}
							else {
								newGrid->PutRow2(row++, vals, &vb);
							}
						}
					}
				}
				delete[] vals;
			}

			newGrid->Save(resultGridfile, GridFileType::UseExtension, NULL, &vb);
			newGrid->Close(&vb);
			newGrid->Release();
			*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// ********************************************************
//		CreateStatisticsFields()
// ********************************************************
void CreateStatisticsFields(IShapefile* sf, std::vector<long>& resultIndices, bool overwrite) {
	ITable* tbl = NULL;
	sf->get_Table(&tbl);
	
	VARIANT_BOOL vb;
	long index = -1; 
	CString fields[13] = {"Mean", "Median", "Majority", "Minority", "Minimum", "Maximum", "Range", "StD", "Sum", "MinX", "MinY", "Variety", "Count" };

	if (overwrite) {
		for (int i = 0; i < 13; i++) {
			tbl->get_FieldIndexByName(A2BSTR(fields[i]), &index);
			if (index != -1) {
				tbl->EditDeleteField(index, NULL, &vb);
			}
		}
	}
	
	for (int i = 0; i < 11; i++) {
		sf->EditAddField(A2BSTR(fields[i]), FieldType::DOUBLE_FIELD, 12, 18, &index);
		resultIndices.push_back(index);
	}
	
	for (int i = 11; i < 13; i++) {
		sf->EditAddField(A2BSTR(fields[i]), FieldType::INTEGER_FIELD, 0, 18, &index);
		resultIndices.push_back(index);
	}
	
	((CTableClass*)tbl)->MakeUniqueFieldNames();
	//((CShapefile*)sf)->MakeUniqueFieldNames();
	tbl->Release();
}

enum GridScanMethod
{
	CenterWithin = 0,		// center of grid cell must be within polygon to pass the data; fast but inadequate for small polygons
	Intersection = 1,		// bounds of polygon and cell must intersect to pass the data; all the values are weighted according
							// to the area of intersection; slower, should be used for small polygons
};

// ********************************************************
//		GridStatsToForPoly()
// ********************************************************
bool GridStatsForPoly(IGrid* grid, IGridHeader* header, IShape* poly, Extent& bounds,
					  float noData, double& meanValue, double& minValue, double& maxValue)
{
	VARIANT_BOOL vb;
	long firstCol, firstRow, lastCol, lastRow;
	grid->ProjToCell(bounds.left, bounds.bottom, &firstCol, &firstRow);
	grid->ProjToCell(bounds.right, bounds.top, &lastCol, &lastRow);

	long minRow = MIN(firstRow, lastRow);
	long maxRow = MAX(firstRow, lastRow);

	int cmnCount = lastCol - firstCol + 1;
	int rowCount = maxRow - minRow + 1;

	double dx, dy, xll, yll;
	header->get_dX(&dx);
	header->get_dY(&dy);
	header->get_XllCenter(&xll);
	header->get_YllCenter(&yll);

	long gridRowCount;
	header->get_NumberRows(&gridRowCount);

	double yllWindow = yll + ((gridRowCount - 1) - maxRow) * dy;
	double xllWindow = xll + firstCol * dx;

	if (cmnCount > 0 && rowCount > 0) 
	{
		// determine rows & cols which correspond to a poly
		float* vals = new float[cmnCount];
		int row = 0;
		float sum = 0.0;
		float max = -FLT_MAX;
		float min = FLT_MAX;
		float count = 0.0f;
		
		std::map<float, int> values;	// value; count

		GridScanMethod method = cmnCount * rowCount > 10 ? GridScanMethod::CenterWithin : GridScanMethod::Intersection;

		if (method == GridScanMethod::CenterWithin)
		{
			CPointInPolygon pip;
			if (pip.SetPolygon(poly))
			{
				for (long i = minRow; i <= maxRow; i++ ) 
				{
					grid->GetFloatWindow(i, i, firstCol, lastCol, vals, &vb);
					
					if (vb) {
						double y = yllWindow + dy * (rowCount - row - 1.5);	  // (rowCount - 1) - row;  -0.5 - center of cell
						pip.PrepareScanLine(y);
						
						// checking values within row
						for (long j = 0; j < cmnCount; j++)
						{
							if (vals[j] == noData)
								continue;

							double x = xllWindow + dx * (j + 0.5);
							if (pip.ScanPoint(x))
							{
								if (vals[j] < min) {
									min = vals[j];
								}
								if (vals[j] > max)
								{
									max = vals[j];
								}
								sum += vals[j];
								count += 1.0f;
							}
						}
					}
					row++;
				}
			}
			
			// it may be a rare case of narrow shapes, like roads
			// let's try to to use intersection logic, as we determined 
			// that there must be an intersection
			if (count == 0.0f)
			{
				row = 0;
				goto intersection;
			}
		}
		else   //GridScanMethod == Intersection
		{
intersection:			
			Extent cell;
			Extent intersection;
			double cellArea = dx * dy;

			for (long i = minRow; i <= maxRow; i++ ) 
			{
				grid->GetFloatWindow(i, i, firstCol, lastCol, vals, &vb);
				if (vb) {
					cell.bottom = yllWindow + dy * (rowCount - row - 1.5);
					cell.top = yllWindow + dy * (rowCount - row - 0.5);
					for (long j = 0; j < cmnCount; j++)
					{
						if (vals[j] == noData)
							continue;

						cell.left = xllWindow + dx * (j - 0.5);
						cell.right = xllWindow + dx * (j + 0.5);
						if (cell.getIntersection(bounds, intersection))
						{
							float weight = (float)(intersection.getArea()/cellArea);  // the part of cell within polygon bounds

							if (vals[j] < min) {
								min = vals[j];
							}
							if (vals[j] > max) 
							{
								max = vals[j];
							}
							sum += vals[j] * weight;
							count += weight;
						}
					}
				}
				row++;
			}
		}
		delete[] vals;

		if (count > 0)
		{
			meanValue = sum/count;
			minValue = min;
			maxValue = max;
			return true;
		}
	}
	return false;
}

// ********************************************************
//		GridStatisticsToForPolygon()
// ********************************************************
STDMETHODIMP CUtils::GridStatisticsForPolygon(IGrid* grid, IGridHeader* header, IExtents* gridExtents, IShape* shape, 
					 double noDataValue, double* meanValue, double* minValue, double* maxValue, VARIANT_BOOL* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	*retVal = VARIANT_FALSE;
	if (!grid || !header || !shape) {
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}

	ShpfileType type;
	shape->get_ShapeType(&type);
	if (type != SHP_POLYGON && type != SHP_POLYGONM && type != SHP_POLYGONZ) {
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return S_FALSE;
	}

	double xMin, xMax, yMin, yMax, zMin, zMax;
	gridExtents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
	Extent gridBounds(xMin, xMax, yMin, yMax);

	((CShape*)shape)->get_ExtentsXY(xMin, yMin, xMax, yMax);
	Extent shapeBounds(xMin, xMax, yMin, yMax);

	Extent bounds;
	if (gridBounds.getIntersection(shapeBounds, bounds))
	{
		bool result = GridStatsForPoly(grid, header, shape, bounds,(float)noDataValue, *meanValue, *minValue, *maxValue);
		*retVal = result ? VARIANT_TRUE: VARIANT_FALSE;
	}
	return S_OK;
}

// ********************************************************
//		GridStatisticsToShapefile()
// ********************************************************
STDMETHODIMP CUtils::GridStatisticsToShapefile(IGrid* grid,  IShapefile* sf, VARIANT_BOOL selectedOnly, VARIANT_BOOL overwriteFields, VARIANT_BOOL* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	if (!grid || !sf) {
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_FALSE;
	}
	
	ShpfileType type;
	sf->get_ShapefileType(&type);
	if (type != SHP_POLYGON && type != SHP_POLYGONM && type != SHP_POLYGONZ) {
		ErrorMessage(tkUNEXPECTED_SHAPE_TYPE);
		return S_FALSE;
	}

	IExtents* extSf = NULL;
	sf->get_Extents(&extSf);

	IExtents* extGrid = NULL;
	((CGrid*)grid)->get_Extents(&extGrid);
	
	VARIANT_BOOL vb;
	((CExtents*)extGrid)->Intersects(extSf, &vb);
	extSf->Release();
	
	if (!vb) {
		ErrorMessage(tkBOUNDS_NOT_INTERSECT);
		return S_FALSE;
	}
	else {
		VARIANT_BOOL editing;
		sf->get_EditingTable(&editing);
		
		if (!editing) {
			sf->StartEditingTable(this->globalCallback, &vb);	// it's safe enough not to check the result
		}

		std::vector<long> fieldIndices;
		CreateStatisticsFields(sf, fieldIndices, overwriteFields ? true: false);
		
		double dx, dy, xll, yll;
		IGridHeader* header = NULL;
		grid->get_Header(&header);
		header->get_dX(&dx);
		header->get_dY(&dy);
		header->get_XllCenter(&xll);
		header->get_YllCenter(&yll);

		long gridRowCount;
		header->get_NumberRows(&gridRowCount);

		// no data values - must not be used in calculations
		CComVariant var;
		header->get_NodataValue(&var);
		float noData;
		fVal(var, noData);

		header->Release();

		long percent = -1;
		long numShapes;
		sf->get_NumShapes(&numShapes);

		for (long n = 0; n < numShapes; n++) 
		{
			if( globalCallback != NULL )
			{
				long newpercent = (long)(((double)(n + 1)/numShapes)*100);
				if( newpercent > percent )
				{	
					percent = newpercent;
					globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Calculating..."));
				}
			}
			
			sf->get_ShapeSelected(n, &vb);
			if (selectedOnly && !vb)
				continue;
			
			IShape* poly = NULL;
			sf->get_Shape(n, &poly);
			
			IExtents* bounds = NULL;
			poly->get_Extents(&extSf);
			((CExtents*)extGrid)->GetIntersection(extSf, &bounds);
			extSf->Release();

			if (bounds != NULL) 
			{
				// determine rows & cols which correspond to a poly
				double xMin, yMin, zMin, xMax, yMax, zMax;
				bounds->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
				bounds->Release();	

				long firstCol, firstRow, lastCol, lastRow;
				grid->ProjToCell(xMin, yMin, &firstCol, &firstRow);
				grid->ProjToCell(xMax, yMax, &lastCol, &lastRow);
				
				long minRow = MIN(firstRow, lastRow);
				long maxRow = MAX(firstRow, lastRow);

				int cmnCount = lastCol - firstCol + 1;
				int rowCount = maxRow - minRow + 1;

				// now find the pixels inside poly and calculate stats
				if (cmnCount > 0 && rowCount > 0) 
				{
					float* vals = new float[cmnCount];
					int row = 0;
					
					int count = 0;
					float sumWeight = 0.0f;	// a sum of cell parts which fall within polygon
					float sum = 0.0;
					float squares = 0.0;
					float max = FLT_MIN;
					float min = -FLT_MAX;
					double minX = 0.0;
					double minY = 0.0;

					double yllWindow = yll + ((gridRowCount - 1) - maxRow) * dy;
					double xllWindow = xll + firstCol * dx;
					std::map<float, int> values;	// value; count

					GridScanMethod method = cmnCount * rowCount > 10 ? GridScanMethod::CenterWithin : GridScanMethod::Intersection;

					if (method == GridScanMethod::CenterWithin)
					{
						CPointInPolygon pip;
						if (pip.SetPolygon(poly))
						{
							for (long i = minRow; i <= maxRow; i++ ) 
							{
								grid->GetFloatWindow(i, i, firstCol, lastCol, vals, &vb);
								
								if (vb) {
									double y = yllWindow + dy * (rowCount - row - 1.5);	  // (rowCount - 1) - row;  -0.5 - center of cell
									pip.PrepareScanLine(y);
									
									// checking values within row
									for (long j = 0; j < cmnCount; j++)
									{
										if (vals[j] == noData)
											continue;

										double x = xllWindow + dx * (j + 0.5);
										if (pip.ScanPoint(x))
										{
											if (vals[j] < min) {
												min = vals[j];
												minX = x;
												minY = y;
											}
											if (vals[j] > max) max = vals[j];
											sum += vals[j];
											squares += vals[j] * vals[j];
											count++;
											
											// counting unique values
											if (values.find(vals[j]) == values.end()) 
												values[vals[j]] = 1;
											else 
												values[vals[j]] += 1;
										}
									}
								}
								row++;
							}
						}
						sumWeight = (float)count;
					}
					else   //GridScanMethod == Intersection
					{
						Extent shape(xMin, xMax, yMin, yMax);
						Extent cell;
						Extent intersection;
						double cellArea = dx * dy;

						for (long i = minRow; i <= maxRow; i++ ) 
						{
							grid->GetFloatWindow(i, i, firstCol, lastCol, vals, &vb);
							if (vb) {
								cell.bottom = yllWindow + dy * (rowCount - row - 1.5);
								cell.top = yllWindow + dy * (rowCount - row - 0.5);
								for (long j = 0; j < cmnCount; j++)
								{
									if (vals[j] == noData)
										continue;

									cell.left = xllWindow + dx * (j - 0.5);
									cell.right = xllWindow + dx * (j + 0.5);
									if (cell.getIntersection(shape, intersection))
									{
										float weight = (float)(intersection.getArea()/cellArea);  // the part of cell within polygon bounds

										if (vals[j] < min) {
											min = vals[j];
											minX = (cell.right + cell.left)/2;
											minY = (cell.top + cell.bottom)/2;
										}
										if (vals[j] > max) max = vals[j];
										sum += vals[j] * weight;
										squares += vals[j] * vals[j] * weight;
										sumWeight += weight;
										count++;
										
										// counting unique values
										if (values.find(vals[j]) == values.end()) 
											values[vals[j]] = 1;
										else 
											values[vals[j]] += 1;
									}
								}
							}
							row++;
						}
					}
						
					if (count > 0) 
					{
						// 0 - "Mean"
						CComVariant vMean(sum/sumWeight);
						sf->EditCellValue(fieldIndices[0], n, vMean, &vb);

						if (!values.empty()) 
						{
							// 1 - "Median"
							int half = count/2;
							int subCount = 0;
							std::map<float, int>::iterator it = values.begin();
							while (it != values.end()) 
							{
								subCount += it->second;
								if (subCount > half)
								{
									CComVariant vMedian(it->first);
									sf->EditCellValue(fieldIndices[1], n, vMedian, &vb);
									break;
								}
								++it;
							}
							
							int maxCount = INT_MIN;
							int minCount = INT_MAX;
							float major, minor;
							it = values.begin();
							while (it != values.end()) 
							{
								if (it->second > maxCount) {
									maxCount = it->second;
									major = it->first;
								}
								if (it->second < minCount) 
								{
									minCount = it->second;
									minor = it->first;
								}
								++it;
							}

							// 2 - "Majority"
							CComVariant vMajor(major);
							sf->EditCellValue(fieldIndices[2], n, vMajor, &vb);
							
							// 3 - "Minority"
							CComVariant vMinor(minor);
							sf->EditCellValue(fieldIndices[3], n, vMinor, &vb);
						}

						// 4 - "Minimum"
						CComVariant vMin(min);
						sf->EditCellValue(fieldIndices[4], n, vMin, &vb);

						// 5 - "Maximum"
						CComVariant vMax(max);
						sf->EditCellValue(fieldIndices[5], n, vMax, &vb);

						// 6 - "Range"
						CComVariant vRange(max - min);
						sf->EditCellValue(fieldIndices[6], n, vRange, &vb);

						// 7 - "StD"
						float stddev = sqrt(squares/sumWeight - pow(sum/sumWeight, 2));		// /count
						CComVariant vStd(stddev);
						sf->EditCellValue(fieldIndices[7], n, vStd, &vb);

						// 8 - "Sum"
						CComVariant vSum(sum);
						sf->EditCellValue(fieldIndices[8], n, vSum, &vb);

						// 9 - "MinX"
						CComVariant vMinX(minX);
						sf->EditCellValue(fieldIndices[9], n, vMinX, &vb);

						// 10 - "MinY"
						CComVariant vMinY(minY);
						sf->EditCellValue(fieldIndices[10], n, vMinY, &vb);

						// 11 - "Variety"
						int variety = values.size();
						CComVariant vVar(variety);
						sf->EditCellValue(fieldIndices[11], n, vVar, &vb);

						// 12 - "Count"
						CComVariant vCount(count);
						sf->EditCellValue(fieldIndices[12], n, vCount, &vb);
					}
					delete[] vals;
				}
			}
			poly->Release();
		}
		extGrid->Release();

		if (!editing) {
			sf->StopEditingTable(VARIANT_TRUE, this->globalCallback, &vb);
			if (!vb) {
				long code;
				sf->get_LastErrorCode(&code);
				this->ErrorMessage(code);	
				return S_FALSE;
			}
		}
		
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// ******************************************************
//	      CreateInstance
// ******************************************************
// Creates a new instance of the specified interface
// Can be useful for scripting clients + creating objects in the background thread
// though penalties for marshalling between apartments can significant
STDMETHODIMP CUtils::CreateInstance(tkInterface interfaceId, IDispatch** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	void* val = NULL;
	switch (interfaceId)
	{
		case tkInterface::idChart:
			CoCreateInstance( CLSID_Chart, NULL, CLSCTX_INPROC_SERVER, IID_IChart, (void**)&val );
			break;
		case tkInterface::idChartField:
			CoCreateInstance( CLSID_ChartField, NULL, CLSCTX_INPROC_SERVER, IID_IChartField, (void**)&val );
			break;
		case tkInterface::idCharts:
			CoCreateInstance( CLSID_Charts, NULL, CLSCTX_INPROC_SERVER, IID_ICharts, (void**)&val );
			break;
		case tkInterface::idColorScheme:
			CoCreateInstance( CLSID_ColorScheme, NULL, CLSCTX_INPROC_SERVER, IID_IColorScheme, (void**)&val );
			break;
		//case tkInterface::idEditShape:
		//	CoCreateInstance( CLSID_EditShape, NULL, CLSCTX_INPROC_SERVER, IID_IEditShape, (void**)&val );
		//	break;
		case tkInterface::idESRIGridManager:
			CoCreateInstance( CLSID_ESRIGridManager, NULL, CLSCTX_INPROC_SERVER, IID_IESRIGridManager, (void**)&val );
			break;
		case tkInterface::idExtents:
			CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&val );
			break;
		case tkInterface::idField:
			CoCreateInstance( CLSID_Field, NULL, CLSCTX_INPROC_SERVER, IID_IField, (void**)&val );
			break;
		case tkInterface::idFieldStatOperations:
			CoCreateInstance( CLSID_FieldStatOperations, NULL, CLSCTX_INPROC_SERVER, IID_IFieldStatOperations, (void**)&val );
			break;
		case tkInterface::idFileManager:
			CoCreateInstance( CLSID_FileManager, NULL, CLSCTX_INPROC_SERVER, IID_IFileManager, (void**)&val );
			break;
		case tkInterface::idGeoProjection:
			CoCreateInstance( CLSID_GeoProjection, NULL, CLSCTX_INPROC_SERVER, IID_IGeoProjection, (void**)&val );
			break;
		case tkInterface::idGlobalSettings:
			CoCreateInstance( CLSID_GlobalSettings, NULL, CLSCTX_INPROC_SERVER, IID_IGlobalSettings, (void**)&val );
			break;
		case tkInterface::idGrid:
			CoCreateInstance( CLSID_Grid, NULL, CLSCTX_INPROC_SERVER, IID_IGrid, (void**)&val );
			break;
		case tkInterface::idGridColorBreak:
			CoCreateInstance( CLSID_GridColorBreak, NULL, CLSCTX_INPROC_SERVER, IID_IGridColorBreak, (void**)&val );
			break;
		case tkInterface::idGridColorScheme:
			CoCreateInstance( CLSID_GridColorScheme, NULL, CLSCTX_INPROC_SERVER, IID_IGridColorScheme, (void**)&val );
			break;
		case tkInterface::idGridHeader:
			CoCreateInstance( CLSID_GridHeader, NULL, CLSCTX_INPROC_SERVER, IID_IGridHeader, (void**)&val );
			break;
		case tkInterface::idImage:
			CoCreateInstance( CLSID_Image, NULL, CLSCTX_INPROC_SERVER, IID_IImage, (void**)&val );
			break;
		case tkInterface::idLabelCategory:
			CoCreateInstance( CLSID_LabelCategory, NULL, CLSCTX_INPROC_SERVER, IID_ILabelCategory, (void**)&val );
			break;
		case tkInterface::idLabel:
			CoCreateInstance( CLSID_Label, NULL, CLSCTX_INPROC_SERVER, IID_ILabel, (void**)&val );
			break;
		case tkInterface::idLabels:
			CoCreateInstance( CLSID_Labels, NULL, CLSCTX_INPROC_SERVER, IID_ILabels, (void**)&val );
			break;
		case tkInterface::idLinePattern:
			CoCreateInstance( CLSID_LinePattern, NULL, CLSCTX_INPROC_SERVER, IID_ILinePattern, (void**)&val );
			break;
		case tkInterface::idLineSegment:
			CoCreateInstance( CLSID_LineSegment, NULL, CLSCTX_INPROC_SERVER, IID_ILineSegment, (void**)&val );
			break;
		case tkInterface::idMeasuring:
			CoCreateInstance( CLSID_Measuring, NULL, CLSCTX_INPROC_SERVER, IID_IMeasuring, (void**)&val );
			break;
		case tkInterface::idPoint:
			CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&val );
			break;
		case tkInterface::idShape:
			CoCreateInstance( CLSID_Shape, NULL, CLSCTX_INPROC_SERVER, IID_IShape, (void**)&val );
			break;
		case tkInterface::idShapeDrawingOptions:
			CoCreateInstance( CLSID_ShapeDrawingOptions, NULL, CLSCTX_INPROC_SERVER, IID_IShapeDrawingOptions, (void**)&val );
			break;
		case tkInterface::idShapefile:
			CoCreateInstance( CLSID_Shapefile, NULL, CLSCTX_INPROC_SERVER, IID_IShapefile, (void**)&val );
			break;
		case tkInterface::idShapefileCategories:
			CoCreateInstance( CLSID_ShapefileCategories, NULL, CLSCTX_INPROC_SERVER, IID_IShapefileCategories, (void**)&val );
			break;
		case tkInterface::idShapefileCategory:
			CoCreateInstance( CLSID_ShapefileCategory, NULL, CLSCTX_INPROC_SERVER, IID_IShapefileCategory, (void**)&val );
			break;
		case tkInterface::idShapefileColorBreak:
			CoCreateInstance( CLSID_ShapefileColorBreak, NULL, CLSCTX_INPROC_SERVER, IID_IShapefileColorBreak, (void**)&val );
			break;
		case tkInterface::idShapefileColorScheme:
			CoCreateInstance( CLSID_ShapefileColorScheme, NULL, CLSCTX_INPROC_SERVER, IID_IShapefileColorScheme, (void**)&val );
			break;
		case tkInterface::idShapeNetwork:
			CoCreateInstance( CLSID_ShapeNetwork, NULL, CLSCTX_INPROC_SERVER, IID_IShapeNetwork, (void**)&val );
			break;
		case tkInterface::idShapeValidationInfo:
			CoCreateInstance( CLSID_ShapeValidationInfo, NULL, CLSCTX_INPROC_SERVER, IID_IShapeValidationInfo, (void**)&val );
			break;
		case tkInterface::idTable:
			CoCreateInstance( CLSID_Table, NULL, CLSCTX_INPROC_SERVER, IID_ITable, (void**)&val );
			break;
		case tkInterface::idTileProviders:
			CoCreateInstance( CLSID_TileProviders, NULL, CLSCTX_INPROC_SERVER, IID_ITileProviders, (void**)&val );
			break;
		case tkInterface::idTiles:
			CoCreateInstance( CLSID_Tiles, NULL, CLSCTX_INPROC_SERVER, IID_ITiles, (void**)&val );
			break;
		case tkInterface::idTin:
			CoCreateInstance( CLSID_Tin, NULL, CLSCTX_INPROC_SERVER, IID_ITin, (void**)&val );
			break;
		case tkInterface::idUtils:
			CoCreateInstance( CLSID_Utils, NULL, CLSCTX_INPROC_SERVER, IID_IUtils, (void**)&val );
			break;
		case tkInterface::idVector:
			CoCreateInstance( CLSID_Vector, NULL, CLSCTX_INPROC_SERVER, IID_IVector, (void**)&val );
			break;
		
	}

	*retVal = val ? (IDispatch*)val : NULL;
	return S_OK;
}

// ********************************************************
//		GeodesicDistance()
// ********************************************************
#include "..\Processing\GeograpicLib\Geodesic.hpp"
STDMETHODIMP CUtils::GeodesicDistance(double lat1, double lng1, double lat2, double lng2, double* retVal) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
	
	geod.Inverse(lat1, lng1, lat2, lng2, *retVal);
	return S_OK;
}

#include "..\Processing\GeograpicLib\PolygonArea.hpp"
double CalcPolyGeodesicArea(std::vector<Point2D>& points)
{
	GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
	GeographicLib::PolygonArea poly(geod);
	for(size_t i = 0; i < points.size(); i++)
	{
		poly.AddPoint(points[i].y, points[i].x);
	}
	double area = 0.0, perimeter = 0.0;
	unsigned int r = poly.Compute(true, true, perimeter, area);
	return area;
}

// ********************************************************
//     MaskRaster()
// ********************************************************
STDMETHODIMP CUtils::MaskRaster(BSTR filename, BYTE newPerBandValue, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	GDALAllRegister();

	USES_CONVERSION;
	GDALDataset* dataset = GdalHelper::OpenDatasetW(OLE2W(filename), GDALAccess::GA_Update);
	if (!dataset)
	{
		this->ErrorMessage(tkINVALID_FILE);
		return S_FALSE;
	}
	
	GByte* pabyData; 
	*retVal = VARIANT_TRUE;
	int count = dataset->GetRasterCount();

	for(int i = 1; i <= count; i++)
	{
		GDALRasterBand* poBand = dataset->GetRasterBand(i);
		if (poBand->GetRasterDataType() != GDT_Byte)
		{
			this->ErrorMessage(tkNON_SINGLE_BYTE_PER_BAND);
			*retVal = VARIANT_FALSE;
			break;		
		}
		
		unsigned char noData = (unsigned char)poBand->GetNoDataValue();

		int nXBlockSize, nYBlockSize;
		poBand->GetBlockSize(&nXBlockSize, &nYBlockSize);
		
		int nXBlocks = (poBand->GetXSize() + nXBlockSize - 1) / nXBlockSize;
		int nYBlocks = (poBand->GetYSize() + nYBlockSize - 1) / nYBlockSize;
		pabyData = (GByte *) CPLMalloc(nXBlockSize * nYBlockSize);
		
		CPLErr err = CPLErr::CE_None; 
		long percent = 0;
		for( int iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
		{
			for( int iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
			{
				if( globalCallback != NULL )
				{
					double count = iYBlock * nXBlocks + iXBlock;
					long newpercent = (long)((double)count/(double)(nXBlocks * nYBlocks)*100.0);
					if( newpercent > percent )
					{	
						percent = newpercent;
						globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR("Calculating..."));
					}
				}
				
				int nXValid, nYValid;
				err = poBand->ReadBlock( iXBlock, iYBlock, pabyData );
				if (err != CPLErr::CE_None)
				{
					Debug::WriteLine("Error on reading band: %d; %d", iXBlock, iYBlock);
					ErrorMessage(tkFAILED_READ_BLOCK);
					*retVal = VARIANT_FALSE;
					break;
				}
				
				// Compute the portion of the block that is valid partial edge blocks.
				if( (iXBlock+1) * nXBlockSize > poBand->GetXSize() )
				{
					nXValid = poBand->GetXSize() - iXBlock * nXBlockSize;
				}
				else
				{
					nXValid = nXBlockSize;
				}
				if( (iYBlock+1) * nYBlockSize > poBand->GetYSize() )
				{
					nYValid = poBand->GetYSize() - iYBlock * nYBlockSize;
				}
				else
				{
					nYValid = nYBlockSize;
				}
			 
				// loop through valid pixels
				for( int iY = 0; iY < nYValid; iY++ )
				{
					int y = iY * nXBlockSize;
					for( int iX = 0; iX < nXValid; iX++ )
					{
						if (pabyData[iX + y] != noData)
						{
							pabyData[iX + y] = newPerBandValue;
						}
					}
				}
				err = poBand->WriteBlock(iXBlock, iYBlock, pabyData);
				if (err != CPLErr::CE_None)
				{
					Debug::WriteLine("Error on writing band: %d; %d", iXBlock, iYBlock);
					ErrorMessage(tkFAILED_WRITE_BLOCK);
					*retVal = VARIANT_FALSE;
					break;
				}
			}
			if(err != CPLErr::CE_None)
				break;
		}
		CPLFree(pabyData);

		if(err != CPLErr::CE_None)
			break;
	}
	
	if (dataset)
	{
		delete dataset;
	}

	if( globalCallback != NULL )
	{
		globalCallback->Progress(OLE2BSTR(key),100,A2BSTR(""));
	}
	return S_OK;
}

#pragma region "Gdal error handler"
ICallback* gdalCallback;

void CPL_STDCALL GdalErrorHandler(CPLErr eErrClass, int err_no, const char *msg)
{
	if (gdalCallback) {
		CString s = msg;
		s = "Gdal error: " + s;
		USES_CONVERSION;
		gdalCallback->Error(A2BSTR("Gdal"), A2BSTR(s));
	}
}

void ClearGdalErrorHandler() 
{
	gdalCallback = NULL;
}

void SetGdalErrorHandler(ICallback* callback) 
{
	gdalCallback = callback;
	CPLSetErrorHandler(&GdalErrorHandler);
}

#pragma endregion

// ********************************************************
//     CopyNodataValues()
// ********************************************************
STDMETHODIMP CUtils::CopyNodataValues(BSTR sourceFilename, BSTR destFilename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;
	GDALAllRegister();

	if (globalCallback) {
		SetGdalErrorHandler(globalCallback);
	}

	USES_CONVERSION;

	GDALDataset* dsSource =  GdalHelper::OpenDatasetW(OLE2W(sourceFilename), GDALAccess::GA_ReadOnly);
	
	GDALDataset* dsDest = GdalHelper::OpenDatasetW(OLE2W(destFilename), GDALAccess::GA_ReadOnly); 

	if (!dsSource || !dsDest)
	{
		this->ErrorMessage(tkINVALID_FILE);
	}
	else
	{
		if(dsSource->GetRasterCount() != dsDest->GetRasterCount() ||
			dsSource->GetRasterXSize() != dsDest->GetRasterXSize() ||
			dsSource->GetRasterYSize() != dsDest->GetRasterYSize()) {
			this->ErrorMessage(tkINPUT_RASTERS_DIFFER);
		}
		else
		{
			GByte* pabyDataSource, *pabyDataDest; 
			*retVal = VARIANT_TRUE;
			int count = dsSource->GetRasterCount();

			for(int i = 1; i <= count; i++)
			{
				GDALRasterBand* bandSource = dsSource->GetRasterBand(i);
				GDALRasterBand* bandDest = dsDest->GetRasterBand(i);
				if (bandSource->GetRasterDataType() != GDT_Byte ||
					bandDest->GetRasterDataType() != GDT_Byte)
				{
					this->ErrorMessage(tkNON_SINGLE_BYTE_PER_BAND);
					*retVal = VARIANT_FALSE;
					break;		
				}
				
				int nXBlockSize, nYBlockSize;
				int nXBlockSize2, nYBlockSize2;
				bandSource->GetBlockSize(&nXBlockSize, &nYBlockSize);
				bandDest->GetBlockSize(&nXBlockSize2, &nYBlockSize2);

				if (nXBlockSize != nXBlockSize2 || nYBlockSize != nYBlockSize2) {
					this->ErrorMessage(tkINPUT_RASTERS_DIFFER);
					*retVal = VARIANT_FALSE;
					break;		
				}

				unsigned char noData = (unsigned char)bandSource->GetNoDataValue();
				
				int nXBlocks = (bandSource->GetXSize() + nXBlockSize - 1) / nXBlockSize;
				int nYBlocks = (bandSource->GetYSize() + nYBlockSize - 1) / nYBlockSize;
				pabyDataSource = (GByte *) CPLMalloc(nXBlockSize * nYBlockSize);
				pabyDataDest = (GByte *) CPLMalloc(nXBlockSize * nYBlockSize);
				
				CPLErr err = CPLErr::CE_None; 
				long percent = 0;
				for( int iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
				{
					for( int iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
					{
						if( globalCallback != NULL )
						{
							double count = iYBlock * nXBlocks + iXBlock;
							long newpercent = (long)((double)count/(double)(nXBlocks * nYBlocks)*100.0);
							if( newpercent > percent )
							{	
								percent = newpercent;
								CString s;
								s.Format("Calculating band %d", i);
								globalCallback->Progress(OLE2BSTR(key),percent,A2BSTR(s));
							}
						}
						
						int nXValid, nYValid;
						CPLErr err1 = bandSource->ReadBlock( iXBlock, iYBlock, pabyDataSource );
						CPLErr err2 = bandDest->ReadBlock( iXBlock, iYBlock, pabyDataDest );
						if (err1 != CPLErr::CE_None || err2 != CPLErr::CE_None)
						{
							Debug::WriteLine("Error on reading band: %d; %d", iXBlock, iYBlock);
							ErrorMessage(tkFAILED_READ_BLOCK);
							*retVal = VARIANT_FALSE;
							break;
						}
						
						// Compute the portion of the block that is valid partial edge blocks.
						if( (iXBlock+1) * nXBlockSize > bandSource->GetXSize() )
						{
							nXValid = bandSource->GetXSize() - iXBlock * nXBlockSize;
						}
						else
						{
							nXValid = nXBlockSize;
						}
						if( (iYBlock+1) * nYBlockSize > bandSource->GetYSize() )
						{
							nYValid = bandSource->GetYSize() - iYBlock * nYBlockSize;
						}
						else
						{
							nYValid = nYBlockSize;
						}
					 
						// loop through valid pixels
						for( int iY = 0; iY < nYValid; iY++ )
						{
							int y = iY * nXBlockSize;
							for( int iX = 0; iX < nXValid; iX++ )
							{
								if (pabyDataSource[iX + y] == noData)
								{
									pabyDataDest[iX + y] = noData;
								}
							}
						}
						
						err = bandDest->WriteBlock(/*iXBlock*/ -1, iYBlock, pabyDataDest);
						if (err != CPLErr::CE_None)
						{
							Debug::WriteLine("Error on writing band: %d; %d", iXBlock, iYBlock);
							ErrorMessage(tkFAILED_WRITE_BLOCK);
							*retVal = VARIANT_FALSE;
							break;
						}
					}
					if(err != CPLErr::CE_None)
						break;
				}
				CPLFree(pabyDataSource);
				CPLFree(pabyDataDest);

				if(err != CPLErr::CE_None || (*retVal == VARIANT_FALSE))
					break;
			}
		}
	}
	
	if (dsSource)
		delete dsSource;

	if (dsDest)
		delete dsDest;

	if( globalCallback != NULL )
	{
		ClearGdalErrorHandler();
		globalCallback->Progress(OLE2BSTR(key),100,A2BSTR(""));
	}
	return S_OK;
}

// ********************************************************
//     ErrorMsgFromObject()
// ********************************************************
STDMETHODIMP CUtils::ErrorMsgFromObject(IDispatch * comClass, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!comClass)
	{
		ErrorMsg(tkUNEXPECTED_NULL_PARAMETER);
		*retVal = A2BSTR("");
		return S_FALSE;
	}
	long errorCode;

	//IChart* Chart = NULL;
	//IChartField* ChartField = NULL;

	ICharts* Charts = NULL;
	comClass->QueryInterface(IID_ICharts, (void**)(&Charts));
	if (Charts) 
	{
		Charts->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Charts->Release();
	}

	IColorScheme* ColorScheme = NULL;
	comClass->QueryInterface(IID_IColorScheme, (void**)(&ColorScheme));
	if (ColorScheme) 
	{
		ColorScheme->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		ColorScheme->Release();
	}

	IESRIGridManager* ESRIGridManager = NULL;
	comClass->QueryInterface(IID_IESRIGridManager, (void**)(&ESRIGridManager));
	if (ESRIGridManager) 
	{
		ESRIGridManager->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		ESRIGridManager->Release();
	}

	//IExtents* Extents = NULL;

	IField* Field = NULL;
	comClass->QueryInterface(IID_IField, (void**)(&Field));
	if (Field) 
	{
		Field->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Field->Release();
	}

	IGeoProjection* GeoProjection = NULL;
	comClass->QueryInterface(IID_IGeoProjection, (void**)(&GeoProjection));
	if (GeoProjection) 
	{
		GeoProjection->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		GeoProjection->Release();
	}

	//IGlobalSettings* GlobalSettings = NULL;
	
	IGrid* Grid = NULL;
	comClass->QueryInterface(IID_IGrid, (void**)(&Grid));
	if (Grid) 
	{
		Grid->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Grid->Release();
	}

	IGridColorBreak* GridColorBreak = NULL;
	comClass->QueryInterface(IID_IGridColorBreak, (void**)(&GridColorBreak));
	if (GridColorBreak) 
	{
		GridColorBreak->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		GridColorBreak->Release();
	}

	IGridColorScheme* GridColorScheme = NULL;
	comClass->QueryInterface(IID_IGridColorScheme, (void**)(&GridColorScheme));
	if (GridColorScheme) 
	{
		GridColorScheme->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		GridColorScheme->Release();
	}

	IGridHeader* GridHeader = NULL;
	comClass->QueryInterface(IID_IGridHeader, (void**)(&GridHeader));
	if (GridHeader) 
	{
		GridHeader->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		GridHeader->Release();
	}

	IImage* Image = NULL;
	comClass->QueryInterface(IID_IImage, (void**)(&Image));
	if (Image) 
	{
		Image->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Image->Release();
	}

	//ILabelCategory* LabelCategory = NULL;

	ILabels* Labels = NULL;
	comClass->QueryInterface(IID_ILabels, (void**)(&Labels));
	if (Labels) 
	{
		Labels->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Labels->Release();
	}

	ILinePattern* LinePattern = NULL;
	comClass->QueryInterface(IID_ILinePattern, (void**)(&LinePattern));
	if (LinePattern) 
	{
		LinePattern->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		LinePattern->Release();
	}

	//ILineSegment* LineSegment = NULL;

	//IMeasuring* Measuring = NULL;

	IPoint* Point = NULL;
	comClass->QueryInterface(IID_IPoint, (void**)(&Point));
	if (Point) 
	{
		Point->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Point->Release();
	}

	IShape* Shape = NULL;
	comClass->QueryInterface(IID_IShape, (void**)(&Shape));
	if (Shape) 
	{
		Shape->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Shape->Release();
	}

	IShapeDrawingOptions* ShapeDrawingOptions = NULL;
	comClass->QueryInterface(IID_IShapeDrawingOptions, (void**)(&ShapeDrawingOptions));
	if (ShapeDrawingOptions) 
	{
		ShapeDrawingOptions->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		ShapeDrawingOptions->Release();
	}

	IShapefile* Shapefile = NULL;
	comClass->QueryInterface(IID_IShapefile, (void**)(&Shapefile));
	if (Shapefile) 
	{
		Shapefile->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Shapefile->Release();
	}

	IShapefileCategories* ShapefileCategories = NULL;
	comClass->QueryInterface(IID_IShapefileCategories, (void**)(&ShapefileCategories));
	if (ShapefileCategories) 
	{
		ShapefileCategories->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		ShapefileCategories->Release();
	}

	//IShapefileCategory* ShapefileCategory = NULL;

	//IShapefileColorBreak* ShapefileColorBreak = NULL;

	IShapefileColorScheme* ShapefileColorScheme = NULL;
	comClass->QueryInterface(IID_IShapefileColorScheme, (void**)(&ShapefileColorScheme));
	if (ShapefileColorScheme) 
	{
		ShapefileColorScheme->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		ShapefileColorScheme->Release();
	}

	IShapeNetwork* ShapeNetwork = NULL;
	comClass->QueryInterface(IID_IShapeNetwork, (void**)(&ShapeNetwork));
	if (ShapeNetwork) 
	{
		ShapeNetwork->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		ShapeNetwork->Release();
	}

	ITable* Table = NULL;
	comClass->QueryInterface(IID_ITable, (void**)(&Table));
	if (Table) 
	{
		Table->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Table->Release();
	}

	ITileProviders* TileProviders = NULL;
	comClass->QueryInterface(IID_ITileProviders, (void**)(&TileProviders));
	if (TileProviders) 
	{
		TileProviders->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		TileProviders->Release();
	}

	ITin* Tin = NULL;
	comClass->QueryInterface(IID_ITin, (void**)(&Tin));
	if (Tin) 
	{
		Tin->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Tin->Release();
	}

	IUtils* Utils = NULL;
	comClass->QueryInterface(IID_IUtils, (void**)(&Utils));
	if (Utils) 
	{
		Utils->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Utils->Release();
	}

	IVector* Vector = NULL;
	comClass->QueryInterface(IID_IVector, (void**)(&Vector));
	if (Vector) 
	{
		Vector->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		Vector->Release();
	}

	IFileManager* FileManager = NULL;
	comClass->QueryInterface(IID_IFileManager, (void**)(&FileManager));
	if (FileManager) 
	{
		FileManager->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		FileManager->Release();
	}

	//IShapeValidationInfo* ShapeValidationInfo = NULL;

	IFieldStatOperations* FieldStatOperations = NULL;
	comClass->QueryInterface(IID_IFieldStatOperations, (void**)(&FieldStatOperations));
	if (FieldStatOperations) 
	{
		FieldStatOperations->get_LastErrorCode(&errorCode);
		get_ErrorMsg(errorCode, retVal);
		FieldStatOperations->Release();
	}

	if (*retVal == A2BSTR(""))
	{
		*retVal = A2BSTR("Unknown class");
		return S_FALSE;
	}

	return S_OK;
}

// ********************************************************
//     TileProjectionToGeoProjectionCore()
// ********************************************************
HRESULT CUtils::TileProjectionToGeoProjectionCore(tkTileProjection projection, VARIANT_BOOL useCache, IGeoProjection** retVal)
{
	*retVal = NULL;

	if (projection != Amersfoort && projection != SphericalMercator)
		return S_FALSE;

	if (_tileProjections[projection] && useCache)
	{
		// let's take it from cache
		_tileProjections[projection]->AddRef();
		*retVal = _tileProjections[projection];
	}
	else
	{
		IGeoProjection* gp = NULL;
		GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&gp);
		VARIANT_BOOL vb;
		switch(projection)
		{
		case tkTileProjection::SphericalMercator:
			gp->SetGoogleMercator(&vb);
			break;
		case tkTileProjection::Amersfoort:
			gp->ImportFromEPSG(EPSG_AMERSFOORT, &vb);
			//gp->ImportFromProj4(A2BSTR("+proj=sterea +lat_0=52.15616055555555 +lon_0=5.38763888888889 +k=0.9999079 +x_0=155000 +y_0=463000 +ellps=bessel +units=m +no_defs"), &vb);
			break;
		}
		_tileProjections[projection] = gp;
		gp->AddRef();
		*retVal = gp;
	}
	return S_OK;
}

// ********************************************************
//     TileProjectionToGeoProjection()
// ********************************************************
STDMETHODIMP CUtils::TileProjectionToGeoProjection(tkTileProjection projection, IGeoProjection** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return TileProjectionToGeoProjectionCore(projection, VARIANT_FALSE, retVal);
}

// ********************************************************
//     ComUsageReport()
// ********************************************************
STDMETHODIMP CUtils::get_ComUsageReport(VARIANT_BOOL unreleasedOnly, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString s = gReferenceCounter.GetReport(unreleasedOnly ? true: false);
	USES_CONVERSION;
	*retVal = A2BSTR(s);
	return S_OK;
}

// ********************************************************
//     ValidateInputNames()
// ********************************************************
// Accesses data in safe array of BSTR
bool CUtils::ValidateInputNames(SAFEARRAY* InputNames, LONG& lLBound, LONG& lUBound, BSTR **pbstr)
{
	// Check dimensions of the array.
	if (SafeArrayGetDim(InputNames) != 1)
	{
		// most likely this error will be caught while marshalling the array
		ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return false;
	}

	HRESULT hr;
	hr = SafeArrayGetLBound(InputNames, 1, &lLBound);
	if (FAILED(hr))
	{
		ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return false;
	}

	hr = SafeArrayGetUBound(InputNames, 1, &lUBound);
	if (FAILED(hr))
	{
		ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return false;
	}

	// TODO: add check that we have an array of BSTR and not the other data type
	hr = SafeArrayAccessData(InputNames, (void HUGEP* FAR*)pbstr);
	if (FAILED(hr))
	{
		ErrorMessage(tkFAILED_TO_READ_INPUT_NAMES);
		return false;
	}
	return true;
}

// ********************************************************
//     OpenOutputDriver()
// ********************************************************
GDALDriverH OpenOutputDriver(CString driverName)
{
	GDALDriverH outputDriver = GDALGetDriverByName( driverName );
	if ( outputDriver )
	{
		char **driverMetadata = GDALGetMetadata( outputDriver, NULL );
		if ( !CSLFetchBoolean( driverMetadata, GDAL_DCAP_CREATE, false ) )
		{
			outputDriver = NULL; //driver exist, but it does not support the create operation
		}
	}
	return outputDriver;
}

// ********************************************************
//     OpenOutputFile()
// ********************************************************
GDALDataset* OpenOutputFile( GDALDriverH outputDriver, CStringW filename, int xSize, int ySize, GDALDataset* sourceTransform )
{
	m_globalSettings.SetGdalUtf8(true);

	char **papszOptions = NULL;
	GDALDataset* outputDataset = (GDALDataset *)GDALCreate( outputDriver, Utility::ConvertToUtf8(filename), xSize, ySize, 1, GDT_Float32, papszOptions );
	
	double transform[6];
	sourceTransform->GetGeoTransform((double*)&transform);
	outputDataset->SetGeoTransform(transform);

	outputDataset->SetProjection( sourceTransform->GetProjectionRef() );

	m_globalSettings.SetGdalUtf8(false);
	return outputDataset;
}

int atoi_custom( const char *c ) {
	int value = 0;
	int sign = 1;
	if( *c == '+' || *c == '-' ) {
		if( *c == '-' ) sign = -1;
		c++;
	}
	while ( isdigit( *c ) ) {
		value *= 10;
		value += (int) (*c-'0');
		c++;
	}
	return value * sign;
}

// ********************************************************
//     CalculateRaster()
// ********************************************************
STDMETHODIMP CUtils::CalculateRaster(SAFEARRAY* InputNames, BSTR expression, BSTR outputFilename, BSTR gdalOutputFormat,
									 float outputNodataValue, ICallback* callback, BSTR* errorMsg, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	CExpression expr;	
	CString err;

	// --------------------------------------------------------
	//   Open input
	// --------------------------------------------------------
	LONG lBound, uBound;
	BSTR* names;
	if (!ValidateInputNames(InputNames,  lBound, uBound, &names))
	{
		*errorMsg = A2BSTR(ErrorMsg(lastErrorCode));
		return S_FALSE;
	}

	int count = uBound - lBound + 1;
	if (count < 2)
	{
		ErrorMessage(tkAT_LEAST_TWO_DATASOURCES_EXPECTED);
		*errorMsg = A2BSTR(ErrorMsg(lastErrorCode));
		return S_FALSE;
	}

	int xSize = 0;
	int ySize = 0;
	
	GDALAllRegister();

	USES_CONVERSION;
	map<CString, GDALDataset*> datasets;
	for (int i = 0; i < count; i++)
	{
		CStringW path = OLE2W(names[i]);
		CString name = W2A(Utility::GetNameFromPath(path));
		name = name.MakeLower();

		GDALDataset* dt = GdalHelper::OpenDatasetW(path, GDALAccess::GA_ReadOnly);
		if (dt)
		{
			// perhaps check rotation and create north up dataset in case bounds should be supported
			/*double inputGeoTransform[6];
			if ( GDALGetGeoTransform( inputDataset, inputGeoTransform ) == CE_None
				&& ( inputGeoTransform[1] < 0.0
				|| inputGeoTransform[2] != 0.0
				|| inputGeoTransform[4] != 0.0
				|| inputGeoTransform[5] > 0.0 ) )
			{
				GDALDatasetH vDataset = GDALAutoCreateWarpedVRT( inputDataset, NULL, NULL, GRA_NearestNeighbour, 0.2, NULL );
				mInputDatasets.push_back( vDataset );
				mInputDatasets.push_back( inputDataset );
				inputDataset = vDataset;
			}
			else
			{
				mInputDatasets.push_back( inputDataset );
			}*/
		}
		else
		{
			CStringW temp = L"Failed to open dataset: " + path;
			*errorMsg = W2BSTR(temp);
			goto cleaning;
		}

		if (dt)
			datasets [name] = dt;

		if (i == 0)
		{
			// take size from the first one
			xSize = dt->GetRasterXSize();
			ySize = dt->GetRasterYSize();
		}
		else
		{
			// make sure that others are of the same size
			if (dt->GetRasterXSize() != xSize || dt->GetRasterYSize() != ySize )
			{
				ErrorMessage(tkIMAGES_MUST_HAVE_THE_SAME_SIZE);
				*errorMsg = A2BSTR(ErrorMsg(lastErrorCode));
				goto cleaning;
			}
		}
	}

	if (datasets.size() < 2)
	{
		*errorMsg = A2BSTR(ErrorMsg(tkAT_LEAST_TWO_DATASOURCES_EXPECTED));
		ErrorMessage(lastErrorCode);
		goto cleaning;
	}
	
	// --------------------------------------------------------
	//   Creating output
	// --------------------------------------------------------
	GDALDataset* dtOutput = NULL;
	GDALDriverH driver = OpenOutputDriver(OLE2A(gdalOutputFormat));
	if (driver != NULL)
	{
		dtOutput = OpenOutputFile(driver, OLE2W(outputFilename), xSize, ySize, datasets.begin()->second);
	}

	if (!dtOutput)
	{
		ErrorMsg(tkINVALID_FILENAME);
		*errorMsg = A2BSTR("Failed to create output dataset");
		goto cleaning;
	}

	// setting no data value
	GDALRasterBand* bandOutput = dtOutput->GetRasterBand(1);
	bandOutput->SetNoDataValue(outputNodataValue);

	if (!expr.ParseExpression(OLE2A(expression), false, err))
	{
		ErrorMessage(tkINVALID_EXPRESSION);
		*errorMsg = A2BSTR(err);
		goto cleaning;
	}
	else
	{
		// --------------------------------------------------------
		//   looking for source bands
		// --------------------------------------------------------
		int numFields = expr.get_NumFields();
		for(int i = 0; i < numFields; i++)
		{
			CString name = expr.get_FieldName(i);
			int pos = name.Find('@', 0);
			if (pos == -1 || pos == name.GetLength() - 1)
			{
				ErrorMessage(tkINVALID_EXPRESSION);
				err.Format("Invalid formula field: %s; @ sign to separate filename and band index is expected", name);
				*errorMsg = A2BSTR(err);
				goto cleaning;
			}

			CString dtName = name.Mid(0, pos).MakeLower();
			if (datasets.find(dtName) != datasets.end())
			{
				GDALDataset* dt = datasets[dtName];
				int bandIndex = atoi_custom(name.Mid(pos + 1));
				GDALRasterBand* band = dt->GetRasterBand(bandIndex);
				if (band)
				{
					CExpressionValue* val = expr.get_FieldValue(i);
					val->band = band;
					val->type = vtFloatArray;
					double nodv = band->GetNoDataValue();
					val->matrix = new RasterMatrix(xSize, 1, new float[xSize * 1], nodv);
				}
				else
				{
					ErrorMessage(tkINVALID_EXPRESSION);
					err.Format("Band wasn't found: %s", name);
					*errorMsg = A2BSTR(err);
					goto cleaning;
				}
			}
			else
			{
				err.Format("Invalid formula field: %s; no dataset with such name in input names", name);
				goto cleaning;
			}
		}
		
		// --------------------------------------------------------
		//   doing calculations
		// --------------------------------------------------------
		long numColumns = xSize;
		long numRows = ySize;
		long percent = 0;
		for(long i = 0; i < numRows; i++ )
		{
			Utility::DisplayProgress(callback, i, numRows, "Calculating", key, percent);

			for(int j = 0; j < numFields; j++)
			{
				CExpressionValue* val = expr.get_FieldValue(j);
				GDALRasterBand* band = ((GDALRasterBand*)val->band);
				double nodv = band->GetNoDataValue();
				if (val->matrix) delete val->matrix;
				val->matrix = new RasterMatrix(xSize, 1, new float[xSize * 1], nodv);
				band->RasterIO(GF_Read, 0, i, numColumns, 1, val->matrix->data(), numColumns, 1, GDALDataType::GDT_Float32, 0, 0);
			}

			CString errorMsg;
			CExpressionValue* resultVal = expr.Calculate(errorMsg);
			if (resultVal)
			{
				RasterMatrix* resultMatrix = resultVal->matrix;

				bool resultIsNumber = resultMatrix->isNumber();
				float* calcData = new float[numColumns];

				if ( resultIsNumber ) //scalar result. Insert number for every pixel
				{
					for ( int j = 0; j < numColumns; ++j )
					{
						calcData[j] = (float)resultMatrix->number();
					}
				}
				else //result is real matrix
				{
					memcpy(calcData, resultMatrix->data(), resultMatrix->GetBufferSize());
					//calcData = resultMatrix->data();
				}

				float ndv = (float)resultMatrix->nodataValue();
				int count = 0;
				for ( int j = 0; j < numColumns; ++j )
				{
					if ( calcData[j] == ndv )
					{
						calcData[j] = outputNodataValue;
						count++;
					}
				}
				
				if (bandOutput->RasterIO( GF_Write, 0, i, numColumns, 1, calcData, numColumns, 1, GDT_Float32, 0, 0 ) != CE_None )
				{
					return S_FALSE;
				}
				
				delete[] calcData;

				expr.ReleaseMemory();
			}
		}

		expr.Clear();

	}

	*retVal = VARIANT_TRUE;

cleaning:
	// ------------------------------------------------------
	//	Cleaning
	// ------------------------------------------------------
	Utility::DisplayProgressCompleted(callback);
	map<CString, GDALDataset*>::iterator it = datasets.begin();
	while (it != datasets.end()) 
	{
		GDALDataset* dt = it->second;
		if (dt)
			GdalHelper::CloseDataset(dt);
		it++;
	}
	if (dtOutput)
		GdalHelper::CloseDataset(dtOutput);
	
	return S_OK;
}