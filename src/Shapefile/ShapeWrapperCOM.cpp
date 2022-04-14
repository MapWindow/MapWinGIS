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

#include <gsl/util>

#pragma region ShapeType

 // ***************************************************************
 //	   put_ShapeType()
 // ***************************************************************
bool CShapeWrapperCOM::put_ShapeType(const ShpfileType shpType)
{
	if (shpType == SHP_MULTIPATCH)
	{
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		return false;
	}
	_shapeType = shpType;

	if (_shapeType == SHP_NULLSHAPE)
	{
		for (auto& point : _points)
		{
			point->Release();
			point = nullptr;
		}
		_points.clear();
		_parts.clear();
	}
	else if (_shapeType == SHP_POINT || _shapeType == SHP_POINTZ || _shapeType == SHP_POINTM)
	{
		for (auto& point : _points)
		{
			point->Release();
			point = nullptr;
		}
		_parts.clear();
	}
	else if (_shapeType == SHP_POLYLINE || _shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYLINEM)
	{
		if (_parts.empty())
		{
			_parts.push_back(0);
		}
	}
	else if (_shapeType == SHP_POLYGON || _shapeType == SHP_POLYGONZ || _shapeType == SHP_POLYGONM)
	{
		if (_parts.empty())
		{
			_parts.push_back(0);
		}
	}
	else if (_shapeType == SHP_MULTIPOINT || _shapeType == SHP_MULTIPOINTZ || _shapeType == SHP_MULTIPOINTM)
	{
		_parts.clear();
	}
	return true;
}

#pragma endregion

#pragma region Bounds

