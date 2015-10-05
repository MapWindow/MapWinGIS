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
#include "ShapeWrapper.h"

class CShapeWrapperM : public CShapeWrapper
{
public:
	// Constructors
	CShapeWrapperM(ShpfileType shpType)
		: CShapeWrapper(shpType)
	{
		_mMin =	_mMax = 0.0;
	}

	CShapeWrapperM(char* shpData)
		: CShapeWrapper(shpData)
	{
		_mMin = _mMax = 0.0;
	}

	CShapeWrapperM(char* shpData, Extent* extents)
		: CShapeWrapper(shpData, extents)
	{
		_mMin = _mMax = 0.0;
	}

	// -----------------------------------------------------
	//		Destructor
	// -----------------------------------------------------
	virtual ~CShapeWrapperM()
	{
		_pointsM.clear();
	}
	
private:	
	double _mMin;
	double _mMax;

public:
	std::vector<double> _pointsM;
	
public:
	//int get_ContentLength();
	//int get_PointCount(){return _points.size();}
	//int get_PartCount(){return _parts.size();}

	//// shpData
	//bool put_ShapeData(char* shapeData, Extent* extents);
	//bool put_ShapeData(char* shapeData)
	//{
	//	return put_ShapeData(shapeData, NULL);
	//}
	//int* get_ShapeData(void);
	//
	//// type
	//ShpfileType get_ShapeType(void) {return _ShapeType;}
	//ShpfileType get_ShapeType2D(void){return _ShapeType2D;}
	//bool put_ShapeType(ShpfileType shpType);
	//
	//// bounds
	//void RefreshBounds();
	//void RefreshBoundsXY();
	//bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax);
	//bool get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
	//				double& zMin, double& zMax, double& mMin, double& mMax);

	//// getting all values
	//double* get_PointsXY();
	//double* get_PointsM();
	//double* get_PointsZ();
	//

	//void get_XYFast(int PointIndex, double& x, double& y);
	//bool get_PointXY(int PointIndex, double& x, double& y);
	//bool put_PointXY(int PointIndex, double x, double y);
	//bool get_PointZ(int PointIndex, double& z);
	//bool get_PointM(int PointIndex, double& m);
	//bool put_PointZ(int PointIndex, double z);
	//bool put_PointM(int PointIndex, double m);
	//
	//// COM points
	//IPoint* get_Point(long Index);
	//bool put_Point(long Index, IPoint* pnt);

	//// changing size
	//void Clear();
	//bool InsertPoint(int PointIndex, IPoint* pnt);
	//bool InsertPointXY(int Pointindex, double x, double y);
	//bool InsertPointXYZM(int PointIndex, double x, double y, double z, double m);
	//bool DeletePoint(int Pointindex);
	//
	//// parts
	//bool InsertPart(int PartIndex, int PointIndex);
	//bool DeletePart(int PartIndex);
	//int get_PartStartPoint(int PartIndex);
	//int get_PartEndPoint(int PartIndex);
	//bool put_PartStartPoint(long PartIndex, long newVal);
	//
	//bool PointInRing(int partIndex, double pointX, double pointY);

	//int get_LastErrorCode()
	//{
	//	int code = _lastErrorCode;
	//	_lastErrorCode = tkNO_ERROR;
	//	return code;
	//}

	//static int get_ContentLength(ShpfileType shpType,  int numPoints, int numParts);
	//static ShpfileType ShapeTypeConvert2D(ShpfileType type);
private:
	/*	Inner functions */
	// ------------
	
};

