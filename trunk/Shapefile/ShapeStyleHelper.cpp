#include "stdafx.h"
#include "ShapeStyleHelper.h"
#include "ShapefileHelper.h"

// *****************************************************
//		ApplyRandomDrawingOptions()
// *****************************************************
void ShapeStyleHelper::ApplyRandomDrawingOptions(IShapefile* sf)
{
	if (!sf) return;
	ShpfileType type = ShapefileHelper::GetShapeType(sf);

	// setting default colors for shapefile layer
	IShapeDrawingOptions* options = NULL;
	sf->get_DefaultDrawingOptions(&options);
	if (options)
	{
		//srand( (unsigned)time( NULL ));
		unsigned char r1, r2, g1, g2, b1, b2;

		if (type == SHP_POLYLINE || type == SHP_POLYLINEZ || type == SHP_POLYLINEM)
		{
			r1 = g1 = b1 = 75;	 // dark colors
			r2 = g2 = b2 = 150;
			unsigned char r = r1 + unsigned char(double(rand() / double(RAND_MAX) * (r2 - r1)));
			unsigned char g = g1 + unsigned char(double(rand() / double(RAND_MAX) * (g2 - g1)));
			unsigned char b = b1 + unsigned char(double(rand() / double(RAND_MAX) * (b2 - b1)));
			options->put_LineColor(RGB(r, g, b));
			options->put_FillColor(RGB(255, 255, 255));
		}
		else if (type == SHP_POLYGON || type == SHP_POLYGONZ || type == SHP_POLYGONM)
		{
			r1 = g1 = b1 = 180;	// light colors
			r2 = g2 = b2 = 230;
			unsigned char r = r1 + unsigned char(double(rand() / double(RAND_MAX) * (r2 - r1)));
			unsigned char g = g1 + unsigned char(double(rand() / double(RAND_MAX) * (g2 - g1)));
			unsigned char b = b1 + unsigned char(double(rand() / double(RAND_MAX) * (b2 - b1)));
			options->put_FillColor(RGB(r, g, b));

			// gray color for outlines
			options->put_LineColor(RGB(150, 150, 150));
		}
		else   // point and multipoints
		{
			r1 = g1 = b1 = 50;
			r2 = g2 = b2 = 150;
			unsigned char r = r1 + unsigned char(double(rand() / double(RAND_MAX) * (r2 - r1)));
			unsigned char g = g1 + unsigned char(double(rand() / double(RAND_MAX) * (g2 - g1)));
			unsigned char b = b1 + unsigned char(double(rand() / double(RAND_MAX) * (b2 - b1)));
			options->put_FillColor(RGB(r, g, b));

			// gray color for outlines
			options->put_LineColor(RGB(150, 150, 150));
		}
		options->Release();
		options = NULL;
	}

	sf->get_SelectionDrawingOptions(&options);
	if (options)
	{
		if (type == SHP_POLYLINE || type == SHP_POLYLINEZ || type == SHP_POLYLINEM)
		{
			options->put_LineColor(RGB(255, 255, 0));
		}
		else if (type == SHP_POLYGON || type == SHP_POLYGONZ || type == SHP_POLYGONM)
		{
			options->put_FillColor(RGB(255, 255, 0));
			// gray color for outlines
			options->put_LineColor(RGB(150, 150, 150));
		}
		else   // point and multipoints
		{
			options->put_FillColor(RGB(255, 255, 0));
			// gray color for outlines
			options->put_LineColor(RGB(150, 150, 150));
		}
		options->Release();
		options = NULL;
	}

	ILabels* labels;
	sf->get_Labels(&labels);
	if (labels)
	{
		if (type == SHP_POINT || type == SHP_POINTZ || type == SHP_POINTM)
		{
			labels->put_Alignment(laCenterRight);
		}
		labels->Release();
	}
}

