#include "stdafx.h"
#include "Map.h"

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
{
	return _lockCount > 0 ? lmLock : lmUnlock;
}

// *******************************************************
//	  GetShowVersionNumber()
// *******************************************************
VARIANT_BOOL CMapView::GetShowVersionNumber(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _showVersionNumber ? VARIANT_TRUE : VARIANT_FALSE;
}

// *******************************************************
//	  SetShowVersionNumber()
// *******************************************************
void CMapView::SetShowVersionNumber(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_showVersionNumber != newVal)
	{
		_showVersionNumber = newVal;
		RedrawCore(RedrawSkipAllLayers, false);
	}
}

// *******************************************************
//	  ShowRedrawTime()
// *******************************************************
VARIANT_BOOL CMapView::GetShowRedrawTime(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _showRedrawTime ? VARIANT_TRUE : VARIANT_FALSE;
}

// *******************************************************
//	  SetShowRedrawTime()
// *******************************************************
void CMapView::SetShowRedrawTime(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_showRedrawTime != newVal)
	{
		_showRedrawTime = newVal;
		RedrawCore(RedrawSkipAllLayers, false);
	}
}

VARIANT_BOOL CMapView::GetCanUseImageGrouping()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _canUseImageGrouping ? VARIANT_TRUE : VARIANT_FALSE;
}

void CMapView::SetCanUseImageGrouping(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_canUseImageGrouping = (newVal != VARIANT_FALSE);
}

short CMapView::GetMapResizeBehavior()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _mapResizeBehavior;
}

void CMapView::SetMapResizeBehavior(short nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_mapResizeBehavior = (tkResizeBehavior)nNewValue;
}

void CMapView::SetTrapRMouseDown(BOOL nNewValue)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_doTrapRMouseDown = nNewValue;
}
BOOL CMapView::GetTrapRMouseDown()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _doTrapRMouseDown;
}

void CMapView::SetDisableWaitCursor(BOOL nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_disableWaitCursor = nNewValue;
}

BOOL CMapView::GetDisableWaitCursor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _disableWaitCursor;
}

ICallback* CMapView::GetGlobalCallback()
{
	if( _globalCallback )
		_globalCallback->AddRef();
	return _globalCallback;
}

void CMapView::SetGlobalCallback(ICallback* newValue)
{
	ICallback * cback = NULL;
	newValue->QueryInterface(IID_ICallback, (void**)&cback);

	if( _globalCallback )
		_globalCallback->Release();

	_globalCallback = cback;

}

BOOL CMapView::GetUseSeamlessPan(void)
{
	return _useSeamlessPan;
}

void CMapView::SetUseSeamlessPan(BOOL newVal)
{
	_useSeamlessPan = newVal;
}

BSTR CMapView::GetSerialNumber(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return _serial.AllocSysString();
}

void CMapView::SetSerialNumber(LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (VerifySerial(newVal))
	{
		_serial = newVal;
		SetModifiedFlag();
	}
	else
		AfxMessageBox("Invalid serial number!", MB_OK | MB_ICONEXCLAMATION, 0);
}

void CMapView::SetUseAlternatePanCursor(VARIANT_BOOL nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_useAlternatePanCursor = (nNewValue != VARIANT_FALSE);
	//_cursorPan = (_useAlternatePanCursor == TRUE ? AfxGetApp()->LoadCursor(IDC_PAN_ALTERNATE) : AfxGetApp()->LoadCursor(IDC_PAN));
}

VARIANT_BOOL CMapView::GetUseAlternatePanCursor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _useAlternatePanCursor ? VARIANT_TRUE : VARIANT_FALSE;
}

void CMapView::SetRecenterMapOnZoom(VARIANT_BOOL nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_recenterMapOnZoom = (nNewValue != VARIANT_FALSE);
}

VARIANT_BOOL CMapView::GetRecenterMapOnZoom()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _recenterMapOnZoom ? VARIANT_TRUE : VARIANT_FALSE;
}

