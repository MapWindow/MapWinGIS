#pragma once
#include "ComHelper.h"

struct SelectedItem
{
	long LayerHandle;
	long ShapeIndex;
	tkLayerType LayerType;
	long RasterX;
	long RasterY;
	bool Calculated;
	bool Polygon;
	IShape* ShapePixel;

	SelectedItem(long layerHandle, long shapeIndex)
	{
		Init();
		LayerType = ltVector;
		LayerHandle = layerHandle;
		ShapeIndex = shapeIndex;
	}

	SelectedItem(long layerHandle, long rasterX, long rasterY)
	{
		Init();
		LayerType = ltRaster;
		LayerHandle = layerHandle;
		RasterX = rasterX;
		RasterY = rasterY;
	}

	~SelectedItem()
	{
		if (ShapePixel)
		{
			ShapePixel->Release();
		}
	}

	void AssignShapePixel(double x, double y, double x2, double y2)
	{
		if (!ShapePixel)
		{
			ComHelper::CreateShape(&ShapePixel);
			ShapePixel->put_ShapeType(SHP_POLYGON);
		}
		else
		{
			ShapePixel->Clear();
		}

		long pointIndex = 0;
		ShapePixel->AddPoint(x, y, &pointIndex);
		ShapePixel->AddPoint(x2, y, &pointIndex);
		ShapePixel->AddPoint(x2, y2, &pointIndex);
		ShapePixel->AddPoint(x, y2, &pointIndex);
		ShapePixel->AddPoint(x, y, &pointIndex);
	}

	void Init()
	{
		Calculated = false;
		ShapePixel = NULL;
		ShapeIndex = -1;
		RasterX = -1;
		RasterY = -1;
		Polygon = true;
	}
};

