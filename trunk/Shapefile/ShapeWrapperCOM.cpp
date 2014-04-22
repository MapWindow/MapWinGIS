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
 // Sergei Leschinski 03-feb-2011 - created the file
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
		_ShapeType = shpType;
		_ShapeType2D = ShapeTypeConvert2D(_ShapeType);
		
		// specific _ShapeType settings
		if( _ShapeType == SHP_NULLSHAPE )
		{	
			for( unsigned int i = 0; i < _allPoints.size(); i++ )
			{	
				_allPoints[i]->Release();
				_allPoints[i] = NULL;
			}
			_allPoints.clear();
			_allParts.clear();
		}
		else if( _ShapeType == SHP_POINT || _ShapeType == SHP_POINTZ || _ShapeType == SHP_POINTM )
		{	
			for( unsigned int i = 1; i < _allPoints.size(); i++ )
			{	
				_allPoints[i]->Release();
				_allPoints[i] = NULL;
			}
			_allParts.clear();
		}
		else if( _ShapeType == SHP_POLYLINE || _ShapeType == SHP_POLYLINEZ || _ShapeType == SHP_POLYLINEM )
		{	
			if( _allParts.size() <= 0 )
			{
				_allParts.push_back(0);
			}
		}	
		else if( _ShapeType == SHP_POLYGON || _ShapeType == SHP_POLYGONZ || _ShapeType == SHP_POLYGONM )
		{	
			if( _allParts.size() <= 0 )
			{
				_allParts.push_back(0);
			}
		}
		else if( _ShapeType == SHP_MULTIPOINT || _ShapeType == SHP_MULTIPOINTZ || _ShapeType == SHP_MULTIPOINTM )
		{	
			_allParts.clear();
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
	if (_boundsChanged)
	{
		if (_ShapeType2D == SHP_NULLSHAPE)
		{
			_xMin = _xMax = _yMin = _yMax = 0.0;
		}
		else if (_ShapeType2D == SHP_POINT)
		{
			_allPoints[0]->get_X(&_xMin);
			_allPoints[0]->get_Y(&_yMin);
			_xMax = _xMin;
			_yMax = _yMin;
		}
		else
		{
			double x, y;
			for( int i = 0; i < (int)_allPoints.size(); i++ )
			{	
				if( i == 0 )
				{	
					_allPoints[0]->get_X(&_xMin);
					_allPoints[0]->get_Y(&_yMin);
					_xMax = _xMin;
					_yMax = _yMin;
				}
				else
				{	
					_allPoints[i]->get_X(&x);
					_allPoints[i]->get_Y(&y);

					if		( x < _xMin )	_xMin = x;
					else if ( x > _xMax )	_xMax = x;
					if		( y < _yMin )	_yMin = y;
					else if	( y > _yMax )	_yMax = y;
				}
			}
		}
		_boundsChanged = false;
	}
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
	if (_ShapeType2D == SHP_NULLSHAPE)
	{
		_xMin = _xMax = _yMin = _yMax =	_zMin = _zMax = _mMin = _mMax = 0.0;
	}
	else if (_ShapeType2D == SHP_POINT)
	{
		_allPoints[0]->get_X(&_xMin);
		_allPoints[0]->get_Y(&_yMin);
		_allPoints[0]->get_Z(&_zMin);
		_allPoints[0]->get_M(&_mMin);
		_xMax = _xMin;
		_yMax = _yMin;
		_zMax = _zMin;
		_mMax = _mMin;
	}
	else
	{
		double x, y, z, m;
		for( unsigned int i = 0; i < _allPoints.size(); i++ )
		{	
			if( i == 0 )
			{	
				_allPoints[0]->get_X(&_xMin);
				_allPoints[0]->get_Y(&_yMin);
				_allPoints[0]->get_Z(&_zMin);
				_allPoints[0]->get_M(&_mMin);
				_xMax = _xMin;
				_yMax = _yMin;
				_zMax = _zMin;
				_mMax = _mMin;
			}
			else
			{	
				_allPoints[i]->get_X(&x);
				_allPoints[i]->get_Y(&y);
				_allPoints[i]->get_Z(&z);
				_allPoints[i]->get_M(&m);

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
	if( PointIndex < 0 || PointIndex >= (int)_allPoints.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_allPoints[PointIndex]->get_X(&x);
		_allPoints[PointIndex]->get_Y(&y);
		return true;
	}
}

// ********************************************************
//		get_PointsXY ()
// ********************************************************
void CShapeWrapperCOM::get_XYFast(int PointIndex, double& x, double& y)
{
	_allPoints[PointIndex]->get_X(&x);
	_allPoints[PointIndex]->get_Y(&y);
}

// ********************************************************
//		put_PointXY()
// ********************************************************
bool CShapeWrapperCOM::put_PointXY(int PointIndex, double x, double y)
{
	if( PointIndex < 0 || PointIndex >= (int)_allPoints.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_allPoints[PointIndex]->put_X(x);
		_allPoints[PointIndex]->put_Y(y);
		_boundsChanged = true;
		return true;
	}
}

// ********************************************************
//		get_PointZ()
// ********************************************************
bool CShapeWrapperCOM::get_PointZ(int PointIndex, double& z)
{
	if( PointIndex < 0 || PointIndex >= (int)_allPoints.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		_allPoints[PointIndex]->get_Z(&z);
		return true;
	}
}

// ********************************************************
//		put_PointZ()
// ********************************************************
bool CShapeWrapperCOM::put_PointZ(int PointIndex, double z)
{
	if( PointIndex < 0 || PointIndex >= (int)_allPoints.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_allPoints[PointIndex]->put_Z(z);
		return true;
	}
}

// ********************************************************
//		get_PointM()
// ********************************************************
bool CShapeWrapperCOM::get_PointM(int PointIndex, double& m)
{
	if( PointIndex < 0 || PointIndex >= (int)_allPoints.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		_allPoints[PointIndex]->get_M(&m);
		return true;
	}
}

// ********************************************************
//		put_PointM()
// ********************************************************
bool CShapeWrapperCOM::put_PointM(int PointIndex, double m)
{
	if( PointIndex < 0 || PointIndex >= (int)_allPoints.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_allPoints[PointIndex]->put_M(m);
		return true;
	}
}

// ********************************************************
//		get_Point()
// ********************************************************
IPoint* CShapeWrapperCOM::get_Point(long Index)
{
	if( Index < 0 || Index >= (long)_allPoints.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return NULL;
	}
	else
	{	
		_allPoints[Index]->AddRef();
		return _allPoints[Index];
	}
}

// ********************************************************
//		put_Point()
// ********************************************************
bool CShapeWrapperCOM::put_Point(long Index, IPoint* pnt)
{
	if( Index < 0 || Index >= (long)_allPoints.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	_allPoints[Index]->Release();
		_allPoints[Index] = pnt;			
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
	for( unsigned int i = 0; i < (int)_allPoints.size(); i++ )
	{	
		_allPoints[i]->Release();
	}
	_allPoints.clear();		
	_allParts.clear();
	_boundsChanged = true;
}

// ******************************************************
//		InsertPoint()
// ******************************************************
bool CShapeWrapperCOM::InsertPoint(int PointIndex, IPoint* point)
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
		else if( PointIndex > (int)_allPoints.size() )
		{
			PointIndex = _allPoints.size();
		}

		if( _ShapeType == SHP_POINT)
		{	
			if( _allPoints.size() != 0)
			{	
				return false;
			}
			else
			{
				point->AddRef();
				_allPoints.push_back(point);
				_boundsChanged = true;
				return true;
			}
		}
		else //(_ShapeType  == SHP_POLYLINE || _ShapeType == SHP_POLYGON || _ShapeType == SHP_MULTIPOINT)
		{	
			point->AddRef();
			_allPoints.insert( _allPoints.begin() + PointIndex, point );			
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
	
	m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
	//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
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
	m_factory.pointFactory->CreateInstance(NULL, IID_IPoint, (void**)&pnt);
	//CoCreateInstance(CLSID_Point,NULL,CLSCTX_INPROC_SERVER,IID_IPoint,(void**)&pnt);
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
	if( PointIndex < 0 || PointIndex >= (int)_allPoints.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_allPoints[PointIndex]->Release();
		_allPoints.erase( _allPoints.begin() + PointIndex );
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
		else if( PartIndex > (int)_allParts.size() )
		{
			PartIndex = _allParts.size();
		}
		
		// this part already exists
		for (unsigned int i = 0; i < _allParts.size(); i++)
		{
			if (_allParts[i] == PointIndex)
			{
				return true;	// returning true to preserve compatibility
			}
		}

		_allParts.insert( _allParts.begin() + PartIndex, PointIndex );
		return true;
	}
}

// **************************************************************
//		DeletePart()
// **************************************************************
bool CShapeWrapperCOM::DeletePart(int PartIndex)
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
	else if( PartIndex < 0 || PartIndex >= (int)_allParts.size() )
	{	
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{	
		_allParts.erase( _allParts.begin() + PartIndex );
		return true;
	}
}

// **********************************************************
//		get_allPartstartPoint()
// **********************************************************
int CShapeWrapperCOM::get_PartStartPoint(int PartIndex)
{
	if( PartIndex < 0 || PartIndex >= (int)_allParts.size())
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	else
	{
		return _allParts[PartIndex];
	}
}

// **********************************************************
//		get_PartEndPoint()
// **********************************************************
int CShapeWrapperCOM::get_PartEndPoint(int PartIndex)
{
	if( PartIndex < 0 || PartIndex >= (int)_allParts.size())
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	else
	{
		if (PartIndex == _allParts.size() - 1)
		{
			return _allPoints.size() - 1;
		}
		else
		{
			return _allParts[PartIndex + 1] - 1;
		}
	}
}

// **********************************************************
//		put_allPartstartPoint()
// **********************************************************
bool CShapeWrapperCOM::put_PartStartPoint(long PartIndex, long newVal)
{
	if( PartIndex < 0 || PartIndex >= (long)_allParts.size() )
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	else
	{
		_allParts[PartIndex] = newVal;
		return true;
	}
}
#pragma endregion

// **************************************************************
//		ShapeTypeConvert2D()
// **************************************************************
ShpfileType CShapeWrapperCOM::ShapeTypeConvert2D(ShpfileType shpType)
{
	if		(shpType == SHP_NULLSHAPE)																return SHP_NULLSHAPE;
	else if (shpType == SHP_POINT || shpType == SHP_POINTM || shpType == SHP_POINTZ)				return SHP_POINT;
	else if	(shpType == SHP_MULTIPOINT || shpType == SHP_MULTIPOINTM || shpType == SHP_MULTIPOINTZ)	return SHP_MULTIPOINT;
	else if (shpType == SHP_POLYGON || shpType == SHP_POLYGONM || shpType == SHP_POLYGONZ)			return SHP_POLYGON;
	else if (shpType == SHP_POLYLINE || shpType == SHP_POLYLINEM || shpType == SHP_POLYLINEZ)		return SHP_POLYLINE;
	else																							return SHP_NULLSHAPE;
}

