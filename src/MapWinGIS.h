// MapWinGIS.h : main header file for MAPWINGIS.DLL
#pragma once

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"
#include "errorcodes.h"
#include "MapWinGIS_i.h"
#include "GlobalClassFactory.h"

#define VERSION_MAJOR 5
#define VERSION_MINOR 2

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;
HINSTANCE GetModuleInstance();

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

extern GlobalClassFactory m_factory;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.