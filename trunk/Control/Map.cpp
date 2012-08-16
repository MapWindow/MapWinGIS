//********************************************************************************************************
//File name: Map.cpp
//Description: Implementation of the CMapView ActiveX Control class.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//3-28-2005 dpa - Updated with label rotation and line stipples and compile modifications for VS2003
//6-8-2005 ah - Removed check for serial number in add layer function
//1-22-2006 cdm -- Implemented the resizebehavior discussion at http://www.mapwindow.org/phorum/read.php?3,2152,2199#msg-2199
//2-7-2006 cdm -- Implemented polygon transparency with code provided by neztypezero of the MapWindow forums
//3-16-2006 Chris Michaelis (cdm) -- Added the ability to use multiple (differing) icons 
//			in a single layer through the new functions set_ShapePointImageListID and 
//			set_UDPointImageListAdd; also the new tkPointType enum value "ptImageList".
//			Also a few functions to get the count of images and get an image, etc. The functions have "set_" in
//			them to try to make the functions appear next to the properties in VB intellisense.
//9-30-2006 Stan 
//			-- Implemented possibility of adding labels to Drawings
//			-- Passing parameters as reference - several - to avoid not necessary object creation (time!)
//4-07-2007 Tom Shanley (tws) - support for large-scale image exports: added SnapShot2(), tweaked DrawImage()
//6-24-2007 Tom Shanley (tws) - support for tkHJustification=hjNone (draw label AT the indicated point)
//11-7-2008 Andrew Purkis (ajp) - added support for multiple font characters in a point shape layer, works
//          similar to ptImageList (thanks Chris!). Functions to set font for a specific layer (try Webdings
//          or WingDings), then to add a list of seperate font character values (integer) with associated
//          shape index, also added a seperate function to set the Font Size, so it can be scaled 
//          as you zoom in or out. Added the new tkPointType enum value "ptFontChar".
//          functions - set_UDPointFontCharFont, SetShapePointFontCharListID, GetShapePointFontCharListID,
//                      set_UDPointFontCharListAdd adds font char and associated font color,
//                      set_UDPointFontCharFontSize.
//3-16-2009 Ray Quay - Added ability to turn off trapping of right mouse click for zoom, and property
//          to turn trapping on and off.  This allows right click context menus without zoom.
//          Added property TrapRMouseDown:BOOL Functions SetTrapRMouseDown, GetTrapRMouseDown
//5-15-2009 Andrew Purkis (ajp) - added new enumeration to tkHJustification = hjRaw. 
//					Rotating labels makes the bounding rectangle much larger, if you use DT_BOTTOM
//					the point gets offset more and more as you rotate towards 45deg, by using DT_TOP
//					the problem disappears. So as not to break others code, I added hjRaw, and only
//					applied the DT_TOP to that justification. Applies to DrawScaledLabels and DrawUnscaledLabels
//07-03-2009 Sergei Leschinski (lsu) - 
//			1. New ResizeBehavior added: tkResizeBehavior = rbKeepScale (see http://www.mapwindow.org/phorum/read.php?5,13081). 
//			Changes to CalculateVisibleExtents function. Whole new extent will be shown in spite of resize behavior.
//			2. Changes to DrawShapefile function. Correct colors for shape's fill and lines when points are drawn. 
//			3. UseSeamlessPanning property added (suggested by Bobby, see http://www.mapwindow.org/phorum/read.php?3,13099). Changes in OnMouseMove exent.
//			4. Cached extents are used in DrawShapefile function in edit mode (see http://www.mapwindow.org/phorum/read.php?5,13065).
//07-29-2009 ÷‹¥œª‘ (Neio) - 
//          1. Add Quad-Tree Support in edit mode
//          2. Fix the caculation error if exists empty layer(s).
//          3. Fix the problem when using Chinese path, following by http://www.mapwindow.org/phorum/read.php?7,12162 by gischai
//08-24-2009 (sm) Fixes to performance issues with the spatial index trees as per http://www.mapwindow.org/phorum/read.php?5,13738
//27 aug 2009 lsu Modified DrawShapefile function. Support for bitmap fill patterns.
//02 aug 2012 Brad Hester - DC is released when the map is resized.
//          
//********************************************************************************************************
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"

#include "xtiffio.h"  /* for TIFF */
#include "geotiffio.h" /* for GeoTIFF */
#include "tiff.h"
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"
#include "tiffiop.h"
#include <fstream>
#include <vector>
#include <atlsafe.h>
#include "IndexSearching.h"

#include "MapPpg.h"
#include "Enumerations.h"

#include "LabelCategory.h"
#include "Labels.h"
#include "Image.h"

#include "ShapefileDrawing.h"
#include "ImageDrawing.h"
#include "LabelDrawing.h"
#include "ChartDrawing.h"
#include "GridColorScheme.h"

//#include "UtilityFunctions.h"
#include "Projections.h"
#include "gdal.h"

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


#pragma region REGION ControlInfo

