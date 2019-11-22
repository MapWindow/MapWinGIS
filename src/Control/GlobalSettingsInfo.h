#pragma once
#include "MapWinGIS_i.h"
#include <map>

#pragma warning(disable:4482)	// non-standard extension used - for names of enumerations

struct GlobalSettingsInfo {
	const double METERS_PER_DEGREE = 110899.999942;
	tkProxyAuthentication proxyAuthentication;
	bool forceHideLabels;
	bool ogrLayerForceUpdateMode;
	bool autoChooseRenderingHintForLabels;
	long maxReprojectionShapeCount;
	ICallback* callback;
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
	tkInterpolationMode gridUpsamplingMode; // currently not used
	tkInterpolationMode gridDownsamplingMode;
	tkOgrEncoding ogrEncoding;
	int ogrLayerMaxFeatureCount;
	bool autoChooseOgrLoadingMode;
	bool useSchemesForStyles;
	bool saveOgrLabels;
	int getOgrMaxLabelCount() { return ogrLayerMaxFeatureCount; }
	int ctorCount;
	int dtorCount;
	bool attachMapCallbackToLayers;
	int hotTrackingMaxShapeCount;
	OLE_COLOR identifierColor;
	BSTR emptyBstr;
	BSTR gdalBstr;
	bool allowLayersWithoutProjection;
	bool allowProjectionMismatch;
	bool reprojectLayersOnAdding;
	double mouseTolerance;
	bool commonCollisionListForCharts;
	bool commonCollisionListForLabels;
	bool useShortUrlForTiles;
	CString floatNumberFormat;
	tkPixelOffsetMode pixelOffsetMode;
	bool suppressGdalErrors;
	bool forceReadOnlyModeForGdalRasters;
	CString bingApiKey;
	CString hereAppId;
	CString hereAppCode;
	bool gridFavorGreyScale;
	bool gridUseHistogram;
	bool overrideLocalCallback;
	bool cacheDbfRecords;
	bool cacheShapeRenderingData;
	bool wmsDiskCaching;
	tkCallbackVerbosity callbackVerbosity;
	bool ogrShareConnection;
	bool ogrListAllGeometryTypes;
	bool ogrInterpretYNStringAsBoolean;
	bool allowLayersWithIncompleteReprojection;
	CString httpUserAgent;

	~GlobalSettingsInfo() {
		SysFreeString(emptyBstr);
		SysFreeString(gdalBstr);
		if (callback)
			callback->Release();
	}

