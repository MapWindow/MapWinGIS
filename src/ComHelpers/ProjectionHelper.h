#pragma once
class ProjectionHelper
{
public:
	static bool IsEmpty(IGeoProjection* gp);
	static bool IsGeographic(IGeoProjection* gp);
	static bool IsSame(const OGRSpatialReference* sr1, const OGRSpatialReference* sr2);
	static bool IsSame(IGeoProjection* gp1, IGeoProjection* gp2, IExtents* bounds, int sampleSize);
	static CString ToString(IGeoProjection* gp);
	static void GetSpatialReference(IGeoProjection* gp, OGRSpatialReference& result);
	static OGRErr ImportFromEsri(OGRSpatialReference* sr, CString proj);
	static OGRErr ImportFromWkt(OGRSpatialReference* sr, CString proj);
	static OGRErr ExportToWkt(OGRSpatialReference* sr, CString& proj);
	static OGRErr ExportToWktEx(OGRSpatialReference* sr, CString& proj);
	static bool SupportsWorldWideTransform(IGeoProjection* mapProjection, IGeoProjection* wgsProjection);
};

