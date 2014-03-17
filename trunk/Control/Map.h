//********************************************************************************************************
//File name: Map.h
//Description: Declaration of the CMapView ActiveX Control class.
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

// --------------------------------------------------------------------------------------------------------
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
// --------------------------------------------------------------------------------------------------------
//3-28-2005 dpa - Updated for custom line stipples and label collision.
//3-16-2006 Chris Michaelis (cdm) -- Added the ability to use multiple (differing) icons 
//			in a single layer through the new functions set_ShapePointImageListID and 
//			set_UDPointImageListAdd; also the new tkPointType enum value "ptImageList".
//			Also a few functions to get the count of images and get an image, etc. The functions have "set_" in
//			them to try to make the functions appear next to the properties in VB intellisense.
//4-07-2007 Tom Shanley (tws) - support for large-scale image exports: added SnapShot2(), m_isSnapshot
//3-16-2009 Ray Quay - Added ability to turn off trapping of right mouse click for zoom, and property
//          to turn trapping on and off.  This allows right click context menus without zoom.
//          Added property TrapRMouseDown:BOOL Functions SetTrapRMouseDown, GetTrapRMouseDown
//7-03-2009 Sergei Leschinski (lsu) UseSeamlessPan property added.
// --------------------------------------------------------------------------------------------------------

#if !defined(AFX_MAP_H__FAF5AFC5_D242_402A_B6E3_E6FF5E2E514C__INCLUDED_)
#define AFX_MAP_H__FAF5AFC5_D242_402A_B6E3_E6FF5E2E514C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma region Include
# include "MapWinGis.h"

# include <stdio.h>
# include <io.h>
# include <vector>
# include <stack>
# include <winbase.h>
# include <afxmt.h>
# include <math.h>
# include <gdiplus.h>

# include "MapTracker.h"
# include "macros.h"
# include "ErrorCodes.h"
# include "ToolTipEx.h"

# include "varH.h"

# include "ShapeLayerInfo.h"
# include "ShapeInfo.h"
# include "ImageLayerInfo.h"
# include "Layer.h"
# include "DrawList.h"

# include "DrawingOptions.h"

# include "GeometryOperations.h"
# include "MapRotate.h"   // ajp
# include "ImageGroup.h"
# include "CollisionList.h"
# include "Measuring.h"

#pragma endregion

# define SHOWTEXT 450
# define HIDETEXT 451

class CMapView : public COleControl
{
	DECLARE_DYNCREATE(CMapView)

public:
	// Constructor
	CMapView();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapView)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	~CMapView();

	DECLARE_OLECREATE_EX(CMapView)    // Class factory and guid
	DECLARE_OLETYPELIB(CMapView)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMapView)     // Property page IDs
	DECLARE_OLECTLTYPE(CMapView)		// Type name and misc status

#pragma region Message maps
	//{{AFX_MSG(CMapView)
	// NOTE - ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg void OnKeyPressEvent(USHORT nChar);
	
	// 64-bit compatibility -- Christopher Michaelis Feb 2009
	#ifdef WIN64
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	#else
	afx_msg void OnTimer(UINT nIDEvent);
	#endif

	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion

#pragma region Dispatch map
	//{{AFX_DISPATCH(CMapView)
	// NOTE - ClassWizard will add and remove member functions here.
	// DO NOT EDIT what you see in these blocks of generated code !
	
	// a function is called on change of value	
	#pragma region Notify properties  
	OLE_COLOR m_backColor;
	afx_msg void OnBackColorChanged();

	double m_zoomPercent;
	afx_msg void OnZoomPercentChanged();

	short m_cursorMode;
	afx_msg void OnCursorModeChanged();

	short m_mapCursor;
	afx_msg void OnMapCursorChanged();

	long m_uDCursorHandle;
	afx_msg void OnUDCursorHandleChanged();

	BOOL m_sendMouseDown;
	afx_msg void OnSendMouseDownChanged();

	BOOL m_sendMouseUp;
	afx_msg void OnSendMouseUpChanged();

	BOOL m_sendMouseMove;
	afx_msg void OnSendMouseMoveChanged();

	BOOL m_sendSelectBoxDrag;
	afx_msg void OnSendSelectBoxDragChanged();

	BOOL m_sendSelectBoxFinal;
	afx_msg void OnSendSelectBoxFinalChanged();

	double m_extentPad;
	afx_msg void OnExtentPadChanged();

	long m_extentHistory;
	afx_msg void OnExtentHistoryChanged();

	CString m_key;
	afx_msg void OnKeyChanged();

	BOOL m_doubleBuffer;
	afx_msg void OnDoubleBufferChanged();

	BOOL m_sendOnDrawBackBuffer;
	afx_msg void OnSendOnDrawBackBufferChanged();
	#pragma endregion


	//Read-only properties
	#pragma region Read-only properties

	afx_msg long GetNumLayers();
	afx_msg long GetLastErrorCode();	
	afx_msg short GetIsLocked();
	afx_msg LPDISPATCH GetGetObject(long LayerHandle);
	afx_msg BSTR GetErrorMsg(long ErrorCode);
	afx_msg long GetLayerPosition(long LayerHandle);
	afx_msg long GetLayerHandle(long LayerPosition);
	afx_msg LPDISPATCH GetColorScheme(long LayerHandle);
	afx_msg VARIANT_BOOL CMapView::get_Fileame(LONG LayerHandle, BSTR* layerName );

	#pragma endregion


	// Regular properties
	#pragma region Regular properties
	
	afx_msg LONG GetVersionNumber(void);
	afx_msg void SetVersionNumber(LONG newVal);

	afx_msg BSTR GetSerialNumber(void);
	afx_msg void SetSerialNumber(LPCTSTR newVal);

	afx_msg void SetMapResizeBehavior(short nNewValue);
	afx_msg short GetMapResizeBehavior();
	
	afx_msg LPDISPATCH GetGlobalCallback();
	afx_msg void SetGlobalCallback(LPDISPATCH newValue);
	
	afx_msg LPDISPATCH GetExtents();
	afx_msg void SetExtents(LPDISPATCH newValue);

	afx_msg BSTR GetLayerKey(long LayerHandle);
	afx_msg void SetLayerKey(long LayerHandle, LPCTSTR lpszNewValue);

	afx_msg BOOL GetLayerVisible(long LayerHandle);
	afx_msg void SetLayerVisible(long LayerHandle, BOOL bNewValue);

	afx_msg void SetTrapRMouseDown(BOOL nNewValue);
	afx_msg BOOL GetTrapRMouseDown();

	afx_msg void SetDisableWaitCursor(BOOL nNewValue);
	afx_msg BOOL GetDisableWaitCursor();

	afx_msg void SetUseSeamlessPan(BOOL newVal);
	afx_msg BOOL GetUseSeamlessPan(void);

	afx_msg void SetMouseWheelSpeed(DOUBLE newVal);
	afx_msg DOUBLE GetMouseWheelSpeed(void);

	afx_msg void SetShapeDrawingMethod(short newVal);
	afx_msg short GetShapeDrawingMethod(void);

	afx_msg void SetCanUseImageGrouping(VARIANT_BOOL newVal);
	afx_msg VARIANT_BOOL GetCanUseImageGrouping();

	afx_msg ILabels* GetLayerLabels(LONG LayerHandle);
	afx_msg void SetLayerLabels(LONG LayerHandle, ILabels* pVal);

	afx_msg IShapefile* GetShapefile(LONG LayerHandle);
	afx_msg void SetShapefile(LONG LayerHandle, IShapefile* pVal);

	afx_msg IImage* GetImage(LONG LayerHandle);
	afx_msg void SetImage(LONG LayerHandle, IImage* pVal);

	afx_msg BSTR GetLayerName(LONG LayerHandle);
	afx_msg void SetLayerName(LONG LayerHandle, LPCTSTR newVal);

	afx_msg IGeoProjection* GetGeoProjection(void);
	afx_msg void SetGeoProjection(IGeoProjection* pVal);

	afx_msg DOUBLE GetLayerMaxVisibleScale(LONG LayerHandle);
	afx_msg void SetLayerMaxVisibleScale(LONG LayerHandle, DOUBLE newVal);

	afx_msg DOUBLE GetLayerMinVisibleScale(LONG LayerHandle);
	afx_msg void SetLayerMinVisibleScale(LONG LayerHandle, DOUBLE newVal);

	afx_msg int GetLayerMaxVisibleZoom(LONG LayerHandle);
	afx_msg void SetLayerMaxVisibleZoom(LONG LayerHandle, int newVal);

	afx_msg int GetLayerMinVisibleZoom(LONG LayerHandle);
	afx_msg void SetLayerMinVisibleZoom(LONG LayerHandle, int newVal);

	afx_msg VARIANT_BOOL GetLayerDynamicVisibility(LONG LayerHandle);
	afx_msg void SetLayerDynamicVisibility(LONG LayerHandle, VARIANT_BOOL newVal);

	afx_msg VARIANT_BOOL SaveMapState(LPCTSTR Filename, VARIANT_BOOL RelativePaths, VARIANT_BOOL Overwrite);
	afx_msg VARIANT_BOOL LoadMapState(LPCTSTR Filename, IDispatch* Callback);

	afx_msg VARIANT_BOOL SaveLayerOptions(LONG LayerHandle, LPCTSTR OptionsName, VARIANT_BOOL Overwrite, LPCTSTR Description);
	afx_msg VARIANT_BOOL LoadLayerOptions(LONG LayerHandle, LPCTSTR OptionsName, BSTR* Description);