IMPLEMENT_DYNCREATE(CMapView, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

int WM_SPACEBALL = RegisterWindowMessage("SpaceWareMessage00");
BEGIN_MESSAGE_MAP(CMapView, COleControl)
	//{{AFX_MSG_MAP(CMapView)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	//// No spaceballs in 64-bit mode
	//#ifndef WIN64
	//ON_REGISTERED_MESSAGE( WM_SPACEBALL, OnSpaceball )
	//#endif
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CMapView, COleControl)
	//{{AFX_DISPATCH_MAP(CMapView)
	// NOTE - ClassWizard will add and remove dispatch map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	DISP_PROPERTY_NOTIFY(CMapView, "BackColor", m_backColor, OnBackColorChanged, VT_COLOR)
	DISP_PROPERTY_NOTIFY(CMapView, "ZoomPercent", m_zoomPercent, OnZoomPercentChanged, VT_R8)
	DISP_PROPERTY_NOTIFY(CMapView, "CursorMode", m_cursorMode, OnCursorModeChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CMapView, "MapCursor", m_mapCursor, OnMapCursorChanged, VT_I2)
	DISP_PROPERTY_NOTIFY(CMapView, "UDCursorHandle", m_uDCursorHandle, OnUDCursorHandleChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CMapView, "SendMouseDown", m_sendMouseDown, OnSendMouseDownChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMapView, "SendMouseUp", m_sendMouseUp, OnSendMouseUpChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMapView, "SendMouseMove", m_sendMouseMove, OnSendMouseMoveChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMapView, "SendSelectBoxDrag", m_sendSelectBoxDrag, OnSendSelectBoxDragChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMapView, "SendSelectBoxFinal", m_sendSelectBoxFinal, OnSendSelectBoxFinalChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY(CMapView, "ExtentPad", m_extentPad, OnExtentPadChanged, VT_R8)
	DISP_PROPERTY_NOTIFY(CMapView, "ExtentHistory", m_extentHistory, OnExtentHistoryChanged, VT_I4)
	DISP_PROPERTY_NOTIFY(CMapView, "Key", m_key, OnKeyChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY(CMapView, "DoubleBuffer", m_doubleBuffer, OnDoubleBufferChanged, VT_BOOL)
	DISP_PROPERTY_EX(CMapView, "GlobalCallback", GetGlobalCallback, SetGlobalCallback, VT_DISPATCH)
	DISP_PROPERTY_EX(CMapView, "NumLayers", GetNumLayers, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CMapView, "Extents", GetExtents, SetExtents, VT_DISPATCH)
	DISP_PROPERTY_EX(CMapView, "LastErrorCode", GetLastErrorCode, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CMapView, "IsLocked", GetIsLocked, SetNotSupported, VT_I2)
	DISP_PROPERTY_EX(CMapView, "MapState", GetMapState, SetMapState, VT_BSTR)
	DISP_FUNCTION(CMapView, "Redraw", Redraw, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMapView, "AddLayer", AddLayer, VT_I4, VTS_DISPATCH VTS_BOOL)
	DISP_FUNCTION(CMapView, "RemoveLayer", RemoveLayer, VT_EMPTY, VTS_I4)
	
	DISP_FUNCTION(CMapView, "RemoveAllLayers", RemoveAllLayers, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMapView, "MoveLayerUp", MoveLayerUp, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CMapView, "MoveLayerDown", MoveLayerDown, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CMapView, "MoveLayer", MoveLayer, VT_BOOL, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMapView, "MoveLayerTop", MoveLayerTop, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CMapView, "MoveLayerBottom", MoveLayerBottom, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CMapView, "ZoomToMaxExtents", ZoomToMaxExtents, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMapView, "ZoomToLayer", ZoomToLayer, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMapView, "ZoomToShape", ZoomToShape, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMapView, "ZoomIn", ZoomIn, VT_EMPTY, VTS_R8)
	DISP_FUNCTION(CMapView, "ZoomOut", ZoomOut, VT_EMPTY, VTS_R8)
	DISP_FUNCTION(CMapView, "ZoomToPrev", ZoomToPrev, VT_I4, VTS_NONE)
	DISP_FUNCTION(CMapView, "ProjToPixel", ProjToPixel, VT_EMPTY, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION(CMapView, "PixelToProj", PixelToProj, VT_EMPTY, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION(CMapView, "ClearDrawing", ClearDrawing, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMapView, "ClearDrawings", ClearDrawings, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMapView, "SnapShot", SnapShot, VT_DISPATCH, VTS_DISPATCH)
	DISP_FUNCTION(CMapView, "ApplyLegendColors", ApplyLegendColors, VT_BOOL, VTS_DISPATCH)
	DISP_FUNCTION(CMapView, "LockWindow", LockWindow, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CMapView, "Resize", Resize, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMapView, "ShowToolTip", ShowToolTip, VT_EMPTY, VTS_BSTR VTS_I4)
	DISP_FUNCTION(CMapView, "AddLabel", AddLabel, VT_EMPTY, VTS_I4 VTS_BSTR VTS_COLOR VTS_R8 VTS_R8 VTS_I2)
	DISP_FUNCTION(CMapView, "ClearLabels", ClearLabels, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMapView, "LayerFont", LayerFont, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION(CMapView, "GetColorScheme", GetColorScheme, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CMapView, "NewDrawing", NewDrawing, VT_I4, VTS_I2)
	DISP_FUNCTION(CMapView, "DrawPoint", DrawPoint, VT_EMPTY, VTS_R8 VTS_R8 VTS_I4 VTS_COLOR)
	DISP_FUNCTION(CMapView, "DrawLine", DrawLine, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_COLOR)
	DISP_FUNCTION(CMapView, "DrawCircle", DrawCircle, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_COLOR VTS_BOOL)
	DISP_FUNCTION(CMapView, "DrawPolygon", DrawPolygon, VT_EMPTY, VTS_VARIANT VTS_VARIANT VTS_I4 VTS_COLOR VTS_BOOL)
	DISP_PROPERTY_PARAM(CMapView, "LayerKey", GetLayerKey, SetLayerKey, VT_BSTR, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerPosition", GetLayerPosition, SetNotSupported, VT_I4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerHandle", GetLayerHandle, SetNotSupported, VT_I4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerVisible", GetLayerVisible, SetLayerVisible, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerFillColor", GetShapeLayerFillColor, SetShapeLayerFillColor, VT_COLOR, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeFillColor", GetShapeFillColor, SetShapeFillColor, VT_COLOR, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerLineColor", GetShapeLayerLineColor, SetShapeLayerLineColor, VT_COLOR, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLineColor", GetShapeLineColor, SetShapeLineColor, VT_COLOR, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerPointColor", GetShapeLayerPointColor, SetShapeLayerPointColor, VT_COLOR, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapePointColor", GetShapePointColor, SetShapePointColor, VT_COLOR, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerDrawFill", GetShapeLayerDrawFill, SetShapeLayerDrawFill, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeDrawFill", GetShapeDrawFill, SetShapeDrawFill, VT_BOOL, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerDrawLine", GetShapeLayerDrawLine, SetShapeLayerDrawLine, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeDrawLine", GetShapeDrawLine, SetShapeDrawLine, VT_BOOL, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerDrawPoint", GetShapeLayerDrawPoint, SetShapeLayerDrawPoint, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeDrawPoint", GetShapeDrawPoint, SetShapeDrawPoint, VT_BOOL, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerLineWidth", GetShapeLayerLineWidth, SetShapeLayerLineWidth, VT_R4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLineWidth", GetShapeLineWidth, SetShapeLineWidth, VT_R4, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerPointSize", GetShapeLayerPointSize, SetShapeLayerPointSize, VT_R4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapePointSize", GetShapePointSize, SetShapePointSize, VT_R4, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerFillTransparency", GetShapeLayerFillTransparency, SetShapeLayerFillTransparency, VT_R4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeFillTransparency", GetShapeFillTransparency, SetShapeFillTransparency, VT_R4, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerLineStipple", GetShapeLayerLineStipple, SetShapeLayerLineStipple, VT_I2, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLineStipple", GetShapeLineStipple, SetShapeLineStipple, VT_I2, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerFillStipple", GetShapeLayerFillStipple, SetShapeLayerFillStipple, VT_I2, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeFillStipple", GetShapeFillStipple, SetShapeFillStipple, VT_I2, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeVisible", GetShapeVisible, SetShapeVisible, VT_BOOL, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ImageLayerPercentTransparent", GetImageLayerPercentTransparent, SetImageLayerPercentTransparent, VT_R4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ErrorMsg", GetErrorMsg, SetNotSupported, VT_BSTR, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "DrawingKey", GetDrawingKey, SetDrawingKey, VT_BSTR, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapeLayerPointType", GetShapeLayerPointType, SetShapeLayerPointType, VT_I2, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "ShapePointType", GetShapePointType, SetShapePointType, VT_I2, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerLabelsVisible", GetLayerLabelsVisible, SetLayerLabelsVisible, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "UDLineStipple", GetUDLineStipple, SetUDLineStipple, VT_I4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "UDFillStipple", GetUDFillStipple, SetUDFillStipple, VT_I4, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "UDPointType", GetUDPointType, SetUDPointType, VT_DISPATCH, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "GetObject", GetGetObject, SetNotSupported, VT_DISPATCH, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerName", dispidLayerName, GetLayerName, SetLayerName, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "SetImageLayerColorScheme", dispidSetImageLayerColorScheme, SetImageLayerColorScheme, VT_BOOL, VTS_I4 VTS_DISPATCH)
	DISP_PROPERTY_PARAM_ID(CMapView, "GridFileName", dispidGridFileName, GetGridFileName, SetGridFileName, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "UpdateImage", dispidUpdateImage, UpdateImage, VT_EMPTY, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "SerialNumber", dispidSerialNumber, GetSerialNumber, SetSerialNumber, VT_BSTR)
	DISP_PROPERTY_EX_ID(CMapView, "LineSeparationFactor", dispidLineSeparationFactor, GetLineSeparationFactor, SetLineSeparationFactor, VT_UI1)
	DISP_PROPERTY_PARAM(CMapView, "LayerLabelsShadow", GetLayerLabelsShadow, SetLayerLabelsShadow, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerLabelsScale", GetLayerLabelsScale, SetLayerLabelsScale, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CMapView, "AddLabelEx", AddLabelEx, VT_EMPTY, VTS_I4 VTS_BSTR VTS_COLOR VTS_R8 VTS_R8 VTS_I2 VTS_R8)
	DISP_FUNCTION(CMapView, "GetLayerStandardViewWidth", GetLayerStandardViewWidth, VT_EMPTY, VTS_I4 VTS_PR8)
	DISP_FUNCTION(CMapView, "SetLayerStandardViewWidth", SetLayerStandardViewWidth, VT_EMPTY, VTS_I4 VTS_R8)
	DISP_PROPERTY_PARAM(CMapView, "LayerLabelsOffset", GetLayerLabelsOffset, SetLayerLabelsOffset, VT_I4, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerLabelsShadowColor", GetLayerLabelsShadowColor, SetLayerLabelsShadowColor, VT_COLOR, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "UseLabelCollision", GetUseLabelCollision, SetUseLabelCollision, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CMapView, "IsTIFFGrid", IsTIFFGrid, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION(CMapView, "IsSameProjection", IsSameProjection, VT_BOOL, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CMapView, "ZoomToMaxVisibleExtents", ZoomToMaxVisibleExtents, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CMapView, "MapResizeBehavior", GetMapResizeBehavior, SetMapResizeBehavior, VT_I2)
	DISP_PROPERTY_EX(CMapView, "HWnd", HWnd, SetNotSupported, VT_I4)	
	DISP_FUNCTION(CMapView, "set_UDPointImageListAdd", set_UDPointImageListAdd, VT_I4, VTS_I4 VTS_DISPATCH)
	DISP_PROPERTY_PARAM(CMapView, "ShapePointImageListID", GetShapePointImageListID, SetShapePointImageListID, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMapView, "get_UDPointImageListCount", get_UDPointImageListCount, VT_I4, VTS_I4)
	DISP_FUNCTION(CMapView, "get_UDPointImageListItem", get_UDPointImageListItem, VT_DISPATCH, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMapView, "ClearUDPointImageList", ClearUDPointImageList, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CMapView, "DrawLineEx", DrawLineEx, VT_EMPTY, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4 VTS_COLOR)
	DISP_FUNCTION(CMapView, "DrawPointEx",  DrawPointEx, VT_EMPTY,VTS_I4 VTS_R8 VTS_R8 VTS_I4 VTS_COLOR)
	DISP_FUNCTION(CMapView, "DrawCircleEx", DrawCircleEx, VT_EMPTY, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_COLOR VTS_BOOL)
	DISP_PROPERTY_NOTIFY(CMapView, "SendOnDrawBackBuffer", m_sendOnDrawBackBuffer, OnSendOnDrawBackBufferChanged, VT_I4)
	DISP_PROPERTY_EX_ID(CMapView, "MultilineLabels", dispidMultilineLabels, GetMultilineLabeling, SetMultilineLabeling, VT_BOOL)
	DISP_FUNCTION_ID(CMapView, "LabelColor", dispidLabelColor, LabelColor, VT_EMPTY, VTS_I4 VTS_COLOR)
	DISP_FUNCTION_ID(CMapView, "SetDrawingLayerVisible", dispidSetDrawingLayerVisible, SetDrawingLayerVisible, VT_EMPTY, VTS_I4 VTS_BOOL)
	DISP_FUNCTION_ID(CMapView, "ClearDrawingLabels", dispidClearDrawingLabels, ClearDrawingLabels, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "DrawingFont", dispidDrawingFont, DrawingFont, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMapView, "AddDrawingLabelEx", dispidAddDrawingLabelEx, AddDrawingLabelEx, VT_EMPTY, VTS_I4 VTS_BSTR VTS_COLOR VTS_R8 VTS_R8 VTS_I2 VTS_R8)
	DISP_FUNCTION_ID(CMapView, "AddDrawingLabel", dispidAddDrawingLabel, AddDrawingLabel, VT_EMPTY, VTS_I4 VTS_BSTR VTS_COLOR VTS_R8 VTS_R8 VTS_I2)
	DISP_PROPERTY_PARAM_ID(CMapView, "DrawingLabelsOffset",dispidDrawingLabelsOffset, GetDrawingLabelsOffset, SetDrawingLabelsOffset, VT_I4, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "DrawingLabelsShadowColor", dispidDrawingLabelsShadowColor, GetDrawingLabelsShadowColor, SetDrawingLabelsShadowColor, VT_COLOR, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "DrawingLabelsShadow", dispidDrawingLabelsShadow, GetDrawingLabelsShadow, SetDrawingLabelsShadow, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "DrawingLabelsScale", dispidDrawingLabelsScale, GetDrawingLabelsScale, SetDrawingLabelsScale, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "UseDrawingLabelCollision", dispidUseDrawingLabelCollision, GetUseDrawingLabelCollision, SetUseDrawingLabelCollision, VT_BOOL, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "DrawingLabelsVisible", dispidDrawingLabelsVisible, GetDrawingLabelsVisible, SetDrawingLabelsVisible, VT_BOOL, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "GetDrawingStandardViewWidth", dispidGetDrawingStandardViewWidth, GetDrawingStandardViewWidth, VT_EMPTY, VTS_I4 VTS_PR8)
	DISP_FUNCTION_ID(CMapView, "SetDrawingStandardViewWidth", dispidSetDrawingStandardViewWidth, SetDrawingStandardViewWidth, VT_EMPTY, VTS_I4 VTS_R8)
	DISP_FUNCTION(CMapView, "DrawWidePolygon", DrawWidePolygon, VT_EMPTY, VTS_VARIANT VTS_VARIANT VTS_I4 VTS_COLOR VTS_BOOL VTS_I2)
	DISP_FUNCTION(CMapView, "DrawWideCircle", DrawWideCircle, VT_EMPTY, VTS_R8 VTS_R8 VTS_R8 VTS_COLOR VTS_BOOL VTS_I2)
	DISP_FUNCTION_ID(CMapView, "SnapShot2", dispidSnapShot2, SnapShot2, VT_DISPATCH, VTS_I4 VTS_R8 VTS_I4)
	DISP_FUNCTION_ID(CMapView, "SnapShot3", dispidSnapShot3, SnapShot3, VT_DISPATCH, VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_I4)
	DISP_FUNCTION_ID(CMapView, "LayerFontEx", dispidLayerFontEx, LayerFontEx, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4 VTS_BOOL VTS_BOOL VTS_BOOL) 
	DISP_FUNCTION_ID(CMapView, "set_UDPointFontCharFont", dispidset_UDPointFontCharFont, set_UDPointFontCharFont, VT_EMPTY, VTS_I4 VTS_BSTR VTS_R4 VTS_BOOL VTS_BOOL VTS_BOOL)
	DISP_FUNCTION_ID(CMapView, "set_UDPointFontCharListAdd", dispidset_UDPointFontCharListAdd, set_UDPointFontCharListAdd, VT_I4, VTS_I4 VTS_I4 VTS_COLOR)
    DISP_FUNCTION_ID(CMapView, "set_UDPointFontCharFontSize", dispidset_UDPointFontCharFontSize, set_UDPointFontCharFontSize,VT_EMPTY, VTS_I4 VTS_R4)
	DISP_PROPERTY_PARAM_ID(CMapView, "ShapePointFontCharListID", dispidShapePointFontCharListID, GetShapePointFontCharListID, SetShapePointFontCharListID, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION_ID(CMapView, "ReSourceLayer", dispidReSourceLayer, ReSourceLayer, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_PROPERTY_PARAM_ID(CMapView, "ShapeStippleColor", dispidShapeStippleColor, GetShapeStippleColor, SetShapeStippleColor, VT_COLOR, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "ShapeLayerStippleColor", dispidShapeLayerStippleColor, GetShapeLayerStippleColor, SetShapeLayerStippleColor, VT_COLOR, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "ShapeStippleTransparent", dispidShapeStippleTransparent, GetShapeStippleTransparent, SetShapeStippleTransparent, VT_BOOL, VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "ShapeLayerStippleTransparent", dispidShapeLayerStippleTransparent, GetShapeLayerStippleTransparent, SetShapeLayerStippleTransparent, VT_BOOL, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "TrapRMouseDown", dispidTrapRMouseDown, GetTrapRMouseDown, SetTrapRMouseDown, VT_BOOL)
	DISP_PROPERTY_EX_ID(CMapView, "DisableWaitCursor", dispidDisableWaitCursor, GetDisableWaitCursor, SetDisableWaitCursor, VT_BOOL)
	DISP_FUNCTION_ID(CMapView, "AdjustLayerExtents", dispidAdjustLayerExtents, AdjustLayerExtents, VT_BOOL, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "UseSeamlessPan", dispidUseSeamlessPan, GetUseSeamlessPan, SetUseSeamlessPan, VT_BOOL)
	DISP_PROPERTY_EX_ID(CMapView, "MouseWheelSpeed", dispidMouseWheelSpeed, GetMouseWheelSpeed, SetMouseWheelSpeed, VT_R8)
	DISP_PROPERTY_EX_ID(CMapView, "ShapeDrawingMethod", dispidShapeDrawingMethod, GetShapeDrawingMethod, SetShapeDrawingMethod, VT_I2)
	DISP_FUNCTION_ID(CMapView, "DrawPolygonEx", dispidDrawPolygonEx, DrawPolygonEx, VT_EMPTY, VTS_I4 VTS_VARIANT VTS_VARIANT VTS_I4 VTS_COLOR VTS_BOOL)
	DISP_PROPERTY_EX_ID(CMapView, "CurrentScale", dispidCurrentScale, GetCurrentScale, SetCurrentScale, VT_R8)
	DISP_PROPERTY_PARAM_ID(CMapView, "DrawingLabels", dispidDrawingLabels, GetDrawingLabels, SetDrawingLabels, VT_DISPATCH, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "MapUnits", dispidMapUnits, GetMapUnits, SetMapUnits, VT_I2)
	DISP_FUNCTION_ID(CMapView, "SnapShotToDC", dispidSnapShotToDC, SnapShotToDC, VT_BOOL,VTS_I4 VTS_DISPATCH VTS_I4)
	DISP_FUNCTION_ID(CMapView, "DrawWideCircleEx", dispidDrawWideCircleEx, DrawWideCircleEx, VT_EMPTY, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_COLOR VTS_BOOL VTS_I2)
	DISP_FUNCTION_ID(CMapView, "DrawWidePolygonEx", dispidDrawWidePolygonEx, DrawWidePolygonEx, VT_EMPTY, VTS_I4 VTS_VARIANT VTS_VARIANT VTS_I4 VTS_COLOR VTS_BOOL VTS_I2)
    // Added ajp June 2010
	DISP_PROPERTY_EX_ID(CMapView, "DegreeRotationAngle", dispidMapRotationAngle, GetMapRotationAngle, SetMapRotationAngle, VT_R4)
	DISP_PROPERTY_EX_ID(CMapView, "RotatedExtent", dispidRotatedExtent, GetRotatedExtent, SetNotSupported, VT_DISPATCH)
    DISP_FUNCTION_ID(CMapView, "GetBaseProjectionPoint", dispidGetBaseProjectionPoint, GetBaseProjectionPoint, VT_DISPATCH, VTS_R8 VTS_R8)
	DISP_PROPERTY_EX_ID(CMapView, "CanUseImageGrouping", dispidCanUseImageGrouping, GetCanUseImageGrouping, SetCanUseImageGrouping, VT_BOOL)

	/*DISP_PROPERTY_EX_ID(CMapView, "VersionNumber1", dispidVersionNumber1, GetVersionNumber1, SetVersionNumber1, VT_I4)

	DISP_PROPERTY_PARAM(CMapView, "LayerMaxVisibleScale", GetLayerMaxVisibleScale, SetLayerMaxVisibleScale, VT_R8, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerMinVisibleScale", GetLayerMinVisibleScale, SetLayerMinVisibleScale, VT_R8, VTS_I4)
	DISP_PROPERTY_PARAM(CMapView, "LayerDynamicVisibility", GetLayerDynamicVisibility, SetLayerDynamicVisibility, VT_BOOL, VTS_I4)*/
	

	DISP_PROPERTY_EX_ID(CMapView, "VersionNumber", dispidVersionNumber, GetVersionNumber, SetVersionNumber, VT_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerMaxVisibleScale", dispidLayerMaxVisibleScale, GetLayerMaxVisibleScale, SetLayerMaxVisibleScale, VT_R8, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerMinVisibleScale", dispidLayerMinVisibleScale, GetLayerMinVisibleScale, SetLayerMinVisibleScale, VT_R8, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerDynamicVisibility", dispidLayerDynamicVisibility, GetLayerDynamicVisibility, SetLayerDynamicVisibility, VT_BOOL, VTS_I4)

	DISP_FUNCTION_ID(CMapView, "DrawBackBuffer", dispidDrawBackBuffer, DrawBackBuffer, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerLabels", dispidLayerLabels, GetLayerLabels, SetLayerLabels, VT_DISPATCH, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "ShowRedrawTime", dispidShowRedrawTime, GetShowRedrawTime, SetShowRedrawTime, VT_BOOL)
	DISP_PROPERTY_EX_ID(CMapView, "ShowVersionNumber", dispidShowVersionNumber, GetShowVersionNumber, SetShowVersionNumber, VT_BOOL)

	DISP_PROPERTY_PARAM_ID(CMapView, "Shapefile", dispidShapefile, GetShapefile, SetShapefile, VT_DISPATCH, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "Image", dispidImage, GetImage, SetImage, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "SerializeLayer", dispidSerializeLayerOptions, SerializeLayerOptions, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "DeserializeLayer", dispidDeserializeLayerOptions, DeserializeLayerOptions, VT_BOOL, VTS_I4 VTS_BSTR)
	DISP_FUNCTION_ID(CMapView, "SaveMapState", dispidSaveMapState, SaveMapState, VT_BOOL, VTS_BSTR VTS_BOOL VTS_BOOL)
	DISP_FUNCTION_ID(CMapView, "LoadMapState", dispidLoadMapState, LoadMapState, VT_BOOL, VTS_BSTR VTS_DISPATCH)
	DISP_FUNCTION_ID(CMapView, "SaveLayerOptions", dispidSaveLayerOptions, SaveLayerOptions, VT_BOOL, VTS_I4 VTS_BSTR VTS_BOOL VTS_BSTR)
	DISP_FUNCTION_ID(CMapView, "LoadLayerOptions", dispidLoadLayerOptions, LoadLayerOptions, VT_BOOL, VTS_I4 VTS_BSTR VTS_PBSTR)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerDescription", dispidLayerDescription, GetLayerDescription, SetLayerDescription, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "DeserializeMapState", dispidDeserializeMapState, DeserializeMapState, VT_BOOL, VTS_BSTR VTS_BOOL VTS_BSTR)
	DISP_FUNCTION_ID(CMapView, "SerializeMapState", dispidSerializeMapState, SerializeMapState, VT_BSTR, VTS_BOOL VTS_BSTR)
	DISP_FUNCTION_ID(CMapView, "RemoveLayerOptions", dispidRemoveLayerOptions, RemoveLayerOptions, VT_BOOL, VTS_I4 VTS_BSTR)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerSkipOnSaving", dispidLayerSkipOnSaving, GetLayerSkipOnSaving, SetLayerSkipOnSaving, VT_BOOL, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "RemoveLayerWithoutClosing", dispidRemoveLayerWithoutClosing, RemoveLayerWithoutClosing, VT_EMPTY, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "MaxExtents", dispidMaxExtents, GetMaxExtents, SetMaxExtents, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CMapView, "PixelsPerDegree", dispidPixelsPerDegree, GetPixelsPerDegree, SetPixelsPerDegree, VT_R8)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerFilename", dispidLayerFilename, GetLayerFilename, SetNotSupported, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "ZoomToSelected", dispidZoomToSelected, ZoomToSelected, VT_BOOL, VTS_I4)
	END_DISPATCH_MAP()
	
	//}}AFX_DISPATCH_MAP

/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CMapView, COleControl)
	//{{AFX_EVENT_MAP(CMapView)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	EVENT_CUSTOM("MouseDown", FireMouseDown, VTS_I2  VTS_I2  VTS_I4  VTS_I4)
	EVENT_CUSTOM("MouseUp", FireMouseUp, VTS_I2  VTS_I2  VTS_I4  VTS_I4)
	EVENT_CUSTOM("MouseMove", FireMouseMove, VTS_I2  VTS_I2  VTS_I4  VTS_I4)
	EVENT_CUSTOM("FileDropped", FireFileDropped, VTS_BSTR)
	EVENT_CUSTOM("SelectBoxFinal", FireSelectBoxFinal, VTS_I4  VTS_I4  VTS_I4  VTS_I4)
	EVENT_CUSTOM("SelectBoxDrag", FireSelectBoxDrag, VTS_I4  VTS_I4  VTS_I4  VTS_I4)
	EVENT_CUSTOM("ExtentsChanged", FireExtentsChanged, VTS_NONE)
	EVENT_CUSTOM("MapState", FireMapState, VTS_I4)
	//}}AFX_EVENT_MAP
	EVENT_STOCK_DBLCLICK()
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CMapView, 1)
	PROPPAGEID(CMapPropPage::guid)
END_PROPPAGEIDS(CMapView)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMapView, "MAPWINGIS.Map.1",
	0x54f4c2f7, 0xed40, 0x43b7, 0x9d, 0x6f, 0xe4, 0x59, 0x65, 0xdf, 0x7f, 0x95)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CMapView, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DMap =
		{ 0x1d077739, 0xe866, 0x46a0, { 0xb2, 0x56, 0x8a, 0xec, 0xc0, 0x4f, 0x23, 0x12 } };
const IID BASED_CODE IID_DMapEvents =
		{ 0xabea1545, 0x8ab, 0x4d5c, { 0xa5, 0x94, 0xd3, 0x1, 0x72, 0x11, 0xea, 0x95 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwMapOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMapView, IDS_MAP, _dwMapOleMisc)

#pragma endregion

#pragma region REGION Initialization

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

// **************************************************************
// CMapView::CMap
// **************************************************************
// Constructor
CMapView::CMapView():vals("AZ0CY1EX2GV3IT4KR5MP6ON7QL8SJ9UH0WF1DB2"), valsLen(39), m_forceBounds(false)
{
	GdiplusStartup();
	InitializeIIDs(&IID_DMap, &IID_DMapEvents);

	m_backbuffer = NULL;

	MultilineLabeling = true;

	m_canbitblt = FALSE;
	m_bitbltClickMove = CPoint(0,0);
	m_bitbltClickDown = CPoint(0,0);

	m_globalCallback = NULL;
	m_lastErrorCode = tkNO_ERROR;

	RespondTo3dConnexionEvents = true;
	SbLocked = false;

	m_key = "";

	m_viewHeight = 0;
	m_viewWidth = 0;

	rbMapResizeBehavior = rbClassic;

	//Cursors
	m_cursorPan = AfxGetApp()->LoadCursor(IDC_PAN);
	m_cursorZoomin = AfxGetApp()->LoadCursor(IDC_ZOOMIN);
	m_cursorZoomout = AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
	m_cursorSelect = AfxGetApp()->LoadCursor(IDC_SELECT);
	m_udCursor = NULL;

	m_lockCount = 0;
	m_numImages = 0;

	m_currentDrawing = -1;

	m_ttipCtrl = new CButton();
	m_showingToolTip = FALSE;

	snapshot = false;

	m_drawMutex.Unlock();
	m_legendMutex.Unlock();
	m_mapstateMutex.Unlock();

	m_LineSeparationFactor = 3;		

	m_useLabelCollision = false;

	_setmaxstdio(2048);

	// Added QUAY 3.16.09 for TrapRMouseDown
    DoTrapRMouseDown = TRUE;
	//--------------------------------------
	m_UseSeamlessPan = FALSE;
	m_MouseWheelSpeed = 0.5;
	m_ShapeDrawingMethod = dmNewSymbology;
	m_unitsOfMeasure = umMeters;
	m_DisableWaitCursor = false;

	m_RotateAngle = 0.0f;
	m_Rotate = NULL;

	_canUseImageGrouping = VARIANT_FALSE;

	m_imageGroups = NULL;

	m_ShowRedrawTime = VARIANT_FALSE;
	m_ShowVersionNumber = VARIANT_FALSE;

	srand( (unsigned)time( NULL ));

	m_isSizing = false;

	#ifdef _DEBUG
	GDALAllRegister();
	#endif

	m_gdalInitialized = true;
}

// ********************************************************************
//		OnCreate()
// ********************************************************************
int CMapView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_backbuffer = new CDC();
	m_backbuffer->CreateCompatibleDC(GetDC());

	DragAcceptFiles( TRUE );

	CRect rect(0,0,0,0);
	m_ttipCtrl->Create(NULL,WS_CHILD,rect,this,IDC_TTBTN);
	m_ttipCtrl->ShowWindow(FALSE);

	//CTool Tip
	m_ttip.Create(this,TTS_ALWAYSTIP);
	m_ttip.Activate(TRUE);
	m_ttip.AddTool(this,"",rect,IDC_TTBTN);
	m_ttip.SetDelayTime(TTDT_AUTOPOP,0);
	m_ttip.SetDelayTime(TTDT_INITIAL,0);
	m_ttip.SetDelayTime(TTDT_RESHOW,0);

	return 0;
}

// *****************************************************************
//	  CMapView::~CMap
// *****************************************************************
// Destructor
CMapView::~CMapView()
{
	m_collisionList.Clear();
	
	RemoveAllLayers();

	ClearDrawings();

	delete m_backbuffer;

	if( m_globalCallback )
		m_globalCallback->Release();

	delete m_ttipCtrl;

	GdiplusShutdown();

	if (m_Rotate)
	{
		delete m_Rotate;
		m_Rotate = NULL;
	}

	if (m_imageGroups)
	{
		for (int i = 0; i < m_imageGroups->size(); i++)
		{
			delete (*m_imageGroups)[i];
		}
		m_imageGroups->clear();
		delete m_imageGroups;
		m_imageGroups = NULL;
		
	}
}

// **********************************************************************
//	CMapView::DoPropExchange
// **********************************************************************
// Persistence support
void CMapView::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

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
	// lsu: 1-feb-2011
	PX_Bool( pPX, "ShowRedrawTime", (BOOL)m_ShowRedrawTime, FALSE);
	PX_Bool( pPX, "ShowVersionNumber", (BOOL)m_ShowVersionNumber, FALSE);
	PX_Double( pPX, "MouseWheelSpeed", m_MouseWheelSpeed, .5 );
	PX_Bool( pPX, "UseSeamlessPan", m_UseSeamlessPan, FALSE );
	PX_Bool( pPX, "MultilineLabels", MultilineLabeling, FALSE );
	PX_Bool( pPX, "TrapRMouseDown", DoTrapRMouseDown, TRUE );
	PX_Bool( pPX, "DisableWaitCursor", m_DisableWaitCursor, FALSE );	
	PX_Float( pPX, "MapRotationAngle", m_RotateAngle, 0.0 );
	PX_Bool( pPX, "CanUseImageGrouping", _canUseImageGrouping, FALSE );
	
	// enumerated constants aren't supported directly so temp buffer will be used
	// we don't care about the direction of exchanage, doing both getting and setting
	long temp;
	temp = (long)rbMapResizeBehavior;
	PX_Long( pPX, "MapResizeBehavior", temp, 0 );	//rbClassic
	rbMapResizeBehavior = (tkResizeBehavior)temp;

	temp = (long)m_ShapeDrawingMethod;
	PX_Long( pPX, "ShapeDrawingMethod", temp, 3 );	// dmNewSymbology
	m_ShapeDrawingMethod = (tkShapeDrawingMethod)temp;

	temp = (long)m_unitsOfMeasure;
	PX_Long( pPX, "MapUnits", temp, 6 );			//umMeters
	m_unitsOfMeasure = (tkUnitsOfMeasure)temp;

	m_mapCursor = 0;	// why not to save it?
}

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
CCriticalSection CMapView::ms_gdiplusLock;

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

// ***************************************************************
//	CMapView::PreCreateWindow()
// ***************************************************************
BOOL CMapView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CS_OWNDC | CS_BYTEALIGNWINDOW;
	return COleControl::PreCreateWindow(cs);
}
#pragma endregion

#pragma region MouseEvents
// ***************************************************************
// 		OnMouseWheel()					           
// ***************************************************************
//  Processing mouse wheel event. Amount of zoom is determined by MouseWheelsSpeed parameter
BOOL CMapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_MouseWheelSpeed < 0.1 || m_MouseWheelSpeed > 10) m_MouseWheelSpeed = 1;
	if (m_MouseWheelSpeed == 1) return FALSE;
	
	RECT rect;
  double width, height;
  double xCent, yCent;
  double dx, dy;

  // absolute cursor position
  this->GetWindowRect(&rect);
  if (pt.x < rect.left || pt.x > rect.right || pt.y < rect.top || pt.y > rect.bottom)
	  return false;
  if ((rect.right - rect.left == 0) && (rect.bottom - rect.top == 0))
		return false;

  if (m_Rotate != NULL && m_RotateAngle != 0)
  {
    CPoint curMousePt, origMousePt, rotCentre;
    
    curMousePt.x = pt.x - rect.left;
    curMousePt.y = pt.y - rect.top;
    rotCentre.x = (rect.right - rect.left) / 2;
    rotCentre.y = (rect.bottom - rect.top) / 2;

    m_Rotate->getOriginalPixelPoint(curMousePt.x, curMousePt.y, &(origMousePt.x), &(origMousePt.y));
    PixelToProj((double)(origMousePt.x), (double)(origMousePt.y), &xCent, &yCent);

    dx = (double)(origMousePt.x) / (double)(rect.right - rect.left);
		dy = (double)(origMousePt.y) / (double)(rect.bottom - rect.top);
  }
  else
  {
    PixelToProj((double)(pt.x - rect.left), (double)(pt.y - rect.top), &xCent, &yCent);
		dx = (double)(pt.x - rect.left) / (rect.right - rect.left);
	  dy = (double)(pt.y - rect.top) / (rect.bottom - rect.top);
  }

    // new extents
	
	double ratio = zDelta > 0?m_MouseWheelSpeed:(1/m_MouseWheelSpeed);
	height = (extents.top - extents.bottom) * ratio;
	width = (extents.right - extents.left) * ratio;
	
	Extent ext;
	ext.left = xCent - width * dx;
	ext.right = xCent + width * (1 - dx);
	ext.bottom = yCent - height * (1 - dy);
	ext.top = yCent + height * dy;

	CalculateVisibleExtents(ext, false);

	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();

	FireExtentsChanged();
	ReloadImageBuffers();
	return true;
}

// ************************************************************
//		OnLButtonDown
// ************************************************************
void CMapView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint rotPoint = point;
	if (m_Rotate != NULL && m_RotateAngle != 0)
	{
		m_Rotate->getOriginalPixelPoint(point.x, point.y, &(rotPoint.x), &(rotPoint.y));
		m_clickDown = rotPoint;
    }
	else
		m_clickDown = point;

	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;

	if( m_cursorMode == cmZoomIn )
	{
		if (m_sendMouseDown)
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				FireMouseDown(MK_LBUTTON,vbflags,rotPoint.x,rotPoint.y -1);
			else
				FireMouseDown(MK_LBUTTON,vbflags,point.x,point.y -1);
		}

		m_ttip.Activate(FALSE);
		
		//Selection Box
		CMapTracker selectBox = CMapTracker( this,
			CRect(0,0,0,0),
			CRectTracker::solidLine +
			CRectTracker::resizeOutside );
		selectBox.m_sizeMin = 0;

		bool selected = selectBox.TrackRubberBand( this, point, TRUE );
		m_ttip.Activate(TRUE);

		CRect rect = selectBox.m_rect;
		rect.NormalizeRect();

		if( ( rect.BottomRight().x - rect.TopLeft().x ) < 10 &&
			( rect.BottomRight().y - rect.TopLeft().y ) < 10 )
			selected = false;

		if( selected == true )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
			{
				CRect rectTmp = rect;
				long tmpX = 0, tmpY = 0;
				// adjust rectangle to unrotated coordinates
				m_Rotate->getOriginalPixelPoint(rect.left, rect.top, &tmpX, &tmpY);
				rectTmp.TopLeft().x = tmpX;
				rectTmp.TopLeft().y = tmpY;
				m_Rotate->getOriginalPixelPoint(rect.right, rect.bottom, &tmpX, &tmpY);
				rectTmp.BottomRight().x = tmpX;
				rectTmp.BottomRight().y = tmpY;
				rect = rectTmp;
			}
			
			double zrx = extents.right, zby = extents.bottom;
			double zlx = extents.left, zty = extents.top;
			PixelToProjection( rect.TopLeft().x, rect.TopLeft().y, zrx, zby );
			PixelToProjection( rect.BottomRight().x, rect.BottomRight().y, zlx, zty );

			double cLeft = MINIMUM( zrx, zlx );
			double cRight = MAXIMUM( zrx, zlx );
			double cBottom = MINIMUM( zty, zby );
			double cTop = MAXIMUM( zty, zby );

			m_canbitblt = FALSE;
			CalculateVisibleExtents( Extent( cLeft, cRight, cBottom, cTop ) );
			InvalidateControl();

			if( m_sendSelectBoxFinal == TRUE )
			{
				long iby = rect.BottomRight().y;
				long ity = rect.TopLeft().y;
				FireSelectBoxFinal( rect.TopLeft().x, rect.BottomRight().x, iby, ity );
			}

			FireExtentsChanged();
			ReloadImageBuffers();
		}
		else
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
			point = rotPoint;

			double zx = extents.left, zy = extents.bottom;
			PixelToProjection( point.x, point.y, zx, zy );

			double halfxRange = (extents.right - extents.left)*.5;
			double halfyRange = (extents.top - extents.bottom)*.5;

			// Chris M and Steve Abbot -- Log the previous extents here,
			// since we're about to change them.
			LogPrevExtent(); 

			extents.left = zx - halfxRange;
			extents.right = zx + halfxRange;
			extents.bottom = zy - halfyRange;
			extents.top = zy + halfyRange;

			// Chris M and Steve Abbot -- commented out the next line and added the following code from ZoomIn
			//ZoomIn( m_zoomPercent );
			double xzin = ((extents.right - extents.left)*( 1.0 - m_zoomPercent ))*.5;
			double xmiddle = extents.left + (extents.right - extents.left)*.5;

			double cLeft = xmiddle - xzin;
			double cRight = xmiddle + xzin;

			double yzin = ((extents.top - extents.bottom)*( 1.0 - m_zoomPercent ))*.5;
			double ymiddle = extents.bottom + (extents.top - extents.bottom)*.5;

			double cBottom = ymiddle - yzin;
			double cTop = ymiddle + yzin;
	        
			// Note also the added false in this call!
			CalculateVisibleExtents( Extent(cLeft, cRight, cBottom, cTop), false);
			m_canbitblt = FALSE;
			if( !m_lockCount )
				InvalidateControl();

			FireExtentsChanged();
			// End of PreviousExtents fix for ZoomIn
			ReloadImageBuffers();

			//the MapTracker interferes with the OnMouseUp event so we will call it manually
			if( m_sendMouseUp == TRUE )
			{
				if (m_Rotate != NULL && m_RotateAngle != 0)
					FireMouseUp(MK_LBUTTON,vbflags,rotPoint.x,rotPoint.y -1);
				else
					FireMouseUp( MK_LBUTTON, vbflags, point.x, point.y - 1 );
			}
		}
	}
	else if( m_cursorMode == cmZoomOut )
	{
		if (m_Rotate != NULL && m_RotateAngle != 0)
			point = rotPoint;
		double zx = extents.left, zy = extents.bottom;
		PixelToProjection( point.x, point.y, zx, zy );

		double halfxRange = (extents.right - extents.left)*.5;
		double halfyRange = (extents.top - extents.bottom)*.5;

		extents.left = zx - halfxRange;
		extents.right = zx + halfxRange;
		extents.bottom = zy - halfyRange;
		extents.top = zy + halfyRange;

		if( m_sendMouseDown == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				FireMouseDown(MK_LBUTTON,vbflags,rotPoint.x,rotPoint.y -1);
			else
				FireMouseDown( MK_LBUTTON, vbflags, point.x, point.y - 1 );
		}

		// Chris M and Steve Abbot -- commented out the next line and added the following code from ZoomOut
        // ZoomOut( m_zoomPercent );
        double xzout = ((extents.right - extents.left)*( 1.0 + m_zoomPercent ))*.5;
        double xmiddle = extents.left + (extents.right - extents.left)*.5;

        double cLeft = xmiddle - xzout;
        double cRight = xmiddle + xzout;

        double yzout = ((extents.top - extents.bottom)*( 1.0 + m_zoomPercent ))*.5;
        double ymiddle = extents.bottom + (extents.top - extents.bottom)*.5;

        double cBottom = ymiddle - yzout;
        double cTop = ymiddle + yzout;
        
		// Note also the added false in this call!
        CalculateVisibleExtents( Extent( cLeft, cRight, cBottom, cTop ), false);
        m_canbitblt = FALSE;
        if( !m_lockCount )
            InvalidateControl();
        // End of PreviousExtents fix for ZoomOut 

		FireExtentsChanged();
		ReloadImageBuffers();
	}
	else if( m_cursorMode == cmPan )
	{
		if( m_sendMouseDown == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				FireMouseDown(MK_LBUTTON,vbflags,rotPoint.x,rotPoint.y -1);
			else
				FireMouseDown(MK_LBUTTON, vbflags, point.x, point.y - 1);
		}

		CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
		m_bitbltClickDown = point;
		m_bitbltClickMove = point;
		LogPrevExtent();
		SetCapture();
		LockWindow(lmLock);

		//sent on mouse-up event
		//FireExtentsChanged();
	}
	else if( m_cursorMode == cmSelection )
	{
		m_ttip.Activate(FALSE);
		//Selection Box

		if( m_sendMouseDown == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				FireMouseDown(MK_LBUTTON,vbflags,rotPoint.x,rotPoint.y -1);
			else
				FireMouseDown(MK_LBUTTON, vbflags, point.x, point.y - 1 );
		}

		CMapTracker selectBox = CMapTracker( this,
			CRect(0,0,0,0),
			CRectTracker::solidLine +
			CRectTracker::resizeOutside );

		selectBox.m_sizeMin = 0;

		bool selected = selectBox.TrackRubberBand( this, point, TRUE );
		m_ttip.Activate(TRUE);

		CRect rect = selectBox.m_rect;
		rect.NormalizeRect();

		if( ( rect.BottomRight().x - rect.TopLeft().x ) < 10 &&
			( rect.BottomRight().y - rect.TopLeft().y ) < 10 )
			selected = false;

		if( selected == true )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
			{
				CRect rectTmp = rect;
				long tmpX = 0, tmpY = 0;
				// adjust rectangle to unrotated coordinates
				m_Rotate->getOriginalPixelPoint(rect.left, rect.top, &tmpX, &tmpY);
				rectTmp.TopLeft().x = tmpX;
				rectTmp.TopLeft().y = tmpY;
				m_Rotate->getOriginalPixelPoint(rect.right, rect.bottom, &tmpX, &tmpY);
				rectTmp.BottomRight().x = tmpX;
				rectTmp.BottomRight().y = tmpY;
				rect = rectTmp;
			}

			if( m_sendSelectBoxFinal == TRUE )
			{	long iby = rect.BottomRight().y;
				long ity = rect.TopLeft().y;
				FireSelectBoxFinal( rect.TopLeft().x, rect.BottomRight().x,	iby, ity );
				return; // exit out so that the FireMouseUp does not get called! DB 12/10/2002
			}
		}
		
		//the MapTracker interferes with the OnMouseUp event so we will call it manually
		if( m_sendMouseUp == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				FireMouseUp(MK_LBUTTON,vbflags,rotPoint.x,rotPoint.y -1);
			else
				FireMouseUp( MK_LBUTTON, vbflags, point.x, point.y - 1 );
		}
	}
	else //if( m_cursorMode == cmNone )
	{
		SetCapture();
		if( m_sendMouseDown == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				FireMouseDown(MK_LBUTTON,vbflags,rotPoint.x,rotPoint.y -1);
			else
				FireMouseDown( MK_LBUTTON, vbflags, point.x, point.y - 1 );
		}
	}
}

// ************************************************************
//		OnLButtonUp
// ************************************************************
void CMapView::OnLButtonUp(UINT nFlags, CPoint point)
{
	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;
  
	if (m_Rotate != NULL && m_RotateAngle != 0)
	{
		CPoint rotPoint = point;
		m_Rotate->getOriginalPixelPoint(point.x, point.y, &(rotPoint.x), &(rotPoint.y));
		point = rotPoint;
	}

	m_clickMove = point;
	m_clickUp = point;

	if( m_sendMouseUp == TRUE )
		FireMouseUp( MK_LBUTTON, vbflags, point.x, point.y - 1 );

	ReleaseCapture();

	if( m_cursorMode == cmPan )
	{
		//this is the only mode we care about for this event
		m_bitbltClickDown = CPoint(0,0);
		m_bitbltClickMove = CPoint(0,0);

		//InvalidateControl();
		m_canbitblt = FALSE;
		LockWindow(lmUnlock);

		FireExtentsChanged();
		ReloadImageBuffers();
	}
}

// ************************************************************
//		OnMouseMove
// ************************************************************
void CMapView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint rotPoint = point;
	if (m_Rotate != NULL && m_RotateAngle != 0)
		m_Rotate->getOriginalPixelPoint(point.x, point.y, &(rotPoint.x), &(rotPoint.y));

	if( m_showingToolTip == TRUE )
	{
		CToolInfo cti;
		m_ttip.GetToolInfo(cti,this,IDC_TTBTN);
		cti.rect.left = point.x - 2;
		cti.rect.right = point.x + 2;
		cti.rect.top = point.y - 2;
		cti.rect.bottom = point.y + 2;
		m_ttip.SetToolInfo(&cti);
		m_ttip.SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

		MSG pMsg;
		pMsg.hwnd = this->m_hWnd;
		pMsg.message = WM_MOUSEMOVE;
		pMsg.wParam = nFlags;
		pMsg.lParam = MAKELPARAM(point.x, point.y);
		pMsg.pt = point;
		m_ttip.RelayEvent(&pMsg);
	}

	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;

	if (m_Rotate != NULL && m_RotateAngle != 0)
		m_clickMove = rotPoint;
	else
		m_clickMove = point;

	if( m_sendMouseMove == TRUE )
	{	
		long mbutton = 0;
		if( nFlags & MK_LBUTTON )
			mbutton = 1;
		else if( nFlags & MK_RBUTTON )
			mbutton = 2;
		else if( nFlags & MK_MBUTTON )
			mbutton = 3;

		if( (m_cursorMode == cmPan) && (nFlags & MK_LBUTTON ))
		{
			//Do Not Send the Event
		}
		else
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
  				FireMouseMove( mbutton, vbflags, rotPoint.x, rotPoint.y - 1 );   
			else
 				FireMouseMove( mbutton, vbflags, point.x, point.y - 1 );
		}
	}

	if( m_cursorMode == cmZoomIn )
	{
	}
	else if( m_cursorMode == cmZoomOut )
	{
	}
	else if( m_cursorMode == cmPan )
	{
		if( nFlags & MK_LBUTTON )
		{
			m_bitbltClickMove = point;
			double zmx = 0, zmy = 0;
			double zpx = 0, zpy = 0;

			double xAmount = (m_clickDown.x - m_clickMove.x)*m_inversePixelPerProjectionX;
			double yAmount = (m_clickMove.y - m_clickDown.y)*m_inversePixelPerProjectionY;

			m_clickDown = m_clickMove;

			extents.left += xAmount;
			extents.right += xAmount;
			extents.bottom += yAmount;
			extents.top += yAmount;

			if (m_UseSeamlessPan)
			{
				m_canbitblt = FALSE;	// lsu (07/03/2009) added for seamless panning; suggested by Bobby at http://www.mapwindow.org/phorum/read.php?3,13099
				LockWindow(lmUnlock);	

				FireExtentsChanged(); 
				ReloadImageBuffers(); 
			}	
			else
				InvalidateControl();
		}
	}
	else if( m_cursorMode == cmSelection )
	{
	}
}

