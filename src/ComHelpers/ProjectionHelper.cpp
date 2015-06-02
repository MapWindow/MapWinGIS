#include "stdafx.h"
#include "ProjectionHelper.h"

// ***************************************************************
//		IsEmpty()
// ***************************************************************
bool ProjectionHelper::IsEmpty(IGeoProjection* gp)
{
	if (!gp) return true;
	VARIANT_BOOL isEmpty;
	gp->get_IsEmpty(&isEmpty);
	return isEmpty ? true : false;
}

// ***************************************************************
//		IsGeographic()
// ***************************************************************
bool ProjectionHelper::IsGeographic(IGeoProjection* gp)
{
	if (!gp) return false;
	VARIANT_BOOL geograhpic;
	gp->get_IsGeographic(&geograhpic);
	return geograhpic ? true : false;
}

// ***************************************************************
//		IsSame()
// ***************************************************************
bool ProjectionHelper::IsSame(IGeoProjection* gp1, IGeoProjection* gp2, IExtents* bounds, int sampleSize)
{
	if (!gp1 || !gp2 || !bounds) return false;
	VARIANT_BOOL vb;
	gp1->get_IsSameExt(gp2, bounds, sampleSize, &vb);
	return vb ? true: false;
}

// ***************************************************************
//		ToString()
// ***************************************************************
CString ProjectionHelper::ToString(IGeoProjection* gp)
{
	if (!gp) return "";

	CComBSTR str;
	if (!ProjectionHelper::IsEmpty(gp)) {
		gp->ExportToProj4(&str);
	}
	else {
		str = L"";
	}

	USES_CONVERSION;
	return str.Length() > 0 ? OLE2A(str) : "<empty>";
}