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

#pragma region Memo
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
	else
	{	
		_ShapeType = newVal;
		if( _ShapeType == SHP_NULLSHAPE )
		{	
			this->Clear();
			_ShapeType2D = SHP_NULLSHAPE;
		}
		else if( _ShapeType == SHP_POINT || _ShapeType == SHP_POINTZ || _ShapeType == SHP_POINTM )
		{	
			//_points.resize(1);
			_ShapeType2D = SHP_POINT;
		}
		else if( _ShapeType == SHP_POLYLINE || _ShapeType == SHP_POLYLINEZ || _ShapeType == SHP_POLYLINEM)
		{	
			_ShapeType2D = SHP_POLYLINE;
			if( /*_points.size() > 0 &&*/ _parts.size() <= 0 )
			{
				_parts.push_back(0);
			}
		}	
		else if (_ShapeType == SHP_POLYGON || _ShapeType == SHP_POLYGONZ || _ShapeType == SHP_POLYGONM )
		{
			_ShapeType2D = SHP_POLYGON;
			if( /*_points.size() > 0 &&*/ _parts.size() <= 0 )	
			{
				_parts.push_back(0);
			}
		}
		else if( _ShapeType == SHP_MULTIPOINT || _ShapeType == SHP_MULTIPOINTZ || _ShapeType == SHP_MULTIPOINTM )
		{	
			_ShapeType2D = SHP_MULTIPOINT;
			_parts.clear();
		}
		
		// this is a bot crude, we need to separate Z and M values
		if ( _ShapeType != SHP_POINT && _ShapeType != SHP_POLYLINE && _ShapeType != SHP_POLYGON && _ShapeType != SHP_MULTIPOINT  )
		{
			if (_pointsZ.size() != _points.size() )
			{
				_pointsZ.resize(_points.size());
			}

			if (_pointsM.size() != _points.size() )
			{
				_pointsM.resize(_points.size());
			}
		}
		else
		{
			_pointsM.clear();
			_pointsZ.clear();
		}
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
	if( PointIndex < 0 || PointIndex >= (int)_pointsZ.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_pointsZ[PointIndex] = z;
		return true;
	}
}

// **********************************************************
//		put_PointM()
// **********************************************************
bool CShapeWrapper::put_PointM(int PointIndex, double m)
{
	if( PointIndex < 0 || PointIndex >= (int)_pointsM.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_pointsM[PointIndex] = m;
		return true;
	}
}

// **********************************************************
//		get_PointsXY()
// **********************************************************
double* CShapeWrapper::get_PointsXY()
{
	if (_points.size() > 0)
		return (double*)&_points[0];
	else
		return NULL;
}

// **********************************************************
//		get_PointZ()
// **********************************************************
bool CShapeWrapper::get_PointZ(int PointIndex, double& z)
{
	if( PointIndex < 0 || PointIndex >= (int)_pointsZ.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		z = _pointsZ[PointIndex];
		return true;
	}
}

