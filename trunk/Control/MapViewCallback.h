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
	virtual void _FireValidateShape(tkCursorMode Action, LONG LayerHandle, IDispatch* Shape, tkMwBoolean* Cancel) = 0;
	virtual void _FireAfterShapeEdit(tkUndoOperation Action, LONG LayerHandle, LONG ShapeIndex) = 0;
	virtual void _FireShapeValidationFailed(LPCTSTR ErrorMessage) = 0;
	virtual void _ZoomToEditor() = 0;
	virtual void _SetMapCursor(tkCursorMode mode) = 0;
};


