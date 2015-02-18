#pragma once
class GeoProcessing
{
public:
	static void CopyFields(IShapefile* sfSubject, IShapefile* sfOverlay, IShapefile* sfResult, map<long, long>& fieldMap, bool mergeFields = false);
	
};

