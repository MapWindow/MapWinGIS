// MapTracker.h: interface for the CMapTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPTRACKER_H__1C842303_191F_11D5_A566_00104BCC583E__INCLUDED_)
#define AFX_MAPTRACKER_H__1C842303_191F_11D5_A566_00104BCC583E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMapTracker : public CRectTracker  
{
public:
	CMapTracker();
	CMapTracker( COleControl * Parent, LPCRECT lpSrcRect, UINT nStyle );
	virtual ~CMapTracker();	
	void OnChangedRect( const CRect& rectOld );
	BOOL TrackRubberBand(CWnd* pWnd, CPoint point, BOOL bAllowInvert = TRUE);
	
private:
	COleControl * parent;
};

#endif // !defined(AFX_MAPTRACKER_H__1C842303_191F_11D5_A566_00104BCC583E__INCLUDED_)