// ************************************************************
//		OnRButtonDown
// ************************************************************
void CMapView::OnRButtonDown(UINT nFlags, CPoint point)
{
   // Added QUAY 3.16.09
   if (DoTrapRMouseDown == TRUE )
   {
	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;

	m_clickDown = point;

	if( m_sendMouseDown == TRUE )
		FireMouseDown( MK_RBUTTON, vbflags, point.x, point.y - 1 );

	if( m_cursorMode == cmZoomOut )
	{
		double zx = extents.left, zy = extents.bottom;
		PixelToProjection( point.x, point.y, zx, zy );

		double halfxRange = (extents.right - extents.left)*.5;
		double halfyRange = (extents.top - extents.bottom)*.5;

		extents.left = zx - halfxRange;
		extents.right = zx + halfxRange;
		extents.bottom = zy - halfyRange;
		extents.top = zy + halfyRange;

		ZoomIn( m_zoomPercent );

		FireExtentsChanged();
		ReloadImageBuffers();
	}
	else if( m_cursorMode == cmZoomIn )
	{
		double zx = extents.left, zy = extents.bottom;
		PixelToProjection( point.x, point.y, zx, zy );

		double halfxRange = (extents.right - extents.left)*.5;
		double halfyRange = (extents.top - extents.bottom)*.5;

		extents.left = zx - halfxRange;
		extents.right = zx + halfxRange;
		extents.bottom = zy - halfyRange;
		extents.top = zy + halfyRange;

		ZoomOut( m_zoomPercent );

		FireExtentsChanged();
		ReloadImageBuffers();
	}
   // Added QUAY 3.16.09
   }
}

// *********************************************************
//		OnRButtonUp()
// *********************************************************
void CMapView::OnRButtonUp(UINT nFlags, CPoint point)
{
	COleControl::OnRButtonUp(nFlags, point);
	ReleaseCapture();//why is this being called, capture isn't set on RButtonDown as far as I can see...

	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;

	if( m_sendMouseUp == TRUE )
		FireMouseUp( MK_RBUTTON, vbflags, point.x, point.y - 1 );
}

// *********************************************************
//		OnMButtonUp()
// *********************************************************
void CMapView::OnMButtonUp(UINT nFlags, CPoint point)
{
	double zx = extents.left, zy = extents.bottom;
	PixelToProjection( point.x, point.y, zx, zy );

	double halfxRange = (extents.right - extents.left)*.5;
	double halfyRange = (extents.top - extents.bottom)*.5;

	extents.left = zx - halfxRange;
	extents.right = zx + halfxRange;
	extents.bottom = zy - halfyRange;
	extents.top = zy + halfyRange;

	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();

	FireExtentsChanged();
	ReloadImageBuffers();
}
#pragma endregion

#pragma region REGION Events

// *************************************************************
//		CMapView::OnSize()
// *************************************************************
void CMapView::OnSize(UINT nType, int cx, int cy)
{
	// the redraw is prohibited before the job here is done
	m_isSizing = true;

	COleControl::OnSize(nType, cx, cy);
	if ((HGDIOBJ)m_backbitmap)
	{
		VERIFY(m_backbitmap.DeleteObject());  
	}

	CDC* pDC = GetDC();
	VERIFY(m_backbitmap.CreateDiscardableBitmap(pDC,cx,cy));
	m_backbuffer->SelectObject(&m_backbitmap);
	
	// we shall fill the new regions with back color
	if (cx > m_viewWidth)
	{
		pDC->FillSolidRect(m_viewWidth, 0, cx - m_viewWidth, cy, m_backColor);
	}
	if (cy > m_viewHeight)
	{
		pDC->FillSolidRect(0, m_viewHeight, cx, cy - m_viewHeight, m_backColor);
	}

	ReleaseDC(pDC);

	if( cx > 0 && cy > 0 )
	{
		m_viewWidth = cx;
		m_viewHeight = cy;
		m_aspectRatio = (double)m_viewWidth/(double)m_viewHeight;
		FireExtentsChanged();
		CalculateVisibleExtents( extents, false, true );
		ReloadImageBuffers();
		m_canbitblt = FALSE;
		m_isSizing = false;
		InvalidateControl();
	}
	else
	{
		m_isSizing = false;
	}
}

// *******************************************************
//		OnDropFiles()
// *******************************************************
void CMapView::OnDropFiles(HDROP hDropInfo)
{
	long numFiles = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );

	register int i;
	for( i = 0; i < numFiles; i++ )
	{
		long fsize = DragQueryFile( hDropInfo, i, NULL, 0 );
		if( fsize > 0 )
		{	char * fname = new char[fsize + 2];
			DragQueryFile( hDropInfo, i, fname, fsize + 1 );
			FireFileDropped( fname );
			delete [] fname;
		}
	}

	COleControl::OnDropFiles(hDropInfo);
}

// *******************************************************
//		OnBackColorChanged()
// *******************************************************
void CMapView::OnBackColorChanged()
{
	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();
}

// *******************************************************
//		OnMapCursorChanged()
// *******************************************************
void CMapView::OnMapCursorChanged()
{	
	OnSetCursor(this,0,0);
}

// *******************************************************
//		OnSetCursor()
// *******************************************************
BOOL CMapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	//crsMapDefault
	if( m_mapCursor == 0 )
	{
		CPoint cpos;
		GetCursorPos(&cpos);
		CRect wrect;
		GetWindowRect(&wrect);
		
		// prevents changing the cursor outside contrls window
		HWND wndActive = ::GetActiveWindow();
		if ((wndActive == this->GetSafeHwnd()) || (wndActive == this->GetParentOwner()->GetSafeHwnd()))
		{
			if( wrect.PtInRect(cpos)) //&& (m_mapCursor != crsrUserDefined))
			{
				if( m_cursorMode == cmZoomIn )
					::SetCursor( m_cursorZoomin );
				else if( m_cursorMode == cmZoomOut )
					::SetCursor( m_cursorZoomout );
				else if( m_cursorMode == cmPan )
					::SetCursor( m_cursorPan );
				else if( m_cursorMode == cmSelection )
					::SetCursor( m_cursorSelect );
				else if( m_cursorMode == cmNone )
				{	
					if( m_uDCursorHandle != NULL )
					{
						::SetCursor( (HCURSOR)m_uDCursorHandle );
					}
					else
					{
						COleControl::OnSetCursor(pWnd, nHitTest, message);
					}
				}
			}
		}
	}
	else
	{
		//crsrAppStarting
		if( m_mapCursor == 1 )
			::SetCursor(LoadCursor(NULL, IDC_APPSTARTING) );
		//crsrArrow
		else if( m_mapCursor == 2 )
			::SetCursor(LoadCursor(NULL, IDC_ARROW) );
		//crsrCross
		else if( m_mapCursor == 3 )
			::SetCursor(LoadCursor(NULL, IDC_CROSS) );
		//crsrHelp
		else if( m_mapCursor == 4 )
			::SetCursor(LoadCursor(NULL, IDC_HELP) );
		//crsrIBeam
		else if( m_mapCursor == 5 )
			::SetCursor(LoadCursor(NULL, IDC_IBEAM) );
		//crsrNo
		else if( m_mapCursor == 6 )
			::SetCursor(LoadCursor(NULL, IDC_NO) );
		//crsrSizeAll
		else if( m_mapCursor == 7 )
			::SetCursor(LoadCursor(NULL, IDC_SIZEALL) );
		//crsrSizeNESW
		else if( m_mapCursor == 8 )
			::SetCursor(LoadCursor(NULL, IDC_SIZENESW) );
		//crsrSizeNS
		else if( m_mapCursor == 9 )
			::SetCursor(LoadCursor(NULL, IDC_SIZENS) );
		//crsrSizeNWSE
		else if( m_mapCursor == 10 )
			::SetCursor(LoadCursor(NULL, IDC_SIZENWSE) );
		//crsrSizeWE
		else if( m_mapCursor == 11 )
			::SetCursor(LoadCursor(NULL, IDC_SIZEWE) );
		//crsrUpArrow
		else if( m_mapCursor == 12 )
			::SetCursor(LoadCursor(NULL, IDC_UPARROW) );
		//crsrWait
		else if( m_mapCursor == 13 )
		{
			if (!m_DisableWaitCursor)
				::SetCursor(LoadCursor(NULL, IDC_WAIT) );
		}
		//crsrUserDefined
		else if( m_mapCursor == 14 )
		{	if( m_uDCursorHandle != NULL )
		::SetCursor( (HCURSOR)m_uDCursorHandle );
		else
			COleControl::OnSetCursor(pWnd, nHitTest, message);
		}
		else
			COleControl::OnSetCursor(pWnd, nHitTest, message);
	}

	return TRUE;

}

// *************************************************************
//		OnTimer()
// *************************************************************
#ifdef WIN64
void CMapView::OnTimer(UINT_PTR nIDEvent)
#else
void CMapView::OnTimer(UINT nIDEvent)
#endif
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == SHOWTEXT )
	{	KillTimer(SHOWTEXT);
		m_showingToolTip = TRUE;
	}
	else if( nIDEvent == HIDETEXT)
	{	KillTimer(HIDETEXT);
		m_showingToolTip = FALSE;

		CToolInfo cti;
		m_ttip.GetToolInfo(cti,this,IDC_TTBTN);
		cti.rect.left = -1;
		cti.rect.top = - 1;
		cti.rect.right = - 1;
		cti.rect.bottom = - 1;
		m_ttip.SetToolInfo(&cti);
		m_ttip.Pop();
	}

	COleControl::OnTimer(nIDEvent);
}

// *********************************************************
//		OnResetState 
// *********************************************************
// Reset control to default state
void CMapView::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	// TODO: Reset any other control state here.
}

// *********************************************************
//		Unimplemented events
// *********************************************************
void CMapView::OnExtentPadChanged(){}
void CMapView::OnExtentHistoryChanged(){}
void CMapView::OnKeyChanged(){}
void CMapView::OnDoubleBufferChanged(){}
void CMapView::OnZoomPercentChanged(){}
void CMapView::OnCursorModeChanged(){}
void CMapView::OnUDCursorHandleChanged(){}
void CMapView::OnSendMouseDownChanged(){}
void CMapView::OnSendOnDrawBackBufferChanged(){}
void CMapView::OnSendMouseUpChanged(){}
void CMapView::OnSendMouseMoveChanged(){}
void CMapView::OnSendSelectBoxDragChanged(){}
void CMapView::OnSendSelectBoxFinalChanged(){}

#pragma endregion

#pragma region REGION Drawing

