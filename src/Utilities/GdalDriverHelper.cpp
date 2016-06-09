#include "stdafx.h"
#include "GdalDriverHelper.h"


// ****************************************************************
//		GetDriverMetadata
// ****************************************************************
CString GdalDriverHelper::GetMetadata(GDALDriver* driver, tkGdalDriverMetadata metadata)
{
	if (!driver) return "";
	return const_cast<char*>(driver->GetMetadataItem(GdalHelper::GetMetadataNameString(metadata)));
}

// ****************************************************************
//		get_DriverMetadataCount
// ****************************************************************
int GdalDriverHelper::get_MetadataCount(GDALDriver* driver)
{
	if (!driver) return 0;

	char** data = driver->GetMetadata();
	return CSLCount(data);
}

// *************************************************************
//		get_DriverMetadataItem()
// *************************************************************
CString GdalDriverHelper::get_MetadataItem(GDALDriver* driver, int metadataIndex)
{
	if (!driver) return "";

	
	char** data = driver->GetMetadata();
	if (metadataIndex < 0 || metadataIndex >= CSLCount(data))
	{
		CallbackHelper::ErrorMsg("Driver metadata index is out of range.");
		return "";
	}
	else
	{
		return const_cast<char*>(CSLGetField(data, metadataIndex));
	}

	return "";
}
