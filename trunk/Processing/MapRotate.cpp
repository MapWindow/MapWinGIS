/* ****************************************************************** **
** MapRotate.cpp: implementation of the MapRotate class.
**
** ****************************************************************** */
#include <stdlib.h>
#include "stdafx.h"
#include "MapRotate.h"
#include <strsafe.h>


/* *************************************************************************** **
** Rotate::Rotate()
**
** Constructor
**
** *************************************************************************** */
Rotate::Rotate()
  {
	curExtent.left = 0;
	curExtent.top = 0;
	curExtent.right = 0;
	curExtent.bottom = 0;
  xAxisDiff = 0;
  yAxisDiff = 0;
	_curHeight = 0;
	_curWidth = 0;
	_lastError = true;
	_hBMBuffer = NULL;
  }
/* *************************************************************************** **
** Rotate::~Rotate()
**
** Destructor
**
** *************************************************************************** */
Rotate::~Rotate()
	{
	}
/* *************************************************************************** **
** bool Rotate::calcRotatedExtent(long curViewWidth, long curViewHeight)
**
** Function to caculate the rotated extent for the current rotation angle.
**
** *************************************************************************** */
void Rotate::calcRotatedExtent(long curViewWidth, long curViewHeight)
	{
	setupWorldTransform(curViewWidth, curViewHeight);
	}
/* *************************************************************************** **
** bool Rotate::getOriginalPixelPoint(long rotatedX, long rotatedY, long *origX, long *origY)
**
** Function to calculate the real (unrotated XY's) from the 
** mouse xy's passed to function (rotated xy's).
**
** *************************************************************************** */
void Rotate::getOriginalPixelPoint(long rotatedX, long rotatedY, long *origX, long *origY)
	{
  LONG xLen = (_curWidth / 2) - rotatedX;
  LONG yLen = (_curHeight / 2) - rotatedY;
  
  float angle1 = atan2((float) yLen, (float) xLen);

  float diffAngle = angle1 + (float)((degAngle * pi) / 180.0);

  float radLen = sqrtf((float)((xLen * xLen) + (yLen * yLen)));
  
  float yNew1 = (float)(_curHeight / 2.0) - (radLen * sinf(diffAngle));
  float xNew1 = (float)(_curWidth / 2.0) - (radLen * cosf(diffAngle));

  *origX = (long)xNew1;
  *origY = (long)yNew1;
	}
/* *************************************************************************** **
** bool Rotate::getNewPixelPoint(long origX, long origY, long *rotatedX, long *rotatedY)
**
** Function to calculate the new (rotated XY's) from the 
** current xy's passed to function.
**
** *************************************************************************** */
void Rotate::getNewPixelPoint(long origX, long origY, long *rotatedX, long *rotatedY)
	{
  LONG xLen = (_curWidth / 2) - origX;
  LONG yLen = (_curHeight / 2) - origY;
  
  float angle1 = atan2((float) yLen, (float) xLen);

  float tmpA1 = angle1 * (float)(180.00 / pi);

  float diffAngle = angle1 - (float)((degAngle * pi) / 180.0);
  
  float tmpdiffA = diffAngle * (float)(180.00 / pi);

  float radLen = sqrtf((float)((xLen * xLen) + (yLen * yLen)));
  
  float yNew1 = (float)(_curHeight / 2.0) + (radLen * sinf(diffAngle));
  float xNew1 = (float)(_curWidth / 2.0) + (radLen * cosf(diffAngle));

  *rotatedX = (long)xNew1;
  *rotatedY = (long)yNew1;
	}
/* *************************************************************************** **
** bool Rotate::setupRotateBackbuffer(HDC rotateBuffer, HDC controlDC, 
**                                                           OLE_COLOR backColor)
**
** Function to set up a backbuffer for rotating the output.
**
** *************************************************************************** */
bool Rotate::setupRotateBackbuffer(HDC rotateBuffer, HDC controlDC, OLE_COLOR backColor)
	{
	RECT  rotateExtent;
	
  // Set Graphics mode to advanced, (clearer output)
	_saveGraphicsMode = GetGraphicsMode(rotateBuffer);

	if (SetGraphicsMode(rotateBuffer, GM_ADVANCED) == 0)
		{
		_lastError = GetLastError();
		return false;
		}

	// Setup the XFORM structure
	setupWorldTransform(_curWidth, _curHeight);

	// bitmap for scribbling on
	if ((_hBMBuffer = CreateCompatibleBitmap(controlDC, rotatedWidth, rotatedHeight)) == NULL)
		{
		_lastError = GetLastError();
		return false;
		}

  SelectObject(rotateBuffer, _hBMBuffer);

	rotateExtent.left = 0;
	rotateExtent.top = 0;
	rotateExtent.right =  rotatedWidth;
	rotateExtent.bottom = rotatedHeight;

	HBRUSH fillBrush = CreateSolidBrush(backColor);
	FillRect(rotateBuffer, &(rotateExtent), fillBrush);
	DeleteObject(fillBrush);

	// Save current setting, then apply new
	if (GetWorldTransform(rotateBuffer, &_savedXform))
		{
	  if (SetWorldTransform(rotateBuffer, &_curXform) == 0)
			{
			_lastError = GetLastError();
			return false;
			}
		}
	else
		{
		_lastError = GetLastError();
		return false;
		}
 
  return true;
	}
