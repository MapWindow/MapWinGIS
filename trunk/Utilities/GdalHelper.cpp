#include "stdafx.h"
#include "GdalHelper.h"

// **************************************************************
//		GdalOpen
// **************************************************************
GDALDataset* GdalHelper::OpenDatasetA( char* filenameUtf8 )
{
	m_globalSettings.SetGdalUtf8(true);

	GDALDataset* dt = (GDALDataset *) GDALOpen(filenameUtf8, GA_Update );

	if (dt == NULL) 
		dt = (GDALDataset *) GDALOpen(filenameUtf8, GA_ReadOnly );

	m_globalSettings.SetGdalUtf8(false);

	return dt;
}

GDALDataset* GdalHelper::OpenDatasetA( CStringA& filenameUtf8 )
{
	return OpenDatasetA(filenameUtf8.GetBuffer());
}

GDALDataset* GdalHelper::OpenDatasetW( CStringW filenameW )
{
	CStringA filenameA = Utility::ConvertToUtf8(filenameW);
	return OpenDatasetA(filenameA.GetBuffer());
}

GDALDataset* GdalHelper::OpenDatasetA(CStringA& filenameUtf8, GDALAccess accessType )
{
	return OpenDatasetA(filenameUtf8.GetBuffer(), accessType);
}

GDALDataset* GdalHelper::OpenDatasetA(char* filenameUtf8, GDALAccess accessType )
{
	m_globalSettings.SetGdalUtf8(true);

	GDALDataset* dt = (GDALDataset *) GDALOpen(filenameUtf8, accessType );

	m_globalSettings.SetGdalUtf8(false);

	return dt;
}

GDALDataset* GdalHelper::OpenDatasetW(CStringW filenameW, GDALAccess accessType )
{
	CStringA filenameA = Utility::ConvertToUtf8(filenameW);
	return OpenDatasetA(filenameA.GetBuffer(), accessType);
}

// **************************************************************
//		CPLParseXMLFile
// **************************************************************
bool GdalHelper::IsRgb(GDALDataset* dt)
{
	bool isRgb = false;
	if (dt)
	{
		bool r, g, b;
		r = g = b = false;
		int numBands = dt->GetRasterCount();
		for(int i = 1; i <= numBands; i++)
		{
			GDALRasterBand* band = dt->GetRasterBand(i);
			if (band)
			{
				GDALColorInterp ci = band->GetColorInterpretation();
				if (ci == GCI_RedBand) r = true;
				if (ci == GCI_GreenBand) g = true;
				if (ci == GCI_BlueBand) b = true;
			}
		}
		isRgb = r && g && b;
	}
	return isRgb;
}


// **************************************************************
//		CPLParseXMLFile
// **************************************************************
CPLXMLNode* GdalHelper::ParseXMLFile(CStringW filename)
{
	CStringA nameA = Utility::ConvertToUtf8(filename);
	m_globalSettings.SetGdalUtf8(true);
	CPLXMLNode* node = CPLParseXMLFile(nameA);
	m_globalSettings.SetGdalUtf8(false);
	return node;
}

// **************************************************************
//		SerializeXMLTreeToFile
// **************************************************************
int GdalHelper::SerializeXMLTreeToFile(CPLXMLNode* psTree, CStringW filename)
{
	CStringA nameA = Utility::ConvertToUtf8(filename);	
	m_globalSettings.SetGdalUtf8(true);
	int val = CPLSerializeXMLTreeToFile(psTree, nameA);
	m_globalSettings.SetGdalUtf8(false);
	return val;
}

// **************************************************************
//		CloseDataset
// **************************************************************
void GdalHelper::CloseDataset(GDALDataset* dt)
{
	if (dt)
	{
		int count = dt->Dereference();
		dt->Reference();
		if (count > 0)
			Debug::WriteLine("References remain on closing dataset: %d", count);
		GDALClose(dt);
	}
}