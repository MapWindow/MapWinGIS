// ********************************************************************************************************
// File name: Map.cpp
// Description: Implementation of the CMapView ActiveX Control class.
// ********************************************************************************************************
// The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at 
// http://www.mozilla.org/MPL/ 
// Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND, either express or implied. See the License for the specific language governing rights and 
// limitations under the License. 
//
// The Original Code is MapWindow Open Source. 
//
// The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
// Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
// public domain in March 2004.  
//
// Contributor(s): (Open source contributors should list themselves and their modifications here). 
// ----------------------------------------------------------------------------------------
// dpa, ah, Chris Michaelis (cdm), Stan, Tom Shanley (tws), Andrew Purkis (ajp), Ray Quay,
// Rob Cairns, Sergei Leschinski (lsu), Neio, sm, 
// ----------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Map.h"
#include "MapPpg.h"
#include "Measuring.h"
#include "Tiles.h"
#include "EditShape.h"
using namespace std;

//disable some known warnings we don't care about
#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4800)
#pragma warning(disable:4554)
#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#pragma region Control info
IMPLEMENT_DYNCREATE(CMapView, COleControl)

// Property pages
// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CMapView, 1)
	PROPPAGEID(CMapPropPage::guid)
END_PROPPAGEIDS(CMapView)

// Initialize class factory and guid
IMPLEMENT_OLECREATE_EX(CMapView, "MAPWINGIS.Map.1",
	0x54f4c2f7, 0xed40, 0x43b7, 0x9d, 0x6f, 0xe4, 0x59, 0x65, 0xdf, 0x7f, 0x95)

// Type library ID and version
IMPLEMENT_OLETYPELIB(CMapView, _tlid, _wVerMajor, _wVerMinor)

// Interface IDs
const IID BASED_CODE IID_DMap =
		{ 0x1d077739, 0xe866, 0x46a0, { 0xb2, 0x56, 0x8a, 0xec, 0xc0, 0x4f, 0x23, 0x12 } };
const IID BASED_CODE IID_DMapEvents =
		{ 0xabea1545, 0x8ab, 0x4d5c, { 0xa5, 0x94, 0xd3, 0x1, 0x72, 0x11, 0xea, 0x95 } };

// Control type information
static const DWORD BASED_CODE _dwMapOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMapView, IDS_MAP, _dwMapOleMisc)

#pragma endregion

// ***********************************************************
// CMapView::CMapFactory::UpdateRegistry
// ***********************************************************
// Adds or removes system registry entries for CMap
BOOL CMapView::CMapViewFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MAP,
			IDB_MAP,
			afxRegApartmentThreading,
			_dwMapOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

#pragma region Constructor/destructor

// ********************************************************************
//		CMapView() constructor
// ********************************************************************
CMapView::CMapView() 
	: _vals("AZ0CY1EX2GV3IT4KR5MP6ON7QL8SJ9UH0WF1DB2"), 
	_valsLen(39), _isSnapshot(false),
	_brushBlack(Gdiplus::Color::Black), 
	_brushWhite(Gdiplus::Color::White), 
	_penGray(Gdiplus::Color::Gray),
	_brushGray(Gdiplus::Color::Gray), 
	_penDarkGray(Gdiplus::Color::DarkSlateGray),
	_propertyExchange(NULL)
{
	Startup();
	SetDefaults();
}

// ****************************************************
//	    Destructor
// ****************************************************
CMapView::~CMapView()
{
	this->RemoveAllLayers();

	this->ClearDrawings();

	ReleaseTempObjects();
	
	this->Shutdown();
}

// **********************************************************************
//	Clear
// **********************************************************************
void CMapView::Clear()
{
	this->RemoveAllLayers();
	this->ClearDrawings();
	ReleaseTempObjects();
	SetDefaults();

	IGeoProjection* p = NULL;
	GetUtils()->CreateInstance(idGeoProjection, (IDispatch**)&p);
	SetGeoProjection(p);

	Redraw();
}

