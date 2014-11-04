#pragma once
#include "resource.h"
#include "errorcodes.h"
#include "MapWinGIS_i.h"
#include <map>

#pragma warning(disable:4482)	// non-standard extension used - for names of enumerations

struct GlobalSettingsInfo
{
	double minPolygonArea;
	double minAreaToPerimeterRatio;
	double clipperGcsMultiplicationFactor;
	bool shapefileFastMode;
	double invalidShapesBufferDistance;
	tkGridProxyMode gridProxyMode;
	bool shapefileFastUnion;
	CString gdalErrorMessage;
	tkCompositingQuality labelsCompositingQuality;
	tkSmoothingMode labelsSmoothingMode;
	std::map<tkLocalizedStrings, CStringW> shortUnitStrings;
	bool zoomToFirstLayer;
	tkCollisionMode labelsCollisionMode;
	tkGridProxyFormat gridProxyFormat;
	double MaxDirectGridSizeMb;
	int maxUniqueValuesCount;
	bool randomColorSchemeForGrids;
	PredefinedColorScheme defaultColorSchemeForGrids;
	tkShapeValidationMode inputValidation;
	tkShapeValidationMode outputValidation;
	tkGeometryEngine geometryEngine;
	bool saveGridColorSchemeToFile;
	int xmlFileVersion;
	CString xmlFilenameEncoding;
	tkTiffCompression tiffCompression;
	tkRasterOverviewCreation rasterOverviewCreation;
	int minOverviewWidth;
	tkGDALResamplingMethod rasterOverviewResampling;
	int tilesThreadPoolSize;
	bool loadSymbologyOnAddLayer;
	int tilesMaxZoomOnProjectionMismatch;
	tkInterpolationMode imageUpsamplingMode;
	tkInterpolationMode imageDownsamplingMode;
	tkInterpolationMode gridUpsamplingMode;			// currently not used
	tkInterpolationMode gridDownsamplingMode;
	tkOgrEncoding ogrEncoding;
	int ogrLayerMaxFeatureCount;
	bool autoChooseOgrLoadingMode;
	bool ogrUseStyles;
	bool useSchemesForStyles;
	bool saveOgrLabels;
	int getOgrMaxLabelCount() { return ogrLayerMaxFeatureCount; }
	int ctorCount;
	int dtorCount;
	
	GlobalSettingsInfo::GlobalSettingsInfo()
	{
		ctorCount = 0;
		dtorCount = 0;
		saveOgrLabels = false;
		useSchemesForStyles = false;
		ogrUseStyles = false;
		autoChooseOgrLoadingMode = true;
		ogrLayerMaxFeatureCount = 50000;
		ogrEncoding = oseUtf8;
		imageUpsamplingMode = imNone;
		imageDownsamplingMode = imBilinear;
		gridUpsamplingMode = imNone;
		gridDownsamplingMode = imBilinear;
		rasterOverviewResampling = tkGDALResamplingMethod::grmNearest;
		minOverviewWidth = 512;
		rasterOverviewCreation = tkRasterOverviewCreation::rocAuto;
		tiffCompression = tkTiffCompression::tkmAUTO;
		labelsCollisionMode = tkCollisionMode::LocalList;
		minPolygonArea = 1.0;
		minAreaToPerimeterRatio = 0.0001;
		clipperGcsMultiplicationFactor = 100000.0;
		shapefileFastMode = false;
		invalidShapesBufferDistance = 0.001;
		shapefileFastUnion = true;
		labelsCompositingQuality = HighQuality;
		labelsSmoothingMode = HighQualityMode ;
		zoomToFirstLayer = true;
		gridProxyFormat = gpfBmpProxy;
		MaxDirectGridSizeMb = 20.0;
		gridProxyMode = gpmAuto;
		maxUniqueValuesCount = 100;
		randomColorSchemeForGrids = true;
		defaultColorSchemeForGrids = SummerMountains;
		inputValidation = tkShapeValidationMode::NoValidation;
		outputValidation = tkShapeValidationMode::NoValidation;
		geometryEngine = tkGeometryEngine::engineGeos;
		saveGridColorSchemeToFile = true;
		xmlFileVersion = 2;
		xmlFilenameEncoding = "utf8";
		tilesThreadPoolSize = 5;
		loadSymbologyOnAddLayer = true;
		tilesMaxZoomOnProjectionMismatch = 6;

		shortUnitStrings[tkLocalizedStrings::lsHectars] = L"ha";
		shortUnitStrings[tkLocalizedStrings::lsMeters] = L"m";
		shortUnitStrings[tkLocalizedStrings::lsKilometers] = L"km";
		shortUnitStrings[tkLocalizedStrings::lsSquareKilometers] = L"sq.km";
		shortUnitStrings[tkLocalizedStrings::lsSquareMeters] = L"sq.m";
		shortUnitStrings[tkLocalizedStrings::lsMapUnits] = L"mu";
		shortUnitStrings[tkLocalizedStrings::lsSquareMapUnits] = L"sq.mu";
		shortUnitStrings[tkLocalizedStrings::lsMiles] = L"miles";
		shortUnitStrings[tkLocalizedStrings::lsFeet] = L"feet";
		shortUnitStrings[tkLocalizedStrings::lsLatitude] = L"Lat";
		shortUnitStrings[tkLocalizedStrings::lsLongitude] = L"Lng";
	}
	
	// *******************************************************
	//		GetTiffCompression()
	// *******************************************************
	CString GetTiffCompression()
	{
		switch(tiffCompression) {
			
			case tkmLZW:
				return "LZW";
			case tkmPACKBITS:
				return "PACKBITS";
			case tkmDEFLATE:
				return "DEFLATE";
			case tkmCCITTRLE:
				return "CCITTRLE";
			case tkmCCITTFAX3:
				return "CCITTFAX3";
			case tkmCCITTFAX4:
				return "CCITTFAX4";
			case tkmNONE:
				return "NONE";
			case tkmAUTO:
			case tkmJPEG:
			default:
				return "JPEG";
		}
	}

	PredefinedColorScheme GetGridColorScheme()
	{
		PredefinedColorScheme coloring = defaultColorSchemeForGrids;
		if (randomColorSchemeForGrids)
		{
			srand ((unsigned int)time(NULL));
			int r = rand();
			coloring = (PredefinedColorScheme)(r % 7);
		}
		return coloring;
	}

	CStringW GetLocalizedString(tkLocalizedStrings s)
	{
		return shortUnitStrings.find(s) != shortUnitStrings.end() ? shortUnitStrings[s] : L"";
	}

	double GetMinPolygonArea(IGeoProjection* proj)
	{
		VARIANT_BOOL isGeographic;
		if (proj == NULL)
			return minPolygonArea;

		proj->get_IsGeographic(&isGeographic);
		return this->GetMinPolygonArea(isGeographic);
	}	

	double GetMinPolygonArea(VARIANT_BOOL isGeographic)
	{
		if (isGeographic)
		{
			return minPolygonArea/ pow(110899.999942, 2.0);	 // degrees to meters
		}
		else
		{
			return minPolygonArea;
		}
	}	

	void SetGdalUtf8(bool turnon)
	{
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", turnon ? "YES" : "NO");
	}

	int GetTilesThreadPoolSize()
	{
		int size = tilesThreadPoolSize > 20 ? 20 : tilesThreadPoolSize;
		return size;
	}
};