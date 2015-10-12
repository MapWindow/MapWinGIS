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

// ***************************************************************
//		GetSpatialReference()
// ***************************************************************
void ProjectionHelper::GetSpatialReference(IGeoProjection* gp, OGRSpatialReference& result)
{
	if (!gp) return;

	CComBSTR bstr;
	gp->ExportToProj4(&bstr);

	USES_CONVERSION;
	CString s = OLE2A(bstr);
	result.importFromProj4(s);
}

// ***************************************************************
//		ImportFromEsri()
// ***************************************************************
OGRErr ProjectionHelper::ImportFromEsri(OGRSpatialReference* sr, CString proj)
{
	if (!sr) {
		return false;
	}

	char* s = proj.GetBuffer();

	return sr->importFromESRI(&s);
}

// ***************************************************************
//		ImportFromWkt()
// ***************************************************************
OGRErr ProjectionHelper::ImportFromWkt(OGRSpatialReference* sr, CString proj)
{
	if (!sr) {
		return false;
	}

	char* s = proj.GetBuffer();

	return sr->importFromWkt(&s);
}

// ***************************************************************
//		ExportFromWkt()
// ***************************************************************
OGRErr ProjectionHelper::ExportToWkt(OGRSpatialReference* sr, CString& proj)
{
	if (!sr) {
		return false;
	}

	char* s = NULL;
	OGRErr err = sr->exportToWkt(&s);

	proj = s;
	
	if (s) {
		CPLFree(s);
	}

	return err;
}

// ************************************************************
//		SupportsWorldWideTransform
// ************************************************************
bool ProjectionHelper::SupportsWorldWideTransform(IGeoProjection* mapProjection, IGeoProjection* wgsProjection)
{
	VARIANT_BOOL isGeograpic;
	mapProjection->get_IsGeographic(&isGeograpic);
	if (isGeograpic)
	{
		return true;
	}

	double TOLERANCE = 0.00001;
	VARIANT_BOOL vb1, vb2;
	double minLng = -180.0, maxLng = 180.0, minLat = -85.05112878, maxLat = 85.05112878;
	double x1 = minLng, x2 = maxLng, y1 = minLat, y2 = maxLat;

	m_globalSettings.suppressGdalErrors = true;
	wgsProjection->Transform(&x1, &y1, &vb1);
	wgsProjection->Transform(&x2, &y2, &vb2);
	m_globalSettings.suppressGdalErrors = false;

	if (vb1 && vb2)
	{
		mapProjection->Transform(&x1, &y1, &vb1);
		mapProjection->Transform(&x2, &y2, &vb1);
		if (abs(x1 - minLng) < TOLERANCE &&
			abs(x2 - maxLng) < TOLERANCE &&
			abs(y1 - minLat) < TOLERANCE &&
			abs(y2 - maxLat) < TOLERANCE)
		{
			return true;
		}
	}
	
	return false;
}