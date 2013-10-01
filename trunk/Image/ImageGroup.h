#pragma once
#include "MapWinGis.h"
#include <vector>
#include "gdiplus.h"
#include "Extent.h"

// Content of the image resampled to the screen resolution, as a GDIPlus bitmap
// Used to speed up drawing of grouped images along with mwTiles
struct ScreenBitmap
{
public:
	Gdiplus::Bitmap* bitmap;
	// position to draw
	double left;	
	double top;
	// map scale and position for which it was obtained
	Extent extents; 
	double pixelPerProjectionX;
	double pixelPerProjectionY;
	long viewWidth;
	long viewHeight;
	
	ScreenBitmap()
	{
		bitmap = NULL;
		pixelPerProjectionX = 0.0;
		pixelPerProjectionY = 0.0;
		viewWidth = 0;
		viewHeight = 0;
	}

	~ScreenBitmap()
	{
		if (bitmap != NULL)
		{
			delete bitmap;
			bitmap = NULL;
		}
	}
};

// for grouped images
struct ImageGroup
{
public:
	double dx;
	double dy;
	double xllCenter;
	double yllCenter;
	int width;
	int height;
	std::vector<int> imageIndices;
	bool isValid;
	bool wasDrawn;
	IImage* image;
	ScreenBitmap* screenBitmap;
	
	ImageGroup(double dX, double dY, double XllCenter, double YllCenter, int Width, int Height)
	{
		dx = dX;
		dy = dY;
		width = Width;
		height = Height;
		xllCenter = XllCenter;
		yllCenter = YllCenter;
		isValid = true;
		wasDrawn = false;
		image = NULL;
		screenBitmap = NULL;
	};

	~ImageGroup()
	{
		if (image != NULL)
		{
			image->Release();
			image = NULL;
		}

		if (screenBitmap != NULL)
		{
			delete screenBitmap;
			screenBitmap = NULL;
		}

	}
	
	// comparing 2 groups, their positions and their images
	bool ImageGroup::operator==(const ImageGroup& group)
	{
		if (this->dx != group.dx) return false;
		if (this->dy != group.dy) return false;
		if (this->height != group.height) return false;
		if (this->width != group.width) return false;
		if (this->xllCenter != group.xllCenter) return false;
		if (this->yllCenter != group.yllCenter) return false;

		if (this->imageIndices.size() != group.imageIndices.size()) return false;

		for (unsigned int i = 0; i < group.imageIndices.size(); i++)
		{
			if (this->imageIndices[i] != group.imageIndices[i])
				return false;
		}
		return true;
	}
};