// ********************************************************************
//		OnDraw()
// ********************************************************************
void CMapView::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (m_isSizing)
	{
		// lsu: 1-feb-2011: An option to clear the control surface with back color before redraw while sizing
		// but to leave the the previous bitmap and to paint only the new regions seems nicer
		
		//m_backbuffer->FillSolidRect(rcBounds,m_backColor);
		//ShowRedrawTime(0.0f, "Drawing...");
		//pdc->BitBlt( 0, 0,  rcBounds.Width(), rcBounds.Height(), m_backbuffer, 0, 0, SRCCOPY);
		
		return;	  // redraw is prohibited before the sizing will be finished
	}
	
	//This line is intended to ensure proper function in MSAccess by verifying that the hWnd handle exists
	//before trying to draw. Lailin Chen - 2005/10/17
	if (this->m_hWnd == NULL)
	{
		return;
	}

	if(m_canbitblt || m_lockCount )
	{	
		// lsu 1-feb-2011: better to fill screen with back color rather than white
		
		pdc->FillSolidRect(0, 0, m_bitbltClickMove.x - m_bitbltClickDown.x, rcBounds.Height(), m_backColor);
		pdc->FillSolidRect(rcBounds.Width() - ( m_bitbltClickDown.x - m_bitbltClickMove.x ), 0, rcBounds.Width(), rcBounds.Height(), m_backColor);
		pdc->FillSolidRect(0, 0, rcBounds.Width(), m_bitbltClickMove.y - m_bitbltClickDown.y, m_backColor);
		pdc->FillSolidRect(0, rcBounds.Height() - ( m_bitbltClickDown.y - m_bitbltClickMove.y ), rcBounds.Width(), rcBounds.Height(), m_backColor);

		pdc->BitBlt( m_bitbltClickMove.x - m_bitbltClickDown.x,
			m_bitbltClickMove.y - m_bitbltClickDown.y,
			rcBounds.Width(), rcBounds.Height(),
			m_backbuffer, 0, 0, SRCCOPY);

		DrawLists(rcBounds,pdc,dlScreenReferencedList);
	}
	else
	{
		DWORD startTick = ::GetTickCount();
		if( m_doubleBuffer )
		{
			if (m_RotateAngle != 0)
			{
				CDC     *tmpBackbuffer = new CDC();
				CRect   tmpRcBounds = new CRect();
				Extent  tmpExtent, saveExtent;       
				long    save_viewWidth, save_viewHeight;
		       
				if (m_Rotate == NULL)
				  m_Rotate = new Rotate();

				tmpBackbuffer->CreateCompatibleDC(m_backbuffer);
				m_Rotate->setSize(rcBounds);
				m_Rotate->setupRotateBackbuffer(tmpBackbuffer->m_hDC, pdc->m_hDC, m_backColor);

				save_viewWidth = m_viewWidth;
				save_viewHeight = m_viewHeight;
				m_viewWidth = m_Rotate->rotatedWidth;
				m_viewHeight = m_Rotate->rotatedHeight;
				saveExtent = extents;
				tmpExtent = extents;
				tmpExtent.right += (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
				tmpExtent.bottom -= (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
				tmpExtent.left -= (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
				tmpExtent.top += (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
				extents = tmpExtent;

				// draw the Map
				DrawNextFrame(rcBounds,tmpBackbuffer);
				
				// drawing the old labels 
				if ((m_ShapeDrawingMethod == dmStandard || m_ShapeDrawingMethod == dmNewWithSelection) && !FORCE_NEW_LABELS)
				{
					DrawOldLabels(m_backbuffer);
				}
				
				// Cleanup
				extents = saveExtent;
				m_viewWidth = save_viewWidth;
				m_viewHeight = save_viewHeight;
				m_Rotate->resetWorldTransform(tmpBackbuffer->m_hDC);
				m_backbuffer->BitBlt(0,0,rcBounds.Width(),rcBounds.Height(), tmpBackbuffer, 0, 0, SRCCOPY);
				m_Rotate->cleanupRotation(tmpBackbuffer->m_hDC);
				tmpBackbuffer->DeleteDC();
			}
			else
			{
				// main drawing
				DrawNextFrame(rcBounds,m_backbuffer);

				// drawing the old labels 
				if ((m_ShapeDrawingMethod == dmStandard || m_ShapeDrawingMethod == dmNewWithSelection) && !FORCE_NEW_LABELS)
				{
					DrawOldLabels(m_backbuffer);
				}
			}
			
			// displaying the time
			DWORD endTick = ::GetTickCount();
			ShowRedrawTime((float)(endTick - startTick)/1000.0f);

			// Fire the Draw event, notifying with the backbuffer
			if (m_sendOnDrawBackBuffer)
			{
				FireOnDrawBackBuffer((long)m_backbuffer->m_hDC);
			}
			
			//Swap the Buffers
			pdc->BitBlt(0,0,rcBounds.Width(),rcBounds.Height(),m_backbuffer,0,0,SRCCOPY);

			m_canbitblt=TRUE;
			
			DrawLists(rcBounds,pdc,dlScreenReferencedList);
		}
		else
		{
			DrawNextFrame(rcBounds,pdc);
			DrawLists(rcBounds,pdc,dlScreenReferencedList);
		}
	}
}

// ****************************************************************
//		ShowRedrawTime()
// ****************************************************************
// Displays redraw time in the bottom left corner
void CMapView::ShowRedrawTime(float time, CStringW message )
{
	if (!m_ShowRedrawTime && !m_ShowVersionNumber)	return;

	// preparing canvas
	Gdiplus::Graphics g(m_backbuffer->m_hDC);
	Gdiplus::SmoothingMode smoothing = g.GetSmoothingMode();
	Gdiplus::TextRenderingHint hint = g.GetTextRenderingHint();
	g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	// initializing brushes
	Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
	Gdiplus::Pen pen(Gdiplus::Color::White, 3.0f);
	pen.SetLineJoin(Gdiplus::LineJoinRound);

	// initializing font
	Gdiplus::FontFamily family(L"Arial");
	Gdiplus::Font font(&family, (Gdiplus::REAL)12.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
	Gdiplus::PointF point(0.0f, 0.0f);
	Gdiplus::StringFormat format; 
	
	// vars
	CStringW s;
	Gdiplus::GraphicsPath path;
	Gdiplus::RectF rect;
	Gdiplus::Matrix mtx;
	
	int width = m_viewWidth; //r.right - r.left;
	int height;

	if (m_ShowVersionNumber)
	{
		s.Format(L"MapWinGIS %d.%d", _wVerMajor, _wVerMinor);
		path.StartFigure();
		path.AddString(s.GetString(), wcslen(s), &family, font.GetStyle(), font.GetSize(), point, &format);
		path.CloseFigure();
		path.GetBounds(&rect);
		height = rect.Height;
		
		if (rect.Width + 10 < width)		// control must be big enough to host the string
		{
			mtx.Translate((float)(m_viewWidth - rect.Width - 10), (float)(m_viewHeight - height - 10));
			path.Transform(&mtx);
			g.DrawPath(&pen, &path);
			g.FillPath(&brush, &path);
			width -= (rect.Width);
		}
	}
	
	path.Reset();
	mtx.Reset();

	if (m_ShowRedrawTime)
	{
		if (wcslen(message) != 0)
		{
			s = message;
		}
		else
		{
			s.Format(L"Redraw time: %.3f sec", time);
		}
		path.StartFigure();
		path.AddString(s.GetString(), wcslen(s), &family, font.GetStyle(), font.GetSize(), point, &format);
		path.CloseFigure();
		path.GetBounds(&rect);
		height = m_ShowVersionNumber?height:rect.Height + 3;
		if (rect.Width + 15 < width)		// control must be big enough to host the string
		{
			mtx.Translate(5.0f, (float)(m_viewHeight - height - 10));
			path.Transform(&mtx);
			g.DrawPath(&pen, &path);
			g.FillPath(&brush, &path);
			width -= (rect.Width + 15);
		}
	}
	
	g.SetTextRenderingHint(hint);
	g.SetSmoothingMode(smoothing);
	g.ReleaseHDC(m_backbuffer->m_hDC);
}

// ****************************************************************
//		DrawNextFrame()
// ****************************************************************
void CMapView::DrawNextFrame(const CRect & rcBounds, CDC * dc)
{
	if (m_lockCount > 0 && !m_forceBounds)
	{
		return;
	}
	
	HCURSOR oldCursor = ::GetCursor();
	SetWaitCursor();

	// clear extents of drawn labels and charts
	ClearLabelFrames();
	
	m_drawMutex.Lock();

	dc->FillSolidRect(rcBounds,m_backColor);

	register int i, j;
	long startcondition = 0;
	long endcondition = m_activeLayers.size();

	if (endcondition == 0)
	{
		// drawing
		DrawLists(rcBounds,dc,dlSpatiallyReferencedList);
		m_drawMutex.Unlock();
		::SetCursor(oldCursor);
		return;
	}
	
	// ------------------------------------------------------------------
	//	Check whether some layers are completely concealed by images 
	//	no need to draw them then
	// ------------------------------------------------------------------
	bool * isConcealed = NULL;
	if( endcondition )
	{
		isConcealed = new bool[endcondition];
		memset(isConcealed,0,endcondition*sizeof(bool));
	}
	
	double scale = GetCurrentScale();

	if( m_numImages > 0 && !_canUseImageGrouping )
	{
		for( i = endcondition - 1; i >= 0; i-- )
		{
			Layer * l = m_allLayers[m_activeLayers[i]];
			if( IS_VALID_PTR(l) )
			{
				if( l->type == ImageLayer && l->IsVisible(scale))   // l->flags & Visible )
				{
					IImage * iimg = NULL;
					l->object->QueryInterface(IID_IImage,(void**)&iimg);
					if( iimg == NULL )continue;
					
					// lsu: to avoid calling this function externally
					AdjustLayerExtents(i);

					VARIANT_BOOL useTransparencyColor;
					iimg->get_UseTransparencyColor(&useTransparencyColor);
					iimg->Release();
					iimg = NULL;

					if( useTransparencyColor == FALSE )
					{
						//Check if this is the end condition layer
						if( l->extents.left <= extents.left && 
							l->extents.right >= extents.right &&
							l->extents.bottom <= extents.bottom && 
							l->extents.top >= extents.top )
						{	
							startcondition = i;
							break;
						}
						//Check if this layer conceals any others
						else if( isConcealed[i] == false )
						{
							for( j = i - 1; j >= 0; j-- )
							{
								Layer * l2 = m_allLayers[m_activeLayers[j]];
								if( IS_VALID_PTR(l2) )
								{
									if( l->extents.left <= l2->extents.left && 
										l->extents.right >= l2->extents.right &&
										l->extents.bottom <= l2->extents.bottom && 
										l->extents.top >= l2->extents.top )
									{
										isConcealed[j] = true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	// ------------------------------------------------------------------
	//		Drawing of grouped image layers
	// ------------------------------------------------------------------
	if ( _canUseImageGrouping )
	{
		std::vector<ImageGroup*>* newGroups = new std::vector<ImageGroup*>;

		// building groups
		BuildImageGroups(*newGroups);
		
		// comparing them with the old list
		if (m_imageGroups != NULL)
		{
			if (ImageGroupsAreEqual(*m_imageGroups, *newGroups))
			{
				// groups are the same so we can continue to use them
				for (int i = 0; i < newGroups->size(); i++)
				{
					delete (*newGroups)[i];
				}
				newGroups->clear();
				delete newGroups;
				newGroups = NULL;
			}
			else
			{
				// groups ahs changed, swapping pointers
				if (m_imageGroups != NULL)
				{
					for (int i = 0; i < m_imageGroups->size(); i++)
					{
						delete (*m_imageGroups)[i];
					}
					
					m_imageGroups->clear();
					delete m_imageGroups;
					m_imageGroups = NULL;
				}
				m_imageGroups = newGroups;
			}
		}
		else
		{
			m_imageGroups = newGroups;
		}
		
		// mark all images as undrawn
		for (int i = 0; i < m_imageGroups->size(); i++)
		{
			(*m_imageGroups)[i]->wasDrawn = false;
		}
	}
	
	// ------------------------------------------------------------------
	//		Actual drawing
	// ------------------------------------------------------------------
	double currentScale = this->GetCurrentScale();

	bool useCommonCollisionListForCharts = true;
	bool useCommonCollisionListForLabels = true;

	// collision avoidance
	//CCollisionList collisionList;
	m_collisionList.Clear();
	CCollisionList collisionListLabels;
	CCollisionList collisionListCharts;
	
	CCollisionList* chosenListLabels = NULL;
	CCollisionList* chosenListCharts = NULL;
	
	chosenListLabels = useCommonCollisionListForLabels?(&m_collisionList):(&collisionListLabels);
	chosenListCharts = useCommonCollisionListForCharts?(&m_collisionList):(&collisionListCharts);

	// initializing classes for drawing
	bool forceGdiplus = this->m_RotateAngle != 0.0f;
	CShapefileDrawer sfDrawer(dc, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, &m_collisionList, forceGdiplus);
	CImageDrawer imgDrawer(dc, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, m_viewWidth, m_viewHeight);
	CLabelDrawer lblDrawer(dc, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, currentScale, chosenListLabels, m_RotateAngle);
	CChartDrawer chartDrawer(dc, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, currentScale, chosenListCharts);

	for(int i = startcondition; i < endcondition; i++)
	{
		//CString str;
		//str.Format("Drawing layer %d", i);
		//timer.PrintTime(str.GetBuffer());
		
		if( isConcealed[i] == false )
		{
			Layer * l = m_allLayers[m_activeLayers[i]];
			if( l != NULL )
			{	
				if (l->IsVisible(scale))
				{
					if(l->type == ImageLayer)
					{
						if(l->object == NULL ) continue;
						IImage * iimg = NULL;
						l->object->QueryInterface(IID_IImage,(void**)&iimg);
						if( iimg == NULL ) continue;
						
						CImageClass* img = (CImageClass*)iimg;
						
						if (_canUseImageGrouping && img->m_groupID != -1)
						{
							// this is grouped image, if this is the first image of group, we shall draw the whole group
							if (!(*m_imageGroups)[img->m_groupID]->wasDrawn)
							{
								DrawImageGroups(rcBounds, dc, img->m_groupID);
								(*m_imageGroups)[img->m_groupID]->wasDrawn = true;
							}
						}
						else
						{
							bool saveBitmap = true;
							
							if (saveBitmap)
							{
								CImageClass* img = (CImageClass*)iimg;
								ScreenBitmap* bmp = img->_screenBitmap;
								bool wasDrawn = false;

								// in case we have saved bitmap and map position is the same we shall draw it
								if (bmp)
								{
									if (bmp->extents == extents &&
										bmp->pixelPerProjectionX == m_pixelPerProjectionX &&
										bmp->pixelPerProjectionY == m_pixelPerProjectionY &&
										bmp->viewWidth == m_viewWidth &&
										bmp->viewHeight == m_viewHeight && !((CImageClass*)iimg)->_imageChanged
										)
									{
										Gdiplus::Graphics g(dc->m_hDC);
										g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
										
										// TODO: choose interpolation mode more precisely
										// TODO: set image attributes
										
										g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);	
										g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
										g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
										g.DrawImage(bmp->bitmap, Gdiplus::REAL(bmp->left), Gdiplus::REAL(bmp->top));
										wasDrawn = true;
									}
								}
								
								if (!wasDrawn)
								{
									long width, height;
									iimg->get_OriginalWidth(&width);
									iimg->get_OriginalHeight(&height);

									if ((width == 256 && height == 256) || m_forceBounds)
									{
										// it's tiles, I don't want to cache bitmap here to avoid seams
										// the same thing with Snapshot calls
										bmp = imgDrawer.DrawImage(rcBounds, iimg);
									}
									else
									{
										// image hasn't been saved so far
										bmp = imgDrawer.DrawImage(rcBounds, iimg, true);
										
										if (img->_screenBitmap)
										{
											delete img->_screenBitmap;
											img->_screenBitmap = NULL;
										}

										img->_screenBitmap = bmp;
									}
								}
							}
							else
							{
								imgDrawer.DrawImage(rcBounds, iimg);
							}
						}

						img->Release();
						
						// drawing labels for images
						ILabels* labels = l->get_Labels();
						if(labels != NULL)
						{
							tkVerticalPosition vertPos;
							labels->get_VerticalPosition(&vertPos);
							if (vertPos == vpAboveParentLayer)		
							{
								lblDrawer.DrawLabels(labels);
							}
							labels->Release();
							labels = NULL;
						}
					}
					else if( l->type == ShapefileLayer )
					{
						// grab extents from shapefile in case they've changed
						AdjustLayerExtents(m_activeLayers[i]);

						if( l->extents.left   < extents.left   && l->extents.right < extents.left )		continue;
						if( l->extents.left   > extents.right  && l->extents.right > extents.right )	continue;
						if( l->extents.bottom < extents.bottom && l->extents.top   < extents.bottom )	continue;
						if( l->extents.bottom > extents.top    && l->extents.top   > extents.top )		continue;
					
						if( l->object == NULL )
						{
							continue;	// TODO: report the error?
						}
						
						IShapefile* sf = NULL;
						l->object->QueryInterface(IID_IShapefile,(void**)&sf);
						
						if( sf )
						{
							// shapes
							if (m_ShapeDrawingMethod == dmStandard)
							{
								DrawShapefile( rcBounds, dc, l );
							}
							else if (m_ShapeDrawingMethod == dmNewWithSelection || m_ShapeDrawingMethod == dmNewWithLabels)
							{
								DrawShapefileAlt( rcBounds, dc, l );
							}
							else // if (m_ShapeDrawingMethod == dmNewWithSymbology)
							{
								sfDrawer.Draw(rcBounds, sf, ((CShapefile*)sf)->get_File());
							}
							
							// for old modes we shall mark all the shapes of shapefile as visible as no visiblity expressions were analyzed
							if (m_ShapeDrawingMethod != dmNewSymbology)
							{
								std::vector<ShapeData*>* shapeData = ((CShapefile*)sf)->get_ShapeVector();
								if (shapeData)
								{
									for (int n = 0; n < shapeData->size(); n++)
									{
										(*shapeData)[n]->isVisible = true;
									}
								}
							}

							// labels
							if ((m_ShapeDrawingMethod == dmStandard || m_ShapeDrawingMethod == dmNewWithSelection) && !FORCE_NEW_LABELS)
							{
								m_labelsToDraw.push(l);
							}
							else //(m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology)
							{
								ILabels* labels = l->get_Labels();
								if(labels != NULL)
								{
									tkVerticalPosition vertPos;
									labels->get_VerticalPosition(&vertPos);
									if (vertPos == vpAboveParentLayer)		
									{
										lblDrawer.DrawLabels(labels);
									}
									labels->Release();
									labels = NULL;
								}
							}

							// charts: available for all modes
							ICharts* charts = NULL;
							sf->get_Charts(&charts);
							if (charts)
							{
								tkVerticalPosition vertPosition;
								charts->get_VerticalPosition(&vertPosition);
								if (vertPosition == vpAboveParentLayer )
								{
									chartDrawer.DrawCharts(sf);
								}
								charts->Release();
								charts = NULL;
							}
							sf->Release();
						}
					}
				}
			}
		}
	}
	
	// -------------------------------------------------
	//	Drawing labels and charts above the layers
	// -------------------------------------------------
	for (i = 0; i < (int)m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			if (l->IsVisible(scale))
			{
				//  labels: for the new modes only
				if (m_ShapeDrawingMethod == dmNewWithLabels || m_ShapeDrawingMethod == dmNewSymbology || l->type == ImageLayer || FORCE_NEW_LABELS)
				{
					ILabels* labels = l->get_Labels();
					if ( labels )
					{
						tkVerticalPosition vertPos;
						labels->get_VerticalPosition(&vertPos);
						if (vertPos == vpAboveAllLayers)
						{
							lblDrawer.DrawLabels(labels);
						}
						labels->Release(); 
						labels = NULL;
					}
				}
				
				// charts: for all modes
				IShapefile* sf = NULL;
				l->object->QueryInterface(IID_IShapefile,(void**)&sf);
				if ( sf )
				{
					ICharts* charts = NULL;
					sf->get_Charts(&charts);
					if (charts)
					{
						tkVerticalPosition vertPosition;
						charts->get_VerticalPosition(&vertPosition);
						if (vertPosition == vpAboveAllLayers )
						{
							chartDrawer.DrawCharts(sf);
						}
						charts->Release();
						charts = NULL;
					}
					
					sf->Release();
					sf = NULL;
				}
			}
		}
	}

	// drawing
	DrawLists(rcBounds,dc,dlSpatiallyReferencedList);
	
	m_drawMutex.Unlock();
	::SetCursor(oldCursor);

	delete [] isConcealed;
}
#pragma endregion

#pragma region ImageGrouping
// *****************************************************************
//		BuildImageGroups
// *****************************************************************
// Here we'll make groups from the images with the same size and positions
// Group number will be written to the each image groupID property
void CMapView::BuildImageGroups(std::vector<ImageGroup*>& imageGroups)
{
	imageGroups.clear();

	for(int i = 0; i < m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			if(l->type == ImageLayer)
			{
				IImage* iimg = NULL;
				l->object->QueryInterface(IID_IImage, (void**)&iimg);

				if ( iimg != NULL )
				{
					CImageClass* img = (CImageClass*)iimg;
					img->m_groupID = -1;
					
					if (l->flags & Visible)
					{
						if ( img->_canUseGrouping)
						{
							double dx, dy, xllCenter, yllCenter;
							LONG width, height;

							img->get_OriginalHeight(&height);
							img->get_OriginalWidth(&width);
							
							img->get_OriginalDX(&dx);
							img->get_OriginalDY(&dy);
							img->get_OriginalXllCenter(&xllCenter);
							img->get_OriginalYllCenter(&yllCenter);

							//img->GetOriginal_dX(&dx);
							//img->GetOriginal_dY(&dy);
							//img->GetOriginalXllCenter(&xllCenter);
							//img->GetOriginalYllCenter(&yllCenter);

							bool groupFound = false;
							for(int j = 0; j < imageGroups.size(); j++)
							{
								ImageGroup* group = imageGroups[j];
								
								if ((group->dx == dx) && 
									(group->dy == dy) && 
									(group->width == width) && 
									(group->height == height) &&
									(group->xllCenter == xllCenter) && 
									(group->yllCenter == yllCenter))
								{
									groupFound = true;
									group->imageIndices.push_back(i);
									break;
								}
							}
							
							if (! groupFound )
							{
								// adding new group
								ImageGroup* group = new ImageGroup(dx, dy, xllCenter, yllCenter, width, height);
								imageGroups.push_back(group);
								imageGroups[imageGroups.size() - 1]->imageIndices.push_back(i);
							}
						}
					}
				}
			}
		}
	}

	// now we'll check whether the pixels of image are scarce enough for us
	// the group wil work only in case there is more then 1 suitable image
	int groupId = 0;
	IImage* iimg = NULL;
	for (int i = 0; i < imageGroups.size(); i++)
	{
		std::vector<int>* indices = &imageGroups[i]->imageIndices;
		int groupSize = indices->size();

		if (groupSize > 1)
		{
			for (int j = 0; j < indices->size(); j++ )
			{
				Layer * l = m_allLayers[m_activeLayers[(*indices)[j]]];
				l->object->QueryInterface(IID_IImage, (void**)&iimg);
				CImageClass* img = (CImageClass*)iimg;
				
				if (!img->_pixelsSaved)				// it's the first time we try to draw image or transparency color chnaged
				{
					if (!img->SaveNotNullPixels())	// analysing pixels...
					{
						(*indices)[j] = -1;
						img->put_CanUseGrouping(VARIANT_FALSE);	//  don't try this image any more - there are to many data pixels in it
						groupSize--;
					}
				}
			}
		}
		
		// saving the valid groups
		if (groupSize > 1)
		{
			imageGroups[i]->isValid = true;
			for (int i = 0; i< indices->size(); i++)
			{
				int imageIndex = (*indices)[i];
				if (imageIndex != -1)
				{
					Layer * l = m_allLayers[m_activeLayers[imageIndex]];
					l->object->QueryInterface(IID_IImage, (void**)&iimg);
					CImageClass* img = (CImageClass*)iimg;
					img->m_groupID = groupId;
				}
			}
			groupId++;
		}
		else
		{
			imageGroups[i]->isValid = false;
		}
	}
}

// *****************************************************************
//		ChooseInterpolationMode
// *****************************************************************
// Choosing the mode with better quality from the pair
tkInterpolationMode CMapView::ChooseInterpolationMode(tkInterpolationMode mode1, tkInterpolationMode mode2)
{
	if (mode1 == imHighQualityBicubic || mode2 == imHighQualityBicubic )
	{
		return imHighQualityBicubic;
	}
	else if (mode1 == imHighQualityBilinear || mode2 == imHighQualityBilinear )
	{
		return imHighQualityBilinear;
	}
	else if (mode1 == imBicubic || mode2 == imBicubic )
	{
		return imBicubic;
	}
	else if (mode1 == imBilinear || mode2 == imBilinear )
	{
		return imBilinear;
	}
	else
	{
		return imNone;
	}
}

// *****************************************************************
//		DrawImageGroups
// *****************************************************************
// groupIndex - index of group that should be drawn
void CMapView::DrawImageGroups(const CRect& rcBounds, CDC* dc, int groupIndex)
{
	CImageDrawer imgDrawer(dc, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, m_viewWidth, m_viewHeight);
	IImage* iimg = NULL;

	ImageGroup* group = (*m_imageGroups)[groupIndex];
	if (! group->isValid ) 
		return;
	
	// in case the image was drawn at least once at current resolution, we can use screenBitmap
	ScreenBitmap* bmp = NULL;
	bmp = group->screenBitmap;
	if (bmp != NULL)
	{
		if (bmp->extents == extents &&
			bmp->pixelPerProjectionX == m_pixelPerProjectionX &&
			bmp->pixelPerProjectionY == m_pixelPerProjectionY &&
			bmp->viewWidth == m_viewWidth &&
			bmp->viewHeight == m_viewHeight)
		{
			Gdiplus::Graphics g(dc->m_hDC);
			g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
			
			g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			g.SetSmoothingMode(Gdiplus::SmoothingModeDefault);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			g.DrawImage(bmp->bitmap, Gdiplus::REAL(bmp->left), Gdiplus::REAL(bmp->top));
			return;
		}
	}
	
	double scale = GetCurrentScale();

	if(group->image == NULL)
	{
		// creating a new temporary image		
		IImage* imgGroup = NULL;
		VARIANT_BOOL vbretval;
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&imgGroup);
		imgGroup->CreateNew(group->width, group->height, &vbretval);

		if ( !vbretval )
		{
			return;
		}
		else
		{
			// setting it's position
			imgGroup->put_dX(group->dx);
			imgGroup->put_dY(group->dy);
			imgGroup->put_XllCenter(group->xllCenter);
			imgGroup->put_YllCenter(group->yllCenter);
			
			tkInterpolationMode downsamplingMode = imNone;
			tkInterpolationMode upsamplingMode = imNone;
			
			// acquiring reference to the destination color array
			unsigned char* data = ((CImageClass*)imgGroup)->get_ImageData();
			colour* dstData = reinterpret_cast<colour*>(data);
			
			// passing the data from all images
			bool visibleLayerExists = false;
			bool useTransparencyColor = true;		
			for(int j = 0; j < m_activeLayers.size(); j++)
			{
				Layer * l = m_allLayers[m_activeLayers[j]];
				if( l != NULL )
				{	
					//if(l->type == ImageLayer && (l->flags & Visible))
					if(l->type == ImageLayer && l->IsVisible(scale))
					{
						l->object->QueryInterface(IID_IImage, (void**)&iimg);
						CImageClass* img = (CImageClass*)iimg;

						if ( img )
						{
							if (img->m_groupID == groupIndex)
							{
								tkInterpolationMode downMode;
								tkInterpolationMode upMode;
								img->get_DownsamplingMode(&downMode);
								img->get_UpsamplingMode(&upMode);
								
								// in case at least one image don't use transparency the grouped bitmap will have white background
								VARIANT_BOOL transp;
								img->get_UseTransparencyColor(&transp);
								if (!transp) 
									useTransparencyColor = false;

								downsamplingMode = ChooseInterpolationMode(downsamplingMode, downMode);
								upsamplingMode = ChooseInterpolationMode(upsamplingMode, upMode);
								
								visibleLayerExists = true;

								DataPixels* pixels = img->m_pixels;
								int pixelsCount = img->m_pixelsCount;

								// passing data
								DataPixels* val;
								for (int p = 0; p < pixelsCount; p++ )
								{
									val = pixels + p;
									memcpy(&(dstData[val->position]), &val->value, sizeof(colour));
									//dstData[val->position] = val->value;
								}
							}
						}
					}
				}
			}
			
			if (useTransparencyColor)
			{
				imgGroup->put_TransparencyColor(RGB(255, 255, 255));
				imgGroup->put_TransparencyColor2(RGB(255, 255, 255));
				imgGroup->put_UseTransparencyColor(VARIANT_TRUE);
			}
			else
			{
				imgGroup->put_UseTransparencyColor(VARIANT_FALSE);
			}

			if (!visibleLayerExists)
			{
				return;
			}
			else
			{
				// setting sampling mode
				imgGroup->put_UpsamplingMode(upsamplingMode);
				imgGroup->put_DownsamplingMode(downsamplingMode);
				group->image = imgGroup;
			}
		}
	}
	
	// drawing; in case we draw it first time screen bitmap will be saved, for not doing resampling when loading each new tile
	/* ScreenBitmap*  */
	bmp = imgDrawer.DrawImage(rcBounds, group->image, true);
	if (bmp)
	{
		if (group->screenBitmap != NULL)
		{
			delete group->screenBitmap;
			group->screenBitmap = NULL;
		}
		
		int width = bmp->bitmap->GetWidth();
		int height = bmp->bitmap->GetHeight();
		
		group->screenBitmap = bmp;	// saving bitmap in screen resolution
	}
}

// *****************************************************************
//		ImageGroupsAreEqual()
// *****************************************************************
bool CMapView::ImageGroupsAreEqual(std::vector<ImageGroup*>& groups1, std::vector<ImageGroup*>& groups2)
{
	if (groups1.size() != groups2.size())
	{
		return false;
	}
	else
	{
		for (int i = 0; i < groups1.size(); i++)
		{
			if (!(groups1[i] == groups2[i]))
			{
				return false;
			}
		}
	}
	return true;
}
#pragma endregion

#pragma region REGION ControlProperties

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


// *********************************************************
//		SaveMapState()
// *********************************************************
VARIANT_BOOL CMapView::SaveMapState(LPCTSTR Filename, VARIANT_BOOL RelativePaths, VARIANT_BOOL Overwrite)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (Utility::fileExists(Filename))
	{
		if (!Overwrite)
		{
			ErrorMessage(tkCANT_CREATE_FILE);
			return VARIANT_FALSE;
		}
		else
		{
			if( remove( Filename ) != 0 )
			{
				ErrorMessage(tkCANT_DELETE_FILE);
				return VARIANT_FALSE;
			}
		}
	}

	CPLXMLNode* node = SerializeMapStateCore(RelativePaths, Filename);
	return CPLSerializeXMLTreeToFile(node, Filename) ? VARIANT_TRUE : VARIANT_FALSE;
}

// *********************************************************
//		LoadMapState()
// *********************************************************
VARIANT_BOOL CMapView::LoadMapState(LPCTSTR Filename, IDispatch* Callback)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	try
	{
		CPLXMLNode* node = CPLParseXMLFile(Filename);
		if (node)
		{
			IStopExecution* cb = NULL;
			if (Callback)
			{
				Callback->QueryInterface(IID_IStopExecution, (void**)&cb);
			}
			
			bool result = DeserializeMapStateCore(node, Filename, VARIANT_TRUE, cb);
			
			if (cb) 
			{
				cb->Release();
			}
			return result ? VARIANT_TRUE : VARIANT_FALSE;
		}
		else
		{
			return VARIANT_FALSE;
		}
	}
	catch(...)
	{
		return VARIANT_FALSE;
	}
}

// ************************************************************
//		SerializeMapState()
// ************************************************************
BSTR CMapView::SerializeMapState(VARIANT_BOOL RelativePaths, LPCTSTR BasePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strResult;
	CPLXMLNode* node = SerializeMapStateCore(RelativePaths, BasePath);
	if (node)
	{
		strResult = CPLSerializeXMLTree(node);
	}
	return strResult.AllocSysString();
}

// ************************************************************
//		SerializeMapStateCore()
// ************************************************************
CPLXMLNode* CMapView::SerializeMapStateCore(VARIANT_BOOL RelativePaths, CString ProjectName)
{
	CPLXMLNode* psTree = CPLCreateXMLNode( NULL, CXT_Element, "MapWinGIS");
	if (psTree) 
	{
		// TODO: implement version autoincrement
		// ocx version			
		CString s;
		s.Format("%d.%d", _wVerMajor, _wVerMinor);
		CPLCreateXMLAttributeAndValue( psTree, "OcxVersion", s);
		CPLCreateXMLAttributeAndValue( psTree, "FileType", "MapState");
		CPLCreateXMLAttributeAndValue( psTree, "FileVersion", CPLString().Printf("%d", 0));

		// control options
		CPLXMLNode* psState = CPLCreateXMLNode( NULL, CXT_Element, "MapState");
		if (psState)
		{
			if (m_backColor != RGB(255, 255, 255)) 
				CPLCreateXMLAttributeAndValue(psState, "BackColor", CPLString().Printf("%d", m_backColor));

			if (m_extentPad != 0.02) 
				CPLCreateXMLAttributeAndValue(psState, "ExtentPad", CPLString().Printf("%f", m_extentPad));

			if (m_extentHistory != 20) 
				CPLCreateXMLAttributeAndValue(psState, "ExtentHistory", CPLString().Printf("%d", m_extentHistory));
			
			if (m_doubleBuffer != TRUE) 
				CPLCreateXMLAttributeAndValue(psState, "DoubleBuffer", CPLString().Printf("%d", m_doubleBuffer));

			if (m_sendMouseMove != FALSE) 
				CPLCreateXMLAttributeAndValue(psState, "SendMouseMove", CPLString().Printf("%d", m_sendMouseMove));

			if (m_sendMouseDown != FALSE) 
				CPLCreateXMLAttributeAndValue(psState, "SendMouseDown", CPLString().Printf("%d", m_sendMouseDown));
			
			if (m_sendMouseUp != FALSE) 
				CPLCreateXMLAttributeAndValue(psState, "SendMouseUp", CPLString().Printf("%d", m_sendMouseUp));

			if (m_sendSelectBoxDrag != FALSE) 
				CPLCreateXMLAttributeAndValue(psState, "SendSelectBoxDrag", CPLString().Printf("%d", m_sendSelectBoxDrag));

			if (m_sendSelectBoxFinal != FALSE) 
				CPLCreateXMLAttributeAndValue(psState, "SendSelectBoxFinal", CPLString().Printf("%d", m_sendSelectBoxFinal));

			if (m_sendOnDrawBackBuffer != FALSE) 
				CPLCreateXMLAttributeAndValue(psState, "SendOnDrawBackBuffer", CPLString().Printf("%d", m_sendOnDrawBackBuffer));

			if (m_zoomPercent != 0.3) 
				CPLCreateXMLAttributeAndValue(psState, "ZoomPercent", CPLString().Printf("%f", m_zoomPercent));

			if (m_key != "")
				CPLCreateXMLAttributeAndValue(psState, "Key", m_key);
			
			if (m_cursorMode != 0)
				CPLCreateXMLAttributeAndValue(psState, "CursorMode", CPLString().Printf("%d",(int)m_cursorMode));

			if (m_mapCursor != 0)
				CPLCreateXMLAttributeAndValue(psState, "MapCursor", CPLString().Printf("%d", (int)m_mapCursor));

			if (rbMapResizeBehavior != rbClassic)
				CPLCreateXMLAttributeAndValue(psState, "ResizeBehavior", CPLString().Printf("%d", (int)rbMapResizeBehavior));

			if (DoTrapRMouseDown != FALSE)
				CPLCreateXMLAttributeAndValue(psState, "TrapRightMouseDown", CPLString().Printf("%d", (int)DoTrapRMouseDown));

			if (m_UseSeamlessPan != FALSE)
				CPLCreateXMLAttributeAndValue(psState, "UseSeamlessPan", CPLString().Printf("%d", (int)m_UseSeamlessPan));

			if (m_MouseWheelSpeed != 0.5)
				CPLCreateXMLAttributeAndValue(psState, "MouseWheelSpeed", CPLString().Printf("%f", m_MouseWheelSpeed));

			if (m_ShapeDrawingMethod != dmNewSymbology)
				CPLCreateXMLAttributeAndValue(psState, "ShapeDrawingMethod", CPLString().Printf("%d", (int)m_ShapeDrawingMethod));

			if (m_unitsOfMeasure != umMeters)
				CPLCreateXMLAttributeAndValue(psState, "UnitsOfMeasure", CPLString().Printf("%d", (int)m_unitsOfMeasure));

			if (m_DisableWaitCursor != FALSE)
				CPLCreateXMLAttributeAndValue(psState, "DisableWaitCursor", CPLString().Printf("%d", (int)m_DisableWaitCursor));

			if (m_RotateAngle != 0.0f)
				CPLCreateXMLAttributeAndValue(psState, "RotationAngle", CPLString().Printf("%f", m_RotateAngle));
			
			if (_canUseImageGrouping != FALSE)
				CPLCreateXMLAttributeAndValue(psState, "CanUseImageGrouping", CPLString().Printf("%d", (int)_canUseImageGrouping));

			if (m_ShowRedrawTime != FALSE)
				CPLCreateXMLAttributeAndValue(psState, "ShowRedrawTime", CPLString().Printf("%d", (int)m_ShowRedrawTime));

			if (m_ShowVersionNumber != FALSE)
				CPLCreateXMLAttributeAndValue(psState, "ShowVersionNumber", CPLString().Printf("%d", (int)m_ShowVersionNumber));

			CPLCreateXMLAttributeAndValue(psState, "ExtentsLeft", CPLString().Printf("%f", extents.left));
			CPLCreateXMLAttributeAndValue(psState, "ExtentsRight", CPLString().Printf("%f", extents.right));
			CPLCreateXMLAttributeAndValue(psState, "ExtentsBottom", CPLString().Printf("%f", extents.bottom));
			CPLCreateXMLAttributeAndValue(psState, "ExtentsTop", CPLString().Printf("%f", extents.top));
			CPLCreateXMLAttributeAndValue(psState, "ExtentsPad", CPLString().Printf("%f", m_extentPad));
			
			CPLAddXMLChild(psTree, psState);

			// layer options
			CPLXMLNode* psLayers = CPLCreateXMLNode( NULL, CXT_Element, "Layers");
			if (psLayers)
			{
				for(unsigned int i = 0; i < m_activeLayers.size(); i++ )
				{	
					LONG handle = m_activeLayers[i];

					if (!this->GetLayerSkipOnSaving(handle))
					{
						// getting relative name
						CComBSTR layerName;
						layerName = this->GetLayerFilename(handle);
						
						USES_CONVERSION;
						CString tempLayerName = OLE2CA(layerName);
						if (_stricmp(tempLayerName, "") == 0)
						{
							// in-memory layer, it won't be saved
						}
						else
						{
							CString name = OLE2CA(layerName);
							if (RelativePaths)
							{
								name = Utility::GetRelativePath(ProjectName, name);
							}
							CPLXMLNode* node = this->SerializeLayerCore(handle, name);
							if (node)
							{
								// saving grid name
								BSTR s = this->GetGridFileName(handle);
								CString gridFilename = OLE2CA(s);
								if (gridFilename != "")
								{
									LPDISPATCH obj = this->GetColorScheme(handle);
									if (obj)
									{
										IGridColorScheme* scheme = NULL;
										obj->QueryInterface(IID_IGridColorScheme,(void**)&scheme);
										if (scheme)
										{
											CPLXMLNode* nodeScheme = ((CGridColorScheme*)scheme)->SerializeCore("GridColorSchemeClass");
											if (nodeScheme)
											{
												CPLAddXMLChild(node, nodeScheme);

												// it is grid layer
												CPLCreateXMLAttributeAndValue(node, "GridFilename", gridFilename);
											}
											scheme->Release();
										}
										obj->Release();
									}
								}
								CPLAddXMLChild(psLayers, node);	
							}
						}
					}
				}
				CPLAddXMLChild(psTree, psLayers);
			}
		}
	}
	return psTree;
}

// ************************************************************
//		GetMapState()
// ************************************************************
BSTR CMapView::GetMapState()
{
	USES_CONVERSION;
	CString mapState = "";

	if (this->m_ShapeDrawingMethod == dmNewSymbology)
	{
		CPLXMLNode* node = SerializeMapStateCore(VARIANT_FALSE,"");
		CString str = CPLSerializeXMLTree(node);	
		return A2BSTR(str);
	}
	else
	{
		//MAP Clause
		CString map;
		map.Format("&MAP&;%f;%f;%f;%f;%f;%f;%s;%i;%i;%i;",
						extents.left,
						extents.right,
						extents.bottom,
						extents.top,
						m_extentPad,
						m_zoomPercent,
						m_key,
						m_cursorMode,
						m_mapCursor,
						m_activeLayers.size() );
		mapState = map;

		//LAYER Clause
		register int i;
		for( i = 0; i < m_activeLayers.size(); i++ )
		{	Layer * l = m_allLayers[m_activeLayers[i]];
			CString layer;
			if( l->type == ShapefileLayer )
			{	IShapefile * ishp = NULL;
				l->object->QueryInterface(IID_IShapefile,(void**)&ishp);

				BSTR fname;
				ishp->get_Filename(&fname);
				layer.Format("&LAYER&;%s;%i;%i;%i;%i;%i;%s;%s;%i;%i;",
								OLE2A(fname),
								m_activeLayers[i],
								i,
								ShapefileLayer,
								l->flags & Visible,
								l->visibleLabels,
								OLE2A(l->key),
								l->fontName,
								l->fontSize,
								l->allLabels.size());
				::SysFreeString(fname);
				mapState += layer;

				register int j;
				for( j = 0; j < l->allLabels.size(); j++ )
				{
					//LABEL Clause
					CString label;
					label.Format("&LABEL&;%s;%f;%f;%i;%i;",
									l->allLabels[j].text,
									l->allLabels[j].x,
									l->allLabels[j].y,
									l->allLabels[j].color,
									l->allLabels[j].justif);
					mapState += label;
				}

				ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
				CString udPointTypeImageFilename;
				OLE_COLOR udITransparentColor = 0;
				VARIANT_BOOL udIUseTransparentColor = FALSE;
				if( sli->udPointType != NULL )
				{	BSTR ifname;
					sli->udPointType->get_Filename(&ifname);
					sli->udPointType->get_TransparencyColor(&udITransparentColor);
					sli->udPointType->get_UseTransparencyColor(&udIUseTransparentColor);
					udPointTypeImageFilename = OLE2A(ifname);
					::SysFreeString(ifname);
				}
				CString udFillStipple;
				udFillStipple.Format("%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i",
									  GetUDFillStipple(m_activeLayers[i],0),
									  GetUDFillStipple(m_activeLayers[i],1),
									  GetUDFillStipple(m_activeLayers[i],2),
									  GetUDFillStipple(m_activeLayers[i],3),
									  GetUDFillStipple(m_activeLayers[i],4),
									  GetUDFillStipple(m_activeLayers[i],5),
									  GetUDFillStipple(m_activeLayers[i],6),
									  GetUDFillStipple(m_activeLayers[i],7),
									  GetUDFillStipple(m_activeLayers[i],8),
									  GetUDFillStipple(m_activeLayers[i],9),
									  GetUDFillStipple(m_activeLayers[i],10),
									  GetUDFillStipple(m_activeLayers[i],11),
									  GetUDFillStipple(m_activeLayers[i],12),
									  GetUDFillStipple(m_activeLayers[i],13),
									  GetUDFillStipple(m_activeLayers[i],14),
									  GetUDFillStipple(m_activeLayers[i],15),
									  GetUDFillStipple(m_activeLayers[i],16),
									  GetUDFillStipple(m_activeLayers[i],17),
									  GetUDFillStipple(m_activeLayers[i],18),
									  GetUDFillStipple(m_activeLayers[i],19),
									  GetUDFillStipple(m_activeLayers[i],20),
									  GetUDFillStipple(m_activeLayers[i],21),
									  GetUDFillStipple(m_activeLayers[i],22),
									  GetUDFillStipple(m_activeLayers[i],23),
									  GetUDFillStipple(m_activeLayers[i],24),
									  GetUDFillStipple(m_activeLayers[i],25),
									  GetUDFillStipple(m_activeLayers[i],26),
									  GetUDFillStipple(m_activeLayers[i],27),
									  GetUDFillStipple(m_activeLayers[i],28),
									  GetUDFillStipple(m_activeLayers[i],29),
									  GetUDFillStipple(m_activeLayers[i],30),
									  GetUDFillStipple(m_activeLayers[i],31));
				CString layerSpecific;
				layerSpecific.Format("&LAYERSPECIFIC&;%i;%i;%i;%s;%i;%f;%i;%i;%i;%i;%f;%i;%i;%s;%i;%i;",
										sli->fillClr,
										sli->fillStipple,
										sli->flags & slfDrawFill,
										udFillStipple,
										sli->lineClr,
										sli->lineWidth,
										sli->lineStipple,
										sli->udLineStipple,
										sli->flags & slfDrawLine,
										sli->pointClr,
										sli->pointSize,
										sli->pointType,
										sli->flags & slfDrawPoint,
										udPointTypeImageFilename,
										udITransparentColor,
										udIUseTransparentColor);
				mapState += layerSpecific;

				//LEGEND Clause
				if( sli->legend != NULL )
				{
					long fi = 0, numBreaks;
					BSTR lkey;
					sli->legend->get_FieldIndex(&fi);
					sli->legend->NumBreaks(&numBreaks);
					sli->legend->get_Key(&lkey);

					if( numBreaks > 0 )
					{	CString legend;
						legend.Format("&LEGEND&;%i;%s;%i;",
										fi,
										OLE2A(lkey),
										numBreaks);

						mapState += legend;

						//LEGENDBREAK Clause
						register int k;
						for( k = 0; k < numBreaks; k++ )
						{
							IShapefileColorBreak * lb = NULL;
							sli->legend->get_ColorBreak(k,&lb);

							VARIANT sv, ev;
							VariantInit(&sv); //added by Rob Cairns 4-Jan-06
							VariantInit(&ev); //added by Rob Cairns 4-Jan-06
							OLE_COLOR sc, ec;
							lb->get_StartValue(&sv);
							lb->get_EndValue(&ev);
							lb->get_StartColor(&sc);
							lb->get_EndColor(&ec);

							CString ssv, sev;
							stringVal(sv,ssv);
							stringVal(ev,sev);

							CString legendBreak;
							legendBreak.Format("&LEGENDBREAK&;%s;%s;%i;%i;",
													ssv,
													sev,
													sc,
													ec);

							mapState += legendBreak;
							lb->Release();
							VariantClear(&sv); //added by Rob Cairns 4-Jan-06
							VariantClear(&ev); //added by Rob Cairns 4-Jan-06
						}
					}

					::SysFreeString(lkey);
				}

				ishp->Release();
				ishp = NULL;
			}
			else if( l->type == ImageLayer )
			{	IImage * iimg = NULL;
				l->object->QueryInterface(IID_IImage,(void**)&iimg);

				BSTR fname;
				iimg->get_Filename(&fname);
				layer.Format("&LAYER&;%s;%i;%i;%i;%i;%i;%s;%s;%i;%i;",
								OLE2A(fname),
								m_activeLayers[i],
								i,
								ImageLayer,
								l->flags & Visible,
								l->visibleLabels,
								OLE2A(l->key),
								l->fontName,
								l->fontSize,
								l->allLabels.size());
				::SysFreeString(fname);
				mapState += layer;

				register int j;
				for( j = 0; j < l->allLabels.size(); j++ )
				{	if( l->allLabels[j].text.GetLength() <= 0 )
						continue;

					//LABEL Clause
					CString label;
					label.Format("&LABEL&;%s;%f;%f;%i;%i;",
									l->allLabels[j].text,
									l->allLabels[j].x,
									l->allLabels[j].y,
									l->allLabels[j].color,
									l->allLabels[j].justif);
					mapState += label;
				}

				//LAYERSPECIFIC Clause
				ImageLayerInfo * ili = (ImageLayerInfo*)l->addInfo;
				OLE_COLOR tc = 0;
				VARIANT_BOOL utc = FALSE;
				double transparencyPercent;
				iimg->get_TransparencyColor(&tc);
				iimg->get_UseTransparencyColor(&utc);
				iimg->get_TransparencyPercent(&transparencyPercent);

				CString layerSpecific;
				layerSpecific.Format("&LAYERSPECIFIC&;%i;%i;%f;",
										tc,
										utc,
										transparencyPercent);

				mapState += layerSpecific;
				iimg->Release();
				iimg = NULL;
			}
		}
	}
	return mapState.AllocSysString();
}

// ******************************************************************
//	   DeserializeMapStateCore()
// ******************************************************************
VARIANT_BOOL CMapView::DeserializeMapState(LPCTSTR State, VARIANT_BOOL LoadLayers, LPCTSTR BasePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPLXMLNode* node = CPLParseXMLString(State);
	if (node)
	{
		return DeserializeMapStateCore(node, BasePath, LoadLayers, NULL) ? VARIANT_TRUE : VARIANT_FALSE;
	}
	return VARIANT_TRUE;
}

// ******************************************************************
//	   DeserializeMapStateCore()
// ******************************************************************
bool CMapView::DeserializeMapStateCore(CPLXMLNode* node, CString ProjectName, VARIANT_BOOL LoadLayers, IStopExecution* callback)
{
	if (!node)
	{
		ErrorMessage(tkINVALID_FILE);
		return false;
	}
	
	if (_stricmp( node->pszValue, "MapWinGIS") != 0)
	{
		// it can be MW4 project file, then MapWinGis should be the first child node
		CPLXMLNode* nodeChild = node->psChild;
		while (nodeChild)
		{
			if (_stricmp( nodeChild->pszValue, "MapWinGIS") == 0)
			{
				// we got it
				node = nodeChild;
				break;
			}
			nodeChild = nodeChild->psNext;
		}
		
		// check once again, if it wasn't find - abandon it
		if (_stricmp( node->pszValue, "MapWinGIS") != 0)
		{
			ErrorMessage(tkINVALID_FILE);
			return false;
		}
	}
	
	CPLXMLNode* nodeState = CPLGetXMLNode(node, "MapState");
	CPLXMLNode* nodeLayers = CPLGetXMLNode(node, "Layers");
	 
	if (!nodeState || !nodeLayers)
	{
		ErrorMessage(tkINVALID_FILE);
		return false;
	}

	// control options
	CString s;
	s = CPLGetXMLValue( nodeState, "BackColor", NULL );
	m_backColor = (s != "") ? (OLE_COLOR)atoi(s.GetString()) : RGB(255, 255, 255);
	
	s = CPLGetXMLValue( nodeState, "ExtentPad", NULL );
	m_extentPad = (s != "") ? atoi(s) : 0.02;

	s = CPLGetXMLValue( nodeState, "ExtentHistory", NULL );
	m_extentHistory = (s != "") ? atoi(s) : 20;

	s = CPLGetXMLValue( nodeState, "DoubleBuffer", NULL );
	m_doubleBuffer = (s != "") ? (BOOL)atoi(s) : TRUE;

	s = CPLGetXMLValue( nodeState, "SendMouseMove", NULL );
	m_sendMouseMove = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendMouseDown", NULL );
	m_sendMouseDown = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendMouseUp", NULL );
	m_sendMouseUp = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendSelectBoxDrag", NULL );
	m_sendSelectBoxDrag = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "SendSelectBoxFinal", NULL );
	m_sendSelectBoxFinal = (s != "") ? (BOOL)atoi(s) : FALSE;
	
	s = CPLGetXMLValue( nodeState, "SendOnDrawBackBuffer", NULL );
	m_sendOnDrawBackBuffer = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "ZoomPercent", NULL );
	m_cursorMode = (s != "") ? (float)Utility::atof_custom(s) : 0.3;
	
	m_key = CPLGetXMLValue( nodeState, "Key", NULL );
	
	s = CPLGetXMLValue( nodeState, "CursorMode", NULL );
	m_cursorMode = (s != "") ? (short)atoi(s) : cmZoomIn;

	s = CPLGetXMLValue( nodeState, "MapCursor", NULL );
	m_mapCursor = (s != "") ? (short)atoi(s) : crsrMapDefault;

	s = CPLGetXMLValue( nodeState, "ResizeBehavior", NULL );
	rbMapResizeBehavior = (s != "") ? (tkResizeBehavior)atoi(s) : rbClassic;

	s = CPLGetXMLValue( nodeState, "TrapRightMouseDown", NULL );
	DoTrapRMouseDown = (s != "") ? (BOOL)atoi(s) : FALSE;
	
	s = CPLGetXMLValue( nodeState, "UseSeamlessPan", NULL );
	m_UseSeamlessPan = (s != "") ? (BOOL)atoi(s) : FALSE;
	
	s = CPLGetXMLValue( nodeState, "MouseWheelSpeed", NULL );
	m_MouseWheelSpeed = (s != "") ? Utility::atof_custom(s) : 0.5;

	s = CPLGetXMLValue( nodeState, "ShapeDrawingMethod", NULL );
	m_ShapeDrawingMethod = (s != "") ? (tkShapeDrawingMethod)atoi(s) : dmNewSymbology;

	s = CPLGetXMLValue( nodeState, "UnitsOfMeasure", NULL );
	m_unitsOfMeasure = (s != "") ? (tkUnitsOfMeasure)atoi(s) : umMeters;

	s = CPLGetXMLValue( nodeState, "DisableWaitCursor", NULL );
	m_DisableWaitCursor = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "RotationAngle", NULL );
	m_RotateAngle = (s != "") ? (float)Utility::atof_custom(s) : 0.0;

	s = CPLGetXMLValue( nodeState, "CanUseImageGrouping", NULL );
	_canUseImageGrouping = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "ShowRedrawTime", NULL );
	m_ShowRedrawTime = (s != "") ? (BOOL)atoi(s) : FALSE;

	s = CPLGetXMLValue( nodeState, "ShowVersionNumber", NULL );
	m_ShowVersionNumber = (s != "") ? (BOOL)atoi(s) : FALSE;

	if (LoadLayers)
	{
		// processing layers
		this->RemoveAllLayers();

		char* cwd = new char[4096];
		_getcwd(cwd,4096);
		
		ProjectName = Utility::GetFolderFromPath(ProjectName);
		_chdir(ProjectName);

		CPLXMLNode* nodeLayer = nodeLayers->psChild;
		if (nodeLayer)
		{
			while (nodeLayer)
			{
				if (_stricmp(nodeLayer->pszValue, "Layer") == 0)
				{
					int handle = DeserializeLayerCore( nodeLayer, ProjectName, callback);
					
					if (handle != -1)
					{
						s = CPLGetXMLValue( nodeState, "GridFilename", NULL );
						if (s != "")
						{
							node = CPLGetXMLNode(nodeState, "GridColorSchemeClass");
							if (node)
							{
								IGridColorScheme* scheme = NULL;
								CoCreateInstance(CLSID_GridColorScheme,NULL,CLSCTX_INPROC_SERVER,IID_IGridColorScheme,(void**)&scheme);
								if (scheme)
								{
									if (((CGridColorScheme*)scheme)->DeserializeCore(node))
									{
										this->SetGridFileName(handle, s);
										this->SetImageLayerColorScheme(handle, (IDispatch*)scheme);
									}
								}
							}
						}
					}
				}
				nodeLayer = nodeLayer->psNext;
			}
		}
		_chdir(cwd);
	}

	// extents
	s = CPLGetXMLValue( nodeState, "ExtentsLeft", NULL );
	if (s != "") extents.left = Utility::atof_custom(s);

	s = CPLGetXMLValue( nodeState, "ExtentsRight", NULL );
	if (s != "") extents.right = Utility::atof_custom(s);
	
	s = CPLGetXMLValue( nodeState, "ExtentsBottom", NULL );
	if (s != "") extents.bottom = Utility::atof_custom(s);

	s = CPLGetXMLValue( nodeState, "ExtentsTop", NULL );
	if (s != "") extents.top = Utility::atof_custom(s);

	s = CPLGetXMLValue( nodeState, "ExtentsPad", NULL );
	if (s != "") m_extentPad = Utility::atof_custom(s);

	CalculateVisibleExtents(extents, false);
	FireExtentsChanged();

	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();

	ReloadImageBuffers();
	return true;
}

// ******************************************************************
//	   SetMapState()
// ******************************************************************
// TODO: write support for the new symbology
void CMapView::SetMapState(LPCTSTR lpszNewValue)
{
	if (this->m_ShapeDrawingMethod == dmNewSymbology)
	{
		CString s = lpszNewValue;
		CPLXMLNode* node = CPLParseXMLString(s.GetString());
		DeserializeMapStateCore(node, "", VARIANT_TRUE, NULL);
	}
	else
	{
		m_mapstateMutex.Lock();

		//Tokenize the String
		CString mapState = lpszNewValue;
		mapState.TrimLeft("\"");
		mapState.TrimRight("\"");

		std::vector<CString> stringToken;
		while( mapState.GetLength() > 0 )
		{	long found = mapState.Find(";",0);
			if( found >= 0 )
			{	
				CString value = mapState.Left( found );
				stringToken.push_back( value );
			}
			else
			{
				mapState = "";
			}
			mapState = mapState.Right( mapState.GetLength() - found - 1 );
		}

		long SIZE_MAP_CLAUSE = 11;
		long SIZE_LAYER_CLAUSE = 11;
		long SIZE_LABEL_CLAUSE = 6;
		long SIZE_LAYER_SPECIFIC_CLAUSE_IMAGE = 4;
		long SIZE_LAYER_SPECIFIC_CLAUSE_SHAPEFILE = 48;
		long SIZE_LEGEND_CLAUSE = 4;
		long SIZE_LEGENDBREAK_CLAUSE = 5;

		//Verify the map string
		if( stringToken.size() < SIZE_MAP_CLAUSE || stringToken[0].CompareNoCase("&MAP&") != 0 )
		{	
			ErrorMessage(tkMAP_INVALID_MAPSTATE);
			m_mapstateMutex.Unlock();
			return;
		}

		long vcnt = 10;
		long numLayers = atoi(stringToken[vcnt]);
		for(int i = 0; i < numLayers; i++ )
		{
			if( vcnt + SIZE_LAYER_CLAUSE >= stringToken.size() || stringToken[vcnt + 1].CompareNoCase("&LAYER&") != 0 )
			{	
				ErrorMessage(tkMAP_INVALID_MAPSTATE);
				m_mapstateMutex.Unlock();
				return;
			}

			LayerType layerType = (LayerType)atoi(stringToken[vcnt + 5]);
			vcnt += SIZE_LAYER_CLAUSE;

			long numLabels = atoi(stringToken[vcnt]);
			for(int j = 0; j < numLabels; j++ )
			{
				if( vcnt + SIZE_LABEL_CLAUSE >= stringToken.size() || stringToken[vcnt + 1].CompareNoCase("&LABEL&") != 0 )
				{	
					ErrorMessage(tkMAP_INVALID_MAPSTATE);
					m_mapstateMutex.Unlock();
					return;
				}
				vcnt += SIZE_LABEL_CLAUSE;
			}

			if( layerType == ShapefileLayer )
			{	
				if( vcnt + SIZE_LAYER_SPECIFIC_CLAUSE_SHAPEFILE >= stringToken.size() || stringToken[vcnt + 1].CompareNoCase("&LAYERSPECIFIC&") != 0 )
				{	
					ErrorMessage(tkMAP_INVALID_MAPSTATE);
					m_mapstateMutex.Unlock();
					return;
				}

				vcnt += SIZE_LAYER_SPECIFIC_CLAUSE_SHAPEFILE;

				if( vcnt + 1 < stringToken.size() )
				{	
					if( stringToken[vcnt + 1].CompareNoCase("&LEGEND&") == 0 )
					{
						if( vcnt + SIZE_LEGEND_CLAUSE >= stringToken.size() || stringToken[vcnt + 1].CompareNoCase("&LEGEND&") != 0 )
						{	
							ErrorMessage(tkMAP_INVALID_MAPSTATE);
							m_mapstateMutex.Unlock();
							return;
						}

						vcnt += SIZE_LEGEND_CLAUSE;

						//NumBreaks
						long numLegendBreaks = atoi(stringToken[vcnt]);

						register int k;
						for( k = 0; k < numLegendBreaks; k++ )
						{
							if( vcnt + SIZE_LEGENDBREAK_CLAUSE >= stringToken.size() || stringToken[vcnt + 1].CompareNoCase("&LEGENDBREAK&") != 0 )
							{	
								ErrorMessage(tkMAP_INVALID_MAPSTATE);
								m_mapstateMutex.Unlock();
								return;
							}

							vcnt += SIZE_LEGENDBREAK_CLAUSE;
						}
					}
				}
			}
			else if( layerType == ImageLayer )
			{	
				if( vcnt + SIZE_LAYER_SPECIFIC_CLAUSE_IMAGE >= stringToken.size() || stringToken[vcnt + 1].CompareNoCase("&LAYERSPECIFIC&") != 0 )
				{	
					ErrorMessage(tkMAP_INVALID_MAPSTATE);
					m_mapstateMutex.Unlock();
					return;
				}

				vcnt += SIZE_LAYER_SPECIFIC_CLAUSE_IMAGE;
			}
			else
			{	
				ErrorMessage(tkMAP_INVALID_MAPSTATE);
				m_mapstateMutex.Unlock();
				return;
			}
		}

		if( vcnt + 1 != stringToken.size() )
		{
			ErrorMessage(tkMAP_INVALID_MAPSTATE);
			m_mapstateMutex.Unlock();
			return;
		}

		//Populate the Map
		VARIANT_BOOL retval = FALSE;

		//Lock the Window
		LockWindow(lmLock);

		RemoveAllLayers();

		std::deque<long> desiredLayerHandle;
		std::deque<long> desiredLayerPosition;
		std::deque<long> actualLayerHandle;
		long largestLayerHandle = 0;

		m_extentPad = atof(stringToken[5]);
		m_zoomPercent = atof(stringToken[6]);
		m_key = stringToken[7];
		m_cursorMode = atoi(stringToken[8]);
		m_mapCursor = atoi(stringToken[9]);
		numLayers = atoi(stringToken[10]);

		long cnt = 10;
		register int pmi;

		for( pmi = 0; pmi < numLayers; pmi++ )
		{
			//AfxMessageBox("Before");
			LayerType layerType = (LayerType)atoi(stringToken[cnt + 5]);

			if( layerType == ShapefileLayer )
			{
				//AfxMessageBox("LT = ShapefileLayer");

				IShapefile * ishp = NULL;
				CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)&ishp);

				cnt++;	//Move to &LAYER&
				cnt++;	//Move to Filename
				ishp->Open(stringToken[cnt].AllocSysString(),m_globalCallback,&retval);

				if( retval == FALSE )
				{
					m_lastErrorCode = tkMAP_MAPSTATE_LAYER_LOAD_FAILED;
					if( m_globalCallback != NULL )
					{	CString layerFailedMsg = "Failed to open:" + stringToken[cnt];
						m_globalCallback->Error(m_key.AllocSysString(),layerFailedMsg.AllocSysString());
					}

					//Move to the position before the next &LAYER&
					for( ; cnt < stringToken.size(); cnt++ )
					{	if( stringToken[cnt].CompareNoCase("&LAYER&") == 0 )
						{	cnt--;
							break;
						}
					}
				}
				else
				{
					IDispatch * idisp = NULL;
					ishp->QueryInterface(IID_IDispatch,(void**)&idisp);

					cnt++; //Move to LayerHandle
					if( atoi(stringToken[cnt]) > largestLayerHandle )
						largestLayerHandle = atoi(stringToken[cnt]);
					desiredLayerHandle.push_back(atoi(stringToken[cnt]));
					cnt++; //Move to LayerPosition
					desiredLayerPosition.push_back(atoi(stringToken[cnt]));
					cnt++; //Move to LayerType
					cnt++; //Move to Visible

					long layerHandle = AddLayer(idisp,atoi(stringToken[cnt]));

					actualLayerHandle.push_back(layerHandle);

					cnt++; //Move to LabelsVisible
					SetLayerLabelsVisible(layerHandle,atoi(stringToken[cnt]));

					cnt++; //Move to Key
					SetLayerKey(layerHandle,stringToken[cnt]);

					cnt++; //Move to Fontname
					LayerFont(layerHandle,stringToken[cnt],atoi(stringToken[cnt + 1]));
					cnt++; //Move to FontSize

					idisp->Release();
					idisp = NULL;

					//Labels
					cnt++; //Move to NumLabels
					long numLabels = atoi(stringToken[cnt]);
					register int pmj;
					for( pmj = 0; pmj < numLabels; pmj++ )
					{	cnt++; //Move to &LABEL&
						cnt++; //Move to Text

						AddLabel(layerHandle,stringToken[cnt],atoi(stringToken[cnt+3]),atof(stringToken[cnt+1]),atof(stringToken[cnt+2]),atoi(stringToken[cnt+4]));

						cnt++; //Move to X
						cnt++; //Move to Y
						cnt++; //Move to Color
						cnt++; //Move to Justif
					}


					//Layer Specific
					cnt++; //Move to &LAYERSPECIFIC&
					cnt++; //Move to Fill Color
					SetShapeLayerFillColor(layerHandle,atoi(stringToken[cnt]));
					cnt++; //Move to Fill Stipple
					SetShapeLayerFillStipple(layerHandle,atoi(stringToken[cnt]));
					cnt++; //Move to Draw Fill
					SetShapeLayerDrawFill(layerHandle,atoi(stringToken[cnt]));
					register int udfs;
					for( udfs = 0; udfs < 32; udfs++ )
					{	cnt++; //Move to Next Fill Stipple Row
						SetUDFillStipple(layerHandle,udfs,atoi(stringToken[cnt]));
					}

					cnt++; //Move to Line Color
					SetShapeLayerLineColor(layerHandle,atoi(stringToken[cnt]));
					cnt++; //Move to Line Width
					SetShapeLayerLineWidth(layerHandle,atof(stringToken[cnt]));
					cnt++; //Move to Line Stipple
					SetShapeLayerLineStipple(layerHandle,atoi(stringToken[cnt]));
					cnt++; //Move to UDLine Stipple
					SetUDLineStipple(layerHandle,atoi(stringToken[cnt]));
					cnt++; //Move to Draw Line
					SetShapeLayerDrawLine(layerHandle,atoi(stringToken[cnt]));

					cnt++; //Move to Point Color
					SetShapeLayerPointColor(layerHandle,atoi(stringToken[cnt]));
					cnt++; //Move to Point Size
					SetShapeLayerPointSize(layerHandle,atof(stringToken[cnt]));
					cnt++; //Move to Point Type
					SetShapeLayerPointType(layerHandle,atoi(stringToken[cnt]));
					cnt++; //Move to Draw Point
					SetShapeLayerDrawPoint(layerHandle,atoi(stringToken[cnt]));

					//AfxMessageBox("Create Custom Image");

					//Set up the user-defined Point Type
					cnt++; //Move to UDPointTypeFilename
					IImage * iimg = NULL;
					CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimg);

					iimg->Open(stringToken[cnt].AllocSysString(),USE_FILE_EXTENSION,FALSE,m_globalCallback,&retval);
					cnt++; //Move to Transparent Color
					iimg->put_TransparencyColor(atoi(stringToken[cnt]));
					cnt++; //Move to Use Transparent Color
					iimg->put_UseTransparencyColor(atoi(stringToken[cnt]));

					if( retval != FALSE )
					{	IDispatch * idisp = NULL;
						iimg->QueryInterface(IID_IDispatch,(void**)&idisp);
						SetUDPointType(layerHandle,idisp);
						idisp->Release();
						idisp = NULL;
					}

					iimg->Release();
					iimg = NULL;

					//AfxMessageBox("End Create Custom Image");

					//Legend
					if( cnt + 1 < stringToken.size() && stringToken[cnt+1].CompareNoCase("&LEGEND&") == 0 )
					{
						//AfxMessageBox("Create Legend");

						cnt++; //Move to &LEGEND&

						IShapefileColorScheme * ilegend = NULL;
						CoCreateInstance(CLSID_ShapefileColorScheme,NULL,CLSCTX_INPROC_SERVER,IID_IShapefileColorScheme,(void**)&ilegend);

						cnt++; //Move to FieldIndex
						ilegend->put_FieldIndex(atoi(stringToken[cnt]));
						cnt++; //Move to Key
						ilegend->put_Key(stringToken[cnt].AllocSysString());
						cnt++; //Move to Num LegendBreaks
						long numLegendBreaks = atoi(stringToken[cnt]);

						ilegend->put_LayerHandle(layerHandle);

						register int nlb;
						for( nlb = 0; nlb < numLegendBreaks; nlb++ )
						{
							IShapefileColorBreak * lb = NULL;
							CoCreateInstance(CLSID_ShapefileColorBreak,NULL,CLSCTX_INPROC_SERVER,IID_IShapefileColorBreak,(void**)&lb);

							cnt++; //Move to &LEGENDBREAK&
							cnt++; //Move to StartValue

							//AfxMessageBox("VARIANT sv");

							VARIANT sv;
							VariantInit(&sv);
							sv.vt = VT_BSTR;
							sv.bstrVal = stringToken[cnt].AllocSysString();
							lb->put_StartValue(sv);
							VariantClear(&sv);
							cnt++; //Move to EndValue

							//AfxMessageBox("Between VARIANT sv & ev");

							VARIANT ev;
							VariantInit(&ev);
							ev.vt = VT_BSTR;
							ev.bstrVal = stringToken[cnt].AllocSysString();
							lb->put_EndValue(ev);
							VariantClear(&ev);

							//AfxMessageBox("End VARIANT ev");

							cnt++; //Move to StartColor
							lb->put_StartColor(atoi(stringToken[cnt]));
							cnt++; //Move to EndColor
							lb->put_EndColor(atoi(stringToken[cnt]));

							//AfxMessageBox("Add Break");

							long res = 0;
							ilegend->Add(lb,&res);

							lb->Release();
							lb = NULL;

							//AfxMessageBox("Release Break");
						}

						//AfxMessageBox("Before");

						IDispatch * ildisp = NULL;
						ilegend->QueryInterface(IID_IDispatch,(void**)&ildisp);

						//AfxMessageBox("Apply Legend Colors");

						ApplyLegendColors(ildisp);

						//AfxMessageBox("Apply Legend Colors");

						ildisp->Release();
						ildisp = NULL;
						ilegend->Release();
						ilegend = NULL;

						//AfxMessageBox("After");
					}
				}

				ishp->Release();
				ishp = NULL;

				//AfxMessageBox("End ShapefileLayer");

				//AfxMessageBox("After");
			}
			else if( layerType == ImageLayer )
			{	IImage * iimg = NULL;
				CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimg);

				cnt++; //Move to &LAYER&
				cnt++; //Move to Filename

				iimg->Open(stringToken[cnt].AllocSysString(),USE_FILE_EXTENSION, FALSE, m_globalCallback,&retval);

				if( retval == FALSE )
				{	m_lastErrorCode = tkMAP_MAPSTATE_LAYER_LOAD_FAILED;
					if( m_globalCallback != NULL )
					{	CString layerFailedMsg = "Failed to open:" + stringToken[cnt];
						m_globalCallback->Error(m_key.AllocSysString(),layerFailedMsg.AllocSysString());
					}
					//Move to the position before the next &LAYER&
					for( ; cnt < stringToken.size(); cnt++ )
					{	if( stringToken[cnt].CompareNoCase("&LAYER&") == 0 )
						{	cnt--;
							break;
						}
					}
				}
				else
				{	IDispatch * idisp = NULL;
					iimg->QueryInterface(IID_IDispatch,(void**)&idisp);

					cnt++; //Move to LayerHandle
					if( atoi(stringToken[cnt]) > largestLayerHandle )
						largestLayerHandle = atoi(stringToken[cnt]);
					desiredLayerHandle.push_back(atoi(stringToken[cnt]));
					cnt++; //Move to LayerPosition
					desiredLayerPosition.push_back(atoi(stringToken[cnt]));
					cnt++; //Move to LayerType
					cnt++; //Move to Visible
					long layerHandle = AddLayer(idisp,atoi(stringToken[cnt]));
					actualLayerHandle.push_back(layerHandle);

					cnt++; //Move to LabelsVisible
					SetLayerLabelsVisible(layerHandle,atoi(stringToken[cnt]));

					cnt++; //Move to Key
					SetLayerKey(layerHandle,stringToken[cnt]);

					cnt++; //Move to Fontname
					LayerFont(layerHandle,stringToken[cnt],atoi(stringToken[cnt + 1]));
					cnt++; //Move to FontSize

					idisp->Release();
					idisp = NULL;

					//Labels
					cnt++; //Move to NumLabels
					long numLabels = atoi(stringToken[cnt]);
					register int pmj;
					for( pmj = 0; pmj < numLabels; pmj++ )
					{	cnt++; //Move to &LABEL&
						cnt++; //Move to Text

						AddLabel(layerHandle,stringToken[cnt],atoi(stringToken[cnt+3]),atof(stringToken[cnt+1]),atof(stringToken[cnt+2]),atoi(stringToken[cnt+4]));

						cnt++; //Move to X
						cnt++; //Move to Y
						cnt++; //Move to Color
						cnt++; //Move to Justif
					}

					//Layer Specific
					cnt++; //Move to &LAYERSPECIFIC&
					cnt++; //Move to Transparent Color
					iimg->put_TransparencyColor(atoi(stringToken[cnt]));
					cnt++; //Move to Use Transparent Color
					iimg->put_UseTransparencyColor(atoi(stringToken[cnt]));
					cnt++; //Move to Transluency
					SetImageLayerPercentTransparent(layerHandle,atof(stringToken[cnt]));

					m_numImages++;
				}
				iimg->Release();
				iimg = NULL;
			}
			else
			{
				//Move to the position before the next &LAYER&
				long endcondition = stringToken.size();
				for( ; cnt < endcondition; cnt++ )
				{	if( stringToken[cnt].CompareNoCase("&LAYER&") == 0 )
					{	cnt--;
						break;
					}
				}
			}
		}


		//AfxMessageBox("Adjust LayerHandles");

		//Adjust LayerHandles and Positions
		std::deque<Layer *> tmpLayers;	//array of Layers used for moving layers around
		std::deque<long> tmpPositions; //array of handles (array order is drawing order)
		register int sl, sp, lh, fp;
		for( sl = 0; sl < largestLayerHandle + 1; sl++ )
			tmpLayers.push_back(NULL);
		for( sp = 0; sp < m_activeLayers.size(); sp++ )
			tmpPositions.push_back(-1);

		for( lh = 0; lh < desiredLayerHandle.size(); lh++ )
		{
			//move the layer from the actual location to the desired location in the layer list
			tmpLayers[desiredLayerHandle[lh]] = m_allLayers[actualLayerHandle[lh]];

			//Change the layerHandle in the legend
			Layer * l = tmpLayers[desiredLayerHandle[lh]];
			if( l->type == ShapefileLayer )
			{	ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
				if( sli->legend )
					sli->legend->put_LayerHandle(desiredLayerHandle[lh]);
			}

			if( tmpPositions[desiredLayerPosition[lh]] == -1 )//position not taken yet
				tmpPositions[desiredLayerPosition[lh]] = desiredLayerHandle[lh];

			else //position was already taken, search for next empty position
			{	for( fp = 0; fp < tmpPositions.size(); fp++ )
				{	if( tmpPositions[fp] == -1 )
					{	tmpPositions[fp] = desiredLayerHandle[lh];
						break;
					}
				}
			}
		}

		m_activeLayers.clear();
		m_allLayers.clear();

		register int cal, calp;
		for( cal = 0; cal < tmpLayers.size(); cal++ )
			m_allLayers.push_back(tmpLayers[cal]);
		for( calp = 0; calp < tmpPositions.size(); calp++ )
			m_activeLayers.push_back(tmpPositions[calp]);

		//Set the Extents
		extents.left = atof(stringToken[1]);
		extents.right = atof(stringToken[2]);
		extents.bottom = atof(stringToken[3]);
		extents.top = atof(stringToken[4]);
		CalculateVisibleExtents(extents,false);

		//AfxMessageBox("Fire Events");

		//Send events for layerchanged
		register int el;
		for( el = 0; el < m_activeLayers.size(); el++ )
			FireMapState(m_activeLayers[el]);//send the layer handle back in drawing order

		m_canbitblt = FALSE;
		//if( !m_lockCount )
		//	InvalidateControl();

		//Draw the Window
		LockWindow(lmUnlock);

		m_mapstateMutex.Unlock();
		//AfxMessageBox("Exit MapState");
	}
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

