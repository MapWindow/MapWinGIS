//********************************************************************************************************
//File name: Map.h
//Description: Declaration of the CMapView ActiveX Control class.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by 
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as 
//public domain in March 2004.  
//********************************************************************************************************
#pragma once
# include <list>
# include <afxmt.h>
# include "DispIds.h"
# include "macros.h"
# include "ToolTipEx.h"
# include "Layer.h"
# include "DrawList.h"
# include "DrawingOptions.h"
# include "MapRotate.h"
# include "ImageGroup.h"
# include "CollisionList.h"
# include "MeasuringBase.h"
# include "EditorBase.h"
# include "ShapeEditor.h"
#include "HotTrackingInfo.h"
#include "DraggingState.h"

# define SHOWTEXT 450
# define HIDETEXT 451

class CMapView : public COleControl, IMapViewCallback
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
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
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

public:	
	// a function is called on change of value	
	#pragma region Notify properties  
	OLE_COLOR m_backColor;
	afx_msg void OnBackColorChanged();

	double m_zoomPercent;
	afx_msg void OnZoomPercentChanged();

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
	afx_msg VARIANT_BOOL get_Fileame(LONG LayerHandle, BSTR* layerName );

	#pragma endregion

	// Regular properties
	#pragma region Regular properties

	afx_msg tkCursorMode GetCursorMode();
	afx_msg void SetCursorMode(tkCursorMode mode);

	afx_msg BSTR GetVersionNumber(void);
	//afx_msg void SetVersionNumber(LONG newVal);

	afx_msg BSTR GetSerialNumber(void);
	afx_msg void SetSerialNumber(LPCTSTR newVal);

	afx_msg void SetMapResizeBehavior(short nNewValue);
	afx_msg short GetMapResizeBehavior();

	afx_msg void SetZoomBehavior(short nNewValue);
	afx_msg short GetZoomBehavior();
	
	afx_msg ICallback* GetGlobalCallback();
	afx_msg void SetGlobalCallback(ICallback* newValue);
	
	afx_msg IExtents* GetExtents();
	afx_msg void SetExtents(IExtents* newValue);

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

	afx_msg IOgrLayer* GetOgrLayer(LONG LayerHandle);
	afx_msg void SetOgrLayer(LONG LayerHandle, IShapefile* pVal);

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
	afx_msg VARIANT_BOOL LoadMapState(LPCTSTR Filename, LPDISPATCH Callback);

	afx_msg VARIANT_BOOL SaveLayerOptions(LONG LayerHandle, LPCTSTR OptionsName, VARIANT_BOOL Overwrite, LPCTSTR Description);
	afx_msg VARIANT_BOOL LoadLayerOptions(LONG LayerHandle, LPCTSTR OptionsName, BSTR* Description);

	afx_msg IExtents* GetGeographicExtents();
	afx_msg VARIANT_BOOL SetGeographicExtents(IExtents* extents);

	afx_msg ITiles* GetTiles(void);
	afx_msg IFileManager* GetFileManager(void);
	//afx_msg void SetTiles(ITiles* pVal);

	afx_msg void ProjToPixel(double projX, double projY, double FAR* pixelX, double FAR* pixelY);
	afx_msg void PixelToProj(double pixelX, double pixelY, double FAR* projX, double FAR* projY);

	afx_msg BSTR GetLayerDescription(LONG LayerHandle);
	afx_msg void SetLayerDescription(LONG LayerHandle, LPCTSTR newVal);

	afx_msg BSTR GetGridFileName(LONG LayerHandle);
	afx_msg void SetGridFileName(LONG LayerHandle, LPCTSTR newVal);

	afx_msg VARIANT_BOOL GetScalebarVisible(void);
	afx_msg void SetScalebarVisible(VARIANT_BOOL pVal);

	afx_msg VARIANT_BOOL GetShowZoomBar(void);
	afx_msg void SetShowZoomBar(VARIANT_BOOL pVal);

	afx_msg tkScalebarUnits GetScalebarUnits(void);
	afx_msg void SetScalebarUnits(tkScalebarUnits pVal);

	afx_msg VARIANT_BOOL GetShowRedrawTime(void);
	afx_msg void SetShowRedrawTime(VARIANT_BOOL newVal);

	afx_msg VARIANT_BOOL GetShowVersionNumber(void);
	afx_msg void SetShowVersionNumber(VARIANT_BOOL newVal);

	afx_msg ILabels* GetDrawingLabels(long DrawingLayerIndex);
	afx_msg void SetDrawingLabels(long DrawingLayerIndex, ILabels* newValue); //LPDISPATCH newValue);

	afx_msg VARIANT_BOOL GetLayerSkipOnSaving(LONG LayerHandle);
	afx_msg void SetLayerSkipOnSaving(LONG LayerHandle, VARIANT_BOOL newVal);

	afx_msg void Clear();

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
	afx_msg void Redraw2(tkRedrawType redrawType);
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
		
	#pragma region New API members
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
	afx_msg void SetMapUnits(tkUnitsOfMeasure units);
	afx_msg tkUnitsOfMeasure GetMapUnits(void);
	afx_msg BOOL SnapShotToDC2(PVOID hdc, IExtents* Extents, LONG Width, float OffsetX, float OffsetY, float ClipX, float ClipY, float clipWidth, float clipHeight);
	afx_msg BOOL SnapShotToDC(PVOID hdc, IExtents* Extents, LONG Width);
	afx_msg void LoadTilesForSnapshot(IExtents* Extents, LONG WidthPixels, LPCTSTR Key, tkTileProvider provider);
	afx_msg INT TilesAreInCache(IExtents* Extents, LONG WidthPixels, tkTileProvider provider);
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
	afx_msg IShapeEditor* GetShapeEditor(void);
	afx_msg VARIANT_BOOL ZoomToWorld(void);
	afx_msg VARIANT_BOOL FindSnapPoint(double tolerance, double xScreen, double yScreen, double* xFound, double* yFound);
	afx_msg long AddLayerFromFilename(LPCTSTR Filename, tkFileOpenStrategy openStrategy, VARIANT_BOOL visible);
	afx_msg long AddLayerFromDatabase(LPCTSTR ConnectionString, LPCTSTR layerNameOrQuery, VARIANT_BOOL visible);
	afx_msg VARIANT_BOOL SetGeographicExtents2(double xLongitude, double yLatitude, double widthKilometers);
	afx_msg IExtents* GetKnownExtents(tkKnownExtents extents);
	afx_msg void SetLatitude(float nNewValue);
	afx_msg float GetLatitude();
	afx_msg void SetLongitude(float nNewValue);
	afx_msg float GetLongitude();
	afx_msg void SetCurrentZoom(int nNewValue);
	afx_msg int GetCurrentZoom();
	afx_msg void SetTileProvider(tkTileProvider nNewValue);
	afx_msg tkTileProvider GetTileProvider();
	afx_msg void SetProjection(tkMapProjection nNewValue);
	afx_msg tkMapProjection GetProjection();
	afx_msg void SetKnownExtentsCore(tkKnownExtents nNewValue);
	afx_msg tkKnownExtents GetKnownExtentsCore();
	afx_msg void SetShowCoordinates(tkCoordinatesDisplay nNewValue);
	afx_msg tkCoordinatesDisplay GetShowCoordinates();
	afx_msg void SetGrabProjectionFromData(VARIANT_BOOL nNewValue);
	afx_msg VARIANT_BOOL GetGrabProjectionFromData();
	afx_msg VARIANT_BOOL ProjToDegrees(double projX, double projY, double* degreesLngX, double * degreesLatY);
	afx_msg VARIANT_BOOL DegreesToProj(double degreesLngX, double degreesLatY, double* projX, double* projY);
	afx_msg VARIANT_BOOL PixelToDegrees(double pixelX, double pixelY, double* degreesLngX, double * degreesLatY);
	afx_msg VARIANT_BOOL DegreesToPixel(double degreesLngX, double degreesLatY, double* pixelX, double* pixelY);
	afx_msg tkCustomState GetAnimationOnZooming();
	afx_msg void SetAnimationOnZooming(tkCustomState newVal);
	afx_msg tkCustomState GetInertiaOnPanning();
	afx_msg void SetInertiaOnPanning(tkCustomState newVal);
	afx_msg VARIANT_BOOL GetReuseTileBuffer();
	afx_msg void SetReuseTileBuffer(VARIANT_BOOL newVal);
	afx_msg tkZoomBarVerbosity GetZoomBarVerbosity();
	afx_msg void SetZoomBarVerbosity(tkZoomBarVerbosity newVal);
	afx_msg tkZoomBoxStyle GetZoomBoxStyle();
	afx_msg void SetZoomBoxStyle(tkZoomBoxStyle newVal);
	afx_msg tkMismatchBehavior GetProjectionMismatchBehavior();
	afx_msg void SetProjectionMismatchBehavior(tkMismatchBehavior newVal);
	afx_msg long GetZoomBarMinZoom();
	afx_msg void SetZoomBarMinZoom(long newVal);
	afx_msg long GetZoomBarMaxZoom();
	afx_msg void SetZoomBarMaxZoom(long newVal);
	afx_msg VARIANT_BOOL GetLayerVisibleAtCurrentScale(LONG LayerHandle);
	afx_msg IUndoList* GetUndoList();
	afx_msg VARIANT_BOOL GetHotTracking();
	afx_msg void SetHotTracking(VARIANT_BOOL newVal);
	afx_msg void OnHotTrackingColorChanged();
	afx_msg void OnMouseToleranceChanged();
	#pragma endregion

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
	void FireExtentsChanged(){ FireEvent(eventidExtentsChanged,EVENT_PARAM(VTS_NONE));}
	void FireMapState(long LayerHandle)
		{FireEvent(eventidMapState,EVENT_PARAM(VTS_I4), LayerHandle);}
	void FireOnDrawBackBuffer(long BackBuffer)
		{FireEvent(eventidOnDrawBackBuffer,EVENT_PARAM(VTS_I4), BackBuffer);}
	void FireShapeHighlighted(long LayerHandle, long ShapeIndex)
		{ FireEvent(eventidShapeHighlighted,EVENT_PARAM(VTS_I4 VTS_I4), LayerHandle, ShapeIndex);}
	void FireBeforeDrawing(long hdc, long xMin, long xMax, long yMin, long yMax, VARIANT_BOOL* Handled)
		{FireEvent(eventidBeforeDrawing,EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PBOOL), hdc, xMin, xMax, yMin, yMax, Handled);}
	void FireAfterDrawing(long hdc, long xMin, long xMax, long yMin, long yMax, VARIANT_BOOL* Handled)
		{FireEvent(eventidAfterDrawing,EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PBOOL), hdc, xMin, xMax, yMin, yMax, Handled);}
	void FireTilesLoaded(IDispatch* tiles, IDispatch* extents, VARIANT_BOOL snapshot, LPCTSTR key)
		{FireEvent(eventidTilesLoaded,EVENT_PARAM(VTS_DISPATCH VTS_DISPATCH VTS_BOOL VTS_BSTR ), tiles, extents, snapshot,key);}
	void FireMeasuringChanged(IDispatch* measuring, tkMeasuringAction action)
		{FireEvent(eventidMeasuringChanged,EVENT_PARAM(VTS_DISPATCH VTS_I4), measuring, action);}
	void FireLayersChanged()
		{FireEvent(eventidLayersChanged,EVENT_PARAM(VTS_NONE));}
	void FireBeforeShapeEdit(tkUndoOperation action, LONG layerHandle, LONG shapeIndex, tkMwBoolean* Cancel)
		{FireEvent(eventidBeforeShapeEdit, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_PI4), action, layerHandle, shapeIndex, Cancel);	}
	void FireValidateShape(LONG LayerHandle, IDispatch* Shape, tkMwBoolean* Cancel)
		{FireEvent(eventidValidateShape, EVENT_PARAM(VTS_I4 VTS_DISPATCH VTS_PI4), LayerHandle, Shape, Cancel);	}
	void FireAfterShapeEdit(tkMwBoolean Action, LONG LayerHandle, LONG ShapeIndex)
		{FireEvent(eventidAfterShapeEdit, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4), Action, LayerHandle, ShapeIndex); }
	void FireChooseLayer(long x, long y, LONG* LayerHandle)
	{
		double xProj, yProj;
		PixelToProjection(x, y, xProj, yProj);
		FireEvent(eventidChooseLayer, EVENT_PARAM(VTS_R8 VTS_R8 VTS_PI4), xProj, yProj, LayerHandle);
	}
	void FireShapeValidationFailed(LPCTSTR ErrorMessage)
		{ FireEvent(eventidShapeValidationFailed, EVENT_PARAM(VTS_BSTR), ErrorMessage);}
	void FireBeforeDeleteShape(tkDeleteTarget target, tkMwBoolean* cancel)
		{FireEvent(eventidBeforeDeleteShape, EVENT_PARAM(VTS_I4 VTS_PI4), target, cancel);}
	void FireProjectionChanged() { FireEvent(eventidProjectionChanged, EVENT_PARAM(VTS_NONE)); }
	void FireUndoListChanged(){ FireEvent(eventidUndoListChanged, EVENT_PARAM(VTS_NONE)); }
	void FireSelectionChanged(LONG LayerHandle) { FireEvent(eventidSelectionChanged, EVENT_PARAM(VTS_I4), LayerHandle); }
	void FireShapeIdentified(LONG LayerHandle, LONG ShapeIndex, LONG pointX, LONG pointY) { FireEvent(eventidShapeIdentified, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4), LayerHandle, ShapeIndex, pointX, pointY); }

	//}}AFX_EVENT
	DECLARE_EVENT_MAP()
