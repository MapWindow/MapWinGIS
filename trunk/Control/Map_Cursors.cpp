#include "stdafx.h"
#include "Map.h"
#include "ShapeEditor.h"

// *******************************************************
//		OnSetCursor()
// *******************************************************
BOOL CMapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   HCURSOR NewCursor = NULL;

	if( nHitTest != HTCLIENT )
	{
		COleControl::OnSetCursor( pWnd, nHitTest, message );
		return TRUE;
	}

	bool hasGuiCursor = true;
	switch(_lastZooombarPart)
	{
		case ZoombarHandle:
			NewCursor = LoadCursor(NULL, IDC_SIZENS);
			break;
		case ZoombarMinus:
		case ZoombarPlus:
		case ZoombarBar:
			NewCursor = LoadCursor(NULL, IDC_HAND);
			break;
		default:
			hasGuiCursor = false;
	}

	if (!hasGuiCursor)
	{
		switch( m_mapCursor )
		{
			case crsrMapDefault:

				switch( m_cursorMode )
				{
					case cmZoomIn:
						NewCursor = _reverseZooming ?  _cursorZoomout : _cursorZoomin;
						break;

					case cmZoomOut:
						NewCursor = _reverseZooming ?  _cursorZoomin : _cursorZoomout;
						break;

					case cmPan:
						NewCursor = _cursorPan;
						break;

					case cmSelection:
						NewCursor = _cursorSelect;
						break;
					
					case cmMeasure:
						NewCursor = _cursorMeasure;
						break;

					case cmNone:
						NewCursor = (HCURSOR)m_uDCursorHandle;
						break;
				}
				break;
		
			case crsrAppStarting:
				NewCursor = LoadCursor(NULL, IDC_APPSTARTING);
				break;

			case crsrArrow:
				NewCursor = LoadCursor(NULL, IDC_ARROW);
				break;

			case crsrCross:
				NewCursor = LoadCursor(NULL, IDC_CROSS);
				break;

			case crsrHelp:
				NewCursor = LoadCursor(NULL, IDC_HELP);
				break;

			case crsrIBeam:
				NewCursor = LoadCursor(NULL, IDC_IBEAM);
				break;

			case crsrNo:
				NewCursor = LoadCursor(NULL, IDC_NO);
				break;

			case crsrSizeAll:
				NewCursor = LoadCursor(NULL, IDC_SIZEALL);
				break;

			case crsrSizeNESW:
				NewCursor = LoadCursor(NULL, IDC_SIZENESW);
				break;

			case crsrSizeNS:
				NewCursor = LoadCursor(NULL, IDC_SIZENS);
				break;

			case crsrSizeNWSE:
				NewCursor = LoadCursor(NULL, IDC_SIZENWSE);
				break;

			case crsrSizeWE:
				NewCursor = LoadCursor(NULL, IDC_SIZEWE);
				break;

			case crsrUpArrow:
				NewCursor = LoadCursor(NULL, IDC_UPARROW);
				break;

			case crsrHand:
				NewCursor = LoadCursor(NULL, IDC_HAND);
				break;
			
			case crsrWait:

				if (!_disableWaitCursor)
					NewCursor = LoadCursor(NULL, IDC_WAIT);
				break;

			case crsrUserDefined:
				NewCursor = (HCURSOR)m_uDCursorHandle;
				break;
		}
	}

	if (NewCursor != NULL)
		::SetCursor( NewCursor );
	else
		COleControl::OnSetCursor( pWnd, nHitTest, message );

	return TRUE;
}

// ***************************************************************
//		UpdateCursor
// ***************************************************************
void CMapView::UpdateCursor(tkCursorMode cursor)
{
	m_cursorMode = cursor;
	OnSetCursor(this,HTCLIENT,0);
	OnCursorModeChanged();
}

// *******************************************************
//		OnMapCursorChanged()
// *******************************************************
void CMapView::OnMapCursorChanged()
{	
	OnSetCursor(this,0,0);
}

// *********************************************************
//		OnCursorModeChanged 
// *********************************************************
void CMapView::OnCursorModeChanged()
{
	if (_measuring)
	{
		VARIANT_BOOL vb;
		_measuring->get_Persistent(&vb);
		if (m_cursorMode != cmMeasure && !vb)
			_measuring->Clear();
	}

	VARIANT_BOOL empty;
	_shapeEditor->get_IsEmpty(&empty);

	if (!empty) {
		VARIANT_BOOL vb;
		_shapeEditor->get_CreationMode(&vb);
		
		if ((vb && m_cursorMode == cmEditShape) || (!vb && m_cursorMode == cmAddShape))
		{
			// TODO: check if we have changes			
			_shapeEditor->Clear();
			RedrawCore(RedrawSkipDataLayers, false, true);
		}
	}

	if (m_cursorMode == cmAddShape) {
		((CShapeEditor*)_shapeEditor)->put_EditorState(EditorCreation);
	}
}

// *********************************************************
//		SetWaitCursor()
// *********************************************************
HCURSOR CMapView::SetWaitCursor()
{
   if (_disableWaitCursor)
		return NULL;

	HCURSOR oldCursor = ::GetCursor();
   
	CPoint cpos;
	GetCursorPos(&cpos);
	CRect wrect;
	GetWindowRect(&wrect);
	
	HWND wndActive = ::GetActiveWindow();
	if ((wndActive == this->GetSafeHwnd()) || (wndActive == this->GetParentOwner()->GetSafeHwnd()))
	{
		if( wrect.PtInRect(cpos) && (m_mapCursor != crsrUserDefined) && !_disableWaitCursor)
		{
			::SetCursor(LoadCursor(NULL, IDC_WAIT) );
		}
	}

   return oldCursor;
}