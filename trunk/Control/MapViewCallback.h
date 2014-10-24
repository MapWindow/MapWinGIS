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
	virtual tkTransformationMode _GetTransformationMode() = 0;
	virtual void _ProjectionToPixel(double projX, double projY, double * pixelX, double * pixelY) = 0;
	virtual void _PixelToProjection(double pixelX, double pixelY, double * projX, double * projY) = 0;
};