#pragma endregion

public:
#pragma region Members
	// --------------------------------------------
	//	  Drawing
	// --------------------------------------------
	BOOL _canUseLayerBuffer;			// the data layers can be drawn from buffer
	bool _canUseMainBuffer;				// all the stuff can taken from buffer (only mouse moves will be drawn above)
	int _redrawId;					// the ordinal number of redraw request
	bool _isSnapshot;					// used by SnapShots
	int _lockCount;
	bool _isSizing;					// the sizing of control takes place now, redraw is forbidden - TODO: replace by Mutex
	
	::CMutex m_drawMutex;

	Gdiplus::Bitmap* _layerBitmap;	   // layer buffer
	Gdiplus::Bitmap* _tilesBitmap;	   // tiles buffer
	Gdiplus::Bitmap* _drawingBitmap;   // a back buffer for drawing objects, the stuff like rubber band lines, etc.
	Gdiplus::Bitmap* _bufferBitmap;    // combined buffer, holds all the other ones (tiles, layers, drawing layers)
	Gdiplus::Bitmap* _moveBitmap;      // shapes being moved are rendered to this bitmap
	Gdiplus::Bitmap* _tempBitmap;	   // to scale contents of the rest bitmaps 

	Gdiplus::SolidBrush _brushBlack;
	Gdiplus::SolidBrush _brushWhite;
	Gdiplus::SolidBrush _brushGray;
	Gdiplus::Pen _penGray;
	Gdiplus::Pen _penDarkGray;
	Gdiplus::Font* _fontCourier;
	Gdiplus::Font* _fontArial;

	short m_cursorMode;
	HCURSOR _cursorPan;
	HCURSOR _cursorZoomin;
	HCURSOR _cursorZoomout;
	HCURSOR _cursorSelect;
	HCURSOR _cursorMeasure;
	HCURSOR _udCursor;
	
	// --------------------------------------------
	//	  Layers
	// --------------------------------------------
	std::deque<long> _activeLayers;
	std::vector<Layer *> _allLayers;
	
	// draw layers
	deque<long> _activeDrawLists;
	std::deque<DrawList *> _allDrawLists;
	std::deque<long> _drawingLayerInvisilbe; //stores all the invisible layer handles
	long _currentDrawing;					//current Drawing
	long _interactiveLayerHandle;			// shapefile edited by user

	// --------------------------------------------
	//	 Extents
	// --------------------------------------------
	TileBuffer _tileBuffer;
	Extent _extents;
	std::deque<Extent> _prevExtents;

	// window properties
	long _viewWidth;
	long _viewHeight;
	double _aspectRatio;

	// projection variables
	double _pixelPerProjectionX;
	double _pixelPerProjectionY;
	double _inversePixelPerProjectionX;
	double _inversePixelPerProjectionY;

	// ---------------------------------------------
	//	Public control properties
	// ---------------------------------------------
	BOOL _doTrapRMouseDown;
	BOOL _disableWaitCursor;
	BOOL _useSeamlessPan;
	BOOL _showVersionNumber;			   // whether to show version number in the lower right corner
	BOOL _showRedrawTime;				   // whether to show time in the lower left corner
	BOOL _scalebarVisible;
	BOOL _grabProjectionFromData;
	BOOL _zoombarVisible;
	BOOL _canUseImageGrouping;
	BOOL _useHotTracking;
	tkCustomState _panningInertia;			
	BOOL _reuseTileBuffer;			
	tkCustomState _zoomAnimation;			
	OLE_COLOR _hotTrackingColor;

	tkZoomBoxStyle _zoomBoxStyle;
	tkShapeDrawingMethod _shapeDrawingMethod;
	tkUnitsOfMeasure _unitsOfMeasure;
	tkResizeBehavior _mapResizeBehavior;  // How to behave on resize
	tkZoomBehavior _zoomBehavior;
	tkKnownExtents _knownExtents;
	tkCoordinatesDisplay _showCoordinates;
	tkScalebarUnits  _scalebarUnits;
	tkZoomBarVerbosity _zoomBarVerbosity;
	tkMismatchBehavior _projectionMismatchBehavior;
	
	CString _versionNumber;
	double _mouseWheelSpeed;
	long _zoomBarMinZoom;
	long _zoomBarMaxZoom;
	long _lastErrorCode;
	DOUBLE _mouseTolerance;

	// ---------------------------------------------
	//	COM instances
	// ---------------------------------------------
	IFileManager* _fileManager;
	IMeasuring* _measuring;
	CShapeEditor* _shapeEditor;
	ITiles* _tiles;						// the list of tiles (in-memory GDI+ bitmaps)
	ICallback * _globalCallback;
	IUndoList* _undoList;

	// ---------------------------------------------
	//	Projections
	// ---------------------------------------------
	IGeoProjection* _projection;			// must always have transformation to WWGS84
	IGeoProjection* _wgsProjection;		// must always have transform to current map projection (set in map.SetGeoProjection)
	IGeoProjection* _gmercProjection;		// must always have transform to current map projection (set in map.SetGeoProjection)
	IGeoProjection* _tileProjection;
	IGeoProjection* _tileReverseProjection;
	tkTransformationMode _transformationMode;
	TileProjectionState _tileProjectionState;

	// ---------------------------------------------
	//	snapshot temp variables
	// ---------------------------------------------
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

	// ---------------------------------------------
	//	cursor state
	// ---------------------------------------------
	BOOL _leftButtonDown;
	DraggingState _dragging;
	Extent _clickDownExtents;
	::CCriticalSection _panningLock;
	
	// ---------------------------------------------
	//	structures and lists
	// ---------------------------------------------
	std::vector<ImageGroup*>* _imageGroups;
	std::vector<TimedPoint*> _panningList;

	ZoombarPart _lastZooombarPart;
	ZoombarParts _zoombarParts;
	CCollisionList _collisionList;	// global collision list for labels and charts
	HotTrackingInfo _hotTracking;	// temporary shapefile to display hot tracking
	
	// ---------------------------------------------
	//	temp state variable
	// ---------------------------------------------
	tkCursorMode _lastCursorMode;   // last one set externally (OnCursorChanged callback)
	double _lastWidthMeters;		// last width of the screen in meters (to avoid recalculation of scale bar on tile redraw)
	int _activeLayerPosition;		// for zooming between layer
	bool _rectTrackerIsActive;
	bool _measuringPersistent;
	bool _reverseZooming;
	bool _hasHotTracking;
	int _zoombarTargetZoom;
	bool _panningAnimation;
	int _currentZoom;				// cached zoom set with ZoomToTileLevel (works only with ZoomBehavior = zbUseTileLevels)
	bool _spacePressed;
	float _lastRedrawTime;
	int _projectionChangeCount;
	int _shapeCountInView;

	// ---------------------------------------------
	//	various stuff
	// ---------------------------------------------
	// rotation
	Rotate *_rotate;
	float   _rotateAngle;			//Map Rotation (ajp June 2010) 
	
	// tooltips
	CToolTipEx _ttip;
	CButton * _ttipCtrl;
	BOOL _showingToolTip;

	// serial number
	CString _serial;
	const char * _vals;
	const int _valsLen;

	::CMutex _mapstateMutex;
	
	// obsolete members
	BOOL _multilineLabeling;			// Allow multi line labels?
	long _lineSeparationFactor;	// Thickness between double and triple lines; adjust factor.
	bool _useLabelCollision;		//Label Collision

	CPropExchange* _propertyExchange;
