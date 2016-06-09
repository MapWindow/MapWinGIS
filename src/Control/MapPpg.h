#if !defined(AFX_MAPPPG_H__3F334A9F_5EDE_4880_A246_508773DC7293__INCLUDED_)
#define AFX_MAPPPG_H__3F334A9F_5EDE_4880_A246_508773DC7293__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MapPpg.h : Declaration of the CMapPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CMapPropPage : See MapPpg.cpp.cpp for implementation.

class CMapPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMapPropPage)
	DECLARE_OLECREATE_EX(CMapPropPage)

// Constructor
public:
	CMapPropPage();

// Dialog Data
	//{{AFX_DATA(CMapPropPage)
	enum { IDD = IDD_PROPPAGE_MAP };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CMapPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPPPG_H__3F334A9F_5EDE_4880_A246_508773DC7293__INCLUDED)
