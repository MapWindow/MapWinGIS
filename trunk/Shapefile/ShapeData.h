/**************************************************************************************
 * File name: ShapeData.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CShapeData. Used for holding shape data in fast non-editabe mode.
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
 // Sergei Leschinski (lsu) 30 jan 2011 - created the file
#pragma once
#include "ShapeInterfaces.h"

// CShapeData declaration
class CShapeData: public IShapeData
{
public:
	// Constructors
	CShapeData(char* shpData)
	{
		_lastErrorCode = tkNO_ERROR;
		_bounds = NULL;
		_points = NULL;
		_parts = NULL;
		put_ShapeData(shpData);
	}

	// Destructor
	~CShapeData()
	{
		this->Clear();
	}
	
private:	
	ShpfileType _shapeType;	// actually 2D type will be used, as Z, M values aren't supported here
	int _lastErrorCode;
	// size
	int _pointCount;
	int _partCount;
	// data
	double* _bounds;	// we don't need bounds for points; so memory will be allocated dynamically
	double* _points;
	int* _parts;
	// functions
	ShpfileType ShapeTypeConvert2D(ShpfileType type);
	void Clear();
public:
	ShpfileType get_shapeType()
	{
		return _shapeType;
	}
	int get_PointCount()
	{	
		return _pointCount;
	}
	int get_PartCount()
	{
		return _partCount;
	}
	int get_LastErrorCode()
	{
		int code = _lastErrorCode;
		_lastErrorCode = tkNO_ERROR;
		return code;
	}

	bool get_BoundsXY(double& xMin, double& xMax, double& yMin, double& yMax);

	// data
	bool put_ShapeData(char* shapeData);

	// parts
	int get_PartStartPoint(int PartIndex);
	int get_PartEndPoint(int PartIndex);

	// points
	double* get_PointsXY();
	void get_XYFast(int PointIndex, double& x, double& y);
	bool get_PointXY(int PointIndex, double& x, double& y);
	
	// utility
	bool PointInRing(int partIndex, double pointX, double pointY);
};