#pragma endregion

#pragma region Methods

public:
	IExtents* GetGeographicExtentsCore(bool clipForTiles, Extent* clipExtents = NULL);
	ITiles* GetTilesNoRef(void);
	bool SendMouseMove();
	bool SendSelectBoxDrag();
	void ErrorMessage(long ErrorCode);
	CString Crypt(CString str);
	bool VerifySerial(CString str);
	void DrawLayers(const CRect & rcBounds, Gdiplus::Graphics* graphics, bool layerBuffer = true);
	bool HasImages() ;
	bool HasHotTracking();
	bool HasVolatileShapefiles();
	
	DOUBLE GetPixelsPerDegree(void);
	DOUBLE PixelsPerMapUnit(void);

	inline void PixelToProjection( double piX, double piY, double & prX, double & prY );
	inline void ProjectionToPixel( double prX, double prY, double & piX, double & piY );

	// some simple encapsulation for code readability
	IGeoProjection* GetMapToWgs84Transform();
	IGeoProjection* GetMapToTilesTransform();
	IGeoProjection* GetWgs84ToMapTransform();
	IGeoProjection* GetTilesToMapTransform();
	IGeoProjection* GetWgs84Projection();
	IGeoProjection* GetGMercProjection();
	IGeoProjection* GetMapProjection();
	
	void RedrawCore( tkRedrawType redrawType, bool updateTiles, bool atOnce );

	void ReleaseProjections();
	void InitProjections();
	void UpdateTileProjection();
	
