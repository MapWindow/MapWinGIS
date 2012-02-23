// MapWinGIS.cpp : Implementation of CMapWinGISApp and DLL registration.

#include "stdafx.h"

#include <initguid.h>
#include <fstream>

#include "MapWinGis.h"
#include "MapWinGIS_i.c"
#include "ShapefileColorScheme.h"
#include "ShapefileColorBreak.h"
#include "cpl_conv.h" 
#include "cpl_string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// cdm 1/27/2006
#ifdef _DEBUG
#include "utilities/debugging/MemLeakDetect.h"
#pragma warning(disable:4100)
CMemLeakDetect gMemLeakDetect;
#endif


CMapWinGISApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xc368d713, 0xcc5f, 0x40ed, { 0x9f, 0x53, 0xf8, 0x4f, 0xe1, 0x97, 0xb9, 0x6a } };
const WORD _wVerMajor = 4;
const WORD _wVerMinor = 8;

CComModule _Module;

////////////////////////////////////////////////////////////////////////////
// CMapWinGISApp::InitInstance - DLL initialization

BOOL CMapWinGISApp::InitInstance()
{
	//Neio modified 2009, following http://www.mapwindow.org/phorum/read.php?7,12162 by gischai, for multi-language support
	//std::locale::global(std::locale(""));
	//19-Oct-09 Rob Cairns: (See Bug 1446) - I hate doing this if it prevents our Chinese friends opening Chinese character shapefiles and data.
	//However, there are just too many bugs associated with this change. See Bug 1446 for more information. Changing back to classic.
	std::locale::global(std::locale("C"));
	
	// UTF8 string are expected by default; the enviroment variable shoud be set to restore older behavior
	// see more details here: http://trac.osgeo.org/gdal/wiki/ConfigOptions
	if( CSLTestBoolean(CPLGetConfigOption( "GDAL_FILENAME_IS_UTF8", "YES" ) ) )
	{
		CPLSetConfigOption( "GDAL_FILENAME_IS_UTF8", "NO" );
	}
	
	return COleControlModule::InitInstance() && InitATL();
	// NB: GdiplusStartup was here. Please see Map.cpp for more explanation.

	m_utils = NULL;
}


////////////////////////////////////////////////////////////////////////////
// CMapWinGISApp::ExitInstance - DLL termination

int CMapWinGISApp::ExitInstance()
{
	if (m_utils)
	{
		m_utils->Release();
		m_utils = NULL;
	}

	// NB: GdiplusShutdown was here. Please see Map.cpp for more explanation.
	_Module.Term();
	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return _Module.RegisterServer(TRUE);

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	_Module.UnregisterServer(TRUE); //TRUE indicates that typelib is unreg'd

	return NOERROR;
}

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_ShapefileColorScheme, CShapefileColorScheme)
OBJECT_ENTRY(CLSID_ShapefileColorBreak, CShapefileColorBreak)
END_OBJECT_MAP()

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(AfxDllGetClassObject(rclsid, riid, ppv) == S_OK)
		return S_OK;
	return _Module.GetClassObject(rclsid, riid, ppv);
}

BOOL CMapWinGISApp::InitATL()
{
	_Module.Init(ObjectMap, AfxGetInstanceHandle());
	return TRUE;

}

// ********************************************************
//	  put_ComReference
// ********************************************************
// Sets new instance of COM object to the given pointer
// Takes care to release the old reference
// Placed here because this included in every CO interface definition
bool put_ComReference(IDispatch* newVal, IDispatch** oldVal, bool allowNull)
{
	if (*oldVal == NULL)
	{
		if (newVal)
		{
			(*oldVal) = newVal;
			newVal->AddRef();
		}
	}
	else if ((*oldVal) != newVal)
	{
		if ( newVal == NULL && !allowNull)
		{
			// NULL reference isn't allowed
			return false;	
		}
		else
		{
			if (*oldVal)
			{
				(*oldVal)->Release();
				(*oldVal) = NULL;
			}
			(*oldVal) = newVal;
		
			if (newVal)
			{
				newVal->AddRef();
			}
		}
	}
	else
	{
		// do nothing, the reference is on place
	}
	return true;
}

// ********************************************************
//     CPLCreateXMLAttributeAndValue()
// ********************************************************
CPLXMLNode* CPLCreateXMLAttributeAndValue(CPLXMLNode *psParent,
                                                 const char *pszName,
                                                 const char *pszValue)
{
	CPLXMLNode* psNode = CPLCreateXMLNode(psParent, CXT_Attribute, pszName);
	CPLCreateXMLNode( psNode, CXT_Text, pszValue);	//CPLString().Printf(pszValue)
	return psNode;
}

GlobalSettingsInfo m_globalSettings;
GlobalClassFactory m_factory;
bool m_gdalInitialized;
IUtils* m_utils;