public:
	afx_msg IExtents* CMapView::GetGeographicExtents();
	afx_msg VARIANT_BOOL SetGeographicExtents(IExtents* extents);

	afx_msg ITiles* GetTiles(void);
	afx_msg void SetTiles(ITiles* pVal);
protected:
	afx_msg BSTR GetLayerDescription(LONG LayerHandle);
	afx_msg void SetLayerDescription(LONG LayerHandle, LPCTSTR newVal);

	afx_msg BSTR GetGridFileName(LONG LayerHandle);
	afx_msg void SetGridFileName(LONG LayerHandle, LPCTSTR newVal);

	afx_msg VARIANT_BOOL GetScalebarVisible(void);
	afx_msg void SetScalebarVisible(VARIANT_BOOL pVal);

	afx_msg VARIANT_BOOL GetShowRedrawTime(void);
	afx_msg void SetShowRedrawTime(VARIANT_BOOL newVal);

	afx_msg VARIANT_BOOL GetShowVersionNumber(void);
	afx_msg void SetShowVersionNumber(VARIANT_BOOL newVal);

	afx_msg ILabels* GetDrawingLabels(long DrawingLayerIndex);
	afx_msg void SetDrawingLabels(long DrawingLayerIndex, ILabels* newValue); //LPDISPATCH newValue);

	afx_msg VARIANT_BOOL GetLayerSkipOnSaving(LONG LayerHandle);
	afx_msg void SetLayerSkipOnSaving(LONG LayerHandle, VARIANT_BOOL newVal);

	#pragma endregion
	

	// Obsolete properties
	#pragma region Obsolete properties
	afx_msg long get_UDPointImageListCount(long LayerHandle);
	afx_msg IDispatch* get_UDPointImageListItem(long LayerHandle, long ImageIndex);
	
	afx_msg BSTR GetDrawingKey(long DrawHandle);
	afx_msg void SetDrawingKey(long DrawHandle, LPCTSTR lpszNewValue);
	
	afx_msg BSTR GetMapState();
	afx_msg void SetMapState(LPCTSTR lpszNewValue);

	afx_msg OLE_COLOR GetShapeLayerFillColor(long LayerHandle);
	afx_msg void SetShapeLayerFillColor(long LayerHandle, OLE_COLOR nNewValue);

	afx_msg OLE_COLOR GetShapeFillColor(long LayerHandle, long Shape);
	afx_msg void SetShapeFillColor(long LayerHandle, long Shape, OLE_COLOR nNewValue);

	afx_msg OLE_COLOR GetShapeLayerLineColor(long LayerHandle);
	afx_msg void SetShapeLayerLineColor(long LayerHandle, OLE_COLOR nNewValue);

	afx_msg OLE_COLOR GetShapeLineColor(long LayerHandle, long Shape);
	afx_msg void SetShapeLineColor(long LayerHandle, long Shape, OLE_COLOR nNewValue);

	afx_msg OLE_COLOR GetShapeLayerPointColor(long LayerHandle);
	afx_msg void SetShapeLayerPointColor(long LayerHandle, OLE_COLOR nNewValue);

	afx_msg OLE_COLOR GetShapePointColor(long LayerHandle, long Shape);
	afx_msg void SetShapePointColor(long LayerHandle, long Shape, OLE_COLOR nNewValue);

	afx_msg BOOL GetShapeLayerDrawFill(long LayerHandle);
	afx_msg void SetShapeLayerDrawFill(long LayerHandle, BOOL bNewValue);

	afx_msg BOOL GetShapeDrawFill(long LayerHandle, long Shape);
	afx_msg void SetShapeDrawFill(long LayerHandle, long Shape, BOOL bNewValue);

	afx_msg BOOL GetShapeLayerDrawLine(long LayerHandle);
	afx_msg void SetShapeLayerDrawLine(long LayerHandle, BOOL bNewValue);

	afx_msg BOOL GetShapeDrawLine(long LayerHandle, long Shape);
	afx_msg void SetShapeDrawLine(long LayerHandle, long Shape, BOOL bNewValue);

	afx_msg BOOL GetShapeLayerDrawPoint(long LayerHandle);
	afx_msg void SetShapeLayerDrawPoint(long LayerHandle, BOOL bNewValue);

	afx_msg BOOL GetShapeDrawPoint(long LayerHandle, long Shape);
	afx_msg void SetShapeDrawPoint(long LayerHandle, long Shape, BOOL bNewValue);

	afx_msg float GetShapeLayerLineWidth(long LayerHandle);
	afx_msg void SetShapeLayerLineWidth(long LayerHandle, float newValue);

	afx_msg float GetShapeLineWidth(long LayerHandle, long Shape);
	afx_msg void SetShapeLineWidth(long LayerHandle, long Shape, float newValue);

	afx_msg float GetShapeLayerPointSize(long LayerHandle);
	afx_msg void SetShapeLayerPointSize(long LayerHandle, float newValue);

	afx_msg float GetShapePointSize(long LayerHandle, long Shape);
	afx_msg void SetShapePointSize(long LayerHandle, long Shape, float newValue);

	afx_msg float GetShapeLayerFillTransparency(long LayerHandle);
	afx_msg void SetShapeLayerFillTransparency(long LayerHandle, float newValue);

	afx_msg float GetShapeFillTransparency(long LayerHandle, long Shape);
	afx_msg void SetShapeFillTransparency(long LayerHandle, long Shape, float newValue);

	afx_msg short GetShapeLayerLineStipple(long LayerHandle);
	afx_msg void SetShapeLayerLineStipple(long LayerHandle, short nNewValue);

	afx_msg short GetShapeLineStipple(long LayerHandle, long Shape);
	afx_msg void SetShapeLineStipple(long LayerHandle, long Shape, short nNewValue);

	afx_msg short GetShapeLayerFillStipple(long LayerHandle);
	afx_msg void SetShapeLayerFillStipple(long LayerHandle, short nNewValue);

	afx_msg short GetShapeFillStipple(long LayerHandle, long Shape);
	afx_msg void SetShapeFillStipple(long LayerHandle, long Shape, short nNewValue);

	afx_msg BOOL GetShapeVisible(long LayerHandle, long Shape);
	afx_msg void SetShapeVisible(long LayerHandle, long Shape, BOOL bNewValue);

	afx_msg float GetImageLayerPercentTransparent(long LayerHandle);
	afx_msg void SetImageLayerPercentTransparent(long LayerHandle, float newValue);
	
	afx_msg short GetShapeLayerPointType(long LayerHandle);
	afx_msg void SetShapeLayerPointType(long LayerHandle, short nNewValue);

	afx_msg short GetShapePointType(long LayerHandle, long Shape);
	afx_msg void SetShapePointType(long LayerHandle, long Shape, short nNewValue);

	afx_msg BOOL GetLayerLabelsVisible(long LayerHandle);
	afx_msg void SetLayerLabelsVisible(long LayerHandle, BOOL bNewValue);

	afx_msg long GetUDLineStipple(long LayerHandle);
	afx_msg void SetUDLineStipple(long LayerHandle, long nNewValue);

	afx_msg long GetUDFillStipple(long LayerHandle, long StippleRow);
	afx_msg void SetUDFillStipple(long LayerHandle, long StippleRow, long nNewValue);

	afx_msg LPDISPATCH GetUDPointType(long LayerHandle);
	afx_msg void SetUDPointType(long LayerHandle, LPDISPATCH newValue);

	afx_msg	long GetLineSeparationFactor();
	afx_msg void SetLineSeparationFactor(long sepFactor);

	afx_msg BOOL GetLayerLabelsShadow(long LayerHandle);
	afx_msg void SetLayerLabelsShadow(long LayerHandle, BOOL newValue);

	afx_msg BOOL GetLayerLabelsScale(long LayerHandle);
	afx_msg void SetLayerLabelsScale(long LayerHandle, BOOL newValue);
	
	afx_msg void GetLayerStandardViewWidth(long LayerHandle, double * Width);
	afx_msg void SetLayerStandardViewWidth(long LayerHandle, double Width);

	afx_msg long GetLayerLabelsOffset(long LayerHandle);
	afx_msg void SetLayerLabelsOffset(long LayerHandle, long Offset);

	afx_msg OLE_COLOR GetLayerLabelsShadowColor(long LayerHandle);
	afx_msg void SetLayerLabelsShadowColor(long LayerHandle, OLE_COLOR color);

	afx_msg BOOL GetUseLabelCollision(long LayerHandle);
	afx_msg void SetUseLabelCollision(long LayerHandle, BOOL value);

	afx_msg long GetShapePointImageListID(long LayerHandle, long Shape);
	afx_msg void SetShapePointImageListID(long LayerHandle, long Shape, long ImageIndex);

	afx_msg long GetShapePointFontCharListID(long LayerHandle, long Shape);
	afx_msg void SetShapePointFontCharListID(long LayerHandle, long Shape, long FontCharIndex);

	afx_msg	bool GetMultilineLabeling();
	afx_msg void SetMultilineLabeling(bool Value);

	afx_msg BOOL GetDrawingLabelsShadow(long drawHandle);
	afx_msg void SetDrawingLabelsShadow(long drawHandle, BOOL newValue);

	afx_msg BOOL GetDrawingLabelsScale(long drawHandle);
	afx_msg void SetDrawingLabelsScale(long drawHandle, BOOL newValue);

	afx_msg long GetDrawingLabelsOffset(long drawHandle);
	afx_msg void SetDrawingLabelsOffset(long drawHandle, long Offset);

	afx_msg OLE_COLOR GetDrawingLabelsShadowColor(long drawHandle);
	afx_msg void SetDrawingLabelsShadowColor(long drawHandle, OLE_COLOR color);

	afx_msg BOOL GetUseDrawingLabelCollision(long drawHandle);
	afx_msg void SetUseDrawingLabelCollision(long drawHandle, BOOL bNewValue);

	afx_msg BOOL GetDrawingLabelsVisible(long drawHandle);
	afx_msg void SetDrawingLabelsVisible(long drawHandle, BOOL bNewValue);

	afx_msg void GetDrawingStandardViewWidth(long drawHandle, double * Width);
	afx_msg void SetDrawingStandardViewWidth(long drawHandle, double Width);

	afx_msg void SetShapeLayerStippleColor(long LayerHandle, OLE_COLOR nNewValue);
	afx_msg void SetShapeStippleColor(long LayerHandle, long Shape, OLE_COLOR nNewValue);

	afx_msg OLE_COLOR GetShapeLayerStippleColor(long LayerHandle);
	afx_msg OLE_COLOR GetShapeStippleColor(long LayerHandle, long Shape);

	afx_msg void SetShapeLayerStippleTransparent(long LayerHandle, BOOL nNewValue);
	afx_msg void SetShapeStippleTransparent(long LayerHandle, long Shape, BOOL nNewValue);

	afx_msg BOOL GetShapeLayerStippleTransparent(long LayerHandle);
	afx_msg BOOL GetShapeStippleTransparent(long LayerHandle, long Shape);
	#pragma endregion


	// Regular methods
	#pragma region Methods
	afx_msg void Redraw();
	afx_msg long AddLayer(LPDISPATCH Object, BOOL Visible);
	afx_msg void RemoveLayer(long LayerHandle);
	afx_msg void RemoveLayerWithoutClosing(long LayerHandle);
	afx_msg void RemoveAllLayers();
	afx_msg BOOL MoveLayerUp(long InitialPosition);
	afx_msg BOOL MoveLayerDown(long InitialPosition);
	afx_msg BOOL MoveLayer(long InitialPosition, long TargetPosition);
	afx_msg BOOL MoveLayerTop(long InitialPosition);
	afx_msg BOOL MoveLayerBottom(long InitialPosition);
	afx_msg void ZoomToMaxExtents();
	afx_msg void ZoomToMaxVisibleExtents();
	afx_msg void ZoomToLayer(long LayerHandle);
	afx_msg void ZoomToShape(long LayerHandle, long Shape);
	afx_msg void ZoomIn(double Percent);
	afx_msg void ZoomOut(double Percent);
	afx_msg long ZoomToPrev();
	afx_msg void ProjToPixel(double projX, double projY, double FAR* pixelX, double FAR* pixelY);
	afx_msg void PixelToProj(double pixelX, double pixelY, double FAR* projX, double FAR* projY);
	afx_msg void ClearDrawing(long DrawHandle);
	afx_msg void ClearDrawings();
	afx_msg LPDISPATCH SnapShot(LPDISPATCH BoundBox);
	afx_msg BOOL ApplyLegendColors(LPDISPATCH Legend);
	afx_msg void LockWindow(short LockMode);
	afx_msg void Resize(long Width, long Height);
	afx_msg void ShowToolTip(LPCTSTR Text, long Milliseconds);
	#pragma endregion


	// Obsolete methods
	#pragma region Obsolete methods
	afx_msg void AddLabel(long LayerHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification);
	afx_msg void ClearLabels(long LayerHandle);
	afx_msg void LayerFont(long LayerHandle, LPCTSTR FontName, long FontSize);
	afx_msg void LayerFontEx(long LayerHandle, LPCTSTR FontName, long FontSize, BOOL isBold, BOOL isItalic, BOOL isUnderline);//ajp (30/11/07)
	afx_msg void AddLabelEx(long LayerHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification, double Rotation);
	afx_msg long set_UDPointImageListAdd(long LayerHandle, LPDISPATCH newValue);
	afx_msg long set_UDPointFontCharListAdd(long LayerHandle, long newValue, OLE_COLOR color);
    afx_msg void set_UDPointFontCharFont(long LayerHandle, LPCTSTR FontName, float FontSize, BOOL isBold, BOOL isItalic, BOOL isUnderline);
	afx_msg void set_UDPointFontCharFontSize(long LayerHandle, float FontSize);
	afx_msg void ClearUDPointImageList(long LayerHandle);
	afx_msg void AddDrawingLabel(long drawHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification);
	afx_msg void AddDrawingLabelEx(long drawHandle, LPCTSTR Text, OLE_COLOR Color, double x, double y, short hJustification, double Rotation);
	afx_msg void DrawingFont(long drawHandle, LPCTSTR FontName, long FontSize);
	afx_msg void ClearDrawingLabels(long drawHandle);
	#pragma endregion
		
	afx_msg long NewDrawing(short Projection);
	afx_msg void DrawPoint(double x, double y, long size, OLE_COLOR color);
	afx_msg void DrawLine(double x1, double y1, double x2, double y2, long width, OLE_COLOR color);
	afx_msg void DrawCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill);
	afx_msg void DrawPolygon(VARIANT *xPoints, VARIANT *yPoints, long numPoints, OLE_COLOR color, BOOL fill);
	afx_msg void DrawWideCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill, int width);
	afx_msg void DrawWidePolygon(VARIANT *xPoints, VARIANT *yPoints, long numPoints, OLE_COLOR color, BOOL fill, short Width);

	afx_msg BOOL IsTIFFGrid(LPCTSTR Filename);
	afx_msg BOOL IsSameProjection(LPCTSTR proj4_a, LPCTSTR proj4_b);
	afx_msg long HWnd();
	
	afx_msg void ReSourceLayer(long LayerHandle, LPCTSTR newSrcPath);
	afx_msg BOOL AdjustLayerExtents(long LayerHandle);	// Rob Cairns 29-Jun-09
	
	afx_msg void SetCurrentScale(DOUBLE newVal);
	afx_msg DOUBLE GetCurrentScale(void);
	//afx_msg LPDISPATCH GetDrawingLabels(long DrawingLayerIndex);
	
	afx_msg void SetMapUnits(tkUnitsOfMeasure units);
	afx_msg tkUnitsOfMeasure GetMapUnits(void);
	
	afx_msg BOOL SnapShotToDC2(PVOID hdc, IExtents* Extents, LONG Width, float OffsetX, float OffsetY,
							 float ClipX, float ClipY, float clipWidth, float clipHeight);
	afx_msg BOOL SnapShotToDC(PVOID hdc, IExtents* Extents, LONG Width);
	afx_msg void LoadTiles(IExtents* Extents, LONG WidthPixels, LPCTSTR Key, tkTileProvider provider);

	afx_msg INT CMapView::TilesAreInCache(IExtents* Extents, LONG WidthPixels, tkTileProvider provider);

	// Added ajp June 2010  
	afx_msg void SetMapRotationAngle(float nNewValue);
	afx_msg float GetMapRotationAngle(void);
	afx_msg IExtents* GetRotatedExtent(void);
	afx_msg IPoint* GetBaseProjectionPoint(double rotPixX, double rotPixY);
	
	afx_msg void DrawLineEx(LONG LayerHandle, DOUBLE x1, DOUBLE y1, DOUBLE x2, DOUBLE y2, LONG pixelWidth, OLE_COLOR color);
	afx_msg void DrawPointEx(LONG LayerHandle, DOUBLE x, DOUBLE y, LONG pixelSize, OLE_COLOR color);
	afx_msg void DrawCircleEx(LONG LayerHandle, DOUBLE x, DOUBLE y, DOUBLE pixelRadius, OLE_COLOR color, VARIANT_BOOL fill);
	afx_msg void DrawPolygonEx(LONG LayerHandle, VARIANT* xPoints, VARIANT* yPoints, LONG numPoints, OLE_COLOR color, VARIANT_BOOL fill);
	afx_msg void DrawWideCircleEx(LONG LayerHandle, double x, double y, double radius, OLE_COLOR color, VARIANT_BOOL fill, short OutlineWidth);
	afx_msg void DrawWidePolygonEx(LONG LayerHandle, VARIANT *xPoints, VARIANT *yPoints, long numPoints, OLE_COLOR color, VARIANT_BOOL fill, short OutlineWidth);

	// saves layer options, on loading doesn't add new layer
	afx_msg BSTR SerializeLayerOptions(LONG LayerHandle);
	afx_msg VARIANT_BOOL DeserializeLayerOptions(LONG LayerHandle, LPCTSTR newVal);


	afx_msg IDispatch* SnapShot2(LONG ClippingLayerNbr, DOUBLE Zoom, long pWidth);
	afx_msg IDispatch* SnapShot3(double left, double right, double top, double bottom, long Width);

	afx_msg VARIANT_BOOL SetImageLayerColorScheme(LONG LayerHandle, IDispatch* ColorScheme);
	afx_msg void UpdateImage(LONG LayerHandle);

	afx_msg VARIANT_BOOL DeserializeMapState(LPCTSTR State, VARIANT_BOOL LoadLayers, LPCTSTR BasePath);
	afx_msg BSTR SerializeMapState(VARIANT_BOOL RelativePaths, LPCTSTR BasePath);
	afx_msg CString get_OptionsFilename(LONG LayerHandle, LPCTSTR OptionsName);
	afx_msg VARIANT_BOOL RemoveLayerOptions(LONG LayerHandle, LPCTSTR OptionsName);

	afx_msg VARIANT_BOOL ZoomToSelected(LONG LayerHandle);

	afx_msg VARIANT_BOOL ZoomToTileLevel(int zoom);

	afx_msg IMeasuring* GetMeasuring(void);

	afx_msg VARIANT_BOOL ZoomToWorld(void);

	afx_msg VARIANT_BOOL FindSnapPoint(double tolerance, double xScreen, double yScreen, double* xFound, double* yFound);

	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