private:
	bool IS_VALID_LAYER(int layerHandle, vector<Layer*> allLayers)
	{
		return (layerHandle >= 0 && layerHandle < (int)allLayers.size()?(allLayers[layerHandle]!=NULL?TRUE:FALSE):FALSE);
	}
public:	
	// ---------------------------------------------
	// Gdiplus startup/shutdown handling. 
	// Please see *.cpp for more explanation.
	// ---------------------------------------------
	static void GdiplusStartup();
	static void GdiplusShutdown();
	static ULONG_PTR ms_gdiplusToken;
	static ULONG_PTR ms_gdiplusBGThreadToken;
	static unsigned ms_gdiplusCount;
	static ::CCriticalSection ms_gdiplusLock;
	static Gdiplus::GdiplusStartupOutput ms_gdiplusStartupOutput;
private:	
	// ---------------------------------------------
	//	 Init/terminate
	// ---------------------------------------------
	void SetDefaults();
	void ReleaseTempObjects();
	void Shutdown();
	void Startup();

	// ---------------------------------------------
	//	 Drawing
	// ---------------------------------------------
	void HandleNewDrawing(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool drawToOutputCanvas, float offsetX = 0.0f, float offsetY = 0.0f);
	void ShowRedrawTime(Gdiplus::Graphics* g, float time, bool layerRedraw, CStringW message = L"");
	//void DrawMeasuringToMainBuffer(Gdiplus::Graphics* g );
	//IPoint* GetMeasuringPolyCenter(Gdiplus::PointF* data, int length);
	//void DrawMeasuringPolyArea(Gdiplus::Graphics* g, bool lastPoint, double lastGeogX, double lastGeogY, IPoint* pnt);
	//void DrawSegmentInfo(Gdiplus::Graphics* g, double xScr, double yScr, double xScr2, double yScr2, double length, double totalLength, int segmentIndex);
	void DrawCoordinates(Gdiplus::Graphics* g);
	void DrawScaleBar(Gdiplus::Graphics* g);
	bool HasDrawingData(tkDrawingDataAvailable type);
	void DrawTiles(Gdiplus::Graphics* g);
	void DrawLayersRotated(CDC* pdc, Gdiplus::Graphics* gLayers, const CRect& rcBounds);
	void DrawImageGroups();
	void DrawStringWithShade(Gdiplus::Graphics* g, CStringW s, Gdiplus::Font *font, Gdiplus::PointF &point, Gdiplus::Brush *brush, Gdiplus::Brush *brushOutline);
	void DrawZoombar(Gdiplus::Graphics* g);
	void DrawLists(const CRect & rcBounds, Gdiplus::Graphics* graphics, tkDrawReferenceList listType);
	void DrawDrawing(Gdiplus::Graphics* graphics, DrawList * dlist);
	void DrawDynamic(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool drawBackBuffer = false, float offsetX = 0.0f, float offsetY = 0.0f);
	IDispatch* SnapShotCore(double left, double right, double top, double bottom, long Width, long Height, 
								  CDC* snapDC = NULL, float offsetX = 0.0f, float offsetY = 0.0f,
								  float clipX = 0.0f, float clipY = 0.0f, float clipWidth = 0.0f, float clipHeight = 0.0f);

	// ---------------------------------------------
	//	 Serialization
	// ---------------------------------------------
	CPLXMLNode* SerializeMapStateCore(VARIANT_BOOL RelativePaths, CStringW ProjectName);		// used by SaveMapState and GetMapState
	bool DeserializeMapStateCore(CPLXMLNode* node, CStringW ProjectName, VARIANT_BOOL LoadLayers, IStopExecution* callback);
	CPLXMLNode* SerializeLayerCore(LONG LayerHandle, CStringW Filename);
	int DeserializeLayerCore(CPLXMLNode* node, CStringW ProjectName, bool utf8Filenames, IStopExecution* callback);		// adds new layer on loading (is used by map state)
	VARIANT_BOOL DeserializeLayerOptionsCore(LONG LayerHandle, CPLXMLNode* node);
	void WriteXmlHeaderAttributes(CPLXMLNode* psTree, CString fileType);
	
	// -------------------------------------------
	//	Extents and projection
	// -------------------------------------------
	void CalculateVisibleExtents( Extent e, bool LogPrev = true, bool MapSizeChanged = false);
	void SetExtentsCore(Extent ext, bool logExtents = false, bool mapSizeChanged = false, bool adjustZoom = true);
	void SetTempExtents(double left, double right, double top, double bottom, long Width, long Height);
	void RestoreExtents();
	void SetNewExtentsWithForcedZooming( Extent ext, bool zoomIn );
	IExtents* GetMaxExtents(void);
	DOUBLE DegreesPerMapUnit(void);
	double UnitsPerPixel();
	void LogPrevExtent();
	bool GetGeoPosition(double& x, double& y);
	bool SetGeoPosition(double x, double y);
	void SetCurrentZoomCore(int zoom, bool forceUpdate = false);
	void SetInitGeoExtents();
	

	// ---------------------------------------------
	//	Data layers
	// ---------------------------------------------
	bool IsValidLayer( long layerHandle );
	BSTR GetLayerFilename(LONG layerHandle);
	void CMapView::RemoveLayerCore(long LayerHandle, bool closeDatasources);
	
	// shapefiles	
	bool IsValidShape( long layerHandle, long shape );
	Layer* get_ShapefileLayer(long layerHandle);
	Layer* get_Layer(long layerHandle);
	CDrawingOptionsEx* get_ShapefileDrawingOptions(long LayerHandle);
	void AlignShapeLayerAndShapes(Layer * layer);
	ShpfileType get_ShapefileType(long layerHandle);
	
	// images
	void ReloadImageBuffers();
	void CheckForConcealedImages(bool* isConcealed, long& startCondition, long& endCondition, double scale, int zoom) ;

	// image grouping
	void BuildImageGroups(std::vector<ImageGroup*>& imageGroups);
	void DrawImageGroups(const CRect& rcBounds, Gdiplus::Graphics* graphics, int groupIndex);
	bool ImageGroupsAreEqual(std::vector<ImageGroup*>& groups1, std::vector<ImageGroup*>& groups2);

	// labels
	void LabelColor(LONG LayerHandle, OLE_COLOR LabelFontColor);
	void ClearLabelFrames();

	// drawing layers
	void SetDrawingLayerVisible(LONG LayerHandle, VARIANT_BOOL Visible);
	void DrawBackBuffer(int** hdc, int ImageWidth, int ImageHeight);
	bool IsValidDrawList(long listHandle);
	
	// ---------------------------------------------
	//	Various
	// ---------------------------------------------
	void UpdateCursor(tkCursorMode cursor, bool clearEditor);
	void ResizeBuffers(int cx, int cy);
	UINT StartDrawLayers(LPVOID pParam);
	HCURSOR SetWaitCursor();
	tkInterpolationMode ChooseInterpolationMode(tkInterpolationMode mode1, tkInterpolationMode mode2);
	void ClearMapProjectionWithLastLayer();
	ZoombarPart ZoombarHitTest(int x, int y);
	HotTrackingResult RecalcHotTracking(CPoint point, LayerShape& result);
	void ClearHotTracking();
	void UpdateHotTracking(LayerShape info, bool fireEvent);
	void DoPanning(CPoint point);
	void DoUpdateTiles(bool isSnapshot = false, CString key = "");
	bool HandleOnZoombarMouseDown( CPoint point );
	bool HandleOnZoombarMouseMove( CPoint point );
	DWORD GetPropertyExchangeVersion();
	bool ForceDiscreteZoom();
	bool HasRotation();
	void ClearPanningList();
	void DisplayPanningInertia( CPoint point );
	void UpdateTileBuffer(CDC* dc, bool zoomingAnimation);
	void DrawZoomingAnimation(Extent match, Gdiplus::Graphics* gTemp, CDC* dc, Gdiplus::RectF& source, Gdiplus::RectF& target, bool zoomingAnimation);
	void TurnOffPanning();
	void DrawZoombox(Gdiplus::Graphics* g);
	bool CheckLayerProjection( Layer* layer );
	void GrabLayerProjection( Layer* layer );
	bool HaveDataLayersWithinView();
	void AdjustZoom(int zoom);
	double GetCurrentTileSize( int zoom );
	bool GetMinMaxZoom(int& minZoom, int& maxZoom);
	bool GetTileMismatchMinZoom( int& minZoom );
	VARIANT_BOOL LoadLayerOptionsCore(CString baseName, LONG LayerHandle, LPCTSTR OptionsName, BSTR* Description);
	bool LayerIsEmpty(long LayerHandle);
	LayerShape FindShapeAtScreenPoint(CPoint point, LayerSelector selector);
	LayerShape FindShapeAtProjPoint(double projX, double projY, std::vector<int>& layers);
	MeasuringBase* GetMeasuringBase();
	EditorBase* GetEditorBase();
	ActiveShape* GetActiveShape();
	void DrawShapeEditor( Gdiplus::Graphics* g, bool dynamicBuffer );
	bool SelectLayerHandles(LayerSelector selector, std::vector<int>& layers);
	bool CheckLayer(LayerSelector selector, int layerHandle);
	bool SelectShapeForEditing(int x, int y, long& layerHandle, long& shapeIndex);
	double GetMouseTolerance(MouseTolerance tolernace, bool proj = true);
	int AddLayerCore(Layer* layer);
	
	// shapefile editor
	
	IShapefile* GetShapeEditorShapefile();
	bool RunShapefileUndoList(bool undo);
	bool RemoveSelectedShape();
	long ParseKeyboardEventFlags(UINT nFlags);
	long ParseMouseEventFlags(UINT nFlags);
	bool HandleOnMouseMoveShapeEditor(int x, int y, long nFlags);
	bool SnappingIsOn(long nFlags, tkSnapBehavior& behavior);
	bool HandleLButtonUpDragVertexOrShape(long nFlags);
	VARIANT_BOOL ZoomToShape2(long LayerHandle, long ShapeIndex, VARIANT_BOOL ifOutsideOnly = VARIANT_TRUE);
	void SetExtentsWithPadding(Extent extents);
	void HandleLButtonDownSelection(CPoint& point, long vbflags);
	void ZoomToEditor();
	void OnCursorModeChangedCore(bool clearEditor);
	void AttachGlobalCallbackToLayers(IDispatch* object);
	Point2D GetDraggingProjOffset();
	void HandleOnLButtonMoveOrRotate(long x, long y);
	void DrawMovingShapes(Gdiplus::Graphics* g, const CRect& rect, bool dynamicBuffer);
	void RegisterGroupOperation(DraggingOperation operation);
	bool InitRotationTool();
	void DrawRotationCenter(Gdiplus::Graphics* g);
	bool InitDraggingShapefile();
	double GetDraggingRotationAngle();
	double GetDraggingRotationAngle(long screenX, long screenY);
	void RegisterRotationOperation();
	bool SplitByPolyline(long layerHandle, IShapefile* sf, vector<long>& indices, IShape* polyline);
	void ShowToolTipOnMouseMove(UINT nFlags, CPoint point);
	void HandleLButtonUpZoomBox(long vbflags, long x, long y);
	Extent GetPointSelectionBox(IShapefile* sf, double xProj, double yProj);
	bool DrillDownSelect(double projX, double projY, long& layerHandle, long& shapeIndex);
	bool StartNewBoundShape(long x, long y);
	CPLXMLNode* LayerOptionsToXmlTree(long layerHandle);
	void LoadOgrStyle(Layer* layer, long layerHandle, CStringW name);
	
