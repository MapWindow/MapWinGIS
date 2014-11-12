#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <cmath>
using namespace std;

#define pi_ 3.1415926535897932384626433832795

class Rotate
	{

	public:
		float        degAngle;           // Angle in degrees to rotate map
		RECT         curExtent;          // left and top not = 0
    LONG         rotatedHeight;
		LONG 	       rotatedWidth;
    float        xAxisDiff, yAxisDiff;
    float        _cosine;
		float        _sine;

		Rotate(void);
	  ~Rotate(void);
    
		bool setupRotateBackbuffer(HDC rotateBuffer, HDC controlDC, OLE_COLOR backColor);
    bool resetWorldTransform(HDC rotateBuffer);
    bool cleanupRotation(HDC rotateBuffer);
    void calcRotatedExtent(long width, long height);
		
		void drawCenter(HDC tmpBuffer, COLORREF color);
    void displayError(DWORD dw);
    void calcRotatedPoint(float ptX, float ptY, float minX, float minY, float *rotPtX, float *rotPtY);
    void getOriginalPixelPoint(long rotatedX, long rotatedY, long *origX, long *origY);
    void getNewPixelPoint(long origX, long origY, long *rotatedX, long *rotatedY);

	void AdjustRect(CRect& rect);

		void setRotateAngle(float angle)
			{
			degAngle = angle;
			_cosine = (float) cos((angle * pi_) / 180);
	    _sine   = (float) sin((angle * pi_) / 180);
			}

 		float getRotateAngle()
			{
			return(degAngle);
			}
   
		void setSize(RECT curExt)
			{
      curExtent = curExt;
	    _curHeight = curExtent.bottom - curExtent.top;
	    _curWidth = curExtent.right - curExtent.left;
		  }
		
		void getSize(int *ret_width, int *ret_height)
			{
			*ret_width = _curWidth;
			*ret_height = _curHeight;
			}
		
		RECT getSize()
			{
			return(curExtent);
			}

		void setLastError(DWORD lastError)
			{
			_lastError = lastError;
			}

		DWORD getLastError()
			{
			return _lastError;
			}

	private:
 		HBITMAP _hBMBuffer;
    LONG    _curHeight;             // Current view height in pixels
		LONG 	  _curWidth;              // Current viewing width in pixels
		DWORD   _lastError;
		XFORM   _savedXform, _curXform;
 	  int     _saveGraphicsMode;

    void         setupWorldTransform(long bufWidth, long bufHeight);
    inline long  Round(float val);

	};