#pragma endregion

public:
#pragma region EventMap
	// Event maps
	//{{AFX_EVENT(CMapView)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	void FireMouseDown(short Button, short Shift, long x, long y)
		{FireEvent(eventidMouseDown,EVENT_PARAM(VTS_I2  VTS_I2  VTS_I4  VTS_I4), Button, Shift, x, y);}
	void FireMouseUp(short Button, short Shift, long x, long y)
		{FireEvent(eventidMouseUp,EVENT_PARAM(VTS_I2  VTS_I2  VTS_I4  VTS_I4), Button, Shift, x, y);}
	void FireMouseMove(short Button, short Shift, long x, long y)
		{FireEvent(eventidMouseMove,EVENT_PARAM(VTS_I2  VTS_I2  VTS_I4  VTS_I4), Button, Shift, x, y);}
	void FireFileDropped(LPCTSTR Filename)
		{FireEvent(eventidFileDropped,EVENT_PARAM(VTS_BSTR), Filename);}
	void FireSelectBoxFinal(long Left, long Right, long Bottom, long Top)
		{FireEvent(eventidSelectBoxFinal,EVENT_PARAM(VTS_I4  VTS_I4  VTS_I4  VTS_I4), Left, Right, Bottom, Top);}
	void FireSelectBoxDrag(long Left, long Right, long Bottom, long Top)
		{FireEvent(eventidSelectBoxDrag,EVENT_PARAM(VTS_I4  VTS_I4  VTS_I4  VTS_I4), Left, Right, Bottom, Top);}
	void FireExtentsChanged()
		{FireEvent(eventidExtentsChanged,EVENT_PARAM(VTS_NONE));}
	void FireMapState(long LayerHandle)
		{FireEvent(eventidMapState,EVENT_PARAM(VTS_I4), LayerHandle);}
	void FireOnDrawBackBuffer(long BackBuffer)
		{FireEvent(eventidOnDrawBackBuffer,EVENT_PARAM(VTS_I4), BackBuffer);}
	void FireShapeHighlighted(long LayerHandle, long ShapeIndex)
		{FireEvent(eventidShapeHighlighted,EVENT_PARAM(VTS_I4 VTS_I4), LayerHandle, ShapeIndex);}
	void FireBeforeDrawing(long hdc, long xMin, long xMax, long yMin, long yMax, VARIANT_BOOL* Handled)
		{FireEvent(eventidBeforeDrawing,EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PBOOL), hdc, xMin, xMax, yMin, yMax, Handled);}
	void FireAfterDrawing(long hdc, long xMin, long xMax, long yMin, long yMax, VARIANT_BOOL* Handled)
		{FireEvent(eventidAfterDrawing,EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PBOOL), hdc, xMin, xMax, yMin, yMax, Handled);}
	void FireTilesLoaded(IDispatch* tiles, IDispatch* extents, VARIANT_BOOL snapshot, LPCTSTR key)
		{FireEvent(eventidTilesLoaded,EVENT_PARAM(VTS_DISPATCH VTS_DISPATCH VTS_BOOL VTS_BSTR ), tiles, extents, snapshot,key);}
	void FireMeasuringChanged(IDispatch* measuring, tkMeasuringAction action)
	{
		FireEvent(eventidMeasuringChanged,EVENT_PARAM(VTS_DISPATCH VTS_I4), measuring, action);
	}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()
