#include "stdafx.h"
#include "point.h"

Point::Point()
{	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Point::Point( const Point & p )
{	x = p.x;
	y = p.y;
	z = p.z;
}

Point::Point( double p_X, double p_Y, double p_Z )
{	x = p_X;
	y = p_Y;
	z = p_Z;
}

Point::~Point()
{
}

Point Point::operator=( const Point & p )
{	x = p.x;
	y = p.y;
	z = p.z;
	return *this;
}

bool Point::operator == ( const Point & p )
{	if( x == p.x && y == p.y && z == p.z )
		return true;

	return false;
}

double Point::getX()
{	return x;
}

double Point::getY()
{	return y;
}

double Point::getZ()
{	return z;
}

void Point::setX( double p_X )
{	x = p_X;
}

void Point::setY( double p_Y )
{	y = p_Y;
}

void Point::setZ( double p_Z )
{	z = p_Z;
}