	GlobalSettingsInfo::GlobalSettingsInfo() {
		allowLayersWithIncompleteReprojection = false;
		ogrInterpretYNStringAsBoolean = true;
		ogrListAllGeometryTypes = true;
		ogrShareConnection = false;
		callbackVerbosity = cvAll;
		wmsDiskCaching = true;
		cacheShapeRenderingData = false;
		cacheDbfRecords = true;
		overrideLocalCallback = true;
		proxyAuthentication = asBasic;
		httpUserAgent = "MapWinGIS/5.0"; // TODO Use VERSION Macros
		hereAppId = "";
		hereAppCode = "";
		bingApiKey = "";
		forceReadOnlyModeForGdalRasters = false;
		forceHideLabels = false;
		ogrLayerForceUpdateMode = false;
		autoChooseRenderingHintForLabels = true;
		suppressGdalErrors = false;
		pixelOffsetMode = pomDefault;
		floatNumberFormat = "%g";
		maxReprojectionShapeCount = 50000;
		useShortUrlForTiles = false;
		callback = nullptr;
		mouseTolerance = 20;
		allowLayersWithoutProjection = true;
		allowProjectionMismatch = true;
		reprojectLayersOnAdding = false;
		emptyBstr = SysAllocString(L"");
		gdalBstr = SysAllocString(L"GDAL");
		identifierColor = RGB(30, 144, 255);
		hotTrackingMaxShapeCount = 200;
		attachMapCallbackToLayers = true;
		ctorCount = 0;
		dtorCount = 0;
		saveOgrLabels = false;
		useSchemesForStyles = false;
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
		tiffCompression = tkTiffCompression::tkmLZW;
		labelsCollisionMode = tkCollisionMode::LocalList;
		minPolygonArea = 1.0;
		minAreaToPerimeterRatio = 0.0001;
		clipperGcsMultiplicationFactor = 100000.0;
		shapefileFastMode = false;
		invalidShapesBufferDistance = 0.001;
		shapefileFastUnion = true;
		labelsCompositingQuality = HighQuality;
		labelsSmoothingMode = HighQualityMode;
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
		commonCollisionListForCharts = true;
		commonCollisionListForLabels = true;
		gridFavorGreyScale = true;
		gridUseHistogram = true;

		shortUnitStrings[lsHectars] = L"公顷";
		shortUnitStrings[lsMeters] = L"米";
		shortUnitStrings[lsKilometers] = L"公里";
		shortUnitStrings[lsSquareKilometers] = L"平方公里";
		shortUnitStrings[lsSquareMeters] = L"平方米";
		shortUnitStrings[lsMapUnits] = L"亩";
		shortUnitStrings[lsSquareMapUnits] = L"平方亩";
		shortUnitStrings[lsMiles] = L"英里";
		shortUnitStrings[lsFeet] = L"英尺";
		shortUnitStrings[lsLatitude] = L"纬度";
		shortUnitStrings[lsLongitude] = L"经度";
		shortUnitStrings[lsRadians] = L"弧度";
		shortUnitStrings[lsNorthEast] = L"北东";
		shortUnitStrings[lsSouthEast] = L"南东";
		shortUnitStrings[lsSouthWest] = L"南西";
		shortUnitStrings[lsNorthWest] = L"北西";
		shortUnitStrings[lsNorth] = L"北";
		shortUnitStrings[lsEast] = L"东";
		shortUnitStrings[lsSouth] = L"南";
		shortUnitStrings[lsWest] = L"西";
		shortUnitStrings[lsSquareFeet] = L"平方英尺";
		shortUnitStrings[lsAcres] = L"英亩";
		shortUnitStrings[lsSquareMiles] = L"平方英里";
	}

	BSTR CreateEmptyBSTR() {
		USES_CONVERSION;
		return A2BSTR("");
	}

	// *******************************************************
	//		GetTiffCompression()
	// *******************************************************
	CString GetTiffCompression() {
		return GdalHelper::TiffCompressionToString(tiffCompression);
	}

	PredefinedColorScheme GetGridColorScheme() {
		PredefinedColorScheme coloring = defaultColorSchemeForGrids;
		if (randomColorSchemeForGrids) {
			srand((unsigned int)time(nullptr));
			int r = rand();
			coloring = (PredefinedColorScheme)(r % 7);
		}
		return coloring;
	}

	CStringW GetLocalizedString(tkLocalizedStrings s) {
		return shortUnitStrings.find(s) != shortUnitStrings.end() ? shortUnitStrings[s] : L"";
	}

	double GetMinPolygonArea(IGeoProjection* proj) {
		VARIANT_BOOL isGeographic;
		if (proj == nullptr)
			return minPolygonArea;

		proj->get_IsGeographic(&isGeographic);
		return this->GetMinPolygonArea(isGeographic);
	}

	double GetMinPolygonArea(const VARIANT_BOOL isGeographic) {
		if (isGeographic) {
			return minPolygonArea / pow(METERS_PER_DEGREE, 2.0); // degrees to meters
		}

		return minPolygonArea;
	}

	void SetGdalUtf8(bool turnon) {
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", turnon ? "YES" : "NO");
	}

	int GetTilesThreadPoolSize() {
		int size = tilesThreadPoolSize > 20 ? 20 : tilesThreadPoolSize;
		return size;
	}

	double GetInvalidShapeBufferDistance(tkUnitsOfMeasure units) {
		double val = invalidShapesBufferDistance;
		if (Utility::ConvertDistance(units, umMeters, val)) {
			return invalidShapesBufferDistance / val;
		} else {
			return invalidShapesBufferDistance;
		}
	}
};
