#pragma once
#include "Extent.h"
#include <GdiPlus.h>

class CBaseDrawer
{
protected:	
	Extent* _extents; 
	double _pixelPerProjectionX;
	double _pixelPerProjectionY;
	CDC* _dc;
	Gdiplus::Graphics* _graphics;

public:
	CBaseDrawer(void);
	virtual ~CBaseDrawer(void);

	void PixelToProjection( double piX, double piY, double& prX, double& prY )
	{
		prX = _extents->left + piX* 1.0/_pixelPerProjectionX;
		prY = _extents->top - piY*1.0/_pixelPerProjectionY;
	}

	void ProjectionToPixel( double prX, double prY, double& piX, double& piY )
	{
		piX = (prX - _extents->left)* _pixelPerProjectionX;
		piY = (_extents->top - prY) * _pixelPerProjectionY;
	}
};
