// ToolTipEx.cpp : implementation file
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

// last revised: 19 Apr 98
// ToolTipEx.cpp : implementation file
//
// Description:
//	CToolTipEx provides additional features to the standard tooltip
//  control "CToolTipCtrl".
//  To use you must have the latest (as of Apr 98) commctrl.h and
//  commctrl.dll (the one that came with IE4 is fine).
// Usage:
//  Add a variable to your mainfrm.h for CToolTipEx.
//  In your mainfrm.cpp's OnCreate add a line setting the toolbar's
//  tooltip control to the variable you declared, such as:
//		m_wndToolBar.GetToolBarCtrl().SetToolTips(&MyToolTipCtrl);
//  To get a pointer to your new tooltip you will need to use:
//		_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
//		CToolTipEx* pToolTip = (CToolTipEx*)pThreadState->m_pToolTip;
//	If you have any further question you can check the help file that
//	was included, or send me an email.

#include "stdafx.h"
#include "ToolTipEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolTipEx

CToolTipEx::CToolTipEx()
{
}

CToolTipEx::~CToolTipEx()
{
}


BEGIN_MESSAGE_MAP(CToolTipEx, CToolTipCtrl)
	//{{AFX_MSG_MAP(CToolTipEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTipEx message handlers

COLORREF CToolTipEx::SetBkColor(COLORREF crColor)
{
	COLORREF oldColor;

	oldColor = SendMessage(TTM_GETTIPBKCOLOR, 0, 0);
	SendMessage(TTM_SETTIPBKCOLOR, (WPARAM)(COLORREF)crColor, 0);

	return oldColor;
}

COLORREF CToolTipEx::SetTextColor(COLORREF crColor)
{
	COLORREF oldColor;

	oldColor = SendMessage(TTM_GETTIPTEXTCOLOR, 0, 0);
	SendMessage(TTM_SETTIPTEXTCOLOR, (WPARAM)(COLORREF)crColor, 0);

	return oldColor;
}

COLORREF CToolTipEx::GetBkColor()
{
	return SendMessage(TTM_GETTIPBKCOLOR, 0, 0);
}

COLORREF CToolTipEx::GetTextColor()
{
	return SendMessage(TTM_GETTIPTEXTCOLOR, 0, 0);
}

int CToolTipEx::SetDelayTime(DWORD dwType, int nTime)
{
	// dwType can be one of the following:
	// TTDT_AUTOPOP - The length of time the tooltip window
	// remains visible if the pointer is stationary within a tool's
	// bounding rectangle.  
	//
	// TTDT_INITIAL - The length of time the pointer must
	// remain stationary within a tool's bounding rectangle before
	// the tooltip window appears.  
	//
	// TTDT_RESHOW  The length of time it takes for
	// subsequent tooltip windows to appear as the pointer moves
	// from one tool to another.  
	int nDuration;

	switch(dwType)
	{
	case TTDT_AUTOPOP:
	case TTDT_INITIAL:
	case TTDT_RESHOW:
		nDuration = SendMessage(TTM_GETDELAYTIME, (DWORD)dwType, 0);
		SendMessage(TTM_SETDELAYTIME, (WPARAM)(DWORD)dwType, (LPARAM)(INT)MAKELONG(nTime, 0));
		return nDuration;
	}
	return -1;
}

int CToolTipEx::GetDelayTime(DWORD dwType)
{
	// dwType can be one of the following:
	// TTDT_AUTOPOP - Retrieve the length of time the tooltip window
	// remains visible if the pointer is stationary within a tool's
	// bounding rectangle.  
	//
	// TTDT_INITIAL - Retrieve the length of time the pointer must
	// remain stationary within a tool's bounding rectangle before
	// the tooltip window appears.  
	//
	// TTDT_RESHOW  Retrieve the length of time it takes for
	// subsequent tooltip windows to appear as the pointer moves
	// from one tool to another.  
	switch(dwType)
	{
	case TTDT_AUTOPOP:
	case TTDT_INITIAL:
	case TTDT_RESHOW:
		return SendMessage(TTM_GETDELAYTIME, (DWORD)dwType, 0);
	}
	return -1;
}

void CToolTipEx::GetMargin(LPRECT lpRect)
{
	// lpRect.top  Distance between top border and top of tooltip
	//   text, in pixels.  
	// lpRect.left  Distance between left border and left end of
	//   tooltip text, in pixels.  
	// lpRect.bottom  Distance between bottom border and bottom of
	//   tooltip text, in pixels.  
	// lpRect.right  Distance between right border and right end of
	//   tooltip text, in pixels
	SendMessage(TTM_GETMARGIN, 0, (LPARAM)(LPRECT)lpRect);
}

int CToolTipEx::GetMaxTipWidth()
{
	// The maximum tooltip width value does not indicate a tooltip
	// window's actual width. Rather, if a tooltip string exceeds
	// the maximum width, the control breaks the text into multiple
	// lines, using spaces to determine line breaks. If the text
	// cannot be segmented into multiple lines, it will be dis-
	// played on a single line. The length of this line may exceed
	// the maximum tooltip width.
	// Defaults to -1 when tooltip control is first created.
	return SendMessage(TTM_GETMAXTIPWIDTH, 0, 0);
}

RECT CToolTipEx::SetMargin(LPRECT lpRect)
{
	// lpRect.top  Distance between top border and top of tooltip
	//   text, in pixels.  
	// lpRect.left  Distance between left border and left end of
	//   tooltip text, in pixels.  
	// lpRect.bottom  Distance between bottom border and bottom of
	//   tooltip text, in pixels.  
	// lpRect.right  Distance between right border and right end of
	//   tooltip text, in pixels.  
	RECT TempRect;
	GetMargin(&TempRect);
	SendMessage(TTM_SETMARGIN, 0, (LPARAM)(LPRECT)lpRect);

	return TempRect;
}

int CToolTipEx::SetMaxTipWidth(int nWidth)
{
	// The maximum tooltip width value does not indicate a tooltip
	// window's actual width. Rather, if a tooltip string exceeds
	// the maximum width, the control breaks the text into multiple
	// lines, using spaces to determine line breaks. If the text
	// cannot be segmented into multiple lines, it will be dis-
	// played on a single line. The length of this line may exceed
	// the maximum tooltip width.
	return SendMessage(TTM_SETMAXTIPWIDTH, 0, (LPARAM)(INT)nWidth);
}

void CToolTipEx::TrackActivate(BOOL bActivate, LPTOOLINFO lpti)
{
	SendMessage(TTM_TRACKACTIVATE, (WPARAM)(BOOL)bActivate,
		(LPARAM)(LPTOOLINFO)lpti);
}

void CToolTipEx::TrackPosition(LPPOINT lppt)
{
	// The tooltip control chooses where to display the tooltip
	// window based on the coordinates you provide with this
	// message. This causes the tooltip window to appear beside the
	// tool to which it corresponds. To have tooltip windows
	// displayed at specific coordinates, include the TTF_ABSOLUTE
	// flag in the uFlags member of the TOOLINFO structure when
	// adding the tool.
	SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(lppt->x, lppt->y));
}

void CToolTipEx::TrackPosition(int xPos, int yPos)
{
	// The tooltip control chooses where to display the tooltip
	// window based on the coordinates you provide with this
	// message. This causes the tooltip window to appear beside the
	// tool to which it corresponds. To have tooltip windows
	// displayed at specific coordinates, include the TTF_ABSOLUTE
	// flag in the uFlags member of the TOOLINFO structure when
	// adding the tool.
	SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(xPos, yPos));
}
