#include "stdafx.h"
#include "colour.h"

colour::colour() : red(0), green(0), blue(0), alpha(255) {}

colour::colour(unsigned char r, unsigned char g, unsigned char b) :red(r), green(g), blue(b), alpha(255) { }

bool colour::operator==( const colour & c )
{	
	return red == c.red && green == c.green && blue == c.blue && alpha == c.alpha;
}
