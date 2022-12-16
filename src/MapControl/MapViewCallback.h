#pragma once

class IMapViewCallback
{
public:
	virtual IShapefile* _GetShapefile(LONG layerHandle) = 0;
	virtual IShapeEditor* _GetShapeEditor() = 0;
	virtual ICallback* _GetGlobalCallback() = 0;
	virtual void _ZoomToShape(long layerHandle, long shapeIndex) = 0;
	virtual IGeoProjection* _GetWgs84Projection() = 0;
	virtual IGeoProjection* _GetMapProjection() = 0;
	virtual IUndoList* _GetUndoList() = 0;
	virtual tkTransformationMode _GetTransformationMode() = 0;
	virtual void _ProjectionToPixel(double projX, double projY, double * pixelX, double * pixelY) = 0;
	virtual void _PixelToProjection(double pixelX, double pixelY, double * projX, double * projY) = 0;
	virtual void _FireBeforeDeleteShape(tkDeleteTarget target, tkMwBoolean* cancel) = 0;
	virtual tkCursorMode _GetCursorMode() = 0;
	virtual void _FireValidateShape(LONG LayerHandle, IDispatch* Shape, tkMwBoolean* Cancel) = 0;
	virtual void _FireAfterShapeEdit(tkUndoOperation action, LONG LayerHandle, LONG ShapeIndex) = 0;
	virtual void _FireShapeValidationFailed(LPCTSTR ErrorMessage) = 0;
	virtual void _FireBeforeVertexDigitized(DOUBLE* pointX, DOUBLE* pointY) = 0;
	virtual void _ZoomToEditor() = 0;
	virtual void _SetMapCursor(tkCursorMode mode, bool clearEditor) = 0;
	virtual void _Redraw(tkRedrawType redrawType, bool updateTiles, bool atOnce) = 0;
	virtual void _FireUndoListChanged() = 0;
	virtual void _UnboundShapeFinished(IShape* shp) = 0;
	virtual double _GetMouseProjTolerance() = 0;
	virtual void _StartDragging(DraggingOperation operation) = 0;
	virtual void _FireBackgroundLoadingStarted(long taskId, long layerHandle) = 0;
	virtual void _FireBackgroundLoadingFinished(long taskId, long layerHandle, long numFeatures, long numLoaded) = 0;
	virtual void _FireTilesLoaded(bool isSnapshot, CString key, bool fromCache) = 0;
	virtual long _GetWidth() = 0;
	virtual long _GetHeight() = 0;

	// tiles
	virtual bool _GetTilesForMap(void* p, double scalingRatio, CRect& indices, int& zoom) = 0;
	virtual int _ChooseZoom(void* provider, double scalingRatio, bool limitByProvider) = 0;
	virtual Extent* _GetExtents() = 0;
	virtual void _MarkTileBufferChanged() = 0;
};

