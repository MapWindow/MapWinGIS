#include "stdafx.h"
#include "ImageHelper.h"

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
