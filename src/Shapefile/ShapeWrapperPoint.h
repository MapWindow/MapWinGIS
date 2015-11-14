/**************************************************************************************
 * File name: CShapeWrapperPoint.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CShapeWrapperPoint
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

class CShapeWrapperPoint: public IShapeWrapper
{
private:
	CShapeWrapperPoint() 
		: _lastErrorCode(tkNO_ERROR), _x(0.0), _y(0.0), _z(0.0), _m(0.0), _initialized(false)
	{
	}
public:
	CShapeWrapperPoint(ShpfileType shpType)
		: CShapeWrapperPoint()
	{
		_shpType = shpType;
	}

	CShapeWrapperPoint(char* shpData, int recordLength) 
		: CShapeWrapperPoint()
	{
		put_RawData(shpData, recordLength);
	}

private:	
	bool _initialized;
	double _x;
	double _y;
	double _z;
	double _m;

	short _lastErrorCode;
	ShpfileType _shpType;	

public:
	ShapeWrapperType get_WrapperType() { return swtPoint; }

	int get_ContentLength();
	int get_PointCount(){ return _initialized ? 1 : 0; }
	int get_PartCount() { return 0; }

	// shpData
	bool put_RawData(char* shapeData, int recordLength);
	int* get_RawData(void);
	
	// type
	ShpfileType get_ShapeType(void) { return _shpType; }
	ShpfileType get_ShapeType2D(void){ return ShapeUtility::Convert2D(_shpType); }
	bool put_ShapeType(ShpfileType shpType);
	
	// bounds
	void RefreshBounds() { };
	void RefreshBoundsXY() { };
	bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax);
	bool get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
					double& zMin, double& zMax, double& mMin, double& mMax);

	void get_XYFast(int PointIndex, double& x, double& y);
	bool get_PointXY(int PointIndex, double& x, double& y);
	bool put_PointXY(int PointIndex, double x, double y);
	bool get_PointZ(int PointIndex, double& z);
	bool get_PointM(int PointIndex, double& m);
	bool put_PointZ(int PointIndex, double z);
	bool put_PointM(int PointIndex, double m);
	bool get_PointXYZM(int pointIndex, double& x, double& y, double& z, double& m);
	
	// COM points
	IPoint* get_Point(long Index);
	bool put_Point(long Index, IPoint* pnt);

	// changing size
	void Clear() { _initialized = false; }
	bool InsertPoint(int PointIndex, IPoint* pnt);
	bool InsertPointXY(int Pointindex, double x, double y);
	bool InsertPointXYZM(int PointIndex, double x, double y, double z, double m);
	bool DeletePoint(int Pointindex);
	
	// parts
	bool InsertPart(int PartIndex, int PointIndex) { return false; }
	bool DeletePart(int PartIndex) { return false; }
	int get_PartStartPoint(int PartIndex) { return -1; }
	int get_PartEndPoint(int PartIndex) { return -1; }
	bool put_PartStartPoint(long PartIndex, long newVal) { return false; }
	
	bool PointInRing(int partIndex, double pointX, double pointY) { return false; }
	void ReversePoints(long startIndex, long endIndex) { };

	int get_LastErrorCode()
	{
		int code = _lastErrorCode;
		_lastErrorCode = tkNO_ERROR;
		return code;
	}
};