// **********************************************************************
//	Startup
// **********************************************************************
// Must be called from constructor only
void CMapView::Startup()
{
	this->GdiplusStartup();
	InitializeIIDs(&IID_DMap, &IID_DMapEvents);
	
	Gdiplus::FontFamily family(L"Courier New");
	_fontCourier = new Gdiplus::Font(&family, (Gdiplus::REAL)9.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);

	Gdiplus::FontFamily family2(L"Arial");
	_fontArial = new Gdiplus::Font(&family2, (Gdiplus::REAL)9.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
	
	_panningAnimation = false;
	_zoombarTargetZoom = -1;
	_dragging.Move = CPoint(0,0);
	_dragging.Start = CPoint(0,0);
	_dragging.Operation = DragNone;
	_currentZoom = -1;
	_spacePressed = false;
	_lastRedrawTime = 0.0f;
	_projectionChangeCount = 0;
	_tileBuffer.Extents.left = 0.0;
	_tileBuffer.Extents.right = 0.0;
	_tileBuffer.Extents.top = 0.0;
	_tileBuffer.Extents.bottom = 0.0;
	_tileBuffer.Zoom = -1;
	_tileBuffer.Provider = (int)tkTileProvider::ProviderNone;
	_tileBuffer.Initialized = false;
	_tileProjectionState = ProjectionDoTransform;

	_globalCallback = NULL;

	m_key = "";

	_viewHeight = 0;
	_viewWidth = 0;

	//Cursors
	_cursorPan = AfxGetApp()->LoadCursor(IDC_PAN);
	_cursorZoomin = AfxGetApp()->LoadCursor(IDC_ZOOMIN);
	_cursorZoomout = AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
	_cursorSelect = AfxGetApp()->LoadCursor(IDC_SELECT);
	_cursorMeasure = AfxGetApp()->LoadCursor(IDC_MEASURE);
	_udCursor = NULL;

	_ttipCtrl = new CButton();
	m_drawMutex.Unlock();
	_mapstateMutex.Unlock();
	_setmaxstdio(2048);
	srand (time(NULL));

	_imageGroups = NULL;
	_hotTracking.Shapefile = NULL;
	_rotate = NULL;

	#ifdef _DEBUG
	gMemLeakDetect.stopped = true;
	#endif

	GetUtils()->CreateInstance(idTiles, (IDispatch**)&_tiles);
	((CTiles*)_tiles)->Init((void*)this);

	GetUtils()->CreateInstance(idMeasuring, (IDispatch**)&_measuring);
	GetUtils()->CreateInstance(idEditShape, (IDispatch**)&_editShape);

	GetUtils()->CreateInstance(idFileManager, (IDispatch**)&_fileManager);

	InitProjections();
	
	GetMeasuringBase()->SetMapView((void*)this, ShapeInputMode::simMeasuring);
	GetEditShapeBase()->SetMapView((void*)this, ShapeInputMode::simEditing);
	

	if (_panningInertia != csFalse)
		_panningLock.Unlock();

	#ifdef _DEBUG
	gMemLeakDetect.stopped = false;
	#endif
}

// **********************************************************************
//	SetDefaults
// **********************************************************************
void CMapView::SetDefaults()
{
	// temp state variables
	_currentDrawing = -1;
	_rectTrackerIsActive = false;
	_lastWidthMeters = 0.0;
	_redrawId = 0;
	_lastZooombarPart = ZoombarNone;
	_hotTracking.LayerHandle = -1;
	_hotTracking.ShapeId = -1;
	_isSizing = false;
	_showingToolTip = FALSE;
	_lockCount = 0;
	_canUseLayerBuffer = FALSE;
	_canUseMainBuffer = false;
	_leftButtonDown = FALSE;
	_lastCursorMode = cmNone;
	_reverseZooming = false;
	_activeLayerPosition = 0;
	_knownExtents = keNone;
	_measuringPersistent = false;
	_lastErrorCode = tkNO_ERROR;

	// public control properties
	m_sendMouseMove = FALSE;
	m_sendMouseDown = FALSE;
	m_sendMouseUp = FALSE;
	m_sendSelectBoxDrag = FALSE;
	m_sendSelectBoxFinal = FALSE;
	m_sendOnDrawBackBuffer = FALSE;
	m_extentHistory = 20;
	m_doubleBuffer = TRUE;
	m_zoomPercent = 0.3;
	m_cursorMode = tkCursorMode::cmZoomIn;
	m_backColor = RGB( 255, 255, 255 );
	m_extentPad = 0.02;
	_rotateAngle = 0.0f;
	_canUseImageGrouping = VARIANT_FALSE;
	_grabProjectionFromData = VARIANT_TRUE;
	_hasHotTracking = false;
	_showCoordinates = cdmAuto;
	_zoomBehavior = zbUseTileLevels;
	_scalebarVisible = VARIANT_TRUE;
	_zoombarVisible = VARIANT_TRUE;
	_multilineLabeling = true;
	_mapResizeBehavior = rbClassic;
	_doTrapRMouseDown = TRUE;
	_useSeamlessPan = FALSE;
	_mouseWheelSpeed = 0.5;
	_shapeDrawingMethod = dmNewSymbology;
	_unitsOfMeasure = umMeters;
	_disableWaitCursor = false;
	_lineSeparationFactor = 3;		
	_useLabelCollision = false;
	_showRedrawTime = VARIANT_FALSE;
	_showVersionNumber = VARIANT_FALSE;	
	_scalebarUnits = tkScalebarUnits::GoogleStyle;
	_zoomBarVerbosity = tkZoomBarVerbosity::zbvFull;
	_panningInertia = csFalse;
	_reuseTileBuffer = TRUE;
	_zoomAnimation = csAuto;
	_zoomBoxStyle = tkZoomBoxStyle::zbsBlue;
	_projectionMismatchBehavior = mbIgnore;
	_zoomBarMinZoom = -1;
	_zoomBarMaxZoom = -1;

	// TODO: perhaps it's better to grab those from property exchanged (i.e. reverting only runtime changes)
	// perhaps this call can do this:
	//RequestAsynchronousExchange(GetPropertyExchangeVersion());

	((CTiles*)_tiles)->SetDefaults();
	GetMeasuringBase()->Clear();
}

// **********************************************************************
//	ReleaseTempObjects
// **********************************************************************
void CMapView::ReleaseTempObjects()
{	
	_collisionList.Clear();
	
	_rotateAngle = 0.0f;
	if (_rotate)
	{
		delete _rotate;
		_rotate = NULL;
	}

	if (_imageGroups)
	{
		for (int i = 0; i < _imageGroups->size(); i++)
		{
			delete (*_imageGroups)[i];
		}
		_imageGroups->clear();
		delete _imageGroups;
		_imageGroups = NULL;
	}
}

// **********************************************************************
//	Shutdown
// **********************************************************************
// Must be called from destructor only
void CMapView::Shutdown()
{
	if (_fontCourier) {
		delete _fontCourier;
		_fontCourier = NULL;
	}

	if (_fontArial) {
		delete _fontArial;
		_fontArial = NULL;
	}
	
	((CTiles*)_tiles)->Stop();
	if (_bufferBitmap)
	{
		delete _bufferBitmap;
		_bufferBitmap = NULL;
	}

	if (_tempBitmap)
	{
		delete _tempBitmap;
		_tempBitmap = NULL;
	}

	if (_tilesBitmap)
	{
		delete _tilesBitmap;
		_tilesBitmap = NULL;
	}

	if (_drawingBitmap)
	{
		delete _drawingBitmap;
		_drawingBitmap = NULL;
	}

	if (_layerBitmap)
	{
		delete _layerBitmap;
		_layerBitmap = NULL;
	}

	if( _globalCallback )
		_globalCallback->Release();

	
	ClearPanningList();

	ReleaseProjections();

	if (_fileManager)
		_fileManager->Release();

	if (_measuring)
		_measuring->Release();

	if (_editShape) {
		_editShape->Release();
		_editShape = NULL;
	}

	if (_tiles)
	{
		((CTiles*)_tiles)->ClearAll();
		_tiles->Release();
	}

	if (_hotTracking.Shapefile)
		_hotTracking.Shapefile->Release();

	delete _ttipCtrl;

	this->GdiplusShutdown();
}

// ********************************************************************
//		OnCreate()
// ********************************************************************
int CMapView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	_bufferBitmap = NULL;
	_tilesBitmap = NULL;
	_layerBitmap = NULL;
	_drawingBitmap = NULL;
	_tempBitmap = NULL;

	DragAcceptFiles( TRUE );

	CRect rect(0,0,0,0);
	_ttipCtrl->Create(NULL,WS_CHILD,rect,this,IDC_TTBTN);
	_ttipCtrl->ShowWindow(FALSE);

	//CTool Tip
	_ttip.Create(this,TTS_ALWAYSTIP);
	_ttip.Activate(TRUE);
	_ttip.AddTool(this,"",rect,IDC_TTBTN);
	_ttip.SetDelayTime(TTDT_AUTOPOP,0);
	_ttip.SetDelayTime(TTDT_INITIAL,0);
	_ttip.SetDelayTime(TTDT_RESHOW,0);

	return 0;
}
#pragma endregion

