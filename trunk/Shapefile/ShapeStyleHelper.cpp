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