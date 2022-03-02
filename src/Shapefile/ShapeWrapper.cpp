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
#include "StdAfx.h"
#include "ShapeWrapper.h"
#include <ErrorCodes.h>
#include <gsl/util>

#pragma region ShapeType
// **************************************************************
//		get/put_ShapeType()
// **************************************************************
bool CShapeWrapper::put_ShapeType(const ShpfileType shpType)
{
	if (shpType == SHP_MULTIPATCH)
	{
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		return false;
	}

	if (ShapeUtility::IsM(shpType) || ShapeUtility::IsZ(shpType))
	{
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
		return false;
	}

	_shapeType = shpType;

	const ShpfileType shpType2D = ShapeUtility::Convert2D(_shapeType);
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
		if (_parts.empty())
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
bool CShapeWrapper::get_PointXY(const int pointIndex, double& x, double& y)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}

	x = _points[pointIndex].X;
	y = _points[pointIndex].Y;
	return true;
}

// *******************************************************************
//     get_PointXYZM()
// *******************************************************************
bool CShapeWrapper::get_PointXYZM(const int pointIndex, double& x, double& y, double& z, double& m)
{
	z = 0.0; m = 0.0;
	return get_PointXY(pointIndex, x, y);
}

// ********************************************************
//		get_XYFast 
// ********************************************************
void CShapeWrapper::get_XYFast(const int pointIndex, double& x, double& y)
{
	x = _points[pointIndex].X;
	y = _points[pointIndex].Y;
}

// **********************************************************
//		put_PointXY()
// **********************************************************
bool CShapeWrapper::put_PointXY(const int pointIndex, const double x, const double y)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}
	_points[pointIndex].X = x;
	_points[pointIndex].Y = y;
	_boundsChanged = true;
	return true;
}

// **********************************************************
//		put_PointZ()
// **********************************************************
bool CShapeWrapper::put_PointZ(int pointIndex, double z)
{
	// TODO: Not implemented
	_lastErrorCode = tkPROPERTY_NOT_IMPLEMENTED;
	return false;
}

// **********************************************************
//		put_PointM()
// **********************************************************
bool CShapeWrapper::put_PointM(int pointIndex, double m)
{
	// TODO: Not implemented
	_lastErrorCode = tkPROPERTY_NOT_IMPLEMENTED;
	return false;
}

// **********************************************************
//		get_PointsXY()
// **********************************************************
double* CShapeWrapper::get_PointsXY()
{
	if (gsl::narrow_cast<int>(_points.size()) > 0)
		return reinterpret_cast<double*>(&gsl::at(_points, 0));

	return nullptr;
}

// **********************************************************
//		get_PointZ()
// **********************************************************
bool CShapeWrapper::get_PointZ(int pointIndex, double& z)
{
	// TODO: Not implemented
	_lastErrorCode = tkPROPERTY_NOT_IMPLEMENTED;
	return false;
}

// **********************************************************
//		get_PointM()
// **********************************************************
bool CShapeWrapper::get_PointM(int pointIndex, double& m)
{
	// TODO: Not implemented
	_lastErrorCode = tkPROPERTY_NOT_IMPLEMENTED;
	return false;
}

// **********************************************************
//		get_Point()
// **********************************************************
IPoint* CShapeWrapper::get_Point(const long index)
{
	if (index < 0 || index >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return nullptr;
	}

	IPoint* pnt = nullptr;
	ComHelper::CreatePoint(&pnt);
	if (pnt)
	{
		pnt->put_X(_points[index].X);
		pnt->put_Y(_points[index].Y);
	}
	return pnt;
}

// **********************************************************
//		put_Point()
// **********************************************************
bool CShapeWrapper::put_Point(const long index, IPoint* pnt)
{
	if (index < 0 || index >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}

	if (!pnt)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return false;
	}

	pnt->get_X(&_points[index].X);
	pnt->get_Y(&_points[index].Y);
	return true;
}

#pragma endregion

#pragma region Parts