#pragma region Property exchange

// **********************************************************************
//	GetPropertyExchangeVersion
// **********************************************************************
DWORD CMapView::GetPropertyExchangeVersion()
{
	// properties can be added between versions, so let use a bit different numbering
	//return MAKELONG(_wVerMinor, _wVerMajor);
	return MAKELONG(49, 11);
}

// **********************************************************************
//	CMapView::DoPropExchange
// **********************************************************************
// Persistence support: http://msdn.microsoft.com/en-us/library/xxf9wx2c.aspx
void CMapView::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, GetPropertyExchangeVersion());		// version should be changed when new properties are added
	
	GetPropertyExchangeVersion();

	// this will exchange stock properties like BackColor or Enabled: 
	// http://msdn.microsoft.com/en-us/library/aa268861(v=vs.60).aspx
	COleControl::DoPropExchange(pPX);

	try
	{
		bool loading = pPX->IsLoading();

		PX_Color( pPX, "BackColor", m_backColor, RGB( 255, 255, 255 ) );
		PX_Double( pPX, "ExtentPad", m_extentPad, .02 );
		PX_Short( pPX, "CursorMode", m_cursorMode, 0 );
		PX_Double( pPX, "ZoomPercent", m_zoomPercent, .3 );
		PX_Bool( pPX, "SendMouseMove", m_sendMouseMove, FALSE );
		PX_Bool( pPX, "SendMouseDown", m_sendMouseDown, FALSE );
		PX_Bool( pPX, "SendMouseUp", m_sendMouseUp, FALSE );
		PX_Bool( pPX, "SendSelectBoxDrag", m_sendSelectBoxDrag, FALSE );
		PX_Bool( pPX, "SendSelectBoxFinal", m_sendSelectBoxFinal, FALSE );
		PX_Long( pPX, "ExtentHistory", m_extentHistory, 20 );
		PX_Bool( pPX, "DoubleBuffer", m_doubleBuffer, TRUE );
		PX_Bool( pPX, "SendOnDrawBackBuffer", m_sendOnDrawBackBuffer, FALSE);
		PX_Bool( pPX, "ShowRedrawTime", _showRedrawTime, FALSE);
		PX_Bool( pPX, "ShowVersionNumber", _showVersionNumber, FALSE);
		PX_Double( pPX, "MouseWheelSpeed", _mouseWheelSpeed, .5 );
		PX_Bool( pPX, "UseSeamlessPan", _useSeamlessPan, FALSE );
		PX_Bool( pPX, "MultilineLabels", _multilineLabeling, FALSE );
		PX_Bool( pPX, "TrapRMouseDown", _doTrapRMouseDown, TRUE );
		PX_Bool( pPX, "DisableWaitCursor", _disableWaitCursor, FALSE );	
		PX_Float( pPX, "MapRotationAngle", _rotateAngle, 0.0 );
		PX_Bool( pPX, "CanUseImageGrouping", _canUseImageGrouping, FALSE );
		PX_Bool( pPX, "ScalebarVisible", _scalebarVisible, TRUE );
		
		// enumerated constants aren't supported directly so temp buffer will be used
		// we don't care about the direction of exchange, doing both getting and setting
		long temp;
		temp = (long)_mapResizeBehavior;
		PX_Long( pPX, "MapResizeBehavior", temp, 0 );	//rbClassic
		_mapResizeBehavior = (tkResizeBehavior)temp;

		temp = (long)_zoomBehavior;
		PX_Long( pPX, "ZoomBehavior", temp, 1 );	// zbUseTileLevels
		_zoomBehavior = (tkZoomBehavior)temp;

		temp = (long)_scalebarUnits;
		PX_Long( pPX, "ScalebarUnits", temp, 2 );	//suGoogleStyle
		_scalebarUnits = (tkScalebarUnits)temp;

		temp = (long)_shapeDrawingMethod;
		PX_Long( pPX, "ShapeDrawingMethod", temp, 3 );	// dmNewSymbology
		_shapeDrawingMethod = (tkShapeDrawingMethod)temp;

		temp = (long)_unitsOfMeasure;
		PX_Long( pPX, "MapUnits", temp, 6 );			//umMeters
		_unitsOfMeasure = (tkUnitsOfMeasure)temp;

		temp = (long)_showCoordinates;
		PX_Long( pPX, "ShowCoordinates", temp, 1 );			//cdmAuto
		_showCoordinates = (tkCoordinatesDisplay)temp;

		temp = (long)GetTileProvider();
		PX_Long( pPX, "TileProvider", temp, 0 );		// OpenStreetMap
		SetTileProvider((tkTileProvider)temp);

		tkMapProjection projection;
		if (!loading) {
			projection = GetProjection();
			temp = (long)projection;
		}

		if (loading) {
			this->LockWindow(lmLock);
		}

		try
		{
			PX_Long( pPX, "Projection", temp, 0 );			// PROJECTION_NONE
			projection = (tkMapProjection)temp;
			if (loading) {
				SetProjection(projection);
			}
			
			PX_Double( pPX, "xMin", _extents.left, .3 );
			PX_Double( pPX, "xMax", _extents.right, .3 );
			PX_Double( pPX, "yMin", _extents.bottom, .3 );
			PX_Double( pPX, "yMax", _extents.top, .3 );

			if (loading) {
				SetExtentsCore(_extents, false);
			}
		}
		catch(...) {
		}
		
		if (loading) {
			this->LockWindow(lmUnlock);
		}

		PX_Bool( pPX, "ShowZoomBar", _zoombarVisible, TRUE );
		PX_Bool( pPX, "GrabProjectionFromData", _grabProjectionFromData, TRUE );

		temp = (long)_zoomAnimation;
		PX_Long( pPX, "AnimationOnZooming", temp, 0 );			// csAuto
		_zoomAnimation = (tkCustomState)temp;

		temp = (long)_panningInertia;
		PX_Long( pPX, "InertiaOnPanning", temp, 0 );			// csAuto
		_panningInertia = (tkCustomState)temp;

		PX_Bool( pPX, "ReuseTileBuffer", _reuseTileBuffer, TRUE );

		temp = (long)_zoomBarVerbosity;
		PX_Long( pPX, "ZoomBarVerbosity", temp, 1 );			// zbvFull
		_zoomBarVerbosity = (tkZoomBarVerbosity)temp;

		temp = (long)_zoomBoxStyle;
		PX_Long( pPX, "ZoomBoxStyle", temp, 4 );			// zbsBlue
		_zoomBoxStyle = (tkZoomBoxStyle)temp;

		temp = (long)_projectionMismatchBehavior;
		PX_Long( pPX, "ProjectionMistmatchBehavior", temp, 0 );			// mbIgnore
		_projectionMismatchBehavior = (tkMismatchBehavior)temp;

		PX_Long( pPX, "ZoombarMinZoom", _zoomBarMinZoom, -1 );
		PX_Long( pPX, "ZoombarMaxZoom", _zoomBarMaxZoom, -1 );

		m_mapCursor = 0;	// why not to save it?
	}
	catch(...)
	{
	}
}
#pragma endregion

