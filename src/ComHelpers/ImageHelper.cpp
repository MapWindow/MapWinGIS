#include "stdafx.h"
#include "ImageHelper.h"
#include "Image.h"

// *************************************************************
//		IsEmpty()
// *************************************************************
bool ImageHelper::IsEmpty(IImage* img)
{
	if (!img) return true;
	VARIANT_BOOL isEmpty;
	img->get_IsEmpty(&isEmpty);
	return isEmpty ? true: false;
}

// *************************************************************
//		Cast()
// *************************************************************
CImageClass* ImageHelper::Cast(IImage* img)
{
	return ((CImageClass*)img);
}

// *************************************************************
//		GetColorMatrix()
// *************************************************************
Gdiplus::ColorMatrix ImageHelper::GetColorMatrix(IImage* img)
{
	if (!img) {
		Gdiplus::ColorMatrix m;
		m.m[3][3] = 1.0f;
		return m;
	}

	return Cast(img)->GetColorMatrix();
}

// *************************************************************
//		GetImageAttributes()
// *************************************************************
Gdiplus::ImageAttributes* ImageHelper::GetImageAttributes(IImage* img)
{
	if (!img) return NULL;

	Gdiplus::ImageAttributes* attr = new Gdiplus::ImageAttributes();

	attr->SetWrapMode(Gdiplus::WrapModeTileFlipXY);

	VARIANT_BOOL useTransparency;
	img->get_UseTransparencyColor(&useTransparency);

	OLE_COLOR clr1, clr2;
	img->get_TransparencyColor(&clr1);
	img->get_TransparencyColor2(&clr2);

	SetTransparency(*attr, useTransparency ? true : false, clr1, clr2);

	Gdiplus::ColorMatrix m = GetColorMatrix(img);
	attr->SetColorMatrix(&m);

	Gdiplus::REAL gamma;	
	img->get_Gamma(&gamma);
	attr->SetGamma(gamma);

	return attr;
}

// *************************************************************
//		GetImageAttributes()
// *************************************************************
Gdiplus::ImageAttributes* ImageHelper::GetImageAttributes(float alpha, bool useTransparency, OLE_COLOR clr1, OLE_COLOR clr2)
{
	Gdiplus::ImageAttributes* attr = new Gdiplus::ImageAttributes();

	attr->SetWrapMode(Gdiplus::WrapModeTileFlipXY);

	Gdiplus::ColorMatrix matrix;
	matrix.m[3][3] = alpha;
	attr->SetColorMatrix(&matrix);

	SetTransparency(*attr, useTransparency, clr1, clr2);

	return attr;
}

// *************************************************************
//		SetTransparency()
// *************************************************************
void ImageHelper::SetTransparency(Gdiplus::ImageAttributes& attributes, bool useTransparency, OLE_COLOR clr1, OLE_COLOR clr2)
{
	// setting range of colors which will be transparent
	if (useTransparency)
	{
		Gdiplus::Color color1(MIN(GetRValue(clr1), GetRValue(clr2)),
			MIN(GetGValue(clr1), GetGValue(clr2)),
			MIN(GetBValue(clr1), GetBValue(clr2)));

		Gdiplus::Color color2(MAX(GetRValue(clr1), GetRValue(clr2)),
			MAX(GetGValue(clr1), GetGValue(clr2)),
			MAX(GetBValue(clr1), GetBValue(clr2)));

		attributes.SetColorKey(color1, color2);
	}
}