// **********************************************************
//		InsertPart()
// **********************************************************
bool CShapeWrapper::InsertPart(int partIndex, const int pointIndex)
{
	const ShpfileType shpType2D = get_ShapeType2D();
	if (shpType2D != SHP_POLYLINE && shpType2D != SHP_POLYGON)
	{
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
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
	for (const int part : _parts)
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
bool CShapeWrapper::DeletePart(const int partIndex)
{
	const ShpfileType shpType2D = get_ShapeType2D();
	if (shpType2D != SHP_POLYLINE && shpType2D != SHP_POLYGONM)
	{
		_lastErrorCode = tkUNSUPPORTED_SHAPEFILE_TYPE;
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
//		get_PartStartPoint()
// **********************************************************
int CShapeWrapper::get_PartStartPoint(const int partIndex)
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
int CShapeWrapper::get_PartEndPoint(const int partIndex)
{
	if (partIndex < 0 || partIndex >= gsl::narrow_cast<int>(_parts.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return -1;
	}
	if (partIndex == gsl::narrow_cast<int>(_parts.size()) - 1)
	{
		return gsl::narrow_cast<int>(_parts.size()) - 1;
	}
	return _parts[partIndex + 1] - 1;
}

// **********************************************************
//		put_PartStartPoint()
// **********************************************************
// TODO: Why is partIndex long? In the function above it is int.
bool CShapeWrapper::put_PartStartPoint(const long partIndex, const long newVal)
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
			const int pointSize = gsl::narrow_cast<int>(_points.size());
			for (int i = 0; i < pointSize; i++)
			{
				if (i == 0)
				{
					_xMin = _xMax = gsl::at(_points, i).X;
					_yMin = _yMax = gsl::at(_points, i).Y;
				}
				else
				{
					if (gsl::at(_points, i).X < _xMin)	_xMin = gsl::at(_points, i).X;
					else if (gsl::at(_points, i).X > _xMax)	_xMax = gsl::at(_points, i).X;
					if (gsl::at(_points, i).Y < _yMin)	_yMin = gsl::at(_points, i).Y;
					else if (gsl::at(_points, i).Y > _yMax)	_yMax = gsl::at(_points, i).Y;
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
bool CShapeWrapper::put_RawData(char* shapeData, int length)
{
	_shapeType = (ShpfileType) * (int*)shapeData;  // TODO: Fix compile warning
	_boundsChanged = true;

	int numPoints;

	const ShpfileType shpType2D = _shapeType;
	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
		return true; // TODO: Should this be return false??
	}

	if (shpType2D == SHP_MULTIPOINT)
	{
		// TODO: Fix compile warnings:
		double* bounds = (double*)(shapeData + 4);
		_xMin = bounds[0];					// 12
		_yMin = bounds[1];					// 20
		_xMax = bounds[2];					// 28
		_yMax = bounds[3];					// 36

		// TODO: Fix compile warnings:
		numPoints = *(int*)(shapeData + 36);
		double* points = (double*)(shapeData + 40);

		// points
		_points.resize(numPoints);
		memcpy(&gsl::at(_points, 0), points, sizeof(double) * numPoints * 2);

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

		// TODO: Fix compile warnings:
		int numParts = *(int*)(shapeData + 36);
		numPoints = *(int*)(shapeData + 40);

		// parts
		int* parts = (int*)(shapeData + 44); // TODO: Fix compile warning
		if (numParts > 0)
		{
			_parts.resize(numParts);
			memcpy(&gsl::at(_parts, 0), parts, sizeof(int) * numParts);
		}

		// points
		// TODO: Fix compile warnings:
		double* points = (double*)(shapeData + 44 + sizeof(int) * numParts);
		if (numPoints > 0)
		{
			_points.resize(numPoints);
			memcpy(&gsl::at(_points, 0), points, sizeof(double) * numPoints * 2);
		}
	}
	return true;
}

// **************************************************************
//		get_Data 
// **************************************************************
// Forming the data to write to the disk. Should be optimized as far as possible.
int* CShapeWrapper::get_RawData()
{
	const int numPoints = gsl::narrow_cast<int>(_points.size());
	const int numParts = gsl::narrow_cast<int>(_parts.size());

	const int length = ShapeUtility::get_ContentLength(_shapeType, numPoints, numParts);

	// TODO: Fix compile warnings:
	int* intdata = new int[length / 4];
	intdata[0] = static_cast<int>(_shapeType);

	double* ddata;// TODO: Fix compile warning

	const ShpfileType shpType2D = get_ShapeType2D();
	if (shpType2D == SHP_NULLSHAPE)
	{
		// do nothing;
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
		// TODO: Fix compile warnings:
		intdata[9] = numPoints;
		ddata = (double*)&intdata[10];

		memcpy(ddata, &gsl::at(_points, 0), sizeof(double) * numPoints * 2);
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
		// TODO: Fix compile warnings:
		intdata[9] = numParts;
		intdata[10] = numPoints;

		// parts
		int* parts = &intdata[11]; // TODO: Fix compile warning
		memcpy(parts, &gsl::at(_parts, 0), sizeof(int) * numParts);

		// points
		ddata = (double*)&intdata[11 + numParts]; // TODO: Fix compile warning
		memcpy(ddata, &gsl::at(_points, 0), sizeof(double) * numPoints * 2);
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
bool CShapeWrapper::InsertPoint(const int pointIndex, IPoint* pnt)
{
	if (!pnt)
	{
		_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return false;
	}

	double x, y, z, m;
	pnt->get_X(&x);
	pnt->get_Y(&y);
	pnt->get_Z(&z);
	pnt->get_M(&m);
	return InsertPointXYZM(pointIndex, x, y, z, m);
}

// ******************************************************
//		InsertPointXY()
// ******************************************************
bool CShapeWrapper::InsertPointXY(const int pointIndex, const double x, const double y)
{
	return InsertPointXYZM(pointIndex, x, y, 0.0, 0.0);
}

// ******************************************************
//		InsertPointXYZM()
// ******************************************************
bool CShapeWrapper::InsertPointXYZM(int pointIndex, double x, double y, double z, double m)
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

	_points.insert(_points.begin() + pointIndex, pointEx(x, y));
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
	//_points.push_back(pointEx(x, y));
	_points.emplace_back(x, y);
	_boundsChanged = true;
}

// ******************************************************
//		AddPoint()
// ******************************************************
void CShapeWrapper::AddPoint(const double x, const double y, double z, double m)
{
	// TODO: z and m are not used
	AddPoint(x, y);
}

// ********************************************************
//		DeletePoint()
// ********************************************************
bool CShapeWrapper::DeletePoint(const int pointIndex)
{
	if (pointIndex < 0 || pointIndex >= gsl::narrow_cast<int>(_points.size()))
	{
		_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
		return false;
	}

	_boundsChanged = true;
	_points.erase(_points.begin() + pointIndex);
	return true;
}

#pragma endregion


// *******************************************************************
//     PointInRing()
// *******************************************************************
bool CShapeWrapper::PointInRing(const int partIndex, const double pointX, const double pointY)
{
	const ShpfileType shptype = this->get_ShapeType();
	if (shptype != SHP_POLYGON &&
		shptype != SHP_POLYGONZ &&
		shptype != SHP_POLYGONM)
		return false;

	// TODO: add test of bounds !!!
	double xMin, xMax, yMin, yMax, zMin, zMax, mMin, mMax;
	this->get_Bounds(xMin, xMax, yMin, yMax, zMin, zMax, mMin, mMax);

	if (pointX <  xMin || pointY < yMin || pointX > xMax || pointY > yMax)
		return false;

	const int beginPart = this->get_PartStartPoint(partIndex);
	const int endPart = this->get_PartEndPoint(partIndex);

	int crossCount = 0;
	for (int nPoint = beginPart; nPoint <= endPart; nPoint++)
	{
		// TODO: Fix compile warnings:
		const double* points = this->get_PointsXY();
		const double x1 = points[nPoint * 2] - pointX;
		const double y1 = points[nPoint * 2 + 1] - pointY;
		const double x2 = points[(nPoint + 1) * 2] - pointX;
		const double y2 = points[(nPoint + 1) * 2 + 1] - pointY;

		const double y1y2 = y1 * y2;
		if (y1y2 > 0.0) // If the signs are the same
		{
			// Then it does not cross
			continue;
		}

		if (y1y2 == 0.0) // Then it has intersected a vertex
		{
			if (y1 == 0.0)
			{
				if (y2 > 0.0)
					continue;
			}
			else if (y1 > 0.0)
				continue;
		}

		if (x1 > 0.0 && x2 > 0.0)
		{
			crossCount++;
			continue;
		}

		// Calculate Intersection
		const double dy = y2 - y1;
		const double dx = x2 - x1;

		// CDM March 2008 - if dy is zero (horiz line), this will be a bad idea...
		if (dy != 0)
		{
			if (x1 - y1 * (dx / dy) > 0.0)
				crossCount++;
		}
	}
	return crossCount & 1 ? true : false;
}

// *******************************************************************
//     ReversePoints()
// *******************************************************************
void CShapeWrapper::ReversePoints(const long startIndex, const long endIndex)
{
	if (_points.size() > 1)
	{
		auto iter1 = _points.begin();
		auto iter2 = _points.begin();
		iter1 += startIndex;
		iter2 += endIndex;
		reverse(iter1, iter2);
	}
}

// *******************************************************************
//     AddPart()
// *******************************************************************
void CShapeWrapper::AddPart(const int pointIndex)
{
	_parts.push_back(pointIndex);
}