#pragma region Overidden properties
// ***********************************************************************
//	   CMapView::GetControlFlags
// ***********************************************************************
// Flags to customize MFC's implementation of ActiveX controls.
// For information on using these flags, please see MFC technical note
// #nnn, "Optimizing an ActiveX Control".
DWORD CMapView::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();

	// The control's output is not being clipped.
	// The control guarantees that it will not paint outside its
	// client rectangle.
	dwFlags &= ~clipPaintDC;

	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}

// ***************************************************************
//	CMapView::PreCreateWindow()
// ***************************************************************
BOOL CMapView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_OWNDC | CS_BYTEALIGNWINDOW;
	return COleControl::PreCreateWindow(cs);
}
#pragma endregion


#pragma region GdiPlus
// ***************************************************************
//	CMapView::GdiplusStartup()
// ***************************************************************

/* Gdiplus startup/shutdown. Do it on a per control basis.
NB: this used to be in CMapViewWinGISApp::Init/ExitInstance. 
These functions are called from DllMain during control registration
and MSDN says it's not allowed to call GdiplusStartup/Shutdown from DllMain.
Indeed, on our Win2003 and XP boxes, doing so hangs regsvr32.exe.

So, I decided on "gdiplus registration per control" strategy:
1. in ctor/dtor of the control, we bump up/down gdiplus count.
2. when count goes from/to 0, we call GdiplusStartup/Shutdown.
3. we protect GdiplusStartup/Shutdown calls by a critical section,
   to avoid multithreaded surprises (albeit multiple threads are, I think, highly unlikely).*/