// *********************************************************
//	   RefreshBounds()
// *********************************************************
void CShapeWrapperCOM::RefreshBoundsXY()
{
	if (!_boundsChanged) return;

	const ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);

	if (shpType2D == SHP_NULLSHAPE)
	{
		_xMin = _xMax = _yMin = _yMax = 0.0;
	}
	else if (shpType2D == SHP_POINT)
	{
		gsl::at(_points, 0)->get_X(&_xMin);
		gsl::at(_points, 0)->get_Y(&_yMin);
		_xMax = _xMin;
		_yMax = _yMin;
	}
	else
	{
		double x, y;
		for (int i = 0; i < gsl::narrow_cast<int>(_points.size()); i++)
		{
			if (i == 0)
			{
				gsl::at(_points, 0)->get_X(&_xMin);
				gsl::at(_points, 0)->get_Y(&_yMin);
				_xMax = _xMin;
				_yMax = _yMin;
			}
			else
			{
				gsl::at(_points, i)->get_X(&x);
				gsl::at(_points, i)->get_Y(&y);

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
	const ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);

	if (shpType2D == SHP_NULLSHAPE || _points.empty())
	{
		_xMin = _xMax = _yMin = _yMax = _zMin = _zMax = _mMin = _mMax = 0.0;
	}
	else if (shpType2D == SHP_POINT)
	{
		gsl::at(_points, 0)->get_X(&_xMin);
		gsl::at(_points, 0)->get_Y(&_yMin);
		gsl::at(_points, 0)->get_Z(&_zMin);
		gsl::at(_points, 0)->get_M(&_mMin);
		_xMax = _xMin;
		_yMax = _yMin;
		_zMax = _zMin;
		_mMax = _mMin;
	}
	else
	{
		double x, y, z, m;
		const int pointSize = gsl::narrow_cast<int>(_points.size());
		for (int i = 0; i < pointSize; i++)
		{
			if (i == 0)
			{
				gsl::at(_points, 0)->get_X(&_xMin);
				gsl::at(_points, 0)->get_Y(&_yMin);
				gsl::at(_points, 0)->get_Z(&_zMin);
				gsl::at(_points, 0)->get_M(&_mMin);
				_xMax = _xMin;
				_yMax = _yMin;
				_zMax = _zMin;
				_mMax = _mMin;
			}
			else
			{
				gsl::at(_points, i)->get_X(&x);
				gsl::at(_points, i)->get_Y(&y);
				gsl::at(_points, i)->get_Z(&z);
				gsl::at(_points, i)->get_M(&m);

				if (x < _xMin)	_xMin = x;
				else if (x > _xMax)	_xMax = x;
				if (y < _yMin)	_yMin = y;
				else if (y > _yMax)	_yMax = y;
				if (z < _zMin)	_zMin = z;
				else if (z > _zMax)	_zMax = z;
				if (m < _mMin)	_mMin = m;
				else if (m > _mMax)	_mMax = m;
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
bool CShapeWrapperCOM::get_PointXY(const int pointIndex, double& x, double& y)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	gsl::at(_points, pointIndex)->get_X(&x);
	gsl::at(_points, pointIndex)->get_Y(&y);
	return true;
}

// ********************************************************
//		get_PointXYZM ()
// ********************************************************
bool CShapeWrapperCOM::get_PointXYZM(const int pointIndex, double& x, double& y, double& z, double& m)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	gsl::at(_points, pointIndex)->get_X(&x);
	gsl::at(_points, pointIndex)->get_Y(&y);
	gsl::at(_points, pointIndex)->get_Z(&z);
	gsl::at(_points, pointIndex)->get_M(&m);
	return true;
}

// ********************************************************
//		get_PointsXY ()
// ********************************************************
void CShapeWrapperCOM::get_XYFast(const int pointIndex, double& x, double& y)
{
	gsl::at(_points, pointIndex)->get_X(&x);
	gsl::at(_points, pointIndex)->get_Y(&y);
}

// ********************************************************
//		put_PointXY()
// ********************************************************
bool CShapeWrapperCOM::put_PointXY(const int pointIndex, const double x, const double y)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	gsl::at(_points, pointIndex)->put_X(x);
	gsl::at(_points, pointIndex)->put_Y(y);
	_boundsChanged = true;
	return true;
}

// ********************************************************
//		get_PointZ()
// ********************************************************
bool CShapeWrapperCOM::get_PointZ(const int pointIndex, double& z)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	gsl::at(_points, pointIndex)->get_Z(&z);
	return true;
}

// ********************************************************
//		put_PointZ()
// ********************************************************
bool CShapeWrapperCOM::put_PointZ(const int pointIndex, const double z)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	gsl::at(_points, pointIndex)->put_Z(z);
	return true;
}

// ********************************************************
//		get_PointM()
// ********************************************************
bool CShapeWrapperCOM::get_PointM(const int pointIndex, double& m)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	gsl::at(_points, pointIndex)->get_M(&m);
	return true;
}

// ********************************************************
//		put_PointM()
// ********************************************************
bool CShapeWrapperCOM::put_PointM(const int pointIndex, const double m)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	gsl::at(_points, pointIndex)->put_M(m);
	return true;
}

// ********************************************************
//		get_Point()
// ********************************************************
IPoint* CShapeWrapperCOM::get_Point(const long index)
{
	if (index < 0 || index >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return nullptr;
	}
	_points[index]->AddRef();
	return _points[index];
}

