// ********************************************************************************************************
// File name: Map.cpp
// Description: Implementation of the CMapView ActiveX Control class.
// ********************************************************************************************************
// The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at 
// http://www.mozilla.org/MPL/ 
// Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
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

#pragma region Include
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"

#include <fstream>
#include <vector>
#include <atlsafe.h>
#include "MapPpg.h"
#include "Measuring.h"
#include "Tiles.h"
#pragma endregion

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


#pragma region Message map
BEGIN_MESSAGE_MAP(CMapView, COleControl)
	//{{AFX_MSG_MAP(CMapView)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()

	//ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()
#pragma endregion


#pragma region Dispatch map
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
	DISP_FUNCTION_ID(CMapView, "SetImageLayerColorScheme", dispidSetImageLayerColorScheme, SetImageLayerColorScheme, VT_BOOL, VTS_I4 VTS_DISPATCH)
	DISP_FUNCTION_ID(CMapView, "UpdateImage", dispidUpdateImage, UpdateImage, VT_EMPTY, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "SerialNumber", dispidSerialNumber, GetSerialNumber, SetSerialNumber, VT_BSTR)
	DISP_PROPERTY_EX_ID(CMapView, "LineSeparationFactor", dispidLineSeparationFactor, GetLineSeparationFactor, SetLineSeparationFactor, VT_UI1)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerName", dispidLayerName, GetLayerName, SetLayerName, VT_BSTR, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "GridFileName", dispidGridFileName, GetGridFileName, SetGridFileName, VT_BSTR, VTS_I4)
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
	DISP_PROPERTY_NOTIFY_ID(CMapView, "SendOnDrawBackBuffer", dispidSendOnDrawBackBuffer, m_sendOnDrawBackBuffer, OnSendOnDrawBackBufferChanged, VT_BOOL)
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
	DISP_FUNCTION_ID(CMapView, "SnapShotToDC2", dispidSnapShotToDC2, SnapShotToDC2, VT_BOOL,VTS_I4 VTS_DISPATCH VTS_I4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4 VTS_R4)
	DISP_FUNCTION_ID(CMapView, "LoadTilesForSnapshot", dispidLoadTilesForSnapshot, LoadTilesForSnapshot, VT_EMPTY,VTS_DISPATCH VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION_ID(CMapView, "DrawWideCircleEx", dispidDrawWideCircleEx, DrawWideCircleEx, VT_EMPTY, VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_COLOR VTS_BOOL VTS_I2)
	DISP_FUNCTION_ID(CMapView, "DrawWidePolygonEx", dispidDrawWidePolygonEx, DrawWidePolygonEx, VT_EMPTY, VTS_I4 VTS_VARIANT VTS_VARIANT VTS_I4 VTS_COLOR VTS_BOOL VTS_I2)
	DISP_FUNCTION_ID(CMapView, "TilesAreInCache", dispidTilesAreInCache, TilesAreInCache, VT_I4,VTS_DISPATCH VTS_I4 VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "DegreeRotationAngle", dispidMapRotationAngle, GetMapRotationAngle, SetMapRotationAngle, VT_R4)
	DISP_PROPERTY_EX_ID(CMapView, "RotatedExtent", dispidRotatedExtent, GetRotatedExtent, SetNotSupported, VT_DISPATCH)
    DISP_FUNCTION_ID(CMapView, "GetBaseProjectionPoint", dispidGetBaseProjectionPoint, GetBaseProjectionPoint, VT_DISPATCH, VTS_R8 VTS_R8)
	DISP_PROPERTY_EX_ID(CMapView, "CanUseImageGrouping", dispidCanUseImageGrouping, GetCanUseImageGrouping, SetCanUseImageGrouping, VT_BOOL)
	DISP_PROPERTY_EX_ID(CMapView, "VersionNumber", dispidVersionNumber, GetVersionNumber, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerMaxVisibleScale", dispidLayerMaxVisibleScale, GetLayerMaxVisibleScale, SetLayerMaxVisibleScale, VT_R8, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerMinVisibleScale", dispidLayerMinVisibleScale, GetLayerMinVisibleScale, SetLayerMinVisibleScale, VT_R8, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerMaxVisibleZoom", dispidLayerMaxVisibleZoom, GetLayerMaxVisibleZoom, SetLayerMaxVisibleZoom, VT_I4, VTS_I4)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerMinVisibleZoom", dispidLayerMinVisibleZoom, GetLayerMinVisibleZoom, SetLayerMinVisibleZoom, VT_I4, VTS_I4)
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
	DISP_PROPERTY_EX_ID(CMapView, "MaxExtents", dispidMaxExtents, GetMaxExtents, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CMapView, "PixelsPerDegree", dispidPixelsPerDegree, GetPixelsPerDegree, SetNotSupported, VT_R8)
	DISP_PROPERTY_EX_ID(CMapView, "Tiles", dispidTiles, GetTiles, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_PARAM_ID(CMapView, "LayerFilename", dispidLayerFilename, GetLayerFilename, SetNotSupported, VT_BSTR, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "ZoomToSelected", dispidZoomToSelected, ZoomToSelected, VT_BOOL, VTS_I4)
	DISP_PROPERTY_EX_ID(CMapView, "Projection", dispidProjection, GetGeoProjection, SetGeoProjection, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CMapView, "GeographicExtents", dispidGeographicExtents, GetGeographicExtents, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION_ID(CMapView, "SetGeographicExtents", dispidSetGeographicExtents, SetGeographicExtents, VT_BOOL, VTS_DISPATCH)
	DISP_PROPERTY_EX_ID(CMapView, "ScalebarVisible", dispidScalebarVisible, GetScalebarVisible, SetScalebarVisible, VT_BOOL)
	DISP_PROPERTY_EX_ID(CMapView, "ScalebarUnits", dispidScalebarUnits, GetScalebarUnits, SetScalebarUnits, VT_I2)
	DISP_PROPERTY_EX_ID(CMapView, "Measuring", dispidMeasuring, GetMeasuring, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION_ID(CMapView, "ZoomToTileLevel", dispidZoomToTileLevel, ZoomToTileLevel, VT_BOOL, VTS_I4)
	DISP_FUNCTION_ID(CMapView, "ZoomToWorld", dispidZoomToWorld, ZoomToWorld, VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID(CMapView, "FindSnapPoint", dispidFindSnapPoint, FindSnapPoint, VT_BOOL, VTS_R8 VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION_ID(CMapView, "Clear", dispidClear, Clear, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMapView, "SetGeographicExtents2", dispidSetGeographicExtents2, SetGeographicExtents2, VT_BOOL, VTS_R8 VTS_R8 VTS_R8)
	DISP_FUNCTION_ID(CMapView, "AddLayerFromFilename", dispidAddLayerFromFilename, AddLayerFromFilename, VT_I4, VTS_BSTR VTS_I2 VTS_BOOL)
	DISP_FUNCTION_ID(CMapView, "GetKnownExtents", dispidGetKnownExtents, GetKnownExtents, VT_DISPATCH, VTS_I2)
	DISP_PROPERTY_EX_ID(CMapView, "ZoomBehavior", dispidZoomBehavior, GetZoomBehavior, SetZoomBehavior, VT_I2)
	DISP_PROPERTY_EX_ID(CMapView, "FileManager", dispidFileManager, GetFileManager, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CMapView, "Latitude", dispidLatitude, GetLatitude, SetLatitude, VT_R4)
	DISP_PROPERTY_EX_ID(CMapView, "Longitude", dispidLongitude, GetLongitude, SetLongitude, VT_R4)
	DISP_PROPERTY_EX_ID(CMapView, "CurrentZoom", dispidCurrentZoom, GetCurrentZoom, SetCurrentZoom, VT_I4)
	DISP_PROPERTY_EX_ID(CMapView, "TileProvider", dispidTileProvider, GetTileProvider, SetTileProvider, VT_I2)
	DISP_PROPERTY_EX_ID(CMapView, "Projection", dispidMapProjection, GetProjection, SetProjection, VT_I2)
	DISP_PROPERTY_EX_ID(CMapView, "KnownExtents", dispidKnownExtents, GetKnownExtentsCore, SetKnownExtentsCore, VT_I2)
	DISP_PROPERTY_EX_ID(CMapView, "ShowCoordinates", dispidShowCoordinates, GetShowCoordinates, SetShowCoordinates, VT_I2)
	DISP_PROPERTY_EX_ID(CMapView, "GrabProjectionFromData", dispidGrabProjectionFromData, GetGrabProjectionFromData, SetGrabProjectionFromData, VT_I2)
	DISP_FUNCTION_ID(CMapView, "Redraw2", dispidRedraw2, Redraw2, VT_EMPTY, VTS_I2)
	DISP_FUNCTION_ID(CMapView, "ProjToDegrees", dispidProjToDegrees, ProjToDegrees, VT_BOOL, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION_ID(CMapView, "DegreesToProj", dispidDegreesToProj, DegreesToProj, VT_BOOL, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION_ID(CMapView, "PixelToDegrees", dispidPixelToDegrees, PixelToDegrees, VT_BOOL, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	DISP_FUNCTION_ID(CMapView, "DegreesToPixel", dispidDegreesToPixel, DegreesToPixel, VT_BOOL, VTS_R8 VTS_R8 VTS_PR8 VTS_PR8)
	
END_DISPATCH_MAP()
//}}AFX_DISPATCH_MAP
#pragma endregion


#pragma region Event map
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
#pragma endregion


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


#pragma region Registration

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
#pragma endregion

#pragma region Constructor/destructor


// ********************************************************************
//		CMapView() constructor
// ********************************************************************
CMapView::CMapView() : vals("AZ0CY1EX2GV3IT4KR5MP6ON7QL8SJ9UH0WF1DB2"), valsLen(39), m_isSnapshot(false)
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
}

// **********************************************************************
//	Startup
// **********************************************************************
// Must be called from constructor only
void CMapView::Startup()
{
	this->GdiplusStartup();
	InitializeIIDs(&IID_DMap, &IID_DMapEvents);
	
	// TODO: release in destructor
	Gdiplus::FontFamily family(L"Courier New");
	_font = new Gdiplus::Font(&family, (Gdiplus::REAL)9.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);

	_canUseLayerBuffer = FALSE;
	_canUseMainBuffer = false;
	m_leftButtonDown = FALSE;
	m_bitbltClickMove = CPoint(0,0);
	m_bitbltClickDown = CPoint(0,0);

	m_globalCallback = NULL;

	m_key = "";

	m_viewHeight = 0;
	m_viewWidth = 0;

	//Cursors
	m_cursorPan = AfxGetApp()->LoadCursor(IDC_PAN);
	m_cursorZoomin = AfxGetApp()->LoadCursor(IDC_ZOOMIN);
	m_cursorZoomout = AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
	m_cursorSelect = AfxGetApp()->LoadCursor(IDC_SELECT);
	m_cursorMeasure = AfxGetApp()->LoadCursor(IDC_MEASURE);
	m_udCursor = NULL;

	m_lockCount = 0;

	m_currentDrawing = -1;

	m_ttipCtrl = new CButton();
	m_showingToolTip = FALSE;

	m_drawMutex.Unlock();
	m_legendMutex.Unlock();
	m_mapstateMutex.Unlock();
	
	_setmaxstdio(2048);

	m_RotateAngle = 0.0f;
	m_Rotate = NULL;

	_canUseImageGrouping = VARIANT_FALSE;

	m_imageGroups = NULL;

	srand (time(NULL));

	m_isSizing = false;

	#ifdef _DEBUG
	gMemLeakDetect.stopped = true;
	#endif

	GetUtils()->CreateInstance(idTiles, (IDispatch**)&m_tiles);
	((CTiles*)m_tiles)->Init((void*)this);

	GetUtils()->CreateInstance(idMeasuring, (IDispatch**)&m_measuring);
	
	GetUtils()->CreateInstance(idFileManager, (IDispatch**)&_fileManager);

	InitProjections();
	
	((CMeasuring*)m_measuring)->SetMapView((void*)this);

	#ifdef _DEBUG
	gMemLeakDetect.stopped = false;
	#endif

	m_hotTracking.Shapefile = NULL;
	m_hotTracking.LayerHandle = -1;
	m_hotTracking.ShapeId = -1;

	m_rectTrackerIsActive = false;

	m_lastWidthMeters = 0.0;
}

// **********************************************************************
//	SetDefaults
// **********************************************************************
void CMapView::SetDefaults()
{
	// TODO: set defaults for property exchanged
	_grabProjectionFromData = true;
	_hasHotTracking = false;
	_showCoordinates = cdmAuto;
	_knownExtents = keNone;
	_zoomBehavior = zbUseTileLevels;
	_measuringPersistent = false;
	_lastCursorMode = cmNone;
	_reverseZooming = false;
	_activeLayerPosition = 0;
	m_scalebarVisible = VARIANT_TRUE;
	MultilineLabeling = true;
	m_lastErrorCode = tkNO_ERROR;
	rbMapResizeBehavior = rbClassic;
	DoTrapRMouseDown = TRUE;
	m_UseSeamlessPan = FALSE;
	m_MouseWheelSpeed = 0.5;
	m_ShapeDrawingMethod = dmNewSymbology;
	m_unitsOfMeasure = umMeters;
	m_DisableWaitCursor = false;
	m_LineSeparationFactor = 3;		
	m_useLabelCollision = false;
	m_ShowRedrawTime = VARIANT_FALSE;
	m_ShowVersionNumber = VARIANT_FALSE;	
	m_scalebarUnits = tkScalebarUnits::GoogleStyle;
	((CTiles*)m_tiles)->SetDefaults();
	((CMeasuring*)m_measuring)->SetDefaults();
}

// **********************************************************************
//	ReleaseTempObjects
// **********************************************************************
void CMapView::ReleaseTempObjects()
{	
	m_collisionList.Clear();
	
	m_RotateAngle = 0.0f;
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
//	Shutdown
// **********************************************************************
// Must be called from desctructor only
void CMapView::Shutdown()
{
	((CTiles*)m_tiles)->Stop();
	if (m_bufferBitmap)
	{
		delete m_bufferBitmap;
		m_bufferBitmap = NULL;
	}

	if (m_tilesBitmap)
	{
		delete m_tilesBitmap;
		m_tilesBitmap = NULL;
	}

	if (m_drawingBitmap)
	{
		delete m_drawingBitmap;
		m_drawingBitmap = NULL;
	}

	if (m_layerBitmap)
	{
		delete m_layerBitmap;
		m_layerBitmap = NULL;
	}

	if( m_globalCallback )
		m_globalCallback->Release();

	ReleaseProjections();

	if (_fileManager)
		_fileManager->Release();

	if (m_measuring)
		m_measuring->Release();

	if (m_tiles)
	{
		((CTiles*)m_tiles)->ClearAll();
		m_tiles->Release();
	}

	if (m_hotTracking.Shapefile)
		m_hotTracking.Shapefile->Release();

	delete m_ttipCtrl;

	this->GdiplusShutdown();
}

// ********************************************************************
//		OnCreate()
// ********************************************************************
int CMapView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_bufferBitmap = NULL;
	m_tilesBitmap = NULL;
	m_layerBitmap = NULL;
	m_drawingBitmap = NULL;

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
#pragma endregion


#pragma region Overidden properties
// **********************************************************************
//	CMapView::DoPropExchange
// **********************************************************************
// Persistence support
void CMapView::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

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
	PX_Bool( pPX, "ShowRedrawTime", m_ShowRedrawTime, FALSE);
	PX_Bool( pPX, "ShowVersionNumber", m_ShowVersionNumber, FALSE);
	PX_Double( pPX, "MouseWheelSpeed", m_MouseWheelSpeed, .5 );
	PX_Bool( pPX, "UseSeamlessPan", m_UseSeamlessPan, FALSE );
	PX_Bool( pPX, "MultilineLabels", MultilineLabeling, FALSE );
	PX_Bool( pPX, "TrapRMouseDown", DoTrapRMouseDown, TRUE );
	PX_Bool( pPX, "DisableWaitCursor", m_DisableWaitCursor, FALSE );	
	PX_Float( pPX, "MapRotationAngle", m_RotateAngle, 0.0 );
	PX_Bool( pPX, "CanUseImageGrouping", _canUseImageGrouping, FALSE );
	PX_Bool( pPX, "ScalebarVisible", m_scalebarVisible, TRUE );
	
	// enumerated constants aren't supported directly so temp buffer will be used
	// we don't care about the direction of exchanage, doing both getting and setting
	long temp;
	temp = (long)rbMapResizeBehavior;
	PX_Long( pPX, "MapResizeBehavior", temp, 0 );	//rbClassic
	rbMapResizeBehavior = (tkResizeBehavior)temp;

	temp = (long)_zoomBehavior;
	PX_Long( pPX, "ZoomBehavior", temp, 1 );	// zbUseTileLevels
	_zoomBehavior = (tkZoomBehavior)temp;

	temp = (long)m_scalebarUnits;
	PX_Long( pPX, "ScalebarUnits", temp, 2 );	//suGoogleStyle
	m_scalebarUnits = (tkScalebarUnits)temp;

	temp = (long)m_ShapeDrawingMethod;
	PX_Long( pPX, "ShapeDrawingMethod", temp, 3 );	// dmNewSymbology
	m_ShapeDrawingMethod = (tkShapeDrawingMethod)temp;

	temp = (long)m_unitsOfMeasure;
	PX_Long( pPX, "MapUnits", temp, 6 );			//umMeters
	m_unitsOfMeasure = (tkUnitsOfMeasure)temp;

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

	PX_Long( pPX, "Projection", temp, 0 );			// PROJECTION_NONE
	projection = (tkMapProjection)temp;
	if (loading) {
		SetProjection(projection);
	}
	
	PX_Double( pPX, "xMin", extents.left, .3 );
	PX_Double( pPX, "xMax", extents.right, .3 );
	PX_Double( pPX, "yMin", extents.bottom, .3 );
	PX_Double( pPX, "yMax", extents.top, .3 );
	if (loading) {
		SetExtentsCore(extents, false);
	}

	if (loading) {
		this->LockWindow(lmUnlock);
	}

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


