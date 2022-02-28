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
// Paul Meems sept. 2019 - MWGIS-183: Merge .NET and VB drawing functions

// ReSharper disable CppInconsistentNaming
#pragma once
# include <afxmt.h>
# include "DispIds.h"
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
#include "ImageDrawing.h"
#include "ShapefileDrawing.h"
#include "BaseProvider.h"
#include "TileManager.h"

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

	long _extentHistoryCount;
	afx_msg void OnExtentHistoryChanged();

	CString m_key;
	afx_msg void OnKeyChanged();

	BOOL m_doubleBuffer;
	afx_msg void OnDoubleBufferChanged();

	BOOL m_sendOnDrawBackBuffer;
	afx_msg void OnSendOnDrawBackBufferChanged();

	void OnShowCoordinatesFormatChanged();
	tkAngleFormat _showCoordinatesFormat;

#pragma endregion

	//Read-only properties
#pragma region Read-only properties

	afx_msg long GetNumLayers();
	afx_msg long GetLastErrorCode();
	afx_msg short GetIsLocked();
	afx_msg LPDISPATCH GetGetObject(long layerHandle);
	afx_msg BSTR GetErrorMsg(long ErrorCode);
	afx_msg long GetLayerPosition(long layerHandle);
	afx_msg long GetLayerHandle(long layerPosition);
	afx_msg LPDISPATCH GetColorScheme(long LayerHandle);
	afx_msg VARIANT_BOOL get_Fileame(LONG LayerHandle, BSTR* layerName);

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

	afx_msg BSTR GetLayerKey(long layerHandle);
	afx_msg void SetLayerKey(long layerHandle, LPCTSTR lpszNewValue);

	afx_msg BOOL GetLayerVisible(long layerHandle);
	afx_msg void SetLayerVisible(long layerHandle, BOOL bNewValue);

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

	afx_msg IWmsLayer* GetWmsLayer(LONG LayerHandle);

	afx_msg IShapefile* GetShapefile(LONG LayerHandle);
	afx_msg void SetShapefile(LONG LayerHandle, IShapefile* pVal);

	afx_msg IImage* GetImage(LONG LayerHandle);
	afx_msg void SetImage(LONG LayerHandle, IImage* pVal);

	afx_msg BSTR GetLayerName(LONG layerHandle);
	afx_msg void SetLayerName(LONG layerHandle, LPCTSTR newVal);

	afx_msg IGeoProjection* GetGeoProjection(void);
	afx_msg void SetGeoProjection(IGeoProjection* pVal);

	afx_msg DOUBLE GetLayerMaxVisibleScale(LONG layerHandle);
	afx_msg void SetLayerMaxVisibleScale(LONG layerHandle, DOUBLE newVal);

	afx_msg DOUBLE GetLayerMinVisibleScale(LONG layerHandle);
	afx_msg void SetLayerMinVisibleScale(LONG layerHandle, DOUBLE newVal);

	afx_msg int GetLayerMaxVisibleZoom(LONG layerHandle);
	afx_msg void SetLayerMaxVisibleZoom(LONG layerHandle, int newVal);

	afx_msg int GetLayerMinVisibleZoom(LONG layerHandle);
	afx_msg void SetLayerMinVisibleZoom(LONG layerHandle, int newVal);

	afx_msg VARIANT_BOOL GetLayerDynamicVisibility(LONG layerHandle);
	afx_msg void SetLayerDynamicVisibility(LONG layerHandle, VARIANT_BOOL newVal);

	afx_msg VARIANT_BOOL SaveMapState(LPCTSTR filename, VARIANT_BOOL RelativePaths, VARIANT_BOOL Overwrite);
	afx_msg VARIANT_BOOL LoadMapState(LPCTSTR Filename, LPDISPATCH Callback);

	afx_msg VARIANT_BOOL SaveLayerOptions(LONG layerHandle, LPCTSTR optionsName, VARIANT_BOOL overwrite, LPCTSTR description);
	afx_msg VARIANT_BOOL LoadLayerOptions(LONG layerHandle, LPCTSTR optionsName, BSTR* description);

	afx_msg IExtents* GetGeographicExtents();
	afx_msg VARIANT_BOOL SetGeographicExtents(IExtents* extents);

	afx_msg ITiles* GetTiles(void);
	afx_msg IFileManager* GetFileManager(void);
	afx_msg IIdentifier* GetIdentifier(void);

	afx_msg void ProjToPixel(double projX, double projY, double FAR* pixelX, double FAR* pixelY);
	afx_msg void PixelToProj(double pixelX, double pixelY, double FAR* projX, double FAR* projY);

	afx_msg BSTR GetLayerDescription(LONG layerHandle);
	afx_msg void SetLayerDescription(LONG layerHandle, LPCTSTR newVal);

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

	afx_msg VARIANT_BOOL GetLayerSkipOnSaving(LONG layerHandle);
	afx_msg void SetLayerSkipOnSaving(LONG layerHandle, VARIANT_BOOL newVal);

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

	afx_msg void GetLayerStandardViewWidth(long LayerHandle, double* Width);
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

	afx_msg void GetDrawingStandardViewWidth(long drawHandle, double* Width);
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
	afx_msg void Redraw3(tkRedrawType redrawType, VARIANT_BOOL reloadTiles);
	afx_msg void Redraw2(tkRedrawType redrawType);
	afx_msg void Redraw();
	afx_msg long AddLayer(LPDISPATCH object, BOOL Visible);
	afx_msg void RemoveLayer(long layerHandle);
	afx_msg void RemoveLayerWithoutClosing(long layerHandle);
	afx_msg void RemoveAllLayers();
	afx_msg BOOL MoveLayerUp(long initialPosition);
	afx_msg BOOL MoveLayerDown(long initialPosition);
	afx_msg BOOL MoveLayer(long initialPosition, long targetPosition);
	afx_msg BOOL MoveLayerTop(long initialPosition);
	afx_msg BOOL MoveLayerBottom(long initialPosition);
	afx_msg void ZoomToMaxExtents();
	afx_msg void ZoomToMaxVisibleExtents();
	afx_msg void ZoomToLayer(long LayerHandle);
	afx_msg void ZoomToShape(long LayerHandle, long Shape);
	afx_msg void ZoomIn(double Percent);
	afx_msg void ZoomOut(double Percent);
	afx_msg long ZoomToPrev();
	afx_msg long ZoomToNext();

	afx_msg void ClearDrawing(long DrawHandle);
	afx_msg void ClearDrawings();
	afx_msg LPDISPATCH SnapShot(IExtents* BoundBox);
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
	afx_msg long NewDrawing(short projection);
	afx_msg void DrawPoint(double x, double y, long size, OLE_COLOR color, uint8_t alpha = 255);
	afx_msg void DrawLine(double x1, double y1, double x2, double y2, long width, OLE_COLOR color, uint8_t alpha = 255);
	afx_msg void DrawCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill, uint8_t alpha = 255);
	afx_msg void DrawPolygon(VARIANT* xPoints, VARIANT* yPoints, long numPoints, OLE_COLOR color, BOOL fill, uint8_t alpha = 255);
	afx_msg void DrawWideCircle(double x, double y, double radius, OLE_COLOR color, BOOL fill, int width, uint8_t alpha = 255);
	afx_msg void DrawWidePolygon(VARIANT* xPoints, VARIANT* yPoints, long numPoints, OLE_COLOR color, BOOL fill, short width, uint8_t alpha = 255);
	afx_msg BOOL IsTIFFGrid(LPCTSTR filename);
	afx_msg BOOL IsSameProjection(LPCTSTR proj4A, LPCTSTR proj4B);
	afx_msg long HWnd();
	afx_msg void ReSourceLayer(long layerHandle, LPCTSTR newSrcPath);
	afx_msg BOOL ReloadOgrLayerFromSource(long ogrLayerHandle);
	afx_msg void RestartBackgroundLoading(long ogrLayerHandle);
	afx_msg BOOL AdjustLayerExtents(long layerHandle);	// Rob Cairns 29-Jun-09
	afx_msg void SetCurrentScale(DOUBLE newVal);
	afx_msg DOUBLE GetCurrentScale(void);
	afx_msg void SetMapUnits(tkUnitsOfMeasure units);
	afx_msg tkUnitsOfMeasure GetMapUnits(void);
	afx_msg BOOL SnapShotToDC2(PVOID hdc, IExtents* extents, LONG width, float offsetX, float offsetY, float clipX, float clipY, float clipWidth, float clipHeight);
	afx_msg BOOL SnapShotToDC(PVOID hdc, IExtents* extents, LONG width);
	afx_msg BOOL LoadTilesForSnapshot(IExtents* extents, LONG widthPixels, LPCTSTR key);
	afx_msg void SetMapRotationAngle(float nNewValue);
	afx_msg float GetMapRotationAngle();
	afx_msg IExtents* GetRotatedExtent();
	afx_msg IPoint* GetBaseProjectionPoint(double rotPixX, double rotPixY);
	afx_msg void DrawLineEx(LONG layerHandle, DOUBLE x1, DOUBLE y1, DOUBLE x2, DOUBLE y2, LONG pixelWidth, OLE_COLOR color, uint8_t alpha = 255);
	afx_msg void DrawPointEx(LONG layerHandle, DOUBLE x, DOUBLE y, LONG pixelSize, OLE_COLOR color, uint8_t alpha = 255);
	afx_msg void DrawCircleEx(LONG layerHandle, DOUBLE x, DOUBLE y, DOUBLE pixelRadius, OLE_COLOR color, VARIANT_BOOL fill, uint8_t alpha = 255);
	afx_msg void DrawPolygonEx(LONG layerHandle, VARIANT* xPoints, VARIANT* yPoints, LONG numPoints, OLE_COLOR color, VARIANT_BOOL fill, uint8_t alpha = 255);
	afx_msg void DrawWideCircleEx(LONG layerHandle, double x, double y, double radius, OLE_COLOR color, VARIANT_BOOL fill, short outlineWidth, uint8_t alpha = 255);
	afx_msg void DrawWidePolygonEx(LONG layerHandle, VARIANT* xPoints, VARIANT* yPoints, long numPoints, OLE_COLOR color, VARIANT_BOOL fill, short outlineWidth, uint8_t alpha = 255);
	afx_msg BSTR SerializeLayer(LONG layerHandle);
	afx_msg VARIANT_BOOL DeserializeLayer(LONG layerHandle, LPCTSTR newVal);
	afx_msg IDispatch* SnapShot2(LONG clippingLayerNbr, DOUBLE zoom, long pWidth);
	afx_msg IDispatch* SnapShot3(double left, double right, double top, double bottom, long width);
	afx_msg VARIANT_BOOL SetImageLayerColorScheme(LONG layerHandle, IDispatch* colorScheme);
	afx_msg void UpdateImage(LONG layerHandle);
	afx_msg VARIANT_BOOL DeserializeMapState(LPCTSTR state, VARIANT_BOOL loadLayers, LPCTSTR basePath);
	afx_msg BSTR SerializeMapState(VARIANT_BOOL relativePaths, LPCTSTR basePath);
	afx_msg CString get_OptionsFilename(LONG layerHandle, LPCTSTR optionsName);
	afx_msg VARIANT_BOOL RemoveLayerOptions(LONG layerHandle, LPCTSTR optionsName);
	afx_msg VARIANT_BOOL ZoomToSelected(LONG layerHandle);
	afx_msg VARIANT_BOOL ZoomToTileLevel(int zoom);
	afx_msg IMeasuring* GetMeasuring();
	afx_msg IShapeEditor* GetShapeEditor();
	afx_msg VARIANT_BOOL ZoomToWorld();
	afx_msg VARIANT_BOOL FindSnapPoint(double tolerance, double xScreen, double yScreen, double* xFound, double* yFound);
	afx_msg long AddLayerFromFilename(LPCTSTR filename, tkFileOpenStrategy openStrategy, VARIANT_BOOL visible);
	afx_msg long AddLayerFromDatabase(LPCTSTR connectionString, LPCTSTR layerNameOrQuery, VARIANT_BOOL visible);
	afx_msg VARIANT_BOOL SetGeographicExtents2(double xLongitude, double yLatitude, double widthKilometers);
	afx_msg IExtents* GetKnownExtents(tkKnownExtents extents);
	afx_msg void SetLatitude(float nNewValue);
	afx_msg float GetLatitude();
	afx_msg void SetLongitude(float nNewValue);
	afx_msg float GetLongitude();
	afx_msg void SetLatitudeLongitude(double latitude, double longitude);
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
	afx_msg VARIANT_BOOL ProjToDegrees(double projX, double projY, double* degreesLngX, double* degreesLatY);
	afx_msg VARIANT_BOOL DegreesToProj(double degreesLngX, double degreesLatY, double* projX, double* projY);
	afx_msg VARIANT_BOOL PixelToDegrees(double pixelX, double pixelY, double* degreesLngX, double* degreesLatY);
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
	afx_msg long GetZoomBarMinZoom();
	afx_msg void SetZoomBarMinZoom(long newVal);
	afx_msg long GetZoomBarMaxZoom();
	afx_msg void SetZoomBarMaxZoom(long newVal);
	afx_msg VARIANT_BOOL GetLayerVisibleAtCurrentScale(LONG layerHandle);
	afx_msg IUndoList* GetUndoList();
	afx_msg VARIANT_BOOL GetHotTracking();
	afx_msg void SetHotTracking(VARIANT_BOOL newVal);
	afx_msg void OnIdentifierColorChanged();
	afx_msg void OnMouseToleranceChanged();
	afx_msg void OnIdentifierModeChanged();
	afx_msg bool Undo() { return UndoCore(false); }
	afx_msg LONG DrawLabel(LPCTSTR text, DOUBLE x, DOUBLE y, DOUBLE rotation);
	afx_msg LONG DrawLabelEx(LONG layerHandle, LPCTSTR text, DOUBLE x, DOUBLE y, DOUBLE rotation);
	afx_msg DOUBLE GeodesicLength(IShape* polyline);
	afx_msg DOUBLE GeodesicDistance(DOUBLE projX1, DOUBLE projY1, DOUBLE projX2, DOUBLE projY2);
	afx_msg DOUBLE GeodesicArea(IShape* polygon);
	afx_msg void OnMouseLeave();
	afx_msg ISelectionList* GetIdentifiedShapes();
	afx_msg IDrawingRectangle* GetFocusRectangle();
	afx_msg IExtents* GetLayerExtents(LONG layerIndex);
	afx_msg void ClearExtentHistory();
	afx_msg long GetExtentHistoryUndoCount();
	afx_msg long GetExtentHistoryRedoCount();
	afx_msg VARIANT_BOOL StartNewBoundShape(DOUBLE x, DOUBLE y);
	afx_msg VARIANT_BOOL StartNewBoundShapeEx(long layerHandle);

	afx_msg void SetUseAlternatePanCursor(VARIANT_BOOL nNewValue);
	afx_msg VARIANT_BOOL GetUseAlternatePanCursor();

	afx_msg void SetRecenterMapOnZoom(VARIANT_BOOL nNewValue);
	afx_msg VARIANT_BOOL GetRecenterMapOnZoom();
	afx_msg void SetShowCoordinatesBackground(VARIANT_BOOL nNewValue);
	afx_msg VARIANT_BOOL GetShowCoordinatesBackground();
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
	void FireMouseDown(const short button, const short shift, const long x, const long y)
	{
		FireEvent(eventidMouseDown, EVENT_PARAM(VTS_I2  VTS_I2  VTS_I4  VTS_I4), button, shift, x, y);
	}
	void FireMouseUp(short Button, short Shift, long x, long y)
	{
		FireEvent(eventidMouseUp, EVENT_PARAM(VTS_I2  VTS_I2  VTS_I4  VTS_I4), Button, Shift, x, y);
	}
	void FireMouseMove(short Button, short Shift, long x, long y)
	{
		FireEvent(eventidMouseMove, EVENT_PARAM(VTS_I2  VTS_I2  VTS_I4  VTS_I4), Button, Shift, x, y);
	}
	void FireFileDropped(LPCTSTR Filename)
	{
		FireEvent(eventidFileDropped, EVENT_PARAM(VTS_BSTR), Filename);
	}
	void FireSelectBoxFinal(long Left, long Right, long Bottom, long Top)
	{
		FireEvent(eventidSelectBoxFinal, EVENT_PARAM(VTS_I4  VTS_I4  VTS_I4  VTS_I4), Left, Right, Bottom, Top);
	}
	void FireSelectBoxDrag(long Left, long Right, long Bottom, long Top)
	{
		FireEvent(eventidSelectBoxDrag, EVENT_PARAM(VTS_I4  VTS_I4  VTS_I4  VTS_I4), Left, Right, Bottom, Top);
	}
	void FireExtentsChanged() { FireEvent(eventidExtentsChanged, EVENT_PARAM(VTS_NONE)); }
	void FireMapState(long LayerHandle)
	{
		FireEvent(eventidMapState, EVENT_PARAM(VTS_I4), LayerHandle);
	}
	void FireOnDrawBackBuffer(long BackBuffer)
	{
		FireEvent(eventidOnDrawBackBuffer, EVENT_PARAM(VTS_I4), BackBuffer);
	}
	void FireShapeHighlighted(long LayerHandle, long ShapeIndex)
	{
		FireEvent(eventidShapeHighlighted, EVENT_PARAM(VTS_I4 VTS_I4), LayerHandle, ShapeIndex);
	}
	void FireBeforeDrawing(long hdc, long xMin, long xMax, long yMin, long yMax, tkMwBoolean* Handled)
	{
		FireEvent(eventidBeforeDrawing, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4), hdc, xMin, xMax, yMin, yMax, Handled);
	}
	void FireAfterDrawing(long hdc, long xMin, long xMax, long yMin, long yMax, tkMwBoolean* Handled)
	{
		FireEvent(eventidAfterDrawing, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4), hdc, xMin, xMax, yMin, yMax, Handled);
	}
	void FireTilesLoaded(bool snapshot, LPCTSTR key, bool fromCache)
	{
		VARIANT_BOOL isSnapShot = snapshot ? VARIANT_TRUE : VARIANT_FALSE;
		VARIANT_BOOL isFromCache = fromCache ? VARIANT_TRUE : VARIANT_FALSE;
		FireEvent(eventidTilesLoaded, EVENT_PARAM(VTS_BOOL VTS_BSTR VTS_BOOL), isSnapShot, key, isFromCache);
	}
	void FireMeasuringChanged(tkMeasuringAction action)
	{
		FireEvent(eventidMeasuringChanged, EVENT_PARAM(VTS_I4), action);
	}
	void FireBeforeShapeEdit(LONG layerHandle, LONG shapeIndex, tkMwBoolean* Cancel)
	{
		FireEvent(eventidBeforeShapeEdit, EVENT_PARAM(VTS_I4 VTS_I4 VTS_PI4), layerHandle, shapeIndex, Cancel);
	}
	void FireValidateShape(LONG LayerHandle, IDispatch* Shape, tkMwBoolean* Cancel)
	{
		FireEvent(eventidValidateShape, EVENT_PARAM(VTS_I4 VTS_DISPATCH VTS_PI4), LayerHandle, Shape, Cancel);
	}
	void FireBeforeVertexDigitized(DOUBLE* pointX, DOUBLE* pointY)
	{
		FireEvent(eventidBeforeVertexDigitized, EVENT_PARAM(VTS_PR8 VTS_PR8), pointX, pointY);
	}
	void FireSnapPointRequested(DOUBLE pointX, DOUBLE pointY, DOUBLE* snappedX, DOUBLE* snappedY, tkMwBoolean* isFound, tkMwBoolean* isFinal)
	{
		FireEvent(eventidSnapPointRequested, EVENT_PARAM(VTS_R8 VTS_R8 VTS_PR8 VTS_PR8 VTS_PI4 VTS_PI4), pointX, pointY, snappedX, snappedY, isFound, isFinal);
	}
	void FireSnapPointFound(DOUBLE pointX, DOUBLE pointY, DOUBLE* snappedX, DOUBLE* snappedY)
	{
		FireEvent(eventidSnapPointFound, EVENT_PARAM(VTS_R8 VTS_R8 VTS_PR8 VTS_PR8), pointX, pointY, snappedX, snappedY);
	}
	void FireAfterShapeEdit(tkUndoOperation Action, LONG LayerHandle, LONG ShapeIndex)
	{
		FireEvent(eventidAfterShapeEdit, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4), Action, LayerHandle, ShapeIndex);
	}
	void FireChooseLayer(long x, long y, LONG* LayerHandle)
	{
		double xProj, yProj;
		PixelToProjection(x, y, xProj, yProj);
		FireEvent(eventidChooseLayer, EVENT_PARAM(VTS_R8 VTS_R8 VTS_PI4), xProj, yProj, LayerHandle);
	}
	void FireShapeValidationFailed(LPCTSTR ErrorMessage)
	{
		FireEvent(eventidShapeValidationFailed, EVENT_PARAM(VTS_BSTR), ErrorMessage);
	}
	void FireBeforeDeleteShape(tkDeleteTarget target, tkMwBoolean* cancel)
	{
		FireEvent(eventidBeforeDeleteShape, EVENT_PARAM(VTS_I4 VTS_PI4), target, cancel);
	}
	void FireProjectionChanged()
	{
		FireEvent(eventidProjectionChanged, EVENT_PARAM(VTS_NONE));
	}
	void FireUndoListChanged()
	{
		FireEvent(eventidUndoListChanged, EVENT_PARAM(VTS_NONE));
	}
	void FireSelectionChanged(LONG LayerHandle)
	{
		FireEvent(eventidSelectionChanged, EVENT_PARAM(VTS_I4), LayerHandle);
	}
	void FireShapeIdentified(LONG LayerHandle, LONG ShapeIndex, DOUBLE pointX, DOUBLE pointY)
	{
		FireEvent(eventidShapeIdentified, EVENT_PARAM(VTS_I4 VTS_I4 VTS_R8 VTS_R8), LayerHandle, ShapeIndex, pointX, pointY);
	}
	void FireLayerProjectionIsEmpty(LONG LayerHandle, tkMwBoolean* cancelAdding)
	{
		FireEvent(eventidLayerProjectionIsEmpty, EVENT_PARAM(VTS_I4 VTS_PI4), LayerHandle, cancelAdding);
	}
	void FireProjectionMismatch(LONG LayerHandle, tkMwBoolean* cancelAdding, tkMwBoolean* reproject)
	{
		FireEvent(eventidProjectionMismatch, EVENT_PARAM(VTS_I4 VTS_PI4 VTS_PI4), LayerHandle, cancelAdding, reproject);
	}
	void FireLayerReprojected(LONG LayerHandle, VARIANT_BOOL Success)
	{
		FireEvent(eventidLayerReprojected, EVENT_PARAM(VTS_I4 VTS_BOOL), LayerHandle, Success);
	}
	void FireLayerAdded(LONG LayerHandle)
	{
		FireEvent(eventidLayerAdded, EVENT_PARAM(VTS_I4), LayerHandle);
	}
	void FireLayerRemoved(LONG LayerHandle, VARIANT_BOOL fromRemoveAllLayers)
	{
		FireEvent(eventidLayerRemoved, EVENT_PARAM(VTS_I4 VTS_BOOL), LayerHandle, fromRemoveAllLayers);
	}
	void FireBackgroundLoadingStarted(LONG TaskId, LONG LayerHandle)
	{
		FireEvent(eventidBackgroundLoadingStarted, EVENT_PARAM(VTS_I4 VTS_I4), TaskId, LayerHandle);
	}
	void FireBackgroundLoadingFinished(LONG TaskId, LONG LayerHandle, LONG numFeatures, LONG numLoaded)
	{
		FireEvent(eventidBackgroundLoadingFinished, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4), TaskId, LayerHandle, numFeatures, numLoaded);
	}
	void FireGridOpened(LONG LayerHandle, LPCTSTR gridFilename, LONG bandIndex, VARIANT_BOOL isUsingProxy)
	{
		FireEvent(eventidGridOpened, EVENT_PARAM(VTS_I4 VTS_BSTR VTS_I4 VTS_BOOL), LayerHandle, gridFilename, bandIndex, isUsingProxy);
	}
	void FireOnDrawBackBuffer2(LONG height, LONG Width, LONG Stride, LONG pixelFormat, LONG scan0)
	{
		FireEvent(eventidOnDrawBackBuffer2, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4), height, Width, Stride, pixelFormat, scan0);
	}
	void FireBeforeLayers(long hdc, long xMin, long xMax, long yMin, long yMax, tkMwBoolean* Handled)
	{
		FireEvent(eventidBeforeLayers, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4), hdc, xMin, xMax, yMin, yMax, Handled);
	}
	void FireAfterLayers(long hdc, long xMin, long xMax, long yMin, long yMax, tkMwBoolean* Handled)
	{
		FireEvent(eventidAfterLayers, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4), hdc, xMin, xMax, yMin, yMax, Handled);
	}
	void FireLayerReprojectedIncomplete(LONG LayerHandle, LONG NumReprojected, LONG NumShapes)
	{
		FireEvent(eventidLayerReprojectedIncomplete, EVENT_PARAM(VTS_I4 VTS_I4 VTS_I4), LayerHandle, NumReprojected, NumShapes);
	}

	// Never used: void FireShapesIdentified(ISelectionList* selectedShapes, DOUBLE projX, DOUBLE projY) { FireEvent(eventidShapesIdentified, EVENT_PARAM(VTS_DISPATCH VTS_R8 VTS_R8), selectedShapes, projX, projY); }

	//}}AFX_EVENT
	DECLARE_EVENT_MAP()
