// MapWinGIS.h : main header file for MAPWINGIS.DLL
#if !defined(AFX_MAPWINGIS_H__C5A46386_D829_489E_B24E_B630F522C689__INCLUDED_)
#define AFX_MAPWINGIS_H__C5A46386_D829_489E_B24E_B630F522C689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#pragma warning(disable:4482)	// non-standard extention used - for names of enumerations
#pragma warning(disable:4099)	// missing program database

#include "resource.h"
#include "errorcodes.h"
#include "MapWinGIS_i.h"
#include <math.h>
#include "gdal_priv.h"
#include "geos_c.h"
#include "ogr_geometry.h"
#include <map>
#include "Utilities\Debugging\ReferenceCounter.h"

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

#define VERSION_MAJOR 4
#define VERSION_MINOR 9

// new labels will be used inspite of the mode
#define FORCE_NEW_LABELS true

extern GEOSContextHandle_t _geosContextHandle;
GEOSContextHandle_t getGeosHandle();

extern IUtils* m_utils;
IUtils* GetUtils();
HINSTANCE GetModuleInstance();

// comment these lines to turn off the memory leaking detection tools
#ifdef _DEBUG
#include "../debugging/MemLeakDetect.h"
#pragma warning(disable:4100)
extern CMemLeakDetect gMemLeakDetect;
#endif

extern ReferenceCounter gReferenceCounter;

// **********************************************************
//		CMapWinGISApp class
// **********************************************************
class CMapWinGISApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
private:
	BOOL InitATL();
};

class GlobalClassFactory
{
public:	
	IClassFactory* pointFactory;

	GlobalClassFactory::GlobalClassFactory()
	{
		pointFactory = NULL;
		CoGetClassObject(CLSID_Point, CLSCTX_INPROC_SERVER, NULL, IID_IClassFactory, (void**)&pointFactory); 
	}

	~GlobalClassFactory()
	{
		if (pointFactory)
			pointFactory->Release(); 
	}
};

// **********************************************************
//	GlobalSettingsInfo
// **********************************************************
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
	double maxNoProxyGridSizeMb;
	int maxUniqueValuesCount;
	bool grabMapProjectionFromFirstLayer;
	bool randomColorSchemeForGrids;
	PredefinedColorScheme defaultColorSchemeForGrids;
	tkShapeValidationMode inputValidation;
	tkShapeValidationMode outputValidation;
	tkGeometryEngine geometryEngine;

	GlobalSettingsInfo::GlobalSettingsInfo()
	{
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
		maxNoProxyGridSizeMb = 20.0;
		gridProxyMode = gpmAuto;
		maxUniqueValuesCount = 100;
		grabMapProjectionFromFirstLayer = true;
		randomColorSchemeForGrids = true;
		defaultColorSchemeForGrids = SummerMountains;
		inputValidation = tkShapeValidationMode::NoValidation;
		outputValidation = tkShapeValidationMode::NoValidation;
		geometryEngine = tkGeometryEngine::engineGeos;

		shortUnitStrings[tkLocalizedStrings::lsHectars] = L"ha";
		shortUnitStrings[tkLocalizedStrings::lsMeters] = L"m";
		shortUnitStrings[tkLocalizedStrings::lsKilometers] = L"km";
		shortUnitStrings[tkLocalizedStrings::lsSquareKilometers] = L"sq.km";
		shortUnitStrings[tkLocalizedStrings::lsSquareMeters] = L"sq.m";
		shortUnitStrings[tkLocalizedStrings::lsMapUnits] = L"mu";
		shortUnitStrings[tkLocalizedStrings::lsSquareMapUnits] = L"sq.mu";
		shortUnitStrings[tkLocalizedStrings::lsMiles] = L"miles";
		shortUnitStrings[tkLocalizedStrings::lsFeet] = L"feet";
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
};

extern GlobalSettingsInfo m_globalSettings;
extern GlobalClassFactory m_factory;




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPWINGIS_H__C5A46386_D829_489E_B24E_B630F522C689__INCLUDED)
