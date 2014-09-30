/**************************************************************************************
 * File name: PointSymbols.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
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
 // lsu 27 feb 2011 - Created the file.

#include "stdafx.h"
#include "PointSymbols.h"
#include "GeometryOperations.h"

// ***************************************************************
//	  get_SimplePointShape()
// ***************************************************************
float* get_SimplePointShape(tkDefaultPointSymbol symbol, float size, int* numPoints)
{
	tkPointShapeType type;
	double rotation;
	int numSides;
	float sidesRatio;
	get_DefaultSymbolOptions(symbol, type, rotation, numSides, sidesRatio);
	return get_SimplePointShape(type, size, rotation, numSides, sidesRatio, numPoints);
}

// ***************************************************************
//	  get_SimplePointShape()
// ***************************************************************
// Returns coordinates of the simple shape to represent a point symbol (squar, star, cross)
float* get_SimplePointShape(tkPointShapeType type, float& size, double& rotation, int& numSides, float& sidesRatio, int* numPoints)
{
	switch ( type )
	{
		case ptShapeArrow: 
			*numPoints = 8;
			return get_ArrowShape(size, size, rotation);

		case ptShapeCross: 
			*numPoints = numSides * 3 + 1;
			return get_CrossShape(	size, numSides, sidesRatio, rotation);

		case ptShapeFlag: 
			*numPoints = 7;
			return get_FlagShape(size, size, rotation);

		case ptShapeRegular: 
			*numPoints = numSides + 1;
			return get_RegularShape(size, size, numSides, rotation);

		case ptShapeStar:
			*numPoints = numSides * 2 + 1;
			return get_StarShape(size, size, numSides, sidesRatio, rotation);

		case ptShapeCircle:
			*numPoints = 16 + 1;
			return get_RegularShape(size, size, 16, 0.0);

		default: 
			*numPoints = 0;
			return NULL;
	}
}

// ***************************************************************
//	  get_RegularShape()
// ***************************************************************
// Returns regular shape like trinagle, square, pentagon
float* get_RegularShape(float sizeX, float sizeY, int numSides, double rotation)
{
	float* points = new float[(numSides + 1) * 2];
	for (int i = 0; i <= numSides; i++)
    {
        float angle = (float)(i * (2 * pi_) / numSides + rotation/180.0*pi_);
        points[i * 2] = (float)(cos(angle) * sizeX/2.0f);
		points[i * 2 + 1] = (float)(sin(angle) * sizeY/2.0f);
    }
	return points;
}

// ***************************************************************
//	  get_StarShape()
// ***************************************************************
// Start with varible number of beams
float* get_StarShape(float sizeX, float sizeY, int numSides, float shapeRatio, double rotation)
{
	float* points = new float[(numSides * 2 + 1) * 2];
    for (int i = 0; i <= numSides * 2; i++)
    {
        double angle = i * pi_ / numSides + rotation/180.0*pi_;
        float x = (float)cos(angle) * sizeX / 2.0f;
        float y = (float)sin(angle) * sizeY / 2.0f;
        if (i % 2 == 0)
        {
            x *= shapeRatio;  // the shorter radius points of the star (0-1)
            y *= shapeRatio;
        }
        points[i * 2] = x;
		points[i * 2 + 1] = y;
    }
    return points;
}

// ***************************************************************
//	  get_CrossShape()
// ***************************************************************
float* get_CrossShape(float size, int numSides, float shapeRatio, double rotation)
{
    float r = size / 2.0f * shapeRatio;		// radius of circle around the regular shape in the center of cross
    double alpha = (pi_ * 2.0f) / numSides;
    double side = 2.0 * r * sin(alpha / 2.0);	// side of the regular shape in the center of cross
    float d = (float)(sqrt(pow(side / 2.0, 2.0) + pow(size / 2.0, 2.0)));	// distance to the side point of arm
    double beta = atan(side / size);			// angle between arm direction and direction ot it's side point
     
	float* points = new float[(numSides * 3 + 1) * 2];
    float x, y;
	for (int i = 0; i <= numSides * 3; i++)
    {
        if (i % 3 == 0)	
		{
			double angle = double(i/3) / numSides * (pi_ * 2.0) + rotation/180.0*pi_;
			angle -= beta;
			x = (float)cos(angle) * d;
			y = (float)sin(angle) * d;
		}
		else if(i % 3 == 1)
		{
			double angle = double(i/3) / numSides * (pi_ * 2.0) + rotation/180.0*pi_;
			angle += beta;
			x = (float)cos(angle) * d;
			y = (float)sin(angle) * d;
		}
		else if (i % 3 == 2)
		{
			double angle = double (i/3 * 2 + 1) /(numSides * 2.0) * (pi_ * 2.0) + rotation/180.0*pi_;
			x = (float)cos(angle) * r;
			y = (float)sin(angle) * r;
		}
		
        points[i * 2] = x;
		points[i * 2 + 1] = y;
    }
	return points;
}

// ***************************************************************
//	  get_ArrowShape()
// ***************************************************************
float* get_ArrowShape(float sizeX, float sizeY, double rotation)
{
	float xRatio = sizeX/10.0f;
	float yRatio = sizeY/10.0f;
	float* points = new float[8 * 2];
	int i = 0;

	points[i] = -5;	points[i + 1] = -1; i+=2;
	points[i] = -5;	points[i + 1] = 1; i+=2;
	points[i] = 1;	points[i + 1] = 1; i+=2;
	points[i] = 1;	points[i + 1] = 3; i+=2;
	points[i] = 5;	points[i + 1] = 0; i+=2;
	points[i] = 1;	points[i + 1] = -3; i+=2;
	points[i] = 1;	points[i + 1] = -1; i+=2;
	points[i] = -5;	points[i + 1] = -1;
	
	for (i = 0; i < 8; i++)
	{
		points[i * 2] = points[i * 2] * xRatio;
		points[i * 2 + 1] = points[i * 2 + 1] * yRatio;
	}

	if ( rotation != 0.0 )
	{
		double x,y;
		for (i = 0; i < 8; i++)
		{
			x = (double)points[i * 2];
			y = (double)points[i * 2 + 1];
			float angle = (float)GetPointAngle(x,y);
			float dist = (float)sqrt(pow(x,2) + pow(y,2));
			points[i * 2] = (float)(dist * sin(angle - rotation/180.0*pi_));
			points[i * 2 + 1] = (float)(dist * cos(angle - rotation/180.0*pi_));
		}
	}
	return points;
}

// ***************************************************************
//	  get_FlagShape()
// ***************************************************************
float* get_FlagShape(float sizeX, float sizeY, double rotation)
{
	float xRatio = sizeX/8.0f;
	float yRatio = sizeY/8.0f;
	float* points = new float[7 * 2];
	int i = 0;
	
	points[i] = 3;	points[i + 1] = 0; i+=2;
	points[i] = 0;	points[i + 1] = -2; i+=2;
	points[i] = 3;	points[i + 1] = -4; i+=2;
	points[i] = -3;	points[i + 1] = -4; i+=2;
	points[i] = -3;	points[i + 1] = 4; i+=2;
	points[i] = -3;	points[i + 1] = 0; i+=2;
	points[i] = 3;	points[i + 1] = 0;

	for (i = 0; i < 7; i++)
	{
		points[i * 2] = points[i * 2] * xRatio;
		points[i * 2 + 1] = points[i * 2 + 1] * yRatio;
	}
	
	if ( rotation != 0.0 )
	{
		double x,y;
		for (i = 0; i < 7; i++)
		{
			x = (double)points[i * 2];
			y = (double)points[i * 2 + 1];
			double angle = GetPointAngle(x,y);
			double dist = sqrt(pow(x,2) + pow(y,2));
			points[i * 2] = (float)(dist * sin(angle - rotation/180.0*pi_));
			points[i * 2 + 1] = (float)(dist * cos(angle - rotation/180.0*pi_));
		}

	}
	return points;
}

// predefined options
void get_DefaultSymbolOptions(tkDefaultPointSymbol symbol, tkPointShapeType& type, double& rotation, int& numSides, float& sidesRatio)
{
	rotation = 0.0;

	switch (symbol)
	{
		case dpsArrowDown:
			type = ptShapeArrow;
			rotation = 0;
			break;
		case dpsArrowLeft:
			type = ptShapeArrow;
			rotation = 90.0;
			break;
		case dpsArrowRight:
			type = ptShapeArrow;
			rotation = 180.0;
			break;
		case dpsArrowUp:
			type = ptShapeArrow;
			rotation = 270.0;
			break;
		case dpsAsterisk:
			type = ptShapeCross;
			numSides = 6;
			sidesRatio = 0.5;
			rotation = 0.0;
			break;
		case dpsCircle:
			type = ptShapeCircle;
			break;
		case dpsCross:
			type = ptShapeCross;
			numSides = 4;
			sidesRatio = 0.5;
			break;
		case dpsDiamond:
			type = ptShapeRegular;
			numSides = 4;
			break;
		case dpsFlag:
			type = ptShapeFlag;
			break;
		case dpsPentagon:
			type = ptShapeRegular;
			numSides = 5;
			break;
		case dpsSquare:
			type = ptShapeRegular;
			numSides = 4;
			rotation = 45.0;
			break;
		case dpsStar:
			type = ptShapeStar;
			numSides = 5;
			sidesRatio = 0.5;
			rotation = 18.0;
			break;
		case dpsTriangleUp:
			type = ptShapeRegular;
			numSides = 3;
			sidesRatio = 0.5;
			rotation = 90.0;
			break;
		case dpsTriangleDown:
			type = ptShapeRegular;
			numSides = 3;
			sidesRatio = 0.5;
			rotation = 30.0;
			break;
		case dpsTriangleLeft:
			type = ptShapeRegular;
			numSides = 3;
			sidesRatio = 0.5;
			rotation = 60.0;
			break;
		case dpsTriangleRight:
			type = ptShapeRegular;
			numSides = 3;
			sidesRatio = 0.5;
			rotation = 0.0;
			break;
		case dpsXCross:
			type = ptShapeCross;
			numSides = 4;
			sidesRatio = 0.5;
			rotation = 45.0;
			break;
	}
}