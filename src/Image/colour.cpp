#include "stdafx.h"
#include "colour.h"

colour::colour() : red(0), green(0), blue(0), alpha(255) {}

colour::colour(unsigned char r, unsigned char g, unsigned char b) :red(r), green(g), blue(b), alpha(255) { }

colour::colour(long oleColor)
{
	FromOleColor(oleColor);
}

bool colour::operator==( const colour & c )
{	
	return red == c.red && green == c.green && blue == c.blue && alpha == c.alpha;
}

COLORREF colour::ToOleColor()
{
	return red | green << 8 | blue << 16 | alpha << 24;
}

void colour::FromOleColor(long color)
{
	red = GetRValue(color);
	green = GetGValue(color);
	blue = GetBValue(color);
	alpha = GET_ALPHA(color);
}