#pragma endregion

#pragma region DispatchAndEventIds
public:
enum {		//{{AFX_DISP_ID(CMapView)
	dispidTilesAreInCache = 204L,
	dispidFindSnapPoint = 203L,
	dispidSnapShotToDC2 = 202L,
	dispidLoadTiles = 201L,
	dispidZoomToWorld = 200L,
	dispidLayerMinVisibleZoom = 199L,
	dispidLayerMaxVisibleZoom = 198L,
	dispidZoomToTileLevel = 197L,
	dispidMeasuring = 196L,
	dispidScalebarVisible = 195L,
	dispidSetGeographicExtents = 194L,
	dispidGeographicExtents = 193L,
	dispidProjection = 192L,
	dispidTiles = 191L,
	dispidZoomToSelected = 190L,
	dispidLayerFilename = 189,
	dispidPixelsPerDegree = 188,
	dispidMaxExtents = 187,
	dispidLayerSkipOnSaving = 186,
	dispidRemoveLayerOptions = 185L,
	dispidSerializeMapState = 184L,
	dispidDeserializeMapState = 183L,
	dispidLayerDescription = 182,
	dispidLoadLayerOptions = 181L,
	dispidSaveLayerOptions = 180L,
	dispidLoadMapState = 179L,
	dispidSaveMapState = 178L,
	dispidDeserializeLayerOptions = 177L,
	dispidSerializeLayerOptions = 176L,
	dispidImage = 175,
	dispidShapefile = 174,
	dispidShowVersionNumber = 173,
	dispidShowRedrawTime = 172,
	dispidLayerLabels = 171,
	dispidLayerDynamicVisibility = 169,
	dispidLayerMinVisibleScale = 168,
	dispidLayerMaxVisibleScale = 167,
	dispidVersionNumber = 166,
	dispidDrawWideCircleEx = 160L,
	dispidDrawWidePolygonEx = 161L,

