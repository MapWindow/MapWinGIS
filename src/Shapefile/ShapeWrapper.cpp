/**************************************************************************************
 * File name: ShapeWrapper.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of ShapeWrapper
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
 // Sergei Leschinski (lsu) 25 june 2010 - created the file
#include "stdafx.h"
#include "ShapeWrapper.h"
#include "ErrorCodes.h"

#pragma region ShapeType
// **************************************************************
//		get/put_ShapeType()
// **************************************************************
bool CShapeWrapper::put_ShapeType(ShpfileType newVal)
{	
	if( newVal == SHP_MULTIPATCH )
	{	
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		return false;		
	}

	if (ShapeUtility::IsM(newVal) || ShapeUtility::IsZ(newVal))
	{
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		return false;
	}

	_shapeType = newVal;

	ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);
	if (shpType2D == SHP_NULLSHAPE)
	{	
		Clear();
	}
	else if (shpType2D == SHP_POINT)
	{	
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		return false;
	}
	else if (shpType2D == SHP_POLYLINE || shpType2D == SHP_POLYGON)
	{	
		if( _parts.size() == 0 )
		{
			_parts.push_back(0);
		}
	}	
	else if (shpType2D == SHP_MULTIPOINT)
	{	
		_parts.clear();
	}

	return true;
}

#pragma endregion

#pragma region Points
// ********************************************************
//		get_PointsXY 
// ********************************************************
bool CShapeWrapper::get_PointXY(int PointIndex, double& x, double& y)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		x = _points[PointIndex].X;
		y = _points[PointIndex].Y;
		return true;
	}
}

// ********************************************************
//		get_XYFast 
// ********************************************************
void CShapeWrapper::get_XYFast(int PointIndex, double& x, double& y)
{
	x = _points[PointIndex].X;
	y = _points[PointIndex].Y;
}

// **********************************************************
//		put_PointXY()
// **********************************************************
bool CShapeWrapper::put_PointXY(int PointIndex, double x, double y)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_points[PointIndex].X = x;
		_points[PointIndex].Y = y;
		_boundsChanged = true;
		return true;
	}
}

// **********************************************************
//		put_PointZ()
// **********************************************************
bool CShapeWrapper::put_PointZ(int PointIndex, double z)
{
	return false;
}

// **********************************************************
//		put_PointM()
// **********************************************************
bool CShapeWrapper::put_PointM(int PointIndex, double m)
{
	return false;
}

// **********************************************************
//		get_PointsXY()
// **********************************************************
double* CShapeWrapper::get_PointsXY()
{
	return _points.size() > 0 ? (double*)&_points[0] : NULL;
}

// **********************************************************
//		get_PointZ()
// **********************************************************
bool CShapeWrapper::get_PointZ(int PointIndex, double& z)
{
	return false;
}

// **********************************************************
//		get_PointM()
// **********************************************************
bool CShapeWrapper::get_PointM(int PointIndex, double& m)
{
	return false;
}

// **********************************************************
//		get_Point()
// **********************************************************
IPoint* CShapeWrapper::get_Point(long Index)
{
	if( Index < 0 || Index >= (int)_points.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return NULL;
	}
	else
	{
		IPoint* pnt = NULL;
		ComHelper::CreatePoint(&pnt);
		if (pnt)
		{
			pnt->put_X(_points[Index].X);
			pnt->put_Y(_points[Index].Y);
		}
		return pnt;
	}
}

// **********************************************************
//		put_Point()
// **********************************************************
bool CShapeWrapper::put_Point(long Index, IPoint* pnt)
{
	if( Index < 0 || Index >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}

	if (!pnt)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return false;
	}

	pnt->get_X(&_points[Index].X);
	pnt->get_Y(&_points[Index].Y);
	return true;
}

#pragma endregion

#pragma region Parts

// **********************************************************
//		InsertPart()
// **********************************************************
bool CShapeWrapper::InsertPart(int PartIndex, int PointIndex)
{
	ShpfileType shpType2D = get_ShapeType2D();
	if (shpType2D != SHP_POLYLINE && shpType2D != SHP_POLYGON)
	{	
		return false;
	}	
	
	if( PartIndex < 0 )
	{
		PartIndex = 0;
	}
	else if( PartIndex > (int)_parts.size() )
	{
		PartIndex = _parts.size();
	}
		
	// this part already exists
	for (unsigned int i = 0; i < _parts.size(); i++)
	{
		if (_parts[i] == PointIndex)
		{
			return true;	// returning true to preserve compatibility
		}
	}

	_parts.insert(_parts.begin() + PartIndex, PointIndex);
	return true;
}

// **************************************************************
//		DeletePart()
// **************************************************************
bool CShapeWrapper::DeletePart(int PartIndex)
{
	ShpfileType shpType2D = get_ShapeType2D();
	if (shpType2D != SHP_POLYLINE && shpType2D != SHP_POLYGONM)
	{
		return false;
	}
	
	if( PartIndex < 0 || PartIndex >= (int)_parts.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	
	_parts.erase( _parts.begin() + PartIndex );
	return true;
}

// **********************************************************
//		get_PartStartPoint()
// **********************************************************
int CShapeWrapper::get_PartStartPoint(int PartIndex)
{
	if( PartIndex < 0 || PartIndex >= (int)_parts.size())
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	else
		return _parts[PartIndex];
}

// **********************************************************
//		get_PartEndPoint()
// **********************************************************
int CShapeWrapper::get_PartEndPoint(int PartIndex)
{
	if( PartIndex < 0 || PartIndex >= (int)_parts.size())
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	else
	{
		if (PartIndex == _parts.size() - 1)
		{
			return _points.size() - 1;
		}
		else
		{
			return _parts[PartIndex + 1] - 1;
		}
	}
}

// **********************************************************
//		put_PartStartPoint()
// **********************************************************
bool CShapeWrapper::put_PartStartPoint(long PartIndex, long newVal)
{
	if( PartIndex < 0 || PartIndex >= (long)_parts.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		_parts[PartIndex] = newVal;
		return true;
	}
}
#pragma endregion

#pragma region Bounds
// ********************************************************
//		get_XYBounds 
// ********************************************************
bool CShapeWrapper::get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax)
{
	if (_boundsChanged)
	{
		this->RefreshBoundsXY();
	}

	xMin = _xMin;
	xMax = _xMax;
	yMin = _yMin;
	yMax = _yMax;

	return true;
}

// ********************************************************
//		RefreshBoundsXY
// ********************************************************
void CShapeWrapper::RefreshBoundsXY()
{
	if (_boundsChanged)
	{
		if (get_ShapeType2D() == SHP_NULLSHAPE)
		{
			_xMin = 0.0;
			_xMax = 0.0;
			_yMin = 0.0;
			_yMax = 0.0;
		}
		else
		{
			for( int i = 0; i < (int)_points.size(); i++ )
			{	
				if( i == 0 )
				{	
					_xMin = _xMax = _points[i].X;
					_yMin = _yMax = _points[i].Y;
				}
				else
				{	if		( _points[i].X < _xMin )	_xMin = _points[i].X;
					else if ( _points[i].X > _xMax )	_xMax = _points[i].X;
					if		( _points[i].Y < _yMin )	_yMin = _points[i].Y;
					else if	( _points[i].Y > _yMax )	_yMax = _points[i].Y;
				}
			}
		}

		_boundsChanged = false;
	}
}

// ********************************************************
//		RefreshBounds
// ********************************************************
void CShapeWrapper::RefreshBounds()
{
	RefreshBoundsXY();
}

// **********************************************************
//		get_Bounds
// **********************************************************
bool CShapeWrapper::get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
							   double& zMin, double& zMax, double& mMin, double& mMax)
{
	if (_boundsChanged) {
		RefreshBounds();
	}

	xMin = _xMin;
	xMax = _xMax;
	yMin = _yMin;
	yMax = _yMax;
	zMin = zMax = mMin = mMax = 0.0;

	return true;
}
#pragma endregion

#pragma region Shapedata
// **************************************************************
//		put_Data 
// **************************************************************
// Passing the shape data from disk to the memory structures.
bool CShapeWrapper::put_RawData(char* shapeData)
{
	_shapeType = (ShpfileType)*(int*)shapeData;
	_boundsChanged = true;

	int numPoints = 0;
	int numParts = 0;

	ShpfileType shpType2D = _shapeType;
	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
	}
	else if (shpType2D == SHP_MULTIPOINT)
	{
		double* bounds = (double*)(shapeData + 4);
		_xMin = bounds[0];					// 12
		_yMin = bounds[1];					// 20
		_xMax = bounds[2];					// 28
		_yMax = bounds[3];					// 36

		numPoints = *(int*)(shapeData + 36);
		double* points = (double*)(shapeData + 40);

		// points
		_points.resize(numPoints);
		memcpy(&_points[0], points, sizeof(double) * numPoints * 2);
	}

	else if (shpType2D == SHP_POLYLINE || shpType2D == SHP_POLYGON)
	{
		double* bounds = (double*)(shapeData + 4);
		_xMin = bounds[0];					// 12
		_yMin = bounds[1];					// 20
		_xMax = bounds[2];					// 28
		_yMax = bounds[3];					// 36

		numParts = *(int*)(shapeData + 36);
		numPoints = *(int*)(shapeData + 40);

		// parts
		int* parts = (int*)(shapeData + 44);
		if (numParts > 0)
		{
			_parts.resize(numParts);
			memcpy(&_parts[0], parts, sizeof(int) * numParts);
		}

		// points
		double* points = (double*)(shapeData + 44 + sizeof(int) * numParts);
		if (numPoints > 0)
		{
			_points.resize(numPoints);
			memcpy(&_points[0], points, sizeof(double) * numPoints * 2);
		}
	}
	return true;
}

// **************************************************************
//		get_Data 
// **************************************************************
// Forming the data to write to the disk. Should be optimized as far as possible.
int* CShapeWrapper::get_RawData(void)
{
	int numPoints = _points.size();
	int numParts = _parts.size();

	int length =  ShapeUtility::get_ContentLength(_shapeType, numPoints , numParts);
	int* intdata = new int[length/4];
	intdata[0] = (int)_shapeType;
	
	double* ddata;

	ShpfileType shpType2D = get_ShapeType2D();
	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
	}
	else if (shpType2D == SHP_MULTIPOINT)
	{	
		ddata = (double*)&intdata[1];
		ddata[0] = _xMin;
		ddata[1] = _yMin;
		ddata[2] = _xMax;
		ddata[3] = _yMax;
		
		// points
		intdata[9] = numPoints;
		ddata = (double*)&intdata[10];
		memcpy(ddata, &_points[0], sizeof(double) * numPoints * 2);
	}
	else if (shpType2D == SHP_POLYLINE || shpType2D == SHP_POLYGON)
	{	
		ddata = (double*)&intdata[1];
		ddata[0] = _xMin;
		ddata[1] = _yMin;
		ddata[2] = _xMax;
		ddata[3] = _yMax;

		// points
		intdata[9] = numParts;
		intdata[10] = numPoints;

		// parts
		int* parts = &intdata[11];
		memcpy(parts, &_parts[0], sizeof(int) * numParts);

		// points
		ddata = (double*)&intdata[11 + numParts];
		memcpy(ddata, &_points[0], sizeof(double) * numPoints * 2);
	}
	return intdata;
}
#pragma endregion

#pragma region PointEditing

// ******************************************************
//		Clear()
// ******************************************************
void CShapeWrapper::Clear()
{
	_parts.clear();
	_points.clear();
	_boundsChanged = true;
}

// ******************************************************
//		InsertPoint()
// ******************************************************
bool CShapeWrapper::InsertPoint(int PointIndex, IPoint* pnt)
{
	if (!pnt)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return false;
	}
	else
	{
		double x, y, z, m;
		pnt->get_X(&x);
		pnt->get_Y(&y);
		pnt->get_Z(&z);
		pnt->get_M(&m);
		return InsertPointXYZM(PointIndex, x, y, z, m);
	}
}

// ******************************************************
//		InsertPointXY()
// ******************************************************
bool CShapeWrapper::InsertPointXY(int PointIndex, double x, double y)
{
	return InsertPointXYZM(PointIndex, x, y, 0.0, 0.0);
}

// ******************************************************
//		InsertPointXYZM()
// ******************************************************
bool CShapeWrapper::InsertPointXYZM(int PointIndex, double x, double y, double z, double m)
{
	if( _shapeType == SHP_NULLSHAPE )
	{	
		return false;
	}
	
	if (PointIndex < 0)
	{
		PointIndex = 0;
	}
	else if (PointIndex > (int)_points.size())
	{
		PointIndex = _points.size();
	}
	
	_points.insert(_points.begin() + PointIndex, pointEx(x, y));
	_boundsChanged = true;
	return true;
}

// ******************************************************
//		AddPoint()
// ******************************************************
void CShapeWrapper::AddPoint(IPoint* pnt)
{
	if (!pnt)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return;
	}
	
	double x, y;
	pnt->get_X(&x);
	pnt->get_Y(&y);
	
	AddPoint(x, y);
}

// ******************************************************
//		AddPoint()
// ******************************************************
void CShapeWrapper::AddPoint(double x, double y)
{
	_points.push_back(pointEx(x, y));
	_boundsChanged = true;
}

// ******************************************************
//		AddPoint()
// ******************************************************
void CShapeWrapper::AddPoint(double x, double y, double z, double m)
{
	AddPoint(x, y);
}

// ********************************************************
//		DeletePoint()
// ********************************************************
bool CShapeWrapper::DeletePoint(int PointIndex)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_boundsChanged = true;
		_points.erase( _points.begin() + PointIndex );
		return true;
	}
}

#pragma endregion


// *******************************************************************
//     PointInRing()
// *******************************************************************
bool CShapeWrapper::PointInRing( int partIndex, double pointX, double pointY )
{
	ShpfileType shptype =  this->get_ShapeType();
	if( shptype != SHP_POLYGON && 
		shptype != SHP_POLYGONZ && 
		shptype != SHP_POLYGONM )
		return false;
	
	// TODO: add test of bounds !!!
	double xMin, xMax, yMin, yMax, zMin, zMax, mMin, mMax;
	this->get_Bounds(xMin, xMax, yMin, yMax, zMin, zMax, mMin, mMax);
	
	if (pointX <  xMin || pointY < yMin || pointX > xMax || pointY > yMax)
		return false;

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
		else if(y1y2 == 0.0) // Then it has intersected a vertex
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

// *******************************************************************
//     ReversePoints()
// *******************************************************************
void CShapeWrapper::ReversePoints(long startIndex, long endIndex)
{
	if (_points.size() > 1)
	{
		std::vector<pointEx>::iterator iter1 = _points.begin();
		std::vector<pointEx>::iterator iter2 = _points.begin();
		iter1 += startIndex;
		iter2 += endIndex;
		reverse(iter1, iter2);
	}
}

// *******************************************************************
//     AddPart()
// *******************************************************************
void CShapeWrapper::AddPart(int pointIndex)
{
	_parts.push_back(pointIndex);
}