void CMapView::SetShowCoordinatesBackground(VARIANT_BOOL nNewValue)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _showCoordinatesBackground = (nNewValue != VARIANT_FALSE);
}

VARIANT_BOOL CMapView::GetShowCoordinatesBackground()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return _showCoordinatesBackground ? VARIANT_TRUE : VARIANT_FALSE;
}

// *************************************************************** //
//		Setting/getting mouse wheel speed						   //
// *************************************************************** //
DOUBLE CMapView::GetMouseWheelSpeed(void)
{
	return _mouseWheelSpeed;
}
void CMapView::SetMouseWheelSpeed(DOUBLE newVal)
{
	if(newVal < 0.1 || newVal > 10)
	{
		ErrorMessage(tkINVALID_PARAMETER_VALUE);
		return;
	}
	_mouseWheelSpeed = newVal;
}

//*********************************************************************
//	ShapeDrawingMethod()								
//*********************************************************************
void CMapView::SetShapeDrawingMethod(short newVal)
{
	_shapeDrawingMethod = (tkShapeDrawingMethod)newVal;
	
	// generating or clearing per-shape options
	for(size_t i = 0; i < _activeLayers.size(); i++)
	{
		Layer * l = _allLayers[_activeLayers[i]];
		if( l != NULL )
		{	
			if(l->IsShapefile())
			{
				AlignShapeLayerAndShapes(l);
			}
		}
	}
}
short CMapView::GetShapeDrawingMethod(void)
{
	return _shapeDrawingMethod;
}

// **********************************************************
//			Get/SetMapUnits()
// **********************************************************
tkUnitsOfMeasure CMapView::GetMapUnits(void)
{
	return _unitsOfMeasure;
}
void CMapView::SetMapUnits(tkUnitsOfMeasure units)
{
	_unitsOfMeasure = units;
}

// **********************************************************
//			Get/SetMapRotationAngle()
// **********************************************************
//ajp (June 2010)
void CMapView::SetMapRotationAngle(float nNewValue)
{
	// !!! Map rotation is temporary unsupported. GDI+ version of it should be implemented
	// !!! See CMapView::DrawNextFrame
	ErrorMessage(tkMETHOD_NOT_IMPLEMENTED);
	return; 

	_rotateAngle = nNewValue;
	if (_rotate == NULL)
		_rotate = new Rotate();
	_rotate->setRotateAngle(_rotateAngle);
}
//ajp (June 2010)
float CMapView::GetMapRotationAngle()
{
	return (float)_rotateAngle;
}


// ****************************************************************** 
//			VersionNumber
// ****************************************************************** 
BSTR CMapView::GetVersionNumber(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_versionNumber.GetLength() == 0)
		_versionNumber = Utility::GetFileVersionString();
	USES_CONVERSION;
	return A2BSTR(_versionNumber);
}

// *****************************************************
//		ScalebarUnits
// *****************************************************
tkScalebarUnits CMapView::GetScalebarUnits(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _scalebarUnits;
}
void CMapView::SetScalebarUnits(tkScalebarUnits pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_scalebarUnits = pVal;
	if( !_lockCount )
		InvalidateControl();
}

// *****************************************************
//		ScalebarVisible
// *****************************************************
void CMapView::SetScalebarVisible(VARIANT_BOOL pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_scalebarVisible = (pVal != VARIANT_FALSE);
	if( !_lockCount )
		InvalidateControl();
}
VARIANT_BOOL CMapView::GetScalebarVisible(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _scalebarVisible ? VARIANT_TRUE : VARIANT_FALSE;
}