	// NOTE: ClassWizard will add and remove enumeration elements here
	//    DO NOT EDIT what you see in these blocks of generated code !
	// **ClassWizard is a thing of the past... feel free to edit this code.
	dispidBackColor = 1L,
	dispidZoomPercent = 2L,
	dispidCursorMode = 3L,
	dispidMapCursor = 4L,
	dispidUDCursorHandle = 5L,
	dispidSendMouseDown = 6L,
	dispidSendMouseUp = 7L,
	dispidSendMouseMove = 8L,
	dispidSendSelectBoxDrag = 9L,
	dispidSendSelectBoxFinal = 10L,
	dispidExtentPad = 11L,
	dispidExtentHistory = 12L,
	dispidKey = 13L,
	dispidDoubleBuffer = 14L,
	dispidGlobalCallback = 15L,
	dispidNumLayers = 16L,
	dispidExtents = 17L,
	dispidLastErrorCode = 18L,
	dispidIsLocked = 19L,
	dispidMapState = 20L,
	dispidInitializeMap = 21L,
	dispidUninitializeMap = 22L,
	dispidRedraw = 23L,
	dispidAddLayer = 24L,
	dispidRemoveLayer = 25L,
	dispidRemoveLayerWithoutClosing = 138L,
	dispidRemoveAllLayers = 26L,
	dispidMoveLayerUp = 27L,
	dispidMoveLayerDown = 28L,
	dispidMoveLayer = 29L,
	dispidMoveLayerTop = 30L,
	dispidMoveLayerBottom = 31L,
	dispidZoomToMaxExtents = 32L,
	dispidZoomToLayer = 33L,
	dispidZoomToShape = 34L,
	dispidZoomIn = 35L,
	dispidZoomOut = 36L,
	dispidZoomToPrev = 37L,
	dispidProjToPixel = 38L,
	dispidPixelToProj = 39L,
	dispidClearDrawing = 40L,
	dispidClearDrawings = 41L,
	dispidSnapShot = 42L,
	dispidApplyLegendColors = 43L,
	dispidLockWindow = 44L,
	dispidResize = 45L,
	dispidShowToolTip = 46L,
	dispidAddLabel = 47L,
	dispidClearLabels = 48L,
	dispidLayerFont = 49L,
	dispidGetColorScheme = 50L,
	dispidNewDrawing = 51L,
	dispidPoint = 52L,
	dispidLine = 53L,
	dispidCircle = 54L,
	dispidPolygon = 55L,
	dispidLayerKey = 56L,
	dispidLayerPosition = 57L,
	dispidLayerHandle = 58L,
	dispidLayerVisible = 59L,
	dispidShapeLayerFillColor = 60L,
	dispidShapeFillColor = 61L,
	dispidShapeLayerLineColor = 62L,
	dispidShapeLineColor = 63L,
	dispidShapeLayerPointColor = 64L,
	dispidShapePointColor = 65L,
	dispidShapeLayerDrawFill = 66L,
	dispidShapeDrawFill = 67L,
	dispidShapeLayerDrawLine = 68L,
	dispidShapeDrawLine = 69L,
	dispidShapeLayerDrawPoint = 70L,
	dispidShapeDrawPoint = 71L,
	dispidShapeLayerLineWidth = 72L,
	dispidShapeLineWidth = 73L,
	dispidShapeLayerPointSize = 74L,
	dispidShapePointSize = 75L,
	dispidShapeLayerFillTransparency = 76L,
	dispidShapeFillTransparency = 77L,
	dispidShapeLayerLineStipple = 78L,
	dispidShapeLineStipple = 79L,
	dispidShapeLayerFillStipple = 80L,
	dispidShapeFillStipple = 81L,
	dispidShapeVisible = 82L,
	dispidImageLayerPercentTransparent = 83L,
	dispidErrorMsg = 84L,
	dispidDrawingKey = 85L,
	dispidShapeLayerPointType = 86L,
	dispidShapePointType = 87L,
	dispidLayerLabelsVisible = 88L,
	dispidUDLineStipple = 89L,
	dispidUDFillStipple = 90L,
	dispidUDPointType = 91L,
	dispidGetObject = 92L,
	dispidLayerName = 91,	
	dispidSetImageLayerColorScheme = 92L,		
	dispidGridFileName = 93,	
	dispidUpdateImage = 94L,
	dispidSerialNumber = 95,
	dispidLineSeparationFactor = 96,
	dispidLayerLabelsShadow = 97L,
	dispidLayerLabelsScale = 98L,
	dispidAddLabelEx = 99L,
	dispidGetLayerStandardViewWidth = 100L,
	dispidSetLayerStandardViewWidth = 101L,
	dispidLayerLabelsOffset = 102L,
	dispidLayerLabelsShadowColor = 103L,
	dispidUseLabelCollision = 104L,
	dispidIsTIFFGrid = 105L,
	dispidIsSameProjection = 106L,
	dispidZoomToMaxVisibleExtents = 107L,
	dispidMapResizeBehavior = 108L,
	dispidDrawLineEx = 115L,		
	dispidDrawPointEx = 116L,		
	dispidDrawCircleEx = 117L,		
	dispidLabelColor = 119L,		
	dispidSetDrawingLayerVisible = 120L,		
	dispidClearDrawingLabels = 121L,
	dispidDrawingFont = 122L,
	dispidAddDrawingLabelEx = 123L,
	dispidAddDrawingLabel =124L,
	dispidDrawingLabelsOffset = 125L,
	dispidDrawingLabelsScale = 126L,
	dispidDrawingLabelsShadow = 127L,
	dispidDrawingLabelsShadowColor = 128L,
	dispidUseDrawingLabelCollision = 129L,
	dispidDrawingLabelsVisible = 130L,
	dispidGetDrawingStandardViewWidth = 131L,
	dispidSetDrawingStandardViewWidth = 132L,
	dispidMultilineLabels = 133,
	dispidSnapShot2 = 136L,
	dispidLayerFontEx = 137L, 
	dispidset_UDPointFontCharFont = 139L, 
	dispidset_UDPointFontCharFontSize = 141L,
	dispidset_UDPointFontCharListAdd = 140L, 
	dispidShapePointFontCharListID = 142L,
    dispidReSourceLayer = 143L,
	dispidShapeLayerStippleColor = 144L,
	dispidShapeStippleColor = 145L,
	dispidShapeStippleTransparent = 146L,
	dispidShapeLayerStippleTransparent = 147L,
    dispidTrapRMouseDown = 148L,
	dispidDisableWaitCursor = 149L,
	dispidAdjustLayerExtents = 150L,
	dispidUseSeamlessPan = 151L,
	dispidMouseWheelSpeed = 152L,
	dispidSnapShot3 = 153L,
	dispidShapeDrawingMethod = 154L,
	dispidDrawPolygonEx = 155L,
	dispidCurrentScale = 156L,
	dispidDrawingLabels = 157L,
	dispidMapUnits = 158L,
	dispidSnapShotToDC = 159L,
    dispidMapRotationAngle = 162L,           //ajp June 2010
    dispidRotatedExtent = 163L,				  //ajp June 2010
    dispidGetBaseProjectionPoint = 164L,     //ajp June 2010
	dispidCanUseImageGrouping = 165L,		 // lsu
	dispidDrawBackBuffer = 170L,