LPDISPATCH CMapView::GetExtents()
{
	IExtents * box = NULL;
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&box);

	box->SetBounds( extents.left, extents.bottom, 0, extents.right, extents.top, 0 );
	return box;
}

void CMapView::SetExtents(LPDISPATCH newValue)
{
	if( newValue == NULL )
	{	m_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		return;
	}

	IExtents * box = NULL;
	newValue->QueryInterface(IID_IExtents, (void**)&box);

	if( box != NULL )
	{	double nv = 0;
		Extent ext;
		box->GetBounds(&ext.left, &ext.bottom, &nv, &ext.right, &ext.top, &nv);

		// Chris Michaelis / Steve Abbot --
		// Added "false" to this call to prevent the visible extent calculation
		// from logging the previous extent, seeing as it will be logged elsewhere
		// already. This seems to have been an oversight on writing this function initially.
		CalculateVisibleExtents(ext, false);

		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();

		FireExtentsChanged();

		//Rob Cairns
		// Called in support of faster ECW (and presumably other formats in future) image reading.
		ReloadImageBuffers();
	}
	else
		ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
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

CString CMapView::Crypt(CString str)
{
	CString tmp;

	int len = str.GetLength();
	int end = len;
	int cur = 0;

	str.MakeUpper();

	// convert to uppercase and remove non alpha chars
	for (int i = 0; i < end && cur < 7; i++)
	{
		char t = str[i];
		if (t >= 'A' && t <= 'Z')
		{
			tmp.AppendChar(t);
			cur++;
		}
	}

	for (int j = cur; j < 7; j++)
		tmp.AppendChar((char)((rand() % 26) + 64));

//	PrepString(str, tmp);

	int curPosition = 0;
	int offset = 0;
	for (int i = 0; i < 7; i++)
	{
		offset = (int)(tmp[6 - i] - 'A') + 7;
		curPosition += offset; // spin the decoder wheel to get then encoded character
		curPosition %= valsLen; // wrap around the "end" of the wheel if needed
		tmp.AppendChar(vals[curPosition]); // read what the magic wheel says
	}

	return tmp;
}

bool CMapView::VerifySerial(CString str)
{
	if (Crypt(str) == str)
		return true;
	else
		return false;
}

long CMapView::HWnd()
{
	return (long)this->m_hWnd;
}

short CMapView::GetIsLocked()
{	if( m_lockCount > 0 )
		return lmLock;
	else
		return lmUnlock;
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
	for(int i = 0; i < m_activeLayers.size(); i++)
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

bool CMapView::SendMouseMove()
{	
	return m_sendMouseMove;
}

bool CMapView::SendSelectBoxDrag()
{	
	return  m_sendSelectBoxDrag;
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

void CMapView::SetLineSeparationFactor(long sepFactor)
{
	m_LineSeparationFactor = sepFactor;
}

long CMapView::GetLineSeparationFactor(void)
{
	// 0 is invalid, since it would result in no adjustments
    if (m_LineSeparationFactor == 0) m_LineSeparationFactor = 3;
	return m_LineSeparationFactor;
}
#pragma endregion

#pragma region REGION ScaleAndZooming

// **********************************************************
//			CurrentScale()
// **********************************************************
DOUBLE CMapView::GetCurrentScale(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	 
	double minX, maxX, minY, maxY;	// size of ap control in pixels
    PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	if (minX == maxX && minY == maxY)
	{
		return 0.0;
	}
	else
	{
		long pixX = m_backbuffer->GetDeviceCaps(LOGPIXELSX);	// Number of pixels per logical inch along the screen width.
		long pixY = m_backbuffer->GetDeviceCaps(LOGPIXELSY);	// Number of pixels per logical inch along the screen height
		if (pixX == 0.0 || pixY == 0.0)	return 0.0;
		
		// logical size of screen, inches
		double screenHeigth = fabs(maxY - minY)/(double)pixY;	//96.0
		double screenWidth =  fabs(maxX - minX)/(double)pixX;	//96.0
		
		// size of map being displayed, inches
		double convFact = Utility::getConversionFactor(m_unitsOfMeasure);	
		if (convFact == 0) return 0.0;
		double mapHeight = (extents.top - extents.bottom)*convFact;
		double mapWidth = (extents.right - extents.left)*convFact;
		
		// calculate it as diagonal
		return sqrt(pow(mapWidth, 2) + pow(mapHeight, 2)) / sqrt(pow(screenWidth,2) + pow(screenHeigth,2));
	}
}

// **********************************************************
//			SetCurrentScale()
// **********************************************************
void CMapView::SetCurrentScale(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal == 0.0) return;

	double xCent = (extents.left + extents.right)/2.0;
	double yCent = (extents.bottom + extents.top)/2.0;
	
	double minX, maxX, minY, maxY;	// size of ap control in pixels
    PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	
	// getting screen size
	long pixX = m_backbuffer->GetDeviceCaps(LOGPIXELSX);
	long pixY = m_backbuffer->GetDeviceCaps(LOGPIXELSY);
	if (pixX == 0.0 || pixY == 0.0)	
		return;
	double screenHeight = (maxY - minY)/pixY;	//96.0
	double screenWidth =  (maxX - minX)/pixX;	//96.0
	
	// diagonal of map extents in inches
	double mapDiag = newVal * sqrt(pow(screenWidth,2) + pow(screenHeight,2));
	
	if (screenHeight == 0.0) return;
	double a = screenWidth/screenHeight;						// we need width and height, but have diagonal and the sides ratio of triangle only; this makes 2 equations:
	double mapHeight = sqrt(pow(mapDiag,2)/(pow(a,2) + 1));		// x/y = a
	double mapWidth = mapHeight * a;							// x^2 + y^2 = b^2		// where b - mapDiag
																// Taking x from first:
																// y^2*a^2 + y^2 = b^2
																// y = sqrt(b^2/(a^2 + 1))
																// x = y*a
	// converting to the map units
	mapHeight /= Utility::getConversionFactor(m_unitsOfMeasure);
	mapWidth /= Utility::getConversionFactor(m_unitsOfMeasure);

	IExtents* box = NULL;
	CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&box);
	box->SetBounds(xCent - mapWidth/2.0, yCent - mapHeight/2.0, 0.0, xCent + mapWidth/2.0, yCent + mapHeight/2.0, 0.0);
	this->SetExtents((LPDISPATCH)box);
	box->Release(); box = NULL;
	return;
}

// ***************************************************************
//		CalculateVisibleExtents()
// ***************************************************************
void CMapView::CalculateVisibleExtents( Extent e, bool LogPrev, bool MapSizeChanged )
{
	if( LogPrev == true )
		LogPrevExtent();

	double left = MIN( e.left, e.right );
	double right = MAX( e.left, e.right );
	double bottom = MIN( e.bottom, e.top );
	double top = MAX( e.bottom, e.top );
	
	if (left == right)	// lsu 26 jul 2009 for zooming to single point
	{	
		left -= 0.5;  
		right +=0.5;
	}
	if (bottom == top)
	{	
		bottom -=0.5;
		top +=0.5;
	}
	double xrange = right - left;
	double yrange = top - bottom;

	double yextent, xextent;
	double xadjust = 0, yadjust = 0;
	
	if (!MapSizeChanged && rbMapResizeBehavior != rbWarp)
	{
		// size of control is the same, we need just to apply new extents
		if (m_viewHeight!= 0 && m_viewWidth != 0 && xrange != 0.0 && yrange != 0.0)
		{	 
			// make extents proportional to screen sides ratio
			double ratio = ((double)m_viewWidth/(double)m_viewHeight)/(xrange/yrange);
			if (ratio > 1 )
			{	
				left = left - xrange * (ratio - 1)/2;
				right = left + xrange * (ratio);
				xrange *= ratio;
			}
			else if (ratio < 1)
			{	
				top = top + yrange * ((1/ratio) - 1)/2;
				bottom = top - yrange * (1/ratio);
				yrange *= (1/ratio);	
			}
		}
	}
	else
	{	
		// size of control changed; we need to calculate new extents
		if ( (rbMapResizeBehavior == rbClassic) || (rbMapResizeBehavior == rbIntuitive) )
		{
			if( xrange > yrange )
			{
				yextent = (xrange / m_viewWidth) * m_viewHeight;
				xextent = xrange;
				yadjust = (yextent - yrange) * .5;
				xadjust = 0;
				
				if (rbMapResizeBehavior == rbClassic)
				{
					if( yextent < yrange )
					{
						yextent = yrange;
						xextent = (yrange / m_viewHeight) * m_viewWidth;
						yadjust = 0;
						xadjust = (xextent - xrange) * .5;
					}
				}
			}
			else
			{
				xextent = (yrange / m_viewHeight) * m_viewWidth;
				yextent = yrange;
				xadjust = (xextent - xrange) * .5;
				yadjust = 0;
				
				if (rbMapResizeBehavior == rbClassic)
				{
					if( xextent < xrange )
					{
						xextent = xrange;
						yextent = (xrange / m_viewWidth) * m_viewHeight;
						xadjust = 0;
						yadjust = (yextent - yrange) * .5;
					}
				}
			}
		}
		else if (rbMapResizeBehavior == rbModern)
		{   
			//sizeOption is "modern" - this leaves scale on dX but adjusts scale on dY
			xextent = (yrange / m_viewHeight) * m_viewWidth;
		    yextent = yrange;
		}
		
		else if (rbMapResizeBehavior == rbKeepScale)
		{   
			// lsu (07/03/09) sizeOption is "keep scale", no scale adjustments for both axes
			if (m_pixelPerProjectionX == 0 || m_pixelPerProjectionY == 0)
			{	xextent = xrange;
				yextent = yrange;
			}
			else
			{	xextent = m_viewWidth/m_pixelPerProjectionX;
				yextent = m_viewHeight/m_pixelPerProjectionY;
			}
		}
		else if (rbMapResizeBehavior == rbWarp)
		{
			//sizeOption is "Warp" - this does not scale
			xextent = xrange;
			yextent = yrange;
			xadjust = 0;
			yadjust = 0;
		}
		
		left = left - xadjust;
		right = left + xextent;
		bottom = bottom - yadjust;
		if (rbMapResizeBehavior == rbKeepScale)
			bottom = top - yextent;
		else
			top = bottom + yextent;	

		xrange = right - left;
		yrange = top - bottom;
	}
	
	// save new extents and recalculate scale
	extents.left = left;
	extents.right = right;
	extents.bottom = bottom;
	extents.top = top;
	
	if (xrange == 0)
	{
		m_pixelPerProjectionX = 0;
		m_inversePixelPerProjectionX = 0;
	}
	else
	{
		m_pixelPerProjectionX = m_viewWidth/xrange;
		m_inversePixelPerProjectionX = 1.0/m_pixelPerProjectionX;
	}

	if (yrange == 0)
	{
		m_pixelPerProjectionY = 0;
		m_inversePixelPerProjectionY = 0;
	}
	else
	{
		m_pixelPerProjectionY = m_viewHeight/yrange;
		m_inversePixelPerProjectionY = 1.0/m_pixelPerProjectionY;
	}
}

void CMapView::ProjToPixel(double projX, double projY, double FAR* pixelX, double FAR* pixelY)
{
	ProjectionToPixel(projX,projY,*pixelX,*pixelY);
}

void CMapView::PixelToProj(double pixelX, double pixelY, double FAR* projX, double FAR* projY)
{
	PixelToProjection(pixelX,pixelY,*projX,*projY);
}

inline void CMapView::PixelToProjection( double piX, double piY, double & prX, double & prY )
{
	//Commented out by Rob Cairns 10 Jan 06
	//piX++;
	//Commented out by Shade 1974 11 Jan 06
	//piY++;
	prX = extents.left + piX*m_inversePixelPerProjectionX;
	prY = extents.top - piY*m_inversePixelPerProjectionY;
}

inline void CMapView::ProjectionToPixel( double prX, double prY, double & piX, double & piY )
{
	piX = (prX - extents.left)*m_pixelPerProjectionX;
	//Shade1974 - to help eliminate 1 pixel deviation in y direction for smo
	piY = (extents.top - prY) * m_pixelPerProjectionY;
	
	//code Shade1974 replaced 
	//piY = (prY - extents.bottom)*m_pixelPerProjectionY;
    //piY = m_viewHeight-piY-1;
}

// ***********************************************************
//		UnitsPerPixel
// ***********************************************************
// Returns number of map units per pixel of map
double CMapView::UnitsPerPixel()
{
    double minX, maxX, minY, maxY;
    PROJECTION_TO_PIXEL(extents.left, extents.bottom, minX, minY);
	PROJECTION_TO_PIXEL(extents.right, extents.top, maxX, maxY);
	if (minX == maxX && minY == maxY)
	{
		return 0.0;
	}
	else
	{
		return sqrt(pow(extents.right - extents.left, 2) + pow(extents.top - extents.bottom, 2)) / 
			   sqrt(pow(maxX - minX,2) + pow(maxY - minY,2));
	}
}

inline void CMapView::LogPrevExtent()
{
	m_prevExtents.push_back( extents );
	if( m_prevExtents.size() > m_extentHistory )
		m_prevExtents.pop_front();
}

// ****************************************************************
//		GetMaxExtents()
// ****************************************************************
IExtents* CMapView::GetMaxExtents(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	bool extentsSet = false;
	Extent maxExtents;

	for(int i = 0; i <  m_activeLayers.size(); i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		this->AdjustLayerExtents(m_activeLayers[i]);

		if( !extentsSet )
		{	
			maxExtents.left = l->extents.left;
			maxExtents.right = l->extents.right;
			maxExtents.top = l->extents.top;
			maxExtents.bottom = l->extents.bottom;
			extentsSet = true;
		}
		else
		{	

			if( l->extents.left < maxExtents.left )
				maxExtents.left = l->extents.left;
			if( l->extents.right > maxExtents.right )
				maxExtents.right = l->extents.right;
			if( l->extents.bottom < maxExtents.bottom )
				maxExtents.bottom = l->extents.bottom;
			if( l->extents.top > maxExtents.top )
				maxExtents.top = l->extents.top;
		}
	}

	if( !extentsSet )
	{
		maxExtents = Extent(0,0,0,0);
	}
	
	IExtents* ext = NULL;
	CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&ext);
	ext->SetBounds(maxExtents.left, maxExtents.bottom, 0.0, maxExtents.right, maxExtents.top, 0.0);
	return ext;
}