// *****************************************************
//		GetPointBounds()
// *****************************************************
bool ShapeStyleHelper::GetPointBounds(IShapefile* sf, Extent& extents)
{
	if (!sf) return false;
	ShpfileType shpType = ShapefileHelper::GetShapeType2D(sf);
	if (shpType != SHP_POINT) return false;
	CComPtr<IShapeDrawingOptions> options = NULL;
	sf->get_DefaultDrawingOptions(&options);
	return GetPointBounds(options, extents);
}

// *****************************************************
//		GetPointBounds()
// *****************************************************
bool ShapeStyleHelper::GetPointBounds(IShapeDrawingOptions* options, Extent& extents)
{
	if (!options) return false;
	tkPointSymbolType pointType;
	options->get_PointType(&pointType);

	if (pointType == ptSymbolPicture) 
	{
		CComPtr<IImage> img = NULL;
		options->get_Picture(&img);
		if (img) {
			long width, height;
			img->get_Width(&width);
			img->get_Height(&height);

			double scaleX, scaleY;
			options->get_PictureScaleX(&scaleX);
			options->get_PictureScaleY(&scaleY);

			extents.right = width / 2.0 * scaleX;
			extents.left = -extents.left;

			VARIANT_BOOL alignByBottom;
			options->get_AlignPictureByBottom(&alignByBottom);
			if (alignByBottom) {
				extents.top = 0.0;;
				extents.bottom = -height * scaleY;
			}
			else {
				extents.top = height / 2.0 * scaleY;
				extents.bottom = -extents.top;
			}
		}
	}
	else 
	{
		float pointSize;
		options->get_PointSize(&pointSize);
		pointSize /= 2.0f;
		extents.right = extents.top = pointSize;
		extents.left = extents.bottom = -pointSize;
	}
	return true;
}

// *****************************************************
//		GetHotTrackingStyle()
// *****************************************************
IShapeDrawingOptions* ShapeStyleHelper::GetHotTrackingStyle(IShapefile* sf, OLE_COLOR outlineColor, bool identify)
{
	if (!sf) return NULL;

	CComPtr<IShapeDrawingOptions> options = NULL;
	sf->get_DefaultDrawingOptions(&options);
	if (options)
	{
		IShapeDrawingOptions* newOptions = NULL;
		options->Clone(&newOptions);
		if (newOptions)
		{
			VARIANT_BOOL interactiveEditing;
			sf->get_InteractiveEditing(&interactiveEditing);

			ShpfileType type = ShapefileHelper::GetShapeType2D(sf);

			if (identify || !interactiveEditing)
			{
				bool point = type == SHP_POINT || type == SHP_MULTIPOINT;
				newOptions->put_FillVisible(point ? VARIANT_TRUE : VARIANT_FALSE);
				newOptions->put_LineColor(outlineColor);
				newOptions->put_LineWidth(2.0f);
			}
			else
			{
				if (type == SHP_POINT || type == SHP_MULTIPOINT)
				{
					newOptions->put_FillColor(RGB(0, 0, 255));   // blue
					newOptions->put_FillVisible(VARIANT_TRUE);
				}
				else {
					newOptions->put_LineVisible(VARIANT_FALSE);
					newOptions->put_FillVisible(VARIANT_FALSE);
					newOptions->put_VerticesVisible(VARIANT_TRUE);  // vertices only
				}
			}
			return newOptions;
		}
	}
	return NULL;
}

// *****************************************************
//		GetSymbologyFileAsXml()
// *****************************************************
CStringW ShapeStyleHelper::GetSymbologyFileAsXml(IShapefile* sf)
{
	CStringW style = L"";
	
	CStringW filename = ShapefileHelper::GetSymbologyFilename(sf);
	if (filename.GetLength() == 0) return style;
	
	CPLXMLNode* node = GdalHelper::ParseXMLFile(filename);
	if (node) {
		USES_CONVERSION;
		style = Utility::ConvertFromUtf8(CPLSerializeXMLTree(node));
		CPLDestroyXMLNode(node);
	}
	return style;
}