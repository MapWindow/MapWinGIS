#include <StdAfx.h>
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
bool ProjectionHelper::IsSame(const OGRSpatialReference* sr1, const OGRSpatialReference* sr2)
{
	if (!sr1 || !sr2) return false;
	// use OGRSpatialReference to test same-ness
	return(sr1->IsSame(sr2) == TRUE);
}

// ***************************************************************
//		IsSame() (including Extents verification)
// ***************************************************************
bool ProjectionHelper::IsSame(IGeoProjection* gp1, IGeoProjection* gp2, IExtents* bounds, int sampleSize)
{
	if (!gp1 || !gp2 || !bounds) return false;
	VARIANT_BOOL vb;
	gp1->get_IsSameExt(gp2, bounds, sampleSize, &vb);
	return vb ? true : false;
}

// ***************************************************************
//		ToString()
// ***************************************************************
CString ProjectionHelper::ToString(IGeoProjection* gp)
{
	if (!gp) return "";

	CComBSTR str;
	if (!IsEmpty(gp)) {
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

	// TODO: importFromProj4 is deprecated:
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

	char* apszPrj[2];
	char* s = proj.GetBuffer();
	apszPrj[0] = s;
	apszPrj[1] = nullptr;

	// Parameters: papszPrj	NULL terminated list of strings containing the definition.
	return sr->importFromESRI(apszPrj);
}

// ***************************************************************
//		ImportFromWkt()
// ***************************************************************
OGRErr ProjectionHelper::ImportFromWkt(OGRSpatialReference* sr, CString proj)
{
	if (!sr)
	{
		return OGRERR_FAILURE;
	}

	const char* s = proj.GetBuffer();

	OGRErr result = OGRERR_NONE;
	// use newer method importFromWkt(const char*)
	if ((result = sr->importFromWkt(&s)) == OGRERR_NONE)
	{
		// then Validate (https://gdal.org/api/ogrspatialref.html#_CPPv4NK19OGRSpatialReference8ValidateEv)
		return sr->Validate();
	}
	else
	{
		// return error from failed import
		return result;
	}
}

// ***************************************************************
//		ExportFromWkt()
// ***************************************************************
OGRErr ProjectionHelper::ExportToWkt(OGRSpatialReference* sr, CString& proj)
{
	if (!sr) {
		return false;
	}

	char* s = nullptr;
	const OGRErr err = sr->exportToWkt(&s);

	proj = s;

	if (s) {
		CPLFree(s);
	}

	return err;
}

// ***************************************************************
//		ExportFromWktEx()
//  Starting with GDAL 3.0, the OGRSpatialReference::exportToWkt() method accepts options
// ***************************************************************
OGRErr ProjectionHelper::ExportToWktEx(OGRSpatialReference* sr, CString& proj)
{
	if (!sr) {
		return false;
	}

	char* s = nullptr;
	const char* apszOptions[3] = { "FORMAT=WKT2_2019", "MULTILINE=YES", nullptr };
	const OGRErr err = sr->exportToWkt(&s, apszOptions);

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
	VARIANT_BOOL vb1, vb2, vb3;
	double minLng = -180.0, maxLng = 180.0, minLat = -85.05112878, maxLat = 85.05112878;
	double x1 = minLng, x2 = maxLng, y1 = minLat, y2 = maxLat;
	double x3 = 0.0, y3 = 0.0;

	m_globalSettings.suppressGdalErrors = true;
	wgsProjection->Transform(&x1, &y1, &vb1);
	wgsProjection->Transform(&x2, &y2, &vb2);
	wgsProjection->Transform(&x3, &y3, &vb3);
	m_globalSettings.suppressGdalErrors = false;

	if ((y3 > y2 && y3 > y1) || (y3 < y2 && y3 < y1))
	{
		// this check that direction of Y axis is the same on its whole length;
		// for Lambert Conformal Conic, that's not the case
		return false;
	}

	if (vb1 && vb2 && vb3)
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

