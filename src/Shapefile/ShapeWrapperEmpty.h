/**************************************************************************************
 * File name: CShapeWrapperEmpty.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CShapeWrapperEmpty
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

class CShapeWrapperEmpty: public IShapeWrapper
{
public:
	ShapeWrapperType get_WrapperType() { return ShapeWrapperType::swtEmpty; }

	int get_ContentLength() { return ShapeUtility::get_ContentLength(SHP_NULLSHAPE, 0, 0); }
	int get_PointCount(){ return 0; }
	int get_PartCount() { return 0; }

	// shpData
	bool put_RawData(char* shapeData, int recordLength) { return false; }
	int* get_ShapeData(void) {return nullptr; } 
	
	// type
	ShpfileType get_ShapeType(void) { return SHP_NULLSHAPE; }
	ShpfileType get_ShapeType2D(void){ return SHP_NULLSHAPE; }
	bool put_ShapeType(ShpfileType shpType) { return shpType == SHP_NULLSHAPE; }
	
	// bounds
	void RefreshBounds() { };
	void RefreshBoundsXY() { };
	bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax) { return false; }
	bool get_Bounds(double& xMin, double& xMax, double& yMin, double& yMax, 
		double& zMin, double& zMax, double& mMin, double& mMax) { return false; }

	void get_XYFast(int PointIndex, double& x, double& y) { }
	bool get_PointXY(int PointIndex, double& x, double& y) { return false; }
	bool put_PointXY(int PointIndex, double x, double y){ return false; }
	bool get_PointZ(int PointIndex, double& z){ return false; }
	bool get_PointM(int PointIndex, double& m){ return false; }
	bool put_PointZ(int PointIndex, double z){ return false; }
	bool put_PointM(int PointIndex, double m){ return false; }
	bool get_PointXYZM(int pointIndex, double& x, double& y, double& z, double& m) { return false; }
	
	// COM points
	IPoint* get_Point(long Index) {return NULL; }
	bool put_Point(long Index, IPoint* pnt) { return false; }

	// changing size
	void Clear() { }
	bool InsertPoint(int PointIndex, IPoint* pnt) { return false; }
	bool InsertPointXY(int Pointindex, double x, double y) { return false; }
	bool InsertPointXYZM(int PointIndex, double x, double y, double z, double m) { return false; }
	bool DeletePoint(int Pointindex) { return false; }
	
	// parts
	bool InsertPart(int PartIndex, int PointIndex) { return false; }
	bool DeletePart(int PartIndex) { return false; }
	int get_PartStartPoint(int PartIndex) { return -1; }
	int get_PartEndPoint(int PartIndex) { return -1; }
	bool put_PartStartPoint(long PartIndex, long newVal) { return false; }
	
	bool PointInRing(int partIndex, double pointX, double pointY) { return false; }

	int get_LastErrorCode() { return tkNO_ERROR; }

	void ReversePoints(long startIndex, long endIndex) { };

	int* get_RawData() { return new int[get_ContentLength() / 4]; }
};