// **********************************************************
//		get_PointM()
// **********************************************************
bool CShapeWrapper::get_PointM(int PointIndex, double& m)
{
	if( PointIndex < 0 || PointIndex >= (int)_pointsM.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		m = _pointsM[PointIndex];
		return true;
	}
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
		m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
		//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);	
		if (pnt)
		{
			pnt->put_X(_points[Index].X);
			pnt->put_Y(_points[Index].Y);
			//pnt->AddRef();			
			// Z, M values aren't supported			
			//pnt->put_Z(_pointsZ[Index]);
			//pnt->put_M(_pointsM[Index]);
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
	else
	{
		if (!pnt)
		{
			_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
			return false;
		}
		else
		{
			pnt->get_X(&_points[Index].X);
			pnt->get_Y(&_points[Index].Y);
			return true;
		}
	}
}
#pragma endregion

#pragma region Parts

// **********************************************************
//		InsertPart()
// **********************************************************
bool CShapeWrapper::InsertPart(int PartIndex, int PointIndex)
{
	if( _ShapeType != SHP_POLYLINE &&
		_ShapeType != SHP_POLYLINEZ &&
		_ShapeType != SHP_POLYLINEM &&
		_ShapeType != SHP_POLYGON &&
		_ShapeType != SHP_POLYGONZ &&
		_ShapeType != SHP_POLYGONM )
	{	
		return false;
	}	
	else
	{
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

		_parts.insert( _parts.begin() + PartIndex, PointIndex );
		return true;
	}
}

// **************************************************************
//		DeletePart()
// **************************************************************
bool CShapeWrapper::DeletePart(int PartIndex)
{
	if( _ShapeType != SHP_POLYLINE &&
		_ShapeType != SHP_POLYLINEZ &&
		_ShapeType != SHP_POLYLINEM &&
		_ShapeType != SHP_POLYGON &&
		_ShapeType != SHP_POLYGONZ &&
		_ShapeType != SHP_POLYGONM )
	{	
		return false;		
	}
	else if( PartIndex < 0 || PartIndex >= (int)_parts.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_parts.erase( _parts.begin() + PartIndex );
		return true;
	}
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
		this->RefreshBounds();
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
		if (_ShapeType2D == SHP_NULLSHAPE)
		{
			_xMin = 0.0;
			_xMax = 0.0;
			_yMin = 0.0;
			_yMax = 0.0;
		}
		else if (_ShapeType2D == SHP_POINT)
		{
			_xMin = _points[0].X;
			_xMax = _points[0].X;
			_yMin = _points[0].Y;
			_yMax = _points[0].Y;
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
	if (_boundsChanged)
	{
		if (_ShapeType2D == SHP_NULLSHAPE)
		{
			_xMin = _xMax = _yMin = _yMax =	_zMin = _zMax = _mMin = _mMax = 0.0;
		}
		else if (_ShapeType2D == SHP_POINT)
		{
			_xMin = _points[0].X;
			_yMin = _points[0].Y;
			_zMin = _pointsZ.size() == 1 ? _pointsZ[0] : 0.0;
			_mMin = _pointsM.size() == 1 ? _pointsM[0] : 0.0;
			_xMax = _xMin;
			_yMax = _yMin;
			_zMax = _zMin;
			_mMax = _mMin;
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
			for( int i = 0; i < (int)_pointsZ.size(); i++ )
			{	
				if (i == 0)
				{
					_zMin = _zMax = _pointsZ[i];
				}
				else
				{
					if		( _pointsZ[i] < _zMin )		_zMin = _pointsZ[i];
					else if ( _pointsZ[i] > _zMax )		_zMax = _pointsZ[i];
				}
			}
			for( int i = 0; i < (int)_pointsM.size(); i++ )
			{	
				if (i == 0)
				{
					_mMin = _mMax = _pointsM[i];
				}
				else
				{
					if		( _pointsM[i] < _mMin )		_mMin = _pointsM[i];
					else if	( _pointsM[i] > _mMax )		_mMax = _pointsM[i];
				}
			}

		}
		_boundsChanged = false;
	}
}

// **********************************************************
//		get_Bounds
// **********************************************************
bool CShapeWrapper::get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
							   double& zMin, double& zMax, double& mMin, double& mMax)
{
	if (!_boundsChanged)
	{
		xMin = _xMin;
		xMax = _xMax;
		yMin = _yMin;
		yMax = _yMax;
		zMin = _zMin;
		zMax = _zMax;
		mMin = _mMin;
		mMax = _mMax;
	}
	else
	{
		if (_ShapeType2D == SHP_NULLSHAPE || _points.size() == 0)
		{
			xMin = 0.0;
			xMax = 0.0;
			yMin = 0.0;
			yMax = 0.0;
			zMin = 0.0;
			zMax = 0.0;
			mMin = 0.0;
			mMax = 0.0;
		}
		else if (_ShapeType2D == SHP_POINT)
		{
			xMin = _points[0].X;
			xMax = _points[0].X;
			yMin = _points[0].Y;
			yMax = _points[0].Y;
			if (_pointsZ.size() > 0)
			{
				zMin = _pointsZ[0];
				zMax = _pointsZ[0];
			}
			if (_pointsM.size() > 0)
			{
				mMin = _pointsM[0];
				mMax = _pointsM[0];
			}
		}
		else
		{
			xMin = xMax = yMin = yMax = 0.0;
			zMin = zMax = mMin = mMax = 0.0;
			for( int i = 0; i < (int)_points.size(); i++ )
			{	
				if( i == 0 )
				{	
					xMin = xMax = _points[i].X;
					yMin = yMax = _points[i].Y;
				}
				else
				{	if		( _points[i].X < xMin )	xMin = _points[i].X;
					else if ( _points[i].X > xMax )	xMax = _points[i].X;
					if		( _points[i].Y < yMin )	yMin = _points[i].Y;
					else if	( _points[i].Y > yMax )	yMax = _points[i].Y;
				}
			}

			for (unsigned int i = 0; i < _pointsZ.size(); i++)
			{
				if( i == 0 )
				{	
					zMin = zMax = _pointsZ[i];
				}
				else
				{	if		( _pointsZ[i] < zMin )	zMin = _pointsZ[i];
					else if ( _pointsZ[i] > zMax )	zMax = _pointsZ[i];
				}
			}

			for (unsigned int i = 0; i < _pointsZ.size(); i++)
			{
				if( i == 0 )
				{	
					mMin = mMax = _pointsM[i];
				}
				else
				{	if		( _pointsZ[i] < zMin )	mMin = _pointsZ[i];
					else if ( _pointsZ[i] > zMax )	mMax = _pointsZ[i];
				}
			}
		}

		_xMin = xMin;
		_xMax = xMax;
		_yMin = yMin;
		_yMax = yMax;
		_zMin = zMin;
		_zMax = zMax;
		_mMin = mMin;
		_mMax = mMax;
		_boundsChanged = false;
	}
	return true;
}
#pragma endregion

#pragma region Shapedata
// **************************************************************
//		put_Data 
// **************************************************************
// Passing the shape data from disk to the memory structures.
bool CShapeWrapper::put_ShapeData(char* shapeData, Extent* extents)
{
	// type
	_ShapeType = (ShpfileType)*(int*)shapeData;
	_ShapeType2D = ShapeTypeConvert2D(_ShapeType);
	_boundsChanged = true;
	int numPoints = 0;
	int numParts = 0;

	if (_ShapeType2D == SHP_NULLSHAPE)
	{
		// do nothing;
	}
	else if( _ShapeType2D == SHP_POINT)
	{
		numPoints = 1;
		double* ddata = (double*)(shapeData + 4);
		_points.resize(1);
		_points[0].X = ddata[0];
		_points[0].Y = ddata[1];
		
		if (extents)
		{
			if (_points[0].X < extents->left || _points[0].X > extents->right || 
				_points[0].Y > extents->top || _points[0].Y < extents->bottom)
				return false;
		}
		
		if(_ShapeType == SHP_POINTM)
		{
			_pointsM.resize(1);
			_pointsM[0] = ddata[2];
		}
		else if (_ShapeType == SHP_POINTZ)
		{
			_pointsM.resize(1);
			_pointsZ.resize(1);
			_pointsM[0] = ddata[2];
			_pointsZ[0] = ddata[3];
		}
	}
	else if( _ShapeType2D == SHP_MULTIPOINT)
	{	
		double* bounds = (double*)(shapeData + 4);
		_xMin = bounds[0];					// 12
		_yMin = bounds[1];					// 20
		_xMax = bounds[2];					// 28
		_yMax = bounds[3];					// 36
		
		if (extents)
		{
			if (_xMin > extents->right||_xMax < extents->left||_yMin > extents->top||_yMax < extents->bottom)
				return false;
		}

		numPoints = *(int*)(shapeData + 36);
		double* points = (double*)(shapeData + 40);
		
		// points
		_points.resize(numPoints);	
		memcpy(&_points[0], points, sizeof(double) * numPoints * 2);
		
		// z values
		if(_ShapeType == SHP_MULTIPOINTZ || _ShapeType == SHP_MULTIPOINTM)
		{
			points += numPoints * 2;
			_mMin = *(points);
			_mMax = *(points + 1);
			points += 2;
			_pointsM.resize(numPoints);
			memcpy(&_pointsM[0], points, sizeof(double) * numPoints);
			
			// m values
			if(_ShapeType == SHP_MULTIPOINTZ)
			{
				points += numPoints;
				_zMin = *(points);
				_zMax = *(points + 1);
				points += 2;
				_pointsZ.resize(numPoints);
				memcpy(&_pointsZ[0], points, sizeof(double) * numPoints);
			}
		}
	}
	
	else if( _ShapeType2D == SHP_POLYLINE || _ShapeType2D == SHP_POLYGON)
	{	
		double* bounds = (double*)(shapeData + 4);
		_xMin = bounds[0];					// 12
		_yMin = bounds[1];					// 20
		_xMax = bounds[2];					// 28
		_yMax = bounds[3];					// 36
		
		if (extents)
		{
			if (_xMin > extents->right||_xMax < extents->left||_yMin > extents->top||_yMax < extents->bottom)
				return false;
		}

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
		
		// z values
		if( _ShapeType == SHP_POLYLINEZ || _ShapeType == SHP_POLYGONZ ||
			_ShapeType == SHP_POLYLINEM || _ShapeType == SHP_POLYGONM)
		{
			points += numPoints * 2;
			_mMin = *(points);
			_mMax = *(points + 1);
			points += 2;
			
			if (numPoints > 0)
			{
				_pointsM.resize(numPoints);
				memcpy(&_pointsM[0], points, sizeof(double) * numPoints);
			}
			
			// m values
			if(_ShapeType == SHP_POLYLINEZ || _ShapeType == SHP_POLYGONZ)
			{
				points += numPoints;
				_zMin = *(points);
				_zMax = *(points + 1);
				points += 2;
				
				if (numPoints > 0)
				{
					_pointsZ.resize(numPoints);
					memcpy(&_pointsZ[0], points, sizeof(double) * numPoints);
				}
			}
		}
	}
	return true;
}

// **************************************************************
//		get_Data 
// **************************************************************
// Forming the data to write to the disk. Should be optimized as far as possible.
int* CShapeWrapper::get_ShapeData(void)
{
	int numPoints = _points.size();
	int numParts = _parts.size();

	int length =  get_ContentLength(_ShapeType, numPoints , numParts);
	int* intdata = new int[length/4];
	intdata[0] = (int)_ShapeType;
	
	double* ddata;

	if (_ShapeType2D == SHP_NULLSHAPE)
	{
		// do nothing;
	}
	else if( _ShapeType2D == SHP_POINT)
	{
		if (_points.size() == 0)
		{
			intdata[0] = (int)SHP_NULLSHAPE;
		}
		else
		{
			ddata = (double*)&intdata[1];
			ddata[0] = _points[0].X;
			ddata[1] = _points[0].Y;
					
			if (_ShapeType == SHP_POINTM)	
			{
				ddata[2] = _pointsM[0];
			}
			else if(_ShapeType == SHP_POINTZ )
			{
				ddata[2] = _pointsM[0];
				ddata[3] = _pointsZ[0];
			}
		}
	}
	else if( _ShapeType2D == SHP_MULTIPOINT)
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
		
		// z values
		if(_ShapeType == SHP_MULTIPOINTZ || _ShapeType == SHP_MULTIPOINTM)
		{
			ddata += numPoints * 2;
			ddata[0] = _mMin;
			ddata[1] = _mMax;
			ddata += 2;
			memcpy(ddata, &_pointsM[0], sizeof(double) * numPoints);
			
			// m values
			if(_ShapeType == SHP_MULTIPOINTZ)
			{
				ddata += numPoints;
				ddata[0] = _zMin;
				ddata[1] = _zMax;
				ddata += 2;
				memcpy(ddata, &_pointsZ[0], sizeof(double) * numPoints);
			}
		}
	}
	
	else if( _ShapeType2D == SHP_POLYLINE || _ShapeType2D == SHP_POLYGON)
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
		//if (numParts > 0)
			memcpy(parts, &_parts[0], sizeof(int) * numParts);

		// points
		ddata = (double*)&intdata[11 + numParts];
		//if (numPoints > 0)
		{
			memcpy(ddata, &_points[0], sizeof(double) * numPoints * 2);
		
			// z values
			if( _ShapeType == SHP_POLYLINEZ || _ShapeType == SHP_POLYGONZ ||
				_ShapeType == SHP_POLYLINEM || _ShapeType == SHP_POLYGONM)
			{
				ddata += numPoints * 2;
				ddata[0] = _mMin;
				ddata[1] = _mMax;
				ddata += 2;
				memcpy(ddata, &_pointsM[0], sizeof(double) * numPoints);
				
				// m values
				if(_ShapeType == SHP_POLYLINEZ || _ShapeType == SHP_POLYGONZ)
				{
					ddata += numPoints;
					ddata[0] = _zMin;
					ddata[1] = _zMax;
					ddata += 2;
					memcpy(ddata, &_pointsZ[0], sizeof(double) * numPoints);
				}
			}
		}
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
	_pointsZ.clear();
	_pointsM.clear();
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
	if( _ShapeType == SHP_NULLSHAPE )
	{	
		return false;
	}
	else
	{	
		if( PointIndex < 0 )
		{
			PointIndex = 0;
		}
		else if( PointIndex > (int)_points.size() )
		{
			PointIndex = _points.size();
		}
		

		if( _ShapeType == SHP_POINT)
		{	
			if( _points.size() == 0)
			{	
				_points.push_back(pointEx(x,y));
				_boundsChanged = true;
				return true;
			}
			else
				return false;
		}
		else if ( _ShapeType == SHP_POINTZ || _ShapeType == SHP_POINTM )
		{
			if( _points.size() == 0)
			{	
				_points.push_back(pointEx(x,y));
				_pointsZ.push_back(z);
				_pointsM.push_back(m);
				_boundsChanged = true;
				return true;
			}
			else
				return false;
		}
		else if (_ShapeType  == SHP_POLYLINE || _ShapeType == SHP_POLYGON || _ShapeType == SHP_MULTIPOINT)
		{	
			_points.insert( _points.begin() + PointIndex, pointEx(x,y));			
			_boundsChanged = true;
			return true;
		}
		else
		{
			_points.insert( _points.begin() + PointIndex, pointEx(x,y));			
			_pointsZ.insert (_pointsZ.begin() + PointIndex, z);
			_pointsM.insert (_pointsM.begin() + PointIndex, m);
			_boundsChanged = true;
			return true;
		}
	}
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

// ********************************************************
//		get_ContentLength()
// ********************************************************
int CShapeWrapper::get_ContentLength()
{
	return CShapeWrapper::get_ContentLength(this->_ShapeType, this->get_PointCount(), this->get_PartCount());
}

#pragma endregion

#pragma region Static functions
// **************************************************************
//		get_ContentLength 
// **************************************************************
int CShapeWrapper::get_ContentLength(ShpfileType shptype,  int numPoints, int numParts)
{
	int contentLength;
		if( shptype == SHP_NULLSHAPE )
			contentLength = sizeof(int);		// type is stored
		else if( shptype == SHP_POINT )
		{	
			contentLength = sizeof(int) +
							sizeof(double)*2;
		}
		else if( shptype == SHP_POINTZ )
		{	
			contentLength = sizeof(int) +
							sizeof(double)*4;
		}
		else if( shptype == SHP_POINTM )
		{	
			contentLength = sizeof(int) +
							sizeof(double)*3;							
		}
		else if( shptype == SHP_POLYLINE )
		{	contentLength = sizeof(int) + 
							sizeof(double)*4 + 
							sizeof(int) + 
							sizeof(int) +
							sizeof(int)*numParts +
							sizeof(double)*2*numPoints;
		}
		else if( shptype == SHP_POLYLINEZ )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 +
							sizeof(int) +
							sizeof(int) +
							sizeof(int)*numParts + 
							sizeof(double)*2*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints;
		}
		else if( shptype == SHP_POLYLINEM )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 +
							sizeof(int) +
							sizeof(int) +
							sizeof(int)*numParts + 
							sizeof(double)*2*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints;
		}
		else if( shptype == SHP_POLYGON )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 + 
							sizeof(int) + 
							sizeof(int) +
							sizeof(int)*numParts +
							sizeof(double)*2*numPoints;
		}
		else if( shptype == SHP_POLYGONZ )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 +
							sizeof(int) +
							sizeof(int) +
							sizeof(int)*numParts + 
							sizeof(double)*2*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints;
		}
		else if( shptype == SHP_POLYGONM )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 +
							sizeof(int) +
							sizeof(int) +
							sizeof(int)*numParts + 
							sizeof(double)*2*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints;
		}
		else if( shptype == SHP_MULTIPOINT )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 +
							sizeof(int) +
							sizeof(double)*2*numPoints;
		}
		else if( shptype == SHP_MULTIPOINTZ )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 +
							sizeof(int) +
							sizeof(double)*2*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints;
		}
		else if( shptype == SHP_MULTIPOINTM )
		{	
			contentLength = sizeof(int) + 
							sizeof(double)*4 +
							sizeof(int) +
							sizeof(double)*2*numPoints +
							sizeof(double)*2 +
							sizeof(double)*numPoints;							
		}
		else
		{
			return 0;
		}
		return contentLength;
}

// **************************************************************
//		ShapeTypeConvert2D()
// **************************************************************
ShpfileType CShapeWrapper::ShapeTypeConvert2D(ShpfileType shpType)
{
	if		(shpType == SHP_NULLSHAPE)																return SHP_NULLSHAPE;
	else if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ)				return SHP_POINT;
	else if	(shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ)	return SHP_MULTIPOINT;
	else if (shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)			return SHP_POLYGON;
	else if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)		return SHP_POLYLINE;
	else																							return SHP_NULLSHAPE;
}

#pragma endregion

#pragma region Utilities

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
#pragma endregion