// ****************************************************************
//		SetMaxExtents()
// ****************************************************************
void CMapView::SetMaxExtents(IExtents* pVal)
{
	SetNotSupported();
}

// ****************************************************************
//		ZoomToMaxExtents()
// ****************************************************************
void CMapView::ZoomToMaxExtents()
{
	bool extentsSet = false;

	if( m_activeLayers.size() > 0 )
		LogPrevExtent();

	long endcondition = m_activeLayers.size();
	for(int i = 0; i < endcondition; i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		AdjustLayerExtents(m_activeLayers[i]);

		if( extentsSet == false )
		{	
			double xrange = l->extents.right - l->extents.left;
			double yrange = l->extents.top - l->extents.bottom;

			//Neio 2009-07-02 for empty layer will cause the caculation error
			if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
				continue;

			extents.left = l->extents.left - xrange*m_extentPad;
			extents.right = l->extents.right + xrange*m_extentPad;
			extents.top = l->extents.top + yrange*m_extentPad;
			extents.bottom = l->extents.bottom - yrange*m_extentPad;
			extentsSet = true;
		}
		else
		{	double xrange = l->extents.right - l->extents.left;
			double yrange = l->extents.top - l->extents.bottom;

			//Neio 2009-07-02 for empty layer will cause the caculation error
			if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
				continue;

			if( l->extents.left - xrange*m_extentPad < extents.left )
				extents.left = l->extents.left - xrange*m_extentPad;
			if( l->extents.right + xrange*m_extentPad > extents.right )
				extents.right = l->extents.right + xrange*m_extentPad;
			if( l->extents.bottom - yrange*m_extentPad < extents.bottom )
				extents.bottom = l->extents.bottom - yrange*m_extentPad;
			if( l->extents.top + yrange*m_extentPad > extents.top )
				extents.top = l->extents.top + yrange*m_extentPad;
		}
	}

	if( extentsSet == false )
		extents = Extent(0,0,0,0);

	CalculateVisibleExtents( extents, false );
	FireExtentsChanged();
	ReloadImageBuffers();

	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();
}

// ****************************************************************
//		ZoomToMaxVisibleExtents()
// ****************************************************************
//Rob Cairns - this is a suggested fix for gdal images
void CMapView::ZoomToMaxVisibleExtents(void)
{
	bool extentsSet = false;

	if( m_activeLayers.size() > 0 )
		LogPrevExtent();

	register int i;
	long endcondition = m_activeLayers.size();
	for( i = 0; i < endcondition; i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		AdjustLayerExtents(m_activeLayers[i]);
		if( l->flags & Visible)
		{
			if( extentsSet == false )
			{	double xrange = l->extents.right - l->extents.left;
				double yrange = l->extents.top - l->extents.bottom;

				//Neio 2009-07-02 for empty layer will cause the caculation error
				if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
					continue;

				extents.left = l->extents.left - xrange*m_extentPad;
				extents.right = l->extents.right + xrange*m_extentPad;
				extents.top = l->extents.top + yrange*m_extentPad;
				extents.bottom = l->extents.bottom - yrange*m_extentPad;
				extentsSet = true;
			}
			else
			{	double xrange = l->extents.right - l->extents.left;
				double yrange = l->extents.top - l->extents.bottom;

				//Neio 2009-07-02 for empty layer will cause the caculation error
				if(xrange == 0 && yrange ==0 && l->extents.right ==0 && l->extents.top == 0)
					continue;

				if( l->extents.left - xrange*m_extentPad < extents.left )
					extents.left = l->extents.left - xrange*m_extentPad;
				if( l->extents.right + xrange*m_extentPad > extents.right )
					extents.right = l->extents.right + xrange*m_extentPad;
				if( l->extents.bottom - yrange*m_extentPad < extents.bottom )
					extents.bottom = l->extents.bottom - yrange*m_extentPad;
				if( l->extents.top + yrange*m_extentPad > extents.top )
					extents.top = l->extents.top + yrange*m_extentPad;
			}
		}

	}

	if( extentsSet == false )
		extents = Extent(0,0,0,0);

	CalculateVisibleExtents( extents, false );
	FireExtentsChanged();
	ReloadImageBuffers();

	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();
}

// **************************************************************
//		ZoomToLayer()
// **************************************************************
void CMapView::ZoomToLayer(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{	
		AdjustLayerExtents(LayerHandle);
		
		LogPrevExtent();
		Layer * l = m_allLayers[LayerHandle];
		double xrange = l->extents.right - l->extents.left;
		double yrange = l->extents.top - l->extents.bottom;
		extents.left = l->extents.left - xrange*m_extentPad;
		extents.right = l->extents.right + xrange*m_extentPad;
		extents.top = l->extents.top + yrange*m_extentPad;
		extents.bottom = l->extents.bottom - yrange*m_extentPad;
		CalculateVisibleExtents( extents, false );

		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();

		FireExtentsChanged();
		ReloadImageBuffers();
	}
	else
		ErrorMessage(tkINVALID_LAYER_HANDLE);
}



void CMapView::ZoomToShape(long LayerHandle, long Shape)
{
	if( IsValidShape(LayerHandle, Shape) )
	{	IShapefile * ishp = NULL;
		Layer * l = m_allLayers[LayerHandle];
		l->object->QueryInterface(IID_IShapefile,(void**)&ishp);

		if( ishp == NULL )
		{	m_lastErrorCode = tkINTERFACE_NOT_SUPPORTED;
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
			return;
		}

		LogPrevExtent();

		IShape * s = NULL;
		ishp->get_Shape(Shape,&s);

		IExtents * box = NULL;
		s->get_Extents( &box );

		double left, right, top, bottom, nv;
		box->GetBounds(&left, &bottom, &nv, &right, &top, &nv);

		double xrange = right - left;
		double yrange = top - bottom;

		//if the minimum and maximum extents are the same, use a range of 1 for
		// xrange and yrange to allow a point to be centered in the map window
		// when ZoomToShape is used on a point shapefile.
		if(xrange == 0){
			xrange = 1;
		}
		if(yrange == 0){
			yrange = 1;
		}

		extents.left = left - xrange*m_extentPad;
		extents.right = right + xrange*m_extentPad;
		extents.top = top + yrange*m_extentPad;
		extents.bottom = bottom - yrange*m_extentPad;
		CalculateVisibleExtents( extents, false );
		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();

		box->Release();
		s->Release();
		ishp->Release();

		FireExtentsChanged();
		ReloadImageBuffers();
	}
	else
	{	//Error Code set in func
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
	}

}

