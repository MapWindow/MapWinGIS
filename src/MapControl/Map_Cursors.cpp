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
   HCURSOR NewCursor = nullptr;

	if( nHitTest != HTCLIENT )
	{
		COleControl::OnSetCursor( pWnd, nHitTest, message );
		return TRUE;
	}

	bool hasGuiCursor = true;
	if (_copyrightLinkActive) {
		NewCursor = LoadCursor(nullptr, IDC_HAND);
	}
	else
	{
		switch (_lastZooombarPart)
		{
			case ZoombarHandle:
				NewCursor = LoadCursor(nullptr, IDC_SIZENS);
				break;
			case ZoombarMinus:
			case ZoombarPlus:
			case ZoombarBar:
				NewCursor = LoadCursor(nullptr, IDC_HAND);
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

	if (NewCursor != nullptr)
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
	HCURSOR newCursor = nullptr;
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
					newCursor = (_useAlternatePanCursor == TRUE) ? _cursorAlternatePan : _cursorPan;
					break;

				case cmSelection:
				case cmSelectByPolygon:
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
					newCursor = reinterpret_cast<HCURSOR>(static_cast<LONG_PTR>(m_uDCursorHandle));
					break;
			}
			break;

		case crsrAppStarting:
			newCursor = LoadCursor(nullptr, IDC_APPSTARTING);
			break;

		case crsrArrow:
			newCursor = LoadCursor(nullptr, IDC_ARROW);
			break;

		case crsrCross:
			newCursor = LoadCursor(nullptr, IDC_CROSS);
			break;

		case crsrHelp:
			newCursor = LoadCursor(nullptr, IDC_HELP);
			break;

		case crsrIBeam:
			newCursor = LoadCursor(nullptr, IDC_IBEAM);
			break;

		case crsrNo:
			newCursor = LoadCursor(nullptr, IDC_NO);
			break;

		case crsrSizeAll:
			newCursor = LoadCursor(nullptr, IDC_SIZEALL);
			break;

		case crsrSizeNESW:
			newCursor = LoadCursor(nullptr, IDC_SIZENESW);
			break;

		case crsrSizeNS:
			newCursor = LoadCursor(nullptr, IDC_SIZENS);
			break;

		case crsrSizeNWSE:
			newCursor = LoadCursor(nullptr, IDC_SIZENWSE);
			break;

		case crsrSizeWE:
			newCursor = LoadCursor(nullptr, IDC_SIZEWE);
			break;

		case crsrUpArrow:
			newCursor = LoadCursor(nullptr, IDC_UPARROW);
			break;

		case crsrHand:
			newCursor = LoadCursor(nullptr, IDC_HAND);
			break;

		case crsrWait:

			if (!_disableWaitCursor)
				newCursor = LoadCursor(nullptr, IDC_WAIT);
			break;

		case crsrUserDefined:
			newCursor = reinterpret_cast<HCURSOR>(static_cast<LONG_PTR>(m_uDCursorHandle));
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
		return nullptr;

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
			::SetCursor(LoadCursor(nullptr, IDC_WAIT) );
		}
	}

	return oldCursor;
}




