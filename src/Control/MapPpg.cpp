// MapPpg.cpp : Implementation of the CMapPropPage property page class.

#include "stdafx.h"
#include "MapPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMapPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CMapPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CMapPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMapPropPage, "MAPWINGIS.MapPropPage.1",
	0xdd2a522, 0xf200, 0x4678, 0x84, 0x8e, 0x6b, 0xc5, 0xb3, 0x59, 0xe, 0x1d)


/////////////////////////////////////////////////////////////////////////////
// CMapPropPage::CMapPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CMapPropPage

BOOL CMapPropPage::CMapPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MAP_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CMapPropPage::CMapPropPage - Constructor

CMapPropPage::CMapPropPage() :
	COlePropertyPage(IDD, IDS_MAP_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CMapPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CMapPropPage::DoDataExchange - Moves data between page and properties

void CMapPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CMapPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CMapPropPage message handlers
