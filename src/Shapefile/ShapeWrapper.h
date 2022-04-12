/**************************************************************************************
 * File name: ShapeWrapper.h
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
#include <gsl/util>

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

class CShapeWrapper : public IShapeWrapper, public IShapeData
{
private:
	CShapeWrapper()
		: _xMin(0.0), _yMin(0.0), _xMax(0.0), _yMax(0.0), _boundsChanged(true), _shapeType(SHP_NULLSHAPE), _lastErrorCode(tkNO_ERROR)
	{
	}

public:
	CShapeWrapper(ShpfileType shpType)
		: CShapeWrapper()
	{
		put_ShapeType(shpType);
	}

	CShapeWrapper(char* data, int length)
		: CShapeWrapper()
	{
		put_RawData(data, length);
	}

	virtual ~CShapeWrapper()
	{
		_parts.clear();
		_points.clear();
	}

private:
	// bounds
	double _xMin;
	double _yMin;
	double _xMax;
	double _yMax;

	bool _boundsChanged;

	std::vector<int> _parts;
	std::vector<pointEx> _points;

protected:
	ShpfileType _shapeType;
	short _lastErrorCode;

public:
	ShapeWrapperType get_WrapperType() { return ShapeWrapperType::swtFast; }

	vector<pointEx>* get_Points() { return &_points; }
	vector<int>* get_Parts() { return &_parts; }

	int get_PointCount() { return gsl::narrow_cast<int>(_points.size()); }
	int get_PartCount() { return gsl::narrow_cast<int>(_parts.size()); }

	// shape data
	bool put_RawData(char* shapeData, int length);
	virtual int* get_RawData();

	// type
	ShpfileType get_ShapeType() { return _shapeType; }
	ShpfileType get_ShapeType2D() { return ShapeUtility::Convert2D(_shapeType); }
	virtual bool put_ShapeType(ShpfileType shpType);

	// bounds
	virtual void RefreshBounds();
	void RefreshBoundsXY();
	bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax);
	virtual bool get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax,
		double& zMin, double& zMax, double& mMin, double& mMax);

	// getting all values
	double* get_PointsXY();
	virtual double* get_PointsM() { return nullptr; }
	virtual double* get_PointsZ() { return nullptr; }

	void get_XYFast(int pointIndex, double& x, double& y);
	bool get_PointXY(int pointIndex, double& x, double& y);
	bool put_PointXY(int pointIndex, double x, double y);
	virtual bool get_PointZ(int pointIndex, double& z);
	virtual bool get_PointM(int pointIndex, double& m);
	virtual bool put_PointZ(int pointIndex, double z);
	virtual bool put_PointM(int pointIndex, double m);
	bool get_PointXYZM(int pointIndex, double& x, double& y, double& z, double& m);

	// COM points
	virtual IPoint* get_Point(long index);
	virtual bool put_Point(long index, IPoint* pnt);

	// changing size
	virtual void Clear();
	virtual void AddPoint(IPoint* pnt);
	virtual void AddPoint(double x, double y);
	virtual void AddPoint(double x, double y, double z, double m);
	virtual bool InsertPoint(int pointIndex, IPoint* pnt);
	virtual bool InsertPointXY(int pointIndex, double x, double y);
	virtual bool InsertPointXYZM(int pointIndex, double x, double y, double z, double m);
	virtual bool DeletePoint(int pointIndex);

	// parts
	void AddPart(int pointIndex);
	bool InsertPart(int partIndex, int pointIndex);
	bool DeletePart(int partIndex);
	int get_PartStartPoint(int partIndex);
	int get_PartEndPoint(int partIndex);
	bool put_PartStartPoint(long partIndex, long newVal);

	bool PointInRing(int partIndex, double pointX, double pointY);

	virtual void ReversePoints(long startIndex, long endIndex);

	int get_LastErrorCode()
	{
		const int code = _lastErrorCode;
		_lastErrorCode = tkNO_ERROR;
		return code;
	}
};