/* ***************************************************************************
** bool Rotate::setupWorldTransform(HDC rotateBuffer)
**
** Function to setup the WorldTransform on the backbuffer.
**
** *************************************************************************** */
void Rotate::setupWorldTransform(long bufWidth, long bufHeight)
	{
	float x1, x2, x3, y1, y2, y3;
	float minX, minY, maxX, maxY;
	float rotCenX, rotCenY;
	float origRotX, origRotY, newRotX, newRotY;
	float newWidth, newHeight;

  origRotX = (float)(bufWidth / 2.0);
	origRotY = (float)(bufHeight / 2.0);

  x1 = (float)(bufHeight * _sine);
  y1 = (float)(bufHeight * _cosine);
  x2 = (float)(bufWidth * _cosine) + (float)(bufHeight * _sine);
  y2 = (float)(bufHeight * _cosine) - (float)(bufWidth * _sine);
  x3 = (float)(bufWidth * _cosine);
  y3 = (float)(-bufWidth * _sine);
	
	minX = min(0,min(x1, min(x2, x3)));
	minY = min(0,min(y1, min(y2, y3)));
	maxX = max(0, max(x1, max(x2, x3)));
	maxY = max(0, max(y1, max(y2, y3)));
	
	calcRotatedPoint(origRotX, origRotY, minX, minY, &rotCenX, &rotCenY);

	xAxisDiff = abs(rotCenX - origRotX);
	yAxisDiff = abs(rotCenY - origRotY);

	newRotX = origRotX + xAxisDiff;
  newRotY = origRotY + yAxisDiff;
	newWidth = bufWidth + (float)(2 * xAxisDiff);
  newHeight = bufHeight + (float)(2.0 * yAxisDiff);

  x1 = newHeight * _sine;
  y1 = newHeight * _cosine;
  x2 = newWidth * _cosine + newHeight * _sine;
  y2 = newHeight * _cosine - newWidth * _sine;
  x3 = newWidth * _cosine;
  y3 = -newWidth * _sine;
	
	minX = min(0,min(x1, min(x2, x3)));
	minY = min(0,min(y1, min(y2, y3)));
	maxX = max(0, max(x1, max(x2, x3)));
	maxY = max(0, max(y1, max(y2, y3)));
	
	calcRotatedPoint(newRotX, newRotY, minX, minY, &rotCenX, &rotCenY);

	_curXform.eM11 = _cosine;
	_curXform.eM12 = -_sine;
	_curXform.eM21 = _sine;
	_curXform.eM22 = _cosine;
	_curXform.eDx =  -minX - (rotCenX - newRotX) - xAxisDiff;
	_curXform.eDy =  -minY - (rotCenY - newRotY) - yAxisDiff;
		
	rotatedWidth =  Round(maxX - minX);
	rotatedHeight = Round(maxY - minY);

	}
/* ***************************************************************************
** bool Rotate::resetWorldTransform(HDC rotateBuffer)
**
** Function to reset WorldTransform on the backbuffer.
**
** *************************************************************************** */
bool Rotate::resetWorldTransform(HDC rotateBuffer)
	{
	// Reset World Transform to previous value (if not BitBlt fails , invalid parameter)
	if (SetWorldTransform(rotateBuffer, &_savedXform) != 0)
		{
	  if (SetGraphicsMode(rotateBuffer, _saveGraphicsMode) == 0)
			{
			_lastError = GetLastError();
			return false;
			}
		}
	else
		{
		_lastError = GetLastError();
		return false;
		}

	return true;
	}
/* ***************************************************************************
** bool Rotate::cleanupRotation(HDC rotateBuffer)
**
** Function to cleanup up any DC's.
**
** *************************************************************************** */
bool Rotate::cleanupRotation(HDC rotateBuffer)
	{
	if (_hBMBuffer != NULL )
		{
		if (DeleteObject(_hBMBuffer) == 0)
			{
			_lastError = GetLastError();
			return false;
			}
		}

	return true;
	}
/* *************************************************************************** **
** void Rotate::calcRotatedPoint(float ptX, float ptY, long minX, long minY,
**                                                 float *rotPtX, float *rotPtY)
**
** Function to calculate a rotated point
**
** *************************************************************************** */
void Rotate::calcRotatedPoint(float ptX, float ptY, float minX, float minY, float *rotPtX, float *rotPtY)
	{
	*rotPtX = (ptX * _cosine) + (ptY * _sine) + (-minX);
	*rotPtY = (ptX * -_sine) + (ptY * _cosine) + (-minY);
	}
/* ***************************************************************************
** void Rotate::drawCenter(HDC tmpBuffer)
**
** Function to draw cross hairs in center of map. Used in testing.
**
** *************************************************************************** */
void Rotate::drawCenter(HDC tmpBuffer, COLORREF color)
	{
	POINT pts[2];
	HPEN penOld = NULL;
  HPEN penBlue = NULL;

	penBlue = CreatePen(PS_SOLID, 1, color);
  penOld = (HPEN) SelectObject(tmpBuffer, penBlue);

	pts[0].x = (int)(_curWidth / 2 - 100);
	pts[0].y = (int)(_curHeight / 2);
	pts[1].x = (int)(_curWidth / 2 + 100);
	pts[1].y = (int)(_curHeight / 2);
	Polyline(tmpBuffer,(CONST POINT *) &pts, 2); 
	
	pts[0].x = (int)(_curWidth / 2);
	pts[0].y = (int)(_curHeight / 2 - 100);
	pts[1].x = (int)(_curWidth / 2);
	pts[1].y = (int)(_curHeight / 2 + 100);
	Polyline(tmpBuffer,(CONST POINT *) &pts, 2); 

	if (penOld != NULL)
		{
		SelectObject(tmpBuffer, penOld);
		DeleteObject(penBlue);
		}
	}
/* ***************************************************************************
** long Rotate::Round(float val)
**
** Function to float value to integer
**
** *************************************************************************** */
inline long Rotate::Round(float val)
  {
	if (val > 0)
		return((long)(val + 0.5));
	else
	  return((long)(val - 0.5));
	}