// ********************************************************
//		put_Point()
// ********************************************************
bool CShapeWrapperCOM::put_Point(const long index, IPoint* pnt)
{
	if (index < 0 || index >= gsl::narrow_cast<long>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	_points[index]->Release();
	_points[index] = pnt;
	return true;
}
#pragma endregion

#pragma region EditingPoints
// ******************************************************
//		Clear()
// ******************************************************
void CShapeWrapperCOM::Clear()
{
	const auto pointsSize = gsl::narrow_cast<unsigned int>(_points.size());
	for (unsigned int i = 0; i < pointsSize; i++)
	{
		gsl::at(_points, i)->Release();
	}
	_points.clear();
	_parts.clear();
	_boundsChanged = true;
}

// ******************************************************
//		InsertPoint()
// ******************************************************
bool CShapeWrapperCOM::InsertPoint(int pointIndex, IPoint* point)
{
	if (_shapeType == SHP_NULLSHAPE)
	{
		return false;
	}

	if (pointIndex < 0)
	{
		pointIndex = 0;
	}
	else if (pointIndex > gsl::narrow_cast<int>(_points.size()))
	{
		pointIndex = gsl::narrow_cast<int>(_points.size());
	}

	if (_shapeType == SHP_POINT)
	{
		if (!_points.empty())
		{
			return false;
		}
		point->AddRef();
		_points.push_back(point);
		_boundsChanged = true;
		return true;
	}

	// else (_ShapeType  == SHP_POLYLINE || _ShapeType == SHP_POLYGON || _ShapeType == SHP_MULTIPOINT)
	point->AddRef();
	_points.insert(_points.begin() + pointIndex, point);
	_boundsChanged = true;
	return true;
}

// ******************************************************
//		InsertPointXY()
// ******************************************************
bool CShapeWrapperCOM::InsertPointXY(const int pointIndex, const double x, const double y)
{
	IPoint* pnt = nullptr;

	ComHelper::CreatePoint(&pnt);
	if (pnt)
	{
		pnt->put_X(x);
		pnt->put_Y(y);
		const bool result = this->InsertPoint(pointIndex, pnt);
		pnt->Release();
		return result;
	}
	return false;
}

// ******************************************************
//		InsertPointXYZM()
// ******************************************************
bool CShapeWrapperCOM::InsertPointXYZM(const int pointIndex, const double x, const double y, const double z, const double m)
{
	IPoint* pnt = nullptr;
	ComHelper::CreatePoint(&pnt);
	if (pnt)
	{
		pnt->put_X(x);
		pnt->put_Y(y);
		pnt->put_Z(z);
		pnt->put_M(m);
		return this->InsertPoint(pointIndex, pnt);
	}
	return false;
}

// ********************************************************
//		DeletePoint()
// ********************************************************
bool CShapeWrapperCOM::DeletePoint(const int pointIndex)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	_points[pointIndex]->Release();
	_points.erase(_points.begin() + pointIndex);
	_boundsChanged = true;
	return true;
}
#pragma endregion

#pragma region Parts

// **********************************************************
//		InsertPart()
// **********************************************************
bool CShapeWrapperCOM::InsertPart(int partIndex, const int pointIndex)
{
	if (_shapeType != SHP_POLYLINE &&
		_shapeType != SHP_POLYLINEZ &&
		_shapeType != SHP_POLYLINEM &&
		_shapeType != SHP_POLYGON &&
		_shapeType != SHP_POLYGONZ &&
		_shapeType != SHP_POLYGONM)
	{
		return false;
	}
	if (partIndex < 0)
	{
		partIndex = 0;
	}
	else if (partIndex > gsl::narrow_cast<int>(_parts.size()))
	{
		partIndex = gsl::narrow_cast<int>(_parts.size());
	}

	// this part already exists
	for (const long part : _parts)
	{
		if (part == pointIndex)
		{
			return true;	// returning true to preserve compatibility
		}
	}

	_parts.insert(_parts.begin() + partIndex, pointIndex);
	return true;
}