// *****************************************************
//		ShowZoomBar
// *****************************************************
void CMapView::SetShowZoomBar(VARIANT_BOOL pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_zoombarVisible = (pVal != VARIANT_FALSE);
	if( !_lockCount )
	{
		_canUseMainBuffer = false;
		InvalidateControl();
	}
}
VARIANT_BOOL CMapView::GetShowZoomBar(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _zoombarVisible ? VARIANT_TRUE : VARIANT_FALSE;
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
	_tiles->AddRef();
	return _tiles;
}

// *****************************************************
//		GetFileManager
// *****************************************************
IFileManager* CMapView::GetFileManager(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_fileManager)
		_fileManager->AddRef();
	return _fileManager;
}

// *****************************************************
//		GetIdentifier
// *****************************************************
IIdentifier* CMapView::GetIdentifier(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_identifier)
		_identifier->AddRef();
	return _identifier;
}


// *****************************************************
//		ZoomBehavior
// *****************************************************
short CMapView::GetZoomBehavior()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _zoomBehavior;
}
void CMapView::SetZoomBehavior(short nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_zoomBehavior = (tkZoomBehavior)nNewValue;
}

// ***************************************************************
//	ForceDiscreteZoom
// ***************************************************************
bool CMapView::ForceDiscreteZoom()
{
	// should we use it when tiles are invisible or there is no Internet connection?
	return _zoomBehavior == zbUseTileLevels && _transformationMode != tmNotDefined && _tileProjectionState != ProjectionDoTransform;
}

// ***************************************************************
//	HasRotation
// ***************************************************************
bool CMapView::HasRotation()
{
	return _rotate != NULL && _rotateAngle != 0 && false;		// TODO: restore, reimplement and test
}

// *****************************************************
//		AnimationOnZooming
// *****************************************************
tkCustomState CMapView::GetAnimationOnZooming()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _zoomAnimation;
}
void CMapView::SetAnimationOnZooming(tkCustomState nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_zoomAnimation = nNewValue;
}

// *****************************************************
//		InertiaOnPanning
// *****************************************************
tkCustomState CMapView::GetInertiaOnPanning()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _panningInertia;
}
void CMapView::SetInertiaOnPanning(tkCustomState nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_panningInertia = nNewValue;
}

// *****************************************************
//		ReuseTileBuffer
// *****************************************************
VARIANT_BOOL CMapView::GetReuseTileBuffer()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _reuseTileBuffer;
}
void CMapView::SetReuseTileBuffer(VARIANT_BOOL nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_reuseTileBuffer = nNewValue;
}

// *****************************************************
//		ZoomBarVerbosity
// *****************************************************
tkZoomBarVerbosity CMapView::GetZoomBarVerbosity()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _zoomBarVerbosity;
}
void CMapView::SetZoomBarVerbosity(tkZoomBarVerbosity nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_zoomBarVerbosity = nNewValue;
}

// *****************************************************
//		ZoomBoxStyle
// *****************************************************
tkZoomBoxStyle CMapView::GetZoomBoxStyle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _zoomBoxStyle;
}
void CMapView::SetZoomBoxStyle(tkZoomBoxStyle nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_zoomBoxStyle = nNewValue;
}

// *****************************************************
//		ZoomBarMinZoom
// *****************************************************
long CMapView::GetZoomBarMinZoom()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _zoomBarMinZoom;
}
void CMapView::SetZoomBarMinZoom(long nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (nNewValue < 1)	nNewValue = -1;
	if (nNewValue > 25)	nNewValue = 25;
	_zoomBarMinZoom = nNewValue;

	RedrawCore(tkRedrawType::RedrawAll, true, true);
}

// *****************************************************
//		ZoomBarMaxZoom
// *****************************************************
long CMapView::GetZoomBarMaxZoom()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return _zoomBarMaxZoom;
}
void CMapView::SetZoomBarMaxZoom(long nNewValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (nNewValue < 1)	nNewValue = -1;
	if (nNewValue > 25)	nNewValue = 25;
	_zoomBarMaxZoom = nNewValue;

	RedrawCore(tkRedrawType::RedrawAll, true, true);
}