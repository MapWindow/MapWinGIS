#include "stdafx.h"
# include "cppVector.h"
# include <math.h>

cppVector::cppVector()
{	i = 0.0;
	j = 0.0;
	k = 0.0;
}

cppVector::~cppVector()
{
}

cppVector::cppVector( double ii, double jj, double kk )
{	i = ii;
	j = jj;
	k = kk;
}

cppVector::cppVector(const cppVector & v )
{	i = v.i;
	j = v.j;
	k = v.k;
}

cppVector cppVector::operator=( const cppVector & v )
{	
	i = v.i;
	j = v.j;
	k = v.k;
	return *this;
}

cppVector cppVector::operator*( Matrix & m )
{	
	cppVector result;

	result.i += i * m.get( 0, 0 );
	result.i += j * m.get( 1, 0 );
	result.i += k * m.get( 2, 0 );
	
	result.j += i * m.get( 0, 1 );
	result.j += j * m.get( 1, 1 );
	result.j += k * m.get( 2, 1 );
	
	result.k += i * m.get( 0, 2 );
	result.k += j * m.get( 1, 2 );
	result.k += k * m.get( 2, 2 );
	
	return result;	
}

cppVector cppVector::operator+( const cppVector & v )
{	cppVector result;
	
	result.i = i + v.i;
	result.j = j + v.j;
	result.k = k + v.k;

	return result;
}

void cppVector::seti( double ii )
{	i = ii;
}

void cppVector::setj( double jj )
{	j = jj;
}

void cppVector::setk( double kk )
{	k = kk;
}

double cppVector::geti()
{	return i;
}

double cppVector::getj()
{	return j;
}

double cppVector::getk()
{	return k;
}

void cppVector::Normalize()
{
	double length = sqrt( pow( i, 2 ) + pow( j, 2 ) + pow( k, 2 ) );
	if( length <= 0 )
		return;

	i = i/length;
	j = j/length;
	k = k/length;
}

double cppVector::dot( const cppVector & v )
{	return i*v.i + j*v.j + k*v.k;
}

cppVector cppVector::crossProduct( const cppVector & v )
{	
	cppVector result;

	result.i = j*v.k - k*v.j;
	result.j = k*v.i - i*v.k;
	result.k = i*v.j - j*v.i;

	result.Normalize();
	return result;
}
