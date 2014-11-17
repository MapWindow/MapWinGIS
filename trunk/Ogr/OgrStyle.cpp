#include "stdafx.h"
#include "OgrStyle.h"
#include "OgrHelper.h"

char* STYLES_TABLE_NAME = "mw_styles";

// *************************************************************
//		HasStyleTable()
// *************************************************************
bool OgrStyleHelper::HasStyleTable(GDALDataset* dataset, CStringW layerName)
{
	USES_CONVERSION;
	CStringW sql;
	sql.Format(L"SELECT COUNT(*) FROM %s", A2W(STYLES_TABLE_NAME));
	CPLErrorReset();
	OGRLayer* lyr = dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
	bool hasTable = lyr != NULL;
	if (lyr)
		dataset->ReleaseResultSet(lyr);
	return hasTable;
}

// *************************************************************
//		HasStyleTable()
// *************************************************************
bool OgrStyleHelper::SaveStyle(GDALDataset* dataset, IShapefile* shapefile, CStringW layerName, CStringW styleName)
{
	CComBSTR bstr;
	shapefile->Serialize2(VARIANT_FALSE, VARIANT_FALSE, &bstr);

	USES_CONVERSION;
	CStringW style = OLE2W(bstr);
	style.Replace(L"\n", L"");
	style.Replace(L"'", L"''''");

	CStringW sql;
	sql.Format(L"INSERT INTO %s(layername, stylename, style) VALUES ('%s', '%s', '%s')", GetStyleTableName(layerName),
		layerName, styleName, style);

	CPLErrorReset();
	OGRLayer* layer = dataset->ExecuteSQL(OgrHelper::String2OgrString(sql), NULL, NULL);
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
//		CreateStyleTable()
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