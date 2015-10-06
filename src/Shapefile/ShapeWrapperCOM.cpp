/**************************************************************************************
 * File name: ShapeWrapperCOM.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CShapeWrapperCOM class
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
 #include "stdafx.h"
#include "ShapeWrapperCOM.h"

#pragma region ShapeType

// ***************************************************************
//	   put_ShapeType()
// ***************************************************************
bool CShapeWrapperCOM::put_ShapeType(ShpfileType shpType)
{
	if( shpType == SHP_MULTIPATCH )
	{	
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		return false;
	}
	else
	{	
		_shapeType = shpType;
		
		if( _shapeType == SHP_NULLSHAPE )
		{	
			for( unsigned int i = 0; i < _points.size(); i++ )
			{	
				_points[i]->Release();
				_points[i] = NULL;
			}
			_points.clear();
			_parts.clear();
		}
		else if( _shapeType == SHP_POINT || _shapeType == SHP_POINTZ || _shapeType == SHP_POINTM )
		{	
			for( unsigned int i = 1; i < _points.size(); i++ )
			{	
				_points[i]->Release();
				_points[i] = NULL;
			}
			_parts.clear();
		}
		else if( _shapeType == SHP_POLYLINE || _shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYLINEM )
		{	
			if( _parts.size() <= 0 )
			{
				_parts.push_back(0);
			}
		}	
		else if( _shapeType == SHP_POLYGON || _shapeType == SHP_POLYGONZ || _shapeType == SHP_POLYGONM )
		{	
			if( _parts.size() <= 0 )
			{
				_parts.push_back(0);
			}
		}
		else if( _shapeType == SHP_MULTIPOINT || _shapeType == SHP_MULTIPOINTZ || _shapeType == SHP_MULTIPOINTM )
		{	
			_parts.clear();
		}
		return true;
	}
}

#pragma endregion

#pragma region Bounds

// *********************************************************
//	   RefreshBounds()
// *********************************************************
void CShapeWrapperCOM::RefreshBoundsXY()
{
	if (!_boundsChanged) return;

	ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);

	if (shpType2D == SHP_NULLSHAPE)
	{
		_xMin = _xMax = _yMin = _yMax = 0.0;
	}
	else if (shpType2D == SHP_POINT)
	{
		_points[0]->get_X(&_xMin);
		_points[0]->get_Y(&_yMin);
		_xMax = _xMin;
		_yMax = _yMin;
	}
	else
	{
		double x, y;
		for (int i = 0; i < (int)_points.size(); i++)
		{
			if (i == 0)
			{
				_points[0]->get_X(&_xMin);
				_points[0]->get_Y(&_yMin);
				_xMax = _xMin;
				_yMax = _yMin;
			}
			else
			{
				_points[i]->get_X(&x);
				_points[i]->get_Y(&y);

				if (x < _xMin)	_xMin = x;
				else if (x > _xMax)	_xMax = x;
				if (y < _yMin)	_yMin = y;
				else if (y > _yMax)	_yMax = y;
			}
		}
	}

	_boundsChanged = false;
}

// ********************************************************
//		get_XYBounds 
// ********************************************************
bool CShapeWrapperCOM::get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax)
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

// **********************************************************
//		RefreshBounds()
// **********************************************************
void CShapeWrapperCOM::RefreshBounds()
{
	ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);

	if (shpType2D == SHP_NULLSHAPE || _points.size() == 0)
	{
		_xMin = _xMax = _yMin = _yMax =	_zMin = _zMax = _mMin = _mMax = 0.0;
	}
	else if (shpType2D == SHP_POINT)
	{
		_points[0]->get_X(&_xMin);
		_points[0]->get_Y(&_yMin);
		_points[0]->get_Z(&_zMin);
		_points[0]->get_M(&_mMin);
		_xMax = _xMin;
		_yMax = _yMin;
		_zMax = _zMin;
		_mMax = _mMin;
	}
	else
	{
		double x, y, z, m;
		for( unsigned int i = 0; i < _points.size(); i++ )
		{	
			if( i == 0 )
			{	
				_points[0]->get_X(&_xMin);
				_points[0]->get_Y(&_yMin);
				_points[0]->get_Z(&_zMin);
				_points[0]->get_M(&_mMin);
				_xMax = _xMin;
				_yMax = _yMin;
				_zMax = _zMin;
				_mMax = _mMin;
			}
			else
			{	
				_points[i]->get_X(&x);
				_points[i]->get_Y(&y);
				_points[i]->get_Z(&z);
				_points[i]->get_M(&m);

				if		( x < _xMin )	_xMin = x;
				else if ( x > _xMax )	_xMax = x;
				if		( y < _yMin )	_yMin = y;
				else if	( y > _yMax )	_yMax = y;
				if		( z < _zMin )	_zMin = z;
				else if ( z > _zMax )	_zMax = z;
				if		( m < _mMin )	_mMin = m;
				else if	( m > _mMax )	_mMax = m;
			}
		}
	}

	_boundsChanged = false;
}

// **********************************************************
//		get_Bounds()
// **********************************************************
bool CShapeWrapperCOM::get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
								  double& zMin, double& zMax, double& mMin, double& mMax)
{
	if (_boundsChanged)
	{
		RefreshBounds();
	}
		
	xMin = _xMin;
	xMax = _xMax;
	yMin = _yMin;
	yMax = _yMax;
	zMin = _zMin;
	zMax = _zMax;
	mMin = _mMin;
	mMax = _mMax;
		
	return true;
}
#pragma endregion

#pragma region Points

// ********************************************************
//		get_PointsXY ()
// ********************************************************
bool CShapeWrapperCOM::get_PointXY(int PointIndex, double& x, double& y)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_points[PointIndex]->get_X(&x);
		_points[PointIndex]->get_Y(&y);
		return true;
	}
}

// ********************************************************
//		get_PointsXY ()
// ********************************************************
void CShapeWrapperCOM::get_XYFast(int PointIndex, double& x, double& y)
{
	_points[PointIndex]->get_X(&x);
	_points[PointIndex]->get_Y(&y);
}

// ********************************************************
//		put_PointXY()
// ********************************************************
bool CShapeWrapperCOM::put_PointXY(int PointIndex, double x, double y)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_points[PointIndex]->put_X(x);
		_points[PointIndex]->put_Y(y);
		_boundsChanged = true;
		return true;
	}
}

// ********************************************************
//		get_PointZ()
// ********************************************************
bool CShapeWrapperCOM::get_PointZ(int PointIndex, double& z)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		_points[PointIndex]->get_Z(&z);
		return true;
	}
}

// ********************************************************
//		put_PointZ()
// ********************************************************
bool CShapeWrapperCOM::put_PointZ(int PointIndex, double z)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_points[PointIndex]->put_Z(z);
		return true;
	}
}

// ********************************************************
//		get_PointM()
// ********************************************************
bool CShapeWrapperCOM::get_PointM(int PointIndex, double& m)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		_points[PointIndex]->get_M(&m);
		return true;
	}
}

// ********************************************************
//		put_PointM()
// ********************************************************
bool CShapeWrapperCOM::put_PointM(int PointIndex, double m)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_points[PointIndex]->put_M(m);
		return true;
	}
}

// ********************************************************
//		get_Point()
// ********************************************************
IPoint* CShapeWrapperCOM::get_Point(long Index)
{
	if( Index < 0 || Index >= (long)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return NULL;
	}
	else
	{	
		_points[Index]->AddRef();
		return _points[Index];
	}
}

// ********************************************************
//		put_Point()
// ********************************************************
bool CShapeWrapperCOM::put_Point(long Index, IPoint* pnt)
{
	if( Index < 0 || Index >= (long)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	_points[Index]->Release();
		_points[Index] = pnt;			
		return true;
	}
}
#pragma endregion

#pragma region EditingPoints
// ******************************************************
//		Clear()
// ******************************************************
void CShapeWrapperCOM::Clear()
{
	for( unsigned int i = 0; i < (int)_points.size(); i++ )
	{	
		_points[i]->Release();
	}
	_points.clear();		
	_parts.clear();
	_boundsChanged = true;
}

// ******************************************************
//		InsertPoint()
// ******************************************************
bool CShapeWrapperCOM::InsertPoint(int PointIndex, IPoint* point)
{
	if( _shapeType == SHP_NULLSHAPE )
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

		if( _shapeType == SHP_POINT)
		{	
			if( _points.size() != 0)
			{	
				return false;
			}
			else
			{
				point->AddRef();
				_points.push_back(point);
				_boundsChanged = true;
				return true;
			}
		}
		else //(_ShapeType  == SHP_POLYLINE || _ShapeType == SHP_POLYGON || _ShapeType == SHP_MULTIPOINT)
		{	
			point->AddRef();
			_points.insert( _points.begin() + PointIndex, point );			
			_boundsChanged = true;
			return true;
		}
	}
}

// ******************************************************
//		InsertPointXY()
// ******************************************************
bool CShapeWrapperCOM::InsertPointXY(int PointIndex, double x, double y)
{
	IPoint* pnt = NULL;
	
	ComHelper::CreatePoint(&pnt);
	if (pnt)
	{
		pnt->put_X(x);
		pnt->put_Y(y);
		bool result = this->InsertPoint(PointIndex, pnt);
		pnt->Release();
		return result;
	}
	else
	{
		return false;
	}
}

// ******************************************************
//		InsertPointXYZM()
// ******************************************************
bool CShapeWrapperCOM::InsertPointXYZM(int PointIndex, double x, double y, double z, double m)
{
	IPoint* pnt = NULL;
	ComHelper::CreatePoint(&pnt);
	if (pnt)
	{
		pnt->put_X(x);
		pnt->put_Y(y);
		pnt->put_Z(z);
		pnt->put_M(m);
		return this->InsertPoint(PointIndex, pnt);
	}
	else
	{
		return false;
	}
}

// ********************************************************
//		DeletePoint()
// ********************************************************
bool CShapeWrapperCOM::DeletePoint(int PointIndex)
{
	if( PointIndex < 0 || PointIndex >= (int)_points.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_points[PointIndex]->Release();
		_points.erase( _points.begin() + PointIndex );
		_boundsChanged = true;
		return true;
	}
}
#pragma endregion

#pragma region Parts

// **********************************************************
//		InsertPart()
// **********************************************************
bool CShapeWrapperCOM::InsertPart(int PartIndex, int PointIndex)
{
	if( _shapeType != SHP_POLYLINE &&
		_shapeType != SHP_POLYLINEZ &&
		_shapeType != SHP_POLYLINEM &&
		_shapeType != SHP_POLYGON &&
		_shapeType != SHP_POLYGONZ &&
		_shapeType != SHP_POLYGONM )
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
bool CShapeWrapperCOM::DeletePart(int PartIndex)
{
	if( _shapeType != SHP_POLYLINE &&
		_shapeType != SHP_POLYLINEZ &&
		_shapeType != SHP_POLYLINEM &&
		_shapeType != SHP_POLYGON &&
		_shapeType != SHP_POLYGONZ &&
		_shapeType != SHP_POLYGONM )
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
//		get_allPartstartPoint()
// **********************************************************
int CShapeWrapperCOM::get_PartStartPoint(int PartIndex)
{
	if( PartIndex < 0 || PartIndex >= (int)_parts.size())
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	else
	{
		return _parts[PartIndex];
	}
}

// **********************************************************
//		get_PartEndPoint()
// **********************************************************
int CShapeWrapperCOM::get_PartEndPoint(int PartIndex)
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
bool CShapeWrapperCOM::put_PartStartPoint(long PartIndex, long newVal)
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

// **************************************************************
//		ReversePoints()
// **************************************************************
void CShapeWrapperCOM::ReversePoints(long startIndex, long endIndex)
{
	vector<IPoint *>::iterator iter1 = _points.begin();
	vector<IPoint *>::iterator iter2 = _points.begin();
	iter1 += startIndex;
	iter2 += endIndex;
	reverse(iter1, iter2);
}

// **************************************************************
//		put_RawData 
// **************************************************************
bool CShapeWrapperCOM::put_RawData(char* shapeData)
{
	_points.clear();
	_parts.clear();

	_shapeType = (ShpfileType)*(int*)shapeData;
	ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);

	_boundsChanged = true;

	int numPoints = 0;
	int numParts = 0;

	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
	}
	else if (shpType2D == SHP_POINT)
	{
		numPoints = 1;
		double* ddata = (double*)(shapeData + 4);

		IPoint* pnt = NULL;
		ComHelper::CreatePoint(&pnt);
		pnt->put_X(ddata[0]);
		pnt->put_Y(ddata[1]);

		if (_shapeType == SHP_POINTM)
		{
			pnt->put_M(ddata[2]);
		}
		else if (_shapeType == SHP_POINTZ)
		{
			pnt->put_M(ddata[2]);
			pnt->put_Z(ddata[3]);
		}

		_points.push_back(pnt);
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
		for (int i = 0; i < numPoints; i++) 
		{
			IPoint* pnt = NULL;
			ComHelper::CreatePoint(&pnt);
			pnt->put_X(points[i * 2]);
			pnt->put_Y(points[i * 2 + 1]);
			_points[i] = pnt;
		}

		// z values
		if (_shapeType == SHP_MULTIPOINTZ || _shapeType == SHP_MULTIPOINTM)
		{
			points += numPoints * 2;
			_mMin = *(points);
			_mMax = *(points + 1);

			points += 2;
			
			for (int i = 0; i < numPoints; i++) {
				_points[i]->put_M(points[i]);
			}

			if (_shapeType == SHP_MULTIPOINTZ)
			{
				points += numPoints;
				_zMin = *(points);
				_zMax = *(points + 1);
				points += 2;

				for (int i = 0; i < numPoints; i++) {
					_points[i]->put_Z(points[i]);
				}
			}
		}
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
			for (int i = 0; i < numPoints; i++)
			{
				IPoint* pnt = NULL;
				ComHelper::CreatePoint(&pnt);
				pnt->put_X(points[i * 2]);
				pnt->put_Y(points[i * 2 + 1]);
				_points[i] = pnt;
			}
		}

		// z values
		if (_shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYGONZ ||
			_shapeType == SHP_POLYLINEM || _shapeType == SHP_POLYGONM)
		{
			points += numPoints * 2;
			_mMin = *(points);
			_mMax = *(points + 1);
			points += 2;

			if (numPoints > 0)
			{
				for (int i = 0; i < numPoints; i++) {
					_points[i]->put_M(points[i]);
				}
			}

			// m values
			if (_shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYGONZ)
			{
				points += numPoints;
				_zMin = *(points);
				_zMax = *(points + 1);
				points += 2;

				if (numPoints > 0)
				{
					for (int i = 0; i < numPoints; i++) {
						_points[i]->put_Z(points[i]);
					}
				}
			}
		}
	}
	return true;
}

// **************************************************************
//		get_RawData 
// **************************************************************
// Forming the data to write to the disk. Should be optimized as far as possible.
int* CShapeWrapperCOM::get_RawData()
{
	int numPoints = _points.size();
	int numParts = _parts.size();

	int length = ShapeUtility::get_ContentLength(_shapeType, numPoints, numParts);
	int* intdata = new int[length / 4];
	intdata[0] = (int)_shapeType;

	double* ddata;

	ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);

	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
	}
	else if (shpType2D == SHP_POINT)
	{
		if (_points.size() == 0)
		{
			intdata[0] = (int)SHP_NULLSHAPE;
		}
		else
		{
			ddata = (double*)&intdata[1];
			
			_points[0]->get_X(&ddata[0]);
			_points[0]->get_Y(&ddata[1]);

			if (_shapeType == SHP_POINTM)
			{
				_points[0]->get_M(&ddata[2]);
			}
			else if (_shapeType == SHP_POINTZ)
			{
				_points[0]->get_M(&ddata[2]);
				_points[0]->get_Z(&ddata[3]);
			}
		}
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

		for (int i = 0; i < numPoints; i++)
		{
			_points[i]->get_X(&ddata[i * 2]);
			_points[i]->get_Y(&ddata[i * 2 + 1]);
		}

		// z values
		if (_shapeType == SHP_MULTIPOINTZ || _shapeType == SHP_MULTIPOINTM)
		{
			ddata += numPoints * 2;
			ddata[0] = _mMin;
			ddata[1] = _mMax;
			ddata += 2;

			for (int i = 0; i < numPoints; i++)
			{
				_points[i]->get_M(&ddata[i]);
			}

			// m values
			if (_shapeType == SHP_MULTIPOINTZ)
			{
				ddata += numPoints;
				ddata[0] = _zMin;
				ddata[1] = _zMax;
				ddata += 2;

				for (int i = 0; i < numPoints; i++)
				{
					_points[i]->get_Z(&ddata[i]);
				}
			}
		}
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
		if (numPoints > 0)
		{
			for (int i = 0; i < numPoints; i++)
			{
				_points[i]->get_X(&ddata[i * 2]);
				_points[i]->get_Y(&ddata[i * 2 + 1]);
			}

			// z values
			if (_shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYGONZ ||
				_shapeType == SHP_POLYLINEM || _shapeType == SHP_POLYGONM)
			{
				ddata += numPoints * 2;
				ddata[0] = _mMin;
				ddata[1] = _mMax;
				ddata += 2;

				for (int i = 0; i < numPoints; i++)
				{
					_points[i]->get_M(&ddata[i]);
				}

				// m values
				if (_shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYGONZ)
				{
					ddata += numPoints;
					ddata[0] = _zMin;
					ddata[1] = _zMax;
					ddata += 2;

					for (int i = 0; i < numPoints; i++)
					{
						_points[i]->get_Z(&ddata[i]);
					}
				}
			}
		}
	}

	return intdata;
}