	// events
	eventidMouseDown = 1L,
	eventidMouseUp = 2L,
	eventidMouseMove = 3L,
	eventidFileDropped = 4L,
	eventidSelectBoxFinal = 5L,
	eventidSelectBoxDrag = 6L,
	eventidExtentsChanged = 7L,
	eventidMapState = 8L,
	eventidOnDrawBackBuffer = 9L,
	eventidShapeHighlighted = 10L,
	eventidBeforeDrawing = 11L,
	eventidAfterDrawing = 12L,
	eventidTilesLoaded = 13L,
	eventidMeasuringChanged = 14L,
	//}}AFX_DISP_ID
	};
#pragma endregion

#pragma region Drawing variables
	// only mouse moves are updated everything else is restored from buffer
	bool m_drawMouseMoves;		

	// temporary shapefile to display hot tracking
	struct HotTrackingInfo
	{
		IShapefile* Shapefile;
		int ShapeId;
		int LayerHandle;
	}m_hotTracking;

	Gdiplus::Bitmap* m_layerBitmap;
	BOOL m_canbitblt;	// the data layers can be drawn from buffer
	bool m_rectTrackerIsActive;

	// tiles buffer
	Gdiplus::Bitmap* m_tilesBitmap;
	ITiles* m_tiles;			// the list of tiles (in-memory GDI+ bitmaps)

	// a back buffer for drawing objects, the stuff like rubber band lines, etc.
	Gdiplus::Bitmap* m_drawingBitmap;

	// combined buffer, holds all the other ones (tiles, layers, drawing layers)
	// is used for dragging operation
	Gdiplus::Bitmap* m_bufferBitmap;

	// map is being dragged by user
	CPoint m_bitbltClickDown;
	CPoint m_bitbltClickMove;

	// image grouping
	BOOL _canUseImageGrouping;	// lsu 11 jul 2010

	// mutexes
	::CMutex m_drawMutex;
	int m_lockCount;
	::CMutex m_legendMutex;		//Legend Mutes
	::CMutex m_mapstateMutex;
	bool m_isSizing;			// the sizing of control takes place now, redraw is forbidden - TODO: replace by Mutex
	bool snapshot;

	//Layers
	std::deque<long> m_activeLayers;
	std::vector<Layer *> m_allLayers;
	
	//Draw Layers
	std::deque<long> m_activeDrawLists;
	std::deque<DrawList *> m_allDrawLists;
	std::deque<long> DrawingLayerInVisilbe; //stores all the invisiable layer handles
	long m_currentDrawing;					//current Drawing

	//Extents
	Extent extents;
	std::deque<Extent> m_prevExtents;

	//Window Properties
	long m_viewWidth;
	long m_viewHeight;
	double m_aspectRatio;

	//Projection Variables
	double m_pixelPerProjectionX;
	double m_pixelPerProjectionY;
	double m_inversePixelPerProjectionX;
	double m_inversePixelPerProjectionY;

	// snapshot temp variables
	long mm_viewWidth;
	long mm_viewHeight;
	double mm_pixelPerProjectionX;
	double mm_pixelPerProjectionY;
	double mm_inversePixelPerProjectionX;
	double mm_inversePixelPerProjectionY;
	double mm_aspectRatio;
	double mm_left;
	double mm_right;
	double mm_bottom;
	double mm_top;
	bool mm_newExtents;

	CCollisionList m_collisionList;		// global collision list for labels and charts

	bool m_isSnapshot;		// tws added 4/7/2007: used by SnapShot2 to coerce DrawImage() to use the desired size rather than the screen size
	int m_numImages;		// TODO: use by DrawLayers only

	double m_lastWidthMeters;		// last width of the screen in meters (to avoid recalculation of scalebar on tile redraw)
