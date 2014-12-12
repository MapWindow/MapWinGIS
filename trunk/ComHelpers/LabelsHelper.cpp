#include "stdafx.h"
#include "LabelsHelper.h"
#include "ShapefileHelper.h"

// ******************************************************* 
//		UpdateLabelsPositioning()
// ******************************************************* 
// Should be called after change of shapefile type (CreateNew, Open, Resource, Close)
void LabelsHelper::UpdateLabelsPositioning(IShapefile* sf)
{
	ShpfileType type = ShapefileHelper::GetShapeType2D(sf);
	CComPtr<ILabels> labels = NULL;
	sf->get_Labels(&labels);
	if (labels)
	{
		if (type == SHP_POINT || type == SHP_MULTIPOINT)
		{
			labels->put_Positioning(lpCenter);
		}
		else if (type == SHP_POLYLINE)
		{
			labels->put_Positioning(lpLongestSegement);
		}
		else if (type == SHP_POLYGON)
		{
			labels->put_Positioning(lpCentroid);
		}
		else
		{
			labels->put_Positioning(lpNone);
		}
	}
}

// ******************************************************* 
//		GetFloatNumberFormat()
// ******************************************************* 
CString LabelsHelper::GetFloatNumberFormat(ILabels* labels)
{
	if (!labels) return m_globalSettings.floatNumberFormat;
	USES_CONVERSION;
	CComBSTR bstrFormat;
	labels->get_FloatNumberFormat(&bstrFormat);
	CString format = OLE2A(bstrFormat);
	if (format.GetLength() == 0)
		format = m_globalSettings.floatNumberFormat;
	return format;
}
