#include "stdafx.h"
#include "OgrStyle.h"
#include "OgrHelper.h"
#include "ShapefileHelper.h"
#include "ShapeStyleHelper.h"

char* STYLES_TABLE_NAME = "mw_styles";

// *************************************************************
//		LoadStyle()
// *************************************************************
CStringW OgrStyleHelper::LoadStyle(GDALDataset* dataset, CStringW styleTableName, CStringW layerName, CStringW styleName)
{
	USES_CONVERSION;
	CStringW sql;
	sql.Format(L"SELECT style FROM %s WHERE layername = '%s' AND stylename = '%s'", styleTableName, layerName, styleName);

	CStringW xml = L"";

	bool found = false;
	CPLErrorReset();
	OGRLayer* layer = dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	if (layer) {
		OGRFeature* ft = layer->GetNextFeature();
		if (ft) {
			const char* s = ft->GetFieldAsString(0);
			if (s) {
				xml = OgrHelper::OgrString2Unicode(s);
			}
			OGRFeature::DestroyFeature(ft);
		}
		dataset->ReleaseResultSet(layer);
	}
	return xml;
}

// *************************************************************
//		HasStyleTable()
// *************************************************************
bool OgrStyleHelper::HasStyleTable(GDALDataset* dataset, CStringW layerName)
{
	USES_CONVERSION;
	CStringW sql;
	sql.Format(L"SELECT COUNT(*) FROM %s", A2W(STYLES_TABLE_NAME));
	OGRLayer* lyr = dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	bool hasTable = lyr != NULL;
	if (lyr)
		dataset->ReleaseResultSet(lyr);
	else 
		CPLErrorReset();
	return hasTable;
}

// *************************************************************
//		HasStyleTable()
// *************************************************************
bool OgrStyleHelper::SaveStyle(GDALDataset* dataset, IShapefile* shapefile, CStringW layerName, CStringW styleName)
{
	CStringW xml = ShapeStyleHelper::GetSymbologyFileAsXml(shapefile);
	return SaveStyle(dataset, xml, layerName, styleName);
}

// *************************************************************
//		SaveStyle()
// *************************************************************
bool OgrStyleHelper::SaveStyle(GDALDataset* dataset, CStringW xml, CStringW layerName, CStringW styleName)
{
	xml.Replace(L"\n", L"");
	xml.Replace(L"'", L"''''");
	if (xml.GetLength() == 0) return false;

	CStringW sql;
	sql.Format(L"INSERT INTO %s(layername, stylename, style) VALUES ('%s', '%s', '%s')", GetStyleTableName(layerName),
		layerName, styleName, xml);

	CPLErrorReset();
	OGRLayer* layer = dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	const char* msg = CPLGetLastErrorMsg();
	return CPLGetLastErrorNo() == OGRERR_NONE;
}

// *************************************************************
//		GerStyleTableName()
// *************************************************************
CStringW OgrStyleHelper::GetStyleTableName(CStringW layerName)
{
	CStringW name;
	USES_CONVERSION;
	if (m_globalSettings.useSchemesForStyles) {
		name.Format(L"%s%s", GetDbSchemeName(layerName, true), A2W(STYLES_TABLE_NAME));
	}
	else {
		name = A2W(STYLES_TABLE_NAME);
	}

	return name;
}

// *************************************************************
//		GetDbSchemeName()
// *************************************************************
CStringW OgrStyleHelper::GetDbSchemeName(CStringW layerName, bool withTrailingPoint)
{
	int pos = layerName.ReverseFind(L'.');
	if (pos != -1)
	{
		return layerName.Mid(0, pos + withTrailingPoint ? 1 : 0);
	}
	return L"";
}

// *************************************************************
//		CreateStyleTable()
// *************************************************************
int OgrStyleHelper::CreateStyleTable(GDALDataset* dataset, CStringW layerName)
{
	if (OgrHelper::IsPostGisDatasource(dataset))
	{
		CPLErrorReset();
		CStringW schemaName = GetDbSchemeName(layerName, true);
		CStringW sql;
		sql.Format(L"CREATE Table %s (StyleId serial primary key, LayerName varchar(128), StyleName varchar(128), Style text, CONSTRAINT layer_style_unique UNIQUE (LayerName,StyleName));", GetStyleTableName());
		OGRLayer* lyr = dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
		return CPLGetLastErrorNo() == OGRERR_NONE ? tkNO_ERROR : tkOGR_FAILED_TO_CREATE_STYLE_TABLE;
	}
	else {
		return tkOGR_NO_STYLE_TABLE_CREATION;
	}
}

// *************************************************************
//		SupportsStyles()
// *************************************************************
bool OgrStyleHelper::SupportsStyles(GDALDataset* dataset, CStringW layerName)
{
	if (!OgrStyleHelper::HasStyleTable(dataset, layerName))
	{
		long errorCode = OgrStyleHelper::CreateStyleTable(dataset, layerName);
		if (errorCode != tkNO_ERROR)
			return false;
	}
	return true;
}

// *************************************************************
//		RemoveStyle()
// *************************************************************
bool OgrStyleHelper::RemoveStyle(GDALDataset* dataset, CStringW styleTableName, CStringW layerName, CStringW styleName)
{
	USES_CONVERSION;
	CStringW sql;
	sql.Format(L"DELETE FROM %s WHERE layername = '%s' AND stylename = '%s'", styleTableName, layerName, styleName);

	CPLErrorReset();
	OGRLayer* layer = dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	return CPLGetLastErrorNo() == OGRERR_NONE;
}