#pragma endregion

public:
	// limited interface for callback from related classes
	virtual IUndoList* _GetUndoList() { return _undoList; }
	virtual IShapefile* _GetShapefile(LONG layerHandle) { return GetShapefile(layerHandle); }
	virtual IShapeEditor* _GetShapeEditor() {return _shapeEditor; }
	virtual ICallback* _GetGlobalCallback() {return _globalCallback; }
	virtual void _ZoomToShape(long layerHandle, long shapeIndex) { ZoomToShape2(layerHandle, shapeIndex); }
	virtual IGeoProjection* _GetWgs84Projection() { return GetWgs84Projection(); }
	virtual IGeoProjection* _GetMapProjection() {return GetMapProjection(); }
	virtual tkTransformationMode _GetTransformationMode() { return _transformationMode; }
	virtual void _ProjectionToPixel(double projX, double projY, double* pixelX, double* pixelY){ ProjectionToPixel(projX, projY, *pixelX, *pixelY); }
	virtual void _PixelToProjection(double pixelX, double pixelY, double* projX, double* projY){ PixelToProjection(pixelX, pixelY, *projX, *projY); }
	virtual void _FireBeforeDeleteShape(tkDeleteTarget target, tkMwBoolean* cancel) { FireBeforeDeleteShape(target, cancel); }
	virtual tkCursorMode _GetCursorMode() { return (tkCursorMode)m_cursorMode; }
	virtual void _FireValidateShape(LONG LayerHandle, IDispatch* Shape, tkMwBoolean* Cancel) 	{ FireValidateShape(LayerHandle, Shape, Cancel); }
	virtual void _FireAfterShapeEdit(tkMwBoolean NewShape, LONG LayerHandle, LONG ShapeIndex) { FireAfterShapeEdit(NewShape, LayerHandle, ShapeIndex); }
	virtual void _FireShapeValidationFailed(LPCTSTR ErrorMessage) { FireShapeValidationFailed(ErrorMessage); }
	virtual void _ZoomToEditor(){ ZoomToEditor(); }
	virtual void _SetMapCursor(tkCursorMode mode, bool clearEditor) { UpdateCursor(mode, false); }
	virtual void _Redraw(tkRedrawType redrawType, bool updateTiles, bool atOnce){ RedrawCore(redrawType, updateTiles, atOnce); };
	virtual void _FireUndoListChanged() { FireUndoListChanged(); }
	virtual void _UnboundShapeFinished(IShape* shp);
	virtual double _GetMouseProjTolerance() { return GetMouseTolerance(MouseTolerance::ToleranceSelect); }
	virtual void _StartDragging(DraggingOperation operation) {
		_dragging.Operation = operation;
		SetCapture();
	}
	


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.