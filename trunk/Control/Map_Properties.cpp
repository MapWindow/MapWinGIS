#pragma region Include
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"
#pragma endregion

// *******************************************************
//	  HWnd()
// *******************************************************
long CMapView::HWnd()
{
	return (long)this->m_hWnd;
}

// *******************************************************
//	  GetIsLocked()
// *******************************************************
short CMapView::GetIsLocked()
{	if( m_lockCount > 0 )
		return lmLock;
	else
		return lmUnlock;
}

// *******************************************************
//	  GetShowVersionNumber()
// *******************************************************
VARIANT_BOOL CMapView::GetShowVersionNumber(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_ShowVersionNumber;
}

// *******************************************************
//	  SetShowVersionNumber()
// *******************************************************
void CMapView::SetShowVersionNumber(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_ShowVersionNumber != newVal)
	{
		m_ShowVersionNumber = newVal;
		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
	}
}

// *******************************************************
//	  ShowRedrawTime()
// *******************************************************
VARIANT_BOOL CMapView::GetShowRedrawTime(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_ShowRedrawTime;
}

// *******************************************************
//	  SetShowRedrawTime()
// *******************************************************
void CMapView::SetShowRedrawTime(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_ShowRedrawTime != newVal)
	{
		m_ShowRedrawTime = newVal;
		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();
	}
}

VARIANT_BOOL CMapView::GetCanUseImageGrouping()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _canUseImageGrouping;
}

void CMapView::SetCanUseImageGrouping(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_canUseImageGrouping = newVal;
}

short CMapView::GetMapResizeBehavior()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return rbMapResizeBehavior;
}

void CMapView::SetMapResizeBehavior(short nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	rbMapResizeBehavior = (tkResizeBehavior)nNewValue;
}

void CMapView::SetTrapRMouseDown(BOOL nNewValue)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	DoTrapRMouseDown = nNewValue;
}
BOOL CMapView::GetTrapRMouseDown()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return DoTrapRMouseDown;
}
void CMapView::SetDisableWaitCursor(BOOL nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_DisableWaitCursor = nNewValue;
}

BOOL CMapView::GetDisableWaitCursor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_DisableWaitCursor;
}

LPDISPATCH CMapView::GetGlobalCallback()
{
	if( m_globalCallback )
		m_globalCallback->AddRef();
	return m_globalCallback;
}

void CMapView::SetGlobalCallback(LPDISPATCH newValue)
{
	ICallback * cback = NULL;
	newValue->QueryInterface(IID_ICallback, (void**)&cback);

	if( m_globalCallback )
		m_globalCallback->Release();

	m_globalCallback = cback;

}

BOOL CMapView::GetUseSeamlessPan(void)
{
	return m_UseSeamlessPan;
}

void CMapView::SetUseSeamlessPan(BOOL newVal)
{
	m_UseSeamlessPan = newVal;
}

BSTR CMapView::GetSerialNumber(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return m_serial.AllocSysString();
}

void CMapView::SetSerialNumber(LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (VerifySerial(newVal))
	{
		m_serial = newVal;
		SetModifiedFlag();
	}
	else
		AfxMessageBox("Invalid serial number!", MB_OK | MB_ICONEXCLAMATION, 0);
}

// *************************************************************** //
//		Setting/getting mouse wheel speed						   //
// *************************************************************** //
DOUBLE CMapView::GetMouseWheelSpeed(void)
{
	return m_MouseWheelSpeed;
}
void CMapView::SetMouseWheelSpeed(DOUBLE newVal)
{
	if(newVal < 0.1 || newVal > 10)
	{
		m_lastErrorCode = tkINVALID_PARAMETER_VALUE;
		if( m_globalCallback != NULL ) m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		return;
	}
	m_MouseWheelSpeed = newVal;
}

//*********************************************************************
//	ShapeDrawingMethod()								
//*********************************************************************
void CMapView::SetShapeDrawingMethod(short newVal)
{
	m_ShapeDrawingMethod = (tkShapeDrawingMethod)newVal;
	
	// generating or clearing per-shape options
	for(size_t i = 0; i < m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			if(l->type == ShapefileLayer)
			{
				AlignShapeLayerAndShapes(l);
			}
		}
	}
}
short CMapView::GetShapeDrawingMethod(void)
{
	return m_ShapeDrawingMethod;
}

// **********************************************************
//			Get/SetMapUnits()
// **********************************************************
tkUnitsOfMeasure CMapView::GetMapUnits(void)
{
	return m_unitsOfMeasure;
}
void CMapView::SetMapUnits(tkUnitsOfMeasure units)
{
	m_unitsOfMeasure = units;
}

// **********************************************************
//			Get/SetMapRotationAngle()
// **********************************************************
//ajp (June 2010)
void CMapView::SetMapRotationAngle(float nNewValue)
{
	// !!! Map rotation is temporary unsupported. GDI+ version of it should be implemented
	// !!! See CMapView::DrawNextFrame
	return; 
	m_RotateAngle = nNewValue;
	if (m_Rotate == NULL)
		m_Rotate = new Rotate();
	m_Rotate->setRotateAngle(m_RotateAngle);
}
//ajp (June 2010)
float CMapView::GetMapRotationAngle()
{
	return (float)m_RotateAngle;
}

// ****************************************************************** 
//			VersionNumber
// ****************************************************************** 
// Returns the state of SHAPE_OLD constant
LONG CMapView::GetVersionNumber(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	#ifdef SHAPE_OLD
		return 0;
	#else
		return 1;
	#endif
}

void CMapView::SetVersionNumber(LONG newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetNotSupported();
}

// *****************************************************
//		ScalebarUnits
// *****************************************************
tkScalebarUnits CMapView::GetScalebarUnits(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_scalebarUnits;
}
void CMapView::SetScalebarUnits(tkScalebarUnits pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_scalebarUnits = pVal;
	if( !m_lockCount )
		InvalidateControl();
}

// *****************************************************
//		ScalebarVisible
// *****************************************************
void CMapView::SetScalebarVisible(VARIANT_BOOL pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_scalebarVisible = pVal;
	if( !m_lockCount )
		InvalidateControl();
}
VARIANT_BOOL CMapView::GetScalebarVisible(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_scalebarVisible;
}

bool CMapView::SendMouseMove()
{	
	return m_sendMouseMove ? true : false;
}

bool CMapView::SendSelectBoxDrag()
{	
	return  m_sendSelectBoxDrag? true : false;
}

// *****************************************************
//		GetTiles
// *****************************************************
ITiles* CMapView::GetTiles(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_tiles->AddRef();
	return m_tiles;
}

ITiles* CMapView::GetTilesNoRef(void)
{
	return m_tiles;
}

// *****************************************************
//		SetTiles
// *****************************************************
void CMapView::SetTiles(ITiles* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetModifiedFlag();
}