void CMapView::ZoomIn(double Percent)
{
	double xzin = ((extents.right - extents.left)*( 1.0 - Percent ))*.5;
	double xmiddle = extents.left + (extents.right - extents.left)*.5;

	double cLeft = xmiddle - xzin;
	double cRight = xmiddle + xzin;

	double yzin = ((extents.top - extents.bottom)*( 1.0 - Percent ))*.5;
	double ymiddle = extents.bottom + (extents.top - extents.bottom)*.5;

	double cBottom = ymiddle - yzin;
	double cTop = ymiddle + yzin;

	CalculateVisibleExtents( Extent(cLeft, cRight, cBottom, cTop) );
	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();

	FireExtentsChanged();
	ReloadImageBuffers();
}

void CMapView::ZoomOut(double Percent)
{
	double xzout = ((extents.right - extents.left)*( 1.0 + Percent ))*.5;
	double xmiddle = extents.left + (extents.right - extents.left)*.5;

	double cLeft = xmiddle - xzout;
	double cRight = xmiddle + xzout;

	double yzout = ((extents.top - extents.bottom)*( 1.0 + Percent ))*.5;
	double ymiddle = extents.bottom + (extents.top - extents.bottom)*.5;

	double cBottom = ymiddle - yzout;
	double cTop = ymiddle + yzout;

	CalculateVisibleExtents( Extent( cLeft, cRight, cBottom, cTop ) );
	m_canbitblt = FALSE;
	if( !m_lockCount )
		InvalidateControl();

	FireExtentsChanged();
	ReloadImageBuffers();

}

long CMapView::ZoomToPrev()
{
	if( m_prevExtents.size() > 0 )
	{	CalculateVisibleExtents(m_prevExtents[m_prevExtents.size() - 1],false);
		m_prevExtents.pop_back();

		m_canbitblt = FALSE;
		if( !m_lockCount )
			InvalidateControl();

		if( !snapshot )
		{
			FireExtentsChanged();
			ReloadImageBuffers();
	
		}
	}

	return m_prevExtents.size();
}


//ajp (June 2010)
//LPDISPATCH 
IPoint* CMapView::GetBaseProjectionPoint(double rotPixX, double rotPixY)
{
  IPoint *curPoint = NULL;
  long basePixX = 0, basePixY = 0; 
  double baseProjX = 0, baseProjY = 0; 
	
  CoCreateInstance( CLSID_Point, NULL, CLSCTX_INPROC_SERVER, IID_IPoint, (void**)&curPoint);

	if (m_Rotate == NULL || m_Rotate->degAngle == 0.0)
  {
    basePixX = rotPixX;
    basePixY = rotPixY;
  }
  else
  {
     m_Rotate->getOriginalPixelPoint((long) rotPixX, (long) rotPixY, &basePixX, &basePixY);
  }
  PixelToProjection( basePixX, basePixY, baseProjX, baseProjY);
  
  curPoint->put_X(baseProjX);
  curPoint->put_Y(baseProjY);
  return curPoint;
}
//ajp (June 2010)
//LPDISPATCH 
IExtents* CMapView::GetRotatedExtent()
{
  Extent rotExtent;
	IExtents * box = NULL;

	rotExtent = extents;
	CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&box);
  box->SetBounds( rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0 );

	if (m_RotateAngle == 0)
	  return box;

	if (m_Rotate == NULL)
		m_Rotate = new Rotate();

	m_Rotate->calcRotatedExtent(m_viewWidth, m_viewHeight);
  rotExtent.right += (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
  rotExtent.bottom -= (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
  rotExtent.left -= (m_Rotate->xAxisDiff * m_inversePixelPerProjectionX);
  rotExtent.top += (m_Rotate->yAxisDiff * m_inversePixelPerProjectionY);
  box->SetBounds( rotExtent.left, rotExtent.bottom, 0, rotExtent.right, rotExtent.top, 0 );

	return box;
}

#pragma endregion

#pragma region REGION SnapShots
LPDISPATCH CMapView::SnapShot(LPDISPATCH BoundBox)
{
	if( BoundBox == NULL )
	{	
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return NULL;
	}

	IExtents * box = NULL;
	BoundBox->QueryInterface(IID_IExtents,(void**)&box);

	if( box == NULL )
	{	
		ErrorMessage(tkINTERFACE_NOT_SUPPORTED);
		return NULL;
	}

	LockWindow( lmLock );

	double left, right, bottom, top, nv;
	box->GetBounds(&left,&bottom,&nv,&right,&top,&nv);

	IImage * iimg = NULL;
	CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
	
	// ---------------------------------------------------------
	// Current extents are needed
	// ---------------------------------------------------------
	if( left == extents.left &&
		right == extents.right &&
		bottom == extents.bottom &&
		top == extents.top )
	{
		CDC * cdc;
		bool createdDC=false;
		if( m_doubleBuffer )
		{	
			cdc = m_backbuffer;
			if( m_canbitblt == FALSE )
			{
				CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
				DrawNextFrame(rcBounds,cdc);
			}
		}
		else
		{
			createdDC = true;
			cdc = new CDC();
			cdc->CreateCompatibleDC(GetDC());
			CBitmap * bmp = new CBitmap();
			CBitmap * oldBmp = NULL;
			bmp->CreateDiscardableBitmap(GetDC(),m_viewWidth,m_viewHeight);
			oldBmp = cdc->SelectObject(bmp);
			DrawNextFrame(rcBounds,cdc);
			
			// 12-Oct-2009 Rob Cairns
			cdc->SelectObject(oldBmp);
			bmp->DeleteObject();
			delete bmp;
		}
		
		if ((m_ShapeDrawingMethod == dmNewWithSelection || m_ShapeDrawingMethod == dmStandard) && !FORCE_NEW_LABELS)
		{
			DrawOldLabels(cdc);
		}
		DrawLists(rcBounds,cdc,dlScreenReferencedList);

		//Capture the draw code
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimg);
		VARIANT_BOOL retval;
		iimg->SetImageBitsDC((long)cdc->m_hDC,&retval);
		//iimg->CreateNew(m_viewWidth, m_viewHeight, &retval);

		double dx = (right-left)/(double)m_viewWidth;
		iimg->put_dX(dx);
		double dy = (top-bottom)/(double)m_viewHeight;
		iimg->put_dY(dy);
		iimg->put_XllCenter(left + dx*.5);
		iimg->put_YllCenter(bottom + dy*.5);

		if( createdDC )
		{	
			ReleaseDC(cdc);
			delete cdc;
		}
	}
	else
	{
		CalculateVisibleExtents(Extent(left,right,bottom,top));

		//1. Draw to a back buffer
		//2. Populate an Image object

		CDC * snapDC = new CDC();
		snapDC->CreateCompatibleDC(GetDC());
		CBitmap * bmp = new CBitmap();
		bmp->CreateDiscardableBitmap(GetDC(),m_viewWidth,m_viewHeight);
		CBitmap * oldBMP = snapDC->SelectObject(bmp);
		
		bool oldValue = m_forceBounds;
		m_forceBounds = true;
		DrawNextFrame(rcBounds,snapDC);
		m_forceBounds = oldValue;
	
		if ((m_ShapeDrawingMethod == dmNewWithSelection || m_ShapeDrawingMethod == dmStandard) && !FORCE_NEW_LABELS)
		{
			DrawOldLabels(snapDC);
		}
	
		DrawLists(rcBounds,snapDC,dlScreenReferencedList);

		//Capture the draw code
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimg);

		VARIANT_BOOL retval;
		iimg->SetImageBitsDC((long)snapDC->m_hDC,&retval);
		
		USES_CONVERSION;
		iimg->Save(A2BSTR("c:\\temp.bmp"), VARIANT_FALSE, BITMAP_FILE, NULL, &retval);
		//iimg->CreateNew(m_viewWidth, m_viewHeight, &retval);

		double dx = (right-left)/(double)m_viewWidth;
		iimg->put_dX(dx);
		double dy = (top-bottom)/(double)m_viewHeight;
		iimg->put_dY(dy);
		iimg->put_XllCenter(left + dx*.5);
		iimg->put_YllCenter(bottom + dy*.5);

		snapDC->SelectObject(oldBMP);
		bmp->DeleteObject();
		delete bmp;
		snapDC->DeleteDC();
		delete snapDC;

		snapshot = true;
		ZoomToPrev();
		snapshot = false;

		m_canbitblt=FALSE;
	}

	LockWindow( lmUnlock );
	box->Release();
	box = NULL;
	return iimg;
}


IDispatch* CMapView::SnapShot2(LONG ClippingLayerNbr, DOUBLE Zoom, long pWidth)
{   // tws added 04/07/2007
	// use the indicated layer and zoom/width to determine the output size and clipping

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// first decide just how big we want it, in actual pixels
	long Width, Height;
	double left, right, bottom, top;
	double mm_left, mm_right, mm_bottom, mm_top;

	Layer * l = m_allLayers[ClippingLayerNbr];
	if( IS_VALID_PTR(l) )
	{	
		if( l->type == ShapefileLayer )
		{
			// shapefile does not have pre-determined size 
			// give priority to the pWidth parameter
			// and set the height by the shapefile's aspect ratio
			IShapefile * sf = NULL;
			l->object->QueryInterface(IID_IShapefile, (void**)&sf);
			if (sf == NULL)
				return NULL;
			IExtents * box = NULL;
			sf->get_Extents(&box);
			sf->Release();
			sf = NULL;
			double zm, zM;
			box->GetBounds(&left, &bottom, &zm, &right, &top, &zM);
			box->Release();
			box = NULL;
			double ar = (right-left)/(top-bottom);
			Width = (long) pWidth;
			if (Width == 0)
			{
				Width = (right - left) * Zoom;
			}
			Height = (long)((double)pWidth / ar);
		}
		else if(l->type == ImageLayer)
		{
			// an image file has a natural size, 
			// exporting at that size does not add distortion
			// give priority to the Zoom parameter:
			// get the original image width and extent, 
			// then apply the Zoom to scale it
			//
			// NB: we MUST use the original height and width to get it 
			// pixel-for-pixel, calculating one or the other from the AR 
			// is always off just enough to force resampling and 
			// turn the output image into junk
			IImage * iimg = NULL;
			l->object->QueryInterface(IID_IImage,(void**)&iimg);
			if( iimg == NULL )
				return NULL;

			iimg->get_OriginalWidth(&Width);
			iimg->get_OriginalHeight(&Height);
			iimg->Release();
			iimg = NULL;

			left = l->extents.left;
			right = l->extents.right;
			bottom = l->extents.bottom;
			top = l->extents.top;			

			if (Zoom > 0)
			{
				Width *= Zoom;
				Height *= Zoom;
			}
		}
		else
		{
			// ISSUE: (tws) these MB's could be a problem for any ocx users who don't 
			//        have a user sitting in front of their application?
			//        I guess if you want the error messages you need to provide the callback
			//AfxMessageBox("Cannot clip to selected layer type");
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Cannot clip to selected layer type"));
			return NULL;
		}
	}
	else
	{
		//AfxMessageBox("Cannot clip to selected layer");
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Cannot clip to selected layer"));
		return NULL;
	}
	if (Width <= 0 || Height <= 0)
	{
		//AfxMessageBox("Invalid Width and/or Zoom");
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Invalid Width and/or Zoom"));
		return NULL;
	}

	// many routines are tied to these member vars which are
	// normally just the size of the screen canvas etc
	// so we have to set them to coerce the desired output from the existing logic
	long mm_viewWidth = m_viewWidth;
	long mm_viewHeight = m_viewHeight;
	double mm_pixelPerProjectionX = m_pixelPerProjectionX;
	double mm_pixelPerProjectionY = m_pixelPerProjectionY;
	double mm_inversePixelPerProjectionX = m_inversePixelPerProjectionX;
	double mm_inversePixelPerProjectionY = m_inversePixelPerProjectionY;
	double mm_aspectRatio = m_aspectRatio;

	m_viewWidth=Width;
	m_viewHeight=Height;
	m_aspectRatio = (double)Width / (double)Height; 

	double xrange = right - left;
	double yrange = top - bottom;
	m_pixelPerProjectionX = m_viewWidth/xrange;
	m_inversePixelPerProjectionX = 1.0/m_pixelPerProjectionX;
	m_pixelPerProjectionY = m_viewHeight/yrange;
	m_inversePixelPerProjectionY = 1.0/m_pixelPerProjectionY;

	mm_left = extents.left;
	mm_right = extents.right;
	mm_bottom = extents.bottom;
	mm_top = extents.top;
	extents.left = left;
	extents.right = right - m_inversePixelPerProjectionX;
	extents.bottom = bottom;
	extents.top = top - m_inversePixelPerProjectionY;

	m_canbitblt=FALSE;

	// much of what follows now is pretty much straight from Snapshot()

	//1. Draw to a back buffer
	//2. Populate an Image object

	CBitmap * bmp = new CBitmap();
	// creating big bmps means more chance of running out of memory
	// try it early so we find out sooner
	if (!bmp->CreateDiscardableBitmap(GetDC(),m_viewWidth,m_viewHeight))
	{
		//AfxMessageBox("Failed to create bitmap; not enough memory?");
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Failed to create bitmap; not enough memory?"));
		// in which event we have a lot of unwinding to do here - ugh
		m_viewWidth = mm_viewWidth;
		m_viewHeight = mm_viewHeight;
		m_aspectRatio = mm_aspectRatio; 
		m_pixelPerProjectionX = mm_pixelPerProjectionX;
		m_pixelPerProjectionY = mm_pixelPerProjectionY;
		m_inversePixelPerProjectionX = mm_inversePixelPerProjectionX;
		m_inversePixelPerProjectionY = mm_inversePixelPerProjectionY;
		extents.left = mm_left;
		extents.right = mm_right;
		extents.bottom = mm_bottom;
		extents.top = mm_top;
		snapshot = false;
		m_canbitblt=FALSE;
		return NULL;
	}

	// get any images reloaded at the current scale so we get all the detail
	ReloadImageBuffers();

	// proceed to draw

	LockWindow( lmLock );
	IImage * iimg = NULL;

	CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
	CalculateVisibleExtents(Extent(left,right,bottom,top));

	CDC * snapDC = new CDC();
	snapDC->CreateCompatibleDC(GetDC());
	CBitmap * oldBMP = snapDC->SelectObject(bmp);
	m_forceBounds = true;
	DrawNextFrame(rcBounds,snapDC);
	m_forceBounds = false;

	if ((m_ShapeDrawingMethod == dmNewWithSelection || m_ShapeDrawingMethod == dmStandard) && !FORCE_NEW_LABELS)
	{
		DrawOldLabels(snapDC);
	}

	DrawLists(rcBounds,snapDC,dlScreenReferencedList);

	CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimg);
	VARIANT_BOOL retval;
	iimg->SetImageBitsDC((long)snapDC->m_hDC,&retval);

	double dx = (right-left)/(double)(m_viewWidth);
	iimg->put_dX(dx);
	double dy = (top-bottom)/(double)(m_viewHeight);
	iimg->put_dY(dy);
	iimg->put_XllCenter(left + dx*.5);
	iimg->put_YllCenter(bottom + dy*.5);

	snapDC->SelectObject(oldBMP);
	bmp->DeleteObject();
	delete bmp;
	snapDC->DeleteDC();
	delete snapDC;

	snapshot = true;

	// finally put everything back as needed for normal screen-based drawing
	m_viewWidth = mm_viewWidth;
	m_viewHeight = mm_viewHeight;
	m_aspectRatio = mm_aspectRatio; 
	m_pixelPerProjectionX = mm_pixelPerProjectionX;
	m_pixelPerProjectionY = mm_pixelPerProjectionY;
	m_inversePixelPerProjectionX = mm_inversePixelPerProjectionX;
	m_inversePixelPerProjectionY = mm_inversePixelPerProjectionY;
	extents.left = mm_left;
	extents.right = mm_right;
	extents.bottom = mm_bottom;
	extents.top = mm_top;
	ReloadImageBuffers();
	snapshot = false;
	m_canbitblt=FALSE;
	LockWindow( lmUnlock );
	return iimg;
}

//Created a new snapshot method which works a bit better specifically for the printing engine
LPDISPATCH CMapView::SnapShot3(double left, double right, double top, double bottom, long Width)
{   
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Get the image height based on the box aspect ration
	long Height = (long)((double)Width / ((right-left)/(top-bottom)));

	//Make sure that the width and height are valid
	if (Width <= 0 || Height <= 0)
	{
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Invalid Width and/or Zoom"));
		return NULL;
	}

	// many routines are tied to these member vars which are
	// normally just the size of the screen canvas etc
	// so we have to set them to coerce the desired output from the existing logic
	double mm_left, mm_right, mm_bottom, mm_top;
	long mm_viewWidth = m_viewWidth;
	long mm_viewHeight = m_viewHeight;
	double mm_pixelPerProjectionX = m_pixelPerProjectionX;
	double mm_pixelPerProjectionY = m_pixelPerProjectionY;
	double mm_inversePixelPerProjectionX = m_inversePixelPerProjectionX;
	double mm_inversePixelPerProjectionY = m_inversePixelPerProjectionY;
	double mm_aspectRatio = m_aspectRatio;

	m_viewWidth=Width;
	m_viewHeight=Height;
	m_aspectRatio = (double)Width / (double)Height; 

	double xrange = right - left;
	double yrange = top - bottom;
	m_pixelPerProjectionX = m_viewWidth/xrange;
	m_inversePixelPerProjectionX = 1.0/m_pixelPerProjectionX;
	m_pixelPerProjectionY = m_viewHeight/yrange;
	m_inversePixelPerProjectionY = 1.0/m_pixelPerProjectionY;

	mm_left = extents.left;
	mm_right = extents.right;
	mm_bottom = extents.bottom;
	mm_top = extents.top;
	extents.left = left;
	extents.right = right - m_inversePixelPerProjectionX;
	extents.bottom = bottom;
	extents.top = top - m_inversePixelPerProjectionY;

	m_canbitblt=FALSE;

	//1. Draw to a back buffer
	//2. Populate an Image object

	CBitmap * bmp = new CBitmap();

	// creating big bmps means more chance of running out of memory
	// try it early so we find out sooner
	if (!bmp->CreateDiscardableBitmap(GetDC(),m_viewWidth,m_viewHeight))
	{
		//AfxMessageBox("Failed to create bitmap; not enough memory?");
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR("Failed to create bitmap; not enough memory?"));
		// in which event we have a lot of unwinding to do here - ugh
		m_viewWidth = mm_viewWidth;
		m_viewHeight = mm_viewHeight;
		m_aspectRatio = mm_aspectRatio; 
		m_pixelPerProjectionX = mm_pixelPerProjectionX;
		m_pixelPerProjectionY = mm_pixelPerProjectionY;
		m_inversePixelPerProjectionX = mm_inversePixelPerProjectionX;
		m_inversePixelPerProjectionY = mm_inversePixelPerProjectionY;
		extents.left = mm_left;
		extents.right = mm_right;
		extents.bottom = mm_bottom;
		extents.top = mm_top;
		snapshot = false;
		m_canbitblt=FALSE;
		return NULL;
	}

	// get any images reloaded at the current scale so we get all the detail
	ReloadImageBuffers();

	// proceed to draw
	LockWindow( lmLock );
	IImage * iimg = NULL;

	CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
	CalculateVisibleExtents(Extent(left,right,bottom,top));

	CDC * snapDC = new CDC();
	snapDC->CreateCompatibleDC(GetDC());
	CBitmap * oldBMP = snapDC->SelectObject(bmp);
	m_forceBounds = true;
	DrawNextFrame(rcBounds,snapDC);
	m_forceBounds = false;
	
	if ((m_ShapeDrawingMethod == dmNewWithSelection || m_ShapeDrawingMethod == dmStandard) && !FORCE_NEW_LABELS)
	{
		DrawOldLabels(snapDC);
	}
	DrawLists(rcBounds,snapDC,dlScreenReferencedList);

	CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&iimg);
	VARIANT_BOOL retval;
	iimg->SetImageBitsDC((long)snapDC->m_hDC,&retval);

	double dx = (right-left)/(double)(m_viewWidth);
	iimg->put_dX(dx);
	double dy = (top-bottom)/(double)(m_viewHeight);
	iimg->put_dY(dy);
	iimg->put_XllCenter(left + dx*.5);
	iimg->put_YllCenter(bottom + dy*.5);

	snapDC->SelectObject(oldBMP);
	bmp->DeleteObject();
	delete bmp;
	snapDC->DeleteDC();
	delete snapDC;

	snapshot = true;

	// finally put everything back as needed for normal screen-based drawing
	m_viewWidth = mm_viewWidth;
	m_viewHeight = mm_viewHeight;
	m_aspectRatio = mm_aspectRatio; 
	m_pixelPerProjectionX = mm_pixelPerProjectionX;
	m_pixelPerProjectionY = mm_pixelPerProjectionY;
	m_inversePixelPerProjectionX = mm_inversePixelPerProjectionX;
	m_inversePixelPerProjectionY = mm_inversePixelPerProjectionY;
	extents.left = mm_left;
	extents.right = mm_right;
	extents.bottom = mm_bottom;
	extents.top = mm_top;
	ReloadImageBuffers();
	snapshot = false;
	m_canbitblt=FALSE;
	LockWindow( lmUnlock );
	return iimg;
}

// *********************************************************************
//    SnapShotToDC()
// *********************************************************************
// Draws the specified extents of map at given DC.
BOOL CMapView::SnapShotToDC(PVOID hdc, IExtents* Extents, LONG Width)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if(!Extents) return FALSE;
	if (!hdc) return FALSE;
	
	// getting DC to draw
	HDC dc = reinterpret_cast<HDC>(hdc);
	CDC * tempDC = CDC::FromHandle(dc);

	// Get the image height based on the box aspect ration
	double xMin, xMax, yMin, yMax, zMin, zMax;
	Extents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
	
	// Make sure that the width and height are valid
	long Height = static_cast<long>((double)Width *(yMax - yMin) / (xMax - xMin));
	if (Width <= 0 || Height <= 0)
	{
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(), A2BSTR("Invalid Width and/or Zoom"));
		return FALSE;
	}
	
	// -----------------------------------------------------
	// saving the current extents
	// -----------------------------------------------------
	long mm_viewWidth = m_viewWidth;
	long mm_viewHeight = m_viewHeight;
	double mm_right = extents.right;
	double mm_left = extents.left;
	double mm_bottom = extents.bottom;
	double mm_top = extents.top;
	double mm_pixelPerProjectionX = m_pixelPerProjectionX;
	double mm_pixelPerProjectionY = m_pixelPerProjectionY;
	double mm_inversePixelPerProjectionX = m_inversePixelPerProjectionX;
	double mm_inversePixelPerProjectionY = m_inversePixelPerProjectionY;
	double mm_aspectRatio = m_aspectRatio;
	
	LockWindow( lmLock );

	// applying new size
	m_viewWidth=Width;
	m_viewHeight=Height;
	m_aspectRatio = (double)Width / (double)Height; 
	m_pixelPerProjectionX = m_viewWidth/(xMax - xMin);
	m_pixelPerProjectionY = m_viewHeight/(yMax - yMin);
	m_inversePixelPerProjectionX = 1.0/m_pixelPerProjectionX;
	m_inversePixelPerProjectionY = 1.0/m_pixelPerProjectionY;

	extents.left = xMin;
	extents.right = xMax - m_inversePixelPerProjectionX;
	extents.bottom = yMin;
	extents.top = yMax - m_inversePixelPerProjectionY;

	CalculateVisibleExtents(Extent(xMin,xMax,yMin,yMax), false, true);
	m_canbitblt=FALSE;


    // get any images reloaded at the current scale
	ReloadImageBuffers();

	m_forceBounds = true;	
	CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
	DrawNextFrame(rcBounds,tempDC);
	m_forceBounds = false;	// can be exluded; DrawImage uses rcBounds in all cases
	
	if ((m_ShapeDrawingMethod == dmNewWithSelection || m_ShapeDrawingMethod == dmStandard) && !FORCE_NEW_LABELS)
	{
		DrawOldLabels(tempDC);
	}
	DrawLists(rcBounds,tempDC,dlScreenReferencedList);

	// --------------------------------------------------------
	// finally put everything back as needed for normal screen-based drawing
	// --------------------------------------------------------
	m_viewWidth = mm_viewWidth;
	m_viewHeight = mm_viewHeight;
	m_aspectRatio = mm_aspectRatio; 
	m_pixelPerProjectionX = mm_pixelPerProjectionX;
	m_pixelPerProjectionY = mm_pixelPerProjectionY;
	m_inversePixelPerProjectionX = mm_inversePixelPerProjectionX;
	m_inversePixelPerProjectionY = mm_inversePixelPerProjectionY;
	extents.left = mm_left;
	extents.right = mm_right;
	extents.bottom = mm_bottom;
	extents.top = mm_top;
	
	m_canbitblt=FALSE;
	
	ReloadImageBuffers();
	
	LockWindow( lmUnlock );

	return TRUE;
}
#pragma endregion

#pragma region REGION Images

// ****************************************************************
//		ReloadImageBuffers()
// ****************************************************************
void CMapView::ReloadImageBuffers()
{
	IImage * iimg = NULL;
	for(int i = 0; i < m_activeLayers.size(); i++ )
	{
		Layer * l = m_allLayers[m_activeLayers[i]];

		if ((l->type == ImageLayer) && (l->flags & Visible))
		{
			l->object->QueryInterface(IID_IImage,(void**)&iimg);
			if( iimg != NULL ) 
			{	
				((CImageClass*)iimg)->_bufferReloadIsNeeded = true;
			}
		}
	}
}

// ***************************************************
//  GetImageLayerPercentTransparent
// ***************************************************
// Will be deprecated
float CMapView::GetImageLayerPercentTransparent(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{
		Layer * l = m_allLayers[LayerHandle];
		if( l->type == ImageLayer )
		{
			IImage * iimg = NULL;
			l->object->QueryInterface(IID_IImage,(void**)&iimg);
			if( iimg == NULL )	
				return 1.0;
			
			double val;
			iimg->get_TransparencyPercent(&val);
			iimg->Release(); iimg = NULL;
			return static_cast<float>(val);
		}
		else
		{	
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return 0.0f;
		}
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return 0.0f;
	}
}

