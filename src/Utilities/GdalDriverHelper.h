#pragma once
class GdalDriverHelper
{
public:
	static CString GetMetadata(GDALDriver* driver, tkGdalDriverMetadata metadata);
	static int get_MetadataCount(GDALDriver* driver);
	static CString get_MetadataItem(GDALDriver* driver, int metadataIndex);
};

