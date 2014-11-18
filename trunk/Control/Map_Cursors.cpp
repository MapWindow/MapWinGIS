#include "stdafx.h"
#include "Map.h"
#include "ShapeEditor.h"
#include "EditorHelper.h"

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

// internal call; perhaps some additional logic will be needed
void CMapView::UpdateCursor(tkCursorMode mode, bool clearEditor)
{
	if (mode == m_cursorMode) return;

	if (mode == cmRotateShapes)
	{
		if (!InitRotationTool())
			return;
	}

	bool refreshNeeded = mode == cmRotateShapes || m_cursorMode == cmRotateShapes;

	if (_measuring)
	{
		VARIANT_BOOL vb;
		_measuring->get_Persistent(&vb);
		if (m_cursorMode != cmMeasure && !vb)
			_measuring->Clear();
	}

	if (clearEditor)		// TODO: move to shape editor
	{
		CComPtr<IShape> shp = NULL;
		_shapeEditor->get_RawData(&shp);

		if (shp)
		{
			if (!_shapeEditor->TryStopDigitizing())
				return;	  // don't change cursor as user may loose some data
			tkEditorState state;
			_shapeEditor->get_EditorState(&state);
			if (state != esEmpty)
			{
				if (!_shapeEditor->TryStopDigitizing())
					return;	  // don't change cursor as user may loose some data
			}

			RedrawCore(RedrawSkipDataLayers, false, true);
		}
		else {
			VARIANT_BOOL empty;
			_shapeEditor->get_IsEmpty(&empty);
			if (!empty)
			{
				_shapeEditor->Clear();
				RedrawCore(RedrawSkipDataLayers, false, true);
			}
		}

		if (m_cursorMode == cmAddShape) {
			_shapeEditor->put_EditorState(esDigitize);
		}
	}

	if (EditorHelper::IsGroupOverlayCursor(mode))
	{
		_shapeEditor->StartUnboundShape(mode);
	}

	m_cursorMode = mode;

	OnSetCursor(this, HTCLIENT, 0);
	
	if (refreshNeeded)
		RedrawCore(RedrawTempObjectsOnly, false, true);
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




