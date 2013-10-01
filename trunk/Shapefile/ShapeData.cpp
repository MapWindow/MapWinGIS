/**************************************************************************************
 * File name: ShapeData.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CShapeData. Used for holding shape data in fast non-editabe mode.
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 // Sergei Leschinski (lsu) 30 jan 2011 - created the file

#include "stdafx.h"
#include "ShapeData.h"
#include "ErrorCodes.h"

#pragma region ShapefileMemo
// Record header
// ---------------------------------------------------------
// Position     Value               Type        Number  Byte Order
// ---------------------------------------------------------
// Byte 0       Record Number       Integer     1           Big
// Byte 4       Content Length      Integer     1           Big       


// Null Shape
// ---------------------------------------------------------
// Position     Value               Type        Number  Byte Order
// ---------------------------------------------------------
// Byte 0		Shape Type			Integer		1			Little

// Points
// ---------------------------------------------------------
// Position     Value               Type        Number  Byte Order
// ---------------------------------------------------------
// Byte 0		Shape Type			Integer		1			Little
// Byte 4		X					Double		1			Little
// Byte 12		Y					Double		1			Little

// MultiPoints
// ---------------------------------------------------------
// Position     Value               Type        Number  Byte Order
// ---------------------------------------------------------
// Byte 0       Shape Type		    Integer     1           Little
// Byte 4       Box                 Double      4           Little
// Byte 36      NumPoints           Integer     1           Little
// Byte 40      Points              Point       NumPoints   Little
// Byte X       Zmin                Double      1           Little
// Byte X + 8   Zmax                Double      1           Little
// Byte X + 16  Zarray              Double      NumPoints   Little
// Byte Z*      Mmin                Double      1           Little
// Byte Z+8*    Mmax                Double      1           Little
// Byte Z+16*   Marray              Double      NumPoints   Little
// X = 40 + (16 * NumPoints); Y = X + 16 + (8 * NumPoints)

// Polygons, Polylines
// ---------------------------------------------------------
// Position     Value               Type        Number  Byte Order
// ---------------------------------------------------------
// Byte 0       Shape Type          Integer     1           Little
// Byte 4       Box                 Double      4           Little
// Byte 36      NumParts            Integer     1           Little
// Byte 40      NumPoints           Integer     1           Little
// Byte 44      Parts               Integer     NumParts    Little
// Byte X       Points              Point       NumPoints   Little
// Byte Y       Zmin                Double      1           Little
// Byte Y + 8   Zmax                Double      1           Little
// Byte Y + 16  Zarray              Double      NumPoints   Little
// Byte Z*      Mmin                Double      1           Little
// Byte Z+8*    Mmax                Double      1           Little
// Byte Z+16*   Marray              Double      NumPoints   Little
// X = 44 + (4 * NumParts), Y= X + (16 * NumPoints), Z = Y + 16 + (8 * NumPoints)
// * optional
#pragma endregion

// *****************************************************
//		get_PointXY 
// *****************************************************
bool CShapeData::get_PointXY(int PointIndex, double& x, double& y)
{
	if( PointIndex < 0 || PointIndex >= (int)_pointCount )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		x = _points[PointIndex * 2];
		y = _points[PointIndex * 2 + 1];
		return true;
	}
}

// ********************************************************
//		get_XYFast 
// ********************************************************
void CShapeData::get_XYFast(int PointIndex, double& x, double& y)
{
	x = _points[PointIndex * 2];
	y = _points[PointIndex * 2 + 1];
}

// *****************************************************
//		get_PointsXY 
// *****************************************************
double* CShapeData::get_PointsXY()
{
	if (_pointCount > 0)
	{
		return (double*)_points;
	}
	else
	{
		return NULL;
	}
}

// **************************************************
//		get_PartStartPoint()
// ******************************************************
int CShapeData::get_PartStartPoint(int PartIndex)
{
	if( PartIndex < 0 || PartIndex >= (int)_partCount)
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	else
	{
		return _parts[PartIndex];
	}
}

// ********************************************************
//		get_PartEndPoint()
// ********************************************************
int CShapeData::get_PartEndPoint(int PartIndex)
{
	if( PartIndex < 0 || PartIndex >= (int)_partCount)
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	else
	{
		if (PartIndex == _partCount - 1)
		{
			return _pointCount - 1;
		}
		else
		{
			return _parts[PartIndex + 1] - 1;
		}
	}
}

// ******************************************************
//	  get_XYBounds()
// ******************************************************
bool CShapeData::get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax)
{
	if (_bounds)
	{
		xMin = _bounds[0];
		yMin = _bounds[1];
		xMax = _bounds[2];
		yMax = _bounds[3];
		return true;
	}
	else if (_shapeType == SHP_POINT && _points)
	{
		// it's far more effective to check type externally to avoid duplicating values
		xMin = _points[0];
		xMax = _points[0];
		yMin = _points[1];
		yMax = _points[1];
		return true;
	}
	else
	{
		// we shall not be here normally
		ASSERT(FALSE);
		return false;
	}
}

// ******************************************************
//	  Clear()
// ******************************************************
void CShapeData::Clear()
{
	_pointCount = 0;
	_partCount = 0;
	
	if ( _parts )
	{
		delete[] _parts;
		_parts = NULL;
	}

	if ( _points )
	{
		delete[] _points;
		_points = NULL;
	}

	if (_bounds)
	{
		delete[] _bounds;
		_bounds = NULL;
	}
}

#pragma region ShapeData

// ******************************************************
//		put_ShapeData 
// ******************************************************
// Passing the shape data from disk to the memory structures.
bool CShapeData::put_ShapeData(char* shapeData)
{
	// type
	_shapeType = (ShpfileType)*(int*)shapeData;
	_shapeType = ShapeTypeConvert2D(_shapeType);
	
	this->Clear();
	
	if (_shapeType == SHP_NULLSHAPE)
	{
		// do nothing;
	}
	else if( _shapeType == SHP_POINT)
	{
		_pointCount = 1;
		double* ddata = (double*)(shapeData + 4);
		_points = new double[2];
		memcpy(_points, ddata, sizeof(double) * 2);
	}
	else if( _shapeType == SHP_MULTIPOINT)
	{	
		double* bounds = (double*)(shapeData + 4);
		_bounds = new double[4];
		memcpy(_bounds, bounds, sizeof(double) * 4);	// 36

		_pointCount = *(int*)(shapeData + 36);
		_points = new double[_pointCount * 2];
		
		double* points = (double*)(shapeData + 40);
		memcpy(_points, points, sizeof(double) * _pointCount * 2);
	}
	
	else if( _shapeType == SHP_POLYLINE || _shapeType == SHP_POLYGON)
	{	
		double* bounds = (double*)(shapeData + 4);
		_bounds = new double[4];
		memcpy(_bounds, bounds, sizeof(double) * 4);	// 36

		_partCount = *(int*)(shapeData + 36);
		_pointCount = *(int*)(shapeData + 40);

		// parts
		int* parts = (int*)(shapeData + 44);
		if (_partCount > 0)
		{
			_parts = new int[_partCount];
			memcpy(_parts, parts, sizeof(int) * _partCount);
		}
		
		// points
		double* points = (double*)(shapeData + 44 + sizeof(int) * _partCount);
		if (_pointCount > 0)
		{
			_points = new double[_pointCount * 2];
			memcpy(_points, points, sizeof(double) * _pointCount * 2);
		}
	}
	return true;
}
#pragma endregion

#pragma region Utilities
// ********************************************************
//     PointInRing()
// ********************************************************
bool CShapeData::PointInRing( int partIndex, double pointX, double pointY )
{
	ShpfileType shptype =  this->get_shapeType();
	if( shptype != SHP_POLYGON && 
		shptype != SHP_POLYGONZ && 
		shptype != SHP_POLYGONM )
	{
		return false;
	}
	
	double xMin, xMax, yMin, yMax;
	this->get_BoundsXY(xMin, xMax, yMin, yMax);
	
	if (pointX <  xMin || pointY < yMin || pointX > xMax || pointY > yMax)
	{
		return false;
	}

	int begin = this->get_PartStartPoint(partIndex);
	int end = this->get_PartEndPoint(partIndex);
	
	int CrossCount = 0;
	for(int nPoint = begin; nPoint <= end; nPoint++)
	{
		double* points= this->get_PointsXY();
		double x1 = points[nPoint * 2] - pointX;
		double y1 = points[nPoint * 2 + 1] - pointY;
		double x2 = points[(nPoint + 1) * 2] - pointX;
		double y2 = points[(nPoint + 1) * 2 + 1] - pointY;

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
	return (CrossCount&1) ? true : false;
}

// **************************************************************
//		ShapeTypeConvert2D()
// **************************************************************
ShpfileType CShapeData::ShapeTypeConvert2D(ShpfileType shpType)
{
	if		(shpType == SHP_NULLSHAPE)																return SHP_NULLSHAPE;
	else if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ)				return SHP_POINT;
	else if	(shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ)	return SHP_MULTIPOINT;
	else if (shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)			return SHP_POLYGON;
	else if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)		return SHP_POLYLINE;
	else																							return SHP_NULLSHAPE;
}



#pragma endregion