// ***************************************************
//  GetImageLayerPercentTransparent
// ***************************************************
// Will be deprecated
void CMapView::SetImageLayerPercentTransparent(long LayerHandle, float newValue)
{
	if( newValue < 0.0 )		newValue = 0.0;
	else if( newValue > 1.0 )	newValue = 1.0;

	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{
		Layer * l = m_allLayers[LayerHandle];
		if( l->type == ImageLayer )
		{	
			IImage * iimg = NULL;
			l->object->QueryInterface(IID_IImage,(void**)&iimg);
			if( iimg == NULL )	return;
			iimg->put_TransparencyPercent(newValue);
			iimg->Release(); iimg = NULL;
		}
		else
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
	}
	else
		ErrorMessage(tkINVALID_LAYER_HANDLE);
}

// ***************************************************
//  SetImageLayerColorScheme
// ***************************************************
// Will be deprecated
VARIANT_BOOL CMapView::SetImageLayerColorScheme(LONG LayerHandle, IDispatch* ColorScheme)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(IS_VALID_LAYER(LayerHandle,m_allLayers))
	{
		Layer * l = m_allLayers[LayerHandle];
		if(l->type == ImageLayer)
		{
			ImageLayerInfo * ili = (ImageLayerInfo*)(l->addInfo);

			if(ili->ColorScheme != NULL)
				ili->ColorScheme->Release();

			ili->ColorScheme = ColorScheme;

			if(ColorScheme!= NULL)
				ColorScheme->AddRef();

			return VARIANT_TRUE;
		}
		else
		{
			
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);
			return VARIANT_FALSE;
		}
	}
	else
	{
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return VARIANT_FALSE;
	}
}

// deprecated
void CMapView::UpdateImage(LONG LayerHandle)
{
	ErrorMessage(tkPROPERTY_DEPRECATED);
}

// ***************************************************************
//		AdjustLayerExtents()
// ***************************************************************
BOOL CMapView::AdjustLayerExtents(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{	
		Layer * l = m_allLayers[LayerHandle];
		if(l->object == NULL) return FALSE;
		
		if (l->type == ImageLayer)
		{
			IImage * iimg = NULL;
			l->object->QueryInterface(IID_IImage,(void**)&iimg);
			if( iimg == NULL )return FALSE;
			double xllCenter=0, yllCenter=0, dx=0, dy=0;
			long width=0, height=0;
			
			iimg->get_OriginalXllCenter(&xllCenter);
			iimg->get_OriginalYllCenter(&yllCenter);
			iimg->get_OriginalDX(&dx);
			iimg->get_OriginalDY(&dy);

			//iimg->GetOriginalWidth(&width);
			//iimg->GetOriginalHeight(&height);	
			iimg->get_OriginalWidth(&width);
			iimg->get_OriginalHeight(&height);	
			l->extents = Extent( xllCenter, xllCenter + dx*width, yllCenter, yllCenter + dy*height );
			iimg->Release();
			iimg = NULL;
			return TRUE;
		}
		else if (l->type == ShapefileLayer)
		{
			IShapefile * ishp = NULL;
			l->object->QueryInterface(IID_IShapefile,(void**)&ishp);
			if (ishp == NULL) return FALSE;
			IExtents * box = NULL;
			ishp->get_Extents(&box);
			double xm,ym,zm,xM,yM,zM;
			box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
			l->extents = Extent(xm,xM,ym,yM);
			box->Release();
			box = NULL;
			ishp->Release();
			ishp=NULL;
			return TRUE;
		}
		else return FALSE;

	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return FALSE;
	}
}

#pragma endregion

#pragma region REGION Methods
void CMapView::LockWindow(short LockMode)
{
	if( LockMode == lmUnlock )
	{
		m_lockCount--;
		if( m_lockCount <= 0 )
		{
			m_lockCount = 0;
			ReloadImageBuffers();
			InvalidateControl();
		}
	}
	else if( LockMode == lmLock )
		m_lockCount++;

}

void CMapView::Resize(long Width, long Height)
{
	POINTL pl;
	pl.x = Width;
	pl.y = Height;

	POINTF pf;
	pf.x = Width;
	pf.y = Height;

	TransformCoords( &pl, &pf, XFORMCOORDS_SIZE | XFORMCOORDS_CONTAINERTOHIMETRIC );

	CSize size;
	size.cx = pl.x;
	size.cy = pl.y;

	CDC *dc = GetDC();
	dc->HIMETRICtoDP( &size );
	ReleaseDC( dc );

	CRect rect;
	GetRectInContainer( rect );
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;

	SetRectInContainer( rect );

	OnSize( SIZE_RESTORED, size.cx, size.cy );
}

void CMapView::Redraw()
{
	m_canbitblt = FALSE;
	InvalidateControl();
	ReloadImageBuffers();
}
void CMapView::ShowToolTip(LPCTSTR Text, long Milliseconds)
{
	m_ttip.UpdateTipText(Text,this,IDC_TTBTN);
	m_ttip.SetDelayTime(TTDT_AUTOPOP,Milliseconds);
	KillTimer(HIDETEXT);
	SetTimer(SHOWTEXT,0,NULL);

	if( Milliseconds < 0 )
		Milliseconds = 0;

	SetTimer(HIDETEXT,Milliseconds,NULL);
}

BSTR CMapView::GetErrorMsg(long ErrorCode)
{
	return A2BSTR(ErrorMsg(ErrorCode));
}

long CMapView::GetLastErrorCode()
{
	long lec = m_lastErrorCode;
	m_lastErrorCode = tkNO_ERROR;
	return lec;
}

/***********************************************************************/
/*						ErrorMessage()						           */
/***********************************************************************/
inline void CMapView::ErrorMessage(long ErrorCode)
{
	USES_CONVERSION;
	m_lastErrorCode = ErrorCode;
	if( m_globalCallback != NULL) 
		m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
	return;
}

BOOL CMapView::IsSameProjection(LPCTSTR proj4_a, LPCTSTR proj4_b)
{
	ProjectionTools * pt = new ProjectionTools();
	bool rt = pt->IsSameProjection(proj4_a, proj4_b);
	delete pt;

	return (rt ? TRUE : FALSE);
}

BOOL CMapView::IsTIFFGrid(LPCTSTR Filename)
{
	try
	{
		TIFF 	*tiff=(TIFF*)0;  // TIFF-level descriptor 
		const TIFFCodec *tCodec=(TIFFCodec*)0;

		tiff = XTIFFOpen((char *)Filename, "r"); 
		if (tiff)
		{
			int w=0, h=0;

			tdir_t d = 0;
			TIFFSetDirectory(tiff,d);

			uint32 SamplesPerPixel = 0;

			TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH, &w);
			TIFFGetField(tiff,TIFFTAG_IMAGELENGTH, &h);
			TIFFGetField(tiff,TIFFTAG_SAMPLESPERPIXEL, &SamplesPerPixel);

			uint16 photo = 0;
			// If it's a color-mapped palette, consider it an image --
			// it's probably an image (USGS DLG or USGS Quad Map most commonly)
			TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &photo);

			XTIFFClose(tiff);

			if(photo == PHOTOMETRIC_PALETTE) // && SamplesPerPixel != 1)
			{
				return false;
			}

			else if (SamplesPerPixel == 1)
				return true;
			else
				return false;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}



#pragma endregion

#pragma region REGION LayerUpdate
// **********************************************************
//			GetDrawingLabels()
// **********************************************************
// Deletes dynamically alocated frames info for all layers; drops isDrawn flag
void CMapView::ClearLabelFrames()
{
	// clear frames for regular labels
	for (int i = 0; i < (int)m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			// charts
			if (l->type == ShapefileLayer)
			{
				IShapefile * sf = NULL;
				l->object->QueryInterface(IID_IShapefile, (void**)&sf);
				if (sf != NULL)
				{
					((CShapefile*)sf)->ClearChartFrames();
				}
				sf->Release();
			}
			
			// labels
			ILabels* LabelsClass = l->get_Labels();
			if (LabelsClass == NULL) continue;
			
			CLabels* coLabels = static_cast<CLabels*>(LabelsClass);
			coLabels->ClearLabelFrames();
			LabelsClass->Release(); LabelsClass = NULL;
		}
	}

	// clear frames for drawing labels
	for(int j = 0; j < m_activeDrawLists.size(); j++ )
	{
		bool isSkip = false;
		for (int i = 0; i < DrawingLayerInVisilbe.size(); i++)
		{
			if (DrawingLayerInVisilbe[i] == j)
			{
				isSkip = true;	// skip if this layer is set invisiable
				break;  
			}
		}
		if(isSkip) 
			continue;

		DrawList * dlist = m_allDrawLists[m_activeDrawLists[j]];
		if( IS_VALID_PTR(dlist) )
		{
			if (dlist->listType == dlSpatiallyReferencedList)
			{
				CLabels* coLabels = static_cast<CLabels*>(dlist->m_labels);
				coLabels->ClearLabelFrames();
			}
		}
	}
}

// ***************************************************************
//		AlignShapeLayerAndShapes()
// ***************************************************************
// Refreshes drawing options for shapes (old implementation), creates
// or deletes if necessary. If the shape count was changed, the options
// will be initialize with default values
void CMapView::AlignShapeLayerAndShapes(Layer * layer)
{
	IDispatch * object = layer->object;
	IShapefile * ishp = NULL;

	object->QueryInterface(IID_IShapefile,(void**)&ishp);

	long numShapes = 0;
	if( ishp == NULL )
	{
		numShapes = 0;
	}
	else
	{	
		ishp->get_NumShapes(&numShapes);
		ishp->Release();
		ishp = NULL;
	}

	ShapeLayerInfo * sli = (ShapeLayerInfo*)layer->addInfo;
	
	if (m_ShapeDrawingMethod == dmNewSymbology)
	{
		// clear all the info; it's not used in this mode		
		for(unsigned int i = 0; i < sli->shapeInfo.size(); i++ )
		{
			delete sli->shapeInfo[i];
			sli->shapeInfo[i] = NULL;
		}
		sli->shapeInfo.clear();
	}
	else
	{
		// generate and refresh the info
		if( sli->shapeInfo.size() == numShapes )
		{
			return;
		}
		else
		{	
			m_canbitblt = FALSE;
			register int i, j;
			long endcondition = sli->shapeInfo.size();

			//Try to make the loop smarter
			if( endcondition < numShapes )
			{
				for( i = endcondition; i < numShapes; i++ )
				{	
					ShapeInfo * sla = new ShapeInfo();
					sli->shapeInfo.push_back(sla);
				}
			}
			else
			{
				for( i = endcondition-1; i >= numShapes; i-- )
				{
					delete sli->shapeInfo[i];
					sli->shapeInfo[i] = NULL;
					sli->shapeInfo.erase( sli->shapeInfo.begin() + i );
				}
			}

			for( j = 0; j < numShapes; j++ )
			{
				sli->AlignShapeInfo( *(sli->shapeInfo[j]) );
			}
		}
	}
}
#pragma endregion

#pragma region REGION Others
BOOL CMapView::ApplyLegendColors(LPDISPATCH pLegend)
{
	USES_CONVERSION;

	m_legendMutex.Lock();

	if( pLegend == NULL )
	{
		m_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		m_legendMutex.Unlock();

		return FALSE;
	}

	//Query the Interface
	IShapefileColorScheme * legend = NULL;
	extern const IID IID_IShapefileColorScheme;
	pLegend->QueryInterface( IID_IShapefileColorScheme, (void**)&legend );

	if( legend != NULL )
	{	//Test for a valid Shapefile
		long layerHandle = -1;
		legend->get_LayerHandle( &layerHandle );
		if( IS_VALID_LAYER(layerHandle,m_allLayers) )
		{
			Layer * l = m_allLayers[layerHandle];
			if( l->type == ShapefileLayer )
			{
				ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
				m_canbitblt = FALSE;

				IShapefile * object = NULL;
				l->object->QueryInterface(IID_IShapefile,(void**)&object);

				ShpfileType shptype = SHP_NULLSHAPE;
				object->get_ShapefileType(&shptype);

				//Test for a clear legend call ... A legend w/no breaks
				long cl_numbreaks;
				legend->NumBreaks(&cl_numbreaks);
				if( cl_numbreaks == 0 )
				{
					long numshapes = sli->shapeInfo.size();
					register int i;
					for( i = 0; i < numshapes; i++ )
						sli->AlignShapeInfo(*(sli->shapeInfo[i]));

					if( sli->legend )
						sli->legend->Release();
					sli->legend = NULL;
					legend->Release();
					legend = NULL;
					object->Release();
					object = NULL;

					m_canbitblt = FALSE;
					if( !m_lockCount )
						InvalidateControl();

					m_legendMutex.Unlock();

					return TRUE;
				}


				long numFields = 0;
				object->get_NumFields(&numFields);
				long field = -1;
				legend->get_FieldIndex(&field);

				//Test for a valid Field
				if( field >= 0 && field < numFields )
				{
					IField * iField = NULL;
					object->get_Field( field, &iField );

					FieldType dbftype;
					iField->get_Type(&dbftype);

					long percent = 0;
					long newPercent = 0;

					register int i;
					long endcondition = sli->shapeInfo.size();
					for( i = 0; i < endcondition; i++ )
					{
						long icellValue = 0;
						double dcellValue = 0.0;
						CString scellValue = "";

						VARIANT v;
						VariantInit(&v);

						object->get_CellValue(field,i,&v);

						//Added by Rob Cairns 22-Mar-06 to handle null values in the attribute table
						bool nullValFound = false;
						if ( v.vt == VT_NULL || v.vt ==VT_EMPTY )
							nullValFound = true;
						else
						{
							if( dbftype == INTEGER_FIELD )
								lVal(v,icellValue);
							else if( dbftype == DOUBLE_FIELD )
								dVal(v,dcellValue);
							else if( dbftype == STRING_FIELD )
								stringVal(v,scellValue);
						}
						VariantClear(&v);

						long numBreaks = 0;
						legend->NumBreaks(&numBreaks);
						VARIANT startValue, endValue;
						OLE_COLOR startColor, endColor;

						register int j;

						for( j = 0; j < numBreaks; j++ )
						{	IShapefileColorBreak * lBreak = NULL;
							legend->get_ColorBreak( j, &lBreak );

							VariantInit(&startValue);
							VariantInit(&endValue);

							lBreak->get_StartValue(&startValue);
							lBreak->get_EndValue(&endValue);

							bool betweenBreak = false;

							//Added by Rob Cairns 22-Mar-06 to handle null values in the color break
							if (startValue.vt == VT_NULL || startValue.vt == VT_EMPTY )
							{							
								if (nullValFound)
									betweenBreak = true;
								//Seems to be OK to leave it at that as long as the color break value sent
								//though is = nothing (VB) or = null (C#)
							}
							else
							{
								if( dbftype == INTEGER_FIELD )
								{	long iendValue, istartValue;
									lVal( startValue, istartValue );
									lVal( endValue, iendValue );
									if( istartValue <= icellValue && icellValue <= iendValue )
										betweenBreak = true;
								}
								else if( dbftype == DOUBLE_FIELD )
								{	double dendValue, dstartValue;
									dVal( startValue, dstartValue );
									dVal( endValue, dendValue );
									if( dstartValue <= dcellValue && dcellValue <= dendValue )
										betweenBreak = true;
								}
								else if( dbftype == STRING_FIELD )
								{	CString sendValue;
									CString sstartValue;
									stringVal(endValue,sendValue);
									stringVal(startValue,sstartValue);

									if( scellValue.Compare(sendValue) <= 0 && scellValue.Compare(sstartValue) >= 0 )
										betweenBreak = true;
								}
							}
							if( betweenBreak == true )
							{
								lBreak->get_StartColor( &startColor );
								lBreak->get_EndColor( &endColor );

								long r1, g1, b1;
								long r2, g2, b2;
								long fR, fG, fB;
								r1 = GetRValue(startColor);
								g1 = GetGValue(startColor);
								b1 = GetBValue(startColor);
								r2 = GetRValue(endColor);
								g2 = GetGValue(endColor);
								b2 = GetBValue(endColor);

								if( dbftype == DOUBLE_FIELD || dbftype == INTEGER_FIELD )
								{
									double cellValue = 0.0;
									if( dbftype == DOUBLE_FIELD )
										cellValue = dcellValue;
									else if( dbftype == INTEGER_FIELD )
										cellValue = icellValue;

									double dstartValue, dendValue;
									dVal(startValue,dstartValue);
									dVal(endValue,dendValue);
									double range = dendValue - dstartValue;
									if( range > 0.0 )
									{	double valPercent = ( cellValue - dstartValue ) / range;
										double iValPercent = 1.0 - valPercent;

										fR = r2*valPercent + r1*iValPercent;
										fG = g2*valPercent + g1*iValPercent;
										fB = b2*valPercent + b1*iValPercent;
									}
									else
									{	fR = r1;
										fG = g1;
										fB = b1;
									}
								}
								else
								{
									CString sendValue;
									CString sstartValue;
									stringVal(endValue,sendValue);
									stringVal(startValue,sstartValue);

									if( scellValue.Compare(sendValue) == 0 )
									{
										fR = r2;
										fG = g2;
										fB = b2;
									}
									else if( scellValue.Compare(sstartValue) == 0 )
									{
										fR = r1;
										fG = g1;
										fB = b1;
									}
									else
									{
										fR = (r1+r2)*.5;
										fG = (g1+g2)*.5;
										fB = (b1+b2)*.5;
									}
								}

								VARIANT_BOOL vis;
								lBreak->get_Visible(&vis);
								if (vis == VARIANT_FALSE)
									sli->shapeInfo[i]->flags = sli->shapeInfo[i]->flags  & (0xFFFFFFFF ^ sfVisible);
								else
									sli->shapeInfo[i]->flags |= sfVisible;

								if( shptype == SHP_POINT || shptype == SHP_POINTZ || shptype == SHP_POINTM )
									sli->shapeInfo[i]->pointClr = RGB( fR, fG, fB );
								else if( shptype == SHP_POLYLINE || shptype == SHP_POLYLINEZ || shptype == SHP_POLYLINEM )
									sli->shapeInfo[i]->lineClr = RGB( fR, fG, fB );
								else if( shptype == SHP_POLYGON || shptype == SHP_POLYGONZ || shptype == SHP_POLYGONM )
									sli->shapeInfo[i]->fillClr = RGB( fR, fG, fB );
								else if( shptype == SHP_MULTIPOINT || shptype == SHP_MULTIPOINTZ || shptype == SHP_MULTIPOINTM )
									sli->shapeInfo[i]->pointClr = RGB( fR, fG, fB );

								if( lBreak != NULL )
									lBreak->Release();
								lBreak = NULL;

								VariantClear(&startValue);
								VariantClear(&endValue);
								break;
							}

							VariantClear(&startValue);
							VariantClear(&endValue);

							if( lBreak != NULL )
								lBreak->Release();
							lBreak = NULL;
						}

						if( m_globalCallback )
						{
							newPercent = (i+1.0/sli->shapeInfo.size())*100;
							if( newPercent > percent )
							{	percent = newPercent;
								m_globalCallback->Progress(m_key.AllocSysString(),percent,A2BSTR("Applying Legend Colors"));
							}
						}
					}

					iField->Release();
					iField = NULL;

					//Release the old legend
					if( sli->legend != NULL )
						sli->legend->Release();
					sli->legend = NULL;

					//Legend will be released in the destructor
					sli->legend = legend;

					object->Release();
					object = NULL;

					m_legendMutex.Unlock();

					m_canbitblt = FALSE;
					if( !m_lockCount )
						InvalidateControl();

					return TRUE;
				}
				else
				{	m_lastErrorCode = tkINDEX_OUT_OF_BOUNDS;
					if( m_globalCallback != NULL )
						m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
					legend->Release();
					legend = NULL;
					object->Release();
					object = NULL;
					m_legendMutex.Unlock();

					return FALSE;
				}
			}
			else
			{	m_lastErrorCode = tkUNEXPECTED_LAYER_TYPE;
				if( m_globalCallback != NULL )
					m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
				legend->Release();
				legend = NULL;
				m_legendMutex.Unlock();

				return FALSE;
			}
		}
		else
		{	m_lastErrorCode = tkINVALID_LAYER_HANDLE;
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
			legend->Release();
			legend = NULL;
			m_legendMutex.Unlock();

			return FALSE;
		}
	}
	else
	{	m_lastErrorCode = tkINTERFACE_NOT_SUPPORTED;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		m_legendMutex.Unlock();

		return FALSE;
	}
}

LPDISPATCH CMapView::GetColorScheme(long LayerHandle)
{
	if( IS_VALID_LAYER(LayerHandle,m_allLayers) )
	{	Layer * l = m_allLayers[LayerHandle];
		if( l->type == ShapefileLayer )
		{	
			ShapeLayerInfo * sli = (ShapeLayerInfo*)l->addInfo;
			if( sli->legend != NULL )
				sli->legend->AddRef();
			return sli->legend;
		}
		else if(l->type == ImageLayer)
		{
			ImageLayerInfo* ili = (ImageLayerInfo*)(l->addInfo);
			if( ili->ColorScheme != NULL)
				ili->ColorScheme->AddRef();

			return (ili->ColorScheme);
		}
		else
		{
			ErrorMessage(tkUNEXPECTED_LAYER_TYPE);	
			return NULL;
		}
	}
	else
	{	
		ErrorMessage(tkINVALID_LAYER_HANDLE);
		return NULL;
	}
}

inline double CMapView::makeVal( const char * sVal )
{
	double val = 0.0;

	if( sVal != NULL )
	{	register int i;
		for( i = 0; i < _tcslen( sVal ); i++ )
		{
			char c = sVal[i];
			if( isalpha(c) || isdigit(c) )
			{	c = toupper(c);

				//Adjust by the ASCII value of 0
				c = c - 48;

				//90-48 = range
				val += ((double)c)*(42*pow((double)10, (double)i));
			}
		}
	}

	return val;
}


BSTR CMapView::GetGridFileName(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString retval;

	if(IS_VALID_LAYER(LayerHandle,m_allLayers))
	{
		Layer * l = m_allLayers[LayerHandle];
		if(l->type == ImageLayer)
		{
			ImageLayerInfo * ili = (ImageLayerInfo*)(l->addInfo);
			return ili->GridFileName.AllocSysString();
		}
		else
		{
			m_lastErrorCode = tkUNEXPECTED_LAYER_TYPE;
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
			return retval.AllocSysString();
		}
	}
	else
	{
		m_lastErrorCode = tkINVALID_LAYER_HANDLE;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		return retval.AllocSysString();
	}
}

void CMapView::SetGridFileName(LONG LayerHandle, LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(IS_VALID_LAYER(LayerHandle,m_allLayers))
	{
		Layer * l = m_allLayers[LayerHandle];
		if(l->type == ImageLayer)
		{
			ImageLayerInfo * ili = (ImageLayerInfo*)(l->addInfo);

			ili->GridFileName = newVal;
			return;
		}
		else
		{
			m_lastErrorCode = tkUNEXPECTED_LAYER_TYPE;
			if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
			return;
		}
	}
	else
	{
		m_lastErrorCode = tkINVALID_LAYER_HANDLE;
		if( m_globalCallback != NULL )
			m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		return;
	}
}

#pragma endregion

// ********************************************************************
//		DrawBackBuffer()
// ********************************************************************
// Draws the backbuffer to the specified DC (probably external)
void CMapView::DrawBackBuffer(int** hdc, int ImageWidth, int ImageHeight)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (!hdc)
	{
		m_lastErrorCode = tkUNEXPECTED_NULL_PARAMETER;
		return;
	}
	
	CDC* dc = CDC::FromHandle((HDC)hdc);
	CRect rect(0,0, ImageWidth, ImageHeight);
	OnDraw(dc, rect, rect);
}



// **************************************************************************
//		SetWaitCursor()
// **************************************************************************
void CMapView::SetWaitCursor()
{
	CPoint cpos;
	GetCursorPos(&cpos);
	CRect wrect;
	GetWindowRect(&wrect);
	
	HWND wndActive = ::GetActiveWindow();
	if ((wndActive == this->GetSafeHwnd()) || (wndActive == this->GetParentOwner()->GetSafeHwnd()))
	{
		if( wrect.PtInRect(cpos) && (m_mapCursor != crsrUserDefined) && !m_DisableWaitCursor)
		{
			::SetCursor(LoadCursor(NULL, IDC_WAIT) );
		}
	}
}

// *********************************************************
//		DrawOldLabels()
// *********************************************************
void CMapView::DrawOldLabels(CDC* dc)
{
	if ((m_ShapeDrawingMethod == dmStandard || m_ShapeDrawingMethod == dmNewWithSelection) && !FORCE_NEW_LABELS)
	{
		while(m_labelsToDraw.size() > 0)
		{
			DrawLabels(dc,m_labelsToDraw.top());
			m_labelsToDraw.pop();
		}

		while(m_labelsDrawingLists.size() > 0)
		{
			DrawLabels(dc,m_labelsDrawingLists.back());
			m_labelsDrawingLists.pop_back();
		}
		
		if(m_labelExtentsDrawn.size() > 0)
		{
			m_labelExtentsDrawn.clear();
		}
	}
}


// *****************************************************
//		GetPixelsPerDegree
// *****************************************************
DOUBLE CMapView::GetPixelsPerDegree(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	double val = 1.0;
	
	if (this->m_unitsOfMeasure != umDecimalDegrees)
	{
		if (!Utility::ConvertDistance(this->m_unitsOfMeasure, umDecimalDegrees, val))
		{
			return 0.0;
		}
	}
    
	double x, y;
    x = y = 0.0;
    double screenX = 0.0, screenY = 0.0;
    this->ProjToPixel(x, y, &screenX, &screenY);
    double x1 = screenX;

    x = y = val;
    this->ProjToPixel(x, y, &screenX, &screenY);
    return abs(screenX - x1);
}

// *****************************************************
//		SetPixelsPerDegree
// *****************************************************
void CMapView::SetPixelsPerDegree(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SetNotSupported();
}

// *****************************************************
//		ZoomToSelected()
// *****************************************************
VARIANT_BOOL CMapView::ZoomToSelected(LONG LayerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	long numSelected = 0;

	IShapefile* sf = this->GetShapefile(LayerHandle);
	if (sf)
	{
		long numShapes;
		sf->get_NumShapes(&numShapes);
		sf->get_NumSelected(&numSelected);
		
		if (numSelected > 0)
		{
			double xMin, xMax, yMin, yMax, zMin, zMax;
			double _minX, _maxX, _minY, _maxY;
			bool first = true;
			for (int i = 0; i < numShapes; i++)
			{
				VARIANT_BOOL selected;
				sf->get_ShapeSelected(i, &selected);
				if (selected)
				{
					IExtents* extents = NULL;
					sf->QuickExtents(i, &extents);
					if (extents)
					{
						extents->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);
						extents->Release();

						if (first)
						{
							_minX = xMin, _maxX = xMax;
							_minY = yMin, _maxY = yMax;
							first = false;
						}
						else	
						{	if( xMin < _minX )	_minX = xMin; 
							if( xMax > _maxX )	_maxX = xMax;
							if( yMin < _minY )	_minY = yMin;
							if( yMax > _maxY )	_maxY = yMax;
						}
					}
				}
			}
			
			IExtents* bounds = NULL;
			CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&bounds);
			bounds->SetBounds(_minX, _minY, 0.0, _maxX, _maxY, 0.0);
			this->SetExtents(bounds);
			bounds->Release();
		}
		sf->Release();
	}
	return numSelected > 0 ? VARIANT_TRUE : VARIANT_FALSE;
}
