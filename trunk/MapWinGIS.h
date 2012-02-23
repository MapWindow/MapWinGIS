#if !defined(AFX_MAPWINGIS_H__C5A46386_D829_489E_B24E_B630F522C689__INCLUDED_)
#define AFX_MAPWINGIS_H__C5A46386_D829_489E_B24E_B630F522C689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MapWinGIS.h : main header file for MAPWINGIS.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#include "errorcodes.h"
#include "MapWinGIS_i.h"
#include "cpl_minixml.h"
#include "cpl_string.h"


// Sets new instance of COM object to the given pointer
// Placed here because this file included in every COM interface
bool put_ComReference(IDispatch* newVal, IDispatch** oldVal, bool allowNull = true);

CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent,
                                                 const char *pszName,
                                                 const char *pszValue);

// **********************************************************
// CMapWinGISApp : See MapWinGIS.cpp for implementation.
// **********************************************************
class CMapWinGISApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
private:
	BOOL InitATL();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

#define VERSION_MAJOR 4
#define VERSION_MINOR 8

// New labels will be used inspite of the mode
#define FORCE_NEW_LABELS true

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

struct GlobalSettingsInfo
{
	double minPolygonArea;
	double minAreaToPerimeterRatio;
	double clipperGcsMultiplicationFactor;
	bool shapefileFastMode;
	double invalidShapesBufferDistance;
	bool shapefileFastUnion;
	CString gdalErrorMessage;
	tkCompositingQuality labelsCompositingQuality;
	tkSmoothingMode labelsSmoothingMode;

	GlobalSettingsInfo::GlobalSettingsInfo()
	{
		minPolygonArea = 1.0;
		minAreaToPerimeterRatio = 0.0001;
		clipperGcsMultiplicationFactor = 10000.0;
		shapefileFastMode = false;
		invalidShapesBufferDistance = 0.001;
		shapefileFastUnion = true;
		labelsCompositingQuality = HighQuality;
		labelsSmoothingMode = HighQualityMode ;
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
};

extern GlobalSettingsInfo m_globalSettings;
extern GlobalClassFactory m_factory;
extern bool m_gdalInitialized;
extern IUtils* m_utils;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPWINGIS_H__C5A46386_D829_489E_B24E_B630F522C689__INCLUDED)
