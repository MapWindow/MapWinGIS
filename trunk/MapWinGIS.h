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
#include "GlobalSettingsInfo.h"
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



extern GlobalSettingsInfo m_globalSettings;
extern GlobalClassFactory m_factory;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPWINGIS_H__C5A46386_D829_489E_B24E_B630F522C689__INCLUDED)
