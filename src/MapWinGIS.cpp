// MapWinGIS.cpp : Implementation of CMapWinGISApp and DLL registration.
#include "stdafx.h"
#include <initguid.h>
#include "MapWinGIS_i.c"
#include "ShapefileColorScheme.h"
#include "ShapefileColorBreak.h"
#include "cpl_conv.h" 
#include "cpl_string.h"
#include "map.h"
#include "Functions.h"
#include "PointClass.h"

#ifdef _DEBUG
#include "gdal.h"
#define new DEBUG_NEW
#endif
#include "MercatorProjection.h"
#include "PrefetchManager.h"
#include "TileCacheManager.h"

class CMapWinGISModule :
	public ATL::CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_MapWinGIS);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MAPWINGIS, "{8308CC9E-4AEF-4D31-9081-86CD61B9E641}");
};

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

const GUID CDECL BASED_CODE _tlid = { 0xc368d713, 0xcc5f, 0x40ed, { 0x9f, 0x53, 0xf8, 0x4f, 0xe1, 0x97, 0xb9, 0x6a } };
const WORD _wVerMajor = 5;
const WORD _wVerMinor = 1;

CMapWinGISApp NEAR theApp;
CMapWinGISModule _AtlModule;    // this one is from ATL7 (used by all ATL co-classes)
CComModule _Module;				// this one is from ATL3 (used for ShapefileColorScheme and ShapefileColorBreak)
GlobalClassFactory m_factory;	// make sure that this one is initialized after the _Module above

// ******************************************************
// CMapWinGISApp::InitInstance - DLL initialization
// ******************************************************
BOOL CMapWinGISApp::InitInstance()
{
	// let's generate floating point exceptions
	#ifndef RELEASE_MODE
		//_clearfp();
		//_controlfp(0, EM_ZERODIVIDE);
	#endif

	Debug::Init();

	//Neio modified 2009, following http_://www.mapwindow.org/phorum/read.php?7,12162 by gischai, for multi-language support
	//std::locale::global(std::locale(""));
	//19-Oct-09 Rob Cairns: (See Bug 1446) - I hate doing this if it prevents our Chinese friends opening Chinese character shapefiles and data.
	//However, there are just too many bugs associated with this change. See Bug 1446 for more information. Changing back to classic.
	std::locale::global(std::locale("C"));

	// http_://stackoverflow.com/questions/7659127/createex-causes-unhandled-exception-the-activation-context-being-deactivated-is
	//AfxSetAmbientActCtx(FALSE);

	// UTF8 string are expected by default; the enviroment variable shoud be set to restore older behavior
	// see more details here: http_://trac.osgeo.org/gdal/wiki/ConfigOptions
	m_globalSettings.SetGdalUtf8(false);
	
	GdalHelper::SetDefaultConfigPaths();

	parser::InitializeFunctions();

	// initialize all static variables, to keep our memory leaking report clean from them
#ifdef _DEBUG
	gMemLeakDetect.stopped = true;
	GDALAllRegister();
	gMemLeakDetect.stopped = false;
#endif

	//CMapView::GdiplusStartup(); // moved back to CMapView constructor

	return COleControlModule::InitInstance() && InitATL();
}

// *****************************************************
// CMapWinGISApp::ExitInstance - DLL termination
// *****************************************************
int CMapWinGISApp::ExitInstance()
{
	CPLErrorReset();

	MercatorProjection::ReleaseGeoProjection();

	#ifndef RELEASE_MODE

	CComBSTR bstr;
    // make sure m_utils is still extant
	if (m_utils)
	{
		m_utils->get_ComUsageReport(VARIANT_TRUE, &bstr);
	}

	USES_CONVERSION;
	CString s = OLE2A(bstr);
	Debug::WriteLine(s);

	#endif

	if (m_utils)
	{
		m_utils->Release();
	}

	TileCacheManager::CloseAll();

	PrefetchManagerFactory::Clear();

	parser::ReleaseFunctions();

	//CMapView::GdiplusShutdown(); // moved back to CMapView destructor

	_Module.Term();
	return COleControlModule::ExitInstance();
}

// **************************************************************
// DllRegisterServer - Adds entries to the system registry
// **************************************************************
STDAPI DllRegisterServer(void)
{
	_AtlModule.UpdateRegistryAppId(TRUE);
	HRESULT hRes2 = _AtlModule.RegisterServer(TRUE);
	if (hRes2 != S_OK)
		return hRes2;
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return _Module.RegisterServer(TRUE);
	

	return NOERROR;
}

// **************************************************************
//   GetModuleInstance
// **************************************************************
HINSTANCE GetModuleInstance()
{
	HINSTANCE instance = _Module.GetModuleInstance();
	return instance;
}

// **************************************************************
// DllUnregisterServer - Removes entries from the system registry
// **************************************************************
STDAPI DllUnregisterServer(void)
{
	_AtlModule.UpdateRegistryAppId(FALSE);
	HRESULT hRes2 = _AtlModule.UnregisterServer(TRUE);
	if (hRes2 != S_OK)
		return hRes2;
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	_Module.UnregisterServer(TRUE); //TRUE indicates that typelib is unreg'd

	return NOERROR;
}

#if !defined(_WIN32_WCE) && !defined(_AMD64_) && !defined(_IA64_)
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")
#else
#if defined(_X86_) || defined(_SHX_)
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer,PRIVATE")
#else
#pragma comment(linker, "/EXPORT:DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer,PRIVATE")
#endif // (_X86_)||(_SHX_)
#endif // !_WIN32_WCE && !_AMD64_ && !_IA64_ 

// **************************************************************
//		DllCanUnloadNow()
// **************************************************************
STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (_AtlModule.GetLockCount() > 0)
		return S_FALSE;
	
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

// ******************************************************************
// Returns a class factory to create an object of the requested type
// ******************************************************************
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _DEBUG
	bool state = gMemLeakDetect.stopped;
	gMemLeakDetect.stopped = true;
#endif

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	HRESULT hres;
	if (S_OK == _AtlModule.GetClassObject(rclsid, riid, ppv))
		hres = S_OK;
	else if(AfxDllGetClassObject(rclsid, riid, ppv) == S_OK)
		hres = S_OK;
	else
		hres = _Module.GetClassObject(rclsid, riid, ppv);
	
#ifdef _DEBUG	
	gMemLeakDetect.stopped = state;
#endif

	return hres;
}

// *****************************************************************
//		Object map
// *****************************************************************
// Provides support for the registration, initialization, and creation of instances of ATL COM classes
// According to MSDN this macro is obsolete. OBJECT_ENTRY_AUTO should be used instead
BEGIN_OBJECT_MAP(ObjectMap)
#ifdef OLD_API
	OBJECT_ENTRY(CLSID_ShapefileColorScheme, CShapefileColorScheme)
	OBJECT_ENTRY(CLSID_ShapefileColorBreak, CShapefileColorBreak)
#endif
END_OBJECT_MAP()

// *****************************************************************
//		InitATL
// *****************************************************************
BOOL CMapWinGISApp::InitATL()
{
	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	return TRUE;
}