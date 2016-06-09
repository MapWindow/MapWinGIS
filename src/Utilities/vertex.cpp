# include "stdafx.h"
# include "vertex.h"

vertex::vertex()
{	x = 0.0;
	y = 0.0;
	z = 0.0;
}
vertex::vertex( const vertex & v )
{	x = v.x;
	y = v.y;
	z = v.z;
	normal = v.normal;
}
vertex::~vertex()
{
}
vertex::vertex( double v_X, double v_Y, double v_Z )
{	x = v_X;
	y = v_Y;
	z = v_Z;
}
vertex vertex::operator=( const vertex & v )
{	x = v.x;
	y = v.y;
	z = v.z;
	normal = v.normal;
	return *this;
}
vertex vertex::operator-( const vertex & v )
{	vertex result;
	result.x = x - v.x;
	result.y = y - v.y;
	result.z = z - v.z;
	return result;
}

vertex vertex::operator+( double scalar )
{	vertex result;
	result.x = x + scalar;
	result.y = y + scalar;
	result.z = z + scalar;
	return result;
}

vertex vertex::operator*( double scalar )
{	vertex result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

vertex vertex::operator-( double scalar )
{	vertex result;
	result.x = x - scalar;
	result.y = y - scalar;
	result.z = z - scalar;
	return result;
}

bool vertex::operator==( const vertex & v )
{	if( x == v.x && y == v.y && z == v.z )
		return true;
	return false;
}

double vertex::getX()
{	return x;
}
double vertex::getY()
{	return y;
}
double vertex::getZ()
{	return z;
}
void vertex::setX( double v_X )
{	x = v_X;
}
void vertex::setY( double v_Y )
{	y = v_Y;
}
void vertex::setZ( double v_Z )
{	z = v_Z;
}
void vertex::setNormal( cppVector p_normal )
{	normal = p_normal;
}
cppVector vertex::getNormal()
{	return normal;
}
double vertex::xyDistance( const vertex & v )
{	return sqrt( pow( x - v.x, 2 ) + pow( y - v.y, 2 ) );
}
double vertex::xyzDistance( const vertex & v )
{	return sqrt( pow( x - v.x, 2 ) + pow( y - v.y, 2 ) + pow( z - v.z, 2) );
}