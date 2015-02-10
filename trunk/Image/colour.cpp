#include "stdafx.h"
#include "colour.h"

colour::colour()
{	red = 255;
	green = 255;
	blue = 255;
}

colour::colour(unsigned char r, unsigned char g, unsigned char b) :red(r),green(g),blue(b)
{

}

bool colour::operator==( const colour & c )
{	if( red == c.red && green == c.green && blue == c.blue )
		return true;
	return false;
}