ULONG_PTR CMapView::ms_gdiplusToken=NULL;
unsigned CMapView::ms_gdiplusCount=0;
::CCriticalSection CMapView::ms_gdiplusLock;

void CMapView::GdiplusStartup()
{
	CSingleLock l(&ms_gdiplusLock, TRUE);
	if (ms_gdiplusCount == 0)
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::Status s = Gdiplus::GdiplusStartup(&ms_gdiplusToken, &gdiplusStartupInput, NULL);
		if (s != Gdiplus::Ok)
		{
			TRACE(_T("GdiplusStartup failed, error: %d, GetLastError = %d\n"), s, GetLastError());
			ASSERT(FALSE);
			AfxThrowResourceException();
		}
	}
	ms_gdiplusCount++;
}

// ***************************************************************
//	CMapView::GdiplusShutdown()
// ***************************************************************
void CMapView::GdiplusShutdown()
{
	CSingleLock l(&ms_gdiplusLock, TRUE);
	ms_gdiplusCount--;
	if (ms_gdiplusCount == 0)
	{
		Gdiplus::GdiplusShutdown(ms_gdiplusToken);
		ms_gdiplusToken = NULL;
	}
}
#pragma endregion

// ***************************************************************
//	DoUpdateTiles
// ***************************************************************
void CMapView::DoUpdateTiles(bool isSnapshot, CString key)
{
	((CTiles*)_tiles)->LoadTiles((void*)this, isSnapshot, key);		// simply move the to the screen buffer (is performed synchronously)
}

// ***************************************************************
//	ClearPanningList
// ***************************************************************
void CMapView::ClearPanningList()
{
	if (_panningInertia != csFalse)
	{
		_panningLock.Lock();
		for(int i = 0; i < _panningList.size(); i++)
		{
			delete _panningList[i];
		}
		_panningList.clear();
		_panningLock.Unlock();
	}
}

// ***************************************************************
//	GetMeasuringBase
// ***************************************************************
MeasuringBase* CMapView::GetMeasuringBase()
{
	return ((CMeasuring*)_measuring)->GetBase();
}

// ***************************************************************
//	GetEditShapeBase
// ***************************************************************
EditShapeBase* CMapView::GetEditShapeBase()
{
	return ((CEditShape*)_editShape)->GetBase();
}

// ***************************************************************
//	GetAtiveShape
// ***************************************************************
ActiveShape* CMapView::GetAtiveShape()
{
	if (m_cursorMode == cmMeasure) return GetMeasuringBase();
	if (m_cursorMode == cmAddShape) return GetEditShapeBase();
	return NULL;		
}






