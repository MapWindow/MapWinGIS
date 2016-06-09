#include "stdafx.h"
#include "Map.h"
#include "ShapeEditor.h"
#include "EditorHelper.h"
#include "MeasuringHelper.h"

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
	if (_copyrightLinkActive) {
		NewCursor = LoadCursor(NULL, IDC_HAND);
	}
	else
	{
		switch (_lastZooombarPart)
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
	}

	if (!hasGuiCursor)
	{
		NewCursor = GetCursorIcon();
		if (!NewCursor) 
		{
			m_mapCursor = crsrMapDefault;
			NewCursor = GetCursorIcon();
		}
	}

	if (NewCursor != NULL)
		::SetCursor( NewCursor );
	else
		COleControl::OnSetCursor( pWnd, nHitTest, message );

	return TRUE;
}

// *******************************************************
//		GetCursorIcon()
// *******************************************************
HCURSOR CMapView::GetCursorIcon()
{
	HCURSOR newCursor = NULL; 
	switch (m_mapCursor)
	{
		case crsrMapDefault:

			switch (m_cursorMode)
			{
				case cmZoomIn:
					newCursor = _reverseZooming ? _cursorZoomout : _cursorZoomin;
					break;

				case cmZoomOut:
					newCursor = _reverseZooming ? _cursorZoomin : _cursorZoomout;
					break;

				case cmPan:
					newCursor = _cursorPan;
					break;

				case cmSelection:
					newCursor = _cursorSelect;
					break;

				case cmMeasure:
					newCursor = _cursorMeasure;
					break;

				case cmAddShape:
					newCursor = _cursorDigitize;
					break;

				case cmEditShape:
					newCursor = _cursorVertex;
					break;

				case cmIdentify:
					newCursor = _cursorIdentify;
					break;

					// don't look good enough
					/*case cmSelectByPolygon:
					NewCursor = _cursorSelect2;
					break;

					case cmRotateShapes:
					NewCursor = _cursorRotate;
					break;

					case cmMoveShapes:
					NewCursor = _cursorMove;
					break;*/

				case cmNone:
					newCursor = (HCURSOR)m_uDCursorHandle;
					break;
			}
			break;

		case crsrAppStarting:
			newCursor = LoadCursor(NULL, IDC_APPSTARTING);
			break;

		case crsrArrow:
			newCursor = LoadCursor(NULL, IDC_ARROW);
			break;

		case crsrCross:
			newCursor = LoadCursor(NULL, IDC_CROSS);
			break;

		case crsrHelp:
			newCursor = LoadCursor(NULL, IDC_HELP);
			break;

		case crsrIBeam:
			newCursor = LoadCursor(NULL, IDC_IBEAM);
			break;

		case crsrNo:
			newCursor = LoadCursor(NULL, IDC_NO);
			break;

		case crsrSizeAll:
			newCursor = LoadCursor(NULL, IDC_SIZEALL);
			break;

		case crsrSizeNESW:
			newCursor = LoadCursor(NULL, IDC_SIZENESW);
			break;

		case crsrSizeNS:
			newCursor = LoadCursor(NULL, IDC_SIZENS);
			break;

		case crsrSizeNWSE:
			newCursor = LoadCursor(NULL, IDC_SIZENWSE);
			break;

		case crsrSizeWE:
			newCursor = LoadCursor(NULL, IDC_SIZEWE);
			break;

		case crsrUpArrow:
			newCursor = LoadCursor(NULL, IDC_UPARROW);
			break;

		case crsrHand:
			newCursor = LoadCursor(NULL, IDC_HAND);
			break;

		case crsrWait:

			if (!_disableWaitCursor)
				newCursor = LoadCursor(NULL, IDC_WAIT);
			break;

		case crsrUserDefined:
			newCursor = (HCURSOR)m_uDCursorHandle;
			break;
	}
	return newCursor;
}

// *******************************************************
//		OnMapCursorChanged()
// *******************************************************
void CMapView::OnMapCursorChanged()
{	
	OnSetCursor(this,0,0);
}

// *******************************************************
//		GetCursorMode()
// *******************************************************
tkCursorMode CMapView::GetCursorMode()
{
	return (tkCursorMode)m_cursorMode;
}

// *******************************************************
//		SetCursorMode()
// *******************************************************
void CMapView::SetCursorMode(tkCursorMode mode)
{
	UpdateCursor(mode, true);
}

// *******************************************************
//		UpdateCursor()
// *******************************************************
void CMapView::UpdateCursor(tkCursorMode newCursor, bool clearEditor)
{
	if (newCursor == m_cursorMode) return;

	if (newCursor == cmRotateShapes)
	{
		if (!InitRotationTool())
			return;
	}
	
	bool refreshNeeded = newCursor == cmRotateShapes || m_cursorMode == cmRotateShapes;

	if (MeasuringHelper::OnCursorChanged(_measuring, newCursor))
		refreshNeeded = true;
	
	if (!EditorHelper::OnCursorChanged(_shapeEditor, clearEditor, newCursor, refreshNeeded))
		return;

	m_cursorMode = newCursor;

	OnSetCursor(this, HTCLIENT, 0);
	
	if (refreshNeeded)
		RedrawCore(RedrawSkipDataLayers, true);
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