#pragma endregion

public:
#pragma region Members
	// --------------------------------------------
	//	  Drawing
	// --------------------------------------------
	bool _canUseLayerBuffer;			// the data layers can be drawn from buffer
	bool _canUseVolatileBuffer;			// volatile shapefiles can be drawn from buffer
	bool _canUseMainBuffer;				// all the stuff can taken from buffer (only mouse moves will be drawn above)
	int _redrawId;					// the ordinal number of redraw request
	bool _isSnapshot;					// used by SnapShots
	int _lockCount;
	bool _isSizing;					// the sizing of control takes place now, redraw is forbidden - TODO: replace by Mutex

	::CMutex m_drawMutex;

	Gdiplus::Bitmap* _layerBitmap;	   // layer buffer
	Gdiplus::Bitmap* _volatileBitmap;  // drawing layers and volatile shapefiles	
	Gdiplus::Bitmap* _tilesBitmap;	   // tiles buffer
	Gdiplus::Bitmap* _bufferBitmap;    // combined buffer, holds all the other ones (tiles, layers, drawing layers)
	Gdiplus::Bitmap* _drawingBitmap;   // a back buffer for drawing objects, the stuff like rubber band lines, etc.

	Gdiplus::Bitmap* _moveBitmap;      // shapes being moved are rendered to this bitmap
	Gdiplus::Bitmap* _tempBitmap;	   // to scale contents of the rest bitmaps 

	Gdiplus::SolidBrush* _brushBlue;
	Gdiplus::SolidBrush* _brushBlack;
	Gdiplus::SolidBrush* _brushWhite;
	Gdiplus::SolidBrush* _brushLightGray;
	Gdiplus::SolidBrush* _brushGray;
	Gdiplus::Pen* _penGray;
	Gdiplus::Pen* _penDarkGray;

	Gdiplus::Font* _fontCourier;
	Gdiplus::Font* _fontCourierSmall;
	Gdiplus::Font* _fontCourierLink;
	Gdiplus::Font* _fontArial;

	short m_cursorMode;
	HCURSOR _cursorPan;
	HCURSOR _cursorZoomin;
	HCURSOR _cursorZoomout;
	HCURSOR _cursorSelect;
	HCURSOR _cursorMeasure;
	HCURSOR _cursorDigitize;
	HCURSOR _cursorVertex;
	HCURSOR _cursorSelect2;
	HCURSOR _cursorIdentify;
	HCURSOR _cursorRotate;
	HCURSOR _cursorMove;
	HCURSOR _cursorAlternatePan;

	HCURSOR _udCursor;

	tkCustomDrawingFlags _customDrawingFlags;		// tkCustomDrawing

	// --------------------------------------------
	//	  Layers
	// --------------------------------------------
	std::deque<long> _activeLayers;
	std::vector<Layer*> _allLayers;

	// draw layers
	deque<long> _activeDrawLists;
	std::deque<DrawList*> _allDrawLists;
	std::deque<long> _drawingLayerInvisilbe; //stores all the invisible layer handles
	long _currentDrawing;					//current Drawing
	long _interactiveLayerHandle;			// shapefile edited by user

	// --------------------------------------------
	//	 Extents
	// --------------------------------------------
	TileBuffer _tileBuffer;
	Extent _extents;
	vector<Extent> _prevExtents;
	int _prevExtentsIndex;
	bool _panningExtentsChanged;

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
	BOOL _useAlternatePanCursor;		// use traditional Hand cursor for panning rather than NSEW cursor
	BOOL _recenterMapOnZoom;			// recenter the map at the clicked zoom point
	BOOL _showCoordinatesBackground;    // draw white background behind coordinate display
	tkCustomState _panningInertia;
	BOOL _reuseTileBuffer;
	tkCustomState _zoomAnimation;
	tkZoomBoxStyle _zoomBoxStyle;
	tkShapeDrawingMethod _shapeDrawingMethod;
	tkUnitsOfMeasure _unitsOfMeasure;
	tkResizeBehavior _mapResizeBehavior;  // How to behave on resize
	tkZoomBehavior _zoomBehavior;
	tkKnownExtents _knownExtents;
	tkCoordinatesDisplay _showCoordinates;
	tkScalebarUnits  _scalebarUnits;
	tkZoomBarVerbosity _zoomBarVerbosity;
	tkIdentifierMode _identifierMode;

	CString _versionNumber;
	double _mouseWheelSpeed;
	long _zoomBarMinZoom;
	long _zoomBarMaxZoom;
	long _lastErrorCode;

	// ---------------------------------------------
	//	COM instances
	// ---------------------------------------------
	IIdentifier* _identifier;
	IFileManager* _fileManager;
	IMeasuring* _measuring;
	CShapeEditor* _shapeEditor;
	CShapeEditor* _geodesicShape;
	ITiles* _tiles;						// the list of tiles (in-memory GDI+ bitmaps)
	ICallback* _globalCallback;
	IUndoList* _undoList;
	ISelectionList* _identifiedShapes;
	IShapefile* _identifiedShapefile;
	IDrawingRectangle* _focusRectangle;

	// ---------------------------------------------
	//	Projections
	// ---------------------------------------------
	IGeoProjection* _projection;			// must always have transformation to WGS84. Since proj.7 transformation to WGS84 is no longer mandatory
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

	Gdiplus::RectF _copyrightRect;
	BOOL _copyrightLinkActive;
	bool _mouseTracking;

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
	Rotate* _rotate;
	float   _rotateAngle;			//Map Rotation (ajp June 2010) 

	// tooltips
	CToolTipEx _ttip;
	CButton* _ttipCtrl;
	BOOL _showingToolTip;

	// serial number
	CString _serial;
	const char* _vals;
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
	IExtents* GetGeographicExtentsCore(bool clipForTiles, Extent* clipExtents = nullptr);
	bool GetGeographicExtentsInternal(bool clipForTiles, Extent* clipExtents, Extent& result);
	bool SendMouseMove();
	bool SendSelectBoxDrag();
	void ErrorMessage(long ErrorCode, tkCallbackVerbosity verbose = cvLimited);
	CString Crypt(CString str);
	bool VerifySerial(CString str);
	void DrawLayers(const CRect& rcBounds, Gdiplus::Graphics* graphics, bool layerBuffer = true);
	bool HasImages();
	bool HasHotTracking();
	bool HasVolatileShapefiles();

	DOUBLE GetPixelsPerDegree(void);
	DOUBLE PixelsPerMapUnit(void);

	inline void PixelToProjection(double piX, double piY, double& prX, double& prY)
	{
		prX = _extents.left + piX * _inversePixelPerProjectionX;
		prY = _extents.top - piY * _inversePixelPerProjectionY;
	}

	inline void ProjectionToPixel(const double prX, const double prY, double& piX, double& piY)
	{
		piX = (prX - _extents.left) * _pixelPerProjectionX;
		piY = (_extents.top - prY) * _pixelPerProjectionY;
	}

	// some simple encapsulation for code readability
	IGeoProjection* GetMapToWgs84Transform();
	IGeoProjection* GetMapToTilesTransform();
	IGeoProjection* GetWgs84ToMapTransform();
	IGeoProjection* GetTilesToMapTransform();
	IGeoProjection* GetWgs84Projection();
	IGeoProjection* GetGMercProjection();
	// Return GeoProjection
	IGeoProjection* GetMapProjection();

	void RedrawCore(tkRedrawType redrawType, bool atOnce, bool reloadTiles = false);

	void ReleaseProjections();
	void InitProjections();
	void UpdateTileProjection();

