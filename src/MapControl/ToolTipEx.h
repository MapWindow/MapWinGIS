// ToolTipEx.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Shane Martin
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    shane.kim@kaiserslautern.netsurf.de
/////////////////////////////////////////////////////////////////////////////

#ifndef __ToolTipEx__
class CToolTipEx : public CToolTipCtrl
{
// Construction
public:
	CToolTipEx();

// Attributes
public:

// Operations
public:
	void TrackPosition(int xPos, int yPos);
	void TrackPosition(LPPOINT lppt);
	void TrackActivate(BOOL bActivate, LPTOOLINFO lpti);
	int SetMaxTipWidth(int nWidth);
	RECT SetMargin(LPRECT lpRect);
	int GetMaxTipWidth();
	void GetMargin(LPRECT lpRect);
	int GetDelayTime(DWORD dwType);
	int SetDelayTime(DWORD dwType, int nTime);
	COLORREF GetTextColor();
	COLORREF GetBkColor();
	COLORREF SetTextColor(COLORREF crColor);
	COLORREF SetBkColor(COLORREF crColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolTipEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
#define __ToolTipEx__
#endif
/////////////////////////////////////////////////////////////////////////////
