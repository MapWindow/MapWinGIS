/**************************************************************************************
 * File name: ShapeWrapper.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CShapeWrapper
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
 
#pragma once
#include "ShapeInterfaces.h"

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

class CShapeWrapper: public IShapeWrapper, public IShapeData
{
public:
	CShapeWrapper(ShpfileType shpType)
	{
		_shapeType = shpType;
		_boundsChanged = true;
		_lastErrorCode = tkNO_ERROR;
		_xMin = _yMin =	_xMax =	_yMax =	0.0;
	}
	CShapeWrapper(char* shpData)
	{
		_xMin = _yMin =	_xMax =	_yMax =	0.0;
		_boundsChanged = true;
		_lastErrorCode = tkNO_ERROR;
		put_ShapeData(shpData, NULL);
	}
	CShapeWrapper(char* shpData, Extent* extents)
	{
		_xMin = _yMin =	_xMax =	_yMax =	0.0;
		_boundsChanged = true;
		_lastErrorCode = tkNO_ERROR;
		put_ShapeData(shpData, extents);
	}

	virtual ~CShapeWrapper()
	{
		_parts.clear();
		_points.clear();
	}
	
private:	
	ShpfileType _shapeType;

	// bounds
	double _xMin;
	double _yMin;
	double _xMax;
	double _yMax;

	bool _boundsChanged;
	short _lastErrorCode;

	std::vector<int> _parts;
	std::vector<pointEx> _points;
	
public:
	vector<pointEx>* get_Points() { return &_points; }
	vector<int>* get_Parts() { return &_parts; }

	int get_ContentLength();
	int get_PointCount(){return _points.size();}
	int get_PartCount(){return _parts.size();}

	// shpData
	bool put_ShapeData(char* shapeData, Extent* extents);
	bool put_ShapeData(char* shapeData) { return put_ShapeData(shapeData, NULL); }
	int* get_ShapeData(void);
	
	// type
	ShpfileType get_ShapeType(void) { return _shapeType; }
	ShpfileType get_ShapeType2D(void){ return Utility::ShapeTypeConvert2D(_shapeType); }
	bool put_ShapeType(ShpfileType shpType);
	
	// bounds
	void RefreshBounds();
	void RefreshBoundsXY();
	bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax);
	bool get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
					double& zMin, double& zMax, double& mMin, double& mMax);

	// getting all values
	double* get_PointsXY();
	double* get_PointsM();
	double* get_PointsZ();

	void get_XYFast(int PointIndex, double& x, double& y);
	bool get_PointXY(int PointIndex, double& x, double& y);
	bool put_PointXY(int PointIndex, double x, double y);
	bool get_PointZ(int PointIndex, double& z);
	bool get_PointM(int PointIndex, double& m);
	bool put_PointZ(int PointIndex, double z);
	bool put_PointM(int PointIndex, double m);
	
	// COM points
	IPoint* get_Point(long Index);
	bool put_Point(long Index, IPoint* pnt);

	// changing size
	void Clear();
	void AddPoint(IPoint* pnt);
	void AddPoint(double x, double y);
	void AddPoint(double x, double y, double z, double m);
	bool InsertPoint(int PointIndex, IPoint* pnt);
	bool InsertPointXY(int Pointindex, double x, double y);
	bool InsertPointXYZM(int PointIndex, double x, double y, double z, double m);
	bool DeletePoint(int Pointindex);
	
	// parts
	void AddPart(int pointIndex);
	bool InsertPart(int PartIndex, int PointIndex);
	bool DeletePart(int PartIndex);
	int get_PartStartPoint(int PartIndex);
	int get_PartEndPoint(int PartIndex);
	bool put_PartStartPoint(long PartIndex, long newVal);
	
	bool PointInRing(int partIndex, double pointX, double pointY);

	void ReversePoints(long startIndex, long endIndex);

	int get_LastErrorCode()
	{
		int code = _lastErrorCode;
		_lastErrorCode = tkNO_ERROR;
		return code;
	}
};