#pragma endregion
		
#pragma region Property variables
	
	IMeasuring* m_measuring;
	
	std::vector<ImageGroup*>* m_imageGroups;
	BOOL m_scalebarVisible;
	
	tkTransformationMode m_transformationMode;
	IGeoProjection* m_projection;
	IGeoProjection* m_wgsProjection;
	IGeoProjection* m_GMercProjection;

	//Map Rotation (ajp June 2010) 
	float   m_RotateAngle;
	Rotate *m_Rotate;

	//Callback
	ICallback * m_globalCallback;
	long m_lastErrorCode;

	//Cursors
	HCURSOR m_cursorPan;
	HCURSOR m_cursorZoomin;
	HCURSOR m_cursorZoomout;
	HCURSOR m_cursorSelect;
	HCURSOR m_udCursor;

	//CursorModesHelpers
	CPoint m_clickDown;
	Extent m_clickDownExtents;
	BOOL m_leftButtonDown;

    BOOL DoTrapRMouseDown;					// Added Quay for TrapRMouseDown 3.16.09
	BOOL m_DisableWaitCursor;
	BOOL m_UseSeamlessPan;					//lsu 07/03/09
	double m_MouseWheelSpeed;
	tkShapeDrawingMethod m_ShapeDrawingMethod;
	tkUnitsOfMeasure m_unitsOfMeasure;
	BOOL m_ShowRedrawTime;				   // whether to show time in the lower left corner
	BOOL m_ShowVersionNumber;			   // whether to show version nubmer in the lower right corner
	tkResizeBehavior rbMapResizeBehavior;  // How to behave on resize
	
		//Tool Tips
	CToolTipEx m_ttip;
	CButton * m_ttipCtrl;
	BOOL m_showingToolTip;

	// Serial number handling stuff
	CString m_serial;
	const char * vals;
	const int valsLen;
