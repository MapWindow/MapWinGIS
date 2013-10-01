// MapTracker.cpp: implementation of the CMapTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapWinGis.h"
#include "MapTracker.h"
#include "Map.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapTracker::CMapTracker()
{	parent = NULL;
}

CMapTracker::~CMapTracker()
{
}

CMapTracker::CMapTracker( COleControl * Parent, LPCRECT lpSrcRect, UINT nStyle ):CRectTracker( lpSrcRect, nStyle )
{	parent = Parent;
}

void CMapTracker::OnChangedRect( const CRect& rectOld )
{	CRect rect;
	rect.CopyRect(rectOld);
	rect.NormalizeRect();

	long vbflags = 0;
	if( GetKeyState(VK_LSHIFT) & 0x8000 )
		vbflags |= 1;
	if( GetKeyState(VK_RSHIFT) & 0x8000 )
		vbflags |= 1;
	if( GetKeyState(VK_LCONTROL) & 0x8000 )
		vbflags |= 2;
	if( GetKeyState(VK_RCONTROL) & 0x8000 )
		vbflags |= 2;

	if( ((CMapView*)parent)->SendMouseMove() == TRUE )
	{	int x = 0, height = 0; 
		parent->GetControlSize( &x, &height );
		((CMapView*)parent)->FireMouseMove( MK_LBUTTON, (short)vbflags, rect.BottomRight().x, rect.BottomRight().y );
	}

	if( ((CMapView*)parent)->SendSelectBoxDrag() == TRUE )
		((CMapView*)parent)->FireSelectBoxDrag( rect.TopLeft().x, rect.BottomRight().x, rect.BottomRight().y, rect.TopLeft().y );

	CRectTracker::OnChangedRect( rectOld );
}
BOOL CMapTracker::TrackRubberBand(CWnd* pWnd, CPoint point, BOOL bAllowInvert)
{
	((CMapView*)parent)->m_rectTrackerIsActive = true;
	BOOL ret = CRectTracker::TrackRubberBand(pWnd, point, bAllowInvert);
	((CMapView*)parent)->m_rectTrackerIsActive = false;
	return ret;
}

