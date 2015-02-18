#pragma once;

#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif

class CImageAttributesEx: public Gdiplus::ImageAttributes
{
public:
	// Custom constructor
	CImageAttributesEx::CImageAttributesEx(double alphaValue, bool setToGray, bool useTransparency, int transpColor, int transpColor2)
	{
		Gdiplus::ColorMatrix clrMatrix = { 
										1.0f, 0.0f,	0.0f,		0.0f, 0.0f,
										0.0f, 1.0f,	0.0f,		0.0f, 0.0f,
										0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
										0.0f, 0.0f, 0.0f, (float)alphaValue, 0.0f,
										0.0f, 0.0f, 0.0f,		0.0f, 1.0f };
		if (setToGray)
		{
			float r = 0.299f;
			float g = 0.588f;
			float b = 0.111f;	
			for(int i = 0; i < 3; i ++)
			{
				clrMatrix.m[0][i] = r;
				clrMatrix.m[1][i] = g;
				clrMatrix.m[2][i] = b;
			}
		}
		
		this->SetWrapMode(Gdiplus::WrapModeTileFlipXY);

		this->SetColorMatrix(&clrMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

		// setting range of colors which will be transparent
		if (useTransparency)
		{
			Gdiplus::Color color1(MIN(GetRValue(transpColor), GetRValue(transpColor2)),
									MIN(GetGValue(transpColor), GetGValue(transpColor2)),
									MIN(GetBValue(transpColor), GetBValue(transpColor2)));
			
			Gdiplus::Color color2(MAX(GetRValue(transpColor), GetRValue(transpColor2)),
									MAX(GetGValue(transpColor), GetGValue(transpColor2)),
									MAX(GetBValue(transpColor), GetBValue(transpColor2)));
			
			this->SetColorKey(color1, color2, Gdiplus::ColorAdjustTypeBitmap);
		}
	}
};