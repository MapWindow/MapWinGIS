/**************************************************************************************
 * File name: ShapeInterfaces.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of ShapeInterfaces.h
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
//#include "ShapeUtility.h"

enum class ShapeWrapperType
{
	swtEmpty,
	swtPoint,
	swtFast,
	swtCom
};

class IShapeData
{
public:
	//DeclareInterface(IShapeData)
	// Interface with methods common to CShapeWrapper and CShapeData
	// Is used while drawing shapefile, to provide common function for both classes
	// function = 0 - pure virtual function, which makes class abstract; 
	// all the virtual function must be overriden in the derived class
	// abstract class can not be instantiated
	virtual int get_PointCount() = 0;
	virtual int get_PartCount()= 0;
	virtual int get_LastErrorCode()= 0;
	virtual bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax)= 0;
	virtual bool put_RawData(char* shapeData, int length)= 0;
	virtual int get_PartStartPoint(int partIndex)= 0;
	virtual int get_PartEndPoint(int partIndex)= 0;
	virtual double* get_PointsXY()= 0;
	virtual bool get_PointXY(int pointIndex, double& x, double& y)= 0;
	virtual bool PointInRing(int partIndex, double pointX, double pointY)= 0;
	virtual void get_XYFast(int pointIndex, double& x, double& y) = 0;
//EndInterface
};

class IShapeWrapper
{
	// Interface for CShapeWrapper and CShapeWrapperCom classes
public:	
//DeclareInterface(IShapeWrapper)	
	// function = 0 - pure virtual function, which makes class abstract; 
	// all the virtual function must be overriden in the derived class
	// abstract class can not be instantiated

	virtual ~IShapeWrapper(){}

	int get_ContentLength();

	virtual ShapeWrapperType get_WrapperType() = 0;

	virtual int get_PointCount() = 0;
	virtual int get_PartCount()= 0;
	virtual int get_LastErrorCode()= 0;
	
	// shape type
	virtual ShpfileType get_ShapeType()  = 0;
	virtual ShpfileType get_ShapeType2D() = 0;
	virtual bool put_ShapeType(ShpfileType shpType) = 0;
	
	// bounds
	virtual void RefreshBounds() = 0;
	virtual void RefreshBoundsXY() = 0;
	virtual bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax) = 0;
	virtual bool get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, double& zMin, double& zMax, double& mMin, double& mMax) = 0;
	
	// COM points
	virtual IPoint* get_Point(long index) = 0;
	virtual bool put_Point(long index, IPoint* pnt) = 0;

	// point values
	virtual bool get_PointXY(int pointIndex, double& x, double& y) = 0;
	virtual bool put_PointXY(int pointIndex, double x, double y) = 0;
	virtual bool get_PointZ(int pointIndex, double& z) = 0;
	virtual bool get_PointM(int pointIndex, double& m) = 0;
	virtual bool put_PointZ(int pointIndex, double z) = 0;
	virtual bool put_PointM(int pointIndex, double m) = 0;
	virtual void get_XYFast(int pointIndex, double& x, double& y) = 0;
	virtual bool get_PointXYZM(int pointIndex, double& x, double& y, double& z, double& m) = 0;

	// changing size
	virtual void Clear() = 0;
	virtual bool InsertPoint(int pointIndex, IPoint* pnt) = 0;
	virtual bool InsertPointXY(int pointIndex, double x, double y) = 0;
	virtual bool InsertPointXYZM(int pointIndex, double x, double y, double z, double m) = 0;
	virtual bool DeletePoint(int pointindex) = 0;

	// parts
	virtual bool InsertPart(int partIndex, int pointIndex) = 0;
	virtual bool DeletePart(int partIndex) = 0;
	virtual int get_PartStartPoint(int partIndex) = 0;
	virtual int get_PartEndPoint(int partIndex) = 0;
	virtual bool put_PartStartPoint(long partIndex, long newVal) = 0;

	virtual void ReversePoints(long startIndex, long endIndex) = 0;

	virtual int* get_RawData() = 0;
	virtual bool put_RawData(char* shapeData, int length) = 0;

	void CopyTo(IShapeWrapper* target);
	
//EndInterface
};