private:
	bool IS_VALID_LAYER(const int layerHandle, const vector<Layer*> allLayers)
	{
		return layerHandle >= 0 && layerHandle < static_cast<int>(allLayers.size()) ? (allLayers[layerHandle] != nullptr ? TRUE : FALSE) : FALSE;
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
	static CCriticalSection ms_gdiplusLock;
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
	void DrawFocusRectangle(Gdiplus::Graphics* g);
	void DrawCoordinates(Gdiplus::Graphics* g);
	void DrawScaleBar(Gdiplus::Graphics* g);
	bool HasDrawingData(tkDrawingDataAvailable type);
	void DrawTiles(Gdiplus::Graphics* g);
	void RedrawTiles(Gdiplus::Graphics* g, CDC* dc);
	void DrawLayersRotated(CDC* pdc, Gdiplus::Graphics* gLayers, const CRect& rcBounds);
	void DrawImageGroups();
	void DrawStringWithShade(Gdiplus::Graphics* g, CStringW s, Gdiplus::Font* font, Gdiplus::PointF& point, Gdiplus::Brush* brush, Gdiplus::Brush* brushOutline);
	void DrawZoombar(Gdiplus::Graphics* g);
	void DrawLists(const CRect& rcBounds, Gdiplus::Graphics* graphics, tkDrawReferenceList listType);
	void DrawDrawing(Gdiplus::Graphics* graphics, DrawList* dlist);
	void DrawPolygonOnGraphics(Gdiplus::Graphics* graphics, _DrawPolygon* polygon, bool project = false);
	void DrawPointOnGraphics(Gdiplus::Graphics* graphics, _DrawPoint* point, bool project = false);
	void DrawLineOnGraphics(Gdiplus::Graphics* graphics, _DrawLine* line, bool project = false);
	void DrawCircleOnGraphics(Gdiplus::Graphics* graphics, _DrawCircle* circle, bool project = false);
	void DrawDynamic(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid, bool drawBackBuffer = false, float offsetX = 0.0f, float offsetY = 0.0f);
	IDispatch* SnapShotCore(double left, double right, double top, double bottom, long Width, long Height,
		CDC* snapDC = nullptr, float offsetX = 0.0f, float offsetY = 0.0f,
		float clipX = 0.0f, float clipY = 0.0f, float clipWidth = 0.0f, float clipHeight = 0.0f);


	// ---------------------------------------------
	//	 Serialization
	// ---------------------------------------------
	CPLXMLNode* SerializeMapStateCore(VARIANT_BOOL RelativePaths, CStringW ProjectName);		// used by SaveMapState and GetMapState
	bool DeserializeMapStateCore(CPLXMLNode* node, CStringW ProjectName, VARIANT_BOOL LoadLayers, IStopExecution* callback);
	CPLXMLNode* SerializeLayerCore(LONG layerHandle, CStringW filename);
	int DeserializeLayerCore(CPLXMLNode* node, CStringW projectName, bool utf8Filenames, IStopExecution* callback);		// adds new layer on loading (is used by map state)
	VARIANT_BOOL DeserializeLayerOptionsCore(LONG layerHandle, CPLXMLNode* node);
	void WriteXmlHeaderAttributes(CPLXMLNode* psTree, CString fileType);

	// -------------------------------------------
	//	Extents and projection
	// -------------------------------------------
	void CalculateVisibleExtents(Extent e, bool MapSizeChanged = false);
	void SetExtentsCore(Extent ext, bool logExtents = true, bool mapSizeChanged = false, bool adjustZoom = true);
	void SetTempExtents(double left, double right, double top, double bottom, long Width, long Height);
	void RestoreExtents();
	void SetNewExtentsWithForcedZooming(Extent ext, bool zoomIn);
	IExtents* GetMaxExtents();
	DOUBLE DegreesPerMapUnit();
	double UnitsPerPixel();
	void LogPrevExtent();
	bool GetGeoPosition(double& x, double& y);
	bool SetGeoPosition(double x, double y);
	void SetCurrentZoomCore(int zoom, bool forceUpdate = false);
	void SetInitGeoExtents();

	// ---------------------------------------------
	//	Data layers
	// ---------------------------------------------
	bool IsValidLayer(long layerHandle);
	BSTR GetLayerFilename(LONG layerHandle);
	void RemoveLayerCore(long layerHandle, bool closeDatasources, bool fromRemoveAll = false, bool suppressEvent = false);

	// shapefiles	
	bool IsValidShape(long layerHandle, long shape);
	Layer* GetShapefileLayer(long layerHandle);
	Layer* GetLayer(long layerHandle);
	CDrawingOptionsEx* get_ShapefileDrawingOptions(long layerHandle);
	void AlignShapeLayerAndShapes(Layer* layer);
	ShpfileType get_ShapefileType(long layerHandle);

	// images
	void ReloadBuffers();
	void CheckForConcealedImages(bool* isConcealed, long& startCondition, long& endCondition, double scale, int zoom);

	// image grouping
	void BuildImageGroups(std::vector<ImageGroup*>& imageGroups);
	void DrawImageGroups(const CRect& rcBounds, Gdiplus::Graphics* graphics, int groupIndex);
	bool ImageGroupsAreEqual(std::vector<ImageGroup*>& groups1, std::vector<ImageGroup*>& groups2);

	// labels
	void LabelColor(LONG layerHandle, OLE_COLOR labelFontColor);
	void ClearLabelFrames();

	// drawing layers
	void SetDrawingLayerVisible(LONG layerHandle, VARIANT_BOOL visible);
	void DrawBackBuffer(int hdc, int imageWidth, int imageHeight);
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
	bool ReloadTiles(bool force = true, bool snapshot = false, CString key = "");
	bool HandleOnZoombarMouseDown(CPoint point);
	bool HandleOnZoombarMouseMove(CPoint point);
	DWORD GetPropertyExchangeVersion();
	bool ForceDiscreteZoom();
	bool HasRotation();
	void ClearPanningList();
	void DisplayPanningInertia(CPoint point);
	void UpdateTileBuffer(CDC* dc, bool zoomingAnimation);
	void DrawZoomingAnimation(Extent match, Gdiplus::Graphics* gTemp, CDC* dc, Gdiplus::RectF& source, Gdiplus::RectF& target, bool zoomingAnimation);
	void TurnOffPanning();
	void DrawZoombox(Gdiplus::Graphics* g);
	bool CheckLayerProjection(Layer* layer, int layerHandle);
	void GrabLayerProjection(Layer* layer);
	bool HaveDataLayersWithinView();
	void AdjustZoom(int zoom);
	double GetCurrentTileSize(int zoom);
	bool GetMinMaxZoom(int& minZoom, int& maxZoom);
	bool GetTileMismatchMinZoom(int& minZoom);
	VARIANT_BOOL LoadLayerOptionsCore(CString baseName, LONG layerHandle, LPCTSTR optionsName, BSTR* description);
	bool LayerIsEmpty(long layerHandle);
	LayerShape FindShapeAtScreenPoint(CPoint point, LayerSelector selector);
	LayerShape FindShapeAtProjPoint(double projX, double projY, std::vector<int>& layers);
	MeasuringBase* GetMeasuringBase();
	EditorBase* GetEditorBase();
	ActiveShape* GetActiveShape();
	void DrawShapeEditor(Gdiplus::Graphics* g, bool dynamicBuffer);
	bool SelectLayerHandles(LayerSelector selector, std::vector<int>& layers);
	bool CheckLayer(LayerSelector selector, int layerHandle);
	bool SelectShapeForEditing(int x, int y, long& layerHandle, long& shapeIndex);
	double GetMouseTolerance(MouseTolerance tolernace, bool proj = true);
	int AddLayerCore(Layer* layer);

	// shapefile editor
	bool RunShapefileUndoList(bool undo);
	bool RemoveSelectedShape();
	long ParseKeyboardEventFlags(UINT nFlags);
	long ParseMouseEventFlags(UINT nFlags);
	bool HandleOnMouseMoveShapeEditor(int x, int y, long nFlags);
	bool SnappingIsOn(bool shift);
	bool SnappingIsOn(UINT flags);
	bool HandleLButtonUpDragVertexOrShape(UINT nFlags);
	VARIANT_BOOL ZoomToShape2(long layerHandle, long shapeIndex, VARIANT_BOOL ifOutsideOnly = VARIANT_TRUE);
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
	bool DrillDownSelect(double projX, double projY, ISelectionList* list, bool stopOnFirst, bool ctrl);
	CPLXMLNode* LayerOptionsToXmlTree(long layerHandle);
	VARIANT_BOOL LoadOgrStyle(Layer* layer, long layerHandle, CStringW name, bool reportError);
	VARIANT_BOOL LayerIsIdentifiable(long layerHandle, IShapefile* sf);
	VARIANT_BOOL FindSnapPointCore(double xScreen, double yScreen, double* xFound, double* yFound);
	double GetProjectedTolerance(double xScreen, double yScreen, double tolerance);
	void RedrawVolatileData(Gdiplus::Graphics* g, CDC* dc, const CRect& rcBounds);
	void RedrawTools(Gdiplus::Graphics* g, const CRect& rcBounds);
	bool RedrawLayers(Gdiplus::Graphics* g, CDC* dc, const CRect& rcBounds);
	void UpdateShapeEditor();
	bool HasDrawLists();
	bool UndoCore(bool shift);
	void ZoomToCursorPosition(bool zoomIn);
	HCURSOR GetCursorIcon();
	void ScheduleLayerRedraw();
	void ScheduleVolatileRedraw();
	void ClearDrawingLabelFrames();
	bool ReprojectLayer(Layer* layer, int layerHandle);
	void StartDragging(DraggingOperation operation);
	void DrawImageLayer(const CRect& rcBounds, Layer* l, Gdiplus::Graphics* graphics, CImageDrawer& imgDrawer);
	long AddSingleLayer(LPDISPATCH object, BOOL pVisible);
	void OnDrawingLayersChanged();
	bool HandleOnCopyrightMouseMove(CPoint point);
	void StartMouseTracking();
	bool HandleOnCopyrighMouseDown(CPoint point);
	void UndoMeasuringPoint();
	void FireOnDrawbackBufferCore(Gdiplus::Graphics* g, Gdiplus::Bitmap* bitmap);
	void DumpBuffers();
	bool CheckShapefileLayer(LayerSelector selector, int layerHandle, IShapefile* sf);
	void DrawIdentified(Gdiplus::Graphics* g, const CRect& rcBounds);
	void RenderSelectedPixels(vector<long>& handles, CShapefileDrawer& drawer, const CRect& rcBounds);
	void RenderIdentifiedShapes(vector<long>& handles, CShapefileDrawer& drawer, const CRect& rcBounds);
	void UpdateSelectedPixels(vector<long>& handles, bool& hasPolygons, bool& hasPoints);
	VARIANT_BOOL ZoomToTileLevelCore(int zoom, bool logPrevious);
	bool ValidatePreviousExtent();
	bool MapIsEmpty();
	void UpdateMapTranformation();
	bool TilesAreInCache();
	Layer* get_LayerByPosition(int position);
	void GetScreenInches(double& sw, double& sh);
	void GetMapSizeInches(double& mw, double& mh);
	// tiles
	int ChooseZoom(BaseProvider* provider, Extent ext, double scalingRatio, bool limitByProvider);
	int ChooseZoom(void* baseProvider, double scalingRatio, bool limitByProvider);
	bool get_TileProviderBounds(BaseProvider* provider, Extent& retVal);
	bool get_TilesForMap(void* p, double scalingRatio, CRect& indices, int& zoom);
	TileManager* get_TileManager();
	TileProjectionState GetTmsProjectionState();
	void StartTmsProjectionTransform(TileProjectionState state);
	void InitTmsProjection();
	void GetTilesZoomLevelSize(int zoom, SizeLatLng& size);
	void SetMapExtentsSize(SizeLatLng size, bool logPrevious);
	bool MapExtentsForTileBounds(SizeLatLng size, Extent& extents);
	bool ExtrapolateSphericalMercatorToDegrees(double xMin, double xMax, double yMin, double yMax, Extent& extents);
	// WMS
	void ReloadWmsLayers(bool snapshot, CString key);
	void ResizeWmsLayerBuffers(int cx, int cy);
	void RedrawWmsLayers(Gdiplus::Graphics* g);
	void AdjustWmsLayerVerticalPosition(int layerHandle);
	void UpdateWmsLayerBounds(IWmsLayer* layer);
	void UpdateWmsLayerBounds(IWmsLayer* wms, Layer& layer);
	// Snapping internal algorithm:
	VARIANT_BOOL DefaultSnappingAlgorithm(double maxDist, double minDist, double x, double y, double* xFound, double* yFound);
	VARIANT_BOOL CheckSnapPointForTolerance(double maxDist, double x, double y, double xF, double yF, double* xFound, double* yFound);
#pragma endregion

public:
	// limited interface for callback from related classes
	virtual IUndoList* _GetUndoList() { return _undoList; }
	virtual IShapefile* _GetShapefile(const LONG layerHandle) { return GetShapefile(layerHandle); }
	virtual IShapeEditor* _GetShapeEditor() { return _shapeEditor; }
	virtual ICallback* _GetGlobalCallback() { return _globalCallback; }
	virtual void _ZoomToShape(const long layerHandle, const long shapeIndex) { ZoomToShape2(layerHandle, shapeIndex); }
	virtual IGeoProjection* _GetWgs84Projection() { return GetWgs84Projection(); }
	virtual IGeoProjection* _GetMapProjection() { return GetMapProjection(); }
	virtual tkTransformationMode _GetTransformationMode() { return _transformationMode; }
	virtual void _ProjectionToPixel(const double projX, const double projY, double* pixelX, double* pixelY) { ProjectionToPixel(projX, projY, *pixelX, *pixelY); }
	virtual void _PixelToProjection(const double pixelX, const double pixelY, double* projX, double* projY) { PixelToProjection(pixelX, pixelY, *projX, *projY); }
	virtual void _FireBeforeDeleteShape(const tkDeleteTarget target, tkMwBoolean* cancel) { FireBeforeDeleteShape(target, cancel); }
	virtual tkCursorMode _GetCursorMode() { return static_cast<tkCursorMode>(m_cursorMode); }
	virtual void _FireValidateShape(const LONG layerHandle, IDispatch* shape, tkMwBoolean* cancel) { FireValidateShape(layerHandle, shape, cancel); }
	virtual void _FireAfterShapeEdit(const tkUndoOperation newShape, const LONG layerHandle, const LONG shapeIndex) { FireAfterShapeEdit(newShape, layerHandle, shapeIndex); }
	virtual void _FireShapeValidationFailed(const LPCTSTR errorMessage) { FireShapeValidationFailed(errorMessage); }
	virtual void _FireBeforeVertexDigitized(DOUBLE* pointX, DOUBLE* pointY) { FireBeforeVertexDigitized(pointX, pointY); }
	virtual void _ZoomToEditor() { ZoomToEditor(); }
	virtual void _SetMapCursor(const tkCursorMode mode, bool clearEditor) { UpdateCursor(mode, false); }
	virtual void _Redraw(const tkRedrawType redrawType, const bool updateTiles, const bool atOnce) { RedrawCore(redrawType, atOnce, updateTiles); };
	virtual void _FireUndoListChanged() { FireUndoListChanged(); }
	virtual void _UnboundShapeFinished(IShape* shp);
	virtual double _GetMouseProjTolerance() { return GetMouseTolerance(MouseTolerance::ToleranceSelect); }
	virtual void _StartDragging(const DraggingOperation operation) { StartDragging(operation); }
	virtual void _FireBackgroundLoadingStarted(const long taskId, const long layerHandle) { FireBackgroundLoadingStarted(taskId, layerHandle); };
	virtual void _FireBackgroundLoadingFinished(const long taskId, const long layerHandle, const long numFeatures, const long numLoaded) { FireBackgroundLoadingFinished(taskId, layerHandle, numFeatures, numLoaded); };
	virtual void _FireTilesLoaded(const bool isSnapshot, const CString key, const bool fromCache) { FireTilesLoaded(isSnapshot, key, fromCache); }
	virtual bool _GetTilesForMap(void* p, const double scalingRatio, CRect& indices, int& zoom) { return get_TilesForMap(p, scalingRatio, indices, zoom); }
	virtual int _ChooseZoom(void* provider, const double scalingRatio, const bool limitByProvider) { return ChooseZoom(provider, scalingRatio, limitByProvider); }
	virtual Extent* _GetExtents() { return &_extents; }
	virtual void _MarkTileBufferChanged() { _tileBuffer.Initialized = false; }
	virtual long _GetWidth() { return _viewWidth; }
	virtual long _GetHeight() { return _viewHeight; }



protected:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.