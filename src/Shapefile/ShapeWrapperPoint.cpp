/**************************************************************************************
 * File name: CShapeWrapperPoint.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CShapeWrapperPoint
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
#include "ShapeWrapperPoint.h"

// **************************************************************
//		put_ShapeType()
// **************************************************************
bool CShapeWrapperPoint::put_ShapeType(ShpfileType newVal)
{	
	if (newVal == SHP_POINT || newVal == SHP_POINTZ || newVal == SHP_POINTM)
	{
		_shpType = newVal;
		_lastErrorCode = tkUNEXPECTED_SHAPE_TYPE;
		return true;
	}

	return false;
}

// ********************************************************
//		get_PointsXY 
// ********************************************************
bool CShapeWrapperPoint::get_PointXY(int PointIndex, double& x, double& y)
{
	if (_initialized) {
		x = _x;
		y = _y;
	}

	return _initialized;
}

// ********************************************************
//		get_XYFast 
// ********************************************************
void CShapeWrapperPoint::get_XYFast(int PointIndex, double& x, double& y)
{
	get_PointXY(PointIndex, x, y);
}

// **********************************************************
//		put_PointXY()
// **********************************************************
bool CShapeWrapperPoint::put_PointXY(int PointIndex, double x, double y)
{
	if (_initialized) {
		_x = x;
		_y = y;
	}
	return _initialized;
}

// **********************************************************
//		put_PointZ()
// **********************************************************
bool CShapeWrapperPoint::put_PointZ(int PointIndex, double z)
{
	if (_initialized) {
		_z = z;
	}
	return _initialized;
}

// **********************************************************
//		put_PointM()
// **********************************************************
bool CShapeWrapperPoint::put_PointM(int PointIndex, double m)
{
	if (_initialized) {
		_m = m;
	}
	return _initialized;
}

// **********************************************************
//		get_PointZ()
// **********************************************************
bool CShapeWrapperPoint::get_PointZ(int PointIndex, double& z)
{
	if (_initialized) {
		z = _z;
	}

	return _initialized;
}

// **********************************************************
//		get_PointM()
// **********************************************************
bool CShapeWrapperPoint::get_PointM(int PointIndex, double& m)
{
	if (_initialized) {
		m = _m;
	}

	return _initialized;
}

// **********************************************************
//		get_Point()
// **********************************************************
IPoint* CShapeWrapperPoint::get_Point(long Index)
{
	if (!_initialized) return NULL;

	IPoint* pnt = NULL;
	ComHelper::CreatePoint(&pnt);

	if (pnt)
	{
		pnt->put_X(_x);
		pnt->put_Y(_y);
		pnt->put_Z(_z);
		pnt->put_M(_m);
	}

	return pnt;
}

// **********************************************************
//		put_Point()
// **********************************************************
bool CShapeWrapperPoint::put_Point(long Index, IPoint* pnt)
{
	if (!_initialized) return false;

	if (!pnt)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return false;
	}

	pnt->get_X(&_x);
	pnt->get_Y(&_y);
	pnt->get_Z(&_z);
	pnt->get_M(&_m);
	return true;
}
#pragma endregion

#pragma region Bounds

// ********************************************************
//		get_XYBounds 
// ********************************************************
bool CShapeWrapperPoint::get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax)
{
	if (_initialized) {
		xMin = _x;
		xMax = _x;
		yMin = _y;
		yMax = _y;
	}

	return _initialized;
}

// **********************************************************
//		get_Bounds
// **********************************************************
bool CShapeWrapperPoint::get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
							   double& zMin, double& zMax, double& mMin, double& mMax)
{
	if (_initialized) {
		xMin = _x;
		xMax = _x;
		yMin = _y;
		yMax = _y;
		zMin = _z;
		zMax = _z;
		mMin = _m;
		mMax = _m;
	}

	return _initialized;
}

#pragma endregion

#pragma region Shapedata

// **************************************************************
//		put_Data 
// **************************************************************
// Passing the shape data from disk to the memory structures.
bool CShapeWrapperPoint::put_RawData(char* shapeData)
{
	ShpfileType shpType = (ShpfileType)*(int*)shapeData;
		
	if (!put_ShapeType(shpType)) return false;

	double* ddata = (double*)(shapeData + 4);
	
	_x = ddata[0];
	_y = ddata[1];

	if (shpType == SHP_POINTM)
	{
		_m = ddata[2];
	}
	else if (shpType == SHP_POINTZ)
	{
		_m = ddata[2];
		_z = ddata[3];
	}

	_initialized = true;

	return true;
}

// **************************************************************
//		get_Data 
// **************************************************************
// Forming the data to write to the disk. Should be optimized as far as possible.
int* CShapeWrapperPoint::get_RawData(void)
{
	int length =  ShapeUtility::get_ContentLength(_shpType, 1, 0);
	int* intdata = new int[length/4];
	intdata[0] = (int)_shpType;
	
	double* ddata;

	ddata = (double*)&intdata[1];
	ddata[0] = _x;
	ddata[1] = _y;

	if (_shpType == SHP_POINTM)
	{
		ddata[2] = _m;
	}
	else if (_shpType == SHP_POINTZ)
	{
		ddata[2] = _m;
		ddata[3] = _z;
	}
	
	return intdata;
}

#pragma endregion

#pragma region PointEditing

// ******************************************************
//		InsertPoint()
// ******************************************************
bool CShapeWrapperPoint::InsertPoint(int PointIndex, IPoint* pnt)
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
bool CShapeWrapperPoint::InsertPointXY(int PointIndex, double x, double y)
{
	return InsertPointXYZM(PointIndex, x, y, 0.0, 0.0);
}

// ******************************************************
//		InsertPointXYZM()
// ******************************************************
bool CShapeWrapperPoint::InsertPointXYZM(int PointIndex, double x, double y, double z, double m)
{
	if (!_initialized) {
		_x = x;
		_y = y;
		_z = z;
		_m = m;
		_initialized = true;
		return true;
	}

	return false;
}

// ********************************************************
//		DeletePoint()
// ********************************************************
bool CShapeWrapperPoint::DeletePoint(int PointIndex)
{
	if (_initialized) {
		_initialized = false;
		return true;
	}

	return false;
}

// ********************************************************
//		get_ContentLength()
// ********************************************************
int CShapeWrapperPoint::get_ContentLength()
{
	return ShapeUtility::get_ContentLength(_shpType, this->get_PointCount(), this->get_PartCount());
}

// ********************************************************
//		get_PointXYZM()
// ********************************************************
bool CShapeWrapperPoint::get_PointXYZM(int pointIndex, double& x, double& y, double& z, double& m)
{
	if (_initialized) {
		x = _x;
		y = _y;
		z = _z;
		m = _m;
		return true;
	}

	return false;
}

#pragma endregion
