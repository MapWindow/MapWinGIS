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
#include "ShapeEditor.h"
#include "UndoList.h"
#include "curl.h"
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

BEGIN_EVENT_MAP(CMapView, COleControl)
	//{{AFX_EVENT_MAP(CMapView)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	EVENT_CUSTOM_ID("MouseDown", eventidMouseDown, FireMouseDown, VTS_I2  VTS_I2  VTS_I4  VTS_I4)
	EVENT_CUSTOM_ID("MouseUp", eventidMouseUp, FireMouseUp, VTS_I2  VTS_I2  VTS_I4  VTS_I4)
	EVENT_CUSTOM_ID("MouseMove", eventidMouseMove, FireMouseMove, VTS_I2  VTS_I2  VTS_I4  VTS_I4)
	EVENT_CUSTOM_ID("FileDropped", eventidFileDropped, FireFileDropped, VTS_BSTR)
	EVENT_CUSTOM_ID("SelectBoxFinal", eventidSelectBoxFinal, FireSelectBoxFinal, VTS_I4  VTS_I4  VTS_I4  VTS_I4)
	EVENT_CUSTOM_ID("SelectBoxDrag", eventidSelectBoxDrag, FireSelectBoxDrag, VTS_I4  VTS_I4  VTS_I4  VTS_I4)
	EVENT_CUSTOM_ID("ExtentsChanged", eventidExtentsChanged, FireExtentsChanged, VTS_NONE)
	EVENT_CUSTOM_ID("MapState", eventidMapState, FireMapState, VTS_I4)
	EVENT_CUSTOM_ID("OnDrawBackBuffer", eventidOnDrawBackBuffer, FireOnDrawBackBuffer, VTS_I4)
	EVENT_CUSTOM_ID("ShapeHighlighted", eventidShapeHighlighted, FireShapeHighlighted, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("BeforeDrawing", eventidBeforeDrawing, FireBeforeDrawing, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("AfterDrawing", eventidAfterDrawing, FireAfterDrawing, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("TilesLoaded", eventidTilesLoaded, FireTilesLoaded, VTS_BOOL VTS_BSTR VTS_BOOL)
	EVENT_CUSTOM_ID("MeasuringChanged", eventidMeasuringChanged, FireMeasuringChanged, VTS_I4)
	EVENT_CUSTOM_ID("BeforeShapeEdit", eventidBeforeShapeEdit, FireBeforeShapeEdit, VTS_I4 VTS_I4 VTS_PI4)
	EVENT_CUSTOM_ID("ValidateShape", eventidValidateShape, FireValidateShape, VTS_I4 VTS_I4 VTS_DISPATCH VTS_PI4)
	EVENT_CUSTOM_ID("AfterShapeEdit", eventidAfterShapeEdit, FireAfterShapeEdit, VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("ChooseLayer", eventidChooseLayer, FireChooseLayer, VTS_I4 VTS_R8 VTS_R8 VTS_PI4)
	EVENT_CUSTOM_ID("ShapeValidationFailed", eventidShapeValidationFailed, FireShapeValidationFailed, VTS_BSTR)
	EVENT_CUSTOM_ID("BeforeDeleteShape", eventidBeforeDeleteShape, FireBeforeDeleteShape, VTS_I4 VTS_PI4)
	EVENT_CUSTOM_ID("ProjectionChanged", eventidProjectionChanged, FireProjectionChanged, VTS_NONE)
	EVENT_CUSTOM_ID("UndoListChanged", eventidUndoListChanged, FireUndoListChanged, VTS_NONE)
	EVENT_CUSTOM_ID("SelectionChanged", eventidSelectionChanged, FireSelectionChanged, VTS_I4)
	EVENT_CUSTOM_ID("ShapeIdentified", eventidShapeIdentified, FireShapeIdentified, VTS_I4 VTS_I4 VTS_R8 VTS_R8)
	EVENT_CUSTOM_ID("LayerProjectionIsEmpty", eventidLayerProjectionIsEmpty, FireLayerProjectionIsEmpty, VTS_I4 VTS_PI4)
	EVENT_CUSTOM_ID("ProjectionMismatch", eventidProjectionMismatch, FireProjectionMismatch, VTS_I4 VTS_PI4 VTS_PI4)
	EVENT_CUSTOM_ID("LayerReprojected", eventidLayerReprojected, FireLayerReprojected, VTS_I4 VTS_BOOL)
	EVENT_CUSTOM_ID("LayerAdded", eventidLayerAdded, FireLayerAdded, VTS_I4)
	EVENT_CUSTOM_ID("LayerRemoved", eventidLayerRemoved, FireLayerRemoved, VTS_I4 VTS_BOOL)
	EVENT_CUSTOM_ID("BackgroundLoadingStarted", eventidBackgroundLoadingStarted, FireBackgroundLoadingStarted, VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("BackgroundLoadingFinished", eventidBackgroundLoadingFinished, FireBackgroundLoadingFinished, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("GridOpened", eventidGridOpened, FireGridOpened, VTS_I4 VTS_BSTR VTS_I4 VTS_BOOL)
	EVENT_CUSTOM_ID("OnDrawBackBuffer2", eventidOnDrawBackBuffer2, FireOnDrawBackBuffer2, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("BeforeLayers", eventidBeforeLayers, FireBeforeLayers, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("AfterLayers", eventidAfterLayers, FireAfterLayers, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("LayerReprojectedIncomplete", eventidLayerReprojectedIncomplete, FireLayerReprojectedIncomplete, VTS_I4 VTS_I4 VTS_I4)
	EVENT_CUSTOM_ID("BeforeVertexDigitized", eventidBeforeVertexDigitized, FireBeforeVertexDigitized, VTS_PR8 VTS_PR8)
	EVENT_CUSTOM_ID("SnapPointRequested", eventidSnapPointRequested, FireSnapPointRequested, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI4)
	EVENT_CUSTOM_ID("SnapPointFound", eventidSnapPointFound, FireSnapPointFound, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	EVENT_STOCK_DBLCLICK()
	//}}AFX_EVENT_MAP

	// Is never fired: EVENT_CUSTOM_ID("ShapesIdentified", eventidShapesIdentified, FireShapesIdentified, VTS_DISPATCH VTS_R8 VTS_R8)
END_EVENT_MAP()

#pragma region Constructor/destructor

// ********************************************************************
//		CMapView() constructor
// ********************************************************************
CMapView::CMapView()
	: _vals("AZ0CY1EX2GV3IT4KR5MP6ON7QL8SJ9UH0WF1DB2"),
	_valsLen(39), _isSnapshot(false),
	_brushBlue(NULL),
	_brushBlack(NULL),
	_brushWhite(NULL),
	_brushLightGray(NULL),
	_brushGray(NULL),
	_penGray(NULL),
	_penDarkGray(NULL),
	_propertyExchange(NULL),
	_bufferBitmap(NULL),
	_tilesBitmap(NULL),
	_layerBitmap(NULL),
	_drawingBitmap(NULL),
	_tempBitmap(NULL),
	_moveBitmap(NULL),
	_volatileBitmap(NULL)
{
	// GDI Plus Startup
	GdiplusStartup();

	Startup();
	SetDefaults();
}

// ****************************************************
//	    Destructor
// ****************************************************
CMapView::~CMapView()
{
	LockWindow(lmLock);   // we don't need any more redraws

	this->RemoveAllLayers();

	this->ClearDrawings();

	ReleaseTempObjects();

	this->Shutdown();

	// GDI Plus Shutdown
	// TileCacheManager::CloseAll is now by GdiplusShutdown,
	// managed within the reference count
	GdiplusShutdown();
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

	CComPtr<IGeoProjection> p = NULL;
	ComHelper::CreateInstance(idGeoProjection, (IDispatch**)&p);
	SetGeoProjection(p);

	Redraw();
}

// **********************************************************************
//	Startup
// **********************************************************************
// Must be called from constructor only
void CMapView::Startup()
{
	InitializeIIDs(&IID_DMap, &IID_DMapEvents);

	_brushBlue = new Gdiplus::SolidBrush(Gdiplus::Color::Blue);
	_brushBlack = new Gdiplus::SolidBrush(Gdiplus::Color::Black);
	_brushWhite = new Gdiplus::SolidBrush(Gdiplus::Color::White);
	_brushLightGray = new Gdiplus::SolidBrush(Gdiplus::Color::LightGray);
	_brushGray = new Gdiplus::SolidBrush(Gdiplus::Color::Gray);
	_penGray = new Gdiplus::Pen(Gdiplus::Color::Gray);
	_penDarkGray = new Gdiplus::Pen(Gdiplus::Color::DarkSlateGray);

	Utility::InitGdiPlusFont(&_fontCourier, L"Courier New", 9.0f);
	Utility::InitGdiPlusFont(&_fontArial, L"Arial", 9.0f);
	_fontCourierSmall = new Gdiplus::Font(L"Courier New", 8.0f);
	_fontCourierLink = new Gdiplus::Font(L"Courier New", 8.0f);

	m_mapCursor = crsrMapDefault;
	_interactiveLayerHandle = -1;			// TODO: remove (currently not used)
	_panningAnimation = false;
	_zoombarTargetZoom = -1;
	_dragging.Move = CPoint(0, 0);
	_dragging.Start = CPoint(0, 0);
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
	_cursorDigitize = AfxGetApp()->LoadCursor(IDC_DIGITIZE_CURSOR);
	_cursorVertex = AfxGetApp()->LoadCursor(IDC_VERTEX_CURSOR);
	_cursorRotate = AfxGetApp()->LoadCursor(IDC_ROTATE_CURSOR);
	_cursorMove = AfxGetApp()->LoadCursor(IDC_MOVE_CURSOR);
	_cursorSelect2 = AfxGetApp()->LoadCursor(IDC_SELECT2_CURSOR);
	_cursorIdentify = AfxGetApp()->LoadCursor(IDC_IDENTIFY_CURSOR);
	_cursorAlternatePan = AfxGetApp()->LoadCursor(IDC_PAN_ALTERNATE);

	_udCursor = NULL;

	_ttipCtrl = new CButton();
	m_drawMutex.Unlock();
	_mapstateMutex.Unlock();
	_setmaxstdio(2048);
	srand(time(NULL));

	_imageGroups = NULL;
	_hotTracking.Shapefile = NULL;
	_rotate = NULL;

	ComHelper::CreateInstance(idTiles, (IDispatch**)&_tiles);
	((CTiles*)_tiles)->Init(this);

	ComHelper::CreateInstance(idMeasuring, (IDispatch**)&_measuring);
	ComHelper::CreateInstance(idShapeEditor, (IDispatch**)&_shapeEditor);
	ComHelper::CreateInstance(idShapeEditor, (IDispatch**)&_geodesicShape);

	ComHelper::CreateInstance(idIdentifier, (IDispatch**)&_identifier);
	ComHelper::CreateInstance(idFileManager, (IDispatch**)&_fileManager);
	ComHelper::CreateInstance(idUndoList, (IDispatch**)&_undoList);
	((CUndoList*)_undoList)->SetMapCallback(this);

	ComHelper::CreateInstance(idSelectionList, (IDispatch**)&_identifiedShapes);
	ComHelper::CreateInstance(idShapefile, (IDispatch**)&_identifiedShapefile);
	ComHelper::CreateInstance(idDrawingRectangle, (IDispatch**)&_focusRectangle);
	_focusRectangle->put_Visible(VARIANT_TRUE);

	InitProjections();

	// let them all work by default 
	_customDrawingFlags = (tkCustomDrawingFlags)(BeforeAfterDrawing | BeforeAfterLayers | OnDrawBackBufferBitmapData | OnDrawBackBufferHdc);

	GetMeasuringBase()->SetMapCallback(this, ShapeInputMode::simMeasuring);
	_shapeEditor->SetMapCallback(this);
	_geodesicShape->SetMapCallback(this);

	// initialize cURL
	curl_global_init(CURL_GLOBAL_ALL);
}

// **********************************************************************
//	SetDefaults
// **********************************************************************
void CMapView::SetDefaults()
{
	// temp state variables
	_mouseTracking = false;
	_copyrightLinkActive = FALSE;
	_copyrightRect = Gdiplus::RectF(0.0F, 0.0F, 0.0F, 0.0F);
	_shapeCountInView = 0;
	_currentDrawing = -1;
	_rectTrackerIsActive = false;
	_lastWidthMeters = 0.0;
	_redrawId = 0;
	_lastZooombarPart = ZoombarNone;
	_hotTracking.LayerHandle = -1;
	_hotTracking.ShapeIndex = -1;
	_isSizing = false;
	_showingToolTip = FALSE;
	_lockCount = 0;
	_canUseLayerBuffer = false;
	_canUseVolatileBuffer = false;
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
	_extentHistoryCount = 20;
	m_doubleBuffer = TRUE;
	m_zoomPercent = 0.3;
	m_cursorMode = tkCursorMode::cmZoomIn;
	m_backColor = RGB(255, 255, 255);
	m_extentPad = 0.02;
	_rotateAngle = 0.0f;
	_canUseImageGrouping = FALSE;
	_grabProjectionFromData = TRUE;
	_hasHotTracking = false;
	_showCoordinates = cdmAuto;
	_zoomBehavior = zbUseTileLevels;
	_scalebarVisible = TRUE;
	_zoombarVisible = TRUE;
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
	_showRedrawTime = FALSE;
	_showVersionNumber = FALSE;
	_scalebarUnits = tkScalebarUnits::GoogleStyle;
	_zoomBarVerbosity = tkZoomBarVerbosity::zbvFull;
	_panningInertia = csFalse;
	_reuseTileBuffer = TRUE;
	_zoomAnimation = csAuto;
	_zoomBoxStyle = tkZoomBoxStyle::zbsBlue;
	_identifierMode = imAllLayers;
	_zoomBarMinZoom = -1;
	_zoomBarMaxZoom = -1;
	_showCoordinatesFormat = afDegrees;
	_panningExtentsChanged = false;
	_prevExtentsIndex = 0;
	_useAlternatePanCursor = FALSE;
	_recenterMapOnZoom = FALSE;
	_showCoordinatesBackground = FALSE;

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

	_dragging.CloseShapefile();

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
	// clean up cURL
	curl_global_cleanup();

	Utility::ClosePointer(&_fontCourier);
	Utility::ClosePointer(&_fontCourierSmall);
	Utility::ClosePointer(&_fontCourierLink);
	Utility::ClosePointer(&_fontArial);

	((CTiles*)_tiles)->Stop();

	if (_globalCallback)
		_globalCallback->Release();

	Utility::ClosePointer(&_bufferBitmap);
	Utility::ClosePointer(&_tilesBitmap);
	Utility::ClosePointer(&_drawingBitmap);
	Utility::ClosePointer(&_volatileBitmap);
	Utility::ClosePointer(&_moveBitmap);
	Utility::ClosePointer(&_tempBitmap);
	Utility::ClosePointer(&_layerBitmap);

	ClearPanningList();

	ReleaseProjections();

	if (_identifier)
		_identifier->Release();

	if (_fileManager)
		_fileManager->Release();

	if (_measuring)
		_measuring->Release();

	if (_shapeEditor)
		_shapeEditor->Release();

	if (_geodesicShape)
		_geodesicShape->Release();

	if (_undoList)
		_undoList->Release();

	if (_identifiedShapes)
		_identifiedShapes->Release();

	if (_identifiedShapefile)
		_identifiedShapefile->Release();

	if (_focusRectangle)
		_focusRectangle->Release();

	if (_tiles)
	{
		((CTiles*)_tiles)->ClearAll();
		_tiles->Release();
	}

	delete _ttipCtrl;
}

// ********************************************************************
//		OnCreate()
// ********************************************************************
int CMapView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;



	DragAcceptFiles(TRUE);

	CRect rect(0, 0, 0, 0);
	_ttipCtrl->Create(NULL, WS_CHILD, rect, this, IDC_TTBTN);
	_ttipCtrl->ShowWindow(FALSE);

	//CTool Tip
	_ttip.Create(this, TTS_ALWAYSTIP);
	_ttip.Activate(TRUE);
	_ttip.AddTool(this, "", rect, IDC_TTBTN);
	_ttip.SetDelayTime(TTDT_AUTOPOP, 0);
	_ttip.SetDelayTime(TTDT_INITIAL, 0);
	_ttip.SetDelayTime(TTDT_RESHOW, 0);

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
	return MAKELONG(49, 12);
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

		PX_Color(pPX, "BackColor", m_backColor, RGB(255, 255, 255));
		PX_Double(pPX, "ExtentPad", m_extentPad, .02);
		PX_Short(pPX, "CursorMode", m_cursorMode, 0);
		PX_Double(pPX, "ZoomPercent", m_zoomPercent, .3);
		PX_Bool(pPX, "SendMouseMove", m_sendMouseMove, FALSE);
		PX_Bool(pPX, "SendMouseDown", m_sendMouseDown, FALSE);
		PX_Bool(pPX, "SendMouseUp", m_sendMouseUp, FALSE);
		PX_Bool(pPX, "SendSelectBoxDrag", m_sendSelectBoxDrag, FALSE);
		PX_Bool(pPX, "SendSelectBoxFinal", m_sendSelectBoxFinal, FALSE);
		PX_Long(pPX, "ExtentHistory", _extentHistoryCount, 20);
		PX_Bool(pPX, "DoubleBuffer", m_doubleBuffer, TRUE);
		PX_Bool(pPX, "SendOnDrawBackBuffer", m_sendOnDrawBackBuffer, FALSE);
		PX_Bool(pPX, "ShowRedrawTime", _showRedrawTime, FALSE);
		PX_Bool(pPX, "ShowVersionNumber", _showVersionNumber, FALSE);
		PX_Double(pPX, "MouseWheelSpeed", _mouseWheelSpeed, .5);
		PX_Bool(pPX, "UseSeamlessPan", _useSeamlessPan, FALSE);
		PX_Bool(pPX, "MultilineLabels", _multilineLabeling, FALSE);
		PX_Bool(pPX, "TrapRMouseDown", _doTrapRMouseDown, TRUE);
		PX_Bool(pPX, "DisableWaitCursor", _disableWaitCursor, FALSE);
		PX_Float(pPX, "MapRotationAngle", _rotateAngle, 0.0);
		PX_Bool(pPX, "CanUseImageGrouping", _canUseImageGrouping, FALSE);
		PX_Bool(pPX, "ScalebarVisible", _scalebarVisible, TRUE);

		// enumerated constants aren't supported directly so temp buffer will be used
		// we don't care about the direction of exchange, doing both getting and setting
		long temp;
		temp = (long)_mapResizeBehavior;
		PX_Long(pPX, "MapResizeBehavior", temp, 0);	//rbClassic
		_mapResizeBehavior = (tkResizeBehavior)temp;

		temp = (long)_zoomBehavior;
		PX_Long(pPX, "ZoomBehavior", temp, 1);	// zbUseTileLevels
		_zoomBehavior = (tkZoomBehavior)temp;

		temp = (long)_scalebarUnits;
		PX_Long(pPX, "ScalebarUnits", temp, 2);	//suGoogleStyle
		_scalebarUnits = (tkScalebarUnits)temp;

		temp = (long)_shapeDrawingMethod;
		PX_Long(pPX, "ShapeDrawingMethod", temp, 3);	// dmNewSymbology
		_shapeDrawingMethod = (tkShapeDrawingMethod)temp;

		temp = (long)_unitsOfMeasure;
		PX_Long(pPX, "MapUnits", temp, 6);			//umMeters
		_unitsOfMeasure = (tkUnitsOfMeasure)temp;

		temp = (long)_showCoordinates;
		PX_Long(pPX, "ShowCoordinates", temp, 1);			//cdmAuto
		_showCoordinates = (tkCoordinatesDisplay)temp;

		temp = (long)GetTileProvider();
		PX_Long(pPX, "TileProvider", temp, 0);		// OpenStreetMap
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
			PX_Long(pPX, "Projection", temp, 0);			// PROJECTION_NONE
			projection = (tkMapProjection)temp;
			if (loading) {
				SetProjection(projection);
			}

			Extent extents;
			PX_Double(pPX, "xMin", extents.left, .3);
			PX_Double(pPX, "xMax", extents.right, .3);
			PX_Double(pPX, "yMin", extents.bottom, .3);
			PX_Double(pPX, "yMax", extents.top, .3);

			if (loading) {
				SetExtentsCore(extents, false);
			}
		}
		catch (...) {
		}

		if (loading) {
			this->LockWindow(lmUnlock);
		}

		PX_Bool(pPX, "ShowZoomBar", _zoombarVisible, TRUE);
		PX_Bool(pPX, "GrabProjectionFromData", _grabProjectionFromData, TRUE);

		temp = (long)_zoomAnimation;
		PX_Long(pPX, "AnimationOnZooming", temp, 0);			// csAuto
		_zoomAnimation = (tkCustomState)temp;

		temp = (long)_panningInertia;
		PX_Long(pPX, "InertiaOnPanning", temp, 0);			// csAuto
		_panningInertia = (tkCustomState)temp;

		PX_Bool(pPX, "ReuseTileBuffer", _reuseTileBuffer, TRUE);

		temp = (long)_zoomBarVerbosity;
		PX_Long(pPX, "ZoomBarVerbosity", temp, 1);			// zbvFull
		_zoomBarVerbosity = (tkZoomBarVerbosity)temp;

		temp = (long)_zoomBoxStyle;
		PX_Long(pPX, "ZoomBoxStyle", temp, 4);			// zbsBlue
		_zoomBoxStyle = (tkZoomBoxStyle)temp;

		PX_Long(pPX, "ZoombarMinZoom", _zoomBarMinZoom, -1);
		PX_Long(pPX, "ZoombarMaxZoom", _zoomBarMaxZoom, -1);

		temp = (long)_showCoordinatesFormat;
		PX_Long(pPX, "ShowCoordinatesFormat", temp, 0);			// afDegrees
		_showCoordinatesFormat = (tkAngleFormat)temp;

		PX_Bool(pPX, "UseAlternatePanCursor", _useAlternatePanCursor, FALSE);

		PX_Bool(pPX, "RecenterMapOnZoom", _recenterMapOnZoom, FALSE);

		PX_Bool(pPX, "ShowCoordinatesBackground", _showCoordinatesBackground, FALSE);
	}
	catch (...)
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

   // IMPORTANT: the behavior is changed on 13 oct 14, startup/shutdown is moved back to
   // App::InitInstaance/App::ExitInstance because of issues with tile cache, which uses
   // GdiPlus::Bitmaps. It's desirable to keep it open even when the last CMapView contol
   // is destructed. Solution described at http://mikevdm.com/BlogEntry/Key/GdiplusShutdown-Hangs-Mysteriously
   // was implemented. Hopefully it will work.
   //
   // FOLLOW-UP: Having startup/shutdown being called in InitInstance/ExitInstance was not 
   // working properly from all development environments, and has been moved back to the CMapView
   // constructor/destructor.  The above-mentioned Tile cache issue is being handled by calling 
   // TileCacheManager::CloseAll() just prior to calling GdiplusShutdown.  This better follows
   // Microsoft's recommendations, and allows for transparent initialization whether or not the 
   // calling environment does it's own statrup and shutdown.  If any individual classes end up 
   // requiring GDI initialization outside of the map initialization, these can be handled on a 
   // case-by-case basis. (jf, 11/27/2019, https://mapwindow.atlassian.net/browse/MWGIS-205)

ULONG_PTR CMapView::ms_gdiplusToken = NULL;
ULONG_PTR CMapView::ms_gdiplusBGThreadToken = NULL;
unsigned CMapView::ms_gdiplusCount = 0;
::CCriticalSection CMapView::ms_gdiplusLock;
Gdiplus::GdiplusStartupOutput CMapView::ms_gdiplusStartupOutput;

void CMapView::GdiplusStartup()
{
	CSingleLock l(&ms_gdiplusLock, TRUE);
	if (ms_gdiplusCount == 0)
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		gdiplusStartupInput.SuppressBackgroundThread = TRUE;
		Gdiplus::Status s = Gdiplus::GdiplusStartup(&ms_gdiplusToken, &gdiplusStartupInput, &ms_gdiplusStartupOutput);
		if (s != Gdiplus::Ok)
		{
			TRACE(_T("GdiplusStartup failed, error: %d, GetLastError = %d\n"), s, GetLastError());
			ASSERT(FALSE);
			AfxThrowResourceException();
		}
		else {
			ms_gdiplusStartupOutput.NotificationHook(&ms_gdiplusBGThreadToken);
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
		TileCacheManager::CloseAll();
		ms_gdiplusStartupOutput.NotificationUnhook(ms_gdiplusBGThreadToken);
		Gdiplus::GdiplusShutdown(ms_gdiplusToken);
		ms_gdiplusToken = NULL;
	}
}
#pragma endregion


// ***************************************************************
//	ClearPanningList
// ***************************************************************
void CMapView::ClearPanningList()
{
	if (_panningInertia != csFalse)
	{
		_panningLock.Lock();
		for (int i = 0; i < _panningList.size(); i++)
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
//	GetShapeEditorBase
// ***************************************************************
EditorBase* CMapView::GetEditorBase()
{
	return _shapeEditor->GetActiveShape();
}

// ***************************************************************
//	GetACtiveShape
// ***************************************************************
ActiveShape* CMapView::GetActiveShape()
{
	if (m_cursorMode == cmMeasure) return GetMeasuringBase();
	return GetEditorBase();
}

// ***************************************************************
//	GetLayerVisibleAtCurrentScale
// ***************************************************************
VARIANT_BOOL CMapView::GetLayerVisibleAtCurrentScale(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!IsValidLayer(layerHandle)) return VARIANT_FALSE;
	Layer* l = _allLayers[layerHandle];
	if (IS_VALID_PTR(l))
	{
		double scale = GetCurrentScale();
		int zoom = GetCurrentZoom();
		return l->IsVisible(scale, zoom) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	return VARIANT_FALSE;
}

// ***************************************************************
//	ZoomToEditor
// ***************************************************************
void CMapView::ZoomToEditor()
{
	CComPtr<IShape> shp = NULL;
	_shapeEditor->get_RawData(&shp);
	if (shp) {
		CComPtr<IExtents> ext = NULL;
		shp->get_Extents(&ext);
		if (ext) {
			double xMin, xMax, yMin, yMax, zMin, zMax;
			ext->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
			Extent temp(xMin, xMax, yMin, yMax);
			if (_extents.Intersects(temp)) return;

			CComPtr<IPoint> pnt = NULL;
			ext->get_Center(&pnt);
			if (pnt) {
				double x, y;
				pnt->get_X(&x);
				pnt->get_Y(&y);
				double ratio = 2.0;   // increase the size
				double w = _extents.Width() * ratio;
				double h = _extents.Height() * ratio;
				Extent newExt(x - w / 2.0, x + w / 2.0, y - h / 2.0, y + h / 2.0);
				SetExtentsWithPadding(newExt);
			}
		}
	}
}

// ***************************************************************
//	StartDragging
// ***************************************************************
void CMapView::StartDragging(DraggingOperation operation)
{
	_dragging.Operation = operation;
	SetCapture();
}

// ************************************************************
//		GetIdentifiedShapes
// ************************************************************
ISelectionList* CMapView::GetIdentifiedShapes()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_identifiedShapes) {
		_identifiedShapes->AddRef();
	}
	return _identifiedShapes;
}

// ************************************************************
//		GetFocusRectangle
// ************************************************************
IDrawingRectangle* CMapView::GetFocusRectangle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_focusRectangle) {
		_focusRectangle->AddRef();
	}
	return _focusRectangle;
}

