#include "stdafx.h"
#include "GdiPlusHelper.h"
#include "ImageHelper.h"

// ********************************************************************
//		SaveBitmap
// ********************************************************************
Gdiplus::Status GdiPlusHelper::SaveBitmap(CStringW filename, int width, int height, colour* buffer, CStringW format)
{
	return SaveAuto(filename, width, height, buffer, format);
}

// ********************************************************************
//		SaveManual
// ********************************************************************
Gdiplus::Status GdiPlusHelper::SaveManual(CStringW filename, int width, int height, colour* buffer, CStringW format)
{
	Gdiplus::Bitmap *bmap = new Gdiplus::Bitmap(width, height);

	int c = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Gdiplus::Color newClr(buffer[c].alpha, buffer[c].blue, buffer[c].green, buffer[c].red);
			bmap->SetPixel(j, i, newClr);
			c++;
		}
	}

	  CLSID  clsid;
	  int    result = Utility::GetEncoderClsid(format, &clsid);

	  Gdiplus::Status status = bmap->Save(filename, &clsid);
	  delete bmap;

	  return status;
}

// ********************************************************************
//		SaveBitmapData
// ********************************************************************
Gdiplus::Status GdiPlusHelper::SaveAuto(CStringW filename, int width, int height, colour* buffer, CStringW format)
{
	int bitsPerPixel = 32;
	int pad = ImageHelper::GetRowBytePad(width, bitsPerPixel);
	int stride = width * bitsPerPixel / 8 + pad;
	Gdiplus::Bitmap img(width, height, stride, PixelFormat32bppARGB, reinterpret_cast<BYTE*>(buffer));

	CLSID  clsid;
	int    result = Utility::GetEncoderClsid(format, &clsid);

	Gdiplus::Status status = img.Save(filename, &clsid);

	return status;
}

