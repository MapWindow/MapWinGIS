#pragma once
#include "Image.h"

class ImageHelper
{
private:
	static void SetTransparency(Gdiplus::ImageAttributes& attributes, bool useTransparency, OLE_COLOR clr1, OLE_COLOR clr2);
public:
	static bool IsEmpty(IImage* img);
	static CImageClass* Cast(IImage* img);
	static Gdiplus::ColorMatrix GetColorMatrix(IImage* img);
	static Gdiplus::ImageAttributes* GetImageAttributes(IImage* img);
	static Gdiplus::ImageAttributes* GetImageAttributes(float alpha, bool useTransparency, OLE_COLOR clr1, OLE_COLOR clr2);
};