#pragma endregion
	
#pragma region Obsolete variables
	// OBSOLETE members - should be removed after new drawing procedures are tested
	BOOL MultilineLabeling;			// Allow multiline labels?
	long m_LineSeparationFactor;	// Thickness between double and triple lines; adjust factor.
	bool m_useLabelCollision;		//Label Collision
#pragma endregion

#pragma region Methods
public:
	//void ZoomToTileLevel(int zoom);
	IExtents* GetGeographicExtentsCore(bool clipForTiles, Extent* clipExtents = NULL);
	ITiles* GetTilesNoRef(void);
	bool SendMouseMove();
	bool SendSelectBoxDrag();
	void ErrorMessage(long ErrorCode);
	CString Crypt(CString str);
	bool VerifySerial(CString str);
	void DrawLayers(const CRect & rcBounds, Gdiplus::Graphics* graphics);
	
	DOUBLE GetPixelsPerDegree(void);
	void SetPixelsPerDegree(DOUBLE newVal);

	inline void PixelToProjection( double piX, double piY, double & prX, double & prY );
	inline void ProjectionToPixel( double prX, double prY, double & piX, double & piY );
protected:
	void CMapView::HandleNewDrawing(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, 
									float offsetX = 0.0f, float offsetY = 0.0f);
	IDispatch* CMapView::SnapShotCore(double left, double right, double top, double bottom, long Width, long Height, 
									  CDC* snapDC = NULL, float offsetX = 0.0f, float offsetY = 0.0f,
									  float clipX = 0.0f, float clipY = 0.0f, float clipWidth = 0.0f, float clipHeight = 0.0f);

	void LabelColor(LONG LayerHandle, OLE_COLOR LabelFontColor);
	void SetDrawingLayerVisible(LONG LayerHandle, VARIANT_BOOL Visiable);
	
	void DrawBackBuffer(int** hdc, int ImageWidth, int ImageHeight);
	bool IsValidDrawList(long listHandle);
	
	ShpfileType CMapView::get_ShapefileType(long layerHandle);
	void CMapView::ShowRedrawTime(Gdiplus::Graphics* g, float time, CStringW message = L"");
	void CMapView::DrawMeasuring(Gdiplus::Graphics* g );

	DOUBLE CMapView::PixelsPerMapUnit(void);
	DOUBLE CMapView::DegreesPerMapUnit(void);
	void CMapView::DrawScaleBar(Gdiplus::Graphics* g);

	CPLXMLNode* SerializeMapStateCore(VARIANT_BOOL RelativePaths, CString ProjectName);		// used by SaveMapState and GetMapState
	bool DeserializeMapStateCore(CPLXMLNode* node, CString ProjectName, VARIANT_BOOL LoadLayers, IStopExecution* callback);
	CPLXMLNode* SerializeLayerCore(LONG LayerHandle, CString Filename);
	int DeserializeLayerCore(CPLXMLNode* node, CString ProjectName, IStopExecution* callback);		// adds new layer on loading (is used by map state)
	VARIANT_BOOL DeserializeLayerOptionsCore(LONG LayerHandle, CPLXMLNode* node);
	
	IExtents* GetMaxExtents(void);
	void SetMaxExtents(IExtents* pVal);
	
private:
	//bool CMapView::FindSnapPoint(double tolerance, double xScreen, double yScreen, double& xFound, double& yFound);
	void CMapView::ResizeBuffers(int cx, int cy);
	void CMapView::DrawMouseMoves(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool isSnapShot = false, float offsetX = 0.0f, float offsetY = 0.0f);
	void CMapView::DrawMouseMovesCore(Gdiplus::Graphics* g, bool isSnapShot);
	
	UINT CMapView::StartDrawLayers(LPVOID pParam);
	
	// Gdiplus startup/shutdown handling. Please see *.cpp for more explanation.
	static void GdiplusStartup();
	static void GdiplusShutdown();
	static ULONG_PTR ms_gdiplusToken;
	static unsigned ms_gdiplusCount;
	static ::CCriticalSection ms_gdiplusLock;
	
	// drawing procedures to be replaced	
	void DrawLists(const CRect & rcBounds, Gdiplus::Graphics* graphics, tkDrawReferenceList listType);
	void DrawDrawing(Gdiplus::Graphics* graphics, DrawList * dlist);//, CLabelDrawer* lblDrawer);

	HCURSOR CMapView::SetWaitCursor();
	tkInterpolationMode CMapView::ChooseInterpolationMode(tkInterpolationMode mode1, tkInterpolationMode mode2);

	bool IsValidShape( long layerHandle, long shape );
	bool IsValidLayer( long layerHandle );
	Layer* get_ShapefileLayer(long layerHandle);
	CDrawingOptionsEx* get_ShapefileDrawingOptions(long LayerHandle);

	void CalculateVisibleExtents( Extent e, bool LogPrev = true, bool MapSizeChanged = false);
	void AlignShapeLayerAndShapes(Layer * layer);
	void ClearLabelFrames();
	void ReloadImageBuffers();
	
	
	double UnitsPerPixel();

	inline double makeVal( const char * sVal );
	void LogPrevExtent();
	
	bool IS_VALID_LAYER(int layerHandle, vector<Layer*> allLayers)
	{
		return (layerHandle >= 0 && layerHandle < (int)allLayers.size()?(allLayers[layerHandle]!=NULL?TRUE:FALSE):FALSE);
	}
	// image grouping
	void BuildImageGroups(std::vector<ImageGroup*>& imageGroups);
	void DrawImageGroups(const CRect& rcBounds, Gdiplus::Graphics* graphics, int groupIndex);
	bool ImageGroupsAreEqual(std::vector<ImageGroup*>& groups1, std::vector<ImageGroup*>& groups2);

	BSTR GetLayerFilename(LONG layerHandle);
	void SetExtentsCore(Extent ext, bool logExtents = false, bool mapSizeChanged = false);

	void CMapView::SetTempExtents(double left, double right, double top, double bottom, long Width, long Height);
	void CMapView::RestoreExtents();

	void CMapView::DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, double length, 
					 double totalLength, int segmentIndex, CMeasuring* measure);

	IPoint* CMapView::GetMeasuringPolyCenter(Gdiplus::PointF* data, int length);
	void CMapView::DrawMeasuringPolyArea(Gdiplus::Graphics* g, bool lastPoint, double lastGeogX, double lastGeogY, IPoint* pnt);


	#ifdef _DEBUG //Code added by Lailin Chen to profile the time consumption of this function. --- Lailin Chen 11/7/2005
		 DWORD OnDrawTicks;
		 DWORD DrawShapefileTicks;
	#endif
#pragma endregion
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAP_H__FAF5AFC5_D242_402A_B6E3_E6FF5E2E514C__INCLUDED)
