#pragma once
#include "colour.h"

class GdiPlusHelper
{
public:
	static Gdiplus::Status SaveBitmap(CStringW filename, int width, int height, colour* buffer, CStringW format);
private:	
	static Gdiplus::Status SaveManual(CStringW filename, int width, int height, colour* buffer, CStringW format);
	static Gdiplus::Status SaveAuto(CStringW filename, int width, int height, colour* buffer, CStringW format);
};