// **************************************************************
//		DeletePart()
// **************************************************************
bool CShapeWrapperCOM::DeletePart(const int partIndex)
{
	if (_shapeType != SHP_POLYLINE &&
		_shapeType != SHP_POLYLINEZ &&
		_shapeType != SHP_POLYLINEM &&
		_shapeType != SHP_POLYGON &&
		_shapeType != SHP_POLYGONZ &&
		_shapeType != SHP_POLYGONM)
	{
		return false;
	}

	if (partIndex < 0 || partIndex >= gsl::narrow_cast<int>(_parts.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	_parts.erase(_parts.begin() + partIndex);
	return true;
}

// **********************************************************
//		get_allPartstartPoint()
// **********************************************************
int CShapeWrapperCOM::get_PartStartPoint(const int partIndex)
{
	if (partIndex < 0 || partIndex >= gsl::narrow_cast<int>(_parts.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	return _parts[partIndex];
}

// **********************************************************
//		get_PartEndPoint()
// **********************************************************
int CShapeWrapperCOM::get_PartEndPoint(const int partIndex)
{
	if (partIndex < 0 || partIndex >= gsl::narrow_cast<int>(_parts.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	if (partIndex == gsl::narrow_cast<int>(_parts.size()) - 1)
	{
		return gsl::narrow_cast<int>(_points.size()) - 1;
	}
	return _parts[partIndex + 1] - 1;
}

// **********************************************************
//		put_PartStartPoint()
// **********************************************************
bool CShapeWrapperCOM::put_PartStartPoint(const long partIndex, const long newVal)
{
	if (partIndex < 0 || partIndex >= gsl::narrow_cast<long>(_parts.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	_parts[partIndex] = newVal;
	return true;
}
#pragma endregion

// **************************************************************
//		ReversePoints()
// **************************************************************
void CShapeWrapperCOM::ReversePoints(const long startIndex, const long endIndex)
{
	auto iter1 = _points.begin();
	auto iter2 = _points.begin();
	iter1 += startIndex;
	iter2 += endIndex;
	reverse(iter1, iter2);
}

// **************************************************************
//		put_RawData 
// **************************************************************
bool CShapeWrapperCOM::put_RawData(char* shapeData, const int recordLength)
{
	_points.clear();
	_parts.clear();

	_shapeType = (ShpfileType) * (int*)shapeData; // TODO: Fix compile warning
	const ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);
	const bool is25d = _shapeType != shpType2D;

	_boundsChanged = true;

	int numPoints;

	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
		return true;
	}

	if (shpType2D == SHP_POINT)
	{
		double* ddata = (double*)(shapeData + 4); // TODO: Fix compile warning

		IPoint* pnt;
		ComHelper::CreatePoint(&pnt);
		// TODO: Fix compile warnings:
		pnt->put_X(ddata[0]);
		pnt->put_Y(ddata[1]);

		if (_shapeType == SHP_POINTM)
		{
			pnt->put_M(ddata[2]); // TODO: Fix compile warning
		}
		else if (_shapeType == SHP_POINTZ)
		{
			pnt->put_Z(ddata[2]); // TODO: Fix compile warning

			if (recordLength > 28)  // 4 (shpType) + 3 * 8 (X, Y, Z) 
			{
				pnt->put_M(ddata[3]); // TODO: Fix compile warning
			}
		}

		_points.push_back(pnt);

		return true;
	}

	if (shpType2D == SHP_MULTIPOINT)
	{
		// TODO: Fix compile warnings:
		double* bounds = (double*)(shapeData + 4);
		_xMin = bounds[0];					// 12
		_yMin = bounds[1];					// 20
		_xMax = bounds[2];					// 28
		_yMax = bounds[3];					// 36

		numPoints = *(int*)(shapeData + 36); // TODO: Fix compile warning

		int readCount = 40;
		double* points = (double*)(shapeData + readCount); // TODO: Fix compile warning

		// points
		_points.resize(numPoints);
		for (int i = 0; i < numPoints; i++)
		{
			IPoint* pnt;
			ComHelper::CreatePoint(&pnt);
			// TODO: Fix compile warnings
			pnt->put_X(points[i * 2]);
			pnt->put_Y(points[i * 2 + 1]);
			gsl::at(_points, i) = pnt;
		}

		points += numPoints * 2;   // X, Y  // TODO: Fix compile warning
		readCount += numPoints * 2 * 8;

		if (is25d)
		{
			if (_shapeType == SHP_MULTIPOINTZ)
			{
				_zMin = *points;
				_zMax = *(points + 1);  // TODO: Fix compile warning

				points += 2; // TODO: Fix compile warning
				readCount += 2 * 8;

				for (int i = 0; i < numPoints; i++) {
					gsl::at(_points, i)->put_Z(points[i]);
				}

				points += numPoints; // TODO: Fix compile warning
				readCount += numPoints * 8;
			}

			// M values (only if record length is long enough)
			if (readCount + (2 + numPoints) * 8 <= recordLength)
			{
				_mMin = *points;
				_mMax = *(points + 1); // TODO: Fix compile warning

				points += 2;  // TODO: Fix compile warning

				for (int i = 0; i < numPoints; i++) {
					gsl::at(_points, i)->put_M(points[i]);
				}
			}
		}
		return true;
	}

	if (shpType2D == SHP_POLYLINE || shpType2D == SHP_POLYGON)
	{
		// TODO: Fix compile warnings:
		double* bounds = (double*)(shapeData + 4);
		_xMin = bounds[0];					// 12
		_yMin = bounds[1];					// 20
		_xMax = bounds[2];					// 28
		_yMax = bounds[3];					// 36

		const int numParts = *(int*)(shapeData + 36); // TODO: Fix compile warning
		numPoints = *(int*)(shapeData + 40); // TODO: Fix compile warning

		// parts
		const int* parts = (int*)(shapeData + 44);  // TODO: Fix compile warning
		if (numParts > 0)
		{
			_parts.resize(numParts);
			memcpy(&gsl::at(_parts, 0), parts, sizeof(int) * numParts);
		}

		// points
		int readCount = 44 + sizeof(int) * numParts;
		auto points = (double*)(shapeData + readCount);  // TODO: Fix compile warning

		if (numPoints > 0)
		{
			_points.resize(numPoints);
			for (int i = 0; i < numPoints; i++)
			{
				IPoint* pnt;
				ComHelper::CreatePoint(&pnt);
				pnt->put_X(points[i * 2]);   // TODO: Fix compile warning
				pnt->put_Y(points[i * 2 + 1]); // TODO: Fix compile warning
				gsl::at(_points, i) = pnt;
			}
		}

		points += numPoints * 2;  // X and Y  // TODO: Fix compile warning
		readCount += numPoints * 2 * 8;

		if (is25d)
		{
			if (_shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYGONZ)
			{
				_zMin = *points;
				_zMax = *(points + 1);  // TODO: Fix compile warning

				points += 2;  // TODO: Fix compile warning
				readCount += 2 * 8;

				for (int i = 0; i < numPoints; i++) {
					gsl::at(_points, i)->put_Z(points[i]);
				}

				points += numPoints;   // TODO: Fix compile warning
				readCount += numPoints * 8;
			}

			// M values (only if record length is long enough)
			if (readCount + (2 + numPoints) * 8 <= recordLength)
			{
				_mMin = *points;
				_mMax = *(points + 1);  // TODO: Fix compile warning
				points += 2;  // TODO: Fix compile warning

				if (numPoints > 0)
				{
					for (int i = 0; i < numPoints; i++) {
						gsl::at(_points, i)->put_M(points[i]);
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
	const int numPoints = gsl::narrow_cast<int>(_points.size());
	const int numParts = gsl::narrow_cast<int>(_parts.size());

	const int length = ShapeUtility::get_ContentLength(_shapeType, numPoints, numParts);
	// TODO: Fix compile warnings:
	int* intdata = new int[length / 4];
	intdata[0] = (int)_shapeType;

	double* ddata;  // TODO: Fix compile warning

	const ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);

	const bool is25d = shpType2D != _shapeType;

	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
		return intdata;
	}

	if (shpType2D == SHP_POINT)
	{
		if (_points.empty())
		{
			intdata[0] = static_cast<int>(SHP_NULLSHAPE);
		}
		else
		{
			ddata = (double*)&intdata[1];  // TODO: Fix compile warning

			gsl::at(_points, 0)->get_X(&ddata[0]);
			gsl::at(_points, 0)->get_Y(&ddata[1]);

			if (_shapeType == SHP_POINTM)
			{
				gsl::at(_points, 0)->get_M(&ddata[2]);
			}
			else if (_shapeType == SHP_POINTZ)
			{
				gsl::at(_points, 0)->get_M(&ddata[2]);
				gsl::at(_points, 0)->get_Z(&ddata[3]);
			}
		}
		return intdata;
	}

	if (shpType2D == SHP_MULTIPOINT)
	{
		// TODO: Fix compile warnings:
		ddata = (double*)&intdata[1];
		ddata[0] = _xMin;
		ddata[1] = _yMin;
		ddata[2] = _xMax;
		ddata[3] = _yMax;

		// points
		intdata[9] = numPoints;  // TODO: Fix compile warning
		ddata = (double*)&intdata[10]; // TODO: Fix compile warning

		for (int i = 0; i < numPoints; i++)
		{
			gsl::at(_points, i)->get_X(&ddata[i * 2]);
			gsl::at(_points, i)->get_Y(&ddata[i * 2 + 1]);
		}

		// z values
		if (is25d)
		{
			ddata += numPoints * 2;  // TODO: Fix compile warning

			if (_shapeType == SHP_MULTIPOINTZ)
			{
				// TODO: Fix compile warnings:
				ddata[0] = _zMin;
				ddata[1] = _zMax;
				ddata += 2;

				for (int i = 0; i < numPoints; i++)
				{
					gsl::at(_points, i)->get_Z(&ddata[i]);
				}

				ddata += numPoints; // TODO: Fix compile warning
			}

			// TODO: Fix compile warnings:
			ddata[0] = _mMin;
			ddata[1] = _mMax;
			ddata += 2;

			for (int i = 0; i < numPoints; i++)
			{
				gsl::at(_points, i)->get_M(&ddata[i]);
			}
		}
		return intdata;
	}

	if (shpType2D == SHP_POLYLINE || shpType2D == SHP_POLYGON)
	{
		// TODO: Fix compile warnings:
		ddata = (double*)&intdata[1];
		ddata[0] = _xMin;
		ddata[1] = _yMin;
		ddata[2] = _xMax;
		ddata[3] = _yMax;

		// points
		intdata[9] = numParts;  // TODO: Fix compile warning
		intdata[10] = numPoints; // TODO: Fix compile warning

		// parts
		int* parts = &intdata[11];  // TODO: Fix compile warning
		memcpy(parts, &gsl::at(_parts, 0), sizeof(int) * numParts);

		// points
		ddata = (double*)&intdata[11 + numParts]; // TODO: Fix compile warning
		if (numPoints > 0)
		{
			for (int i = 0; i < numPoints; i++)
			{
				gsl::at(_points, i)->get_X(&ddata[i * 2]);
				gsl::at(_points, i)->get_Y(&ddata[i * 2 + 1]);
			}

			// z values
			if (is25d)
			{
				ddata += numPoints * 2; // TODO: Fix compile warning

				if (_shapeType == SHP_POLYLINEZ || _shapeType == SHP_POLYGONZ)
				{
					// TODO: Fix compile warnings:
					ddata[0] = _zMin;
					ddata[1] = _zMax;
					ddata += 2;

					for (int i = 0; i < numPoints; i++)
					{
						gsl::at(_points, i)->get_Z(&ddata[i]);
					}

					ddata += numPoints; // TODO: Fix compile warning
				} 

				// TODO: Fix compile warnings:
				ddata[0] = _mMin;
				ddata[1] = _mMax;
				ddata += 2;

				for (int i = 0; i < numPoints; i++)
				{
					gsl::at(_points, i)->get_M(&ddata[i]);
				}
			}
		}
	}

	return intdata;
}
