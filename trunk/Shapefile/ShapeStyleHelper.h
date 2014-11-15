#pragma once
class ShapeStyleHelper
{
public:
	static void ApplyRandomDrawingOptions(IShapefile* sf);
	static bool GetPointBounds(IShapefile* sf, Extent& extents);
	static bool GetPointBounds(IShapeDrawingOptions* options, Extent& extents);
	static IShapeDrawingOptions* GetHotTrackingStyle(IShapefile* sf, OLE_COLOR outlineColor, bool identify);
};

